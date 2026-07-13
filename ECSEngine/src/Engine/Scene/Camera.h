// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

struct Matrix4;
struct Vector2;
struct Quaternion;

class Camera
{
public:
    Camera();

    void SetPosition(const Vector3& position);
    void SetRotation(const Quaternion& rotation);
    void SetPerspective(float fov, float aspect, float nearClip, float farClip);

    void UpdateMatrices();

    void LookAt(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3::Up());
    bool WorldToScreen(const Vector3& world, int width, int height, Vector2& screen) const;

    [[nodiscard]] const Vector3& GetPosition() const { return m_position; };
    [[nodiscard]] const Quaternion& GetRotation() const { return m_rotation; };

    [[nodiscard]] const Matrix4& GetViewMatrix() const;
    [[nodiscard]] const Matrix4& GetProjectionMatrix() const;
    [[nodiscard]] const Matrix4& GetViewProjectionMatrix() const;

private:
    bool m_dirty = true;

    // Transform
    Vector3 m_position;
    Quaternion m_rotation;

    // Projection
    float m_fov = 45.0f;
    float m_aspect = 16.0f / 9.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 10000.0f;

    // Cached matrices
    Matrix4 m_view;
    Matrix4 m_projection;
    Matrix4 m_viewProjection;
};
