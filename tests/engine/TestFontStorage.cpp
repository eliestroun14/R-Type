#include <gtest/gtest.h>

#include <filesystem>

#define private public
#include <engine/render/FontStorage.hpp>
#undef private

#include <common/constants/render/Assets.hpp>

namespace {

bool fontFileExists(FontAssets id)
{
    const auto& path = pathFontAssets[static_cast<std::size_t>(id)];
    return std::filesystem::exists(path);
}

} // namespace

TEST(FontStorageCoverage, GetFontOutOfRangeReturnsNull)
{
    FontStorage storage;
    auto font = storage.getFont(static_cast<FontAssets>(NUMBER_FONT_ASSETS));
    EXPECT_EQ(font, nullptr);
}

TEST(FontStorageCoverage, InitLoadsFontsOrThrows)
{
    FontStorage storage;

    const bool allFontsExist = fontFileExists(FontAssets::DEFAULT_FONT)
        && fontFileExists(FontAssets::DYSLEXIC_FONT);

    if (allFontsExist) {
        EXPECT_NO_THROW(storage.init());
        EXPECT_NE(storage.getFont(FontAssets::DEFAULT_FONT), nullptr);
        EXPECT_NE(storage.getFont(FontAssets::DYSLEXIC_FONT), nullptr);
    } else {
        EXPECT_THROW(storage.init(), Error);
    }
}

TEST(FontStorageCoverage, LoadFontUsesStorage)
{
    FontStorage storage;
    storage._fonts.resize(NUMBER_FONT_ASSETS);

    if (fontFileExists(FontAssets::DEFAULT_FONT)) {
        EXPECT_NO_THROW(storage.loadFont(FontAssets::DEFAULT_FONT));
        EXPECT_NE(storage._fonts[FontAssets::DEFAULT_FONT], nullptr);
    } else {
        EXPECT_THROW(storage.loadFont(FontAssets::DEFAULT_FONT), Error);
    }
}
