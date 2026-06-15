// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/RenderComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/RenderJsonParsers.h"


namespace RenderComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<MaterialComponent>(coordinator, "MaterialComponent");
    registry.Register<MaterialRequestComponent>(coordinator, "MaterialRequestComponent");
    registry.Register<PrimitiveMeshComponent>(coordinator, "PrimitiveMeshComponent");
}
}
