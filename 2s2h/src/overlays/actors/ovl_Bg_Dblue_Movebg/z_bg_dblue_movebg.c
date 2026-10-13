/*
 * File: z_bg_dblue_movebg.c
 * Overlay: ovl_Bg_Dblue_Movebg
 * Description: Great Bay Temple - Waterwheels, push switches, gear shafts, and whirlpools
 */

#include "prevent_bss_reordering.h"
#include "z_bg_dblue_movebg.h"
#include "objects/object_dblue_object/object_dblue_object.h"
#include "overlays/actors/ovl_Obj_Hunsui/z_obj_hunsui.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgDblueMovebg_Init(Actor* thisx, PlayState* play);
void BgDblueMovebg_Destroy(Actor* thisx, PlayState* play);
void BgDblueMovebg_Update(Actor* thisx, PlayState* play);
void BgDblueMovebg_Draw(Actor* thisx, PlayState* play2);
void BgDblueMovebg_Reset(void);

void func_80A2A1E0(BgDblueMovebg* this, PlayState* play);
void func_80A2A32C(BgDblueMovebg* this, PlayState* play);
void func_80A2A444(BgDblueMovebg* this, PlayState* play);
void func_80A2A670(BgDblueMovebg* this, PlayState* play);
void func_80A2A688(BgDblueMovebg* this, PlayState* play);
void func_80A2A714(BgDblueMovebg* this, PlayState* play);
void func_80A2A7F8(BgDblueMovebg* this, PlayState* play);
void func_80A2AAB8(BgDblueMovebg* this, PlayState* play);
void func_80A2AED0(BgDblueMovebg* this, PlayState* play);
void func_80A2B1A0(BgDblueMovebg* this, PlayState* play);
void func_80A2B274(Actor* thisx, PlayState* play);
void func_80A2B308(Actor* thisx, PlayState* play);

BgDblueMovebg* D_80A2BBF0;

typedef struct {
    /* 0x0 */ u8 unk_00;
    /* 0x1 */ u8 unk_01;
} BgDblueMovebgUnkStruct; // size = 0x2

BgDblueMovebgUnkStruct D_80A2BBF4;

u8 D_80A2B870[][2] = {
    { 0x01, 0x01 }, { 0x01, 0x00 }, { 0x02, 0x03 }, { 0x02, 0x01 }, { 0x02, 0x02 }, { 0x02, 0x00 }, { 0x03, 0x07 },
    { 0x03, 0x03 }, { 0x03, 0x05 }, { 0x03, 0x01 }, { 0x03, 0x06 }, { 0x03, 0x02 }, { 0x03, 0x04 }, { 0x03, 0x00 },
};

ActorProfile Bg_Dblue_Movebg_Profile = {
    /**/ ACTOR_BG_DBLUE_MOVEBG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DBLUE_OBJECT,
    /**/ sizeof(BgDblueMovebg),
    /**/ BgDblueMovebg_Init,
    /**/ BgDblueMovebg_Destroy,
    /**/ BgDblueMovebg_Update,
    /**/ BgDblueMovebg_Draw,
    /**/ BgDblueMovebg_Reset,
};

static Gfx* sOpaDLists[] = {
    NULL,
    gGreatBayTempleObjectTwoWaySwitchDL,
    NULL,
    NULL,
    NULL,
    NULL,
    gGreatBayTempleObjectGearShaftWithPlatformsDL,
    gGreatBayTempleObjectOneWaySwitchDL,
    NULL,
    NULL,
    NULL,
    NULL,
};

static Gfx* sXluDLists[] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, gGreatBayTempleObjectWhirlpoolDL, NULL,
};

static CollisionHeader* sColHeaders[] = {
    NULL,
    &gGreatBayTempleObjectTwoWaySwitchCol,
    NULL,
    NULL,
    NULL,
    &gGreatBayTempleObjectUnusedCol,
    &gGreatBayTempleObjectGearShaftWithPlatformsCol,
    &gGreatBayTempleObjectOneWaySwitchCol,
    &gGreatBayTempleObjectWaterwheelCol,
    &gGreatBayTempleObjectWaterwheelCol,
    NULL,
    NULL,
};

static AnimatedMaterial* sTexAnims[] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, gGreatBayTempleObjectWhirlpoolTexAnim, NULL,
};

s16 D_80A2B96C[] = { 0, 0x16C, -0x16C, 0 };

static s16 sCsIdList[] = { CS_ID_NONE, CS_ID_NONE };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

Vec3f D_80A2B988 = { 1785.0f, 0.0f, 220.0f };

s32 func_80A29A80(PlayState* play, s32 switchFlagBase, s32 arg2) {
    s32 sp2C = 1;
    s32 switchFlagOffset;
    s32 val2;

    if (arg2 < 14) {
        switchFlagOffset = D_80A2B870[arg2][0];
        val2 = D_80A2B870[arg2][1];

        while (switchFlagOffset--) {
            if ((1 << switchFlagOffset) & val2) {
                if (!Flags_GetSwitch(play, switchFlagBase + switchFlagOffset)) {
                    sp2C = 0;
                    break;
                }
            } else if (Flags_GetSwitch(play, switchFlagBase + switchFlagOffset)) {
                sp2C = 0;
                break;
            }
        }
    } else {
        sp2C = 0;
        switch (arg2) {
            case 14:
                if (!Flags_GetSwitch(play, switchFlagBase)) {
                    sp2C = 1;
                }

                if (Flags_GetSwitch(play, switchFlagBase + 1) && Flags_GetSwitch(play, switchFlagBase + 2) &&
                    Flags_GetSwitch(play, switchFlagBase + 3)) {
                    sp2C += 2;
                }
                break;

            case 15:
                if (!Flags_GetSwitch(play, switchFlagBase) ||
                    (Flags_GetSwitch(play, switchFlagBase + 1) && Flags_GetSwitch(play, switchFlagBase + 2) &&
                     Flags_GetSwitch(play, switchFlagBase + 3))) {
                    sp2C = 1;
                }
                break;
        }
    }
    return sp2C;
}

void BgDblueMovebg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->unk_160 = BGDBLUEMOVEBG_GET_F(thisx);
    this->unk_1BC = BGDBLUEMOVEBG_GET_F000(thisx);
    this->switchFlag = BGDBLUEMOVEBG_GET_SWITCH_FLAG(thisx);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    if ((this->unk_160 == 9) || (this->unk_160 == 8)) {
        if (D_80A2BBF4.unk_00 != 0) {
            Actor_Kill(&this->dyna.actor);
            return;
        }
        this->unk_170 = this->dyna.actor.room;
        this->dyna.actor.room = -1;
        D_80A2BBF4.unk_00 = 1;
        this->unk_171 = this->dyna.actor.world.rot.z;
        this->dyna.actor.world.rot.z = 0;
        this->dyna.actor.shape.rot.z = 0;
    } else if (this->unk_160 == 6) {
        if (D_80A2BBF4.unk_01 != 0) {
            Actor_Kill(&this->dyna.actor);
            return;
        }
        this->unk_171 = this->dyna.actor.world.rot.z;
        this->unk_170 = this->dyna.actor.room;
        this->dyna.actor.world.rot.z = 0;
        this->dyna.actor.shape.rot.z = 0;
        this->dyna.actor.room = -1;
        D_80A2BBF4.unk_01 = 1;
    }

    if (sColHeaders[this->unk_160] != NULL) {
        DynaPolyActor_LoadMesh(play, &this->dyna, sColHeaders[this->unk_160]);
    }

    this->opaDList = sOpaDLists[this->unk_160];
    this->xluDList = sXluDLists[this->unk_160];
    this->texAnim = sTexAnims[this->unk_160];

    SubS_FillCutscenesList(&this->dyna.actor, this->csIdList, ARRAY_COUNT(this->csIdList));

    switch (this->unk_160) {
        case 1:
            this->unk_1C4 = this->dyna.actor.world.rot.x;
            this->unk_1C8 = this->dyna.actor.world.rot.z;
            this->dyna.actor.world.rot.x = 0;
            this->dyna.actor.shape.rot.x = 0;
            this->dyna.actor.world.rot.z = 0;
            this->dyna.actor.shape.rot.z = 0;
            if (func_80A29A80(play, this->unk_1C8, this->unk_1C4)) {
                this->unk_172 |= 1;
            } else {
                this->unk_172 &= ~1;
            }
            this->unk_17E = 0;
            this->unk_184 = 0.0f;
            if (Flags_GetSwitch(play, this->switchFlag)) {
                this->unk_18C = 0x384;
            } else {
                this->unk_18C = 0;
            }
            this->dyna.actor.shape.rot.y += DEG_TO_BINANG(this->unk_18C / 10.0f);
            this->actionFunc = func_80A2A714;
            break;

        case 6:
            this->unk_178 = func_80A29A80(play, this->switchFlag, this->unk_1BC);
            this->unk_1CC = D_80A2B96C[this->unk_178];
            this->unk_1CE = this->unk_1CC;
            this->actionFunc = func_80A2A1E0;
            break;

        case 7:
            this->unk_1C4 = this->dyna.actor.world.rot.x;
            this->unk_1C8 = this->dyna.actor.world.rot.z;
            this->dyna.actor.world.rot.x = 0;
            this->dyna.actor.shape.rot.x = 0;
            this->dyna.actor.world.rot.z = 0;
            this->dyna.actor.shape.rot.z = 0;
            if (Flags_GetSwitch(play, this->switchFlag)) {
                Actor_Kill(&this->dyna.actor);
                break;
            }
            this->unk_18C = 0;
            this->dyna.actor.draw = func_80A2B308;
            this->unk_17E = 0;
            this->unk_184 = 0.0f;
            this->dyna.actor.shape.rot.y += DEG_TO_BINANG(this->unk_18C / 10.0f);
            this->actionFunc = func_80A2A32C;
            break;

        case 8:
        case 9:
            this->unk_2F8[1] = NULL;
            this->unk_2F8[0] = NULL;
            for (i = 0; i < ARRAY_COUNT(sCsIdList); i++) {
                sCsIdList[i] = this->csIdList[i];
            }
            this->unk_178 = func_80A29A80(play, this->switchFlag, this->unk_1BC);
            this->unk_1CC = D_80A2B96C[this->unk_178];
            this->unk_1CE = this->unk_1CC;
            Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
            Matrix_MultVecX(2240.0f, &this->unk_190);
            Matrix_MultVecX(-10.0f, &this->unk_19C);
            Math_Vec3f_Sum(&this->unk_190, &this->dyna.actor.world.pos, &this->unk_190);
            Math_Vec3f_Sum(&this->unk_19C, &this->dyna.actor.world.pos, &this->unk_19C);
            D_80A2BBF0 = this;
            this->dyna.actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
            this->actionFunc = func_80A2AED0;
            break;

        case 10:
            this->actionFunc = func_80A2B1A0;
            break;

        case 11:
            this->unk_1CC = D_80A2B96C[func_80A29A80(play, this->switchFlag, this->unk_1BC)];
            D_80A2BBF0 = this;
            this->dyna.actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
            this->dyna.actor.update = Actor_Noop;
            this->dyna.actor.draw = func_80A2B274;
            break;
    }
}

void BgDblueMovebg_Destroy(Actor* thisx, PlayState* play) {
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    if ((this->unk_160 == 9) || (this->unk_160 == 8)) {
        AudioSfx_StopByPos(&this->unk_1A8);
    }
}

void func_80A2A128(BgDblueMovebg* this, PlayState* play) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, ACTORCAT_BG, ACTOR_OBJ_HUNSUI);
        if (actorIter != NULL) {
            if ((OBJHUNSUI_GET_F000(actorIter) == 5) && (actorIter->update != NULL)) {
                this->unk_2F8[1] = actorIter;
            } else if ((OBJHUNSUI_GET_F000(actorIter) == 6) && (actorIter->update != NULL)) {
                this->unk_2F8[0] = actorIter;
            }
            actorIter = actorIter->next;
        }

        if (actorIter == NULL) {
            break;
        }
    }
}

void func_80A2A1E0(BgDblueMovebg* this, PlayState* play) {
    s32 temp_v0 = func_80A29A80(play, this->switchFlag, this->unk_1BC);

    if (temp_v0 != this->unk_178) {
        switch (temp_v0) {
            case 1:
            case 2:
                this->unk_17E = 40;
                break;

            case 0:
            case 3:
                this->unk_17E = 15;
                break;
        }
    }
    this->unk_178 = temp_v0;

    if (this->unk_17E <= 0) {
        this->unk_1CE = D_80A2B96C[temp_v0];
    } else {
        this->unk_17E--;
    }

    Math_StepToS(&this->unk_1CC, this->unk_1CE, 12);
    this->dyna.actor.shape.rot.y += this->unk_1CC;

    if (play->roomCtx.curRoom.num == 0) {
        this->opaDList = gGreatBayTempleObjectGearShaftWithPlatformsDL;
    } else if (play->roomCtx.curRoom.num == 8) {
        this->opaDList = NULL;
    }

    if (play->roomCtx.curRoom.num != this->unk_170) {
        if (play->roomCtx.curRoom.num != this->unk_171) {
            if ((play->roomCtx.prevRoom.num != this->unk_170) && (play->roomCtx.prevRoom.num != this->unk_171)) {
                D_80A2BBF4.unk_01 = 0;
                Actor_Kill(&this->dyna.actor);
            }
        }
    }
}

void func_80A2A32C(BgDblueMovebg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 phi_v0;
    f32 phi_f0;
    s32 phi_v1;

    if (this->dyna.pushForce != 0.0f) {
        this->unk_18E = BINANG_SUB(this->dyna.actor.shape.rot.y, this->dyna.actor.yawTowardsPlayer);
        this->unk_184 = this->dyna.actor.xzDistToPlayer;

        if (this->dyna.pushForce >= 0.0f) {
            phi_f0 = 1.0f;
        } else {
            phi_f0 = -1.0f;
        }

        if (BINANG_SUB(this->dyna.actor.yawTowardsPlayer, player->actor.shape.rot.y) > 0) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        this->unk_17E = phi_v0 * phi_f0;

        phi_v1 = false;
        if (this->unk_17E > 0) {
            phi_v1 = true;
        }

        if (phi_v1) {
            this->unk_180 = CutsceneManager_GetCutsceneCustomValue(this->csIdList[0]);
            this->csId = this->csIdList[0];
            this->unk_172 |= 8;
            this->actionFunc = func_80A2A444;
        } else {
            player->stateFlags2 &= ~PLAYER_STATE2_10;
            this->dyna.pushForce = 0.0f;
        }
    }
}

void func_80A2A444(BgDblueMovebg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 sp20;
    Player* temp_a3;
    s16 temp_v0;

    this->unk_188++;
    if (GameInteractor_Should(VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED, true, this)) {
        this->unk_188 = CLAMP_MAX(this->unk_188, 5);
    }

    sp20 = Math_StepToS(&this->unk_18A, 900, this->unk_188);
    temp_v0 = this->unk_18A * this->unk_17E;
    this->dyna.actor.shape.rot.y =
        (s32)DEG_TO_BINANG_ALT3((this->unk_18C + temp_v0) * 0.1f) + this->dyna.actor.home.rot.y;

    if ((player->stateFlags2 & PLAYER_STATE2_10) && (this->unk_184 > 0.0f)) {
        player->actor.world.pos.x =
            (Math_SinS(this->dyna.actor.shape.rot.y - this->unk_18E) * this->unk_184) + this->dyna.actor.home.pos.x;
        player->actor.world.pos.z =
            (Math_CosS(this->dyna.actor.shape.rot.y - this->unk_18E) * this->unk_184) + this->dyna.actor.home.pos.z;
    } else {
        this->unk_184 = 0.0f;
    }

    if (sp20) {
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
        Flags_SetSwitch(play, this->switchFlag);
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);

        if (func_80A29A80(play, this->unk_1C8, this->unk_1C4)) {
            this->unk_172 |= 1;
        } else {
            this->unk_172 &= ~1;
        }

        if (!(this->unk_174 & 1) && (this->unk_172 & 1)) {
            Lib_PlaySfx_2(NA_SE_EV_PIPE_STREAM_START);
        }
        func_80A2A670(this, play);
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_COCK_SWITCH_ROLL - SFX_FLAG);
    }
}

void func_80A2A670(BgDblueMovebg* this, PlayState* play) {
    this->actionFunc = func_80A2A688;
}

void func_80A2A688(BgDblueMovebg* this, PlayState* play) {
    this->unk_180--;
    if (this->unk_180 <= 0) {
        CutsceneManager_Stop(this->csIdList[0]);
    }

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 60.0f, 2.0f) &&
        (this->unk_180 <= 0)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80A2A714(BgDblueMovebg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 phi_v0;
    f32 phi_f0;

    if (this->dyna.pushForce != 0.0f) {
        this->unk_18E = BINANG_SUB(this->dyna.actor.shape.rot.y, this->dyna.actor.yawTowardsPlayer);
        this->unk_184 = this->dyna.actor.xzDistToPlayer;
        if (this->dyna.pushForce >= 0.0f) {
            phi_f0 = 1.0f;
        } else {
            phi_f0 = -1.0f;
        }

        if (BINANG_SUB(this->dyna.actor.yawTowardsPlayer, player->actor.shape.rot.y) > 0) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        this->unk_17E = phi_v0 * phi_f0;

        this->unk_180 = CutsceneManager_GetCutsceneCustomValue(this->csIdList[0]);
        this->csId = this->csIdList[0];
        this->unk_172 |= 8;
        this->actionFunc = func_80A2A7F8;
    }
}

void func_80A2A7F8(BgDblueMovebg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 sp28;
    s16 sp26;
    s32 temp;

    this->unk_188++;
    if (GameInteractor_Should(VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED, true, this)) {
        this->unk_188 = CLAMP_MAX(this->unk_188, 5);
    }

    sp28 = Math_StepToS(&this->unk_18A, 900, this->unk_188);
    sp26 = this->unk_18A * this->unk_17E;
    this->dyna.actor.shape.rot.y = (s32)DEG_TO_BINANG_ALT3((this->unk_18C + sp26) * 0.1f) + this->dyna.actor.home.rot.y;

    if ((player->stateFlags2 & PLAYER_STATE2_10) && (this->unk_184 > 0.0f)) {
        player->actor.world.pos.x =
            (Math_SinS(this->dyna.actor.shape.rot.y - this->unk_18E) * this->unk_184) + this->dyna.actor.home.pos.x;
        player->actor.world.pos.z =
            (Math_CosS(this->dyna.actor.shape.rot.y - this->unk_18E) * this->unk_184) + this->dyna.actor.home.pos.z;
    } else {
        this->unk_184 = 0.0f;
    }

    if (sp28) {
        temp = (this->unk_18C + sp26 + 3600) % 3600;

        if ((temp == 900) || (temp == 2700)) {
            Flags_SetSwitch(play, this->switchFlag);
        } else {
            Flags_UnsetSwitch(play, this->switchFlag);
        }

        player->stateFlags1 |= PLAYER_STATE1_20;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;

        this->unk_18C = (this->unk_18C + sp26 + 3600) % 3600;
        this->unk_188 = 0;
        this->unk_18A = 0;
        this->unk_17E = 0;

        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);

        if (func_80A29A80(play, this->unk_1C8, this->unk_1C4)) {
            this->unk_172 |= 1;
        } else {
            this->unk_172 &= ~1;
        }

        if (!(this->unk_174 & 1) && (this->unk_172 & 1)) {
            Lib_PlaySfx_2(NA_SE_EV_PIPE_STREAM_START);
        }

        this->unk_174 = this->unk_172;
        this->unk_1D0 = 17;
        this->actionFunc = func_80A2AAB8;
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_COCK_SWITCH_ROLL - SFX_FLAG);
    }
}

void func_80A2AAB8(BgDblueMovebg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 sp18;

    if (this->unk_180-- <= 0) {
        CutsceneManager_Stop(this->csIdList[0]);
    }

    sp18 = false;
    if ((this->unk_1D0 > 0) && ((sCsIdList[0] >= 0) || (sCsIdList[1] >= 0))) {
        if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
            sp18 = true;
        }
    }

    if (!sp18 && (this->unk_180 <= 0)) {
        this->unk_1D0--;

        if (this->unk_1D0 == 1) {
            this->dyna.pushForce = 0.0f;
            player->stateFlags1 |= PLAYER_STATE1_20;
            player->stateFlags2 &= ~PLAYER_STATE2_10;
        }

        if (this->unk_1D0 <= 0) {
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            this->actionFunc = func_80A2A714;
        }
    }
}

void func_80A2ABD0(BgDblueMovebg* this, PlayState* play) {
    Vec3f spAC;
    f32 temp_f0;
    f32 temp_f20;
    s32 phi_v0;
    s32 i;
    s32 j;

    for (i = 0; i < ARRAY_COUNT(this->unk_2F8); i++) {
        if (this->unk_2F8[i] == NULL) {
            func_80A2A128(this, play);
            continue;
        }

        for (j = 0; j < ARRAY_COUNT(this->unk_1D8[0]); j++) {
            Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(this->dyna.actor.shape.rot.x + (j * 0x2000), MTXMODE_APPLY);
            Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
            Matrix_MultVec3f(&D_80A2B988, &spAC);
            Math_Vec3f_Sum(&this->dyna.actor.world.pos, &spAC, &spAC);
            Math_Vec3f_Copy(&this->unk_238[i][j], &spAC);
            temp_f20 = spAC.y - this->unk_2F8[i]->world.pos.y;
            temp_f0 = Math_Vec3f_DistXZ(&this->unk_2F8[i]->world.pos, &spAC);

            if ((this->unk_300[i][j] != 0) && (temp_f20 <= 12.0f) && (temp_f20 >= 0.0f) && (temp_f0 <= 60.0f)) {
                phi_v0 = 1;
            } else {
                phi_v0 = 1;
                phi_v0 = (s64)phi_v0;
                if (((temp_f20 >= 0.0f) && ((temp_f20 > 0.0f) || (temp_f0 > 60.f))) ||
                    ((temp_f20 < 0.0f) && ((temp_f20 < -150.0f) || (temp_f0 > 40.0f)))) {
                    phi_v0 = 0;
                }
            }

            if (phi_v0) {
                this->unk_300[i][j] = i + 1;
                Math_StepToF(&this->unk_1F8[i][j], 1.0f, 0.2f);
                Math_StepToS(&this->unk_1D8[i][j], 255, 20);
            } else {
                this->unk_300[i][j] = 0;
                Math_StepToF(&this->unk_1F8[i][j], 0.0f, 0.01f);
                Math_StepToS(&this->unk_1D8[i][j], 0, 20);
            }
        }
    }
}

void func_80A2AED0(BgDblueMovebg* this, PlayState* play) {
    s32 pad;
    s32 temp_v0_3;

    if (play->roomCtx.curRoom.num != this->unk_170) {
        if (play->roomCtx.curRoom.num != this->unk_171) {
            if ((play->roomCtx.prevRoom.num != this->unk_170) && (play->roomCtx.prevRoom.num != this->unk_171)) {
                D_80A2BBF4.unk_00 = 0;
                Actor_Kill(&this->dyna.actor);
                return;
            }
        }
    }

    temp_v0_3 = func_80A29A80(play, this->switchFlag, this->unk_1BC);
    if (temp_v0_3 != this->unk_178) {
        switch (temp_v0_3) {
            case 1:
            case 2:
                this->csId = this->csIdList[0];
                this->unk_17E = 40;
                break;

            case 0:
            case 3:
                this->csId = this->csIdList[1];
                this->unk_17E = 15;
                break;
        }
        this->unk_172 |= 8;
    }
    this->unk_178 = temp_v0_3;

    if (!(this->unk_172 & 8)) {
        if (this->unk_17E <= 0) {
            this->unk_1CE = D_80A2B96C[temp_v0_3];
        } else {
            this->unk_17E--;
        }
    }

    Math_StepToS(&this->unk_1CC, this->unk_1CE, 12);
    this->dyna.actor.shape.rot.x += this->unk_1CC;

    if (this->unk_160 == 8) {
        if (play->roomCtx.curRoom.num == 8) {
            func_80A2ABD0(this, play);
            this->unk_172 |= 0x20;
        } else {
            this->unk_172 &= ~0x20;
        }
    }

    if (play->roomCtx.curRoom.num == 0) {
        this->opaDList = gGreatBayTempleObjectWaterwheelDL;
    } else if (play->roomCtx.curRoom.num == 8) {
        this->opaDList = gGreatBayTempleObjectWaterwheelWithFakeGearDL;
    }

    if (this == D_80A2BBF0) {
        Vec3f sp54;
        f32 sp50;

        if (Math3D_PointDistSqToLine2DImpl(play->view.eye.x, play->view.eye.z, this->unk_190.x, this->unk_190.z,
                                           this->unk_19C.x, this->unk_19C.z, &sp54.x, &sp54.z, &sp50)) {
            sp54.y = this->dyna.actor.world.pos.y;
        } else {
            if (Math_Vec3f_DistXYZ(&play->view.eye, &this->unk_190) <=
                Math_Vec3f_DistXYZ(&play->view.eye, &this->unk_19C)) {
                Math_Vec3f_Copy(&sp54, &this->unk_190);
            } else {
                Math_Vec3f_Copy(&sp54, &this->unk_19C);
            }
        }

        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &sp54, &this->unk_1A8);
        this->unk_1D4 = ABS(this->unk_1CC) / 364.0f;
    }
}

void func_80A2B1A0(BgDblueMovebg* this, PlayState* play) {
    switch (func_80A29A80(play, this->switchFlag, this->unk_1BC)) {
        case 1:
            this->dyna.actor.shape.rot.z = this->dyna.actor.home.rot.z;
            break;

        case 2:
            this->dyna.actor.shape.rot.z = BINANG_ROT180(this->dyna.actor.home.rot.z);
            break;
    }
}

void BgDblueMovebg_Update(Actor* thisx, PlayState* play) {
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;

    this->actionFunc(this, play);

    if (this->unk_172 & 8) {
        if (SubS_StartCutscene(&this->dyna.actor, this->csId, CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
            this->unk_172 &= ~8;
        }
    }
}

void func_80A2B274(Actor* thisx, PlayState* play) {
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;
    s16 temp_v1;

    if (this != D_80A2BBF0) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    temp_v1 = D_80A2B96C[func_80A29A80(play, this->switchFlag, this->unk_1BC)];
    if (temp_v1 != 0) {
        if (temp_v1 > 0) {
            Audio_PlaySfx_WaterWheel(&this->dyna.actor.projectedPos, NA_SE_EV_DUMMY_WATER_WHEEL_RR - SFX_FLAG);
        } else {
            Audio_PlaySfx_WaterWheel(&this->dyna.actor.projectedPos, NA_SE_EV_DUMMY_WATER_WHEEL_LR - SFX_FLAG);
        }
    }
}

void func_80A2B308(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->opaDList);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgDblueMovebg_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgDblueMovebg* this = (BgDblueMovebg*)thisx;
    s32 i;
    s32 j;
    Gfx* gfx;
    Gfx* gfx2;

    Matrix_Push();

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->unk_160 == 9) || (this->unk_160 == 8) || (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        if (this->texAnim != NULL) {
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(this->texAnim));
        }

        if ((this->opaDList != NULL) || (this->unk_160 == 6)) {
            gfx2 = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_25);

            MATRIX_FINALIZE_AND_LOAD(&gfx2[0], play->state.gfxCtx);

            if (this->unk_160 == 6) {
                gSPDisplayList(&gfx2[1], gGreatBayTempleObjectGearShaftDL);
                if (this->opaDList != NULL) {
                    gSPDisplayList(&gfx2[2], this->opaDList);
                    POLY_OPA_DISP = &gfx2[3];
                } else {
                    POLY_OPA_DISP = &gfx2[2];
                }
            } else {
                gSPDisplayList(&gfx2[1], this->opaDList);
                POLY_OPA_DISP = &gfx2[2];
            }
        }

        if (this->xluDList != NULL) {
            gfx = Gfx_SetupDL71(POLY_XLU_DISP);

            MATRIX_FINALIZE_AND_LOAD(&gfx[0], play->state.gfxCtx);
            gSPDisplayList(&gfx[1], this->xluDList);

            POLY_XLU_DISP = &gfx[2];
        }
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);

    if ((this->unk_160 == 8) && (this->unk_172 & 0x20)) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gGreatBayTempleObjectWaterwheelSplashTexAnim));

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
        gfx = Gfx_SetupDL71(POLY_XLU_DISP);

        for (i = 0; i < ARRAY_COUNT(this->unk_1D8[0]); i++) {
            for (j = 0; j < ARRAY_COUNT(this->unk_1D8); j++) {
                if (this->unk_1D8[j][i] > 0) {
                    if (this->unk_1F8[j][i] > 0.1f) {
                        gSPSegment(
                            gfx++, 0x09,
                            Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0,
                                               (s32)(((play->gameplayFrames % 128) * -9.0f) / this->unk_1F8[j][i]),
                                               0x20, 0x20, 1, 0, 0, 0x20, 0x20, 0, -9 / this->unk_1F8[j][i], 0, 0));
                    }
                    Matrix_Push();
                    Matrix_RotateXS(i * 0x2000, MTXMODE_APPLY);
                    Matrix_Translate(1785.0f, 0.0f, 270.0f, MTXMODE_APPLY);
                    if (j != 0) {
                        Matrix_RotateZS(-0x8000, MTXMODE_APPLY);
                    }
                    Matrix_Scale(this->unk_1F8[j][i] * this->dyna.actor.scale.x,
                                 this->unk_1F8[j][i] * this->dyna.actor.scale.y,
                                 this->unk_1F8[j][i] * this->dyna.actor.scale.z, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
                    gDPSetEnvColor(gfx++, 255, 255, 255, this->unk_1D8[j][i]);
                    gSPDisplayList(gfx++, gGreatBayTempleObjectWaterwheelSplashDL);

                    Matrix_Pop();
                }
            }
        }
        POLY_XLU_DISP = gfx;

        CLOSE_DISPS(play->state.gfxCtx);
    }

    if ((this->unk_160 == 9) || (this->unk_160 == 8)) {
        if (this->unk_1CC >= 0) {
            Audio_PlaySfx_AtPosWithFreqAndChannelIO(&this->unk_1A8, NA_SE_EV_BIG_WATER_WHEEL_RR - SFX_FLAG,
                                                    this->unk_1D4, 32);
        } else {
            Audio_PlaySfx_AtPosWithFreqAndChannelIO(&this->unk_1A8, NA_SE_EV_BIG_WATER_WHEEL_LR - SFX_FLAG,
                                                    this->unk_1D4, 32);
        }
    }
}

void BgDblueMovebg_Reset(void) {
    D_80A2BBF0 = NULL;
    D_80A2BBF4.unk_00 = 0;
    D_80A2BBF4.unk_01 = 0;
}
