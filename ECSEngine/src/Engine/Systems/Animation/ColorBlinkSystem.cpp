// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Systems/Animation/ColorBlinkSystem.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Components/CoreComponents.h"
#include "Engine/Components/AnimationComponents.h"

#include <cmath>

void ColorBlinkSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    const auto& entities = GetEntities();
    int totalEntities = static_cast<int>(entities.size());
    if (totalEntities == 0) {
        return;
    }

    auto& blinkArray = coordinator.GetComponentArray<ColorBlinkComponent>();
    auto& materialArray = coordinator.GetComponentArray<MaterialComponent>();

    ParallelFor(totalEntities, [this, simulationTime , &entities, &blinkArray, &materialArray](int startIdx, int endIdx) {
        for (int i = startIdx; i < endIdx; ++i) {
            Entity entity = entities[i];
            ProcessEntity(entity, simulationTime, materialArray.GetData(entity), blinkArray.GetData(entity));
        }
    });
}


inline void ColorBlinkSystem::ProcessEntity(Entity entity, double simulationTime, MaterialComponent& material, const ColorBlinkComponent& blink) const
{
    const float elapsedTime = static_cast<float>(simulationTime - blink.creationTime - blink.delayTime);
    if (elapsedTime <= 0.0f) {
        return;
    }

    // Generate a highly optimized triangle wave oscillating between 0.0 and 1.0.
    // Using fmod creates a 0.0 to 2.0 cycle. Shifting by 1.0 and taking 
    // the absolute value forms the back-and-forth V-shaped wave.
    float cycle = std::fmod(elapsedTime * blink.speed, 2.0f);
    float t = 1.0f - std::abs(cycle - 1.0f);

    material.color = Color::Lerp(blink.color1, blink.color2, t);
}
