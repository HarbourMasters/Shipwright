/*
 * File: z_en_oe2.c
 * Overlay: ovl_En_Oe2
 * Description: Blue Navi Target Spot
 */

#include "z_en_oe2.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnOE2_Init(Actor* thisx, PlayState* play);
void EnOE2_Destroy(Actor* thisx, PlayState* play);
void EnOE2_Update(Actor* thisx, PlayState* play);
void EnOE2_Draw(Actor* thisx, PlayState* play);

void EnOE2_DoNothing(EnOE2* this, PlayState* play);

const ActorInit En_OE2_InitVars = {
    ACTOR_EN_OE2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OE2,
    sizeof(EnOE2),
    (ActorFunc)EnOE2_Init,
    (ActorFunc)EnOE2_Destroy,
    (ActorFunc)EnOE2_Update,
    (ActorFunc)EnOE2_Draw,
    NULL,
};

void EnOE2_SetupAction(EnOE2* this, EnOE2ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnOE2_Init(Actor* thisx, PlayState* play) {
    EnOE2* this = (EnOE2*)thisx;

    EnOE2_SetupAction(this, EnOE2_DoNothing);
}

void EnOE2_Destroy(Actor* thisx, PlayState* play) {
}

void EnOE2_DoNothing(EnOE2* this, PlayState* play) {
}

void EnOE2_Update(Actor* thisx, PlayState* play) {
}

void EnOE2_Draw(Actor* thisx, PlayState* play) {
}
