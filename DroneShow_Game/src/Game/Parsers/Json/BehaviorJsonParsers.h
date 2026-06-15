// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/BehaviorComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, OrbitComponent& c)
{
    c.speed = j.value("speed", c.speed);
    c.duration = j.value("duration", c.duration);

    if (j.contains("axis")) {
        c.axis = j["axis"].get<Vector3>();
        c.axis = c.axis.Normalized();
    }
}

inline void from_json(const nlohmann::json& j, SteeringComponent& c)
{
    float s = j.value("stiffness", c.stiffness);
    float d = j.value("dampingRatio", c.dampingRatio);
    c.SetParameters(s, d);
}
