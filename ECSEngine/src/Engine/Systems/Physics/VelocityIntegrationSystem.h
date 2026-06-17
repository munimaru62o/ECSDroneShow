// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct ForceComponent;
struct VelocityComponent;

/**
 * @class VelocityIntegrationSystem
 * @brief A system that integrates accumulated forces over time to update entity velocity.
 *
 * This system serves as the primary bridge between the Force Accumulator pattern and
 * kinematic motion. It treats the total accumulated force as acceleration (assuming
 * unit mass, m=1) and applies it to the VelocityComponent using the delta time (`dt`).
 *
 * This integration happens after all steering and environmental forces are collected,
 * and before the Damping and Constraint systems are applied to refine the motion.
 *
 * Signature: [ForceComponent, VelocityComponent]
 */
class VelocityIntegrationSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(const ForceComponent& force, VelocityComponent& velocity, float dt) const;
};

