// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <json.hpp>


namespace JsonParser
{
template<typename T>
inline T Parse(const nlohmann::json& j)
{
    if (j.is_null() || j.empty()) {
        return T{};
    }
    return j.get<T>();
}
}
