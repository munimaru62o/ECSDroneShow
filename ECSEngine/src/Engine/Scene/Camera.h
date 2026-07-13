// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Matrix4.h"

struct Vector2;

class Camera
{
public:
    Camera();

    void SetPosition(const Vector3& position);
    void SetRotation(const Quaternion& rotation);
    void SetPerspective(float fov, float nearClip, float farClip);
    void SetViewport(int width, int height);

    void LookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::Up());
    bool WorldToScreen(const Vector3& world, Vector2& screen) const;

    [[nodiscard]] const Vector3& GetPosition() const { return m_position; };
    [[nodiscard]] const Quaternion& GetRotation() const { return m_rotation; };

    [[nodiscard]] const Matrix4& GetViewMatrix() const;
    [[nodiscard]] const Matrix4& GetProjectionMatrix() const;
    [[nodiscard]] const Matrix4& GetViewProjectionMatrix() const;

private:
    void EnsureMatricesUpdated() const;
    void UpdateMatrices() const;

private:
    // Transform
    Vector3 m_position;
    Quaternion m_rotation;

    // Projection
    float m_fov = 45.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 10000.0f;

    // Viewport
    int m_viewportWidth = 0;
    int m_viewportHeight = 0;

    // Cached matrices
    mutable Matrix4 m_view;
    mutable Matrix4 m_projection;
    mutable Matrix4 m_viewProjection;

    mutable bool m_dirty = true;
};
