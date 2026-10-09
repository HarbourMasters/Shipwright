/*
 * File: z_dm_char08.c
 * Overlay: ovl_Dm_Char08
 * Description: Large Great Bay Turtle
 */

#include "z_dm_char08.h"
#include "objects/object_kamejima/object_kamejima.h"
#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "stdlib.h"

#define FLAGS (ACTOR_FLAG_UPDATE_DURING_OCARINA)

void DmChar08_Init(Actor* thisx, PlayState* play2);
void DmChar08_Destroy(Actor* thisx, PlayState* play);
void DmChar08_Update(Actor* thisx, PlayState* play);
void DmChar08_Draw(Actor* thisx, PlayState* play);
void DmChar08_Reset(void);

void func_80AAFAC4(DmChar08* this, PlayState* play);
void DmChar08_WaitForSong(DmChar08* this, PlayState* play);
void func_80AAF8F4(DmChar08* this, PlayState* play);
void func_80AAFAE4(DmChar08* this, PlayState* play);
void DmChar08_DoNothing(DmChar08* this, PlayState* play);
void func_80AAFA18(DmChar08* this, PlayState* play);
void DmChar08_SetupAppearCs(DmChar08* this, PlayState* play);
void func_80AAF884(DmChar08* this, PlayState* play);
void func_80AAFB04(DmChar08* this, PlayState* play);
void func_80AAFB94(DmChar08* this, PlayState* play);

typedef enum {
    /* 0 */ TURTLE_EYEMODE_BLINK_LEFT,
    /* 1 */ TURTLE_EYEMODE_BLINK_STRAIGHT,
    /* 2 */ TURTLE_EYEMODE_CLOSED,
    /* 3 */ TURTLE_EYEMODE_LOOK_STRAIGHT,
    /* 4 */ TURTLE_EYEMODE_UNUSED,
    /* 5 */ TURTLE_EYEMODE_LOOK_RIGHT
} TurtleEyeMode;

ActorProfile Dm_Char08_Profile = {
    ACTOR_DM_CHAR08,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_KAMEJIMA,
    sizeof(DmChar08),
    (ActorFunc)DmChar08_Init,
    (ActorFunc)DmChar08_Destroy,
    (ActorFunc)DmChar08_Update,
    (ActorFunc)DmChar08_Draw,
    /**/ DmChar08_Reset,
};

#include "overlays/ovl_Dm_Char08/ovl_Dm_Char08.h"

typedef enum {
    /*  0 */ TURTLE_ANIM_IDLE,
    /*  1 */ TURTLE_ANIM_SWIM,
    /*  2 */ TURTLE_ANIM_FLOAT,
    /*  3 */ TURTLE_ANIM_SPEAK1,
    /*  4 */ TURTLE_ANIM_COUGH,
    /*  5 */ TURTLE_ANIM_SPEAK2,
    /*  6 */ TURTLE_ANIM_YAWN,
    /*  7 */ TURTLE_ANIM_MAX,
    /* 99 */ TURTLE_ANIM_NONE = 99
} TurtleAnimation;

static AnimationInfo sAnimationInfo[TURTLE_ANIM_MAX] = {
    { &gTurtleIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f },   // TURTLE_ANIM_IDLE
    { &gTurtleSwimAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f },   // TURTLE_ANIM_SWIM
    { &gTurtleFloatAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f },  // TURTLE_ANIM_FLOAT
    { &gTurtleSpeak1Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f }, // TURTLE_ANIM_SPEAK1
    { &gTurtleCoughAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f },  // TURTLE_ANIM_COUGH
    { &gTurtleSpeak2Anim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f }, // TURTLE_ANIM_SPEAK2
    { &gTurtleYawnAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -24.0f },   // TURTLE_ANIM_YAWN
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 4000, ICHAIN_STOP),
};

static CollisionHeader* sTurtleGreatBayTempleColData;
static CollisionPoly* sTurtleGreatBayTempleColPolygonsData;
static CollisionPoly* sTurtleGreatBayTempleColPolygons2Data;
static Vec3s* sTurtleGreatBayTempleColVerticesOrigData;
static Vec3s* sTurtleGreatBayTempleColVerticesData;
static Vec3s* sTurtleGreatBayTempleColVertices2Data;

void DmChar08_UpdateEyes(DmChar08* this) {
    switch (this->eyeMode) {
        case TURTLE_EYEMODE_BLINK_LEFT:
            this->eyeIndex = 0;
            if (this->blinkTimer > 0) {
                this->blinkTimer--;
            } else {
                this->blinkTimer = 0;
            }
            if (this->blinkTimer < 4) {
                this->eyeIndex = this->blinkTimer;
            }
            if (this->blinkTimer == 0) {
                this->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;

        case TURTLE_EYEMODE_BLINK_STRAIGHT:
            this->eyeIndex = 4;
            if (this->blinkTimer > 0) {
                this->blinkTimer--;
            } else {
                this->blinkTimer = 0;
            }
            if (this->blinkTimer < 4) {
                this->eyeIndex = this->blinkTimer + 4;
            }
            if (this->blinkTimer == 0) {
                this->blinkTimer = Rand_S16Offset(30, 30);
            }
            break;

        case TURTLE_EYEMODE_CLOSED:
            this->eyeIndex = 2;
            break;

        case TURTLE_EYEMODE_LOOK_STRAIGHT:
            this->eyeIndex = 4;
            break;

        case TURTLE_EYEMODE_LOOK_RIGHT:
            this->eyeIndex = 8;
            break;

        default:
            break;
    }
}

void DmChar08_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
    f32 endFrame;

    animInfo += animIndex;

    if (animInfo->frameCount < 0.0f) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

void DmChar08_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DmChar08* this = (DmChar08*)thisx;

    // region 2S2H [Port] The turtle does some unique things with CPU modified collision poly
    // The effect is to have a the collision move with the turtle's fin. Depending on the player's
    // position, the collision swaps to the closest fin.
    // To handle this, we setup and track temporary pointers for holding the original values and
    // staging pointers attached over the resource that we will modify.
    if (sTurtleGreatBayTempleColData == NULL) {
        sTurtleGreatBayTempleColData = ResourceMgr_LoadColByName(sTurtleGreatBayTempleCol);
        // These vertex lists need to be manually freed when we are done
        sTurtleGreatBayTempleColVerticesData = ResourceMgr_LoadArrayByNameAsVec3s(sTurtleGreatBayTempleColVertices);
        sTurtleGreatBayTempleColVertices2Data = ResourceMgr_LoadArrayByNameAsVec3s(sTurtleGreatBayTempleColVertices2);

        // Track the original vtx list and attach the staged pointer
        sTurtleGreatBayTempleColVerticesOrigData = sTurtleGreatBayTempleColData->vtxList;
        sTurtleGreatBayTempleColData->vtxList = sTurtleGreatBayTempleColVerticesData;

        // The original poly list matches the right fin. So we create a copy for the left fin.
        sTurtleGreatBayTempleColPolygonsData = sTurtleGreatBayTempleColData->polyList;
        sTurtleGreatBayTempleColPolygons2Data =
            (CollisionPoly*)malloc(sizeof(CollisionPoly) * sTurtleGreatBayTempleColData->numPolygons);
        memcpy(sTurtleGreatBayTempleColPolygons2Data, sTurtleGreatBayTempleColPolygonsData,
               sizeof(CollisionPoly) * sTurtleGreatBayTempleColData->numPolygons);

        // Swap the order of the triangles to invert the direction of the faces
        for (int i = 0; i < sTurtleGreatBayTempleColData->numPolygons; i++) {
            u16 temp = sTurtleGreatBayTempleColPolygons2Data[i].vtxData[1];
            sTurtleGreatBayTempleColPolygons2Data[i].vtxData[1] = sTurtleGreatBayTempleColPolygons2Data[i].vtxData[2];
            sTurtleGreatBayTempleColPolygons2Data[i].vtxData[2] = temp;
        }
    }
    // #endregion

    thisx->attentionRangeType = ATTENTION_RANGE_5;
    this->eyeMode = TURTLE_EYEMODE_CLOSED;
    thisx->lockOnArrowOffset = 120.0f;
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gTurtleSkel, NULL, NULL, NULL, 0);
    Actor_ProcessInitChain(thisx, sInitChain);
    Actor_SetScale(thisx, 0.1f);
    this->unk_1F4 = 0;
    this->bubbleCount = 0;
    this->unk_1FF = 0;
    this->alpha = 0;
    this->animIndex = TURTLE_ANIM_IDLE;
    this->unk_1FC = 0xFFFF;
    this->dynapolyInitialized = false;
    this->targetYPos = thisx->world.pos.y;
    this->unk_1F0 = 0.0f;
    if (play->sceneId == SCENE_31MISAKI) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_20)) {
            DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gTurtleZoraCapeAwakeCol);
        } else {
            DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gTurtleZoraCapeAsleepCol);
        }
        this->dynapolyInitialized = true;
    } else if (play->sceneId == SCENE_SEA) {
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
        DynaPolyActor_LoadMesh(play, &this->dyna, sTurtleGreatBayTempleColData);
        this->dynapolyInitialized = true;
    }

    this->palmTree1 = Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_YASI, thisx->world.pos.x + -80.0f,
                                  thisx->world.pos.y + 390.0f, thisx->world.pos.z, 0, 0, 0, 1);
    this->palmTree2 = Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_YASI, thisx->world.pos.x + 68.0f,
                                  thisx->world.pos.y + 368.0f, thisx->world.pos.z - 174.0f, 0, 0x7530, 0, 1);

    switch (play->sceneId) {
        case SCENE_31MISAKI:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_53_20)) {
                thisx->world.pos.x = -6480.0f;
                this->targetYPos = -120.0f;
                thisx->world.pos.z = 1750.0f;
                thisx->world.rot.x = 0;
                thisx->world.rot.y = -0x7234;
                thisx->world.rot.z = 0;
                thisx->shape.rot.x = 0;
                thisx->shape.rot.y = -0x7234;
                thisx->shape.rot.z = 0;
                this->unk_1F0 = 1.0f;
                this->unk_1FF = 2;
                this->animIndex = TURTLE_ANIM_FLOAT;
                this->prevAnimIndex = TURTLE_ANIM_NONE;
                this->eyeMode = TURTLE_EYEMODE_BLINK_STRAIGHT;
                this->unk_207 = 0;
                this->unk_208 = 0;
                thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                if (gSaveContext.save.entrance == ENTRANCE(ZORA_CAPE, 8)) {
                    this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    this->actionFunc = func_80AAFAC4;
                } else {
                    this->actionFunc = func_80AAF8F4;
                }
            } else {
                this->actionFunc = DmChar08_WaitForSong;
            }
            break;

        case SCENE_SEA:
            this->unk_1FF = 2;
            this->animIndex = TURTLE_ANIM_FLOAT;
            this->prevAnimIndex = TURTLE_ANIM_NONE;
            this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
            this->unk_207 = 0;
            this->unk_208 = 0;
            thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = func_80AAFAE4;
            this->unk_1F0 = 1.0f;
            break;

        case SCENE_KONPEKI_ENT:
            this->unk_1FF = 2;
            this->animIndex = TURTLE_ANIM_FLOAT;
            this->prevAnimIndex = TURTLE_ANIM_NONE;
            this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
            this->unk_207 = 0;
            this->unk_208 = 0;
            this->actionFunc = DmChar08_DoNothing;
            this->unk_1F0 = 1.0f;
            break;

        default:
            break;
    }
    DmChar08_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
}

void DmChar08_Destroy(Actor* thisx, PlayState* play) {
    DmChar08* this = (DmChar08*)thisx;

    if (this->dynapolyInitialized) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static s16 sSuccessSoundAlreadyPlayed = false;

void DmChar08_WaitForSong(DmChar08* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    if ((player2->stateFlags2 & PLAYER_STATE2_USING_OCARINA) &&
        ((player2->actor.world.pos.x > -5780.0f) && (player2->actor.world.pos.x < -5385.0f) &&
         (player2->actor.world.pos.z > 1120.0f) && (player2->actor.world.pos.z < 2100.0f))) {
        if (!sSuccessSoundAlreadyPlayed) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            sSuccessSoundAlreadyPlayed = true;
        }
    } else {
        sSuccessSoundAlreadyPlayed = false;
    }
    if (GameInteractor_Should(VB_OPEN_GREAT_BAY_FROM_SONG,
                              (player->transformation == PLAYER_FORM_ZORA) &&
                                  (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
                                  (play->msgCtx.lastPlayedSong == OCARINA_SONG_NEW_WAVE),
                              this)) {
        if ((player2->actor.world.pos.x > -5780.0f) && (player2->actor.world.pos.x < -5385.0f)) {
            if ((player2->actor.world.pos.z > 1120.0f) && (player2->actor.world.pos.z < 2100.0f)) {
                this->actionFunc = DmChar08_SetupAppearCs;
            }
        }
    }
}

void DmChar08_SetupAppearCs(DmChar08* this, PlayState* play) {
    s16 csId = this->dyna.actor.csId;
    s16 additionalCsId =
        CutsceneManager_GetAdditionalCsId(CutsceneManager_GetAdditionalCsId(CutsceneManager_GetAdditionalCsId(csId)));

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_93_08)) {
        csId = additionalCsId;
    }

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_Start(csId, &this->dyna.actor);
        SET_WEEKEVENTREG(WEEKEVENTREG_53_20);
        SET_WEEKEVENTREG(WEEKEVENTREG_93_08);
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = func_80AAF884;
    } else {
        CutsceneManager_Queue(csId);
    }
}

void func_80AAF884(DmChar08* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
        DynaPolyActor_LoadMesh(play, &this->dyna, &gTurtleZoraCapeAwakeCol);
        this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = func_80AAF8F4;
    }
}

void func_80AAF8F4(DmChar08* this, PlayState* play) {
    s32 pad;
    f32 yOffset;

    if (fabsf(this->dyna.actor.xzDistToPlayer) < 200.0f) {
        this->actionFunc = func_80AAFA18;
    }
    this->unk_1FA += 200;
    yOffset = 50.0f;
    yOffset *= Math_SinS(this->unk_1FA);
    Math_SmoothStepToF(&this->dyna.actor.world.pos.x, -6400.0f, 0.2f, 2.0f, 0.1f);
    Math_SmoothStepToF(&this->targetYPos, -180.0f + yOffset, 0.5f, 5.0f, 0.1f);
    Math_SmoothStepToF(&this->dyna.actor.world.pos.z, 1750.0f, 0.5f, 20.0f, 0.1f);
    Math_SmoothStepToS(&this->dyna.actor.world.rot.y, -0x7234, 0xA, 0xDC, 1);
    Math_SmoothStepToS(&this->dyna.actor.shape.rot.y, -0x7234, 0xA, 0xDC, 1);
}

void func_80AAFA18(DmChar08* this, PlayState* play) {
    s16 nextCsId;
    s16 nextCsId2;
    s16 nextCsId1;

    nextCsId1 = CutsceneManager_GetAdditionalCsId(this->dyna.actor.csId);
    nextCsId2 = nextCsId1;
    nextCsId1 = CutsceneManager_GetAdditionalCsId(nextCsId1);

    nextCsId = CHECK_WEEKEVENTREG(WEEKEVENTREG_53_40) ? nextCsId1 : nextCsId2;

    if (CutsceneManager_IsNext(nextCsId)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_53_40);
        CutsceneManager_Start(nextCsId, &this->dyna.actor);
        this->actionFunc = DmChar08_DoNothing;
    } else {
        CutsceneManager_Queue(nextCsId);
    }
}

void func_80AAFAC4(DmChar08* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = func_80AAF8F4;
    }
}

void func_80AAFAE4(DmChar08* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = func_80AAFB04;
    }
}

void func_80AAFB04(DmChar08* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((fabsf(this->dyna.actor.xzDistToPlayer) < 200.0f) && (player->actor.world.pos.y > 0.0f)) {
        play->nextEntrance = ENTRANCE(ZORA_CAPE, 7);
        play->transitionTrigger = TRANS_TRIGGER_START;
        Scene_SetExitFade(play);
        this->actionFunc = func_80AAFB94;
    }
}

void func_80AAFB94(DmChar08* this, PlayState* play) {
}

Color_RGBA8 sLargeTurtleBubblePrimColor = { 250, 200, 250, 100 };
Color_RGBA8 sLargeTurtleBubbleEnvColor = { 80, 80, 80, 0 };
Vec3f sLargeTurtleBubbleAccel = { 0.0f, -0.8f, 0.0f };

void DmChar08_SpawnBubbles(DmChar08* this, PlayState* play) {
    s32 i;
    Vec3f velocity;

    velocity.x = (Rand_ZeroOne() - 0.5f) * 24.0f;
    velocity.y = -5.2f;
    velocity.z = 4.0f;
    sLargeTurtleBubbleAccel.x = 0.0f;
    sLargeTurtleBubbleAccel.y = 1.18f;
    sLargeTurtleBubbleAccel.z = 0.0f;
    for (i = 0; i < this->bubbleCount; i++) {
        EffectSsDtBubble_SpawnCustomColor(play, &this->bubblePos, &velocity, &sLargeTurtleBubbleAccel,
                                          &sLargeTurtleBubblePrimColor, &sLargeTurtleBubbleEnvColor,
                                          Rand_S16Offset(80, 130), 30, 0);
    }
}

void func_80AAFCCC(DmChar08* this, PlayState* play) {
    switch (play->sceneId) {
        case SCENE_31MISAKI:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                switch (this->unk_206) {
                    case 0:
                        break;

                    case 1:
                        // Fire hookshot
                        Message_StartTextbox(play, 0x102A, &this->dyna.actor);
                        this->unk_206++;
                        break;

                    case 2:
                        // I know your name
                        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                            Message_StartTextbox(play, 0x102B, &this->dyna.actor);
                            this->unk_206 = 0;
                        }
                        break;

                    default:
                        break;
                }
            } else {
                switch (this->unk_206) {
                    case 0:
                        break;

                    case 1:
                        // Going to the temple again
                        Message_StartTextbox(play, 0x103B, &this->dyna.actor);
                        this->unk_206++;
                        break;

                    case 2:
                        if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                            Message_CloseTextbox(play);
                            this->unk_206 = 0;
                        }
                        break;

                    default:
                        break;
                }
            }
            return;

        case SCENE_SEA:
            switch (this->unk_206) {
                case 0:
                    break;

                case 1:
                    Message_StartTextbox(play, 0x102E, &this->dyna.actor);
                    this->unk_206++;
                    break;

                case 2:
                    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
                        Message_StartTextbox(play, 0x102F, &this->dyna.actor);
                        this->unk_206 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void DmChar08_DoNothing(DmChar08* this, PlayState* play) {
}

void DmChar08_HandleCutscene(DmChar08* this, PlayState* play) {
    s32 cueChannel;
    s32 pad;
    f32 phi_f12;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_474)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_474);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->animIndex = TURTLE_ANIM_IDLE;
                    break;

                case 3:
                    this->bubbleCount = 2;
                    break;

                case 4:
                    this->animIndex = TURTLE_ANIM_FLOAT;
                    break;

                case 5:
                    this->animIndex = TURTLE_ANIM_SWIM;
                    break;

                case 6:
                    this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    break;

                case 7:
                    this->eyeMode = TURTLE_EYEMODE_CLOSED;
                    break;

                case 8:
                    this->animIndex = TURTLE_ANIM_YAWN;
                    break;

                case 9:
                    this->animIndex = TURTLE_ANIM_FLOAT;
                    break;

                case 10:
                    this->animIndex = TURTLE_ANIM_COUGH;
                    break;

                case 12:
                    this->animIndex = TURTLE_ANIM_SPEAK2;
                    break;

                case 13:
                    this->bubbleCount = 0;
                    break;

                case 14:
                    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BIG_TORTOISE_ROLL);
                    this->animIndex = TURTLE_ANIM_FLOAT;
                    break;

                default:
                    break;
            }
        }
        switch (play->csCtx.actorCues[cueChannel]->id) {
            case 2:
                this->unk_1FF = 1;
                phi_f12 =
                    2.0f * Environment_LerpWeight(play->csCtx.actorCues[cueChannel]->endFrame,
                                                  play->csCtx.actorCues[cueChannel]->startFrame, play->csCtx.curFrame);
                if (phi_f12 > 1.0f) {
                    phi_f12 = 1.0f;
                }

                this->alpha = (1.0f - phi_f12) * 255.0f;
                if (this->alpha < 6) {
                    this->unk_1FF = 2;
                }

                Cutscene_ActorTranslateAndYaw(&this->dyna.actor, play, cueChannel);
                break;

            case 5:
                Cutscene_ActorTranslateAndYawSmooth(&this->dyna.actor, play, cueChannel);
                break;

            case 14:
                Cutscene_ActorTranslate(&this->dyna.actor, play, cueChannel);
                Math_SmoothStepToS(&this->dyna.actor.world.rot.y, play->csCtx.actorCues[cueChannel]->rot.y, 0xA, 0xDC,
                                   1);
                this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
                break;

            default:
                Cutscene_ActorTranslateAndYaw(&this->dyna.actor, play, cueChannel);
                break;
        }
        this->targetYPos = this->dyna.actor.world.pos.y;
        if ((this->unk_1FF >= 2) || (play->csCtx.actorCues[cueChannel]->id == 2)) {
            Math_SmoothStepToF(&this->unk_1F0, 1.0f, 0.02f, 0.1f, 0.00001f);
        }
    } else {
        this->cueId = 99;
    }
}

void DmChar08_UpdateAnim(DmChar08* this, PlayState* play) {
    if (this->animIndex != this->prevAnimIndex) {
        this->prevAnimIndex = this->animIndex;
        DmChar08_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
    }
}

void func_80AB023C(DmChar08* this, PlayState* play) {
    if ((play->msgCtx.currentTextId != this->unk_1FC) && (play->msgCtx.currentTextId != 0)) {
        this->unk_1FC = play->msgCtx.currentTextId;
        this->unk_208 = 0;
        switch (play->msgCtx.currentTextId) {
            case 0x1025: // Slept well
                this->unk_207 = 1;
                break;

            case 0x1026: // Time Flies
                this->unk_207 = 2;
                break;

            case 0x1027: // See Everything
                this->unk_207 = 3;
                break;

            case 0x1028: // No time
                this->unk_207 = 4;
                break;

            case 0x1029: // Quick lets go
                this->unk_207 = 10;
                break;

            case 0x102C: // Having trouble
                this->unk_207 = 5;
                break;

            case 0x102D: // Ready, climb
                this->unk_207 = 8;
                break;

            case 0x102A: // Fire hookshot
                this->unk_207 = 6;
                break;

            case 0x102B: // I know your name
                this->unk_207 = 2;
                break;

            case 0x102E: // Returning to shore
                this->unk_207 = 9;
                break;

            case 0x102F: // It is okay to retreat
                this->unk_207 = 7;
                break;

            case 0x103B: // Going to the temple again
                this->unk_207 = 6;

                break;
            case 0x1030: // Return to sleep
                this->unk_207 = 4;
                break;

            default:
                break;
        }
    }
}

void func_80AB032C(DmChar08* this, PlayState* play) {
    switch (this->unk_207) {
        case 1:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_YAWN;
                    this->eyeMode = TURTLE_EYEMODE_CLOSED;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 2:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_COUGH;
                    this->eyeMode = TURTLE_EYEMODE_CLOSED;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK1;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_208++;
                    }
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 3:
        case 4:
        case 8:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK1;
                    if (this->unk_207 >= 5) {
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                    } else {
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    }
                    this->unk_208 = 2;
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK2;
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                        this->unk_208++;
                    }
                    break;

                case 3:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        if (this->unk_207 != 4) {
                            this->animIndex = TURTLE_ANIM_FLOAT;
                            this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                            this->unk_207 = 0;
                            this->unk_208 = 0;
                        } else {
                            this->animIndex = TURTLE_ANIM_SPEAK1;
                            this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                            this->unk_208++;
                        }
                    }
                    break;

                case 4:
                case 5:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->unk_208++;
                    }
                    break;

                case 6:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 5:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK1;
                    this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    this->unk_208++;
                    break;

                case 2:
                case 3:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->unk_208++;
                    }
                    break;

                case 4:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK2;
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                        this->unk_208++;
                    }
                    break;

                case 5:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 6:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK1;
                    this->eyeMode = TURTLE_EYEMODE_LOOK_RIGHT;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->unk_208++;
                    }
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 7:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK2;
                    this->eyeMode = TURTLE_EYEMODE_CLOSED;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK1;
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                        this->unk_208++;
                    }
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 9:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK1;
                    this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->unk_208++;
                    }
                    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
                        this->unk_208 = 3;
                    }
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_208++;
                    }
                    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
                        this->unk_208 = 3;
                    }
                    break;

                case 3:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_COUGH;
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                        this->unk_208++;
                    }
                    break;

                case 4:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        case 10:
            switch (this->unk_208) {
                case 0:
                    this->animIndex = TURTLE_ANIM_SPEAK1;
                    this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                    this->unk_208++;
                    break;

                case 1:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK2;
                        this->eyeMode = TURTLE_EYEMODE_CLOSED;
                        this->unk_208++;
                    }
                    break;

                case 2:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_SPEAK1;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_208++;
                    }
                    break;

                case 3:
                    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                        this->animIndex = TURTLE_ANIM_FLOAT;
                        this->eyeMode = TURTLE_EYEMODE_BLINK_LEFT;
                        this->unk_207 = 0;
                        this->unk_208 = 0;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void func_80AB096C(DmChar08* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->sceneId == SCENE_31MISAKI) && (gSaveContext.sceneLayer == 0) &&
        (play->csCtx.scriptIndex == 0)) {
        if ((play->csCtx.curFrame >= 890) && (play->csCtx.curFrame < 922)) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_EARTHQUAKE_LAST2 - SFX_FLAG);
        }
    }
    if ((this->animIndex == TURTLE_ANIM_SWIM) && Animation_OnFrame(&this->skelAnime, 16.0f)) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BIG_TORTOISE_SWIM);
    }
}

void DmChar08_UpdateCollision(DmChar08* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame;
    f32 phi_f2;
    f32 phi_f0;
    f32 phi_f12;
    s32 i;

    if (player->actor.world.pos.x > 0.0f) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame <= 19.0f) {
            phi_f12 = 19.0f;
            phi_f2 = curFrame / 19.0f;
        } else {
            phi_f2 = (29.0f - curFrame) / 9.0f;
            phi_f12 = 29.0f;
        }

        sTurtleGreatBayTempleColData->polyList = sTurtleGreatBayTempleColPolygonsData;

        for (i = 0; i < sTurtleGreatBayTempleColData->numVertices; i++) {
            sTurtleGreatBayTempleColVerticesData[i].x = sTurtleGreatBayTempleColVertices2Data[i].x;
        }

        sTurtleGreatBayTempleColVerticesData[0].y = (100.0f * phi_f2) + 900.0f;
        sTurtleGreatBayTempleColVerticesData[1].y = (100.0f * phi_f2) + 900.0f;
        sTurtleGreatBayTempleColVerticesData[2].y = (500.0f * phi_f2) + -200.0f;
        sTurtleGreatBayTempleColVerticesData[3].y = (900.0f * phi_f2) + -800.0f;
        sTurtleGreatBayTempleColVerticesData[5].y = 0x4B0;
        sTurtleGreatBayTempleColVerticesData[9].y = 0x6A4;
    } else {
        phi_f0 = this->skelAnime.curFrame + 26.0f;
        if (phi_f0 > 29.0f) {
            phi_f12 = 29.0f;
            phi_f0 -= 29.0f;
        }

        if (phi_f0 <= 18.0f) {
            phi_f2 = phi_f0 / 18.0f;
        } else {
            phi_f2 = (29.0f - phi_f0) / 10.0f;
        }

        sTurtleGreatBayTempleColData->polyList = sTurtleGreatBayTempleColPolygons2Data;

        for (i = 0; i < sTurtleGreatBayTempleColData->numVertices; i++) {
            sTurtleGreatBayTempleColVerticesData[i].x = -sTurtleGreatBayTempleColVertices2Data[i].x;
        }

        sTurtleGreatBayTempleColVerticesData[0].y = (500.0f * phi_f2) + 720.0f;
        sTurtleGreatBayTempleColVerticesData[1].y = (660.0f * phi_f2) + 420.0f;
        sTurtleGreatBayTempleColVerticesData[2].y = (1130.0f * phi_f2) + -430.0f;
        sTurtleGreatBayTempleColVerticesData[3].y = (1430.0f * phi_f2) + -1060.0f;
        sTurtleGreatBayTempleColVerticesData[5].y = 0x4B0;
        sTurtleGreatBayTempleColVerticesData[9].y = 0x6A4;
    }
    DynaPoly_InvalidateLookup(play, &play->colCtx.dyna);
}

void DmChar08_Update(Actor* thisx, PlayState* play) {
    DmChar08* this = (DmChar08*)thisx;

    this->dyna.actor.focus.pos.x = this->focusPos.x;
    this->dyna.actor.focus.pos.y = this->focusPos.y + this->dyna.actor.lockOnArrowOffset;
    this->dyna.actor.focus.pos.z = this->focusPos.z;
    this->dyna.actor.focus.rot.x = this->dyna.actor.world.rot.x;
    this->dyna.actor.focus.rot.y = this->dyna.actor.world.rot.y;
    this->dyna.actor.focus.rot.z = this->dyna.actor.world.rot.z;

    if (Actor_TalkOfferAccepted(&this->dyna.actor, &play->state)) {
        this->unk_206 = 1;
    }

    DmChar08_UpdateEyes(this);
    this->actionFunc(this, play);
    DmChar08_HandleCutscene(this, play);
    func_80AB023C(this, play);
    func_80AB032C(this, play);
    DmChar08_UpdateAnim(this, play);
    SkelAnime_Update(&this->skelAnime);
    DmChar08_SpawnBubbles(this, play);
    this->dyna.actor.world.pos.y = this->targetYPos;
    if (play->sceneId == SCENE_31MISAKI) {
        if (this->dyna.actor.xzDistToPlayer > 1300.0f) {
            DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        } else {
            DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    }
    if (this->unk_1FF != 0) {
        func_80AAFCCC(this, play);
        Actor_OfferTalk(&this->dyna.actor, play, 400.0f);
    }
    func_80AB096C(this, play);
    DmChar08_UpdateCollision(this, play);
}

s32 DmChar08_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    if ((play->csCtx.state == CS_STATE_IDLE) && (play->sceneId == SCENE_31MISAKI) &&
        (limbIndex == TURTLE_LIMB_FRONT_RIGHT_UPPER_FLIPPER)) {
        rot->z = -0x5E24;
    }
    return false;
}

void DmChar08_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    DmChar08* this = (DmChar08*)thisx;
    Vec3f src;

    if (limbIndex == TURTLE_LIMB_SHELL) {
        src.x = 800.0f;
        src.y = 2600.0f;
        src.z = -800.0f;
        Matrix_MultVec3f(&src, &this->tree1Pos);
        src.x = 2600.0f;
        src.y = 2500.0f;
        src.z = 700.0f;
        Matrix_MultVec3f(&src, &this->tree2Pos);
    } else if (limbIndex == TURTLE_LIMB_LOWER_MOUTH) {
        src.x = 1600.0f;
        src.y = -200.0f;
        src.z = 0.0f;
        Matrix_MultVec3f(&src, &this->bubblePos);
    } else if (limbIndex == TURTLE_LIMB_UPPER_MOUTH) {
        src.x = 600.0f;
        src.y = 700.0f;
        src.z = 0.0f;
        Matrix_MultVec3f(&src, &this->focusPos);
    }
}

void DmChar08_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    DmChar08* this = (DmChar08*)thisx;
    f32 one;

    switch (limbIndex) {
        case TURTLE_LIMB_SHELL:
            break;

        case TURTLE_LIMB_NECK:
            Matrix_Pop();
            one = 1.0f;
            Matrix_Scale(((one - 0.7f) * this->unk_1F0) + 0.7f, ((one - 0.7f) * this->unk_1F0) + 0.7f, 1.0f,
                         MTXMODE_APPLY);
            Matrix_Push();
            break;

        case TURTLE_LIMB_FRONT_LEFT_UPPER_FLIPPER:
        case TURTLE_LIMB_FRONT_LEFT_LOWER_FLIPPER:
        case TURTLE_LIMB_FRONT_RIGHT_UPPER_FLIPPER:
        case TURTLE_LIMB_FRONT_RIGHT_LOWER_FLIPPER:
            Matrix_Pop();
            Matrix_Scale((this->unk_1F0 * 0.4f) + 0.6f, (this->unk_1F0 * 0.4f) + 0.6f, (this->unk_1F0 * 0.4f) + 0.6f,
                         MTXMODE_APPLY);
            Matrix_Push();
            break;

        case TURTLE_LIMB_FRONT_LEFT_END_FLIPPER:
        case TURTLE_LIMB_FRONT_RIGHT_END_FLIPPER:
            Matrix_Scale((this->unk_1F0 * 0.4f) + 0.6f, (this->unk_1F0 * 0.4f) + 0.6f, (this->unk_1F0 * 0.4f) + 0.6f,
                         MTXMODE_APPLY);
            break;

        case TURTLE_TAIL_LIMB:
            Matrix_Pop();
            Matrix_Scale((this->unk_1F0 * 0.52f) + 0.48f, (this->unk_1F0 * 0.52f) + 0.48f,
                         (this->unk_1F0 * 0.52f) + 0.48f, MTXMODE_APPLY);
            Matrix_Push();
            break;

        case TURTLE_LIMB_BACK_LEFT_FLIPPER:
        case TURTLE_LIMB_BACK_RIGHT_FLIPPER:
            Matrix_Scale((this->unk_1F0 * 0.55f) + 0.45f, (this->unk_1F0 * 0.2f) + 0.8f,
                         (this->unk_1F0 * 0.55f) + 0.45f, MTXMODE_APPLY);
            break;

        default:
            break;
    }
}

TexturePtr sBigTurtleEyeTextures[] = {
    gTurtleEyeOpenLeftTex, gTurtleEyeHalfLeftTex,     gTurtleEyeClosedTex,
    gTurtleEyeHalfLeftTex, gTurtleEyeOpenStraightTex, gTurtleEyeHalfStraightTex,
    gTurtleEyeClosedTex,   gTurtleEyeHalfStraightTex, gTurtleEyeOpenRightTex,
};

void DmChar08_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DmChar08* this = (DmChar08*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sBigTurtleEyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sBigTurtleEyeTextures[this->eyeIndex]));
    if ((this->unk_1FF > 0) || (play->csCtx.state != CS_STATE_IDLE)) {
        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, DmChar08_OverrideLimbDraw, DmChar08_PostLimbDraw,
                                       DmChar08_TransformLimbDraw, &this->dyna.actor);
        this->palmTree1->world.pos.x = this->tree1Pos.x;
        this->palmTree1->world.pos.y = this->tree1Pos.y;
        this->palmTree1->world.pos.z = this->tree1Pos.z;
        this->palmTree2->world.pos.x = this->tree2Pos.x;
        this->palmTree2->world.pos.y = this->tree2Pos.y;
        this->palmTree2->world.pos.z = this->tree2Pos.z;
    }
    if (this->unk_1FF == 0) {
        Scene_SetRenderModeXlu(play, 0, 1);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gTurtleAsleepDL);
    } else if (this->unk_1FF == 1) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Scene_SetRenderModeXlu(play, 2, 2);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gTurtleAsleepDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void DmChar08_Reset(void) {
    // Free up our managed memory
    free(sTurtleGreatBayTempleColVerticesData);
    free(sTurtleGreatBayTempleColVertices2Data);
    free(sTurtleGreatBayTempleColPolygons2Data);

    // Assign back the original pointers from the LUS resource
    sTurtleGreatBayTempleColData->polyList = sTurtleGreatBayTempleColPolygonsData;
    sTurtleGreatBayTempleColData->vtxList = sTurtleGreatBayTempleColVerticesOrigData;

    // Wipe out the pointers for next init
    sTurtleGreatBayTempleColData = NULL;
    sTurtleGreatBayTempleColVerticesData = NULL;
    sTurtleGreatBayTempleColVertices2Data = NULL;
    sTurtleGreatBayTempleColVerticesOrigData = NULL;
    sTurtleGreatBayTempleColPolygonsData = NULL;
    sTurtleGreatBayTempleColPolygons2Data = NULL;
}
