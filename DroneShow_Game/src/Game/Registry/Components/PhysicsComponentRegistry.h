// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/PhysicsComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/PhysicsJsonParsers.h"


namespace PhysicsComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<RandomVelocityRequestComponent>(coordinator, "RandomVelocityRequestComponent");
    registry.Register<SpeedConstraintComponent>(coordinator, "SpeedConstraintComponent");
    registry.Register<PlaneConstraintComponent>(coordinator, "PlaneConstraintComponent");
    registry.Register<DampingComponent>(coordinator, "DampingComponent");
    registry.Register<VelocityRotationComponent>(coordinator, "VelocityRotationComponent");
}
}
