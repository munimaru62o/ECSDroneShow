// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <cmath>
#include <cassert>

/**
 * @struct Vector3
 * @brief Represents a 3D vector and point, commonly used for position, direction, and velocity in 3D space.
 *
 * This structure provides standard mathematical operations (addition, subtraction, scalar multiplication/division)
 * along with essential spatial calculations such as Dot Product, Cross Product, magnitude (Length), and normalization.
 * It also defines `constexpr` static helpers for standard directional axes (e.g., Up, Forward, Right).
 */
struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    [[nodiscard]] static constexpr Vector3 Up() { return { 0.0f,  1.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector3 Down() { return { 0.0f, -1.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector3 Forward() { return { 0.0f,  0.0f,  1.0f }; }
    [[nodiscard]] static constexpr Vector3 Back() { return { 0.0f,  0.0f, -1.0f }; }
    [[nodiscard]] static constexpr Vector3 Right() { return { 1.0f,  0.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector3 Left() { return { -1.0f,  0.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector3 Zero() { return { 0.0f,  0.0f,  0.0f }; }

    [[nodiscard]] Vector3 operator+(const Vector3& rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }
    Vector3& operator+=(const Vector3& rhs)
    {
        x += rhs.x; y += rhs.y; z += rhs.z;
        return *this;
    }

    [[nodiscard]] Vector3 operator-() const
    {
        return { -x, -y, -z };
    }
    [[nodiscard]] Vector3 operator-(const Vector3& rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }
    Vector3& operator-=(const Vector3& rhs)
    {
        x -= rhs.x; y -= rhs.y; z -= rhs.z;
        return *this;
    }

    [[nodiscard]] Vector3 operator*(float scalar) const
    {
        return { x * scalar, y * scalar, z * scalar };
    }
    Vector3& operator*=(float scalar)
    {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    [[nodiscard]] Vector3 operator/(float scalar) const
    {
        assert(std::abs(scalar) > 0.00001f && "Divide by zero in Vector3.");
        float inv = 1.0f / scalar;
        return { x * inv, y * inv, z * inv };
    }
    Vector3& operator/=(float scalar)
    {
        x /= scalar; y /= scalar; z /= scalar;
        return *this;
    }

    [[nodiscard]] float LengthSq() const
    {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] float Length() const
    {
        return std::sqrt(LengthSq());
    }

    [[nodiscard]] static float DistanceSq(const Vector3& a, const Vector3& b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return dx * dx + dy * dy + dz * dz;
    }

    [[nodiscard]] static float Distance(const Vector3& a, const Vector3& b)
    {
        return std::sqrt(DistanceSq(a, b));
    }

    [[nodiscard]] Vector3 Normalized() const
    {
        float len = Length();
        if (len > 0.0001f) {
            return { x / len, y / len, z / len };
        }
        return { 0.0f, 0.0f, 0.0f };
    }

    [[nodiscard]] bool IsNormalized(float epsilon = 0.0001f) const
    {
        return std::abs(LengthSq() - 1.0f) < epsilon;
    }

    [[nodiscard]] Vector3 Cross(const Vector3& rhs) const
    {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }
    [[nodiscard]] static Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return a.Cross(b);
    }

    [[nodiscard]] float Dot(const Vector3& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    [[nodiscard]] static float Dot(const Vector3& a, const Vector3& b)
    {
        return a.Dot(b);
    }
};
