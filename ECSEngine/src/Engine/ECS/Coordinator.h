// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/ComponentManager.h"
#include "Engine/ECS/EntityManager.h"
#include "Engine/ECS/SystemManager.h"
#include "Engine/ECS/ThreadCommandContext.h"

#include <memory>
#include <functional>

class ThreadPool;

/**
 * @class Coordinator
 * @brief Central facade and integration point of the ECS architecture.
 *
 * The Coordinator manages and connects the three ECS core managers: EntityManager, ComponentManager, and SystemManager.
 *
 * It acts as a mediator between game code and ECS internals, providing a unified interface for entity creation, component management,
 * system execution, and deferred command processing.
 *
 * Responsibilities:
 * - Initialize and manage ECS core subsystems
 * - Execute systems by phase (Input, Logic, Physics, Render, etc.)
 * - Provide thread-safe deferred component operations
 * - Coordinate entity, component, and system interactions
 */
class Coordinator final : private NonCopyable
{
public:
    Coordinator();
    ~Coordinator();

public:
    void Init();
    void EndFrame();

    // ---------------------------------------------------------
    // Entity Interface
    // ---------------------------------------------------------
    [[nodiscard]] Entity CreateEntity();
    void DestroyEntity(Entity entity);

    [[nodiscard]] Entity GetSceneEntity() const;
    [[nodiscard]] uint32_t GetLivingEntityCount() const;

    // ---------------------------------------------------------
    // Component Interface
    // ---------------------------------------------------------
    template<typename T>
    void RegisterComponent()
    {
        m_componentManager->RegisterComponent<T>();
        m_registeredComponentTypes.push_back(GetComponentType<T>());

        // Register automatic attachment for components that must exist
        // on every gameplay entity.
        if constexpr (ComponentTraits<T>::CoreComponent) {
            m_coreComponentAppliers.push_back([this](Coordinator& c, Entity entity) {
                c.AddComponent<T>(entity, T{});
            });
        }

        m_flushers[GetComponentType<T>()] = [](Coordinator& coordinator, ThreadCommandContext& context, ComponentType type) {
            auto& removes = context.GetBuffer<T>(type).removes;
            for (const auto& remove : removes) {
                coordinator.RemoveComponent<T>(remove.entity);
            }
            removes.clear();
            auto& assigns = context.GetBuffer<T>(type).assigns;
            for (const auto& assign : assigns) {
                coordinator.AssignComponent<T>(assign.entity, assign.component);
            }
            assigns.clear();
        };
    }

    template<typename T>
    void AddComponent(Entity entity, const T& component)
    {
        m_componentManager->AddComponent<T>(entity, component);

        auto signature = m_entityManager->GetSignature(entity);
        signature.set(m_componentManager->GetComponentType<T>(), true);
        m_entityManager->SetSignature(entity, signature);

        m_systemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void AddComponent(Entity entity, T&& component)
    {
        using ComponentT = std::remove_reference_t<T>;
        m_componentManager->AddComponent<ComponentT>(entity, std::forward<T>(component));

        auto signature = m_entityManager->GetSignature(entity);
        signature.set(m_componentManager->GetComponentType<ComponentT>(), true);
        m_entityManager->SetSignature(entity, signature);

        m_systemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void AssignComponent(Entity entity, const T& component)
    {
        if (HasComponent<T>(entity)) {
            SetComponent<T>(entity, component);
        } else {
            AddComponent<T>(entity, component);
        }
    }

    template<typename T>
    void AssignComponent(Entity entity, T&& component)
    {
        using ComponentT = std::remove_reference_t<T>;
        if (HasComponent<ComponentT>(entity)) {
            SetComponent<ComponentT>(entity, std::forward<T>(component));
        } else {
            AddComponent<ComponentT>(entity, std::forward<T>(component));
        }
    }

    template<typename T>
    void SetComponent(Entity entity, const T& component)
    {
        m_componentManager->SetComponent<T>(entity, component);
    }

    template<typename T>
    void SetComponent(Entity entity, T&& component)
    {
        using ComponentT = std::remove_reference_t<T>;
        m_componentManager->SetComponent<ComponentT>(entity, std::forward<T>(component));
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        m_componentManager->RemoveComponent<T>(entity);

        Signature signature = m_entityManager->GetSignature(entity);
        signature.set(m_componentManager->GetComponentType<T>(), false);
        m_entityManager->SetSignature(entity, signature);

        m_systemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponentIfExists(Entity entity)
    {
        if (HasComponent<T>(entity)) {
            RemoveComponent<T>(entity);
        }
    }

    template<typename T>
    [[nodiscard]] bool HasComponent(Entity entity) const
    {
        return m_entityManager->GetSignature(entity).test(GetComponentType<T>());
    }

    template<typename T>
    [[nodiscard]] T& GetComponent(Entity entity)
    {
        return m_componentManager->GetComponent<T>(entity);
    }

    template<typename T>
    [[nodiscard]] ComponentArray<T>& GetComponentArray()
    {
        return m_componentManager->GetComponentArray<T>();
    }

    template<typename T>
    [[nodiscard]] ComponentType GetComponentType() const
    {
        return m_componentManager->GetComponentType<T>();
    }

    template<typename T>
    [[nodiscard]] const ComponentEvents<T>& GetEvents() const
    {
        return m_componentManager->GetEvents<T>();
    }

    /**
     * @brief Apply all registered core components to an entity.
     *
     * Core components are automatically attached to every gameplay entity
     * during creation.
     */
    void ApplyCoreComponents(Entity entity);

    // ---------------------------------------------------------
    // System Interface
    // ---------------------------------------------------------
    template<typename T>
    T* RegisterSystem(Signature signature, SystemPhase phase, int order)
    {
        T* system = m_systemManager->RegisterSystem<T>(signature, phase, order);

        if (m_ThreadPool) {
            system->SetThreadPool(m_ThreadPool.get());
        }
        return system;
    }

    template<typename T>
    T* GetSystem()
    {
        return m_systemManager->GetSystem<T>();
    }

    void InitSystems();
    void UpdatePhase(SystemPhase phase, float dt, double simulationTime);

    // ---------------------------------------------------------
    // Command Interface
    // ---------------------------------------------------------
    [[nodiscard]] int GetWorkerThreadCount() const;

    template<typename T>
    void DeferAssignComponent(Entity entity, T component)
    {
        ThreadCommandContext& context = GetCommandContext();
        context.GetBuffer<T>(GetComponentType<T>()).assigns.push_back({ entity, std::move(component) });
    }

    template<typename T>
    void DeferRemoveComponent(Entity entity)
    {
        ThreadCommandContext& context = GetCommandContext();
        context.GetBuffer<T>(GetComponentType<T>()).removes.push_back({ entity });
    }

private:
    [[nodiscard]] ThreadCommandContext& GetCommandContext();

    void FlushCommands();
    void FlushWorkerCommands();
    void FlushMainThreadCommands();
    void FlushCommandContext(ThreadCommandContext& context);

private:
    std::unique_ptr<ThreadPool> m_ThreadPool;
    std::unique_ptr<EntityManager> m_entityManager;
    std::unique_ptr<ComponentManager> m_componentManager;
    std::unique_ptr<SystemManager> m_systemManager;

    Entity m_sceneEntity = 0;

    std::vector<ComponentType> m_registeredComponentTypes;

    ThreadCommandContext m_mainThreadCommandContext;
    std::vector<ThreadCommandContext> m_workerThreadCommandContexts;
    std::array<std::function<void(Coordinator&, ThreadCommandContext&, ComponentType)>, MAX_COMPONENTS> m_flushers;

    /**
     * Functions that automatically attach mandatory core components
     * to newly created gameplay entities.
     */
    std::vector<std::function<void(Coordinator&, Entity)>> m_coreComponentAppliers;
};
