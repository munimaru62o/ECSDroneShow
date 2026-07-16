// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"
#include "Engine/ECS/ECSTypes.h"

class Camera;
struct Vector3;
struct Color;

/**
 * @class DebugManager
 * @brief Manages all runtime debug features.
 *
 * This class owns debug settings and coordinates debug-related systems such as
 * debug drawing and overlays. It provides a single entry point for updating
 * debug input and rendering debug information.
 */
class DebugManager final : private NonCopyable
{
public:
    DebugManager();
    ~DebugManager();

    void BeginFrame();
    void EndFrame();

    void Render(const Camera& camera);

    void DrawLine(const Vector3& start, const Vector3& end, const Color& color);
    void DrawForce(Entity entity, const Vector3& position, const Vector3& force, const Color& color);

    [[nodiscard]] DebugDrawManager& GetDrawManager() { return m_drawManager; };

    void SetDraw3DEnabled(bool enabled) { m_settings.draw3DVisible = enabled; };
    void SetOverlayEnabled(bool enabled) { m_settings.overlayVisible = enabled; };

    [[nodiscard]] bool IsDraw3DEnabled() const { return m_settings.draw3DVisible; };
    [[nodiscard]] bool IsOverlayEnabled() const { return m_settings.overlayVisible; };

private:
    DebugSettings m_settings;
    DebugDrawManager m_drawManager;
};
