// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <numbers>
#include <cstdint>


namespace MathConstants
{
constexpr float ZERO_TOLERANCE = 0.00001f;
constexpr float INV_UINT32 = 1.0f / 4294967296.0f;

constexpr float PI = std::numbers::pi_v<float>;
constexpr float TWO_PI = PI * 2.0f;
constexpr float GOLDEN_ANGLE = PI * (4.0f - 2.0f * std::numbers::phi_v<float>);

/**
 * @brief 32-bit Golden Ratio Constant.
 * Used for bit dispersion (avalanche effect) during HashCombine operations and random seed generation.
 *
 * value = floor(2^32 / φ)
 * = 2654435769
 * = 0x9e3779b9
 */
constexpr uint32_t HASH_GOLDEN_RATIO_32 = 0x9e3779b9u;
constexpr uint32_t MURMUR3_MIX32_C1 = 0x85EBCA6Bu;
constexpr uint32_t MURMUR3_MIX32_C2 = 0xC2B2AE35u;
}
