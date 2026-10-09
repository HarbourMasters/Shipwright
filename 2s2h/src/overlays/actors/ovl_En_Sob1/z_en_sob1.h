#ifndef Z_EN_SOB1_H
#define Z_EN_SOB1_H

#include "global.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include "objects/object_rsn/object_rsn.h"
#include "objects/object_zo/object_zo.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"

#define ENSOB1_LIMB_MAX MAX(MAX((s32)ZORA_LIMB_MAX, (s32)BOMB_SHOPKEEPER_LIMB_MAX), (s32)GORON_LIMB_MAX)

struct EnSob1;

typedef void (*EnSob1ActionFunc)(struct EnSob1*, PlayState*);
typedef void (*EnSob1BlinkFunc)(struct EnSob1*);

#define ENSOB1_GET_SHOPTYPE(thisx) ((thisx)->params & 0x1F)
#define ENSOB1_GET_PATH_INDEX(thisx) (((thisx)->params & 0x3E0) >> 5)

#define ENSOB1_PATH_INDEX_NONE 0x1F

typedef struct EnSob1XZRange {
    /* 0x0 */ f32 xMin;
    /* 0x4 */ f32 xMax;
    /* 0x8 */ f32 zMin;
    /* 0xC */ f32 zMax;
} EnSob1XZRange; // size = 0x10

typedef struct EnSob1 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnSob1ActionFunc actionFunc;
    /* 0x18C */ EnSob1ActionFunc prevActionFunc; // Used to restore back to correct browsing function
    /* 0x190 */ EnSob1ActionFunc changeObjectFunc;
    /* 0x194 */ ColliderCylinder collider;
    /* 0x1E0 */ Path* path;
    /* 0x1E4 */ s32 waypoint;
    /* 0x1E8 */ s16 delayTimer;
    /* 0x1EA */ s8 mainObjectSlot;
    /* 0x1EB */ s8 unusedObjectSlot;
    /* 0x1EC */ s8 shopkeeperAnimObjectSlot;
    /* 0x1EE */ s16 headRot;
    /* 0x1F0 */ s16 headRotTarget;
    /* 0x1F2 */ Vec3s jointTable[ENSOB1_LIMB_MAX];
    /* 0x26A */ Vec3s morphTable[ENSOB1_LIMB_MAX];
    /* 0x2E2 */ s16 eyeTexIndex;
    /* 0x2E4 */ s16 blinkTimer;
    /* 0x2E8 */ EnSob1BlinkFunc blinkFunc;
    /* 0x2EC */ EnGirlA* items[3]; // Items on shelf are indexed as: /* 2 1 0 */
    /* 0x2F8 */ s32 stickAccumX;
    /* 0x2FC */ s32 stickAccumY;
    /* 0x300 */ Vec3f cursorPos;
    /* 0x30C */ Color_RGBAu32 cursorColor;
    /* 0x31C */ f32 cursorAnimTween;
    /* 0x320 */ u8 cursorAnimState;
    /* 0x321 */ u8 drawCursor;
    /* 0x322 */ u8 cursorIndex;
    /* 0x324 */ StickDirectionPrompt stickLeftPrompt;
    /* 0x35C */ StickDirectionPrompt stickRightPrompt;
    /* 0x394 */ f32 arrowAnimTween;
    /* 0x398 */ f32 stickAnimTween;
    /* 0x39C */ u8 arrowAnimState;
    /* 0x39D */ u8 stickAnimState;
    /* 0x39E */ s16 cutsceneState;
    /* 0x3A0 */ s16 csId;
    /* 0x3A2 */ s16 lookFowardCsId;
    /* 0x3A4 */ s16 lookToShelfCsId;
    /* 0x3A6 */ s16 lookToShopkeeperCsId;
    /* 0x3A8 */ UNK_TYPE1 pad3A8[0x4];
    /* 0x3AC */ f32 shopItemSelectedTween;
    /* 0x3B0 */ UNK_TYPE1 pad3B0[0x4];
    /* 0x3B4 */ u16 welcomeTextId;
    /* 0x3B6 */ u16 talkOptionTextId;
    /* 0x3B8 */ u16 goodbyeTextId;
    /* 0x3BA */ u8 wasTalkedToWhileWalking;
    /* 0x3BC */ EnSob1XZRange posXZRange;
    /* 0x3CC */ s16 shopType;
} EnSob1; // size = 0x3D0

typedef enum EnSob1ShopType {
    /* 0 */ ZORA_SHOP,
    /* 1 */ GORON_SHOP,
    /* 2 */ BOMB_SHOP,
    /* 3 */ GORON_SHOP_SPRING
} EnSob1ShopType;

typedef enum {
    /* 0 */ ENSOB1_CUTSCENESTATE_STOPPED,
    /* 1 */ ENSOB1_CUTSCENESTATE_WAITING,
    /* 2 */ ENSOB1_CUTSCENESTATE_PLAYING
} EnSob1CutsceneState;

#endif // Z_EN_SOB1_H
