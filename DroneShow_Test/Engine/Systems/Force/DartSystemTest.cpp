#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/DartSystem.h"

class DartTestFixture : public ECSTestFixture
{
protected:
    // sonar-ignore-next-line
    DartSystem* system = nullptr;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<DartSystem>();
    }
};

// ---------------------------------------------------------
// 1. Cached Direction: Verify cached direction is applied when not yet time to dart
// ---------------------------------------------------------
TEST_F(DartTestFixture, Dart_AppliesCachedDirection)
{
    // Arrange: nextDartTime is in the future, with a pre-cached direction
    DartComponent dart;
    dart.strength = 10.0f;
    dart.seed = 42;
    dart.nextDartTime = 100.0;  // Far future
    dart.cachedDirection = Vector3{ 1.0f, 0.0f, 0.0f };  // Cached: along +X

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, dart);

    // Act: simulationTime = 1.0 < nextDartTime = 100.0
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Force = cachedDirection * strength = {10, 0, 0}
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.x, 10.0f, 0.001f);
    EXPECT_NEAR(force.y, 0.0f, 0.001f);
    EXPECT_NEAR(force.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. New Direction: Verify a new random direction is generated when dart triggers
// ---------------------------------------------------------
TEST_F(DartTestFixture, Dart_GeneratesNewDirectionOnTrigger)
{
    // Arrange: nextDartTime in the past to trigger a new dart
    DartComponent dart;
    dart.strength = 100.0f;
    dart.seed = 42;
    dart.nextDartTime = 0.0;  // Already past
    dart.intervalMin = 2.0f;
    dart.intervalMax = 5.0f;
    dart.cachedDirection = {};

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, dart);

    // Act: simulationTime = 1.0 > nextDartTime = 0.0
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: Force should be non-zero (new random direction applied)
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_GT(force.Length(), 0.0f);

    // Assert: nextDartTime should have advanced
    auto& updatedDart = coordinator.GetComponent<DartComponent>(drone);
    EXPECT_GT(updatedDart.nextDartTime, 1.0);
}

// ---------------------------------------------------------
// 3. Force Magnitude: Verify force length matches strength (unit sphere direction)
// ---------------------------------------------------------
TEST_F(DartTestFixture, Dart_ForceMagnitudeMatchesStrength)
{
    // Arrange
    DartComponent dart;
    dart.strength = 75.0f;
    dart.seed = 999;
    dart.nextDartTime = 0.0;
    dart.intervalMin = 1.0f;
    dart.intervalMax = 2.0f;
    dart.cachedDirection = {};

    Entity drone = coordinator.CreateEntity();
    coordinator.AddComponent(drone, ForceComponent{ .value = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(drone, dart);

    // Act
    system->Update(coordinator, 0.016f, 1.0);

    // Assert: After trigger, cachedDirection is on unit sphere, so force length = strength
    auto& force = coordinator.GetComponent<ForceComponent>(drone).value;
    EXPECT_NEAR(force.Length(), 75.0f, 0.01f);
}
