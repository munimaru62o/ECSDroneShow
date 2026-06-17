#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/DampingSystem.h"

class DampingTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    DampingSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<DampingSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Damping: Verify velocity is scaled by the damping factor
// ---------------------------------------------------------
TEST_F(DampingTestFixture, Damping_ScalesVelocity)
{
    // Arrange: Velocity {100, 0, 0}, damping factor 0.9
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DampingComponent{ .factor = 0.9f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity should be {90, 0, 0}
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 90.0f, 0.001f);
    EXPECT_NEAR(velocity.y, 0.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Full Damping: Verify velocity is zeroed when factor is 0
// ---------------------------------------------------------
TEST_F(DampingTestFixture, Damping_ZeroFactor_StopsCompletely)
{
    // Arrange: factor = 0.0 (full stop)
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 50.0f, 30.0f, -20.0f } });
    coordinator.AddComponent(drone, DampingComponent{ .factor = 0.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity should be completely zero
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.Length(), 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. No Damping: Verify velocity is unchanged when factor is 1.0
// ---------------------------------------------------------
TEST_F(DampingTestFixture, Damping_FullFactor_NoChange)
{
    // Arrange: factor = 1.0 (no damping)
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 50.0f, 30.0f, -20.0f } });
    coordinator.AddComponent(drone, DampingComponent{ .factor = 1.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity should be unchanged
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 50.0f, 0.001f);
    EXPECT_NEAR(velocity.y, 30.0f, 0.001f);
    EXPECT_NEAR(velocity.z, -20.0f, 0.001f);
}
