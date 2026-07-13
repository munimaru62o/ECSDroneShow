// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Scene/Camera.h"

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector2.h"

#include <cmath>


Camera::Camera() = default;

void Camera::SetPosition(const Vector3& position)
{
    m_position = position;
    m_dirty = true;
}

void Camera::SetRotation(const Quaternion& rotation)
{
    m_rotation = rotation;
    m_dirty = true;
}

void Camera::SetPerspective(float fov, float nearClip, float farClip)
{
    m_fov = fov;
    m_nearClip = nearClip;
    m_farClip = farClip;
    m_dirty = true;
}

void Camera::SetViewport(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;
    m_dirty = true;
}

void Camera::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    m_position = eye;
    m_rotation = Quaternion::LookRotation(target - eye, up);
    UpdateMatrices();
}

bool Camera::WorldToScreen(const Vector3& world, Vector2& screen) const
{
    EnsureMatricesUpdated();
    const Matrix4& vp = GetViewProjectionMatrix();

    float clipX = world.x * vp.m[0][0] + world.y * vp.m[1][0] + world.z * vp.m[2][0] + vp.m[3][0];
    float clipY = world.x * vp.m[0][1] + world.y * vp.m[1][1] + world.z * vp.m[2][1] + vp.m[3][1];
    float clipW = world.x * vp.m[0][3] + world.y * vp.m[1][3] + world.z * vp.m[2][3] + vp.m[3][3];

    if (clipW <= 0.0001f) {
        return false;
    }

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (ndcX + 1.0f) * 0.5f * m_viewportWidth;
    screen.y = (1.0f - ndcY) * 0.5f * m_viewportHeight;

    return true;
}

const Matrix4& Camera::GetViewMatrix() const
{
    EnsureMatricesUpdated();
    return m_view;
}

const Matrix4& Camera::GetProjectionMatrix() const
{
    EnsureMatricesUpdated();
    return m_projection;
}

const Matrix4& Camera::GetViewProjectionMatrix() const
{
    EnsureMatricesUpdated();
    return m_viewProjection;
}

void Camera::EnsureMatricesUpdated() const
{
    if (m_dirty) {
        UpdateMatrices();
    }
}

void Camera::UpdateMatrices() const
{
    assert(m_viewportWidth > 0);
    assert(m_viewportHeight > 0);

    m_view = Matrix4::LookAt(
        m_position,
        m_position + m_rotation.Forward(),
        Vector3::Up());

    m_projection = Matrix4::Perspective(
        m_fov,
        static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight),
        m_nearClip,
        m_farClip);

    m_viewProjection = m_view * m_projection;
    m_dirty = false;
}
