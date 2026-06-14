// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/ECS/EntitySparseSet.h"

#include <cassert>


EntitySparseSet::EntitySparseSet()
{
    // Initialize the dictionary with invalid IDs
    m_sparse.fill(INVALID_ENTITY);

    // Pre-allocate the maximum capacity to avoid dynamic reallocations
    m_dense.reserve(MAX_ENTITIES);
}
EntitySparseSet::~EntitySparseSet() = default;


void EntitySparseSet::Add(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    if (!Has(entity)) {
        Entity newIndex = static_cast<Entity>(m_dense.size());
        m_sparse[entity] = newIndex;
        m_dense.push_back(entity);
    }
}

void EntitySparseSet::Remove(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    if (Has(entity)) {
        Entity indexOfRemoved = m_sparse[entity];
        Entity indexOfLast = static_cast<Entity>(m_dense.size() - 1);
        Entity lastEntity = m_dense.back();

        // Swap-and-Pop in the dense array
        m_dense[indexOfRemoved] = lastEntity;
        m_dense.pop_back();

        // Update the sparse array (dictionary) mappings
        m_sparse[lastEntity] = indexOfRemoved;
        m_sparse[entity] = INVALID_ENTITY;
    }
}

bool EntitySparseSet::Has(Entity entity) const
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return m_sparse[entity] != INVALID_ENTITY;
}

size_t EntitySparseSet::Size() const
{
    return m_dense.size();
}

Entity EntitySparseSet::GetIndex(Entity entity) const
{
    assert(Has(entity) && "Entity does not exist in set.");
    return m_sparse[entity];
}

const std::vector<Entity>& EntitySparseSet::GetEntities() const
{
    return m_dense;
}

void EntitySparseSet::Clear()
{
    m_dense.clear();
    m_sparse.fill(INVALID_ENTITY);
}
