// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"
#include "Engine/Math/Matrix4.h"
#include <DxLib.h>


[[nodiscard]] inline VECTOR ToDxVec(const Vector3& v)
{
    return VGet(v.x, v.y, v.z);
}

[[nodiscard]] inline Vector3 FromDxVec(const VECTOR& v)
{
    return Vector3(v.x, v.y, v.z);
}

[[nodiscard]] inline unsigned int ToDxColor(const Color& c)
{
    return GetColor(
        static_cast<int>(c.r * 255.0f),
        static_cast<int>(c.g * 255.0f),
        static_cast<int>(c.b * 255.0f)
    );
}

[[nodiscard]] inline MATRIX ToDxMatrix(const Matrix4& matrix)
{
    static_assert(sizeof(Matrix4) == sizeof(MATRIX));

    MATRIX result;
    std::memcpy(&result, &matrix, sizeof(MATRIX));
    return result;
}

[[nodiscard]] inline Matrix4 FromDxMatrix(const MATRIX& matrix)
{
    static_assert(sizeof(Matrix4) == sizeof(MATRIX));

    Matrix4 result{};
    std::memcpy(&result, &matrix, sizeof(Matrix4));
    return result;
}
