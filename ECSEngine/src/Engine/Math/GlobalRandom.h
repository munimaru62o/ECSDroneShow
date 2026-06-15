// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Constants.h"
#include "Engine/Math/Vector3.h"

#include <random>
#include <cmath>

/**
 * @class GlobalRandom
 * @brief A global random number utility for non-deterministic use cases.
 *
 * This should not be used for core game simulation or ECS behavioral logic.
 * Instead, it is intended for "reproducibility-independent" randomness,
 * such as debugging, editor features, and visual effects (presentation).
 *
 * For deterministic processes, please use SeededRandom or StatelessRandom.
 */
class GlobalRandom final
{
public:
    GlobalRandom() = delete;

public:
    static void Seed(uint32_t seed)
    {
        GetGenerator().seed(seed);
    }

    [[nodiscard]] static float Value01()
    {
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(GetGenerator());
    }

    [[nodiscard]] static float Range(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetGenerator());
    }

    [[nodiscard]] static int Range(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetGenerator());
    }

    [[nodiscard]] static bool Chance(float probability)
    {
        return Value01() < probability;
    }

    [[nodiscard]] static Vector3 OnUnitSphere()
    {
        const float z = Range(-1.0f, 1.0f);
        const float theta = Range(0.0f, MathConstants::TWO_PI);

        const float r = std::sqrt(1.0f - z * z);

        return {
            r * std::cos(theta),
            r * std::sin(theta),
            z
        };
    }

    [[nodiscard]] static Vector3 InsideUnitSphere()
    {
        const float radius = std::cbrt(Value01());
        return OnUnitSphere() * radius;
    }

private:
    [[nodiscard]] static std::mt19937& GetGenerator()
    {
        static std::mt19937 generator{ std::random_device{}() };
        return generator;
    }
};
