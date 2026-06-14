// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Physics/RandomVelocityRequestSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/PhysicsComponents.h"
#include "Engine/Math/StatelessRandom.h"

#include <utility>

void RandomVelocityRequestSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    auto& requestArray = coordinator.GetComponentArray<RandomVelocityRequestComponent>();

    // Process only newly added requests (One-shot execution)
    const auto& events = coordinator.GetEvents<RandomVelocityRequestComponent>();
    for (auto entity : events.added) {
        const auto& request = requestArray.GetData(entity);

        // Derive deterministic sub-seeds for direction and speed independently
        uint32_t dirSeed = StatelessRandom::Combine(request.seed, 0);
        uint32_t speedSeed = StatelessRandom::Combine(request.seed, 1);

        // Generate uniform random direction and speed
        const Vector3 randomDir = StatelessRandom::OnUnitSphere(dirSeed);
        float speed = StatelessRandom::Range(speedSeed, request.minSpeed, request.maxSpeed);

        VelocityComponent velocity = { .value = randomDir * speed };

        coordinator.DeferAssignComponent<VelocityComponent>(entity, std::move(velocity));
        coordinator.DeferRemoveComponent<RandomVelocityRequestComponent>(entity);
    }
}
