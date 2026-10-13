/*
 * File: z_obj_hakaisi.c
 * Overlay: ovl_Obj_Hakaisi
 * Description: Gravestone
 */

#include "z_obj_hakaisi.h"
#include "objects/object_hakaisi/object_hakaisi.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjHakaisi_Init(Actor* thisx, PlayState* play);
void ObjHakaisi_Destroy(Actor* thisx, PlayState* play);
void ObjHakaisi_Update(Actor* thisx, PlayState* play);
void ObjHakaisi_Draw(Actor* thisx, PlayState* play);

void func_80B1444C(ObjHakaisi* this);
void func_80B14460(ObjHakaisi* this, PlayState* play);
void func_80B14510(ObjHakaisi* this);
void func_80B14524(ObjHakaisi* this, PlayState* play);
void func_80B14558(ObjHakaisi* this);
void func_80B1456C(ObjHakaisi* this, PlayState* play);
void func_80B145F4(ObjHakaisi* this);
void func_80B14648(ObjHakaisi* this, PlayState* play);
void func_80B149A8(ObjHakaisi* this);
void func_80B149C0(ObjHakaisi* this, PlayState* play);
void func_80B14A24(ObjHakaisi* this, PlayState* play, Vec3f vec);
void func_80B14B6C(ObjHakaisi* this, PlayState* play, Vec3f vec, s16 arg3);
void func_80B14CF8(PlayState* play, Vec3f vec, s16 arg2, s16 arg3, s32 arg4);
void func_80B14F4C(ObjHakaisi* this, PlayState* play, s32 arg2);
void func_80B151E0(ObjHakaisi* this, PlayState* play);
void func_80B15254(Actor* thisx, PlayState* play);
void func_80B15264(ObjHakaisi* this);
void func_80B15330(ObjHakaisi* this, PlayState* play);
void func_80B1544C(Actor* thisx, PlayState* play);
void func_80B154A0(Actor* thisx, PlayState* play);

ActorProfile Obj_Hakaisi_Profile = {
    /**/ ACTOR_OBJ_HAKAISI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HAKAISI,
    /**/ sizeof(ObjHakaisi),
    /**/ ObjHakaisi_Init,
    /**/ ObjHakaisi_Destroy,
    /**/ ObjHakaisi_Update,
    /**/ ObjHakaisi_Draw,
};

Vec3f D_80B155B0 = { 0.0f, 25.0f, 30.0f };

Vec3f D_80B155BC[] = { { 0.0f, 65.0f, 8.0f }, { 0.0f, 35.0f, 8.0f }, { 0.0f, 15.0f, 8.0f } };

void ObjHakaisi_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjHakaisi* this = (ObjHakaisi*)thisx;
    CollisionHeader* sp7C = NULL;
    MtxF sp3C;
    s32 i;

    switch (OBJHAKAISI_GET_FF(&this->dyna.actor)) {
        case 0:
            this->dyna.actor.textId = 0x13F9;
            break;

        case 1:
            this->dyna.actor.textId = 0x13FA;
            break;

        case 2:
            this->dyna.actor.textId = 0x13FB;
            break;

        case 3:
            this->dyna.actor.textId = 0x13FC;
            break;

        case 4:
        case 5:
            func_80B151E0(this, play);
            return;

        default:
            this->dyna.actor.textId = 0x1412;
            break;
    }

    Actor_SetScale(&this->dyna.actor, 1.0f);
    this->dyna.actor.attentionRangeType = ATTENTION_RANGE_0;
    this->dyna.actor.colChkInfo.health = 30;
    if (OBJHAKAISI_GET_FF(&this->dyna.actor) == 3) {
        this->dyna.actor.draw = NULL;
        func_80B1444C(this);
        return;
    }

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    CollisionHeader_GetVirtual(&object_hakaisi_Colheader_002FC4, &sp7C);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, sp7C);
    this->unk_19A = 0;
    this->unk_198 = 0;
    this->switchFlag = OBJHAKAISI_GET_SWITCH_FLAG(thisx);
    this->csId = this->dyna.actor.csId;

    if (this->switchFlag == OBJHAKAISI_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if ((this->switchFlag != SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->dyna.actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (this->dyna.actor.floorPoly == NULL) {
        Actor_Kill(&this->dyna.actor);
    }

    func_800C0094(this->dyna.actor.floorPoly, this->dyna.actor.world.pos.x, this->dyna.actor.floorHeight,
                  this->dyna.actor.world.pos.z, &sp3C);
    Matrix_Put(&sp3C);
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    Matrix_Get(&sp3C);
    Matrix_MtxFToYXZRot(&sp3C, &this->dyna.actor.shape.rot, true);
    this->dyna.actor.world.rot = this->dyna.actor.shape.rot;
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_80B155B0, &this->dyna.actor.focus.pos);

    for (i = 0; i < ARRAY_COUNT(D_80B155BC); i++) {
        Matrix_MultVec3f(&D_80B155BC[i], &this->unk_160[i]);
    }

    func_80B1444C(this);
}

void ObjHakaisi_Destroy(Actor* thisx, PlayState* play) {
    ObjHakaisi* this = (ObjHakaisi*)thisx;

    if (OBJHAKAISI_GET_FF(&this->dyna.actor) != 3) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_80B1444C(ObjHakaisi* this) {
    this->actionFunc = func_80B14460;
}

void func_80B14460(ObjHakaisi* this, PlayState* play) {
    s16 sp26 = BINANG_SUB(this->dyna.actor.shape.rot.y, this->dyna.actor.yawTowardsPlayer);

    if (Actor_TalkOfferAccepted(&this->dyna.actor, &play->state)) {
        func_80B14510(this);
    } else if (this->dyna.actor.textId != 0) {
        if (ABS_ALT(sp26) < 0x2000) {
            Actor_OfferTalk(&this->dyna.actor, play, 100.0f);
        }
    }

    if (this->unk_198 == 1) {
        func_80B14558(this);
    }
}

void func_80B14510(ObjHakaisi* this) {
    this->actionFunc = func_80B14524;
}

void func_80B14524(ObjHakaisi* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->dyna.actor, play)) {
        func_80B1444C(this);
    }
}

void func_80B14558(ObjHakaisi* this) {
    this->actionFunc = func_80B1456C;
}

void func_80B1456C(ObjHakaisi* this, PlayState* play) {
    if (this->csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->dyna.actor);
        } else {
            CutsceneManager_Queue(this->csId);
        }
    }
    if (this->dyna.actor.colChkInfo.health < 30) {
        func_80B145F4(this);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_80B145F4(ObjHakaisi* this) {
    this->unk_19A = 0;
    this->dyna.actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
    this->actionFunc = func_80B14648;
}

void func_80B14648(ObjHakaisi* this, PlayState* play) {
    if (this->unk_19A < 2) {
        func_80B14CF8(play, this->unk_160[this->unk_194], 100, 30, 5);
    }

    this->unk_19A++;

    if (this->dyna.actor.colChkInfo.health == 0) {
        func_80B14A24(this, play, this->unk_160[this->unk_194]);
        func_80B14A24(this, play, this->unk_160[this->unk_194]);
        func_80B14B6C(this, play, this->unk_160[this->unk_194], 70);
        Flags_SetSwitch(play, this->switchFlag);
        this->dyna.actor.draw = NULL;
        func_80B149A8(this);
    }

    if (this->unk_19E != this->dyna.actor.colChkInfo.health) {
        if ((this->unk_19E > 20) && (this->dyna.actor.colChkInfo.health <= 20)) {
            func_80B14A24(this, play, this->unk_160[this->unk_194]);
            func_80B14A24(this, play, this->unk_160[this->unk_194]);
            func_80B14F4C(this, play, false);
            func_80B14B6C(this, play, this->unk_160[this->unk_194], 40);
            this->unk_194 = 1;
        } else if ((this->unk_19E > 10) && (this->dyna.actor.colChkInfo.health <= 10)) {
            func_80B14A24(this, play, this->unk_160[this->unk_194]);
            func_80B14A24(this, play, this->unk_160[this->unk_194]);
            func_80B14B6C(this, play, this->unk_160[this->unk_194], 60);
            this->unk_194 = 2;
        }
        this->unk_19A = 0;
        this->dyna.actor.shape.yOffset -= 3.0f;
    }

    Math_ApproachZeroF(&this->dyna.actor.shape.yOffset, 0.8f, 100.0f);
    this->unk_19E = this->dyna.actor.colChkInfo.health;
}

void func_80B149A8(ObjHakaisi* this) {
    this->unk_19A = 0;
    this->actionFunc = func_80B149C0;
}

void func_80B149C0(ObjHakaisi* this, PlayState* play) {
    if (this->unk_19A < 60) {
        this->unk_19A++;
    } else if ((this->csId != CS_ID_NONE) && !CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Stop(this->csId);
    }
}

void func_80B14A24(ObjHakaisi* this, PlayState* play, Vec3f vec) {
    s32 i;

    func_80B14CF8(play, vec, 100, 30, 5);

    for (i = 0; i < 5; i++) {
        vec.x += Rand_Centered() * 20.0f;
        vec.y += Rand_ZeroOne() * 5.0f;
        vec.z += Rand_Centered() * 20.0f;
        EffectSsHahen_SpawnBurst(play, &vec, 5.0f, 0, 20, 15, 3, OBJECT_HAKAISI, 10, object_hakaisi_DL_0021B0);
    }
}

void func_80B14B6C(ObjHakaisi* this, PlayState* play, Vec3f vec, s16 arg3) {
    s32 i;
    s16 temp_s1;
    Vec3f sp6C;

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);

    for (i = 0; i < 5; i++) {
        temp_s1 = Rand_Next();

        sp6C.x = (Math_SinS(temp_s1) * 15.0f) + vec.x;
        sp6C.y = (Rand_ZeroOne() * 3.0f) + vec.y;
        sp6C.z = (Math_CosS(temp_s1) * 15.0f) + vec.z;

        EffectSsHahen_SpawnBurst(play, &sp6C, 10.0f, 0, arg3, 30, 2, OBJECT_HAKAISI, 15, object_hakaisi_DL_0021B0);
    }

    func_80B14CF8(play, vec, 100, 50, 20);
}

void func_80B14CF8(PlayState* play, Vec3f vec, s16 arg2, s16 arg3, s32 arg4) {
    static Color_RGBA8 sPrimColor = { 170, 130, 90, 255 };
    static Color_RGBA8 sEnvColor = { 100, 60, 20, 0 };
    s32 i;
    s16 temp_s0;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    f32 temp_f20;
    f32 temp_f22;

    for (i = 0; i < arg4; i++) {
        temp_f20 = Rand_ZeroOne() * 30.0f;
        temp_f22 = Rand_ZeroOne() * 1.5f;
        temp_s0 = Rand_Next();

        pos.x = (Math_SinS(temp_s0) * temp_f20) + vec.x;
        pos.y = (Rand_Centered() * 4.0f) + vec.y;
        pos.z = (Math_CosS(temp_s0) * temp_f20) + vec.z;

        velocity.x += temp_f22 * Math_SinS(temp_s0);
        velocity.y += Rand_Centered() + 0.5f;
        velocity.z += temp_f22 * Math_CosS(temp_s0);

        accel.x = -0.1f * velocity.x;
        accel.y = -0.1f * velocity.y;
        accel.z = -0.1f * velocity.z;

        func_800B0EB0(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, arg2, arg3, 10);
    }
}

void func_80B14F4C(ObjHakaisi* this, PlayState* play, s32 arg2) {
    if (!arg2) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_HAKAISI, this->dyna.actor.world.pos.x,
                    this->dyna.actor.world.pos.y + 55.0f, this->dyna.actor.world.pos.z - 10.0f,
                    this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 4);
    } else {
        Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_HAKAISI, this->dyna.actor.world.pos.x + 20.0f,
                    this->dyna.actor.world.pos.y + 30.0f, this->dyna.actor.world.pos.z - 10.0f,
                    this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 5);
    }
}

void ObjHakaisi_Update(Actor* thisx, PlayState* play) {
    ObjHakaisi* this = (ObjHakaisi*)thisx;

    this->actionFunc(this, play);
}

void ObjHakaisi_Draw(Actor* thisx, PlayState* play) {
    ObjHakaisi* this = (ObjHakaisi*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (this->unk_194 == 0) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_hakaisi_DL_002650);
    } else if (this->unk_194 == 1) {
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_hakaisi_DL_0029C0);
    } else {
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_hakaisi_DL_002CC0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B151E0(ObjHakaisi* this, PlayState* play) {
    this->dyna.actor.update = func_80B1544C;
    this->dyna.actor.draw = func_80B154A0;
    this->dyna.actor.destroy = func_80B15254;
    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->dyna.actor.shape.yOffset = 100.0f;
    this->dyna.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    func_80B15264(this);
}

void func_80B15254(Actor* thisx, PlayState* play) {
}

void func_80B15264(ObjHakaisi* this) {
    static Vec3f D_80B15600 = { 1.0f, 0.0f, 0.0f };
    s32 pad;
    s16 sp32 = Rand_Next();

    Matrix_RotateZYX(Rand_Next(), Rand_Next(), Rand_Next(), MTXMODE_NEW);
    Matrix_MultVec3f(&D_80B15600, &this->unk_184);
    this->dyna.actor.gravity = -1.0f;
    this->unk_19C = (s32)Rand_Next() >> 0x12;
    this->dyna.actor.velocity.x = Math_SinS(sp32) * 4.0f;
    this->dyna.actor.velocity.z = Math_CosS(sp32) * 4.0f;
    this->dyna.actor.velocity.y = 7.0f;
    this->actionFunc = func_80B15330;
}

void func_80B15330(ObjHakaisi* this, PlayState* play) {
    s32 pad;
    MtxF sp34;

    this->dyna.actor.velocity.y += this->dyna.actor.gravity;
    Actor_UpdatePos(&this->dyna.actor);

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        func_80B14B6C(this, play, this->dyna.actor.world.pos, 40);
        func_80B14CF8(play, this->dyna.actor.world.pos, 100, 30, 10);
        Actor_Kill(&this->dyna.actor);
    }

    Matrix_RotateAxisS(this->unk_19C, &this->unk_184, MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateXS(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Get(&sp34);

    Matrix_MtxFToYXZRot(&sp34, &this->dyna.actor.shape.rot, false);
}

void func_80B1544C(Actor* thisx, PlayState* play) {
    ObjHakaisi* this = (ObjHakaisi*)thisx;

    this->actionFunc(this, play);

    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void func_80B154A0(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (OBJHAKAISI_GET_FF(thisx) == 4) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_hakaisi_DL_001F10);
    } else {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_hakaisi_DL_0021B0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
