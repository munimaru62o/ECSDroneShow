// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Color.h"

#include <imgui.h>


[[nodiscard]] inline ImVec4 ToImVec4(const Color& c)
{
    return { c.r, c.g, c.b, c.a };
}
