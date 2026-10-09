/*
 * File: z_bg_fu_mizu.c
 * Overlay: ovl_Bg_Fu_Mizu
 * Description: Water in Honey and Darling's Second Day game
 */

#include "z_bg_fu_mizu.h"
#include "objects/object_fu_kaiten/object_fu_kaiten.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgFuMizu_Init(Actor* thisx, PlayState* play);
void BgFuMizu_Destroy(Actor* thisx, PlayState* play);
void BgFuMizu_Update(Actor* thisx, PlayState* play);
void BgFuMizu_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Fu_Mizu_Profile = {
    /**/ ACTOR_BG_FU_MIZU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FU_KAITEN,
    /**/ sizeof(BgFuMizu),
    /**/ BgFuMizu_Init,
    /**/ BgFuMizu_Destroy,
    /**/ BgFuMizu_Update,
    /**/ BgFuMizu_Draw,
};

void BgFuMizu_Init(Actor* thisx, PlayState* play) {
    BgFuMizu* this = (BgFuMizu*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_SetScale(&this->dyna.actor, 1.0f);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    CollisionHeader_GetVirtual(&object_fu_kaiten_Colheader_0037F8, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->unk_160 = 0;
    this->dyna.actor.world.pos.y = -10.0f + this->dyna.actor.home.pos.y;
}

void BgFuMizu_Destroy(Actor* thisx, PlayState* play) {
    BgFuMizu* this = (BgFuMizu*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

s32 func_80ADABA4(BgFuMizu* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (actor != NULL) {
        if ((actor != NULL) && (actor->id == ACTOR_EN_FU_KAGO) && (actor->colChkInfo.health == 0)) {
            return false;
        }
        actor = actor->next;
    }

    return true;
}

void BgFuMizu_Update(Actor* thisx, PlayState* play) {
    f32 heightTarget;
    BgFuMizu* this = (BgFuMizu*)thisx;

    if (this->unk_160 == 0) {
        if (func_80ADABA4(this, play)) {
            heightTarget = -10.0f + this->dyna.actor.home.pos.y;
        } else {
            heightTarget = 10.0f + this->dyna.actor.home.pos.y;
        }
    } else {
        heightTarget = 25.0f + this->dyna.actor.home.pos.y;
    }
    if (Math_SmoothStepToF(&this->dyna.actor.world.pos.y, heightTarget, 0.05f, 1.0f, 0.5f) > 1.0f) {
        if (this->unk_160 == 1) {
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN_FIX - SFX_FLAG);
        } else {
            Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN_FIX - SFX_FLAG);
        }
    }
}

void BgFuMizu_Draw(Actor* thisx, PlayState* play) {
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_fu_kaiten_Matanimheader_0037D8));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_fu_kaiten_DL_002FC0);

    CLOSE_DISPS(play->state.gfxCtx);
}
