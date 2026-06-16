// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Behavior/OrbitSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/BehaviorComponents.h"

#include <cmath>
#include <cassert>

void OrbitSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& targetArray = coordinator.GetComponentArray<TargetComponent>();
    auto& orbitArray = coordinator.GetComponentArray<OrbitComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &targetArray,
        &orbitArray,
        &coordinator,
        simulationTime
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& target = targetArray.GetData(entity);
            const auto& orbit = orbitArray.GetData(entity);
            const float elapsedTime = static_cast<float>(simulationTime - orbit.creationTime);

            bool isInfinite = (orbit.duration <= 0.0f);

            if (isInfinite || elapsedTime < orbit.duration) {
                float currentAngle = orbit.speed * elapsedTime;
                float cosA = std::cos(currentAngle);
                float sinA = std::sin(currentAngle);

                assert(orbit.axis.IsNormalized() && "orbit.axis is not normalized.");

                // The target.value is refreshed every frame by an external system/logic.
                // We treat this dynamically updated coordinate as the base, applying the 
                // absolute rotation offset to enable orbiting around a moving target.
                const Vector3& v = target.value;
                const Vector3& k = orbit.axis;

                // Rodrigues' rotation formula
                 // v_rot = v * cos(θ) + (k × v) * sin(θ) + k * (k · v) * (1 - cos(θ))
                Vector3 crossKV = k.Cross(v);
                float dotKV = k.Dot(v);
                Vector3 rotatedTarget = v * cosA + crossKV * sinA + k * (dotKV * (1.0f - cosA));

                target.value = rotatedTarget;
            } else {
                coordinator.DeferRemoveComponent<OrbitComponent>(entity);
            }
        }
    });
}
