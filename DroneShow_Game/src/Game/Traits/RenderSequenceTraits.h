// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/RenderComponents.h"


template<>
struct SequenceTraits<MaterialComponent> : DefaultSequenceTraits
{
    static constexpr bool SyncOnCatchUp = true;
};

template<>
struct SequenceTraits<MaterialRequestComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
    static constexpr bool OneShot = true;
};

template<>
struct SequenceTraits<PrimitiveMeshComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};
