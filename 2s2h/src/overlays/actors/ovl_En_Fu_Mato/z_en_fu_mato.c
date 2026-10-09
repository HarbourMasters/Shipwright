/*
 * File: z_en_fu_mato.c
 * Overlay: ovl_En_Fu_Mato
 * Description: Honey & Darling's Shop - Target
 */

#include "z_en_fu_mato.h"
#include "overlays/actors/ovl_En_Fu/z_en_fu.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_fu_mato/object_fu_mato.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnFuMato_Init(Actor* thisx, PlayState* play);
void EnFuMato_Destroy(Actor* thisx, PlayState* play);
void EnFuMato_Update(Actor* thisx, PlayState* play);
void EnFuMato_Draw(Actor* thisx, PlayState* play);

void func_80ACE4B4(EnFuMato* this);
void func_80ACE4C8(EnFuMato* this, PlayState* play);
void func_80ACE508(EnFuMato* this);
void func_80ACE51C(EnFuMato* this, PlayState* play);
void func_80ACE718(EnFuMato* this, PlayState* play);
void func_80ACECFC(EnFuMato* this, PlayState* play);
void func_80ACEFC4(EnFuMato* this);
void func_80ACEFD8(EnFuMato* this, PlayState* play);

ActorProfile En_Fu_Mato_Profile = {
    /**/ ACTOR_EN_FU_MATO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FU_MATO,
    /**/ sizeof(EnFuMato),
    /**/ EnFuMato_Init,
    /**/ EnFuMato_Destroy,
    /**/ EnFuMato_Update,
    /**/ EnFuMato_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 200 }, 100 },
};

Gfx* D_80ACF63C[] = {
    object_fu_mato_DL_001C80, object_fu_mato_DL_001D68, object_fu_mato_DL_001E50,
    object_fu_mato_DL_001F38, object_fu_mato_DL_002020, object_fu_mato_DL_002108,
};

Vec2f D_80ACF654[] = {
    { -1.0f, -1.73f }, { 1.0f, -1.73f }, { 2.0f, 0.0f }, { 1.0f, 1.73f }, { -1.0f, 1.73f }, { -2.0f, 0.0f },
};

void EnFuMato_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFuMato* this = (EnFuMato*)thisx;
    CollisionHeader* sp2C = NULL;
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;
    EnFu* fu;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    CollisionHeader_GetVirtual(&object_fu_mato_Colheader_0023D4, &sp2C);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, sp2C);
    Actor_SetScale(&this->dyna.actor, 0.1f);
    Collider_InitSphere(play, &this->collider);
    Collider_SetSphere(play, &this->collider, &this->dyna.actor, &sSphereInit);
    this->collider.dim.worldSphere.radius = 20;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_FU) {
            this->dyna.actor.parent = actor;
            break;
        }
        actor = actor->next;
    }

    if (actor == NULL) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    fu = (EnFu*)this->dyna.actor.parent;
    if (fu->unk_542 == 2) {
        this->collider.elem.elemMaterial = ELEM_MATERIAL_UNK0;
        this->collider.dim.worldSphere.radius = 30;
    } else {
        this->collider.dim.worldSphere.radius = 17;
        this->unk_2F8 = fu->pathPoints;
        this->unk_2F4 = fu->unk_520;
        this->unk_2F0 = ENFUMATO_GET(&this->dyna.actor);
    }

    this->unk_302 = 0;
    this->unk_304 = 0;
    func_80ACE4B4(this);
}

void EnFuMato_Destroy(Actor* thisx, PlayState* play) {
    EnFuMato* this = (EnFuMato*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroySphere(play, &this->collider);
}

void func_80ACE4B4(EnFuMato* this) {
    this->actionFunc = func_80ACE4C8;
}

void func_80ACE4C8(EnFuMato* this, PlayState* play) {
    EnFu* fu = (EnFu*)this->dyna.actor.parent;

    if ((fu->unk_54A == 0) && (fu->unk_542 == 0)) {
        func_80ACE508(this);
    }
}

void func_80ACE508(EnFuMato* this) {
    this->actionFunc = func_80ACE51C;
}

void func_80ACE51C(EnFuMato* this, PlayState* play) {
    EnFu* fu = (EnFu*)this->dyna.actor.parent;
    Vec3f sp30;
    f32 sp2C;

    sp30.x = this->unk_2F8[this->unk_2F0].x;
    sp30.y = this->unk_2F8[this->unk_2F0].y;
    sp30.z = this->unk_2F8[this->unk_2F0].z;

    sp2C = Math_Vec3f_DistXZ(&this->dyna.actor.world.pos, &sp30);
    this->dyna.actor.world.rot.y = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &sp30);
    this->dyna.actor.world.rot.x = Math_Vec3f_Pitch(&this->dyna.actor.world.pos, &sp30);

    if (sp2C < 10.0f) {
        if (this->unk_2F0 < (this->unk_2F4 - 1)) {
            this->unk_2F0++;
        } else {
            this->unk_2F0 = 0;
        }
    }

    this->dyna.actor.speed = 2.0f;
    this->dyna.actor.shape.rot.y = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &this->dyna.actor.parent->world.pos);
    Actor_MoveWithGravity(&this->dyna.actor);

    if (fu->unk_54A != 0) {
        func_80ACE4B4(this);
    }
}

void func_80ACE680(EnFuMato* this) {
    this->dyna.actor.gravity = -0.5f;
    this->dyna.actor.velocity.y = 1.0f;
    this->dyna.actor.velocity.x = 2.0f * Rand_Centered();
    this->dyna.actor.velocity.z = 2.0f * Rand_Centered();
    this->unk_2FC.x = Rand_Next() & 0xFFF;
    this->unk_2FC.y = Rand_Next() & 0xFFF;
    this->unk_2FC.z = Rand_Next() & 0xFFF;
    this->unk_302 = 3;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WOODPLATE_BOUND);
    this->actionFunc = func_80ACE718;
}

void func_80ACE718(EnFuMato* this, PlayState* play) {
    this->dyna.actor.shape.rot.x += this->unk_2FC.x;
    this->dyna.actor.shape.rot.y += this->unk_2FC.y;
    this->dyna.actor.shape.rot.z += this->unk_2FC.z;
    this->dyna.actor.velocity.y += this->dyna.actor.gravity;

    Actor_UpdatePos(&this->dyna.actor);
    Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 15.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    if ((this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->dyna.actor.world.pos.y < -500.0f)) {
        Vec3f sp3C = { 0.0f, 0.0f, 0.0f };
        Vec3f sp30 = { 0.0f, 2.0f, 0.0f };

        func_800B3030(play, &this->dyna.actor.world.pos, &sp3C, &sp30, 20, 40, 2);
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80ACE850(EnFuMato* this, PlayState* play) {
    EnFuMatoStruct* ptr = this->unk_1B8;
    s16 temp_s4 = this->dyna.actor.shape.rot.y;
    s32 phi_s2;
    s32 i;

    this->unk_308 = Rand_Next() % ARRAY_COUNT(this->unk_1B8);
    this->unk_302 = 1;
    this->dyna.actor.gravity = -1.0f;
    this->dyna.actor.velocity.y = Rand_ZeroOne();

    phi_s2 = this->unk_308;

    for (i = 0; i < 2; i++) {
        ptr->unk_00 = this->dyna.actor.world.pos;

        ptr->unk_0C.x = Math_CosS(temp_s4) * D_80ACF654[phi_s2].x;
        ptr->unk_0C.y = D_80ACF654[phi_s2].z;
        ptr->unk_0C.z = -Math_SinS(temp_s4) * D_80ACF654[phi_s2].x;

        ptr->unk_0C.x += Rand_Centered();
        ptr->unk_0C.y += Rand_ZeroOne() * 0.5f;
        ptr->unk_0C.z += Rand_Centered();

        ptr->unk_18.x = 0.0f;
        ptr->unk_18.y = -1.0f;
        ptr->unk_18.z = 0.0f;

        ptr->unk_00.x += 9.0f * ptr->unk_0C.x;
        ptr->unk_00.y += 9.0f * ptr->unk_0C.y;
        ptr->unk_00.z += 9.0f * ptr->unk_0C.z;

        ptr->unk_24 = this->dyna.actor.shape.rot;

        ptr->unk_2A.x = (s16)Rand_Next() >> 6;
        ptr->unk_2A.y = (s16)Rand_Next() >> 6;
        ptr->unk_2A.z = (s16)Rand_Next() >> 6;
        ptr->unk_30 = 1;

        phi_s2 = (phi_s2 + 3) % ARRAY_COUNT(this->unk_1B8);
        ptr++;
    }

    this->dyna.actor.freezeTimer = 2;
    EffectSsHahen_SpawnBurst(play, &this->dyna.actor.world.pos, 13.0f, 0, 7, 8, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WOODPLATE_BROKEN);
    this->actionFunc = func_80ACECFC;
}

void func_80ACEB2C(EnFuMato* this) {
    EnFuMatoStruct sp18[2];
    s32 phi_v0 = this->unk_308;
    s32 i;
    s32 phi_v1;
    s32 phi_a1;

    sp18[0] = this->unk_1B8[0];
    sp18[1] = this->unk_1B8[1];

    for (i = 0; i < ARRAY_COUNT(sp18); i++) {
        if (phi_v0 == 0) {
            phi_v1 = 5;
        } else {
            phi_v1 = phi_v0 - 1;
        }

        if (phi_v0 == 5) {
            phi_a1 = 0;
        } else {
            phi_a1 = phi_v0 + 1;
        }

        this->unk_1B8[phi_v0] = sp18[i];
        this->unk_1B8[phi_v1] = sp18[i];
        this->unk_1B8[phi_a1] = sp18[i];

        phi_v0 = (phi_v0 + 3) % ARRAY_COUNT(this->unk_1B8);
    }

    this->unk_302 = 2;
    this->actionFunc = func_80ACECFC;
}

void func_80ACECFC(EnFuMato* this, PlayState* play) {
    EnFuMatoStruct* ptr = this->unk_1B8;
    s32 phi_s3;
    s32 i;

    this->dyna.actor.velocity.y += this->dyna.actor.gravity;
    Actor_UpdatePos(&this->dyna.actor);

    if (this->dyna.actor.world.pos.y < -500.0f) {
        this->dyna.actor.world.pos.y = -500.0f;
    }

    if (this->unk_302 == 1) {
        phi_s3 = 2;
    } else {
        phi_s3 = ARRAY_COUNT(this->unk_1B8);
    }

    for (i = 0; i < phi_s3; i++, ptr++) {
        ptr->unk_00.x += ptr->unk_0C.x;
        ptr->unk_00.y += ptr->unk_0C.y;
        ptr->unk_00.z += ptr->unk_0C.z;

        ptr->unk_0C.x += ptr->unk_18.x;
        ptr->unk_0C.y += ptr->unk_18.y;
        ptr->unk_0C.z += ptr->unk_18.z;

        ptr->unk_24.x += ptr->unk_2A.x;
        ptr->unk_24.y += ptr->unk_2A.y;
        ptr->unk_24.z += ptr->unk_2A.z;

        if ((this->unk_302 == 2) && (ptr->unk_30 == 1) && (ptr->unk_00.y < (this->dyna.actor.floorHeight + 10.0f))) {
            ptr->unk_0C.y = ABS_ALT(ptr->unk_0C.y);
            ptr->unk_0C.y *= 0.5f;
            ptr->unk_0C.x *= (Rand_Centered() * 1.5f) + 2.0f;
            ptr->unk_0C.z *= (Rand_Centered() * 1.5f) + 2.0f;

            ptr->unk_2A.x = (s16)Rand_Next() >> 5;
            ptr->unk_2A.y = (s16)Rand_Next() >> 5;
            ptr->unk_2A.z = (s16)Rand_Next() >> 5;

            ptr->unk_30 = 0;
        }
    }

    if (this->unk_302 == 1) {
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 15.0f, 30.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            func_80ACEB2C(this);
        }
    }

    if (this->unk_304 < 60) {
        this->unk_304++;
    } else {
        func_80ACEFC4(this);
    }
}

void func_80ACEFC4(EnFuMato* this) {
    this->actionFunc = func_80ACEFD8;
}

void func_80ACEFD8(EnFuMato* this, PlayState* play) {
    Vec3f* scale = &this->dyna.actor.scale;

    Math_SmoothStepToF(&scale->x, 0.0f, 0.1f, 0.005f, 0.005f);
    scale->y = scale->z = scale->x;
    if (scale->x == 0.0f) {
        Actor_Kill(&this->dyna.actor);
    }
}

s32 func_80ACF04C(EnFuMato* this, PlayState* play) {
    EnFu* fu = (EnFu*)this->dyna.actor.parent;

    this->collider.dim.worldSphere.center.x = this->dyna.actor.world.pos.x;
    this->collider.dim.worldSphere.center.y = this->dyna.actor.world.pos.y;
    this->collider.dim.worldSphere.center.z = this->dyna.actor.world.pos.z;

    if ((this->unk_302 == 0) && (((fu->unk_542 == 0) && (this->collider.base.acFlags & AC_HIT)) ||
                                 ((fu->unk_542 == 2) && (this->collider.base.ocFlags1 & OC1_HIT)))) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.ocFlags1 &= ~OC1_HIT;

        Actor_PlaySfx(&this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);

        fu->unk_548++;
        if ((fu->unk_542 == 2) || (GET_PLAYER_FORM == PLAYER_FORM_DEKU)) {
            fu->unk_546 = 1;
            func_80ACE680(this);
        } else {
            func_80ACE850(this, play);
        }
        return true;
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    return false;
}

void EnFuMato_Update(Actor* thisx, PlayState* play) {
    EnFuMato* this = (EnFuMato*)thisx;

    this->actionFunc(this, play);

    func_80ACF04C(this, play);

    if (this->unk_30A == 1) {
        func_80ACEFC4(this);
    }
}

void func_80ACF1F4(EnFuMato* this, PlayState* play) {
    EnFuMatoStruct* ptr = this->unk_1B8;
    s32 temp_s4 = this->unk_308;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < 2; i++) {
        Matrix_Push();
        Matrix_SetTranslateRotateYXZ(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, &ptr->unk_24);
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[temp_s4]);

        switch (temp_s4) {
            case 0:
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[5]);
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[temp_s4 + 1]);
                break;

            case 5:
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[temp_s4 - 1]);
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[0]);
                break;

            default:
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[temp_s4 - 1]);
                gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[temp_s4 + 1]);
                break;
        }

        Matrix_Pop();

        temp_s4 = (temp_s4 + 3) % ARRAY_COUNT(this->unk_1B8);
        ptr++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80ACF3F4(EnFuMato* this, PlayState* play) {
    EnFuMatoStruct* ptr = this->unk_1B8;
    Vec3f* scale = &this->dyna.actor.scale;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(D_80ACF63C); i++, ptr++) {
        Matrix_Push();
        Matrix_SetTranslateRotateYXZ(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, &ptr->unk_24);
        Matrix_Scale(scale->x, scale->y, scale->z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, D_80ACF63C[i]);

        Matrix_Pop();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFuMato_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFuMato* this = (EnFuMato*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    if ((this->unk_302 == 0) || (this->unk_302 == 3)) {
        gSPDisplayList(POLY_OPA_DISP++, object_fu_mato_DL_002720);
    } else if (this->unk_302 == 1) {
        func_80ACF1F4(this, play);
    } else {
        func_80ACF3F4(this, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
