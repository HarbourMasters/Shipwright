/*
 * File: z_bg_breakwall.c
 * Overlay: ovl_Bg_Breakwall
 * Description: Great Bay Temple? (or the weather around it?)
 */

#include "z_bg_breakwall.h"
#include "objects/object_omoya_obj/object_omoya_obj.h"
#include "objects/object_yukimura_obj/object_yukimura_obj.h"
#include "objects/object_keikoku_obj/object_keikoku_obj.h"
#include "objects/object_posthouse_obj/object_posthouse_obj.h"
#include "objects/object_kumo30/object_kumo30.h"
#include "objects/object_mnk/object_mnk.h"
#include "objects/object_ikninside_obj/object_ikninside_obj.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"
#include "objects/object_spot11_obj/object_spot11_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgBreakwall_Init(Actor* thisx, PlayState* play);
void BgBreakwall_Update(Actor* thisx, PlayState* play);

void BgBreakwall_SetupAction(BgBreakwall* this, BgBreakwallActionFunc actionFunc);
bool func_808B736C(BgBreakwall* this, PlayState* play);
bool func_808B7380(BgBreakwall* this, PlayState* play);
bool func_808B73C4(BgBreakwall* this, PlayState* play);
bool func_808B73FC(BgBreakwall* this, PlayState* play);
bool func_808B7410(BgBreakwall* this, PlayState* play);
bool func_808B7460(BgBreakwall* this, PlayState* play);
bool func_808B74A8(BgBreakwall* this, PlayState* play);
bool func_808B74D8(BgBreakwall* this, PlayState* play);
bool func_808B751C(BgBreakwall* this, PlayState* play);
void func_808B76CC(BgBreakwall* this, PlayState* play);
void func_808B77D0(BgBreakwall* this, PlayState* play);
void func_808B77E0(BgBreakwall* this, PlayState* play);
void func_808B782C(BgBreakwall* this, PlayState* play);
void func_808B78A4(BgBreakwall* this, PlayState* play);
void func_808B78DC(BgBreakwall* this, PlayState* play);
void func_808B7914(BgBreakwall* this, PlayState* play);
void func_808B7A10(BgBreakwall* this, PlayState* play);
void func_808B7A90(Actor* thisx, PlayState* play);
void func_808B7B54(Actor* thisx, PlayState* play);
void func_808B7D34(Actor* thisx, PlayState* play);
void BgBreakwall_Draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Breakwall_Profile = {
    /**/ ACTOR_BG_BREAKWALL,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(BgBreakwall),
    /**/ BgBreakwall_Init,
    /**/ NULL,
    /**/ BgBreakwall_Update,
    /**/ NULL,
};

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ Gfx* unk_04;
    /* 0x08 */ Gfx* unk_08;
    /* 0x0C */ AnimatedMaterial* unk_0C;
    /* 0x10 */ CollisionHeader* unk_10;
    /* 0x14 */ BgBreakwallUnkFunc unk_14;
    /* 0x18 */ BgBreakwallUnkFunc2 unk_18;
    /* 0x1C */ ActorFunc unk_1C;
} BgBreakwallStruct; // size = 0x20

BgBreakwallStruct D_808B8140[] = {
    { OBJECT_OMOYA_OBJ, object_omoya_obj_DL_0001A0, NULL, NULL, NULL, func_808B7380, func_808B77D0, NULL },
    { OBJECT_YUKIMURA_OBJ, object_yukimura_obj_DL_000F98, object_yukimura_obj_DL_000EB0,
      object_yukimura_obj_Matanimheader_002090, NULL, func_808B73C4, func_808B77D0, NULL },
    { OBJECT_KEIKOKU_OBJ, object_keikoku_obj_DL_0027D8, NULL, object_keikoku_obj_Matanimheader_004290, NULL,
      func_808B73FC, func_808B77D0, NULL },
    { OBJECT_KEIKOKU_OBJ, object_keikoku_obj_DL_0044A8, NULL, NULL, NULL, func_808B736C, func_808B77D0, NULL },
    { OBJECT_POSTHOUSE_OBJ, object_posthouse_obj_DL_000A50, NULL, NULL, NULL, func_808B7410, func_808B77E0, NULL },
    { OBJECT_POSTHOUSE_OBJ, object_posthouse_obj_DL_001870, NULL, NULL, &object_posthouse_obj_Colheader_002948,
      func_808B7460, func_808B782C, NULL },
    { OBJECT_KUMO30, NULL, object_kumo30_DL_002A40, object_kumo30_Matanimheader_003368, NULL, func_808B74A8,
      func_808B78A4, func_808B7A90 },
    { OBJECT_KUMO30, NULL, NULL, object_kumo30_Matanimheader_002890, NULL, func_808B736C, func_808B78DC,
      func_808B7B54 },
    { OBJECT_MNK, object_mnk_DL_000F58, NULL, object_mnk_Matanimheader_002E28, &object_mnk_Colheader_0033E8,
      func_808B74D8, func_808B7914, NULL },
    { OBJECT_IKNINSIDE_OBJ, NULL, object_ikninside_obj_DL_00AB10, object_ikninside_obj_Matanimheader_00BCC8, NULL,
      func_808B751C, func_808B7A10, func_808B7D34 },
    { OBJECT_KAIZOKU_OBJ, object_kaizoku_obj_DL_00AD50, NULL, NULL, &object_kaizoku_obj_Colheader_00B868, func_808B736C,
      func_808B77D0, NULL },
    { OBJECT_IKNINSIDE_OBJ, NULL, object_ikninside_obj_DL_00AEC0, object_ikninside_obj_Matanimheader_00BCB8, NULL,
      func_808B751C, func_808B7A10, func_808B7D34 },
    { OBJECT_SPOT11_OBJ, gWoodStepDL, NULL, NULL, &gWoodStepCol, func_808B736C, func_808B77D0, NULL },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

Color_RGBA8 D_808B82F0[] = {
    { 255, 230, 240, 255 },
    { 255, 255, 255, 255 },
    { 255, 230, 220, 255 },
    { 80, 120, 160, 255 },
};

Color_RGBA8 D_808B8300[] = {
    { 140, 60, 70, 255 },
    { 50, 150, 150, 255 },
    { 140, 60, 50, 255 },
    { 30, 40, 160, 255 },
};

Color_RGBA8 D_808B8310[] = {
    { 210, 160, 180, 255 },
    { 120, 150, 160, 255 },
    { 210, 170, 160, 255 },
    { 70, 100, 120, 255 },
};

Color_RGBA8 D_808B8320[] = {
    { 255, 240, 255, 255 },
    { 255, 255, 255, 255 },
    { 255, 255, 240, 255 },
    { 150, 170, 180, 255 },
};

Color_RGBA8 D_808B8330[] = {
    { 60, 20, 40, 255 },
    { 0, 50, 50, 255 },
    { 60, 30, 30, 255 },
    { 0, 20, 30, 255 },
};

Color_RGBA8 D_808B8340[] = {
    { 160, 120, 140, 255 },
    { 100, 160, 150, 255 },
    { 160, 150, 120, 255 },
    { 60, 110, 110, 255 },
};

void BgBreakwall_SetupAction(BgBreakwall* this, BgBreakwallActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

bool func_808B736C(BgBreakwall* this, PlayState* play) {
    return true;
}

bool func_808B7380(BgBreakwall* this, PlayState* play) {
    if ((gSaveContext.save.day >= 2) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        return false;
    }
    return true;
}

bool func_808B73C4(BgBreakwall* this, PlayState* play) {
    return CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE) || CHECK_WEEKEVENTREG(WEEKEVENTREG_21_01);
}

bool func_808B73FC(BgBreakwall* this, PlayState* play) {
    return true;
}

bool func_808B7410(BgBreakwall* this, PlayState* play) {
    if (Flags_GetSwitch(play, this->switchFlag)) {
        this->dyna.actor.draw = NULL;
    }
    Actor_SetScale(&this->dyna.actor, 0.1f);
    return true;
}

bool func_808B7460(BgBreakwall* this, PlayState* play) {
    if (!Flags_GetSwitch(play, this->switchFlag)) {
        this->dyna.actor.scale.x = 0.1f;
    }
    return true;
}

bool func_808B74A8(BgBreakwall* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        return false;
    }
    return true;
}

bool func_808B74D8(BgBreakwall* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80) || CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
        return false;
    }
    return true;
}

bool func_808B751C(BgBreakwall* this, PlayState* play) {
    Actor_SetScale(&this->dyna.actor, 0.1f);

    if ((BGBREAKWALL_SWITCH_FLAG(&this->dyna.actor) != 0x7F) &&
        !Flags_GetSwitch(play, BGBREAKWALL_SWITCH_FLAG(&this->dyna.actor))) {
        return false;
    }

    func_800FEA50(play);

    if (gSaveContext.save.isNight) {
        this->unk_15E = 0;
    } else {
        this->unk_15E = 255;
    }
    return true;
}

void BgBreakwall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBreakwall* this = (BgBreakwall*)thisx;
    BgBreakwallStruct* sp24 = &D_808B8140[BGBREAKWALL_GET_F(&this->dyna.actor)];

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->objectSlot = Object_GetSlot(&play->objectCtx, sp24->objectId);

    if ((this->objectSlot <= OBJECT_SLOT_NONE) || !sp24->unk_14(this, play)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    BgBreakwall_SetupAction(this, func_808B76CC);
    this->switchFlag = BGBREAKWALL_SWITCH_FLAG(&this->dyna.actor);
}

void BgBreakwall_Destroy(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;
    BgBreakwallStruct* temp_s1 = &D_808B8140[BGBREAKWALL_GET_F(&this->dyna.actor)];

    if (temp_s1->unk_10 != NULL) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_808B76CC(BgBreakwall* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        BgBreakwallStruct* temp_s1 = &D_808B8140[BGBREAKWALL_GET_F(&this->dyna.actor)];

        this->dyna.actor.objectSlot = this->objectSlot;
        this->dyna.actor.draw = BgBreakwall_Draw;

        if (((BGBREAKWALL_GET_F(&this->dyna.actor)) != BGBREAKWALL_F_7) &&
            ((BGBREAKWALL_GET_F(&this->dyna.actor)) != BGBREAKWALL_F_9) &&
            ((BGBREAKWALL_GET_F(&this->dyna.actor)) != BGBREAKWALL_F_11)) {
            this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }

        Actor_SetObjectDependency(play, &this->dyna.actor);

        if (temp_s1->unk_0C != NULL) {
            temp_s1->unk_0C = Lib_SegmentedToVirtual(temp_s1->unk_0C);
        }

        if (temp_s1->unk_10 != NULL) {
            DynaPolyActor_Init(&this->dyna, 0);
            DynaPolyActor_LoadMesh(play, &this->dyna, temp_s1->unk_10);
        }

        BgBreakwall_SetupAction(this, temp_s1->unk_18);
        this->dyna.actor.destroy = BgBreakwall_Destroy;
    }
}

void func_808B77D0(BgBreakwall* this, PlayState* play) {
}

void func_808B77E0(BgBreakwall* this, PlayState* play) {
    if (!Flags_GetSwitch(play, this->switchFlag)) {
        this->dyna.actor.draw = BgBreakwall_Draw;
    } else {
        this->dyna.actor.draw = NULL;
    }
}

void func_808B782C(BgBreakwall* this, PlayState* play) {
    f32 phi_f0;

    if (Flags_GetSwitch(play, this->switchFlag)) {
        phi_f0 = 1.0f;
    } else {
        phi_f0 = 0.1f;
    }
    Math_SmoothStepToF(&this->dyna.actor.scale.x, phi_f0, 0.2f, 0.3f, 0.06f);
}

void func_808B78A4(BgBreakwall* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_808B78DC(BgBreakwall* this, PlayState* play) {
    Actor_SetScale(&this->dyna.actor, 3.5f);
    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_TORNADE - SFX_FLAG);
}

void func_808B7914(BgBreakwall* this, PlayState* play) {
    s32 pad;
    Vec3f sp30;
    Vec3f sp24;

    if ((play->gameplayFrames % 8) == 0) {
        sp30.x = Rand_ZeroFloat(80.0f) + (this->dyna.actor.world.pos.x - 55.0f);
        sp30.y = this->dyna.actor.world.pos.y + 70.0f;
        sp30.z = Rand_ZeroFloat(80.0f) + (this->dyna.actor.world.pos.z - 130.0f);

        sp24.x = 0.0f;
        sp24.y = 0.5f;
        sp24.z = 0.0f;

        EffectSsIceSmoke_Spawn(play, &sp30, &sp24, &gZeroVec3f, -200 - (s32)(Rand_ZeroOne() * 50.0f));
    }
}

void func_808B7A10(BgBreakwall* this, PlayState* play) {
    if (gSaveContext.save.isNight) {
        if (this->unk_15E < 245) {
            this->unk_15E += 10;
        } else {
            this->unk_15E = 255;
        }
    } else if (this->unk_15E > 10) {
        this->unk_15E -= 10;
    } else {
        this->unk_15E = 0;
    }
}

void BgBreakwall_Update(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;

    this->actionFunc(this, play);
}

void func_808B7A90(Actor* thisx, PlayState* play) {
    Color_RGBA8 sp2C;
    Color_RGBA8 sp28;

    OPEN_DISPS(play->state.gfxCtx);

    Environment_LerpSandstormColors(D_808B82F0, &sp2C);
    Environment_LerpSandstormColors(D_808B8300, &sp28);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sp2C.r, sp2C.g, sp2C.b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sp28.r, sp28.g, sp28.b, 255);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808B7B54(Actor* thisx, PlayState* play) {
    s32 pad;
    Color_RGBA8 sp50;
    Color_RGBA8 sp4C;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Environment_LerpSandstormColors(D_808B8310, &sp50);
    Environment_LerpSandstormColors(D_808B8330, &sp4C);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sp50.r, sp50.g, sp50.b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sp4C.r, sp4C.g, sp4C.b, 255);
    gSPDisplayList(POLY_XLU_DISP++, object_kumo30_DL_000A50);

    Environment_LerpSandstormColors(D_808B8320, &sp50);
    Environment_LerpSandstormColors(D_808B8340, &sp4C);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, sp50.r, sp50.g, sp50.b, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, sp4C.r, sp4C.g, sp4C.b, 255);
    gSPDisplayList(POLY_XLU_DISP++, object_kumo30_DL_000C98);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808B7D34(Actor* thisx, PlayState* play) {
    BgBreakwall* this = (BgBreakwall*)thisx;
    s32 sp48;
    s32 tempA;
    s32 tempB;
    s32 tempC;

    OPEN_DISPS(play->state.gfxCtx);

    sp48 = 255 - this->unk_15E;

    tempA = (((sp48 * 245) + (this->unk_15E * 255)) & 0xFF00) >> 8;
    tempB = (((sp48 * 145) + (this->unk_15E * 85)) & 0xFF00) >> 8;
    tempC = (((sp48 * 135) + (this->unk_15E * 55)) & 0xFF00) >> 8;

    gSPSegment(POLY_XLU_DISP++, 0x09, Gfx_PrimColor(play->state.gfxCtx, 255, 255, tempA, tempB, tempC));

    tempA = (((sp48 * 255) + (this->unk_15E * 150)) & 0xFF00) >> 8;
    tempB = (((sp48 * 255) + (this->unk_15E * 100)) & 0xFF00) >> 8;
    tempC = (((sp48 * 255) + (this->unk_15E * 75)) & 0xFF00) >> 8;

    gSPSegment(POLY_XLU_DISP++, 0x0A, Gfx_PrimColor(play->state.gfxCtx, 255, tempA, tempA, tempB, tempC));

    tempA = (((sp48 * 185) + (this->unk_15E * 140)) & 0xFF00) >> 8;
    tempB = (((sp48 * 235) + (this->unk_15E * 45)) & 0xFF00) >> 8;

    gSPSegment(POLY_XLU_DISP++, 0x0B, Gfx_PrimColor(play->state.gfxCtx, 255, 255, 255, tempA, tempB));

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgBreakwall_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBreakwall* this = (BgBreakwall*)thisx;
    BgBreakwallStruct* temp_s2 = &D_808B8140[BGBREAKWALL_GET_F(&this->dyna.actor)];

    OPEN_DISPS(play->state.gfxCtx);

    if (temp_s2->unk_0C != NULL) {
        AnimatedMat_Draw(play, temp_s2->unk_0C);
    }

    if (temp_s2->unk_1C != NULL) {
        temp_s2->unk_1C(&this->dyna.actor, play);
    }

    if (temp_s2->unk_04 != NULL) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, temp_s2->unk_04);
    }

    if (temp_s2->unk_08 != NULL) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, temp_s2->unk_08);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
