#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Physics/VelocityDirectionSystem.h"

class VelocityDirectionTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    VelocityDirectionSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<VelocityDirectionSystem>();
    }
};

// ---------------------------------------------------------
// 1. Basic Direction: Verify direction is normalized velocity
// ---------------------------------------------------------
TEST_F(VelocityDirectionTestFixture, Direction_MatchesNormalizedVelocity)
{
    // Arrange: Velocity along +X
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 100.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Direction should be {1, 0, 0}
    auto& direction = coordinator.GetComponent<DirectionComponent>(drone).value;
    EXPECT_NEAR(direction.x, 1.0f, 0.001f);
    EXPECT_NEAR(direction.y, 0.0f, 0.001f);
    EXPECT_NEAR(direction.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Diagonal Velocity: Verify direction is properly normalized
// ---------------------------------------------------------
TEST_F(VelocityDirectionTestFixture, Direction_NormalizesDiagonalVelocity)
{
    // Arrange: Velocity along {1, 1, 0}, normalized = {0.7071, 0.7071, 0}
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 50.0f, 50.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Direction is normalized
    auto& direction = coordinator.GetComponent<DirectionComponent>(drone).value;
    float expectedComponent = 1.0f / std::sqrt(2.0f);
    EXPECT_NEAR(direction.x, expectedComponent, 0.001f);
    EXPECT_NEAR(direction.y, expectedComponent, 0.001f);
    EXPECT_NEAR(direction.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Zero Velocity: Verify direction is unchanged when velocity is near-zero
// ---------------------------------------------------------
TEST_F(VelocityDirectionTestFixture, Direction_IgnoresZeroVelocity)
{
    // Arrange: Zero velocity, initial direction is {1, 0, 0}
    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, VelocityComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, DirectionComponent{ .value = Vector3{ 1.0f, 0.0f, 0.0f } });

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Direction should remain {1, 0, 0} (unchanged)
    auto& direction = coordinator.GetComponent<DirectionComponent>(drone).value;
    EXPECT_NEAR(direction.x, 1.0f, 0.001f);
    EXPECT_NEAR(direction.y, 0.0f, 0.001f);
    EXPECT_NEAR(direction.z, 0.0f, 0.001f);
}
