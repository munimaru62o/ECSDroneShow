// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/Spatial/SpatialGrid.h"
#include <vector>

class Coordinator;

/**
 * @struct SpatialBoidData
 * @brief A tightly packed structure containing the minimum data (Entity, Position, Velocity) required for Boids calculations.
 *
 * This struct is padded to exactly 32 bytes (a power of two) to ensure that exactly
 * two instances fit perfectly within a standard 64-byte CPU cache line.
 * The `alignas(32)` specifier guarantees that the compiler aligns the memory boundary
 * to prevent instances from crossing cache lines.
 */
struct alignas(32) SpatialBoidData
{
    Entity entity = INVALID_ENTITY; // 4byte
    Vector3 position{};             // 12byte
    Vector3 velocity{};             // 12byte
    float padding = 0.0f;           // 4byte
};

/**
 * @class SpatialBoidCacheSystem
 * @brief A system that reorganizes spatial data into contiguous memory to maximize CPU cache hits during Boids processing.
 *
 * Utilizing the grid offsets calculated by the SpatialPartitionSystem, this system
 * gathers scattered components (Transform, Velocity) and packs them into a localized
 * Array of Structures (AoS). By grouping entities in the same spatial cell sequentially
 * in memory, the BoidsSystem can iterate through neighbors with near-zero cache misses.
 *
 * Signature: [TransformComponent, VelocityComponent, SpatialPartitionComponent]
 */
class SpatialBoidCacheSystem : public System
{
public:
    void Init(Coordinator& coordinator) override;
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

    void SetSpatialGrid(SpatialGrid* grid)
    {
        m_grid = grid;
        if (grid) {
            writeOffsets.resize(grid->GetTotalCells());
        }
    }

    [[nodiscard]] const std::vector<SpatialBoidData>& GetCache() const
    {
        return cache;
    }

private:
    SpatialGrid* m_grid = nullptr;
    std::vector<SpatialBoidData> cache;

    // Temporary buffer to track write cursors for each cell
    std::vector<int> writeOffsets;
};

