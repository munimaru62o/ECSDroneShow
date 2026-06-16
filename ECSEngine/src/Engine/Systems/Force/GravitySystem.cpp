// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/GravitySystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"

void GravitySystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& gravityArray = coordinator.GetComponentArray<GravityComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &gravityArray,
        &forceArray
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& gravity = gravityArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            // Accumulate the gravity force
            force.value += gravity.direction * gravity.strength;
        }
    });
}
