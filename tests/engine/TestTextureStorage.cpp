#include <gtest/gtest.h>

#include <filesystem>

#include <engine/render/TextureStorage.hpp>
#include <common/constants/render/Assets.hpp>

namespace {

bool textureFileExists(Assets id)
{
    const auto& path = pathAssets[static_cast<std::size_t>(id)];
    return std::filesystem::exists(path);
}

bool allTextureFilesExist()
{
    for (size_t i = 0; i < NUMBER_ASSETS; ++i) {
        if (!std::filesystem::exists(pathAssets[i])) {
            return false;
        }
    }
    return true;
}

} // namespace

TEST(TextureStorageCoverage, GetTextureOutOfRangeReturnsNull)
{
    TextureStorage storage;
    auto texture = storage.getTexture(static_cast<Assets>(NUMBER_ASSETS));
    EXPECT_EQ(texture, nullptr);
}

TEST(TextureStorageCoverage, InitLoadsTexturesOrThrows)
{
    TextureStorage storage;

    if (allTextureFilesExist()) {
        EXPECT_NO_THROW(storage.init());
        EXPECT_NE(storage.getTexture(Assets::LOGO_RTYPE), nullptr);
    } else {
        EXPECT_THROW(storage.init(), Error);
    }
}

TEST(TextureStorageCoverage, GetTextureAfterPartialLoad)
{
    TextureStorage storage;

    if (textureFileExists(Assets::LOGO_RTYPE)) {
        // If assets exist, init should succeed and texture should be available.
        EXPECT_NO_THROW(storage.init());
        EXPECT_NE(storage.getTexture(Assets::LOGO_RTYPE), nullptr);
    } else {
        // If assets are missing, init will throw and textures remain inaccessible.
        EXPECT_THROW(storage.init(), Error);
        EXPECT_EQ(storage.getTexture(Assets::LOGO_RTYPE), nullptr);
    }
}
