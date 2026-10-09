/*
 * File: z_door_ana.c
 * Overlay: ovl_Door_Ana
 * Description: Grotto Hole Entrance
 */

#include "z_door_ana.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_DURING_OCARINA)

void DoorAna_Init(Actor* thisx, PlayState* play);
void DoorAna_Destroy(Actor* thisx, PlayState* play);
void DoorAna_Update(Actor* thisx, PlayState* play);
void DoorAna_Draw(Actor* thisx, PlayState* play);

void DoorAna_WaitClosed(DoorAna* this, PlayState* play);
void DoorAna_WaitOpen(DoorAna* this, PlayState* play);
void DoorAna_GrabLink(DoorAna* this, PlayState* play);

ActorProfile Door_Ana_Profile = {
    /**/ ACTOR_DOOR_ANA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(DoorAna),
    /**/ DoorAna_Init,
    /**/ DoorAna_Destroy,
    /**/ DoorAna_Update,
    /**/ DoorAna_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 50, 10, 0, { 0, 0, 0 } },
};

static u16 sEntrances[] = {
    ENTRANCE(UNSET_0D, 0), ENTRANCE(GROTTOS, 0),  ENTRANCE(GROTTOS, 1),  ENTRANCE(GROTTOS, 2),  ENTRANCE(GROTTOS, 3),
    ENTRANCE(GROTTOS, 4),  ENTRANCE(GROTTOS, 5),  ENTRANCE(GROTTOS, 6),  ENTRANCE(GROTTOS, 7),  ENTRANCE(GROTTOS, 8),
    ENTRANCE(GROTTOS, 9),  ENTRANCE(GROTTOS, 10), ENTRANCE(GROTTOS, 11), ENTRANCE(GROTTOS, 12), ENTRANCE(GROTTOS, 13),
};

void DoorAna_SetupAction(DoorAna* this, DoorAnaActionFunc actionFunction) {
    this->actionFunc = actionFunction;
}

void DoorAna_Init(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;
    s32 grottoType = DOORANA_GET_TYPE(&this->actor);

    this->actor.shape.rot.y = this->actor.shape.rot.z = 0;

    if (grottoType == DOORANA_TYPE_HIDDEN_STORMS || grottoType == DOORANA_TYPE_HIDDEN_BOMB) {
        if (grottoType == DOORANA_TYPE_HIDDEN_BOMB) {
            Collider_InitAndSetCylinder(play, &this->bombCollider, &this->actor, &sCylinderInit);
        } else {
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED; // always update
        }

        Actor_SetScale(&this->actor, 0);
        DoorAna_SetupAction(this, DoorAna_WaitClosed);

    } else {
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_0;
}

void DoorAna_Destroy(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;
    s32 grottoType = DOORANA_GET_TYPE(&this->actor);

    if (grottoType == DOORANA_TYPE_HIDDEN_BOMB) {
        Collider_DestroyCylinder(play, &this->bombCollider);
    }
}

void DoorAna_WaitClosed(DoorAna* this, PlayState* play) {
    s32 grottoIsOpen = false;
    u32 grottoType = DOORANA_GET_TYPE(&this->actor);

    if (grottoType == DOORANA_TYPE_HIDDEN_STORMS) {
        //! @bug Implementation from OoT is not updated for MM, grotto does not open on Song of Storms
        if (this->actor.xyzDistToPlayerSq < SQ(200.0f) && CutsceneFlags_Get(play, 5)) {
            grottoIsOpen = true;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED; // always update OFF
        }

    } else {
        if (this->bombCollider.base.acFlags & AC_HIT) { // bomb collision
            grottoIsOpen = true;
            Collider_DestroyCylinder(play, &this->bombCollider);

        } else {
            Collider_UpdateCylinder(&this->actor, &this->bombCollider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bombCollider.base);
        }
    }

    if (grottoIsOpen) {
        DOORANA_SET_TYPE(&this->actor, DOORANA_TYPE_VISIBLE);
        DoorAna_SetupAction(this, DoorAna_WaitOpen);
        Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
    }

    Actor_SetClosestSecretDistance(&this->actor, play);
}

void DoorAna_WaitOpen(DoorAna* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 grottoType = DOORANA_GET_TYPE(&this->actor);

    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.001f)) {
        if ((this->actor.attentionRangeType != ATTENTION_RANGE_0) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
            (play->transitionMode == TRANS_MODE_OFF) && (player->stateFlags1 & PLAYER_STATE1_80000000) &&
            (player->av1.actionVar1 == 0)) {

            if (grottoType == DOORANA_TYPE_VISIBLE_SCENE_EXIT) {
                s32 exitIndex = DOORANA_GET_EXIT_INDEX(&this->actor);

                play->nextEntrance = play->setupExitList[exitIndex];
            } else {
                s32 destinationIdx = DOORANA_GET_ENTRANCE(&this->actor);

                Play_SetupRespawnPoint(play, RESPAWN_MODE_UNK_3, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_GROTTO));

                gSaveContext.respawn[RESPAWN_MODE_UNK_3].pos.y = this->actor.world.pos.y;
                gSaveContext.respawn[RESPAWN_MODE_UNK_3].yaw = this->actor.home.rot.y;

                // Stores item and chest flag that ACTOR_EN_TORCH uses for spawning the grotto chest
                gSaveContext.respawn[RESPAWN_MODE_UNK_3].data = DOORANA_GET_ITEMFLAGS(&this->actor);

                if (destinationIdx < 0) {
                    destinationIdx = DOORANA_GET_EX_ENTRANCE(&this->actor);
                }

                play->nextEntrance = sEntrances[destinationIdx];
            }

            DoorAna_SetupAction(this, DoorAna_GrabLink);

        } else if (!Play_InCsMode(play) && !(player->stateFlags1 & (PLAYER_STATE1_8000000 | PLAYER_STATE1_800000)) &&
                   (this->actor.xzDistToPlayer <= 20.0f) && (this->actor.playerHeightRel >= -50.0f) &&
                   (this->actor.playerHeightRel <= 15.0f)) {
            player->stateFlags1 |= PLAYER_STATE1_80000000;
            this->actor.attentionRangeType = ATTENTION_RANGE_1;

        } else {
            this->actor.attentionRangeType = ATTENTION_RANGE_0;
        }
    }

    Actor_SetScale(&this->actor, this->actor.scale.x);
}

void DoorAna_GrabLink(DoorAna* this, PlayState* play) {
    Player* player;
    s8 pad[2];

    if (CutsceneManager_GetCurrentCsId() != this->actor.csId) {
        if (CutsceneManager_IsNext(this->actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        } else {
            CutsceneManager_Queue(this->actor.csId);
        }
    }

    if ((this->actor.playerHeightRel <= 0.0f) && (this->actor.xzDistToPlayer > 20.0f)) {
        player = GET_PLAYER(play);
        player->actor.world.pos.x = this->actor.world.pos.x + Math_SinS(this->actor.yawTowardsPlayer) * 20.0f;
        player->actor.world.pos.z = this->actor.world.pos.z + Math_CosS(this->actor.yawTowardsPlayer) * 20.0f;
    }
}

void DoorAna_Update(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    this->actionFunc(this, play);
    if (GameInteractor_Should(VB_ROTATE_GROTTO_ENTRANCE, true)) {
        this->actor.shape.rot.y = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
    }
}

void DoorAna_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListXlu(play, gameplay_field_keep_DL_000C40);
}
