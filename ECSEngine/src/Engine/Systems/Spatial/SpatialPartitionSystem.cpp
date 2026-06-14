// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Spatial/SpatialPartitionSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/SpatialComponents.h"

void SpatialPartitionSystem::Init(Coordinator& coordinator)
{
    grid.GetEntities().reserve(MAX_ENTITIES);
    writeOffsets.resize(grid.GetTotalCells());
}

void SpatialPartitionSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        grid.GetEntities().clear();
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& spatialPartitionArray = coordinator.GetComponentArray<SpatialPartitionComponent>();

    grid.Clear();

    auto& cellCounts = grid.GetCellCounts();
    auto& cellOffsets = grid.GetCellOffsets();
    auto& gridEntities = grid.GetEntities();
    gridEntities.resize(totalEntities);

    // 1. Counting Phase: Tally the number of entities in each cell
    for (Entity entity : entities) {
        const auto& transform = transformArray.GetData(entity);
        auto& spatialPartition = spatialPartitionArray.GetData(entity);

        auto coord = grid.GetCellCoordFromPosition(transform.position);
        int index = grid.GetFlatIndex(coord);

        // Cache the index and coord for the distribution phase
        spatialPartition.index = index;
        spatialPartition.coord = coord;
        cellCounts[index]++;
    }

    // 2. Prefix Sum Phase: Calculate the starting offset for each cell
    int offset = 0;
    int totalCells = grid.GetTotalCells();
    for (int i = 0; i < totalCells; ++i) {
        cellOffsets[i] = offset;
        offset += cellCounts[i];
    }

    // 3. Distribution Phase: Place entities into the contiguous array based on offsets
    std::copy(cellOffsets.begin(), cellOffsets.end(), writeOffsets.begin());
    for (Entity entity : entities) {
        const auto& spatialPartition = spatialPartitionArray.GetData(entity);
        
        int writeIndex = writeOffsets[spatialPartition.index]++;
        gridEntities[writeIndex] = entity;
    }
}
