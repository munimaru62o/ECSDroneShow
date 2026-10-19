#include "pch.h"
#include "Common/TempDirectory.h"
#include "Game/Loaders/JsonLoader.h"
#include "Game/Data/Prefab/PrefabData.h"
#include "Game/Parsers/Json/DataJsonParsers.h"

class JsonLoaderTest : public ::testing::Test
{
protected:
    // sonar-ignore-next-line
    TempDirectory temp;
};

// ---------------------------------------------------------
// 1. Successful Load: Parses the file into the target type
// ---------------------------------------------------------
TEST_F(JsonLoaderTest, LoadsValidJson)
{
    const auto path = temp.WriteFile("prefab.json", R"({"name": "TestDrone", "components": {"DroneComponent": {}}})");

    PrefabData data;
    ASSERT_TRUE(JsonLoader::Load(path, data));

    EXPECT_EQ(data.name, "TestDrone");
    EXPECT_TRUE(data.components.contains("DroneComponent"));
}

// ---------------------------------------------------------
// 2. Missing File: Returns false without modifying the output
// ---------------------------------------------------------
TEST_F(JsonLoaderTest, ReturnsFalseForMissingFile)
{
    PrefabData data;
    data.name = "Unchanged";

    EXPECT_FALSE(JsonLoader::Load(temp.PathOf("missing.json"), data));
    EXPECT_EQ(data.name, "Unchanged");
}

// ---------------------------------------------------------
// 3. Malformed JSON: Syntax errors are caught and reported as failure
// ---------------------------------------------------------
TEST_F(JsonLoaderTest, ReturnsFalseForMalformedJson)
{
    const auto path = temp.WriteFile("broken.json", R"({"name": "TestDrone", )");

    PrefabData data;
    EXPECT_FALSE(JsonLoader::Load(path, data));
}

// ---------------------------------------------------------
// 4. Type Mismatch: Valid JSON that does not match the target type is rejected
// ---------------------------------------------------------
TEST_F(JsonLoaderTest, ReturnsFalseForTypeMismatch)
{
    const auto path = temp.WriteFile("mismatch.json", R"({"name": 123, "components": {}})");

    PrefabData data;
    EXPECT_FALSE(JsonLoader::Load(path, data));
}
