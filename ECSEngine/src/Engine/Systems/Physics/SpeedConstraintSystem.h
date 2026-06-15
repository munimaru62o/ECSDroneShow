// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class SpeedConstraintSystem
 * @brief A physics system that clamps an entity's velocity within a specified minimum and maximum range.
 *
 * Executed toward the end of the physics pipeline (prior to position integration),
 * this system serves as a crucial safeguard against infinite acceleration (physics explosions)
 * caused by accumulated forces. Additionally, by enforcing a minimum speed, it prevents
 * flocking entities from unnaturally halting or becoming stagnant, ensuring a continuously
 * dynamic and organic swarm animation.
 *
 * Signature: [SpeedConstraintComponent, VelocityComponent]
 */
class SpeedConstraintSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

