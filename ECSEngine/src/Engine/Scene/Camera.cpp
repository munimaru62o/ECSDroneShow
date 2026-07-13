// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Scene/Camera.h"

#include "Engine/Math/Matrix4.h"
#include "Engine/Math/Vector2.h"

#include <cmath>


Camera::Camera()
{
    UpdateMatrices();
}

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

void Camera::SetPerspective(float fov, float aspect, float nearClip, float farClip)
{
    m_fov = fov;
    m_aspect = aspect;
    m_nearClip = nearClip;
    m_farClip = farClip;
    m_dirty = true;
}

void Camera::UpdateMatrices()
{
    m_view = Matrix4::LookAt(
        m_position,
        m_position + m_rotation.Forward(),
        Vector3::Up());

    m_projection = Matrix4::Perspective(
        m_fov,
        m_aspect,
        m_nearClip,
        m_farClip);

    m_viewProjection = m_view * m_projection;
}

void Camera::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    m_position = eye;
    m_rotation = Quaternion::LookRotation(target - eye, up);
    m_dirty = true;
}

bool Camera::WorldToScreen(const Vector3& world, int width, int height, Vector2& screen) const
{
    float clipX =
        world.x * m_viewProjection.m[0][0] +
        world.y * m_viewProjection.m[1][0] +
        world.z * m_viewProjection.m[2][0] +
        m_viewProjection.m[3][0];

    float clipY =
        world.x * m_viewProjection.m[0][1] +
        world.y * m_viewProjection.m[1][1] +
        world.z * m_viewProjection.m[2][1] +
        m_viewProjection.m[3][1];

    float clipW =
        world.x * m_viewProjection.m[0][3] +
        world.y * m_viewProjection.m[1][3] +
        world.z * m_viewProjection.m[2][3] +
        m_viewProjection.m[3][3];

    if (clipW <= 0.0001f) {
        return false;
    }

    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    screen.x = (ndcX + 1.0f) * 0.5f * static_cast<float>(width);
    screen.y = (1.0f - ndcY) * 0.5f * static_cast<float>(height);

    return true;
}

const Matrix4& Camera::GetViewMatrix() const
{
    return m_view;
}

const Matrix4& Camera::GetProjectionMatrix() const
{
    return m_projection;
}

const Matrix4& Camera::GetViewProjectionMatrix() const
{
    if (m_dirty) {
        UpdateMatrices();
    }
    return m_viewProjection;
}
