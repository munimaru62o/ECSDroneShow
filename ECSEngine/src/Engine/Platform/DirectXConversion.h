// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"
#include <DirectXMath.h>


[[nodiscard]] inline DirectX::XMFLOAT3 ToXMFloat3(const Vector3& v)
{
    return { v.x, v.y, v.z };
}

[[nodiscard]] inline DirectX::XMFLOAT4 ToXMFloat4(const Color& c)
{
    return { c.r, c.g, c.b, c.a };
}

[[nodiscard]] inline DirectX::XMFLOAT3 ToDXFloat3(const Vector3& v)
{
    return { v.x, v.y, v.z };
}

[[nodiscard]] inline DirectX::XMFLOAT4X4 ToDXFloat4(const Matrix4& m)
{
    DirectX::XMFLOAT4X4 result;
    memcpy(&result, &m, sizeof(result));
    return result;
}
