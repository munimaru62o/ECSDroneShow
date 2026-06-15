// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/ForceComponents.h"


template<>
struct SequenceTraits<BoidsComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<JitterMovementComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<LiftComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<AttractionComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<SwirlComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<RandomSwirlAxisRequestComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<GravityComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<BoundaryComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<WanderComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};

template<>
struct SequenceTraits<DartComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};
