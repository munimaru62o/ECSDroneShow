#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/LiftSystem.h"

class LiftTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    LiftSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<LiftSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Lift: Verify upward lift force with constant profile
// ---------------------------------------------------------
TEST_F(LiftTestFixture, Lift_AppliesUpwardForce)
{
    // Arrange: Lift center at origin, entity also at origin (zero distance)
    LiftComponent lift;
    lift.center = Vector3{ 0.0f, 0.0f, 0.0f };
    lift.axis = Vector3{ 0.0f, 1.0f, 0.0f }; // Up
    lift.strength = 42.0f;
    lift.maxForce = 500.0f;
    lift.profile.falloffType = ForceFalloffType::Constant;
    lift.profile.innerScale = 1.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, lift);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force along axis (0, 1, 0) * strength 42
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 42.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. MaxForce Clamping: Verify lift force is clamped
// ---------------------------------------------------------
TEST_F(LiftTestFixture, Lift_ClampsToMaxForce)
{
    // Arrange: Very strong lift but low maxForce cap
    LiftComponent lift;
    lift.center = Vector3{ 0.0f, 0.0f, 0.0f };
    lift.axis = Vector3{ 0.0f, 1.0f, 0.0f };
    lift.strength = 10000.0f;
    lift.maxForce = 50.0f;
    lift.profile.falloffType = ForceFalloffType::Constant;
    lift.profile.innerScale = 1.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, lift);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force is clamped to maxForce (50) along the axis
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.y, 50.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Linear Falloff: Verify force scales with distance from center
// ---------------------------------------------------------
TEST_F(LiftTestFixture, Lift_LinearFalloff)
{
    // Arrange: Center at origin, entity at distance 50. Radius 100, so t=0.5
    LiftComponent lift;
    lift.center = Vector3{ 0.0f, 0.0f, 0.0f };
    lift.axis = Vector3{ 0.0f, 1.0f, 0.0f };
    lift.strength = 100.0f;
    lift.maxForce = 500.0f;
    lift.profile.falloffType = ForceFalloffType::Linear;
    lift.profile.radius = 100.0f;
    lift.profile.invRadius = 0.01f;
    lift.profile.innerScale = 1.0f;
    lift.profile.outerScale = 0.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 50.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, lift);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Scale = lerp(1.0, 0.0, 0.5) = 0.5. Force = 100 * 0.5 = 50
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.y, 50.0f, 0.001f);
}

// ---------------------------------------------------------
// 4. Off-Center: Verify offset calculation when center is not at origin
// ---------------------------------------------------------
TEST_F(LiftTestFixture, Lift_OffCenterCalculation)
{
    // Arrange: Lift center at X=100. Entity at X=150. Distance is 50.
    // With radius 100, t=0.5. Scale should be 0.5.
    LiftComponent lift;
    lift.center = Vector3{ 100.0f, 0.0f, 0.0f }; // Center is offset
    lift.axis = Vector3{ 0.0f, 1.0f, 0.0f };     // Upward lift
    lift.strength = 100.0f;
    lift.maxForce = 500.0f;
    lift.profile.falloffType = ForceFalloffType::Linear;
    lift.profile.radius = 100.0f;
    lift.profile.invRadius = 0.01f;
    lift.profile.innerScale = 1.0f;
    lift.profile.outerScale = 0.0f;

    Entity drone = coordinator.CreateEntity();
    // Drone is positioned away from the center, but within the radius
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 150.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, lift);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Distance is 50. t = 50/100 = 0.5. Scale = 0.5. Force = 100 * 0.5 = 50.
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 50.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}
