// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class VelocityDirectionSystem
 * @brief A system that extracts and generates a normalized direction vector from an entity's velocity.
 *
 * This system normalizes the velocity vector held by the VelocityComponent and writes it
 * to the DirectionComponent.
 *
 * The DirectionComponent serves as a versatile intermediate data structure that can represent
 * various vectors (e.g., movement direction, line of sight, or tangential direction).
 * This specific system is responsible for supplying the "current movement direction".
 *
 * The generated Direction is subsequently consumed by downstream systems, such as the
 * DirectionRotationSystem. To prevent erratic snapping or NaN errors, the direction is
 * not updated if the velocity is infinitesimally small.
 *
 * Signature: [VelocityComponent, DirectionComponent]
 */
class VelocityDirectionSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

