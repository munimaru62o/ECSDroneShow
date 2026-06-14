// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once


struct Vector3;
struct Quaternion;

/**
 * @struct Matrix4
 * @brief Represents a 4x4 mathematical matrix, primarily used for 3D affine transformations.
 *
 * This structure provides essential static utility functions to generate standard
 * transformation matrices, including Translation, Rotation (from Quaternions), Scale,
 * and optimized combined TRS (Translation-Rotation-Scale) matrices for 3D spatial data.
 */
struct Matrix4
{
    float m[4][4];

    [[nodiscard]] static Matrix4 Identity();
    [[nodiscard]] static Matrix4 Translation(const Vector3& position);
    [[nodiscard]] static Matrix4 Rotation(const Quaternion& rotation);
    [[nodiscard]] static Matrix4 Scale(const Vector3& scale);

    [[nodiscard]] static Matrix4 TRS(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
};
