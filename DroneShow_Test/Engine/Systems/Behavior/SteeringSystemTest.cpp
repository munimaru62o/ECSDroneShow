#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Behavior/SteeringSystem.h"

class SteeringTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    SteeringSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<SteeringSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Steering: Verify entity accelerates toward target
// ---------------------------------------------------------
TEST_F(SteeringTestFixture, Steering_AcceleratesTowardTarget)
{
    // Arrange: Entity at origin, target at {100, 0, 0}, zero velocity
    SteeringComponent steering;
    steering.SetParameters(15.0f, 1.0f);

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, steering);

    // Act: dt = 1.0
    system->Update(coordinator, 1.0f, 0.0);

    // Assert: Velocity should have a positive X component (moving toward target)
    // F = k * diff - c * v = 15 * {100,0,0} - c * {0,0,0} = {1500, 0, 0}
    // v += F * dt = {1500, 0, 0}
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_GT(velocity.x, 0.0f);
    EXPECT_NEAR(velocity.y, 0.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Damping Effect: Verify velocity is damped when moving
// ---------------------------------------------------------
TEST_F(SteeringTestFixture, Steering_DampsVelocity)
{
    // Arrange: Entity already at target (diff = 0), but has velocity
    SteeringComponent steering;
    steering.SetParameters(15.0f, 1.0f);

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 50.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, steering);

    // Act: dt = 1.0
    system->Update(coordinator, 1.0f, 0.0);

    // Assert: Since diff = 0, only damping acts: F = -c * v
    // v' = v + F * dt = 50 + (-c * 50) * 1.0
    // c = 2 * 1.0 * sqrt(15) ≈ 7.746
    // F = -7.746 * 50 = -387.3
    // v' = 50 - 387.3 = -337.3 (overshoot damping)
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_LT(velocity.x, 50.0f); // Velocity should have decreased
}

// ---------------------------------------------------------
// 3. Equilibrium: Verify zero velocity when exactly at target
// ---------------------------------------------------------
TEST_F(SteeringTestFixture, Steering_NoForceAtRestOnTarget)
{
    // Arrange: Entity exactly at target with zero velocity
    SteeringComponent steering;
    steering.SetParameters(15.0f, 1.0f);

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 50.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 50.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, steering);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: diff = 0, velocity = 0, so acceleration = 0. Velocity stays zero.
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.Length(), 0.0f, 0.001f);
}
