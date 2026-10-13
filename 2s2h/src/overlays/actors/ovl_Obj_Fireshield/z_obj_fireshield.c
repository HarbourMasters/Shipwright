/*
 * File: z_obj_fireshield.c
 * Overlay: ovl_Obj_Fireshield
 * Description: Ring of fire
 */

#include "z_obj_fireshield.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjFireshield_Init(Actor* thisx, PlayState* play);
void ObjFireshield_Destroy(Actor* thisx, PlayState* play);
void ObjFireshield_Update(Actor* thisx, PlayState* play);
void ObjFireshield_Draw(Actor* thisx, PlayState* play);

void func_80A4CABC(ObjFireshield* this);
void func_80A4CB7C(ObjFireshield* this);
void func_80A4CC54(ObjFireshield* this);
void func_80A4CCBC(ObjFireshield* this);
void func_80A4CD28(ObjFireshield* this);

ActorProfile Obj_Fireshield_Profile = {
    /**/ ACTOR_OBJ_FIRESHIELD,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjFireshield),
    /**/ ObjFireshield_Init,
    /**/ ObjFireshield_Destroy,
    /**/ ObjFireshield_Update,
    /**/ ObjFireshield_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 28, 144, 0, { 0, 0, 0 } },
};

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ f32 unk_04;
} ObjFireshieldStruct; // size = 0x8

ObjFireshieldStruct D_80A4D84C[] = {
    { 1.0f, 200.0f },
    { 2.0f, 400.0f },
    { 3.0f, 700.0f },
};

s32 D_80A4D864[] = { 0, 0, 0, 0 };
s32 D_80A4D874[] = { 0, 0, 0, 0 };
s32 D_80A4D884[] = { 0, 0, 0, 0 };
s32 D_80A4D894[] = { 0, 0, 0, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80A4CA90(ObjFireshield* this) {
    if (this->actor.csId > CS_ID_NONE) {
        this->actionFunc = func_80A4CABC;
    } else {
        this->actionFunc = func_80A4CC54;
    }
}

void func_80A4CABC(ObjFireshield* this) {
    s32 pad;

    if (CutsceneManager_IsNext(this->actor.csId)) {
        s32 sp18 = OBJFIRESHIELD_GET_FLAGS(&this->actor);
        s32 temp_a0 = (sp18 & ~0x1F) >> 5;

        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->actionFunc = func_80A4CB7C;
        this->unk_194 = 20;
        D_80A4D884[temp_a0] |= 1 << (sp18 & 0x1F);
        D_80A4D894[temp_a0] |= 1 << (sp18 & 0x1F);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80A4CB7C(ObjFireshield* this) {
    s32 pad;
    s32 sp18 = OBJFIRESHIELD_GET_FLAGS(&this->actor);

    if (this->unk_194 > 0) {
        if (this->unk_194 == 20) {
            D_80A4D884[(sp18 & ~0x1F) >> 5] &= ~(1 << (sp18 & 0x1F));
        }
        this->unk_194--;
        return;
    }

    CutsceneManager_Stop(this->actor.csId);
    this->actionFunc = func_80A4CD28;
    D_80A4D894[(sp18 & ~0x1F) >> 5] &= ~(1 << (sp18 & 0x1F));
}

void func_80A4CC54(ObjFireshield* this) {
    s32 temp_v0 = OBJFIRESHIELD_GET_FLAGS(&this->actor);
    s32 temp_v1 = 1 << (temp_v0 & 0x1F);

    this->actionFunc = func_80A4CCBC;
    D_80A4D884[(temp_v0 & ~0x1F) >> 5] |= temp_v1;
    D_80A4D894[(temp_v0 & ~0x1F) >> 5] |= temp_v1;
}

void func_80A4CCBC(ObjFireshield* this) {
    s32 temp_v0 = OBJFIRESHIELD_GET_FLAGS(&this->actor);

    this->actionFunc = func_80A4CD28;
    D_80A4D884[(temp_v0 & ~0x1F) >> 5] &= ~(1 << (temp_v0 & 0x1F));
    D_80A4D894[(temp_v0 & ~0x1F) >> 5] &= ~(1 << (temp_v0 & 0x1F));
}

void func_80A4CD28(ObjFireshield* this) {
}

void func_80A4CD34(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjFireshield* this = (ObjFireshield*)thisx;
    s32 isSwitchFlagSet = Flags_GetSwitch(play, OBJFIRESHIELD_GET_FLAGS(&this->actor));
    s32 phi_v1;
    s32 phi_a0;

    if (this->unk_1A8 == 0) {
        if (OBJFIRESHIELD_GET_2000(&this->actor)) {
            phi_v1 = false;
            phi_a0 = false;
        } else {
            phi_v1 = Flags_GetTreasure(play, OBJFIRESHIELD_GET_1F00(&this->actor));
            phi_a0 = false;
        }
    } else {
        phi_a0 = Flags_GetSwitch(play, OBJFIRESHIELD_GET_SWITCH_FLAG(&this->actor));
        phi_v1 = false;
    }

    if (phi_v1 || phi_a0) {
        this->unk_19C = 0;
    } else if (isSwitchFlagSet) {
        this->unk_19C = 0;
    } else {
        this->unk_19C = 1;
    }

    if (this->unk_19C == 0) {
        this->unk_198 = 0.0f;
    } else {
        this->unk_198 = 1.0f;
    }
}

void func_80A4CE28(ObjFireshield* this, PlayState* play) {
    s32 pad;
    s32 sp30 = OBJFIRESHIELD_GET_FLAGS(&this->actor);
    s32 pad2[2];
    s32 sp24;
    s32 sp20;

    if (this->unk_1A8 == 0) {
        if (OBJFIRESHIELD_GET_2000(&this->actor)) {
            sp24 = false;
            sp20 = false;
        } else {
            sp24 = Flags_GetTreasure(play, OBJFIRESHIELD_GET_1F00(&this->actor));
            sp20 = false;
        }
    } else {
        sp20 = Flags_GetSwitch(play, OBJFIRESHIELD_GET_SWITCH_FLAG(&this->actor));
        sp24 = false;
    }

    if (!sp24 || !sp20 || (this->unk_19C != 0)) {
        s32 sp1C = Flags_GetSwitch(play, sp30);
        s32 temp_v0 = (sp30 & ~0x1F) >> 5;

        if (this->unk_19C == 2) {
            if (Math_StepToF(&this->unk_198, 1.0f, 0.03f)) {
                this->unk_19C = 1;
                if (((this->actor.home.rot.z * 10) > 0) && (this->unk_1A7 != 0)) {
                    Flags_UnsetSwitch(play, sp30);
                    sp1C = false;
                }
            }
        } else if (this->unk_19C == 3) {
            if (Math_StepToF(&this->unk_198, 0.0f, 0.03f)) {
                this->unk_19C = 0;
            }
        }

        if ((this->unk_1A0 > 0) && (this->unk_19C == 1) && ((this->actor.home.rot.z * 10) > 0) &&
            !(D_80A4D894[temp_v0] & (1 << (sp30 & 0x1F)))) {
            this->unk_1A0 = 0;
        }

        if ((this->unk_19C == 0) || (this->unk_19C == 3)) {
            if (!sp24 && !sp20) {
                if ((this->actor.home.rot.z * 10) > 0) {
                    if (this->unk_1A0 > 1) {
                        this->unk_1A0--;
                    }

                    if (this->unk_1A0 == 1) {
                        this->unk_19C = 2;
                    }
                }

                if (!sp1C) {
                    this->unk_19C = 2;
                }
            }
        } else if (sp24 || sp20) {
            this->unk_19C = 3;
        } else if (sp1C && ((this->unk_19C == 1) || (this->unk_19C == 2))) {
            if ((D_80A4D884[temp_v0] & 1 << (sp30 & 0x1F)) && !(D_80A4D874[temp_v0] & 1 << (sp30 & 0x1F))) {
                this->unk_19C = 3;
            } else if (((this->actor.home.rot.z * 10) == 0) || (this->unk_1A0 == 0)) {
                if ((this->actor.home.rot.z * 10) > 0) {
                    this->unk_1A0 = this->actor.home.rot.z * 10;
                }

                if (this->unk_1A7 != 0) {
                    func_80A4CA90(this);
                }
            }
        }
    }
}

void func_80A4D174(ObjFireshield* this) {
    s32 temp_v0 = OBJFIRESHIELD_GET_FLAGS(&this->actor);
    s32 temp_v1 = 1 << (temp_v0 & 0x1F);

    if (!(D_80A4D864[(temp_v0 & ~0x1F) >> 5] & temp_v1)) {
        this->unk_1A7 = 1;
        D_80A4D864[(temp_v0 & ~0x1F) >> 5] |= temp_v1;
    }
}

void func_80A4D1CC(void) {
    D_80A4D864[0] = 0;
    D_80A4D864[1] = 0;
    D_80A4D864[2] = 0;
    D_80A4D864[3] = 0;
}

void ObjFireshield_Init(Actor* thisx, PlayState* play) {
    ObjFireshield* this = (ObjFireshield*)thisx;
    s32 temp = 0x8000;
    ObjFireshieldStruct* sp2C = &D_80A4D84C[OBJFIRESHIELD_GET_C000(&this->actor)];
    s32 sp28 = OBJFIRESHIELD_GET_ROTX(&this->actor);

    Actor_ProcessInitChain(&this->actor, sInitChain);

    if (sp28) {
        this->actor.shape.rot.z = temp;
    } else {
        this->actor.shape.rot.z = 0;
    }

    this->actor.world.rot.z = 0;
    this->actor.shape.rot.x = 0;
    this->actor.world.rot.x = 0;

    this->actor.scale.x = sp2C->unk_00 * 0.0348f;
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = 0.05f;

    this->actor.cullingVolumeScale = sp2C->unk_04;
    this->unk_1A4 = Rand_ZeroOne() * 128.0f;

    if ((this->actor.home.rot.z * 10) < 0) {
        this->unk_1A8 = 1;
        this->actor.home.rot.z = -thisx->home.rot.z;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->collider.dim.radius *= sp2C->unk_00;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    func_80A4D174(this);
    func_80A4CD34(&this->actor, play);
    this->actionFunc = func_80A4CD28;
}

void ObjFireshield_Destroy(Actor* thisx, PlayState* play) {
    ObjFireshield* this = (ObjFireshield*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjFireshield_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjFireshield* this = (ObjFireshield*)thisx;
    s32 sp44 = OBJFIRESHIELD_GET_ROTX(&this->actor);
    s32 sp40 = OBJFIRESHIELD_GET_FLAGS(&this->actor);
    s32 temp_a0;
    s32 temp_v1;

    func_80A4D1CC();

    if (this->unk_1A7 != 0) {
        temp_a0 = (sp40 & ~0x1F) >> 5;
        temp_v1 = 1 << (sp40 & 0x1F);
        if ((D_80A4D884[temp_a0] & temp_v1)) {
            D_80A4D874[temp_a0] |= temp_v1;
        } else {
            D_80A4D874[temp_a0] &= ~temp_v1;
        }
    }

    this->unk_1A4++;

    this->actionFunc(this);

    if (this->actionFunc == func_80A4CABC) {
        this->collider.base.atFlags &= ~AT_HIT;
    } else if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_800B8D98(play, thisx, 5.0f, thisx->yawTowardsPlayer, 1.0f);
    }

    func_80A4CE28(this, play);

    thisx->world.pos.y = ((sp44 ? 144.0f : -144.0f) * (1.0f - this->unk_198)) + thisx->home.pos.y;

    this->unk_1A6 = this->unk_198 * 255.0f;

    if (this->unk_198 >= 0.7f) {
        Player* player = GET_PLAYER(play);

        this->collider.dim.height = this->unk_198 * 80.0f;

        if (sp44) {
            this->collider.dim.yShift = (s32)(thisx->home.pos.y - thisx->world.pos.y) - this->collider.dim.height;
        } else {
            this->collider.dim.yShift = thisx->home.pos.y - thisx->world.pos.y;
        }

        Actor_PlaySfx_Flagged(thisx, NA_SE_EV_BURNING - SFX_FLAG);

        if (player->transformation == PLAYER_FORM_GORON) {
            this->collider.elem.atDmgInfo.damage = 0;
            this->collider.elem.atDmgInfo.effect = 0;
        } else {
            this->collider.elem.atDmgInfo.damage = 4;
            this->collider.elem.atDmgInfo.effect = 1;
        }

        Collider_UpdateCylinder(thisx, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void ObjFireshield_Draw(Actor* thisx, PlayState* play) {
    ObjFireshield* this = (ObjFireshield*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 220, 0, this->unk_1A6);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, this->unk_1A4 & 0x7F, 0, 0x20, 0x40, 1, 0,
                                  (this->unk_1A4 * -15) & 0xFF, 0x20, 0x40, 1, 0, 0, -15));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_02E510);

    CLOSE_DISPS(play->state.gfxCtx);
}
