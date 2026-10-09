#ifndef Z_EN_RAF_H
#define Z_EN_RAF_H

#include "global.h"
#include "objects/object_raf/object_raf.h"

#define CARNIVOROUS_LILY_GET_TYPE(thisx) (((thisx)->params >> 12) & 0xF)
#define CARNIVOROUS_LILY_GET_REVIVE_TIMER(thisx) (((thisx)->params >> 7) & 0x1F)
#define CARNIVOROUS_LILY_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#define CARNIVOROUS_LILY_SWITCH_FLAG_NONE 0x7F

typedef enum CarnivorousLilyType {
    /* 0 */ CARNIVOROUS_LILY_TYPE_NORMAL,
    /* 1 */ CARNIVOROUS_LILY_TYPE_DORMANT,              // Spawns without trap, so it can't eat bombs/player
    /* 2 */ CARNIVOROUS_LILY_TYPE_NO_WATER_INTERACTIONS // Won't produce ripples, and effects won't produce splashes
} CarnivorousLilyType;

struct EnRaf;

typedef void (*EnRafActionFunc)(struct EnRaf*, PlayState*);

typedef struct {
    /* 0x00 */ u8 isEnabled;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Vec3s rot;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 timer;
} CarnivorousLilyEffect; // size = 0x38

#define CARNIVOROUS_LILY_EFFECT_COUNT 31

typedef struct EnRaf {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ Vec3s jointTable[CARNIVOROUS_LILY_PAD_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[CARNIVOROUS_LILY_PAD_LIMB_MAX];
    /* 0x230 */ EnRafActionFunc actionFunc;
    /* 0x234 */ Vec3f limbScale[CARNIVOROUS_LILY_PAD_LIMB_MAX];
    /* 0x2C4 */ Vec3f targetLimbScale[CARNIVOROUS_LILY_PAD_LIMB_MAX];
    /* 0x354 */ Vec3s chewLimbRot[CARNIVOROUS_LILY_PAD_LIMB_MAX];
    /* 0x39C */ s16 grabTarget;
    /* 0x39E */ u8 isCurrentlyInRidingMovingState;
    /* 0x3A0 */ f32 animEndFrame;
    /* 0x3A4 */ f32 chewScale;
    /* 0x3A8 */ f32 bobOffset;
    /* 0x3AC */ f32 heightDiffFromPlayer;
    /* 0x3B0 */ f32 bobPhase;
    /* 0x3B4 */ s16 timer;
    /* 0x3B6 */ s16 dissolveTimer;
    /* 0x3B8 */ s16 rippleTimer;
    /* 0x3BA */ s16 reviveTimer;
    /* 0x3BC */ s16 playerRotYWhenGrabbed;
    /* 0x3BE */ s16 mainType;
    /* 0x3C0 */ s16 switchFlag;
    /* 0x3C2 */ s16 petalScaleType;
    /* 0x3C4 */ s16 chewCount;
    /* 0x3C6 */ s16 action;
    /* 0x3C8 */ s16 petalClearPixelFirstPassIndex;
    /* 0x3CA */ s16 teethClearPixelFirstPassIndex;
    /* 0x3CC */ s16 petalClearPixelSecondPassIndex;
    /* 0x3CE */ s16 teethClearPixelSecondPassIndex;
    /* 0x3D0 */ ColliderCylinder collider;
    /* 0x41C */ CarnivorousLilyEffect effects[CARNIVOROUS_LILY_EFFECT_COUNT];
} EnRaf; // size = 0xAE4

#endif // Z_EN_RAF_H
