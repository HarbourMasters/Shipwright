/*
 * File: z_obj_skateblock.c
 * Overlay: ovl_Obj_Skateblock
 * Description: Pushable block that slides on ice
 */

#include "z_obj_skateblock.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjSkateblock_Init(Actor* thisx, PlayState* play);
void ObjSkateblock_Destroy(Actor* thisx, PlayState* play);
void ObjSkateblock_Update(Actor* thisx, PlayState* play);
void ObjSkateblock_Draw(Actor* thisx, PlayState* play);

void func_80A22308(ObjSkateblock* this);
void func_80A22334(ObjSkateblock* this, PlayState* play);
void func_80A2244C(ObjSkateblock* this);
void func_80A224A4(ObjSkateblock* this, PlayState* play);
void func_80A22628(ObjSkateblock* this);
void func_80A2264C(ObjSkateblock* this, PlayState* play);
void func_80A22728(ObjSkateblock* this);
void func_80A2273C(ObjSkateblock* this, PlayState* play);
void func_80A227A4(ObjSkateblock* this);
void func_80A227C0(ObjSkateblock* this, PlayState* play);

ActorProfile Obj_Skateblock_Profile = {
    /**/ ACTOR_OBJ_SKATEBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(ObjSkateblock),
    /**/ ObjSkateblock_Init,
    /**/ ObjSkateblock_Destroy,
    /**/ ObjSkateblock_Update,
    /**/ ObjSkateblock_Draw,
};

s32 D_80A22A10 = 0;

s32 D_80A22A14 = 0;

AnimatedMaterial* D_80A22A18 = NULL;

Vec2f D_80A22A1C[] = {
    { -300.0f, 300.0f }, { 300.0f, 300.0f }, { -300.0f, -300.0f }, { 300.0f, -300.0f }, { 0.0f, 0.0f },
};

Vec2f D_80A22A44[] = {
    { 0.5f, -0.5f }, { -0.5f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f }, { 0.0f, 0.0f },
};

Vec2f D_80A22A6C[] = {
    { -300.0f, 0.0f }, { 300.0f, 0.0f }, { -300.0f, 600.0f }, { 300.0f, 600.0f }, { 0.0f, 300.0f },
};

Vec2f D_80A22A94[] = {
    { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, 0.0f },
};

f32 D_80A22ABC[] = { -300.0f, 300.0f };

f32 D_80A22AC4[] = { 1.0f, -1.0f };

Vec3f D_80A22ACC = { 0.0f, 0.3f, 0.0f };

Color_RGBA8 D_80A22AD8 = { 250, 250, 250, 255 };

Color_RGBA8 D_80A22ADC = { 180, 180, 180, 255 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -1700, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(terminalVelocity, -20000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 150, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_STOP),
};

f32 D_80A22AF8[] = { 14.0f, -14.0f, 14.0f, -14.0f };

Color_RGB8 D_80A22B08[] = {
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
};

s32 func_80A21150(ObjSkateblock* this) {
    s32 temp_v0;

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        if (this->dyna.pushForce > 0.0f) {
            temp_v0 = BINANG_ADD(this->dyna.yRotation, 0x2000);
        } else {
            temp_v0 = BINANG_SUB(this->dyna.yRotation, 0x6000);
        }

        if (temp_v0 < -0x4000) {
            return 3;
        }

        if (temp_v0 < 0) {
            return 1;
        }

        if (temp_v0 < 0x4000) {
            return 2;
        }

        return 0;
    }
    return -1;
}

void func_80A211F4(ObjSkateblock* this, s32 arg1) {
    s32 i;
    s32 phi_v0 = false;

    if (this->dyna.pushForce > 0.0f) {
        phi_v0 = true;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_172); i++) {
        if ((arg1 == i) && phi_v0) {
            this->unk_172[i]++;
        } else if (this->unk_172[i] > 0) {
            this->unk_172[i]--;
        }
    }
}

void func_80A212F0(Vec3f* arg0, Vec3f* arg1, s16 arg2) {
    f32 sp1C = Math_SinS(arg2);
    f32 temp_f0 = Math_CosS(arg2);

    arg0->x = (arg1->z * sp1C) + (arg1->x * temp_f0);
    arg0->y = arg1->y;
    arg0->z = (arg1->z * temp_f0) - (arg1->x * sp1C);
}

s32 func_80A21370(ObjSkateblock* this, PlayState* play) {
    ObjSkateblockFloor* floor;
    s32 i;
    s32 pad;
    s32 sp98;
    Vec3f sp8C;
    s32 sp88;
    f32 phi_f20 = BGCHECK_Y_MIN;

    sp88 = false;
    sp8C.y = this->dyna.actor.world.pos.y + 40.0f;
    sp98 = -1;

    for (i = 0; i < ARRAY_COUNT(this->unk_184); i++) {
        floor = &this->unk_184[i];

        floor->poly = NULL;
        floor->height = BGCHECK_Y_MIN;
        floor->bgId = BG_ACTOR_MAX;

        sp8C.x = this->dyna.actor.world.pos.x + ((D_80A22A1C[i].x * this->dyna.actor.scale.x) + D_80A22A44[i].x);
        sp8C.z = this->dyna.actor.world.pos.z + ((D_80A22A1C[i].z * this->dyna.actor.scale.z) + D_80A22A44[i].z);

        floor->height =
            BgCheck_EntityRaycastFloor6(&play->colCtx, &floor->poly, &floor->bgId, &this->dyna.actor, &sp8C, 0.0f);

        if (floor->height > BGCHECK_Y_MIN + 1.0f) {
            sp88 = true;
            if (phi_f20 < floor->height) {
                phi_f20 = floor->height;
                sp98 = i;
            }
        }
    }

    if (sp98 >= 0) {
        this->dyna.actor.floorPoly = this->unk_184[sp98].poly;
        this->dyna.actor.floorHeight = this->unk_184[sp98].height;
        this->dyna.actor.floorBgId = this->unk_184[sp98].bgId;
    }
    return sp88;
}

s32 func_80A21548(ObjSkateblock* this, PlayState* play) {
    s32 i;
    ObjSkateblockFloor* floor;

    this->unk_1C1 &= ~4;

    if (func_80A21370(this, play)) {
        f32 maxHeight = BGCHECK_Y_MIN;
        s32 phi_v1 = false;
        f32 temp_f12 = this->dyna.actor.world.pos.y - 10.0f;

        for (i = 0; i < ARRAY_COUNT(this->unk_184); i++) {
            floor = &this->unk_184[i];
            if ((temp_f12 < floor->height) && (floor->height > maxHeight)) {
                maxHeight = floor->height;
                phi_v1 = true;
            }
        }

        if (phi_v1) {
            this->dyna.actor.world.pos.y = maxHeight;
            this->unk_1C1 |= 4;
            this->dyna.actor.velocity.y = 0.0f;
        }
        return phi_v1;
    }
    return false;
}

s32 func_80A216D4(ObjSkateblock* this, PlayState* play, f32 arg2, Vec3f* arg3) {
    s32 pad;
    f32 spE8;
    f32 spE4;
    s32 i;
    Vec3f spD4;
    Vec3f spC8;
    Vec3f spBC;
    Vec3f spB0;
    f32 temp_f20;
    f32 temp_f24;
    s32 bgId;
    CollisionPoly* spA0;
    s32 ret;
    f32 temp_f26;
    s16 sp96 = this->dyna.yRotation;
    f32 temp_f2;

    if (this->dyna.pushForce < 0.0f) {
        sp96 -= 0x8000;
    }
    spE8 = Math_SinS(sp96);
    spE4 = Math_CosS(sp96);

    temp_f2 = Math3D_Vec3f_DistXYZ(&this->dyna.actor.world.pos, &this->dyna.actor.prevPos) +
              (300.0f * this->dyna.actor.scale.z) + arg2;
    temp_f24 = temp_f2 * spE8;
    temp_f26 = temp_f2 * spE4;

    ret = false;
    temp_f20 = FLT_MAX;

    for (i = 0; i < ARRAY_COUNT(D_80A22A6C); i++) {
        spBC.x = (D_80A22A6C[i].x * this->dyna.actor.scale.x) + D_80A22A94[i].x;
        spBC.y = (D_80A22A6C[i].z * this->dyna.actor.scale.y) + D_80A22A94[i].z;
        spBC.z = 0.0f;

        func_80A212F0(&spD4, &spBC, sp96);

        spD4.x += this->dyna.actor.prevPos.x;
        spD4.y += this->dyna.actor.prevPos.y;
        spD4.z += this->dyna.actor.prevPos.z;

        spC8.x = temp_f24 + spD4.x;
        spC8.y = spD4.y;
        spC8.z = temp_f26 + spD4.z;

        if (BgCheck_EntityLineTest3(&play->colCtx, &spD4, &spC8, &spB0, &spA0, true, false, false, true, &bgId,
                                    &this->dyna.actor, 0.0f)) {
            temp_f2 = Math3D_Vec3fDistSq(&spD4, &spB0);
            if (temp_f2 < temp_f20) {
                temp_f20 = temp_f2;
                ret = true;
                Math_Vec3f_Diff(&spB0, &spC8, arg3);
            }
        }
    }
    return ret;
}

s32 func_80A21934(ObjSkateblock* this, PlayState* play) {
    Vec3f sp1C;

    if (func_80A216D4(this, play, 0.0f, &sp1C)) {
        this->dyna.actor.world.pos.x += sp1C.x;
        this->dyna.actor.world.pos.z += sp1C.z;
        return true;
    }
    return false;
}

s32 func_80A21990(ObjSkateblock* this, PlayState* play, Vec3f* arg2) {
    s32 i;
    f32 sp100;
    s16 phi_s6 = this->dyna.yRotation;
    f32 phi_f22;
    Vec3f spEC;
    Vec3f spE0;
    Vec3f spD4;
    Vec3f spC8;
    f32 temp_f0;
    f32 temp_f12;
    s32 bgId;
    CollisionPoly* spB8;
    s32 ret;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f30;

    if (this->dyna.pushForce < 0.0f) {
        phi_s6 -= 0x8000;
    }

    sp100 = Math_SinS(phi_s6);
    temp_f26 = Math_CosS(phi_s6);

    temp_f12 = (300.0f * this->dyna.actor.scale.z +
                Math3D_Vec3f_DistXYZ(&this->dyna.actor.world.pos, &this->dyna.actor.prevPos)) +
               2.0f;
    temp_f28 = -temp_f12 * sp100;
    temp_f30 = -temp_f12 * temp_f26;

    ret = false;
    phi_f22 = FLT_MAX;

    for (i = 0; i < ARRAY_COUNT(D_80A22ABC); i++) {
        spD4.x = (D_80A22ABC[i] * this->dyna.actor.scale.x) + D_80A22AC4[i];
        spD4.y = -10.0f;
        spD4.z = 0.0f;

        func_80A212F0(&spEC, &spD4, phi_s6);

        spEC.x += this->dyna.actor.world.pos.x;
        spEC.y += this->dyna.actor.world.pos.y;
        spEC.z += this->dyna.actor.world.pos.z;

        spE0.x = temp_f28 + spEC.x;
        spE0.y = spEC.y;
        spE0.z = temp_f30 + spEC.z;

        if (BgCheck_EntityLineTest3(&play->colCtx, &spEC, &spE0, &spC8, &spB8, true, false, false, true, &bgId,
                                    &this->dyna.actor, 0.0f)) {
            temp_f0 = Math3D_Vec3fDistSq(&spEC, &spC8);
            if (temp_f0 < phi_f22) {
                ret = true;
                arg2->x = (spC8.x - spEC.x) + (300.0f * this->dyna.actor.scale.z * sp100);
                arg2->y = 0.0f;
                arg2->z = (spC8.z - spEC.z) + (300.0f * this->dyna.actor.scale.z * temp_f26);
                phi_f22 = temp_f0;
            }
        }
    }
    return ret;
}

s32 func_80A21C30(ObjSkateblock* this, PlayState* play) {
    Vec3f sp1C;

    if (func_80A21990(this, play, &sp1C)) {
        this->dyna.actor.world.pos.x += sp1C.x;
        this->dyna.actor.world.pos.z += sp1C.z;
        return true;
    }

    return false;
}

void func_80A21C88(ObjSkateblock* this, s32 arg1) {
    if ((arg1 == 0) || (arg1 == 1)) {
        this->unk_16C = &this->dyna.actor.world.pos.x;
    } else {
        this->unk_16C = &this->dyna.actor.world.pos.z;
    }
    this->unk_170 = arg1;
}

void func_80A21CB4(ObjSkateblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 &= ~PLAYER_STATE2_10;
    this->dyna.pushForce = 0.0f;
}

void func_80A21CD8(ObjSkateblock* this) {
    this->dyna.actor.velocity.y += this->dyna.actor.gravity;
    if (this->dyna.actor.velocity.y < this->dyna.actor.terminalVelocity) {
        this->dyna.actor.velocity.y = this->dyna.actor.terminalVelocity;
    }
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
}

void func_80A21D1C(ObjSkateblock* this) {
    f32 sp4;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f6;
    f32 phi_f2;

    temp_f14 = this->dyna.actor.world.pos.x - this->dyna.actor.home.pos.x;
    phi_f2 = ((temp_f14 >= 0.0f) ? 0.5f : -0.5f);
    sp4 = ((s32)(phi_f2 + temp_f14) / 30) * 30;
    temp_f14 -= sp4;

    if (fabsf(temp_f14) < 3.0f) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + sp4;
    }

    temp_f16 = this->dyna.actor.world.pos.z - this->dyna.actor.home.pos.z;
    phi_f2 = ((temp_f16 >= 0.0f) ? 0.5f : -0.5f);
    temp_f6 = ((s32)(phi_f2 + temp_f16) / 30) * 30;
    temp_f16 -= temp_f6;

    if (fabsf(temp_f16) < 3.0f) {
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + temp_f6;
    }
}

void func_80A21E9C(ObjSkateblock* this, PlayState* play) {
    DynaPolyActor* temp_v0;
    s32 i;
    ObjSkateblockFloor* ptr;

    if (!(this->unk_1C1 & 4)) {
        return;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_184); i++) {
        ptr = &this->unk_184[i];

        temp_v0 = DynaPoly_GetActor(&play->colCtx, ptr->bgId);
        if ((temp_v0 != NULL) && (temp_v0->actor.id == ACTOR_OBJ_SKATEBLOCK)) {
            ObjSkateblock* skateBlock = (ObjSkateblock*)temp_v0;
            f32 height = this->dyna.actor.world.pos.y - ptr->height;

            if (fabsf(height) < 0.1f) {
                skateBlock->unk_1C1 |= 2;
            }
        }
    }
}

void func_80A21F68(ObjSkateblock* this) {
    this->unk_17C = 0;
}

void func_80A21F74(ObjSkateblock* this, PlayState* play) {
    s32 pad;
    Vec3f spA8;
    Vec3f sp9C;
    f32 phi_f22;
    s32 i;
    f32 temp;
    s32 pad2;
    s16 sp8A = this->dyna.yRotation;

    if (this->dyna.pushForce < 0.0f) {
        sp8A -= 0x8000;
    }

    this->unk_17C++;

    if (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
        if (this->unk_17C >= 46) {
            phi_f22 = 1.0f;
        } else {
            phi_f22 = (this->unk_17C * 0.017777778f) + 0.2f;
        }
        sp9C.y = 12.0f;

        for (i = 0; i < 2; i++) {
            if (phi_f22 < (Rand_ZeroOne() * 1.2f)) {
                continue;
            }
            this->unk_180 += Rand_ZeroOne();

            if (this->unk_180 > 1.0f) {
                this->unk_180 -= 1.0f;
            }

            sp9C.x = (this->unk_180 - 0.5f) * 600.0f * this->dyna.actor.scale.x;
            sp9C.z = (this->dyna.actor.scale.z * -300.0f) + 4.0f;

            func_80A212F0(&spA8, &sp9C, sp8A);

            spA8.x += this->dyna.actor.world.pos.x;
            spA8.y += this->dyna.actor.world.pos.y;
            spA8.z += this->dyna.actor.world.pos.z;

            temp = ((Rand_ZeroOne() * 800.0f) + (1600.0f * this->dyna.actor.scale.x)) * phi_f22;
            func_800B0E48(play, &spA8, &gZeroVec3f, &D_80A22ACC, &D_80A22AD8, &D_80A22ADC, temp,
                          (Rand_ZeroOne() * 20.0f) + 30.0f);
        }
    }
}

void ObjSkateblock_Init(Actor* thisx, PlayState* play) {
    ObjSkateblock* this = (ObjSkateblock*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gameplay_dangeon_keep_Colheader_007498);
    if (D_80A22A18 == NULL) {
        D_80A22A18 = Lib_SegmentedToVirtual(gameplay_dangeon_keep_Matanimheader_01B370);
    }
    func_80A22308(this);
    this->unk_1C0 = D_80A22A14;
    D_80A22A14++;
    D_80A22A14 &= 0x1F;
}

void ObjSkateblock_Destroy(Actor* thisx, PlayState* play) {
    ObjSkateblock* this = (ObjSkateblock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80A22308(ObjSkateblock* this) {
    this->actionFunc = func_80A22334;
    this->unk_172[1] = 0;
    this->unk_172[2] = 0;
    this->unk_172[3] = 0;
    this->unk_172[0] = 0;
    this->dyna.actor.velocity.y = 0.0f;
}

void func_80A22334(ObjSkateblock* this, PlayState* play) {
    s32 pad;
    s32 sp30;
    s32 sp2C;
    Vec3f sp20;

    func_80A21CD8(this);
    func_80A21548(this, play);
    sp2C = func_80A21150(this);
    func_80A211F4(this, sp2C);

    sp30 = true;

    if (sp2C == -1) {
        sp30 = false;
    } else if (!(this->unk_1C1 & 2) &&
               GameInteractor_Should(VB_SKATE_BLOCK_BEGIN_MOVE, (this->unk_172[sp2C] > 10), this, sp2C) &&
               (D_80A22A10 == 0) && !func_80A216D4(this, play, 2.0f, &sp20) && !Player_InCsMode(play)) {
        func_80A21C88(this, sp2C);
        func_80A2244C(this);
        sp30 = false;
        Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_WAIT);
        this->unk_1C1 |= 1;
    }

    if (sp30) {
        func_80A21CB4(this, play);
    }
}

void func_80A2244C(ObjSkateblock* this) {
    this->actionFunc = func_80A224A4;
    this->unk_168 = D_80A22AF8[this->unk_170];
    this->unk_164 = 0.0f;
    this->unk_17A = 15;
    this->dyna.actor.velocity.y = this->dyna.actor.gravity;
    func_80A21F68(this);
}

void func_80A224A4(ObjSkateblock* this, PlayState* play) {
    s32 pad;
    s32 sp28 = false;
    s32 sp24 = false;

    Math_StepToF(&this->unk_164, this->unk_168, 0.75f);

    *this->unk_16C += this->unk_164;

    if (func_80A21934(this, play)) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        func_80A21D1C(this);
        sp28 = true;
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_PL_SLIP_ICE_LEVEL - SFX_FLAG);
    }

    func_80A21CD8(this);

    if (!func_80A21548(this, play)) {
        func_80A21C30(this, play);
        func_80A21D1C(this);
        func_80A22628(this);
        sp24 = true;
    } else if (sp28) {
        func_80A22308(this);
    }

    if (this->unk_17A > 0) {
        this->unk_17A--;
    }

    if (sp24 || sp28 || (this->unk_17A == 1)) {
        func_80A21CB4(this, play);
    }

    if ((this->unk_1C1 & 1) && (sp24 || sp28 || (this->dyna.actor.xzDistToPlayer > 400.0f))) {
        this->unk_1C1 &= ~1;
        Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_END);
    }

    func_80A21F74(this, play);
}

void func_80A22628(ObjSkateblock* this) {
    this->actionFunc = func_80A2264C;
    this->unk_160 = this->dyna.actor.world.pos.y;
    this->dyna.actor.velocity.y = this->dyna.actor.gravity;
}

void func_80A2264C(ObjSkateblock* this, PlayState* play) {
    s32 pad;
    s32 sp20;

    if (func_80A21150(this) != -1) {
        func_80A21CB4(this, play);
    }
    func_80A21CD8(this);

    sp20 = func_80A21548(this, play);
    if (sp20 || ((this->unk_160 - this->dyna.actor.world.pos.y) > 300.0f)) {
        if (SurfaceType_GetFloorProperty(&play->colCtx, this->dyna.actor.floorPoly, this->dyna.actor.floorBgId) ==
            FLOOR_PROPERTY_12) {
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            this->dyna.actor.draw = NULL;
            func_80A22728(this);
            return;
        }
    }

    if (sp20) {
        func_80A22308(this);
    }
}

void func_80A22728(ObjSkateblock* this) {
    this->actionFunc = func_80A2273C;
}

void func_80A2273C(ObjSkateblock* this, PlayState* play) {
    if (!DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.y = (this->dyna.actor.home.pos.y - (600.0f * this->dyna.actor.scale.y)) - 10.0f;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;

        func_80A227A4(this);
    }
}

void func_80A227A4(ObjSkateblock* this) {
    this->actionFunc = func_80A227C0;
    this->unk_17A = 40;
}

void func_80A227C0(ObjSkateblock* this, PlayState* play) {
    if (func_80A21150(this) != -1) {
        func_80A21CB4(this, play);
    }

    if (this->unk_17A > 0) {
        this->unk_17A--;
        return;
    }

    DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    this->dyna.actor.draw = ObjSkateblock_Draw;

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f)) {
        func_80A22308(this);
    }

    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        D_80A22A10 |= 1 << this->unk_1C0;
    }
}

void ObjSkateblock_Update(Actor* thisx, PlayState* play) {
    ObjSkateblock* this = (ObjSkateblock*)thisx;

    D_80A22A10 &= ~(1 << this->unk_1C0);
    this->actionFunc(this, play);
    this->unk_1C1 &= ~2;
}

void ObjSkateblock_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjSkateblock* this = (ObjSkateblock*)thisx;
    Color_RGB8* sp2C = &D_80A22B08[OBJSKAEBLOCK_GET_F(&this->dyna.actor)];

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    AnimatedMat_DrawStep(play, D_80A22A18, 0);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0xFF, 0xFF, sp2C->r, sp2C->g, sp2C->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_dangeon_keep_DL_0182A8);

    func_80A21E9C(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}
