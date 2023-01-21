/*
 * File: z_item_shield.c
 * Overlay: ovl_Item_Shield
 * Description: Deku Shield
 */

#include "vt.h"
#include "z_item_shield.h"
#include "objects/object_link_child/object_link_child.h"

#define FLAGS ACTOR_FLAG_4

void ItemShield_Init(Actor* thisx, PlayState* play);
void ItemShield_Destroy(Actor* thisx, PlayState* play);
void ItemShield_Update(Actor* thisx, PlayState* play);
void ItemShield_Draw(Actor* thisx, PlayState* play);

void func_80B86F68(ItemShield* this, PlayState* play);
void func_80B86BC8(ItemShield* this, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000004, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 15, 0, { 0, 0, 0 } },
};

const ActorInit Item_Shield_InitVars = {
    ACTOR_ITEM_SHIELD,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_LINK_CHILD,
    sizeof(ItemShield),
    (ActorFunc)ItemShield_Init,
    (ActorFunc)ItemShield_Destroy,
    (ActorFunc)ItemShield_Update,
    (ActorFunc)ItemShield_Draw,
    NULL,
};

static Color_RGBA8 unused = { 255, 255, 0, 255 };
static Color_RGBA8 unused2 = { 255, 0, 0, 255 };

void ItemShield_SetupAction(ItemShield* this, ItemShieldActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ItemShield_Init(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;
    s32 i;

    this->timer = 0;
    this->unk_19C = 0;

    switch (this->actor.params) {
        case 0:
            ActorShape_Init(&this->actor.shape, 1400.0f, NULL, 0.0f);
            this->actor.shape.rot.x = 0x4000;
            ItemShield_SetupAction(this, func_80B86BC8);
            break;

        case 1:
            ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
            ItemShield_SetupAction(this, func_80B86F68);
            this->unk_19C |= 2;
            for (i = 0; i < 8; i++) {
                this->unk_19E[i] = 1 + 2 * i;
                this->unk_1A8[i].x = Rand_CenteredFloat(10.0f);
                this->unk_1A8[i].y = Rand_CenteredFloat(10.0f);
                this->unk_1A8[i].z = Rand_CenteredFloat(10.0f);
            }
            break;
    }

    Actor_SetScale(&this->actor, 0.01f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    osSyncPrintf(VT_FGCOL(GREEN) "Item_Shild %d \n" VT_RST, this->actor.params);
}

void ItemShield_Destroy(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80B86AC8(ItemShield* this, PlayState* play) {
    Actor_MoveForward(&this->actor);
    if (Actor_HasParent(&this->actor, play)) {
        Actor_Kill(&this->actor);
        return;
    }
    func_8002F434(&this->actor, play, GI_SHIELD_DEKU, 30.0f, 50.0f);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f, 5);
    if (this->actor.bgCheckFlags & 1) {
        this->timer--;
        if (this->timer < 60) {
            if (this->timer & 1) {
                this->unk_19C |= 2;
            } else {
                this->unk_19C &= ~2;
            }
        }
        if (this->timer == 0) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80B86BC8(ItemShield* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        Actor_Kill(&this->actor);
        return;
    }
    func_8002F434(&this->actor, play, GI_SHIELD_DEKU, 30.0f, 50.0f);
    if (this->collider.base.acFlags & AC_HIT) {
        ItemShield_SetupAction(this, func_80B86AC8);
        this->actor.velocity.y = 4.0f;
        this->actor.minVelocityY = -4.0f;
        this->actor.gravity = -0.8f;
        this->actor.speedXZ = 0.0f;
        this->timer = 160;
    } else {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80B86CA8(ItemShield* this, PlayState* play) {
    static Vec3f D_80B871F4 = { 0.0f, 0.0f, 0.0f };
    static f32 D_80B87200[] = { 0.3f, 0.6f,  0.9f, 1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
                                1.0f, 0.85f, 0.7f, 0.55f, 0.4f, 0.25f, 0.1f, 0.0f };
    static f32 D_80B87240[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f,
                                0.6f, 0.4f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    s32 i;
    s32 temp;

    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f, 5);
    this->actor.shape.yOffset = ABS(Math_SinS(this->actor.shape.rot.x)) * 1500.0f;

    for (i = 0; i < 8; i++) {
        temp = 15 - this->unk_19E[i];
        D_80B871F4.x = this->unk_1A8[i].x;
        D_80B871F4.y = this->unk_1A8[i].y + (this->actor.shape.yOffset * 0.01f) + (D_80B87200[temp] * -10.0f * 0.2f);
        D_80B871F4.z = this->unk_1A8[i].z;
        EffectSsFireTail_SpawnFlame(play, &this->actor, &D_80B871F4, D_80B87200[temp] * 0.2f, -1,
                                    D_80B87240[temp]);
        if (this->unk_19E[i] != 0) {
            this->unk_19E[i]--;
        } else if (this->timer > 16) {
            this->unk_19E[i] = 15;
            this->unk_1A8[i].x = Rand_CenteredFloat(15.0f);
            this->unk_1A8[i].y = Rand_CenteredFloat(10.0f);
            this->unk_1A8[i].z = Rand_CenteredFloat(15.0f);
        }
    }
    if (this->actor.bgCheckFlags & 1) {
        this->unk_198 -= this->actor.shape.rot.x >> 1;
        this->unk_198 -= this->unk_198 >> 2;
        this->actor.shape.rot.x += this->unk_198;
        if ((this->timer >= 8) && (this->timer < 24)) {
            Actor_SetScale(&this->actor, (this->timer - 8) * 0.000625f);
        }
        if (this->timer != 0) {
            this->timer--;
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80B86F68(ItemShield* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    MtxF* shield = &player->shieldMf;

    this->actor.world.pos.x = shield->xw;
    this->actor.world.pos.y = shield->yw;
    this->actor.world.pos.z = shield->zw;
    this->unk_19C &= ~2;

    this->actor.shape.rot.y = Math_Atan2S(-shield->zz, -shield->xz);
    this->actor.shape.rot.x = Math_Atan2S(-shield->yz, sqrtf(shield->zz * shield->zz + shield->xz * shield->xz));

    if (ABS(this->actor.shape.rot.x) > 0x4000) {
        this->unk_19C |= 1;
    }

    ItemShield_SetupAction(this, func_80B86CA8);

    this->actor.velocity.y = 4.0;
    this->actor.minVelocityY = -4.0;
    this->actor.gravity = -0.8;
    this->unk_198 = 0;
    this->timer = 70;
    this->actor.speedXZ = 0;
}

void ItemShield_Update(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    this->actionFunc(this, play);
}

void ItemShield_Draw(Actor* thisx, PlayState* play) {
    ItemShield* this = (ItemShield*)thisx;

    if (!(this->unk_19C & 2)) {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gLinkChildDekuShieldDL));
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
