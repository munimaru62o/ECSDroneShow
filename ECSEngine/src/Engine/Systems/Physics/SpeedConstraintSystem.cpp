// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/SpeedConstraintSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/PhysicsComponents.h"
#include "Engine/Math/Constants.h"

#include <cmath>

void SpeedConstraintSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& speedLimitArray = coordinator.GetComponentArray<SpeedConstraintComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& speedLimit = speedLimitArray.GetData(entity);
            auto& velocity = velocityArray.GetData(entity);

            const float speedSq = velocity.value.LengthSq();
            const float maxSq = speedLimit.max * speedLimit.max;
            const float minSq = speedLimit.min * speedLimit.min;

            if (speedSq > maxSq) {
                float speed = std::sqrt(speedSq);
                float scale = speedLimit.max / speed;
                velocity.value *= scale;
            } else if (speedSq < minSq && speedSq > MathConstants::ZERO_TOLERANCE) {
                float speed = std::sqrt(speedSq);
                float scale = speedLimit.min / speed;
                velocity.value *= scale;
            }
        }
    });
}
