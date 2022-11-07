#ifndef Z_EN_RU1_H
#define Z_EN_RU1_H

#include "ultra64.h"
#include "global.h"

#include "overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

struct EnRu1;

typedef void (*EnRu1ActionFunc)(struct EnRu1*, PlayState*);
typedef void (*EnRu1DrawFunc)(struct EnRu1*, PlayState*);
typedef void (*EnRu1PreLimbDrawFunc)(struct EnRu1*, PlayState*, s32, Vec3s*);

typedef struct EnRu1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ s16 eyeIndex;
    /* 0x025E */ s16 blinkTimer;
    /* 0x0260 */ s16 mouthIndex;
    /* 0x0264 */ s32 action;
    /* 0x0268 */ s32 drawConfig;
    /* 0x026C */ f32 unk_26C;
    /* 0x0270 */ f32 unk_270;
    /* 0x0274 */ char unk_274[0x4];
    /* 0x0278 */ DoorWarp1* blueWarp;
    /* 0x027C */ f32 unk_27C;
    /* 0x0280 */ s32 unk_280;
    /* 0x0284 */ s8 roomNum1;
    /* 0x0285 */ s8 roomNum2;
    /* 0x0286 */ s8 roomNum3;
    /* 0x0288 */ f32 unk_288;
    /* 0x028C */ BgBdanObjects* unk_28C;
    /* 0x0290 */ s32 unk_290;
    /* 0x0294 */ char unk_294[0x4];
    /* 0x0298 */ s32 unk_298;
    /* 0x029C */ char unk_29C[0x2];
    /* 0x029E */ s16 unk_29E;
    /* 0x02A0 */ char unk_2A0[0x4];
    /* 0x02A4 */ f32 unk_2A4;
    /* 0x02A8 */ s32 alpha;
    /* 0x02AC */ s16 unk_2AC;
    /* 0x02B0 */ s32 unk_2B0;
    /* 0x02B4 */ ColliderCylinder collider;
    /* 0x0300 */ ColliderCylinder collider2;
    /* 0x034C */ s32 unk_34C;
    /* 0x0350 */ s32 unk_350;
    /* 0x0354 */ f32 unk_354;
    /* 0x0358 */ f32 unk_358;
    /* 0x035C */ s16 unk_35C;
    /* 0x0360 */ f32 unk_360;
    /* 0x0364 */ Vec3f unk_364;
    /* 0x0370 */ f32 unk_370;
    /* 0x0374 */ struct_80034A14_arg1 unk_374;
} EnRu1; // size = 0x039C

typedef enum {
    /* 0 */ RUTO_CHILD_ROOT,
    /* 1 */ RUTO_CHILD_LEFT_THIGH,
    /* 2 */ RUTO_CHILD_LEFT_SHIN,
    /* 3 */ RUTO_CHILD_LEFT_FOOT,
    /* 4 */ RUTO_CHILD_RIGHT_THIGH,
    /* 5 */ RUTO_CHILD_RIGHT_SHIN,
    /* 6 */ RUTO_CHILD_RIGHT_FOOT,
    /* 7 */ RUTO_CHILD_CHEST,
    /* 8 */ RUTO_CHILD_LEFT_UPPER_ARM,
    /* 9 */ RUTO_CHILD_LEFT_FIN,
    /* 10 */ RUTO_CHILD_LEFT_HAND,
    /* 11 */ RUTO_CHILD_RIGHT_UPPER_ARM,
    /* 12 */ RUTO_CHILD_RIGHT_FIN,
    /* 13 */ RUTO_CHILD_RIGHT_HAND,
    /* 14 */ RUTO_CHILD_HEAD,
    /* 15 */ RUTO_CHILD_TORSO
} RutoLimb;

#endif
