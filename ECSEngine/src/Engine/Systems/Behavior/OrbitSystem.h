// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TargetComponent;
struct OrbitComponent;

/**
 * @class OrbitSystem
 * @brief An animation system that makes an entity's target position orbit around a specific axis.
 *
 * For any entity possessing an OrbitComponent, this system calculates the current rotation angle
 * based on absolute simulation time. It then applies Rodrigues' rotation formula to rotate
 * the saved initial position (`basePosition`) around an arbitrary axis.
 * * @note **Zero Drift & Perfect Synchronization:** * Because the calculation is performed using absolute time and the original base position
 * every frame (rather than accumulating deltas), floating-point errors do not accumulate.
 * This also allows entities that start orbiting mid-simulation to perfectly synchronize
 * with the global rotation cycle.
 *
 * Signature: [TargetComponent, OrbitComponent]
 */
class OrbitSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, Coordinator& coordinator, double simulationTime, TargetComponent& target, const OrbitComponent& orbit) const;
};

