// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class JitterMovementSystem
 * @brief A system that applies a random directional force (noise) to entities every frame.
 *
 * Using uniform random sampling on a sphere, this system generates a completely random 3D
 * direction vector and accumulates a force scaled by the `strength` specified in the
 * JitterMovementComponent into the entity's ForceComponent.
 *
 * @note This is highly effective for simulating micro-jitters (shivering) or erratic,
 * unpredictable flight trajectories similar to flying insects.
 *
 * Signature: [ForceComponent, JitterMovementComponent]
 */
class JitterMovementSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

