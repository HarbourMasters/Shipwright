/*
 * File: z_magic_dark.c
 * Overlay: ovl_Magic_Dark
 * Description: Nayru's Love
 */

#include "z_magic_dark.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void MagicDark_Init(Actor* thisx, PlayState* play);
void MagicDark_Destroy(Actor* thisx, PlayState* play);
void MagicDark_OrbUpdate(Actor* thisx, PlayState* play);
void MagicDark_OrbDraw(Actor* thisx, PlayState* play);
void MagicDark_DiamondUpdate(Actor* thisx, PlayState* play);
void MagicDark_DiamondDraw(Actor* thisx, PlayState* play);

void MagicDark_DimLighting(PlayState* play, f32 intensity);

const ActorInit Magic_Dark_InitVars = {
    ACTOR_MAGIC_DARK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(MagicDark),
    (ActorFunc)MagicDark_Init,
    (ActorFunc)MagicDark_Destroy,
    (ActorFunc)MagicDark_OrbUpdate,
    (ActorFunc)MagicDark_OrbDraw,
    NULL,
};

#include "overlays/ovl_Magic_Dark/ovl_Magic_Dark.h"

// unused
static Color_RGBA8 D_80B88B10[] = { { 50, 100, 150, 200 }, { 255, 200, 150, 100 } };

void MagicDark_Init(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    Player* player = GET_PLAYER(play);

    if (!LINK_IS_ADULT) {
        this->scale = 0.4f;
    } else {
        this->scale = 0.6f;
    }

    thisx->world.pos = player->actor.world.pos;
    Actor_SetScale(&this->actor, 0.0f);
    thisx->room = -1;

    if (gSaveContext.nayrusLoveTimer != 0) {
        thisx->update = MagicDark_DiamondUpdate;
        thisx->draw = MagicDark_DiamondDraw;
        thisx->scale.x = thisx->scale.z = this->scale * 1.6f;
        thisx->scale.y = this->scale * 0.8f;
        this->timer = 0;
        this->primAlpha = 0;
    } else {
        this->timer = 0;
        gSaveContext.nayrusLoveTimer = 0;
    }
}

void MagicDark_Destroy(Actor* thisx, PlayState* play) {
    if (gSaveContext.nayrusLoveTimer == 0) {
        func_800876C8(play);
    }
}

void MagicDark_DiamondUpdate(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    u8 phi_a0;
    Player* player = GET_PLAYER(play);
    s16 pad;
    s16 nayrusLoveTimer = gSaveContext.nayrusLoveTimer;
    s32 msgMode = play->msgCtx.msgMode;

    if ((msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) || (msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_Kill(thisx);
        return;
    }

    if (nayrusLoveTimer >= 1200) {
        player->invincibilityTimer = 0;
        gSaveContext.nayrusLoveTimer = 0;
        Actor_Kill(thisx);
        return;
    }

    player->invincibilityTimer = -100;
    thisx->scale.x = thisx->scale.z = this->scale;

    if (this->timer < 20) {
        thisx->scale.x = thisx->scale.z = (1.6f - (this->timer * 0.03f)) * this->scale;
        thisx->scale.y = ((this->timer * 0.01f) + 0.8f) * this->scale;
    } else {
        thisx->scale.x = thisx->scale.z = this->scale;
        thisx->scale.y = this->scale;
    }

    thisx->scale.x *= 1.3f;
    thisx->scale.z *= 1.3f;

    phi_a0 = (this->timer < 20) ? (this->timer * 12) : 255;

    if (nayrusLoveTimer >= 1180) {
        this->primAlpha = 15595 - (nayrusLoveTimer * 13);
        if (nayrusLoveTimer & 1) {
            this->primAlpha = this->primAlpha >> 1;
        }
    } else if (nayrusLoveTimer >= 1100) {
        this->primAlpha = (u8)(nayrusLoveTimer << 7) + 127;
    } else {
        this->primAlpha = 255;
    }

    if (this->primAlpha > phi_a0) {
        this->primAlpha = phi_a0;
    }

    thisx->world.rot.y += 0x3E8;
    thisx->shape.rot.y = thisx->world.rot.y + Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    this->timer++;
    gSaveContext.nayrusLoveTimer = nayrusLoveTimer + 1;

    if (nayrusLoveTimer < 1100) {
        func_8002F974(thisx, NA_SE_PL_MAGIC_SOUL_NORMAL - SFX_FLAG);
    } else {
        func_8002F974(thisx, NA_SE_PL_MAGIC_SOUL_FLASH - SFX_FLAG);
    }
}

void MagicDark_DimLighting(PlayState* play, f32 intensity) {
    s32 i;
    f32 colorScale;
    f32 fogScale;

    if (play->roomCtx.curRoom.behaviorType1 != ROOM_BEHAVIOR_TYPE1_5) {
        intensity = CLAMP_MIN(intensity, 0.0f);
        intensity = CLAMP_MAX(intensity, 1.0f);
        fogScale = intensity - 0.2f;

        if (intensity < 0.2f) {
            fogScale = 0.0f;
        }

        play->envCtx.adjFogNear = (850.0f - play->envCtx.lightSettings.fogNear) * fogScale;

        if (intensity == 0.0f) {
            for (i = 0; i < ARRAY_COUNT(play->envCtx.adjFogColor); i++) {
                play->envCtx.adjFogColor[i] = 0;
            }
        } else {
            colorScale = intensity * 5.0f;

            if (colorScale > 1.0f) {
                colorScale = 1.0f;
            }

            for (i = 0; i < ARRAY_COUNT(play->envCtx.adjFogColor); i++) {
                play->envCtx.adjFogColor[i] = -(s16)(play->envCtx.lightSettings.fogColor[i] * colorScale);
            }
        }
    }
}

void MagicDark_OrbUpdate(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    func_8002F974(&this->actor, NA_SE_PL_MAGIC_SOUL_BALL - SFX_FLAG);
    if (this->timer < 35) {
        MagicDark_DimLighting(play, this->timer * (1 / 45.0f));
        Math_SmoothStepToF(&thisx->scale.x, this->scale * (1 / 12.000001f), 0.05f, 0.01f, 0.0001f);
        Actor_SetScale(&this->actor, thisx->scale.x);
    } else if (this->timer < 55) {
        Actor_SetScale(&this->actor, thisx->scale.x * 0.9f);
        Math_SmoothStepToF(&this->orbOffset.y, player->bodyPartsPos[0].y, 0.5f, 3.0f, 1.0f);
        if (this->timer > 48) {
            MagicDark_DimLighting(play, (54 - this->timer) * 0.2f);
        }
    } else {
        thisx->update = MagicDark_DiamondUpdate;
        thisx->draw = MagicDark_DiamondDraw;
        thisx->scale.x = thisx->scale.z = this->scale * 1.6f;
        thisx->scale.y = this->scale * 0.8f;
        this->timer = 0;
        this->primAlpha = 0;
    }

    this->timer++;
}

void MagicDark_DiamondDraw(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    s32 pad;
    u16 gameplayFrames = play->gameplayFrames;
    Color_RGB8 Spell_env_ori = {0, 100, 255};
    Color_RGB8 Spell_col_ori = {170, 255, 255};
    Color_RGB8 Spell_env = CVar_GetRGB("gNL_Diamond_Env", Spell_env_ori);
    Color_RGB8 Spell_col = CVar_GetRGB("gNL_Diamond_Col", Spell_col_ori);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);

    {
        Player* player = GET_PLAYER(play);
        f32 heightDiff;

        this->actor.world.pos.x = player->bodyPartsPos[0].x;
        this->actor.world.pos.z = player->bodyPartsPos[0].z;
        heightDiff = player->bodyPartsPos[0].y - this->actor.world.pos.y;
        if (heightDiff < -2.0f) {
            this->actor.world.pos.y = player->bodyPartsPos[0].y + 2.0f;
        } else if (heightDiff > 2.0f) {
            this->actor.world.pos.y = player->bodyPartsPos[0].y - 2.0f;
        }
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateY(this->actor.shape.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (CVar_GetS32("gUseSpellsCol",0)) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, Spell_col.r, Spell_col.g, Spell_col.b, (s32)(this->primAlpha * 0.6f) & 0xFF);
            gDPSetEnvColor(POLY_XLU_DISP++, Spell_env.r, Spell_env.g, Spell_env.b, 128);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 255, 255, (s32)(this->primAlpha * 0.6f) & 0xFF);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
        }
        gSPDisplayList(POLY_XLU_DISP++, sDiamondMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, gameplayFrames * 2, gameplayFrames * -4, 32, 32, 1,
                                        0, gameplayFrames * -16, 64, 32));
        gSPDisplayList(POLY_XLU_DISP++, sDiamondModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void MagicDark_OrbDraw(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    Vec3f pos;
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 sp6C = play->state.frames & 0x1F;

    if (this->timer < 32) {
        pos.x = (player->bodyPartsPos[12].x + player->bodyPartsPos[15].x) * 0.5f;
        pos.y = (player->bodyPartsPos[12].y + player->bodyPartsPos[15].y) * 0.5f;
        pos.z = (player->bodyPartsPos[12].z + player->bodyPartsPos[15].z) * 0.5f;
        if (this->timer > 20) {
            pos.y += (this->timer - 20) * 1.4f;
        }
        this->orbOffset = pos;
    } else if (this->timer < 130) {
        pos = this->orbOffset;
    } else {
        return;
    }

    pos.x -= (this->actor.scale.x * 300.0f * Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) *
              Math_CosS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));
    pos.y -= (this->actor.scale.x * 300.0f * Math_SinS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));
    pos.z -= (this->actor.scale.x * 300.0f * Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) *
              Math_CosS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 255);
    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_Push();
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Matrix_RotateZ(sp6C * (M_PI / 32), MTXMODE_APPLY);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    Matrix_Pop();
    Matrix_RotateZ(-sp6C * (M_PI / 32), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
