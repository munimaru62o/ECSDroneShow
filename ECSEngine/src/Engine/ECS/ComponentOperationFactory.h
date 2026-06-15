// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/Traits/ComponentTraits.h"

#include <string>
#include <functional>

/**
 * @brief Context shared by component operations during execution.
 *
 * Provides runtime information required when assigning or removing components.
 * This context allows component initialization logic to access data such as sequence start times and deterministic seed generation parameters.
 */
struct ComponentOperationContext
{
    double creationTime = 0.0f;
};

/**
 * @brief Encapsulates a component assignment operation.
 *
 * Stores a callable object that applies a component to an entity.
 * The operation may perform trait-driven initialization such as start time assignment,
 * deterministic seed generation, and custom component initialization before the component is registered.
 */
struct ComponentAssignOperation
{
    std::function<void(Coordinator&, Entity, const ComponentOperationContext&)> execute;
};

/**
 * @brief Encapsulates a component removal operation.
 *
 * Stores a callable object that removes a component from an entity if it exists.
 */
struct ComponentRemoveOperation
{
    std::function<void(Coordinator&, Entity, const ComponentOperationContext&)> execute;
};


/**
 * @brief Factory for creating trait-aware component operations.
 *
 * Generates component assignment and removal operations based on compile-time component traits.
 * The factory automatically applies optional behaviors such as:
 *
 * - Start time propagation
 * - Deterministic seed generation
 * - Component-specific initialization
 *
 * This allows higher-level systems to construct component operations without requiring knowledge of individual component implementation details.
 */
class ComponentOperationFactory final
{
public:
    ComponentOperationFactory() = delete;

public:
    template<typename T>
    [[nodiscard]] static ComponentAssignOperation CreateAssignOperation(T data)
    {
        return ComponentAssignOperation{
            .execute = [data](Coordinator& coordinator, Entity entity, const ComponentOperationContext& ctx) {
            T component = data;

            if constexpr (ComponentTraits<T>::HasCreationTime) {
                component.creationTime = ctx.creationTime;
            }

            if constexpr (ComponentTraits<T>::SeedPolicy != ComponentTrait::SeedPolicy::None) {
                component.seed = GenerateSeed(ComponentTraits<T>::SeedPolicy, entity, ctx);
            }

            if constexpr (ComponentTraits<T>::HasInitializer) {
                component.Initialize(entity);
            }
            coordinator.AssignComponent<T>(entity, component);
        } };
    }

    template<typename T>
    [[nodiscard]] static ComponentRemoveOperation CreateRemoveOperation()
    {
        return ComponentRemoveOperation {
            .execute = [](Coordinator& coordinator, Entity entity,  const ComponentOperationContext& ctx) {
            coordinator.RemoveComponentIfExists<T>(entity);
        } };
    }

private:
    [[nodiscard]] static uint32_t HashCombine(uint32_t a, uint32_t b);
    [[nodiscard]] static uint32_t GenerateSeed(ComponentTrait::SeedPolicy policy, Entity entity, const ComponentOperationContext& ctx);
};
