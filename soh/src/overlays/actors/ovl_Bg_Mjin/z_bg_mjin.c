/*
 * File: z_bg_mjin.c
 * Overlay: ovl_Bg_Mjin
 * Description: Warp Pad
 */

#include "z_bg_mjin.h"
#include "objects/object_mjin/object_mjin.h"
#include "objects/object_mjin_wind/object_mjin_wind.h"
#include "objects/object_mjin_soul/object_mjin_soul.h"
#include "objects/object_mjin_dark/object_mjin_dark.h"
#include "objects/object_mjin_ice/object_mjin_ice.h"
#include "objects/object_mjin_flame/object_mjin_flame.h"
#include "objects/object_mjin_flash/object_mjin_flash.h"
#include "objects/object_mjin_oka/object_mjin_oka.h"

#define FLAGS ACTOR_FLAG_4

void BgMjin_Init(Actor* thisx, PlayState* play);
void BgMjin_Destroy(Actor* thisx, PlayState* play);
void BgMjin_Update(Actor* thisx, PlayState* play);
void BgMjin_Draw(Actor* thisx, PlayState* play);

void func_808A0850(BgMjin* this, PlayState* play);
void BgMjin_DoNothing(BgMjin* this, PlayState* play);

const ActorInit Bg_Mjin_InitVars = {
    ACTOR_BG_MJIN,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgMjin),
    (ActorFunc)BgMjin_Init,
    (ActorFunc)BgMjin_Destroy,
    (ActorFunc)BgMjin_Update,
    NULL,
    NULL,
};

extern UNK_TYPE D_06000000;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

static s16 sObjectIDs[] = { OBJECT_MJIN_FLASH, OBJECT_MJIN_DARK, OBJECT_MJIN_FLAME,
                            OBJECT_MJIN_ICE,   OBJECT_MJIN_SOUL, OBJECT_MJIN_WIND };

void* gPedestalEmblems[] = { gLightMedallionPlatformTex, gShadowMedallionPlatformTex, gFireMedallionPlatformTex,
                             gWaterMedallionPlatformTex, gSpiritMedallionPlatformTex, gForestMedallionPlatformTex };

void BgMjin_SetupAction(BgMjin* this, BgMjinActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgMjin_Init(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;
    s8 objBankIndex;

    Actor_ProcessInitChain(thisx, sInitChain);
    objBankIndex = Object_GetIndex(&play->objectCtx, (thisx->params != 0 ? OBJECT_MJIN : OBJECT_MJIN_OKA));
    this->objBankIndex = objBankIndex;
    if (objBankIndex < 0) {
        Actor_Kill(thisx);
    } else {
        BgMjin_SetupAction(this, func_808A0850);
    }
}

void BgMjin_Destroy(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808A0850(BgMjin* this, PlayState* play) {
    CollisionHeader* colHeader;
    CollisionHeader* collision;

    if (Object_IsLoaded(&play->objectCtx, this->objBankIndex)) {
        colHeader = NULL;
        this->dyna.actor.flags &= ~ACTOR_FLAG_4;
        this->dyna.actor.objBankIndex = this->objBankIndex;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        DynaPolyActor_Init(&this->dyna, 0);
        collision = this->dyna.actor.params != 0 ? &gWarpPadCol : &gOcarinaWarpPadCol;
        CollisionHeader_GetVirtual(collision, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        BgMjin_SetupAction(this, BgMjin_DoNothing);
        this->dyna.actor.draw = BgMjin_Draw;
    }
}

void BgMjin_DoNothing(BgMjin* this, PlayState* play) {
}

void BgMjin_Update(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;

    this->actionFunc(this, play);
}

void BgMjin_Draw(Actor* thisx, PlayState* play) {
    BgMjin* this = (BgMjin*)thisx;
    Gfx* dlist;

    OPEN_DISPS(play->state.gfxCtx);

    if (thisx->params != 0) {
        s32 objBankIndex = Object_GetIndex(&play->objectCtx, sObjectIDs[thisx->params - 1]);

        if (objBankIndex >= 0) {
            gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[objBankIndex].segment);
        }

        gSPSegment(POLY_OPA_DISP++, 0x08, gPedestalEmblems[thisx->params - 1]);
        dlist = gWarpPadBaseDL;
    } else {
        dlist = gOcarinaWarpPadDL;
    }

    func_80093D18(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);
}
