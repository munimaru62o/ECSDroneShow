#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/PlaneConstraintSystem.h"

class PlaneConstraintTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    PlaneConstraintSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<PlaneConstraintSystem>();
    }
};

// ---------------------------------------------------------
// 1. Above Plane: Verify no velocity change when entity is above the plane
// ---------------------------------------------------------
TEST_F(PlaneConstraintTestFixture, PlaneConstraint_NoChangeAbovePlane)
{
    // Arrange: Ground plane (normal = Up, distance = 0). Entity at Y=10 (above plane)
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, 10.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, -5.0f, 0.0f } });
    coordinator.AddComponent(drone, PlaneConstraintComponent{ .normal = Vector3::Up(), .distance = 0.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity is unchanged (entity is above the plane)
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.y, -5.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Below Plane Moving Into: Verify inward velocity component is removed
// ---------------------------------------------------------
TEST_F(PlaneConstraintTestFixture, PlaneConstraint_RemovesInwardVelocity)
{
    // Arrange: Ground plane (normal = Up, distance = 0). Entity at Y=-5 (below plane), moving down
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, -5.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 10.0f, -20.0f, 5.0f } });
    coordinator.AddComponent(drone, PlaneConstraintComponent{ .normal = Vector3::Up(), .distance = 0.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Y velocity (inward component) should be zeroed, X and Z preserved
    // vn = Dot({10, -20, 5}, {0, 1, 0}) = -20. Since vn < 0, subtract normal * vn
    // velocity -= {0, 1, 0} * (-20) = {10, -20+20, 5} = {10, 0, 5}
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.x, 10.0f, 0.001f);
    EXPECT_NEAR(velocity.y, 0.0f, 0.001f);
    EXPECT_NEAR(velocity.z, 5.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Below Plane Moving Away: Verify no change when entity is moving away from the plane
// ---------------------------------------------------------
TEST_F(PlaneConstraintTestFixture, PlaneConstraint_AllowsOutwardVelocity)
{
    // Arrange: Below ground plane but moving UP (away from the plane)
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, TransformComponent{ .position = Vector3{ 0.0f, -5.0f, 0.0f } });
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 10.0f, 0.0f } });
    coordinator.AddComponent(drone, PlaneConstraintComponent{ .normal = Vector3::Up(), .distance = 0.0f });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Velocity is unchanged (moving away from the plane is allowed)
    // vn = Dot({0, 10, 0}, {0, 1, 0}) = 10. Since vn >= 0, no change.
    auto& velocity = coordinator.GetComponent<VelocityComponent>(drone).value;
    EXPECT_NEAR(velocity.y, 10.0f, 0.001f);
}
