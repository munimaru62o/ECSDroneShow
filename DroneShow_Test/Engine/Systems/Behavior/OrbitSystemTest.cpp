#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Behavior/OrbitSystem.h"

class OrbitTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    OrbitSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<OrbitSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Orbit: Verify target position is rotated around the axis
// ---------------------------------------------------------
TEST_F(OrbitTestFixture, Orbit_RotatesTarget)
{
    // Arrange: Target at {100, 0, 0}, orbit around Y axis, speed = PI (180 degrees/sec)
    OrbitComponent orbit;
    orbit.creationTime = 0.0;
    orbit.speed = 3.14159265f; // PI rad/s
    orbit.duration = 0.0f; // Infinite
    orbit.axis = Vector3::Up();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, orbit);

    // Act: At simulationTime = 0.5, angle = PI * 0.5 = PI/2 (90 degrees)
    // Rotating {100, 0, 0} by 90 degrees around Y should give approximately {0, 0, -100}
    system->Update(coordinator, 0.016f, 0.5);

    // Assert: Target should be rotated ~90 degrees around Y
    auto& target = coordinator.GetComponent<TargetComponent>(drone).value;
    EXPECT_NEAR(target.x, 0.0f, 0.1f);
    EXPECT_NEAR(target.y, 0.0f, 0.001f);
    EXPECT_NEAR(target.z, -100.0f, 0.1f);
}

// ---------------------------------------------------------
// 2. Duration Expiry: Verify component is removed after duration
// ---------------------------------------------------------
TEST_F(OrbitTestFixture, Orbit_RemovedAfterDuration)
{
    // Arrange: Duration = 1.0 second
    OrbitComponent orbit;
    orbit.creationTime = 0.0;
    orbit.speed = 1.0f;
    orbit.duration = 1.0f;
    orbit.axis = Vector3::Up();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, orbit);

    // Act: At simulationTime = 2.0, elapsed = 2.0 > duration 1.0
    system->Update(coordinator, 0.016f, 2.0);
    coordinator.EndFrame();

    // Assert: OrbitComponent should be removed
    EXPECT_FALSE(coordinator.HasComponent<OrbitComponent>(drone));
}

// ---------------------------------------------------------
// 3. Infinite Duration: Verify orbit continues indefinitely when duration is 0
// ---------------------------------------------------------
TEST_F(OrbitTestFixture, Orbit_InfiniteDurationContinues)
{
    // Arrange: Duration = 0 (infinite)
    OrbitComponent orbit;
    orbit.creationTime = 0.0;
    orbit.speed = 1.0f;
    orbit.duration = 0.0f;
    orbit.axis = Vector3::Up();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, orbit);

    // Act: At a large simulation time
    system->Update(coordinator, 0.016f, 100.0);

    // Assert: Component should still exist
    EXPECT_TRUE(coordinator.HasComponent<OrbitComponent>(drone));
}

// ---------------------------------------------------------
// 4. Full Rotation: Verify target returns to original position after full 360-degree rotation
// ---------------------------------------------------------
TEST_F(OrbitTestFixture, Orbit_FullRotationReturnsToOriginal)
{
    // Arrange: speed = 2*PI, so after 1 second, angle = 2*PI = 360 degrees
    OrbitComponent orbit;
    orbit.creationTime = 0.0;
    orbit.speed = 6.28318530f; // 2*PI
    orbit.duration = 0.0f;
    orbit.axis = Vector3::Up();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TargetComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, orbit);

    // Act: At simulationTime = 1.0, angle = 2*PI
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Target should return to approximately the original position
    auto& target = coordinator.GetComponent<TargetComponent>(drone).value;
    EXPECT_NEAR(target.x, 100.0f, 0.1f);
    EXPECT_NEAR(target.y, 0.0f, 0.001f);
    EXPECT_NEAR(target.z, 0.0f, 0.1f);
}
