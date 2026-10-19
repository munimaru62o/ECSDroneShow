// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <print>
#include <ranges>
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
    namespace fs = std::filesystem;

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::println(stderr, "Directory not found or is not a directory: {}", path);
        return {};
    }

    return fs::directory_iterator(path)
        | std::views::filter([](const auto& entry) { return entry.is_regular_file(); })
        | std::views::filter([extension](const auto& entry) {
              return extension.empty() || entry.path().extension() == extension;
          })
        | std::views::transform([](const auto& entry) { return entry.path().generic_string(); })
        | std::ranges::to<std::vector>();
}
}
