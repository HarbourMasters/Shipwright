/*
 * File: z_item_b_heart.c
 * Overlay: ovl_Item_B_Heart
 * Description: Heart Container
 */

#include "z_item_b_heart.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"

#define FLAGS 0

void ItemBHeart_Init(Actor* thisx, PlayState* play);
void ItemBHeart_Destroy(Actor* thisx, PlayState* play);
void ItemBHeart_Update(Actor* thisx, PlayState* play);
void ItemBHeart_Draw(Actor* thisx, PlayState* play);

void func_80B85264(ItemBHeart* this, PlayState* play);

const ActorInit Item_B_Heart_InitVars = {
    ACTOR_ITEM_B_HEART,
    ACTORCAT_MISC,
    FLAGS,
    OBJECT_GI_HEARTS,
    sizeof(ItemBHeart),
    (ActorFunc)ItemBHeart_Init,
    (ActorFunc)ItemBHeart_Destroy,
    (ActorFunc)ItemBHeart_Update,
    (ActorFunc)ItemBHeart_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

static ColliderCylinderInit sUselessCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 30, 40, 0, { 0 } },
};

void ItemBHeart_Init(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;

    if (Flags_GetCollectible(play, 0x1F)) {
        Actor_Kill(&this->actor);
    } else {
        Actor_ProcessInitChain(&this->actor, sInitChain);
        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.8f);
    }

    if (CVarGetInteger("gCollisionGoggles", 0)) {
        this->useUselessCylinder = 1;
    } else {
        this->useUselessCylinder = 0;
    }
    if (this->useUselessCylinder) {
        Collider_InitCylinder(play, &this->uselessCollider);
        Collider_SetCylinder(play, &this->uselessCollider, &this->actor, &sUselessCylinderInit);
    }
}

void ItemBHeart_Destroy(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;

    if (this->useUselessCylinder) {
        Collider_DestroyCylinder(play, &this->uselessCollider);
    }
}

void ItemBHeart_Update(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;

    func_80B85264(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    if (Actor_HasParent(&this->actor, play)) {
        Flags_SetCollectible(play, 0x1F);
        Actor_Kill(&this->actor);
    } else {
        if (!IS_RANDO) {
            func_8002F434(&this->actor, play, GI_HEART_CONTAINER_2, 30.0f, 40.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromActor(this->actor.id, play->sceneNum, this->actor.params, GI_HEART_CONTAINER_2);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 30.0f, 40.0f);
        }
    }
    if (this->useUselessCylinder) {
        Collider_UpdateCylinder(&this->actor, &this->uselessCollider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->uselessCollider.base);
    }
}

void func_80B85264(ItemBHeart* this, PlayState* play) {
    f32 yOffset;

    this->unk_164++;
    yOffset = (Math_SinS(this->unk_164 * 0x60C) * 5.0f) + 20.0f;
    Math_ApproachF(&this->actor.world.pos.y, this->actor.home.pos.y + yOffset, 0.1f, this->unk_158);
    Math_ApproachF(&this->unk_158, 2.0f, 1.0f, 0.1f);
    this->actor.shape.rot.y += 0x400;

    Math_ApproachF(&this->actor.scale.x, 0.4f, 0.1f, 0.01f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
}

void ItemBHeart_Draw(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;
    Actor* actorIt;
    u8 flag = false;

    OPEN_DISPS(play->state.gfxCtx);

    actorIt = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (actorIt != NULL) {
        if ((actorIt->id == ACTOR_DOOR_WARP1) && (actorIt->projectedPos.z > this->actor.projectedPos.z)) {
            flag = true;
            break;
        }
        actorIt = actorIt->next;
    }

    if (IS_RANDO) {
        GetItemEntry_Draw(play, Randomizer_GetItemFromActor(this->actor.id, 
            play->sceneNum,this->actor.params, GI_HEART_CONTAINER_2));
    } else {
        if (flag) {
            Gfx_SetupDL_25Xlu(play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gGiHeartBorderDL);
            gSPDisplayList(POLY_XLU_DISP++, gGiHeartContainerDL);
        } else {
            Gfx_SetupDL_25Opa(play->state.gfxCtx);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gGiHeartBorderDL);
            gSPDisplayList(POLY_OPA_DISP++, gGiHeartContainerDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
