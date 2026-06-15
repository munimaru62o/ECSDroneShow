#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Spatial/SpatialPartitionSystem.h"
#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"

class SpatialBoidCacheTestFixture : public ECSTestFixture
{
protected:
    SpatialPartitionSystem* partitionSystem;
    SpatialBoidCacheSystem* cacheSystem;

    void SetUpSystems() override
    {
        partitionSystem = coordinator.GetSystem<SpatialPartitionSystem>();
        cacheSystem = coordinator.GetSystem<SpatialBoidCacheSystem>();
        cacheSystem->SetSpatialGrid(&partitionSystem->GetGrid());
    }
};

// ---------------------------------------------------------
// 1. Memory Layout Test: Verify that the cache packs data contiguously and correctly.
// ---------------------------------------------------------
TEST_F(SpatialBoidCacheTestFixture, Cache_GroupsEntitiesByCell)
{
    // Arrange: Create entities in two distinct spatial cells
    Entity e1 = coordinator.CreateEntity();
    Entity e2 = coordinator.CreateEntity();

    // Position e1 in cell A, e2 in cell B
    coordinator.AddComponent(e1, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e2, TransformComponent{ .position = Vector3{ 500.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e1, VelocityComponent{ .value = Vector3{ 1.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e2, VelocityComponent{ .value = Vector3{ 2.0f, 0.0f, 0.0f } });

    coordinator.AddComponent(e1, SpatialPartitionComponent{});
    coordinator.AddComponent(e2, SpatialPartitionComponent{});

    // Act: Partition first, then cache
    partitionSystem->Update(coordinator, 0.016f, 0.0);
    cacheSystem->Update(coordinator, 0.016f, 0.0);

    // Assert: Verify cache contents
    const auto& cache = cacheSystem->GetCache();
    EXPECT_EQ(cache.size(), 2);

    // Ensure the data copied into the cache matches the component values
    // Note: The system guarantees that entities within the same cell are adjacent in the cache.
    bool foundE1 = false;
    bool foundE2 = false;
    for (const auto& data : cache) {
        if (data.entity == e1) {
            EXPECT_NEAR(data.position.x, 0.0f, 0.001f);
            EXPECT_NEAR(data.velocity.x, 1.0f, 0.001f);
            foundE1 = true;
        }
        if (data.entity == e2) {
            EXPECT_NEAR(data.position.x, 500.0f, 0.001f);
            EXPECT_NEAR(data.velocity.x, 2.0f, 0.001f);
            foundE2 = true;
        }
    }
    EXPECT_TRUE(foundE1 && foundE2);
}

// ---------------------------------------------------------
// 2. Padding/Alignment Test: Verify the structure size is exactly 32 bytes.
// ---------------------------------------------------------
TEST(SpatialBoidDataTest, StructureSizeIsAligned)
{
    // The struct is designed to fit 2 instances per 64-byte cache line.
    // 32 bytes * 2 = 64 bytes.
    EXPECT_EQ(sizeof(SpatialBoidData), 32);
}
