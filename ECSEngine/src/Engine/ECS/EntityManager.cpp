// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/ECS/EntityManager.h"

#include <queue>
#include <array>
#include <cassert>


EntityManager::EntityManager()
{
    // Initialize the queue with all possible Entity IDs
    for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
        m_availableEntities.push(entity);
    }
}
EntityManager::~EntityManager() = default;

Entity EntityManager::CreateEntity()
{
    assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

    // Take an available ID from the front of the queue
    Entity id = m_availableEntities.front();
    m_availableEntities.pop();
    ++m_livingEntityCount;

    return id;
}

void EntityManager::DestroyEntity(Entity entity)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");

    // Reset the Entity's signature
    m_signatures[entity].reset();

    // Put the destroyed ID at the back of the queue to be reused later
    m_availableEntities.push(entity);
    --m_livingEntityCount;
}

void EntityManager::SetSignature(Entity entity, Signature signature)
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    m_signatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity) const
{
    assert(entity < MAX_ENTITIES && "Entity out of range.");
    return m_signatures[entity];
}
