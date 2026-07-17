// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Color.h"
#include "Engine/Math/Vector3.h"


struct DebugSettings
{
    bool overlayVisible = true;
    bool draw3DVisible = true;
};

namespace Debug
{
struct Line
{
    Vector3 start = {};
    Vector3 end = {};
    Color color = Color::White();
};

namespace Config
{
constexpr int EntitySamplingInterval = 1000;
}

namespace Scale
{
constexpr float Force = 0.1f;
}

namespace DrawColor
{
namespace Force
{
constexpr Color Attraction  = Color::Red();
constexpr Color Swirl       = Color::Cyan();
constexpr Color Lift        = Color::Green();
constexpr Color Boids       = Color::Blue();
constexpr Color Wander      = Color::Yellow();
constexpr Color Dart        = Color::Magenta();
constexpr Color Boundary    = Color::White();
}
}
}
