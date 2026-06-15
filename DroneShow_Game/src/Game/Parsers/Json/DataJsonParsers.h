// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Game/Parsers/Json/MathJsonParsers.h"
#include "Game/Data/Config/GameConfig.h"
#include "Game/Data/Prefab/PrefabData.h"
#include "Game/Data/Timeline/TimelineData.h"
#include "Game/Data/Timeline/FormationData.h"

#include <json.hpp>


// GameConfig
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameConfig, window, camera, spawn, prefab, timeline)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimelineConfig, folderPath, playName)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(WindowConfig, width, height, isFullscreen)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraConfig, position)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SpawnConfig, initialSpawnCount, userSpawnNum, userDestroyNum)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PrefabConfig, folderPath, spawnName)

// Prefab
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PrefabData, name, components)

// Formation
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FormationPoint, position, color)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FormationData, name, points)

// Timeline
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SequenceData, name, duration, components)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TimelineData, name, sequences)
