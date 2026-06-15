// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/SceneComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/SceneJsonParsers.h"


namespace SceneComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<SceneLightingComponent>(coordinator, "SceneLightingComponent");
}
}

