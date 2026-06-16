// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/DampingSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/PhysicsComponents.h"

#include <cassert>

void DampingSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();
    auto& dampingArray = coordinator.GetComponentArray<DampingComponent>();

    ParallelFor(totalEntities, [this, &entities, &velocityArray, &dampingArray](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(velocityArray.GetData(entity), dampingArray.GetData(entity));
        }
    });
}


void DampingSystem::ProcessEntity(VelocityComponent& velocity, const DampingComponent& damping) const
{
    assert(damping.factor >= 0.0f && "damping.factor is less than 0.0");
    assert(damping.factor <= 1.0f && "damping.factor is larger than 1.0");

    velocity.value *= damping.factor;
}
