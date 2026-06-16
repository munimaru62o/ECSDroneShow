// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;
struct TransformComponent;
struct SwirlComponent;
struct ForceComponent;

/**
 * @class SwirlSystem
 * @brief A system that applies a tangential force, causing entities to orbit or swirl around a specific center.
 *
 * Based on the parameters in the SwirlComponent, this system calculates a force perpendicular
 * to both the entity's direction to the center and the specified rotation axis (using the cross product).
 * The magnitude of this tangential force is dynamically scaled by a ForceProfile based on distance.
 * * @note Unlike AttractionSystem which pulls entities directly inward, this system purely
 * generates the rotational velocity needed to simulate dynamic whirlpools, tornados,
 * or circular flocking formations.
 *
 * Signature: [TransformComponent, ForceComponent, SwirlComponent]
 */
class SwirlSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, const TransformComponent& transform, const SwirlComponent& swirl, ForceComponent& force);
};

