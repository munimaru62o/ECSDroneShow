// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

namespace SequenceTrait
{
enum class ApplyTarget
{
    System,
    Scene,
    Count,
};
}

/**
 * @brief Default sequence-related traits.
 *
 * Components inherit these values unless a specialized SequenceTraits<T> overrides them.
 */
struct DefaultSequenceTraits
{
    static constexpr SequenceTrait::ApplyTarget Target = SequenceTrait::ApplyTarget::System;
    static constexpr bool SyncOnCatchUp = false;
    static constexpr bool DataDriven = false;
    static constexpr bool OneShot = false;
};

/**
 * @brief Compile-time traits describing how a component participates
 * in the sequence system.
 *
 * SequenceTraits defines sequence-specific behavior that is separate from the component's core ECS functionality.
 *
 * Examples:
 * - Whether the component should be synchronized during catch-up
 * - Whether the component can be created from data-driven definitions
 * - Which target entity receives the component
 * - Whether the component should be removed after a single application
 *
 * Specialize SequenceTraits<T> for components that interact with the sequence system.
 */
template<typename T>
struct SequenceTraits : DefaultSequenceTraits {};
