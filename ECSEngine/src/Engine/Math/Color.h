// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <algorithm>

/**
 * @struct Color
 * @brief RGBA color represented as normalized floating-point values.
 *
 * Each channel is stored in the range [0.0f, 1.0f].
 *
 * This format is suitable for:
 * - Color interpolation and animation
 * - Shader parameters and GPU data
 * - Internal rendering calculations
 *
 * Colors loaded from external formats such as JSON can be converted from 0-255 integer ranges during deserialization.
 */
struct Color
{
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;

    [[nodiscard]] static Color Lerp(const Color& start, const Color& end, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        return {
            start.r + (end.r - start.r) * t,
            start.g + (end.g - start.g) * t,
            start.b + (end.b - start.b) * t,
            start.a + (end.a - start.a) * t
        };
    }

    [[nodiscard]] static constexpr Color White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
    [[nodiscard]] static constexpr Color Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

    [[nodiscard]] static constexpr Color Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
    [[nodiscard]] static constexpr Color Green() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
    [[nodiscard]] static constexpr Color Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }

    [[nodiscard]] static constexpr Color Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
    [[nodiscard]] static constexpr Color Cyan() { return { 0.0f, 1.0f, 1.0f, 1.0f }; }
    [[nodiscard]] static constexpr Color Magenta() { return { 1.0f, 0.0f, 1.0f, 1.0f }; }

    [[nodiscard]] static constexpr Color Clear() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
};
