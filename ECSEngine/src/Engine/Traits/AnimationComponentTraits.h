// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/AnimationComponents.h"


template<>
struct ComponentTraits<SpinAnimationComponent> : DefaultComponentTraits
{
    static constexpr bool HasCreationTime = true;
};

template<>
struct ComponentTraits<SpinAnimationRequestComponent> : DefaultComponentTraits
{
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<ColorFadeComponent> : DefaultComponentTraits
{
    static constexpr bool HasCreationTime = true;
};

template<>
struct ComponentTraits<ColorFadeRequestComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<ColorBlinkComponent> : DefaultComponentTraits
{
    static constexpr bool HasCreationTime = true;
};
