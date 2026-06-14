// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "SpinAnimationRequestSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"
#include "Engine/Math/StatelessRandom.h"

#include <utility>

void SpinAnimationRequestSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    auto& requestArray = coordinator.GetComponentArray<SpinAnimationRequestComponent>();
    auto& transformArray = coordinator.GetComponentArray<TransformComponent>();

    const auto& events = coordinator.GetEvents<SpinAnimationRequestComponent>();
    for (auto entity : events.added) {
        const auto& request = requestArray.GetData(entity);
        const auto& transform = transformArray.GetData(entity);

        uint32_t speedSeed = StatelessRandom::Combine(request.seed, 0);
        uint32_t axisSeed = StatelessRandom::Combine(request.seed, 1);
        float speed = StatelessRandom::Range(speedSeed, request.minSpeed, request.maxSpeed);

        SpinAnimationComponent spin = {
            .baseRotation = request.resetRotation ? Quaternion{} : transform.rotation,
            .axis = request.isRandomAxis ? StatelessRandom::OnUnitSphere(axisSeed) : request.axis,
            .creationTime = simulationTime,
            .speed = speed,
            .duration = request.duration
        };

        coordinator.DeferAssignComponent<SpinAnimationComponent>(entity, std::move(spin));
        coordinator.DeferRemoveComponent<SpinAnimationRequestComponent>(entity);
    }
}
