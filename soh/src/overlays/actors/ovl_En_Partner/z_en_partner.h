#ifndef Z_EN_PARTNER_H
#define Z_EN_PARTNER_H

#include <libultraship/libultra.h>
#include "global.h"
#include <overlays/actors/ovl_En_Boom/z_en_boom.h>

struct EnPartner;

typedef void (*EnPartnerActionFunc)(struct EnPartner*, PlayState*);

typedef struct EnPartner {
    Actor actor;

    SkelAnime skelAnime;

    Vec3s jointTable[15];
    Vec3s morphTable[15];

    ColliderCylinder collider;

    Color_RGBAf innerColor;
    Color_RGBAf outerColor;
    LightInfo lightInfoGlow;
    LightNode* lightNodeGlow;
    LightInfo lightInfoNoGlow;
    LightNode* lightNodeNoGlow;

    f32 yVelocity;

    u8 canMove;
    u8 usedItem;
    u8 usedItemButton;
    u8 usedSpell;
    u8 damageTimer;
    s16 magicTimer;

    u8 shouldDraw;
    s16 itemTimer;

    GetItemEntry entry;
    WeaponInfo stickWeaponInfo;

    EnBoom* boomerangActor;
    Actor* hookshotTarget;
} EnPartner;

#endif