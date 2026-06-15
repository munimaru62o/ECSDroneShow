// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Force/LiftSystem.h"
#include "Engine/Systems/Force/AttractionSystem.h"
#include "Engine/Systems/Force/SwirlSystem.h"
#include "Engine/Systems/Force/GravitySystem.h"
#include "Engine/Systems/Force/BoundarySystem.h"
#include "Engine/Systems/Force/ForceClearSystem.h"
#include "Engine/Systems/Force/JitterMovementSystem.h"
#include "Engine/Systems/Force/BoidsSystem.h"
#include "Engine/Systems/Force/RandomSwirlAxisRequestSystem.h"
#include "Engine/Systems/Force/WanderSystem.h"
#include "Engine/Systems/Force/DartSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"


namespace ForceSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<RandomSwirlAxisRequestSystem,
        RandomSwirlAxisRequestComponent, SwirlComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::ApplyRequest);

    SystemRegistry::RegisterSystemWithComponents<ForceClearSystem, ForceComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceClear);

    SystemRegistry::RegisterSystemWithComponents<BoidsSystem,
        TransformComponent, VelocityComponent, BoidsComponent, ForceComponent, SpatialPartitionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<GravitySystem,
        ForceComponent, GravityComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<BoundarySystem,
        TransformComponent, ForceComponent, BoundaryComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<LiftSystem,
        TransformComponent, ForceComponent, LiftComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<AttractionSystem,
        TransformComponent, ForceComponent, AttractionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<SwirlSystem,
        TransformComponent, ForceComponent, SwirlComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<JitterMovementSystem,
        ForceComponent, JitterMovementComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<WanderSystem,
        ForceComponent, WanderComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);

    SystemRegistry::RegisterSystemWithComponents<DartSystem,
        ForceComponent, DartComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::ForceGenerator);
}
}
