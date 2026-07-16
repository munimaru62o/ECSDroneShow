// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/Debug/DebugTypes.h"

#include <vector>
#include <mutex>

struct Vector3;
struct Matrix4;
struct Color;
class Camera;

/**
 * @class DebugDrawManager
 * @brief Stores and renders debug primitives for the current frame.
 *
 * Debug primitives can be safely queued from multiple threads during simulation.
 * At the beginning of the render phase, the queued primitives are swapped into a
 * render buffer and rendered. The render buffer is cleared at the end of the frame.
 */
class DebugDrawManager final : private NonCopyable
{
public:
    DebugDrawManager();
    ~DebugDrawManager();

    void BeginFrame();
    void EndFrame();

    void Render(const Camera& camera);
    void AddLine(const Vector3& start, const Vector3& end, const Color& color);

private:
    std::vector<Debug::Line> m_lines;
    std::vector<Debug::Line> m_linesToDraw;
    std::mutex m_mutex;
};
