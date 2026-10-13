/*
 * File: z_bg_ctower_rot.c
 * Overlay: ovl_Bg_CtowerRot
 * Description: Twisting path along with the Stone Doors to Clock Tower
 */

#include "z_bg_ctower_rot.h"
#include "objects/object_ctower_rot/object_ctower_rot.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgCtowerRot_Init(Actor* thisx, PlayState* play);
void BgCtowerRot_Destroy(Actor* thisx, PlayState* play);
void BgCtowerRot_Update(Actor* thisx, PlayState* play);
void BgCtowerRot_Draw(Actor* thisx, PlayState* play);

void BgCtowerRot_CorridorRotate(BgCtowerRot* this, PlayState* play);
void BgCtowerRot_DoorDoNothing(BgCtowerRot* this, PlayState* play);
void BgCtowerRot_DoorIdle(BgCtowerRot* this, PlayState* play);
void BgCtowerRot_SetupDoorClose(BgCtowerRot* this, PlayState* play);

ActorProfile Bg_Ctower_Rot_Profile = {
    /**/ ACTOR_BG_CTOWER_ROT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_CTOWER_ROT,
    /**/ sizeof(BgCtowerRot),
    /**/ BgCtowerRot_Init,
    /**/ BgCtowerRot_Destroy,
    /**/ BgCtowerRot_Update,
    /**/ BgCtowerRot_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Gfx* sDLists[] = { gClockTowerCorridorDL, gClockTowerStoneDoorMainDL, gClockTowerStoneDoorDL };

void BgCtowerRot_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgCtowerRot* this = (BgCtowerRot*)thisx;
    Player* player;
    Vec3f offset;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    if (this->dyna.actor.params == BGCTOWERROT_CORRIDOR) {
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTowerCorridorCol);
        this->actionFunc = BgCtowerRot_CorridorRotate;
        return;
    }

    player = GET_PLAYER(play);
    if (this->dyna.actor.params == BGCTOWERROT_STONE_DOOR_MAIN) {
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTowerStoneDoorMainCol);
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y + 0x4000;
    } else {
        DynaPolyActor_LoadMesh(play, &this->dyna, &gClockTowerStoneDoorCol);
        this->dyna.actor.world.rot.y = this->dyna.actor.shape.rot.y - 0x4000;
    }
    Actor_WorldToActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
    if (offset.z < 0.0f) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + (Math_SinS(this->dyna.actor.world.rot.y) * 80.0f);
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + (Math_CosS(this->dyna.actor.world.rot.y) * 80.0f);
        this->timer = 80.0f;
        this->actionFunc = BgCtowerRot_DoorIdle;
    } else {
        this->actionFunc = BgCtowerRot_DoorDoNothing;
    }
}

void BgCtowerRot_Destroy(Actor* thisx, PlayState* play) {
    BgCtowerRot* this = (BgCtowerRot*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgCtowerRot_CorridorRotate(BgCtowerRot* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f offset;
    f32 rotZ;

    Actor_WorldToActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
    rotZ = CLAMP(1100.0f - offset.z, 0.0f, 1000.0f);
    Camera_ChangeSetting(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
    this->dyna.actor.shape.rot.z = rotZ * 16.384f;

    if (play->csCtx.curFrame == 132) {
        Audio_PlaySfx(NA_SE_SY_SPIRAL_DASH);
    }
}

void BgCtowerRot_DoorDoNothing(BgCtowerRot* this, PlayState* play) {
}

void BgCtowerRot_DoorClose(BgCtowerRot* this, PlayState* play) {
    if (!Math_SmoothStepToF(&this->timer, 0.0f, 0.1f, 15.0f, 0.1f)) {
        if (this->dyna.actor.params == BGCTOWERROT_STONE_DOOR_MAIN) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
            CutsceneManager_Stop(this->dyna.actor.csId);
        }
        this->actionFunc = BgCtowerRot_DoorDoNothing;
    } else if (this->dyna.actor.params == BGCTOWERROT_STONE_DOOR_MAIN) {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_STONE_STATUE_OPEN - SFX_FLAG);
    }
    this->dyna.actor.world.pos.x =
        this->dyna.actor.home.pos.x + (Math_SinS(this->dyna.actor.world.rot.y) * this->timer);
    this->dyna.actor.world.pos.z =
        this->dyna.actor.home.pos.z + (Math_CosS(this->dyna.actor.world.rot.y) * this->timer);
}

void BgCtowerRot_DoorIdle(BgCtowerRot* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f offset;

    Actor_WorldToActorCoords(&this->dyna.actor, &offset, &player->actor.world.pos);
    if (offset.z > 30.0f) {
        this->unk160 = 0.0f;
        CutsceneManager_Queue(this->dyna.actor.csId);
        this->actionFunc = BgCtowerRot_SetupDoorClose;
    }
}

void BgCtowerRot_SetupDoorClose(BgCtowerRot* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        if (this->dyna.actor.params == BGCTOWERROT_STONE_DOOR_MAIN) {
            CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        }
        this->actionFunc = BgCtowerRot_DoorClose;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgCtowerRot_Update(Actor* thisx, PlayState* play) {
    BgCtowerRot* this = (BgCtowerRot*)thisx;

    this->actionFunc(this, play);
}

void BgCtowerRot_Draw(Actor* thisx, PlayState* play) {
    BgCtowerRot* this = (BgCtowerRot*)thisx;

    Gfx_DrawDListOpa(play, sDLists[this->dyna.actor.params]);
    if (this->dyna.actor.params == BGCTOWERROT_CORRIDOR) {
        Gfx_DrawDListXlu(play, gClockTowerCorridorFoliageDL);
    }
}
