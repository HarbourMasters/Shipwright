/*
 * File: z_bg_menkuri_kaiten.c
 * Overlay: Bg_Menkuri_Kaiten
 * Description: Large rotating stone ring used in Gerudo Training Grounds and Forest Temple.
 */

#include "z_bg_menkuri_kaiten.h"
#include "objects/object_menkuri_objects/object_menkuri_objects.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMenkuriKaiten_Init(Actor* thisx, PlayState* play);
void BgMenkuriKaiten_Destroy(Actor* thisx, PlayState* play);
void BgMenkuriKaiten_Update(Actor* thisx, PlayState* play);
void BgMenkuriKaiten_Draw(Actor* thisx, PlayState* play);

const ActorInit Bg_Menkuri_Kaiten_InitVars = {
    ACTOR_BG_MENKURI_KAITEN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MENKURI_OBJECTS,
    sizeof(BgMenkuriKaiten),
    (ActorFunc)BgMenkuriKaiten_Init,
    (ActorFunc)BgMenkuriKaiten_Destroy,
    (ActorFunc)BgMenkuriKaiten_Update,
    (ActorFunc)BgMenkuriKaiten_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgMenkuriKaiten_Init(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK3);
    CollisionHeader_GetVirtual(&gGTGRotatingRingPlatformCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void BgMenkuriKaiten_Destroy(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgMenkuriKaiten_Update(Actor* thisx, PlayState* play) {
    BgMenkuriKaiten* this = (BgMenkuriKaiten*)thisx;

    if (!Flags_GetSwitch(play, this->dyna.actor.params) && func_80043590(&this->dyna)) {
        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
        this->dyna.actor.shape.rot.y += 0x80;
    }
}

void BgMenkuriKaiten_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gGTGRotatingRingPlatformDL);
}
