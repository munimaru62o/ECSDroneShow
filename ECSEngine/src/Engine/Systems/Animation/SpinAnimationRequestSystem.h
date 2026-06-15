// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class SpinAnimationRequestSystem
 * @brief An intermediary system that processes spin animation requests and converts them into executable components.
 *
 * For any entity with a SpinAnimationRequestComponent, this system calculates the initial state
 * based on the provided configuration (e.g., random/fixed axis, rotation reset) and generates
 * the active SpinAnimationComponent.
 * Once the conversion is complete, the request component is automatically removed via a deferred command.
 *
 * @note To prevent state corruption (e.g., during formation transitions), set the `resetRotation`
 * flag to true to snap the initial rotation back to identity before spinning.
 *
 * @note **Seed Determinism:** The random seed used for generating the spin speed and axis
 * is provided by the caller via the request component. The system derives distinct sub-seeds
 * using `StatelessRandom::Combine`, ensuring that animations remain completely deterministic.
 *
 * Signature: [SpinAnimationRequestComponent]
 */
class SpinAnimationRequestSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};
