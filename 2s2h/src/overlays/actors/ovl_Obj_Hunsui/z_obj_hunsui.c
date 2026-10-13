/*
 * File: z_obj_hunsui.c
 * Overlay: ovl_Obj_Hunsui
 * Description: Switch-Activated Geyser
 */

#include "z_obj_hunsui.h"
#include "objects/object_hunsui/object_hunsui.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjHunsui_Init(Actor* thisx, PlayState* play);
void ObjHunsui_Destroy(Actor* thisx, PlayState* play);
void ObjHunsui_Update(Actor* thisx, PlayState* play);
void ObjHunsui_Draw(Actor* thisx, PlayState* play);
void ObjHunsui_Reset(void);

void func_80B9CE64(ObjHunsui* this, PlayState* play);
void func_80B9D0FC(ObjHunsui* this, PlayState* play);
void func_80B9D120(ObjHunsui* this, PlayState* play);
void func_80B9D2BC(ObjHunsui* this, PlayState* play);
void func_80B9D4D0(ObjHunsui* this, PlayState* play);
void func_80B9D508(ObjHunsui* this, PlayState* play);
void func_80B9D714(ObjHunsui* this, PlayState* play);
void func_80B9DA60(Actor* thisx, PlayState* play);

AnimatedMaterial* D_80B9DED0;
AnimatedMaterial* D_80B9DED4;

typedef struct {
    /* 0x0 */ u8 unk_00;
    /* 0x1 */ u8 unk_01;
    /* 0x2 */ u8 unk_02;
} ObjHansuiBssStruct; // size = 0x3

ObjHansuiBssStruct D_80B9DED8;

typedef struct {
    /* 0x0 */ u8 unk_00;
    /* 0x1 */ u8 unk_01;
} ObjHansuiStruct; // size = 0x2

ObjHansuiStruct D_80B9DC70[] = {
    { 1, 1 }, { 1, 0 }, { 2, 3 }, { 2, 1 }, { 2, 2 }, { 2, 0 }, { 3, 7 },
    { 3, 3 }, { 3, 5 }, { 3, 1 }, { 3, 6 }, { 3, 2 }, { 3, 4 }, { 3, 0 },
};

ActorProfile Obj_Hunsui_Profile = {
    /**/ ACTOR_OBJ_HUNSUI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HUNSUI,
    /**/ sizeof(ObjHunsui),
    /**/ ObjHunsui_Init,
    /**/ ObjHunsui_Destroy,
    /**/ ObjHunsui_Update,
    /**/ ObjHunsui_Draw,
    /**/ ObjHunsui_Reset,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

s32 func_80B9C450(PlayState* play, s32 switchFlagBase, s32 arg2) {
    s32 sp2C = 1;

    if (arg2 < ARRAY_COUNT(D_80B9DC70)) {
        s32 switchFlagOffset = D_80B9DC70[arg2].unk_00;
        s32 val3 = D_80B9DC70[arg2].unk_01;

        while (switchFlagOffset--) {
            if ((1 << switchFlagOffset) & val3) {
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

            default:
                break;
        }
    }
    return sp2C;
}

void func_80B9C5E8(ObjHunsui* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_f10;
    f32 temp_f16;
    Vec3f sp40;
    s16 sp3E;
    f32 sp38;
    f32 sp34;

    if ((this->dyna.actor.xzDistToPlayer < (45.0f * this->dyna.actor.scale.x * 10.0f)) &&
        (this->dyna.actor.playerHeightRel < -21.0f)) {
        if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
            this->unk_172 &= ~8;
            this->unk_19C = 0.0f;
            this->unk_1A0 = 0.0f;

            this->unk_18C++;
            if (this->unk_18C >= 3) {
                this->unk_18C = 0;
                Math_Vec3f_Copy(&sp40, &player->actor.world.pos);

                sp40.x += Rand_CenteredFloat(10.0f);
                sp40.z += Rand_CenteredFloat(10.0f);
                sp40.y += Rand_ZeroFloat(2.0f);

                EffectSsGSplash_Spawn(play, &sp40, NULL, NULL, 2.0f * Rand_ZeroOne(), 1);
            }
        } else {
            this->unk_172 |= 8;

            this->unk_18C++;
            if (this->unk_18C >= 3) {
                Math_Vec3f_Copy(&sp40, &player->actor.world.pos);
                this->unk_18C = 0;

                sp40.x += Rand_CenteredFloat(10.0f);
                sp40.z += Rand_CenteredFloat(10.0f);
                sp40.y += Rand_ZeroFloat(45.0f);

                EffectSsGSplash_Spawn(play, &sp40, NULL, NULL, 1, 1);
            }

            sp3E = BINANG_ROT180(player->actor.world.rot.y - this->dyna.actor.yawTowardsPlayer);
            player->actor.gravity = 0.0f;
            player->actor.velocity.y = 0.0f;

            if ((this->unk_160 != OBJHUNSUI_F000_5) && (this->unk_160 != OBJHUNSUI_F000_6)) {
                Math_SmoothStepToF(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.5f, 4.0f, 1.0f);
            }

            if ((sp3E < 0x4000) && (sp3E > -0x4000)) {
                this->unk_1A4 = BINANG_ROT180(player->actor.world.rot.y);
                sp34 = this->dyna.actor.xzDistToPlayer / (45.0f * this->dyna.actor.scale.x * 10.0f);
                if (1) {}
                sp38 = this->dyna.actor.xzDistToPlayer / (45.0f * this->dyna.actor.scale.x * 10.0f);

                if (sp38 > 1.0f) {
                    sp38 = sp34;
                }

                player->speedXZ *= sp38;

                if ((this->unk_160 == OBJHUNSUI_F000_5) || (this->unk_160 == OBJHUNSUI_F000_6)) {
                    Math_ApproachF(&this->unk_1A0, 4.5f, 2.0f, 1.0f);
                    Math_ApproachF(&this->unk_19C, this->unk_1A0, 2.0f, 0.3f * sp38);
                } else {
                    Math_ApproachF(&this->unk_1A0, 3.0f, 1.0f, 1.0f);
                    Math_ApproachF(&this->unk_19C, this->unk_1A0, 1.0f, 0.3f * sp38);
                }
            } else {
                this->unk_1A4 = player->actor.world.rot.y;
                player->speedXZ *= 0.5f;
                Math_ApproachF(&this->unk_1A0, 3.0f, 1.0f, 1.0f);
                Math_ApproachF(&this->unk_19C, this->unk_1A0, 1.0f, 0.1f);
            }
            player->pushedYaw = this->unk_1A4;
            player->pushedSpeed = this->unk_19C;
        }
    } else {
        if (this->unk_172 & 8) {
            player->speedXZ = this->unk_19C + player->speedXZ;
            player->yaw = this->unk_1A4;
        }
        this->unk_1A0 = 0.0f;
        this->unk_19C = 0.0f;
        this->unk_172 &= ~8;
    }
}

void ObjHunsui_Init(Actor* thisx, PlayState* play) {
    ObjHunsui* this = (ObjHunsui*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->unk_160 = OBJHUNSUI_GET_F000(thisx);
    this->unk_164 = OBJHUNSUI_GET_F80(thisx);
    this->switchFlag = OBJHUNSUI_GET_SWITCH_FLAG(thisx);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    if ((this->unk_160 != OBJHUNSUI_F000_5) && (this->unk_160 != OBJHUNSUI_F000_6)) {
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_hunsui_Colheader_000C74);
    }

    D_80B9DED0 = Lib_SegmentedToVirtual(object_hunsui_Matanimheader_000BF0);
    D_80B9DED4 = Lib_SegmentedToVirtual(object_hunsui_Matanimheader_001888);
    SubS_FillCutscenesList(&this->dyna.actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    this->unk_18C = 0;

    switch (this->unk_160) {
        case OBJHUNSUI_F000_1:
        case OBJHUNSUI_F000_2:
        case OBJHUNSUI_F000_3:
        case OBJHUNSUI_F000_4:
        case OBJHUNSUI_F000_5:
        case OBJHUNSUI_F000_6:
            this->dyna.actor.cullingVolumeScale = 900.0f;
            this->dyna.actor.cullingVolumeDownward = 90.0f;
            this->dyna.actor.cullingVolumeDistance = 4000.0f;
            break;
    }

    switch (this->unk_160) {
        case OBJHUNSUI_F000_5:
            if (D_80B9DED8.unk_01 == 0) {
                D_80B9DED8.unk_01 = 1;
                this->unk_16C = this->dyna.actor.room;
                this->unk_16D = this->dyna.actor.world.rot.z;
                this->dyna.actor.world.rot.z = 0;
                this->dyna.actor.shape.rot.z = 0;
                this->dyna.actor.room = -1;
            } else {
                Actor_Kill(&this->dyna.actor);
                return;
            }
            break;

        case OBJHUNSUI_F000_6:
            if (D_80B9DED8.unk_02 == 0) {
                D_80B9DED8.unk_02 = 1;
                this->unk_16C = this->dyna.actor.room;
                this->unk_16D = this->dyna.actor.world.rot.z;
                this->dyna.actor.world.rot.z = 0;
                this->dyna.actor.shape.rot.z = 0;
                this->dyna.actor.room = -1;
            } else {
                Actor_Kill(&this->dyna.actor);
                return;
            }
            break;
    }

    switch (this->unk_160) {
        case OBJHUNSUI_F000_0:
            if (D_80B9DED8.unk_00 == 0) {
                D_80B9DED8.unk_00 = 1;
                this->unk_16C = this->dyna.actor.room;
                this->unk_16D = this->unk_164;
                // clang-format off
                this->dyna.actor.room = -1; this->actionFunc = func_80B9D714;
                // clang-format on
            } else {
                Actor_Kill(&this->dyna.actor);
                return;
            }
            break;

        case OBJHUNSUI_F000_2:
            this->unk_172 |= 1;

        case OBJHUNSUI_F000_1:
            this->dyna.actor.draw = func_80B9DA60;
            if ((this->unk_172 & 1) && func_80B9C450(play, this->switchFlag, this->unk_164)) {
                this->unk_174 = 240.0f;
                this->unk_172 |= 4;
                this->unk_184 = 0xFF0;
            } else {
                this->unk_174 = -30.0f;
                this->unk_172 |= 2;
                this->unk_184 = 0;
            }
            this->unk_178 = this->unk_174;
            this->unk_180 = func_80B9C450(play, this->switchFlag, this->unk_164);
            this->actionFunc = func_80B9CE64;
            break;

        case OBJHUNSUI_F000_4:
            this->unk_172 |= 1;

        case OBJHUNSUI_F000_3:
        case OBJHUNSUI_F000_5:
        case OBJHUNSUI_F000_6:
            this->dyna.actor.draw = func_80B9DA60;
            if ((this->unk_160 == OBJHUNSUI_F000_5) || (this->unk_160 == OBJHUNSUI_F000_6)) {
                func_80B9D2BC(this, play);
                this->unk_178 = this->unk_174;
            } else {
                if ((this->unk_172 & 1) || func_80B9C450(play, this->switchFlag, this->unk_164)) {
                    func_80B9D4D0(this, play);
                } else {
                    func_80B9D0FC(this, play);
                }
                this->unk_178 = this->unk_174;
            }
            break;
    }
}

void ObjHunsui_Destroy(Actor* thisx, PlayState* play) {
    ObjHunsui* this = (ObjHunsui*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjHunsui_Update(Actor* thisx, PlayState* play) {
    ObjHunsui* this = (ObjHunsui*)thisx;

    this->actionFunc(this, play);

    if ((this->unk_172 & 0x40) &&
        SubS_StartCutscene(&this->dyna.actor, this->csId, CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        this->unk_172 &= ~0x40;
    }

    if (this->unk_1AC > 1024.0f) {
        this->unk_1AC -= 1024.0f;
    }

    if (this->unk_1AC <= 0.0f) {
        this->unk_1AC += 1024.0f;
    }
}

void func_80B9CE64(ObjHunsui* this, PlayState* play) {
    s32 sp2C;
    f32 sins;

    if (!(this->unk_172 & 2)) {
        func_80B9C5E8(this, play);
    }

    this->unk_18A += 0x71C;
    Math_SmoothStepToF(&this->unk_190, this->unk_194, 1.0f, 0.2f, 0.01f);

    sp2C = func_80B9C450(play, this->switchFlag, this->unk_164);

    if (!(this->unk_172 & 1)) {
        if (sp2C != this->unk_180) {
            this->csId = this->csIdList[0];
            this->unk_172 |= 0x40;
        }
    }

    if ((this->unk_172 & 1) || (sp2C != 0)) {
        this->unk_172 &= ~2;
        if (this->unk_186 == 0) {
            this->unk_172 |= 0x10;
            this->unk_174 = 240.0f;
            if (this->unk_178 == 240.0f) {
                if (this->unk_188++ > 40) {
                    this->unk_188 = 0;
                    this->unk_186 = 1;
                    this->unk_194 = 0.0f;
                } else {
                    this->unk_194 = 8.0f;
                }
            }
        } else {
            this->unk_174 = 30.0f;
            if (this->unk_178 == 30.0f) {
                if (this->unk_188++ > 40) {
                    this->unk_188 = 0;
                    this->unk_186 = 0;
                    this->unk_194 = 0.0f;
                } else {
                    this->unk_194 = 8.0f;
                }
            }
        }
    } else {
        this->unk_174 = -30.0f;
        if (this->unk_178 == -30.0f) {
            this->unk_172 &= ~0x10;
            this->unk_172 |= 2;
        }
    }

    sins = Math_SinS(this->unk_18A);
    this->unk_198 = sins * this->unk_190;
    if (!(this->unk_172 & 0x40)) {
        Math_SmoothStepToF(&this->unk_178, this->unk_174, 0.6f, 10.5f, 0.05f);
    }

    this->dyna.actor.world.pos.y = this->unk_198 + (this->dyna.actor.home.pos.y + this->unk_178);
    this->unk_180 = sp2C;
}

void func_80B9D094(ObjHunsui* this, PlayState* play) {
    f32 sins;

    this->unk_18A += 0x71C;
    Math_SmoothStepToF(&this->unk_190, this->unk_194, 1.0f, 0.2f, 0.01f);
    sins = Math_SinS(this->unk_18A);
    this->unk_198 = sins * this->unk_190;
}

void func_80B9D0FC(ObjHunsui* this, PlayState* play) {
    this->unk_174 = -30.0f;
    this->actionFunc = func_80B9D120;
}

void func_80B9D120(ObjHunsui* this, PlayState* play) {
    if (((this->unk_160 == OBJHUNSUI_F000_5) || (this->unk_160 == OBJHUNSUI_F000_6)) &&
        (this->unk_16C != play->roomCtx.curRoom.num) && (this->unk_16C != play->roomCtx.prevRoom.num) &&
        ((this->unk_16D != play->roomCtx.curRoom.num) && (this->unk_16D != play->roomCtx.prevRoom.num))) {
        switch (this->unk_160) {
            case OBJHUNSUI_F000_5:
                D_80B9DED8.unk_01 = 0;
                break;

            case OBJHUNSUI_F000_6:
                D_80B9DED8.unk_02 = 0;
                break;
        }
        Actor_Kill(&this->dyna.actor);
        return;
    }

    func_80B9D094(this, play);
    if (!(this->unk_172 & 0x40)) {
        Math_SmoothStepToF(&this->unk_178, this->unk_174, 0.6f, 10.5f, 0.05f);
    }

    this->dyna.actor.world.pos.y = this->unk_198 + (this->dyna.actor.home.pos.y + this->unk_178);
    if (this->unk_178 == this->unk_174) {
        this->unk_172 &= ~0x10;
        this->unk_172 |= 2;
    }

    if (func_80B9C450(play, this->switchFlag, this->unk_164)) {
        this->csId = this->csIdList[0];
        this->unk_172 |= 0x40;
        func_80B9D4D0(this, play);
    }
}

s32 func_80B9D288(PlayState* play, Actor* thisx, Actor* iter, void* verifyData) {
    s32 ret = false;

    if (BGDBLUEMOVEBG_GET_F(iter) == BGDBLUEMOVEBG_F_8) {
        ret = true;
    }
    return ret;
}

void func_80B9D2BC(ObjHunsui* this, PlayState* play) {
    if ((this->unk_172 & 1) || func_80B9C450(play, this->switchFlag, this->unk_164)) {
        func_80B9D4D0(this, play);
    } else {
        this->unk_172 |= 2;
        func_80B9D0FC(this, play);
    }
}

void func_80B9D334(ObjHunsui* this, PlayState* play) {
    Vec3f sp74;
    s32 i;
    s32 phi_s2 = 0;
    s32 phi_s3;

    switch (this->unk_160) {
        case OBJHUNSUI_F000_5:
            phi_s2 = 1;
            break;

        case OBJHUNSUI_F000_6:
            break;
    }

    if (this->unk_1B4 != NULL) {
        phi_s3 = false;
        for (i = 0; i < 8; i++) {
            if (this->unk_1B4->unk_300[phi_s2][i] == (phi_s2 + 1)) {
                phi_s3 = true;
                Math_Vec3f_Copy(&sp74, &this->unk_1B4->unk_238[phi_s2][i]);
                this->unk_174 = sp74.y - this->dyna.actor.home.pos.y;
                if (this->unk_174 > 240.0f) {
                    this->unk_174 = 240.0f;
                }
                this->unk_190 = 0.0f;
                this->unk_198 = 0.0f;
                this->unk_18A = 0;
            }
        }

        if (!phi_s3) {
            this->unk_174 = 240;
        }
    } else {
        Actor* dblueMovebg = SubS_FindActorCustom(play, &this->dyna.actor, NULL, ACTORCAT_BG, ACTOR_BG_DBLUE_MOVEBG,
                                                  NULL, func_80B9D288);

        if (dblueMovebg != NULL) {
            this->unk_1B4 = (BgDblueMovebg*)dblueMovebg;
            func_80B9D2BC(this, play);
        }
    }
}

void func_80B9D4D0(ObjHunsui* this, PlayState* play) {
    this->unk_172 &= ~2;
    this->unk_172 |= 0x10;
    this->unk_174 = 240.0f;
    this->actionFunc = func_80B9D508;
}

void func_80B9D508(ObjHunsui* this, PlayState* play) {
    if (((this->unk_160 == OBJHUNSUI_F000_5) || (this->unk_160 == OBJHUNSUI_F000_6)) &&
        (this->unk_16C != play->roomCtx.curRoom.num) && (this->unk_16C != play->roomCtx.prevRoom.num) &&
        (this->unk_16D != play->roomCtx.curRoom.num) && (this->unk_16D != play->roomCtx.prevRoom.num)) {
        switch (this->unk_160) {
            case OBJHUNSUI_F000_5:
                D_80B9DED8.unk_01 = 0;
                break;

            case OBJHUNSUI_F000_6:
                D_80B9DED8.unk_02 = 0;
                break;
        }
        Actor_Kill(&this->dyna.actor);
        return;
    }

    func_80B9C5E8(this, play);
    func_80B9D094(this, play);

    if (((this->unk_160 == OBJHUNSUI_F000_5) || (this->unk_160 == OBJHUNSUI_F000_6)) &&
        (play->roomCtx.curRoom.num == 8)) {
        func_80B9D334(this, play);
    }

    if (!(this->unk_172 & 0x40)) {
        Math_SmoothStepToF(&this->unk_178, this->unk_174, 0.6f, 10.5f, 0.05f);
    }

    this->dyna.actor.world.pos.y = this->unk_198 + (this->dyna.actor.home.pos.y + this->unk_178);

    if ((this->unk_160 == OBJHUNSUI_F000_6) || (this->unk_160 == OBJHUNSUI_F000_5)) {
        this->unk_1AC += -10.0f + (1.8f * (this->dyna.actor.world.pos.y - this->dyna.actor.prevPos.y));
        this->unk_1B0 += -8.0f + (0.9f * (this->dyna.actor.world.pos.y - this->dyna.actor.prevPos.y));
    }

    if (!(this->unk_172 & 1) && !func_80B9C450(play, this->switchFlag, this->unk_164)) {
        this->csId = this->csIdList[0];
        this->unk_172 |= 0x40;
        func_80B9D0FC(this, play);
    }
}

void func_80B9D714(ObjHunsui* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 csId;
    f32 sp28;

    if ((this->unk_16C != play->roomCtx.curRoom.num) && (this->unk_16C != play->roomCtx.prevRoom.num) &&
        (this->unk_16D != play->roomCtx.curRoom.num) && (this->unk_16D != play->roomCtx.prevRoom.num)) {
        Actor_Kill(&this->dyna.actor);
    } else {
        if (Flags_GetSwitch(play, this->switchFlag)) {
            this->unk_172 &= ~2;
            this->unk_172 |= 0x10;
            csId = this->dyna.actor.csId;

            if (this->unk_16E == 0) {
                if ((csId > CS_ID_NONE) && !CutsceneManager_IsNext(csId)) {
                    CutsceneManager_Queue(csId);
                } else if (csId > CS_ID_NONE) {
                    CutsceneManager_StartWithPlayerCs(csId, &this->dyna.actor);
                    this->unk_16E = -1;
                } else {
                    this->unk_16E = 40;
                }
            } else if (this->unk_16E < 0) {
                if (func_800F22C4(csId, &this->dyna.actor) != 0) {
                    this->unk_16E = 40;
                }
            } else {
                s32 pad;

                if (Math_SmoothStepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 800.0f, 0.1f, 8.0f,
                                       1.0f) < 0.5f) {
                    if (DECR(this->unk_16E) == 0) {
                        Flags_UnsetSwitch(play, this->switchFlag);
                    }
                }
                this->dyna.actor.velocity.y = this->dyna.actor.world.pos.y - this->dyna.actor.prevPos.y;
            }
        } else {
            Math_StepToF(&this->dyna.actor.velocity.y, -10.0f, 4.0f);
            this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
            if (this->dyna.actor.world.pos.y < this->dyna.actor.home.pos.y) {
                this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
                this->unk_172 &= ~0x10;
                this->unk_172 |= 2;
            }
        }
    }

    if (!DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        if (this->dyna.actor.xzDistToPlayer < 45.0f) {
            if ((this->dyna.actor.playerHeightRel < -this->dyna.actor.velocity.y) &&
                (this->dyna.actor.playerHeightRel >= -800.0f)) {
                sp28 = (45.0f - this->dyna.actor.xzDistToPlayer) * 0.5f;
                player->actor.world.pos.x += sp28 * Math_SinS(this->dyna.actor.yawTowardsPlayer);
                player->actor.world.pos.z += sp28 * Math_CosS(this->dyna.actor.yawTowardsPlayer);
            }
        }
    }
}

void ObjHunsui_Draw(Actor* thisx, PlayState* play) {
    ObjHunsui* this = (ObjHunsui*)thisx;

    if (this->unk_172 & 0x10) {
        f32 temp_f8 = (this->dyna.actor.world.pos.y - this->dyna.actor.home.pos.y) / 800.0f;

        Audio_PlaySfx_AtPosWithFreq(&this->dyna.actor.projectedPos, NA_SE_EV_WATER_PILLAR - SFX_FLAG, 1.0f + temp_f8);
    }

    if (!(this->unk_172 & 2)) {
        AnimatedMat_Draw(play, D_80B9DED0);
        Gfx_DrawDListXlu(play, object_hunsui_DL_000220);
    }
}

void func_80B9DA60(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjHunsui* this = (ObjHunsui*)thisx;
    f32 temp;

    if (this->unk_172 & 0x10) {
        temp = 1.0f + ((this->unk_178 - 240.0f) / 270.0f);
        Audio_PlaySfx_AtPosWithFreq(&this->dyna.actor.projectedPos, NA_SE_EV_WATER_PILLAR - SFX_FLAG, 1.0f + temp);
    }

    if ((this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && !(this->unk_172 & 2)) {
        if ((this->unk_160 == OBJHUNSUI_F000_6) || (this->unk_160 == OBJHUNSUI_F000_5)) {
            OPEN_DISPS(play->state.gfxCtx);

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, (play->gameplayFrames % 128) * -9, 0x20, 0x20, 1, 0,
                                          (play->gameplayFrames % 128) * -8, 0x20, 0x20, 0, -9, 0, -8));
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, (s32)this->unk_1AC, 0x20, 0x20, 1, 0,
                                          (s32)this->unk_1B0, 0x20, 0x20, 0, 1, 0, 1));

            CLOSE_DISPS(play->state.gfxCtx);
        } else {
            AnimatedMat_DrawXlu(play, D_80B9DED4);
        }

        OPEN_DISPS(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x7F, 255, 255, 255, 127);

        Gfx_DrawDListXlu(play, object_hunsui_DL_000EC0);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void ObjHunsui_Reset(void) {
    D_80B9DED8.unk_00 = 0;
    D_80B9DED8.unk_01 = 0;
    D_80B9DED8.unk_02 = 0;
}
