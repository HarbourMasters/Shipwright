/*
 * File: z_bg_hidan_hrock.c
 * Overlay: ovl_Bg_Hidan_Hrock
 * Description: Huge stone spike platform (Fire Temple)
 */

#include "z_bg_hidan_hrock.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanHrock_Init(Actor* thisx, PlayState* play);
void BgHidanHrock_Destroy(Actor* thisx, PlayState* play);
void BgHidanHrock_Update(Actor* thisx, PlayState* play);
void BgHidanHrock_Draw(Actor* thisx, PlayState* play);

void func_8088960C(BgHidanHrock* this, PlayState* play);
void func_808896B8(BgHidanHrock* this, PlayState* play);
void func_808894A4(BgHidanHrock* this, PlayState* play);

const ActorInit Bg_Hidan_Hrock_InitVars = {
    ACTOR_BG_HIDAN_HROCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanHrock),
    (ActorFunc)BgHidanHrock_Init,
    (ActorFunc)BgHidanHrock_Destroy,
    (ActorFunc)BgHidanHrock_Update,
    (ActorFunc)BgHidanHrock_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, 40.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, -40.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_STOP),
};

void BgHidanHrock_Init(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;
    ColliderTrisElementInit* colliderElementInit;
    Vec3f vertices[3];
    f32 cosRotY;
    f32 sinRotY;
    s32 i;
    s32 j;
    CollisionHeader* collisionHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->unk_16A = thisx->params & 0x3F;
    thisx->params = (thisx->params >> 8) & 0xFF;
    Collider_InitTris(play, &this->collider);
    Collider_SetTris(play, &this->collider, thisx, &sTrisInit, this->colliderItems);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);

    sinRotY = Math_SinS(thisx->shape.rot.y);
    cosRotY = Math_CosS(thisx->shape.rot.y);

    if (thisx->params == 0) {
        sinRotY *= 1.5f;
        cosRotY *= 1.5f;
    }

    for (i = 0; i < 2; i++) {
        colliderElementInit = &sTrisInit.elements[i];

        if (1) {
            for (j = 0; j < 3; j++) {
                Vec3f* vtx = &colliderElementInit->dim.vtx[j];

                vertices[j].x = vtx->z * sinRotY + (thisx->home.pos.x + vtx->x * cosRotY);
                vertices[j].y = vtx->y + thisx->home.pos.y;
                vertices[j].z = vtx->z * cosRotY + (thisx->home.pos.z - vtx->x * sinRotY);
            }
        }
        Collider_SetTrisVertices(&this->collider, i, &vertices[0], &vertices[1], &vertices[2]);
    }

    if (Flags_GetSwitch(play, this->unk_16A)) {
        this->actionFunc = func_808894A4;
        if (thisx->params == 0) {
            thisx->world.pos.y -= 2800.0f;
            thisx->uncullZoneForward = 3000.0f;
        } else if (thisx->params == 1) {
            thisx->world.pos.y -= 800.0f;
        } else if (thisx->params == 2) {
            thisx->world.pos.y -= 240.0f;
        }
    } else {
        if (thisx->params == 0) {
            thisx->flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED;
            thisx->uncullZoneForward = 3000.0f;
        }
        this->actionFunc = func_808896B8;
    }

    if (thisx->params == 0) {
        CollisionHeader_GetVirtual(&gFireTempleTallestPillarAboveRoomBeforeBossCol, &collisionHeader);
    } else {
        CollisionHeader_GetVirtual(&gFireTemplePillarInsertedInGroundCol, &collisionHeader);
    }

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, collisionHeader);
}

void BgHidanHrock_Destroy(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->collider);
}

void func_808894A4(BgHidanHrock* this, PlayState* play) {
}

void func_808894B0(BgHidanHrock* this, PlayState* play) {
    if (this->unk_168 != 0) {
        this->unk_168--;
    }

    this->dyna.actor.world.pos.x =
        (Math_SinS(this->dyna.actor.world.rot.y + (this->unk_168 << 0xE)) * 5.0f) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z =
        (Math_CosS(this->dyna.actor.world.rot.y + (this->unk_168 << 0xE)) * 5.0f) + this->dyna.actor.home.pos.z;

    if (!(this->unk_168 % 4)) {
        func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }

    if (this->unk_168 == 0) {
        if (this->dyna.actor.params == 0) {
            this->dyna.actor.home.pos.y -= 2800.0f;
        } else if (this->dyna.actor.params == 1) {
            this->dyna.actor.home.pos.y -= 800.0f;
        } else {
            this->dyna.actor.home.pos.y -= 240.0f;
        }

        this->actionFunc = func_8088960C;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
}

void func_8088960C(BgHidanHrock* this, PlayState* play) {
    this->dyna.actor.velocity.y++;

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        this->dyna.actor.flags &= ~(ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);

        if (this->dyna.actor.params == 0) {
            if (play->roomCtx.curRoom.num == 10) {
                this->dyna.actor.room = 10;
            } else {
                Actor_Kill(&this->dyna.actor);
            }
        }

        this->actionFunc = func_808894A4;
    }
}

void func_808896B8(BgHidanHrock* this, PlayState* play) {
    // #region SOH [Co-op]
    if ((this->collider.base.acFlags & 2) || Flags_GetSwitch(play, this->unk_16A)) {
    // #endregion
        this->collider.base.acFlags &= ~2;
        this->actionFunc = func_808894B0;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;

        if (this->dyna.actor.params == 0) {
            this->dyna.actor.room = -1;
        }

        this->unk_168 = 20;
        Flags_SetSwitch(play, this->unk_16A);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (func_8004356C(&this->dyna)) {
        Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 5.0f, 1.0f);
    } else {
        Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f);
    }
}

void BgHidanHrock_Update(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;

    this->actionFunc(this, play);
}

void BgHidanHrock_Draw(Actor* thisx, PlayState* play) {
    static Gfx* dlists[] = {
        gFireTempleTallestPillarAboveRoomBeforeBossDL,
        gFireTemplePillarInsertedInGroundDL,
        gFireTemplePillarInsertedInGroundDL,
    };

    Gfx_DrawDListOpa(play, dlists[thisx->params]);
}
