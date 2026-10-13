/*
 * File: z_dm_char07.c
 * Overlay: ovl_Dm_Char07
 * Description: Different Milk Bar Objects (Stage, Credits Stage, Spotlights, Dishes)
 */

#include "z_dm_char07.h"
#include "objects/object_milkbar/object_milkbar.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar07_Init(Actor* thisx, PlayState* play);
void DmChar07_Destroy(Actor* thisx, PlayState* play);
void DmChar07_Update(Actor* thisx, PlayState* play);
void DmChar07_Draw(Actor* thisx, PlayState* play);

void DmChar07_DoNothing(DmChar07* this, PlayState* play);

ActorProfile Dm_Char07_Profile = {
    /**/ ACTOR_DM_CHAR07,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_MILKBAR,
    /**/ sizeof(DmChar07),
    /**/ DmChar07_Init,
    /**/ DmChar07_Destroy,
    /**/ DmChar07_Update,
    /**/ DmChar07_Draw,
};

void DmChar07_SetupAction(DmChar07* this, DmChar07ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DmChar07_Init(Actor* thisx, PlayState* play) {
    DmChar07* this = (DmChar07*)thisx;

    this->isStage = 0;
    Actor_SetScale(&this->dyna.actor, 1.0f);
    this->spotlightFlags = DMCHAR07_GET_SPOTLIGHTFLAGS(thisx);
    thisx->params = DMCHAR07_GET_TYPE(thisx);
    if (this->dyna.actor.params == DMCHAR07_STAGE) {
        Actor_SetScale(&this->dyna.actor, 0.1f);
        this->isStage = 1;
        DynaPolyActor_Init(&this->dyna, 0);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_milkbar_Colheader_006688);
    } else {
        Actor_SetScale(&this->dyna.actor, 1.0f);
    }
    DmChar07_SetupAction(this, DmChar07_DoNothing);
}

void DmChar07_Destroy(Actor* thisx, PlayState* play) {
    DmChar07* this = (DmChar07*)thisx;

    if (this->isStage) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void DmChar07_DoNothing(DmChar07* this, PlayState* play) {
}

void DmChar07_Update(Actor* thisx, PlayState* play) {
    DmChar07* this = (DmChar07*)thisx;

    this->actionFunc(this, play);
}

void DmChar07_Draw(Actor* thisx, PlayState* play) {
    DmChar07* this = (DmChar07*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    switch (this->dyna.actor.params) {
        case DMCHAR07_STAGE:
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_002CD0);
            break;

        case DMCHAR07_CREDITS_STAGE:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_milkbar_Matanimheader_0105F8));
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_007918);
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000240);
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000790);
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000CC0);
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_001210);
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_0016B8);
            break;

        case DMCHAR07_SPOTLIGHTS:
            if (this->spotlightFlags & DMCHAR07_STAGE_RIGHT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000240);
            }
            if (this->spotlightFlags & DMCHAR07_UPSTAGE_RIGHT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000790);
            }
            if (this->spotlightFlags & DMCHAR07_STAGE_CENTER_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000CC0);
            }
            if (this->spotlightFlags & DMCHAR07_UPSTAGE_LEFT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_001210);
            }
            if (this->spotlightFlags & DMCHAR07_DOWNSTAGE_CENTER_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_0016B8);
            }
            break;

        case DMCHAR07_DISHES:
            gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_010D68);
            break;

        default:
            break;
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    switch (this->dyna.actor.params) {
        case DMCHAR07_STAGE:
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_002BA0);
            break;

        case DMCHAR07_CREDITS_STAGE:
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_0076A0);
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_000100);
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_000650);
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_000B80);
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_0010D0);
            gSPDisplayList(POLY_XLU_DISP++, object_milkbar_DL_0015E0);
            break;

        case DMCHAR07_SPOTLIGHTS:
            if (this->spotlightFlags & DMCHAR07_STAGE_RIGHT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000100);
            }
            if (this->spotlightFlags & DMCHAR07_UPSTAGE_RIGHT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000650);
            }
            if (this->spotlightFlags & DMCHAR07_STAGE_CENTER_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_000B80);
            }
            if (this->spotlightFlags & DMCHAR07_UPSTAGE_LEFT_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_0010D0);
            }
            if (this->spotlightFlags & DMCHAR07_DOWNSTAGE_CENTER_SPOTLIGHT) {
                gSPDisplayList(POLY_OPA_DISP++, object_milkbar_DL_0015E0);
            }
            break;

        case DMCHAR07_DISHES:
            break;

        default:
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
