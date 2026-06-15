// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Game/Components/GameComponents.h"


template<>
struct SequenceTraits<FormationComponentWireframe> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<FormationComponentPointCloud> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<FormationRotationComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<DroneComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};
