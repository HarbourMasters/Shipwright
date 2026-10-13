/*
 * File: z_eff_ss_fhg_flash.c
 * Overlay: ovl_Effect_Ss_Fhg_Flash
 * Description: Shock and Light Ball Effect
 */

#include "z_eff_ss_fhg_flash.h"
#include "overlays/actors/ovl_Boss_Hakugin/z_boss_hakugin.h"
#include "assets/overlays/ovl_Effect_Ss_Fhg_Flash/ovl_Effect_Ss_Fhg_Flash.h"

#define rAlpha regs[0]
#define rXZRot regs[3]
#define rParams regs[4]
#define rScale regs[8]

#define PARAMS ((EffectSsFhgFlashInitParams*)initParamsx)

u32 EffectSsFhgFlash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsFhgFlash_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsFhgFlash_Draw(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Fhg_Flash_Profile = {
    EFFECT_SS_FHG_FLASH,
    EffectSsFhgFlash_Init,
};

u32 EffectSsFhgFlash_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
    EffectSsFhgFlashInitParams* initParams = PARAMS;
    Vec3f noActorPos = { 0.0f, -1000.0f, 0.0f };

    this->actor = initParams->actor;
    Math_Vec3f_Copy(&this->velocity, &gZeroVec3f);
    Math_Vec3f_Copy(&this->accel, &gZeroVec3f);
    this->life = (s32)(Rand_ZeroOne() * 10.0f) + 111;
    this->rScale = (s32)Rand_ZeroFloat(initParams->scale) + initParams->scale;
    this->rAlpha = 255;
    this->draw = EffectSsFhgFlash_Draw;
    this->update = EffectSsFhgFlash_Update;
    this->rParams = initParams->params;

    if (initParams->params != FHGFLASH_SHOCK_NO_ACTOR) {
        this->pos = noActorPos;
        this->gfx = gEffFhgFlashDL;
    } else {
        this->pos = initParams->pos;
        this->gfx = gEffFhgFlashDL;
    }

    return 1;
}

#include "overlays/ovl_Effect_Ss_Fhg_Flash/ovl_Effect_Ss_Fhg_Flash.h"

void EffectSsFhgFlash_Draw(PlayState* play, u32 index, EffectSs* this) {
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32 scale = this->rScale / 100.0f;

    OPEN_DISPS(gfxCtx);

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    if (this->rParams != FHGFLASH_SHOCK_NO_ACTOR) {
        Gfx_SetupDL44_Xlu(play->state.gfxCtx);
        Matrix_RotateXS(this->rXZRot, MTXMODE_APPLY);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_DECAL2);
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
    }
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->rAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 155, 0);
    Matrix_RotateZS(this->rXZRot, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, this->gfx);

    CLOSE_DISPS(gfxCtx);
}

void EffectSsFhgFlash_Update(PlayState* play, u32 index, EffectSs* this) {
    s16 randBodyPart;
    s16 rand = Rand_ZeroOne() * 20000.0f;

    this->rXZRot = (this->rXZRot + rand) + 0x4000;
    if (this->rParams == FHGFLASH_SHOCK_PLAYER) {
        Player* player = GET_PLAYER(play);

        randBodyPart = Rand_ZeroFloat(PLAYER_BODYPART_MAX - 0.1f);
        this->pos.x = player->bodyPartsPos[randBodyPart].x + Rand_CenteredFloat(10.0f);
        this->pos.y = player->bodyPartsPos[randBodyPart].y + Rand_CenteredFloat(15.0f);
        this->pos.z = player->bodyPartsPos[randBodyPart].z + Rand_CenteredFloat(10.0f);
    } else if ((this->rParams >= FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH) &&
               (this->rParams <= FHGFLASH_SHOCK_GOHT_FRONT_RIGHT_UPPER_LEG)) {
        s32 pad;
        Vec3f* effectPos;
        BossHakugin* goht = (BossHakugin*)this->actor;
        s32 effectIndex = goht->malfunctionEffectIndex - 3;

        if (effectIndex < 0) {
            effectPos = &goht->malfunctionEffects[this->rParams - FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH]
                                                 [GOHT_MALFUNCTION_EFFECTS_PER_TYPE - 3]
                                                     .pos;
        } else {
            effectPos = &goht->malfunctionEffects[this->rParams - FHGFLASH_SHOCK_GOHT_BACK_LEFT_THIGH][effectIndex].pos;
        }

        this->pos.x = Rand_CenteredFloat(70.0f) + effectPos->x;
        this->pos.y = Rand_CenteredFloat(70.0f) + effectPos->y;
        this->pos.z = Rand_CenteredFloat(70.0f) + effectPos->z;
    }
    if (this->life < 100) {
        this->rAlpha -= 50;
        if (this->rAlpha < 0) {
            this->rAlpha = 0;
            this->life = 0;
        }
    }
}
