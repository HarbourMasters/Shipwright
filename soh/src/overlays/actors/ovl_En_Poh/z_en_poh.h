#ifndef Z_EN_POH_H
#define Z_EN_POH_H

#include "ultra64.h"
#include "global.h"

struct EnPoh;

typedef void (*EnPohActionFunc)(struct EnPoh*, PlayState*);

typedef enum {
    EN_POH_NORMAL,
    EN_POH_RUPEE,
    EN_POH_SHARP,
    EN_POH_FLAT
} EnPohType;

typedef enum {
    EN_POH_INFO_NORMAL,
    EN_POH_INFO_COMPOSER
} EnPohInfoType;

typedef struct {
    /* 0x0000 */ Color_RGB8 primColor;
    /* 0x0003 */ Color_RGB8 lightColor;
    /* 0x0006 */ u8 unk_6; // limb index
    /* 0x0006 */ u8 unk_7; // limb index
    /* 0x0008 */ s8 unk_8; // rate of some kind
    /* 0x000C */ AnimationHeader* idleAnim;
    /* 0x0010 */ AnimationHeader* idleAnim2;
    /* 0x0014 */ AnimationHeader* damageAnim;
    /* 0x0018 */ AnimationHeader* fleeAnim;
    /* 0x001C */ Gfx* lanternDisplayList;
    /* 0x0020 */ Gfx* burnDisplayList;
    /* 0x0024 */ Gfx* soulDisplayList;
} EnPohInfo; // size = 0x28

typedef struct EnPoh {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnPohActionFunc actionFunc;
    /* 0x0194 */ u8 unk_194;
    /* 0x0195 */ u8 unk_195;
    /* 0x0196 */ s8 objectIdx;
    /* 0x0197 */ u8 infoIdx;
    /* 0x0198 */ s16 unk_198;
    /* 0x019A */ s16 visibilityTimer;
    /* 0x019C */ s16 unk_19C;
    /* 0x019E */ Vec3s jointTable[21];
    /* 0x021C */ Vec3s morphTable[21];
    /* 0x029A */ Color_RGBA8 lightColor;
    /* 0x029E */ Color_RGBA8 envColor;
    /* 0x02A4 */ EnPohInfo* info;
    /* 0x02A8 */ LightNode* lightNode;
    /* 0x02AC */ LightInfo lightInfo;
    /* 0x02BC */ ColliderCylinder colliderCyl;
    /* 0x0308 */ ColliderJntSph colliderSph;
    /* 0x0328 */ ColliderJntSphElement colliderSphItem;
    /* 0x0368 */ MtxF unk_368;
} EnPoh; // size = 0x03A8

#endif
