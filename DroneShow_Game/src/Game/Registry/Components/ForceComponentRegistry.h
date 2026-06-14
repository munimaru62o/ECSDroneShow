// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/ForceComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/ForceJsonParsers.h"


namespace ForceComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<BoidsComponent>(coordinator, "BoidsComponent");
    registry.Register<JitterMovementComponent>(coordinator, "JitterMovementComponent");
    registry.Register<LiftComponent>(coordinator, "LiftComponent");
    registry.Register<AttractionComponent>(coordinator, "AttractionComponent");
    registry.Register<SwirlComponent>(coordinator, "SwirlComponent");
    registry.Register<RandomSwirlAxisRequestComponent>(coordinator, "RandomSwirlAxisRequestComponent");
    registry.Register<GravityComponent>(coordinator, "GravityComponent");
    registry.Register<BoundaryComponent>(coordinator, "BoundaryComponent");
    registry.Register<WanderComponent>(coordinator, "WanderComponent");
    registry.Register<DartComponent>(coordinator, "DartComponent");
}
}
