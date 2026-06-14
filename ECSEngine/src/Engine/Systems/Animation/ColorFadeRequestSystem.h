// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class ColorFadeRequestSystem
 * @brief An intermediary system that processes color fade requests and converts them into executable components.
 *
 * External gameplay logic only needs to attach a ColorFadeRequestComponent to a target entity.
 * This system detects the addition event (events.added), automatically retrieves the entity's
 * current color from its MaterialComponent, and generates the active ColorFadeComponent
 * responsible for the actual transition. Finally, it removes the request component
 * via a deferred command.
 *
 * Signature: [ColorFadeRequestComponent, MaterialComponent]
 */
class ColorFadeRequestSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};
