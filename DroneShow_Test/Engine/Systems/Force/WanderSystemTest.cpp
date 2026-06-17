#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/WanderSystem.h"

class WanderTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    WanderSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<WanderSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Wander: Verify force is applied when time exceeds nextUpdateTime
// ---------------------------------------------------------
TEST_F(WanderTestFixture, Wander_AppliesForceOnUpdate)
{
    // Arrange: nextUpdateTime = 0.0 (should update immediately)
    WanderComponent wander;
    wander.strength = 100.0f;
    wander.frequency = 0.5f;
    wander.seed = 42;
    wander.nextUpdateTime = 0.0;
    wander.updateInterval = 0.2f;
    wander.updateIntervalScale = 1.0f;
    wander.cachedDirection = {};

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, wander);

    // Act: simulationTime = 1.0 > nextUpdateTime = 0.0, so noise will be computed
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Force should be non-zero
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_GT(force.Length(), 0.0f);
}

// ---------------------------------------------------------
// 2. Time-Slicing: Verify cached direction is used between update intervals
// ---------------------------------------------------------
TEST_F(WanderTestFixture, Wander_UsesCachedDirectionBetweenIntervals)
{
    // Arrange: Set nextUpdateTime far in the future, with a pre-populated cached direction
    WanderComponent wander;
    wander.strength = 10.0f;
    wander.frequency = 0.5f;
    wander.seed = 42;
    wander.nextUpdateTime = 100.0;  // Far future
    wander.updateInterval = 0.2f;
    wander.updateIntervalScale = 1.0f;
    wander.cachedDirection = Vector3{ 0.0f, 1.0f, 0.0f }; // Cached: straight up

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, wander);

    // Act: simulationTime = 1.0 < nextUpdateTime = 100.0, so cached direction is used
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Force should be cachedDirection {0, 1, 0} * strength 10.0 = {0, 10, 0}
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 10.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. NextUpdateTime Advance: Verify nextUpdateTime is updated after computation
// ---------------------------------------------------------
TEST_F(WanderTestFixture, Wander_AdvancesNextUpdateTime)
{
    // Arrange
    WanderComponent wander;
    wander.strength = 100.0f;
    wander.frequency = 0.5f;
    wander.seed = 42;
    wander.nextUpdateTime = 0.0;
    wander.updateInterval = 0.5f;
    wander.updateIntervalScale = 1.0f;
    wander.cachedDirection = {};

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, wander);

    // Act
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: nextUpdateTime should have advanced beyond simulationTime
    auto& updatedWander = coordinator.GetComponent<WanderComponent>(drone);
    EXPECT_GT(updatedWander.nextUpdateTime, 1.0);
}
