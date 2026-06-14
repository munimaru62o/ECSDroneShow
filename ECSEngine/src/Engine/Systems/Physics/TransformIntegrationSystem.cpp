// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/TransformIntegrationSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"

void TransformIntegrationSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& transform = transformArray.GetData(entity);
            auto& velocity = velocityArray.GetData(entity);

            // Perform Euler integration to update the final position
            transform.position += velocity.value * dt;
        }
    });
}
