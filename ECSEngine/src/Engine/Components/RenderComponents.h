// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Render/RenderTypes.h"

#include <optional>


struct MaterialComponent
{
    Color color = Color::White();
    PixelShaderType shaderType = PixelShaderType::Lit;
};

struct MaterialRequestComponent
{
    std::optional<Color> color{};
    std::optional<PixelShaderType> shaderType{};
};

struct PrimitiveMeshComponent
{
    PrimitiveMeshType type = PrimitiveMeshType::Cube;
    Vector3 scale = { 1.0f, 1.0f, 1.0f };
};
