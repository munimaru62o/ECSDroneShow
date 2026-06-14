// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Game/Data/Config/GameConfig.h"

#include <memory>

class InputManager;
class PrefabManager;
class TimelineManager;
class ComponentRegistry;
class TimelineDirectorSystem;

/**
 * @class Game
 * @brief The core application class that orchestrates the entire game engine, managing initialization, the main loop, and shutdown processes.
 *
 * It handles everything from DxLib window configuration to setting up the ECS (Coordinator),
 * instantiating managers, and registering components and systems (building the pipeline).
 * The main loop (Run) employs a hybrid architecture: it processes input and rendering phases
 * on a variable timestep, while strictly executing the physics simulation phase on a fixed timestep.
 * This creates a highly robust loop that ensures physics calculations remain stable and deterministic,
 * even during sudden frame drops.
 */
class Game
{
public:    
    Game();
    ~Game();

    bool Init();
    void Run();
    void Shutdown();

private:
    [[nodiscard]] bool ShouldRun() const;

    void ConfigureDxLib();
    void InitializeGraphics();
    void InitializeManagers();
    void LoadGameData();

    void UpdateFps(float dt);

    void InitRenderState();
    void SetupSystems();

    void SpawnEntity(int spawnNum, const std::string& prefabName);
    void DestroyEntity(int destroyNum);

    void HandleInput();
    void DrawDebugInfo(float dt, int currentFps);

private:
    Coordinator m_coordinator;
    GameConfig m_config;

    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<PrefabManager> m_prefabManager;
    std::unique_ptr<TimelineManager> m_timelineManager;
    std::unique_ptr<ComponentRegistry> m_registry;

    TimelineDirectorSystem* m_directorSystem = nullptr;

    bool m_isRunning = false;
    float m_fpsAccumulator = 0.0f;
    int m_currentFps = 0;
    int m_frameCount = 0;
};
