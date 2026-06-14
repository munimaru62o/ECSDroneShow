// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Game/Components/GameComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/GameJsonParsers.h"


namespace GameComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<DroneComponent>(coordinator, "DroneComponent");
    registry.Register<FormationComponentWireframe>(coordinator, "FormationComponentWireframe");
    registry.Register<FormationComponentPointCloud>(coordinator, "FormationComponentPointCloud");
    registry.Register<FormationRotationComponent>(coordinator, "FormationRotationComponent");
}
}
