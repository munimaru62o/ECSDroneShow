// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Registration/RegisterAllComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Registry/Components/AnimationComponentRegistry.h"
#include "Game/Registry/Components/BehaviorComponentRegistry.h"
#include "Game/Registry/Components/CoreComponentRegistry.h"
#include "Game/Registry/Components/ForceComponentRegistry.h"
#include "Game/Registry/Components/GameComponentRegistry.h"
#include "Game/Registry/Components/PhysicsComponentRegistry.h"
#include "Game/Registry/Components/RenderComponentRegistry.h"
#include "Game/Registry/Components/SceneComponentRegistry.h"
#include "Game/Registry/Components/SpatialComponentRegistry.h"


namespace GameRegistrations
{
void RegisterAllComponents(ComponentRegistry& registry, Coordinator& coordinator)
{
    AnimationComponentRegistry::RegisterAll(registry, coordinator);
    BehaviorComponentRegistry::RegisterAll(registry, coordinator);
    CoreComponentRegistry::RegisterAll(registry, coordinator);
    ForceComponentRegistry::RegisterAll(registry, coordinator);
    GameComponentRegistry::RegisterAll(registry, coordinator);
    PhysicsComponentRegistry::RegisterAll(registry, coordinator);
    RenderComponentRegistry::RegisterAll(registry, coordinator);
    SceneComponentRegistry::RegisterAll(registry, coordinator);
    SpatialComponentRegistry::RegisterAll(registry, coordinator);
}
}


