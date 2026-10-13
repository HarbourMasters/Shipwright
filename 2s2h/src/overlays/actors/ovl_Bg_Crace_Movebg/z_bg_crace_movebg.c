/*
 * File: z_bg_crace_movebg.c
 * Overlay: ovl_Bg_Crace_Movebg
 * Description: Huge sliding doors in Deku Shrine
 */

#include "z_bg_crace_movebg.h"
#include "overlays/actors/ovl_En_Dno/z_en_dno.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgCraceMovebg_Init(Actor* thisx, PlayState* play);
void BgCraceMovebg_Destroy(Actor* thisx, PlayState* play);
void BgCraceMovebg_Update(Actor* thisx, PlayState* play);
void BgCraceMovebg_Draw(Actor* thisx, PlayState* play);
void BgCraceMovebg_Reset(void);

s32 BgCraceMovebg_GetRaceStatus(PlayState* play);
void BgCraceMovebg_OpeningDoor_SetupIdle(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_OpeningDoor_Idle(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_OpeningDoor_SetupOpen(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_OpeningDoor_Open(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_OpeningDoor_SetupDoNothing(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_OpeningDoor_DoNothing(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_CheckIfPlayerIsBeyondDoor(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_SetupIdle(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_Idle(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_SetupWaitToClose(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_WaitToClose(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_SetupClose(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_Close(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_SetupDoNothing(BgCraceMovebg* this, PlayState* play);
void BgCraceMovebg_ClosingDoor_DoNothing(BgCraceMovebg* this, PlayState* play);

typedef enum {
    /* 0 */ BG_CRACE_MOVEBG_RACE_STATUS_BUTLER_NOT_PRESENT,
    /* 1 */ BG_CRACE_MOVEBG_RACE_STATUS_FIRST_RACE,
    /* 2 */ BG_CRACE_MOVEBG_RACE_STATUS_SUBSEQUENT_RACE
} BgCraceMovebgRaceStatus;

u8 sIsLoaded[32];

ActorProfile Bg_Crace_Movebg_Profile = {
    /**/ ACTOR_BG_CRACE_MOVEBG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_CRACE_OBJECT,
    /**/ sizeof(BgCraceMovebg),
    /**/ BgCraceMovebg_Init,
    /**/ BgCraceMovebg_Destroy,
    /**/ BgCraceMovebg_Update,
    /**/ BgCraceMovebg_Draw,
    /**/ BgCraceMovebg_Reset,
};

static u8 sHasInitializedIsLoaded = 0;
static u8 sLoadedDoorCount = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static Vec3f sUnitVecZ = { 0.0f, 0.0f, 1.0f };

void BgCraceMovebg_Init(Actor* thisx, PlayState* play) {
    BgCraceMovebg* this = (BgCraceMovebg*)thisx;
    s32 i;
    s32 j;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (!sHasInitializedIsLoaded) {
        for (i = 0; i < ARRAY_COUNT(sIsLoaded); i++) {
            sIsLoaded[i] = 0;
        }
        sHasInitializedIsLoaded = true;
    }

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gDekuShrineSlidingDoorCol);

    this->index = BG_CRACE_MOVEBG_GET_INDEX(&this->dyna.actor);
    this->openSpeed = BG_CRACE_MOVEBG_GET_OPEN_SPEED(&this->dyna.actor);
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.world.rot.z = 0;
    this->dyna.actor.home.rot.x = 0;
    this->dyna.actor.home.rot.z = 0;
    this->dyna.actor.flags |= ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED;

    switch (BG_CRACE_MOVEBG_GET_TYPE(&this->dyna.actor)) {
        case BG_CRACE_MOVEBG_TYPE_CLOSING:
        case BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING:
            for (j = 0; j < sLoadedDoorCount; j++) {
                if (sIsLoaded[j] == this->index) {
                    this->stateFlags |= BG_CRACE_MOVEBG_FLAG_ALREADY_LOADED;
                    Actor_Kill(&this->dyna.actor);
                    return;
                }
            }

            if (sLoadedDoorCount < ARRAY_COUNT(sIsLoaded)) {
                sIsLoaded[(s32)sLoadedDoorCount] = this->index;
                sLoadedDoorCount++;
            }

            this->dyna.actor.room = -1;
            switch (BgCraceMovebg_GetRaceStatus(play)) {
                case BG_CRACE_MOVEBG_RACE_STATUS_BUTLER_NOT_PRESENT:
                    BgCraceMovebg_ClosingDoor_SetupIdle(this, play);
                    break;

                case BG_CRACE_MOVEBG_RACE_STATUS_FIRST_RACE:
                    BgCraceMovebg_ClosingDoor_SetupIdle(this, play);
                    break;

                case BG_CRACE_MOVEBG_RACE_STATUS_SUBSEQUENT_RACE:
                    // Makes sure no lingering switch flags are set from the last time the player did a race.
                    Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor) + 1);
                    BgCraceMovebg_ClosingDoor_SetupIdle(this, play);
                    break;
            }
            break;

        case BG_CRACE_MOVEBG_TYPE_OPENING:
            BgCraceMovebg_OpeningDoor_SetupIdle(this, play);
            break;

        default:
            Actor_Kill(&this->dyna.actor);
            break;
    }
}

s32 BgCraceMovebg_GetRaceStatus(PlayState* play) {
    s32 pad;
    s32 raceStatus = BG_CRACE_MOVEBG_RACE_STATUS_BUTLER_NOT_PRESENT;
    Actor* butler = SubS_FindActor(play, NULL, ACTORCAT_NPC, ACTOR_EN_DNO);

    if (butler != NULL) {
        if (Flags_GetSwitch(play, EN_DNO_GET_RACE_STARTED_SWITCH_FLAG(butler))) {
            raceStatus = BG_CRACE_MOVEBG_RACE_STATUS_SUBSEQUENT_RACE;
        } else {
            raceStatus = BG_CRACE_MOVEBG_RACE_STATUS_FIRST_RACE;
        }
    }

    return raceStatus;
}

void BgCraceMovebg_OpeningDoor_SetupIdle(BgCraceMovebg* this, PlayState* play) {
    this->targetDoorHeight = 0.0f;
    this->doorHeight = 0.0f;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    this->actionFunc = BgCraceMovebg_OpeningDoor_Idle;
}

/**
 * Wait for this door's switch flag to be set, then start opening.
 */
void BgCraceMovebg_OpeningDoor_Idle(BgCraceMovebg* this, PlayState* play) {
    if (Flags_GetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor))) {
        BgCraceMovebg_OpeningDoor_SetupOpen(this, play);
    }
}

void BgCraceMovebg_OpeningDoor_SetupOpen(BgCraceMovebg* this, PlayState* play) {
    this->targetDoorHeight = 180.0f;
    this->actionFunc = BgCraceMovebg_OpeningDoor_Open;
}

/**
 * Silde open, then do nothing.
 */
void BgCraceMovebg_OpeningDoor_Open(BgCraceMovebg* this, PlayState* play) {
    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_STONEDOOR_OPEN_S - SFX_FLAG);
    Math_SmoothStepToF(&this->doorHeight, this->targetDoorHeight, 2.0f, this->openSpeed, 0.01f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->doorHeight;
    if (this->doorHeight == this->targetDoorHeight) {
        BgCraceMovebg_OpeningDoor_SetupDoNothing(this, play);
    }
}

void BgCraceMovebg_OpeningDoor_SetupDoNothing(BgCraceMovebg* this, PlayState* play) {
    this->actionFunc = BgCraceMovebg_OpeningDoor_DoNothing;
}

void BgCraceMovebg_OpeningDoor_DoNothing(BgCraceMovebg* this, PlayState* play) {
}

void BgCraceMovebg_Destroy(Actor* thisx, PlayState* play) {
    BgCraceMovebg* this = (BgCraceMovebg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    if (!(this->stateFlags & BG_CRACE_MOVEBG_FLAG_ALREADY_LOADED)) {
        switch (BG_CRACE_MOVEBG_GET_TYPE(&this->dyna.actor)) {
            case BG_CRACE_MOVEBG_TYPE_CLOSING:
            case BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING:
                Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(thisx));
                Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(thisx) + 1);
                break;

            case BG_CRACE_MOVEBG_TYPE_OPENING:
                Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(thisx));
                break;
        }
    }
}

void BgCraceMovebg_Update(Actor* thisx, PlayState* play) {
    BgCraceMovebg* this = (BgCraceMovebg*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    switch (BG_CRACE_MOVEBG_GET_TYPE(&this->dyna.actor)) {
        case BG_CRACE_MOVEBG_TYPE_CLOSING:
        case BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING:
            yawDiff = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.home.rot.y;
            if ((yawDiff >= -0x4000) && (yawDiff <= 0x4000)) {
                this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
            } else {
                this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y + 0x8000;
            }
            break;

        default:
            break;
    }

    this->actionFunc(this, play);
    Math_Vec3f_Copy(&this->prevPlayerPos, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);
}

/**
 * Updates this door's flags if the player has moved beyond it. If the door is of type
 * BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING, then nothing happens here, so the door never
 * knows the player has moved beyond it.
 */
void BgCraceMovebg_ClosingDoor_CheckIfPlayerIsBeyondDoor(BgCraceMovebg* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f intersect;
    Vec3f posDiff;

    if ((BG_CRACE_MOVEBG_GET_TYPE(&this->dyna.actor) != BG_CRACE_MOVEBG_TYPE_UNUSED_CLOSING) &&
        SubS_LineSegVsPlane(&this->dyna.actor.home.pos, &this->dyna.actor.home.rot, &sUnitVecZ, &this->prevPlayerPos,
                            &player->bodyPartsPos[PLAYER_BODYPART_WAIST], &intersect)) {
        Matrix_RotateYS(-this->dyna.actor.home.rot.y, MTXMODE_NEW);
        Math_Vec3f_Diff(&player->bodyPartsPos[PLAYER_BODYPART_WAIST], &this->dyna.actor.home.pos, &posDiff);
        Matrix_MultVec3f(&posDiff, &this->intersectionOffsetFromHome);

        if ((fabsf(this->intersectionOffsetFromHome.x) < 100.0f) && (this->intersectionOffsetFromHome.y >= -10.0f) &&
            (this->intersectionOffsetFromHome.y <= 180.0f)) {
            if (this->intersectionOffsetFromHome.z < 0.0f) {
                Flags_SetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor) + 1);
                this->stateFlags |= BG_CRACE_MOVEBG_FLAG_PLAYER_IS_BEYOND_DOOR;
            } else {
                Flags_UnsetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor) + 1);
                this->stateFlags &= ~BG_CRACE_MOVEBG_FLAG_PLAYER_IS_BEYOND_DOOR;
            }
        }
    }
}

void BgCraceMovebg_ClosingDoor_SetupIdle(BgCraceMovebg* this, PlayState* play) {
    this->targetDoorHeight = 180.0f;
    this->doorHeight = 180.0f;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 180.0f;
    this->actionFunc = BgCraceMovebg_ClosingDoor_Idle;
}

/**
 * Wait for the Deku Butler to move beyond the door, or wait for this door's switch flag
 * to be set. In either case, start closing; how long to wait before closing depends on
 * what triggered it to close.
 */
void BgCraceMovebg_ClosingDoor_Idle(BgCraceMovebg* this, PlayState* play) {
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->doorHeight;
    BgCraceMovebg_ClosingDoor_CheckIfPlayerIsBeyondDoor(this, play);

    if (this->stateFlags & BG_CRACE_MOVEBG_FLAG_BUTLER_IS_BEYOND_DOOR) {
        BgCraceMovebg_ClosingDoor_SetupWaitToClose(this, play);
    }

    if (Flags_GetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor))) {
        BgCraceMovebg_ClosingDoor_SetupClose(this, play);
    }
}

void BgCraceMovebg_ClosingDoor_SetupWaitToClose(BgCraceMovebg* this, PlayState* play) {
    this->closeWaitTimer = BG_CRACE_MOVEBG_GET_CLOSE_WAIT_TIMER(&this->dyna.actor) * 10;
    this->targetDoorHeight = 180.0f;
    this->doorHeight = 180.0f;
    this->actionFunc = BgCraceMovebg_ClosingDoor_WaitToClose;
}

/**
 * Wait until the closeWaitTimer reaches 0, then set the door's switch flag and start closing.
 */
void BgCraceMovebg_ClosingDoor_WaitToClose(BgCraceMovebg* this, PlayState* play) {
    if (this->closeWaitTimer > 0) {
        this->closeWaitTimer--;
    }

    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->doorHeight;
    BgCraceMovebg_ClosingDoor_CheckIfPlayerIsBeyondDoor(this, play);

    if (this->closeWaitTimer <= 0) {
        this->doorHeight = 180.0f;
        this->targetDoorHeight = 0.0f;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 180.0f;
        Flags_SetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor));
        BgCraceMovebg_ClosingDoor_SetupClose(this, play);
    }
}

void BgCraceMovebg_ClosingDoor_SetupClose(BgCraceMovebg* this, PlayState* play) {
    this->actionFunc = BgCraceMovebg_ClosingDoor_Close;
}

/**
 * Slowly slide closed, then check to see if the player is beyond the door. If the player has
 * not moved beyond the door, trigger a voidout. Otherwise, do nothing.
 */
void BgCraceMovebg_ClosingDoor_Close(BgCraceMovebg* this, PlayState* play) {
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->doorHeight;
    BgCraceMovebg_ClosingDoor_CheckIfPlayerIsBeyondDoor(this, play);

    if (Math_StepToF(&this->doorHeight, 0.0f, 1.0f)) {
        if (!(this->stateFlags & BG_CRACE_MOVEBG_FLAG_PLAYER_IS_BEYOND_DOOR) &&
            !Flags_GetSwitch(play, BG_CRACE_MOVEBG_GET_SWITCH_FLAG(&this->dyna.actor) + 1)) {
            play->haltAllActors = true;
            func_80169FDC(play);
            Audio_PlaySfx(NA_SE_OC_ABYSS);
        }

        BgCraceMovebg_ClosingDoor_SetupDoNothing(this, play);
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_STONEDOOR_CLOSE_S - SFX_FLAG);
    }
}

void BgCraceMovebg_ClosingDoor_SetupDoNothing(BgCraceMovebg* this, PlayState* play) {
    this->targetDoorHeight = 0.0f;
    this->doorHeight = 0.0f;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_STONEDOOR_STOP);
    this->actionFunc = BgCraceMovebg_ClosingDoor_DoNothing;
}

void BgCraceMovebg_ClosingDoor_DoNothing(BgCraceMovebg* this, PlayState* play) {
}

void BgCraceMovebg_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gDekuShrineSlidingDoorDL);
}

void BgCraceMovebg_Reset(void) {
    sHasInitializedIsLoaded = false;
    sLoadedDoorCount = 0;
}
