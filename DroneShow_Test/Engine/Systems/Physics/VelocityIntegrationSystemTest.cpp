#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/VelocityIntegrationSystem.h"

class VelocityIntegrationTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    VelocityIntegrationSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<VelocityIntegrationSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Acceleration: Verify velocity is updated by force * dt
// ---------------------------------------------------------
TEST_F(VelocityIntegrationTestFixture, Integration_AcceleratesVelocity)
{
    // Arrange: Velocity at zero, force {100, 0, 0}, dt = 0.5
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });

    // Act: dt = 0.5 (mass = 1.0 assumed)
    system->Update(coordinator, 0.5f, 0.0);

    // Assert: velocity = {0,0,0} + {100,0,0} * 0.5 = {50, 0, 0}
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 50.0f, 0.001f);
    EXPECT_NEAR(velocity.y, 0.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Accumulation: Verify force adds to existing velocity
// ---------------------------------------------------------
TEST_F(VelocityIntegrationTestFixture, Integration_AccumulatesWithExistingVelocity)
{
    // Arrange: Existing velocity {10, 0, 0}, force {20, 0, 0}, dt = 1.0
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 10.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 20.0f, 0.0f, 0.0f } });

    // Act: dt = 1.0
    system->Update(coordinator, 1.0f, 0.0);

    // Assert: velocity = {10 + 20*1, 0, 0} = {30, 0, 0}
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 30.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Zero Force: Verify velocity is unchanged with zero force
// ---------------------------------------------------------
TEST_F(VelocityIntegrationTestFixture, Integration_ZeroForce_NoChange)
{
    // Arrange
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 42.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.5f, 0.0);

    // Assert: Velocity remains unchanged
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 42.0f, 0.001f);
}
