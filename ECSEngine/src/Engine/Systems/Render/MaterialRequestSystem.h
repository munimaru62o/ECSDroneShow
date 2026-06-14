// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class MaterialRequestSystem
 * @brief An intermediary one-shot system that processes and applies partial updates to MaterialComponents.
 *
 * External logic (e.g., gameplay scripts, damage effects) only needs to attach a
 * MaterialRequestComponent to an entity to trigger a material change.
 * * This system listens for addition events (events.added) and applies only the requested
 * modifications to the entity's existing MaterialComponent. By utilizing `std::optional`,
 * it allows for granular partial updates—such as changing only the color or only the
 * shader type—while preserving unmodified properties.
 *
 * Upon successful application, the MaterialRequestComponent is automatically and safely
 * removed via a deferred command.
 *
 * Signature: [MaterialRequestComponent, MaterialComponent]
 */
class MaterialRequestSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

