// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/PhysicsComponents.h"


template<>
struct ComponentTraits<RandomVelocityRequestComponent> : DefaultComponentTraits
{
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<SpeedConstraintComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<PlaneConstraintComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<DampingComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<VelocityRotationComponent> : DefaultComponentTraits
{
};
