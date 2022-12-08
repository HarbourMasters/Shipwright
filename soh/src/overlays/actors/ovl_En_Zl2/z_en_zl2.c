/*
 * File: z_en_zl2.c
 * Overlay: ovl_En_Zl2
 * Description: Adult Zelda (Cutscenes)
 */

#include "z_en_zl2.h"
#include "vt.h"

#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "objects/object_zl2/object_zl2.h"
#include "objects/object_zl2_anime1/object_zl2_anime1.h"

#define FLAGS ACTOR_FLAG_4

void EnZl2_Init(Actor* thisx, PlayState* play);
void EnZl2_Destroy(Actor* thisx, PlayState* play);
void EnZl2_Update(Actor* thisx, PlayState* play);
void EnZl2_Draw(Actor* thisx, PlayState* play);

s32 func_80B4F45C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx);

void func_80B50BBC(EnZl2* this, PlayState* play);
void func_80B50BEC(EnZl2* this, PlayState* play);
void func_80B50C40(EnZl2* this, PlayState* play);
void func_80B50CA8(EnZl2* this, PlayState* play);
void func_80B50CFC(EnZl2* this, PlayState* play);
void func_80B50D50(EnZl2* this, PlayState* play);
void func_80B50D94(EnZl2* this, PlayState* play);
void func_80B50DE8(EnZl2* this, PlayState* play);
void func_80B50E3C(EnZl2* this, PlayState* play);
void func_80B50E90(EnZl2* this, PlayState* play);
void func_80B50EE4(EnZl2* this, PlayState* play);
void func_80B50F38(EnZl2* this, PlayState* play);
void func_80B50F8C(EnZl2* this, PlayState* play);
void func_80B50FE8(EnZl2* this, PlayState* play);
void func_80B51034(EnZl2* this, PlayState* play);
void func_80B51080(EnZl2* this, PlayState* play);
void func_80B510CC(EnZl2* this, PlayState* play);
void func_80B51118(EnZl2* this, PlayState* play);
void func_80B51164(EnZl2* this, PlayState* play);
void func_80B511B0(EnZl2* this, PlayState* play);
void func_80B511FC(EnZl2* this, PlayState* play);
void func_80B51250(EnZl2* this, PlayState* play);
void func_80B512B8(EnZl2* this, PlayState* play);
void func_80B51310(EnZl2* this, PlayState* play);
void func_80B51A5C(EnZl2* this, PlayState* play);
void func_80B51A8C(EnZl2* this, PlayState* play);
void func_80B51AE4(EnZl2* this, PlayState* play);
void func_80B51B44(EnZl2* this, PlayState* play);
void func_80B51BA8(EnZl2* this, PlayState* play);
void func_80B51C0C(EnZl2* this, PlayState* play);
void func_80B51C64(EnZl2* this, PlayState* play);
void func_80B51CA8(EnZl2* this, PlayState* play);
void func_80B52068(EnZl2* this, PlayState* play);
void func_80B52098(EnZl2* this, PlayState* play);
void func_80B52108(EnZl2* this, PlayState* play);
void func_80B521A0(EnZl2* this, PlayState* play);
void func_80B523BC(EnZl2* this, PlayState* play);
void func_80B523C8(EnZl2* this, PlayState* play);
void func_80B525D4(EnZl2* this, PlayState* play);

static void* sEyeTextures[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex,
                                gZelda2Eye03Tex,   gZelda2Eye04Tex,   gZelda2Eye05Tex,
                                gZelda2Eye06Tex,   gZelda2Eye07Tex,   gZelda2Eye08Tex };

static void* sMouthTextures[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

static EnZl2ActionFunc sActionFuncs[] = {
    func_80B521A0, func_80B50BBC, func_80B50BEC, func_80B50C40, func_80B50CA8, func_80B50CFC,
    func_80B50D50, func_80B50D94, func_80B50DE8, func_80B50E3C, func_80B50E90, func_80B50EE4,
    func_80B50F38, func_80B50F8C, func_80B50FE8, func_80B51034, func_80B51080, func_80B510CC,
    func_80B51118, func_80B51164, func_80B511B0, func_80B511FC, func_80B51250, func_80B512B8,
    func_80B51310, func_80B51A5C, func_80B51A8C, func_80B51AE4, func_80B51B44, func_80B51BA8,
    func_80B51C0C, func_80B51C64, func_80B51CA8, func_80B52068, func_80B52098, func_80B52108,
};

static OverrideLimbDraw sOverrideLimbDrawFuncs[] = {
    func_80B4F45C,
};

static EnZl2DrawFunc sDrawFuncs[] = {
    func_80B523BC,
    func_80B523C8,
    func_80B525D4,
};

const ActorInit En_Zl2_InitVars = {
    ACTOR_EN_ZL2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZL2,
    sizeof(EnZl2),
    (ActorFunc)EnZl2_Init,
    (ActorFunc)EnZl2_Destroy,
    (ActorFunc)EnZl2_Update,
    (ActorFunc)EnZl2_Draw,
    NULL,
};

void EnZl2_Destroy(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void EnZl2_UpdateEyes(EnZl2* this) {
    s32 pad[4];
    s16* eyeTexIndex2 = &this->eyeTexIndex2;
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
    *eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EA40(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 3.0f) {
        *eyeTexIndex = 0;
    } else if (*unk_27C < 6.0f) {
        *eyeTexIndex = 1;
    } else if (*unk_27C < 9.0f) {
        *eyeTexIndex = 2;
    } else {
        *eyeTexIndex = 5;
        this->eyeTexIndex2 = *eyeTexIndex;
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EAF4(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 5;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 6.0f) {
        *eyeTexIndex = 1;
    } else {
        EnZl2_UpdateEyes(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EBB8(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 0;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 1;
    } else {
        *eyeTexIndex = 2;
        this->eyeTexIndex2 = *eyeTexIndex;
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EC48(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 1;
    } else {
        EnZl2_UpdateEyes(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void EnZl2_setEyesIndex(EnZl2* this, s16 index) {
    this->eyeTexIndex = index;
    this->eyeTexIndex2 = this->eyeTexIndex;
}

void EnZl2_setEyeIndex2(EnZl2* this, s16 index) {
    this->eyeTexIndex2 = index;
}

void EnZl2_setMouthIndex(EnZl2* this, s16 index) {
    this->mouthTexIndex = index;
}

void func_80B4ED2C(EnZl2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 5);
}

s32 EnZl2_UpdateSkelAnime(EnZl2* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorAction* EnZl2_GetNpcAction(PlayState* play, s32 idx) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[idx];
    }
    return NULL;
}

void func_80B4EDB8(EnZl2* this, PlayState* play, s32 arg2) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, arg2);

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_80B4EE38(EnZl2* this, s16 arg1, s32 arg2) {
    s32 phi_a3;
    s32 temp_v1;
    s32 phi_v0 = arg2;

    if (this->unk_24C != 0) {
        temp_v1 = (s16)(arg1 + this->unk_1DC[phi_v0]);
        phi_a3 = arg1 - this->unk_20C[phi_v0];
        phi_v0 = this->unk_1AC[phi_v0];

        if ((s32)fabsf((f32)phi_a3) > 0x8000) {
            if (arg1 > 0) {
                phi_a3 -= 0x10000;
            } else {
                phi_a3 += 0x10000;
            }
        }
        if (phi_a3 != 0) {
            phi_v0 += (phi_a3 - phi_v0) / 16;
        }
        if (phi_v0 != 0) {
            phi_v0 -= (phi_v0 / 10);
        }
        if ((s16)(temp_v1 - arg1) != 0) {
            phi_v0 -= ((s16)(temp_v1 - arg1) / 50);
        }
        temp_v1 += phi_v0;
        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && ((s16)(temp_v1 - arg1) > -0x64) &&
            ((s16)(temp_v1 - arg1) < 0x64)) {
            temp_v1 = arg1;
            phi_v0 = 0;
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = temp_v1 - arg1;
    }
    this->unk_20C[arg2] = arg1;
}

void func_80B4EF64(EnZl2* this, s16 arg1, s32 arg2) {
    s32 temp_t0 = arg2;
    s32 temp_t2;
    s32 temp_v1;
    s32 phi_t1;
    s32 phi_v0;
    s32 phi_a0;
    f32 curFrame;
    f32 unk_278;

    if (temp_t0 == 2) {
        phi_a0 = 0x3A98;
        phi_t1 = 0;
    } else if (temp_t0 == 5) {
        phi_a0 = 0x32C8;
        phi_t1 = 3;
    } else if (temp_t0 == 8) {
        phi_a0 = 0x2EE0;
        phi_t1 = 6;
    } else if (temp_t0 == 11) {
        phi_a0 = 0x4000;
        phi_t1 = 9;
    } else if (temp_t0 == 14) {
        phi_a0 = 0x4000;
        phi_t1 = 12;
    } else if (temp_t0 == 17) {
        phi_a0 = 0x4000;
        phi_t1 = 15;
    } else {
        phi_a0 = 0x4000;
        phi_t1 = 18;
    }

    if (this->unk_24C != 0) {
        phi_v0 = this->unk_1DC[temp_t0] + arg1;
        temp_v1 = (s16)(phi_v0 & 0xFFFF);
        temp_t2 = arg1 - this->unk_20C[temp_t0];
        phi_v0 = this->unk_1AC[temp_t0];

        if ((s32)fabsf((f32)temp_t2) > 0x8000) {
            if (arg1 > 0) {
                temp_t2 -= 0x10000;
            } else {
                temp_t2 += 0x10000;
            }
        }
        if (phi_t1 >= 0) {
            temp_t2 += (ABS(this->unk_1AC[phi_t1]) / 3);
        }
        if (temp_t2 != 0) {
            phi_v0 += ((temp_t2 - phi_v0) / 16);
        }
        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }
        if ((s16)(temp_v1 - phi_a0) != 0) {
            phi_v0 -= (s16)(temp_v1 - phi_a0) / 50;
        }
        temp_v1 += phi_v0;

        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && ((s16)(temp_v1 - phi_a0) > -0x64) &&
            ((s16)(temp_v1 - phi_a0) < 0x64)) {
            temp_v1 = phi_a0;
            phi_v0 = 0;
        }

        if (arg2 == 2) {
            if ((this->action == 5) || (this->action == 30)) {
                curFrame = this->skelAnime.curFrame;
                unk_278 = this->unk_278;
                temp_t0 = (s32)((3500.0f * curFrame) / unk_278) + phi_a0;
                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            } else if ((this->action == 6) || (this->action == 31)) {
                temp_t0 = phi_a0 + 0xDAC;
                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            } else if (this->action == 20) {
                temp_t0 = phi_a0 - 0x3E8;
                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            }
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = temp_v1 - arg1;
    }
    this->unk_20C[arg2] = arg1;
}

void func_80B4F230(EnZl2* this, s16 arg1, s32 arg2) {
    s32 temp_v1;
    s16 temp_t0;
    s32 temp_t2;
    s32 temp_t3;
    s32 phi_v0;
    s32 index1AC;
    s32 phi_t5;

    if (this->unk_24C != 0) {
        temp_v1 = this->unk_1DC[arg2] - arg1;
        temp_t0 = temp_v1;
        temp_t2 = temp_t0;
        temp_t3 = this->unk_1AC[arg2];
        phi_v0 = temp_t3;
        temp_t3 = arg1 - this->unk_20C[arg2];

        if (arg2 == 1) {
            index1AC = 0;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 4) {
            index1AC = 3;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 7) {
            index1AC = 6;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 10) {
            index1AC = 9;
            phi_t5 = ABS(this->unk_1AC[index1AC]);
        } else if (arg2 == 13) {
            index1AC = 12;
            phi_t5 = ABS(this->unk_1AC[index1AC]);
        } else if (arg2 == 16) {
            index1AC = 15;
            phi_t5 = -ABS(this->unk_1AC[index1AC]);
        } else {
            index1AC = 18;
            phi_t5 = -ABS(this->unk_1AC[index1AC]);
        }

        if ((s32)fabsf(temp_t3) > 0x8000) {
            if (arg1 > 0) {
                temp_t3 -= 0x10000;
            } else {
                temp_t3 += 0x10000;
            }
        }
        if (index1AC >= 0) {
            temp_t3 += phi_t5 / 3;
        }

        if (temp_t3 != 0) {
            phi_v0 += (temp_t3 - phi_v0) / 16;
        }
        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }
        if (temp_t0 != 0) {
            phi_v0 -= temp_t0 / 50;
        }
        temp_v1 += phi_v0;
        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && (temp_t2 > -0x64) && (temp_t2 < 0x64)) {
            temp_v1 = 0;
            phi_v0 = 0;
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = arg1 + temp_v1;
    }
    this->unk_20C[arg2] = arg1;
}

s32 func_80B4F45C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                  Gfx** gfx) {
    s32 pad;
    EnZl2* this = (EnZl2*)thisx;
    Mtx* sp74;
    MtxF sp34;
    Vec3s sp2C;
    s16 pad2;
    s16* unk_1DC = this->unk_1DC;

    if (limbIndex == 14) {
        sp74 = Graph_Alloc(play->state.gfxCtx, sizeof(Mtx) * 7);
        gSPSegment((*gfx)++, 0x0C, sp74);

        Matrix_Push();
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 0);
            func_80B4F230(this, sp2C.x, 1);
            func_80B4EF64(this, sp2C.z, 2);
        }
        Matrix_RotateZYX(unk_1DC[0] + kREG(31), unk_1DC[1] + kREG(32), unk_1DC[2] + kREG(33), MTXMODE_APPLY);
        Matrix_Translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[0]);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 3);
            func_80B4F230(this, sp2C.x, 4);
        }
        Matrix_RotateZYX(unk_1DC[3] + kREG(34), unk_1DC[4] + kREG(35), unk_1DC[5] + kREG(36), MTXMODE_APPLY);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[1]);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 6);
            func_80B4F230(this, sp2C.x, 7);
        }
        Matrix_RotateZYX(unk_1DC[6] + kREG(37), unk_1DC[7] + kREG(38), unk_1DC[8] + kREG(39), MTXMODE_APPLY);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[2]);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 9);
            func_80B4F230(this, sp2C.x, 10);
            func_80B4EF64(this, sp2C.z, 11);
        }
        Matrix_RotateZYX(unk_1DC[9] + kREG(40), unk_1DC[10] + kREG(41), unk_1DC[11] + kREG(42), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[3]);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 12);
            func_80B4F230(this, sp2C.x, 13);
            func_80B4EF64(this, sp2C.z, 14);
        }
        Matrix_RotateZYX(unk_1DC[12] + kREG(43), unk_1DC[13] + kREG(44), unk_1DC[14] + kREG(45), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[4]);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 15);
            func_80B4F230(this, sp2C.x, 16);
            func_80B4EF64(this, sp2C.z, 17);
        }
        Matrix_RotateZYX(unk_1DC[15] + kREG(46), unk_1DC[16] + kREG(47), unk_1DC[17] + kREG(48), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[5]);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 18);
            func_80B4F230(this, sp2C.x, 19);
            func_80B4EF64(this, sp2C.z, 20);
        }
        Matrix_RotateZYX(unk_1DC[18] + kREG(49), unk_1DC[19] + kREG(50), unk_1DC[20] + kREG(51), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TOMTX(&sp74[6]);
        Matrix_Pop();
        Matrix_Pop();
        this->unk_24C = 1;
    }
    return false;
}

void EnZl2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl2* this = (EnZl2*)thisx;
    s32 pad[2];

    if (limbIndex == 10) {
        if ((this->unk_254 != 0) && (play->csCtx.frames >= 900)) {
            gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
        }

        {
            Player* player = GET_PLAYER(play);
            Matrix_Push();
            if (player->rightHandType == 0xFF) {
                Matrix_Put(&player->shieldMf);
                Matrix_Translate(180.0f, 979.0f, -375.0f, MTXMODE_APPLY);
                Matrix_RotateZYX(-0x5DE7, -0x53E9, 0x3333, MTXMODE_APPLY);
                Matrix_Scale(1.2f, 1.2f, 1.2f, MTXMODE_APPLY);
                gSPMatrix((*gfx)++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
            }
            Matrix_Pop();
        }
    }
}

void func_80B4FCCC(EnZl2* this, PlayState* play) {
    s32 unk_274 = this->unk_274;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[unk_274].segment);
}

void func_80B4FD00(EnZl2* this, AnimationHeader* animation, u8 arg2, f32 transitionRate, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        fc = 0.0f;
        unk0 = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, transitionRate);
}

void func_80B4FD90(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, 0.0f, 0);
    this->action = 1;
}

void func_80B4FDD4(EnZl2* this) {
    if (Animation_OnFrame(&this->skelAnime, 14.0f)) {
        func_80078914(&this->actor.projectedPos, NA_SE_PL_WALK_CONCRETE);
    }
}

void func_80B4FE10(PlayState* play) {
    if ((play->csCtx.frames >= 830) && (play->csCtx.frames < 1081)) {
        func_800788CC(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    }
}

void func_80B4FE48(EnZl2* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_EV_GOTO_HEAVEN - SFX_FLAG);
}

void func_80B4FE6C(EnZl2* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_EN_GANON_LAUGH);
}

void func_80B4FE90(EnZl2* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_VO_Z1_SURPRISE);
}

void func_80B4FEB4(EnZl2* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_VO_Z1_PAIN);
}

void func_80B4FED8(EnZl2* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_VO_Z1_CRY_0);
}

void EnZl2_GiveLightArrows(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_244 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y + 80.0f;
        posZ = player->actor.world.pos.z;
        Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 0x17, true);
        Item_Give(play, ITEM_ARROW_LIGHT);
        this->unk_244 = 1;
    }
}

void func_80B4FF84(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y;
        posZ = this->actor.world.pos.z;

        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_YELLOW, true);
        this->unk_250 = 1;
    }
}

void func_80B4FFF0(EnZl2* this, PlayState* play) {
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_248 == 0) {
        posX = this->actor.world.pos.x;
        posY = this->actor.world.pos.y + (kREG(5) + -26.0f);
        posZ = this->actor.world.pos.z;

        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0x4000,
                           0, WARP_PURPLE_CRYSTAL);
        this->unk_248 = 1;
    }
}

void func_80B5008C(EnZl2* this) {
    Actor* child = this->actor.child;

    if (child != NULL) {
        child->world.pos.x = this->actor.world.pos.x;
        child->world.pos.y = this->actor.world.pos.y + (kREG(5) + -26.0f);
        child->world.pos.z = this->actor.world.pos.z;
    }
}

void func_80B500E0(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    Vec3f* thisPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 someFloat;

    if (npcAction != NULL) {
        someFloat =
            Environment_LerpWeightAccelDecel(npcAction->endFrame, npcAction->startFrame, play->csCtx.frames, 8, 8);
        startX = npcAction->startPos.x;
        startY = npcAction->startPos.y;
        startZ = npcAction->startPos.z;
        endX = npcAction->endPos.x;
        endY = npcAction->endPos.y;
        endZ = npcAction->endPos.z;
        thisPos->x = ((endX - startX) * someFloat) + startX;
        thisPos->y = ((endY - startY) * someFloat) + startY;
        thisPos->z = ((endZ - startZ) * someFloat) + startZ;
    }
}

void func_80B501C4(EnZl2* this, s32 alpha) {
    if (this->actor.child != NULL) {
        ((DoorWarp1*)this->actor.child)->crystalAlpha = alpha;
    }
}

void func_80B501E8(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);

    if (npcAction != NULL) {
        this->actor.shape.shadowAlpha = this->alpha =
            (1.0f - Environment_LerpWeight(npcAction->endFrame, npcAction->startFrame, play->csCtx.frames)) *
            255.0f;
        func_80B501C4(this, this->alpha);
    }
}

void func_80B50260(EnZl2* this, PlayState* play) {
    this->action = 1;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B50278(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);

    this->actor.world.pos.x = npcAction->startPos.x;
    this->actor.world.pos.y = npcAction->startPos.y;
    this->actor.world.pos.z = npcAction->startPos.z;
    this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    this->actor.shape.shadowAlpha = 0xFF;
    this->action = 2;
    this->drawConfig = 1;
}

void func_80B50304(EnZl2* this, PlayState* play) {
    s32 pad[2];
    ActorShape* shape = &this->actor.shape;
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    f32 actionXDelta;
    f32 actionZDelta;

    actionXDelta = npcAction->endPos.x - npcAction->startPos.x;
    actionZDelta = npcAction->endPos.z - npcAction->startPos.z;
    func_80B4FD00(this, &gZelda2Anime1Anim_0003BC, 0, -12.0f, 0);
    this->action = 3;
    this->drawConfig = 1;
    this->unk_23C = 0.0f;
    shape->shadowAlpha = 255;
    this->actor.world.rot.y = shape->rot.y = Math_FAtan2F(actionXDelta, actionZDelta) * (0x8000 / M_PI);
}

void func_80B503DC(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);

    if ((npcAction != NULL) && (play->csCtx.frames >= npcAction->endFrame)) {
        this->action = 4;
    }
}

void func_80B5042C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 5;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B50488(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002750, 0, 0.0f, 0);
        this->action = 6;
        this->drawConfig = 1;
    }
}

void func_80B504D4(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00325C, 2, -8.0f, 0);
    this->action = 7;
    this->drawConfig = 1;
    this->unk_27C = 0.0f;
    EnZl2_setMouthIndex(this, 1);
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B5053C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_003538, 0, 0.0f, 0);
        this->action = 8;
    }
}

void func_80B50580(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_000A50, 2, -8.0f, 0);
    this->action = 9;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B505D4(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_000EB0, 0, 0.0f, 0);
        this->action = 10;
    }
}

void func_80B50618(EnZl2* this, PlayState* play) {
    EnZl2_GiveLightArrows(this, play);
    this->action = 11;
}

void func_80B50644(EnZl2* this, PlayState* play) {
    func_80B4FF84(this, play);
    this->action = 12;
}

void func_80B50670(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
    this->action = 13;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B506C4(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00AAD4, 2, -8.0f, 0);
    this->action = 14;
    this->drawConfig = 1;
    EnZl2_setEyesIndex(this, 4);
    EnZl2_setMouthIndex(this, 2);
    this->actor.shape.shadowAlpha = 0xFF;
    func_80B4FE90(this);
}

void func_80B5073C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_00AFE0, 0, 0.0f, 0);
        this->action = 15;
    }
}

void func_80B50780(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_001670, 2, -8.0f, 0);
    this->action = 16;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    func_80B4FFF0(this, play);
    EnZl2_setEyesIndex(this, 3);
}

void func_80B507E8(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_001B48, 0, 0.0f, 0);
        this->action = 17;
    }
}

void func_80B5082C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_002B14, 2, -8.0f, 0);
    this->action = 18;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B50880(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002F30, 0, 0.0f, 0);
        this->action = 19;
        func_80B4FEB4(this);
    }
}

void func_80B508C8(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_001010, 2, -8.0f, 0);
    this->action = 20;
    this->drawConfig = 1;
    EnZl2_setEyesIndex(this, 6);
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B50928(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_0013A0, 0, 0.0f, 0);
        this->action = 21;
        func_80B4FED8(this);
    }
}

void func_80B50970(EnZl2* this, PlayState* play) {
    this->action = 22;
}

void func_80B50980(EnZl2* this, PlayState* play) {
    this->action = 23;
    this->drawConfig = 2;
    this->alpha = 255;
}

void func_80B509A0(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);

    if (npcAction != NULL) {
        if (play->csCtx.frames >= npcAction->endFrame) {
            this->action = 24;
            this->drawConfig = 0;
            func_80B4FE6C(this);
        }
    }
}

void func_80B50A04(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    s32 newAction;
    s32 unk_240;

    if (npcAction != NULL) {
        newAction = npcAction->action;
        unk_240 = this->unk_240;
        if (newAction != unk_240) {
            switch (newAction) {
                case 1:
                    func_80B50260(this, play);
                    break;
                case 2:
                    func_80B50278(this, play);
                    break;
                case 3:
                    func_80B50304(this, play);
                    break;
                case 4:
                    func_80B5042C(this, play);
                    break;
                case 5:
                    func_80B504D4(this, play);
                    break;
                case 6:
                    func_80B50580(this, play);
                    break;
                case 7:
                    func_80B50618(this, play);
                    break;
                case 8:
                    func_80B50670(this, play);
                    break;
                case 9:
                    func_80B506C4(this, play);
                    break;
                case 10:
                    func_80B50780(this, play);
                    break;
                case 11:
                    func_80B5082C(this, play);
                    break;
                case 12:
                    func_80B508C8(this, play);
                    break;
                case 13:
                    func_80B50970(this, play);
                    break;
                case 14:
                    func_80B50980(this, play);
                    break;
                case 15:
                    func_80B50644(this, play);
                    break;
                default:
                    osSyncPrintf("En_Zl2_inAgain_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_240 = newAction;
        }
    }
}

void func_80B50BBC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B50A04(this, play);
}

void func_80B50BEC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50C40(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FDD4(this);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B500E0(this, play);
    func_80B503DC(this, play);
}

void func_80B50CA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50CFC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B4EA40(this);
    func_80B50488(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50D50(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50D94(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B4EAF4(this);
    func_80B5053C(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50DE8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50E3C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    func_80B505D4(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B50E90(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50EE4(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50F38(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50F8C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B50FE8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B5073C(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51034(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B51080(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B507E8(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B510CC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B51118(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B50880(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51164(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B50A04(this, play);
}

void func_80B511B0(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE10(play);
    func_80B4ED2C(this, play);
    func_80B50928(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B511FC(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B5008C(this);
    func_80B50A04(this, play);
}

void func_80B51250(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4FE48(this);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B500E0(this, play);
    func_80B5008C(this);
    func_80B50A04(this, play);
}

void func_80B512B8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B501E8(this, play);
    func_80B509A0(this, play);
}

void func_80B51310(EnZl2* this, PlayState* play) {
    Actor* child;

    if (EnZl2_GetNpcAction(play, 0) == NULL) {
        child = this->actor.child;
        if (child != NULL) {
            Actor_Kill(child);
        }
        Actor_Kill(&this->actor);
    }
}

void func_80B5135C(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00A15C, 0, 0.0f, 0);
    this->action = 25;
    this->unk_254 = 1;
}

void func_80B513A8(EnZl2* this, PlayState* play) {
    Player* player;
    f32 posX;
    f32 posY;
    f32 posZ;

    if (this->unk_250 == 0) {
        player = GET_PLAYER(play);
        posX = player->actor.world.pos.x;
        posY = player->actor.world.pos.y;
        posZ = player->actor.world.pos.z;
        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_UNK_7, true);
        this->unk_250 = 1;
    }
}

void func_80B51418(EnZl2* this, PlayState* play) {
    EnZl2_UpdateEyes(this);
    if (play->csCtx.frames < 431) {
        EnZl2_setMouthIndex(this, 1);
    } else {
        EnZl2_setMouthIndex(this, 0);
    }
}

void func_80B5146C(EnZl2* this, PlayState* play) {
    func_80B4EA40(this);
    EnZl2_setMouthIndex(this, 0);
}

void func_80B5149C(EnZl2* this, PlayState* play) {
    if (play->csCtx.frames < 988) {
        EnZl2_setEyesIndex(this, 7);
        EnZl2_setEyeIndex2(this, 8);
    } else {
        EnZl2_UpdateEyes(this);
    }
    EnZl2_setMouthIndex(this, 0);
}

void func_80B514F8(EnZl2* this, PlayState* play) {
    EnZl2_UpdateEyes(this);
    if (play->csCtx.frames < 1190) {
        EnZl2_setMouthIndex(this, 1);
    } else {
        EnZl2_setMouthIndex(this, 0);
    }
}

void func_80B5154C(EnZl2* this, PlayState* play) {
    CutsceneContext* csCtx;

    if (this->skelAnime.mode != 0) {
        EnZl2_UpdateEyes(this);
    } else {
        csCtx = &play->csCtx;
        if (csCtx->frames < 0x5F0) {
            func_80B4EBB8(this);
        } else if (csCtx->frames == 0x5F0) {
            this->unk_27C = 0.0f;
        } else {
            func_80B4EC48(this);
        }
    }
}

void func_80B515C4(EnZl2* this) {
    this->action = 25;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B515D8(EnZl2* this, PlayState* play) {
    func_80B4FD00(this, &gZelda2Anime1Anim_00A15C, 0, -8.0f, 0);
    func_80B4EDB8(this, play, 0);
    this->action = 26;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B51644(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_0087B8, 0, -8.0f, 0);
    }
}

void func_80B51678(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_007D0C, 2, -8.0f, 0);
    this->action = 27;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B516D0(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_009AD4, 0, -8.0f, 0);
    }
}

void func_80B51704(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0090D8, 2, -8.0f, 0);
    this->action = 28;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B5175C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_006778, 0, -8.0f, 0);
    }
}

void func_80B51790(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_005F40, 2, -8.0f, 0);
    this->action = 29;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
}

void func_80B517E0(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_002750, 0, -8.0f, 0);
        this->action = 31;
    }
}

void func_80B51824(EnZl2* this) {
    func_80B4FD00(this, &gZelda2Anime1Anim_0022D0, 2, -8.0f, 0);
    this->action = 30;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B5187C(EnZl2* this, s32 arg1) {
    if (arg1 != 0) {
        func_80B4FD00(this, &gZelda2Anime1Anim_00A79C, 0, -8.0f, 0);
        this->unk_27C = 0.0f;
    }
}

void func_80B518C0(EnZl2* this) {
    func_80B4FD00(this, SEGMENTED_TO_VIRTUAL(&gZelda2Anime1Anim_004900), 2, -8.0f, 0);
    this->action = 32;
    this->drawConfig = 1;
    this->actor.shape.shadowAlpha = 0xFF;
    this->unk_27C = 0.0f;
}

void func_80B51948(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    s32 newAction;
    s32 unk_240;

    if (npcAction != NULL) {
        newAction = npcAction->action;
        unk_240 = this->unk_240;
        if (newAction != unk_240) {
            switch (newAction) {
                case 1:
                    func_80B515C4(this);
                    break;
                case 2:
                    func_80B515D8(this, play);
                    break;
                case 16:
                    func_80B51678(this);
                    break;
                case 17:
                    func_80B51704(this);
                    break;
                case 18:
                    func_80B51790(this);
                    break;
                case 4:
                    func_80B51824(this);
                    break;
                case 20:
                    func_80B518C0(this);
                    break;
                case 21:
                    func_80B513A8(this, play);
                    break;
                default:
                    osSyncPrintf("En_Zl2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
            }
            this->unk_240 = newAction;
        }
    }
}

void func_80B51A5C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B51948(this, play);
}

void func_80B51A8C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B51418(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B51948(this, play);
}

void func_80B51AE4(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    func_80B51644(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51B44(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5149C(this, play);
    func_80B516D0(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51BA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B514F8(this, play);
    func_80B5175C(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51C0C(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5146C(this, play);
    func_80B517E0(this, EnZl2_UpdateSkelAnime(this));
}

void func_80B51C64(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateSkelAnime(this);
    func_80B51948(this, play);
}

void func_80B51CA8(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    func_80B5154C(this, play);
    func_80B5187C(this, EnZl2_UpdateSkelAnime(this));
    func_80B51948(this, play);
}

void func_80B51D0C(EnZl2* this, PlayState* play) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B51D24(EnZl2* this, PlayState* play) {
    s32 pad[2];
    u32 sfxId;
    SkelAnime* skelAnime = &this->skelAnime;

    if (Animation_OnFrame(skelAnime, 6.0f) || Animation_OnFrame(skelAnime, 0.0f)) {
        if (this->actor.bgCheckFlags & 1) {
            sfxId = SFX_FLAG;
            sfxId += SurfaceType_GetSfx(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
            func_80078914(&this->actor.projectedPos, sfxId);
        }
    }
}

void func_80B51DA4(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    Vec3f* thisPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 someFloat;

    if (npcAction != NULL) {
        someFloat =
            Environment_LerpWeightAccelDecel(npcAction->endFrame, npcAction->startFrame, play->csCtx.frames, 0, 8);
        startX = npcAction->startPos.x;
        startY = npcAction->startPos.y;
        startZ = npcAction->startPos.z;
        endX = npcAction->endPos.x;
        endY = npcAction->endPos.y;
        endZ = npcAction->endPos.z;
        thisPos->x = ((endX - startX) * someFloat) + startX;
        thisPos->y = ((endY - startY) * someFloat) + startY;
        thisPos->z = ((endZ - startZ) * someFloat) + startZ;
        if (npcAction->endFrame < play->csCtx.frames) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80B51EA8(EnZl2* this) {
    this->action = 33;
    this->drawConfig = 0;
    this->actor.shape.shadowAlpha = 0;
}

void func_80B51EBC(EnZl2* this, PlayState* play) {
    ActorShape* shape = &this->actor.shape;
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    s32 pad[2];

    this->actor.world.rot.y = shape->rot.y = npcAction->rot.y;
    func_80B4FD00(this, &gZelda2Anime1Anim_00B224, 0, 0.0f, 0);
    this->action = 34;
    this->drawConfig = 1;
    shape->shadowAlpha = 255;
}

void func_80B51F38(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);

    if (npcAction != NULL) {
        if (play->csCtx.frames - 8 >= npcAction->endFrame) {
            func_80B4FD00(this, &gZelda2Anime1Anim_00B5FC, 0, -8.0f, 0);
            this->action = 35;
        }
    }
}

void func_80B51FA8(EnZl2* this, PlayState* play) {
    CsCmdActorAction* npcAction = EnZl2_GetNpcAction(play, 0);
    s32 action;
    s32 unk_240;

    if (npcAction != NULL) {
        action = npcAction->action;
        unk_240 = this->unk_240;
        if (action != unk_240) {
            switch (action) {
                case 1:
                    func_80B51EA8(this);
                    break;
                case 2:
                    func_80B51EBC(this, play);
                    break;
                case 14:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    osSyncPrintf("En_Zl2_inRunning_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->unk_240 = action;
        }
    }
}

void func_80B52068(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B51FA8(this, play);
}

void func_80B52098(EnZl2* this, PlayState* play) {
    func_80B4FCCC(this, play);
    func_80B4ED2C(this, play);
    EnZl2_UpdateEyes(this);
    EnZl2_UpdateSkelAnime(this);
    func_80B51D24(this, play);
    func_80B51F38(this, play);
    func_80B51DA4(this, play);
}

void func_80B52108(EnZl2* this, PlayState* play) {
}

void func_80B52114(EnZl2* this, PlayState* play) {
    switch (this->actor.params) {
        case 1:
            func_80B5135C(this, play);
            break;
        case 4:
            func_80B51D0C(this, play);
            break;
        case 0:
            func_80B4FD90(this, play);
            break;
        default:
            osSyncPrintf(VT_FGCOL(RED) " En_Oa2 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            func_80B4FD90(this, play);
    }
}

void func_80B521A0(EnZl2* this, PlayState* play) {
    s32 pad;
    ObjectContext* objectCtx = &play->objectCtx;
    s32 bankIndex = Object_GetIndex(objectCtx, OBJECT_ZL2_ANIME1);
    s32 pad2;

    if (bankIndex < 0) {
        osSyncPrintf(VT_FGCOL(RED) "En_Zl2_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    if (Object_IsLoaded(objectCtx, bankIndex)) {
        this->unk_274 = bankIndex;
        func_80B4FCCC(this, play);
        this->unk_278 = Animation_GetLastFrame(&gZelda2Anime1Anim_0022D0);
        func_80B52114(this, play);
    }
}

void EnZl2_Update(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    if (this->action < 0 || this->action >= 0x24 || sActionFuncs[this->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnZl2_Init(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;
    ActorShape* shape = &thisx->shape;
    s32 pad;

    ActorShape_Init(shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    shape->shadowAlpha = 0;
    SkelAnime_InitFlex(play, &this->skelAnime, &gZelda2Skel, NULL, NULL, NULL, 0);

    switch (thisx->params) {
        case 1:
            Audio_SetSoundBanksMute(0x6F);
            break;
        case 4:
            gSaveContext.timer2State = 0;
            break;
    }
}

s32 EnZl2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    EnZl2* this = (EnZl2*)thisx;

    if (this->overrideLimbDrawConfig < 0 || this->overrideLimbDrawConfig > 0 ||
        sOverrideLimbDrawFuncs[this->overrideLimbDrawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return sOverrideLimbDrawFuncs[this->overrideLimbDrawConfig](play, limbIndex, dList, pos, rot, thisx, gfx);
}

void func_80B523BC(EnZl2* this, PlayState* play) {
}

void func_80B523C8(EnZl2* this, PlayState* play) {
    s32 pad[3];
    s16 eyeTexIndex = this->eyeTexIndex;
    s16 eyeTexIndex2 = this->eyeTexIndex2;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    void* eyeTex2 = sEyeTextures[eyeTexIndex2];
    SkelAnime* skelAnime = &this->skelAnime;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex2));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0B, &D_80116280[2]);

    POLY_OPA_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl2_OverrideLimbDraw, EnZl2_PostLimbDraw, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B525D4(EnZl2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0B, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl2_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZl2_Draw(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (sDrawFuncs[this->drawConfig] == NULL)) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
