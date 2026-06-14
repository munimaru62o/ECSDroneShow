// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <string>


struct DroneComponent {};

struct FormationComponentWireframe
{
    float scale = 300.0f;
    std::string file = "";
};

struct FormationComponentPointCloud
{
    float scale = 300.0f;
    std::string file = "";
};

struct FormationRotationComponent
{
    float rotateSpeed = 0.0f;
};

