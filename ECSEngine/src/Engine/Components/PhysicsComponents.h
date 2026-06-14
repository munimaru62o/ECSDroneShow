// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"


struct RandomVelocityRequestComponent
{
    float minSpeed = 100.0f;
    float maxSpeed = 200.0f;
    uint32_t seed = 0;
};

struct SpeedConstraintComponent
{
    float max = 100.0f;
    float min = 0.0f;
};

struct PlaneConstraintComponent
{
    Vector3 normal = Vector3::Up();
    float distance = 0.0f;
};

struct DampingComponent
{
    float factor = 0.95f;
};

struct VelocityRotationComponent
{
    float rotateSpeed = 5.0f;
};
