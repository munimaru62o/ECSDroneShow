// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Render/PrimitiveMeshFactory.h"
#include "Engine/Math/Constants.h"

#include <vector>
#include <cassert>


MeshSourceData PrimitiveMeshFactory::CreateMesh(PrimitiveMeshType type)
{
    switch (type) {
        case PrimitiveMeshType::Cube:
            return CreateCube();
        case PrimitiveMeshType::Sphere:
            return CreateSphere();

        default:
            break;
    }
    assert(false && "Unknown PrimitiveMeshType requested.");
    return MeshSourceData{};
}


MeshSourceData PrimitiveMeshFactory::CreateCube()
{
    MeshSourceData data;

    data.vertices = {
        // --- 1. Front: Z-minus direction ---
        { {-1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f} },
        { { 1.0f,  1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f} },
        { {-1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f} },
        { { 1.0f, -1.0f, -1.0f}, { 0.0f,  0.0f, -1.0f} },

        // --- 2. Back: Z-plus direction ---
        { { 1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f} },
        { {-1.0f,  1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f} },
        { { 1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f} },
        { {-1.0f, -1.0f,  1.0f}, { 0.0f,  0.0f,  1.0f} },

        // --- 3. Top: Y-plus direction ---
        { {-1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f} },
        { { 1.0f,  1.0f,  1.0f}, { 0.0f,  1.0f,  0.0f} },
        { {-1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f} },
        { { 1.0f,  1.0f, -1.0f}, { 0.0f,  1.0f,  0.0f} },

        // --- 4. Bottom: Y-minus direction ---
        { {-1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f} },
        { { 1.0f, -1.0f, -1.0f}, { 0.0f, -1.0f,  0.0f} },
        { {-1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f} },
        { { 1.0f, -1.0f,  1.0f}, { 0.0f, -1.0f,  0.0f} },

        // --- 5. Left: X-minus direction ---
        { {-1.0f,  1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f,  1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f, -1.0f,  1.0f}, {-1.0f,  0.0f,  0.0f} },
        { {-1.0f, -1.0f, -1.0f}, {-1.0f,  0.0f,  0.0f} },

        // --- 6. Right: X-plus direction ---
        { { 1.0f,  1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f} },
        { { 1.0f,  1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f} },
        { { 1.0f, -1.0f, -1.0f}, { 1.0f,  0.0f,  0.0f} },
        { { 1.0f, -1.0f,  1.0f}, { 1.0f,  0.0f,  0.0f} },
    };

    data.indices = {
        // Clockwise order
        0, 1, 2,  2, 1, 3,       // Front
        4, 5, 6,  6, 5, 7,       // Back
        8, 9, 10, 10, 9, 11,     // Top
        12, 13, 14, 14, 13, 15,  // Bottom
        16, 17, 18, 18, 17, 19,  // Left
        20, 21, 22, 22, 21, 23   // Right
    };

    return data;
}


MeshSourceData PrimitiveMeshFactory::CreateSphere(uint32_t stacks, uint32_t slices)
{
    MeshSourceData data;

    // Minimum geometric requirements for a sphere
    if (stacks < 2) stacks = 2;
    if (slices < 3) slices = 3;

    std::vector<Vertex3D>& vertices = data.vertices;
    std::vector<uint32_t>& indices = data.indices;

    const float radius = 1.0f;

    // 1. Vertex Generation
    // North Pole
    vertices.push_back({ {0.0f, radius, 0.0f}, {0.0f, 1.0f, 0.0f} });

    // Intermediate ring vertices
    float phiStep = MathConstants::PI / stacks;
    float thetaStep = MathConstants::TWO_PI / slices;

    for (uint32_t i = 1; i < stacks; ++i) {
        float phi = i * phiStep;

        // Loop until j <= sliceCount to duplicate the vertex at 0 and 360 degrees
        // (Crucial for proper texture coordinate wrapping, though UVs are omitted here)
        for (uint32_t j = 0; j <= slices; ++j) {
            float theta = j * thetaStep;

            vertices.emplace_back();
            Vertex3D& v = vertices.back();

            // Convert spherical coordinates (radius, theta, phi) to Cartesian (X, Y, Z)
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            // For a unit sphere centered at origin, the position vector IS the normal vector
            v.normal = v.position.Normalized();
        }
    }
    // South Pole
    vertices.push_back({ {0.0f, -radius, 0.0f}, {0.0f, -1.0f, 0.0f} });

    // 2. Index Generation (Clockwise winding order to prevent backface culling)

    // Top cap (Triangles connecting the North Pole to the first ring)
    for (uint32_t i = 1; i <= slices; ++i) {
        indices.push_back(0);      // North Pole
        indices.push_back(i + 1);  // Top Left
        indices.push_back(i);      // Top Right
    }

    // Intermediate rings (Dividing each quad into two triangles)
    uint32_t baseIndex = 1;
    uint32_t ringVertexCount = slices + 1;
    for (uint32_t i = 0; i < stacks - 2; ++i) {
        for (uint32_t j = 0; j < slices; ++j) {
            uint32_t top_right = baseIndex + i * ringVertexCount + j;
            uint32_t top_left = baseIndex + i * ringVertexCount + j + 1;
            uint32_t bottom_right = baseIndex + (i + 1) * ringVertexCount + j;
            uint32_t bottom_left = baseIndex + (i + 1) * ringVertexCount + j + 1;

            // First Triangle
            indices.push_back(top_right);
            indices.push_back(bottom_right);
            indices.push_back(bottom_left);

            // Second Triangle
            indices.push_back(top_right);
            indices.push_back(bottom_left);
            indices.push_back(top_left);
        }
    }

    // Bottom cap (Triangles connecting the last ring to the South Pole)
    uint32_t southPoleIndex = static_cast<uint32_t>(vertices.size() - 1);
    baseIndex = southPoleIndex - ringVertexCount;

    for (uint32_t i = 0; i < slices; ++i) {
        indices.push_back(southPoleIndex);    // South Pole
        indices.push_back(baseIndex + i);     // Top Right
        indices.push_back(baseIndex + i + 1); // Top Left
    }

    return data;
}
