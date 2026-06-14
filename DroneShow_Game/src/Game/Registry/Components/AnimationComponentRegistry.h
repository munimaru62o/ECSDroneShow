// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/Components/AnimationComponents.h"
#include "Game/Registry/ComponentRegistry.h"
#include "Game/Parsers/Json/AnimationJsonParsers.h"


namespace AnimationComponentRegistry
{
inline void RegisterAll(ComponentRegistry& registry, Coordinator& coordinator)
{
    registry.Register<SpinAnimationComponent>(coordinator, "SpinAnimationComponent");
    registry.Register<SpinAnimationRequestComponent>(coordinator, "SpinAnimationRequestComponent");
    registry.Register<ColorFadeComponent>(coordinator, "ColorFadeComponent");
    registry.Register<ColorFadeRequestComponent>(coordinator, "ColorFadeRequestComponent");
    registry.Register<ColorBlinkComponent>(coordinator, "ColorBlinkComponent");
}
}
