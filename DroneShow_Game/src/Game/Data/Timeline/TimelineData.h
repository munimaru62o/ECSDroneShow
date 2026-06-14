// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <string>
#include <json.hpp>

struct SequenceData
{
    std::string name;
    float duration;
    nlohmann::json components;
};

struct TimelineData
{
    std::string name;
    std::vector<SequenceData> sequences;
};
