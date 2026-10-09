/*
 * File: z_en_hanabi.c
 * Overlay: ovl_En_Hanabi
 * Description: Fireworks
 */

#include "z_en_hanabi.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnHanabi_Init(Actor* thisx, PlayState* play2);
void EnHanabi_Destroy(Actor* thisx, PlayState* play2);
void EnHanabi_Update(Actor* thisx, PlayState* play);

void func_80B23894(EnHanabi* this, PlayState* play);
void func_80B238D4(EnHanabi* this, PlayState* play);
void func_80B23910(EnHanabi* this, PlayState* play);
void func_80B23934(EnHanabi* this, PlayState* play);
void EnHanabi_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Hanabi_Profile = {
    /**/ ACTOR_EN_HANABI,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnHanabi),
    /**/ EnHanabi_Init,
    /**/ EnHanabi_Destroy,
    /**/ EnHanabi_Update,
    /**/ NULL,
};

Vec3s D_80B23AA0[] = {
    { 85, 14, 49 },    { 70, -70, 0 },    { 85, 14, -49 },  { 90, -32, 28 },   { 90, -32, -28 }, { 98, 16, 0 },
    { 70, 70, 0 },     { 84, 45, 26 },    { 84, 45, -26 },  { 50, -50, 70 },   { 73, -19, 64 },  { 65, -65, 38 },
    { 50, -50, -70 },  { 65, -65, -38 },  { 73, -19, -64 }, { 0, 0, -100 },    { 14, 85, -49 },  { 49, 8, -86 },
    { 8, 49, -86 },    { 57, 57, -57 },   { 45, 84, -26 },  { 27, -27, -92 },  { -50, 50, -70 }, { -27, 27, -92 },
    { -19, 73, -64 },  { -70, 70, 0 },    { 14, 85, 49 },   { -32, 90, -28 },  { -32, 90, 28 },  { 16, 98, 0 },
    { 45, 84, 26 },    { -65, 65, -38 },  { -50, 50, 70 },  { -65, 65, 38 },   { -19, 73, 64 },  { 0, 0, 100 },
    { 8, 49, 86 },     { 49, 8, 86 },     { 57, 57, 57 },   { -27, 27, 92 },   { 27, -27, 92 },  { -14, -85, -49 },
    { -14, -85, 49 },  { 32, -90, -28 },  { 32, -90, 28 },  { -16, -98, 0 },   { -70, -70, 0 },  { -45, -84, -26 },
    { -45, -84, 26 },  { 19, -73, -64 },  { 19, -73, 64 },  { -85, -14, -49 }, { -49, -8, -86 }, { -8, -49, -86 },
    { -57, -57, -57 }, { -84, -45, -26 }, { -73, 19, -64 }, { -85, -14, 49 },  { -90, 32, 28 },  { -90, 32, -28 },
    { -98, -16, 0 },   { -84, -45, 26 },  { -73, 19, 64 },  { -8, -49, 86 },   { -49, -8, 86 },  { -57, -57, 57 },
};

u8 D_80B23C2C[] = {
    255, 255, 200, 255, 200, 255, 200, 255, 255, 255, 200, 200, 200, 255, 200, 200, 200, 255,
};

u8 D_80B23C40[] = {
    200, 200, 0, 200, 0, 200, 0, 200, 200, 200, 0, 0, 0, 200, 0, 0, 200, 200,
};

void func_80B22C00(EnHanabiStruct* arg0) {
    s32 i;

    for (i = 0; i < 400; i++, arg0++) {
        arg0->unk_00 = 0;
    }
}

void func_80B22C2C(EnHanabiStruct* arg0, s32 arg1) {
    arg0->unk_01 = (s32)Rand_ZeroFloat(20.0f) + 60;
    arg0->unk_02 = arg1;
    arg0->unk_14.x = 0.7f;
}

s32 func_80B22C80(Vec3f* arg0, EnHanabiStruct* arg1, f32 arg2) {
    Vec3s* phi_s1 = &D_80B23AA0[0];
    f32 temp_f22 = (Rand_ZeroFloat(0.2f) + 0.6f) * arg2;
    s32 temp_s6 = (s32)Rand_ZeroFloat(6.0f) * 3;
    s32 i;
    s32 j;

    for (i = 0, j = 0; j < ARRAY_COUNT(D_80B23AA0) && i < 400; i++, arg1++) {
        if (arg1->unk_00 <= 0) {
            arg1->unk_00 = 1;
            Math_Vec3f_Copy(&arg1->unk_08, arg0);
            arg1->unk_20.x = phi_s1->x * temp_f22;
            arg1->unk_20.y = (phi_s1->y * temp_f22) + 50.0f;
            arg1->unk_20.z = phi_s1->z * temp_f22;
            arg1->unk_04 = 1.5f * arg2;
            func_80B22C2C(arg1, temp_s6);
            phi_s1++;
            j++;
        }
    }

    return temp_s6;
}

void func_80B22E0C(EnHanabiStruct* arg0) {
    s32 i;

    for (i = 0; i < 400; i++, arg0++) {
        if (arg0->unk_00 == 1) {
            arg0->unk_08.x += arg0->unk_20.x;
            arg0->unk_08.y += arg0->unk_20.y;
            arg0->unk_08.z += arg0->unk_20.z;

            arg0->unk_20.x *= arg0->unk_14.x;
            arg0->unk_20.z *= arg0->unk_14.x;
            arg0->unk_20.y *= arg0->unk_14.x;
            arg0->unk_20.y -= 1.0f;

            arg0->unk_20.x += Rand_CenteredFloat(0.8f);
            arg0->unk_20.y += Rand_CenteredFloat(0.8f);
            arg0->unk_20.z += Rand_CenteredFloat(0.8f);

            if (arg0->unk_01 > 0) {
                arg0->unk_01--;
            } else {
                arg0->unk_00 = 0;
            }
        }
    }
}

s32 func_80B22F34(EnHanabiStruct* arg0) {
    s32 count = 0;
    s32 i;

    for (i = 0; i < 400; i++, arg0++) {
        if (arg0->unk_00 == 1) {
            count++;
        }
    }

    return count;
}

void func_80B22FA8(EnHanabiStruct* arg0, PlayState* play2) {
    PlayState* play = play2;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 i;
    u8 sp53;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);

    gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gSun1Tex));
    gSPDisplayList(POLY_XLU_DISP++, gSunSparkleMaterialDL);

    sp53 = 0xFF;

    for (i = 0; i < 400; i++, arg0++) {
        if (arg0->unk_00 != 1) {
            continue;
        }
        FrameInterpolation_RecordOpenChild(arg0, i);
        Matrix_Translate(arg0->unk_08.x, arg0->unk_08.y, arg0->unk_08.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        if (arg0->unk_01 < 40) {
            Matrix_Scale(arg0->unk_04 * 0.025f * arg0->unk_01, arg0->unk_04 * 0.025f * arg0->unk_01, 1.0f,
                         MTXMODE_APPLY);
        } else {
            Matrix_Scale(arg0->unk_04, arg0->unk_04, 1.0f, MTXMODE_APPLY);
        }
        Matrix_RotateZS(play->gameplayFrames * 4864, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        if (sp53 != arg0->unk_02) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, D_80B23C40[arg0->unk_02], D_80B23C40[arg0->unk_02 + 1],
                           D_80B23C40[arg0->unk_02 + 2], 255);

            sp53 = arg0->unk_02;
        }

        if (arg0->unk_01 < 6) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, D_80B23C2C[arg0->unk_02], D_80B23C2C[arg0->unk_02 + 1],
                            D_80B23C2C[arg0->unk_02 + 2], arg0->unk_01 * 50);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, D_80B23C2C[arg0->unk_02], D_80B23C2C[arg0->unk_02 + 1],
                            D_80B23C2C[arg0->unk_02 + 2], 255);
        }

        gSPDisplayList(POLY_XLU_DISP++, gSunSparkleModelDL);
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(gfxCtx);
}

void EnHanabi_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHanabi* this = (EnHanabi*)thisx;
    s32 i;

    //! FAKE:
    if (1) {}

    Actor_SetScale(thisx, 1.0f);
    this->actionFunc = func_80B23910;
    this->unk_144 = 0;
    func_80B22C00(this->unk_148);

    for (i = 0; i < ARRAY_COUNT(this->unk_4608); i++) {
        Lights_PointNoGlowSetInfo(&this->unk_4608[i], thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 255,
                                  255, 255, -1);
        this->unk_4634[i] = LightContext_InsertLight(play, &play->lightCtx, &this->unk_4608[i]);
    }

    if (ENHANABI_GET_1F(thisx) == ENHANABI_1F_1) {
        this->actionFunc = func_80B23934;
        this->actor.home.rot.x = 0x384;
    }
}

void EnHanabi_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHanabi* this = (EnHanabi*)thisx;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_4634); i++) {
        LightContext_RemoveLight(play, &play->lightCtx, this->unk_4634[i]);
    }
}

void func_80B234C8(EnHanabi* this) {
    s16 phi_v1;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_4640); i++) {
        if (this->unk_4640[i] > 0) {
            this->unk_4640[i]--;
            if (this->unk_4640[i] == 0) {
                phi_v1 = -1;
            } else {
                if (this->unk_4640[i] >= 0xA) {
                    phi_v1 = 0x3E8;
                } else {
                    phi_v1 = this->unk_4640[i] * 0x64;
                }

                if (this->actor.home.rot.x >= 0x259) {
                    phi_v1 *= 2;
                }
            }

            Lights_PointSetColorAndRadius(&this->unk_4608[i], D_80B23C2C[this->unk_4648[i]],
                                          D_80B23C2C[this->unk_4648[i] + 1], D_80B23C2C[this->unk_4648[i] + 2], phi_v1);
        }
    }
}

void func_80B235CC(EnHanabi* this, Vec3f* arg1, s32 arg2) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_4640); i++) {
        if (this->unk_4640[i] == 0) {
            this->unk_4648[i] = arg2;
            Lights_PointNoGlowSetInfo(&this->unk_4608[i], arg1->x, arg1->y, arg1->z, D_80B23C2C[arg2],
                                      D_80B23C2C[arg2 + 1], D_80B23C2C[arg2 + 2], 0x3E8);
            this->unk_4640[i] = 20;
            break;
        }
    }
}

void func_80B236C8(EnHanabi* this, PlayState* play) {
    Vec3f sp34;
    f32 sp30;
    s32 pad;
    s32 sp28;

    this->actor.draw = EnHanabi_Draw;
    func_80B234C8(this);
    if (this->actor.home.rot.x != 0) {
        if (this->unk_144 > 0) {
            this->unk_144--;
            return;
        }

        Math_Vec3f_Copy(&sp34, &this->actor.world.pos);

        if (this->actor.home.rot.x != 0) {
            sp30 = Rand_ZeroFloat(200.0f) + this->actor.home.rot.x;
        } else {
            sp30 = Rand_ZeroFloat(200.0f) + 300.0f;
        }

        sp34.x += sp30 * Math_SinS(this->actor.home.rot.y);
        sp34.y += Rand_CenteredFloat(300.0f);
        sp34.z += sp30 * Math_CosS(this->actor.home.rot.y);

        if (this->actor.home.rot.x > 0x258) {
            sp28 = func_80B22C80(&sp34, this->unk_148, 2.0f);
        } else {
            sp28 = func_80B22C80(&sp34, this->unk_148, 1.0f);
        }

        this->actor.home.rot.y += TRUNCF_BINANG((Rand_ZeroFloat(40.0f) + 80.0f) * 256.0f);
        this->unk_144 = (s32)Rand_ZeroFloat(5.0f) + 20;
        Actor_PlaySfx(&this->actor, NA_SE_OC_FIREWORKS);
        func_80B235CC(this, &sp34, sp28);
    }
}

void func_80B23894(EnHanabi* this, PlayState* play) {
    func_80B236C8(this, play);
    if (func_80B22F34(this->unk_148) == 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80B238D4(EnHanabi* this, PlayState* play) {
    func_80B236C8(this, play);
    if (this->actor.home.rot.x == 0) {
        this->actionFunc = func_80B23894;
    }
}

void func_80B23910(EnHanabi* this, PlayState* play) {
    if (this->actor.home.rot.x != 0) {
        this->actionFunc = func_80B238D4;
    }
}

void func_80B23934(EnHanabi* this, PlayState* play) {
    if ((gSaveContext.save.entrance == ENTRANCE(TERMINA_FIELD, 1)) && (gSaveContext.sceneLayer == 7)) {
        if (play->csCtx.curFrame > 1650) {
            func_80B236C8(this, play);
            Actor_PlaySfx_FlaggedCentered2(&this->actor, NA_SE_EV_FIREWORKS_LAUNCH - SFX_FLAG);
        }
    }

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 7) && (play->csCtx.scriptIndex == 0) &&
        (play->csCtx.curFrame == 610)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_KYOJIN_GROAN);
    }
}

void EnHanabi_Update(Actor* thisx, PlayState* play) {
    EnHanabi* this = (EnHanabi*)thisx;

    this->actionFunc(this, play);

    func_80B22E0C(this->unk_148);
}

void EnHanabi_Draw(Actor* thisx, PlayState* play) {
    EnHanabi* this = (EnHanabi*)thisx;

    Matrix_Push();
    func_80B22FA8(this->unk_148, play);
    Matrix_Pop();
}
