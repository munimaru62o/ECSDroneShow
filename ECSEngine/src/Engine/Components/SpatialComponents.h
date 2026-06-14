// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Spatial/SpatialGrid.h"


struct SpatialPartitionComponent
{
    SpatialGrid::CellCoord coord = {};
    int index = 0;
};
