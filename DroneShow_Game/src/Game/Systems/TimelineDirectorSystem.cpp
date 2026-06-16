// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Systems/TimelineDirectorSystem.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ComponentArray.h"

#include "Game/Registry/ComponentRegistry.h"
#include "Game/Managers/TimelineManager.h"
#include "Game/Data/Timeline/TimelineData.h"

#include <unordered_map>
#include <functional>
#include <json.hpp>


void TimelineDirectorSystem::Update(Coordinator& coordinator, float dt, double simulationTime)
{
    if (!m_timeline) {
        return;
    }

    const auto& sequences = m_timeline->sequences;
    if (sequences.empty()) {
        return;
    }

    m_timer -= dt;

    while (m_timer <= 0.0f) {
        m_currentIndex = (m_currentIndex + 1) % sequences.size();
        auto& sequence = sequences[m_currentIndex];
        m_timer += sequence.duration;
        SwitchState(coordinator, sequence, simulationTime);
    }
}

void TimelineDirectorSystem::SetRegistry(const ComponentRegistry* registry)
{
    m_registry = registry;
}

void TimelineDirectorSystem::SetTimeline(const TimelineData* timeline)
{
    m_timeline = timeline;
    m_currentIndex = -1;
    m_timer = 0.0f;
}


const SequenceData* TimelineDirectorSystem::GetCurrentSequence() const
{
    if (!m_timeline) {
        return nullptr;
    }
    auto& sequences = m_timeline->sequences;
    if (m_currentIndex < 0 || m_currentIndex >= static_cast<int>(sequences.size())) {
        return nullptr;
    }
    return &sequences[m_currentIndex];
}


void TimelineDirectorSystem::CatchUpNewEntity(Coordinator& coordinator, Entity entity)
{
    // Apply all active, non-one-shot components from the current sequence to the late-joiner
    for (const auto& applier : m_currentAppliers) {
        applier.execute(coordinator, entity, ComponentOperationContext{ .creationTime = m_currentSequenceStartTime });
    }

    const auto& activeEntities = GetEntities();
    if (!activeEntities.empty()) {
        // Use the oldest existing entity (the first in the array) as the state prototype
        Entity prototype = activeEntities[0];
        if (prototype != entity) {
            if (m_registry) {
                m_registry->SyncCatchUpComponents(coordinator, prototype, entity);
            }
        }
    }
}


void TimelineDirectorSystem::SwitchState(Coordinator& coordinator, const SequenceData& sequence, double simulationTime)
{
    assert(m_registry);

    // Record the absolute simulation time at the exact moment the sequence switches
    m_currentSequenceStartTime = simulationTime;

    std::array<std::vector<ComponentAssignOperation>, static_cast<size_t>(SequenceTrait::ApplyTarget::Count)> appliersToRun{};
    std::array<std::vector<ComponentRemoveOperation>, static_cast<size_t>(SequenceTrait::ApplyTarget::Count)> removersToRun{};
    std::vector<std::string> newComponents;
    m_currentAppliers.clear();

    for (auto it = sequence.components.begin(); it != sequence.components.end(); ++it) {
        const std::string& compName = it.key();
        const nlohmann::json& compParams = it.value();

        const auto& descriptor = m_registry->GetDescriptor(compName);
        const SequenceTrait::ApplyTarget target = descriptor.target;
        const auto operation = descriptor.applier(compParams);
        appliersToRun[static_cast<size_t>(target)].push_back(operation);
        if (!descriptor.isOneShot) {
            // Retain non-one-shot appliers for entities that might spawn mid-sequence (CatchUp)
            m_currentAppliers.push_back(operation);
        }
        newComponents.push_back(compName);
    }

    // Identify components that existed in the previous sequence but are missing in the new one
    for (const auto& lastComp : m_lastComponents) {
        if (std::find(newComponents.begin(), newComponents.end(), lastComp) == newComponents.end()) {
            const auto& descriptor = m_registry->GetDescriptor(lastComp);
            const SequenceTrait::ApplyTarget target = descriptor.target;
            removersToRun[static_cast<size_t>(target)].push_back(descriptor.remover());
        }
    }

    auto ExecuteToSystemEntities = [&coordinator](const auto& operations, const auto& context, auto entities) {
        for (auto const& entity : entities) {
            for (const auto& op : operations) {
                op.execute(coordinator, entity, context);
            }
        }
    };
    auto ExecuteToSceneEntity = [&coordinator](const auto& operations, const auto& context) {
        Entity sceneEntity = coordinator.GetSceneEntity();

        for (const auto& op : operations) {
            op.execute(coordinator, sceneEntity, context);
        }
    };

    // 1. Removal Phase: Strip obsolete components first
    ComponentOperationContext context{ .creationTime = m_currentSequenceStartTime };
    ExecuteToSceneEntity(removersToRun[static_cast<size_t>(SequenceTrait::ApplyTarget::Scene)], context);
    ExecuteToSystemEntities(removersToRun[static_cast<size_t>(SequenceTrait::ApplyTarget::System)], context, GetEntities());

    // 2. Addition/Update Phase: Attach or update components for the new sequence
    ExecuteToSceneEntity(appliersToRun[static_cast<size_t>(SequenceTrait::ApplyTarget::Scene)], context);
    ExecuteToSystemEntities(appliersToRun[static_cast<size_t>(SequenceTrait::ApplyTarget::System)], context, GetEntities());

    m_lastComponents = newComponents;
}
