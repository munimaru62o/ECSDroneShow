// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/ECSTypes.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Constants.h"
#include "Engine/Physics/ForceProfile.h"
#include "Engine/Physics/ForcePlane.h"


struct BoidsComponent
{
    float separationWeight = 1.0f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 1.0f;
    float visionRadius = 100.0f;

    float maxForce = 5.0f;
    float updateInterval = 0.2f;

    Vector3 cachedDirection = {};
    uint32_t seed = 0;
    float phase = 0.0f;
    double nextUpdateTime = 0.0;
    float updateIntervalScale = 1.0f;

    void Initialize(Entity e)
    {
        phase = static_cast<float>(seed) * MathConstants::INV_UINT32;
        updateIntervalScale = 0.8f + phase * 0.4f; // 0.8~1.2
        nextUpdateTime = phase * updateInterval;
    }
};

struct JitterMovementComponent
{
    float strength = 100.0f;
    uint32_t seed = 0;
};

struct LiftComponent
{
    Vector3 center = { 0.0f, 0.0f, 0.0f };
    Vector3 axis = { 0.0f, 1.0f, 0.0f };
    ForceProfile profile = {};
    float strength = 100.0f;
    float maxForce = 500.0f;
};

struct AttractionComponent
{
    Vector3 center = { 0.0f, 0.0f, 0.0f };
    ForceProfile profile = {};
    ForcePlaneType plane = ForcePlaneType::XYZ;
    float strength = 100.0f;
    float maxForce = 500.0f;
};

struct SwirlComponent
{
    Vector3 center = { 0.0f, 0.0f, 0.0f };
    Vector3 axis = { 0.0f, 1.0f, 0.0f };
    ForceProfile profile = {};
    float strength = 100.0f;
    float maxForce = 500.0f;
};

struct RandomSwirlAxisRequestComponent {
    int axisCount = 2;
    float noiseStrength = 0.15f;
    uint32_t seed = 0;
};

struct GravityComponent
{
    Vector3 direction = Vector3::Down();
    float strength = 100.0f;
};

struct BoundaryComponent
{
    float radius = 100.0f;
    float force = 10.0f;
    Vector3 center{};
};

struct WanderComponent
{
    float strength = 100.0f;
    float frequency = 0.5f;
    float updateInterval = 0.2f;

    uint32_t seed = 0;
    float phase = 0.0f;
    double nextUpdateTime = 0.0;
    Vector3 cachedDirection = {};
    float updateIntervalScale = 1.0f;

    void Initialize(Entity e)
    {
        phase = static_cast<float>(seed) * MathConstants::INV_UINT32;
        updateIntervalScale = 0.8f + phase * 0.4f; // 0.8~1.2
        nextUpdateTime = phase * updateInterval;
    }
};

struct DartComponent
{
    float intervalMin = 2.0f;
    float intervalMax = 5.0f;
    float strength = 100.0f;

    uint32_t seed = 0;
    double nextDartTime = 0.0;
    Vector3 cachedDirection = {};

    void Initialize(Entity e)
    {
        float phase = static_cast<float>(seed) * MathConstants::INV_UINT32;
        nextDartTime = intervalMin * (0.8f + phase * 0.4f); // 0.8~1.2
    }
};
