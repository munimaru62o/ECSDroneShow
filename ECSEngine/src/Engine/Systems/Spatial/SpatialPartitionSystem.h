// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/Spatial/SpatialGrid.h"
#include <vector>

class Coordinator;

/**
 * @class SpatialPartitionSystem
 * @brief A foundational pre-simulation system that maps entity coordinates into a uniform grid to accelerate spatial queries.
 *
 * Executed every frame, this system utilizes a Counting Sort (Prefix Sum) algorithm to
 * seamlessly rearrange all entities into a flat, 1D array (`grid.entities`), grouped
 * perfectly by their cell indices.
 *
 * By strictly avoiding dynamic memory allocation (heap allocations) during runtime,
 * this approach guarantees continuous memory layout and maximum CPU cache coherency.
 * It serves as the high-performance backbone for any system requiring neighborhood
 * searches, such as Boids flocking algorithms or broad-phase collision detection.
 *
 * Signature: [TransformComponent, SpatialPartitionComponent]
 */
class SpatialPartitionSystem : public System
{
public:
    void Init(Coordinator& coordinator) override;
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

    SpatialGrid& GetGrid()
    {
        return grid;
    }

private:
    SpatialGrid grid;

    // Temporary buffer to track write cursors for each cell during entity distribution
    std::vector<int> writeOffsets;
};

