// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/BoidsSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Spatial/SpatialGrid.h"
#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>


struct BoidsSystem::BoidNeighborhood
{
    Vector3 separation{};
    Vector3 alignment{};
    Vector3 cohesion{};
    int count = 0;
};


void BoidsSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    if (!m_grid) {
        assert(false);
        return;
    }

    if (!m_cacheSystem) {
        assert(false);
        return;
    }

    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    const std::vector<SpatialBoidData>& cache = m_cacheSystem->GetCache();
    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();
    auto& boidsArray = coordinator.GetComponentArray<BoidsComponent>();
    auto& forceArray =  coordinator.GetComponentArray<ForceComponent>();

    ParallelFor(totalEntities, [this, &entities, &transformArray, &velocityArray, &boidsArray, &forceArray, &cache, simulationTime](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(entity, transformArray.GetData(entity), velocityArray.GetData(entity), boidsArray.GetData(entity), forceArray.GetData(entity), cache, simulationTime);
        }
    });
}


void BoidsSystem::ProcessEntity( // NOSONAR (cpp:S107 - ECS architecture requires passing multiple components for entity processing)
    Entity entity,
    const TransformComponent& transform,
    const VelocityComponent& velocity,
    BoidsComponent& boids,
    ForceComponent& force,
    const std::vector<SpatialBoidData>& cache,
    double simulationTime
) const {
    // Optimization: Time-slicing. Only compute expensive neighbor searches at specified intervals.
    if (boids.nextUpdateTime <= simulationTime) {

        auto neighborhood = CollectNeighbors(entity, transform.position, boids.visionRadius, cache);
        if (neighborhood.count <= 0) {
            return;
        }

        // Average the accumulated vectors and calculate the desired steering relative to current state
        float invNeighborCount = 1.0f / static_cast<float>(neighborhood.count);
        neighborhood.separation *= invNeighborCount;
        neighborhood.alignment = (neighborhood.alignment * invNeighborCount) - velocity.value;
        neighborhood.cohesion = (neighborhood.cohesion * invNeighborCount) - transform.position;

        // Multiply each Boid rule by its weight and synthesize the final steering force
        Vector3 steeringForce =
            neighborhood.separation * boids.separationWeight +
            neighborhood.alignment * boids.alignmentWeight +
            neighborhood.cohesion * boids.cohesionWeight;

        // Normalize and clamp only if the force exceeds the maximum limit
        float forceSq = steeringForce.LengthSq();
        float maxForceSq = boids.maxForce * boids.maxForce;
        if (forceSq > maxForceSq) {
            steeringForce = steeringForce.Normalized() * boids.maxForce;
        }

        boids.cachedDirection = steeringForce;
        boids.nextUpdateTime = simulationTime + boids.updateInterval * boids.updateIntervalScale;
    }

    // Apply the cached force every frame
    force.value += boids.cachedDirection;

    // Debug drawing
    if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
        DebugDrawManager::GetInstance().AddLine(
            transform.position,
            transform.position + boids.cachedDirection * Debug::Scale::Force,
            Debug::DrawColor::Force::Boids
        );
    }
}


BoidsSystem::BoidNeighborhood BoidsSystem::CollectNeighbors(Entity entity, const Vector3& position, float visionRadius, const std::vector<SpatialBoidData>& cache) const
{
    BoidNeighborhood neighborhood;

    // Threshold to prevent CPU spikes in extremely dense clusters
    const int MAX_NEIGHBORS = 16;
    bool searchFinished = false;

    // Use squared distance to avoid expensive sqrt operations during comparisons
    float visionRadiusSq = visionRadius * visionRadius;

    // 1. Calculate the vision bounding box (min/max coordinates)
    Vector3 minBounds = position - Vector3{ visionRadius, visionRadius, visionRadius };
    Vector3 maxBounds = position + Vector3{ visionRadius, visionRadius, visionRadius };

    // 2. Convert world coordinates to cell indices (CellCoord)
    auto minCell = m_grid->GetCellCoordFromPosition(minBounds);
    auto maxCell = m_grid->GetCellCoordFromPosition(maxBounds);
    int gridSize = m_grid->GetGridSize();

    // 3. Safely clamp indices to prevent out-of-bounds grid access
    int minX = (std::max)(0, minCell.x);
    int maxX = (std::min)(gridSize - 1, maxCell.x);
    int minY = (std::max)(0, minCell.y);
    int maxY = (std::min)(gridSize - 1, maxCell.y);
    int minZ = (std::max)(0, minCell.z);
    int maxZ = (std::min)(gridSize - 1, maxCell.z);

    auto processCell = [&](int x, int y, int z) {
        int otherCellIndex = m_grid->GetFlatIndex(SpatialGrid::CellCoord{ x, y, z });
        int offset = m_grid->GetCellOffsets()[otherCellIndex];
        int count = m_grid->GetCellCounts()[otherCellIndex];

        for (int j = 0; j < count; ++j) {

            const auto& other = cache[offset + j];
            if (entity == other.entity) {
                continue;
            }

            const auto& otherPosition = other.position;
            const auto& otherVelocity = other.velocity;

            Vector3 delta = position - otherPosition;
            float distSq = delta.LengthSq();

            // Exclude if out of vision range, or if too close (prevents division by zero)
            if (distSq < 0.001f || distSq > visionRadiusSq) {
                continue;
            }

            // 1. Separation: Apply stronger repulsive weights to closer entities
            const float weight = 1.0f / (distSq + 0.0001f);
            neighborhood.separation += delta * weight;

            // 2. Alignment & 3. Cohesion: Accumulate values to be averaged later
            neighborhood.alignment += otherVelocity;
            neighborhood.cohesion += otherPosition;
            neighborhood.count++;

            // Early exit if max neighbors reached
            if (neighborhood.count >= MAX_NEIGHBORS) {
                searchFinished = true;
                break;
            }
        }
    };

    // 4. Efficiently iterate only through the filtered surrounding cells
    for (int z = minZ; z <= maxZ && !searchFinished; ++z) {
        for (int y = minY; y <= maxY && !searchFinished; ++y) {
            for (int x = minX; x <= maxX && !searchFinished; ++x) {
                processCell(x, y, z);
            }
        }
    }

    return neighborhood;
}
