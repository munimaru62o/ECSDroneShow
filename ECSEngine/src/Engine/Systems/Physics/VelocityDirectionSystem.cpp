// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/VelocityDirectionSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/PhysicsComponents.h"
#include "Engine/Math/Constants.h"

void VelocityDirectionSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    if (entities.empty()) {
        return;
    }

    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();
    auto& directionArray = coordinator.GetComponentArray<DirectionComponent>();

    ParallelFor(static_cast<int>(entities.size()), [
        &entities,
        &velocityArray,
        &directionArray
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& velocity = velocityArray.GetData(entity);
            auto& direction = directionArray.GetData(entity);

            // Ignore infinitesimally small velocities to prevent jitter and zero-division
            if (velocity.value.LengthSq() < MathConstants::ZERO_TOLERANCE) {
                continue;
            }

            direction.value = velocity.value.Normalized();
        }
    });
}
