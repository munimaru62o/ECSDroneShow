// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class RandomSwirlAxisRequestSystem
 * @brief A one-shot system that calculates a uniformly distributed random swirl axis using a Fibonacci sphere.
 *
 * This system executes exactly once for any entity that receives a RandomSwirlAxisRequestComponent.
 * By utilizing the golden angle and the Fibonacci sphere algorithm, it selects a base axis from a
 * perfectly uniform spherical distribution (eliminating polar bias). It then applies arbitrary
 * noise (jitter) to calculate the final axis and assigns it to the SwirlComponent.
 * * Upon completion, the request component is automatically removed via a deferred command.
 *
 * Signature: [RandomSwirlAxisRequestComponent, SwirlComponent]
 */
class RandomSwirlAxisRequestSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

