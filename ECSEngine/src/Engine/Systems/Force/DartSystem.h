// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct DartComponent;
struct ForceComponent;

/**
 * @class DartSystem
 * @brief A system that applies sudden, sharp changes in direction (darting) to entities at specific intervals.
 *
 * Each entity selects a new random direction at randomized time intervals and continuously
 * applies force in that direction until the next update tick.
 *
 * While the WanderSystem represents continuous, smooth fluctuations, the DartSystem simulates
 * the sudden, erratic turns and course corrections seen in schools of fish or flocks of birds,
 * often acting as a catalyst for the entire flock to change direction.
 *
 * The time until the next darting event is randomized between `intervalMin` and `intervalMax`,
 * and the impact of the turn is controlled by `strength`.
 *
 * Signature: [ForceComponent, DartComponent]
 */
class DartSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, Coordinator& coordinator, double simulationTime, DartComponent& dart, ForceComponent& force);
};

