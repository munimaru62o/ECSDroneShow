// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/ECSTypes.h"

#include <vector>
#include <memory>
#include <array>


template<typename T>
struct DeferredAssignCommand
{
    Entity entity;
    T component;
};

template<typename T>
struct DeferredRemoveCommand
{
    Entity entity;
};

/**
 * @class IDeferredCommandBuffer
 * @brief Base interface for command buffers, enabling type erasure so they can be stored in a unified array.
 */
class IDeferredCommandBuffer
{
public:
    virtual ~IDeferredCommandBuffer() = default;
};

/**
 * @struct ComponentCommandBuffer
 * @brief A type-specific buffer that queues assignment and removal commands for a given component type.
 */
template<typename T>
struct ComponentCommandBuffer : public IDeferredCommandBuffer
{
    std::vector<DeferredAssignCommand<T>> assigns;
    std::vector<DeferredRemoveCommand<T>> removes;
};

/**
 * @class ThreadCommandContext
 * @brief A thread-local container for deferred component operations.
 *
 * During multi-threaded system execution, directly modifying the ECS registry would cause race conditions.
 * To avoid locking overhead, each thread uses its own ThreadCommandContext to queue structural changes lock-free.
 * The Coordinator subsequently flushes all thread contexts safely on the main thread at the end of a phase.
 */
class ThreadCommandContext
{
public:
    template<typename T>
    ComponentCommandBuffer<T>& GetBuffer(ComponentType type)
    {
        auto& buffer = m_buffers[type];
        if (!buffer) {
            buffer = std::make_unique<ComponentCommandBuffer<T>>();
        }
        return static_cast<ComponentCommandBuffer<T>&>(*buffer);
    }

private:
    std::array<std::unique_ptr<IDeferredCommandBuffer>, MAX_COMPONENTS> m_buffers;
};
