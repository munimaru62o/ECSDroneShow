// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/EntitySparseSet.h"
#include "Engine/Utils/ThreadPool.h"

#include <vector>
#include <functional>

class Coordinator;

/**
 * @class System
 * @brief The base class for all systems, defining the "logic" (behavior) in the ECS architecture.
 *
 * Each system automatically caches a list of Entities that match its signature
 * (the required component configuration) within an internal EntitySparseSet.
 * Derived classes override the `Update` method to define per-frame processing.
 * They can optionally call the base class's `ParallelFor` to safely and easily
 * execute multi-threaded (data-parallel) operations.
 * * * Note: Control functions utilized by the ECS core are strictly encapsulated
 * and accessible only via friend classes to prevent external mutation.
 */
class System : private NonCopyable
{
public:
    System() = default;
    virtual ~System() = default;

public:
    virtual void Init(Coordinator& coordinator) {}
    virtual void Update(Coordinator& coordinator, float dt, double simulationTime) {}

    inline [[nodiscard]] const std::vector<Entity>& GetEntities() const
    {
        return m_entitySet.GetEntities();
    }

protected:
    template<typename Func>
    void ParallelFor(int totalElements, Func&& task)
    {
        if (totalElements <= 0) {
            return;
        }

        // Fallback to single-threaded execution for safety if no thread pool is configured
        if (!m_threadPool) {
            task(0, totalElements);
            return;
        }
        m_threadPool->ParallelFor(totalElements, std::forward<Func>(task));
    }

private:
    void SetThreadPool(ThreadPool* pool)
    {
        m_threadPool = pool;
    }

    void AddEntity(Entity entity)
    {
        m_entitySet.Add(entity);
    }

    void EraseEntity(Entity entity)
    {
        m_entitySet.Remove(entity);
    }

private:
    friend class SystemManager;
    friend class Coordinator;

    ThreadPool* m_threadPool = nullptr;
    EntitySparseSet m_entitySet;
};
