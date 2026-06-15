// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Behavior/SteeringSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/BehaviorComponents.h"

void SteeringSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) return;

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& targetArray = coordinator.GetComponentArray<TargetComponent>();
    auto& steeringArray = coordinator.GetComponentArray<SteeringComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];

            const auto& target = targetArray.GetData(entity);
            const auto& transform = transformArray.GetData(entity);
            const auto& steering = steeringArray.GetData(entity);
            auto& velocity = velocityArray.GetData(entity);

            // 1. Calculate the distance (difference) vector to the target
            Vector3 diff = target.value - transform.position;

            // 2. Retrieve spring parameters
            float k = steering.stiffness;
            float c = steering.precalculatedDamping;

            // 3. Calculate acceleration (Force)
            // F = (k * x) - (c * v) 
            // (Spring attraction pulling towards the target MINUS the damping brake based on current velocity)
            Vector3 accel = (diff * k) - (velocity.value * c);

            // 4. Update velocity (Symplectic Euler Integration: Part 1)
            velocity.value += accel * dt;
        }
    });
}
