// Copyright (c) 2026 munimaru62o. All rights reserved.


#include "Engine/Debug/DebugDrawManager.h"

#include "Engine/Platform/ImGuiConversion.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Color.h"
#include "Engine/Math/Matrix4.h"

#include <imgui.h>


namespace
{
bool ProjectToScreen(const Vector3& pos, const Matrix4& viewProj, int width, int height, ImVec2& outPos)
{
    // Convert to clip space
    float clipX = pos.x * viewProj.m[0][0] + pos.y * viewProj.m[1][0] + pos.z * viewProj.m[2][0] + viewProj.m[3][0];
    float clipY = pos.x * viewProj.m[0][1] + pos.y * viewProj.m[1][1] + pos.z * viewProj.m[2][1] + viewProj.m[3][1];
    float clipW = pos.x * viewProj.m[0][3] + pos.y * viewProj.m[1][3] + pos.z * viewProj.m[2][3] + viewProj.m[3][3];

    if (clipW <= 0.0001f) return false;

    // NDC (Normalized Device Coordinates) 
    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    // Convert to screen space
    outPos.x = (ndcX + 1.0f) * 0.5f * static_cast<float>(width);
    outPos.y = (1.0f - ndcY) * 0.5f * static_cast<float>(height);

    return true;
}
}


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

void DebugDrawManager::RenderAndClear(const Matrix4& viewProj, int screenWidth, int screenHeight)
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
        ImVec2 screenStart, screenEnd;

        if (ProjectToScreen(line.start, viewProj, screenWidth, screenHeight, screenStart) &&
            ProjectToScreen(line.end, viewProj, screenWidth, screenHeight, screenEnd)) {
            ImU32 color = ImGui::GetColorU32(ToImVec4(line.color));
            drawList->AddLine(screenStart, screenEnd, color, 1.0f);
        }
    }

    m_linesToDraw.clear();
}
