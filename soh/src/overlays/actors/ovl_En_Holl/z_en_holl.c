#include "z_en_holl.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

/*
 * Horizontal holls parameters (`ENHOLL_H_*`)
 * All horizontal holls are cuboids which react to how far (depth) and in which direction (side) the player is from the
 * actor, in the actor's local z direction.
 */

// Defines the height of horizontal holls (all kinds) with a Y range relative to the actor position.
#define ENHOLL_H_Y_MIN -50.0f
#define ENHOLL_H_Y_MAX 200.0f

// Defines the width of horizontal holls (all kinds),
// with a half-width extending on both lateral sides (towards local +x and -x).
#define ENHOLL_H_HALFWIDTH_NARROW 100.0f
#define ENHOLL_H_HALFWIDTH 200.0f

// Defines the depth range from horizontal invisible holls (`ENHOLL_H_INVISIBLE`, `ENHOLL_H_INVISIBLE_NARROW`),
// at which rooms get loaded.
// i.e. when the player's distance from the actor (along local z) is within this range, the corresponding room is loaded
// Note: This means there is a depth range in the middle of `2 * ENHOLL_H_INVISIBLE_LOAD_DEPTH_MIN` where nothing
//       happens. That range where nothing happens is useful to avoid quickly repeated room swaps.
// Note: This means the player is expected to be inside the depth range at some point.
//       i.e. this range needs to be deep enough so that the player cannot move past it in a single frame.
#define ENHOLL_H_INVISIBLE_LOAD_DEPTH_MAX 100.0f
#define ENHOLL_H_INVISIBLE_LOAD_DEPTH_MIN 50.0f

// Defines the depth from horizontal switch flag holls (`ENHOLL_H_BGCOVER_SWITCHFLAG`),
//  - at which the background geometry starts fading black;
#define ENHOLL_H_SWITCHFLAG_BGCOVER_DEPTH 100.0f
//  - at which the background geometry is fully faded black,
//    and rooms are loaded if needed according to the side the player is on (along local z).
#define ENHOLL_H_SWITCHFLAG_LOAD_DEPTH 50.0f

/*
 * Vertical holls parameters (`ENHOLL_V_*`)
 * All vertical holls are cylinders which react to how far (y dist) and in which direction (side) the player is from the
 * actor, along the vertical y axis.
 */

// Vertical down holls parameters (`ENHOLL_V_DOWN_BGCOVER_LARGE`)
#define ENHOLL_V_DOWN_RADIUS 500.0f
// Y dist at which the background geometry starts fading black.
#define ENHOLL_V_DOWN_BGCOVER_YDIST 605.0f
// Y dist at which the background geometry is fully faded black, and the room down is loaded.
#define ENHOLL_V_DOWN_LOAD_YDIST 95.0f

// Radius for other vertical holls (`ENHOLL_V_BGCOVER`, `ENHOLL_V_INVISIBLE`)
#define ENHOLL_V_RADIUS 120.0f

// Vertical bg cover holls parameters (`ENHOLL_V_BGCOVER`)
// Y dist at which the background geometry starts fading black.
#define ENHOLL_V_BGCOVER_BGCOVER_YDIST 200.0f
// Y dist at which the background geometry is fully faded black,
// and rooms are loaded if needed according to the side the player is on (along y).
#define ENHOLL_V_BGCOVER_LOAD_YDIST 50.0f

// Vertical invisible holls parameters (`ENHOLL_V_INVISIBLE`)
// Similar to the range defined by `ENHOLL_H_INVISIBLE_LOAD_DEPTH_MAX` and min above for horizontal planes,
// but vertically (along y).
#define ENHOLL_V_INVISIBLE_LOAD_YDIST_MAX 200.0f
#define ENHOLL_V_INVISIBLE_LOAD_YDIST_MIN 50.0f

void EnHoll_Init(Actor* thisx, PlayState* play);
void EnHoll_Destroy(Actor* thisx, PlayState* play);
void EnHoll_Update(Actor* thisx, PlayState* play);
void EnHoll_Draw(Actor* thisx, PlayState* play);

void EnHoll_WaitRoomLoaded(EnHoll* this, PlayState* play);
void EnHoll_HorizontalVisibleNarrow(EnHoll* this, PlayState* play);
void EnHoll_HorizontalInvisible(EnHoll* this, PlayState* play);
void EnHoll_VerticalDownBgCoverLarge(EnHoll* this, PlayState* play);
void EnHoll_VerticalBgCover(EnHoll* this, PlayState* play);
void EnHoll_VerticalInvisible(EnHoll* this, PlayState* play);
void EnHoll_HorizontalBgCoverSwitchFlag(EnHoll* this, PlayState* play);

const ActorInit En_Holl_InitVars = {
    /**/ ACTOR_EN_HOLL,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnHoll),
    /**/ (ActorFunc)EnHoll_Init,
    /**/ (ActorFunc)EnHoll_Destroy,
    /**/ (ActorFunc)EnHoll_Update,
    /**/ (ActorFunc)EnHoll_Draw,
};

static EnHollActionFunc sActionFuncs[] = {
    EnHoll_HorizontalVisibleNarrow,     // ENHOLL_H_VISIBLE_NARROW
    EnHoll_VerticalDownBgCoverLarge,    // ENHOLL_V_DOWN_BGCOVER_LARGE
    EnHoll_VerticalInvisible,           // ENHOLL_V_INVISIBLE
    EnHoll_HorizontalBgCoverSwitchFlag, // ENHOLL_H_BGCOVER_SWITCHFLAG
    EnHoll_HorizontalInvisible,         // ENHOLL_H_INVISIBLE
    EnHoll_VerticalBgCover,             // ENHOLL_V_BGCOVER
    EnHoll_HorizontalInvisible,         // ENHOLL_H_INVISIBLE_NARROW
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

void EnHoll_SetupAction(EnHoll* this, EnHollActionFunc func) {
    this->actionFunc = func;
}

int EnHoll_IsKokiriLayer8(void) {
    return gSaveContext.entranceIndex == ENTR_KOKIRI_FOREST_0 && gSaveContext.sceneLayer == 8;
}

void EnHoll_ChooseAction(EnHoll* this) {
    s32 type = ENHOLL_GET_TYPE(&this->actor);

    EnHoll_SetupAction(this, sActionFuncs[type]);
    if (type != ENHOLL_H_VISIBLE_NARROW) {
        this->actor.draw = NULL;
    } else {
        this->planeAlpha = 255;
    }
}

void EnHoll_Init(Actor* thisx, PlayState* play) {
    EnHoll* this = (EnHoll*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnHoll_ChooseAction(this);
    this->resetBgCoverAlpha = false;
}

void EnHoll_Destroy(Actor* thisx, PlayState* play) {
    s32 transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(thisx);
    TransitionActorEntry* transitionEntry = &play->transiActorCtx.list[transitionActorIndex];

    transitionEntry->id = -transitionEntry->id;
}

void EnHoll_SwapRooms(PlayState* play) {
    Room tempRoom;
    RoomContext* roomCtx = &play->roomCtx;

    tempRoom = roomCtx->curRoom;
    roomCtx->curRoom = roomCtx->prevRoom;
    roomCtx->prevRoom = tempRoom;
    play->roomCtx.activeBufPage ^= 1;
}

/**
 * These are all absolute distances in the relative z direction. That is, moving
 *   towards or away from the "face" of the holl regardless of orientation.
 * Moving within these distances of the holl have the following effects:
 * [0] : Load the room on this side of the holl if not already loaded
 * [1] : Load the room on the other side of the holl
 * [2] : Start of fade region, where the plane is fully opaque
 * [3] : End of fade region region, where the plane is fully transparent
 *
 * Within the fade region, the plane goes:
 *   opaque -> transparent if approaching,
 *   transparent -> opaque if receding
 */
static f32 sHorizontalVisibleNarrowTriggerDists[2][4] = {
    { 200.0f, 150.0f, 100.0f, 50.0f }, // default
    { 100.0f, 75.0f, 50.0f, 25.0f },   // SCENE_SPIRIT_TEMPLE
};

/**
 * When traversing a holl of this kind, it attempts to:
 *   Load Current Room (fails as it is already loaded)
 *   Load Next Room
 *   Load Previous Room
 *   Load Next Room
 *
 *  @bug The striped nature of holls can cause some actors to unload due to
 *      conflicting Object Lists between the two rooms
 *
 *  @bug If you can get around to the other side of the holl without triggering it,
 *      you can load the room on the other side multiple times
 */
void EnHoll_HorizontalVisibleNarrow(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 triggerDistsIndex = (u32)((play->sceneNum == SCENE_SPIRIT_TEMPLE) ? 1 : 0);
    Vec3f relPlayerPos;
    f32 orthogonalDistToPlayer;
    s32 transitionActorIndex;

    Actor_WorldToActorCoords(&this->actor, &relPlayerPos, &player->actor.world.pos);
    this->side = (relPlayerPos.z < 0.0f) ? 0 : 1;
    orthogonalDistToPlayer = fabsf(relPlayerPos.z);
    if (relPlayerPos.y > ENHOLL_H_Y_MIN && relPlayerPos.y < ENHOLL_H_Y_MAX &&
        fabsf(relPlayerPos.x) < ENHOLL_H_HALFWIDTH_NARROW &&
        orthogonalDistToPlayer < sHorizontalVisibleNarrowTriggerDists[triggerDistsIndex][0]) {

        transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);
        if (orthogonalDistToPlayer > sHorizontalVisibleNarrowTriggerDists[triggerDistsIndex][1]) {
            if (play->roomCtx.prevRoom.num >= 0 && play->roomCtx.status == 0) {
                this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[this->side].room;
                EnHoll_SwapRooms(play);
                Room_FinishRoomChange(play, &play->roomCtx);
            }
        } else {
            this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[this->side ^ 1].room;
            if (play->roomCtx.prevRoom.num < 0) {
                Room_RequestNewRoom(play, &play->roomCtx, this->actor.room);
            } else {
                this->planeAlpha =
                    (255.0f / (sHorizontalVisibleNarrowTriggerDists[triggerDistsIndex][2] -
                               sHorizontalVisibleNarrowTriggerDists[triggerDistsIndex][3])) *
                    (orthogonalDistToPlayer - sHorizontalVisibleNarrowTriggerDists[triggerDistsIndex][3]);
                this->planeAlpha = CLAMP(this->planeAlpha, 0, 255);

                if (play->roomCtx.curRoom.num != this->actor.room) {
                    EnHoll_SwapRooms(play);
                }
            }
        }
    }
}

void EnHoll_HorizontalInvisible(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 useViewEye = gDbgCamEnabled || play->csCtx.state != CS_STATE_IDLE;
    Vec3f relSubjectPos;
    s32 isKokiriLayer8;
    f32 hollHalfWidth;
    f32 orthogonalDistToSubject;

    Actor_WorldToActorCoords(&this->actor, &relSubjectPos, useViewEye ? &play->view.eye : &player->actor.world.pos);
    hollHalfWidth =
        (ENHOLL_GET_TYPE(&this->actor) == ENHOLL_H_INVISIBLE_NARROW) ? ENHOLL_H_HALFWIDTH_NARROW : ENHOLL_H_HALFWIDTH;

    isKokiriLayer8 = EnHoll_IsKokiriLayer8();
    if (isKokiriLayer8 || (relSubjectPos.y > ENHOLL_H_Y_MIN && relSubjectPos.y < ENHOLL_H_Y_MAX &&
                           fabsf(relSubjectPos.x) < hollHalfWidth &&
                           (orthogonalDistToSubject = fabsf(relSubjectPos.z),
                            orthogonalDistToSubject < ENHOLL_H_INVISIBLE_LOAD_DEPTH_MAX &&
                                orthogonalDistToSubject > ENHOLL_H_INVISIBLE_LOAD_DEPTH_MIN))) {
        s32 transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);
        s32 side = (relSubjectPos.z < 0.0f) ? 0 : 1;
        TransitionActorEntry* transitionEntry = &play->transiActorCtx.list[transitionActorIndex];
        s32 room = transitionEntry->sides[side].room;

        this->actor.room = room;
        if (isKokiriLayer8) {}
        if (this->actor.room != play->roomCtx.curRoom.num) {
            if (room) {}
            if (Room_RequestNewRoom(play, &play->roomCtx, this->actor.room)) {
                EnHoll_SetupAction(this, EnHoll_WaitRoomLoaded);
            }
        }
    }
}

void EnHoll_VerticalDownBgCoverLarge(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 absYDistToPlayer = fabsf(this->actor.yDistToPlayer);

    if (this->actor.xzDistToPlayer < ENHOLL_V_DOWN_RADIUS &&
        // Nothing happens if `absYDistToPlayer > ENHOLL_V_DOWN_BGCOVER_YDIST`,
        // so this check may as well compare to ENHOLL_V_DOWN_BGCOVER_YDIST
        absYDistToPlayer < (ENHOLL_V_DOWN_BGCOVER_YDIST + 95.0f)) {
        s32 transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);

        if (absYDistToPlayer < ENHOLL_V_DOWN_LOAD_YDIST) {
            play->bgCoverAlpha = 255;
        } else if (absYDistToPlayer > ENHOLL_V_DOWN_BGCOVER_YDIST) {
            play->bgCoverAlpha = 0;
        } else {
            play->bgCoverAlpha = (s16)(ENHOLL_V_DOWN_BGCOVER_YDIST - absYDistToPlayer) *
                                 (255 / (ENHOLL_V_DOWN_BGCOVER_YDIST - ENHOLL_V_DOWN_LOAD_YDIST));
        }

        if (absYDistToPlayer < ENHOLL_V_DOWN_LOAD_YDIST) {
            this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[1].room;
            Math_SmoothStepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 1.0f, 50.0f, 10.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 1.0f, 50.0f, 10.0f);
            if (this->actor.room != play->roomCtx.curRoom.num &&
                Room_RequestNewRoom(play, &play->roomCtx, this->actor.room)) {
                EnHoll_SetupAction(this, EnHoll_WaitRoomLoaded);
                this->resetBgCoverAlpha = true;
                player->actor.speedXZ = 0.0f;
            }
        }
    } else {
        if (this->resetBgCoverAlpha) {
            play->bgCoverAlpha = 0;
            this->resetBgCoverAlpha = false;
        }
    }
}

void EnHoll_VerticalBgCover(EnHoll* this, PlayState* play) {
    f32 absYDistToPlayer;

    if ((this->actor.xzDistToPlayer < ENHOLL_V_RADIUS) &&
        (absYDistToPlayer = fabsf(this->actor.yDistToPlayer), absYDistToPlayer < ENHOLL_V_BGCOVER_BGCOVER_YDIST)) {

        if (absYDistToPlayer < ENHOLL_V_BGCOVER_LOAD_YDIST) {
            play->bgCoverAlpha = 255;
        } else {
            play->bgCoverAlpha = (ENHOLL_V_BGCOVER_BGCOVER_YDIST - absYDistToPlayer) *
                                 (255 / (ENHOLL_V_BGCOVER_BGCOVER_YDIST - ENHOLL_V_BGCOVER_LOAD_YDIST));
        }

        if (absYDistToPlayer > ENHOLL_V_BGCOVER_LOAD_YDIST) {
            s32 transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);
            s32 side = (this->actor.yDistToPlayer > 0.0f) ? 0 : 1;

            this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[side].room;
            if (this->actor.room != play->roomCtx.curRoom.num &&
                Room_RequestNewRoom(play, &play->roomCtx, this->actor.room)) {
                EnHoll_SetupAction(this, EnHoll_WaitRoomLoaded);
                this->resetBgCoverAlpha = true;
            }
        }
    } else {
        if (this->resetBgCoverAlpha) {
            this->resetBgCoverAlpha = false;
            play->bgCoverAlpha = 0;
        }
    }
}

void EnHoll_VerticalInvisible(EnHoll* this, PlayState* play) {
    f32 absYDistToPlayer;
    s8 side;
    s32 transitionActorIndex;

    if (this->actor.xzDistToPlayer < ENHOLL_V_RADIUS) {
        absYDistToPlayer = fabsf(this->actor.yDistToPlayer);
        if (absYDistToPlayer < ENHOLL_V_INVISIBLE_LOAD_YDIST_MAX &&
            absYDistToPlayer > ENHOLL_V_INVISIBLE_LOAD_YDIST_MIN) {
            transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);
            side = (this->actor.yDistToPlayer > 0.0f) ? 0 : 1;
            this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[side].room;
            if (this->actor.room != play->roomCtx.curRoom.num &&
                Room_RequestNewRoom(play, &play->roomCtx, this->actor.room)) {
                EnHoll_SetupAction(this, EnHoll_WaitRoomLoaded);
            }
        }
    }
}

void EnHoll_HorizontalBgCoverSwitchFlag(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Flags_GetSwitch(play, ENHOLL_GET_SWITCH_FLAG(&this->actor))) {
        if (this->resetBgCoverAlpha) {
            play->bgCoverAlpha = 0;
            this->resetBgCoverAlpha = false;
        }
    } else {
        Vec3f relPlayerPos;
        f32 orthogonalDistToPlayer;

        Actor_WorldToActorCoords(&this->actor, &relPlayerPos, &player->actor.world.pos);
        orthogonalDistToPlayer = fabsf(relPlayerPos.z);

        if (ENHOLL_H_Y_MIN < relPlayerPos.y && relPlayerPos.y < ENHOLL_H_Y_MAX &&
            fabsf(relPlayerPos.x) < ENHOLL_H_HALFWIDTH && orthogonalDistToPlayer < ENHOLL_H_SWITCHFLAG_BGCOVER_DEPTH) {
            s32 transitionActorIndex = GET_TRANSITION_ACTOR_INDEX(&this->actor);

            this->resetBgCoverAlpha = true;
            play->bgCoverAlpha =
                255 - (s32)((orthogonalDistToPlayer - ENHOLL_H_SWITCHFLAG_LOAD_DEPTH) *
                            (255 / (ENHOLL_H_SWITCHFLAG_BGCOVER_DEPTH - ENHOLL_H_SWITCHFLAG_LOAD_DEPTH) + 0.8f));
            if (play->bgCoverAlpha > 255) {
                play->bgCoverAlpha = 255;
            } else if (play->bgCoverAlpha < 0) {
                play->bgCoverAlpha = 0;
            }

            if (orthogonalDistToPlayer < ENHOLL_H_SWITCHFLAG_LOAD_DEPTH) {
                s32 side = (relPlayerPos.z < 0.0f) ? 0 : 1;

                this->actor.room = play->transiActorCtx.list[transitionActorIndex].sides[side].room;
                if (this->actor.room != play->roomCtx.curRoom.num &&
                    Room_RequestNewRoom(play, &play->roomCtx, this->actor.room)) {
                    EnHoll_SetupAction(this, EnHoll_WaitRoomLoaded);
                }
            }
        } else {
            if (this->resetBgCoverAlpha) {
                play->bgCoverAlpha = 0;
                this->resetBgCoverAlpha = false;
            }
        }
    }
}

void EnHoll_WaitRoomLoaded(EnHoll* this, PlayState* play) {
    if (!EnHoll_IsKokiriLayer8() && play->roomCtx.status == 0) {
        Room_FinishRoomChange(play, &play->roomCtx);
        if (play->bgCoverAlpha == 0) {
            this->resetBgCoverAlpha = false;
        }
        EnHoll_ChooseAction(this);
    }
}

void EnHoll_Update(Actor* thisx, PlayState* play) {
    EnHoll* this = (EnHoll*)thisx;

    this->actionFunc(this, play);
}

#include "overlays/ovl_En_Holl/ovl_En_Holl.h"

void EnHoll_Draw(Actor* thisx, PlayState* play) {
    EnHoll* this = (EnHoll*)thisx;
    Gfx* gfxP;
    u32 setupDLIndex;

    // Only draw the plane if not invisible
    if (this->planeAlpha != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        if (this->planeAlpha == 255) {
            gfxP = POLY_OPA_DISP;
            setupDLIndex = SETUPDL_37;
        } else {
            gfxP = POLY_XLU_DISP;
            setupDLIndex = SETUPDL_0;
        }
        gfxP = Gfx_SetupDL(gfxP, setupDLIndex);
        if (this->side == 0) {
            Matrix_RotateY(M_PI, MTXMODE_APPLY);
        }

        gSPMatrix(gfxP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(gfxP++, 0, 0, 0, 0, 0, (u8)this->planeAlpha);
        gSPDisplayList(gfxP++, sPlaneDL);

        if (this->planeAlpha == 255) {
            POLY_OPA_DISP = gfxP;
        } else {
            POLY_XLU_DISP = gfxP;
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
