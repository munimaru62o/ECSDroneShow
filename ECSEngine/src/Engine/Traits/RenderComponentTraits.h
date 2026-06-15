// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/RenderComponents.h"


template<>
struct ComponentTraits<MaterialComponent> : DefaultComponentTraits
{
    static constexpr bool CoreComponent = true;
};

template<>
struct ComponentTraits<MaterialRequestComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<PrimitiveMeshComponent> : DefaultComponentTraits
{
};
