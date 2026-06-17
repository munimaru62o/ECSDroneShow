// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct GravityComponent;
struct ForceComponent;

/**
 * @class GravitySystem
 * @brief A system that applies a continuous, directional gravitational force to entities.
 *
 * For each entity possessing a GravityComponent, this system calculates a constant environmental
 * force based on the specified `direction` and `strength`, and accumulates it into the ForceComponent.
 * This system operates independently of an entity's position or mass, applying a uniform
 * pull (e.g., standard downward gravity or wind currents) every frame.
 *
 * Signature: [ForceComponent, GravityComponent]
 */
class GravitySystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(const GravityComponent& gravity, ForceComponent& force) const;
};

