// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <filesystem>

/**
 * @namespace DirectoryLoader
 * @brief A stateless utility namespace for scanning directories and retrieving file paths.
 */
namespace DirectoryLoader
{
/**
 * @brief Scans a specified directory and returns a list of file paths.
 * * This function only collects regular files (directories are ignored). 
 * It does not search recursively.
 *
 * @param path The relative or absolute path to the target directory.
 * @param extension (Optional) The file extension to filter by (e.g., ".json"). 
 * Must include the dot. If empty, all files will be collected.
 * @return std::vector<std::string> A list of generic string paths found in the directory.
 */
inline std::vector<std::string> Load(std::string_view path, std::string_view extension = "")
{
    std::vector<std::string> pathList;
    namespace fs = std::filesystem;

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cerr << "Directory not found or is not a directory: " << path << std::endl;
        return pathList;
    }

    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        if (!extension.empty()) {
            if (entry.path().extension() != extension) {
                continue;
            }
        }
        pathList.emplace_back(entry.path().generic_string());
    }
    return pathList;
}
}
