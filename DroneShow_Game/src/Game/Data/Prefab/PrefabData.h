// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

#include <string>
#include <json.hpp>

struct PrefabData
{
    std::string name;
    nlohmann::json components;
};
