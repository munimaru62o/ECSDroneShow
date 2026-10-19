#pragma once

#include "Engine/Utils/NonCopyable.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

/**
 * @class TempDirectory
 * @brief An RAII helper that provides an isolated, empty directory for file-based tests.
 *
 * The directory is created under the system temp directory, named after the running test,
 * and removed when the object is destroyed. Hold it as a fixture member so it can be combined
 * with any other fixture (e.g. ECSTestFixture).
 */
class TempDirectory : private NonCopyable
{
public:
    TempDirectory()
    {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        m_root = std::filesystem::temp_directory_path()
            / "ECSDroneShow_Test"
            / (std::string(info->test_suite_name()) + "_" + info->name());

        std::filesystem::remove_all(m_root);
        std::filesystem::create_directories(m_root);
    }

    ~TempDirectory()
    {
        std::error_code ec;
        std::filesystem::remove_all(m_root, ec);
    }

    /**
     * @brief Writes a file relative to the temp directory and returns its path (generic format).
     */
    std::string WriteFile(std::string_view relativePath, std::string_view content) const
    {
        const auto path = m_root / relativePath;
        std::filesystem::create_directories(path.parent_path());

        std::ofstream file(path, std::ios::binary);
        file << content;
        return path.generic_string();
    }

    /**
     * @brief Returns the path of an entry inside the temp directory (generic format), without creating it.
     */
    [[nodiscard]] std::string PathOf(std::string_view relativePath) const
    {
        return (m_root / relativePath).generic_string();
    }

    [[nodiscard]] std::string RootPath() const
    {
        return m_root.generic_string();
    }

    [[nodiscard]] const std::filesystem::path& Root() const
    {
        return m_root;
    }

private:
    std::filesystem::path m_root;
};
