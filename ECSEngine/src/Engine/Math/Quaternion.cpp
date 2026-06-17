// Copyright (c) 2026 munimaru62o. All rights reserved.


#include "Engine/Math/Quaternion.h"

#include <cmath>
#include <algorithm>


Quaternion Quaternion::Identity()
{
    return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
}


void Quaternion::Normalize()
{
    float mag2 = w * w + x * x + y * y + z * z;
    if (mag2 < 0.00001f) {
        x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f;
    } else if (std::abs(mag2 - 1.0f) > 0.00001f) {
        float mag = std::sqrt(mag2);
        x /= mag; y /= mag; z /= mag; w /= mag;
    }
}


Vector3 Quaternion::Rotate(const Vector3& v) const
{
    Vector3 qv(x, y, z);
    Vector3 t = qv.Cross(v) * 2.0f;

    return v + t * w + qv.Cross(t);
}


float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
    return a.x * b.x +
        a.y * b.y +
        a.z * b.z +
        a.w * b.w;
}


Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& up)
{
    Vector3 f = forward.Normalized();
    Vector3 r = up.Cross(f).Normalized();
    Vector3 u = f.Cross(r);

    float m00 = r.x;
    float m01 = r.y;
    float m02 = r.z;

    float m10 = u.x;
    float m11 = u.y;
    float m12 = u.z;

    float m20 = f.x;
    float m21 = f.y;
    float m22 = f.z;

    float trace = m00 + m11 + m22;

    Quaternion q;

    if (trace > 0.0f) {
        float s = std::sqrt(trace + 1.0f) * 2.0f;

        q.w = 0.25f * s;
        q.x = (m12 - m21) / s;
        q.y = (m20 - m02) / s;
        q.z = (m01 - m10) / s;
    } else if (m00 > m11 && m00 > m22) {
        float s = std::sqrt(1.0f + m00 - m11 - m22) * 2.0f;

        q.w = (m12 - m21) / s;
        q.x = 0.25f * s;
        q.y = (m01 + m10) / s;
        q.z = (m02 + m20) / s;
    } else if (m11 > m22) {
        float s = std::sqrt(1.0f + m11 - m00 - m22) * 2.0f;

        q.w = (m20 - m02) / s;
        q.x = (m01 + m10) / s;
        q.y = 0.25f * s;
        q.z = (m12 + m21) / s;
    } else {
        float s = std::sqrt(1.0f + m22 - m00 - m11) * 2.0f;

        q.w = (m01 - m10) / s;
        q.x = (m02 + m20) / s;
        q.y = (m12 + m21) / s;
        q.z = 0.25f * s;
    }

    q.Normalize();
    return q;
}


Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);

    Quaternion q1 = a;
    Quaternion q2 = b;

    float dot = q1.Dot(q1, q2);

    if (dot < 0.0f) {
        q2 = q2 * -1.0f;
        dot = -dot;
    }

    constexpr float DOT_THRESHOLD = 0.995f;

    if (dot > DOT_THRESHOLD) {
        Quaternion result = q1 * (1.0f - t) + q2 * t;

        result.Normalize();
        return result;
    }

    float theta0 = std::acos(dot);
    float theta = theta0 * t;

    float sinTheta = std::sin(theta);
    float sinTheta0 = std::sin(theta0);

    float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
    float s1 = sinTheta / sinTheta0;

    Quaternion result = (q1 * s0) + (q2 * s1);

    result.Normalize();
    return result;
}


Quaternion Quaternion::FromEuler(float rotX, float rotY, float rotZ)
{
    float cx = std::cos(rotX * 0.5f);
    float sx = std::sin(rotX * 0.5f);
    float cy = std::cos(rotY * 0.5f);
    float sy = std::sin(rotY * 0.5f);
    float cz = std::cos(rotZ * 0.5f);
    float sz = std::sin(rotZ * 0.5f);

    return Quaternion(
        sx * cy * cz - cx * sy * sz,
        cx * sy * cz + sx * cy * sz,
        cx * cy * sz - sx * sy * cz,
        cx * cy * cz + sx * sy * sz
    );
}


Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
    Vector3 n = axis.Normalized();

    float halfAngle = angle * 0.5f;
    float sinHalf = std::sin(halfAngle);
    float cosHalf = std::cos(halfAngle);

    return Quaternion(
        n.x * sinHalf,
        n.y * sinHalf,
        n.z * sinHalf,
        cosHalf
    );
}
