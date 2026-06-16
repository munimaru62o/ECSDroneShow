// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TransformComponent;
struct BoundaryComponent;
struct ForceComponent;

/**
 * @class BoundarySystem
 * @brief A system that restricts entities to a specified spherical active area.
 *
 * Instead of rigidly clamping coordinates, this system applies a soft restoring force
 * (soft boundary) directed toward the center for any entity that exceeds the configured `radius`.
 * The force magnitude scales linearly with the penetration depth (acting like a spring),
 * enabling organic, smooth U-turns at the borders without conflicting with Boids
 * or other steering systems.
 *
 * Signature: [TransformComponent, ForceComponent, BoundaryComponent]
 */
class BoundarySystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, const TransformComponent& transform, const BoundaryComponent& boundary, ForceComponent& force);
};

