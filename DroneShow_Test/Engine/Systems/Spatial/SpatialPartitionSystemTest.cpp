#include "pch.h"
#include "Common/ECSTestFixture.h"
#include "Engine/Systems/Spatial/SpatialPartitionSystem.h"

class SpatialPartitionTestFixture : public ECSTestFixture
{
protected:
    SpatialPartitionSystem* system;

    void SetUpSystems() override
    {
        system = coordinator.GetSystem<SpatialPartitionSystem>();
    }
};

// ---------------------------------------------------------
// 1. Grid Logic Test: Ensure 3D coordinate-to-flat-index conversion
// ---------------------------------------------------------
TEST_F(SpatialPartitionTestFixture, Grid_CoordinateMapping)
{
    auto& grid = system->GetGrid();

    // Test center of the grid (halfGridSize)
    SpatialGrid::CellCoord centerCoord{ 10, 10, 10 };
    int index = grid.GetFlatIndex(centerCoord);

    // Flat index = x + y*size + z*size*size = 10 + 10*20 + 10*20*20 = 10 + 200 + 4000 = 4210
    EXPECT_EQ(index, 4210);

    // Test position to coordinate mapping (e.g., origin {0,0,0})
    auto originCoord = grid.GetCellCoordFromPosition(Vector3{ 0.0f, 0.0f, 0.0f });
    EXPECT_EQ(originCoord.x, 10);
    EXPECT_EQ(originCoord.y, 10);
    EXPECT_EQ(originCoord.z, 10);
}

// ---------------------------------------------------------
// 2. Spatial Partitioning Distribution Test: Verify Counting and Distribution phases
// ---------------------------------------------------------
TEST_F(SpatialPartitionTestFixture, System_DistributesEntitiesCorrectly)
{
    // Arrange: Create two entities in different cells
    Entity e1 = coordinator.CreateEntity();
    Entity e2 = coordinator.CreateEntity();

    coordinator.AddComponent(e1, TransformComponent{ .position = Vector3{ -50.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e2, TransformComponent{ .position = Vector3{ 150.0f, 0.0f, 0.0f } });

    coordinator.AddComponent(e1, SpatialPartitionComponent{});
    coordinator.AddComponent(e2, SpatialPartitionComponent{});

    // Act: Update the system (this triggers Counting, Prefix Sum, and Distribution)
    system->Update(coordinator, 0.016f, 0.0);

    // Assert: Check the resulting grid
    auto& grid = system->GetGrid();
    auto& entities = grid.GetEntities();

    // Verify both entities were successfully stored
    EXPECT_EQ(entities.size(), 2);

    // Verify cell counts are correct (each should have 1 entity in their respective cell)
    int index1 = grid.GetFlatIndex(grid.GetCellCoordFromPosition(Vector3{ -50.0f, 0.0f, 0.0f }));
    int index2 = grid.GetFlatIndex(grid.GetCellCoordFromPosition(Vector3{ 150.0f, 0.0f, 0.0f }));

    EXPECT_EQ(grid.GetCellCounts()[index1], 1);
    EXPECT_EQ(grid.GetCellCounts()[index2], 1);
}

// ---------------------------------------------------------
// 3. Out-of-Bounds Test: Ensure entities outside the grid are clamped
// ---------------------------------------------------------
TEST_F(SpatialPartitionTestFixture, Grid_ClampsOutOfBoundsPositions)
{
    auto& grid = system->GetGrid();

    // Position far beyond the grid boundaries
    Vector3 extremePos{ 10000.0f, 10000.0f, 10000.0f };
    auto coord = grid.GetCellCoordFromPosition(extremePos);

    // Should be clamped to the max index (size - 1)
    int maxIdx = grid.GetGridSize() - 1;
    EXPECT_EQ(coord.x, maxIdx);
    EXPECT_EQ(coord.y, maxIdx);
    EXPECT_EQ(coord.z, maxIdx);
}

// ---------------------------------------------------------
// 4. Reset Test: Verify grid is cleared correctly every frame
// ---------------------------------------------------------
TEST_F(SpatialPartitionTestFixture, System_ClearsGridEveryFrame)
{
    Entity e1 = coordinator.CreateEntity();
    coordinator.AddComponent(e1, TransformComponent{ .position = Vector3{ 0.0f, 0.0f, 0.0f } });
    coordinator.AddComponent(e1, SpatialPartitionComponent{});

    // Update twice
    system->Update(coordinator, 0.016f, 0.0);
    system->Update(coordinator, 0.016f, 0.0);

    // After two updates, if Clear() is working, entity count in grid should still be 1
    EXPECT_EQ(system->GetGrid().GetEntities().size(), 1);
}
