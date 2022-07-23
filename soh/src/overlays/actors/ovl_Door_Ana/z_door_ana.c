/*
 * File: z_door_ana.c
 * Overlay: ovl_Door_Ana
 * Description: Grottos Entrances/Exits
 */

#include "z_door_ana.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS ACTOR_FLAG_25

void DoorAna_Init(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Update(Actor* thisx, GlobalContext* globalCtx);
void DoorAna_Draw(Actor* thisx, GlobalContext* globalCtx);

void DoorAna_WaitClosed(DoorAna* this, GlobalContext* globalCtx);
void DoorAna_WaitOpen(DoorAna* this, GlobalContext* globalCtx);
void DoorAna_GrabPlayer(DoorAna* this, GlobalContext* globalCtx);

const ActorInit Door_Ana_InitVars = {
    ACTOR_DOOR_ANA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(DoorAna),
    (ActorFunc)DoorAna_Init,
    (ActorFunc)DoorAna_Destroy,
    (ActorFunc)DoorAna_Update,
    (ActorFunc)DoorAna_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 50, 10, 0, { 0 } },
};

// array of entrance table entries to grotto destinations
static s16 entrances[] = {
    0x036D, 0x003F, 0x0598, 0x059C, 0x05A0, 0x05A4, 0x05A8, 0x05AC,
    0x05B0, 0x05B4, 0x05B8, 0x05BC, 0x05C0, 0x05C4, 0x05FC,
};

void DoorAna_SetupAction(DoorAna* this, DoorAnaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DoorAna_Init(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* this = (DoorAna*)thisx;

    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.shape.rot.z;
    // init block for grottos that are initially "hidden" (require explosives/hammer/song of storms to open)
    if ((this->actor.params & 0x300) != 0) {
        // only allocate collider for grottos that need bombing/hammering open
        if ((this->actor.params & 0x200) != 0) {
            Collider_InitCylinder(globalCtx, &this->collider);
            Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
        } else {
            this->actor.flags |= ACTOR_FLAG_4;
        }
        Actor_SetScale(&this->actor, 0);
        DoorAna_SetupAction(this, DoorAna_WaitClosed);
    } else {
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
    }
    this->actor.targetMode = 0;
}

void DoorAna_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* this = (DoorAna*)thisx;

    // free collider if it has one
    if ((this->actor.params & 0x200) != 0) {
        Collider_DestroyCylinder(globalCtx, &this->collider);
    }
}

// update routine for grottos that are currently "hidden"/unopened
void DoorAna_WaitClosed(DoorAna* this, GlobalContext* globalCtx) {
    u32 openGrotto = false;

    if (!(this->actor.params & 0x200)) {
        // opening with song of storms
        if (this->actor.xyzDistToPlayerSq < 40000.0f && Flags_GetEnv(globalCtx, 5)) {
            openGrotto = true;
            this->actor.flags &= ~ACTOR_FLAG_4;
        }
    } else {
        // bombing/hammering open a grotto
        if (this->collider.base.acFlags & AC_HIT) {
            openGrotto = true;
            Collider_DestroyCylinder(globalCtx, &this->collider);
        } else {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        }
    }
    // open the grotto
    if (openGrotto) {
        this->actor.params &= ~0x0300;
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    func_8002F5F0(&this->actor, globalCtx);
}

// update routine for grottos that are open
void DoorAna_WaitOpen(DoorAna* this, GlobalContext* globalCtx) {
    Player* player;
    s32 destinationIdx;

    player = GET_PLAYER(globalCtx);
    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.001f)) {
        if ((this->actor.targetMode != 0) && (globalCtx->sceneLoadFlag == 0) && (player->stateFlags1 & 0x80000000) &&
            (player->unk_84F == 0)) {
            destinationIdx = ((this->actor.params >> 0xC) & 7) - 1;
            Gameplay_SetupRespawnPoint(globalCtx, RESPAWN_MODE_RETURN, 0x4FF);
            gSaveContext.respawn[RESPAWN_MODE_RETURN].pos.y = this->actor.world.pos.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].yaw = this->actor.home.rot.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].data = this->actor.params & 0xFFFF;
            if (destinationIdx < 0) {
                destinationIdx = this->actor.home.rot.z + 1;
            }
            globalCtx->nextEntranceIndex = entrances[destinationIdx];
            DoorAna_SetupAction(this, DoorAna_GrabPlayer);
        } else {
            if (!Player_InCsMode(globalCtx) && !(player->stateFlags1 & 0x8800000) &&
                this->actor.xzDistToPlayer <= 15.0f && -50.0f <= this->actor.yDistToPlayer &&
                this->actor.yDistToPlayer <= 15.0f) {
                player->stateFlags1 |= 0x80000000;
                this->actor.targetMode = 1;
            } else {
                this->actor.targetMode = 0;
            }
        }
    }
    Actor_SetScale(&this->actor, this->actor.scale.x);
}

// update function for after the player has triggered the grotto
void DoorAna_GrabPlayer(DoorAna* this, GlobalContext* globalCtx) {
    Player* player;

    if (this->actor.yDistToPlayer <= 0.0f && 15.0f < this->actor.xzDistToPlayer) {
        player = GET_PLAYER(globalCtx);
        player->actor.world.pos.x = Math_SinS(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.x;
        player->actor.world.pos.z = Math_CosS(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.z;
    }
}

void DoorAna_Update(Actor* thisx, GlobalContext* globalCtx) {
    DoorAna* this = (DoorAna*)thisx;

    this->actionFunc(this, globalCtx);
    // changes the grottos facing angle based on camera angle
    this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000;
}

void DoorAna_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGrottoDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
