// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/WanderSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/StatelessRandom.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>

void WanderSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& wanderArray = coordinator.GetComponentArray<WanderComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();

    ParallelFor(totalEntities, [
        this,
        &entities,
        &wanderArray,
        &forceArray,
        &coordinator,
        simulationTime
    ](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& wander = wanderArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            // Optimization: Time-slicing. Only compute expensive noise functions at specified intervals.
            if (wander.nextUpdateTime <= simulationTime) {
                float t = static_cast<float>(simulationTime) * wander.frequency;

                // Generate deterministic offsets based on the entity's unique seed
                uint32_t offsetSeedX = StatelessRandom::Combine(wander.seed, 0);
                uint32_t offsetSeedY = StatelessRandom::Combine(wander.seed, 1);

                // Assign sufficiently distant coordinates (-10000.0 to 10000.0) to prevent noise overlap
                float offsetX = StatelessRandom::Range(offsetSeedX, -10000.0f, 10000.0f);
                float offsetY = StatelessRandom::Range(offsetSeedY, -10000.0f, 10000.0f);

                // Sample 3D Simplex Noise for each axis using the time-varied parameter.
                // Offsets are added to Y and Z sampling to ensure independent noise values per axis.
                float nx = m_noise.Noise3D_TimeVaried(offsetX, offsetY, t);
                float ny = m_noise.Noise3D_TimeVaried(offsetX + 100.0f, offsetY + 100.0f, t);
                float nz = m_noise.Noise3D_TimeVaried(offsetX + 200.0f, offsetY + 200.0f, t);

                Vector3 forceDir = Vector3{ nx, ny, nz }.Normalized();
                wander.cachedDirection = forceDir;
                wander.nextUpdateTime = simulationTime + wander.updateInterval * wander.updateIntervalScale;
            }
            // Apply the cached force continuously every frame
            force.value += wander.cachedDirection * wander.strength;

            if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
                const auto& transform = coordinator.GetComponent<TransformComponent>(entity);
                DebugDrawManager::GetInstance().AddLine(
                    transform.position,
                    transform.position + wander.cachedDirection * wander.strength * Debug::Scale::Force,
                    Debug::DrawColor::Force::Wander
                );
            }
        }
    });
}
