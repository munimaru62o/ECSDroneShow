// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/RenderComponents.h"
#include "Engine/Render/RenderTypes.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>
#include <string>

inline void from_json(const nlohmann::json& j, MaterialRequestComponent& c)
{
    if (j.contains("color")) {
        c.color = j["color"].get<Color>();
    }

    if (j.contains("shaderType")) {
        std::string shaderStr = j["shaderType"].get<std::string>();
        c.shaderType = ToPixelShaderType(shaderStr);
    }
}

inline void from_json(const nlohmann::json& j, PrimitiveMeshComponent& c)
{
    if (j.contains("type")) {
        std::string shaderStr = j["type"].get<std::string>();
        c.type = ToPrimitiveMeshType(shaderStr);
    }

    if (j.contains("scale")) {
        c.scale = j["scale"].get<Vector3>();
    }
}
