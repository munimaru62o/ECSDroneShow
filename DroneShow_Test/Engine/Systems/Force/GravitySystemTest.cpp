#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/GravitySystem.h"

class GravityTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    GravitySystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<GravitySystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Gravity: Verify downward force is accumulated correctly
// ---------------------------------------------------------
TEST_F(GravityTestFixture, Gravity_AppliesDownwardForce)
{
    // Arrange: Default gravity direction is Down {0, -1, 0}, strength 100
    GravityComponent gravity;
    gravity.direction = Vector3::Down();
    gravity.strength = 100.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, gravity);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force should be {0, -100, 0}
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, -100.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Custom Direction: Verify gravity in an arbitrary direction
// ---------------------------------------------------------
TEST_F(GravityTestFixture, Gravity_CustomDirection)
{
    // Arrange: Gravity along +X axis
    GravityComponent gravity;
    gravity.direction = Vector3{ 1.0f, 0.0f, 0.0f };
    gravity.strength = 50.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, gravity);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force should be {50, 0, 0}
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 50.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Accumulation: Verify gravity adds to existing force
// ---------------------------------------------------------
TEST_F(GravityTestFixture, Gravity_AccumulatesForce)
{
    // Arrange: Pre-existing force of {10, 0, 0}
    GravityComponent gravity;
    gravity.direction = Vector3::Down();
    gravity.strength = 50.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 10.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, gravity);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force should be {10, -50, 0} (accumulated)
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 10.0f, 0.001f);
    EXPECT_NEAR(force.y, -50.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}
