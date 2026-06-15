// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Components/GameComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, DroneComponent& c)
{}

inline void from_json(const nlohmann::json& j, FormationComponentWireframe& c)
{
    c.scale = j.value("scale", c.scale);
    c.file = j.value("file", c.file);
}

inline void from_json(const nlohmann::json& j, FormationComponentPointCloud& c)
{
    c.scale = j.value("scale", c.scale);
    c.file = j.value("file", c.file);
}

inline void from_json(const nlohmann::json& j, FormationRotationComponent& c)
{
    c.rotateSpeed = j.value("rotateSpeed", c.rotateSpeed);
}
