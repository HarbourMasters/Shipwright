/*
 * File: z_en_syateki_niw.c
 * Overlay: ovl_En_Syateki_Niw
 * Description: Hopping Cucco
 */

#include "z_en_syateki_niw.h"
#include "objects/object_niw/object_niw.h"
#include "vt.h"
#include "soh/frame_interpolation.h"

#define FLAGS ACTOR_FLAG_4

void EnSyatekiNiw_Init(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Update(Actor* thisx, PlayState* play);
void EnSyatekiNiw_Draw(Actor* thisx, PlayState* play);

void func_80B11DEC(EnSyatekiNiw* this, PlayState* play);
void func_80B132A8(EnSyatekiNiw* this, PlayState* play);
void func_80B129EC(EnSyatekiNiw* this, PlayState* play);
void func_80B13464(EnSyatekiNiw* this, PlayState* play);
void func_80B123A8(EnSyatekiNiw* this, PlayState* play);
void func_80B11E78(EnSyatekiNiw* this, PlayState* play);
void func_80B12460(EnSyatekiNiw* this, PlayState* play);
void func_80B128D8(EnSyatekiNiw* this, PlayState* play);

void func_80B131B8(EnSyatekiNiw* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4);

const ActorInit En_Syateki_Niw_InitVars = {
    ACTOR_EN_SYATEKI_NIW,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_NIW,
    sizeof(EnSyatekiNiw),
    (ActorFunc)EnSyatekiNiw_Init,
    (ActorFunc)EnSyatekiNiw_Destroy,
    (ActorFunc)EnSyatekiNiw_Update,
    (ActorFunc)EnSyatekiNiw_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 10, 20, 4, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void EnSyatekiNiw_Init(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.flags &= ~ACTOR_FLAG_0;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gCuccoSkel, &gCuccoAnim, this->jointTable, this->morphTable, 16);

    this->unk_29E = this->actor.params;
    if (this->unk_29E < 0) {
        this->unk_29E = 0;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->unk_29E == 0) {
        osSyncPrintf("\n\n");
        // "Archery range chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 射的場鶏 ☆☆☆☆☆ \n" VT_RST);
        Actor_SetScale(&this->actor, 0.01f);
    } else {
        osSyncPrintf("\n\n");
        // "Bomb chicken"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ボムにわ！ ☆☆☆☆☆ \n" VT_RST);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        Actor_SetScale(&this->actor, 0.01f);
    }

    this->unk_2DC = this->actor.world.pos;
    this->unk_2E8 = this->actor.world.pos;
    this->actionFunc = func_80B11DEC;
}

void EnSyatekiNiw_Destroy(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80B11A94(EnSyatekiNiw* this, PlayState* play, s16 arg2) {
    if (this->unk_254 == 0) {
        if (arg2 == 0) {
            this->unk_264 = 0.0f;
        } else {
            this->unk_264 = -10000.0f;
        }

        this->unk_28E += 1;
        this->unk_254 = 3;
        if (!(this->unk_28E & 1)) {
            this->unk_264 = 0.0f;
            if (arg2 == 0) {
                this->unk_254 = Rand_ZeroFloat(30.0f);
            }
        }
    }

    if (this->unk_258 == 0) {
        this->unk_292++;
        this->unk_292 &= 1;
        switch (arg2) {
            case 0:
                this->unk_26C = 0.0f;
                this->unk_268 = 0.0f;
                break;

            case 1:
                this->unk_258 = 3;
                this->unk_26C = 7000.0f;
                this->unk_268 = 7000.0f;
                if (this->unk_292 == 0) {
                    this->unk_26C = 0.0f;
                    this->unk_268 = 0.0f;
                }
                break;

            case 2:
                this->unk_258 = 2;
                this->unk_268 = this->unk_26C = -10000.0f;
                this->unk_280 = this->unk_278 = 25000.0f;
                this->unk_284 = this->unk_27C = 6000.0f;
                if (this->unk_292 == 0) {
                    this->unk_278 = 8000.0f;
                    this->unk_280 = 8000.0f;
                }
                break;

            case 3:
                this->unk_258 = 2;
                this->unk_278 = 10000.0f;
                this->unk_280 = 10000.0f;
                if (this->unk_292 == 0) {
                    this->unk_278 = 3000.0f;
                    this->unk_280 = 3000.0f;
                }
                break;

            case 4:
                this->unk_254 = this->unk_256 = 5;
                break;

            case 5:
                this->unk_258 = 5;
                this->unk_278 = 14000.0f;
                this->unk_280 = 14000.0f;
                if (this->unk_292 == 0) {
                    this->unk_278 = 10000.0f;
                    this->unk_280 = 10000.0f;
                }
                break;
        }
    }

    if (this->unk_264 != this->unk_2BC.x) {
        Math_ApproachF(&this->unk_2BC.x, this->unk_264, 0.5f, 4000.0f);
    }

    if (this->unk_26C != this->unk_2A4.x) {
        Math_ApproachF(&this->unk_2A4.x, this->unk_26C, 0.8f, 7000.0f);
    }

    if (this->unk_280 != this->unk_2A4.y) {
        Math_ApproachF(&this->unk_2A4.y, this->unk_280, 0.8f, 7000.0f);
    }

    if (this->unk_284 != this->unk_2A4.z) {
        Math_ApproachF(&this->unk_2A4.z, this->unk_284, 0.8f, 7000.0f);
    }

    if (this->unk_268 != this->unk_2B0.x) {
        Math_ApproachF(&this->unk_2B0.x, this->unk_268, 0.8f, 7000.0f);
    }

    if (this->unk_278 != this->unk_2B0.y) {
        Math_ApproachF(&this->unk_2B0.y, this->unk_278, 0.8f, 7000.0f);
    }

    if (this->unk_27C != this->unk_2B0.z) {
        Math_ApproachF(&this->unk_2B0.z, this->unk_27C, 0.8f, 7000.0f);
    }
}

void func_80B11DEC(EnSyatekiNiw* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    if (this->unk_29E != 0) {
        Actor_SetScale(&this->actor, this->unk_2F4);
    }

    this->actionFunc = func_80B11E78;
}

void func_80B11E78(EnSyatekiNiw* this, PlayState* play) {
    Vec3f dustVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.2f, 0.0f };
    Color_RGBA8 dustPrimColor = { 0, 0, 0, 255 };
    Color_RGBA8 dustEnvColor = { 0, 0, 0, 255 };
    Vec3f dustPos;
    f32 tmpf2;
    f32 sp4C;
    f32 sp50;
    f32 tmpf1;
    s16 sp4A;

    if ((this->unk_29C != 0) && (this->unk_29E == 0) && (this->actor.bgCheckFlags & 1)) {
        this->unk_29C = 0;
        this->actionFunc = func_80B123A8;
        return;
    }

    sp4A = 0;
    if ((this->unk_25E == 0) && (this->unk_25C == 0)) {
        this->unk_294++;
        if (this->unk_294 >= 8) {
            this->unk_25E = Rand_ZeroFloat(30.0f);
            this->unk_294 = Rand_ZeroFloat(3.99f);

            switch (this->unk_29E) {
                case 0:
                    sp50 = Rand_CenteredFloat(100.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 100.0f;
                    } else {
                        sp50 += 100.0f;
                    }

                    sp4C = Rand_CenteredFloat(100.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 100.0f;
                    } else {
                        sp4C += 100.0f;
                    }

                    this->unk_2E8.x = this->unk_2DC.x + sp50;
                    this->unk_2E8.z = this->unk_2DC.z + sp4C;

                    if (this->unk_2E8.x < -150.0f) {
                        this->unk_2E8.x = -150.0f;
                    }

                    if (this->unk_2E8.x > 150.0f) {
                        this->unk_2E8.x = 150.0f;
                    }

                    if (this->unk_2E8.z < -60.0f) {
                        this->unk_2E8.z = -60.0f;
                    }

                    if (this->unk_2E8.z > -40.0f) {
                        this->unk_2E8.z = -40.0f;
                    }
                    break;

                case 1:
                    sp50 = Rand_CenteredFloat(50.0f);
                    if (sp50 < 0.0f) {
                        sp50 -= 50.0f;
                    } else {
                        sp50 += 50.0f;
                    }

                    sp4C = Rand_CenteredFloat(30.0f);
                    if (sp4C < 0.0f) {
                        sp4C -= 30.0f;
                    } else {
                        sp4C += 30.0f;
                    }

                    this->unk_2E8.x = this->unk_2DC.x + sp50;
                    this->unk_2E8.z = this->unk_2DC.z + sp4C;
                    break;
            }
        } else {
            this->unk_25C = 4;
            if (this->actor.bgCheckFlags & 1) {
                this->actor.velocity.y = 2.5f;
                if ((Rand_ZeroFloat(10.0f) < 1.0f) && (this->unk_29E == 0)) {
                    this->unk_25C = 0xC;
                    this->actor.velocity.y = 10.0f;
                }
            }
        }
    }
    if (this->unk_25C != 0) {
        sp4A = 1;
        Math_ApproachF(&this->actor.world.pos.x, this->unk_2E8.x, 1.0f, this->unk_2C8.y);
        Math_ApproachF(&this->actor.world.pos.z, this->unk_2E8.z, 1.0f, this->unk_2C8.y);
        Math_ApproachF(&this->unk_2C8.y, 3.0f, 1.0f, 0.3f);
        tmpf1 = this->unk_2E8.x - this->actor.world.pos.x;
        tmpf2 = this->unk_2E8.z - this->actor.world.pos.z;

        if (fabsf(tmpf1) < 10.0f) {
            tmpf1 = 0;
        }

        if (fabsf(tmpf2) < 10.0f) {
            tmpf2 = 0.0f;
        }

        if ((tmpf1 == 0.0f) && (tmpf2 == 0.0f)) {
            this->unk_25C = 0;
            this->unk_294 = 7;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, Math_FAtan2F(tmpf1, tmpf2) * (0x8000 / M_PI), 3, this->unk_2C8.z,
                           0);
        Math_ApproachF(&this->unk_2C8.z, 10000.0f, 1.0f, 1000.0f);
    }

    if (this->unk_260 == 0) {
        func_80B11A94(this, play, sp4A);
        return;
    }

    if ((play->gameplayFrames % 4) == 0) {
        dustVelocity.y = Rand_CenteredFloat(5.0f);
        dustAccel.y = 0.2f;
        dustPos = this->actor.world.pos;
        func_8002836C(play, &dustPos, &dustVelocity, &dustAccel, &dustPrimColor, &dustEnvColor, 600, 40, 30);
    }
}

void func_80B123A8(EnSyatekiNiw* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    this->unk_27C = 6000.0f;
    this->unk_288 = -10000.0f;
    this->unk_2B0.z = 6000.0f;
    this->unk_2B0.y = 10000.0f;
    this->actionFunc = func_80B12460;
    this->unk_2A4.z = 6000.0f;
    this->unk_284 = 6000.0f;
    this->unk_2B0.x = -10000.0f;
    this->unk_268 = -10000.0f;
    this->unk_2A4.y = -10000.0f;
    this->unk_2A4.x = -10000.0f;
    this->unk_26C = -10000.0f;
}

void func_80B12460(EnSyatekiNiw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 phi_f16 = 0.0f;

    player->actor.freezeTimer = 10;
    switch (this->unk_29A) {
        case 0:
            this->unk_296 = 2;
            this->unk_2C8.y = 0.0f;
            this->unk_29A = 1;
            break;

        case 1:
            this->actor.speedXZ = 2.0f;
            if (this->unk_25C == 0) {
                this->unk_25C = 3;
                this->actor.velocity.y = 3.5f;
            }

            if (this->unk_25A == 0) {
                this->unk_298++;
                this->unk_298 &= 1;
                this->unk_25A = 5;
            }

            phi_f16 = (this->unk_298 == 0) ? 5000.0f : -5000.0f;
            if (this->actor.world.pos.z > 100.0f) {
                this->actor.speedXZ = 2.0f;
                this->actor.gravity = -0.3f;
                this->actor.velocity.y = 5.0f;
                this->unk_29A = 2;
            }
            break;

        case 2:
            if ((player->actor.world.pos.z - 40.0f) < this->actor.world.pos.z) {
                this->actor.speedXZ = 0.0f;
            }

            if ((this->actor.bgCheckFlags & 1) && (this->actor.world.pos.z > 110.0f)) {
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                this->unk_284 = 0.0f;
                this->unk_27C = 0.0f;
                this->unk_278 = 0.0f;
                this->unk_280 = 0.0f;
                this->unk_288 = 0.0f;
                this->actor.speedXZ = 0.5f;
                this->unk_254 = this->unk_256 = 0;
                this->unk_28E = this->unk_290 = 0;
                this->unk_296 = 1;
                this->unk_29A = 3;
            }
            break;

        case 3:
            if ((player->actor.world.pos.z - 50.0f) < this->actor.world.pos.z) {
                this->actor.speedXZ = 0.0f;
                this->unk_262 = 0x3C;
                this->unk_25A = 0x14;
                this->unk_264 = 10000.0f;
                this->unk_29A = 4;
            }
            break;

        case 4:
            if (this->unk_25A == 0) {
                this->unk_296 = 4;
                this->unk_264 = 5000.0f;
                this->unk_26C = 0.0f;
                this->unk_268 = 0.0f;
                this->unk_284 = 0.0f;
                this->unk_27C = 0.0f;
                this->unk_280 = 14000.0f;
                this->unk_278 = 14000.0f;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
                this->unk_254 = this->unk_256 = this->unk_25A = 0x1E;
                this->unk_29A = 5;
            }
            break;

        case 5:
            if (this->unk_25A == 1) {
                this->unk_258 = 0;
                this->unk_296 = 5;
                this->unk_256 = this->unk_258;
                this->unk_254 = this->unk_258;
                this->actor.speedXZ = 1.0f;
            }

            if ((this->unk_25A == 0) && ((player->actor.world.pos.z - 30.0f) < this->actor.world.pos.z)) {
                Audio_PlaySoundGeneral(NA_SE_VO_LI_DOWN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                this->unk_25E = 0x14;
                this->unk_29A = 6;
                this->actor.speedXZ = 0.0f;
            }
            break;

        case 6:
            if (this->unk_25E == 1) {
                play->sceneLoadFlag = 0x14;
                play->nextEntranceIndex = gSaveContext.entranceIndex;
                play->shootingGalleryStatus = 0;
                player->actor.freezeTimer = 20;
                this->unk_25E = 0x14;
                this->actionFunc = func_80B128D8;
            }
            break;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y,
                       (s16)(Math_FAtan2F(player->actor.world.pos.x - this->actor.world.pos.x,
                                          player->actor.world.pos.z - this->actor.world.pos.z) *
                             (0x8000 / M_PI)) +
                           phi_f16,
                       5, this->unk_2C8.y, 0);
    Math_ApproachF(&this->unk_2C8.y, 3000.0f, 1.0f, 500.0f);
    if (this->unk_296 == 2) {
        this->unk_256 = 10;
        this->unk_254 = this->unk_256;
    }

    func_80B11A94(this, play, this->unk_296);
}

void func_80B128D8(EnSyatekiNiw* this, PlayState* play) {
    if (this->unk_25E == 1) {
        gSaveContext.timer1State = 0;
    }
}

void func_80B128F8(EnSyatekiNiw* this, PlayState* play) {
    s16 sp26;
    s16 sp24;

    Actor_SetFocus(&this->actor, this->unk_2D4);
    Actor_GetScreenPos(play, &this->actor, &sp26, &sp24);
    if ((this->actor.projectedPos.z > 200.0f) && (this->actor.projectedPos.z < 800.0f) && (sp26 > 0) &&
        (sp26 < SCREEN_WIDTH) && (sp24 > 0) && (sp24 < SCREEN_HEIGHT)) {
        this->actor.speedXZ = 5.0f;
        this->unk_298 = Rand_ZeroFloat(1.99f);
        this->unk_2D8 = Rand_CenteredFloat(8000.0f) + -10000.0f;
        this->unk_262 = 0x1E;
        this->unk_25E = 0x64;
        this->actionFunc = func_80B129EC;
    }
}

void func_80B129EC(EnSyatekiNiw* this, PlayState* play) {
    s32 pad;
    f32 phi_f2;
    s16 sp2E;
    s16 sp2C;
    f32 tmpf2;

    Actor_SetFocus(&this->actor, this->unk_2D4);
    Actor_GetScreenPos(play, &this->actor, &sp2E, &sp2C);
    if ((this->unk_25E == 0) || (this->actor.projectedPos.z < -70.0f) || (sp2E < 0) || (sp2E > SCREEN_WIDTH) ||
        (sp2C < 0) || (sp2C > SCREEN_HEIGHT)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->unk_2A0 = 1;
    if (this->unk_25C == 0) {
        this->unk_298++;
        this->unk_298 &= 1;
        this->unk_25C = (s16)Rand_CenteredFloat(4.0f) + 5;
        if ((Rand_ZeroFloat(5.0f) < 1.0f) && (this->actor.bgCheckFlags & 1)) {
            this->actor.velocity.y = 4.0f;
        }
    }

    phi_f2 = (this->unk_298 == 0) ? 5000.0f : -5000.0f;
    tmpf2 = this->unk_2D8 + phi_f2;
    Math_SmoothStepToS(&this->actor.world.rot.y, tmpf2, 3, this->unk_2C8.y, 0);
    Math_ApproachF(&this->unk_2C8.y, 3000.0f, 1.0f, 500.0f);
    func_80B11A94(this, play, 2);
}

void func_80B12BA4(EnSyatekiNiw* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        switch (this->unk_29E) {
            case 0:
                if (this->unk_29C == 0) {
                    this->unk_262 = 0x1E;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                    this->unk_29C = 1;
                    this->unk_2A0 = 1;
                    this->actionFunc = func_80B123A8;
                    this->actor.gravity = -3.0f;
                }
                break;

            case 1:
                this->unk_262 = 0x1E;
                this->unk_2F8 = 1;
                Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                this->unk_260 = 100;
                this->unk_2A0 = 1;
                this->unk_25E = this->unk_260;
                break;
        }
    }
}

void EnSyatekiNiw_Update(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    s32 pad;
    s16 i;
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;

    func_80B132A8(this, play);
    this->unk_28C++;
    if (this->unk_254 != 0) {
        this->unk_254--;
    }

    if (this->unk_258 != 0) {
        this->unk_258--;
    }

    if (this->unk_25A != 0) {
        this->unk_25A--;
    }

    if (this->unk_25C != 0) {
        this->unk_25C--;
    }

    if (this->unk_25E != 0) {
        this->unk_25E--;
    }

    if (this->unk_262 != 0) {
        this->unk_262--;
    }

    if (this->unk_260 != 0) {
        this->unk_260--;
    }

    this->actor.shape.rot = this->actor.world.rot;
    this->actor.shape.shadowScale = 15.0f;

    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, 0x1D);

    if (this->unk_2A0 != 0) {
        for (i = 0; i < 20; i++) {
            sp78.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
            sp78.y = Rand_CenteredFloat(10.0f) + (this->actor.world.pos.y + 20.0f);
            sp78.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;
            sp6C.x = Rand_CenteredFloat(3.0f);
            sp6C.y = (Rand_ZeroFloat(2.0f) * 0.5f) + 2.0f;
            sp6C.z = Rand_CenteredFloat(3.0f);
            sp60.z = sp60.x = 0.0f;
            sp60.y = -0.15f;
            func_80B131B8(this, &sp78, &sp6C, &sp60, Rand_ZeroFloat(8.0f) + 8.0f);
        }

        this->unk_2A0 = 0;
    }

    func_80B12BA4(this, play);
    if (this->unk_262 == 0) {
        if (this->actionFunc == func_80B11E78) {
            this->unk_262 = 0x12C;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_N);
        } else {
            this->unk_262 = 0x1E;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
        }
    }

    i = 0;
    switch (this->unk_29E) {
        case 0:
            if (play->shootingGalleryStatus != 0) {
                i = 1;
            }
            break;

        case 1:
            i = 1;
            break;
    }

    if (i != 0) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 SyatekiNiw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Vec3f sp0 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 13) {
        rot->y += (s16)this->unk_2BC.x;
    }

    if (limbIndex == 11) {
        rot->x += (s16)this->unk_2B0.z;
        rot->y += (s16)this->unk_2B0.y;
        rot->z += (s16)this->unk_2B0.x;
    }

    if (limbIndex == 7) {
        rot->x += (s16)this->unk_2A4.z;
        rot->y += (s16)this->unk_2A4.y;
        rot->z += (s16)this->unk_2A4.x;
    }

    return false;
}

void EnSyatekiNiw_Draw(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Color_RGBA8 sp30 = { 0, 0, 0, 255 };

    if (this->actionFunc != func_80B128F8) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        if (this->unk_260 != 0) {
            func_80026230(play, &sp30, 0, 0x14);
        }

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, SyatekiNiw_OverrideLimbDraw, NULL, this);
        func_80026608(play);
        func_80B13464(this, play);
    }
}

void func_80B131B8(EnSyatekiNiw* this, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, f32 arg4) {
    s16 i;
    EnSyatekiNiw_1* ptr = &this->unk_348[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 == 0) {
            ptr->epoch++;
            ptr->unk_00 = 1;
            ptr->unk_04 = *arg1;
            ptr->unk_10 = *arg2;
            ptr->unk_1C = *arg3;
            ptr->unk_34 = 0;
            ptr->unk_2C = (arg4 / 1000.0f);
            ptr->unk_28 = (s16)Rand_ZeroFloat(20.0f) + 0x28;
            ptr->unk_2A = Rand_ZeroFloat(1000.0f);
            return;
        }
    }
}

void func_80B132A8(EnSyatekiNiw* this, PlayState* play) {
    s16 i;
    EnSyatekiNiw_1* ptr = &this->unk_348[0];

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 != 0) {
            ptr->unk_04.x += ptr->unk_10.x;
            ptr->unk_04.y += ptr->unk_10.y;
            ptr->unk_04.z += ptr->unk_10.z;
            ptr->unk_34++;
            ptr->unk_10.x += ptr->unk_1C.x;
            ptr->unk_10.y += ptr->unk_1C.y;
            ptr->unk_10.z += ptr->unk_1C.z;
            if (ptr->unk_00 == 1) {
                ptr->unk_2A++;
                Math_ApproachF(&ptr->unk_10.x, 0.0f, 1.0f, 0.05f);
                Math_ApproachF(&ptr->unk_10.z, 0.0f, 1.0f, 0.05f);
                if (ptr->unk_10.y < -0.5f) {
                    ptr->unk_10.y = 0.5f;
                }

                ptr->unk_30 = (Math_SinS(ptr->unk_2A * 3000) * M_PI) * 0.2f;
                if (ptr->unk_28 < ptr->unk_34) {
                    ptr->unk_00 = 0;
                }
            }
        }
    }
}

void func_80B13464(EnSyatekiNiw* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    EnSyatekiNiw_1* ptr = &this->unk_348[0];
    u8 flag = 0;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    for (i = 0; i < 5; i++, ptr++) {
        if (ptr->unk_00 == 1) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherMaterialDL);
                flag++;
            }

            FrameInterpolation_RecordOpenChild(ptr, ptr->epoch);
            Matrix_Translate(ptr->unk_04.x, ptr->unk_04.y, ptr->unk_04.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(ptr->unk_2C, ptr->unk_2C, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZ(ptr->unk_30, MTXMODE_APPLY);
            Matrix_Translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}
