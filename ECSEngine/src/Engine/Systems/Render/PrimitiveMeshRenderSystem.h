// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/Render/RenderTypes.h"

#include <vector>
#include <array>

struct TransformComponent;
struct MaterialComponent;
struct PrimitiveMeshComponent;

/**
 * @class PrimitiveMeshRenderSystem
 * @brief A high-performance rendering system dedicated to drawing built-in primitive shapes using hardware instancing.
 *
 * This system aggregates entities that possess both a PrimitiveMeshComponent and a MaterialComponent.
 * To eliminate CPU bottlenecks, it utilizes a lock-free Scatter-Gather pattern via thread-local buffers,
 * collecting transformation matrices and colors across multiple worker threads simultaneously.
 * * The collected instance data is strictly flattened and batched based on the unique permutation
 * of [Shader Type] x [Primitive Mesh Type]. This guarantees minimal state changes and maximizes
 * GPU throughput through efficient instanced draw calls (e.g., glDrawElementsInstanced).
 *
 * Signature: [TransformComponent, PrimitiveMeshComponent, MaterialComponent]
 */
class PrimitiveMeshRenderSystem : public System
{
public:
    PrimitiveMeshRenderSystem();
    ~PrimitiveMeshRenderSystem() override;

public:
    virtual void Init(Coordinator& coordinator) override;
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    struct ThreadLocalBuffer;

    // The total number of batch permutations (Shader Type x Mesh Type)
    static constexpr size_t NUM_BATCH_TYPES = NUM_SHADERS * NUM_MESH_TYPES;

    // Utility function to convert 2D indices into a flat 1D array index
    static constexpr size_t GetBatchIndex(PixelShaderType shader, PrimitiveMeshType mesh)
    {
        // [Shader][Mesh] -> Flat Index
        return static_cast<size_t>(shader) * NUM_MESH_TYPES + static_cast<size_t>(mesh);
    }

    void ProcessEntity(const TransformComponent& transform, const MaterialComponent& material, const PrimitiveMeshComponent& mesh, ThreadLocalBuffer& localBuffer) const;

    std::vector<ThreadLocalBuffer> m_threadBuffers;
    std::array<MeshID, NUM_MESH_TYPES> m_meshIds;

    // Flattened array to store final merged instance data per Shader-Mesh combination
    std::array<std::vector<InstanceData>, NUM_BATCH_TYPES> m_finalBuffers;
};
