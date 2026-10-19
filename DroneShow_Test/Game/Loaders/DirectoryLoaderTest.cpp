#include "pch.h"
#include "Common/TempDirectory.h"
#include "Game/Loaders/DirectoryLoader.h"

#include <filesystem>

class DirectoryLoaderTest : public ::testing::Test
{
protected:
    // sonar-ignore-next-line
    TempDirectory temp;
};

// ---------------------------------------------------------
// 1. Missing Directory: Returns an empty list instead of throwing
// ---------------------------------------------------------
TEST_F(DirectoryLoaderTest, ReturnsEmptyForMissingDirectory)
{
    EXPECT_TRUE(DirectoryLoader::Load(temp.PathOf("does_not_exist")).empty());
}

// ---------------------------------------------------------
// 2. Non-Directory Path: A regular file path is rejected
// ---------------------------------------------------------
TEST_F(DirectoryLoaderTest, ReturnsEmptyForFilePath)
{
    const auto filePath = temp.WriteFile("file.json", "{}");

    EXPECT_TRUE(DirectoryLoader::Load(filePath).empty());
}

// ---------------------------------------------------------
// 3. Extension Filter: Only regular files with the given extension are returned
// ---------------------------------------------------------
TEST_F(DirectoryLoaderTest, FiltersByExtensionAndSkipsDirectories)
{
    const auto a = temp.WriteFile("a.json", "{}");
    const auto b = temp.WriteFile("b.json", "{}");
    temp.WriteFile("note.txt", "text");
    std::filesystem::create_directories(temp.Root() / "sub.json"); // Directory with a matching name

    auto result = DirectoryLoader::Load(temp.RootPath(), ".json");
    std::ranges::sort(result);

    EXPECT_EQ(result, (std::vector<std::string>{ a, b }));
}

// ---------------------------------------------------------
// 4. No Extension Filter: All regular files are returned
// ---------------------------------------------------------
TEST_F(DirectoryLoaderTest, ReturnsAllFilesWhenExtensionIsEmpty)
{
    const auto a = temp.WriteFile("a.json", "{}");
    const auto note = temp.WriteFile("note.txt", "text");
    std::filesystem::create_directories(temp.Root() / "subdir");

    auto result = DirectoryLoader::Load(temp.RootPath());
    std::ranges::sort(result);

    EXPECT_EQ(result, (std::vector<std::string>{ a, note }));
}
