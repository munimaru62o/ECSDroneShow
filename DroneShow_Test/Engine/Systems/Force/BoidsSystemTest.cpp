#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Force/BoidsSystem.h"
#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"

class BoidsTestFixture : public ECSTestFixture
{
protected:
    BoidsSystem* boidsSystem;
    SpatialBoidCacheSystem* cacheSystem;

    SpatialGrid grid{ 100.0f, 20 };

    void SetUpSystems() override
    {
        // -------------------------------------------------
        // BoidsSystem Setup
        // -------------------------------------------------
        boidsSystem = coordinator.GetSystem<BoidsSystem>();
        boidsSystem->SetSpatialGrid(&grid);

        // -------------------------------------------------
        // CacheSystem Setup
        // -------------------------------------------------
        cacheSystem = coordinator.GetSystem<SpatialBoidCacheSystem>();
        cacheSystem->SetSpatialGrid(&grid);
        boidsSystem->SetCacheSystem(cacheSystem);
    }

    void BuildGridForTest(const std::vector<Entity>& targetEntities)
    {
        grid.Clear();
        grid.GetEntities().resize(targetEntities.size());

        auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
        auto& spatialArray = coordinator.GetComponentArray<SpatialPartitionComponent>();

        // 1. Determine the cell for each Entity, increment the count, and update the component coordinate
        for (Entity e : targetEntities) {
            const auto& pos = transformArray.GetData(e).position;
            auto coord = grid.GetCellCoordFromPosition(pos);

            // Write back to the component so BoidsSystem can reference it
            spatialArray.GetData(e).coord = coord;

            int flatIndex = grid.GetFlatIndex(coord);
            grid.GetCellCounts()[flatIndex]++;
        }

        // 2. Calculate the Prefix Sum to determine the starting offset for each cell
        int currentOffset = 0;
        for (int i = 0; i < grid.GetTotalCells(); ++i) {
            grid.GetCellOffsets()[i] = currentOffset;
            currentOffset += grid.GetCellCounts()[i];
        }

        // 3. Store Entities in the flat array (grid.entities) based on the calculated offsets
        std::vector<int> currentInsertPos = grid.GetCellOffsets();
        for (Entity e : targetEntities) {
            const auto& pos = transformArray.GetData(e).position;
            auto coord = grid.GetCellCoordFromPosition(pos);
            int flatIndex = grid.GetFlatIndex(coord);

            int insertPos = currentInsertPos[flatIndex]++;
            grid.GetEntities()[insertPos] = e;
        }

        // 4. Update the CacheSystem to write the latest cell data into the cache
        cacheSystem->Update(coordinator, 0.016f, 0.0);
    }
};


// ---------------------------------------------------------
// 1. Separation Test: Ensures a repulsive force is applied when flockmates are too close.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_SeparationPushesApart)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: Place BoidA at origin, and BoidB very close on the +X axis (x=10)
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{10.0f, 0.0f, 0.0f} };
    VelocityComponent velocityA{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    VelocityComponent velocityB{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Enable ONLY Separation
    BoidsComponent boidsParam{
        .separationWeight = 1.0f,
        .alignmentWeight = 0.0f,
        .cohesionWeight = 0.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, spatialA);
    coordinator.AddComponent(boidA, boidsParam);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, spatialB);
    coordinator.AddComponent(boidB, boidsParam);

    // Act
    BuildGridForTest({ boidA, boidB });
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // Since BoidB is at +X (10.0), BoidA should experience a repulsive force toward -X.
    // Distance = 10.0
    // Offset = {0,0,0} - {10,0,0} = {-10, 0, 0}
    // Normalized Offset = {-1, 0, 0}
    // Force applied = (-1, 0, 0) / 10.0 = {-0.1, 0, 0}
    EXPECT_NEAR(resultForceA.x, -0.1f, 0.001f);
    EXPECT_NEAR(resultForceA.y, 0.0f, 0.001f);
    EXPECT_NEAR(resultForceA.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 2. Alignment Test: Ensures force is applied to match the average velocity of flockmates.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_AlignmentMatchesVelocity)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: A is moving slowly along Y, B is moving rapidly along Y
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{0.0f, 0.0f, 10.0f} };
    VelocityComponent velocityA{ .value = Vector3{0.0f, 1.0f, 0.0f} };
    VelocityComponent velocityB{ .value = Vector3{0.0f, 51.0f, 0.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Enable ONLY Alignment
    BoidsComponent boidsParam{
        .separationWeight = 0.0f,
        .alignmentWeight = 1.0f,
        .cohesionWeight = 0.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, spatialA);
    coordinator.AddComponent(boidA, boidsParam);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, spatialB);
    coordinator.AddComponent(boidB, boidsParam);

    // Act
    BuildGridForTest({ boidA, boidB });
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // Alignment calculates the force needed to match the neighbor's velocity.
    // Expected Force = Velocity of B (51) - Velocity of A (1) = 50
    EXPECT_NEAR(resultForceA.x, 0.0f, 0.001f);
    EXPECT_NEAR(resultForceA.y, 50.0f, 0.001f);
    EXPECT_NEAR(resultForceA.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 3. Cohesion Test: Ensures force is applied toward the center of mass of local flockmates.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_CohesionPullsTowardsCenter)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: A is at origin, B is at Z=20
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{0.0f, 0.0f, 20.0f} };
    VelocityComponent velocityA{ .value = Vector3{0.0f, 0.0f, 1.0f} };
    VelocityComponent velocityB{ .value = Vector3{0.0f, 0.0f, 1.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Enable ONLY Cohesion
    BoidsComponent boidsParam{
        .separationWeight = 0.0f,
        .alignmentWeight = 0.0f,
        .cohesionWeight = 1.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, spatialA);
    coordinator.AddComponent(boidA, boidsParam);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, spatialB);
    coordinator.AddComponent(boidB, boidsParam);

    // Act
    BuildGridForTest({ boidA, boidB });
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // The center of mass from A's perspective is B's position (0, 0, 20).
    // The steering vector towards the center should yield a force of 20 along the Z axis.
    EXPECT_NEAR(resultForceA.x, 0.0f, 0.001f);
    EXPECT_NEAR(resultForceA.y, 0.0f, 0.001f);
    EXPECT_NEAR(resultForceA.z, 20.0f, 0.001f);
}

// ---------------------------------------------------------
// 4. MaxForce Clamping Test: Ensures resulting forces do not exceed the specified maximum.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_ClampsToMaxForce)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: 
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{0.0f, 0.0f, 10.0f} };
    VelocityComponent velocityA{ .value = Vector3{0.0f, 1.0f, 0.0f} };
    VelocityComponent velocityB{ .value = Vector3{0.0f, 10000.0f, 0.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Apply an extreme Alignment weight to guarantee the threshold is exceeded
    BoidsComponent boidsParam{
        .separationWeight = 0.0f,
        .alignmentWeight = 10.0f,
        .cohesionWeight = 0.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
        .nextUpdateTime = 0.0f
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, boidsParam);
    coordinator.AddComponent(boidA, spatialA);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, boidsParam);
    coordinator.AddComponent(boidB, spatialB);

    // Act
    BuildGridForTest({ boidA, boidB });
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // The raw calculated force would be enormous (approx. 100,000), 
    // but the system must clamp the length of the vector to exactly maxForce (1000.0).
    EXPECT_NEAR(resultForceA.Length(), 1000.0f, 0.001f);
}

// ---------------------------------------------------------
// 5. Time-Slicing (Caching) Test: Ensures expensive calculations are skipped between intervals.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_TimeSlicingUsesCachedDirection)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: Place them close to each other
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{5.0f, 0.0f, 0.0f} };
    VelocityComponent velocityA{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    VelocityComponent velocityB{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Set nextUpdateTime to the FUTURE (e.g., simulationTime is 1.0, nextUpdate is 2.0).
    // Pre-populate cachedDirection to verify it is applied directly.
    BoidsComponent boidsParam{
        .separationWeight = 1.0f,
        .alignmentWeight = 1.0f,
        .cohesionWeight = 1.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
        .cachedDirection = Vector3{0.0f, 99.0f, 0.0f}, // Artificial cached value
        .nextUpdateTime = 2.0 // Future time
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, spatialA);
    coordinator.AddComponent(boidA, boidsParam);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, spatialB);
    coordinator.AddComponent(boidB, boidsParam);

    // Act
    BuildGridForTest({ boidA, boidB });
    // Execute update at simulationTime 1.0 (earlier than nextUpdateTime)
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // Because it's not time for a full update, the system should completely ignore 
    // neighbor calculations and directly apply the cachedDirection (0, 99, 0).
    EXPECT_NEAR(resultForceA.x, 0.0f, 0.001f);
    EXPECT_NEAR(resultForceA.y, 99.0f, 0.001f);
    EXPECT_NEAR(resultForceA.z, 0.0f, 0.001f);
}

// ---------------------------------------------------------
// 6. Vision Radius Test: Ensures boids outside the vision radius are ignored.
// ---------------------------------------------------------
TEST_F(BoidsTestFixture, Boids_IgnoresEntitiesOutsideVisionRadius)
{
    Entity boidA = coordinator.CreateEntity();
    Entity boidB = coordinator.CreateEntity();

    // Arrange: Place BoidB far outside the vision radius
    TransformComponent transformA{ .position = Vector3{0.0f, 0.0f, 0.0f} };
    TransformComponent transformB{ .position = Vector3{100.0f, 0.0f, 0.0f} }; // Distance is 100
    VelocityComponent velocityA{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    VelocityComponent velocityB{ .value = Vector3{1.0f, 0.0f, 0.0f} };
    ForceComponent forceA{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    ForceComponent forceB{ .value = Vector3{0.0f, 0.0f, 0.0f} };
    SpatialPartitionComponent spatialA{};
    SpatialPartitionComponent spatialB{};

    // Set visionRadius to 50.0 (BoidB is at 100.0)
    BoidsComponent boidsParam{
        .separationWeight = 1.0f,
        .alignmentWeight = 1.0f,
        .cohesionWeight = 1.0f,
        .visionRadius = 50.0f,
        .maxForce = 1000.0f,
    };

    coordinator.AddComponent(boidA, transformA);
    coordinator.AddComponent(boidA, velocityA);
    coordinator.AddComponent(boidA, forceA);
    coordinator.AddComponent(boidA, spatialA);
    coordinator.AddComponent(boidA, boidsParam);

    coordinator.AddComponent(boidB, transformB);
    coordinator.AddComponent(boidB, velocityB);
    coordinator.AddComponent(boidB, forceB);
    coordinator.AddComponent(boidB, spatialB);
    coordinator.AddComponent(boidB, boidsParam);

    // Act
    BuildGridForTest({ boidA, boidB });
    boidsSystem->Update(coordinator, 0.016f, 1.0);

    auto& resultForceA = coordinator.GetComponent<ForceComponent>(boidA).value;

    // Assert:
    // Since BoidB is outside the visionRadius, BoidA should perceive NO neighbors,
    // resulting in a net steering force of exactly zero.
    EXPECT_NEAR(resultForceA.LengthSq(), 0.0f, 0.001f);
}
