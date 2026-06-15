// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

/**
 * @struct Quaternion
 * @brief Represents a quaternion used for calculating and representing 3D rotations.
 *
 * Quaternions provide a robust alternative to Euler angles, effectively preventing
 * Gimbal Lock and allowing for smooth interpolation (Slerp) between rotations.
 * This implementation includes highly optimized operations for vector rotation,
 * spherical linear interpolation, and conversions to/from various rotation formats.
 */
struct Quaternion
{
    float x, y, z, w;

    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

    [[nodiscard]] inline Quaternion operator*(const Quaternion& rhs) const
    {
        return Quaternion(
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
            w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
        );
    }

    inline Quaternion& operator*=(const Quaternion& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    [[nodiscard]] inline Quaternion operator+(const Quaternion& rhs) const
    {
        return Quaternion(
            x + rhs.x,
            y + rhs.y,
            z + rhs.z,
            w + rhs.w
        );
    }

    [[nodiscard]] inline Quaternion operator*(float s) const
    {
        return Quaternion(
            x * s,
            y * s,
            z * s,
            w * s
        );
    }

    [[nodiscard]] static Quaternion Identity();

    void Normalize();
    [[nodiscard]] Vector3 Rotate(const Vector3& v) const;

    [[nodiscard]] static float Dot(const Quaternion& a, const Quaternion& b);
    [[nodiscard]] static Quaternion LookRotation(const Vector3& forward, const Vector3& up = Vector3::Up());
    [[nodiscard]] static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t);

    /**
    * @brief Creates a rotation from Euler angles.
    * @note The rotation order applied is Z-X-Y (Roll-Pitch-Yaw).
    */
    [[nodiscard]] static Quaternion FromEuler(float rotX, float rotY, float rotZ);
    [[nodiscard]] static Quaternion FromAxisAngle(const Vector3& axis, float angle);
};
