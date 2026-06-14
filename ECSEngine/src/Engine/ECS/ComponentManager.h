// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/ComponentEvents.h"
#include "Engine/ECS/ComponentArray.h"
#include "Engine/Traits/ComponentTraits.h"

#include <unordered_map>
#include <memory>
#include <cassert>

/**
 * @class ComponentTypeManager
 * @brief A utility that assigns a unique, contiguous integer ID (ComponentType) to each component type.
 *
 * By leveraging the behavior of static local variables within C++ template functions,
 * this manager achieves O(1) type ID resolution at compile/runtime initialization
 * without relying on Run-Time Type Information (RTTI).
 */
class ComponentTypeManager
{
public:
    ComponentTypeManager() = delete;

private:
    static inline ComponentType s_nextTypeId = 0;

public:
    template <typename T>
    [[nodiscard]] static ComponentType GetTypeId()
    {
        // Since a static variable within a template function is instantiated exactly once per type T,
        // s_nextTypeId++ is only evaluated the first time this function is called for a given type.
        // Subsequent calls will return the cached ID.
        static const ComponentType typeId = s_nextTypeId++;

        assert(typeId < MAX_COMPONENTS && "Too many components registered!");

        return typeId;
    }
};

/**
 * @class ComponentManager
 * @brief A centralized manager that orchestrates all ComponentArrays, handling the addition, removal, and retrieval of components for Entities.
 *
 * It stores ComponentArrays of various types within a `std::array` as pointers to a common base class (IComponentArray),
 * employing a Type Erasure pattern. When accessing data, it safely and efficiently downcasts to the correct type in O(1) time
 * using the unique ID provided by the ComponentTypeManager.
 */
class ComponentManager final : private NonCopyable
{
public:
    ComponentManager() = default;
    ~ComponentManager() = default;

public:
    template<typename T>
    void RegisterComponent()
    {
        ComponentType typeId = ComponentTypeManager::GetTypeId<T>();
        assert(m_componentArrays[typeId] == nullptr && "Registering component type more than once.");

        m_componentArrays[typeId] = std::make_unique<ComponentArray<T>>();
    }

    template<typename T>
    [[nodiscard]] ComponentType GetComponentType() const
    {
        return ComponentTypeManager::GetTypeId<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, const T& component)
    {
        GetComponentArray<T>().InsertData(entity, component);
    }

    template<typename T>
    void AddComponent(Entity entity, T&& component)
    {
        using ComponentT = std::remove_reference_t<T>;
        GetComponentArray<ComponentT>().InsertData(entity, std::forward<T>(component));
    }

    template<typename T>
    void SetComponent(Entity entity, const T& component)
    {
        GetComponentArray<T>().SetData(entity, component);
    }

    template<typename T>
    void SetComponent(Entity entity, T&& component)
    {
        using ComponentT = std::remove_reference_t<T>;
        GetComponentArray<ComponentT>().SetData(entity, std::forward<T>(component));
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetComponentArray<T>().RemoveData(entity);
    }

    template<typename T>
    [[nodiscard]] T& GetComponent(Entity entity)
    {
        return GetComponentArray<T>().GetData(entity);
    }

    void EntityDestroyed(Entity entity, Signature signature)
    {
        // Only check the bits (IDs) of the components that the Entity actually possessed
        for (ComponentType i = 0; i < MAX_COMPONENTS; ++i) {
            if (signature.test(i)) {
                m_componentArrays[i]->EntityDestroyed(entity);
            }
        }
    }
    template<typename T>
    [[nodiscard]] ComponentArray<T>& GetComponentArray()
    {
        ComponentType typeId = ComponentTypeManager::GetTypeId<T>();
        assert(m_componentArrays[typeId] != nullptr && "Component not registered before use.");

        return *static_cast<ComponentArray<T>*>(m_componentArrays[typeId].get());
    }

    template<typename T>
    [[nodiscard]] const ComponentEvents<T>& GetEvents()
    {
        return GetComponentArray<T>().GetEvents();
    }

    template<typename T>
    void ClearEvents()
    {
        GetComponentArray<T>().ClearEvents();
    }

    void ClearEvents()
    {
        for (const auto& componentArray : m_componentArrays) {
            if (componentArray != nullptr) {
                componentArray->ClearEvents();
            }
        }
    }

private:
    std::array<std::unique_ptr<IComponentArray>, MAX_COMPONENTS> m_componentArrays{};
};
