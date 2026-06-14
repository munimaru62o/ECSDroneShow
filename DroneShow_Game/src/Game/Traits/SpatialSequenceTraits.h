// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/SpatialComponents.h"


template<>
struct SequenceTraits<SpatialPartitionComponent> : DefaultSequenceTraits
{
    static constexpr bool DataDriven = true;
};
