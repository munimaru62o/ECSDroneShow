#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/SpeedConstraintSystem.h"

class SpeedConstraintTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    SpeedConstraintSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<SpeedConstraintSystem>();
    }
};

// ---------------------------------------------------------
// 1. Exceeds Max: Verify velocity is clamped to max speed
// ---------------------------------------------------------
TEST_F(SpeedConstraintTestFixture, SpeedConstraint_ClampsToMax)
{
    // Arrange: Speed = 200, max = 100
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 200.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, SpeedConstraintComponent{ .max = 100.0f, .min = 0.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Speed should be clamped to 100, direction preserved
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.Length(), 100.0f, 0.001f);
    EXPECT_NEAR(velocity.x, 100.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Below Min: Verify velocity is boosted to min speed
// ---------------------------------------------------------
TEST_F(SpeedConstraintTestFixture, SpeedConstraint_BoostsToMin)
{
    // Arrange: Speed = 5, min = 50
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 5.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, SpeedConstraintComponent{ .max = 200.0f, .min = 50.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Speed should be boosted to 50, direction preserved
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.Length(), 50.0f, 0.001f);
    EXPECT_NEAR(velocity.x, 50.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Within Range: Verify velocity is unchanged when within bounds
// ---------------------------------------------------------
TEST_F(SpeedConstraintTestFixture, SpeedConstraint_NoChangeInRange)
{
    // Arrange: Speed = 75, min = 50, max = 100
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 75.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, SpeedConstraintComponent{ .max = 100.0f, .min = 50.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity unchanged
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 75.0f, 0.001f);
}

// ---------------------------------------------------------
// 4. Zero Velocity with Min: Verify zero velocity is NOT boosted (avoids division by zero)
// ---------------------------------------------------------
TEST_F(SpeedConstraintTestFixture, SpeedConstraint_ZeroVelocity_NoBoost)
{
    // Arrange: Zero velocity, but min speed is set
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, SpeedConstraintComponent{ .max = 100.0f, .min = 50.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity remains zero (cannot determine direction from zero vector)
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.Length(), 0.0f, 0.001f);
}
