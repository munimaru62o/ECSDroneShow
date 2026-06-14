#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/BoundarySystem.h"

class BoundaryTestFixture : public ECSTestFixture
{
protected:
    BoundarySystem* system;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<BoundarySystem>();
    }
};

// ---------------------------------------------------------
// Test: Ensures no force is applied to entities well within the boundary (distance < radius).
// ---------------------------------------------------------
TEST_F(BoundaryTestFixture, IgnoresInsideEntities)
{
    // Arrange: Place the entity safely inside (x=100) a boundary with a radius of 500.
    BoundaryComponent boundaryComponent{ .radius = 500.0f, .force = 10.0f };
    TransformComponent transformComponent{ .position = Vector3{ 100.0f, 0.0f, 0.0f } };
    ForceComponent forceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } };

    Entity testEntity = coordinator.CreateEntity();
    coordinator.AddComponent(testEntity, boundaryComponent);
    coordinator.AddComponent(testEntity, transformComponent);
    coordinator.AddComponent(testEntity, forceComponent);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Since the entity is inside the boundary, the force should remain exactly 0.
    auto& force = coordinator.GetComponent<ForceComponent>(testEntity).value;
    EXPECT_NEAR(force.Length(), 0.0f, 0.001f);
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// Test: Ensures entities that breach the boundary (distance > radius) are pushed back toward the center.
// ---------------------------------------------------------
TEST_F(BoundaryTestFixture, PushesBackOutsideEntities)
{
    // Arrange: Place the entity outside (x=600) a boundary with a radius of 500.
    BoundaryComponent boundaryComponent{ .radius = 500.0f, .force = 10.0f };
    TransformComponent transformComponent{ .position = Vector3{ 600.0f, 0.0f, 0.0f } };
    ForceComponent forceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } };

    Entity testEntity = coordinator.CreateEntity();
    coordinator.AddComponent(testEntity, boundaryComponent);
    coordinator.AddComponent(testEntity, transformComponent);
    coordinator.AddComponent(testEntity, forceComponent);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: 
    // Penetration depth = 600 - 500 = 100
    // Restoring force (push) = 100 * 10.0f = 1000.0f
    // To push it back toward the center (origin), a force of -1000.0f must be applied along the X-axis.
    auto& force = coordinator.GetComponent<ForceComponent>(testEntity).value;
    EXPECT_NEAR(force.x, -1000.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}
