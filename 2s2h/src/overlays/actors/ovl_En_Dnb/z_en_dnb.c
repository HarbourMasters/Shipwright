/*
 * File: z_en_dnb.c
 * Overlay: ovl_En_Dnb
 * Description: Unused invisible snowy mountain that explodes?
 */

#include "z_en_dnb.h"
#include "objects/object_hanareyama_obj/object_hanareyama_obj.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_REACT_TO_LENS)

void EnDnb_Init(Actor* thisx, PlayState* play);
void EnDnb_Destroy(Actor* thisx, PlayState* play);
void EnDnb_Update(Actor* thisx, PlayState* play);
void EnDnb_Draw(Actor* thisx, PlayState* play);

s32 func_80A507C0(EnDnbUnkStruct* arg0, Vec3f arg1, Vec3f arg2, u8 arg3, f32 arg4, f32 arg5);
s32 func_80A5086C(EnDnbUnkStruct* arg0);
s32 func_80A50950(EnDnbUnkStruct* arg0, PlayState* play2);

ActorProfile En_Dnb_Profile = {
    /**/ ACTOR_EN_DNB,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HANAREYAMA_OBJ,
    /**/ sizeof(EnDnb),
    /**/ EnDnb_Init,
    /**/ EnDnb_Destroy,
    /**/ EnDnb_Update,
    /**/ EnDnb_Draw,
};

void func_80A4FDD0(EnDnbEffect* effect, EnDnb* this, s16* alloc, s32 idx) {
    Vec3f sp1C;
    s32 idx2 = idx * 3;

    sp1C.x = alloc[idx2 + 0] + this->dyna.actor.world.pos.x;
    sp1C.y = alloc[idx2 + 1] + this->dyna.actor.world.pos.y;
    sp1C.z = alloc[idx2 + 2] + this->dyna.actor.world.pos.z;
    effect->unk_00 = sp1C;
    effect->unk_0C = sp1C;
    effect->unk_24 = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &sp1C);
    effect->unk_18 = gZeroVec3s;
}

s32 func_80A4FEBC(EnDnbEffect* effect, f32 arg1) {
    s32 ret = false;

    if ((DECR(effect->unk_26) == 0) && (arg1 < effect->unk_0C.y)) {
        Math_ApproachF(&effect->unk_30, 1.0f, 0.4f, 1.0f);
        effect->unk_2C += effect->unk_34;
        effect->unk_0C.x += effect->unk_30 * Math_SinS(effect->unk_24);
        effect->unk_0C.z += effect->unk_30 * Math_CosS(effect->unk_24);
        effect->unk_0C.y += effect->unk_2C;
        if (effect->unk_0C.y <= arg1) {
            effect->unk_0C.y = arg1;
        }
        effect->unk_18.x += effect->unk_1E.x;
        effect->unk_18.y += effect->unk_1E.y;
        effect->unk_18.z += effect->unk_1E.z;
        ret = true;
    }

    return ret;
}

void func_80A4FFE8(EnDnbEffect* effect, s16 arg1) {
    effect->unk_0C = effect->unk_00;
    effect->unk_1E.x = (Rand_ZeroOne() - 0.5f) * 400.0f;
    effect->unk_1E.y = (Rand_ZeroOne() - 0.5f) * 400.0f;
    effect->unk_1E.z = (Rand_ZeroOne() - 0.5f) * 400.0f;
    effect->unk_18 = gZeroVec3s;
    effect->unk_30 = 40.0f;
    effect->unk_2C = 0.0f;
    effect->unk_26 = arg1;
    effect->unk_34 = (Rand_ZeroOne() * -2.0f) - 2.0f;
}

s32 func_80A500F8(EnDnb* this) {
    static Vec3f D_80A50CB0 = { 0.0f, 0.0f, 1000.0f };
    Actor* actor = &this->dyna.actor;
    Vec3f spA8 = gZeroVec3f;
    Vec3f sp9C;
    s32 i;
    f32 temp_f20;

    for (i = 0; i < 16; i++) {
        Lib_Vec3f_TranslateAndRotateY(&actor->world.pos, i * 4096, &D_80A50CB0, &spA8);
        temp_f20 = Rand_ZeroOne() * 240.0f;
        sp9C.x = Math_SinS(i * 4096) * temp_f20;
        sp9C.z = Math_CosS(i * 4096) * temp_f20;
        sp9C.y = Rand_ZeroOne() * 180.0f;
        func_80A507C0(this->unk_0D38, spA8, sp9C, 16, 50.0f, 30.0f);
    }

    return i;
}

void EnDnb_Init(Actor* thisx, PlayState* play) {
    EnDnb* this = (EnDnb*)thisx;
    s32 i;
    s16* alloc;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_hanareyama_obj_Colheader_004D8C);

    alloc = Lib_SegmentedToVirtual(object_hanareyama_obj_Vec_004710);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        func_80A4FDD0(&this->effects[i], this, alloc, i);
    }

    Actor_SetScale(&this->dyna.actor, 1.0f);
}

void EnDnb_Destroy(Actor* thisx, PlayState* play) {
    EnDnb* this = (EnDnb*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnDnb_Update(Actor* thisx, PlayState* play) {
    EnDnb* this = (EnDnb*)thisx;
    s32 i;

    if (this->unk_0D30 == 0) {
        for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
            func_80A4FFE8(&this->effects[i], ((53 - i) / 18) * 4);
        }

        for (i = 0; i < ARRAY_COUNT(this->unk_0D38); i++) {
            this->unk_0D38[i].isEnabled = false;
        }
        this->unk_0D32 = 20;
        this->unk_0D30 = 100;
    } else if (DECR(this->unk_0D32) == 0) {
        if ((DECR(this->unk_0D34) == 0) && (this->unk_0D30 >= 21)) {
            this->unk_0D34 = 5;
            func_80A500F8(this);
        }

        for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
            func_80A4FEBC(&this->effects[i], this->dyna.actor.world.pos.y);
        }

        this->unk_0D30--;
    }
    func_80A5086C(this->unk_0D38);
}

void func_80A50510(EnDnb* this, PlayState* play) {
    s32 i;
    Gfx** gfx = Lib_SegmentedToVirtual(object_hanareyama_obj_DLArray_004638);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        Matrix_Push();
        Matrix_Translate(this->effects[i].unk_0C.x, this->effects[i].unk_0C.y, this->effects[i].unk_0C.z, MTXMODE_NEW);
        Matrix_RotateXS(this->effects[i].unk_18.x, MTXMODE_APPLY);
        Matrix_RotateYS(this->effects[i].unk_18.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->effects[i].unk_18.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gfx[i]);

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A5063C(EnDnb* this, PlayState* play) {
    s32 i;
    Gfx** gfx = Lib_SegmentedToVirtual(object_hanareyama_obj_DLArray_004638);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++) {
        Matrix_Push();
        Matrix_Translate(this->effects[i].unk_0C.x, this->effects[i].unk_0C.y, this->effects[i].unk_0C.z, MTXMODE_NEW);
        Matrix_RotateXS(this->effects[i].unk_18.x, MTXMODE_APPLY);
        Matrix_RotateYS(this->effects[i].unk_18.y, MTXMODE_APPLY);
        Matrix_RotateZS(this->effects[i].unk_18.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gfx[i]);

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDnb_Draw(Actor* thisx, PlayState* play) {
    EnDnb* this = (EnDnb*)thisx;

    if (play->actorCtx.lensMaskSize != 0) {
        func_80A50510(this, play);
    } else {
        func_80A5063C(this, play);
    }
    func_80A50950(this->unk_0D38, play);
}

s32 func_80A507C0(EnDnbUnkStruct* arg0, Vec3f arg1, Vec3f arg2, u8 arg3, f32 arg4, f32 arg5) {
    s32 i;
    s32 ret = 0;

    for (i = 0; i < 256; i++, arg0++) {
        if (arg0->isEnabled == false) {
            arg0->isEnabled = true;
            arg0->unk_01 = arg3;
            arg0->unk_02 = arg3;
            arg0->unk_0C = arg1;
            arg0->unk_18 = arg2;
            arg0->unk_04 = arg4;
            arg0->unk_08 = arg5;
            arg0->unk_24 = 255.0f;
            ret = 1;
            break;
        }
    }
    return ret;
}

s32 func_80A5086C(EnDnbUnkStruct* arg0) {
    s32 count = 0;
    s32 i;

    for (i = 0; i < 256; i++, arg0++) {
        if (arg0->isEnabled == true) {
            count++;
            if (arg0->unk_01 != 0) {
                arg0->unk_0C.x += arg0->unk_18.x;
                arg0->unk_0C.y += arg0->unk_18.y;
                arg0->unk_0C.z += arg0->unk_18.z;
                arg0->unk_01 -= 1;
            } else {
                arg0->isEnabled = false;
            }
        }
    }

    return count;
}

s32 func_80A50950(EnDnbUnkStruct* arg0, PlayState* play2) {
    static TexturePtr sDustTextures[] = {
        gEffDust8Tex, gEffDust7Tex, gEffDust6Tex, gEffDust5Tex, gEffDust4Tex, gEffDust3Tex, gEffDust2Tex, gEffDust1Tex,
    };
    PlayState* play = play2;
    s32 isGfxSetup = false;
    s32 sp5C = 0;
    s32 idx;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < 256; i++, arg0++) {
        if (arg0->isEnabled != 1) {
            continue;
        }

        if (!isGfxSetup) {
            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, object_hanareyama_obj_DL_000000);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 0);
            isGfxSetup = true;
        }
        Matrix_Push();

        arg0->unk_24 = (arg0->unk_01 / (f32)arg0->unk_02) * 255.0f;

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)arg0->unk_24);

        Matrix_Translate(arg0->unk_0C.x, arg0->unk_0C.y, arg0->unk_0C.z, MTXMODE_NEW);
        Matrix_Scale(arg0->unk_04, arg0->unk_04, 1.0f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        idx = (arg0->unk_01 / (f32)arg0->unk_02) * 8.0f;
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sDustTextures[idx]));
        gSPDisplayList(POLY_XLU_DISP++, object_hanareyama_obj_DL_000020);

        Matrix_Pop();
        sp5C += 1;
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return sp5C;
}
