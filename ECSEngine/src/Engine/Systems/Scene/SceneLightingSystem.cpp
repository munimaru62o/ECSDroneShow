// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Scene/SceneLightingSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/SceneComponents.h"
#include "Engine/Render/RenderManager.h"

#include <cassert>

void SceneLightingSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    // Process only newly added lighting requests (One-shot execution)
    const auto& events = coordinator.GetEvents<SceneLightingComponent>();
    for (auto entity : events.added) {
        assert(entity == coordinator.GetSceneEntity() && "SceneLightingComponent must be assigned exclusively to the SceneEntity.");

        const auto& lighting = coordinator.GetComponent<SceneLightingComponent>(entity);

        // Apply lighting parameters to the rendering backend
        RenderManager::GetInstance().UpdateLight(
            lighting.direction,
            lighting.ambient,
            lighting.lightColor,
            lighting.fillColor
        );

        // Clean up the request component
        coordinator.DeferRemoveComponent<SceneLightingComponent>(entity);
    }
}
