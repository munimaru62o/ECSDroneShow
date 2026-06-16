// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/Math/SimplexNoise.h"

class Coordinator;
struct WanderComponent;
struct ForceComponent;

/**
 * @class WanderSystem
 * @brief A system that generates smooth, continuous forces using Simplex Noise to give entities natural drifting behaviors.
 *
 * Each entity possesses a unique seed (often derived from its Entity ID) used to generate
 * a continuously changing noise vector over time.
 *
 * Unlike white noise (which produces erratic, frame-by-frame jitter), Simplex Noise
 * produces organic, smooth directional changes. This is highly effective for simulating
 * the natural wandering paths of schools of fish, flocks of birds, or floating particles.
 *
 * The `frequency` property in the WanderComponent controls the rate of change,
 * while `strength` controls the magnitude of the applied force.
 *
 * Signature: [ForceComponent, WanderComponent]
 */
class WanderSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

private:
    void ProcessEntity(Entity entity, Coordinator& coordinator, double simulationTime, WanderComponent& wander, ForceComponent& force);

    SimplexNoise m_noise;
};
