// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"


struct OrbitComponent
{
    double creationTime = 0.0f;
    float speed = 1.0f;
    float duration = 0.0f;
    Vector3 axis = Vector3::Up();
};

struct SteeringComponent
{
    float stiffness = 15.0f;
    float dampingRatio = 1.0f;
    float precalculatedDamping = 2.0f * 1.0f * std::sqrt(15.0f);

    void SetParameters(float newStiffness, float newDampingRatio)
    {
        stiffness = newStiffness;
        dampingRatio = newDampingRatio;
        Recalculate();
    }

    void Recalculate()
    {
        precalculatedDamping = 2.0f * dampingRatio * std::sqrt(stiffness);
    }
};
