// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Components/SpatialComponents.h"
#include "Game/Parsers/Json/MathJsonParsers.h"

#include <json.hpp>


inline void from_json(const nlohmann::json& /*j*/, SpatialPartitionComponent& /*c*/)
{
    // SpatialPartitionComponent is currently a tag component with no data members.
    // Included here to fulfill the nlohmann::json deserialization requirements.
}
