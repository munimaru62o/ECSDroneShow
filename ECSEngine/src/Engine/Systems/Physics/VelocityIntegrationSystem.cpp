// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/VelocityIntegrationSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"

void VelocityIntegrationSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& force = forceArray.GetData(entity);
            auto& velocity = velocityArray.GetData(entity);

            // Apply acceleration (assuming mass = 1.0) to update velocity: v = v + (a * dt)
            velocity.value += force.value * dt;
        }
    });
}
