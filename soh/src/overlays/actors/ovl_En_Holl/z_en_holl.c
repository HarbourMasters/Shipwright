#include "z_en_holl.h"

#define FLAGS ACTOR_FLAG_4

// Horizontal Plane parameters

#define PLANE_Y_MIN -50.0f
#define PLANE_Y_MAX 200.0f

#define PLANE_HALFWIDTH 100.0f
#define PLANE_HALFWIDTH_2 200.0f

void EnHoll_Init(Actor* thisx, PlayState* play);
void EnHoll_Destroy(Actor* thisx, PlayState* play);
void EnHoll_Update(Actor* thisx, PlayState* play);
void EnHoll_Draw(Actor* thisx, PlayState* play);

void EnHoll_NextAction(EnHoll* this, PlayState* play);
void func_80A58DD4(EnHoll* this, PlayState* play);
void func_80A59014(EnHoll* this, PlayState* play);
void func_80A591C0(EnHoll* this, PlayState* play);
void func_80A593A4(EnHoll* this, PlayState* play);
void func_80A59520(EnHoll* this, PlayState* play);
void func_80A59618(EnHoll* this, PlayState* play);

const ActorInit En_Holl_InitVars = {
    ACTOR_EN_HOLL,
    ACTORCAT_DOOR,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnHoll),
    (ActorFunc)EnHoll_Init,
    (ActorFunc)EnHoll_Destroy,
    (ActorFunc)EnHoll_Update,
    (ActorFunc)EnHoll_Draw,
    NULL,
};

static EnHollActionFunc sActionFuncs[] = {
    func_80A58DD4, func_80A591C0, func_80A59520, func_80A59618, func_80A59014, func_80A593A4, func_80A59014,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

/**
 * These are all absolute distances in the relative z direction. That is, moving
 *   towards or away from the "face" of the loading plane regardless of orientation.
 * Moving within these distances of the load plane have the following effects:
 * [0] : Load the room on this side of the loading plane if not already loaded
 * [1] : Load the room on the other side of the loading plane
 * [2] : Fade Region (opaque -> transparent if approaching, transparent -> opaque if receding)
 * [3] : Transparent Region
 *
 * When traversing a loading plane of this kind, it attempts to:
 *   Load Current Room (fails as it is already loaded)
 *   Load Next Room
 *   Load Previous Room
 *   Load Next Room
 *
 *  @bug The striped nature of loading planes can cause some actors to unload due to
 *      conflicting Object Lists between the two rooms
 *
 *  @bug If you can get around to the other side of the loading plane without triggering it,
 *      you can load the room on the other side multiple times
 */
static f32 sHorizTriggerDists[2][4] = {
    { 200.0f, 150.0f, 100.0f, 50.0f },
    { 100.0f, 75.0f, 50.0f, 25.0f },
};

void EnHoll_SetupAction(EnHoll* this, EnHollActionFunc func) {
    this->actionFunc = func;
}

s32 EnHoll_IsKokiriSetup8() {
    return gSaveContext.entranceIndex == 0x00EE && gSaveContext.sceneSetupIndex == 8;
}

void EnHoll_ChooseAction(EnHoll* this) {
    s32 action;

    action = (this->actor.params >> 6) & 7;
    EnHoll_SetupAction(this, sActionFuncs[action]);
    if (action != 0) {
        this->actor.draw = NULL;
    } else {
        this->planeAlpha = 255;
    }
}

void EnHoll_Init(Actor* thisx, PlayState* play) {
    EnHoll* this = (EnHoll*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnHoll_ChooseAction(this);
    this->unk_14F = 0;
}

void EnHoll_Destroy(Actor* thisx, PlayState* play) {
    s32 transitionActorIdx = (u16)thisx->params >> 0xA;
    TransitionActorEntry* transitionEntry = &play->transiActorCtx.list[transitionActorIdx];

    transitionEntry->id = -transitionEntry->id;
}

void EnHoll_SwapRooms(PlayState* play) {
    Room tempRoom;
    RoomContext* roomCtx = &play->roomCtx;

    tempRoom = roomCtx->curRoom;
    roomCtx->curRoom = roomCtx->prevRoom;
    roomCtx->prevRoom = tempRoom;
    play->roomCtx.unk_30 ^= 1;
}

// Horizontal Planes
void func_80A58DD4(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 phi_t0 = ((play->sceneNum == SCENE_JYASINZOU) ? 1 : 0) & 0xFFFFFFFF;
    Vec3f vec;
    f32 absZ;
    s32 transitionActorIdx;

    func_8002DBD0(&this->actor, &vec, &player->actor.world.pos);
    this->side = (vec.z < 0.0f) ? 0 : 1;
    absZ = fabsf(vec.z);
    if (vec.y > PLANE_Y_MIN && vec.y < PLANE_Y_MAX && fabsf(vec.x) < PLANE_HALFWIDTH &&
        absZ < sHorizTriggerDists[phi_t0][0]) {
        transitionActorIdx = (u16)this->actor.params >> 0xA;
        if (absZ > sHorizTriggerDists[phi_t0][1]) {
            if (play->roomCtx.prevRoom.num >= 0 && play->roomCtx.status == 0) {
                this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[this->side].room;
                EnHoll_SwapRooms(play);
                func_80097534(play, &play->roomCtx);
            }
        } else {
            this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[this->side ^ 1].room;
            if (play->roomCtx.prevRoom.num < 0) {
                func_8009728C(play, &play->roomCtx, this->actor.room);
            } else {
                this->planeAlpha = (255.0f / (sHorizTriggerDists[phi_t0][2] - sHorizTriggerDists[phi_t0][3])) *
                                   (absZ - sHorizTriggerDists[phi_t0][3]);
                this->planeAlpha = CLAMP(this->planeAlpha, 0, 255);
                if (play->roomCtx.curRoom.num != this->actor.room) {
                    EnHoll_SwapRooms(play);
                }
            }
        }
    }
}

// Horizontal Planes
void func_80A59014(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 useViewEye = gDbgCamEnabled || play->csCtx.state != CS_STATE_IDLE;
    Vec3f vec;
    s32 temp;
    f32 planeHalfWidth;
    f32 absZ;

    func_8002DBD0(&this->actor, &vec, (useViewEye) ? &play->view.eye : &player->actor.world.pos);
    planeHalfWidth = (((this->actor.params >> 6) & 7) == 6) ? PLANE_HALFWIDTH : PLANE_HALFWIDTH_2;

    temp = EnHoll_IsKokiriSetup8();
    if (temp || (PLANE_Y_MIN < vec.y && vec.y < PLANE_Y_MAX && fabsf(vec.x) < planeHalfWidth &&
                 (absZ = fabsf(vec.z), 100.0f > absZ && absZ > 50.0f))) {
        s32 transitionActorIdx = (u16)this->actor.params >> 0xA;
        s32 side = (vec.z < 0.0f) ? 0 : 1;
        TransitionActorEntry* transitionEntry = &play->transiActorCtx.list[transitionActorIdx];
        s32 room = transitionEntry->sides[side].room;

        this->actor.room = room;
        if (temp) {}
        if (this->actor.room != play->roomCtx.curRoom.num) {
            if (room) {}
            if (func_8009728C(play, &play->roomCtx, this->actor.room)) {
                EnHoll_SetupAction(this, EnHoll_NextAction);
            }
        }
    }
}

// Vertical Planes
void func_80A591C0(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 absY = fabsf(this->actor.yDistToPlayer);
    s32 transitionActorIdx;

    if (this->actor.xzDistToPlayer < 500.0f && absY < 700.0f) {
        transitionActorIdx = (u16)this->actor.params >> 0xA;
        if (absY < 95.0f) {
            play->unk_11E18 = 0xFF;
        } else if (absY > 605.0f) {
            play->unk_11E18 = 0;
        } else {
            play->unk_11E18 = (s16)(605.0f - absY) * 0.5f;
        }
        if (absY < 95.0f) {
            this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[1].room;
            Math_SmoothStepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 1.0f, 50.0f, 10.0f);
            Math_SmoothStepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 1.0f, 50.0f, 10.0f);
            if (this->actor.room != play->roomCtx.curRoom.num &&
                func_8009728C(play, &play->roomCtx, this->actor.room) != 0) {
                EnHoll_SetupAction(this, EnHoll_NextAction);
                this->unk_14F = 1;
                player->actor.speedXZ = 0.0f;
            }
        }
    } else if (this->unk_14F != 0) {
        play->unk_11E18 = 0;
        this->unk_14F = 0;
    }
}

// Vertical Planes
void func_80A593A4(EnHoll* this, PlayState* play) {
    f32 absY;
    s32 side;
    s32 transitionActorIdx;

    if ((this->actor.xzDistToPlayer < 120.0f) && (absY = fabsf(this->actor.yDistToPlayer), absY < 200.0f)) {
        if (absY < 50.0f) {
            play->unk_11E18 = 0xFF;
        } else {
            play->unk_11E18 = (200.0f - absY) * 1.7f;
        }
        if (absY > 50.0f) {
            transitionActorIdx = (u16)this->actor.params >> 0xA;
            side = (0.0f < this->actor.yDistToPlayer) ? 0 : 1;
            this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[side].room;
            if (this->actor.room != play->roomCtx.curRoom.num &&
                func_8009728C(play, &play->roomCtx, this->actor.room) != 0) {
                EnHoll_SetupAction(this, EnHoll_NextAction);
                this->unk_14F = 1;
            }
        }
    } else if (this->unk_14F != 0) {
        this->unk_14F = 0;
        play->unk_11E18 = 0;
    }
}

// Vertical Planes
void func_80A59520(EnHoll* this, PlayState* play) {
    f32 absY;
    s8 side;
    s32 transitionActorIdx;

    if (this->actor.xzDistToPlayer < 120.0f) {
        absY = fabsf(this->actor.yDistToPlayer);
        if (absY < 200.0f && absY > 50.0f) {
            transitionActorIdx = (u16)this->actor.params >> 0xA;
            side = (0.0f < this->actor.yDistToPlayer) ? 0 : 1;
            this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[side].room;
            if (this->actor.room != play->roomCtx.curRoom.num &&
                func_8009728C(play, &play->roomCtx, this->actor.room) != 0) {
                EnHoll_SetupAction(this, EnHoll_NextAction);
            }
        }
    }
}

// Horizontal Planes
void func_80A59618(EnHoll* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f vec;
    f32 absZ;
    s32 side;
    s32 transitionActorIdx;

    if (!Flags_GetSwitch(play, this->actor.params & 0x3F)) {
        if (this->unk_14F != 0) {
            play->unk_11E18 = 0;
            this->unk_14F = 0;
        }
    } else {
        func_8002DBD0(&this->actor, &vec, &player->actor.world.pos);
        absZ = fabsf(vec.z);
        if (PLANE_Y_MIN < vec.y && vec.y < PLANE_Y_MAX && fabsf(vec.x) < PLANE_HALFWIDTH_2 && absZ < 100.0f) {
            this->unk_14F = 1;
            transitionActorIdx = (u16)this->actor.params >> 0xA;
            play->unk_11E18 = 0xFF - (s32)((absZ - 50.0f) * 5.9f);
            if (play->unk_11E18 >= 0x100) {
                play->unk_11E18 = 0xFF;
            } else if (play->unk_11E18 < 0) {
                play->unk_11E18 = 0;
            }
            if (absZ < 50.0f) {
                side = (vec.z < 0.0f) ? 0 : 1;
                this->actor.room = play->transiActorCtx.list[transitionActorIdx].sides[side].room;
                if (this->actor.room != play->roomCtx.curRoom.num &&
                    func_8009728C(play, &play->roomCtx, this->actor.room) != 0) {
                    EnHoll_SetupAction(this, EnHoll_NextAction);
                }
            }
        } else if (this->unk_14F != 0) {
            play->unk_11E18 = 0;
            this->unk_14F = 0;
        }
    }
}

void EnHoll_NextAction(EnHoll* this, PlayState* play) {
    if (!EnHoll_IsKokiriSetup8() && play->roomCtx.status == 0) {
        func_80097534(play, &play->roomCtx);
        if (play->unk_11E18 == 0) {
            this->unk_14F = 0;
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
    u32 setupDlIdx;

    // Only draw the plane if not invisible
    if (this->planeAlpha != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        if (this->planeAlpha == 255) {
            gfxP = POLY_OPA_DISP;
            setupDlIdx = 37;
        } else {
            gfxP = POLY_XLU_DISP;
            setupDlIdx = 0;
        }
        gfxP = Gfx_SetupDL(gfxP, setupDlIdx);
        if (this->side == 0) {
            Matrix_RotateY(M_PI, MTXMODE_APPLY);
        }

        gSPMatrix(gfxP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
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
