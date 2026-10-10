/*
 * File: z_oceff_spot.c
 * Overlay: ovl_Oceff_Spot
 * Description: Sun's Song Ocarina Effect
 */

#include "z_oceff_spot.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffSpot_Init(Actor* thisx, PlayState* play2);
void OceffSpot_Destroy(Actor* thisx, PlayState* play2);
void OceffSpot_Update(Actor* thisx, PlayState* play);
void OceffSpot_Draw(Actor* thisx, PlayState* play);

void OceffSpot_Wait(OceffSpot* this, PlayState* play);
void OceffSpot_GrowCylinder(OceffSpot* this, PlayState* play);
void OceffSpot_End(OceffSpot* this, PlayState* play);

void OceffSpot_SetupAction(OceffSpot* this, OceffSpotActionFunc actionFunc);

ActorProfile Oceff_Spot_Profile = {
    /**/ ACTOR_OCEFF_SPOT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(OceffSpot),
    /**/ OceffSpot_Init,
    /**/ OceffSpot_Destroy,
    /**/ OceffSpot_Update,
    /**/ OceffSpot_Draw,
};

#include "assets/overlays/ovl_Oceff_Spot/ovl_Oceff_Spot.h"

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1500, ICHAIN_STOP),
};

void OceffSpot_SetupAction(OceffSpot* this, OceffSpotActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void OceffSpot_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    OceffSpot* this = (OceffSpot*)thisx;
    Player* player = GET_PLAYER(play);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    OceffSpot_SetupAction(this, OceffSpot_GrowCylinder);

    Lights_PointNoGlowSetInfo(&this->lightInfo1, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->lightNode1 = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo1);

    Lights_PointNoGlowSetInfo(&this->lightInfo2, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->lightNode2 = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo2);
    this->actor.scale.y = 0.3f;
    this->unk16C = 0.0f;
    this->actor.world.pos.y = player->actor.world.pos.y;
    this->actor.world.pos.x = player->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
    this->actor.world.pos.z = player->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
}

void OceffSpot_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    OceffSpot* this = (OceffSpot*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode1);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode2);
    Magic_Reset(play);
}

void OceffSpot_End(OceffSpot* this, PlayState* play) {
    if (this->unk16C > 0.0f) {
        this->unk16C -= 0.05f;
    } else {
        Actor_Kill(&this->actor);
        if ((R_TIME_SPEED != 400) && !play->msgCtx.blockSunsSong) {
            if ((play->msgCtx.ocarinaAction != OCARINA_ACTION_CHECK_NOTIME_DONE) ||
                (play->msgCtx.ocarinaMode != OCARINA_MODE_PLAYED_SUNS)) {
                gSaveContext.sunsSongState = SUNSSONG_START;
            }
        } else {
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        }
    }
}

void OceffSpot_Wait(OceffSpot* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        OceffSpot_SetupAction(this, OceffSpot_End);
    }
}

void OceffSpot_GrowCylinder(OceffSpot* this, PlayState* play) {
    if (this->unk16C < 1.0f) {
        this->unk16C += 0.05f;
    } else {
        OceffSpot_SetupAction(this, OceffSpot_Wait);
        this->timer = 60;
    }
}

void OceffSpot_Update(Actor* thisx, PlayState* play) {
    f32 scale;
    s32 pad;
    Player* player = GET_PLAYER(play);
    f32 temp;
    OceffSpot* this = (OceffSpot*)thisx;

    temp = (1.0f - cosf(this->unk16C * M_PIf)) * 0.5f;
    this->actionFunc(this, play);

    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_DEKU:
            scale = 1.3f;
            break;

        case PLAYER_FORM_ZORA:
            scale = 1.2f;
            break;

        case PLAYER_FORM_GORON:
            scale = 2.0f;
            break;

        default:
            scale = 1.0f;
            break;
    }

    this->actor.scale.z = (scale * 0.42f) * temp;
    this->actor.scale.x = (scale * 0.42f) * temp;

    this->actor.world.pos = player->actor.world.pos;
    this->actor.world.pos.y += 5.0f;

    temp = (2.0f - this->unk16C) * this->unk16C;

    Environment_AdjustLights(play, temp * 0.5f, 880.0f, 0.2f, 0.9f);

    Lights_PointNoGlowSetInfo(&this->lightInfo1, this->actor.world.pos.x, this->actor.world.pos.y + 55.0f,
                              this->actor.world.pos.z, (s32)(255.0f * temp), (s32)(255.0f * temp), (s32)(200.0f * temp),
                              100.0f * temp);
    Lights_PointNoGlowSetInfo(
        &this->lightInfo2, this->actor.world.pos.x + (Math_SinS(player->actor.shape.rot.y) * 20.0f),
        this->actor.world.pos.y + 20.0f, this->actor.world.pos.z + (Math_CosS(player->actor.shape.rot.y) * 20.0f),
        (s32)(255.0f * temp), (s32)(255.0f * temp), (s32)(200.0f * temp), 100.0f * temp);
}

void OceffSpot_Draw(Actor* thisx, PlayState* play) {
    OceffSpot* this = (OceffSpot*)thisx;
    u32 scroll = play->state.frames & 0xFFFF;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, &sSunSongEffectCylinderMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, scroll * 2, scroll * -2, 0x20, 0x20, 1, 0,
                                                       scroll * -8, 0x20, 0x20, 2, -2, 0, -8));
    gSPDisplayList(POLY_XLU_DISP++, &sSunSongEffectCylinderModelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
