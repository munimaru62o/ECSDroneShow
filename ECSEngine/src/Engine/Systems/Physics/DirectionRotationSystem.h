// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Math/Constants.h"

/**
 * @class DirectionRotationSystem
 * @brief A generic system that translates a target direction vector into smooth entity rotation.
 *
 * This system interprets the vector stored in the DirectionComponent as the desired "Forward"
 * direction and generates a target rotation using `Quaternion::LookRotation`. It then smoothly
 * interpolates the entity's current rotation toward this target via `Quaternion::Slerp`,
 * governed by the `rotateSpeed` defined in the specific `TRotationComponent`.
 *
 * [Architecture Design]
 * This system is intentionally agnostic to the origin of the direction data. It seamlessly
 * processes directions supplied by any external generator (e.g., VelocityDirectionSystem,
 * FormationDirectionSystem, or LookAtDirectionSystem). By decoupling "where to look" (Intention)
 * from "how to rotate" (Execution), it serves as a highly modular and extensible rotation pipeline.
 *
 * @note Because this is a template class, its implementation must remain in the header file
 * to ensure proper instantiation by the compiler.
 *
 * Signature: [TRotationComponent, TransformComponent, DirectionComponent]
 */
template<typename TRotationComponent>
class DirectionRotationSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override
    {
        const auto& entities = GetEntities();
        if (entities.empty()) {
            return;
        }

        auto& rotationArray = coordinator.GetComponentArray<TRotationComponent>();
        auto& directionArray = coordinator.GetComponentArray<DirectionComponent>();
        auto& transformArray = coordinator.GetComponentArray<TransformComponent>();

        ParallelFor(static_cast<int>(entities.size()), [this, dt, &entities, &rotationArray, &directionArray, &transformArray](int startIdx, int endIdx) {
            for (int i = startIdx; i < endIdx; ++i) {
                Entity entity = entities[i];
                ProcessEntity(rotationArray.GetData(entity), directionArray.GetData(entity), transformArray.GetData(entity), dt);
            }
        });
    }

private:
    void ProcessEntity(const TRotationComponent& rotation, const DirectionComponent& direction, TransformComponent& transform, float dt) const
    {
        if (direction.value.LengthSq() > MathConstants::ZERO_TOLERANCE) {
            const Quaternion targetRot = Quaternion::LookRotation(direction.value, Vector3::Up());
            transform.rotation = Quaternion::Slerp(transform.rotation, targetRot, dt * rotation.rotateSpeed);
        }
    }
};
