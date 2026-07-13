// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game.h"

#include "Engine/Render/RenderManager.h"
#include "Engine/Utils/Time.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Platform/ImGuiConversion.h"

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

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <chrono>
#include <memory>
#include <imgui.h>
#include "backends/imgui_impl_glfw.h"

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

    if (!InitializeWindow()) {
        return false;
    }

    if (!InitializeGraphics()) {
        return false;
    }

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


bool Game::InitializeWindow()
{
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    GLFWmonitor* monitor = m_config.window.isFullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_window = glfwCreateWindow(
        m_config.window.width,
        m_config.window.height,
        "ECS DroneShow",
        monitor,
        nullptr
    );

    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOther(m_window, true);

    return true;
}


bool Game::InitializeGraphics()
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    HWND hwnd = glfwGetWin32Window(m_window);

    if (!RenderManager::GetInstance().Init(hwnd, width, height)) {
        std::cerr << "Failed to initialize RenderManager" << std::endl;
        return false;
    }
    RenderManager::GetInstance().InitImGui();
    InitRenderState();
    return true;
}


void Game::InitializeManagers()
{
    m_registry = std::make_unique<ComponentRegistry>();
    m_inputManager = std::make_unique<InputManager>(m_window);
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

        // Cap the delta time at a maximum of 0.1 seconds to prevent physics spikes
        if (dt > 0.1f) {
            dt = 0.1f;
        }

        Time::UpdateFrame(dt);
        double simulationTime = Time::GetTotalTime();

        glfwPollEvents();
        HandleInput();

        RenderManager::GetInstance().BeginImGui();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        m_coordinator.UpdatePhase(SystemPhase::Input, dt, simulationTime);
        m_coordinator.UpdatePhase(SystemPhase::Logic, dt, simulationTime);

        fixedAccumulator += dt;

        while (fixedAccumulator >= FIXED_DT) {
            Time::UpdateFixed(FIXED_DT);
            m_coordinator.UpdatePhase(SystemPhase::Physics, Time::GetFixedDeltaTime(), Time::GetFixedTotalTime());
            fixedAccumulator -= FIXED_DT;
        }

        RenderManager::GetInstance().BeginFrame();

        m_coordinator.UpdatePhase(SystemPhase::Render, dt, simulationTime);
        m_coordinator.EndFrame();

        DrawDebugInfo();

        ImGui::Render();
        RenderManager::GetInstance().RenderImGui();
        RenderManager::GetInstance().EndFrame();
    }
}


void Game::Shutdown()
{
    RenderManager::GetInstance().ShutdownImGui();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    RenderManager::GetInstance().Shutdown();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}


bool Game::ShouldRun()
{
    if (!m_isRunning) {
        return false;
    }

    if (glfwWindowShouldClose(m_window)) {
        return false;
    }

    return true;
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
    Matrix4 viewMat = Matrix4::LookAt(m_config.camera.position, Vector3(0, 0, 0), Vector3(0, 1, 0));
    Matrix4 projMat = Matrix4::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);

    RenderManager::GetInstance().UpdateCamera(viewMat * projMat, m_config.camera.position);
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

        if (m_inputManager->IsKeyDown(GLFW_KEY_ESCAPE)) {
            m_isRunning = false;
        }

        // Press [1] to spawn entities
        if (m_inputManager->IsKeyDown(GLFW_KEY_1)) {
            SpawnEntity(m_config.spawn.userSpawnNum, m_config.prefab.spawnName);
        }
        // Press [2] to destroy entities
        else if (m_inputManager->IsKeyDown(GLFW_KEY_2)) {
            DestroyEntity(m_config.spawn.userDestroyNum);
        }
        // Press [3] to toggle debug overlay (2D Text)
        else if (m_inputManager->IsKeyDown(GLFW_KEY_3)) {
            Debug::Overlay::IsVisible = !Debug::Overlay::IsVisible;
        }
        // Press [4] to toggle debug drawing (3D Primitives)
        else if (m_inputManager->IsKeyDown(GLFW_KEY_4)) {
            Debug::Draw3D::IsVisible = !Debug::Draw3D::IsVisible;
        }

    }
}

void Game::DrawDebugInfo()
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    Matrix4 viewMat = Matrix4::LookAt(m_config.camera.position, Vector3(0, 0, 0), Vector3(0, 1, 0));
    Matrix4 projMat = Matrix4::Perspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    Matrix4 viewProj = viewMat * projMat;
    DebugDrawManager::GetInstance().RenderAndClear(viewProj, width, height);

    if (!Debug::Overlay::IsVisible) {
        return;
    }

    std::string sequenceName{};
    if (m_directorSystem) {
        if (const SequenceData* sequence = m_directorSystem->GetCurrentSequence()) {
            sequenceName = sequence->name;
        }
    }

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("Debug Overlay", nullptr, windowFlags)) {
        ImVec4 color = ToImVec4(Color::Green());

        ImGui::TextColored(color, "FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::TextColored(color, "Time: %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::TextColored(color, "State: %s", sequenceName.c_str());
        ImGui::TextColored(color, "Entities: %d", static_cast<int>(m_coordinator.GetLivingEntityCount()));

        ImGui::Separator();

        ImGui::TextColored(color, "Press [1]: Spawn Entity %d", m_config.spawn.userSpawnNum);
        ImGui::TextColored(color, "Press [2]: Destroy Entity %d", m_config.spawn.userDestroyNum);
        ImGui::TextColored(color, "Press [3]: Toggle Enable Debug Overlay");

        if (Debug::Config::IsEnabled) {
            ImGui::TextColored(color, "Press [4]: Toggle Enable Debug 3D Draw");
        }
    }
    ImGui::End();
}
