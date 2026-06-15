// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class DampingSystem
 * @brief A system that applies damping (such as air resistance or friction) to an entity's velocity.
 *
 * Every frame, this system multiplies the current velocity by a specified coefficient
 * (`damping.factor`, typically between 0.0 and 1.0) to gradually reduce momentum.
 * It serves two critical purposes: it allows entities to come to a natural, smooth halt
 * when external forces (like Boids or Steering) are removed, and it acts as an essential
 * safety limiter (governor) to prevent entities from accelerating infinitely due to
 * the continuous accumulation of physics forces.
 *
 * Signature: [VelocityComponent, DampingComponent]
 */
class DampingSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

