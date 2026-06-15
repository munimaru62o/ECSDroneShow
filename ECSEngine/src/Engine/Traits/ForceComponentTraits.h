// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Traits/ComponentTraitsBase.h"
#include "Engine/Components/ForceComponents.h"


template<>
struct ComponentTraits<BoidsComponent> : DefaultComponentTraits
{
    static constexpr bool HasInitializer = true;
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<JitterMovementComponent> : DefaultComponentTraits
{
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<LiftComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<AttractionComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<SwirlComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<RandomSwirlAxisRequestComponent> : DefaultComponentTraits
{
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<GravityComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<BoundaryComponent> : DefaultComponentTraits
{
};

template<>
struct ComponentTraits<WanderComponent> : DefaultComponentTraits
{
    static constexpr bool HasInitializer = true;
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};

template<>
struct ComponentTraits<DartComponent> : DefaultComponentTraits
{
    static constexpr bool HasInitializer = true;
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::Entity;
};
