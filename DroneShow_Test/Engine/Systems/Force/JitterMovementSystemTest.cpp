#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/JitterMovementSystem.h"

class JitterMovementTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    JitterMovementSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<JitterMovementSystem>();
    }
};

// ---------------------------------------------------------
// 1. Non-Zero Force: Verify jitter produces a non-zero force
// ---------------------------------------------------------
TEST_F(JitterMovementTestFixture, Jitter_ProducesNonZeroForce)
{
    // Arrange
    JitterMovementComponent noise;
    noise.strength = 100.0f;
    noise.seed = 42;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, noise);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Force should be non-zero (random direction * strength)
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_GT(force.Length(), 0.0f);
}

// ---------------------------------------------------------
// 2. Force Magnitude: Verify force length matches strength
// ---------------------------------------------------------
TEST_F(JitterMovementTestFixture, Jitter_ForceMagnitudeMatchesStrength)
{
    // Arrange: OnUnitSphere returns a normalized vector, so magnitude = strength
    JitterMovementComponent noise;
    noise.strength = 50.0f;
    noise.seed = 123;

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, noise);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Length should be exactly the strength (unit sphere * strength)
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.Length(), 50.0f, 0.01f);
}

// ---------------------------------------------------------
// 3. Deterministic Seed: Verify same seed produces same result
// ---------------------------------------------------------
TEST_F(JitterMovementTestFixture, Jitter_DeterministicWithSeed)
{
    // Arrange: Two entities with the same seed
    JitterMovementComponent noise1;
    noise1.strength = 100.0f;
    noise1.seed = 999;

    JitterMovementComponent noise2 = noise1; // Same seed

    Entity e1 = coordinator.CreateEntity();
    coordinator.AddComponent(e1, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e1, noise1);

    Entity e2 = coordinator.CreateEntity();
    coordinator.AddComponent(e2, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e2, noise2);

    // Act
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Both entities should have the same force (same seed = same random)
    auto& force1 = coordinator.GetComponent<ForceComponent>(e1).value;
    auto& force2 = coordinator.GetComponent<ForceComponent>(e2).value;
    EXPECT_NEAR(force1.x, force2.x, 0.001f);
    EXPECT_NEAR(force1.y, force2.y, 0.001f);
    EXPECT_NEAR(force1.z, force2.z, 0.001f);
}
