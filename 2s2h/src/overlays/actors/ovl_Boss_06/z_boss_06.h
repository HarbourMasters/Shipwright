#ifndef Z_BOSS_06_H
#define Z_BOSS_06_H

#include "global.h"

struct Boss06;

typedef void (*Boss06ActionFunc)(struct Boss06*, PlayState*);

#define ENBOSS06_GET_PARAMS(thisx) ((thisx)->params)

#define BOSS06_CURTAIN_TEX_WIDTH  64
#define BOSS06_CURTAIN_TEX_HEIGHT 64
#define BOSS06_CURTAIN_TEX_SIZE ((BOSS06_CURTAIN_TEX_WIDTH * BOSS06_CURTAIN_TEX_HEIGHT) / 2) // 64x64 CI4

typedef struct Boss06 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 drawFlags;
    /* 0x146 */ s16 unusedTimer1;
    /* 0x148 */ s16 unusedTimer2;
    /* 0x14C */ ColliderCylinder collider;
    /* 0x198 */ Boss06ActionFunc actionFunc;
    /* 0x19C */ f32 lightRayBrightness;
    /* 0x1A0 */ f32 lightRayTopVerticesOffset;
    /* 0x1A4 */ f32 lightRayBaseOffsetZ;
    /* 0x1A8 */ f32 curtainHeight;
    /* 0x1AC */ f32 lensFlareYOffset;
    /* 0x1B0 */ Vec3f arrowHitPos;
    /* 0x1BC */ Vec3f arrowHitPosScaled;
    /* 0x1C8 */ u8 updateFireEffects;
    /* 0x1C9 */ u8 csState;
    /* 0x1CA */ s16 csFrameCount;
    /* 0x1CC */ f32 fireEffectDistanceAdd;
    /* 0x1D0 */ f32 fireEffectDistanceScale2;
    /* 0x1D4 */ f32 fireEffectDistanceScale1;
    /* 0x1D8 */ f32 fireEffectScale;
    /* 0x1DC */ f32 lightOrbScale;
    /* 0x1E0 */ f32 lightOrbAlphaFactor;
    /* 0x1E4 */ f32 lensFlareScale;
                // Maybe the texture was given 64-byte alignment instead of 64-bit despite not requiring it?
                // However alignment > 0x10 inside an actor structure is not enough to guarantee overall memory
                // alignment since the zelda arena allocator assumes 0x10 alignment is sufficient for any allocation.
    /* 0x1E8 */ UNK_TYPE1 unk_1E8[0x18];
    union {
    /* 0x200 */ u8 curtainTexture[BOSS06_CURTAIN_TEX_SIZE];
                u64 force_structure_alignment; // This buffer is used as a texture so requires 64-bit memory alignment
    };
    /* 0xA00 */ s16 subCamId;
    /* 0xA04 */ Vec3f subCamEye;
    /* 0xA10 */ Vec3f subCamAt;
    /* 0xA1C */ Vec3f subCamMaxStep;
    /* 0xA28 */ f32 maxStepScale;
    /* 0xA2C */ f32 screenFillAlpha;
    // 2S2H [Port]
    /*       */ u8 curtainTexMask[64 * 64]; // [HD Textures] Mask for CPU modified texture
} Boss06; // size = 0xA30

#endif // Z_BOSS_06_H
