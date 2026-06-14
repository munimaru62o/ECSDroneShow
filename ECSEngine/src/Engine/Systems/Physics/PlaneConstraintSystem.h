// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class PlaneConstraintSystem
 * @brief A physics system that restricts entities from penetrating a defined mathematical plane.
 *
 * This system calculates the signed distance from each entity to a mathematical plane
 * defined by a normal vector and a distance from the origin.
 * If an entity breaches the plane (signed distance < 0.0) and its velocity is directed
 * into the plane, the system removes the velocity component along the plane's normal.
 * * * [Features]
 * - Allows entities to smoothly slide along invisible walls, floors, or ceilings without passing through.
 * - Executes prior to the position update phase to ensure robust collision response
 * against structural boundaries.
 *
 * Signature: [PlaneConstraintComponent, VelocityComponent, TransformComponent]
 */
class PlaneConstraintSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

