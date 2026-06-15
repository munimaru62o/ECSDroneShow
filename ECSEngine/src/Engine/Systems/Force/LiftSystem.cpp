// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/LiftSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Physics/ForceProfile.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>
#include <cassert>

void LiftSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& liftArray = coordinator.GetComponentArray<LiftComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& transform = transformArray.GetData(entity);
            const auto& lift = liftArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            Vector3 offset = lift.center - transform.position;
            float scale = lift.profile.Evaluate(offset);

            float magnitude = lift.strength * scale;
            if (std::abs(magnitude) > lift.maxForce) {
                magnitude = std::copysign(lift.maxForce, magnitude);
            }

            assert(lift.axis.IsNormalized() && "lift.axis is not normalized.");

            Vector3 forceDir = lift.axis * magnitude;
            force.value += forceDir;

            if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
                DebugDrawManager::GetInstance().AddLine(
                    transform.position,
                    transform.position + forceDir * Debug::Scale::Force,
                    Debug::DrawColor::Force::Lift
                );
            }
        }
    });
}
