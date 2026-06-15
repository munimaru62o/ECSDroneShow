// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/AnimationComponents.h"


template<>
struct SequenceTraits<SpinAnimationComponent> : DefaultSequenceTraits
{
    static constexpr bool SyncOnCatchUp = true;
};

template<>
struct SequenceTraits<SpinAnimationRequestComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
    static constexpr bool OneShot = true;
};

template<>
struct SequenceTraits<ColorFadeComponent> : DefaultSequenceTraits
{
    static constexpr bool SyncOnCatchUp = true;
};

template<>
struct SequenceTraits<ColorFadeRequestComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
    static constexpr bool OneShot = true;
};

template<>
struct SequenceTraits<ColorBlinkComponent> : DefaultSequenceTraits
{
    static constexpr bool SyncOnCatchUp = true;
    static constexpr bool DataDriven = true;
};
