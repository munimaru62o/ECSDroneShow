// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/SwirlSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Physics/ForceProfile.h"
#include "Engine/Math/Constants.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>
#include <cassert>

void SwirlSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& swirlArray = coordinator.GetComponentArray<SwirlComponent>();

    ParallelFor(totalEntities, [
        &entities,
        &transformArray,
        &forceArray,
        &swirlArray
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& transform = transformArray.GetData(entity);
            const auto& swirl = swirlArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            Vector3 toCenter = swirl.center - transform.position;
            float distSq = toCenter.LengthSq();

            // Only apply force if the entity is not perfectly at the center (prevents zero-division)
            if (distSq > MathConstants::ZERO_TOLERANCE) {
                float scale = swirl.profile.Evaluate(toCenter);

                float magnitude = swirl.strength * scale;
                if (std::abs(magnitude) > swirl.maxForce) {
                    magnitude = std::copysign(swirl.maxForce, magnitude);
                }

                // Normalize the direction to the center
                float invDist = 1.0f / std::sqrt(distSq);
                Vector3 dirToCenter = toCenter * invDist;

                assert(swirl.axis.IsNormalized() && "swirl.axis is not normalized.");

                // Calculate the tangential vector using the cross product:
                // Cross(Inward, Up) = Tangent (Right-hand rule)
                Vector3 forceDir = dirToCenter.Cross(swirl.axis) * magnitude;
                force.value += forceDir;

                if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
                    DebugDrawManager::GetInstance().AddLine(
                        transform.position,
                        transform.position + forceDir * Debug::Scale::Force,
                        Debug::DrawColor::Force::Swirl
                    );
                }
            }
        }
    });
}
