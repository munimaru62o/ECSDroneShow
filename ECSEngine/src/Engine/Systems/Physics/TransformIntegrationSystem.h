// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TransformComponent;
struct VelocityComponent;

/**
 * @class TransformIntegrationSystem
 * @brief A system that integrates an entity's velocity over time to update its final spatial position.
 *
 * This system serves as the culmination of the entire physics and steering pipeline.
 * It performs standard semi-implicit Euler integration using the delta time (`dt`) and
 * the final velocity vector (`velocity.value`)—which has been meticulously accumulated,
 * modified, and constrained by all preceding systems—to seamlessly advance the entity's
 * position in the 3D world.
 *
 * Signature: [TransformComponent, VelocityComponent]
 */
class TransformIntegrationSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(TransformComponent& transform, const VelocityComponent& velocity, float dt) const;
};

