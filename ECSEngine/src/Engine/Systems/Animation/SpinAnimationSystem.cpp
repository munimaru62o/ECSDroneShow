// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Animation/SpinAnimationSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"
#include "Engine/Math/Quaternion.h"

void SpinAnimationSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& spinArray = coordinator.GetComponentArray<SpinAnimationComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &transformArray,
        &spinArray,
        &coordinator,
        simulationTime
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];

            auto& transform = transformArray.GetData(entity);
            const auto& spin = spinArray.GetData(entity);
            const float elapsedTime = static_cast<float>(simulationTime - spin.creationTime);

            if (spin.duration <= 0.0f || elapsedTime < spin.duration) {
                float currentAngle = spin.speed * elapsedTime;
                Quaternion spinRot = Quaternion::FromAxisAngle(spin.axis, currentAngle);
                transform.rotation = spin.baseRotation * spinRot;
            } else {
                coordinator.DeferRemoveComponent<SpinAnimationComponent>(entity);
            }
        }
    });
}
