/*
 * File: z_obj_bigicicle.c
 * Overlay: ovl_Obj_Bigicicle
 * Description: Large icicles
 */

#include "z_obj_bigicicle.h"
#include "overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_bigicicle/object_bigicicle.h"

#define FLAGS 0x00000000

void ObjBigicicle_Init(Actor* thisx, PlayState* play);
void ObjBigicicle_Destroy(Actor* thisx, PlayState* play);
void ObjBigicicle_Update(Actor* thisx, PlayState* play);
void ObjBigicicle_Draw(Actor* thisx, PlayState* play);

void func_80AE8FD4(ObjBigicicle* this, PlayState* play);
void func_80AE9090(ObjBigicicle* this, PlayState* play);
void func_80AE9180(ObjBigicicle* this, PlayState* play);
void func_80AE9258(ObjBigicicle* this, PlayState* play);
void func_80AE939C(ObjBigicicle* this, PlayState* play);

ActorProfile Obj_Bigicicle_Profile = {
    /**/ ACTOR_OBJ_BIGICICLE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BIGICICLE,
    /**/ sizeof(ObjBigicicle),
    /**/ ObjBigicicle_Init,
    /**/ ObjBigicicle_Destroy,
    /**/ ObjBigicicle_Update,
    /**/ ObjBigicicle_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00003820, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 23, 68, -40, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 31, 90, -150, { 0, 0, 0 } },
};

AnimatedMaterial* D_80AE9878 = object_bigicicle_Matanimheader_001678;

Vec3f D_80AE987C = { 0.0f, -1.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(gravity, -2, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 5600, ICHAIN_STOP),
};

s32 D_80AE9890 = false;

Color_RGBA8 D_80AE9894 = { 250, 250, 250, 255 };
Color_RGBA8 D_80AE9898 = { 180, 180, 180, 255 };

Gfx* D_80AE989C[] = { object_bigicicle_DL_001D10, object_bigicicle_DL_002530, object_bigicicle_DL_002C20 };

Gfx* D_80AE98A8[] = {
    object_bigicicle_DL_000B20,
    object_bigicicle_DL_000D60,
    object_bigicicle_DL_000F40,
};

void ObjBigicicle_Init(Actor* thisx, PlayState* play) {
    ObjBigicicle* this = (ObjBigicicle*)thisx;
    f32 sp30;
    s32 sp28;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    sp28 = OBJBIGICLE_GET_FF00(&this->actor);
    if ((sp28 == 0) || (sp28 == 0xFF)) {
        sp28 = 0x3C;
    }
    Actor_SetScale(&this->actor, sp28 * 0.001f);

    this->actor.params = OBJBIGICLE_GET_SWITCH_FLAG_MASK(&this->actor);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    sp30 = sp28 * (1.0f / 60.0f);

    Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
    this->collider1.dim.radius *= sp30;
    this->collider1.dim.height *= sp30;
    this->collider1.dim.yShift *= sp30;

    Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);
    this->collider2.dim.radius *= sp30;
    this->collider2.dim.height *= sp30;
    this->collider2.dim.yShift *= sp30;

    if (Flags_GetSwitch(play, OBJBIGICLE_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    this->unk_148 = this->actor.shape.rot.x;
    this->actor.shape.rot.x = 0x4000;

    Collider_UpdateCylinder(&this->actor, &this->collider1);
    Collider_UpdateCylinder(&this->actor, &this->collider2);

    if (!D_80AE9890) {
        D_80AE9878 = Lib_SegmentedToVirtual(D_80AE9878);
        D_80AE9890 = true;
    }

    this->unk_149 = 0;
    this->actionFunc = func_80AE8FD4;
}

void ObjBigicicle_Destroy(Actor* thisx, PlayState* play) {
    ObjBigicicle* this = (ObjBigicicle*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

void func_80AE8DE4(ObjBigicicle* this, PlayState* play) {
    s32 i;
    Vec3f sp98;
    Vec3f sp8C;
    f32 temp_f20 = this->actor.scale.x * (50.0f / 3.0f);

    for (i = 0; i < 10; i++) {
        sp98.x = (Rand_CenteredFloat(4.0f) * temp_f20) + this->actor.world.pos.x;
        sp98.y = ((Rand_ZeroFloat(20.0f) - 35.0f) * temp_f20) + this->actor.world.pos.y;
        sp98.z = (Rand_CenteredFloat(4.0f) * temp_f20) + this->actor.world.pos.z;

        sp8C.x = Rand_CenteredFloat(13.0f);
        sp8C.y = Rand_ZeroFloat(2.0f);
        sp8C.z = Rand_CenteredFloat(13.0f);

        EffectSsHahen_Spawn(play, &sp98, &sp8C, &D_80AE987C, HAHEN_XLU, (Rand_ZeroFloat(20.0f) + 30.0f) * temp_f20,
                            OBJECT_BIGICICLE, 40, object_bigicicle_DL_0009B0);
    }

    sp98.x = this->actor.world.pos.x;
    sp98.y = this->actor.world.pos.y - (25.0f * temp_f20);
    sp98.z = this->actor.world.pos.z;

    SoundSource_PlaySfxAtFixedWorldPos(play, &sp98, 40, NA_SE_EV_ICE_PIECE);
}

void func_80AE8FD4(ObjBigicicle* this, PlayState* play) {
    if ((this->collider1.base.acFlags & AC_HIT) ||
        ((this->collider2.base.acFlags & AC_HIT) && (this->collider2.elem.acHitElem->atDmgInfo.dmgFlags & 0x3820))) {
        if ((this->unk_148 == 0) || (this->unk_149 == 1)) {
            CutsceneManager_Queue(this->actor.csId);
            this->actionFunc = func_80AE9090;
            return;
        }

        func_80AE8DE4(this, play);
        this->unk_149++;
        this->unk_14A = 50;
        this->actionFunc = func_80AE9180;
    }
}

void func_80AE9090(ObjBigicicle* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->unk_149++;
        func_80AE8DE4(this, play);
        if (this->unk_149 == 2) {
            f32 temp_f0 = this->actor.scale.y * 2100.0f;

            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.shape.yOffset = 2100.0f;
            this->actor.world.pos.y -= temp_f0;
            this->collider1.dim.yShift += TRUNCF_BINANG(temp_f0);
            this->collider2.dim.yShift += TRUNCF_BINANG(temp_f0);
            this->actionFunc = func_80AE9258;
        } else {
            this->unk_14A = 50;
            this->actionFunc = func_80AE9180;
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80AE9180(ObjBigicicle* this, PlayState* play) {
    this->unk_14A--;

    if (this->unk_14A > 0) {
        this->unk_14A--;
        this->actor.shape.rot.x = BINANG_ADD((s32)(Rand_CenteredFloat(0x580) * 0.02f * this->unk_14A), 0x4000);
        this->actor.shape.rot.z = Rand_CenteredFloat(0x580) * 0.02f * this->unk_14A;
    } else {
        this->actor.shape.rot.x = 0x4000;
        this->actor.shape.rot.z = 0;
        CutsceneManager_Stop(this->actor.csId);
        this->actionFunc = func_80AE8FD4;
    }
}

void func_80AE9258(ObjBigicicle* this, PlayState* play) {
    Actor* itemAction;
    f32 temp_f0;
    ObjIcePoly* icePoly;

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (itemAction != NULL) {
        if (itemAction->id == ACTOR_OBJ_ICE_POLY) {
            icePoly = (ObjIcePoly*)itemAction;
            temp_f0 = this->actor.world.pos.y - icePoly->actor.world.pos.y;
            if ((temp_f0 < icePoly->colliders1[0].dim.height) && (temp_f0 > 0.0f) &&
                (Actor_WorldDistXZToActor(&this->actor, &icePoly->actor) < icePoly->colliders1[0].dim.radius)) {
                Flags_SetSwitch(play, OBJBIGICLE_GET_SWITCH_FLAG(&this->actor));
                this->actionFunc = func_80AE939C;
                return;
            }
        }
        itemAction = itemAction->next;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actionFunc = func_80AE939C;
    } else {
        Collider_UpdateCylinder(&this->actor, &this->collider1);
        Collider_UpdateCylinder(&this->actor, &this->collider2);
    }
}

void func_80AE939C(ObjBigicicle* this, PlayState* play) {
    s32 i;
    Vec3f sp98;
    Vec3f sp8C;
    f32 temp_f20 = this->actor.scale.x * (50.0f / 3.0f);

    for (i = 0; i < 20; i++) {
        sp8C.x = Rand_CenteredFloat(15.0f);
        sp8C.y = Rand_ZeroFloat(7.0f) + 3.0f;
        sp8C.z = Rand_CenteredFloat(15.0f);

        sp98.x = this->actor.world.pos.x + (sp8C.x * 5.0f * temp_f20);
        sp98.y = this->actor.world.pos.y + (sp8C.y * 7.0f * temp_f20);
        sp98.z = this->actor.world.pos.z + (sp8C.z * 5.0f * temp_f20);

        EffectSsHahen_Spawn(play, &sp98, &sp8C, &D_80AE987C, HAHEN_XLU, (Rand_ZeroFloat(30.0f) + 40.0f) * temp_f20,
                            OBJECT_BIGICICLE, 30, object_bigicicle_DL_0009B0);
    }

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_GLASSBROKEN_IMPACT);
    CutsceneManager_Stop(this->actor.csId);
    Actor_Kill(&this->actor);
}

void ObjBigicicle_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjBigicicle* this = (ObjBigicicle*)thisx;
    Vec3f sp44;

    this->actionFunc(this, play);

    if (Rand_ZeroOne() < 0.05f) {
        sp44.x = (((215.0f + Rand_ZeroFloat(65.0f)) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1)) * this->actor.scale.x) +
                 this->actor.world.pos.x;
        sp44.y = ((((Rand_ZeroFloat(1400.0f) + -150.0f) - 2100.0f) + this->actor.shape.yOffset) * this->actor.scale.y) +
                 this->actor.world.pos.y;
        sp44.z = (((215.0f + Rand_ZeroFloat(65.0f)) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1)) * this->actor.scale.z) +
                 this->actor.world.pos.z;

        EffectSsKirakira_SpawnDispersed(play, &sp44, &gZeroVec3f, &gZeroVec3f, &D_80AE9894, &D_80AE9898, 2000, 5);
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
}

void ObjBigicicle_Draw(Actor* thisx, PlayState* play) {
    ObjBigicicle* this = (ObjBigicicle*)thisx;

    Gfx_DrawDListXlu(play, D_80AE989C[this->unk_149]);
    Gfx_DrawDListXlu(play, D_80AE98A8[this->unk_149]);
    AnimatedMat_Draw(play, D_80AE9878);
    Gfx_DrawDListXlu(play, object_bigicicle_DL_0014F0);
}
