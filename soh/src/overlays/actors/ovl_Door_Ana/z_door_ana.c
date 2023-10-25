/*
 * File: z_door_ana.c
 * Overlay: ovl_Door_Ana
 * Description: Grottos Entrances/Exits
 */

#include "z_door_ana.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"

#define FLAGS ACTOR_FLAG_NO_FREEZE_OCARINA

void DoorAna_Init(Actor* thisx, PlayState* play);
void DoorAna_Destroy(Actor* thisx, PlayState* play);
void DoorAna_Update(Actor* thisx, PlayState* play);
void DoorAna_Draw(Actor* thisx, PlayState* play);

void DoorAna_WaitClosed(DoorAna* this, PlayState* play);
void DoorAna_WaitOpen(DoorAna* this, PlayState* play);
void DoorAna_GrabPlayer(DoorAna* this, PlayState* play);

s16 GetChestGameRandoGiDrawId(s8 room, s16 ogDrawId, PlayState* play);
void Grotto_OverrideActorEntrance(Actor* thisx);

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
static s16 sGrottoEntrances[] = {
    ENTR_FAIRYS_FOUNTAIN_0, ENTR_GROTTOS_0,  ENTR_GROTTOS_1,  ENTR_GROTTOS_2,  ENTR_GROTTOS_3,
    ENTR_GROTTOS_4,         ENTR_GROTTOS_5,  ENTR_GROTTOS_6,  ENTR_GROTTOS_7,  ENTR_GROTTOS_8,
    ENTR_GROTTOS_9,         ENTR_GROTTOS_10, ENTR_GROTTOS_11, ENTR_GROTTOS_12, ENTR_GROTTOS_13,
};

void DoorAna_SetupAction(DoorAna* this, DoorAnaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DoorAna_Init(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.shape.rot.z;
    // init block for grottos that are initially "hidden" (require explosives/hammer/song of storms to open)
    if ((this->actor.params & 0x300) != 0) {
        // only allocate collider for grottos that need bombing/hammering open
        if ((this->actor.params & 0x200) != 0) {
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        } else {
            this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
        }
        Actor_SetScale(&this->actor, 0);
        DoorAna_SetupAction(this, DoorAna_WaitClosed);
    } else {
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
    }
    this->actor.targetMode = 0;
}

void DoorAna_Destroy(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    // free collider if it has one
    if ((this->actor.params & 0x200) != 0) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

// update routine for grottos that are currently "hidden"/unopened
void DoorAna_WaitClosed(DoorAna* this, PlayState* play) {
    u32 openGrotto = false;

    if (!(this->actor.params & 0x200)) {
        // opening with song of storms
        if (this->actor.xyzDistToPlayerSq < 40000.0f && Flags_GetEnv(play, 5)) {
            openGrotto = true;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_WHILE_CULLED;
        }
    } else {
        // bombing/hammering open a grotto
        if (this->collider.base.acFlags & AC_HIT) {
            openGrotto = true;
            Collider_DestroyCylinder(play, &this->collider);
        } else {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    // open the grotto
    if (openGrotto) {
        this->actor.params &= ~0x0300;
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    func_8002F5F0(&this->actor, play);
}

// update routine for grottos that are open
void DoorAna_WaitOpen(DoorAna* this, PlayState* play) {
    Player* player;
    s32 destinationIdx;

    player = GET_PLAYER(play);
    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.001f)) {
        if ((this->actor.targetMode != 0) && (play->transitionTrigger == TRANS_TRIGGER_OFF) && (player->stateFlags1 & 0x80000000) &&
            (player->unk_84F == 0)) {
            destinationIdx = ((this->actor.params >> 0xC) & 7) - 1;
            Play_SetupRespawnPoint(play, RESPAWN_MODE_RETURN, 0x4FF);
            gSaveContext.respawn[RESPAWN_MODE_RETURN].pos.y = this->actor.world.pos.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].yaw = this->actor.home.rot.y;
            gSaveContext.respawn[RESPAWN_MODE_RETURN].data = this->actor.params & 0xFFFF;
            if (destinationIdx < 0) {
                destinationIdx = this->actor.home.rot.z + 1;
            }
            play->nextEntranceIndex = sGrottoEntrances[destinationIdx];

            // In ER, load the correct entrance based on the grotto link is falling into
            if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES)) {
                Grotto_OverrideActorEntrance(&this->actor);
            }

            DoorAna_SetupAction(this, DoorAna_GrabPlayer);
        } else {
            if (!Player_InCsMode(play) && !(player->stateFlags1 & 0x8800000) &&
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
void DoorAna_GrabPlayer(DoorAna* this, PlayState* play) {
    Player* player;

    if (this->actor.yDistToPlayer <= 0.0f && 15.0f < this->actor.xzDistToPlayer) {
        player = GET_PLAYER(play);
        player->actor.world.pos.x = Math_SinS(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.x;
        player->actor.world.pos.z = Math_CosS(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.z;
    }
}

void DoorAna_Update(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    this->actionFunc(this, play);
    // Changes the grottos facing angle based on camera angle
    if (!CVarGetInteger("gDisableGrottoRotation", 0)) {
        this->actor.shape.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
    }
}

void DoorAna_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gGrottoDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
