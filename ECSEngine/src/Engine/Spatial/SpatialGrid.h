// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/Math/Vector3.h"

#include <vector>
#include <cmath>
#include <algorithm>

/**
 * @class SpatialGrid
 * @brief Subdivides 3D space into uniform cells to massively accelerate spatial queries (e.g., Boids).
 *
 * This class serves as the foundation for a Counting Sort (Prefix Sum) based spatial partitioning algorithm.
 * It provides O(1) conversions from world coordinates to 3D cell indices and flat 1D indices.
 * To strictly avoid dynamic memory allocation during runtime, all cell counts and offsets are
 * pre-allocated in contiguous 1D arrays, ensuring maximum CPU cache coherence.
 */
class SpatialGrid : private NonCopyable
{
public:
    struct CellCoord
    {
        int x;
        int y;
        int z;
    };

    SpatialGrid(float inCellSize = 100.0f, int inGridSize = 20)
        : m_cellSize(inCellSize)
        , m_gridSize(inGridSize)
    {
        m_halfGridSize = m_gridSize / 2;
        m_invCellSize = 1.0f / m_cellSize;
        m_totalCells = m_gridSize * m_gridSize * m_gridSize;

        m_cellCounts.resize(m_totalCells, 0);
        m_cellOffsets.resize(m_totalCells, 0);
    }
    ~SpatialGrid() = default;

    void Clear()
    {
        std::fill(m_cellCounts.begin(), m_cellCounts.end(), 0);
        std::fill(m_cellOffsets.begin(), m_cellOffsets.end(), 0);
    }

    [[nodiscard]] int GetFlatIndex(const CellCoord& coord) const
    {
        return coord.x + coord.y * m_gridSize + coord.z * m_gridSize * m_gridSize;
    }

    [[nodiscard]] CellCoord GetCellCoordFromPosition(const Vector3& pos) const
    {
        CellCoord coord;
        coord.x = static_cast<int>(std::floor(pos.x * m_invCellSize)) + m_halfGridSize;
        coord.y = static_cast<int>(std::floor(pos.y * m_invCellSize)) + m_halfGridSize;
        coord.z = static_cast<int>(std::floor(pos.z * m_invCellSize)) + m_halfGridSize;

        // Clamp to prevent out-of-bounds access for entities flying outside the grid
        coord.x = std::clamp(coord.x, 0, m_gridSize - 1);
        coord.y = std::clamp(coord.y, 0, m_gridSize - 1);
        coord.z = std::clamp(coord.z, 0, m_gridSize - 1);
        return coord;
    }

    [[nodiscard]] std::vector<int>& GetCellCounts() { return m_cellCounts; }
    [[nodiscard]] std::vector<int>& GetCellOffsets() { return m_cellOffsets; }
    [[nodiscard]] std::vector<Entity>& GetEntities() { return m_entities; }

    [[nodiscard]] float GetCellSize() const { return m_cellSize; }
    [[nodiscard]] int GetGridSize() const { return m_gridSize; }
    [[nodiscard]] int GetTotalCells() const { return m_totalCells; }

private:
    float m_cellSize = 100.0f;
    float m_invCellSize = 0.01f; // 1.0f / 100.0f
    int m_gridSize = 20;
    int m_halfGridSize = 10;     // 20 / 2
    int m_totalCells = 8000;     // 20 * 20 * 20

    std::vector<int> m_cellCounts;
    std::vector<int> m_cellOffsets;
    std::vector<Entity> m_entities;
};
