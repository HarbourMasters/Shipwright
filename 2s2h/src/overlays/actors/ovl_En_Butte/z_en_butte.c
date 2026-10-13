/*
 * File: z_en_butte.c
 * Overlay: ovl_En_Butte
 * Description: Butterflies
 */

#include "z_en_butte.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0x00000000

void EnButte_Init(Actor* thisx, PlayState* play);
void EnButte_Destroy(Actor* thisx, PlayState* play2);
void EnButte_Update(Actor* thisx, PlayState* play);
void EnButte_Draw(Actor* thisx, PlayState* play);

void func_8091C748(EnButte* this);
void func_8091C794(EnButte* this, PlayState* play);
void func_8091CB68(EnButte* this);
void func_8091CBB4(EnButte* this, PlayState* play);
void func_8091CF64(EnButte* this);
void func_8091CFB4(EnButte* this, PlayState* play);
void func_8091D070(EnButte* this);
void func_8091D090(EnButte* this, PlayState* play);

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 5 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

ActorProfile En_Butte_Profile = {
    /**/ ACTOR_EN_BUTTE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(EnButte),
    /**/ EnButte_Init,
    /**/ EnButte_Destroy,
    /**/ EnButte_Update,
    /**/ EnButte_Draw,
};

typedef struct {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ s16 unk_10;
} EnButteStruct; // size = 0x14

EnButteStruct D_8091D324[] = {
    { 5, 35, 0.0f, 0.1f, 0.5f, 0 },
    { 10, 45, 1.1f, 0.1f, 0.25f, 1000 },
    { 10, 40, 1.5f, 0.1f, 0.3f, 2000 },
};

EnButteStruct D_8091D360[] = {
    { 3, 3, 0.8f, 0.1f, 0.2f, 0 },
    { 10, 20, 2.0f, 0.3f, 1.0f, 0 },
    { 10, 20, 2.4f, 0.3f, 1.0f, 0 },
};

f32 D_8091D39C = 0.0f;

s16 D_8091D3A0 = 0;

Vec3f D_8091D3A4 = { 0.0f, 0.0f, -3.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 60, ICHAIN_STOP),
};

f32 D_8091D3C0[] = { 50.0f, 80.0f, 100.0f };

f32 D_8091D3CC[] = { 30.0f, 40.0f, 50.0f };

f32 D_8091D3D8[] = { 15.0f, 20.0f, 25.0f };

f32 D_8091D3E4[] = { 7.5f, 10.0f, 12.5f };

s32 D_8091D3F0 = 1500;

void func_8091C0A0(EnButte* this, EnButteStruct* arg1) {
    if (this->unk_24E == 0) {
        if (Rand_ZeroOne() < 0.6f) {
            this->unk_24E = 1;
        } else {
            this->unk_24E = 2;
        }
    } else {
        this->unk_24E = 0;
    }
    this->unk_24C = Rand_S16Offset(arg1->unk_00, arg1->unk_02);
}

void func_8091C124(void) {
    D_8091D39C = 0.0f;
    D_8091D3A0 = 0;
}

void func_8091C140(void) {
    D_8091D39C += 0.003f;
    D_8091D3A0 += 0xFA0;
}

void func_8091C178(EnButte* this, PlayState* play) {
    Vec3f sp4C;
    s32 sp48;
    Vec3s sp40;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu2(play->state.gfxCtx);
    sp48 = Math_SinS(D_8091D3A0) * 250.0f;
    sp48 = CLAMP(sp48, 0, 255);

    sp40 = Camera_GetCamDir(GET_ACTIVE_CAM(play));
    Matrix_RotateYS(sp40.y, MTXMODE_NEW);
    Matrix_RotateXS(sp40.x, MTXMODE_APPLY);
    Matrix_RotateZS(sp40.z, MTXMODE_APPLY);
    Matrix_MultVec3f(&D_8091D3A4, &sp4C);
    Matrix_SetTranslateRotateYXZ(this->actor.focus.pos.x + sp4C.x, this->actor.focus.pos.y + sp4C.y,
                                 this->actor.focus.pos.z + sp4C.z, &sp40);
    Matrix_Scale(D_8091D39C, D_8091D39C, D_8091D39C, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 180, sp48);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 210, 255);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnButte_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnButte* this = (EnButte*)thisx;

    if (BUTTERFLY_GET(&this->actor) == BUTTERFLY_MINUS1) {
        this->actor.params = BUTTERFLY_0;
    }

    this->actor.world.rot.y = Rand_Next();
    this->actor.home.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Actor_ProcessInitChain(&this->actor, sInitChain);

    if ((BUTTERFLY_GET_1(&this->actor) & 0xFF) == BUTTERFLY_1) {
        this->actor.cullingVolumeScale = 200.0f;
    }

    SkelAnime_Init(play, &this->skelAnime, &gameplay_field_keep_Skel_002FA0, &gameplay_field_keep_Anim_001D20,
                   this->jointTable, this->morphTable, BUTTERFLY_LIMB_MAX);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colldierElements);

    this->collider.elements[0].dim.worldSphere.radius =
        this->collider.elements[0].dim.scale * this->collider.elements[0].dim.modelSphere.radius;

    this->actor.colChkInfo.mass = 0;
    this->unk_254 = Rand_ZeroOne() * 0xFFFF;
    this->unk_256 = Rand_ZeroOne() * 0xFFFF;
    this->unk_258 = Rand_ZeroOne() * 0xFFFF;

    Animation_Change(&this->skelAnime, &gameplay_field_keep_Anim_001D20, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
    func_8091C748(this);
    this->actor.shape.rot.x -= 0x2320;
    this->unk_250 = 1;
}

void EnButte_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnButte* this = (EnButte*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void func_8091C524(EnButte* this) {
    this->actor.shape.yOffset +=
        (Math_SinS(this->unk_254) * D_8091D3C0[this->unk_24E]) + (Math_SinS(this->unk_256) * D_8091D3CC[this->unk_24E]);
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -2000.0f, 2000.0f);
}

void func_8091C5EC(EnButte* this) {
    this->actor.shape.yOffset +=
        (Math_SinS(this->unk_254) * D_8091D3D8[this->unk_24E]) + (Math_SinS(this->unk_256) * D_8091D3E4[this->unk_24E]);
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -500.0f, 500.0f);
}

void func_8091C6B4(EnButte* this) {
    s16 temp_a1 = BINANG_ROT180(this->actor.world.rot.y);
    s16 temp_v0 = temp_a1 - this->actor.shape.rot.y;

    Math_ScaledStepToS(&this->actor.shape.rot.y, temp_a1, ABS_ALT(temp_v0) >> 3);
    this->actor.shape.rot.x = TRUNCF_BINANG(Math_SinS(this->unk_258) * 600.0f) - 0x2320;
}

void func_8091C748(EnButte* this) {
    func_8091C0A0(this, &D_8091D324[this->unk_24E]);
    this->actionFunc = func_8091C794;
}

void func_8091C794(EnButte* this, PlayState* play) {
    EnButteStruct* sp4C = &D_8091D324[this->unk_24E];
    f32 distSq;
    Player* player = GET_PLAYER(play);
    f32 distFromHomeSq = Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z, this->actor.home.pos.x,
                                         this->actor.home.pos.z);
    f32 playSpeed;
    f32 sp38;
    s32 pad;
    s16 sp32;
    s16 yaw;

    func_8091C524(this);
    Math_SmoothStepToF(&this->actor.speed, sp4C->unk_04, sp4C->unk_08, sp4C->unk_0C, 0.0f);

    if (this->unk_24F == 1) {
        distSq = SQ(100.0f);
        sp32 = 0x3E8;
    } else {
        distSq = SQ(35.0f);
        sp32 = 0x258;
    }

    sp38 = 0.0f;
    this->unk_25C = this->actor.home.pos.y;

    if ((this->unk_24E != 0) && ((distSq < distFromHomeSq) || (this->unk_24C < 4))) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        if (!Math_ScaledStepToS(&this->actor.world.rot.y, yaw, sp4C->unk_10)) {
            sp38 = 0.5f;
        }
    } else if ((this->unk_24F == 0) && (this->actor.child != NULL) && (&this->actor != this->actor.child)) {
        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.child->world.pos);
        if (!Math_ScaledStepToS(&this->actor.world.rot.y, yaw, sp32)) {
            sp38 = 0.3f;
        }
    } else if (this->unk_24F == 1) {
        if (!Math_ScaledStepToS(&this->actor.world.rot.y,
                                (s32)((Rand_ZeroOne() - 0.5f) * 0x6000) + this->actor.yawTowardsPlayer + 0x8000,
                                sp32)) {
            sp38 = 0.4f;
        }
    } else {
        this->actor.world.rot.y += TRUNCF_BINANG(Math_SinS(this->unk_254) * 100.0f);
    }

    func_8091C6B4(this);

    playSpeed = (((this->actor.speed * 0.5f) + (Rand_ZeroOne() * 0.2f)) + ((1.0f - Math_SinS(this->unk_258)) * 0.15f)) +
                ((1.0f - Math_SinS(this->unk_256)) * 0.3f) + sp38;
    this->skelAnime.playSpeed = CLAMP(playSpeed, 0.2f, 1.5f);

    SkelAnime_Update(&this->skelAnime);

    if (this->unk_24C <= 0) {
        func_8091C0A0(this, &D_8091D324[this->unk_24E]);
    }

    if ((BUTTERFLY_GET_1(&this->actor) == BUTTERFLY_1) && (player->heldItemAction == PLAYER_IA_DEKU_STICK) &&
        (this->unk_252 <= 0) &&
        ((Math3D_Dist2DSq(player->actor.world.pos.x, player->actor.world.pos.z, this->actor.home.pos.x,
                          this->actor.home.pos.z) < SQ(120.0f)) ||
         (this->actor.xzDistToPlayer < 60.0f))) {
        func_8091CB68(this);
        this->unk_24F = 2;
    } else if (this->actor.xzDistToPlayer < 120.0f) {
        this->unk_24F = 1;
    } else {
        this->unk_24F = 0;
    }
}

void func_8091CB68(EnButte* this) {
    func_8091C0A0(this, &D_8091D360[this->unk_24E]);
    this->actionFunc = func_8091CBB4;
}

void func_8091CBB4(EnButte* this, PlayState* play) {
    EnButteStruct* sp5C = &D_8091D360[this->unk_24E];
    Player* player = GET_PLAYER(play);
    f32 playSpeed;
    Vec3f sp48;
    f32 distSq;
    f32 sp40;
    s32 pad;
    s16 pad2;
    s16 yaw;

    func_8091C5EC(this);
    Math_SmoothStepToF(&this->actor.speed, sp5C->unk_04, sp5C->unk_08, sp5C->unk_0C, 0.0f);
    sp40 = 0.0f;

    if ((this->unk_24E != 0) && (this->unk_24C < 12)) {
        sp48.x = player->meleeWeaponInfo[0].tip.x + (Math_SinS(player->actor.shape.rot.y) * 10.0f);
        sp48.y = player->meleeWeaponInfo[0].tip.y;
        sp48.z = player->meleeWeaponInfo[0].tip.z + (Math_CosS(player->actor.shape.rot.y) * 10.0f);

        yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &sp48);
        if (Math_ScaledStepToS(&this->actor.world.rot.y, yaw + (s32)(Rand_ZeroOne() * D_8091D3F0), 0x7D0)) {
            if ((play->gameplayFrames & 0x30) == 0x30) {
                this->actor.world.rot.y += TRUNCF_BINANG(Math_SinS(this->unk_254) * 60.0f);
            }
        } else {
            sp40 = 0.3f;
        }
    }

    if (player->meleeWeaponInfo[0].tip.y < player->actor.world.pos.y + 30.0f) {
        this->unk_25C = player->actor.world.pos.y + 30.0f;
    } else {
        this->unk_25C = player->meleeWeaponInfo[0].tip.y;
    }

    func_8091C6B4(this);

    playSpeed = ((this->actor.speed * 0.5f) + (Rand_ZeroOne() * 0.2f) + ((1.0f - Math_SinS(this->unk_258)) * 0.15f)) +
                ((1.0f - Math_SinS(this->unk_256)) * 0.3f) + sp40;
    this->skelAnime.playSpeed = CLAMP(playSpeed, 0.2f, 1.5f);
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_24C <= 0) {
        func_8091C0A0(this, &D_8091D360[this->unk_24E]);
        D_8091D3F0 = -D_8091D3F0;
    }

    distSq = Math3D_Dist2DSq(this->actor.world.pos.x, this->actor.world.pos.z, this->actor.home.pos.x,
                             this->actor.home.pos.z);
    if ((player->heldItemAction != PLAYER_IA_DEKU_STICK) || !(fabsf(player->actor.speed) < 1.8f) ||
        (this->unk_252 > 0) || !(distSq < SQ(320.0f))) {
        func_8091C748(this);
    } else if ((distSq > SQ(240.0f)) &&
               (Math3D_Dist2DSq(player->meleeWeaponInfo[0].tip.x, player->meleeWeaponInfo[0].tip.z,
                                this->actor.world.pos.x, this->actor.world.pos.z) < SQ(60.0f))) {
        func_8091CF64(this);
    }
}

void func_8091CF64(EnButte* this) {
    this->unk_24C = 9;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->skelAnime.playSpeed = 1.0f;
    func_8091C124();
    this->actionFunc = func_8091CFB4;
}

void func_8091CFB4(EnButte* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    func_8091C140();

    if (this->unk_24C == 5) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 60, NA_SE_EV_BUTTERFRY_TO_FAIRY);
    } else if (this->unk_24C == 4) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.focus.pos.x, this->actor.focus.pos.y,
                    this->actor.focus.pos.z, 0, this->actor.shape.rot.y, 0, FAIRY_PARAMS(FAIRY_TYPE_2, false, 0));
        this->unk_250 = 0;
    } else if (this->unk_24C <= 0) {
        func_8091D070(this);
    }
}

void func_8091D070(EnButte* this) {
    this->unk_24C = 64;
    this->actionFunc = func_8091D090;
    this->actor.draw = NULL;
}

void func_8091D090(EnButte* this, PlayState* play) {
    if (this->unk_24C <= 0) {
        Actor_Kill(&this->actor);
    }
}

void EnButte_Update(Actor* thisx, PlayState* play) {
    EnButte* this = (EnButte*)thisx;

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (&this->actor != this->actor.child)) {
        this->actor.child = NULL;
    }

    if (this->unk_24C > 0) {
        this->unk_24C--;
    }

    this->unk_254 += 0x222;
    this->unk_256 += 0x1000;
    this->unk_258 += 0x600;

    if (BUTTERFLY_GET_1(&this->actor) == BUTTERFLY_1) {
        if (GET_PLAYER(play)->meleeWeaponState == PLAYER_MELEE_WEAPON_STATE_0) {
            if (this->unk_252 > 0) {
                this->unk_252--;
            }
        } else {
            this->unk_252 = 80;
        }
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        Actor_MoveWithGravity(&this->actor);
        Math_StepToF(&this->actor.world.pos.y, this->unk_25C, 0.6f);
        if (this->actor.xyzDistToPlayerSq < 5000.0f) {
            ColliderJntSphElement* jntSphElem = &this->collider.elements[0];

            jntSphElem->dim.worldSphere.center.x = this->actor.world.pos.x;
            jntSphElem->dim.worldSphere.center.y = this->actor.world.pos.y;
            jntSphElem->dim.worldSphere.center.z = this->actor.world.pos.z;
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
        Actor_SetFocus(&this->actor, this->actor.shape.yOffset * this->actor.scale.y);
    }
}

void EnButte_Draw(Actor* thisx, PlayState* play) {
    EnButte* this = (EnButte*)thisx;

    if (this->unk_250 != 0) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
    }

    if ((BUTTERFLY_GET_1(&this->actor) == BUTTERFLY_1) && (this->actionFunc == func_8091CFB4)) {
        func_8091C178(this, play);
    }
}
