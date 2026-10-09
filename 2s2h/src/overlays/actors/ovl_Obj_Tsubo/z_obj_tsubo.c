/*
 * File: z_obj_tsubo.c
 * Overlay: ovl_Obj_Tsubo
 * Description: Pots
 */

#include "z_obj_tsubo.h"
#include "overlays/actors/ovl_En_Sw/z_en_sw.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "objects/object_tsubo/object_tsubo.h"
#include "objects/object_racetsubo/object_racetsubo.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY | ACTOR_FLAG_CAN_PRESS_SWITCHES)

void ObjTsubo_Init(Actor* thisx, PlayState* play);
void ObjTsubo_Destroy(Actor* thisx, PlayState* play2);
void ObjTsubo_Update(Actor* thisx, PlayState* play);
void ObjTsubo_Draw(Actor* thisx, PlayState* play2);

void ObjTsubo_PotBreak1(ObjTsubo* this, PlayState* play);
void ObjTsubo_MagicPotBreak1(ObjTsubo* this, PlayState* play);
void ObjTsubo_PotBreak2(ObjTsubo* this, PlayState* play2);
void ObjTsubo_MagicPotBreak2(ObjTsubo* this, PlayState* play2);
void ObjTsubo_PotBreak3(ObjTsubo* this, PlayState* play2);
void ObjTsubo_MagicPotBreak3(ObjTsubo* this, PlayState* play2);
void func_80928914(ObjTsubo* this);
void func_80928928(ObjTsubo* this, PlayState* play);
void func_809289B4(ObjTsubo* this);
void func_809289E4(ObjTsubo* this, PlayState* play);
void func_80928D6C(ObjTsubo* this);
void func_80928D80(ObjTsubo* this, PlayState* play);
void func_80928E74(ObjTsubo* this);
void func_80928F18(ObjTsubo* this, PlayState* play);
void func_809291DC(ObjTsubo* this);
void func_8092926C(ObjTsubo* this, PlayState* play);

s16 D_80929500 = 0;
s16 D_80929504 = 0;
s16 D_80929508 = 0;
s16 D_8092950C = 0;

ActorProfile Obj_Tsubo_Profile = {
    /**/ ACTOR_OBJ_TSUBO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjTsubo),
    /**/ ObjTsubo_Init,
    /**/ ObjTsubo_Destroy,
    /**/ ObjTsubo_Update,
    /**/ NULL,
};

typedef struct {
    /* 0x00 */ s16 objectId;
    /* 0x04 */ f32 scale;
    /* 0x08 */ Gfx* modelDL;
    /* 0x0C */ Gfx* shardDL;
    /* 0x10 */ s16 radius;
    /* 0x12 */ s16 height;
    /* 0x14 */ ObjTsuboUnkFunc breakPot1;
    /* 0x18 */ ObjTsuboUnkFunc breakPot2;
    /* 0x1C */ ObjTsuboUnkFunc breakPot3;
} ObjTsuboData; // size = 0x20

ObjTsuboData sPotTypeData[4] = {
    { GAMEPLAY_DANGEON_KEEP, 0.197f, gameplay_dangeon_keep_DL_017EA0, gameplay_dangeon_keep_DL_018090, 12, 32,
      ObjTsubo_PotBreak1, ObjTsubo_PotBreak2, ObjTsubo_PotBreak3 },
    { OBJECT_RACETSUBO, 0.29549998f, gMagicPotDL, gMagicPotShardDL, 18, 45, ObjTsubo_MagicPotBreak1,
      ObjTsubo_MagicPotBreak2, ObjTsubo_MagicPotBreak3 },
    { OBJECT_TSUBO, 0.197f, gPotDL, gPotShardDL, 12, 32, ObjTsubo_PotBreak1, ObjTsubo_PotBreak2, ObjTsubo_PotBreak3 },
    { GAMEPLAY_DANGEON_KEEP, 0.197f, gameplay_dangeon_keep_DL_017EA0, gameplay_dangeon_keep_DL_018090, 12, 32,
      ObjTsubo_PotBreak1, ObjTsubo_PotBreak2, ObjTsubo_PotBreak3 },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00400000, 0x00, 0x02 },
        { 0x05CBFFBE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 30, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

bool func_809275C0(ObjTsubo* this, PlayState* play) {
    s32 chestFlag = -1;
    s32 skulltulaParams = (OBJ_TSUBO_P001F(&this->actor) << 2) | 0xFF01;

    if (ENSW_GETS_3(skulltulaParams)) {
        chestFlag = ENSW_GETS_3FC(skulltulaParams);
    }

    return (chestFlag < 0) == true || !Flags_GetTreasure(play, chestFlag);
}

void func_8092762C(ObjTsubo* this, PlayState* play) {
    if (GameInteractor_Should(VB_POT_DROP_COLLECTIBLE,
                              !OBJ_TSUBO_P0010(&this->actor) && (OBJ_TSUBO_ZROT(&this->actor) != 2), this)) {
        Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, OBJ_TSUBO_P000F(&this->actor) << 4);
    }
}

void func_80927690(ObjTsubo* this, PlayState* play) {
    s32 itemDrop;

    if (GameInteractor_Should(VB_POT_DROP_COLLECTIBLE, !this->unk_197 && (OBJ_TSUBO_ZROT(&this->actor) != 2), this)) {
        itemDrop = func_800A8150(OBJ_TSUBO_P003F(&this->actor));
        if (itemDrop > ITEM00_NO_DROP) {
            Item_DropCollectible(play, &this->actor.world.pos, (OBJ_TSUBO_PFE00(&this->actor) << 8) | itemDrop);
            this->unk_197 = true;
        }
    }
}

void ObjTsubo_SpawnBoes(ObjTsubo* this, PlayState* play, s32 arg2) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_MKK, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, 0, 0, 2);
}

void ObjTsubo_SpawnGoldSkulltula(ObjTsubo* this, PlayState* play, s32 arg2) {
    Actor* child;
    s32 params;

    if (func_809275C0(this, play)) {
        params = (OBJ_TSUBO_P001F(&this->actor) << 2) | 0xFF01;
        child = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SW, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, Rand_Next() >> 0x10, 0, params);
        if (child != NULL) {
            child->parent = &this->actor;
            child->velocity.y = 0.0f;
            child->speed = 0.0f;
        }
    }
}

void func_80927818(ObjTsubo* this, PlayState* play, s32 arg2) {
    if (OBJ_TSUBO_ZROT(&this->actor) == 1) {
        ObjTsubo_SpawnBoes(this, play, arg2);
    } else if (OBJ_TSUBO_ZROT(&this->actor) == 2) {
        ObjTsubo_SpawnGoldSkulltula(this, play, arg2);
    }
}

bool ObjTsubo_IsSceneNotGohtOrTwinmold(ObjTsubo* this, PlayState* play) {
    return (play->sceneId != SCENE_HAKUGIN_BS) && (play->sceneId != SCENE_INISIE_BS);
}

void func_8092788C(ObjTsubo* this, PlayState* play) {
    if (!this->unk_197 && (play->roomCtx.curRoom.num != this->homeRoom)) {
        this->unk_197 = true;
    }
}

void ObjTsubo_Init(Actor* thisx, PlayState* play) {
    ObjTsubo* this = (ObjTsubo*)thisx;
    s32 pad;
    s32 type = OBJ_TSUBO_GET_TYPE(&this->actor);
    s32 pad2;
    s32 sp2C = OBJ_TSUBO_ZROT(&this->actor);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, sPotTypeData[type].scale);
    this->actor.shape.rot.z = 0;
    this->actor.world.rot.z = 0;
    Collider_InitCylinder(play, &this->cylinderCollider);
    Collider_SetCylinder(play, &this->cylinderCollider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->cylinderCollider);
    this->cylinderCollider.dim.radius = sPotTypeData[type].radius;
    this->cylinderCollider.dim.height = sPotTypeData[type].height;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->objectSlot = Object_GetSlot(&play->objectCtx, sPotTypeData[type].objectId);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.shape.shadowScale = 1.8f;
    this->homeRoom = this->actor.room;
    if ((type != OBJ_TSUBO_TYPE_3) && (sp2C != 2)) {
        if (Item_CanDropBigFairy(play, OBJ_TSUBO_P003F(&this->actor), OBJ_TSUBO_PFE00(&this->actor))) {
            this->unk_198 = true;
        }
    }
    if ((type == OBJ_TSUBO_TYPE_3) || (sp2C != 2) || !func_809275C0(this, play)) {
        this->unk_19A = -1;
    }
    func_80928914(this);
}

void ObjTsubo_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjTsubo* this = (ObjTsubo*)thisx;

    Collider_DestroyCylinder(play, &this->cylinderCollider);
}

void ObjTsubo_PotBreak1(ObjTsubo* this, PlayState* play) {
    s16 rot;
    s32 i;
    s32 phi_s0;
    s32 pad;
    Vec3f pos;
    Vec3f velocity;
    ObjTsuboData* typeData = &sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)];
    f32 randf;
    f32 sin;
    f32 cos;
    f32 scale;
    s32 pad2;

    for (i = 0, rot = 0; i < 18; rot += 0x4E20, i++) {
        sin = Math_SinS(rot);
        cos = Math_CosS(rot);
        pos.x = sin * 8.0f;
        pos.y = Rand_ZeroOne() * 12.0f + 2.0f;
        pos.z = cos * 8.0f;
        velocity.x = pos.x * 0.23f;
        velocity.y = Rand_ZeroOne() * 5.0f + 2.5f;
        velocity.z = pos.z * 0.23f;
        Math_Vec3f_Sum(&pos, &this->actor.world.pos, &pos);
        randf = Rand_ZeroOne();
        if (randf < 0.2f) {
            phi_s0 = 0x60;
        } else if (randf < 0.6f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        scale = Rand_ZeroOne() * 110.0f + 15.0f;
        EffectSsKakera_Spawn(play, &pos, &velocity, &this->actor.world.pos, -260, phi_s0, 20, 0, 0, scale, 0, 0, 50, -1,
                             typeData->objectId, typeData->shardDL);
    }
    func_800BBFB0(play, &this->actor.world.pos, 30.0f, 2, 20, 50, true);
    func_800BBFB0(play, &this->actor.world.pos, 30.0f, 2, 10, 80, true);
}

void ObjTsubo_MagicPotBreak1(ObjTsubo* this, PlayState* play) {
    s16 rot;
    s32 phi_s0;
    s32 i;
    ObjTsuboData* typeData = &sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)];
    Vec3f pos;
    Vec3f velocity;
    f32 randf;
    f32 sin;
    f32 cos;
    f32 scale;
    s32 pad[2];

    for (i = 0, rot = 0; i < 20; rot += 0x4E20, i++) {
        sin = Math_SinS(rot);
        cos = Math_CosS(rot);
        pos.x = sin * 10.0f;
        pos.y = Rand_ZeroOne() * 20.0f + 2.0f;
        pos.z = cos * 10.0f;
        velocity.x = pos.x * 0.3f;
        velocity.y = Rand_ZeroOne() * 10.0f + 2.0f;
        velocity.z = pos.z * 0.3f;
        Math_Vec3f_Sum(&pos, &this->actor.world.pos, &pos);
        randf = Rand_ZeroOne();
        if (randf < 0.2f) {
            phi_s0 = 0xE0;
        } else if (randf < 0.6f) {
            phi_s0 = 0xC0;
        } else {
            phi_s0 = 0xA0;
        }
        scale = Rand_ZeroOne() * 160.0f + 15.0f;
        EffectSsKakera_Spawn(play, &pos, &velocity, &this->actor.world.pos, -340, phi_s0, 20, 0, 0, scale, 0, 0, 50, -1,
                             typeData->objectId, typeData->shardDL);
    }
    func_800BBFB0(play, &this->actor.world.pos, 50.0f, 2, 40, 50, true);
    func_800BBFB0(play, &this->actor.world.pos, 50.0f, 2, 20, 80, true);
}

void ObjTsubo_PotBreak2(ObjTsubo* this, PlayState* play2) {
    PlayState* play = play2;
    s16 rot;
    s32 i;
    s32 phi_s0;
    Vec3f pos;
    Vec3f velocity;
    Vec3f* worldPos = &this->actor.world.pos;
    ObjTsuboData* typeData = &sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)];
    f32 sin;
    f32 cos;
    f32 scale;

    pos.y = worldPos->y + this->actor.depthInWater;

    for (rot = 0, i = 0; i < 5; i++, rot += 0x10000 / 5) {
        pos.x = worldPos->x + Math_SinS((s32)(Rand_ZeroOne() * 6000) + rot) * 15.0f;
        pos.z = worldPos->z + Math_CosS((s32)(Rand_ZeroOne() * 6000) + rot) * 15.0f;
        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 350);
    }
    pos.x = worldPos->x;
    pos.z = worldPos->z;
    EffectSsGRipple_Spawn(play, &pos, 200, 600, 0);
    for (i = 0, rot = 0; i < 13; rot += 0x4E20, i++) {
        sin = Math_SinS(rot);
        cos = Math_CosS(rot);
        pos.x = sin * 8.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = cos * 8.0f;
        velocity.x = pos.x * 0.2f;
        velocity.y = (Rand_ZeroOne() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;
        Math_Vec3f_Sum(&pos, worldPos, &pos);
        if (Rand_ZeroOne() < .2f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        scale = Rand_ZeroOne() * 105.0f + 10.0f;
        EffectSsKakera_Spawn(play, &pos, &velocity, worldPos, -170, phi_s0, 50, 5, 0, scale, 0, 0, 70, -1,
                             typeData->objectId, typeData->shardDL);
    }
}

void ObjTsubo_MagicPotBreak2(ObjTsubo* this, PlayState* play2) {
    PlayState* play = play2;
    s32 pad;
    s16 rot;
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    ObjTsuboData* typeData = &sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)];
    Vec3f* worldPos = &this->actor.world.pos;
    f32 sin;
    f32 cos;
    s32 phi_s0;
    s16 scale;

    pos.y = this->actor.world.pos.y + this->actor.depthInWater;

    for (rot = 0, i = 0; i < 5; i++, rot += 0x10000 / 5) {
        pos.x = worldPos->x + Math_SinS((s32)(Rand_ZeroOne() * 6000) + rot) * 30.0f;
        pos.z = worldPos->z + Math_CosS((s32)(Rand_ZeroOne() * 6000) + rot) * 30.0f;
        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 350);
    }
    pos.x = worldPos->x;
    pos.z = worldPos->z;
    EffectSsGRipple_Spawn(play, &pos, 300, 700, 0);
    for (i = 0, rot = 0; i < 15; rot += 0x4E20, i++) {
        sin = Math_SinS(rot);
        cos = Math_CosS(rot);
        pos.x = sin * 10.0f;
        pos.y = (Rand_ZeroOne() * 15.0f) + 2.0f;
        pos.z = cos * 10.0f;
        velocity.x = pos.x * 0.3f;
        velocity.y = (Rand_ZeroOne() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.3f;
        Math_Vec3f_Sum(&pos, worldPos, &pos);
        if (Rand_ZeroOne() < 0.2f) {
            phi_s0 = 0xC0;
        } else {
            phi_s0 = 0xA0;
        }
        scale = (Rand_ZeroOne() * 150.0f) + 10.0f;
        EffectSsKakera_Spawn(play, &pos, &velocity, worldPos, -170, phi_s0, 50, 5, 0, scale, 0, 0, 70, -1,
                             typeData->objectId, typeData->shardDL);
    }
}

void ObjTsubo_PotBreak3(ObjTsubo* this, PlayState* play2) {
    PlayState* play = play2;
    Actor* thisx = &this->actor;
    s16 rot;
    s32 phi_s0;
    Vec3f pos;
    Vec3f velocity;
    ObjTsuboData* typeData = &sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)];
    f32 randf;
    f32 temp_f20;
    f32 cos;
    f32 sin;
    s32 i;

    for (i = 0, rot = 0; i < 13; i++, rot += 0x4E20) {
        randf = Rand_ZeroOne();
        temp_f20 = (1.0f - SQ(randf)) * 4.8f + 3.2f;
        sin = Math_SinS(rot);
        cos = Math_CosS(rot);

        pos.x = sin * temp_f20;
        pos.y = (Rand_ZeroOne() * 15.0f) + 2.0f;
        pos.z = cos * temp_f20;

        velocity.x = pos.x * 0.4f;
        velocity.y = (Rand_ZeroOne() * 4.0f) + 4.0f;
        velocity.z = pos.z * 0.4f;

        Math_Vec3f_Sum(&pos, &this->actor.world.pos, &pos);

        if (Rand_ZeroOne() < 0.2f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, &this->actor.world.pos, -170, phi_s0, 50, 3, 0,
                             (Rand_ZeroOne() * 105.0f) + 10.0f, 0, 0, 70, -1, typeData->objectId, typeData->shardDL);
    }

    for (i = 0; i < 7; i++) {
        EffectSsBubble_Spawn(play, &thisx->world.pos, 20.0f, 30.0f, 40.0f, (Rand_ZeroOne() * 0.06f) + 0.09f);
        EffectSsBubble_Spawn(play, &thisx->world.pos, 10.0f, 10.0f, 10.0f, (Rand_ZeroOne() * 0.08f) + 0.09f);
    }
}

void ObjTsubo_MagicPotBreak3(ObjTsubo* this, PlayState* play2) {
}

void func_80928914(ObjTsubo* this) {
    this->actionFunc = func_80928928;
}

void func_80928928(ObjTsubo* this, PlayState* play) {
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 15.0f, 0.0f, UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        func_809289B4(this);
    }
}

void func_809289B4(ObjTsubo* this) {
    if (GameInteractor_Should(VB_POT_DRAW_BE_OVERRIDDEN, true, this)) {
        this->actor.draw = ObjTsubo_Draw;
    }
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->unk_195 = false;
    this->actionFunc = func_809289E4;
}

void func_809289E4(ObjTsubo* this, PlayState* play) {
    ObjTsuboData* typeData;
    s32 pad;
    s32 type = OBJ_TSUBO_GET_TYPE(&this->actor);
    s32 pad2;
    s32 acHit = (this->cylinderCollider.base.acFlags & AC_HIT) != 0;

    if (acHit) {
        this->cylinderCollider.base.acFlags &= ~AC_HIT;
    }
    if (this->cylinderCollider.base.ocFlags1 & OC1_HIT) {
        this->cylinderCollider.base.ocFlags1 &= ~OC1_HIT;
    }
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.room = -1;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        if ((type != OBJ_TSUBO_TYPE_3) && func_800A817C(OBJ_TSUBO_P003F(&this->actor))) {
            func_80927690(this, play);
        }
        func_80927818(this, play, 0);
        //! @bug: This function should only pass Player*: it uses *(this + 0x153), which is meant to be
        //! player->currentMask, but in this case is garbage in the collider
        Player_PlaySfx((Player*)&this->actor, NA_SE_PL_PULL_UP_POT);
        func_80928D6C(this);
    } else if (this->unk_19B || (acHit && (this->cylinderCollider.elem.acHitElem->atDmgInfo.dmgFlags & 0x058BFFBC))) {
        typeData = &sPotTypeData[type];
        this->unk_19B = false;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 15.0f)) {
            typeData->breakPot3(this, play);
        } else {
            typeData->breakPot1(this, play);
        }
        if (type == OBJ_TSUBO_TYPE_3) {
            func_8092762C(this, play);
        } else {
            func_80927690(this, play);
        }
        func_80927818(this, play, 1);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        if (ObjTsubo_IsSceneNotGohtOrTwinmold(this, play)) {
            Actor_Kill(&this->actor);
        } else {
            func_809291DC(this);
        }
    } else {
        if (!this->unk_195) {
            Actor_MoveWithGravity(&this->actor);
            Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 15.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                (DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId) == NULL)) {
                this->unk_195 = true;
                this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            }
        }
        if ((this->actor.xzDistToPlayer < 800.0f) || (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 1))) {
            Collider_UpdateCylinder(&this->actor, &this->cylinderCollider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinderCollider.base);
            if (this->actor.xzDistToPlayer < 150.0f) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinderCollider.base);
                if (this->actor.xzDistToPlayer < 100.0f) {
                    s16 yawDiff = this->actor.yawTowardsPlayer - GET_PLAYER(play)->actor.world.rot.y;
                    s32 absYawDiff = ABS_ALT(yawDiff);

                    if (absYawDiff > (0x10000 / 3)) {
                        Actor_OfferGetItem(&this->actor, play, GI_NONE, 36.0f, 30.0f);
                    }
                }
            }
        }
    }
}

void func_80928D6C(ObjTsubo* this) {
    this->actionFunc = func_80928D80;
}

void func_80928D80(ObjTsubo* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    s32 bgId;

    func_8092788C(this, play);
    if (Actor_HasNoParent(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        Actor_MoveWithGravity(&this->actor);
        this->actor.flags &= ~ACTOR_FLAG_CAN_PRESS_SWITCHES;
        Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 15.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                    UPDBGCHECKINFO_FLAG_80);
        func_80928E74(this);
    } else {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
    }
}

void func_80928E74(ObjTsubo* this) {
    this->unk_194 = 80;
    this->actor.colChkInfo.mass = 180;
    D_80929500 = (Rand_ZeroOne() - 0.78f) * 4500.0f;
    D_80929508 = (Rand_ZeroOne() - 0.5f) * 2000.0f;
    D_80929504 = 0;
    D_8092950C = 0;
    this->actionFunc = func_80928F18;
}

void func_80928F18(ObjTsubo* this, PlayState* play) {
    s32 pad;
    ObjTsuboData* typeData;
    s32 type = OBJ_TSUBO_GET_TYPE(&this->actor);
    s32 pad2;
    s32 atHit = (this->cylinderCollider.base.atFlags & AT_HIT) != 0;

    if (atHit) {
        this->cylinderCollider.base.atFlags &= ~AT_HIT;
    }
    func_8092788C(this, play);

    if (this->unk_194 > 0) {
        this->unk_194--;
    }
    typeData = &sPotTypeData[type];
    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) || atHit ||
        (this->unk_194 <= 0)) {
        typeData->breakPot1(this, play);
        if (type == OBJ_TSUBO_TYPE_3) {
            func_8092762C(this, play);
        } else {
            func_80927690(this, play);
        }
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        if (ObjTsubo_IsSceneNotGohtOrTwinmold(this, play)) {
            Actor_Kill(&this->actor);
            return;
        }

        func_809291DC(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        typeData->breakPot2(this, play);
        if (type == OBJ_TSUBO_TYPE_3) {
            func_8092762C(this, play);
        } else {
            func_80927690(this, play);
        }
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
        if (ObjTsubo_IsSceneNotGohtOrTwinmold(this, play)) {
            Actor_Kill(&this->actor);
            return;
        }

        func_809291DC(this);
    } else {
        Actor_MoveWithGravity(&this->actor);
        Math_StepToS(&D_80929504, D_80929500, 150);
        Math_StepToS(&D_8092950C, D_80929508, 150);
        this->actor.shape.rot.x += D_80929504;
        this->actor.shape.rot.y += D_8092950C;
        Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 15.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40 |
                                    UPDBGCHECKINFO_FLAG_80);
        Collider_UpdateCylinder(&this->actor, &this->cylinderCollider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinderCollider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinderCollider.base);
    }
}

void func_809291DC(ObjTsubo* this) {
    this->actor.draw = NULL;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.shape.rot.z = 0;
    this->actor.world.rot.z = 0;
    this->unk_197 = false;
    this->actor.shape.rot.x = this->actor.home.rot.x;
    this->actor.world.rot.x = this->actor.home.rot.x;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.shape.rot.y = this->actor.home.rot.y;
    this->actor.world.rot.y = this->actor.home.rot.y;
    Actor_SetScale(&this->actor, 0.0f);
    this->unk_194 = 127;
    this->actionFunc = func_8092926C;
}

void func_8092926C(ObjTsubo* this, PlayState* play) {
    f32 scale;

    if (this->unk_194 > 0) {
        this->unk_194--;
        if (this->unk_194 == 0) {
            if (GameInteractor_Should(VB_POT_DRAW_BE_OVERRIDDEN, true, this)) {
                this->actor.draw = ObjTsubo_Draw;
            }
        }
    } else {
        scale = sPotTypeData[OBJ_TSUBO_GET_TYPE(&this->actor)].scale;
        if (Math_StepToF(&this->actor.scale.x, scale, scale * 0.1f)) {
            this->actor.flags |= ACTOR_FLAG_CAN_PRESS_SWITCHES;
            func_809289B4(this);
        }
        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    }
}

void ObjTsubo_Update(Actor* thisx, PlayState* play) {
    ObjTsubo* this = (ObjTsubo*)thisx;

    this->actionFunc(this, play);
    if (this->actor.draw == NULL) {
        this->actor.shape.shadowDraw = NULL;
    } else if (this->actor.projectedPos.z < 811.0f) {
        if (this->actor.projectedPos.z > 300.0f) {
            this->actor.shape.shadowAlpha = (811 - (s32)this->actor.projectedPos.z) >> 1;
            this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        } else if (this->actor.projectedPos.z > -10.0f) {
            this->actor.shape.shadowAlpha = 255;
            this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        } else {
            this->actor.shape.shadowDraw = NULL;
        }
    } else {
        this->actor.shape.shadowDraw = NULL;
    }
    if (!this->unk_197) {
        if (this->unk_198) {
            play->actorCtx.flags |= ACTORCTX_FLAG_3;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
        if (this->unk_19A >= 0) {
            if (this->unk_19A == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
                if (Rand_ZeroOne() < 0.1f) {
                    this->unk_19A = Rand_S16Offset(40, 80);
                } else {
                    this->unk_19A = 8;
                }
            } else {
                this->unk_19A--;
            }
        }
    }
}

void ObjTsubo_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;

    Gfx_DrawDListOpa(play, sPotTypeData[OBJ_TSUBO_GET_TYPE(thisx)].modelDL);
}
