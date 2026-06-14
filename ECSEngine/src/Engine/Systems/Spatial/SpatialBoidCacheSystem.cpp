// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/SpatialComponents.h"


void SpatialBoidCacheSystem::Init(Coordinator& coordinator)
{
    cache.reserve(MAX_ENTITIES);
}

void SpatialBoidCacheSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    if (!m_grid) {
        return;
    }

    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();
    auto& spatialPartitionArray = coordinator.GetComponentArray<SpatialPartitionComponent>();

    cache.resize(totalEntities);
    auto& cellOffsets = m_grid->GetCellOffsets();

    // Copy the base offsets calculated by SpatialPartitionSystem
    std::copy(cellOffsets.begin(), cellOffsets.end(), writeOffsets.begin());
    for (Entity entity : entities) {
        const auto& transform = transformArray.GetData(entity);
        const auto& velocity = velocityArray.GetData(entity);
        const auto& spatialPartition = spatialPartitionArray.GetData(entity);
        
        int writeIndex = writeOffsets[spatialPartition.index]++;
        cache[writeIndex] = SpatialBoidData{
            .entity = entity,
            .position = transform.position,
            .velocity = velocity.value,
            .padding = 0.0f
        };
    }
}
