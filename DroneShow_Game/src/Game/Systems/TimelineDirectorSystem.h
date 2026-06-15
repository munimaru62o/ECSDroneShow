// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/ComponentOperationFactory.h"

class Coordinator;
class ComponentRegistry;

struct TimelineData;
struct SequenceData;

/**
 * @class TimelineDirectorSystem
 * @brief A choreographic progression system that dynamically switches components based on a JSON timeline.
 *
 * This system acts as the global "choreographer" for the simulation. It tracks the elapsed time
 * against the duration of the current SequenceData and seamlessly transitions to the next
 * sequence when the time expires (SwitchState).
 *
 * During a transition, it calculates the difference (diff) between the previous and current
 * sequences to automatically remove obsolete components and attach new ones. Furthermore,
 * it features a "CatchUp" (state synchronization) mechanism to instantly bring entities spawned
 * mid-sequence up to speed with the current visual state.
 *
 * Signature: [Target entities depend on the Timeline configuration]
 */
class TimelineDirectorSystem : public System
{
public:
    void Update(Coordinator& coordinator, float dt, double simulationTime) override;

    void CatchUpNewEntity(Coordinator& coordinator, Entity entity);

    void SetRegistry(const ComponentRegistry* registry);
    void SetTimeline(const TimelineData* timeline);
    [[nodiscard]] const SequenceData* GetCurrentSequence() const;

private:
    void SwitchState(Coordinator& coordinator, const SequenceData& sequence, double simulationTime);

private:
    int m_currentIndex = -1;
    float m_timer = 0.0f;
    double m_currentSequenceStartTime = 0.0;

    const TimelineData* m_timeline = nullptr;
    const ComponentRegistry* m_registry = nullptr;

    std::vector<std::string> m_lastComponents;
    std::vector<ComponentAssignOperation> m_currentAppliers;
};
