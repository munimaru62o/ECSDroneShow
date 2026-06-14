// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

#include <string>
#include <cassert>
#include <string_view>

/**
 * @enum ForcePlaneType
 * @brief Defines the planes or axes to which force applications can be constrained.
 */
enum class ForcePlaneType
{
    XYZ,
    XY,
    XZ,
    YZ,
    X,
    Y,
    Z
};

namespace ForcePlane
{
/**
 * @brief Converts a string representation to its corresponding ForcePlaneType enum.
 * @param str The string to parse (e.g., "XY", "Z").
 * @return The parsed ForcePlaneType. Defaults to XYZ if the string is invalid.
 */
[[nodiscard]] inline ForcePlaneType ToPlaneType(std::string_view str)
{
    if (str == "XYZ") return ForcePlaneType::XYZ;
    if (str == "XY")  return ForcePlaneType::XY;
    if (str == "XZ")  return ForcePlaneType::XZ;
    if (str == "YZ")  return ForcePlaneType::YZ;
    if (str == "X")   return ForcePlaneType::X;
    if (str == "Y")   return ForcePlaneType::Y;
    if (str == "Z")   return ForcePlaneType::Z;

    assert(false && "Invalid constraint type string");
    return ForcePlaneType::XYZ;
}

/**
 * @brief Projects (constrains) a vector onto the specified plane or axis.
 * @param direction The original vector to project.
 * @param plane The plane or axis constraint to apply.
 * @return A new vector constrained to the specified plane or axis.
 */
[[nodiscard]] inline Vector3 Project(const Vector3& direction, ForcePlaneType plane)
{
    switch (plane) {
        case ForcePlaneType::XYZ: return direction;
        case ForcePlaneType::XY:  return { direction.x, direction.y, 0.0f };
        case ForcePlaneType::XZ:  return { direction.x, 0.0f, direction.z };
        case ForcePlaneType::YZ:  return { 0.0f, direction.y, direction.z };
        case ForcePlaneType::X:   return { direction.x, 0.0f, 0.0f };
        case ForcePlaneType::Y:   return { 0.0f, direction.y, 0.0f };
        case ForcePlaneType::Z:   return { 0.0f, 0.0f, direction.z };
    }
    return direction;
}
}
