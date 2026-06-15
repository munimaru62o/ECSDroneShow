// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/DartSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/SeededRandom.h"
#include "Engine/Debug/DebugTypes.h"
#include "Engine/Debug/DebugDrawManager.h"

#include <cmath>

void DartSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& dartArray = coordinator.GetComponentArray<DartComponent>();
    auto& forceArray = coordinator.GetComponentArray<ForceComponent>();

    ParallelFor(totalEntities, [&](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            auto& dart = dartArray.GetData(entity);
            auto& force = forceArray.GetData(entity);

            if (simulationTime > dart.nextDartTime) {
                dart.cachedDirection =  SeededRandom::OnUnitSphere(dart.seed);
                dart.nextDartTime = simulationTime + SeededRandom::Range(dart.seed, dart.intervalMin, dart.intervalMax);
            }
            Vector3 forceDir = dart.cachedDirection * dart.strength;
            force.value += forceDir;

            if (Debug::Config::IsEnabled && (entity % Debug::Config::EntitySamplingInterval == 0)) {
                const auto& transform = coordinator.GetComponent<TransformComponent>(entity);
                DebugDrawManager::GetInstance().AddLine(
                    transform.position,
                    transform.position + forceDir * Debug::Scale::Force,
                    Debug::DrawColor::Force::Dart
                );
            }
        }
    });
}
