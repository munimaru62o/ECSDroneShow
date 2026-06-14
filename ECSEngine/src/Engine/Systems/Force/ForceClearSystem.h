// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class ForceClearSystem
 * @brief A system that resets the applied force of all entities to zero before each frame's physics calculations begin.
 *
 * This system serves as the crucial starting point for the "Force Accumulator" pattern.
 * It must be executed at the very beginning of the physics phase to ensure that
 * subsequent steering and physics systems (e.g., BoidsSystem, AttractionSystem, BoundarySystem)
 * can safely accumulate (+=) their respective forces onto a clean slate.
 *
 * Signature: [ForceComponent]
 */
class ForceClearSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

