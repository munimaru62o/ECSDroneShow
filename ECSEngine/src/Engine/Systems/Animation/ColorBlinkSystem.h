// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class ColorBlinkSystem
 * @brief An animation system that smoothly interpolates (pulses) between two colors.
 *
 * For all entities possessing a ColorBlinkComponent, this system updates their
 * MaterialComponent's color every frame. It calculates a linear interpolation (Lerp)
 * factor driven by a triangle wave, which is based on the absolute simulation time
 * to ensure consistent animation independent of frame rate fluctuations.
 * 
 * * Signature: [ColorBlinkComponent, MaterialComponent]
 */
class ColorBlinkSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};
