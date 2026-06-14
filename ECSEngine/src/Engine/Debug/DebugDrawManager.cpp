// Copyright (c) 2026 munimaru62o. All rights reserved.


#include "Engine/Debug/DebugDrawManager.h"

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"
#include "Engine/Platform/DxLibConversion.h"


DebugDrawManager::DebugDrawManager() = default;
DebugDrawManager::~DebugDrawManager() = default;


void DebugDrawManager::AddLine(const Vector3& start, const Vector3& end, Color color)
{
    if (!Debug::Config::IsEnabled || !Debug::Draw3D::IsVisible) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_lines.push_back({ start, end, color });
}

void DebugDrawManager::RenderAndClear()
{
    if (!Debug::Config::IsEnabled) {
        return;
    }

    for (const auto& line : m_lines) {
        VECTOR p1 = ToDxVec(line.start);
        VECTOR p2 = ToDxVec(line.end);

        DrawLine3D(p1, p2, ToDxColor(line.color));
    }
    m_lines.clear();
}
