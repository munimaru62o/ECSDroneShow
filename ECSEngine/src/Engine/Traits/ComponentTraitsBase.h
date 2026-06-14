// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

namespace ComponentTrait
{
enum class SeedPolicy
{
    None,
    Entity,
    EntityAndCreationTime,
    Random,
};
}

/**
 * @brief Default ECS traits applied to all components.
 *
 * Components inherit these values unless a specialized ComponentTraits<T> overrides them.
 */
struct DefaultComponentTraits
{
    static constexpr ComponentTrait::SeedPolicy SeedPolicy = ComponentTrait::SeedPolicy::None;
    static constexpr bool CoreComponent = false;
    static constexpr bool HasCreationTime = false;
    static constexpr bool HasInitializer = false;
};

/**
 * @brief Compile-time traits describing ECS-level component behavior.
 *
 * ComponentTraits defines properties that are intrinsic to a component type and independent of any gameplay system.
 *
 * Examples:
 * - Whether the component is automatically attached to all entities
 * - How deterministic random seeds are generated
 * - Whether creation-time metadata is required
 * - Whether the component provides a custom initializer
 *
 * Specialize ComponentTraits<T> for individual component types to override the default behavior.
 */
template<typename T>
struct ComponentTraits : DefaultComponentTraits {};
