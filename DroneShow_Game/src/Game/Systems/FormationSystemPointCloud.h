// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
class TimelineManager;

/**
 * @class FormationSystemPointCloud
 * @brief A system that directly distributes drones to specific coordinates defined by point cloud data.
 *
 * This system is designed for formations that consist of discrete points without connecting edges
 * (unlike wireframes), such as data generated from 2D images or 3D scans.
 * It gracefully handles mismatches between the number of active drones and the available vertices:
 * - If there are fewer drones than points, it performs uniform downsampling to preserve the overall shape.
 * - If there are more drones than points, it utilizes modulo distribution to evenly stack entities on existing points.
 *
 * Signature: [DroneComponent, TargetComponent, FormationComponentPointCloud, MaterialComponent]
 */
class FormationSystemPointCloud : public System
{
private:
    TimelineManager* m_formationManager = nullptr;

public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

    void SetTimelineManager(TimelineManager* manager)
    {
        m_formationManager = manager;
    }
};
