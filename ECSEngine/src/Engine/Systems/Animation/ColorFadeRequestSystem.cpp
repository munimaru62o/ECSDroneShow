// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Animation/ColorFadeRequestSystem.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"

#include <algorithm>
#include <utility>

void ColorFadeRequestSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    auto& fadeRequestArray = coordinator.GetComponentArray<ColorFadeRequestComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    // Observe newly added requests and capture the entity's current color
    const auto& events = coordinator.GetEvents<ColorFadeRequestComponent>();
    for (auto entity : events.added) {
        const auto& fadeRequest = fadeRequestArray.GetData(entity);
        const Color& currentColor = materialArray.GetData(entity).color;

        ColorFadeComponent fadeComponent = {
            .startColor = currentColor,
            .targetColor = fadeRequest.targetColor,
            .duration = fadeRequest.duration,
            .creationTime = simulationTime
        };

        coordinator.DeferAssignComponent<ColorFadeComponent>(entity, std::move(fadeComponent));
        coordinator.DeferRemoveComponent<ColorFadeRequestComponent>(entity);
    }
}
