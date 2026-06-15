// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/SpatialComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/SpatialJsonParsers.h"


namespace SpatialComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<SpatialPartitionComponent>(coordinator, "SpatialPartitionComponent");
}
}
