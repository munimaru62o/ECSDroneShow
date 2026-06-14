// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Behavior/SteeringSystem.h"
#include "Engine/Systems/Behavior/OrbitSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/BehaviorComponents.h"


namespace BehaviorSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<OrbitSystem,
        TargetComponent, OrbitComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::TargetModifier);

    SystemRegistry::RegisterSystemWithComponents<SteeringSystem,
        TargetComponent, TransformComponent, VelocityComponent, SteeringComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Steering);
}
}
