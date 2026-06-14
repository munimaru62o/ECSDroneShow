// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Managers/PrefabManager.h"
#include "Game/Loaders/DirectoryLoader.h"
#include "Game/Loaders/JsonLoader.h"
#include "Game/Data/Prefab/PrefabData.h"
#include "Game/Parsers/Json/DataJsonParsers.h"

#include <vector>


PrefabManager::PrefabManager(Coordinator& coordinator, ComponentRegistry& registry)
    : m_coordinator(coordinator)
    , m_registry(registry)
{};
PrefabManager::~PrefabManager() = default;


bool PrefabManager::LoadPrefab(const std::string& filePath)
{
    PrefabData data;
    if (!JsonLoader::Load(filePath, data)) {
        return false;
    }

    auto& applier = m_appliers[data.name];
    for (auto it = data.components.begin(); it != data.components.end(); ++it) {
        const std::string& compName = it.key();
        const nlohmann::json& compData = it.value();

        // Pre-bake the JSON data into a callable operation
        applier.push_back(m_registry.GetDescriptor(compName).applier(compData));
    }
    return true;
}

int PrefabManager::LoadPrefabsFromDirectory(const std::string& directoryPath)
{
    int loadedCount = 0;
    auto fileList = DirectoryLoader::Load(directoryPath, ".json");

    for (const auto& file : fileList) {
        if (LoadPrefab(file)) {
            loadedCount++;
        }
    }
    return loadedCount;
}

Entity PrefabManager::Instantiate(const std::string& prefabName)
{
    auto it = m_appliers.find(prefabName);
    if (it == m_appliers.end()) {
        std::cerr << "[PrefabManager] Prefab not found: " << prefabName << std::endl;
        return INVALID_ENTITY;
    }

    Entity entity = m_coordinator.CreateEntity();
    m_coordinator.ApplyCoreComponents(entity);

    // Execute the pre-baked appliers (Extremely fast, zero allocation)
    for (const auto& applier : it->second) {
        applier.execute(m_coordinator, entity, {});
    }
    return entity;
}
