// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/ECSTypes.h"

#include <bitset>
#include <vector>


template<typename T>
struct ComponentEvents
{
    std::vector<Entity> added = {};
    std::vector<Entity> removed = {};
    std::vector<Entity> updated = {};
    std::bitset<MAX_ENTITIES> dirty = {};
};
