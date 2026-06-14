// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"


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
