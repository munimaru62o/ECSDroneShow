// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

#include <vector>

class Coordinator;
class SpatialGrid;
class SpatialBoidCacheSystem;
struct SpatialBoidData;
struct TransformComponent;
struct VelocityComponent;
struct BoidsComponent;
struct ForceComponent;
struct Vector3;

/**
 * @class BoidsSystem
 * @brief An autonomous flocking simulation system based on Craig Reynolds' Boids algorithm.
 *
 * For each entity, this system calculates and synthesizes three fundamental steering forces:
 * 1. Separation: Steers the entity to avoid crowding local flockmates.
 * 2. Alignment: Steers the entity toward the average heading of local flockmates.
 * 3. Cohesion: Steers the entity toward the average position (center of mass) of local flockmates.
 *
 * [Optimization Strategy]
 * To avoid O(N^2) brute-force comparisons, this system leverages a SpatialGrid and a
 * SpatialBoidCacheSystem. It employs a data-oriented design to quickly query only neighboring
 * entities within adjacent cells (3x3x3), reducing the time complexity to O(N).
 *
 * Signature: [TransformComponent, VelocityComponent, BoidsComponent, ForceComponent]
 */
class BoidsSystem : public System
{
private:
    struct BoidNeighborhood;
    SpatialGrid* m_grid = nullptr;
    SpatialBoidCacheSystem* m_cacheSystem = nullptr;

public:
    void SetSpatialGrid(SpatialGrid* grid)
    {
        m_grid = grid;
    }

    void SetCacheSystem(SpatialBoidCacheSystem* cacheSystem)
    {
        m_cacheSystem = cacheSystem;
    }

    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, const TransformComponent& transform, const VelocityComponent& velocity, BoidsComponent& boids, ForceComponent& force, const std::vector<SpatialBoidData>& cache, double simulationTime) const;
    BoidNeighborhood CollectNeighbors(Entity entity, const Vector3& position, float visionRadius, const std::vector<SpatialBoidData>& cache) const;
};

