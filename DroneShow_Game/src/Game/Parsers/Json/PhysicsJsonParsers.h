// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/PhysicsComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, RandomVelocityRequestComponent& c)
{
    c.minSpeed = j.value("minSpeed", c.minSpeed);
    c.maxSpeed = j.value("maxSpeed", c.maxSpeed);
}

inline void from_json(const nlohmann::json& j, SpeedConstraintComponent& c)
{
    c.max = j.value("max", c.max);
    c.min = j.value("min", c.min);
}

inline void from_json(const nlohmann::json& j, PlaneConstraintComponent& c)
{
    if (j.contains("normal")) {
        c.normal = j["normal"].get<Vector3>();
        c.normal = c.normal.Normalized();
    }
    c.distance = j.value("distance", c.distance);
}

inline void from_json(const nlohmann::json& j, DampingComponent& c)
{
    c.factor = j.value("factor", c.factor);
}

inline void from_json(const nlohmann::json& j, VelocityRotationComponent& c)
{
    c.rotateSpeed = j.value("rotateSpeed", c.rotateSpeed);
}
