/*
 * File: z_bg_open_spot.c
 * Overlay: ovl_Bg_Open_Spot
 * Description: Spotlights that shine on the Skull Kid when he turns Link
 * into a Deku Scrub.
 */
#include "z_bg_open_spot.h"
#include "objects/object_open_obj/object_open_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgOpenSpot_Init(Actor* thisx, PlayState* play);
void BgOpenSpot_Destroy(Actor* thisx, PlayState* play);
void BgOpenSpot_Update(Actor* thisx, PlayState* play);
void BgOpenSpot_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Open_Spot_Profile = {
    /**/ ACTOR_BG_OPEN_SPOT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_OPEN_OBJ,
    /**/ sizeof(BgOpenSpot),
    /**/ BgOpenSpot_Init,
    /**/ BgOpenSpot_Destroy,
    /**/ BgOpenSpot_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 560, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgOpenSpot_Init(Actor* thisx, PlayState* play) {
    BgOpenSpot* this = (BgOpenSpot*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->texScrolls = Lib_SegmentedToVirtual(gSpotlightTexAnim);
}

void BgOpenSpot_Destroy(Actor* thisx, PlayState* play) {
}

void BgOpenSpot_Update(Actor* thisx, PlayState* play) {
    BgOpenSpot* this = (BgOpenSpot*)thisx;
    u32 cueId;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_125)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_125)]->id;
        if (cueId == 1) {
            this->actor.draw = NULL;
        } else if (cueId == 2) {
            this->actor.draw = BgOpenSpot_Draw;
        }
    }
}

void BgOpenSpot_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgOpenSpot* this = (BgOpenSpot*)thisx;

    AnimatedMat_Draw(play, this->texScrolls);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_BAYER);
    gSPDisplayList(POLY_XLU_DISP++, gSpotlightLeftDL);
    gSPDisplayList(POLY_XLU_DISP++, gSpotlightRightDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
