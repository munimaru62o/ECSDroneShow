// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Math/Vector3.h"

#include <string>

struct WindowConfig
{
    int width = 800;
    int height = 600;
    bool isFullscreen = false;
};

struct CameraConfig
{
    Vector3 position{};
};

struct SpawnConfig
{
    int initialSpawnCount = 10000;
    int userSpawnNum = 1000;
    int userDestroyNum = 1000;
};

struct PrefabConfig
{
    std::string folderPath{};
    std::string spawnName{};
};

struct TimelineConfig
{
    std::string folderPath{};
    std::string playName{};
};

struct GameConfig
{
    WindowConfig window;
    CameraConfig camera;
    SpawnConfig spawn;
    PrefabConfig prefab;
    TimelineConfig timeline;
};
