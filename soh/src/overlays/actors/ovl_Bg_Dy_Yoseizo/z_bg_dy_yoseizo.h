#ifndef Z_BG_DY_YOSEIZO_H
#define Z_BG_DY_YOSEIZO_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Dy_Extra/z_en_dy_extra.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"

struct BgDyYoseizo;

typedef void (*BgDyYoseizoActionFunc)(struct BgDyYoseizo*, PlayState*);

typedef struct {
    /* 0x00 */ u8 alive; // drawn if 1, respawn if 0
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Color_RGB8 primColor;
    /* 0x2B */ Color_RGB8 envColor;
    /* 0x2E */ s16 alpha;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 timer; // lifetime
    /* 0x36 */ s16 type; // 0 is general radiance, else is directed towards Player
    /* 0x36 */ f32 pitch;
    /* 0x36 */ f32 yaw;
    /* 0x40 */ f32 roll;
    /* 0x44 */ u32 epoch;
} BgDyYoseizoParticle; // size = 0x48

typedef struct BgDyYoseizo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgDyYoseizoActionFunc actionFunc;
    /* 0x0150 */ SkelAnime skelAnime;
    /* 0x0194 */ Vec3s jointTable[28];
    /* 0x023C */ Vec3s morphTable[28];
    /* 0x02E4 */ u8 lightBallSpawned;
    /* 0x02E5 */ u8 giveDefenseHearts;
    /* 0x02E6 */ u8 healing;
    /* 0x02E8 */ s16 vanishTimer;
    /* 0x02EA */ s16 givingSpell;
    /* 0x02EC */ s16 fountainType;
    /* 0x02EE */ s16 dialogState;
    /* 0x02F0 */ s16 absoluteTimer;
    /* 0x02F2 */ s16 eyeState;
    /* 0x02F4 */ s16 eyeState2; // Used, but does not actually change the actor's eyes
    /* 0x02F6 */ s16 mouthState;
    /* 0x02F8 */ s16 blinkTimer;
    /* 0x02FA */ s16 unusedTimer;
    /* 0x02FC */ s16 animationChanged;
    /* 0x02FE */ s16 finishedSpinGrow;
    /* 0x02FE */ s16 itemSpawned;
    /* 0x0302 */ s16 healingTimer;
    /* 0x0304 */ s16 warpEffectSpawned;
    /* 0x0306 */ s16 refillTimer;
    /* 0x0308 */ f32 scale;
    /* 0x030C */ f32 grownHeight;
    /* 0x0310 */ f32 vanishHeight;
    /* 0x0314 */ f32 heightFraction;
    /* 0x0318 */ f32 scaleFraction;
    /* 0x031C */ f32 targetHeight;
    /* 0x0320 */ f32 bobOffset;
    /* 0x0324 */ f32 bobTimer;
    /* 0x0328 */ f32 heightOffset;
    /* 0x032C */ f32 frameCount;
    /* 0x0330 */ char unk_330[4];
    /* 0x0334 */ Vec3s headRot;
    /* 0x033A */ Vec3s torsoRot;
    /* 0x0340 */ EnDyExtra* beam;
    /* 0x0344 */ EnExItem* item;
    /* 0x0348 */ char unk_348[0x4C];
    /* 0x0394 */ BgDyYoseizoParticle particles[200];
} BgDyYoseizo; // size = 0x38B4

#endif
