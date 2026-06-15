// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/ComponentEvents.h"
#include "Engine/ECS/EntitySparseSet.h"

#include <array>
#include <cassert>

/**
 * @class IComponentArray
 * @brief Type erasure interface for ComponentArray.
 *
 * A base class that allows the ComponentManager to uniformly hold and manage
 * various types (T) of ComponentArrays within a single list or map
 * (e.g., std::unordered_map<const char*, IComponentArray*>).
 */
class IComponentArray
{
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
    virtual void ClearEvents() = 0;
};

/**
 * @class ComponentArray
 * @brief Manages component data of a specific type (T) as a contiguous array in memory.
 *
 * Works in tandem with EntitySparseSet to perform a "Swap-and-Pop" on the data (Dense array).
 * This guarantees that no gaps (memory fragmentation) are created in the array,
 * even when entities are repeatedly added or removed.
 * Additionally, addition, removal, and update operations through this class are
 * automatically recorded in ComponentEvents, supporting the construction of
 * reactive Systems (systems that process only when changes occur).
 */
template <typename T>
class ComponentArray final : public IComponentArray, private NonCopyable
{
public:
    ComponentArray()
    {
        m_componentArray.reserve(MAX_ENTITIES);
        events.added.reserve(MAX_ENTITIES);
        events.removed.reserve(MAX_ENTITIES);
    }
    ~ComponentArray() = default;

public:
    template<typename U>
    void InsertData(Entity entity, U&& component)
    {
        assert(!m_entitySet.Has(entity) && "Component added to same entity more than once.");

        m_entitySet.Add(entity);
        m_componentArray.push_back(std::forward<U>(component));
        events.added.push_back(entity);
    }

    void RemoveData(Entity entity)
    {
        assert(m_entitySet.Has(entity) && "Removing non-existent component.");

        size_t indexOfRemoved = m_entitySet.GetIndex(entity);

        m_componentArray[indexOfRemoved] = std::move(m_componentArray.back());
        m_componentArray.pop_back();

        m_entitySet.Remove(entity);
        events.removed.push_back(entity);
    }

    template<typename U>
    void SetData(Entity entity, U&& component)
    {
        m_componentArray[m_entitySet.GetIndex(entity)] = std::forward<U>(component);

        if (!events.dirty.test(entity)) {
            events.dirty.set(entity);
            events.updated.push_back(entity);
        }
    }

    [[nodiscard]] T& GetData(Entity entity)
    {
        assert(m_entitySet.Has(entity) && "Retrieving non-existent component.");
        return m_componentArray[m_entitySet.GetIndex(entity)];
    }

    [[nodiscard]] const T& GetData(Entity entity) const
    {
        assert(m_entitySet.Has(entity) && "Retrieving non-existent component.");
        return m_componentArray[m_entitySet.GetIndex(entity)];
    }

    void EntityDestroyed(Entity entity) override
    {
        if (m_entitySet.Has(entity)) {
            RemoveData(entity);
        }
    }

    [[nodiscard]] const ComponentEvents<T>& GetEvents() const
    {
        return events;
    }

    void ClearEvents() override
    {
        events.added.clear();
        events.removed.clear();
        events.updated.clear();
        events.dirty.reset();
    }

private:
    std::vector<T> m_componentArray;
    EntitySparseSet m_entitySet;
    ComponentEvents<T> events;
};
