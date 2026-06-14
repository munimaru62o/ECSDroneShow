// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Game/Systems/TimelineDirectorSystem.h"
#include "Game/Systems/FormationSystemWireframe.h"
#include "Game/Systems/FormationSystemPointCloud.h"
#include "Engine/Systems/Physics/DirectionRotationSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Game/Components/GameComponents.h"


namespace GameSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<TimelineDirectorSystem,
        DroneComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::Update);

    SystemRegistry::RegisterSystemWithComponents<FormationSystemWireframe,
        DroneComponent, TargetComponent, DirectionComponent, FormationComponentWireframe>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::TargetGenerator);

    SystemRegistry::RegisterSystemWithComponents<FormationSystemPointCloud,
        DroneComponent, TargetComponent, FormationComponentPointCloud>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::TargetGenerator);

    SystemRegistry::RegisterSystemWithComponents<DirectionRotationSystem<FormationRotationComponent>,
        FormationRotationComponent, TransformComponent, DirectionComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Rotation);
}
}
