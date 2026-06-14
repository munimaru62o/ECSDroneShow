// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/AttractionSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Physics/ForceProfile.h"
#include "Engine/Physics/ForcePlane.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>

void AttractionSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& attractionArray = coordinator.GetComponentArray<AttractionComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            const auto& transform = transformArray.GetData(entity);
            const auto& attraction = attractionArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            // 1. Calculate the vector pointing from the entity to the attraction center
            Vector3 toCenter = attraction.center - transform.position;

            // 2. Evaluate the distance-based scaling factor via the configured Falloff Profile
            float scale = attraction.profile.Evaluate(toCenter);

            // 3. Determine final magnitude and clamp it to avoid physics explosions
            float magnitude = attraction.strength * scale;
            if (std::abs(magnitude) > attraction.maxForce) {
                magnitude = std::copysign(attraction.maxForce, magnitude);
            }

            // 4. Project the direction onto the constrained plane (e.g., ignore Y-axis)
            Vector3 planeDir = ForcePlane::Project(toCenter, attraction.plane);
            float dirLenSq = planeDir.LengthSq();

            // 5. Apply the force if the entity is not exactly at the center
            if (dirLenSq > MathConstants::ZERO_TOLERANCE) {
                float invDist = 1.0f / std::sqrt(dirLenSq); 
                Vector3 forceDir = planeDir * (invDist * magnitude);
                force.value += forceDir;

                // 6. Optional: Debug visualization for sampling entities
                if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
                    DebugDrawManager::GetInstance().AddLine(
                        transform.position,
                        transform.position + forceDir * Debug::Scale::Force,
                        Debug::DrawColor::Force::Attraction
                    );
                }
            }
        }
    });
}
