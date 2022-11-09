/*
 * File: z_bg_gnd_nisekabe.c
 * Overlay: ovl_Bg_Gnd_Nisekabe
 * Description: Ganon's Castle Fake Wall
 */

#include "z_bg_gnd_nisekabe.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS ACTOR_FLAG_4

void BgGndNisekabe_Init(Actor* thisx, PlayState* play);
void BgGndNisekabe_Destroy(Actor* thisx, PlayState* play);
void BgGndNisekabe_Update(Actor* thisx, PlayState* play);
void BgGndNisekabe_Draw(Actor* thisx, PlayState* play);

const ActorInit Bg_Gnd_Nisekabe_InitVars = {
    ACTOR_BG_GND_NISEKABE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_DEMO_KEKKAI,
    sizeof(BgGndNisekabe),
    (ActorFunc)BgGndNisekabe_Init,
    (ActorFunc)BgGndNisekabe_Destroy,
    (ActorFunc)BgGndNisekabe_Update,
    (ActorFunc)BgGndNisekabe_Draw,
    NULL,
};

void BgGndNisekabe_Init(Actor* thisx, PlayState* play) {
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;

    Actor_SetScale(&this->actor, 0.1);
    this->actor.uncullZoneForward = 3000.0;
}

void BgGndNisekabe_Destroy(Actor* thisx, PlayState* play) {
}

void BgGndNisekabe_Update(Actor* thisx, PlayState* play) {
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;

    if (play->actorCtx.lensActive) {
        this->actor.flags |= ACTOR_FLAG_7;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_7;
    }
}

void BgGndNisekabe_Draw(Actor* thisx, PlayState* play) {
    static Gfx* dLists[] = {
        gLightTrialFakeWallDL,
        gGanonsCastleUnusedFakeWallDL,
        gGanonsCastleScrubsFakeWallDL,
    };
    BgGndNisekabe* this = (BgGndNisekabe*)thisx;
    u32 index = this->actor.params & 0xFF;

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) {
        Gfx_DrawDListXlu(play, dLists[index]);
    } else {
        Gfx_DrawDListOpa(play, dLists[index]);
    }
}
