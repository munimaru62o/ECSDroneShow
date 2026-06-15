#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/AttractionSystem.h"

class AttractionTestFixture : public ECSTestFixture
{
protected:
    AttractionSystem* system;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<AttractionSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Attraction Test: Verify constant pull towards the center
// ---------------------------------------------------------
TEST_F(AttractionTestFixture, Attraction_ConstantPull)
{
    // Arrange: Center at {100, 0, 0}, Entity at {0, 0, 0}. Strength 10.0f
    AttractionComponent attr;
    attr.center = Vector3{ 100.0f, 0.0f, 0.0f };
    attr.strength = 10.0f;
    attr.maxForce = 100.0f;
    attr.profile.falloffType = ForceFalloffType::Constant;
    attr.profile.innerScale = 1.0f;
    attr.plane = ForcePlaneType::XYZ;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, attr);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Pulling towards {100, 0, 0}. Vector is {100, 0, 0}, magnitude 10.0
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 10.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Falloff Test: Verify Linear decay (Strength decreases over distance)
// ---------------------------------------------------------
TEST_F(AttractionTestFixture, Attraction_LinearFalloff)
{
    // Arrange: Radius 100, at distance 50 (t=0.5). Linear falloff: lerp(1, 0, 0.5) = 0.5
    AttractionComponent attr;
    attr.center = Vector3{ 100.0f, 0.0f, 0.0f };
    attr.strength = 10.0f;
    attr.maxForce = 100.0f;
    attr.profile.falloffType = ForceFalloffType::Linear;
    attr.profile.radius = 100.0f;
    attr.profile.invRadius = 0.01f;
    attr.profile.innerScale = 1.0f;
    attr.profile.outerScale = 0.0f;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 50.0f, 0.0f, 0.0f } }); // t=0.5
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, attr);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Strength 10 * 0.5 = 5.0
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 5.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Constrained Plane Test: Verify projection onto XZ plane (Ignore Y)
// ---------------------------------------------------------
TEST_F(AttractionTestFixture, Attraction_ProjectXZ)
{
    // Arrange: Entity at {0, 10, 0}, Center at {10, 0, 0}. Constrain to XZ plane
    AttractionComponent attr;
    attr.center = Vector3{ 10.0f, 0.0f, 0.0f };
    attr.strength = 10.0f;
    attr.maxForce = 100.0f;
    attr.profile.falloffType = ForceFalloffType::Constant;
    attr.plane = ForcePlaneType::XZ; // Ignore Y

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 10.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, attr);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Vector to center is {10, -10, 0}. Projected on XZ is {10, 0, 0}.
    // Normalized to 1.0 * Strength 10.0 = {10.0, 0.0, 0.0}
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 10.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 4. MaxForce Clamping Test: Verify the force doesn't explode
// ---------------------------------------------------------
TEST_F(AttractionTestFixture, Attraction_ClampsToMaxForce)
{
    // Arrange: Huge strength, force should be clamped to maxForce
    AttractionComponent attr;
    attr.center = Vector3{ 1000.0f, 0.0f, 0.0f };
    attr.strength = 10000.0f;
    attr.maxForce = 50.0f; // Clamped to 50
    attr.profile.falloffType = ForceFalloffType::Constant;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, attr);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Length must be 50.0
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.Length(), 50.0f, 0.001f);
}
