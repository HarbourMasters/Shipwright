/*
 * File: z_oceff_storm.c
 * Overlay: ovl_Oceff_Storm
 * Description: Song of Storms Ocarina Effect
 */

#include "z_oceff_storm.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void OceffStorm_Init(Actor* thisx, PlayState* play);
void OceffStorm_Destroy(Actor* thisx, PlayState* play);
void OceffStorm_Update(Actor* thisx, PlayState* play);
void OceffStorm_Draw(Actor* thisx, PlayState* play);

void OceffStorm_DefaultAction(OceffStorm* this, PlayState* play);
void func_80981B48(OceffStorm* this, PlayState* play);
void OceffStorm_Draw2(Actor* thisx, PlayState* play);

ActorProfile Oceff_Storm_Profile = {
    /**/ ACTOR_OCEFF_STORM,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(OceffStorm),
    /**/ OceffStorm_Init,
    /**/ OceffStorm_Destroy,
    /**/ OceffStorm_Update,
    /**/ OceffStorm_Draw,
};

void OceffStorm_SetupAction(OceffStorm* this, OceffStormActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s32 func_8098176C(PlayState* play) {
    s32 ret = false;

    switch (play->sceneId) {
        case SCENE_13HUBUKINOMITI:
        case SCENE_11GORONNOSATO:
        case SCENE_10YUKIYAMANOMURA:
        case SCENE_14YUKIDAMANOMITI:
        case SCENE_12HAKUGINMAE:
        case SCENE_17SETUGEN:
        case SCENE_GORONRACE:
            if (gSaveContext.sceneLayer == 0) {
                ret = true;
            }
            break;

        case SCENE_10YUKIYAMANOMURA2:
            if (gSaveContext.sceneLayer == 1) {
                ret = true;
            }
            break;
    }
    return ret;
}

void OceffStorm_Init(Actor* thisx, PlayState* play) {
    s32 pad[2];
    Player* player = GET_PLAYER(play);
    OceffStorm* this = (OceffStorm*)thisx;

    OceffStorm_SetupAction(this, OceffStorm_DefaultAction);

    this->posYOffAdd = 0;
    this->counter = 0;
    this->primColorAlpha = 0;
    this->vtxAlpha = 0;
    this->actor.scale.y = 0.0f;
    this->actor.scale.z = 0.0f;
    this->actor.scale.x = 0.0f;
    this->posYOff = this->posYOffAdd;

    if (this->actor.params == OCEFF_STORM_ACTION_PARAM_1) {
        OceffStorm_SetupAction(this, func_80981B48);
        this->actor.draw = OceffStorm_Draw2;
    } else {
        this->actor.world.pos.y = player->actor.world.pos.y;
        this->actor.world.pos.x = player->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        this->actor.world.pos.z = player->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        gSaveContext.jinxTimer = 0;
        if ((play->interfaceCtx.restrictions.songOfStorms == 0) && !func_8098176C(play)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_OKARINA_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y - 30.0f, this->actor.world.pos.z, 0, 0, 0, 1);
        }
    }
}

void OceffStorm_Destroy(Actor* thisx, PlayState* play) {
    OceffStorm* this = (OceffStorm*)thisx;

    Magic_Reset(play);
}

void OceffStorm_DefaultAction(OceffStorm* this, PlayState* play) {
    f32 cylinderScale;

    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_DEKU:
            cylinderScale = 1.3f;
            break;

        case PLAYER_FORM_ZORA:
            cylinderScale = 1.2f;
            break;

        case PLAYER_FORM_GORON:
            cylinderScale = 2.0f;
            break;

        default:
            cylinderScale = 1.0f;
            break;
    }

    if (this->counter < 10) {
        this->primColorAlpha = (s8)(this->counter * 10.0f);
    } else if (this->counter > 60) {
        this->primColorAlpha = (s8)((70 - this->counter) * 10.0f);
    } else {
        this->primColorAlpha = 100;
    }

    if ((this->counter < 10) || (this->counter >= 60)) {
        this->vtxAlpha = 0;
    } else if (this->counter <= 40) {
        if (this->vtxAlpha <= 200) {
            this->vtxAlpha += 10;
        }
        this->actor.scale.x = this->actor.scale.z = 0.4f * cylinderScale;
        this->actor.scale.y = 0.3f;
    } else if (this->counter > 40) {
        this->vtxAlpha = (60 - this->counter) * 10;
    } else {
        this->actor.scale.x = this->actor.scale.z = 0.4f * cylinderScale;
        this->vtxAlpha = -1;
    }

    if (this->counter > 40) {
        this->actor.world.pos.y += this->posYOff * 0.01f;
        this->posYOff += this->posYOffAdd;
        this->posYOffAdd += 10;
    }

    if (this->counter < 70) {
        this->counter++;
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80981B48(OceffStorm* this, PlayState* play) {
    if (this->primColorAlpha < 100) {
        this->primColorAlpha += 5;
    }
    //! @bug Actor_Kill is never called so the actor will stay alive forever
}

void OceffStorm_Update(Actor* thisx, PlayState* play) {
    OceffStorm* this = (OceffStorm*)thisx;

    this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    this->actionFunc(this, play);
}

#include "assets/overlays/ovl_Oceff_Storm/ovl_Oceff_Storm.h"

void OceffStorm_Draw2(Actor* thisx, PlayState* play) {
    s32 scroll = play->state.frames & 0xFFF;
    OceffStorm* this = (OceffStorm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);

    POLY_XLU_DISP = Gfx_SetupDL64(POLY_XLU_DISP);

    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 150, this->primColorAlpha);

    gSPDisplayList(POLY_XLU_DISP++, &sSongOfStormsMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 8, scroll * 4, 64,
                                                       64, 1, scroll * 4, scroll * 4, 64, 64, 8, 4, 4, 4));
    // 2S2H [Cosmetic] Changed to Wide variant to support widescreen
    gSPWideTextureRectangle(POLY_XLU_DISP++, OTRGetRectDimensionFromLeftEdge(0) << 2, 0,
                            OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH) << 2, SCREEN_HEIGHT << 2, G_TX_RENDERTILE, 0,
                            0, (s32)(0.13671875f * (1 << 10)), (s32)(-0.13671875f * (1 << 10)));

    CLOSE_DISPS(play->state.gfxCtx);
}

void OceffStorm_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffStorm* this = (OceffStorm*)thisx;
    // #region 2S2H [Port] Originally this was just a pointer to the vertices, now it's the OTR path so we need to grab
    // it's actual address
    Vtx* vtxPtr = ResourceMgr_LoadVtxByName(sSongOfStormsCylinderVtx);
    // #endregion

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 200, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 0, 128);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);

    vtxPtr[0].v.cn[3] = vtxPtr[6].v.cn[3] = vtxPtr[16].v.cn[3] = vtxPtr[25].v.cn[3] = this->vtxAlpha >> 1;
    vtxPtr[10].v.cn[3] = vtxPtr[22].v.cn[3] = this->vtxAlpha;

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, &sSongOfStormsCylinderMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 4, (0 - scroll) * 8, 32, 32, 1,
                                      scroll * 8, (0 - scroll) * 12, 32, 32, 4, -8, 8, -12));
    gSPDisplayList(POLY_XLU_DISP++, &sSongOfStormsCylinderModelDL);

    CLOSE_DISPS(play->state.gfxCtx);

    OceffStorm_Draw2(&this->actor, play);
}
