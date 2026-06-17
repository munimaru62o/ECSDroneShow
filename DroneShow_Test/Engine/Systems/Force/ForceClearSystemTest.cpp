#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/ForceClearSystem.h"

class ForceClearTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    ForceClearSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<ForceClearSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Clear: Verify force is reset to zero
// ---------------------------------------------------------
TEST_F(ForceClearTestFixture, ForceClear_ResetsForceToZero)
{
    // Arrange: Entity with a non-zero force
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 100.0f, -50.0f, 25.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force should be zeroed out
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Multiple Entities: Verify all entities are cleared
// ---------------------------------------------------------
TEST_F(ForceClearTestFixture, ForceClear_ClearsAllEntities)
{
    // Arrange: Multiple entities with various forces
    Entity e1 = coordinator.CreateEntity();
    Entity e2 = coordinator.CreateEntity();
    Entity e3 = coordinator.CreateEntity();
    coordinator.AddComponent(e1, ForceComponent{ .value = Vector3{ 1.0f, 2.0f, 3.0f } });
    coordinator.AddComponent(e2, ForceComponent{ .value = Vector3{ -10.0f, 20.0f, -30.0f } });
    coordinator.AddComponent(e3, ForceComponent{ .value = Vector3{ 999.0f, 999.0f, 999.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: All forces should be zero
    EXPECT_NEAR(coordinator.GetComponent<ForceComponent>(e1).value.Length(), 0.0f, 0.001f);
    EXPECT_NEAR(coordinator.GetComponent<ForceComponent>(e2).value.Length(), 0.0f, 0.001f);
    EXPECT_NEAR(coordinator.GetComponent<ForceComponent>(e3).value.Length(), 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Already Zero: Verify no side effects when force is already zero
// ---------------------------------------------------------
TEST_F(ForceClearTestFixture, ForceClear_HandlesZeroForce)
{
    // Arrange: Entity with zero force
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force remains zero
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 0.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}
