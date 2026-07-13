// Copyright (c) 2026 munimaru62o. All rights reserved.


#include "Engine/Debug/DebugDrawManager.h"

#include "Engine/Platform/ImGuiConversion.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector2.h"
#include "Engine/Math/Color.h"
#include "Engine/Scene/Camera.h"

#include <imgui.h>


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

void DebugDrawManager::RenderAndClear(const Camera& camera)
{
    if (!Debug::Config::IsEnabled) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_linesToDraw.swap(m_lines);
    }

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    for (const auto& line : m_linesToDraw) {
        Vector2 screenStart, screenEnd;

        if (camera.WorldToScreen(line.start, screenStart) && camera.WorldToScreen(line.end, screenEnd)) {
            ImU32 color = ImGui::GetColorU32(ToImVec4(line.color));
            drawList->AddLine(ToImVec2(screenStart), ToImVec2(screenEnd), color, 1.0f);
        }
    }

    m_linesToDraw.clear();
}
