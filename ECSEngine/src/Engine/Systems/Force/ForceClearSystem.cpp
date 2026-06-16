// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/ForceClearSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"

void ForceClearSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();

    ParallelFor(totalEntities, [this, &entities, &forceArray](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(forceArray.GetData(entity));
        }
    });
}


void ForceClearSystem::ProcessEntity(ForceComponent& force)
{
    force.value = {};
}
