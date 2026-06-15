// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <bitset>
#include <cstdint>

/**
 * @brief Entity identifier type.
 * Represented as a 32-bit unsigned integer to allow for up to 4 billion entities,
 * though practically capped by MAX_ENTITIES.
 */
using Entity = std::uint32_t;

/**
 * @brief Maximum number of entities allowed in the system.
 * Includes an extra slot for scene-specific entities.
 */
constexpr Entity MAX_ENTITIES = 100000 + 1;

/**
 * @brief Sentinel value for an invalid or null entity.
 */
constexpr Entity INVALID_ENTITY = MAX_ENTITIES;

/**
 * @brief System identifier type.
 */
using SystemType = std::uint8_t;

/**
 * @brief Maximum number of unique systems supported by the engine.
 */
constexpr SystemType MAX_SYSTEMS = 128;

/**
 * @brief Component identifier type.
 */
using ComponentType = std::uint8_t;

/**
 * @brief Maximum number of component types an entity can hold.
 * This limit corresponds to the size of the Signature bitset.
 */
constexpr ComponentType MAX_COMPONENTS = 128;

/**
 * @brief A bitset used to represent the set of components an entity possesses.
 * Used for fast filtering and system matching.
 */
using Signature = std::bitset<MAX_COMPONENTS>;
