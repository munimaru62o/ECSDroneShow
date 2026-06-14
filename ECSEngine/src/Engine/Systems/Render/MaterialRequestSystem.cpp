// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Render/MaterialRequestSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/RenderComponents.h"

void MaterialRequestSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    auto& requestArray = coordinator.GetComponentArray<MaterialRequestComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    // Process only newly added requests (One-shot execution)
    const auto& events = coordinator.GetEvents<MaterialRequestComponent>();
    for (auto entity : events.added) {
        const auto& request = requestArray.GetData(entity);
        auto& material = materialArray.GetData(entity);

        // Apply partial updates using std::optional
        if (request.color) {
            material.color = *request.color;
        }
        if (request.shaderType) {
            material.shaderType = *request.shaderType;
        }

        // Clean up the request component
        coordinator.DeferRemoveComponent<MaterialRequestComponent>(entity);
    }
}
