// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/BehaviorComponents.h"


template<>
struct SequenceTraits<OrbitComponent> : DefaultSequenceTraits
{
    static constexpr bool SyncOnCatchUp = true;
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<SteeringComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

