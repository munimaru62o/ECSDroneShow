// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/PlaneConstraintSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/PhysicsComponents.h"

void PlaneConstraintSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& constraintArray = coordinator.GetComponentArray<PlaneConstraintComponent>();
    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& velocityArray = coordinator.GetComponentArray<VelocityComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &constraintArray,
        &transformArray,
        &velocityArray
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& constraint = constraintArray.GetData(entity);
            const auto& transform = transformArray.GetData(entity);
            auto& velocity = velocityArray.GetData(entity);

            // Calculate the signed distance from the entity's position to the mathematical plane.
            // A negative value indicates the entity has penetrated or is behind the plane.
            float signedDistance = Vector3::Dot(transform.position, constraint.normal) - constraint.distance;
            if (signedDistance < 0.0f) {

                // Calculate the velocity component along the plane's normal vector
                float vn = Vector3::Dot(velocity.value, constraint.normal);

                // If the entity is moving INTO the plane (vn < 0), cancel out that inward velocity.
                // Subtracting the normal velocity projects the movement onto the plane, allowing sliding.
                if (vn < 0.0f) {
                    velocity.value -= constraint.normal * vn;
                }
            }
        }
    });
}
