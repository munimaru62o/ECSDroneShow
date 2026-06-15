// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class AttractionSystem
 * @brief A force-calculation system that pulls entities toward a specific target coordinate.
 *
 * Based on the parameters in the AttractionComponent, this system calculates a directional
 * pull force and accumulates it into the entity's ForceComponent. The magnitude of this
 * force is dynamically scaled based on the entity's distance from the center (evaluated
 * via ForceProfile) and is clamped to a maximum limit to prevent physics instability.
 *
 * @note This mechanism is ideal for flocking or formation behaviors, as it gently (or rapidly)
 * draws straying entities back to their designated cluster centers. The force can also be
 * constrained to specific axes or planes (e.g., XZ plane only) using the ForcePlane settings.
 *
 * Signature: [TransformComponent, ForceComponent, AttractionComponent]
 */
class AttractionSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

