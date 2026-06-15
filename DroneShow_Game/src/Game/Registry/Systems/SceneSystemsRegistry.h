// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Scene/SceneLightingSystem.h"
#include "Engine/Components/SceneComponents.h"


namespace SceneSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<SceneLightingSystem,
        SceneLightingComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::PostUpdate);
}
}
