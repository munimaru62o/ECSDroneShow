// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Render/RenderTypes.h"

#include <cstdint>

/**
 * @namespace PrimitiveMeshFactory
 * @brief A utility namespace responsible for procedurally generating vertex and index data for built-in primitive shapes.
 *
 * All generated geometry is structured as `MeshSourceData`, strictly enforcing a Clockwise (CW)
 * winding order for index buffers to ensure correct backface culling in the rendering pipeline.
 * These primitives are specifically tailored for high-performance, massive hardware instancing.
 */
namespace PrimitiveMeshFactory
{
/**
 * @brief A generic factory function that routes to the appropriate procedural generator.
 * @param type The desired PrimitiveMeshType (e.g., Cube, Sphere).
 * @return The procedurally generated MeshSourceData containing vertices and indices.
 */
[[nodiscard]] MeshSourceData CreateMesh(PrimitiveMeshType type);

/**
 * @brief Generates the geometry for a standard 1x1x1 unit cube centered at the origin.
 * @return The generated MeshSourceData (24 vertices, 36 indices).
 */
[[nodiscard]] MeshSourceData CreateCube();

/**
 * @brief Generates the geometry for a UV sphere centered at the origin.
 * * @param stacks The number of latitudinal subdivisions (from pole to pole).
 * @param slices The number of longitudinal subdivisions (around the equator).
 * @return The generated MeshSourceData.
 *
 * @note The default configuration (4 stacks, 4 slices) is an extreme low-poly setup
 * deliberately chosen to maximize performance when rendering massive swarms (e.g., tens
 * of thousands of boids/drones) via instanced rendering.
 */
[[nodiscard]] MeshSourceData CreateSphere(uint32_t stacks = 4, uint32_t slices = 4);
}
