/*
 * File: z_magic_fire.c
 * Overlay: ovl_Magic_Fire
 * Description: Din's Fire
 */

#include "z_magic_fire.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void MagicFire_Init(Actor* thisx, PlayState* play);
void MagicFire_Destroy(Actor* thisx, PlayState* play);
void MagicFire_Update(Actor* thisx, PlayState* play);
void MagicFire_Draw(Actor* thisx, PlayState* play);

void MagicFire_UpdateBeforeCast(Actor* thisx, PlayState* play);

typedef enum {
    /* 0x00 */ DF_ACTION_INITIALIZE,
    /* 0x01 */ DF_ACTION_EXPAND_SLOWLY,
    /* 0x02 */ DF_ACTION_STOP_EXPANDING,
    /* 0x03 */ DF_ACTION_EXPAND_QUICKLY
} MagicFireAction;

typedef enum {
    /* 0x00 */ DF_SCREEN_TINT_NONE,
    /* 0x01 */ DF_SCREEN_TINT_FADE_IN,
    /* 0x02 */ DF_SCREEN_TINT_MAINTAIN,
    /* 0x03 */ DF_SCREEN_TINT_FADE_OUT,
    /* 0x04 */ DF_SCREEN_TINT_FINISHED
} MagicFireScreenTint;

const ActorInit Magic_Fire_InitVars = {
    ACTOR_MAGIC_FIRE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(MagicFire),
    (ActorFunc)MagicFire_Init,
    (ActorFunc)MagicFire_Destroy,
    (ActorFunc)MagicFire_Update,
    (ActorFunc)MagicFire_Draw,
    NULL,
};

#include "overlays/ovl_Magic_Fire/ovl_Magic_Fire.h"

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00020000, 0x00, 0x01 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 9, 9, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_STOP),
};

static u8 sVertexIndices[] = {
    3,  4,  5,  6,  7,  8,  9,  10, 16, 17, 18, 19, 25, 26, 27, 32, 35, 36, 37, 38,
    39, 45, 46, 47, 52, 53, 54, 59, 60, 61, 67, 68, 69, 70, 71, 72, 0,  1,  11, 12,
    14, 20, 21, 23, 28, 30, 33, 34, 40, 41, 43, 48, 50, 55, 57, 62, 64, 65, 73, 74,
};

void MagicFire_Init(Actor* thisx, PlayState* play) {
    MagicFire* this = (MagicFire*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->action = 0;
    this->screenTintBehaviour = 0;
    this->actionTimer = 0;
    this->alphaMultiplier = -3.0f;
    Actor_SetScale(&this->actor, 0.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->actor.update = MagicFire_UpdateBeforeCast;
    this->actionTimer = 20;
    this->actor.room = -1;
}

void MagicFire_Destroy(Actor* thisx, PlayState* play) {
    func_800876C8(play);
}

void MagicFire_UpdateBeforeCast(Actor* thisx, PlayState* play) {
    MagicFire* this = (MagicFire*)thisx;
    Player* player = GET_PLAYER(play);

    if ((play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) ||
        (play->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (this->actionTimer > 0) {
        this->actionTimer--;
    } else {
        this->actor.update = MagicFire_Update;
        func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_FIRE);
    }
    this->actor.world.pos = player->actor.world.pos;
}

void MagicFire_Update(Actor* thisx, PlayState* play) {
    MagicFire* this = (MagicFire*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;

    this->actor.world.pos = player->actor.world.pos;
    if ((play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) ||
        (play->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (this->action == DF_ACTION_EXPAND_SLOWLY) {
        this->collider.info.toucher.damage = this->actionTimer + 25;
    } else if (this->action == DF_ACTION_STOP_EXPANDING) {
        this->collider.info.toucher.damage = this->actionTimer;
    }
    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->collider.dim.radius = (this->actor.scale.x * 325.0f);
    this->collider.dim.height = (this->actor.scale.y * 450.0f);
    this->collider.dim.yShift = (this->actor.scale.y * -225.0f);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);

    switch (this->action) {
        case DF_ACTION_INITIALIZE:
            this->actionTimer = 30;
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 0.0f;
            this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;
            this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
            this->alphaMultiplier = 0.0f;
            this->scalingSpeed = 0.08f;
            this->action++;
            break;
        case DF_ACTION_EXPAND_SLOWLY: // Fire sphere slowly expands out of player for 30 frames
            Math_StepToF(&this->alphaMultiplier, 1.0f, 1.0f / 30.0f);
            if (this->actionTimer > 0) {
                Math_SmoothStepToF(&this->actor.scale.x, 0.4f, this->scalingSpeed, 0.1f, 0.001f);
                this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
            } else {
                this->actionTimer = 25;
                this->action++;
            }
            break;
        case DF_ACTION_STOP_EXPANDING: // Sphere stops expanding and maintains size for 25 frames
            if (this->actionTimer <= 0) {
                this->actionTimer = 15;
                this->action++;
                this->scalingSpeed = 0.05f;
            }
            break;
        case DF_ACTION_EXPAND_QUICKLY: // Sphere beings to grow again and quickly expands out until killed
            this->alphaMultiplier -= 8.0f / 119.00001f;
            this->actor.scale.x += this->scalingSpeed;
            this->actor.scale.y += this->scalingSpeed;
            this->actor.scale.z += this->scalingSpeed;
            if (this->alphaMultiplier <= 0.0f) {
                this->action = 0;
                Actor_Kill(&this->actor);
            }
            break;
    }
    switch (this->screenTintBehaviour) {
        case DF_SCREEN_TINT_NONE:
            if (this->screenTintBehaviourTimer <= 0) {
                this->screenTintBehaviourTimer = 20;
                this->screenTintBehaviour = DF_SCREEN_TINT_FADE_IN;
            }
            break;
        case DF_SCREEN_TINT_FADE_IN:
            this->screenTintIntensity = 1.0f - (this->screenTintBehaviourTimer / 20.0f);
            if (this->screenTintBehaviourTimer <= 0) {
                this->screenTintBehaviourTimer = 45;
                this->screenTintBehaviour = DF_SCREEN_TINT_MAINTAIN;
            }
            break;
        case DF_SCREEN_TINT_MAINTAIN:
            if (this->screenTintBehaviourTimer <= 0) {
                this->screenTintBehaviourTimer = 5;
                this->screenTintBehaviour = DF_SCREEN_TINT_FADE_OUT;
            }
            break;
        case DF_SCREEN_TINT_FADE_OUT:
            this->screenTintIntensity = (this->screenTintBehaviourTimer / 5.0f);
            if (this->screenTintBehaviourTimer <= 0) {
                this->screenTintBehaviour = DF_SCREEN_TINT_FINISHED;
            }
            break;
    }
    if (this->actionTimer > 0) {
        this->actionTimer--;
    }
    if (this->screenTintBehaviourTimer > 0) {
        this->screenTintBehaviourTimer--;
    }
}

void MagicFire_Draw(Actor* thisx, PlayState* play) {
    MagicFire* this = (MagicFire*)thisx;
    s32 pad1;
    u32 gameplayFrames = play->gameplayFrames;
    s32 pad2;
    s32 i;
    u8 alpha;
    Color_RGB8 Spell_env_ori = {255, 0, 0};
    Color_RGB8 Spell_col_ori = {255, 200, 0};
    Color_RGB8 Spell_env = CVar_GetRGB("gDF_Env", Spell_env_ori);
    Color_RGB8 Spell_col = CVar_GetRGB("gDF_Col", Spell_col_ori);

    if (this->action > 0) {
        OPEN_DISPS(play->state.gfxCtx);
        POLY_XLU_DISP = Gfx_SetupDL_57(POLY_XLU_DISP);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)(s32)(60 * this->screenTintIntensity),
                        (u8)(s32)(20 * this->screenTintIntensity), (u8)(s32)(0 * this->screenTintIntensity),
                        (u8)(s32)(120 * this->screenTintIntensity));
        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_DISABLE);
        gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
        gDPFillRectangle(POLY_XLU_DISP++, 0, 0, 319, 239);
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        if (CVar_GetS32("gUseSpellsCol",0)) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, Spell_col.r, Spell_col.g, Spell_col.b, (u8)(this->alphaMultiplier * 255));
            gDPSetEnvColor(POLY_XLU_DISP++, Spell_env.r, Spell_env.g, Spell_env.b, (u8)(this->alphaMultiplier * 255));
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, Spell_col_ori.r, Spell_col_ori.g, Spell_col_ori.b, (u8)(this->alphaMultiplier * 255));
            gDPSetEnvColor(POLY_XLU_DISP++, Spell_env_ori.r, Spell_env_ori.g, Spell_env_ori.b, (u8)(this->alphaMultiplier * 255));
        }
        Matrix_Scale(0.15f, 0.15f, 0.15f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPPipeSync(POLY_XLU_DISP++);
        gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        gDPLoadTextureBlock(POLY_XLU_DISP++, sTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, 6, 6, 15, G_TX_NOLOD);
        gDPSetTile(POLY_XLU_DISP++, G_IM_FMT_I, G_IM_SIZ_8b, 8, 0, 1, 0, G_TX_NOMIRROR | G_TX_WRAP, 6, 14,
                   G_TX_NOMIRROR | G_TX_WRAP, 6, 14);
        gDPSetTileSize(POLY_XLU_DISP++, 1, 0, 0, 252, 252);
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, (gameplayFrames * 2) % 512,
                                        511 - ((gameplayFrames * 5) % 512), 64, 64, 1, (gameplayFrames * 2) % 256,
                                        255 - ((gameplayFrames * 20) % 256), 32, 32));
        gSPDisplayList(POLY_XLU_DISP++, sModelDL);
        CLOSE_DISPS(play->state.gfxCtx);

        alpha = (s32)(this->alphaMultiplier * 255);
        Vtx* vertices = ResourceMgr_LoadVtxByName(sSphereVtx);
        for (i = 0; i < 36; i++) {
            vertices[sVertexIndices[i]].n.a = alpha;
        }

        alpha = (s32)(this->alphaMultiplier * 76);
        for (i = 36; i < 60; i++) {
            vertices[sVertexIndices[i]].n.a = alpha;
        }
    }
}
