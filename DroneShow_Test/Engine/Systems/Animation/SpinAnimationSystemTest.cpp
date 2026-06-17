#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Animation/SpinAnimationSystem.h"

class SpinAnimationTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    SpinAnimationSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<SpinAnimationSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Spin: Verify rotation changes over time
// ---------------------------------------------------------
TEST_F(SpinAnimationTestFixture, Spin_ChangesRotation)
{
    // Arrange: Spin around Y axis, speed 1.0 rad/s
    SpinAnimationComponent spin;
    spin.baseRotation = Quaternion::Identity();
    spin.axis = Vector3::Up();
    spin.speed = 1.0f;
    spin.duration = 0.0f; // Infinite
    spin.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity() });
    coordinator.AddComponent(drone, spin);

    // Act: At simulationTime = 1.0, angle = 1.0 * 1.0 = 1.0 radian
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Rotation should have changed from identity
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;
    Quaternion expected = Quaternion::FromAxisAngle(Vector3::Up(), 1.0f);

    EXPECT_NEAR(rotation.w, expected.w, 0.001f);
    EXPECT_NEAR(rotation.x, expected.x, 0.001f);
    EXPECT_NEAR(rotation.y, expected.y, 0.001f);
    EXPECT_NEAR(rotation.z, expected.z, 0.001f);
}

// ---------------------------------------------------------
// 2. Duration Expiry: Verify component is removed after duration expires
// ---------------------------------------------------------
TEST_F(SpinAnimationTestFixture, Spin_RemovedAfterDuration)
{
    // Arrange: Duration = 1.0 second
    SpinAnimationComponent spin;
    spin.baseRotation = Quaternion::Identity();
    spin.axis = Vector3::Up();
    spin.speed = 1.0f;
    spin.duration = 1.0f;
    spin.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity() });
    coordinator.AddComponent(drone, spin);

    // Act: At simulationTime = 2.0, elapsed = 2.0 > duration 1.0
    system->Update(coordinator, 0.016f, 2.0);
    coordinator.EndFrame();

    // Assert: SpinAnimationComponent should be removed
    EXPECT_FALSE(coordinator.HasComponent<SpinAnimationComponent>(drone));
}

// ---------------------------------------------------------
// 3. Infinite Duration: Verify spin continues indefinitely when duration is 0
// ---------------------------------------------------------
TEST_F(SpinAnimationTestFixture, Spin_InfiniteDurationContinues)
{
    // Arrange: Duration = 0 (infinite)
    SpinAnimationComponent spin;
    spin.baseRotation = Quaternion::Identity();
    spin.axis = Vector3::Up();
    spin.speed = 1.0f;
    spin.duration = 0.0f;
    spin.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity() });
    coordinator.AddComponent(drone, spin);

    // Act: Even at a very large simulation time, the spin should still be active
    system->Update(coordinator, 0.016f, 1000.0);

    // Assert: SpinAnimationComponent should still exist
    EXPECT_TRUE(coordinator.HasComponent<SpinAnimationComponent>(drone));

    // Assert: Rotation should have changed significantly
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;
    EXPECT_NE(rotation.w, 1.0f); // Not identity
}

// ---------------------------------------------------------
// 4. Base Rotation Composition: Verify spin is applied on top of base rotation
// ---------------------------------------------------------
TEST_F(SpinAnimationTestFixture, Spin_ComposesWithBaseRotation)
{
    // Arrange: Base rotation is 90 degrees around Y
    Quaternion baseRot = Quaternion::FromAxisAngle(Vector3::Up(), 1.5708f); // ~90 degrees

    SpinAnimationComponent spin;
    spin.baseRotation = baseRot;
    spin.axis = Vector3::Up();
    spin.speed = 1.0f;
    spin.duration = 0.0f;
    spin.creationTime = 0.0;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity() });
    coordinator.AddComponent(drone, spin);

    // Act: At simulationTime = 0 (no elapsed), spin angle = 0
    // Result = baseRotation * FromAxisAngle(Up, 0) = baseRotation * Identity = baseRotation
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Rotation should equal the base rotation
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;
    EXPECT_NEAR(rotation.w, baseRot.w, 0.001f);
    EXPECT_NEAR(rotation.x, baseRot.x, 0.001f);
    EXPECT_NEAR(rotation.y, baseRot.y, 0.001f);
    EXPECT_NEAR(rotation.z, baseRot.z, 0.001f);
}
