#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/DirectionRotationSystem.h"

class DirectionRotationTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    DirectionRotationSystem<VelocityRotationComponent>* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<DirectionRotationSystem<VelocityRotationComponent>>();
    }
};

// ---------------------------------------------------------
// 1. Basic Rotation: Verify entity rotates toward the direction
// ---------------------------------------------------------
TEST_F(DirectionRotationTestFixture, DirectionRotation_RotatesTowardDirection)
{
    // Arrange: Direction pointing along +X, initial rotation is identity (facing +Z)
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity(), .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 1.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityRotationComponent{ .rotateSpeed = 100.0f });

    // Act: Large dt * rotateSpeed to ensure full rotation
    system->Update(coordinator, 1.0f, 0.0);

    // Assert: Rotation should be close to LookRotation({1, 0, 0}, Up)
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;

    // The entity should now be mostly facing +X. Transform forward should be ~{1,0,0}
    // Since Slerp with large t approaches the target, we verify the quaternion is non-identity
    Quaternion target = Quaternion::LookRotation(Vector3{ 1.0f, 0.0f, 0.0f }, Vector3::Up());

    // The dot product of the two quaternions measures similarity (1.0 = identical)
    float dot = std::abs(rotation.w * target.w + rotation.x * target.x + rotation.y * target.y + rotation.z * target.z);
    EXPECT_GT(dot, 0.95f);
}

// ---------------------------------------------------------
// 2. Zero Direction: Verify rotation is unchanged when direction is zero
// ---------------------------------------------------------
TEST_F(DirectionRotationTestFixture, DirectionRotation_IgnoresZeroDirection)
{
    // Arrange: Zero direction
    Quaternion initialRotation = Quaternion::Identity();

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = initialRotation, .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityRotationComponent{ .rotateSpeed = 10.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Rotation should remain exactly at the initial value
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;
    EXPECT_NEAR(rotation.w, initialRotation.w, 0.001f);
    EXPECT_NEAR(rotation.x, initialRotation.x, 0.001f);
    EXPECT_NEAR(rotation.y, initialRotation.y, 0.001f);
    EXPECT_NEAR(rotation.z, initialRotation.z, 0.001f);
}

// ---------------------------------------------------------
// 3. Slow Rotation: Verify smooth interpolation with low rotateSpeed
// ---------------------------------------------------------
TEST_F(DirectionRotationTestFixture, DirectionRotation_SmoothInterpolation)
{
    // Arrange: Very slow rotation speed
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .rotation = Quaternion::Identity(), .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 1.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityRotationComponent{ .rotateSpeed = 0.1f });

    // Act: Small dt * low speed = very small interpolation step
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Rotation should be very close to identity (barely rotated)
    auto& rotation = coordinator.GetComponent<TransformComponent>(drone).rotation;
    float dotWithIdentity = std::abs(rotation.w);
    EXPECT_GT(dotWithIdentity, 0.99f);
}
