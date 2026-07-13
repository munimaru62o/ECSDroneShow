// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Color.h"
#include "Engine/Math/Vector2.h"

#include <imgui.h>


[[nodiscard]] inline ImVec4 ToImVec4(const Color& c)
{
    return { c.r, c.g, c.b, c.a };
}

[[nodiscard]] inline ImVec2 ToImVec2(const Vector2& vec)
{
    return { vec.x, vec.y };
}
