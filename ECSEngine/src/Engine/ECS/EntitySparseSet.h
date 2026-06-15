// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"

#include <vector>
#include <array>

/**
 * @class EntitySparseSet
 * @brief A data structure that implements the Sparse-Dense Set pattern to provide O(1) addition, removal, and lookup.
 *
 * This serves as the foundation for guaranteeing contiguous memory layout for Components in Data-Oriented Design (DOD).
 * It consists of two internal arrays:
 * - Sparse Array: A dictionary for O(1) lookups, using the Entity ID as the index to store its corresponding index in the Dense array.
 * - Dense Array: A packed vector that stores existing Entity IDs contiguously without any gaps.
 *
 * Upon removal, it utilizes the "Swap-and-Pop" technique (moving the last element in the Dense array to the deleted position).
 * This prevents memory fragmentation (gaps) and maximizes CPU cache hit rates.
 */
class EntitySparseSet final : private NonCopyable
{
public:
    EntitySparseSet();
    ~EntitySparseSet();

public:
    void Add(Entity entity);
    void Remove(Entity entity);

    [[nodiscard]] bool Has(Entity entity) const;
    [[nodiscard]] size_t Size() const;
    [[nodiscard]] Entity GetIndex(Entity entity) const;
    [[nodiscard]] const std::vector<Entity>& GetEntities() const;

    void Clear();

private:
    std::vector<Entity> m_dense;
    std::array<Entity, MAX_ENTITIES> m_sparse;
};
