// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

#include <string>
#include <cassert>
#include <string_view>

/**
 * @enum ForceDistanceType
 * @brief Defines the spatial shape or metric used to calculate the distance from the force source.
 */
enum class ForceDistanceType
{
    Sphere,     // Distance from a central point (3D)
    Cylinder,   // Distance from the Y-axis (2D XZ plane)
    AxisX,      // Distance along the X-axis (Planar)
    AxisY,      // Distance along the Y-axis (Planar)
    AxisZ       // Distance along the Z-axis (Planar)
};

namespace ForceDistance
{
/**
 * @brief Converts a string representation to its corresponding ForceDistanceType enum.
 */
[[nodiscard]] inline ForceDistanceType ToDistanceType(std::string_view str)
{
    if (str == "Sphere")   return ForceDistanceType::Sphere;
    if (str == "Cylinder") return ForceDistanceType::Cylinder;
    if (str == "AxisX")    return ForceDistanceType::AxisX;
    if (str == "AxisY")    return ForceDistanceType::AxisY;
    if (str == "AxisZ")    return ForceDistanceType::AxisZ;

    assert(false && "Invalid distance type string");
    return  ForceDistanceType::Sphere;
}

/**
 * @brief Calculates the effective distance from the source based on the specified shape.
 */
[[nodiscard]] inline float Evaluate(const Vector3& offset, ForceDistanceType type)
{
    switch (type) {
        case ForceDistanceType::Sphere:
            return offset.Length();

        case ForceDistanceType::Cylinder:
            return Vector3(offset.x, 0.0f, offset.z).Length();

        case ForceDistanceType::AxisX:
            return std::abs(offset.x);

        case ForceDistanceType::AxisY:
            return std::abs(offset.y);

        case ForceDistanceType::AxisZ:
            return std::abs(offset.z);
    }
    return offset.Length();
}
}

/**
 * @enum ForceFalloffType
 * @brief Defines how the force attenuates (decreases) over distance.
 */
enum class ForceFalloffType
{
    Constant,   // No attenuation (constant strength)
    Linear,     // Linear decay
    Quadratic   // Quadratic decay (smooth ease-in/ease-out)
};

namespace ForceFalloff
{
/**
 * @brief Converts a string representation to its corresponding ForceFalloffType enum.
 */
[[nodiscard]] inline ForceFalloffType ToFalloffType(std::string_view str)
{
    if (str == "Constant")  return ForceFalloffType::Constant;
    if (str == "Linear")    return ForceFalloffType::Linear;
    if (str == "Quadratic") return ForceFalloffType::Quadratic;

    assert(false && "Invalid falloff type string");
    return ForceFalloffType::Constant;
}

/**
 * @brief Evaluates the falloff curve at a normalized distance 't' (0.0 to 1.0).
 * @return An interpolation factor (0.0 = inner strength, 1.0 = outer strength).
 */
[[nodiscard]] inline float Evaluate(ForceFalloffType type, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);

    switch (type) {
        case ForceFalloffType::Constant:  return 0.0f;
        case ForceFalloffType::Linear:    return t;
        case ForceFalloffType::Quadratic: return t * t;
    }
    return 0.0f;
}
}

/**
 * @struct ForceProfile
 * @brief Encapsulates the configuration of a spatial force, including its shape, attenuation, and bounds.
 */
struct ForceProfile
{
    ForceFalloffType falloffType = ForceFalloffType::Constant;
    ForceDistanceType distanceType = ForceDistanceType::Sphere;

    float radius = 100.0f;
    float invRadius = 0.01f;
    float innerScale = 1.0f; // Strength at the center (t = 0)
    float outerScale = 0.0f; // Strength at the boundary (t = 1)

    [[nodiscard]] float Evaluate(const Vector3& offset) const
    {
        if (falloffType == ForceFalloffType::Constant) {
            return innerScale;
        }

        float dist = ForceDistance::Evaluate(offset, distanceType);
        float t = std::clamp(dist * invRadius, 0.0f, 1.0f);
        float curve = ForceFalloff::Evaluate(falloffType, t);
        return std::lerp(innerScale, outerScale, curve);
    }
};

