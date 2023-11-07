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
    { 50, 150, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

void EnChristmasTree_Init(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 90.0f);
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
    } else if ((this->actor.xzDistToPlayer < 170.0f)) { // talk range
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

    player->actor.world.pos.x = -929.336;
    player->actor.world.pos.y = 0;
    player->actor.world.pos.z = 446.178;
    player->actor.shape.rot.x = 0;
    player->actor.shape.rot.y = 17537;
    player->actor.shape.rot.z = 0;

    GameInteractor_SetNoUIActive(1);

    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_END_TITLE, 0, 0, 0, 0, 0, 0, 2, false);

    player->stateFlags1 = PLAYER_STATE1_INPUT_DISABLED;

    Flags_SetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY);

    this->actionFunc = EnChristmasTree_Wait;
}

void EnChristmasTree_Update(Actor* thisx, PlayState* play) {
    EnChristmasTree* this = (EnChristmasTree*)thisx;
    ColliderCylinder* collider = &this->collider;

    Collider_UpdateCylinder(thisx, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, (Collider*)collider);

    Actor_SetFocus(&this->actor, 80.0f);

    this->actionFunc(this, play);
}

void EnChristmasTree_Draw(Actor* thisx, PlayState* play) {
    float percentageCompleted = (float)gSaveContext.triforcePiecesCollected /
                                (float)Randomizer_GetSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED);
    float treeSize = 30.0f;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(treeSize, treeSize, treeSize, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasTreeDL);

    if (percentageCompleted >= 0.1f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor10DL);
    }
    if (percentageCompleted >= 0.2f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor20DL);
    }
    if (percentageCompleted >= 0.3f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor30DL);
    }
    if (percentageCompleted >= 0.4f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor40DL);
    }
    if (percentageCompleted >= 0.5f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor50DL);
    }
    if (percentageCompleted >= 0.6f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor60DL);
    }
    if (percentageCompleted >= 0.7f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor70DL);
    }
    if (percentageCompleted >= 0.8f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor80DL);
    }
    if (percentageCompleted >= 0.9f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor90DL);
    }
    if (percentageCompleted >= 1.0f) {
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor100DL);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasStarDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
