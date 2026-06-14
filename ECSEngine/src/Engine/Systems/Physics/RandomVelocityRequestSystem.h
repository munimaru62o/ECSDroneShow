// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class RandomVelocityRequestSystem
 * @brief A one-shot system that applies a randomized initial velocity to entities upon request.
 *
 * This system triggers exactly once for any entity that receives a RandomVelocityRequestComponent.
 * By utilizing uniform spherical sampling, it calculates a completely random 3D direction
 * and scales it by a random speed within the specified [minSpeed, maxSpeed] range.
 *
 * @note This mechanism is highly effective for behaviors and visual effects such as
 * particle explosions, debris scattering, or the initial dispersal of a flock/swarm.
 * Upon completion, the request component is automatically and safely removed via a deferred command.
 *
 * Signature: [RandomVelocityRequestComponent]
 */
class RandomVelocityRequestSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

