// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#ifdef ENABLE_DEBUG_DRAW

#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#define DEBUG_DRAW_LINE(start, end, color)                                  \
    do {                                                                    \
        DebugDrawManager::GetInstance().AddLine((start), (end), (color));   \
    } while (false)

#define DEBUG_DRAW_FORCE(entity, position, forceDir, color)                 \
    do {                                                                    \
        if (((entity) % Debug::Config::EntitySamplingInterval) == 0) {      \
            DEBUG_DRAW_LINE(                                                \
                (position),                                                 \
                (position) + (forceDir) * Debug::Scale::Force,              \
                (color));                                                   \
        }                                                                   \
    } while (false)

#else

#define DEBUG_DRAW_LINE(...)  do { } while (false)
#define DEBUG_DRAW_FORCE(...) do { } while (false)

#endif
