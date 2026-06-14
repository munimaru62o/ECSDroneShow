// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"

class Coordinator;

/**
 * @class SceneLightingSystem
 * @brief A one-shot system that applies global lighting configurations from the SceneEntity to the renderer.
 *
 * This system monitors for the addition of a SceneLightingComponent. Upon detection,
 * it extracts the specified lighting parameters (ambient light, main light direction,
 * primary light color, and fill color) and forwards them to the RenderManager.
 *
 * Because this component acts as a transitional request—often driven by Timelines or
 * Sequences to enable data-driven time-of-day changes or scene transitions—it is
 * automatically safely removed via a deferred command once the renderer is updated.
 *
 * Signature: [SceneLightingComponent]
 */
class SceneLightingSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;
};

