// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"

#include <string>
#include <unordered_map>

struct TimelineData;
struct FormationData;

/**
 * @class TimelineManager
 * @brief Manages choreography sequences (Timelines) and spatial positioning data (Formations) for drone shows.
 *
 * This manager loads JSON-based timeline data and their associated formation coordinate data,
 * caching them in memory. Crucially, it features an automated preloading mechanism:
 * when a timeline is loaded, it parses the sequence to identify and preemptively load
 * all required formation files. This guarantees zero disk I/O during live performances,
 * preventing frame rate spikes and ensuring perfectly smooth drone rendering.
 */
class TimelineManager final : private NonCopyable
{
public:
    TimelineManager();
    ~TimelineManager();

public:
    [[nodiscard]] const TimelineData* GetTimelineData(const std::string& timelineName) const;
    [[nodiscard]] const FormationData* GetFormationData(const std::string& filePath) const;

    int LoadTimelinesFromDirectory(const std::string& directoryPath);

private:
    bool LoadFormationJson(const std::string& filePath);
    bool LoadTimelineJson(const std::string& filepath);

private:
    std::unordered_map<std::string, TimelineData> m_timelines;
    std::unordered_map<std::string, FormationData> m_formationCache;
};
