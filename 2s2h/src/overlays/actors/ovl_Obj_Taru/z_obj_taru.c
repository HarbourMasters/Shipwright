/*
 * File: z_obj_taru.c
 * Overlay: ovl_Obj_Taru
 * Description: Wooden Barrel and Breakable Pirate Panel
 */

#include "z_obj_taru.h"
#include "overlays/actors/ovl_En_Sw/z_en_sw.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "objects/object_taru/object_taru.h"
#include "objects/object_kibako2/object_kibako2.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

void ObjTaru_Init(Actor* thisx, PlayState* play);
void ObjTaru_Destroy(Actor* thisx, PlayState* play);
void ObjTaru_Update(Actor* thisx, PlayState* play);
void ObjTaru_Draw(Actor* thisx, PlayState* play);

void func_80B9C07C(ObjTaru* this, PlayState* play);
void func_80B9C174(ObjTaru* this, PlayState* play);
void func_80B9C1A0(ObjTaru* this, PlayState* play);

ActorProfile Obj_Taru_Profile = {
    /**/ ACTOR_OBJ_TARU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TARU,
    /**/ sizeof(ObjTaru),
    /**/ ObjTaru_Init,
    /**/ ObjTaru_Destroy,
    /**/ ObjTaru_Update,
    /**/ ObjTaru_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x80000508, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 30, 50, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_STOP),
};

bool func_80B9B6E0(ObjTaru* this, PlayState* play) {
    s32 chestFlag = -1;
    s32 skulltulaParams = (OBJ_TSUBO_P001F(&this->dyna.actor) * 4) | 0xFF01;

    if (ENSW_GETS_3(skulltulaParams)) {
        chestFlag = ENSW_GETS_3FC(skulltulaParams);
    }

    return (chestFlag < 0) == true || !Flags_GetTreasure(play, chestFlag);
}

void func_80B9B74C(ObjTaru* this, PlayState* play) {
    s32 pad[2];
    Vec3f* thisPos = &this->dyna.actor.world.pos;
    Vec3f pos;
    Vec3f velocity;
    s16 angle;
    s32 i;

    for (i = 0, angle = 0; i < 0x10; i++, angle += 0x4E20) {
        f32 sin = Math_SinS(angle);
        f32 cos = Math_CosS(angle);
        f32 tempRand = Rand_ZeroOne() * 30.0f;
        s32 phi_s0;

        pos.x = sin * tempRand;
        pos.y = (Rand_ZeroOne() * 10.0f) + 2.0f;
        pos.z = cos * tempRand;

        velocity.x = pos.x * 0.2f;
        velocity.y = (Rand_ZeroOne() * 10.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;

        pos.x += thisPos->x;
        pos.y += thisPos->y;
        pos.z += thisPos->z;

        tempRand = Rand_ZeroOne();
        if (tempRand < 0.05f) {
            phi_s0 = 0x60;
        } else if (tempRand < 0.7f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, -200, phi_s0, 28, 2, 0, (Rand_ZeroOne() * 30.0f) + 5.0f, 0, 0,
                             70, KAKERA_COLOR_NONE, OBJECT_KIBAKO2, gLargeCrateFragment1DL);
    }
    func_800BBFB0(play, thisPos, 90.0f, 6, 100, 160, 1);
}

void func_80B9B9C8(ObjTaru* this, PlayState* play) {
    s32 phi_s5;
    s32 i;
    s32 j;
    Vec3f spD8;
    Vec3f spCC;
    s32 pad[2];
    f32 sin;
    f32 cos;
    s32 phi_fp;
    Vec3f* thisPos = &this->dyna.actor.world.pos;
    f32 temp_fs0;
    f32 temp_fs1;

    sin = Math_SinS(this->dyna.actor.shape.rot.y);
    cos = Math_CosS(this->dyna.actor.shape.rot.y);

    if (OBJ_TARU_GET_100(&this->dyna.actor)) {
        phi_fp = 0;
        phi_s5 = 8;
    } else {
        phi_fp = 2;
        phi_s5 = 6;
    }

    for (i = 0; i < 4; i++) {
        for (j = phi_fp; j < phi_s5; j++) {
            temp_fs0 = Rand_CenteredFloat(10.0f) + -105.0f + (j * 30.0f);
            temp_fs1 = Rand_CenteredFloat(4.0f);
            spD8.x = temp_fs0 * cos;
            spD8.y = Rand_CenteredFloat(10.0f) + 15.0f + (i * 30.0f);
            spD8.z = temp_fs0 * sin;
            spCC.x = (spD8.x * 0.05f) + (temp_fs1 * sin);
            spCC.y = Rand_ZeroFloat(5.0f) + 2.0f;
            spCC.z = (spD8.z * 0.05f) + (temp_fs1 * cos);
            spD8.x += thisPos->x;
            spD8.y += thisPos->y;
            spD8.z += thisPos->z;
            EffectSsKakera_Spawn(play, &spD8, &spCC, &spD8, -0x64, 0x20, 0x1C, 4, 0, (Rand_ZeroOne() * 30.0f) + 5.0f, 0,
                                 0, 0x5A, KAKERA_COLOR_NONE, OBJECT_KIBAKO2, gLargeCrateFragment2DL);
        }
    }
}

void func_80B9BC64(ObjTaru* this, PlayState* play) {
    if (!GameInteractor_Should(VB_BARREL_OR_CRATE_DROP_COLLECTIBLE, true, this)) {
        return;
    }

    s32 item;

    item = func_800A8150(OBJ_TARU_GET_3F(&this->dyna.actor));
    if (item >= 0) {
        Item_DropCollectible(play, &this->dyna.actor.world.pos, (OBJ_TARU_GET_7F00(&this->dyna.actor) << 8) | item);
    }
}

void func_80B9BCBC(ObjTaru* this, PlayState* play) {
    s32 pad;
    s32 params1F;
    Actor* spawnedActor;
    s16 rotY;

    if (func_80B9B6E0(this, play)) {
        params1F = (OBJ_TARU_GET_1F(&this->dyna.actor) * 4) | 0xFF01;
        rotY = (Rand_Next() >> 0x11) + this->dyna.actor.yawTowardsPlayer + 0xC000;
        spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SW, this->dyna.actor.world.pos.x,
                                   this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, rotY, 0, params1F);
        if (spawnedActor != NULL) {
            spawnedActor->parent = &this->dyna.actor;
            spawnedActor->velocity.y = 12.0f;
            spawnedActor->speed = 2.0f;
        }
    }
}

void func_80B9BD84(ObjTaru* this, PlayState* play) {
    if (!OBJ_TARU_GET_8000(&this->dyna.actor)) {
        func_80B9BC64(this, play);
    } else {
        func_80B9BCBC(this, play);
    }
}

void ObjTaru_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjTaru* this = (ObjTaru*)thisx;
    s32 params8000;

    DynaPolyActor_Init(&this->dyna, 0);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    if (OBJ_TARU_GET_80(thisx)) {
        if (Flags_GetSwitch(play, OBJ_TARU_GET_SWITCH_FLAG(thisx))) {
            Actor_Kill(&this->dyna.actor);
        } else {
            DynaPolyActor_LoadMesh(play, &this->dyna, &object_taru_Colheader_001CB0);
        }
        if (OBJ_TARU_GET_100(&this->dyna.actor)) {
            this->dyna.actor.scale.x = 0.2f;
        }
    } else {
        params8000 = OBJ_TARU_GET_8000(thisx);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_taru_Colheader_000FC8);
        Collider_InitAndSetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
        Collider_UpdateCylinder(&this->dyna.actor, &this->collider);

        if (params8000 == 0) {
            if (Item_CanDropBigFairy(play, OBJ_TARU_GET_3F(&this->dyna.actor), OBJ_TARU_GET_7F00(&this->dyna.actor))) {
                this->unk_1AC = 1;
                this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            }
        }
        if ((params8000 != 1) || (!func_80B9B6E0(this, play))) {
            this->unk_1AD = -1;
        }
    }
    this->actionFunc = func_80B9C07C;
}

void ObjTaru_Destroy(Actor* thisx, PlayState* play) {
    ObjTaru* this = (ObjTaru*)thisx;

    if (!OBJ_TARU_GET_80(thisx)) {
        Collider_DestroyCylinder(play, &this->collider);
    }
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

s32 func_80B9BF7C(ObjTaru* this) {
    s32 pad;
    s32 phi_a3 = false;

    if (!OBJ_TARU_GET_80(&this->dyna.actor) && (this->collider.base.acFlags & AC_HIT)) {
        Actor* ac = this->collider.base.ac;

        this->collider.base.acFlags &= ~AC_HIT;
        if (ac != NULL) {
            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000) {
                phi_a3 = false;
                if (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &ac->world.pos) < SQ(160.0f)) {
                    phi_a3 = true;
                }
            } else if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 8) {
                if (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &ac->world.pos) < SQ(100.0f)) {
                    phi_a3 = true;
                }
            } else if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x500) {
                phi_a3 = true;
            }
        }
    } else if (this->dyna.actor.home.rot.z != 0) {
        phi_a3 = true;
    }
    return phi_a3;
}

void func_80B9C07C(ObjTaru* this, PlayState* play) {
    if (func_80B9BF7C(this)) {
        if (!OBJ_TARU_GET_80(&this->dyna.actor)) {
            func_80B9B74C(this, play);
        } else {
            func_80B9B9C8(this, play);
        }
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 0x14, NA_SE_EV_WOODBOX_BREAK);
        if (OBJ_TARU_GET_80(&this->dyna.actor)) {
            this->actionFunc = func_80B9C1A0;
        } else {
            this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            this->dyna.actor.draw = NULL;
            this->actionFunc = func_80B9C174;
        }
    } else {
        if (this->dyna.actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void func_80B9C174(ObjTaru* this, PlayState* play) {
    func_80B9BD84(this, play);
    Actor_Kill(&this->dyna.actor);
}

void func_80B9C1A0(ObjTaru* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        Flags_SetSwitch(play, OBJ_TARU_GET_SWITCH_FLAG(&this->dyna.actor));
        Actor_Kill(&this->dyna.actor);
        return;
    }

    CutsceneManager_Queue(this->dyna.actor.csId);
}

void ObjTaru_Update(Actor* thisx, PlayState* play) {
    ObjTaru* this = (ObjTaru*)thisx;

    if (!OBJ_TARU_GET_80(thisx)) {
        if (this->unk_1AC != 0) {
            play->actorCtx.flags |= ACTORCTX_FLAG_3;
        }
        if (this->unk_1AD >= 0) {
            switch (this->unk_1AD) {
                case 0:
                    Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_STALGOLD_ROLL);
                    if (Rand_ZeroOne() < 0.1f) {
                        this->unk_1AD = Rand_S16Offset(40, 80);
                    } else {
                        this->unk_1AD = 8;
                    }
                    break;
                default:
                    this->unk_1AD--;
                    break;
            }
        }
    }
    this->actionFunc(this, play);
}

void ObjTaru_Draw(Actor* thisx, PlayState* play) {
    Gfx* dList;
    ObjTaru* this = (ObjTaru*)thisx;

    if (OBJ_TARU_GET_80(thisx)) {
        dList = gObjTaruBreakablePiratePanelDL;
    } else {
        dList = gObjTaruBarrelDL;
    }
    Gfx_DrawDListOpa(play, dList);
}
