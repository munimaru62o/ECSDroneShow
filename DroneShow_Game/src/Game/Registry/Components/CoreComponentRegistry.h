// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/CoreComponents.h"
#include "Game/Registry/ComponentRegistry.h"


namespace CoreComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<TransformComponent>(coordinator, "TransformComponent");
    registry.Register<TargetComponent>(coordinator, "TargetComponent");
    registry.Register<VelocityComponent>(coordinator, "VelocityComponent");
    registry.Register<ForceComponent>(coordinator, "ForceComponent");
    registry.Register<DirectionComponent>(coordinator, "DirectionComponent");
}
}
