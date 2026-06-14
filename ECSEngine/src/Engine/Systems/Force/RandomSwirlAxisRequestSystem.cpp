// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Force/RandomSwirlAxisRequestSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/ForceComponents.h"
#include "Engine/Math/StatelessRandom.h"
#include "Engine/Math/Constants.h"

#include <algorithm>
#include <cmath>

void RandomSwirlAxisRequestSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    auto& swirlArray = coordinator.GetComponentArray<SwirlComponent>();
    auto& requestArray = coordinator.GetComponentArray<RandomSwirlAxisRequestComponent>();

    // Process only newly added requests (One-shot execution)
    const auto& events = coordinator.GetEvents<RandomSwirlAxisRequestComponent>();
    for (auto entity : events.added) {
        auto& request = requestArray.GetData(entity);

        // Derive deterministic seeds for selection and noise
        uint32_t selectedSeed = StatelessRandom::Combine(request.seed, 0);
        uint32_t dirSeed = StatelessRandom::Combine(request.seed, 1);

        const int axisCount = (std::max)(1, request.axisCount);
        const int selected = static_cast<int>(StatelessRandom::Range(selectedSeed, 0.0f, static_cast<float>(axisCount)));
        const int safeIndex = (std::clamp)(selected, 0, axisCount - 1);

        // Determine uniform direction using a Fibonacci Sphere
        const float t = static_cast<float>(safeIndex) / static_cast<float>(axisCount);
        const float y = 1.0f - (2.0f * t);

        const float radius = std::sqrt((std::max)(0.0f, 1.0f - y * y));
        const float theta = MathConstants::GOLDEN_ANGLE * static_cast<float>(safeIndex);

        const float x = std::cos(theta) * radius;
        const float z = std::sin(theta) * radius;

        Vector3 axis = { x, y, z };

        // Apply randomized noise (jitter) to the base axis
        const Vector3 noiseDir = StatelessRandom::OnUnitSphere(dirSeed);
        axis += noiseDir * request.noiseStrength;
        
        auto& swirl = swirlArray.GetData(entity);
        swirl.axis = axis.Normalized();

        // Clean up the request component
        coordinator.DeferRemoveComponent<RandomSwirlAxisRequestComponent>(entity);
    }
}
