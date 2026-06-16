// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Render/PrimitiveMeshRenderSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/RenderComponents.h"
#include "Engine/Render/RenderManager.h"
#include "Engine/Render/PrimitiveMeshFactory.h"
#include "Engine/Utils/ThreadPool.h"
#include "Engine/Math/Matrix4.h"

#include <vector>
#include <algorithm>


PrimitiveMeshRenderSystem::PrimitiveMeshRenderSystem() = default;
PrimitiveMeshRenderSystem::~PrimitiveMeshRenderSystem() = default;


struct PrimitiveMeshRenderSystem::ThreadLocalBuffer
{
    // Flattened array for lock-free parallel insertion
    std::array<std::vector<InstanceData>, NUM_BATCH_TYPES> instanceBuffers;

    void Clear()
    {
        for (auto& buffer : instanceBuffers) {
            buffer.clear();
        }
    }
};

void PrimitiveMeshRenderSystem::Init(Coordinator& coordinator)
{
    const int workerCount = coordinator.GetWorkerThreadCount();
    // +1 for the main thread fallback
    m_threadBuffers.resize(workerCount + 1);

    // Pre-allocate memory divided by worker count to prevent mid-frame reallocations
    int reservePerThread = (MAX_ENTITIES / (std::max)(workerCount, 1));

    for (auto& threadBuffer : m_threadBuffers) {
        for (auto& shaderBuffer : threadBuffer.instanceBuffers) {
            shaderBuffer.reserve(reservePerThread);
        }
    }

    for (size_t mesh = 0; mesh < NUM_MESH_TYPES; ++mesh) {
        const MeshSourceData& data = PrimitiveMeshFactory::CreateMesh(static_cast<PrimitiveMeshType>(mesh));
        m_meshIds[mesh] = RenderManager::GetInstance().CreateMesh(data);
    }
}

void PrimitiveMeshRenderSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& meshArray = coordinator.GetComponentArray<PrimitiveMeshComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    // 1. Parallel Collection Phase (Lock-free)
    ParallelFor(totalEntities, [
        this,
        &entities,
        &transformArray,
        &meshArray,
        &materialArray
    ](int startIdx, int endIdx) {
        int workerIndex = ThreadPool::GetCurrentWorkerIndex();
        if (workerIndex < 0) {
            workerIndex = static_cast<int>(m_threadBuffers.size()) - 1;
        }
        auto& localBuffer = m_threadBuffers[workerIndex];

        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& transform = transformArray.GetData(entity);
            const auto& material = materialArray.GetData(entity);
            const auto& mesh = meshArray.GetData(entity);

            // Calculate the flat 1D index for this specific Shader and Mesh combination
            size_t batchIdx = GetBatchIndex(material.shaderType, mesh.type);

            auto& buffer = localBuffer.instanceBuffers[batchIdx];
            buffer.emplace_back();
            InstanceData& instance = buffer.back();

            // Build the transformation matrix and copy the color
            instance.worldMatrix = Matrix4::TRS(transform.position, transform.rotation, mesh.scale);
            instance.color = material.color;
        }
    });

    // 2. Pre-calculate total instances per batch to reserve exact memory
    std::array<size_t, NUM_BATCH_TYPES> totalCounts{};
    for (size_t batch = 0; batch < NUM_BATCH_TYPES; ++batch) {
        for (auto& threadBuffer : m_threadBuffers) {
            totalCounts[batch] += threadBuffer.instanceBuffers[batch].size();
        }
    }

    // 3. Clear and reserve final buffers (Avoiding per-frame dynamic allocations)
    for (size_t batch = 0; batch < NUM_BATCH_TYPES; ++batch) {
        m_finalBuffers[batch].clear();
        m_finalBuffers[batch].reserve(totalCounts[batch]);
    }

    // 4. Merge Thread-Local Buffers into Final Buffers (Gather)
    for (auto& threadBuffer : m_threadBuffers) {
        for (size_t batch = 0; batch < NUM_BATCH_TYPES; ++batch) {
            const auto& src = threadBuffer.instanceBuffers[batch];
            if (!src.empty()) {
                auto& dst = m_finalBuffers[batch];
                dst.insert(dst.end(), src.begin(), src.end());
            }
        }
    }

    // 5. Submission Phase (Decode the flat index back to Shader and Mesh)
    for (size_t shader = 0; shader < NUM_SHADERS; ++shader) {
        PixelShaderType shaderType = static_cast<PixelShaderType>(shader);

        for (size_t mesh = 0; mesh < NUM_MESH_TYPES; ++mesh) {
            PrimitiveMeshType meshType = static_cast<PrimitiveMeshType>(mesh);

            size_t batchIdx = GetBatchIndex(shaderType, meshType);
            const auto& buffer = m_finalBuffers[batchIdx];

            if (!buffer.empty()) {
                RenderManager::GetInstance().DrawInstanced(m_meshIds[mesh], buffer, shaderType);
            }
        }
    }

    // 6. Cleanup Phase
    for (auto& buffer : m_threadBuffers) {
        buffer.Clear();
    }
}
