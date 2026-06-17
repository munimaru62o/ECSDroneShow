#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/SwirlSystem.h"

class SwirlTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    SwirlSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<SwirlSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Tangential Force: Verify cross product generates correct tangent direction
// ---------------------------------------------------------
TEST_F(SwirlTestFixture, Swirl_AppliesTangentialForce)
{
    // Arrange: Entity at {100, 0, 0}, center at origin, axis is Up {0, 1, 0}
    // Direction to center = {-100, 0, 0}, normalized = {-1, 0, 0}
    // Cross({-1,0,0}, {0,1,0}) = {0*0-0*1, 0*0-(-1)*0, (-1)*1-0*0} = {0, 0, -1}
    // Wait: Cross(dirToCenter, axis) = Cross({-1,0,0}, {0,1,0})
    //   = {(0*0 - 0*1), (0*0 - (-1)*0), ((-1)*1 - 0*0)} = {0, 0, -1}
    SwirlComponent swirl;
    swirl.center = Vector3{ 0.0f, 0.0f, 0.0f };
    swirl.axis = Vector3{ 0.0f, 1.0f, 0.0f };
    swirl.strength = 10.0f;
    swirl.maxForce = 500.0f;
    swirl.profile.falloffType = ForceFalloffType::Constant;
    swirl.profile.innerScale = 1.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, swirl);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Tangential force should be along -Z (counterclockwise when viewed from above)
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, -10.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. MaxForce Clamping: Verify swirl force is clamped
// ---------------------------------------------------------
TEST_F(SwirlTestFixture, Swirl_ClampsToMaxForce)
{
    // Arrange: Very strong swirl
    SwirlComponent swirl;
    swirl.center = Vector3{ 0.0f, 0.0f, 0.0f };
    swirl.axis = Vector3{ 0.0f, 1.0f, 0.0f };
    swirl.strength = 10000.0f;
    swirl.maxForce = 50.0f;
    swirl.profile.falloffType = ForceFalloffType::Constant;
    swirl.profile.innerScale = 1.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, swirl);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Magnitude is clamped to 50
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.Length(), 50.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. At Center: Verify no force when entity is exactly at the swirl center
// ---------------------------------------------------------
TEST_F(SwirlTestFixture, Swirl_NoForceAtCenter)
{
    // Arrange: Entity exactly at the center
    SwirlComponent swirl;
    swirl.center = Vector3{ 0.0f, 0.0f, 0.0f };
    swirl.axis = Vector3{ 0.0f, 1.0f, 0.0f };
    swirl.strength = 100.0f;
    swirl.maxForce = 500.0f;
    swirl.profile.falloffType = ForceFalloffType::Constant;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, swirl);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: No force because distance to center is zero
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.Length(), 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 4. Off-Center: Verify tangential force when center is not at origin
// ---------------------------------------------------------
TEST_F(SwirlTestFixture, Swirl_OffCenterCalculation)
{
    // Arrange: Swirl center at {50, 0, 50}. Entity at {100, 0, 50}.
    // Direction to center = {-50, 0, 0}. Normalized dirToCenter = {-1, 0, 0}.
    // Cross({-1, 0, 0}, {0, 1, 0}) = {0, 0, -1}
    SwirlComponent swirl;
    swirl.center = Vector3{ 50.0f, 0.0f, 50.0f }; // Center is offset in X and Z
    swirl.axis = Vector3{ 0.0f, 1.0f, 0.0f };     // Upward axis
    swirl.strength = 10.0f;
    swirl.maxForce = 500.0f;
    swirl.profile.falloffType = ForceFalloffType::Constant;
    swirl.profile.innerScale = 1.0f;

    Entity drone = coordinator.CreateEntity();
    // Drone is positioned at X=100, Z=50.
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 100.0f, 0.0f, 50.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, swirl);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Tangential force should be along -Z
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, -10.0f, 0.001f);
}
