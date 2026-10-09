#pragma once

#include <ship/resource/Resource.h>
#include <libultraship/libultra/types.h>

namespace SOH {

typedef struct Vec3s {
    s16 x;
    s16 y;
    s16 z;
} Vec3s;

enum class KeyframeSkelType {
    Normal,
    Flex,
    Error,
};

// Keyframe limb?
typedef struct {
    /* 0x00 */ void* dList;
    /* 0x04 */ u8 numChildren;
    /* 0x05 */ u8 flags;
    /* 0x06 */ Vec3s translation; // FlexLimbs have translation data in their animations instead
} KeyFrameStandardLimb;           // size = 0xC

// Other limb type?
typedef struct {
    /* 0x00 */ void* dList;
    /* 0x04 */ u8 numChildren;
    /* 0x05 */ u8 flags;
    /* 0x06 */ u8 callbackIndex; // transform callback index
} KeyFrameFlexLimb;              // size = 0x8

typedef struct {
    /* 0x00 */ u8 limbCount;
    /* 0x01 */ u8 dListCount; // non-zero in object files, number of non-null-dlist limbs? used to know how many
                              // matrices to alloc for drawing
    /* 0x04 */ union {
        KeyFrameStandardLimb* limbsStandard;
        KeyFrameFlexLimb* limbsFlex;
    };
} KeyFrameSkeletonData; // Size = 0x8

typedef struct {
    /* 0x00 */ s16 frame;
    /* 0x02 */ s16 value;
    /* 0x04 */ s16 velocity;
} KeyFrame; // Size = 0x6

typedef struct {
    union {
        /* 0x00 */ u8* bitFlags; // bitflags indicating whether to do keyframe interpolation or pull from preset values
        /* 0x00 */ u16*
            bitFlagsFlex; // bitflags indicating whether to do keyframe interpolation or pull from preset values
    };
    /* 0x04 */ KeyFrame* keyFrames; // keyframes array
    /* 0x08 */ s16* kfNums;         // number of keyframes for each limb
    /* 0x0C */ s16* presetValues;   // preset rotation (standard) or scale/rotation/translation (flex) values
    /* 0x0C */ char unk_10[0x2];
    /* 0x12 */ s16 duration;
} KeyFrameAnimationData; // Size = 0x14

class KeyFrameSkel : public Ship::Resource<KeyFrameSkeletonData> {
  public:
    using Resource::Resource;

    KeyFrameSkel() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }
    ~KeyFrameSkel();

    KeyFrameSkeletonData* GetPointer();
    size_t GetPointerSize();

    KeyFrameSkeletonData skelData;
};

class KeyFrameAnim : public Ship::Resource<KeyFrameAnimationData> {
  public:
    using Resource::Resource;

    KeyFrameAnim() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }
    ~KeyFrameAnim();

    KeyFrameAnimationData* GetPointer();
    size_t GetPointerSize();

    KeyFrameAnimationData animData;
};

} // namespace SOH
