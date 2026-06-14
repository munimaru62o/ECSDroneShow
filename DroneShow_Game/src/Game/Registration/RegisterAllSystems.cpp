// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/ECS/Coordinator.h"

#include "Game/Registry/Systems/AnimationSystemsRegistry.h"
#include "Game/Registry/Systems/BehaviorSystemsRegistry.h"
#include "Game/Registry/Systems/ForceSystemsRegistry.h"
#include "Game/Registry/Systems/GameSystemRegistry.h"
#include "Game/Registry/Systems/PhysicsSystemsRegistry.h"
#include "Game/Registry/Systems/RenderSystemRegistry.h"
#include "Game/Registry/Systems/SceneSystemsRegistry.h"
#include "Game/Registry/Systems/SpatialSystemsRegistry.h"


namespace GameRegistrations
{
void RegisterAllSystems(Coordinator& coordinator)
{
    AnimationSystemRegistry::RegisterAll(coordinator);
    BehaviorSystemRegistry::RegisterAll(coordinator);
    ForceSystemRegistry::RegisterAll(coordinator);
    GameSystemRegistry::RegisterAll(coordinator);
    PhysicsSystemRegistry::RegisterAll(coordinator);
    RenderSystemRegistry::RegisterAll(coordinator);
    SceneSystemRegistry::RegisterAll(coordinator);
    SpatialSystemRegistry::RegisterAll(coordinator);
}
}

