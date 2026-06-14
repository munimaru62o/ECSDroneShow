// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include "Engine/ECS/ComponentOperationFactory.h"
#include "Game/Registry/ComponentRegistry.h"

#include <string>
#include <unordered_map>

class Coordinator;

/**
 * @class PrefabManager
 * @brief A manager that loads entity templates (prefabs) from JSON files and provides ultra-fast instantiation.
 *
 * This class scans a target directory for .json files and loads the configuration data
 * detailing which components should be attached to a specific entity type.
 *
 * Crucially, the parsed JSON data is pre-compiled and cached as "apply functions"
 * (ComponentAssignOperations) via the ComponentRegistry. As a result, calling Instantiate()
 * incurs absolutely zero string parsing or JSON evaluation overhead at runtime.
 * This enables the engine to spawn massive quantities of entities with minimal CPU cost,
 * acting as a core pillar of the fully Data-Driven architecture.
 */
class PrefabManager final : private NonCopyable
{
public:
    PrefabManager(Coordinator& coordinator, ComponentRegistry& registry);
    ~PrefabManager();

public:
    bool LoadPrefab(const std::string& filePath);
    int LoadPrefabsFromDirectory(const std::string& directoryPath);
    [[nodiscard]] Entity Instantiate(const std::string& prefabName);

private:
    Coordinator& m_coordinator;
    ComponentRegistry& m_registry;
    std::unordered_map<std::string, std::vector<ComponentAssignOperation>> m_appliers;
};
