// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Systems/FormationSystemPointCloud.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Math/Vector3.h"

#include "Game/Managers/TimelineManager.h"
#include "Game/Components/GameComponents.h"
#include "Game/Data/Timeline/FormationData.h"

#include <memory>
#include <algorithm>
#include <cassert>


void FormationSystemPointCloud::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    if (!m_formationManager) {
        assert(false && "FormationSystemPointCloud: TimelineManager is null. Call SetTimelineManager() before updating.");
        return;
    }

    const auto& entities = GetEntities();
    int numEntities = static_cast<int>(entities.size());
    if (numEntities <= 0) {
        return;
    }

    auto& formationArray = coordinator.GetComponentArray<FormationComponentPointCloud>();
    auto& targetArray = coordinator.GetComponentArray<TargetComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    // Assumption: All entities processed here belong to the same formation group.
    const auto& firstFormation = formationArray.GetData(entities[0]);

    // Retrieve the shared formation vertex data from the TimelineManager
    const auto& formationDataPtr = m_formationManager->GetFormationData(firstFormation.file);
    if (!formationDataPtr) {
        assert(false && "FormationSystemPointCloud: Point cloud data not found. Check if the formation file is loaded and the name is correct.");
        return;
    }

    int numPoints = static_cast<int>(formationDataPtr->points.size());
    if (numPoints <= 0) {
        assert(false && "FormationSystemPointCloud: Point cloud data contains no points. The loaded formation might be empty or corrupted.");
        return;
    }

    ParallelFor(numEntities, [this, &entities, &formationArray, &targetArray, &materialArray, formationDataPtr, numEntities, numPoints](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(i, formationArray.GetData(entity), targetArray.GetData(entity), materialArray.GetData(entity), formationDataPtr, numEntities, numPoints);
        }
    });
}


void FormationSystemPointCloud::ProcessEntity(
    int i,
    const FormationComponentPointCloud& formation,
    TargetComponent& target,
    MaterialComponent& material,
    const FormationData* formationDataPtr,
    int numEntities,
    int numPoints
) const {
    int pointIndex = 0;
    if (numEntities < numPoints) {
        // Downsampling: evenly skip points to maintain the overall silhouette
        float step = static_cast<float>(numPoints) / static_cast<float>(numEntities);
        pointIndex = static_cast<int>(i * step);

        if (pointIndex >= numPoints) {
            pointIndex = numPoints - 1;
        }
    } else {
        // Modulo distribution: cycle through all points
        pointIndex = i % numPoints;
    }

    const auto& pt = formationDataPtr->points[pointIndex];

    // Set Target Position
    target.value = pt.position * formation.scale;

    // Set Material Color (Point clouds often retain color data from source images)
    material.color = pt.color;
}
