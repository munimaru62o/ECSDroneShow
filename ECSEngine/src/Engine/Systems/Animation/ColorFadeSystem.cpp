// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Animation/ColorFadeSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"

#include <algorithm>

void ColorFadeSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& fadeArray = coordinator.GetComponentArray<ColorFadeComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &fadeArray,
        &materialArray,
        &coordinator,
        simulationTime
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& material = materialArray.GetData(entity);
            const auto& fade = fadeArray.GetData(entity);
            float elapsedTime = static_cast<float>(simulationTime - fade.creationTime);

            if (fade.duration > 0.0f && elapsedTime < fade.duration) {
                const float t = (std::clamp)(elapsedTime / fade.duration, 0.0f, 1.0f);
                material.color = Color::Lerp(fade.startColor, fade.targetColor, t);
            } else {
                material.color = fade.targetColor;
                coordinator.DeferRemoveComponent<ColorFadeComponent>(entity);
            }
        }
    });
}
