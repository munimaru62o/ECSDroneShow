// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/BehaviorComponents.h"


template<>
struct ComponentTraits<OrbitComponent> : DefaultComponentTraits
{
    static constexpr bool HasCreationTime = true;
};

template<>
struct ComponentTraits<SteeringComponent> : DefaultComponentTraits
{
};

