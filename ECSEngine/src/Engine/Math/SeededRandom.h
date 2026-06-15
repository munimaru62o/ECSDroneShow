// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Constants.h"

#include <cstdint>
#include <cmath>

/**
 * @class SeededRandom
 * @brief A fast, deterministic pseudo-random number generator (PRNG) designed for ECS and procedural generation.
 *
 * Unlike standard library random generators that maintain internal global state,
 * this utility requires the state to be explicitly passed in. This guarantees
 * reproducible sequences (determinism) and ensures thread-safe, lock-free
 * random number generation across parallel systems.
 * * The underlying algorithm uses a SplitMix32/MurmurHash3-style avalanche function
 * to produce high-quality bit dispersion with extreme performance.
 */
class SeededRandom final
{
public:
    SeededRandom() = delete;

public:
    [[nodiscard]] static uint32_t Next(uint32_t& state)
    {
        state += MathConstants::HASH_GOLDEN_RATIO_32;

        uint32_t z = state;

        z ^= z >> 16;
        z *= MathConstants::MURMUR3_MIX32_C1;
        z ^= z >> 13;
        z *= MathConstants::MURMUR3_MIX32_C2;
        z ^= z >> 16;

        return z;
    }

    [[nodiscard]] static float Value01(uint32_t& state)
    {
        return static_cast<float>(Next(state)) * MathConstants::INV_UINT32;
    }

    [[nodiscard]] static float Range(uint32_t& state, float min, float max)
    {
        return min + (max - min) * Value01(state);
    }

    [[nodiscard]] static int Range(uint32_t& state, int min, int max)
    {
        if (min >= max) {
            return min;
        }
        return min + static_cast<int>(Next(state) % static_cast<uint32_t>(max - min));
    }

    [[nodiscard]] static bool Chance(uint32_t& state, float probability)
    {
        return Value01(state) < probability;
    }

    [[nodiscard]] static Vector3 OnUnitSphere(uint32_t& state)
    {
        const float z = Range(state, -1.0f, 1.0f);
        const float theta = Range(state, 0.0f, MathConstants::TWO_PI);
        const float r = std::sqrt(1.0f - z * z);

        return {
            r * std::cos(theta),
            r * std::sin(theta),
            z
        };
    }

    [[nodiscard]] static Vector3 InsideUnitSphere(uint32_t& state)
    {
        const float radius = std::cbrt(Value01(state));
        return OnUnitSphere(state) * radius;
    }
};
