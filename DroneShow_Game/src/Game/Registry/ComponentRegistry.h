// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/ECS/ComponentOperationFactory.h"
#include "Engine/ECS/Coordinator.h"
#include "Game/Traits/SequenceTraits.h"
#include "Game/Parsers/JsonParser.h"

/**
 * @class ComponentRegistry
 * @brief Registers components and stores metadata required by the sequence system.
 *
 * In addition to registering ECS component types, this registry manages sequence-related metadata such as:
 *
 * - Data-driven component creation
 * - Component removal operations
 * - Catch-up synchronization behavior
 * - Sequence application targets
 * - One-shot component flags
 *
 * The registry serves as the bridge between ECS component definitions and sequence-driven content.
 */
class ComponentRegistry
{
public:
    using ApplyFunc = std::function<ComponentAssignOperation(const nlohmann::json&)>;
    using RemoverFunc = std::function<ComponentRemoveOperation()>;

    /**
     * @brief Metadata describing how a component is handled by the sequence system.
     *  
     * Each descriptor contains the operations required to create, assign,
     * and remove a component from sequence data, along with additional sequence-specific behavior flags.
     */
    struct ComponentDescriptor
    {
        ApplyFunc applier;
        RemoverFunc remover;
        SequenceTrait::ApplyTarget target = SequenceTrait::ApplyTarget::System;
        bool isOneShot = false;
    };

    /**
     * @brief Registers a component type and its sequence-related metadata.
     *
     * This function performs ECS component registration and optionally configures sequence functionality based on SequenceTraits<T>.
     *
     * Depending on the trait configuration, the component may:
     * - Participate in catch-up synchronization
     * - Support data-driven creation
     * - Define a sequence application target
     * - Be marked as one-shot
     */
    template<typename T>
    void Register(Coordinator& coordinator, const std::string& name)
    {
        coordinator.RegisterComponent<T>();

        if constexpr (SequenceTraits<T>::SyncOnCatchUp) {
            m_catchUpSyncs.push_back(
                [](Coordinator& c, Entity src, Entity dst) {
                if (c.HasComponent<T>(src)) {
                    c.AssignComponent<T>(dst, c.GetComponent<T>(src));
                }
            });
        }

        if constexpr (SequenceTraits<T>::DataDriven) {

            ComponentDescriptor& descriptor = m_descriptors[name];

            descriptor.applier = [this](const nlohmann::json& j) {
                T data = JsonParser::Parse<T>(j);
                return ComponentOperationFactory::CreateAssignOperation<T>(data);
            };

            descriptor.remover = [this]() {
                return ComponentOperationFactory::CreateRemoveOperation<T>();
            };

            descriptor.target = SequenceTraits<T>::Target;
            descriptor.isOneShot = SequenceTraits<T>::OneShot;
        }
    }

    [[nodiscard]] inline const ComponentDescriptor& GetDescriptor(const std::string& name) const
    {
        assert(m_descriptors.find(name) != m_descriptors.end());
        return m_descriptors.at(name);
    }

    /**
     * @brief Synchronizes all registered catch-up components from one entity to another.
     *
     * Components marked with SequenceTraits<T>::SyncOnCatchUp are copied
     * from the source entity to the destination entity if present.
     *
     * This is typically used when a sequence-controlled entity is recreated
     * or joins an already-running sequence.
     */
    inline void SyncCatchUpComponents(Coordinator& coordinator, Entity src, Entity dst) const
    {
        for (const auto& sync : m_catchUpSyncs) {
            sync(coordinator, src, dst);
        }
    }

private:
    std::unordered_map<std::string, ComponentDescriptor> m_descriptors;
    std::vector<std::function<void(Coordinator&, Entity, Entity)>> m_catchUpSyncs;
};
