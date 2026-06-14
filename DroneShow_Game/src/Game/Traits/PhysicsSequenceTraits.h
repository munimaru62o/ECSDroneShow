// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/PhysicsComponents.h"


template<>
struct SequenceTraits<RandomVelocityRequestComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
    static constexpr bool OneShot = true;
};

template<>
struct SequenceTraits<SpeedConstraintComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<PlaneConstraintComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<DampingComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<VelocityRotationComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};
