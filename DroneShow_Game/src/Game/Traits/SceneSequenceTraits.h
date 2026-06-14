// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Traits/SequenceTraitsBase.h"
#include "Engine/Components/SceneComponents.h"


template<>
struct SequenceTraits<SceneLightingComponent> : DefaultSequenceTraits
{
    static constexpr SequenceTrait::ApplyTarget Target = SequenceTrait::ApplyTarget::Scene;
    static constexpr bool DataDriven = true;
};
