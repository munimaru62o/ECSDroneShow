// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/SceneComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, SceneLightingComponent& c)
{
    c.ambient = j.value("ambient", c.ambient);

    if (j.contains("direction")) {
        c.direction = j["direction"].get<Vector3>();
    }
    if (j.contains("lightColor")) {
        c.lightColor = j["lightColor"].get<Color>();
    }
    if (j.contains("fillColor")) {
        c.fillColor = j["fillColor"].get<Color>();
    }
}
