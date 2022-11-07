/*
 * File: z_bg_menkuri_nisekabe.c
 * Overlay: ovl_Bg_Menkuri_Nisekabe
 * Description: False Stone Walls (Gerudo Training Grounds)
 */

#include "z_bg_menkuri_nisekabe.h"
#include "objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS 0

void BgMenkuriNisekabe_Init(Actor* thisx, PlayState* play);
void BgMenkuriNisekabe_Destroy(Actor* thisx, PlayState* play);
void BgMenkuriNisekabe_Update(Actor* thisx, PlayState* play);
void BgMenkuriNisekabe_Draw(Actor* thisx, PlayState* play);

const ActorInit Bg_Menkuri_Nisekabe_InitVars = {
    ACTOR_BG_MENKURI_NISEKABE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_MENKURI_OBJECTS,
    sizeof(BgMenkuriNisekabe),
    (ActorFunc)BgMenkuriNisekabe_Init,
    (ActorFunc)BgMenkuriNisekabe_Destroy,
    (ActorFunc)BgMenkuriNisekabe_Update,
    (ActorFunc)BgMenkuriNisekabe_Draw,
    NULL,
};

static Gfx* sDLists[] = { gGTGFakeWallDL, gGTGFakeCeilingDL };

void BgMenkuriNisekabe_Init(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
}

void BgMenkuriNisekabe_Destroy(Actor* thisx, PlayState* play) {
}

void BgMenkuriNisekabe_Update(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;

    if (play->actorCtx.lensActive) {
        this->actor.flags |= ACTOR_FLAG_7;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_7;
    }
}

void BgMenkuriNisekabe_Draw(Actor* thisx, PlayState* play) {
    BgMenkuriNisekabe* this = (BgMenkuriNisekabe*)thisx;
    u32 index = this->actor.params & 0xFF;

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_7)) {
        Gfx_DrawDListXlu(play, sDLists[index]);
    } else {
        Gfx_DrawDListOpa(play, sDLists[index]);
    }
}
