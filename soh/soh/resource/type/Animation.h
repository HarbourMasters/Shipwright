#pragma once

#include "Resource.h"
#include <libultraship/libultra/types.h>

namespace SOH {
enum class AnimationType {
    Normal = 0,
    Link = 1,
    Curve = 2,
    Legacy = 3,
};

struct RotationIndex {
    uint16_t x, y, z;

    RotationIndex(uint16_t nX, uint16_t nY, uint16_t nZ) : x(nX), y(nY), z(nZ) {
    }
};

typedef struct {
    /* 0x0000 */ u16 unk_00; // appears to be flags
    /* 0x0002 */ s16 unk_02;
    /* 0x0004 */ s16 unk_04;
    /* 0x0006 */ s16 unk_06;
    /* 0x0008 */ f32 unk_08;
} TransformData; // size = 0xC

typedef struct {
    /* 0x0000 */ u8* refIndex;
    /* 0x0004 */ TransformData* transformData;
    /* 0x0008 */ s16* copyValues;
    /* 0x000C */ s16 unk_0C;
    /* 0x000E */ s16 unk_0E;
} TransformUpdateIndex; // size = 0x10

typedef struct {
    /* 0x00 */ s16 frameCount;
} AnimationHeaderCommon;

// Index into the frame data table. 
typedef struct {
    /* 0x00 */ u16 x;
    /* 0x02 */ u16 y;
    /* 0x04 */ u16 z;
} JointIndex; // size = 0x06

typedef struct {
    /* 0x00 */ AnimationHeaderCommon common;
    /* 0x04 */ s16* frameData; // "tbl"
    /* 0x08 */ JointIndex* jointIndices; // "ref_tbl"
    /* 0x0C */ u16 staticIndexMax;
} AnimationHeader; // size = 0x10

typedef struct {
    /* 0x00 */ AnimationHeaderCommon common;
    /* 0x04 */ void* segment;
} LinkAnimationHeader; // size = 0x8

union AnimationData {
AnimationHeader animationHeader;
LinkAnimationHeader linkAnimationHeader;
TransformUpdateIndex transformUpdateIndex;
};

class Animation : public Ship::Resource<AnimationData> {
public:
    using Resource::Resource;

    Animation() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {}

    AnimationData* GetPointer();
    size_t GetPointerSize();

    AnimationType type;
    AnimationData animationData;

    // NORMAL
    std::vector<uint16_t> rotationValues;
    std::vector<RotationIndex> rotationIndices;

    // CURVE
    std::vector<uint8_t> refIndexArr;
    std::vector<TransformData> transformDataArr;
    std::vector<int16_t> copyValuesArr;
};
}; // namespace LUS
