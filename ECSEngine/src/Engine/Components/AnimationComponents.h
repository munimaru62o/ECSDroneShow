// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"
#include "Engine/Math/Quaternion.h"


struct SpinAnimationComponent
{
    Quaternion baseRotation{};
    Vector3 axis{};
    double creationTime = 0.0;
    float speed = 0.0f;
    float duration = 0.0f;
};

struct SpinAnimationRequestComponent
{
    Vector3 axis{};
    float duration = 0.0f;
    float minSpeed = 0.0f;
    float maxSpeed = 0.0f;
    bool isRandomAxis = false;
    bool resetRotation = false;
    uint32_t seed = 0;
};

struct ColorFadeComponent
{
    Color startColor = Color::White();
    Color targetColor = Color::White();
    float duration = 0.0f;
    double creationTime = 0.0;
};

struct ColorFadeRequestComponent
{
    Color targetColor = Color::White();
    float duration = 0.0f;
};

struct ColorBlinkComponent
{
    Color color1 = Color::White();
    Color color2 = Color::White();
    float speed = 1.0f;
    float delayTime = 0.0f;
    double creationTime = 0.0;
};
