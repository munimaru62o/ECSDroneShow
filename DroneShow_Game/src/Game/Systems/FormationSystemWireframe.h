// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
class TimelineManager;

struct FormationComponentWireframe;
struct TargetComponent;
struct DirectionComponent;
struct FormationData;

/**
 * @class FormationSystemWireframe
 * @brief A system that distributes drones along the edges (contours) of a specified geometric formation.
 *
 * This system fetches a list of vertices (`points`) for a requested formation from the
 * TimelineManager. It calculates the line segments (edges) connecting these vertices
 * and determines the target position for each drone using linear interpolation (Lerp).
 *
 * By evenly distributing all active entities across the available edges, the system can
 * seamlessly fill the gaps between vertices. This allows it to render smooth, continuous
 * wireframe shapes (like logos or contours) even if the number of drones significantly
 * exceeds the number of defined vertices.
 *
 * Signature: [DroneComponent, TargetComponent, DirectionComponent, FormationComponentWireframe]
 */
class FormationSystemWireframe : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

    void SetTimelineManager(TimelineManager* manager)
    {
        m_timelineManager = manager;
    }

private:
    void ProcessEntity(int i, const FormationComponentWireframe& formation, TargetComponent& target, DirectionComponent& direction, const FormationData* formationDataPtr, int baseCount, int remainder, int numEntities);

    TimelineManager* m_timelineManager = nullptr;
};
