// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game.h"

#include "Engine/Render/RenderManager.h"
#include "Engine/Utils/Time.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Platform/DxLibConversion.h"

#include "Engine/Systems/Spatial/SpatialPartitionSystem.h"
#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"
#include "Engine/Systems/Force/BoidsSystem.h"

#include "Game/Registration/RegisterAllComponents.h"
#include "Game/Registration/RegisterAllSystems.h"

#include "Game/Systems/TimelineDirectorSystem.h"
#include "Game/Systems/FormationSystemWireframe.h"
#include "Game/Systems/FormationSystemPointCloud.h"

#include "Game/Managers/TimelineManager.h"
#include "Game/Managers/PrefabManager.h"
#include "Game/Managers/InputManager.h"
#include "Game/Loaders/JsonLoader.h"
#include "Game/Data/Timeline/TimelineData.h"
#include "Game/Parsers/Json/DataJsonParsers.h"

#include "Engine/Debug/DebugDrawManager.h"

#include <chrono>
#include <memory>

namespace
{
constexpr float FIXED_DT = 1.0f / 60.0f;
constexpr float FPS_INTERVAL = 0.5f;
}


Game::Game() 
    : m_isRunning(false)
{};


Game::~Game() = default;


bool Game::Init()
{
    if (!JsonLoader::Load("data/config/game_config.json", m_config)) {
        std::cerr << "Warning: Could not load config file. Using default settings." << std::endl;
        return false;
    }

    ConfigureDxLib();

    if (DxLib_Init() == -1) {
        return false;
    }

    InitializeGraphics();
    InitializeManagers();

    m_coordinator.Init();

    GameRegistrations::RegisterAllComponents(*m_registry, m_coordinator);
    GameRegistrations::RegisterAllSystems(m_coordinator);

    LoadGameData();

    SetupSystems();
    m_coordinator.InitSystems();

    SpawnEntity(m_config.spawn.initialSpawnCount, m_config.prefab.spawnName);

    m_isRunning = true;
    return true;
}


void Game::ConfigureDxLib()
{
    SetOutApplicationLogValidFlag(FALSE);
    ChangeWindowMode(!m_config.window.isFullscreen);
    SetGraphMode(m_config.window.width, m_config.window.height, 32);
    SetMainWindowText(_TEXT("ECS DroneShow"));
    SetAlwaysRunFlag(TRUE);
}


void Game::InitializeGraphics()
{
    SetDrawScreen(DX_SCREEN_BACK);

    SetCameraPositionAndTarget_UpVecY(ToDxVec(m_config.camera.position), VGet(0.0f, 0.0f, 0.0f));

    RenderManager::GetInstance().Init();
    InitRenderState();
}

void Game::InitializeManagers()
{
    m_registry = std::make_unique<ComponentRegistry>();
    m_inputManager = std::make_unique<InputManager>();
    m_prefabManager = std::make_unique<PrefabManager>(m_coordinator, *m_registry);
    m_timelineManager = std::make_unique<TimelineManager>();
}


void Game::LoadGameData()
{
    // Load game assets (must be executed after component registration)
    m_prefabManager->LoadPrefabsFromDirectory(m_config.prefab.folderPath);
    m_timelineManager->LoadTimelinesFromDirectory(m_config.timeline.folderPath);
}


void Game::Run()
{
    auto lastTime = std::chrono::high_resolution_clock::now();
    float fixedAccumulator = 0.0f;

    while (ShouldRun()) {
        const auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        lastTime = currentTime;

        UpdateFps(dt);

        // Cap the delta time at a maximum of 0.1 seconds to prevent physics spikes
        if (dt > 0.1f) {
            dt = 0.1f;
        }

        Time::UpdateFrame(dt);
        double simulationTime = Time::GetTotalTime();

        HandleInput();

        m_coordinator.UpdatePhase(SystemPhase::Input, dt, simulationTime);
        m_coordinator.UpdatePhase(SystemPhase::Logic, dt, simulationTime);

        fixedAccumulator += dt;

        while (fixedAccumulator >= FIXED_DT) {
            Time::UpdateFixed(FIXED_DT);
            m_coordinator.UpdatePhase(SystemPhase::Physics, Time::GetFixedDeltaTime(), Time::GetFixedTotalTime());
            fixedAccumulator -= FIXED_DT;
        }

        ClearDrawScreen();

        m_coordinator.UpdatePhase(SystemPhase::Render, dt, simulationTime);
        m_coordinator.EndFrame();

        DebugDrawManager::GetInstance().RenderAndClear();
        DrawDebugInfo(dt, m_currentFps);

        ScreenFlip();
    }
}


void Game::Shutdown()
{
    RenderManager::GetInstance().Shutdown();
    DxLib_End();
}


bool Game::ShouldRun()
{
    if (!m_isRunning) {
        return false;
    }

    if (ProcessMessage() == -1) {
        return false;
    }

    return true;
}


void Game::UpdateFps(float dt)
{
    ++m_frameCount;
    m_fpsAccumulator += dt;

    if (m_fpsAccumulator >= FPS_INTERVAL) {
        m_currentFps = static_cast<int>(m_frameCount / m_fpsAccumulator);

        m_fpsAccumulator = 0.0f;
        m_frameCount = 0;
    }
}


void Game::SetupSystems()
{
    auto* partition = m_coordinator.GetSystem<SpatialPartitionSystem>();
    auto* cache = m_coordinator.GetSystem<SpatialBoidCacheSystem>();
    auto* boids = m_coordinator.GetSystem<BoidsSystem>();

    cache->SetSpatialGrid(&partition->GetGrid());
    boids->SetSpatialGrid(&partition->GetGrid());
    boids->SetCacheSystem(cache);

    auto* formationWire = m_coordinator.GetSystem<FormationSystemWireframe>();
    auto* formationPoint = m_coordinator.GetSystem<FormationSystemPointCloud>();

    formationWire->SetTimelineManager(m_timelineManager.get());
    formationPoint->SetTimelineManager(m_timelineManager.get());

    const auto& timeline = m_timelineManager->GetTimelineData(m_config.timeline.playName);
    assert(timeline && "Timeline data is invalid.");

    auto* director = m_coordinator.GetSystem<TimelineDirectorSystem>();
    director->SetTimeline(timeline);
    director->SetRegistry(m_registry.get());

    m_directorSystem = director;
}


void Game::SpawnEntity(int spawnNum, const std::string& prefabName)
{
    if (!m_directorSystem) {
        assert(false);
        return;
    }

    if (!m_prefabManager) {
        assert(false);
        return;
    }

    const auto& drones = m_directorSystem->GetEntities();
    int currentCount = static_cast<int>(drones.size());

    if (currentCount + spawnNum > MAX_ENTITIES) {
        return;
    }

    for (int i = 0; i < spawnNum; ++i) {
        Entity entity = m_prefabManager->Instantiate(prefabName);
        if (entity != INVALID_ENTITY) {
            if (m_directorSystem) {
                m_directorSystem->CatchUpNewEntity(m_coordinator, entity);
            }
        }
    }
}

void Game::DestroyEntity(int destroyNum)
{
    if (!m_directorSystem) {
        assert(false);
        return;
    }

    const auto& entities = m_directorSystem->GetEntities();
    int currentCount = static_cast<int>(entities.size());

    int actualDestroyNum = (std::min)(destroyNum, currentCount);
    if (actualDestroyNum <= 0) {
        return;
    }

    std::vector<Entity> safeEntities = entities;
    for (int i = 0; i < actualDestroyNum; ++i) {
        Entity entityToDestroy = safeEntities.back();
        safeEntities.pop_back();
        m_coordinator.DestroyEntity(entityToDestroy);
    }
}


void Game::InitRenderState()
{
    MATRIX viewMat = GetCameraViewMatrix();
    MATRIX projMat = GetCameraProjectionMatrix();
    MATRIX viewProjMat = MMult(viewMat, projMat);
    VECTOR camPos = GetCameraPosition();

    RenderManager::GetInstance().UpdateCamera(FromDxMatrix(viewProjMat), FromDxVec(camPos));
    RenderManager::GetInstance().UpdateLight(
        Vector3(-1.0f, -1.0f, 1.0f),
        0.1f,
        Color::White(),
        Color(0.01f, 0.01f, 0.05f, 1.0f)
    );
}


void Game::HandleInput()
{
    if (m_inputManager) {
        m_inputManager->Update();

        if (m_inputManager->IsKeyDown(KEY_INPUT_ESCAPE)) {
            m_isRunning = false;
        }

        // Press [1] to spawn entities
        if (m_inputManager->IsKeyDown(KEY_INPUT_1)) {
            SpawnEntity(m_config.spawn.userSpawnNum, m_config.prefab.spawnName);
        }
        // Press [2] to destroy entities
        else if (m_inputManager->IsKeyDown(KEY_INPUT_2)) {
            DestroyEntity(m_config.spawn.userDestroyNum);
        }
        // Press [3] to toggle debug overlay (2D Text)
        else if (m_inputManager->IsKeyDown(KEY_INPUT_3)) {
            Debug::Overlay::IsVisible = !Debug::Overlay::IsVisible;
        }
        // Press [4] to toggle debug drawing (3D Primitives)
        else if (m_inputManager->IsKeyDown(KEY_INPUT_4)) {
            Debug::Draw3D::IsVisible = !Debug::Draw3D::IsVisible;
        }

    }
}

void Game::DrawDebugInfo(float dt, int currentFps)
{
    if (!Debug::Overlay::IsVisible) {
        return;
    }

    std::string sequenceName{};
    if (m_directorSystem) {
        if (const SequenceData* sequence = m_directorSystem->GetCurrentSequence()) {
            sequenceName = sequence->name;
        }
    }

    DrawFormatString(10, 10, GetColor(0, 255, 0), _TEXT("FPS: %d"), currentFps);
    DrawFormatString(10, 30, GetColor(0, 255, 0), _TEXT("Time: %.2f ms"), dt * 1000.0);
    DrawFormatString(10, 50, GetColor(0, 255, 0), _TEXT("State: %S"), sequenceName.c_str());
    DrawFormatString(10, 70, GetColor(0, 255, 0), _TEXT("Entities: %d"), (int)(m_coordinator.GetLivingEntityCount()));
    DrawFormatString(10, 90, GetColor(0, 255, 0), _TEXT("Press [1]: Spawn Entity %d"), m_config.spawn.userSpawnNum);
    DrawFormatString(10, 110, GetColor(0, 255, 0), _TEXT("Press [2]: Destroy Entity %d"), m_config.spawn.userDestroyNum);
    DrawFormatString(10, 130, GetColor(0, 255, 0), _TEXT("Press [3]: Toggle Enable Debug Overlay"));
    if (Debug::Config::IsEnabled) {
        DrawFormatString(10, 150, GetColor(0, 255, 0), _TEXT("Press [4]: Toggle Enable Debug 3D Draw"));
    }
}
