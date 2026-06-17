#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/TransformIntegrationSystem.h"

class TransformIntegrationTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    TransformIntegrationSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<TransformIntegrationSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Integration: Verify position is updated by velocity * dt
// ---------------------------------------------------------
TEST_F(TransformIntegrationTestFixture, Integration_UpdatesPosition)
{
    // Arrange: Position at origin, velocity {100, 0, 0}, dt = 0.5
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });

    // Act: dt = 0.5
    system->Update(coordinator, 0.5f, 0.0);

    // Assert: position = {0,0,0} + {100,0,0} * 0.5 = {50, 0, 0}
    auto& pos = coordinator.GetComponent<TransformComponent>(drone).position;
    EXPECT_NEAR(pos.x, 50.0f, 0.001f);
    EXPECT_NEAR(pos.y, 0.0f, 0.001f);
    EXPECT_NEAR(pos.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Multi-Axis: Verify 3D velocity integration
// ---------------------------------------------------------
TEST_F(TransformIntegrationTestFixture, Integration_3DMovement)
{
    // Arrange: Position at {10, 20, 30}, velocity {-5, 10, 15}, dt = 1.0
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 10.0f, 20.0f, 30.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ -5.0f, 10.0f, 15.0f } });

    // Act: dt = 1.0
    system->Update(coordinator, 1.0f, 0.0);

    // Assert: position = {10-5, 20+10, 30+15} = {5, 30, 45}
    auto& pos = coordinator.GetComponent<TransformComponent>(drone).position;
    EXPECT_NEAR(pos.x, 5.0f, 0.001f);
    EXPECT_NEAR(pos.y, 30.0f, 0.001f);
    EXPECT_NEAR(pos.z, 45.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Zero Velocity: Verify position is unchanged with zero velocity
// ---------------------------------------------------------
TEST_F(TransformIntegrationTestFixture, Integration_ZeroVelocity_NoMovement)
{
    // Arrange
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 42.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.5f, 0.0);

    // Assert: Position remains unchanged
    auto& pos = coordinator.GetComponent<TransformComponent>(drone).position;
    EXPECT_NEAR(pos.x, 42.0f, 0.001f);
}
