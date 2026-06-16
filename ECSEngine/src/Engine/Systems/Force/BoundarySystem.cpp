// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/BoundarySystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/Constants.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>

void BoundarySystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& boundaryArray = coordinator.GetComponentArray<BoundaryComponent>();

    ParallelFor(totalEntities, [this, &entities, &transformArray, &boundaryArray, &forceArray](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(entity, transformArray.GetData(entity), boundaryArray.GetData(entity), forceArray.GetData(entity));
        }
    });
}


void BoundarySystem::ProcessEntity(Entity entity, const TransformComponent& transform, const BoundaryComponent& boundary, ForceComponent& force)
{
    // Calculate the vector pointing OUTWARD from the boundary center to the entity
    const Vector3 offset = transform.position - boundary.center;
    const float distSq = offset.LengthSq();
    const float radiusSq = boundary.radius * boundary.radius;

    // Only apply force if the entity has breached the spherical boundary
    if (distSq > radiusSq && distSq > MathConstants::ZERO_TOLERANCE) {
        float dist = std::sqrt(distSq);

        // Calculate the penetration depth multiplied by the spring force constant
        float push = (dist - boundary.radius) * boundary.force;

        // (offset / dist) normalizes the vector. 
        // We calculate the outward force direction and magnitude simultaneously.
        Vector3 forceDir = offset / dist * push;

        // Subtracting the outward force effectively pulls the entity back toward the center
        force.value -= forceDir;

        // Debug visualization
        if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
            DebugDrawManager::GetInstance().AddLine(
                transform.position,
                transform.position + forceDir * Debug::Scale::Force,
                Debug::DrawColor::Force::Boundary
            );
        }
    }
}
