/*
 * File: z_en_encount3.c
 * Overlay: ovl_En_Encount3
 * Description: Garo spawner
 */

#include "z_en_encount3.h"
//#include "objects/object_big_fwall/object_big_fwall.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "../MMActorCompat.h"

#define THIS ((EnEncount3*)thisx)

extern s16 gEnJsoId;
s16 gEnEncount3Id;

void EnEncount3_Init(Actor* thisx, PlayState* play);
void EnEncount3_Destroy(Actor* thisx, PlayState* play);
void EnEncount3_Update(Actor* thisx, PlayState* play2);
void EnEncount3_Draw(Actor* thisx, PlayState* play);

void func_809AD058(EnEncount3* this);

void func_809AD084(EnEncount3* this, PlayState* play);
void func_809AD194(EnEncount3* this, PlayState* play);
void func_809AD1EC(EnEncount3* this, PlayState* play);

s32 D_809AD810 = false;

void EnEncount3_Init(Actor* thisx, PlayState* play) {
    LUSLOG_INFO("EnEncount3_Init(0x%X, 0x%X)", thisx, play);
    EnEncount3* this = THIS;

    this->unk14A = ENCOUNT3_GET_SPAWN_INDEX(thisx);
    this->childParams = ENCOUNT3_GET_PARAM_F80(thisx);
    this->switchFlag = ENCOUNT3_GET_SWITCH_FLAG(thisx);
    LUSLOG_INFO("EnEncount3_Init(0x%X, 0x%X): this->unk14A=%d this->childParams=%d this->switchFlag=%d", thisx, play,
                this->unk14A, this->childParams, this->switchFlag);

    this->unk14A = 0;
    this->childParams = 0;
    this->switchFlag = -1;

    // this->csId = this->actor.csId;
    this->unk16C = ENCOUNT3_GET_ROTZ(thisx) * 10.0f;
    if (this->unk16C < 10.0f) {
        this->unk16C = 10.0f;
    } else if (this->unk16C > 1000.0f) {
        this->unk16C = 1000.0f;
    }
    if (this->switchFlag == ENCOUNT3_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        LUSLOG_INFO("EnEncount3_Init(0x%X, 0x%X): Actor_Kill(0x%X)", thisx, play, &this->actor);
        Actor_Kill(&this->actor);
    }
    this->actor.flags |= ACTOR_FLAG_CANT_LOCK_ON;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    func_809AD058(this);
}

void EnEncount3_Destroy(Actor* thisx, PlayState* play) {
}

void func_809AD058(EnEncount3* this) {
    this->childActorId = gEnJsoId;
    this->unk150 = 1;
    this->timer = 30;
    this->actionFunc = func_809AD084;
}

void func_809AD084(EnEncount3* this, PlayState* play) {
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        LUSLOG_INFO("func_809AD084(0x%X, 0x%X): Actor_Kill(0x%X)", this, play, &this->actor);
        Actor_Kill(&this->actor);
        return;
    }
    /**/
    LUSLOG_INFO("func_809AD084(0x%X, 0x%X): this->timer=%d this->unk16C=%f this->actor.xzDistToPlayer=%f", this, play,
                this->timer, this->unk16C, this->actor.xzDistToPlayer);
    /**/
    if (!(this->unk16C < this->actor.xzDistToPlayer) && /*(Player_GetMask(play) == PLAYER_MASK_GARO)*/ true &&
        !D_809AD810) {
        if (this->timer > 0) {
            this->timer--;
        } else {
            this->child =
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, this->childActorId, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, this->childParams);
            LUSLOG_INFO("func_809AD084(0x%X, 0x%X): this->child=0x%X", this, play, this->child);
            if (this->child != NULL) {
                this->unk14E++;
                D_809AD810 = true;
                this->actionFunc = func_809AD194;
            }
        }
    }
}

void func_809AD194(EnEncount3* this, PlayState* play) {
    if (this->unk14E == 0) {
        this->unk178 = 0.0f;
        if (this->switchFlag > SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        this->actionFunc = func_809AD1EC;
    }
}

void func_809AD1EC(EnEncount3* this, PlayState* play) {
    if (this->unk174 < 0.002f) {
        D_809AD810 = false;
        LUSLOG_INFO("func_809AD1EC(0x%X, 0x%X): Actor_Kill(0x%X)", this, play, &this->actor);
        Actor_Kill(&this->actor);
    }
}

void EnEncount3_Update(Actor* thisx, PlayState* play2) {
    EnEncount3* this = THIS;
    f32 new_var;
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    f32 sp3C;
    f32 sp38;

    this->actionFunc(this, play);
    if (this->actionFunc == func_809AD194) {
        new_var = this->unk16C * 0.52f;
        if (new_var < this->actor.xzDistToPlayer) {
            this->unk178 = ((this->unk16C * 0.03125f) * 0.001f) + 0.1f;
        } else {
            this->unk178 = 0.06f;
        }

        LUSLOG_INFO("EnEncount3_Update(0x%X, 0x%X): this->unk16C=%f this->actor.xzDistToPlayer=%f "
                    "this->actionFunc=0x%X func_809AD194=0x%X",
                    thisx, play2, this->unk16C, this->actor.xzDistToPlayer, this->actionFunc, func_809AD194);
        if (((this->unk16C + 50.0f) /*+ BREG(0)*/) < this->actor.xzDistToPlayer) {
            if (this->actionFunc == func_809AD194) {
                this->unk148 = 0;
                this->unk178 = 0.0f;
                D_809AD810 = false;
                if (((this->child != NULL) && (this->child->update != NULL)) && (this->child->colChkInfo.health > 0)) {
                    LUSLOG_INFO("EnEncount3_Update(0x%X, 0x%X): Actor_Kill(0x%X) this->child->colChkInfo.health=%d",
                                thisx, play2, this->child, this->child->colChkInfo.health);
                    Actor_Kill(this->child);
                    this->child = NULL;
                }
                func_809AD058(this);
            }
        } else if (this->unk16C < this->actor.xzDistToPlayer) {
            s16 i;

            for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
                player->bodyFlameTimers[i] = Rand_S16Offset(0, 200);
            }
            player->bodyIsBurning = true;

            sp3C = this->actor.world.pos.x - player->actor.world.pos.x;
            sp38 = this->actor.world.pos.z - player->actor.world.pos.z;
            if (!Play_InCsMode(play)) {
                func_800B8D50(play, &this->actor, 10.0f, Math_Atan2S_XY(sp38, sp3C), 0.0f, 1);
            }
        }
        this->child->colChkInfo = this->child->colChkInfo;
    }

    this->unk168 = this->unk16C;
    this->unk168 /= 7666.0f;
    if (this->actionFunc != func_809AD194) {
        Math_ApproachZeroF(&this->unk170, 0.3f, 10.0f);
        Math_ApproachZeroF(&this->unk160, 0.3f, 5.0f);
        if (this->unk160 < 1.0f) {
            // play->unk_18880 = false;
        }
    } else if (this->unk148 != 0) {
        Math_ApproachF(&this->unk170, 255.0f, 0.4f, 10.0f);
        Math_ApproachF(&this->unk160, 60.0f, 0.3f, 5.0f);
    }
    Math_ApproachF(&this->unk174, this->unk178, 0.3f, 0.03f);

    this->unk164 = this->unk160 / 60.0f;
    if (this->unk164 != 0.0f) {
        play->lightCtx.fogNear = (960 - play->lightCtx.fogNear) * this->unk164;
        play->lightCtx.fogColor[0] = (40.0f - play->lightCtx.fogColor[0]) * this->unk164;
        play->lightCtx.fogColor[1] = (10.0f - play->lightCtx.fogColor[1]) * this->unk164;
        play->lightCtx.fogColor[2] = (0.0f - play->lightCtx.fogColor[2]) * this->unk164;
    }
}

void EnEncount3_Draw(Actor* thisx, PlayState* play) {
    EnEncount3* this = THIS;
    s32 pad;

    if (this->unk170 > 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Push();

        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (s32)play->gameplayFrames, 0, 0x20, 0x40, 1,
                                    (s32)play->gameplayFrames * -2, (s32)play->gameplayFrames * -8, 0x20, 0x20));

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 200, 0, (s8)this->unk170);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y - 40.0f, this->actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_Scale(this->unk168, this->unk174, this->unk168, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gRingOfFireDL);

        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}