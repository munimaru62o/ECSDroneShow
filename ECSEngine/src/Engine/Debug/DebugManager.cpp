// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Debug/DebugManager.h"
#include "Engine/Scene/Camera.h"


DebugManager::DebugManager() = default;
DebugManager::~DebugManager() = default;


void DebugManager::BeginFrame()
{
#ifdef ENABLE_DEBUG
    m_drawManager.BeginFrame();
#endif
}

void DebugManager::EndFrame()
{
#ifdef ENABLE_DEBUG
    m_drawManager.EndFrame();
#endif
}

void DebugManager::Render(const Camera& camera)
{
#ifdef ENABLE_DEBUG
    if (m_settings.draw3DVisible) {
        m_drawManager.Render(camera);
    }
#endif
}

void DebugManager::DrawLine(const Vector3& start, const Vector3& end, const Color& color)
{
#ifdef ENABLE_DEBUG
    m_drawManager.AddLine(start, end, color);
#endif
}

void DebugManager::DrawForce(Entity entity, const Vector3& position, const Vector3& force, const Color& color)
{
#ifdef ENABLE_DEBUG
    if (entity % Debug::Config::EntitySamplingInterval == 0) {
        DrawLine(position, position + force * Debug::Scale::Force, color);
    }
#endif
}

