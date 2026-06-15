// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"

#include <queue>
#include <array>

/**
 * @class EntityManager
 * @brief Manages the lifecycle of Entities (issuing and destroying IDs) and their Signatures (component composition).
 *
 * An Entity itself is merely an integer ID (TypeAlias) with no actual data; this class manages the pool of these IDs.
 * To prevent ID exhaustion and fragmentation, destroyed IDs are returned to a queue and reused.
 *
 * [Core Responsibilities]
 * - Issuing (Create) and reclaiming (Destroy) unused Entity IDs.
 * - Maintaining and providing the bitmask (Signature) that represents which active Components an Entity currently has.
 */
class EntityManager final : private NonCopyable
{
public:
    EntityManager();
    ~EntityManager();

public:
    [[nodiscard]] Entity CreateEntity();
    void DestroyEntity(Entity entity);

    void SetSignature(Entity entity, Signature signature);
    [[nodiscard]] Signature GetSignature(Entity entity) const;

    [[nodiscard]] uint32_t GetLivingEntityCount() const { return m_livingEntityCount; }

private:
    // Queue of available Entity IDs
    std::queue<Entity> m_availableEntities{};

    // Array to store the Signature of each Entity
    std::array<Signature, MAX_ENTITIES> m_signatures{};

    // Total number of currently living Entities
    uint32_t m_livingEntityCount{};
};
