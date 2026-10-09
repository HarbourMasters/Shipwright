#include "CosmeticShading.h"

#include <libultraship/libultraship.h>
#include <fast/resource/ResourceType.h>
#include <fast/resource/type/Texture.h>

#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
#include "macros.h"

void Gfx_TextureCacheDelete(const uint8_t* texAddr);
}

//--------------------------------------------------------------------------------------------------------------------
// TEXTURE LOOKUP
// Finding an asset and working out what kind of texture it is.
//--------------------------------------------------------------------------------------------------------------------

// Loads a texture by path, null if that path isn't one.
static std::shared_ptr<Fast::Texture> LoadTextureExact(const std::string& path) {
    auto res = Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path, true);
    if (res == nullptr || res->GetInitData()->Type != static_cast<uint32_t>(Fast::ResourceType::Texture)) {
        return nullptr;
    }
    return std::static_pointer_cast<Fast::Texture>(res);
}

// True if this is an untouched RGBA16 palette with at least this many entries.
static bool IsNativeEntryTexture(const std::shared_ptr<Fast::Texture>& tex, uint32_t entryCount) {
    return tex != nullptr && (tex->Flags & TEX_FLAG_LOAD_AS_RAW) == 0 && tex->HByteScale == 1.0f &&
           tex->VPixelScale == 1.0f && tex->Type == Fast::TextureType::RGBA16bpp &&
           tex->ImageDataSize >= entryCount * 2;
}

// True if this is an HD pack texture, stored as plain RGBA32 pixels.
static bool IsRawRgba32Texture(const std::shared_ptr<Fast::Texture>& tex) {
    return tex != nullptr && (tex->Flags & TEX_FLAG_LOAD_AS_RAW) != 0 && tex->Type != Fast::TextureType::Palette4bpp &&
           tex->Type != Fast::TextureType::Palette8bpp && tex->Width > 0 && tex->Height > 0 &&
           tex->ImageDataSize >= (uint32_t)tex->Width * tex->Height * 4;
}

// True if this is an untouched paletted texture.
static bool IsNativeCiTexture(const std::shared_ptr<Fast::Texture>& tex) {
    if (tex == nullptr || (tex->Flags & TEX_FLAG_LOAD_AS_RAW) != 0 || tex->HByteScale != 1.0f ||
        tex->VPixelScale != 1.0f || tex->Width == 0 || tex->Height == 0) {
        return false;
    }
    uint32_t texelCount = (uint32_t)tex->Width * tex->Height;
    if (tex->Type == Fast::TextureType::Palette8bpp) {
        return tex->ImageDataSize >= texelCount;
    }
    if (tex->Type == Fast::TextureType::Palette4bpp) {
        return tex->ImageDataSize >= (texelCount + 1) / 2;
    }
    return false;
}

//--------------------------------------------------------------------------------------------------------------------
// EDITING THE GAME'S OWN TEXTURES
// Overwriting asset bytes in place, remembering the originals so it can be undone.
//--------------------------------------------------------------------------------------------------------------------

struct OriginalTextureData {
    std::vector<uint16_t> entries;
    std::vector<bool> saved;
};

static std::unordered_map<std::string, OriginalTextureData> sOriginalTextureData;

struct PaletteTarget {
    uint8_t* data = nullptr;
    OriginalTextureData* original = nullptr;
};

static const std::unordered_map<std::string, std::vector<std::string>> sTlutReaders = {
    { "objects/object_link_nuts/object_link_nuts_TLUT_003EB0",
      {
          "objects/object_link_nuts/object_link_nuts_Tex_0042B0",
          "objects/object_link_nuts/object_link_nuts_Tex_0043B0",
          "objects/object_link_nuts/object_link_nuts_Tex_0044B0",
          "objects/object_link_nuts/object_link_nuts_Tex_00B780",
      } },
    { "objects/object_test3/gKafeiBody2TLUT",
      {
          "objects/object_test3/gKafeiHairTex",
          "objects/object_test3/gKafeiHairPartAndUndersideTex",
      } },
    { "objects/object_link_zora/object_link_zora_TLUT_005000",
      {
          "objects/object_link_zora/object_link_zora_Tex_005400",
          "objects/object_link_zora/object_link_zora_Tex_005800",
          "objects/object_link_zora/object_link_zora_Tex_005900",
          "objects/object_link_zora/object_link_zora_Tex_005A00",
          "objects/object_link_zora/object_link_zora_Tex_005A80",
          "objects/object_link_zora/object_link_zora_Tex_005B00",
          "objects/object_link_zora/object_link_zora_Tex_005D00",
          "objects/object_link_zora/object_link_zora_Tex_005F00",
          "objects/object_link_zora/object_link_zora_Tex_006000",
          "objects/object_link_zora/object_link_zora_Tex_006100",
          "objects/object_link_zora/object_link_zora_Tex_006180",
          "objects/object_link_zora/object_link_zora_Tex_0061C0",
          "objects/object_link_zora/object_link_zora_Tex_00E678",
          "objects/object_link_zora/object_link_zora_Tex_00E778",
          "objects/object_link_zora/object_link_zora_Tex_00E878",
      } },
    { "objects/object_link_zora/object_link_zora_TLUT_00C578",
      {
          "objects/object_link_zora/object_link_zora_Tex_00C778",
          "objects/object_link_zora/object_link_zora_Tex_00C978",
      } },
    { "objects/object_link_boy/object_link_boy_TLUT_008128",
      {
          "objects/object_link_boy/object_link_boy_Tex_008C88",
      } },
    { "objects/object_horse_link_child/gEponaTLUT",
      {
          "objects/object_horse_link_child/gEponaEyeOpenTex",
          "objects/object_horse_link_child/gEponaEyeHalfTex",
          "objects/object_horse_link_child/gEponaEyeClosedTex",
      } },
};

// Grabs a palette's raw bytes so they can be edited in place.
static PaletteTarget ResolvePaletteTarget(const char* path, uint32_t lastEntry) {
    auto tex = LoadTextureExact(path);
    if (tex == nullptr || tex->GetInitData()->IsCustom || !IsNativeEntryTexture(tex, lastEntry + 1)) {
        return {};
    }

    return { tex->ImageData, &sOriginalTextureData[path] };
}

// Remembers an entry's original color before we overwrite it.
static void SaveOriginalEntry(const PaletteTarget& target, uint32_t index) {
    OriginalTextureData& original = *target.original;

    if (index >= original.saved.size()) {
        original.saved.resize(index + 1, false);
        original.entries.resize(index + 1, 0);
    }

    if (!original.saved[index]) {
        original.entries[index] = (target.data[index * 2] << 8) | target.data[index * 2 + 1];
        original.saved[index] = true;
    }
}

// Writes a new color into one palette entry.
static void PatchPalette(const PaletteTarget& target, uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
    SaveOriginalEntry(target, index);

    uint16_t col16 = (r << 11) | (g << 6) | (b << 1) | 1;
    target.data[index * 2] = col16 >> 8;
    target.data[index * 2 + 1] = col16 & 0xff;
}

// Puts one palette entry back to its original color.
static void UnpatchPalette(const PaletteTarget& target, uint32_t index) {
    const OriginalTextureData& original = *target.original;
    if (index >= original.saved.size() || !original.saved[index]) {
        return;
    }

    target.data[index * 2] = original.entries[index] >> 8;
    target.data[index * 2 + 1] = original.entries[index] & 0xff;
}

// Writes a new color into one pixel, leaving it see-through if it already was.
static void PatchRGBA16Pixel(const PaletteTarget& target, uint32_t index, uint8_t r, uint8_t g, uint8_t b) {
    SaveOriginalEntry(target, index);

    // Preserve the original alpha bit so transparent pixels stay transparent.
    uint8_t a = target.data[index * 2 + 1] & 1;
    uint16_t col16 = (r << 11) | (g << 6) | (b << 1) | a;
    target.data[index * 2] = col16 >> 8;
    target.data[index * 2 + 1] = col16 & 0xff;
}

// Tells the renderer to reload the textures we just edited.
static void InvalidateNativeTextureCache(const std::string& path, const PaletteTarget& target) {
    auto readers = sTlutReaders.find(path);
    if (readers == sTlutReaders.end()) {
        Gfx_TextureCacheDelete(target.data);
        return;
    }
    for (const std::string& texPath : readers->second) {
        auto tex = LoadTextureExact(texPath);
        if (tex != nullptr) {
            Gfx_TextureCacheDelete(tex->ImageData);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------
// COLOR MATH
// Small helpers shared by the palette and HD paths.
//--------------------------------------------------------------------------------------------------------------------

// True if two colors are identical.
static bool SameColor(Color_RGBA8 a, Color_RGBA8 b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

// A color's hue in degrees, or -1 if it's grey.
static float HueOf(float r, float g, float b) {
    float maxC = std::max({ r, g, b });
    float minC = std::min({ r, g, b });
    float delta = maxC - minC;

    if (delta <= 0.0f) {
        return -1.0f;
    }

    float h;
    if (maxC == r) {
        h = 60.0f * fmodf((g - b) / delta, 6.0f);
    } else if (maxC == g) {
        h = 60.0f * (((b - r) / delta) + 2.0f);
    } else {
        h = 60.0f * (((r - g) / delta) + 4.0f);
    }
    return (h < 0.0f) ? h + 360.0f : h;
}

static constexpr float kTintHueTolerance = 40.0f;

// True if a pixel is colorful enough, and the right hue, to be recolored.
static bool PassesTintFilter(float r, float g, float b, float minSaturation, float matchHue) {
    float value = std::max({ r, g, b });
    if (value <= 0.0f) {
        return false;
    }
    if (minSaturation > 0.0f && ((value - std::min({ r, g, b })) / value) < minSaturation) {
        return false;
    }
    if (matchHue >= 0.0f) {
        float hue = HueOf(r, g, b);
        if (hue < 0.0f) {
            return false;
        }
        float hueDelta = fabsf(hue - matchHue);
        hueDelta = std::min(hueDelta, 360.0f - hueDelta);
        if (hueDelta > kTintHueTolerance) {
            return false;
        }
    }
    return true;
}

// Slides one color along a fade running between two other colors.
static Color_RGBA8 mapNewBaseColorToGradient(Color_RGBA8 currentColor, Color_RGBA8 oldBase, Color_RGBA8 newBase,
                                             Color_RGBA8 targetEnd) {
    double gradientRed = targetEnd.r - oldBase.r;
    double gradientGreen = targetEnd.g - oldBase.g;
    double gradientBlue = targetEnd.b - oldBase.b;

    double deltaRed = currentColor.r - oldBase.r;
    double deltaGreen = currentColor.g - oldBase.g;
    double deltaBlue = currentColor.b - oldBase.b;

    double gradient = gradientRed * gradientRed + gradientGreen * gradientGreen + gradientBlue * gradientBlue;
    double projection = gradientRed * deltaRed + gradientGreen * deltaGreen + gradientBlue * deltaBlue;

    double position = 0.0;
    if (gradient != 0.0) {
        position = projection / gradient;
    }

    return { uint8_t(newBase.r + position * (targetEnd.r - newBase.r)),
             uint8_t(newBase.g + position * (targetEnd.g - newBase.g)),
             uint8_t(newBase.b + position * (targetEnd.b - newBase.b)), 31 };
}

//--------------------------------------------------------------------------------------------------------------------
// HD TEXTURE PACKS
// Pack textures replace the originals wholesale, so they get recolored pixel by pixel instead.
//--------------------------------------------------------------------------------------------------------------------

enum class HdRecolorKind {
    Whiten,
    Gradient,
    Tint,
};

struct HdRecolor {
    HdRecolorKind kind;
    SHADE_MODE mode = MODE_AVG;
    Color_RGBA8 newBase = { 255, 255, 255, 255 };
    Color_RGBA8 oldBase = { 0, 0, 0, 255 };
    Color_RGBA8 targetEnd = { 0, 0, 0, 255 };
    float tintMinSaturation = 0.0f;
    float tintMatchHue = -1.0f;
};

struct HdMask {
    std::vector<uint32_t> pixels;
    SHADE_MODE whitenMode = MODE_REVERT;
    std::vector<uint8_t> whitenLevels;
    bool hasGradient = false;
    Color_RGBA8 gradientOldBase = {};
    Color_RGBA8 gradientTargetEnd = {};
    std::vector<float> gradientPositions;
    bool hasTint = false;
    float tintMinSaturation = 0.0f;
    float tintMatchHue = -1.0f;
    std::vector<uint8_t> tintLevels;
};

struct HdTextureState {
    std::shared_ptr<Fast::Texture> texture;
    std::vector<uint8_t> original;
    std::unordered_map<std::string, HdMask> masks;
};

static std::unordered_map<std::string, HdTextureState> sHdTextures;

struct HdTarget {
    HdTextureState* state;
    HdMask* mask;
};

// Reads which palette entry a single pixel uses.
static uint32_t CiTexelPaletteIndex(const Fast::Texture& tex, uint32_t index) {
    if (tex.Type == Fast::TextureType::Palette4bpp) {
        uint8_t byte = tex.ImageData[index / 2];
        return (index & 1) ? (byte & 0xF) : (byte >> 4);
    }
    return tex.ImageData[index];
}

// Finds a texture's HD replacement and which of its pixels we may touch.
static void CollectHdTarget(std::vector<HdTarget>& targets, const std::string& path, const std::string& maskKey,
                            uint32_t srcWidth, uint32_t srcHeight, const std::vector<bool>& affected, bool revert) {
    std::string altPath = Ship::IResource::gAltAssetPrefix + path;
    auto archiveManager = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
    if (archiveManager == nullptr || !archiveManager->HasFile(altPath)) {
        return;
    }

    auto stateIt = sHdTextures.find(altPath);
    if (stateIt == sHdTextures.end()) {
        if (revert) {
            return;
        }
        auto hd = LoadTextureExact(altPath);
        if (!IsRawRgba32Texture(hd)) {
            return;
        }
        HdTextureState state;
        state.texture = hd;
        state.original.assign(hd->ImageData, hd->ImageData + (size_t)hd->Width * hd->Height * 4);
        stateIt = sHdTextures.emplace(altPath, std::move(state)).first;
    }

    HdTextureState& state = stateIt->second;
    auto maskIt = state.masks.find(maskKey);
    if (maskIt == state.masks.end()) {
        if (revert) {
            return;
        }
        const Fast::Texture& hd = *state.texture;
        HdMask mask;
        for (uint32_t y = 0; y < hd.Height; y++) {
            uint32_t srcY = (uint32_t)((uint64_t)y * srcHeight / hd.Height);
            for (uint32_t x = 0; x < hd.Width; x++) {
                uint32_t srcX = (uint32_t)((uint64_t)x * srcWidth / hd.Width);
                uint32_t index = y * hd.Width + x;
                if (affected[srcY * srcWidth + srcX] && state.original[index * 4 + 3] != 0) {
                    mask.pixels.push_back(index);
                }
            }
        }
        maskIt = state.masks.emplace(maskKey, std::move(mask)).first;
    }

    if (!maskIt->second.pixels.empty()) {
        targets.push_back({ &state, &maskIt->second });
    }
}

// Finds every HD texture affected by changing this palette.
static std::vector<HdTarget> ResolveHdTargets(const std::string& path, uint32_t begin, uint32_t end, bool revert) {
    std::vector<HdTarget> targets;
    std::string maskKey = std::to_string(begin) + "-" + std::to_string(end);

    auto readers = sTlutReaders.find(path);
    if (readers == sTlutReaders.end()) {
        auto src = LoadTextureExact(path);
        if (!IsNativeEntryTexture(src, end + 1) || src->Width == 0 || src->Height == 0) {
            return targets;
        }
        uint32_t texelCount = (uint32_t)src->Width * src->Height;
        std::vector<bool> affected(texelCount, false);
        for (uint32_t i = begin; i <= end && i < texelCount; i++) {
            affected[i] = true;
        }
        CollectHdTarget(targets, path, maskKey, src->Width, src->Height, affected, revert);
        return targets;
    }

    for (const std::string& texPath : readers->second) {
        auto src = LoadTextureExact(texPath);
        if (!IsNativeCiTexture(src)) {
            continue;
        }
        uint32_t texelCount = (uint32_t)src->Width * src->Height;
        std::vector<bool> affected(texelCount, false);
        bool anyAffected = false;
        for (uint32_t i = 0; i < texelCount; i++) {
            uint32_t paletteIndex = CiTexelPaletteIndex(*src, i);
            if (paletteIndex >= begin && paletteIndex <= end) {
                affected[i] = true;
                anyAffected = true;
            }
        }
        if (anyAffected) {
            CollectHdTarget(targets, texPath, maskKey, src->Width, src->Height, affected, revert);
        }
    }
    return targets;
}

// Works out how bright each HD pixel should end up. Cached per texture.
static void PrepareHdWhitenLevels(const HdTextureState& state, HdMask& mask, SHADE_MODE mode) {
    if (mask.whitenMode == mode && !mask.whitenLevels.empty()) {
        return;
    }
    const uint8_t* original = state.original.data();

    uint32_t maxR = 0;
    uint32_t maxG = 0;
    uint32_t maxB = 0;
    for (uint32_t index : mask.pixels) {
        const uint8_t* pixel = &original[index * 4];
        maxR = MAX(maxR, pixel[0]);
        maxG = MAX(maxG, pixel[1]);
        maxB = MAX(maxB, pixel[2]);
    }

    mask.whitenLevels.resize(mask.pixels.size());
    for (size_t i = 0; i < mask.pixels.size(); i++) {
        const uint8_t* pixel = &original[mask.pixels[i] * 4];
        int32_t diffR = maxR - pixel[0];
        int32_t diffG = maxG - pixel[1];
        int32_t diffB = maxB - pixel[2];
        int32_t diff = 0;
        if (mode == MODE_AVG) {
            diff = (diffR + diffG + diffB) / 3;
        } else if (mode == MODE_MIN) {
            diff = MIN(MIN(diffR, diffG), diffB);
        } else if (mode == MODE_MAX) {
            diff = MAX(MAX(diffR, diffG), diffB);
        }
        mask.whitenLevels[i] = (uint8_t)CLAMP(255 - diff, 0, 255);
    }
    mask.whitenMode = mode;
}

// Works out how far along a two-color fade each HD pixel sits. Cached per texture.
static void PrepareHdGradientPositions(const HdTextureState& state, HdMask& mask, Color_RGBA8 oldBase,
                                       Color_RGBA8 targetEnd) {
    if (mask.hasGradient && SameColor(mask.gradientOldBase, oldBase) && SameColor(mask.gradientTargetEnd, targetEnd)) {
        return;
    }
    const uint8_t* original = state.original.data();

    double gradientRed = targetEnd.r - oldBase.r;
    double gradientGreen = targetEnd.g - oldBase.g;
    double gradientBlue = targetEnd.b - oldBase.b;
    double gradient = gradientRed * gradientRed + gradientGreen * gradientGreen + gradientBlue * gradientBlue;

    mask.gradientPositions.resize(mask.pixels.size());
    for (size_t i = 0; i < mask.pixels.size(); i++) {
        const uint8_t* pixel = &original[mask.pixels[i] * 4];
        double projection = gradientRed * (pixel[0] - oldBase.r) + gradientGreen * (pixel[1] - oldBase.g) +
                            gradientBlue * (pixel[2] - oldBase.b);
        mask.gradientPositions[i] = gradient != 0.0 ? (float)(projection / gradient) : 0.0f;
    }
    mask.hasGradient = true;
    mask.gradientOldBase = oldBase;
    mask.gradientTargetEnd = targetEnd;
}

// Marks which HD pixels pass the tint filter and how bright they are.
static void PrepareHdTintLevels(const HdTextureState& state, HdMask& mask, float minSaturation, float matchHue) {
    if (mask.hasTint && mask.tintMinSaturation == minSaturation && mask.tintMatchHue == matchHue) {
        return;
    }
    const uint8_t* original = state.original.data();

    mask.tintLevels.resize(mask.pixels.size());
    for (size_t i = 0; i < mask.pixels.size(); i++) {
        const uint8_t* pixel = &original[mask.pixels[i] * 4];
        float r = pixel[0] / 255.0f;
        float g = pixel[1] / 255.0f;
        float b = pixel[2] / 255.0f;
        mask.tintLevels[i] =
            PassesTintFilter(r, g, b, minSaturation, matchHue) ? MAX(MAX(pixel[0], pixel[1]), pixel[2]) : 0;
    }
    mask.hasTint = true;
    mask.tintMinSaturation = minSaturation;
    mask.tintMatchHue = matchHue;
}

// Rewrites an HD texture's pixels, or restores them when given no recolor.
static void RecolorHdPixels(const HdTarget& target, const HdRecolor* recolor) {
    HdTextureState& state = *target.state;
    HdMask& mask = *target.mask;
    uint8_t* data = state.texture->ImageData;

    if (recolor == nullptr) {
        const uint8_t* original = state.original.data();
        for (uint32_t index : mask.pixels) {
            memcpy(&data[index * 4], &original[index * 4], 4);
        }
    } else if (recolor->kind == HdRecolorKind::Whiten) {
        PrepareHdWhitenLevels(state, mask, recolor->mode);
        for (size_t i = 0; i < mask.pixels.size(); i++) {
            uint8_t* pixel = &data[mask.pixels[i] * 4];
            uint32_t level = mask.whitenLevels[i];
            pixel[0] = (uint8_t)((level * recolor->newBase.r) / 255);
            pixel[1] = (uint8_t)((level * recolor->newBase.g) / 255);
            pixel[2] = (uint8_t)((level * recolor->newBase.b) / 255);
        }
    } else if (recolor->kind == HdRecolorKind::Tint) {
        PrepareHdTintLevels(state, mask, recolor->tintMinSaturation, recolor->tintMatchHue);
        const uint8_t* original = state.original.data();
        for (size_t i = 0; i < mask.pixels.size(); i++) {
            uint8_t* pixel = &data[mask.pixels[i] * 4];
            uint32_t level = mask.tintLevels[i];
            if (level == 0) {
                memcpy(pixel, &original[mask.pixels[i] * 4], 4);
                continue;
            }
            pixel[0] = (uint8_t)((level * recolor->newBase.r) / 255);
            pixel[1] = (uint8_t)((level * recolor->newBase.g) / 255);
            pixel[2] = (uint8_t)((level * recolor->newBase.b) / 255);
        }
    } else {
        PrepareHdGradientPositions(state, mask, recolor->oldBase, recolor->targetEnd);
        float baseR = recolor->newBase.r;
        float baseG = recolor->newBase.g;
        float baseB = recolor->newBase.b;
        float spanR = recolor->targetEnd.r - baseR;
        float spanG = recolor->targetEnd.g - baseG;
        float spanB = recolor->targetEnd.b - baseB;
        for (size_t i = 0; i < mask.pixels.size(); i++) {
            uint8_t* pixel = &data[mask.pixels[i] * 4];
            float position = mask.gradientPositions[i];
            pixel[0] = (uint8_t)CLAMP(baseR + position * spanR, 0.0f, 255.0f);
            pixel[1] = (uint8_t)CLAMP(baseG + position * spanG, 0.0f, 255.0f);
            pixel[2] = (uint8_t)CLAMP(baseB + position * spanB, 0.0f, 255.0f);
        }
    }

    Gfx_TextureCacheDelete(data);
}

// Same, but for the HD textures reading that palette.
static void ShadeHdPaletteNewBase(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase,
                                  SHADE_MODE mode) {
    HdRecolor recolor = { HdRecolorKind::Whiten, mode, newBase };
    for (const HdTarget& hdTarget : ResolveHdTargets(path, begin, end, mode == MODE_REVERT)) {
        RecolorHdPixels(hdTarget, mode == MODE_REVERT ? nullptr : &recolor);
    }
}

//--------------------------------------------------------------------------------------------------------------------
// public functions (used by CosmeticEditor)
//--------------------------------------------------------------------------------------------------------------------

static const Color_RGBA8 whiteBase = { 255, 255, 255, 255 };

// Recolors a range of palette entries, keeping how light or dark each one was.
static void ShadePaletteNewBase(const PaletteTarget& target, uint32_t begin, uint32_t end, Color_RGBA8 newBase,
                                SHADE_MODE mode) {
    uint8_t* data = target.data;

    uint32_t maxR = 0;
    uint32_t maxG = 0;
    uint32_t maxB = 0;

    for (uint32_t i = begin; i <= end; i++) {
        UnpatchPalette(target, i);

        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        maxR = MAX(maxR, r);
        maxG = MAX(maxG, g);
        maxB = MAX(maxB, b);
    }

    if (mode == MODE_REVERT) {
        return;
    }

    for (uint32_t i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        int8_t diffR = maxR - r;
        int8_t diffG = maxG - g;
        int8_t diffB = maxB - b;

        int8_t diff = 0;

        if (mode == MODE_AVG) {
            diff = (diffR + diffG + diffB) / 3;
        } else if (mode == MODE_MIN) {
            diff = MIN(MIN(diffR, diffG), diffB);
        } else if (mode == MODE_MAX) {
            diff = MAX(MAX(diffR, diffG), diffB);
        }

        diff = MIN(MAX(31 - diff, 0), 31);

        r = (diff * newBase.r) / 255;
        g = (diff * newBase.g) / 255;
        b = (diff * newBase.b) / 255;

        PatchPalette(target, i, r, g, b);
    }
}

// Recolors a palette by path, both the native one and any HD replacements.
void ShadePaletteNewBase(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase, SHADE_MODE mode) {
    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data != nullptr && target.original != nullptr) {
        ShadePaletteNewBase(target, begin, end, newBase, mode);
        InvalidateNativeTextureCache(path, target);
    }

    ShadeHdPaletteNewBase(path, begin, end, newBase, mode);
}

// Bleaches a palette white so a display list color can tint it instead.
void ShadePaletteWhite(const char* path, uint32_t begin, uint32_t end, SHADE_MODE mode) {
    ShadePaletteNewBase(path, begin, end, whiteBase, mode);
}

// Puts a palette back to how it shipped.
void ShadePaletteRevert(const char* path, uint32_t begin, uint32_t end) {
    ShadePaletteNewBase(path, begin, end, whiteBase, MODE_REVERT);
}

// Recolors a palette that fades between two colors, swapping out the start color.
void ShadePaletteGradient(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 oldBase, Color_RGBA8 newBase,
                          Color_RGBA8 targetEnd) {
    HdRecolor recolor = { HdRecolorKind::Gradient, MODE_AVG, newBase, oldBase, targetEnd };
    for (const HdTarget& hdTarget : ResolveHdTargets(path, begin, end, false)) {
        RecolorHdPixels(hdTarget, &recolor);
    }

    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data == nullptr || target.original == nullptr) {
        return;
    }

    ShadePaletteNewBase(target, begin, end, whiteBase, MODE_REVERT);

    // Convert 0-255 range to 0-31 range
    newBase.r >>= 3;
    newBase.g >>= 3;
    newBase.b >>= 3;
    newBase.a >>= 3;
    targetEnd.r >>= 3;
    targetEnd.g >>= 3;
    targetEnd.b >>= 3;
    targetEnd.a >>= 3;
    oldBase.r >>= 3;
    oldBase.g >>= 3;
    oldBase.b >>= 3;
    oldBase.a >>= 3;

    uint8_t* data = target.data;
    for (uint32_t i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        Color_RGBA8 currentColor = { r, g, b, a };
        Color_RGBA8 newColor = mapNewBaseColorToGradient(currentColor, oldBase, newBase, targetEnd);
        PatchPalette(target, i, newColor.r, newColor.g, newColor.b);
    }
    InvalidateNativeTextureCache(path, target);
}

// Recolors a texture that has no palette, keeping how light or dark each pixel was.
void ShadeRGBA16NewBase(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase, SHADE_MODE mode) {
    ShadeHdPaletteNewBase(path, begin, end, newBase, mode);

    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data == nullptr || target.original == nullptr) {
        return;
    }

    uint8_t* data = target.data;

    uint32_t maxR = 0;
    uint32_t maxG = 0;
    uint32_t maxB = 0;

    for (uint32_t i = begin; i <= end; i++) {
        UnpatchPalette(target, i);

        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        if (a == 0) {
            continue; // skip transparent pixels
        }

        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        maxR = MAX(maxR, r);
        maxG = MAX(maxG, g);
        maxB = MAX(maxB, b);
    }

    if (mode == MODE_REVERT) {
        InvalidateNativeTextureCache(path, target);
        return;
    }

    for (uint32_t i = begin; i <= end; i++) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        if (a == 0) {
            PatchRGBA16Pixel(target, i, 0, 0, 0);
            continue;
        }

        int8_t diffR = maxR - r;
        int8_t diffG = maxG - g;
        int8_t diffB = maxB - b;

        int8_t diff = 0;
        if (mode == MODE_AVG) {
            diff = (diffR + diffG + diffB) / 3;
        } else if (mode == MODE_MIN) {
            diff = MIN(MIN(diffR, diffG), diffB);
        } else if (mode == MODE_MAX) {
            diff = MAX(MAX(diffR, diffG), diffB);
        }

        diff = MIN(MAX(31 - diff, 0), 31);

        r = (diff * newBase.r) / 255;
        g = (diff * newBase.g) / 255;
        b = (diff * newBase.b) / 255;

        PatchRGBA16Pixel(target, i, r, g, b);
    }
    InvalidateNativeTextureCache(path, target);
}

// Recolors only the colorful pixels of a texture, leaving greys and whites alone.
void ShadeRGBA16Recolor(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 newBase, f32 minSaturation) {
    HdRecolor recolor = { HdRecolorKind::Tint, MODE_AVG, newBase };
    recolor.tintMinSaturation = minSaturation;
    for (const HdTarget& hdTarget : ResolveHdTargets(path, begin, end, false)) {
        RecolorHdPixels(hdTarget, &recolor);
    }

    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data == nullptr || target.original == nullptr) {
        return;
    }

    uint8_t* data = target.data;

    auto readPixel = [&](uint32_t i, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
        uint16_t col16 = (data[i * 2] << 8) | data[i * 2 + 1];

        *r = col16 >> 11;
        *g = (col16 >> 6) & 0x1f;
        *b = (col16 >> 1) & 0x1f;
        *a = col16 & 1;
    };

    auto isRecolored = [&](uint8_t r, uint8_t g, uint8_t b, uint8_t* outValue) {
        uint8_t value = MAX(MAX(r, g), b);
        uint8_t minChannel = MIN(MIN(r, g), b);

        *outValue = value;
        if (value == 0) {
            return false;
        }
        return ((value - minChannel) / (f32)value) >= minSaturation;
    };

    for (uint32_t i = begin; i <= end; i++) {
        uint8_t r, g, b, a, value;

        UnpatchPalette(target, i);
        readPixel(i, &r, &g, &b, &a);

        if (a == 0) {
            PatchRGBA16Pixel(target, i, 0, 0, 0);
            continue;
        }

        if (!isRecolored(r, g, b, &value)) {
            continue;
        }

        f32 brightness = value / 31.0f;

        PatchRGBA16Pixel(target, i, (newBase.r >> 3) * brightness, (newBase.g >> 3) * brightness,
                         (newBase.b >> 3) * brightness);
    }
    InvalidateNativeTextureCache(path, target);
}

// Puts a palette-less texture back to how it shipped.
void ShadeRGBA16Revert(const char* path, uint32_t begin, uint32_t end) {
    ShadeRGBA16NewBase(path, begin, end, whiteBase, MODE_REVERT);
}

// Recolors only the palette entries that are roughly one particular hue.
void ShadePaletteTintMatchingHue(const char* path, uint32_t begin, uint32_t end, Color_RGBA8 match,
                                 Color_RGBA8 newBase) {
    float matchHue = HueOf(match.r / 255.0f, match.g / 255.0f, match.b / 255.0f);

    // HD replacements of the textures reading this palette get the same hue test per pixel.
    HdRecolor recolor = { HdRecolorKind::Tint, MODE_AVG, newBase };
    recolor.tintMatchHue = matchHue;
    for (const HdTarget& hdTarget : ResolveHdTargets(path, begin, end, false)) {
        RecolorHdPixels(hdTarget, &recolor);
    }

    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data == nullptr || target.original == nullptr) {
        return;
    }

    for (uint32_t i = begin; i <= end; i++) {
        UnpatchPalette(target, i);

        uint16_t col16 = (target.data[i * 2] << 8) | target.data[i * 2 + 1];
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;

        if (!PassesTintFilter(r / 31.0f, g / 31.0f, b / 31.0f, 0.0f, matchHue)) {
            continue;
        }

        uint8_t brightness = MAX(MAX(r, g), b);

        PatchPalette(target, i, (newBase.r >> 3) * brightness / 31, (newBase.g >> 3) * brightness / 31,
                     (newBase.b >> 3) * brightness / 31);
    }
    InvalidateNativeTextureCache(path, target);
}

// Puts the entries ShadePaletteTintMatchingHue changed back.
void ShadePaletteTintRevert(const char* path, uint32_t begin, uint32_t end) {
    for (const HdTarget& hdTarget : ResolveHdTargets(path, begin, end, true)) {
        RecolorHdPixels(hdTarget, nullptr);
    }

    PaletteTarget target = ResolvePaletteTarget(path, end);
    if (target.data == nullptr || target.original == nullptr) {
        return;
    }

    for (uint32_t i = begin; i <= end; i++) {
        UnpatchPalette(target, i);
    }
    InvalidateNativeTextureCache(path, target);
}

static uint8_t sKafeiHairTlut[256 * 2];
static OriginalTextureData sKafeiHairTlutOriginal;
static bool sKafeiHairTlutInitialized = false;
static const char* kKafeiBody2TlutPath = "objects/object_test3/gKafeiBody2TLUT";

// Bleaches Kafei a private palette so his hair recolors without his clothes.
uint8_t* ShadeKafeiHairTlut() {
    ShadeHdPaletteNewBase(kKafeiBody2TlutPath, 1, 3, whiteBase, MODE_MIN);
    ShadeHdPaletteNewBase(kKafeiBody2TlutPath, 8, 255, whiteBase, MODE_MIN);

    if (!sKafeiHairTlutInitialized) {
        auto tlut = LoadTextureExact(kKafeiBody2TlutPath);
        if (tlut == nullptr || tlut->GetInitData()->IsCustom || !IsNativeEntryTexture(tlut, 256)) {
            return nullptr;
        }
        memcpy(sKafeiHairTlut, tlut->ImageData, sizeof(sKafeiHairTlut));
        sKafeiHairTlutInitialized = true;
    }

    PaletteTarget hairTlut = { sKafeiHairTlut, &sKafeiHairTlutOriginal };
    ShadePaletteNewBase(hairTlut, 1, 3, whiteBase, MODE_MIN);
    ShadePaletteNewBase(hairTlut, 8, 255, whiteBase, MODE_MIN);
    InvalidateNativeTextureCache(kKafeiBody2TlutPath, hairTlut);

    return sKafeiHairTlut;
}

// Puts Kafei's HD hair textures back.
void ShadeKafeiHairTlutRevert() {
    ShadeHdPaletteNewBase(kKafeiBody2TlutPath, 1, 3, whiteBase, MODE_REVERT);
    ShadeHdPaletteNewBase(kKafeiBody2TlutPath, 8, 255, whiteBase, MODE_REVERT);
}
