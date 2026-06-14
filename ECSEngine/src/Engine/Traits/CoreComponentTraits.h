// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/CoreComponents.h"


template<>
struct ComponentTraits<DirectionComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};

template<>
struct ComponentTraits<TransformComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};

template<>
struct ComponentTraits<TargetComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};

template<>
struct ComponentTraits<VelocityComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};

template<>
struct ComponentTraits<ForceComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};
