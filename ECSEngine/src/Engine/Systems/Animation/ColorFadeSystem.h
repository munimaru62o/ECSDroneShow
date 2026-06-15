// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class ColorFadeSystem
 * @brief An animation system that smoothly transitions an entity's color to a target color over a specified duration.
 *
 * This system performs linear interpolation (Lerp) on entities possessing a ColorFadeComponent
 * and updates their MaterialComponent's color accordingly.
 * * [Features]
 * - Auto-Cleanup: Once the fade is complete, the entity automatically strips itself of the
 * ColorFadeComponent. This removal is executed safely within a multi-threaded environment
 * using deferred commands (ThreadCommandContext).
 *
 * Signature: [ColorFadeComponent, MaterialComponent]
 */
class ColorFadeSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};
