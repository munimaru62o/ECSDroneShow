// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Physics/VelocityIntegrationSystem.h"
#include "Engine/Systems/Physics/RandomVelocityRequestSystem.h"
#include "Engine/Systems/Physics/TransformIntegrationSystem.h"
#include "Engine/Systems/Physics/SpeedConstraintSystem.h"
#include "Engine/Systems/Physics/PlaneConstraintSystem.h"
#include "Engine/Systems/Physics/DampingSystem.h"
#include "Engine/Systems/Physics/DirectionRotationSystem.h"
#include "Engine/Systems/Physics/VelocityDirectionSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/PhysicsComponents.h"


namespace PhysicsSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<RandomVelocityRequestSystem,
        RandomVelocityRequestComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::ApplyRequest);

    SystemRegistry::RegisterSystemWithComponents<DirectionRotationSystem<VelocityRotationComponent>,
        VelocityRotationComponent, TransformComponent, DirectionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Rotation);

    SystemRegistry::RegisterSystemWithComponents<VelocityDirectionSystem,
        VelocityComponent, DirectionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::DirectionGenerator);
    
    SystemRegistry::RegisterSystemWithComponents<VelocityIntegrationSystem,
        ForceComponent, VelocityComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::VelocityGenerator);

    SystemRegistry::RegisterSystemWithComponents<DampingSystem,
        VelocityComponent, DampingComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::VelocityDamping);

    SystemRegistry::RegisterSystemWithComponents<SpeedConstraintSystem,
        SpeedConstraintComponent, VelocityComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Constraint);

    SystemRegistry::RegisterSystemWithComponents<PlaneConstraintSystem,
        TransformComponent, VelocityComponent, PlaneConstraintComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Constraint);

    SystemRegistry::RegisterSystemWithComponents<TransformIntegrationSystem,
        TransformComponent, VelocityComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::TransformIntegration);
}
}
