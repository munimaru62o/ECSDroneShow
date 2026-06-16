// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TransformComponent;
struct LiftComponent;
struct ForceComponent;

/**
 * @class LiftSystem
 * @brief A system that applies a localized, directional lifting force (e.g., updrafts or buoyancy) to entities.
 *
 * This system calculates a force along a specific axis (lift.axis) based on the entity's
 * distance from a designated center point. The force magnitude is dynamically scaled
 * using a ForceProfile and clamped to a maximum limit (maxForce) to ensure stable physics.
 * * * [Use Cases]
 * - Thermal updrafts that push entities upward when they enter a specific zone.
 * - Anti-gravity or levitation fields.
 * - Localized wind currents affecting flight trajectories.
 *
 * Signature: [TransformComponent, ForceComponent, LiftComponent]
 */
class LiftSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, const TransformComponent& transform, const LiftComponent& lift, ForceComponent& force);
};

