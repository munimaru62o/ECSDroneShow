// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Game/Managers/TimelineManager.h"
#include "Game/Loaders/DirectoryLoader.h"
#include "Game/Loaders/JsonLoader.h"
#include "Game/Data/Timeline/TimelineData.h"
#include "Game/Data/Timeline/FormationData.h"
#include "Game/Parsers/Json/DataJsonParsers.h"

#include <cassert>

TimelineManager::TimelineManager() = default;
TimelineManager::~TimelineManager() = default;

int TimelineManager::LoadTimelinesFromDirectory(const std::string& directoryPath)
{
    int loadedCount = 0;
    auto fileList = DirectoryLoader::Load(directoryPath, ".json");

    for (const auto& file : fileList) {
        if (LoadTimelineJson(file)) {
            loadedCount++;
        }
    }
    return loadedCount;
}

const TimelineData* TimelineManager::GetTimelineData(const std::string& timelineName) const
{
    if (timelineName.empty()) {
        return nullptr;
    }

    auto it = m_timelines.find(timelineName);
    if (it != m_timelines.end()) {
        return &it->second;
    }
    return nullptr;
}


const FormationData* TimelineManager::GetFormationData(const std::string& filePath) const
{
    if (filePath.empty()) {
        return nullptr;
    }

    auto it = m_formationCache.find(filePath);
    if (it != m_formationCache.end()) {
        return &it->second;
    }

    assert(false && "TimelineManager: Requested FormationData was not preloaded or does not exist.");
    return nullptr;
}

void TimelineManager::RegisterFormationData(const std::string& key, FormationData data)
{
    m_formationCache[key] = std::move(data);
}

bool TimelineManager::LoadFormationJson(const std::string& filePath)
{
    auto it = m_formationCache.find(filePath);
    if (it != m_formationCache.end()) {
        // already loaded.
        return true;
    }

    FormationData data;
    if (!JsonLoader::Load(filePath, data)) {
        return false;
    }
    RegisterFormationData(filePath, std::move(data));
    return true;
}

bool TimelineManager::LoadTimelineJson(const std::string& filePath)
{
    TimelineData data;
    if (!JsonLoader::Load(filePath, data)) {
        return false;
    }
   
    for (const auto& seq : data.sequences) {
        std::vector<std::string> formationKeys = {
            "FormationComponentWireframe",
            "FormationComponentPointCloud"
        };

        for (const auto& key : formationKeys) {
            if (seq.components.contains(key)) {
                const auto& formParams = seq.components[key];
                if (formParams.contains("file")) {
                    std::string fileParam = formParams.value("file", "");
                    if (!fileParam.empty()) {
                        LoadFormationJson(fileParam);
                    }
                }
            }
        }
    }
    m_timelines[data.name] = std::move(data);
    return true;
}
