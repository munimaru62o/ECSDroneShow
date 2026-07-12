// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"

#include <cmath>


Matrix4 Matrix4::operator*(const Matrix4& rhs) const
{
    Matrix4 result{};

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.m[i][j] =
                m[i][0] * rhs.m[0][j] +
                m[i][1] * rhs.m[1][j] +
                m[i][2] * rhs.m[2][j] +
                m[i][3] * rhs.m[3][j];
        }
    }

    return result;
}


Matrix4& Matrix4::operator*=(const Matrix4& rhs)
{
    *this = *this * rhs;
    return *this;
}


Matrix4 Matrix4::Identity()
{
    return Matrix4{
        {
            {1,0,0,0},
            {0,1,0,0},
            {0,0,1,0},
            {0,0,0,1}
        }
    };
}

Matrix4 Matrix4::Translation(const Vector3& position)
{
    Matrix4 result = Identity();

    result.m[3][0] = position.x;
    result.m[3][1] = position.y;
    result.m[3][2] = position.z;

    return result;
}


Matrix4 Matrix4::Scale(const Vector3& scale)
{
    Matrix4 result{};

    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    result.m[3][3] = 1.0f;

    return result;
}


Matrix4 Matrix4::Rotation(const Quaternion& rotation)
{
    Matrix4 result = Identity();

    const float xx = rotation.x * rotation.x;
    const float yy = rotation.y * rotation.y;
    const float zz = rotation.z * rotation.z;

    const float xy = rotation.x * rotation.y;
    const float xz = rotation.x * rotation.z;
    const float yz = rotation.y * rotation.z;

    const float wx = rotation.w * rotation.x;
    const float wy = rotation.w * rotation.y;
    const float wz = rotation.w * rotation.z;

    result.m[0][0] = 1.0f - 2.0f * (yy + zz);
    result.m[0][1] = 2.0f * (xy + wz);
    result.m[0][2] = 2.0f * (xz - wy);

    result.m[1][0] = 2.0f * (xy - wz);
    result.m[1][1] = 1.0f - 2.0f * (xx + zz);
    result.m[1][2] = 2.0f * (yz + wx);

    result.m[2][0] = 2.0f * (xz + wy);
    result.m[2][1] = 2.0f * (yz - wx);
    result.m[2][2] = 1.0f - 2.0f * (xx + yy);

    return result;
}


Matrix4 Matrix4::TRS(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
    Matrix4 result{};

    const float xx = rotation.x * rotation.x;
    const float yy = rotation.y * rotation.y;
    const float zz = rotation.z * rotation.z;

    const float xy = rotation.x * rotation.y;
    const float xz = rotation.x * rotation.z;
    const float yz = rotation.y * rotation.z;

    const float wx = rotation.w * rotation.x;
    const float wy = rotation.w * rotation.y;
    const float wz = rotation.w * rotation.z;

    // Rotation * Scale
    result.m[0][0] = (1.0f - 2.0f * (yy + zz)) * scale.x;
    result.m[0][1] = (2.0f * (xy + wz)) * scale.x;
    result.m[0][2] = (2.0f * (xz - wy)) * scale.x;
    result.m[0][3] = 0.0f;

    result.m[1][0] = (2.0f * (xy - wz)) * scale.y;
    result.m[1][1] = (1.0f - 2.0f * (xx + zz)) * scale.y;
    result.m[1][2] = (2.0f * (yz + wx)) * scale.y;
    result.m[1][3] = 0.0f;

    result.m[2][0] = (2.0f * (xz + wy)) * scale.z;
    result.m[2][1] = (2.0f * (yz - wx)) * scale.z;
    result.m[2][2] = (1.0f - 2.0f * (xx + yy)) * scale.z;
    result.m[2][3] = 0.0f;

    // Translation
    result.m[3][0] = position.x;
    result.m[3][1] = position.y;
    result.m[3][2] = position.z;
    result.m[3][3] = 1.0f;

    return result;
}


Matrix4 Matrix4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    Vector3 zAxis = target - eye;
    zAxis.Normalize();

    Vector3 xAxis = Vector3::Cross(up, zAxis);
    xAxis.Normalize();

    Vector3 yAxis = Vector3::Cross(zAxis, xAxis);

    Matrix4 result = Identity();
    result.m[0][0] = xAxis.x;
    result.m[1][0] = xAxis.y;
    result.m[2][0] = xAxis.z;

    result.m[0][1] = yAxis.x;
    result.m[1][1] = yAxis.y;
    result.m[2][1] = yAxis.z;

    result.m[0][2] = zAxis.x;
    result.m[1][2] = zAxis.y;
    result.m[2][2] = zAxis.z;

    result.m[3][0] = -Vector3::Dot(xAxis, eye);
    result.m[3][1] = -Vector3::Dot(yAxis, eye);
    result.m[3][2] = -Vector3::Dot(zAxis, eye);

    return result;
}


Matrix4 Matrix4::Perspective(float fovY, float aspect, float zNear, float zFar)
{
    Matrix4 result{};

    const float tanHalfFov = std::tan(fovY * 0.5f);

    result.m[0][0] = 1.0f / (aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;

    result.m[2][2] = zFar / (zFar - zNear);
    result.m[2][3] = 1.0f;

    result.m[3][2] = -(zNear * zFar) / (zFar - zNear);
    result.m[3][3] = 0.0f;

    return result;
}
