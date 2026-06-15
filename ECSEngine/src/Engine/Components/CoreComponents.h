// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"


struct TransformComponent
{
    Quaternion rotation = {};
    Vector3 position = {};
    Vector3 scale = { 1.0f, 1.0f, 1.0f };
};

struct TargetComponent
{
    Vector3 value = {};
};

struct VelocityComponent
{
    Vector3 value = {};
};

struct ForceComponent
{
    Vector3 value = {};
};

struct DirectionComponent
{
    Vector3 value = {};
};
