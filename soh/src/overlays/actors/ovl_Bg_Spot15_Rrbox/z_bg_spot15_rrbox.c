/*
 * File: z_bg_spot15_rrbox.c
 * Overlay: ovl_Bg_Spot15_Rrbox
 * Description: Milk Crate
 */

#include "z_bg_spot15_rrbox.h"
#include "objects/object_spot15_obj/object_spot15_obj.h"

#define FLAGS 0

void BgSpot15Rrbox_Init(Actor* thisx, PlayState* play);
void BgSpot15Rrbox_Destroy(Actor* thisx, PlayState* play);
void BgSpot15Rrbox_Update(Actor* thisx, PlayState* play);
void BgSpot15Rrbox_Draw(Actor* thisx, PlayState* play);

void func_808B4084(BgSpot15Rrbox* this, PlayState* play);
void func_808B40AC(BgSpot15Rrbox* this, PlayState* play);
void func_808B4194(BgSpot15Rrbox* this, PlayState* play);
void func_808B4380(BgSpot15Rrbox* this, PlayState* play);
void func_808B43D0(BgSpot15Rrbox* this, PlayState* play);
void func_808B44B8(BgSpot15Rrbox* this, PlayState* play);
void func_808B4178(BgSpot15Rrbox* this, PlayState* play);
void func_808B44CC(BgSpot15Rrbox* this, PlayState* play);

static s16 D_808B4590 = 0;

const ActorInit Bg_Spot15_Rrbox_InitVars = {
    ACTOR_BG_SPOT15_RRBOX,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT15_OBJ,
    sizeof(BgSpot15Rrbox),
    (ActorFunc)BgSpot15Rrbox_Init,
    (ActorFunc)BgSpot15Rrbox_Destroy,
    (ActorFunc)BgSpot15Rrbox_Update,
    (ActorFunc)BgSpot15Rrbox_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static Vec3f D_808B45C4[] = {
    { 770.0f, 1490.0f, -299.0f },
    { 770.0f, 1550.0f, -299.0f },
};

static Vec3f D_808B45DC[] = {
    { 29.99f, 0.01f, -29.99f }, { -29.99f, 0.01f, -29.99f }, { -29.99f, 0.01f, 29.99f },
    { 29.99f, 0.01f, 29.99f },  { 0.0f, 0.01f, 0.0f },
};

void func_808B3960(BgSpot15Rrbox* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    u32 pad2;

    DynaPolyActor_Init(&this->dyna, flags);
    CollisionHeader_GetVirtual(collision, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void func_808B39E8(Vec3f* arg0, Vec3f* arg1, f32 arg2, f32 arg3) {
    arg0->x = (arg1->z * arg2) + (arg1->x * arg3);
    arg0->y = arg1->y;
    arg0->z = (arg1->z * arg3) - (arg1->x * arg2);
}

void func_808B3A34(BgSpot15Rrbox* this) {
    this->bgId = BG_ACTOR_MAX;
}

s32 func_808B3A40(BgSpot15Rrbox* this, PlayState* play) {
    DynaPolyActor* dynaPolyActor = DynaPoly_GetActor(&play->colCtx, this->bgId);

    if ((dynaPolyActor != NULL) &&
        Math3D_Dist2DSq(dynaPolyActor->actor.world.pos.x, dynaPolyActor->actor.world.pos.z,
                        this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z) < 0.01f) {
        return true;
    }
    return false;
}

s32 func_808B3AAC(BgSpot15Rrbox* this, PlayState* play) {
    s16 rotY;
    Actor* actor = &this->dyna.actor;

    if (play->sceneNum == SCENE_SOUKO) {
        return true;
    } else if (func_808B3A40(this, play)) {
        return false;
    }

    if (actor->world.pos.x <= 930.0f && actor->world.pos.z >= -360.0f) {
        if (this->dyna.unk_150 >= 0.0f) {
            rotY = actor->world.rot.y;
        } else {
            rotY = actor->world.rot.y + 0x8000;
        }

        if (rotY < 0x2000 && rotY > -0x6000) {
            return gSaveContext.eventChkInf[1] & 0x10;
        }
        return true;
    }

    return true;
}

void BgSpot15Rrbox_Init(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    func_808B3960(this, play, &gLonLonMilkCrateCol, DPM_UNK);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    func_808B3A34(this);
    if (Flags_GetSwitch(play, (this->dyna.actor.params & 0x3F))) {
        func_808B44B8(this, play);
        this->dyna.actor.world.pos = D_808B45C4[D_808B4590];
        D_808B4590++;
    } else {
        func_808B4084(this, play);
    }
    osSyncPrintf("(spot15 ロンロン木箱)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void BgSpot15Rrbox_Destroy(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    D_808B4590 = 0;
}

s32 func_808B3CA0(BgSpot15Rrbox* this, PlayState* play, s32 arg2) {
    f32 chkDist = 0.0f;
    Vec3f actorPosition;
    Vec3f actorScale;

    func_808B3A34(this);

    actorScale.x = D_808B45DC[arg2].x * (this->dyna.actor.scale.x * 10.0f);
    actorScale.y = D_808B45DC[arg2].y * (this->dyna.actor.scale.y * 10.0f);
    actorScale.z = D_808B45DC[arg2].z * (this->dyna.actor.scale.z * 10.0f);

    func_808B39E8(&actorPosition, &actorScale, this->unk_16C, this->unk_170);

    actorPosition.x += this->dyna.actor.world.pos.x;
    actorPosition.y += this->dyna.actor.prevPos.y;
    actorPosition.z += this->dyna.actor.world.pos.z;

    this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor6(&play->colCtx, &this->dyna.actor.floorPoly,
                                                               &this->bgId, &this->dyna.actor, &actorPosition, chkDist);

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >= -0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        return true;
    }
    return false;
}

f32 func_808B3DDC(BgSpot15Rrbox* this, PlayState* play) {
    s32 i;
    Vec3f position;
    Vec3f scale;
    Actor* actor = &this->dyna.actor;
    f32 yIntersect;
    f32 returnValue = BGCHECK_Y_MIN;
    s32 bgId;

    func_808B3A34(this);
    for (i = 0; i < ARRAY_COUNT(D_808B45DC); i++) {
        scale.x = D_808B45DC[i].x * (actor->scale.x * 10.0f);
        scale.y = D_808B45DC[i].y * (actor->scale.y * 10.0f);
        scale.z = D_808B45DC[i].z * (actor->scale.z * 10.0f);

        func_808B39E8(&position, &scale, this->unk_16C, this->unk_170);

        position.x += actor->world.pos.x;
        position.y += actor->prevPos.y;
        position.z += actor->world.pos.z;

        yIntersect = BgCheck_EntityRaycastFloor6(&play->colCtx, &actor->floorPoly, &bgId, actor, &position, 0);

        if (returnValue < yIntersect) {
            returnValue = yIntersect;
            this->bgId = bgId;
        }
    }
    return returnValue;
}

s32 func_808B3F58(BgSpot15Rrbox* this, PlayState* play) {
    if (func_808B3CA0(this, play, 0)) {
        return true;
    }
    if (func_808B3CA0(this, play, 1)) {
        return true;
    }
    if (func_808B3CA0(this, play, 2)) {
        return true;
    }
    if (func_808B3CA0(this, play, 3)) {
        return true;
    }
    if (func_808B3CA0(this, play, 4)) {
        return true;
    }
    return false;
}

s32 func_808B4010(BgSpot15Rrbox* this, PlayState* play) {
    return !func_800435D8(play, &this->dyna, this->dyna.actor.scale.x * 290.0f,
                          this->dyna.actor.scale.x * 290.0f + 20.0f, 1.0f);
}

void func_808B4084(BgSpot15Rrbox* this, PlayState* play) {
    this->actionFunc = func_808B40AC;
    this->dyna.actor.gravity = 0.0f;
    this->dyna.actor.velocity.x = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.velocity.z = 0.0f;
}

void func_808B40AC(BgSpot15Rrbox* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_168 <= 0 && fabsf(this->dyna.unk_150) > 0.001f) {
        if (func_808B3AAC(this, play) && !func_808B4010(this, play)) {
            this->unk_17C = this->dyna.unk_150;
            func_808B4178(this, play);
        } else {
            player->stateFlags2 &= ~0x10;
            this->dyna.unk_150 = 0.0f;
        }
    } else {
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
    }
}

void func_808B4178(BgSpot15Rrbox* this, PlayState* play) {
    this->actionFunc = func_808B4194;
    this->dyna.actor.gravity = 0.0f;
}

void func_808B4194(BgSpot15Rrbox* this, PlayState* play) {
    f32 sign;
    Player* player = GET_PLAYER(play);
    f32 tempUnk178;
    s32 approxFResult;
    Actor* actor = &this->dyna.actor;

    this->unk_174 = this->unk_174 + ((CVar_GetS32("gFasterBlockPush", 0) / 2) * 0.5) + 0.5f;

    this->unk_174 = CLAMP_MAX(this->unk_174, 2.0f);

    approxFResult = Math_StepToF(&this->unk_178, 20.0f, this->unk_174);

    sign = this->unk_17C >= 0.0f ? 1.0f : -1.0f;

    tempUnk178 = (f32)sign * this->unk_178;
    actor->world.pos.x = actor->home.pos.x + (tempUnk178 * this->unk_16C);
    actor->world.pos.z = actor->home.pos.z + (tempUnk178 * this->unk_170);

    if (!func_808B3F58(this, play)) {
        actor->home.pos.x = actor->world.pos.x;
        actor->home.pos.z = actor->world.pos.z;
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
        this->unk_178 = 0.0f;
        this->unk_174 = 0.0f;
        func_808B4380(this, play);
    } else if (approxFResult) {
        player = GET_PLAYER(play);
        if (func_808B4010(this, play)) {
            Audio_PlayActorSound2(actor, NA_SE_EV_WOOD_BOUND);
        }
        if (func_808B3A40(this, play)) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
        }
        actor->home.pos.x = actor->world.pos.x;
        actor->home.pos.z = actor->world.pos.z;
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
        this->unk_178 = 0.0f;
        this->unk_174 = 0.0f;
        this->unk_168 = 10 - ((CVar_GetS32("gFasterBlockPush", 0) * 3) / 2);
        func_808B4084(this, play);
    }
    Audio_PlayActorSound2(actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void func_808B4380(BgSpot15Rrbox* this, PlayState* play) {
    this->dyna.actor.velocity.x = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.gravity = -1.0f;
    this->dyna.actor.floorHeight = func_808B3DDC(this, play);
    this->actionFunc = func_808B43D0;
}

void func_808B43D0(BgSpot15Rrbox* this, PlayState* play) {
    f32 floorHeight;
    Player* player = GET_PLAYER(play);
    Actor* actor = &this->dyna.actor;

    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~0x10;
    }

    Actor_MoveForward(actor);

    if (actor->world.pos.y <= BGCHECK_Y_MIN + 10.0f) {
        // "Lon Lon wooden crate fell too much"
        osSyncPrintf("Warning : ロンロン木箱落ちすぎた(%s %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     actor->params);

        Actor_Kill(actor);

        return;
    }

    floorHeight = actor->floorHeight;

    if ((floorHeight - actor->world.pos.y) >= -0.001f) {
        actor->world.pos.y = floorHeight;
        func_808B4084(this, play);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_WOOD_BOUND);
    }
}

void func_808B44B8(BgSpot15Rrbox* this, PlayState* play) {
    this->actionFunc = func_808B44CC;
}

void func_808B44CC(BgSpot15Rrbox* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 &= ~0x10;
    this->dyna.unk_150 = 0.0f;
}

void BgSpot15Rrbox_Update(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    if (this->unk_168 > 0) {
        this->unk_168--;
    }
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    this->unk_16C = Math_SinS(this->dyna.actor.world.rot.y);
    this->unk_170 = Math_CosS(this->dyna.actor.world.rot.y);
    this->actionFunc(this, play);
}

void BgSpot15Rrbox_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gLonLonMilkCrateDL);
}
