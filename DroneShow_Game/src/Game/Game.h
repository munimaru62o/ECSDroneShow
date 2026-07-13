// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Game/Data/Config/GameConfig.h"

#include <memory>

struct GLFWwindow;
class InputManager;
class PrefabManager;
class TimelineManager;
class ComponentRegistry;
class TimelineDirectorSystem;

/**
 * @class Game
 * @brief The core application class that orchestrates the entire game engine, managing initialization, the main loop, and shutdown processes.
 *
 * It manages the lifecycle of the application by initializing the GLFW window, setting up the custom
 * DirectX 11 rendering pipeline, and building the entity component system (ECS) via the Coordinator.
 * The main loop (Run) employs a hybrid architecture: it processes input and rendering phases on a variable
 * timestep, while strictly executing the physics simulation phase on a fixed timestep. This design ensures
 * that physics calculations remain stable, uniform, and deterministic, even under volatile frame rate conditions.
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
    [[nodiscard]] bool ShouldRun();

    bool InitializeWindow();
    bool InitializeGraphics();
    void InitializeManagers();
    void LoadGameData();

    void InitRenderState();
    void SetupSystems();

    void SpawnEntity(int spawnNum, const std::string& prefabName);
    void DestroyEntity(int destroyNum);

    void HandleInput();
    void DrawDebugInfo();

private:
    GLFWwindow* m_window = nullptr;

    Coordinator m_coordinator;
    GameConfig m_config;

    std::unique_ptr<InputManager> m_inputManager;
    std::unique_ptr<PrefabManager> m_prefabManager;
    std::unique_ptr<TimelineManager> m_timelineManager;
    std::unique_ptr<ComponentRegistry> m_registry;

    TimelineDirectorSystem* m_directorSystem = nullptr;

    bool m_isRunning = false;
};
