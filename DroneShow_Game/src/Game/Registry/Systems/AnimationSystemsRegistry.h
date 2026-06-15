// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Registry/SystemRegistry.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/Coordinator.h"

#include "Engine/Systems/Animation/ColorBlinkSystem.h"
#include "Engine/Systems/Animation/ColorFadeSystem.h"
#include "Engine/Systems/Animation/ColorFadeRequestSystem.h"
#include "Engine/Systems/Animation/SpinAnimationSystem.h"
#include "Engine/Systems/Animation/SpinAnimationRequestSystem.h"

#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"


namespace AnimationSystemRegistry
{
inline void RegisterAll(Coordinator& coordinator)
{
    SystemRegistry::RegisterSystemWithComponents<ColorFadeRequestSystem,
        ColorFadeRequestComponent, MaterialComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::ApplyRequest);

    SystemRegistry::RegisterSystemWithComponents<SpinAnimationRequestSystem,
        SpinAnimationRequestComponent>
        (coordinator, SystemPhase::Logic, (int)LogicOrder::ApplyRequest);

    SystemRegistry::RegisterSystemWithComponents<SpinAnimationSystem
        , SpinAnimationComponent, TransformComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::Rotation);

    SystemRegistry::RegisterSystemWithComponents<ColorBlinkSystem,
        ColorBlinkComponent, MaterialComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::MaterialModifier);

    SystemRegistry::RegisterSystemWithComponents<ColorFadeSystem,
        ColorFadeComponent, MaterialComponent>
        (coordinator, SystemPhase::Physics, (int)PhysicsOrder::MaterialModifier);
}
}
