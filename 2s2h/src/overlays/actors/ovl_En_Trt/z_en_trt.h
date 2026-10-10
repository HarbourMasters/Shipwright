#ifndef Z_EN_TRT_H
#define Z_EN_TRT_H

#include "global.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"

struct EnTrt;

typedef void (*EnTrtActionFunc)(struct EnTrt*, PlayState*);
typedef void (*EnTrtBlinkFunc)(struct EnTrt*);

typedef struct EnTrt {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnTrtActionFunc actionFunc;
    /* 0x148 */ EnTrtActionFunc prevActionFunc;
    /* 0x14C */ SkelAnime skelAnime;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ UNK_TYPE1 pad1DC[0x154];
    /* 0x330 */ s16 delayTimer;
    /* 0x332 */ s16 sleepSoundTimer;
    /* 0x334 */ s16 cutsceneState;
    /* 0x338 */ s32 stickAccumX;
    /* 0x33C */ s32 stickAccumY;
    /* 0x340 */ EnGirlA* items[3];
    /* 0x34C */ u8 cursorIndex;
    /* 0x350 */ StickDirectionPrompt stickLeftPrompt;
    /* 0x388 */ StickDirectionPrompt stickRightPrompt;
    /* 0x3C0 */ f32 shopItemSelectedTween;
    /* 0x3C4 */ f32 cursorAnimTween;
    /* 0x3C8 */ u8 cursorAnimState;
    /* 0x3CC */ f32 arrowAnimTween;
    /* 0x3D0 */ f32 stickAnimTween;
    /* 0x3D4 */ u8 arrowAnimState;
    /* 0x3D5 */ u8 stickAnimState;
    /* 0x3D6 */ s16 blinkTimer;
    /* 0x3D8 */ s16 eyeTexIndex;
    /* 0x3DC */ EnTrtBlinkFunc blinkFunc;
    /* 0x3E0 */ s16 csId;
    /* 0x3E2 */ s16 tmpGetMushroomCsId;
    /* 0x3E4 */ Vec3f cursorPos;
    /* 0x3F0 */ Color_RGBAu32 cursorColor;
    /* 0x400 */ u8 drawCursor;
    /* 0x402 */ s16 timer;
    /* 0x404 */ s16 animIndex;
    /* 0x406 */ u16 textId;
    /* 0x408 */ u16 prevTextId;
    /* 0x40A */ u16 talkOptionTextId;
    /* 0x40C */ s16 lookForwardCutscene;
    /* 0x40E */ s16 lookToShelfCsId;
    /* 0x410 */ s16 lookToShopkeeperCsId;
    /* 0x412 */ s16 getMushroomCsId;
    /* 0x414 */ UNK_TYPE1 pad414[0x2];
    /* 0x416 */ Vec3s headRot;
    /* 0x41C */ Vec3f headPos;
    /* 0x428 */ s16 headPitch;
    /* 0x42A */ s16 headYaw;
    /* 0x42C */ u16 flags;
} EnTrt; // size = 0x430

typedef enum {
    /* 0 */ ENTRT_CUTSCENESTATE_STOPPED,
    /* 1 */ ENTRT_CUTSCENESTATE_WAITING,
    /* 2 */ ENTRT_CUTSCENESTATE_PLAYING_SPECIAL, // Used to do/set extra flags (such as waiting mid animation)
    /* 3 */ ENTRT_CUTSCENESTATE_PLAYING
} EnTrtCutsceneState;

#endif // Z_EN_TRT_H
