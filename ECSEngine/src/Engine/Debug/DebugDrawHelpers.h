// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

namespace DebugDraw
{
#ifdef ENABLE_DEBUG_DRAW

inline void Line(const auto& start, const auto& end, const auto& color)
{
    DebugDrawManager::GetInstance().AddLine(start, end, color);
}

inline void Force(const auto& entity, const auto& position, const auto& forceDir, const auto& color)
{
    if ((entity % Debug::Config::EntitySamplingInterval) == 0) {
        Line(position, position + forceDir * Debug::Scale::Force, color);
    }
}

#else

inline void Line(const auto&, const auto&, const auto&) {}
inline void Force(const auto&, const auto&, const auto&, const auto&) {}

#endif
}
