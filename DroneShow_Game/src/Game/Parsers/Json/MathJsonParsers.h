// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& j, Vector3& c)
{
    if (j.is_array() && j.size() >= 3) {
        c.x = j[0].get<float>();
        c.y = j[1].get<float>();
        c.z = j[2].get<float>();
    }
    else if (j.is_object()) {
        c.x = j.value("x", c.x);
        c.y = j.value("y", c.y);
        c.z = j.value("z", c.z);
    }
}


inline void from_json(const nlohmann::json& j, Color& c)
{
    if (j.is_array() && j.size() >= 3) {
        c.r = j[0].get<float>();
        c.g = j[1].get<float>();
        c.b = j[2].get<float>();
        if (j.size() >= 4) {
            c.a = j[3].get<float>();
        }
    }
    else if (j.is_object()) {
        c.r = j.value("r", c.r);
        c.g = j.value("g", c.g);
        c.b = j.value("b", c.b);
        c.a = j.value("a", c.a);
    }

    c.r /= 255.0f;
    c.g /= 255.0f;
    c.b /= 255.0f;
    c.a /= 255.0f;
}
