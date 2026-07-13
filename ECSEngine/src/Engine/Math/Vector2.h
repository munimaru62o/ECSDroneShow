// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <cmath>
#include <cassert>

/**
 * @struct Vector2
 * @brief Represents a 2D vector and point, commonly used for screen coordinates, UI layout, input direction, and planar movement.
 *
 * This structure provides standard mathematical operations (addition, subtraction,
 * scalar multiplication/division) along with essential vector calculations such as
 * Dot Product, magnitude (Length), normalization, and distance.
 * It also defines `constexpr` static helpers for common directions.
 */
struct Vector2
{
    float x = 0.0f;
    float y = 0.0f;

    [[nodiscard]] static constexpr Vector2 Up() { return { 0.0f,  1.0f }; }
    [[nodiscard]] static constexpr Vector2 Down() { return { 0.0f, -1.0f }; }
    [[nodiscard]] static constexpr Vector2 Right() { return { 1.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector2 Left() { return { -1.0f,  0.0f }; }
    [[nodiscard]] static constexpr Vector2 Zero() { return { 0.0f,  0.0f }; }

    [[nodiscard]] Vector2 operator+(const Vector2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    Vector2& operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    [[nodiscard]] Vector2 operator-() const
    {
        return { -x, -y };
    }

    [[nodiscard]] Vector2 operator-(const Vector2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    Vector2& operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    [[nodiscard]] Vector2 operator*(float scalar) const
    {
        return { x * scalar, y * scalar };
    }

    Vector2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    [[nodiscard]] Vector2 operator/(float scalar) const
    {
        assert(std::abs(scalar) > 0.00001f && "Divide by zero in Vector2.");
        float inv = 1.0f / scalar;
        return { x * inv, y * inv };
    }

    Vector2& operator/=(float scalar)
    {
        assert(std::abs(scalar) > 0.00001f && "Divide by zero in Vector2.");
        float inv = 1.0f / scalar;
        x *= inv;
        y *= inv;
        return *this;
    }

    [[nodiscard]] float LengthSq() const
    {
        return x * x + y * y;
    }

    [[nodiscard]] float Length() const
    {
        return std::sqrt(LengthSq());
    }

    [[nodiscard]] static float DistanceSq(const Vector2& a, const Vector2& b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    [[nodiscard]] static float Distance(const Vector2& a, const Vector2& b)
    {
        return std::sqrt(DistanceSq(a, b));
    }

    void Normalize()
    {
        float len = Length();
        if (len > 0.0001f) {
            float invLen = 1.0f / len;
            x *= invLen;
            y *= invLen;
        } else {
            x = y = 0.0f;
        }
    }

    [[nodiscard]] Vector2 Normalized() const
    {
        Vector2 result = *this;
        result.Normalize();
        return result;
    }

    [[nodiscard]] bool IsNormalized(float epsilon = 0.0001f) const
    {
        return std::abs(LengthSq() - 1.0f) < epsilon;
    }

    [[nodiscard]] float Dot(const Vector2& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    [[nodiscard]] static float Dot(const Vector2& a, const Vector2& b)
    {
        return a.Dot(b);
    }

    [[nodiscard]] float Cross(const Vector2& rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }

    [[nodiscard]] static float Cross(const Vector2& a, const Vector2& b)
    {
        return a.Cross(b);
    }
};
