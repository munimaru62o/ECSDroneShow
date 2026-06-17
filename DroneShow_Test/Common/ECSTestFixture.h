#pragma once

#include <gtest/gtest.h>
#include "Engine/ECS/Coordinator.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Registration/RegisterAllComponents.h"
#include "Game/Registration/RegisterAllSystems.h"

/**
 * @class ECSTestFixture
 * @brief A shared base test fixture for ECS integration tests.
 *
 * This fixture provides a fully initialized ECS environment where all game-level
 * components and systems are pre-registered. It serves as a robust and convenient
 * foundation for testing actual game logic and complex system interactions.
 */
class ECSTestFixture : public ::testing::Test
{
    /**
     * @brief Setup method executed immediately before each TEST_F runs.
     * * Initializes the Coordinator, registers all core and game-specific components
     * and systems, allows derived fixtures to perform custom system configurations,
     * and finally initializes all systems.
     */
    void SetUp() override
    {
        coordinator.Init();
        GameRegistrations::RegisterAllComponents(registry, coordinator);
        GameRegistrations::RegisterAllSystems(coordinator);

        SetUpSystems();
        coordinator.InitSystems();
    }

    /**
     * @brief A virtual hook for derived test fixtures to configure or retrieve
     * specific systems before Coordinator::InitSystems() is called.
     */
    virtual void SetUpSystems()
    {
        // Implementation is optional.
    }

    /**
     * @brief Teardown method executed immediately after each TEST_F completes.
     * Useful for safely cleaning up memory or resetting global states.
     */
    virtual void TearDown() override
    {
        // Implementation is optional.
    }

// sonar-ignore-next-line
protected:
    Coordinator coordinator;
    ComponentRegistry registry;
};
