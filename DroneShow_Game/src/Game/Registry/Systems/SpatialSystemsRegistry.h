// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Spatial/SpatialPartitionSystem.h"
#include "Engine/Systems/Spatial/SpatialBoidCacheSystem.h"
#include "Engine/Components/SpatialComponents.h"


namespace SpatialSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<SpatialPartitionSystem,
        TransformComponent, SpatialPartitionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::SpatialPartition);

    SystemRegistry::RegisterSystemWithComponents<SpatialBoidCacheSystem,
        TransformComponent, VelocityComponent, SpatialPartitionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::SpatialCache);
}
}
