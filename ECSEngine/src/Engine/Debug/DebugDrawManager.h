// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/Debug/DebugTypes.h"

#include <vector>
#include <mutex>

struct Vector3;
struct Color;

/**
 * @class DebugDrawManager
 * @brief A thread-safe manager for rendering debug primitives (e.g., lines, shapes) in the 3D world.
 *
 * This singleton utility allows various systems (AI, Physics, etc.) to queue debug geometry
 * for visualization. It handles thread safety for concurrent additions and automatically
 * clears the draw buffer after each frame rendering, ensuring efficient and safe debugging.
 */
class DebugDrawManager final : private NonCopyable
{
private:
    DebugDrawManager();
    ~DebugDrawManager();

public:
    [[nodiscard]] inline static DebugDrawManager& GetInstance()
    {
        static DebugDrawManager instance;
        return instance;
    }

    void AddLine(const Vector3& start, const Vector3& end, Color color);
    void RenderAndClear();

private:
    std::vector<Debug::Line> m_lines;
    std::vector<Debug::Line> m_linesToDraw;
    std::mutex m_mutex;
};
