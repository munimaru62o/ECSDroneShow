// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Constants.h"

#include <cstdint>
#include <cmath>

/**
 * @class StatelessRandom
 * @brief A purely deterministic, hash-based pseudo-random number generator (PRNG).
 *
 * Unlike SeededRandom, this class maintains absolutely no internal state.
 * It treats the input 'seed' as raw data (like an Entity ID or a spatial coordinate)
 * and maps it to a seemingly random output using a high-quality hash function.
 * This is essential for procedural generation, parallel processing, and any scenario
 * where reproducible randomness is required directly from an identifier or coordinate.
 */
class StatelessRandom final
{
public:
    StatelessRandom() = delete;

public:
    [[nodiscard]] static uint32_t Hash(uint32_t value)
    {
        value ^= value >> 16;
        value *= MathConstants::MURMUR3_MIX32_C1;
        value ^= value >> 13;
        value *= MathConstants::MURMUR3_MIX32_C2;
        value ^= value >> 16;

        return value;
    }

    [[nodiscard]] static uint32_t Combine(uint32_t seed, uint32_t index)
    {
        return Hash(seed ^ (index * MathConstants::HASH_GOLDEN_RATIO_32));
    }

    [[nodiscard]] static float Value01(uint32_t seed)
    {
        return static_cast<float>(Hash(seed)) * MathConstants::INV_UINT32;
    }

    [[nodiscard]] static float Value01_FromHashed(uint32_t hashedSeed)
    {
        return static_cast<float>(hashedSeed) * MathConstants::INV_UINT32;
    }

    [[nodiscard]] static float Range(uint32_t seed, float min, float max)
    {
        return min + (max - min) * Value01(seed);
    }

    [[nodiscard]] static int Range(uint32_t seed, int min, int max)
    {
        return min + static_cast<int>(Value01(seed) * static_cast<float>(max - min));
    }

    [[nodiscard]] static bool Chance(uint32_t seed, float probability)
    {
        return Value01(seed) < probability;
    }

    [[nodiscard]] static Vector3 OnUnitSphere(uint32_t seed)
    {
        const float z = Range(Combine(seed, 0), -1.0f, 1.0f);
        const float theta =Range(Combine(seed, 1), 0.0f, MathConstants::TWO_PI);
        const float r = std::sqrt(1.0f - z * z);

        return {
            r * std::cos(theta),
            r * std::sin(theta),
            z
        };
    }

    [[nodiscard]] static Vector3 InsideUnitSphere(uint32_t seed)
    {
        float radius = std::cbrt(Value01(seed));
        return OnUnitSphere(seed) * radius;
    }
};
