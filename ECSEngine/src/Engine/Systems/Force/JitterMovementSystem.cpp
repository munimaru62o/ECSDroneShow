// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/JitterMovementSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/SeededRandom.h"

void JitterMovementSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();
    auto& noiseArray = coordinator.GetComponentArray<JitterMovementComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& force = forceArray.GetData(entity);
            auto& noise = noiseArray.GetData(entity);

            // Generate a random direction on the unit sphere.
            // noise.seed is updated in-place via reference, maintaining determinism.
            const Vector3 randomDir = SeededRandom::OnUnitSphere(noise.seed);

            // Accumulate the jitter force
            force.value += randomDir * noise.strength;
        }
    });
}
