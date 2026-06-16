// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TransformComponent;
struct SpinAnimationComponent;

/**
 * @class SpinAnimationSystem
 * @brief An animation system that continuously rotates an entity around a specific axis.
 *
 * For any entity possessing a SpinAnimationComponent, this system calculates an axis-angle
 * rotation based on absolute simulation time and applies it to the TransformComponent.
 * Calculating rotation based on absolute time (rather than accumulating delta time)
 * prevents floating-point drift and ensures perfect synchronization.
 * * @note **Auto-Cleanup:** If a duration is specified (> 0.0f), the component is automatically
 * removed via a deferred command once the time expires, safely halting the rotation.
 *
 * Signature: [SpinAnimationComponent, TransformComponent]
 */
class SpinAnimationSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    inline void ProcessEntity(Entity entity, Coordinator& coordinator, double simulationTime, TransformComponent& transform, const SpinAnimationComponent& spin) const;
};
