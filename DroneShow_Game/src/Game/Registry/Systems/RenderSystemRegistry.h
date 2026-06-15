// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Render/PrimitiveMeshRenderSystem.h"
#include "Engine/Systems/Render/MaterialRequestSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/RenderComponents.h"


namespace RenderSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<MaterialRequestSystem,
        MaterialRequestComponent, MaterialComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::ApplyRequest);

    SystemRegistry::RegisterSystemWithComponents<PrimitiveMeshRenderSystem,
        TransformComponent, PrimitiveMeshComponent, MaterialComponent>
        (coordinator, SystemPhase::Render, (int)RenderOrder::Opaque);
}
}
