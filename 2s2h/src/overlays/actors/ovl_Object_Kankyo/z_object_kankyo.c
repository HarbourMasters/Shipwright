/*
 * File: z_object_kankyo.c
 * Overlay: ovl_Object_Kankyo
 * Description: Snow, rain in Skull Kid backstory cutscene, and bubbles deep in Pinnacle Rock
 */

#include "z_object_kankyo.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "BenPort.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void ObjectKankyo_Init(Actor* thisx, PlayState* play);
void ObjectKankyo_Destroy(Actor* thisx, PlayState* play);
void ObjectKankyo_Update(Actor* thisx, PlayState* play);
void ObjectKankyo_Draw(Actor* thisx, PlayState* play);

void ObjectKankyo_SetupAction(ObjectKankyo* this, ObjectKankyoActionFunc actionFunc);
void func_808DC18C(ObjectKankyo* this, PlayState* play);
void func_808DCB7C(ObjectKankyo* this, PlayState* play);
void func_808DCBF8(ObjectKankyo* this, PlayState* play);
void func_808DCDB4(ObjectKankyo* this, PlayState* play);
void func_808DD3C8(Actor* thisx, PlayState* play2);
void func_808DD970(Actor* thisx, PlayState* play2);
void func_808DDE9C(Actor* thisx, PlayState* play2);

static f32 D_808DE5B0;

ActorProfile Object_Kankyo_Profile = {
    /**/ ACTOR_OBJECT_KANKYO,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjectKankyo),
    /**/ ObjectKankyo_Init,
    /**/ ObjectKankyo_Destroy,
    /**/ ObjectKankyo_Update,
    /**/ ObjectKankyo_Draw,
};

static u16 D_808DE340 = 0;

void ObjectKankyo_SetupAction(ObjectKankyo* this, ObjectKankyoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void func_808DBE8C(ObjectKankyo* this) {
    ObjectKankyo_SetupAction(this, func_808DC18C);
}

void func_808DBEB0(ObjectKankyo* this, PlayState* play) {
    s32 i;

    D_808DE5B0 = 0.0f;
    this->unk_144 = Rand_ZeroOne() * 360.0f;
    this->unk_148 = Rand_ZeroOne() * 360.0f;
    if (play->envCtx.precipitation[PRECIP_SNOW_CUR] == 128) {
        D_808DE5B0 = 1.0f;
        this->unk_114E = 1;

        for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_CUR]; i++) {
            this->unk_14C[i].unk_10 = Rand_ZeroOne() * -200.0f;
        }
    } else {
        this->unk_114E = 0;
    }
    ObjectKankyo_SetupAction(this, func_808DCB7C);
}

void func_808DBFB0(ObjectKankyo* this, PlayState* play) {
    D_808DE5B0 = 0.0f;
    this->unk_114E = 0;
    this->unk_144 = Rand_ZeroOne() * 360.0f;
    this->unk_148 = Rand_ZeroOne() * 360.0f;
    this->unk_114C = D_808DE340;
    D_808DE340++;
    ObjectKankyo_SetupAction(this, func_808DCBF8);
}

void func_808DC038(ObjectKankyo* this, PlayState* play) {
    s16 i;

    this->unk_144 = Rand_ZeroOne() * 360.0f;
    this->unk_148 = Rand_ZeroOne() * 360.0f;
    this->unk_114C = 0;

    for (i = 0; i < ARRAY_COUNT(this->unk_14C); i++) {
        this->unk_14C[i].unk_1C = 0;
    }

    ObjectKankyo_SetupAction(this, func_808DCDB4);
}

void ObjectKankyo_Init(Actor* thisx, PlayState* play) {
    ObjectKankyo* this = (ObjectKankyo*)thisx;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_14C); i++) {
        this->unk_14C[i].unk_1C = 0;
    }

    this->actor.room = -1;
    switch (this->actor.params) {
        case 0:
            func_808DBE8C(this);
            break;

        case 2:
            play->envCtx.precipitation[PRECIP_SNOW_CUR] = 128;
            func_808DBFB0(this, play);
            break;

        case 1:
        case 3:
            func_808DBEB0(this, play);
            break;

        case 4:
            func_808DC038(this, play);
            break;
    }
}

void ObjectKankyo_Destroy(Actor* thisx, PlayState* play) {
    ObjectKankyo* this = (ObjectKankyo*)thisx;

    Actor_Kill(&this->actor);
}

void func_808DC18C(ObjectKankyo* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 pad3;
    f32 magnitude;
    f32 y;
    f32 z;
    f32 temp_f18;
    Vec3f sp30;
    f32 x;
    f32 sp1C;

    x = play->view.at.x - play->view.eye.x;
    y = play->view.at.y - play->view.eye.y;
    z = play->view.at.z - play->view.eye.z;
    magnitude = sqrtf(SQ(x) + SQ(y) + SQ(z));

    temp_f18 = x / magnitude;
    x = z / magnitude;
    sp1C = (y / magnitude) * 120.0f;

    this->unk_14C[0].unk_00 = play->view.eye.x + (temp_f18 * 50.0f);
    this->unk_14C[0].unk_04 = play->view.eye.y + sp1C;
    this->unk_14C[0].unk_08 = play->view.eye.z + (x * 50.0f);
    this->unk_14C[1].unk_00 = play->view.eye.x + (temp_f18 * 220.0f);
    this->unk_14C[1].unk_08 = play->view.eye.z + (x * 220.0f);
    this->unk_114C = 0;
    this->unk_144 = 100.0f;

    if ((this->unk_14C[0].unk_00 < -252.0f) && (this->unk_14C[0].unk_00 > -500.0f)) {
        if ((this->unk_14C[0].unk_08 > 3820.0f) && (this->unk_14C[0].unk_08 < 4150.0f)) {
            this->unk_114C = 1;
            this->unk_144 = 400.0f;
            if (x < 0.0f) {
                this->unk_14C[0].unk_00 = -350.0f;
                this->unk_14C[0].unk_04 = play->view.eye.y + sp1C;
                this->unk_14C[0].unk_08 = 3680.0f;
                this->unk_14C[1].unk_00 = -350.0f;
                this->unk_14C[1].unk_08 = 3680.0f;
            } else {
                this->unk_14C[0].unk_00 = -350.0f;
                this->unk_14C[0].unk_04 = play->view.eye.y + sp1C;
                this->unk_14C[0].unk_08 = 4280.0f;
                this->unk_14C[1].unk_00 = -350.0f;
                this->unk_14C[1].unk_08 = 4280.0f;
            }
        }
    }

    magnitude = play->envCtx.windSpeed / 60.0f;
    magnitude = CLAMP(magnitude, 0.0f, 1.0f);

    sp30.x = play->envCtx.windDirection.x * magnitude;
    sp30.y = play->envCtx.windDirection.y + 100.0f;
    sp30.z = play->envCtx.windDirection.z * magnitude;
    this->unk_14C[2].unk_00 = 0x4000 - Math_Vec3f_Pitch(&gZeroVec3f, &sp30);
    this->unk_14C[2].unk_04 = Math_Vec3f_Yaw(&gZeroVec3f, &sp30) + 0x8000;
}

void func_808DC454(ObjectKankyo* this, PlayState* play) {
    s16 i;
    s32 pad1;
    f32 phi_f20;
    f32 spD0;
    f32 spCC;
    f32 spC8;
    f32 spC4;
    f32 spC0;
    f32 spBC;
    f32 temp_f0_4;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f28;
    f32 x = play->view.at.x - play->view.eye.x;
    f32 y = play->view.at.y - play->view.eye.y;
    f32 z = play->view.at.z - play->view.eye.z;
    f32 magnitude = sqrtf(SQ(x) + SQ(y) + SQ(z));
    f32 temp_120 = 120.0f;
    f32 temp_f30;
    Vec3f sp88;
    s32 pad;

    spD0 = x / magnitude;
    spCC = y / magnitude;
    spC8 = z / magnitude;

    for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_CUR]; i++) {
        switch (this->unk_14C[i].unk_1C) {
            case 0:
                this->unk_14C[i].unk_00 = play->view.eye.x + (spD0 * 120.0f);
                this->unk_14C[i].unk_04 = play->view.eye.y + (spCC * 120.0f);
                this->unk_14C[i].unk_08 = play->view.eye.z + (spC8 * 120.0f);
                this->unk_14C[i].unk_0C = (Rand_ZeroOne() - 0.5f) * (2.0f * temp_120);

                temp_f22 = (Camera_GetCamDirPitch(GET_ACTIVE_CAM(play)) * 0.004f) + 60.0f;
                if (temp_f22 < 20.0f) {
                    temp_f22 = 20.0f;
                }

                if (this->unk_114E == 0) {
                    this->unk_14C[i].unk_10 = temp_f22;
                } else {
                    this->unk_14C[i].unk_10 += temp_f22;
                    if (play->envCtx.precipitation[PRECIP_SNOW_CUR] == ((u32)i + 1)) {
                        this->unk_114E = 0;
                    }
                }

                this->unk_14C[i].unk_14 = (Rand_ZeroOne() - 0.5f) * (2.0f * temp_120);
                if (play->envCtx.precipitation[PRECIP_SOS_MAX] == 0) {
                    this->unk_14C[i].unk_18 = (Rand_ZeroOne() * 3.0f) + 1.0f;
                } else {
                    this->unk_14C[i].unk_18 = (Rand_ZeroOne() * 3.0f) + 8.0f;
                }
                this->unk_14C[i].unk_1C++;
                this->unk_14C[i].epoch++;
                break;

            case 1:
                temp_f24 = play->view.eye.x + (spD0 * 120.0f);
                temp_f28 = play->view.eye.y + (spCC * 120.0f);
                temp_f30 = play->view.eye.z + (spC8 * 120.0f);

                magnitude = sqrtf((f32)SQ(play->envCtx.windDirection.x) + SQ(play->envCtx.windDirection.y) +
                                  SQ(play->envCtx.windDirection.z));
                if (magnitude == 0.0f) {
                    magnitude = 0.001f;
                }
                spC4 = -play->envCtx.windDirection.x / magnitude;
                spC0 = -play->envCtx.windDirection.y / magnitude;
                spBC = -play->envCtx.windDirection.z / magnitude;

                if (i == 0) {
                    this->unk_144 += 0.049999997f * Rand_ZeroOne();
                    this->unk_148 += 0.049999997f * Rand_ZeroOne();
                }

                phi_f20 = play->envCtx.windSpeed / 120.0f;
                phi_f20 = CLAMP(phi_f20, 0.0f, 1.0f);

                this->unk_14C[i].unk_0C += sinf((this->unk_144 + (i * 100.0f)) * 0.01f) + (spC4 * 10.0f * phi_f20);
                this->unk_14C[i].unk_14 += cosf((this->unk_148 + (i * 100.0f)) * 0.01f) + (spBC * 10.0f * phi_f20);
                this->unk_14C[i].unk_10 -= this->unk_14C[i].unk_18 - (spC0 * 3.0f * (play->envCtx.windSpeed / 100.0f));

                temp_f22 = (-Camera_GetCamDirPitch(GET_ACTIVE_CAM(play)) * 0.012f) + 40.0f;
                if (temp_f22 < -40.0f) {
                    temp_f22 = -40.0f;
                }

                if (((this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C) - temp_f24) > temp_120) {
                    this->unk_14C[i].unk_00 = temp_f24 - temp_120;
                    // 2S2H [Interpolation] Here and below, skip particle interp on next frame when position moves
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C) - temp_f24) < -temp_120) {
                    this->unk_14C[i].unk_00 = temp_f24 + temp_120;
                    this->unk_14C[i].epoch++;
                }

                sp88.x = this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C;
                sp88.y = this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10;
                sp88.z = this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14;

                phi_f20 = Math_Vec3f_DistXZ(&sp88, &play->view.eye) / 200.0f;
                phi_f20 = CLAMP(phi_f20, 0.0f, 1.0f);
                temp_f0_4 = 100.0f + phi_f20 + 60.0f;

                if (temp_f0_4 < (this->unk_14C[i].unk_04 + (this->unk_14C[i].unk_10) - temp_f28)) {
                    this->unk_14C[i].unk_04 = temp_f28 - temp_f0_4;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) - temp_f28) < -temp_f0_4) {
                    this->unk_14C[i].unk_04 = temp_f28 + temp_f0_4;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14) - temp_f30) > temp_120) {
                    this->unk_14C[i].unk_08 = temp_f30 - temp_120;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14) - temp_f30) < -temp_120) {
                    this->unk_14C[i].unk_08 = temp_f30 + temp_120;
                    this->unk_14C[i].epoch++;
                }

                if ((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) < ((play->view.eye.y - temp_f22) - 40.0f)) {
                    this->unk_14C[i].unk_1C = 0;
                }
                break;
        }
    }
}

void func_808DCB7C(ObjectKankyo* this, PlayState* play) {
    if (play->envCtx.precipitation[PRECIP_SNOW_CUR] < play->envCtx.precipitation[PRECIP_SNOW_MAX]) {
        if ((play->state.frames % 16) == 0) {
            play->envCtx.precipitation[PRECIP_SNOW_CUR] += 2;
        }
    } else if (play->envCtx.precipitation[PRECIP_SNOW_MAX] < play->envCtx.precipitation[PRECIP_SNOW_CUR]) {
        if ((play->state.frames % 16) == 0) {
            play->envCtx.precipitation[PRECIP_SNOW_CUR] -= 2;
        }
    }
    func_808DC454(this, play);
}

void func_808DCBF8(ObjectKankyo* this, PlayState* play) {
    f32 temp_f0;

    if ((play->envCtx.precipitation[PRECIP_SNOW_CUR] > 0) && (this->unk_114C == 0)) {
        if ((play->state.frames % 16) == 0) {
            play->envCtx.precipitation[PRECIP_SNOW_CUR] -= 9;
            if ((s8)play->envCtx.precipitation[PRECIP_SNOW_CUR] < 0) {
                play->envCtx.precipitation[PRECIP_SNOW_CUR] = 0;
            }
        }
    }

    temp_f0 = (f32)play->envCtx.precipitation[PRECIP_SNOW_CUR] / 128;
    temp_f0 = CLAMP(temp_f0, 0.0f, 1.0f);

    if (temp_f0 > 0.01f) {
        D_801F4E30 = 155.0f * temp_f0;
        play->envCtx.sandstormState = SANDSTORM_A;
    } else {
        D_801F4E30 = 0;
        play->envCtx.sandstormState = SANDSTORM_A;
    }
    func_808DC454(this, play);
}

void func_808DCDB4(ObjectKankyo* this, PlayState* play) {
    s16 i;
    f32 magnitude;
    f32 temp_80;
    f32 temp_120;
    f32 spAC;
    f32 spA8;
    f32 spA4;
    f32 spA0;
    f32 sp9C;
    f32 x;
    f32 y;
    f32 z;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f26;
    f32 temp_f28;

    if (this->unk_114C < 0x80) {
        this->unk_114C++;
    }

    x = play->view.at.x - play->view.eye.x;
    y = play->view.at.y - play->view.eye.y;
    z = play->view.at.z - play->view.eye.z;

    magnitude = sqrtf(SQ(x) + SQ(y) + SQ(z));

    spAC = x / magnitude;
    spA8 = y / magnitude;
    spA4 = z / magnitude;

    temp_80 = 80.0f;
    temp_120 = 120.0f;

    for (i = 0; i < this->unk_114C; i++) {
        switch (this->unk_14C[i].unk_1C) {
            case 0:
                this->unk_14C[i].unk_00 = play->view.eye.x + (spAC * 120.0f);
                this->unk_14C[i].unk_04 = play->view.eye.y + (spA8 * 120.0f);
                this->unk_14C[i].unk_08 = play->view.eye.z + (spA4 * 120.0f);
                this->unk_14C[i].unk_0C = (Rand_ZeroOne() - 0.5f) * (temp_120 * 2.0f);
                if ((i % 2) == 0) {
                    this->unk_14C[i].unk_10 = -100.0f;
                } else {
                    this->unk_14C[i].unk_10 = 100.0f;
                }
                this->unk_14C[i].unk_14 = (Rand_ZeroOne() - 0.5f) * (temp_120 * 2.0f);
                this->unk_14C[i].unk_18 = Rand_ZeroOne() + 0.2f;
                this->unk_14C[i].unk_1C++;
                this->unk_14C[i].epoch++;
                break;

            case 1:

                temp_f26 = play->view.eye.x + (spAC * 120.0f);
                temp_f28 = play->view.eye.y + (spA8 * 120.0f);
                temp_f18 = play->view.eye.z + (spA4 * 120.0f);

                magnitude = sqrtf((f32)SQ(play->envCtx.windDirection.x) + SQ(play->envCtx.windDirection.y) +
                                  SQ(play->envCtx.windDirection.z));
                if (magnitude == 0.0f) {
                    magnitude = 0.001f;
                }

                spA0 = -play->envCtx.windDirection.x / magnitude;
                sp9C = -play->envCtx.windDirection.z / magnitude;

                if (i == 0) {
                    this->unk_144 += 0.049999997f * Rand_ZeroOne();
                    this->unk_148 += 0.049999997f * Rand_ZeroOne();
                }
                temp_f20 = play->envCtx.windSpeed / 120.0f;
                temp_f20 = CLAMP(temp_f20, 0.0f, 1.0f);

                this->unk_14C[i].unk_0C += sinf((this->unk_144 + i) * 0.01f) + (spA0 * 10.0f * temp_f20);
                this->unk_14C[i].unk_14 += cosf((this->unk_148 + i) * 0.01f) + (sp9C * 10.0f * temp_f20);

                if ((i % 2) == 0) {
                    this->unk_14C[i].unk_10 += this->unk_14C[i].unk_18;
                    if ((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) > (play->view.eye.y + 100.0f)) {
                        this->unk_14C[i].unk_1C = 0;
                    }
                } else {
                    this->unk_14C[i].unk_10 -= this->unk_14C[i].unk_18;
                    if ((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) < (play->view.eye.y - 100.0f)) {
                        this->unk_14C[i].unk_1C = 0;
                    }
                }

                if (((this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C) - temp_f26) > temp_80) {
                    this->unk_14C[i].unk_00 = temp_f26 - temp_80;
                    // 2S2H [Interpolation] Here and below, skip particle interp on next frame when position moves
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C) - temp_f26) < -temp_80) {
                    this->unk_14C[i].unk_00 = temp_f26 + temp_80;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) - temp_f28) > temp_80) {
                    this->unk_14C[i].unk_04 = temp_f28 - temp_80;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10) - temp_f28) < -temp_80) {
                    this->unk_14C[i].unk_04 = temp_f28 + temp_80;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14) - temp_f18) > temp_80) {
                    this->unk_14C[i].unk_08 = temp_f18 - temp_80;
                    this->unk_14C[i].epoch++;
                }

                if (((this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14) - temp_f18) < -temp_80) {
                    this->unk_14C[i].unk_08 = temp_f18 + temp_80;
                    this->unk_14C[i].epoch++;
                }
                break;
        }
    }
}

void ObjectKankyo_Update(Actor* thisx, PlayState* play) {
    ObjectKankyo* this = (ObjectKankyo*)thisx;

    this->actionFunc(this, play);
}

void ObjectKankyo_Draw(Actor* thisx, PlayState* play) {
    ObjectKankyo* this = (ObjectKankyo*)thisx;

    switch (this->actor.params) {
        case 0:
            func_808DDE9C(thisx, play);
            break;

        case 1:
        case 2:
        case 3:
            func_808DD3C8(thisx, play);
            break;

        case 4:
            func_808DD970(thisx, play);
            break;
    }
}

void func_808DD3C8(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjectKankyo* this = (ObjectKankyo*)thisx;
    Vec3f worldPos;
    Vec3f screenPos;
    s16 i;
    u8 pad2;
    u8 spB4;
    f32 temp_f0;
    u8 sp68;
    s32 pad;
    f32 temp_f2;
    f32 tempf;

    if ((play->cameraPtrs[CAM_ID_MAIN]->stateFlags & CAM_STATE_UNDERWATER) ||
        ((u8)play->envCtx.stormState == STORM_STATE_OFF)) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    spB4 = false;

    if (this->actor.params == 3) {
        temp_f0 = func_80173B48(&play->state) / 1.4e7f;
        temp_f0 = CLAMP(temp_f0, 0.0f, 1.0f);
        Math_SmoothStepToF(&D_808DE5B0, temp_f0, 0.2f, 0.1f, 0.001f);

        sp68 = play->envCtx.precipitation[PRECIP_SNOW_CUR];
        sp68 *= D_808DE5B0;

        if ((play->envCtx.precipitation[PRECIP_SNOW_CUR] >= 32) && (sp68 < 32)) {
            sp68 = 32;
        }
    } else {
        sp68 = play->envCtx.precipitation[PRECIP_SNOW_CUR];
    }

    for (i = 0; i < sp68; i++) {
        worldPos.x = this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C;
        worldPos.y = this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10;
        worldPos.z = this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14;

        Play_GetScreenPos(play, &worldPos, &screenPos);

        // #region 2S2H [Cosmetic] Increase snow render area for widescreen
        f32 xMin = 0.0f;
        f32 xMax = SCREEN_WIDTH;
        if (OTRGetAspectRatio() > 4.0f / 3.0f) {
            xMin = OTRGetDimensionFromLeftEdge(xMin);
            xMax = OTRGetDimensionFromRightEdge(xMax);
        }
        // #endregion

        if ((screenPos.x >= xMin) && (screenPos.x < xMax) && (screenPos.y >= 0.0f) && (screenPos.y < SCREEN_HEIGHT)) {
            FrameInterpolation_RecordOpenChild(&this->unk_14C[i], this->unk_14C[i].epoch);
            if (!spB4) {
                spB4 = true;

                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
                gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING);

                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);

                gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_CLD_SURF2);
                gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG);
                gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffDust5Tex));
            }

            Matrix_Translate(worldPos.x, worldPos.y, worldPos.z, MTXMODE_NEW);
            tempf = (i & 7) * 0.008f;
            Matrix_Scale(0.05f + tempf, 0.05f + tempf, 0.05f + tempf, MTXMODE_APPLY);
            temp_f2 = Math_Vec3f_DistXYZ(&worldPos, &play->view.eye) / 300.0f;
            temp_f2 = ((1.0f < temp_f2) ? 0.0f : (((1.0f - temp_f2) > 1.0f) ? 1.0f : 1.0f - temp_f2));

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(160.0f * temp_f2));

            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffDustDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808DD970(Actor* thisx, PlayState* play2) {
    f32 temp_f0;
    f32 temp_f20;
    Vec3f worldPos;
    Vec3f screenPos;
    f32 tempf;
    s16 i;
    f32 phi_f26;
    PlayState* play = play2;
    ObjectKankyo* this = (ObjectKankyo*)thisx;
    f32 tempA;

    if (play->sceneId == SCENE_KYOJINNOMA) {
        phi_f26 = 1.0f;
    } else {
        tempA = Camera_GetWaterYPos(GET_ACTIVE_CAM(play));
        if (tempA != BGCHECK_Y_MIN) {
            tempA -= play->view.eye.y;
            phi_f26 = tempA / 4000.0f;
        } else {
            phi_f26 = 0.0f;
        }

        phi_f26 = CLAMP_MAX(phi_f26, 1.0f);

        if (!(play->cameraPtrs[CAM_ID_MAIN]->stateFlags & CAM_STATE_UNDERWATER) || (phi_f26 == 0.0f)) {
            return;
        }
    }

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < this->unk_114C; i++) {
        worldPos.x = this->unk_14C[i].unk_00 + this->unk_14C[i].unk_0C;
        worldPos.y = this->unk_14C[i].unk_04 + this->unk_14C[i].unk_10;
        worldPos.z = this->unk_14C[i].unk_08 + this->unk_14C[i].unk_14;

        Play_GetScreenPos(play, &worldPos, &screenPos);

        // #region 2S2H [Cosmetic] Increase deep underwater dust render area for widescreen
        f32 xMin = 0.0f;
        f32 xMax = SCREEN_WIDTH;
        if (OTRGetAspectRatio() > 4.0f / 3.0f) {
            xMin = OTRGetDimensionFromLeftEdge(xMin);
            xMax = OTRGetDimensionFromRightEdge(xMax);
        }
        // #endregion

        if ((screenPos.x >= xMin) && (screenPos.x < xMax) && (screenPos.y >= 0.0f) && (screenPos.y < SCREEN_HEIGHT)) {
            FrameInterpolation_RecordOpenChild(&this->unk_14C[i], this->unk_14C[i].epoch);
            Matrix_Translate(worldPos.x, worldPos.y, worldPos.z, MTXMODE_NEW);
            Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);
            temp_f0 = Math_Vec3f_DistXYZ(&worldPos, &play->view.eye);
            temp_f0 = (u8)(255.0f * phi_f26) * (1.0f - (temp_f0 / 300.0f));

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 55, temp_f0);
            gDPSetEnvColor(POLY_XLU_DISP++, 55, 50, 255, temp_f0);

            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffDust5Tex));
            gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING);

            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);

            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_CLD_SURF2);
            gSPSetGeometryMode(POLY_XLU_DISP++, G_FOG);
            gSPDisplayList(POLY_XLU_DISP++, gEffDustDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

f32 func_808DDE74(void) {
    return Rand_ZeroOne() - 0.5f;
}

void func_808DDE9C(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjectKankyo* this = (ObjectKankyo*)thisx;
    Player* player = GET_PLAYER(play);
    s32 i;
    u8 phi_s5;
    u16 end = play->envCtx.precipitation[PRECIP_RAIN_CUR];
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f2;

    OPEN_DISPS(play->state.gfxCtx);

    if (end) {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 25);
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
    }

    for (i = 0; i < end; i++) {
        temp_f20 = this->unk_14C[0].unk_00 + ((Rand_ZeroOne() - 0.7f) * this->unk_144);
        temp_f22 = this->unk_14C[0].unk_04 + ((Rand_ZeroOne() - 0.7f) * this->unk_144);
        temp_f2 = this->unk_14C[0].unk_08 + ((Rand_ZeroOne() - 0.7f) * this->unk_144);

        if (!((temp_f20 < -252.0f) && (temp_f20 > -500.0f) && (temp_f2 > 3820.0f) && (temp_f2 < 4150.0f))) {
            FrameInterpolation_RecordOpenChild(this, i);
            Matrix_Translate(temp_f20, temp_f22, temp_f2, MTXMODE_NEW);

            gSPMatrix(POLY_XLU_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);

            Matrix_RotateYS(TRUNCF_BINANG(this->unk_14C[2].unk_04) + (s16)(i << 5), MTXMODE_APPLY);
            Matrix_RotateXS(TRUNCF_BINANG(this->unk_14C[2].unk_00) + (s16)(i << 5), MTXMODE_APPLY);

            if (this->unk_114C == 0) {
                Matrix_Scale(0.5f, 1.0f, 0.5f, MTXMODE_APPLY);
            } else {
                Matrix_Scale(2.0f, 4.0f, 2.0f, MTXMODE_APPLY);
            }

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gFallingRainDropDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    phi_s5 = false;
    if (player->actor.floorHeight < play->view.eye.y) {
        for (i = 0; i < end; i++) {
            if (!phi_s5) {
                Gfx_SetupDL25_Xlu(play->state.gfxCtx);

                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 255);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 100);
                phi_s5++;
            }

            temp_f20 = this->unk_14C[1].unk_00 + (func_808DDE74() * 220.0f);
            temp_f22 = player->actor.floorHeight + 2.0f;
            temp_f2 = this->unk_14C[1].unk_08 + (func_808DDE74() * 220.0f);

            if (!((temp_f20 < -252.0f) && (temp_f20 > -500.0f) && (temp_f2 > 3820.0f) && (temp_f2 < 4150.0f))) {
                FrameInterpolation_RecordOpenChild(this, i + end);
                Matrix_Translate(temp_f20, temp_f22, temp_f2, MTXMODE_NEW);
                temp_f12 = (Rand_ZeroOne() * 0.05f) + 0.05f;
                Matrix_Scale(temp_f12, temp_f12, temp_f12, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gEffShockwaveDL);
                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
