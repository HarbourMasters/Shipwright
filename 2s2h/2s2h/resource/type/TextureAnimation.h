#pragma once
#include <ship/resource/Resource.h>

namespace SOH {
enum class TextureAnimationParamsType {
    /* 0 */ SingleScroll,
    /* 1 */ DualScroll,
    /* 2 */ ColorChange,
    /* 3 */ ColorChangeLERP,
    /* 4 */ ColorChangeLagrange,
    /* 5 */ TextureCycle,
    /* 6 */ Empty // An empty TextureAnimation has the form 00 00 00 06 00000000
};

class ZTextureAnimationParams {
  public:
    TextureAnimationParamsType type;
};

struct TextureScrollingParamsEntry {
    int8_t xStep;
    int8_t yStep;
    uint8_t width;
    uint8_t height;
};

class TextureScrollingParams : public ZTextureAnimationParams {
  public:
    int count;                           // 1 for Single, 2 for Dual
    TextureScrollingParamsEntry rows[2]; // Too small to make a vector worth it
};

struct F3DPrimColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
    uint8_t lodFrac;
};

struct F3DEnvColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class TextureColorChangingParams : public ZTextureAnimationParams {
  public:
    uint16_t animLength; // size of list for type 2
    uint16_t colorListCount;
    std::vector<F3DPrimColor> primColorList;
    std::vector<F3DEnvColor> envColorList;
    std::vector<uint16_t> frameDataList;
};

class TextureCyclingParams : public ZTextureAnimationParams {
  public:
    uint16_t cycleLength;
    std::vector<void*> textureList;
    std::vector<uint8_t> textureIndexList;
};

typedef struct {
    /* 0x0 */ uint16_t keyFrameLength;
    /* 0x2 */ uint16_t keyFrameCount;
    /* 0x4 */ F3DPrimColor* primColors;
    /* 0x8 */ F3DEnvColor* envColors;
    /* 0xC */ uint16_t* keyFrames;
} AnimatedMatColorParams; // size = 0x10

typedef struct {
    /* 0x0 */ int8_t xStep;
    /* 0x1 */ int8_t yStep;
    /* 0x2 */ uint8_t width;
    /* 0x3 */ uint8_t height;
} AnimatedMatTexScrollParams; // size = 0x4

typedef struct {
    /* 0x0 */ uint16_t keyFrameLength;
    /* 0x4 */ void** textureList;
    /* 0x8 */ uint8_t* textureIndexList;
} AnimatedMatTexCycleParams; // size = 0xC

typedef struct {
    /* 0x0 */ int8_t segment;
    /* 0x2 */ int16_t type;
    /* 0x4 */ void* params;
} AnimatedMaterial; // size = 0x8

class TextureAnimation : public Ship::Resource<AnimatedMaterial> {
  public:
    using Resource::Resource;
    ~TextureAnimation();

    TextureAnimation() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    AnimatedMaterial* GetPointer();
    size_t GetPointerSize();

    std::vector<AnimatedMaterial> anims;
    std::vector<std::string> textureCycleTextures;
};
} // namespace SOH
