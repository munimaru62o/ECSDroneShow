// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TargetComponent;
struct TransformComponent;
struct SteeringComponent;
struct VelocityComponent;

/**
 * @class SteeringSystem
 * @brief A physics-based spring-damper system that smoothly steers entities toward their target positions.
 *
 * This system calculates an attraction force (Hooke's Law) toward the TargetComponent's position,
 * combined with a velocity-dependent resistance (Damper), and updates the VelocityComponent accordingly.
 * * By tuning the `stiffness` and `dampingRatio` in the SteeringComponent, developers can achieve
 * organic and natural entity formation movements—ranging from under-damped (slightly bouncy/elastic arrivals)
 * to critically damped (stopping perfectly at the target without overshooting).
 *
 * Signature: [TransformComponent, TargetComponent, SteeringComponent, VelocityComponent]
 */
class SteeringSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(const TargetComponent& target, const TransformComponent& transform, const SteeringComponent& steering, VelocityComponent& velocity, float dt);
};

