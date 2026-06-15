// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/BehaviorComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/BehaviorJsonParsers.h"


namespace BehaviorComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<OrbitComponent>(coordinator, "OrbitComponent");
    registry.Register<SteeringComponent>(coordinator, "SteeringComponent");
}
}
