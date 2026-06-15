// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/SystemPhases.h"

#include <array>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <algorithm>

class Coordinator;

/**
 * @class SystemTypeManager
 * @brief A utility that assigns a unique, contiguous integer ID (SystemType) to each component type.
 *
 * By leveraging the behavior of static local variables within C++ template functions,
 * this manager achieves O(1) type ID resolution at compile/runtime initialization
 * without relying on Run-Time Type Information (RTTI).
 */
class SystemTypeManager 
{
public:
    SystemTypeManager() = delete;

private:
    static inline SystemType s_nextTypeId = 0;

public:
    template <typename T>
    [[nodiscard]] static SystemType GetTypeId()
    {
        // Since a static variable within a template function is instantiated exactly once per type T,
        // s_nextTypeId++ is only evaluated the first time this function is called for a given type.
        // Subsequent calls will return the cached ID.
        static const SystemType typeId = s_nextTypeId++;

        assert(typeId < MAX_SYSTEMS && "Too many systems registered!");

        return typeId;
    }
};

/**
 * @class SystemManager
 * @brief Manages the registration, lifecycle, and execution order (pipeline) of all Systems.
 *
 * It monitors the Component Signature of each Entity and automatically links Entities
 * to the appropriate Systems that match their requirements. It also sorts the registered
 * Systems based on their Phase and Order to construct the correct execution pipeline.
 */
class SystemManager final : private NonCopyable
{
public:
    SystemManager() = default;
    ~SystemManager() = default;

public:
    template<typename T>
    T* RegisterSystem(Signature signature, SystemPhase phase, int order)
    {
        SystemType typeId = SystemTypeManager::GetTypeId<T>();
        assert(m_systems[typeId].system == nullptr && "Registering system more than once.");

        auto system = std::make_unique<T>();
        T* systemPtr = system.get();

        auto& record = m_systems[typeId];
        record.system = std::move(system);
        record.signature = signature;

        m_systemsByPhase[static_cast<size_t>(phase)].push_back({ order, systemPtr });

        return systemPtr;
    }

    template<typename T>
    [[nodiscard]] T* GetSystem()
    {
        SystemType typeId = SystemTypeManager::GetTypeId<T>();
        assert(m_systems[typeId].system);

        return static_cast<T*>(m_systems[typeId].system.get());
    }

    void EntityDestroyed(Entity entity)
    {
        for (const auto& record : m_systems) {
            if (record.system != nullptr) {
                record.system->EraseEntity(entity);
            }
        }
    }

    void EntitySignatureChanged(Entity entity, Signature entitySignature)
    {
        for (const auto& record : m_systems) {
            if (record.system != nullptr) {
                if ((entitySignature & record.signature) == record.signature) {
                    record.system->AddEntity(entity);
                } else {
                    record.system->EraseEntity(entity);
                }
            }
        }
    }

    void InitSystems(Coordinator& coordinator)
    {
        // Sorts all registered systems in ascending order based on their execution order,
        for (auto& nodeList : m_systemsByPhase) {
            std::sort(nodeList.begin(), nodeList.end(), [](const PhaseNode& a, const PhaseNode& b) {
                return a.order < b.order;
            });
        }

        for (const auto& record : m_systems) {
            if (record.system != nullptr) {
                record.system->Init(coordinator);
            }
        }
    }

    void UpdateSystems(SystemPhase phase, Coordinator& coordinator, float dt, double simulationTime)
    {
        for (const auto& node : m_systemsByPhase[static_cast<size_t>(phase)]) {
            node.system->Update(coordinator, dt, simulationTime);
        }
    }

private:
    struct SystemRecord
    {
        Signature signature{};
        std::unique_ptr<System> system;
    };

    struct PhaseNode
    {
        int order = -1;
        System* system = nullptr;
    };

    std::array<SystemRecord, MAX_SYSTEMS> m_systems{};
    std::array<std::vector<PhaseNode>, static_cast<size_t>(SystemPhase::Count)> m_systemsByPhase;
};
