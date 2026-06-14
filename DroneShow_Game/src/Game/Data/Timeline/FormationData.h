// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"

#include <vector>


struct FormationPoint
{
    Vector3 position = Vector3::Zero();
    Color color = Color::White();
};

struct FormationData
{
    std::string name;
    std::vector<FormationPoint> points;
};
