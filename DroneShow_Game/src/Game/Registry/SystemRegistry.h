// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ECSTypes.h"

/**
 * @brief Utility helpers for registering ECS systems.
 *
 * Provides convenience functions that automatically construct system signatures from component type lists and register
 * systems with the Coordinator.
 *
 * This removes the need to manually build signatures when
 * registering systems.
 */
namespace SystemRegistry
{
/**
 * @brief Registers a system using the specified component types to construct its entity signature.
 *
 * The resulting signature automatically includes all component types listed in TComponents...,
 * allowing the system to process only entities that contain the required components.
 *
 * @tparam TSystem      System type to register.
 * @tparam TComponents  Component types required by the system.
 *
 * @param coordinator ECS coordinator.
 * @param phase        Execution phase.
 * @param order        Execution order within the phase.
 *
 * @return Registered system instance.
 */
template<typename TSystem, typename... TComponents>
inline TSystem* RegisterSystemWithComponents(Coordinator& coordinator, SystemPhase phase, int order)
{
    Signature signature;
    (signature.set(coordinator.GetComponentType<TComponents>()), ...);
    return coordinator.RegisterSystem<TSystem>(signature, phase, order);
}
}
