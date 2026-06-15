// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <json.hpp>

/**
 * @namespace JsonLoader
 * @brief A stateless utility namespace for generic JSON file loading and deserialization.
 *
 * This namespace provides a heavily templated approach to load any C++ data structure
 * from a JSON file. It relies on the target type `T` having a valid nlohmann::json
 * deserializer defined (typically via NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE).
 * * It centralizes file I/O operations and robust exception handling, making it an
 * essential tool for the engine's Data-Driven design.
 */
namespace JsonLoader
{
/**
 * @brief Parses a JSON file and populates the provided data structure.
 * @tparam T The target data type to deserialize into.
 * @param filePath The path to the JSON file.
 * @param outData A reference to the object that will be populated with JSON data.
 * @return true if the file was successfully loaded and parsed; false otherwise.
 */
template<typename T>
inline bool Load(const std::string& filePath, T& outData)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[JsonLoader] Failed to open JSON file: " << filePath << std::endl;
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;
        outData = j.get<T>();
        return true;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[JsonLoader] JSON parsing error in " << filePath << ": " << e.what() << std::endl;
        return false;
    }
}
}
