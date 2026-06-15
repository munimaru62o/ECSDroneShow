// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/AnimationComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, SpinAnimationRequestComponent& c)
{
    if (j.contains("axis")) {
        c.axis = j["axis"].get<Vector3>();
        c.axis = c.axis.Normalized();
    }
    c.duration = j.value("duration", c.duration);
    c.minSpeed = j.value("minSpeed", c.minSpeed);
    c.maxSpeed = j.value("maxSpeed", c.maxSpeed);
    c.isRandomAxis = j.value("isRandomAxis", c.isRandomAxis);
    c.resetRotation = j.value("resetRotation", c.resetRotation);

    if (c.minSpeed > c.maxSpeed) {
        c.maxSpeed = c.minSpeed;
    }
}

inline void from_json(const nlohmann::json& j, ColorFadeRequestComponent& c)
{
    if (j.contains("targetColor")) {
        c.targetColor = j["targetColor"].get<Color>();
    }
    if (j.contains("duration")) {
        c.duration = j["duration"].get<float>();
    }
}

inline void from_json(const nlohmann::json& j, ColorBlinkComponent& c)
{
    if (j.contains("color1")) {
        c.color1 = j["color1"].get<Color>();
    }
    if (j.contains("color2")) {
        c.color2 = j["color2"].get<Color>();
    }
    if (j.contains("speed")) {
        c.speed = j["speed"].get<float>();
    }
    if (j.contains("delayTime")) {
        c.delayTime = j["delayTime"].get<float>();
    }
    assert(c.speed > 0.0f);
}
