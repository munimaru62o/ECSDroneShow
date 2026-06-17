// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Systems/FormationSystemWireframe.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Math/Vector3.h"

#include "Game/Managers/TimelineManager.h"
#include "Game/Components/GameComponents.h"
#include "Game/Data/Timeline/FormationData.h"

#include <memory>
#include <algorithm>


void FormationSystemWireframe::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    if (!m_timelineManager) {
        assert(false && "FormationSystemWireframe: TimelineManager is null. Call SetTimelineManager() before updating.");
        return;
    }

    const auto& entities = GetEntities();
    int numEntities = static_cast<int>(entities.size());
    if (numEntities <= 0) {
        return;
    }

    auto& formationArray = coordinator.GetComponentArray<FormationComponentWireframe>();
    auto& targetArray = coordinator.GetComponentArray<TargetComponent>();
    auto& directionArray = coordinator.GetComponentArray<DirectionComponent>();

    // Assumption: All entities processed here belong to the same formation group.
    // To support multiple simultaneous formations, group-based processing should be considered.
    const auto& firstFormation = formationArray.GetData(entities[0]);

    // Retrieve the shared formation vertex data from the TimelineManager
    const auto& formationDataPtr = m_timelineManager->GetFormationData(firstFormation.file);
    if (!formationDataPtr) {
        assert(false && "FormationSystemWireframe: Formation data not found. Check if the formation file is loaded and the name is correct.");
        return;
    }
    int numPoints = static_cast<int>(formationDataPtr->points.size());
    if (numPoints <= 0) {
        assert(false && "FormationSystemWireframe: Formation data contains no points. The loaded formation might be empty or corrupted.");
        return;
    }

    // Edge case: If the formation is just a single point, collapse all drones to that point
    if (numPoints == 1) {
        ParallelFor(numEntities, [&entities, &targetArray, &formationArray, &formationDataPtr](int startIdx, int endIdx) {
            for (int i = startIdx; i < endIdx; ++i) {
                auto& target = targetArray.GetData(entities[i]);
                target.value = formationDataPtr->points[0].position * formationArray.GetData(entities[i]).scale;
            }
        });
        return;
    }

    // Number of edges to form an open line string
    int numEdges = numPoints - 1;

    // Calculate base distribution and remainders to ensure no edge is left sparse
    int baseCount = numEntities / numEdges;
    int remainder = numEntities % numEdges;

    ParallelFor(numEntities, [this, &entities, &formationArray, &targetArray, &directionArray, formationDataPtr, baseCount, remainder, numEntities](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(i, formationArray.GetData(entity), targetArray.GetData(entity), directionArray.GetData(entity), formationDataPtr, baseCount, remainder, numEntities);
        }
    });
}


void FormationSystemWireframe::ProcessEntity( // NOSONAR (cpp:S107 - ECS architecture requires passing multiple components for entity processing)
    int i,
    const FormationComponentWireframe& formation,
    TargetComponent& target,
    DirectionComponent& direction,
    const FormationData* formationDataPtr,
    int baseCount,
    int remainder,
    int numEntities
) const {
    int edgeIndex = 0;
    int localIdx = 0;
    int currentEdgeCount = 0;

    // Distribute the remainder evenly across the first 'remainder' edges
    int threshold = remainder * (baseCount + 1);
    if (i < threshold) {
        edgeIndex = i / (baseCount + 1);
        localIdx = i % (baseCount + 1);
        currentEdgeCount = baseCount + 1;
    } else {
        int adjustedIdx = i - threshold;
        edgeIndex = remainder + (adjustedIdx / baseCount);
        localIdx = adjustedIdx % baseCount;
        currentEdgeCount = baseCount;
    }

    int indexA = edgeIndex;
    int indexB = edgeIndex + 1;

    const auto& pointA = formationDataPtr->points[indexA];
    const auto& pointB = formationDataPtr->points[indexB];

    // Calculate interpolation factor (t) along the edge segment
    float t = 0.0f;
    if (currentEdgeCount > 1) {
        // Denominator remains currentEdgeCount to prevent drones from overlapping 
        // exactly at the vertex shared with the next edge
        t = static_cast<float>(localIdx) / static_cast<float>(currentEdgeCount);
    }

    // Explicitly snap the very last drone to the terminal vertex
    if (i == numEntities - 1) {
        t = 1.0f;
    }

    const Vector3 edge = pointB.position - pointA.position;

    // Output the tangential direction (facing along the wireframe line)
    direction.value = edge.Normalized();

    // Calculate the scaled target position
    const Vector3 targetPoint = (pointA.position + edge * t) * formation.scale;
    target.value = targetPoint;
}
