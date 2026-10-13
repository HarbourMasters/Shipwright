/*
 * File: z_en_zow.c
 * Overlay: ovl_En_Zow
 * Description: Great Bay - Zora Complaining About Water
 */

#include "z_en_zow.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnZow_Init(Actor* thisx, PlayState* play);
void EnZow_Destroy(Actor* thisx, PlayState* play);
void EnZow_Update(Actor* thisx, PlayState* play);
void EnZow_Draw(Actor* thisx, PlayState* play);

void func_80BDD350(EnZow* this, PlayState* play);
void func_80BDD570(EnZow* this, PlayState* play);
void func_80BDD634(EnZow* this, PlayState* play);
void func_80BDD6BC(EnZow* this, PlayState* play);
void func_80BDD79C(EnZow* this, PlayState* play);

ActorProfile En_Zow_Profile = {
    /**/ ACTOR_EN_ZOW,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZO,
    /**/ sizeof(EnZow),
    /**/ EnZow_Init,
    /**/ EnZow_Destroy,
    /**/ EnZow_Update,
    /**/ EnZow_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

Vec3f D_80BDDD1C = { 0.0f, 1.0f, 0.0f };

Vec3f D_80BDDD28 = { 0.0f, -1.0f, 0.0f };

void func_80BDC270(EnZowStruct* ptr, Vec3f* arg1, f32 arg2, f32 arg3, u8 arg4) {
    s16 i;

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 == 0) {
            ptr->unk_00 = 1;
            ptr->unk_14 = *arg1;
            ptr->unk_04 = arg2;
            ptr->unk_08 = arg3;
            ptr->unk_0F = arg4;
            break;
        }
    }
}

void func_80BDC2D8(EnZow* this, EnZowStruct* ptr, Vec3f* arg2) {
    s16 i;
    f32 temp;

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++, ptr++) {
        if (ptr->unk_00 == 0) {
            temp = this->actor.world.pos.y + this->actor.depthInWater;
            if (temp <= arg2->y) {
                continue;
            }
            ptr->unk_00 = 3;
            ptr->unk_14 = *arg2;
            ptr->unk_20 = *arg2;
            ptr->unk_2C = D_80BDDD1C;
            ptr->unk_04 = ((Rand_ZeroOne() - 0.5f) * 0.02f) + 0.12f;
            break;
        }
    }
}

void func_80BDC3C0(EnZowStruct* ptr, Vec3f* arg1, Vec3f* arg2, f32 arg3) {
    s16 i;

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 != 2) {
            ptr->unk_00 = 2;
            ptr->unk_14 = *arg1;
            ptr->unk_20 = D_80BDDD28;
            ptr->unk_2C = *arg2;
            ptr->unk_0F = (Rand_ZeroOne() * 100.0f) + 100.0f;
            ptr->unk_04 = arg3;
            break;
        }
    }
}

void func_80BDC50C(EnZowStruct* ptr) {
    s16 i;

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 == 1) {
            Math_ApproachF(&ptr->unk_04, ptr->unk_08, 0.2f, 0.8f);

            if (ptr->unk_0F > 20) {
                ptr->unk_0F -= 20;
            } else {
                ptr->unk_0F = 0;
            }

            if (ptr->unk_0F == 0) {
                ptr->unk_00 = 0;
            }
        }
    }
}

void func_80BDC5C8(EnZow* this, EnZowStruct* ptr) {
    f32 temp_f2;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++, ptr++) {
        if (ptr->unk_00 == 3) {
            ptr->unk_14.x = ((Rand_ZeroOne() * 0.5f) - 0.25f) + ptr->unk_20.x;
            ptr->unk_14.z = ((Rand_ZeroOne() * 0.5f) - 0.25f) + ptr->unk_20.z;
            ptr->unk_14.y += ptr->unk_2C.y;

            temp_f2 = this->actor.world.pos.y + this->actor.depthInWater;
            if (temp_f2 <= ptr->unk_14.y) {
                ptr->unk_00 = 0;
                ptr->unk_14.y = temp_f2;
                func_80BDC270(ptr, &ptr->unk_14, 0.06f, 0.12f, 200);
            }
        }
    }
}

void func_80BDC6F8(EnZow* this, EnZowStruct* ptr) {
    s16 i;
    f32 temp_f0_2;

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++, ptr++) {
        if (ptr->unk_00 == 2) {
            ptr->unk_14.x += ptr->unk_2C.x;
            ptr->unk_14.y += ptr->unk_2C.y;
            ptr->unk_14.z += ptr->unk_2C.z;

            if (ptr->unk_2C.y >= -20.0f) {
                ptr->unk_2C.y += ptr->unk_20.y;
            } else {
                ptr->unk_2C.y = -20.0f;
                ptr->unk_20.y = 0.0f;
            }

            temp_f0_2 = this->actor.world.pos.y + this->actor.depthInWater;
            if (ptr->unk_14.y < temp_f0_2) {
                ptr->unk_00 = 0;
                ptr->unk_14.y = temp_f0_2;
                func_80BDC270(ptr, &ptr->unk_14, 0.06f, 0.12f, 200);
            }
        }
    }
}

void func_80BDC830(EnZowStruct* ptr, PlayState* play) {
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);
    u8 flag = false;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 != 1) {
            continue;
        }

        if (!flag) {
            gDPPipeSync(POLY_XLU_DISP++);
            gSPDisplayList(POLY_XLU_DISP++, gZoraRippleMaterialDL);
            gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
            flag = true;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, ptr->unk_0F);

        Matrix_Translate(ptr->unk_14.x, ptr->unk_14.y, ptr->unk_14.z, MTXMODE_NEW);
        Matrix_Scale(ptr->unk_04, 1.0f, ptr->unk_04, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gZoraRippleModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BDC9DC(EnZowStruct* ptr, PlayState* play) {
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);
    u8 flag = false;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 != 3) {
            continue;
        }

        if (!flag) {
            gSPDisplayList(POLY_XLU_DISP++, gZoraBubbleMaterialDL);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            flag = true;
        }

        Matrix_Translate(ptr->unk_14.x, ptr->unk_14.y, ptr->unk_14.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(ptr->unk_04, ptr->unk_04, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gZoraBubbleModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BDCB84(EnZowStruct* ptr, PlayState* play) {
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);
    u8 flag = false;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < 15; i++, ptr++) {
        if (ptr->unk_00 != 2) {
            continue;
        }

        if (!flag) {
            gSPDisplayList(POLY_XLU_DISP++, gZoraSplashMaterialDL);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
            flag = true;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, ptr->unk_0F);

        Matrix_Translate(ptr->unk_14.x, ptr->unk_14.y, ptr->unk_14.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(ptr->unk_04, ptr->unk_04, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gZoraSplashModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BDCD38(EnZow* this, EnZowStruct* ptr, f32 arg2, f32 arg3, u8 arg4) {
    static Vec3f D_80BDDD34 = { 0.0f, 0.0f, 0.0f };

    D_80BDDD34.x = this->actor.world.pos.x;
    D_80BDDD34.y = this->actor.world.pos.y + this->actor.depthInWater;
    D_80BDDD34.z = this->actor.world.pos.z;
    func_80BDC270(ptr, &D_80BDDD34, arg2, arg3, arg4);
}

void func_80BDCDA8(EnZow* this, EnZowStruct* ptr) {
    Vec3f sp84;
    Vec3f sp78;
    f32 temp_f20;
    f32 temp_f22;
    s32 i;

    for (i = 0; i < 10; i++) {
        temp_f20 = (Rand_ZeroOne() * 1.5f) + 0.5f;
        temp_f22 = Rand_ZeroOne() * 6.28f;

        sp78.y = (Rand_ZeroOne() * 3.0f) + 3.0f;
        sp78.x = sinf(temp_f22) * temp_f20;
        sp78.z = cosf(temp_f22) * temp_f20;

        sp84 = this->actor.world.pos;
        sp84.x += sp78.x * 6.0f;
        sp84.z += sp78.z * 6.0f;
        sp84.y += this->actor.depthInWater;

        func_80BDC3C0(ptr, &sp84, &sp78, 0.08f);
    }
}

typedef enum EnZowAnimation {
    /* -1 */ ENZOT_ANIM_NONE = -1,
    /*  0 */ ENZOT_ANIM_0,
    /*  1 */ ENZOT_ANIM_1,
    /*  2 */ ENZOT_ANIM_2,
    /*  3 */ ENZOT_ANIM_MAX,
} EnZowAnimation;

static AnimationHeader* sAnimations[ENZOT_ANIM_MAX] = {
    &gZoraTreadingWaterAnim, // ENZOT_ANIM_0
    &gZoraSurfacingAnim,     // ENZOT_ANIM_1
    &gZoraSurfacingAnim,     // ENZOT_ANIM_2
};

void EnZow_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZow* this = (EnZow*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = func_80BDD79C;
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraSkel, &gZoraIdleAnim, this->jointTable, this->morphTable,
                       ZORA_LIMB_MAX);
    Animation_PlayOnce(&this->skelAnime, &gZoraSurfacingAnim);
    this->animIndex = ENZOT_ANIM_1;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.shape.rot.z = 0;
    this->unk_2CA = 0;
    this->unk_2CC = 0;
    this->unk_2CE = 0;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.textId = 0;
    this->actor.world.rot.z = this->actor.shape.rot.z;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnZow_Destroy(Actor* thisx, PlayState* play) {
    EnZow* this = (EnZow*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnZow_ChangeAnim(EnZow* this, s16 animIndex, u8 animMode) {
    if ((animIndex > ENZOT_ANIM_NONE) && (animIndex < ENZOT_ANIM_MAX)) {
        if (animIndex <= ENZOT_ANIM_1) {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]), animMode, -5.0f);
        } else {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], -1.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]), 0.0f, animMode, 0.0f);
        }
        this->animIndex = animIndex;
    }
}

s32 func_80BDD154(EnZow* this, PlayState* play) {
    if (Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x3000) &&
        (this->actor.xzDistToPlayer < 170.0f) && (fabsf(this->actor.playerHeightRel) < 100.0f)) {
        return true;
    }
    return false;
}

void func_80BDD1E0(EnZow* this, PlayState* play) {
    u16 phi_a1;

    if (ENZOW_GET_F(&this->actor) == ENZOW_F_1) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
            if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_04)) {
                    phi_a1 = 0x12FD;
                } else {
                    phi_a1 = 0x12FA;
                    SET_WEEKEVENTREG(WEEKEVENTREG_78_04);
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_10)) {
                phi_a1 = 0x1301;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_78_10);
                phi_a1 = 0x12FF;
            }
        } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_08)) {
                phi_a1 = 0x12F8;
            } else {
                phi_a1 = 0x12F3;
                SET_WEEKEVENTREG(WEEKEVENTREG_78_08);
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_10)) {
            phi_a1 = 0x1301;
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_78_10);
            phi_a1 = 0x12FF;
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
            phi_a1 = 0x12EC;
        } else {
            phi_a1 = 0x12F1;
        }
    } else if (GET_PLAYER_FORM == PLAYER_FORM_ZORA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_78_02)) {
            phi_a1 = 0x12EB;
        } else {
            phi_a1 = 0x12E8;
            SET_WEEKEVENTREG(WEEKEVENTREG_78_02);
        }
    } else {
        phi_a1 = 0x12EF;
    }
    Message_StartTextbox(play, phi_a1, &this->actor);
}

void func_80BDD350(EnZow* this, PlayState* play) {
    if (this->unk_2CA & 2) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_WATER);
        func_80BDCDA8(this, this->unk_2D0);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->skelAnime.playSpeed = 0.0f;
        this->actor.velocity.y = -4.0f;
    }

    if (this->actor.depthInWater > 120.0f) {
        Math_ApproachF(&this->actor.velocity.y, 0.0f, 0.4f, 0.6f);
        if (this->actor.velocity.y > -0.1f) {
            this->actor.velocity.y = 0.0f;
            this->actionFunc = func_80BDD79C;
        }
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.depthInWater > 180.0f)) {
        this->actor.velocity.y = 0.0f;
        this->actionFunc = func_80BDD79C;
    }

    if (this->skelAnime.playSpeed <= 0.0f) {
        if (this->unk_2CE >= 6) {
            this->unk_2CE -= 5;
            return;
        } else {
            this->unk_2CE = 0;
        }
    }
}

void func_80BDD490(EnZow* this, PlayState* play) {
    this->actor.velocity.y = 0.0f;
    if (this->actor.xzDistToPlayer > 440.0f) {
        this->actionFunc = func_80BDD350;
        EnZow_ChangeAnim(this, ENZOT_ANIM_2, ANIMMODE_ONCE);
    } else if (this->unk_2CA & 2) {
        EnZow_ChangeAnim(this, ENZOT_ANIM_0, ANIMMODE_LOOP);
    }

    if ((play->gameplayFrames & 7) == 0) {
        func_80BDCD38(this, this->unk_2D0, 0.2f, 1.0f, 200);
    }

    if (this->unk_2CE < 245) {
        this->unk_2CE += 10;
    } else {
        this->unk_2CE = 255;
    }
}

void func_80BDD570(EnZow* this, PlayState* play) {
    func_80BDD490(this, play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x12E8:
                    case 0x12E9:
                    case 0x12EC:
                    case 0x12ED:
                    case 0x12EF:
                    case 0x12F1:
                    case 0x12F3:
                    case 0x12F4:
                    case 0x12F5:
                    case 0x12F6:
                    case 0x12F8:
                    case 0x12FA:
                    case 0x12FB:
                    case 0x12FD:
                    case 0x12FF:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    default:
                        Message_CloseTextbox(play);
                        break;
                }
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actionFunc = func_80BDD634;
            break;
    }
}

void func_80BDD634(EnZow* this, PlayState* play) {
    func_80BDD490(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80BDD570;
        func_80BDD1E0(this, play);
    } else if (func_80BDD154(this, play)) {
        Actor_OfferTalk(&this->actor, play, 180.0f);
    }
}

void func_80BDD6BC(EnZow* this, PlayState* play) {
    if (this->unk_2CE < 245) {
        this->unk_2CE += 10;
    } else {
        this->unk_2CE = 255;
    }

    if (this->actor.depthInWater < 54.0f) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER);
        func_80BDCDA8(this, this->unk_2D0);
        EnZow_ChangeAnim(this, ENZOT_ANIM_1, ANIMMODE_ONCE);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.velocity.y = 0.0f;
        this->actionFunc = func_80BDD634;
    } else if (this->actor.depthInWater < 80.0f) {
        Math_ApproachF(&this->actor.velocity.y, 2.0f, 0.4f, 0.6f);
    }
}

void func_80BDD79C(EnZow* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 400.0f) {
        this->actor.velocity.y = 4.0f;
        this->actionFunc = func_80BDD6BC;
    }

    if (this->unk_2CE >= 6) {
        this->unk_2CE -= 5;
    } else {
        this->unk_2CE = 0;
    }

    if (this->actor.depthInWater > 180.0f) {
        this->actor.world.pos.y = this->actor.world.pos.y + -180.0f + this->actor.depthInWater;
    }
}

void EnZow_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZow* this = (EnZow*)thisx;
    Vec3f sp34;

    Actor_MoveWithGravity(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 15.0f, 30.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    if (this->unk_2CE != 0) {
        this->unk_2CA &= ~2;
        if (SkelAnime_Update(&this->skelAnime)) {
            this->unk_2CA |= 2;
        }
    } else {
        this->unk_2CA |= 2;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x800, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    this->actionFunc(this, play);

    if (this->unk_2CE != 0) {
        if ((play->state.frames & 8) != 0) {
            sp34 = this->actor.world.pos;
            sp34.y += ((Rand_ZeroOne() - 0.5f) * 10.0f) + 18.0f;
            sp34.x += (Rand_ZeroOne() - 0.5f) * 28.0f;
            sp34.z += (Rand_ZeroOne() - 0.5f) * 28.0f;
            func_80BDC2D8(this, this->unk_2D0, &sp34);
        }

        if (DECR(this->unk_2C6) == 0) {
            this->unk_2C6 = Rand_S16Offset(60, 60);
        }

        this->unk_2C4 = this->unk_2C6;
        if (this->unk_2C4 >= 3) {
            this->unk_2C4 = 0;
        }
    }

    this->actor.shape.shadowAlpha = this->unk_2CE;
    func_80BDC50C(this->unk_2D0);
    func_80BDC5C8(this, this->unk_2D0);
    func_80BDC6F8(this, this->unk_2D0);
}

Gfx* func_80BDDA7C(GraphicsContext* gfxCtx) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    Gfx* gfx = gfxHead;

    gSPEndDisplayList(gfx++);

    return gfxHead;
}

Vec3f D_80BDDD4C = { 400.0f, 0.0f, 0.0f };

void EnZow_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == ZORA_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BDDD4C, &thisx->focus.pos);
    }
}

void EnZow_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnZow_PostLimbDrawOpa(play, limbIndex, dList, rot, thisx);
}

void EnZow_Draw(Actor* thisx, PlayState* play) {
    TexturePtr sp54[] = {
        gZoraEyeOpenTex,
        gZoraEyeHalfTex,
        gZoraEyeClosedTex,
    };
    EnZow* this = (EnZow*)thisx;

    Matrix_Push();

    func_80BDC830(this->unk_2D0, play);
    func_80BDC9DC(this->unk_2D0, play);
    func_80BDCB84(this->unk_2D0, play);

    Matrix_Pop();

    if (this->unk_2CE != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        if (this->unk_2CE >= 255) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sp54[this->unk_2C4]));
            gSPSegment(POLY_OPA_DISP++, 0x0C, func_80BDDA7C(play->state.gfxCtx));

            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, EnZow_PostLimbDrawOpa, &this->actor);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sp54[this->unk_2C4]));

            func_800BDAA0(play, &this->skelAnime, NULL, EnZow_PostLimbDraw, &this->actor, this->unk_2CE);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
