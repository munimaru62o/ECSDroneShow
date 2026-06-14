// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"


struct SceneLightingComponent
{
    float ambient = 0.1f;
    Vector3 direction = { -1.0f, -1.0f, 1.0f };
    Color lightColor = Color::White();
    Color fillColor = Color::Black();
};


