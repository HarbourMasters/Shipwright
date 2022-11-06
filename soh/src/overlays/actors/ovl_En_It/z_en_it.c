/*
 * File: z_en_it.c
 * Overlay: ovl_En_It
 * Description: Dampe's Minigame digging spot hitboxes
 */

#include "z_en_it.h"

#define FLAGS 0

void EnIt_Init(Actor* thisx, PlayState* play);
void EnIt_Destroy(Actor* thisx, PlayState* play);
void EnIt_Update(Actor* thisx, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 40, 10, 0, { 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_It_InitVars = {
    ACTOR_EN_IT,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnIt),
    (ActorFunc)EnIt_Init,
    (ActorFunc)EnIt_Destroy,
    (ActorFunc)EnIt_Update,
    (ActorFunc)NULL,
    NULL,
};

void EnIt_Init(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;

    this->actor.params = 0x0D05;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, 0, &sColChkInfoInit);
}

void EnIt_Destroy(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnIt_Update(Actor* thisx, PlayState* play) {
    EnIt* this = (EnIt*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}
