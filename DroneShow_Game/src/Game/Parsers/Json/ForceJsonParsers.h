// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/ForceComponents.h"
#include "Engine/Physics/ForceProfile.h"
#include "Engine/Physics/ForcePlane.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, ForceProfile& c)
{
    if (j.contains("falloff")) {
        std::string str = j["falloff"].get<std::string>();
        c.falloffType = ForceFalloff::ToFalloffType(str);
    }
    if (j.contains("distance")) {
        std::string str = j["distance"].get<std::string>();
        c.distanceType = ForceDistance::ToDistanceType(str);
    }
    float radius = j.value("radius", c.radius);
    c.radius = radius;
    c.innerScale = j.value("innerScale", c.innerScale);
    c.outerScale = j.value("outerScale", c.outerScale);

    assert(radius > 0.0f && "Ivalid radius value.");
    if (radius > 0.0f) {
        c.invRadius = (1.0f / radius);
    }
}

inline void from_json(const nlohmann::json& j, BoidsComponent& c)
{
    c.separationWeight = j.value("separationWeight", c.separationWeight);
    c.alignmentWeight = j.value("alignmentWeight", c.alignmentWeight);
    c.cohesionWeight = j.value("cohesionWeight", c.cohesionWeight);
    c.visionRadius = j.value("visionRadius", c.visionRadius);
    c.maxForce = j.value("maxForce", c.maxForce);
    c.updateInterval = j.value("updateInterval", c.updateInterval);
}

inline void from_json(const nlohmann::json& j, JitterMovementComponent& c)
{
    c.strength = j.value("strength", c.strength);
}

inline void from_json(const nlohmann::json& j, LiftComponent& c)
{
    if (j.contains("center")) {
        c.center = j["center"].get<Vector3>();
    }
    if (j.contains("axis")) {
        c.axis = j["axis"].get<Vector3>();
        c.axis = c.axis.Normalized();
    }
    if (j.contains("profile")) {
        c.profile = j["profile"].get<ForceProfile>();
    }
    c.strength = j.value("strength", c.strength);
    c.maxForce = j.value("maxForce", c.maxForce);
}

inline void from_json(const nlohmann::json& j, AttractionComponent& c)
{
    if (j.contains("center")) {
        c.center = j["center"].get<Vector3>();
    }
    if (j.contains("profile")) {
        c.profile = j["profile"].get<ForceProfile>();
    }
    if (j.contains("plane")) {
        c.plane = ForcePlane::ToPlaneType(j["plane"]);
    }
    c.strength = j.value("strength", c.strength);
    c.maxForce = j.value("maxForce", c.maxForce);
}

inline void from_json(const nlohmann::json& j, SwirlComponent& c)
{
    if (j.contains("center")) {
        c.center = j["center"].get<Vector3>();
    }
    if (j.contains("axis")) {
        c.axis = j["axis"].get<Vector3>();
        c.axis = c.axis.Normalized();
    }
    if (j.contains("profile")) {
        c.profile = j["profile"].get<ForceProfile>();
    }
    c.strength = j.value("strength", c.strength);
    c.maxForce = j.value("maxForce", c.maxForce);
}

inline void from_json(const nlohmann::json& j, RandomSwirlAxisRequestComponent& c)
{
    c.axisCount = j.value("axisCount", c.axisCount);
    c.noiseStrength = j.value("noiseStrength", c.noiseStrength);
}

inline void from_json(const nlohmann::json& j, GravityComponent& c)
{
    if (j.contains("direction")) {
        c.direction = j["direction"].get<Vector3>();
        c.direction = c.direction.Normalized();
    }
    c.strength = j.value("strength", c.strength);
}

inline void from_json(const nlohmann::json& j, BoundaryComponent& c)
{
    c.radius = j.value("radius", c.radius);
    c.force = j.value("force", c.force);

    if (j.contains("center")) {
        c.center = j["center"].get<Vector3>();
    }
}

inline void from_json(const nlohmann::json& j, WanderComponent& c)
{
    c.strength = j.value("strength", c.strength);
    c.frequency = j.value("frequency", c.frequency);
    c.updateInterval = j.value("updateInterval", c.updateInterval);
}

inline void from_json(const nlohmann::json& j, DartComponent& c)
{
    c.intervalMin = j.value("intervalMin", c.intervalMin);
    c.intervalMax = j.value("intervalMax", c.intervalMax);
    c.strength = j.value("strength", c.strength);
}
