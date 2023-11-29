/*
 * File: z_en_christmastree.c
 * Overlay: ovl_En_ChristmasTree
 * Description: Custom Christmas Tree for Ornament Hunt
 */

#include "z_en_christmastree.h"
#include "soh_assets.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

void EnChristmasTree_Init(Actor* thisx, PlayState* play);
void EnChristmasTree_Destroy(Actor* thisx, PlayState* play);
void EnChristmasTree_Update(Actor* thisx, PlayState* play);
void EnChristmasTree_Draw(Actor* thisx, PlayState* play);

void EnChristmasTree_Wait(EnChristmasTree* this, PlayState* play);
void EnChristmasTree_Talk(EnChristmasTree* this, PlayState* play);
void EnChristmasTree_SetupEndTitle(EnChristmasTree* this, PlayState* play);
void EnChristmasTree_HandleEndTitle(EnChristmasTree* this, PlayState* play);

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
    { 100, 330, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

void EnChristmasTree_Init(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 160.0f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

    this->actor.targetMode = 1;
    this->actor.textId = 0x406B; // Hijacking bean seller text ID so I'm sure it doesn't clash

    this->actor.shape.rot.y = -14784;

    this->actionFunc = EnChristmasTree_Wait;
}

void EnChristmasTree_Destroy(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnChristmasTree_Wait(EnChristmasTree* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) { // if talk is initiated
        this->actionFunc = EnChristmasTree_Talk;
    } else if ((this->actor.xzDistToPlayer < 170.0f) && Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT)) { // talk range
        func_8002F2CC(&this->actor, play, 170.0f);
    }
}

void EnChristmasTree_Talk(EnChristmasTree* this, PlayState* play) {
    u8 dialogState = Message_GetState(&play->msgCtx);
    if (dialogState != TEXT_STATE_CHOICE) {
        if ((dialogState == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) { // advanced final textbox
            // Teleport to credits when goal is reached.
            if (gSaveContext.triforcePiecesCollected >= Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED)) {
                gSaveContext.sohStats.itemTimestamp[TIMESTAMP_TRIFORCE_COMPLETED] = GAMEPLAYSTAT_TOTAL_TIME;
                gSaveContext.sohStats.gameComplete = 1;
                Play_PerformSave(play);
                GameInteractor_SetTriforceHuntCreditsWarpActive(true);
                this->actionFunc = EnChristmasTree_SetupEndTitle;
            } else {
                this->actionFunc = EnChristmasTree_Wait;
            }
        }
    }
}

void EnChristmasTree_SetupEndTitle(EnChristmasTree* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Actor_Spawn(&play->actorCtx, play, ACTOR_END_TITLE, 0, 0, 0, 0, 0, 0, 2, false);

    player->stateFlags1 = PLAYER_STATE1_INPUT_DISABLED;

    Flags_SetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY);

    Play_PerformSave(play);

    this->actionFunc = EnChristmasTree_HandleEndTitle;
}

void EnChristmasTree_HandleEndTitle(EnChristmasTree* this, PlayState* play) {
    Camera* camera = Play_GetCamera(play, play->mainCamera.thisIdx);
    Player* player = GET_PLAYER(play);
    Vec3f camAt;
    Vec3f camEye;

    // Not forcing camera mode makes the camera jitter for a bit after setting position.
    // Also forces letterbox bars.
    Camera_ChangeMode(camera, CAM_MODE_STILL);

    // Christmas Tree's position
    camAt.x = -734.0f;
    camAt.y = 130.0f;
    camAt.z = 420.0f;

    // Camera's position
    camEye.x = -1237.0f;
    camEye.y = 218.0f;
    camEye.z = 408.0f;

    // Not setting fov manually makes camera zoom in after setting the above for a little bit.
    camera->fov = 60.0f;

    // Set camera
    Play_CameraSetAtEye(play, play->mainCamera.thisIdx, &camAt, &camEye);

    // Hide player so he's not visible in the final screen. Also move him so target arrow on tree dissapears.
    player->actor.scale.x = player->actor.scale.y = player->actor.scale.z = 0.00001f;
    player->actor.world.pos.z = 500.0f;

    // Hide HUD
    Interface_ChangeAlpha(1);
}

void EnChristmasTree_Update(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;
    ColliderCylinder* collider = &this->collider;

    Collider_UpdateCylinder(thisx, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, (Collider*)collider);

    Actor_SetFocus(&this->actor, 80.0f);

    uint8_t triforceHuntActive = Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT);
    float percentageCompleted = (float)gSaveContext.triforcePiecesCollected /
                                (float)Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED);

    if ((percentageCompleted >= 1.0f || !triforceHuntActive) && !this->spawnedRupee) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WONDER_ITEM, this->actor.world.pos.x, this->actor.world.pos.y + 280,
                    this->actor.world.pos.z, 0, 0, LINK_IS_ADULT ? 1 : 4, 0x1ABF, false);
        this->spawnedRupee = 1;
    }

    this->actionFunc(this, play);
}

void EnChristmasTree_Draw(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;
    
    float treeSize = 55.0f;
    uint8_t triforceHuntActive = Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT);
    float percentageCompleted = (float)gSaveContext.triforcePiecesCollected /
                                (float)Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(treeSize, treeSize, treeSize, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasTreeDL);

    if (percentageCompleted >= 0.1f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor10DL);
    }
    if (percentageCompleted >= 0.2f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor20DL);
    }
    if (percentageCompleted >= 0.3f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor30DL);
    }
    if (percentageCompleted >= 0.4f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor40DL);
    }
    if (percentageCompleted >= 0.5f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor50DL);
    }
    if (percentageCompleted >= 0.6f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor60DL);
    }
    if (percentageCompleted >= 0.7f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor70DL);
    }
    if (percentageCompleted >= 0.8f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor80DL);
    }
    if (percentageCompleted >= 0.9f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor90DL);
    }
    if (percentageCompleted >= 1.0f || !triforceHuntActive) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor100DL);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasStarDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
