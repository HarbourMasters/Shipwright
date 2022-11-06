/*
 * File: z_bg_jya_1flift.c
 * Overlay: ovl_Bg_Jya_1flift
 * Description: Shortcut Elevator used in the vanilla version of the Spirit Temple
 */

#include "z_bg_jya_1flift.h"
#include "objects/object_jya_obj/object_jya_obj.h"

#define FLAGS ACTOR_FLAG_4

void BgJya1flift_Init(Actor* thisx, PlayState* play);
void BgJya1flift_Destroy(Actor* thisx, PlayState* play);
void BgJya1flift_Update(Actor* thisx, PlayState* play);
void BgJya1flift_Draw(Actor* thisx, PlayState* play);

void BgJya1flift_SetupWaitForSwitch(BgJya1flift* this);
void BgJya1flift_WaitForSwitch(BgJya1flift* this, PlayState* play);
void BgJya1flift_DoNothing(BgJya1flift* this, PlayState* play);
void BgJya1flift_ChangeDirection(BgJya1flift* this);
void BgJya1flift_Move(BgJya1flift* this, PlayState* play);
void BgJya1flift_SetupDoNothing(BgJya1flift* this);
void BgJya1flift_ResetMoveDelay(BgJya1flift* this);
void BgJya1flift_DelayMove(BgJya1flift* this, PlayState* play);

static u8 sKankyoIsSpawned = false;

const ActorInit Bg_Jya_1flift_InitVars = {
    ACTOR_BG_JYA_1FLIFT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_JYA_OBJ,
    sizeof(BgJya1flift),
    (ActorFunc)BgJya1flift_Init,
    (ActorFunc)BgJya1flift_Destroy,
    (ActorFunc)BgJya1flift_Update,
    (ActorFunc)BgJya1flift_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 70, 80, -82, { 0, 0, 0 } },
};

static f32 sFinalPositions[] = { 443.0f, -50.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

void BgJya1flift_InitDynapoly(BgJya1flift* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG login failed"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgJya1flift_InitCollision(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
}

void BgJya1flift_Init(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;
    // "1 F lift"
    osSyncPrintf("(１Ｆリフト)(flag %d)(room %d)\n", sKankyoIsSpawned, play->roomCtx.curRoom.num);
    this->hasInitialized = false;
    if (sKankyoIsSpawned) {
        Actor_Kill(thisx);
        return;
    }
    BgJya1flift_InitDynapoly(this, play, &g1fliftCol, 0);
    Actor_ProcessInitChain(thisx, sInitChain);
    BgJya1flift_InitCollision(thisx, play);
    if (Flags_GetSwitch(play, (thisx->params & 0x3F))) {
        LINK_AGE_IN_YEARS == YEARS_ADULT ? BgJya1flift_ChangeDirection(this) : BgJya1flift_SetupDoNothing(this);
    } else {
        BgJya1flift_SetupWaitForSwitch(this);
    }
    thisx->room = -1;
    sKankyoIsSpawned = true;
    this->hasInitialized = true;
}

void BgJya1flift_Destroy(Actor* thisx, PlayState* play) {
    BgJya1flift* this = (BgJya1flift*)thisx;

    if (this->hasInitialized) {
        sKankyoIsSpawned = false;
        Collider_DestroyCylinder(play, &this->collider);
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgJya1flift_SetupWaitForSwitch(BgJya1flift* this) {
    this->actionFunc = BgJya1flift_WaitForSwitch;
    this->dyna.actor.world.pos.y = sFinalPositions[0];
}

void BgJya1flift_WaitForSwitch(BgJya1flift* this, PlayState* play) {
    if (Flags_GetSwitch(play, (this->dyna.actor.params & 0x3F))) {
        BgJya1flift_ChangeDirection(this);
    }
}

void BgJya1flift_SetupDoNothing(BgJya1flift* this) {
    this->actionFunc = BgJya1flift_DoNothing;
    this->dyna.actor.world.pos.y = sFinalPositions[0];
}

void BgJya1flift_DoNothing(BgJya1flift* this, PlayState* play) {
}

void BgJya1flift_ChangeDirection(BgJya1flift* this) {
    this->actionFunc = BgJya1flift_Move;
    this->isMovingDown ^= true;
    this->dyna.actor.velocity.y = 0.0f;
}

void BgJya1flift_Move(BgJya1flift* this, PlayState* play) {
    f32 tempVelocity;

    Math_StepToF(&this->dyna.actor.velocity.y, 6.0f, 0.4f);
    if (this->dyna.actor.velocity.y < 1.0f) {
        tempVelocity = 1.0f;
    } else {
        tempVelocity = this->dyna.actor.velocity.y;
    }
    if (fabsf(Math_SmoothStepToF(&this->dyna.actor.world.pos.y, (sFinalPositions[this->isMovingDown]), 0.5f,
                                 tempVelocity, 1.0f)) < 0.001f) {
        this->dyna.actor.world.pos.y = sFinalPositions[this->isMovingDown];
        BgJya1flift_ResetMoveDelay(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE3 - SFX_FLAG);
    }
}

void BgJya1flift_ResetMoveDelay(BgJya1flift* this) {
    this->actionFunc = BgJya1flift_DelayMove;
    this->moveDelay = 0;
}

void BgJya1flift_DelayMove(BgJya1flift* this, PlayState* play) {
    this->moveDelay++;
    if (this->moveDelay >= 21) {
        BgJya1flift_ChangeDirection(this);
    }
}

void BgJya1flift_Update(Actor* thisx, PlayState* play2) {
    BgJya1flift* this = (BgJya1flift*)thisx;
    PlayState* play = play2;
    s32 tempIsRiding;

    // Room 0 is the first room and 6 is the room that the lift starts on
    if (play->roomCtx.curRoom.num == 6 || play->roomCtx.curRoom.num == 0) {
        this->actionFunc(this, play);
        tempIsRiding = func_8004356C(&this->dyna) ? true : false;
        if ((this->actionFunc == BgJya1flift_Move) || (this->actionFunc == BgJya1flift_DelayMove)) {
            if (tempIsRiding) {
                Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_FIRE_PLATFORM);
            } else if (!tempIsRiding && this->isLinkRiding) {
                Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            }
        }
        this->isLinkRiding = tempIsRiding;
        Collider_UpdateCylinder(thisx, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        Actor_Kill(thisx);
    }
}

void BgJya1flift_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, g1fliftDL);
}
