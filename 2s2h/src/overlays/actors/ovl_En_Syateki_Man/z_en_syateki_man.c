/*
 * File: z_en_syateki_man.c
 * Overlay: ovl_En_Syateki_Man
 * Description: Shooting Gallery Man
 *
 * In addition to handling the normal NPC behavior with the Town/Swamp Shooting Gallery Man, this actor is also
 * responsible for running their respective shooting games as well.
 */

#include "z_en_syateki_man.h"
#include "overlays/actors/ovl_En_Syateki_Crow/z_en_syateki_crow.h"
#include "overlays/actors/ovl_En_Syateki_Dekunuts/z_en_syateki_dekunuts.h"
#include "overlays/actors/ovl_En_Syateki_Okuta/z_en_syateki_okuta.h"
#include "overlays/actors/ovl_En_Syateki_Wf/z_en_syateki_wf.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_LOCK_ON_DISABLED)

void EnSyatekiMan_Init(Actor* thisx, PlayState* play);
void EnSyatekiMan_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiMan_Update(Actor* thisx, PlayState* play);
void EnSyatekiMan_Draw(Actor* thisx, PlayState* play);

void EnSyatekiMan_SetupIdle(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_Idle(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_Talk(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_Idle(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_Talk(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_SetupGiveReward(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_GiveReward(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_SetupGiveReward(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_GiveReward(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_MovePlayerAndExplainRules(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_StartGame(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_RunGame(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_EndGame(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Swamp_AddBonusPoints(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_MovePlayerAndSayHighScore(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_StartGame(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_RunGame(EnSyatekiMan* this, PlayState* play);
void EnSyatekiMan_Town_EndGame(EnSyatekiMan* this, PlayState* play);

#define TALK_FLAG_NONE 0
#define TALK_FLAG_TOWN_HAS_SPOKEN_WITH_HUMAN (1 << 0)
#define TALK_FLAG_TOWN_HAS_SPOKEN_WITH_DEKU (1 << 1)
#define TALK_FLAG_TOWN_HAS_SPOKEN_WITH_GORON (1 << 2)
#define TALK_FLAG_TOWN_HAS_SPOKEN_WITH_ZORA (1 << 3)
#define TALK_FLAG_TOWN_HAS_EXPLAINED_THE_RULES (1 << 4)
#define TALK_FLAG_SWAMP_HAS_SPOKEN_WITH_HUMAN (1 << 0)
#define TALK_FLAG_SWAMP_HAS_EXPLAINED_THE_RULES (1 << 1)

// This defines the configuration of a single wave of Octoroks in the Town Shooting Gallery. These are
// arranged in the same columns and rows that appear in-game; the player stands in the center column, and
// the Octorok with an index of 7 appears directly in front of them like so:
// 0     1     2
// 3     4     5
// 6     7     8
//     Player
#define OCTO_FLAGS(type0, type1, type2, type3, type4, type5, type6, type7, type8)           \
    (SG_OCTO_SET_FLAG(type0, 0) | SG_OCTO_SET_FLAG(type1, 1) | SG_OCTO_SET_FLAG(type2, 2) | \
     SG_OCTO_SET_FLAG(type3, 3) | SG_OCTO_SET_FLAG(type4, 4) | SG_OCTO_SET_FLAG(type5, 5) | \
     SG_OCTO_SET_FLAG(type6, 6) | SG_OCTO_SET_FLAG(type7, 7) | SG_OCTO_SET_FLAG(type8, 8))

// These defines assume that sNormalSwampTargetActorList is used to spawn actors and that the logic of
// EnSyatekiMan_Swamp_RunGame is not modified; in other words, it assumes that each wave consists of five
// Deku Scrubs that the player must shoot and three Guays that the player can either shoot or let escape.
// Once all Deku Scrubs and Guays have been shot or escape, the next wave starts.
#define SG_SWAMP_WAVE_COUNT 4
#define SG_SWAMP_DEKUS_PER_WAVE 5
#define SG_SWAMP_GUAYS_PER_WAVE 3
#define SG_SWAMP_BONUS_DEKU_COUNT 2

// This is the score the player will receive if they hit every single Deku Scrub (both normal and bonus), Guay, and
// Wolfos. There are two conditions for a Wolfos to appear, hence why their point value appears twice; one Wolfos
// will appear after shooting two waves of Deku Scrubs, and one Wolfos will appear after shooting one wave of Guays.
// Assuming the point values, actor list, and shooting game logic are unmodified, this should total to 2120 points.
#define SG_SWAMP_PERFECT_SCORE_WITHOUT_BONUS                                                     \
    (SG_POINTS_DEKU_NORMAL * (SG_SWAMP_DEKUS_PER_WAVE * SG_SWAMP_WAVE_COUNT) +                   \
     SG_POINTS_GUAY * (SG_SWAMP_GUAYS_PER_WAVE * SG_SWAMP_WAVE_COUNT) +                          \
     SG_POINTS_DEKU_BONUS * SG_SWAMP_BONUS_DEKU_COUNT + SG_POINTS_WOLFOS * SG_SWAMP_WAVE_COUNT + \
     SG_POINTS_WOLFOS * (SG_SWAMP_WAVE_COUNT / 2))

// To obtain the Heart Piece from the Swamp Shooting Gallery, the player not only needs to achieve a perfect score,
// but they must also have at least six seconds remaining on the minigame timer. If the player has already obtained
// the Heart Piece, then this score will be used instead to determine if the player should get a Purple Rupee.
#define SG_SWAMP_HEART_PIECE_SCORE (SG_SWAMP_PERFECT_SCORE_WITHOUT_BONUS + (6 * SG_BONUS_POINTS_PER_SECOND))

ActorProfile En_Syateki_Man_Profile = {
    /**/ ACTOR_EN_SYATEKI_MAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SHN,
    /**/ sizeof(EnSyatekiMan),
    /**/ EnSyatekiMan_Init,
    /**/ EnSyatekiMan_Destroy,
    /**/ EnSyatekiMan_Update,
    /**/ EnSyatekiMan_Draw,
};

typedef enum ShootingGalleryManAnimation {
    /* 0 */ SG_MAN_ANIM_HANDS_ON_TABLE,
    /* 1 */ SG_MAN_ANIM_HEAD_SCRATCH_LOOP,
    /* 2 */ SG_MAN_ANIM_HEAD_SCRATCH_END,
    /* 3 */ SG_MAN_ANIM_MAX
} ShootingGalleryManAnimation;

static AnimationInfo sAnimationInfo[SG_MAN_ANIM_MAX] = {
    { &gBurlyGuyHandsOnTableAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },    // SG_MAN_ANIM_HANDS_ON_TABLE
    { &gBurlyGuyHeadScratchLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // SG_MAN_ANIM_HEAD_SCRATCH_LOOP
    { &gBurlyGuyHeadScratchEndAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f },  // SG_MAN_ANIM_HEAD_SCRATCH_END
};

/**
 * In the Swamp Shooting Gallery, there are four waves of Guays.
 * For each wave, these flags are used to control which Guays appear.
 * The number of flags per wave should be exactly SG_SWAMP_GUAYS_PER_WAVE for all waves.
 */
static s16 sGuayFlagsPerWave[SG_SWAMP_WAVE_COUNT] = {
    (1 << 7) | (1 << 6) | (1 << 0),
    (1 << 9) | (1 << 8) | (1 << 1),
    (1 << 4) | (1 << 3) | (1 << 0),
    (1 << 5) | (1 << 2) | (1 << 1),
};

typedef struct {
    /* 0x00 */ s16 actorId;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ s32 params;
} SwampTargetActorEntry; // size = 0x14

static SwampTargetActorEntry sNormalSwampTargetActorList[] = {
    { ACTOR_EN_SYATEKI_WF, -1000.0f, 200.0f, -700.0f, SG_WOLFOS_PARAMS(1, 3, 0) },
    { ACTOR_EN_SYATEKI_WF, -1000.0f, 200.0f, -700.0f, SG_WOLFOS_PARAMS(0, 2, 0) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 0, SG_DEKU_TYPE_NORMAL) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 1, SG_DEKU_TYPE_NORMAL) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 2, SG_DEKU_TYPE_NORMAL) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 3, SG_DEKU_TYPE_NORMAL) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 4, SG_DEKU_TYPE_NORMAL) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 0, SG_DEKU_TYPE_BONUS) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 1, SG_DEKU_TYPE_BONUS) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(0, 0, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(1, 0, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(2, 0, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(3, 0, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(4, 2, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(5, 2, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(6, 0, 1) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(7, 0, 2) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(8, 0, 1) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(9, 0, 2) },
};

/**
 * This actor list is never used in-game and doesn't work properly if modded in.
 * Without any "normal" Deku Scrubs, the game will not progress beyond the first wave.
 */
static SwampTargetActorEntry sUnusedSwampTargetActorList[] = {
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(0, 0, 0) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(1, 0, 0) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 0, SG_DEKU_TYPE_BONUS) },
    { ACTOR_EN_SYATEKI_DEKUNUTS, -1000.0f, 200.0f, -700.0f, SG_DEKU_PARAMS(0, 1, SG_DEKU_TYPE_BONUS) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(2, 0, 2) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(3, 0, 2) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(0, 0, 3) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(1, 0, 3) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(4, 2, 3) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(2, 0, 4) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(3, 0, 4) },
    { ACTOR_EN_SYATEKI_CROW, -1000.0f, 200.0f, -700.0f, SG_GUAY_PARAMS(4, 2, 4) },
    { ACTOR_EN_SYATEKI_WF, -1000.0f, 200.0f, -700.0f, SG_WOLFOS_PARAMS(0, 2, 5) },
    { ACTOR_EN_SYATEKI_WF, -1000.0f, 200.0f, -700.0f, SG_WOLFOS_PARAMS(1, 3, 6) },
};

static SwampTargetActorEntry* sSwampTargetActorLists[] = {
    sNormalSwampTargetActorList,
    sUnusedSwampTargetActorList,
};

static s32 sSwampTargetActorListLengths[] = {
    ARRAY_COUNT(sNormalSwampTargetActorList),
    ARRAY_COUNT(sUnusedSwampTargetActorList),
};

static Vec3f sSwampPlayerPos = { 0.0f, 10.0f, 140.0f };
static Vec3f sTownFierceDeityPlayerPos = { -20.0f, 20.0f, 198.0f };
static Vec3f sTownPlayerPos = { -20.0f, 40.0f, 175.0f };

/**
 * Spawns all the actors used as targets in the Swamp Shooting Gallery.
 */
void EnSyatekiMan_Swamp_SpawnTargetActors(EnSyatekiMan* this, PlayState* play2, SwampTargetActorEntry actorList[],
                                          s32 actorListLength) {
    PlayState* play = play2;
    s32 i;

    for (i = 0; i < actorListLength; i++) {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, actorList[i].actorId, actorList[i].pos.x,
                           actorList[i].pos.y, actorList[i].pos.z, 0, 0, 0, actorList[i].params);
    }
}

void EnSyatekiMan_Init(Actor* thisx, PlayState* play) {
    EnSyatekiMan* this = (EnSyatekiMan*)thisx;
    s32 pad;
    Path* path = &play->setupPathList[SG_MAN_GET_PATH_INDEX(&this->actor)];
    s32 actorListLength = sSwampTargetActorListLengths[this->swampTargetActorListIndex];

    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    Actor_SetScale(&this->actor, 0.01f);
    if (play->sceneId == SCENE_SYATEKI_MORI) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gBurlyGuySkel, &gBurlyGuyHeadScratchLoopAnim, this->jointTable,
                           this->morphTable, BURLY_GUY_LIMB_MAX);
    } else {
        SkelAnime_InitFlex(play, &this->skelAnime, &gBurlyGuySkel, &gBurlyGuyHandsOnTableAnim, this->jointTable,
                           this->morphTable, BURLY_GUY_LIMB_MAX);
    }

    this->actor.colChkInfo.cylRadius = 100;
    this->actionFunc = EnSyatekiMan_SetupIdle;
    this->shootingGameState = SG_GAME_STATE_NONE;
    this->talkWaitTimer = 15;
    this->flagsIndex = 0;
    this->lastHitOctorokType = SG_OCTO_TYPE_NONE;
    this->octorokFlags = 0;
    this->dekuScrubFlags = 0;
    this->guayFlags = 0;
    this->score = 0;
    this->dekuScrubHitCounter = 0;
    this->guayHitCounter = 0;
    this->prevTextId = 0;
    this->swampTargetActorListIndex = 0;
    this->talkFlags = TALK_FLAG_NONE;
    this->eyeIndex = 0;
    this->blinkTimer = 0;

    if (play->sceneId == SCENE_SYATEKI_MORI) {
        this->path = path;
        EnSyatekiMan_Swamp_SpawnTargetActors(this, play, sSwampTargetActorLists[this->swampTargetActorListIndex],
                                             actorListLength);
    }
}

void EnSyatekiMan_Destroy(Actor* thisx, PlayState* play) {
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
}

/**
 * Moves the player to the destination through automated control stick movements.
 * This is used to move the player to the right place to play the shooting game.
 */
s32 EnSyatekiMan_MovePlayerToPos(PlayState* play, Vec3f targetPos) {
    Player* player = GET_PLAYER(play);
    f32 distXZ;
    f32 controlStickMagnitude;
    s16 controlStickAngle;

    controlStickAngle = Math_Vec3f_Yaw(&player->actor.world.pos, &targetPos);
    distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &targetPos);

    if (distXZ < 5.0f) {
        controlStickMagnitude = 10.0f;
    } else if (distXZ < 30.0f) {
        controlStickMagnitude = 40.0f;
    } else {
        controlStickMagnitude = 80.0f;
    }

    play->actorCtx.isOverrideInputOn = true;
    Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, controlStickAngle);

    if (distXZ < 5.0f) {
        return true;
    }

    return false;
}

void EnSyatekiMan_SetupIdle(EnSyatekiMan* this, PlayState* play) {
    if (play->sceneId == SCENE_SYATEKI_MORI) {
        this->actionFunc = EnSyatekiMan_Swamp_Idle;
    } else if (play->sceneId == SCENE_SYATEKI_MIZU) {
        this->actionFunc = EnSyatekiMan_Town_Idle;
    }
}

void EnSyatekiMan_Swamp_Idle(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        u16 faceReactionTextId;

        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_MAN_ANIM_HEAD_SCRATCH_END);
        faceReactionTextId = Text_GetFaceReaction(play, FACE_REACTION_SET_SWAMP_SHOOTING_GALLERY_MAN);
        if (faceReactionTextId != 0) {
            Message_StartTextbox(play, faceReactionTextId, &this->actor);
            this->prevTextId = faceReactionTextId;
        } else if (player->transformation == PLAYER_FORM_HUMAN) {
            if (this->talkFlags == TALK_FLAG_NONE) {
                this->talkFlags = TALK_FLAG_SWAMP_HAS_SPOKEN_WITH_HUMAN;
                // How are you? Wanna play?
                Message_StartTextbox(play, 0xA28, &this->actor);
                this->prevTextId = 0xA28;
            } else {
                // Won't you play?
                Message_StartTextbox(play, 0xA29, &this->actor);
                this->prevTextId = 0xA29;
            }
        } else {
            switch (CURRENT_DAY) {
                case 1:
                    // You can't play if you don't have a bow! (Day 1)
                    Message_StartTextbox(play, 0xA38, &this->actor);
                    this->prevTextId = 0xA38;
                    break;

                case 2:
                    // You can't play if you don't have a bow! (Day 2)
                    Message_StartTextbox(play, 0xA39, &this->actor);
                    this->prevTextId = 0xA39;
                    break;

                case 3:
                    // You can't play if you don't have a bow! (Day 3)
                    Message_StartTextbox(play, 0xA3A, &this->actor);
                    this->prevTextId = 0xA3A;
                    break;
            }
        }
        this->actionFunc = EnSyatekiMan_Swamp_Talk;
    } else {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }

    if (player->actor.world.pos.z < 135.0f) {
        player->actor.world.pos.z = 135.0f;
    }
}

void EnSyatekiMan_Swamp_HandleChoice(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                Audio_PlaySfx(NA_SE_SY_ERROR);

                // You don't have a bow!
                Message_StartTextbox(play, 0xA30, &this->actor);
                this->prevTextId = 0xA30;
            } else if (gSaveContext.save.saveInfo.playerData.rupees < 20) {
                Audio_PlaySfx(NA_SE_SY_ERROR);

                // You don't have enough rupees!
                Message_StartTextbox(play, 0xA31, &this->actor);
                this->prevTextId = 0xA31;
                if (this->shootingGameState == SG_GAME_STATE_ONE_MORE_GAME) {
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                }

                this->shootingGameState = SG_GAME_STATE_NOT_PLAYING;
            } else {
                Audio_PlaySfx_MessageDecide();
                Rupees_ChangeBy(-20);
                SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->shootingGameState = SG_GAME_STATE_MOVING_PLAYER;
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->actionFunc = EnSyatekiMan_Swamp_MovePlayerAndExplainRules;
            }
        } else {
            Audio_PlaySfx_MessageCancel();

            switch (CURRENT_DAY) {
                case 1:
                    // You're not playing? Please come again.
                    Message_StartTextbox(play, 0xA2D, &this->actor);
                    this->prevTextId = 0xA2D;
                    break;

                case 2:
                    // You're not playing? Day after tomorrow is the carnival.
                    Message_StartTextbox(play, 0xA2E, &this->actor);
                    this->prevTextId = 0xA2E;
                    break;

                case 3:
                    // You're not playing? Is something happening outside?
                    Message_StartTextbox(play, 0xA2F, &this->actor);
                    this->prevTextId = 0xA2F;
                    break;
            }

            if (this->shootingGameState == SG_GAME_STATE_ONE_MORE_GAME) {
                gSaveContext.minigameStatus = MINIGAME_STATUS_END;
            }

            this->shootingGameState = SG_GAME_STATE_NOT_PLAYING;
        }
    }
}

void EnSyatekiMan_Swamp_HandleNormalMessage(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        switch (this->prevTextId) {
            case 0xA28: // How are you? Wanna play?
            case 0xA29: // Won't you play?
                // It costs 20 rupees to play.
                Message_StartTextbox(play, 0xA2A, &this->actor);
                this->prevTextId = 0xA2A;
                break;

            case 0xA2B: // The rules of the game are a piece of cake!
            case 0xA2C: // I keep saying - you have to aim with [Control Stick]!
            case 0xA35: // You almost had it! Well...just this once...here you go!
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                player->actor.freezeTimer = 0;
                Interface_InitMinigame(play);
                play->interfaceCtx.minigameAmmo = 80;
                Player_SetBButtonAmmo(play, 80);
                this->shootingGameState = SG_GAME_STATE_RUNNING;
                this->actionFunc = EnSyatekiMan_Swamp_StartGame;
                Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
                break;

            case 0xA32: // You have to try harder!
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                    Message_CloseTextbox(play);
                    player->stateFlags1 &= ~PLAYER_STATE1_20;
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    this->actionFunc = EnSyatekiMan_Swamp_Idle;
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                    this->shootingGameState = SG_GAME_STATE_NONE;
                } else {
                    // Wanna play again?
                    Message_StartTextbox(play, 0xA33, &this->actor);
                    this->prevTextId = 0xA33;
                }
                break;

            case 0xA33: // Wanna play again?
                // It costs 20 rupees to play.
                Message_StartTextbox(play, 0xA2A, &this->actor);
                this->prevTextId = 0xA2A;
                this->shootingGameState = SG_GAME_STATE_ONE_MORE_GAME;
                break;

            case 0xA34: // Perfect! Take this!
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                player->actor.freezeTimer = 0;
                gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->actionFunc = EnSyatekiMan_Swamp_SetupGiveReward;
                EnSyatekiMan_Swamp_SetupGiveReward(this, play);
                break;
        }
    }
}

void EnSyatekiMan_Swamp_Talk(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_20) {
        player->stateFlags1 |= PLAYER_STATE1_20;
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            this->actionFunc = EnSyatekiMan_Swamp_Idle;
            this->shootingGameState = SG_GAME_STATE_NONE;
            break;

        case TEXT_STATE_CHOICE:
            EnSyatekiMan_Swamp_HandleChoice(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnSyatekiMan_Swamp_HandleNormalMessage(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                this->actionFunc = EnSyatekiMan_Swamp_Idle;
                this->shootingGameState = SG_GAME_STATE_NONE;
            }
            break;

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
        case TEXT_STATE_AWAITING_NEXT:
            break;
    }

    if (this->skelAnime.animation == &gBurlyGuyHeadScratchEndAnim) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_MAN_ANIM_HANDS_ON_TABLE);
        }
    }
}

void EnSyatekiMan_Town_StartIntroTextbox(EnSyatekiMan* this, PlayState* play) {
    switch (GET_PLAYER_FORM) {
        case PLAYER_FORM_HUMAN:
            Flags_SetAllTreasure(play, Flags_GetAllTreasure(play) + 1);
            if (CURRENT_DAY != 3) {
                if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_HUMAN)) {
                    this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_HUMAN;
                    // Why don't you give it a try?
                    Message_StartTextbox(play, 0x3E8, &this->actor);
                    this->prevTextId = 0x3E8;
                } else {
                    // Wanna try?
                    Message_StartTextbox(play, 0x3E9, &this->actor);
                    this->prevTextId = 0x3E9;
                }
            } else if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_HUMAN)) {
                this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_HUMAN;
                // If you still have time, why don't you try it?
                Message_StartTextbox(play, 0x3EA, &this->actor);
                this->prevTextId = 0x3EA;
            } else {
                // How about it? Wanna try?
                Message_StartTextbox(play, 0x3EB, &this->actor);
                this->prevTextId = 0x3EB;
            }
            break;

        case PLAYER_FORM_DEKU:
            if (CURRENT_DAY != 3) {
                if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_DEKU)) {
                    this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_DEKU;
                    // When I saw your fairy, I thought you were that masked troublemaker.
                    Message_StartTextbox(play, 0x3EC, &this->actor);
                    this->prevTextId = 0x3EC;
                } else {
                    // You can't play because you don't have a bow.
                    Message_StartTextbox(play, 0x3ED, &this->actor);
                    this->prevTextId = 0x3ED;
                }
            } else if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_DEKU)) {
                this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_DEKU;
                // I thought you were a customer, but I guess I can't expect any...
                Message_StartTextbox(play, 0x3EE, &this->actor);
                this->prevTextId = 0x3EE;
            } else {
                // Stop hanging around and go home!
                Message_StartTextbox(play, 0x3EF, &this->actor);
                this->prevTextId = 0x3EF;
            }
            break;

        case PLAYER_FORM_ZORA:
            if (CURRENT_DAY != 3) {
                if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_ZORA)) {
                    this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_ZORA;
                    // I swear I've seen you before...
                    Message_StartTextbox(play, 0x3F0, &this->actor);
                    this->prevTextId = 0x3F0;
                } else {
                    // If you don't have a bow, you can't play.
                    Message_StartTextbox(play, 0x3F1, &this->actor);
                    this->prevTextId = 0x3F1;
                }
            } else if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_ZORA)) {
                this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_ZORA;
                // Huh? You're still here?
                Message_StartTextbox(play, 0x3F4, &this->actor);
                this->prevTextId = 0x3F4;
            } else {
                // Haven't you heard the news?
                Message_StartTextbox(play, 0x3F5, &this->actor);
                this->prevTextId = 0x3F5;
            }
            break;

        // 2S2H [FD Enhancement] Don't allow playing town archery with FD because Swamp prevents FD archery by default.
        // Get Goron's "quite the build" text
        case PLAYER_FORM_FIERCE_DEITY:
        case PLAYER_FORM_GORON:
            if (CURRENT_DAY != 3) {
                if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_GORON)) {
                    this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_GORON;
                    // You have quite the build!
                    Message_StartTextbox(play, 0x3F2, &this->actor);
                    this->prevTextId = 0x3F2;
                } else {
                    // Sorry...you don't have a bow.
                    Message_StartTextbox(play, 0x3F3, &this->actor);
                    this->prevTextId = 0x3F3;
                }
            } else if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_SPOKEN_WITH_GORON)) {
                this->talkFlags |= TALK_FLAG_TOWN_HAS_SPOKEN_WITH_GORON;
                // Huh? You're still here?
                Message_StartTextbox(play, 0x3F4, &this->actor);
                this->prevTextId = 0x3F4;
            } else {
                // Haven't you heard the news?
                Message_StartTextbox(play, 0x3F5, &this->actor);
                this->prevTextId = 0x3F5;
            }
            break;

        default:
            break;
    }
}

void EnSyatekiMan_Town_Idle(EnSyatekiMan* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        u16 faceReactionTextId = Text_GetFaceReaction(play, FACE_REACTION_SET_TOWN_SHOOTING_GALLERY_MAN);

        if (faceReactionTextId != 0) {
            Message_StartTextbox(play, faceReactionTextId, &this->actor);
            this->prevTextId = faceReactionTextId;
        } else {
            EnSyatekiMan_Town_StartIntroTextbox(this, play);
        }

        this->actionFunc = EnSyatekiMan_Town_Talk;
    } else {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void EnSyatekiMan_Town_HandleChoice(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
            if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                if (CURRENT_DAY != 3) {
                    // You don't have a bow? Then you can't play.
                    Message_StartTextbox(play, 0x3F9, &this->actor);
                    this->prevTextId = 0x3F9;
                } else {
                    // You don't have a bow? That's too bad.
                    Message_StartTextbox(play, 0x3FA, &this->actor);
                    this->prevTextId = 0x3FA;
                }
            } else if (gSaveContext.save.saveInfo.playerData.rupees < 20) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                if (CURRENT_DAY != 3) {
                    // You don't have a enough rupees!
                    Message_StartTextbox(play, 0x3FB, &this->actor);
                    this->prevTextId = 0x3FB;
                } else {
                    // You don't have enough rupees? That's too bad.
                    Message_StartTextbox(play, 0x3FC, &this->actor);
                    this->prevTextId = 0x3FC;
                }

                if (this->shootingGameState == SG_GAME_STATE_ONE_MORE_GAME) {
                    player->stateFlags3 &= ~PLAYER_STATE3_400;
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                }

                this->shootingGameState = SG_GAME_STATE_NOT_PLAYING;
            } else {
                Audio_PlaySfx_MessageDecide();
                Rupees_ChangeBy(-20);
                this->shootingGameState = SG_GAME_STATE_EXPLAINING_RULES;
                if (!(this->talkFlags & TALK_FLAG_TOWN_HAS_EXPLAINED_THE_RULES)) {
                    this->talkFlags |= TALK_FLAG_TOWN_HAS_EXPLAINED_THE_RULES;
                    // The rules are simple.
                    Message_StartTextbox(play, 0x3FD, &this->actor);
                    this->prevTextId = 0x3FD;
                } else {
                    // Aim for the red ones.
                    Message_StartTextbox(play, 0x3FF, &this->actor);
                    this->prevTextId = 0x3FF;
                }

                SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
            }
        } else {
            Audio_PlaySfx_MessageCancel();
            if (CURRENT_DAY != 3) {
                // Well, be that way!
                Message_StartTextbox(play, 0x3F7, &this->actor);
                this->prevTextId = 0x3F7;
            } else {
                // Usually this place is packed...
                Message_StartTextbox(play, 0x3F8, &this->actor);
                this->prevTextId = 0x3F8;
            }

            if (this->shootingGameState == SG_GAME_STATE_ONE_MORE_GAME) {
                player->stateFlags3 &= ~PLAYER_STATE3_400;
                gSaveContext.minigameStatus = MINIGAME_STATUS_END;
            }

            this->shootingGameState = SG_GAME_STATE_NOT_PLAYING;
        }
    }
}

void EnSyatekiMan_Town_HandleNormalMessage(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        switch (this->prevTextId) {
            case 0x3E8: // Why don't you give it a try?
            case 0x3E9: // Wanna try?
            case 0x3EA: // If you still have time, why don't you try it?
            case 0x3EB: // How about it? Wanna try?
                // One game is 20 rupees.
                Message_StartTextbox(play, 0x3F6, &this->actor);
                this->prevTextId = 0x3F6;
                break;

            case 0x3EC: // When I saw your fairy, I thought you were that masked troublemaker.
                // You can't play because you don't have a bow.
                Message_StartTextbox(play, 0x3ED, &this->actor);
                this->prevTextId = 0x3ED;
                break;

            case 0x3EE: // I thought you were a customer, but I guess I can't expect any...
                // Stop hanging around and go home!
                Message_StartTextbox(play, 0x3EF, &this->actor);
                this->prevTextId = 0x3EF;
                break;

            case 0x3F0: // I swear I've seen you before...
                // If you don't have a bow, you can't play.
                Message_StartTextbox(play, 0x3F1, &this->actor);
                this->prevTextId = 0x3F1;
                break;

            case 0x3F2: // You have quite the build!
                // Sorry...you don't have a bow.
                Message_StartTextbox(play, 0x3F3, &this->actor);
                this->prevTextId = 0x3F3;
                break;

            case 0x3F4: // Huh? You're still here?
                // Haven't you heard the news?
                Message_StartTextbox(play, 0x3F5, &this->actor);
                this->prevTextId = 0x3F5;
                break;

            case 0x3FD: // The rules are simple.
            case 0x3FF: // Aim for the red ones.
                if (this->shootingGameState == SG_GAME_STATE_ONE_MORE_GAME) {
                    if (this->prevTextId == 0x3FD) {
                        // Our highest score is [score]. If you break the record, you'll win a prize!
                        Message_StartTextbox(play, 0x3FE, &this->actor);
                        this->prevTextId = 0x3FE;
                    } else {
                        // Our highest score is [score]. Good luck!
                        Message_StartTextbox(play, 0x400, &this->actor);
                        this->prevTextId = 0x400;
                    }
                } else {
                    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                    play->msgCtx.stateTimer = 4;
                    player->actor.freezeTimer = 0;
                    this->shootingGameState = SG_GAME_STATE_MOVING_PLAYER;
                    player->stateFlags1 |= PLAYER_STATE1_20;
                    SET_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    this->actionFunc = EnSyatekiMan_Town_MovePlayerAndSayHighScore;
                }
                break;

            case 0x3FE: // Our highest score is [score]. If you break the record, you'll win a prize!
            case 0x400: // Our highest score is [score]. Good luck!
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                player->actor.freezeTimer = 0;
                this->flagsIndex = 0;
                Interface_InitMinigame(play);
                Player_SetBButtonAmmo(play, 99);
                this->shootingGameState = SG_GAME_STATE_RUNNING;
                Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
                this->actionFunc = EnSyatekiMan_Town_StartGame;
                break;

            case 0x401: // You got [score]? Oh, that's too bad...
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                    Message_CloseTextbox(play);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    this->shootingGameState = SG_GAME_STATE_NONE;
                    this->actionFunc = EnSyatekiMan_Town_Idle;
                } else {
                    // You can't stop, can you? You can play as long as you have rupees.
                    Message_StartTextbox(play, 0x402, &this->actor);
                    this->prevTextId = 0x402;
                }
                break;

            case 0x403: // You got [score]? Too bad...
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                    Message_CloseTextbox(play);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    this->shootingGameState = SG_GAME_STATE_NONE;
                    this->actionFunc = EnSyatekiMan_Town_Idle;
                } else {
                    // Frustrating, right? Wanna try again?
                    Message_StartTextbox(play, 0x404, &this->actor);
                    this->prevTextId = 0x404;
                }
                break;

            case 0x402: // You can't stop, can you? You can play as long as you have rupees.
            case 0x404: // Frustrating, right? Wanna try again?
                // One game is 20 rupees.
                Message_StartTextbox(play, 0x3F6, &this->actor);
                this->prevTextId = 0x3F6;
                this->shootingGameState = SG_GAME_STATE_ONE_MORE_GAME;
                break;

            case 0x405: // No way! That was perfect!
            case 0x406: // That was perfect!
            case 0x407: // You got a new record!
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                player->actor.freezeTimer = 0;
                gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                this->actionFunc = EnSyatekiMan_Town_SetupGiveReward;
                EnSyatekiMan_Town_SetupGiveReward(this, play);
                break;
        }
    }
}

void EnSyatekiMan_Town_Talk(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_20) {
        player->stateFlags1 |= PLAYER_STATE1_20;
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            this->actionFunc = EnSyatekiMan_Town_Idle;
            this->shootingGameState = SG_GAME_STATE_NONE;
            break;

        case TEXT_STATE_CHOICE:
            EnSyatekiMan_Town_HandleChoice(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnSyatekiMan_Town_HandleNormalMessage(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                this->actionFunc = EnSyatekiMan_Town_Idle;
                this->shootingGameState = SG_GAME_STATE_NONE;
            }
            break;

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
        case TEXT_STATE_AWAITING_NEXT:
            break;
    }
}

void EnSyatekiMan_Swamp_SetupGiveReward(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_HasParent(&this->actor, play)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE);
        } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE) &&
                   (this->score >= SG_SWAMP_HEART_PIECE_SCORE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE);
        }

        this->actor.parent = NULL;
        this->actionFunc = EnSyatekiMan_Swamp_GiveReward;
    } else {
        if ((CUR_UPG_VALUE(UPG_QUIVER) < 3) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE)) {
            Actor_OfferGetItem(&this->actor, play, GI_QUIVER_30 + CUR_UPG_VALUE(UPG_QUIVER), 500.0f, 100.0f);
        } else if (this->score < SG_SWAMP_HEART_PIECE_SCORE) {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 500.0f, 100.0f);
        } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE)) {
            Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
        } else {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
        }

        player->actor.shape.rot.y = -0x8000;
        player->actor.velocity.z = 0.0f;
        player->actor.velocity.x = 0.0f;
        player->actor.world.rot.y = player->actor.shape.rot.y;
    }
}

void EnSyatekiMan_Swamp_GiveReward(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if ((CURRENT_DAY == 3) && (CURRENT_TIME > CLOCK_TIME(12, 0))) {
            // We've been having a lot of earthquakes lately.
            Message_StartTextbox(play, 0xA36, &this->actor);
            this->prevTextId = 0xA36;
        } else {
            // Tell your friends about us.
            Message_StartTextbox(play, 0xA37, &this->actor);
            this->prevTextId = 0xA37;
        }

        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->score = 0;
        this->shootingGameState = SG_GAME_STATE_NONE;
        this->actionFunc = EnSyatekiMan_Swamp_Talk;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 500.0f, PLAYER_IA_MINUS1);
    }
}

void EnSyatekiMan_Town_SetupGiveReward(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_HasParent(&this->actor, play)) {
        if (this->prevTextId == 0x407) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE);
            }
        }

        if ((this->prevTextId == 0x405) || (this->prevTextId == 0x406)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE);
            }
        }

        this->actor.parent = NULL;
        this->actionFunc = EnSyatekiMan_Town_GiveReward;
    } else {
        if (this->prevTextId == 0x407) {
            if ((CUR_UPG_VALUE(UPG_QUIVER) < 3) &&
                !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE)) {
                Actor_OfferGetItem(&this->actor, play, GI_QUIVER_30 + CUR_UPG_VALUE(UPG_QUIVER), 500.0f, 100.0f);
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
            }
        } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE)) {
            Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
        } else {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_HUGE, 500.0f, 100.0f);
        }

        player->actor.shape.rot.y = -0x8000;
        player->actor.velocity.z = 0.0f;
        player->actor.velocity.x = 0.0f;
        player->actor.world.rot.y = player->actor.shape.rot.y;
    }
}

void EnSyatekiMan_Town_GiveReward(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CURRENT_DAY != 3) {
        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            this->score = 0;
            this->shootingGameState = SG_GAME_STATE_NONE;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
            this->actionFunc = EnSyatekiMan_SetupIdle;
        }
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        // This may be our last day in business...
        Message_StartTextbox(play, 0x408, &this->actor);
        this->prevTextId = 0x408;
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->score = 0;
        this->shootingGameState = SG_GAME_STATE_NONE;
        this->actionFunc = EnSyatekiMan_Town_Talk;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 500.0f, PLAYER_IA_MINUS1);
    }
}

void EnSyatekiMan_Swamp_MovePlayerAndExplainRules(EnSyatekiMan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (EnSyatekiMan_MovePlayerToPos(play, sSwampPlayerPos)) {
        player->stateFlags1 |= PLAYER_STATE1_20;
        this->shootingGameState = SG_GAME_STATE_EXPLAINING_RULES;
        if (this->talkFlags != TALK_FLAG_SWAMP_HAS_EXPLAINED_THE_RULES) {
            this->talkFlags = TALK_FLAG_SWAMP_HAS_EXPLAINED_THE_RULES;
            // The rules of the game are a piece of cake!
            Message_StartTextbox(play, 0xA2B, &this->actor);
            this->prevTextId = 0xA2B;
        } else {
            // I keep saying - you have to aim with [Control Stick]!
            Message_StartTextbox(play, 0xA2C, &this->actor);
            this->prevTextId = 0xA2C;
        }

        this->actionFunc = EnSyatekiMan_Swamp_Talk;
    }
}

void EnSyatekiMan_Swamp_StartGame(EnSyatekiMan* this, PlayState* play) {
    static s16 sGameStartTimer = 30;
    Player* player = GET_PLAYER(play);

    if (sGameStartTimer > 0) {
        player->actor.world.pos = sSwampPlayerPos;
        player->actor.shape.rot.y = -0x8000;
        player->actor.world.rot.y = player->actor.shape.rot.y;
        play->unk_18790(play, -0x8000);
        sGameStartTimer--;
    } else {
        sGameStartTimer = 30;
        this->flagsIndex = 0;
        this->score = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        Actor_PlaySfx(&this->actor, NA_SE_SY_FOUND);
        this->dekuScrubFlags = (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
        this->guayFlags = 0;
        this->wolfosFlags = 0;
        this->guayAppearTimer = 0;
        this->dekuScrubHitCounter = 0;
        this->guayHitCounter = 0;
        this->currentWave = 0;
        this->bonusDekuScrubHitCounter = 0;
        Interface_StartTimer(TIMER_ID_MINIGAME_1, 100);
        this->actor.draw = NULL;
        this->actionFunc = EnSyatekiMan_Swamp_RunGame;
    }
}

void EnSyatekiMan_Swamp_RunGame(EnSyatekiMan* this, PlayState* play) {
    static s16 sHasSignaledGuaysForThisWave = false;
    Player* player = GET_PLAYER(play);

    if (((this->dekuScrubFlags == 0) || (this->guayAppearTimer > 140)) && !sHasSignaledGuaysForThisWave &&
        (this->currentWave < SG_SWAMP_WAVE_COUNT)) {
        // Make three Guays appear after the player has killed all Deku Scrubs, or after 140 frames.
        sHasSignaledGuaysForThisWave = true;
        this->guayAppearTimer = 0;
        Actor_PlaySfx(&this->actor, NA_SE_SY_FOUND);
        this->guayFlags = sGuayFlagsPerWave[this->flagsIndex];
        if (this->flagsIndex == ARRAY_COUNT(sGuayFlagsPerWave) - 1) {
            this->flagsIndex = 0;
        } else {
            this->flagsIndex++;
        }
    } else if ((this->guayFlags == 0) && (this->dekuScrubFlags == 0) && (sHasSignaledGuaysForThisWave == true) &&
               (this->currentWave < SG_SWAMP_WAVE_COUNT)) {
        // Once all Deku Scrubs and Guays in this wave have either disappeared or died, move on to the next wave.
        if (this->guayHitCounter < SG_SWAMP_GUAYS_PER_WAVE) {
            this->guayHitCounter = 0;
        }

        this->guayAppearTimer = 0;
        sHasSignaledGuaysForThisWave = false;
        this->currentWave++;
        if (this->currentWave < SG_SWAMP_WAVE_COUNT) {
            this->dekuScrubFlags = (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
        }
    }

    // Makes a Wolfos appear after the player has shot all the Guays in the current wave.
    if (this->guayHitCounter == SG_SWAMP_GUAYS_PER_WAVE) {
        this->guayHitCounter = 0;
        this->wolfosFlags |= 1;
    }

    // Makes a Wolfos appear after the player has shot two waves of Deku Scrubs.
    if (this->dekuScrubHitCounter == SG_SWAMP_DEKUS_PER_WAVE * 2) {
        this->dekuScrubHitCounter = 0;
        this->wolfosFlags |= 2;
    }

    this->guayAppearTimer++;

    if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] == SECONDS_TO_TIMER(0)) {
        // End the game because the timer ran out.
        gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] = SECONDS_TO_TIMER(0);
        gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
        this->actor.draw = EnSyatekiMan_Draw;
        this->flagsIndex = 0;
        this->currentWave = 0;
        player->stateFlags1 |= PLAYER_STATE1_20;
        sHasSignaledGuaysForThisWave = false;
        Audio_StopSubBgm();
        this->actionFunc = EnSyatekiMan_Swamp_EndGame;
    } else if ((this->currentWave == SG_SWAMP_WAVE_COUNT) && (this->wolfosFlags == 0) &&
               (this->bonusDekuScrubHitCounter == SG_SWAMP_BONUS_DEKU_COUNT)) {
        // End the game because the player has nothing left to shoot. This doesn't mean the
        // player actually hit everything, since Guays and Wolfos can escape.
        this->actor.draw = EnSyatekiMan_Draw;
        this->flagsIndex = 0;
        this->currentWave = 0;
        player->stateFlags1 |= PLAYER_STATE1_20;
        sHasSignaledGuaysForThisWave = false;
        Audio_StopSubBgm();
        this->shootingGameState = SG_GAME_STATE_GIVING_BONUS;

        if (this->score == SG_SWAMP_PERFECT_SCORE_WITHOUT_BONUS) {
            Interface_SetPerfectLetters(play, PERFECT_LETTERS_TYPE_2);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_6;
            this->actionFunc = EnSyatekiMan_Swamp_AddBonusPoints;
        } else {
            // If the player ran out of things to shoot but did *not* get a perfect score, then
            // they must have missed a Guay or Wolfos at some point; don't award any bonus points.
            gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
            this->actionFunc = EnSyatekiMan_Swamp_EndGame;
        }
    }
}

void EnSyatekiMan_Swamp_EndGame(EnSyatekiMan* this, PlayState* play) {
    if ((this->shootingGameState == SG_GAME_STATE_RUNNING) || (this->shootingGameState == SG_GAME_STATE_GIVING_BONUS)) {
        this->octorokFlags = 0;
        this->dekuScrubFlags = 0;
        this->guayFlags = 0;
        this->wolfosFlags = 0;
        if (this->talkWaitTimer <= 0) {
            if (HS_GET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE() < this->score) {
                HS_SET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE(this->score);
            }

            this->talkWaitTimer = 15;
            if (this->score >= SG_SWAMP_PERFECT_SCORE_WITHOUT_BONUS) {
                // Perfect! Take this!
                Message_StartTextbox(play, 0xA34, &this->actor);
                this->prevTextId = 0xA34;
                this->shootingGameState = SG_GAME_STATE_ENDED;
            } else if (this->score >= 2000) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED)) {
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_WAIT);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_KICKOUT_TIME_PASSED);
                    this->shootingGameState = SG_GAME_STATE_NONE;
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                    this->actionFunc = EnSyatekiMan_Swamp_Idle;
                    return;
                }

                // You almost had it! Well...just this once...here you go!
                // Setting prevTextId to this is what triggers a free replay in EnSyatekiMan_Swamp_HandleNormalMessage.
                Message_StartTextbox(play, 0xA35, &this->actor);
                this->prevTextId = 0xA35;
                this->shootingGameState = SG_GAME_STATE_ONE_MORE_GAME;
                this->score = 0;
            } else {
                // You have to try harder!
                Message_StartTextbox(play, 0xA32, &this->actor);
                this->prevTextId = 0xA32;
                this->shootingGameState = SG_GAME_STATE_ENDED;
            }

            this->actionFunc = EnSyatekiMan_Swamp_Talk;
        } else {
            this->talkWaitTimer--;
        }
    }

    if (this->talkWaitTimer < 5) {
        play->bButtonAmmoPlusOne = -10;
    }
}

void EnSyatekiMan_Swamp_AddBonusPoints(EnSyatekiMan* this, PlayState* play) {
    static s32 sBonusTimer = 0;
    Player* player = GET_PLAYER(play);

    player->stateFlags1 |= PLAYER_STATE1_20;
    if (GameInteractor_Should(VB_ARCHERY_ADD_BONUS_POINTS, !play->interfaceCtx.perfectLettersOn, this, &sBonusTimer)) {
        if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] == SECONDS_TO_TIMER(0)) {
            gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] = SECONDS_TO_TIMER(0);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
            this->flagsIndex = 0;
            this->currentWave = 0;
            this->actionFunc = EnSyatekiMan_Swamp_EndGame;
            sBonusTimer = 0;
        } else if (sBonusTimer > 10) {
            gSaveContext.timerStopTimes[TIMER_ID_MINIGAME_1] += SECONDS_TO_TIMER(1);
            play->interfaceCtx.minigamePoints += SG_BONUS_POINTS_PER_SECOND;
            this->score += SG_BONUS_POINTS_PER_SECOND;
            Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
            sBonusTimer = 0;
        } else {
            sBonusTimer++;
        }
    }
}

void EnSyatekiMan_Town_MovePlayerAndSayHighScore(EnSyatekiMan* this, PlayState* play) {
    Vec3f targetPlayerPos;

    if (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY) {
        targetPlayerPos = sTownFierceDeityPlayerPos;
    } else {
        targetPlayerPos = sTownPlayerPos;
    }

    if (EnSyatekiMan_MovePlayerToPos(play, targetPlayerPos)) {
        if (this->prevTextId == 0x3FD) {
            // Our highest score is [score]. If you break the record, you'll win a prize!
            Message_StartTextbox(play, 0x3FE, &this->actor);
            this->prevTextId = 0x3FE;
        } else {
            // Our highest score is [score]. Good luck!
            Message_StartTextbox(play, 0x400, &this->actor);
            this->prevTextId = 0x400;
        }

        this->shootingGameState = SG_GAME_STATE_EXPLAINING_RULES;
        this->actionFunc = EnSyatekiMan_Town_Talk;
    }
}

void EnSyatekiMan_Town_StartGame(EnSyatekiMan* this, PlayState* play) {
    static s16 sGameStartTimer = 30;
    Player* player = GET_PLAYER(play);

    if (sGameStartTimer == 30) {
        if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
            player->actor.world.pos = sTownFierceDeityPlayerPos;
        } else {
            player->actor.world.pos = sTownPlayerPos;
        }

        player->actor.prevPos = player->actor.world.pos;
        player->actor.shape.rot.y = -0x8000;
        player->actor.world.rot.y = player->actor.shape.rot.y;
        play->unk_18790(play, -0x8000);
        player->stateFlags1 |= PLAYER_STATE1_20;
        sGameStartTimer--;
    } else if (sGameStartTimer > 0) {
        player->actor.shape.rot.y = -0x8000;
        player->actor.world.rot.y = player->actor.shape.rot.y;
        sGameStartTimer--;
    } else if (sGameStartTimer == 0) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->score = 0;
        this->flagsIndex = 0;
        this->octorokState = SG_OCTO_STATE_INITIAL;
        this->lastHitOctorokType = SG_OCTO_TYPE_NONE;
        sGameStartTimer = 30;
        Interface_StartTimer(TIMER_ID_MINIGAME_1, 75);
        this->actor.draw = NULL;
        this->actionFunc = EnSyatekiMan_Town_RunGame;
    }
}

/**
 * In the Town Shooting Gallery, there are fifteen waves of Octoroks.
 * For each wave, these flags are used to control which Octoroks appear.
 */
static const s32 sOctorokFlagsPerWave[] = {
    // clang-format off
    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_NONE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE, SG_OCTO_TYPE_BLUE),

    OCTO_FLAGS(SG_OCTO_TYPE_RED,  SG_OCTO_TYPE_RED, SG_OCTO_TYPE_RED,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE,
               SG_OCTO_TYPE_NONE, SG_OCTO_TYPE_RED, SG_OCTO_TYPE_NONE),
    // clang-format on
};

void EnSyatekiMan_Town_RunGame(EnSyatekiMan* this, PlayState* play) {
    static s32 sModFromLosingTime = 0;
    Player* player = GET_PLAYER(play);
    s32 timer =
        (((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1]) * 0.1f) + 1.0f; // unit is tenths of a second

    if (timer <= 750) {
        s32 waveTimer; // unit is hundredths of a second

        // If you hit a Blue Octorok, you lose 2.5 seconds. If we pretend that the code below was not present,
        // then waveTimer would drop by 250, dramatically reducing how much time you have before the Octoroks
        // begin hiding. This code will ultimately correct waveTimer such that its value is not affected by
        // hitting Blue Octoroks.
        if (sModFromLosingTime == 0) {
            waveTimer = ((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1]) % SECONDS_TO_TIMER(5);
        } else {
            waveTimer = (((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1]) + SECONDS_TO_TIMER_PRECISE(2, 50)) %
                        SECONDS_TO_TIMER(5);
        }

        // Octoroks begin hiding four seconds after a wave begins.
        if (waveTimer < 100) {
            this->octorokState = SG_OCTO_STATE_HIDING;
        }

        if (this->lastHitOctorokType != SG_OCTO_TYPE_NONE) {
            if (this->lastHitOctorokType == SG_OCTO_TYPE_BLUE) {
                gSaveContext.timerTimeLimits[TIMER_ID_MINIGAME_1] -= SECONDS_TO_TIMER_PRECISE(2, 50);
                sModFromLosingTime = (sModFromLosingTime + 25) % 50;
            }

            this->lastHitOctorokType = SG_OCTO_TYPE_NONE;
        }

        if (this->octorokState == SG_OCTO_STATE_APPEARING) {
            this->octorokState++;
        }

        // A new wave of Octoroks should appear every five seconds. However, we need to take into account
        // that the player might have lost time from hitting Blue Octoroks, so we do something similar to
        // what was done with waveTimer above.
        if ((sModFromLosingTime == (timer % 50)) && (this->octorokState >= SG_OCTO_STATE_INITIAL)) {
            if (this->flagsIndex < ARRAY_COUNT(sOctorokFlagsPerWave)) {
                this->octorokFlags = sOctorokFlagsPerWave[this->flagsIndex++];
                Actor_PlaySfx(&this->actor, NA_SE_SY_FOUND);
                this->octorokState = SG_OCTO_STATE_APPEARING;
            }
        }

        if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] == SECONDS_TO_TIMER(0)) {
            this->flagsIndex = 0;
            this->octorokState = SG_OCTO_STATE_HIDING;
            gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_1] = SECONDS_TO_TIMER(0);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_1] = TIMER_STATE_STOP;
            player->stateFlags1 |= PLAYER_STATE1_20;
            sModFromLosingTime = 0;
            this->actor.draw = EnSyatekiMan_Draw;
            Audio_StopSubBgm();
            this->actionFunc = EnSyatekiMan_Town_EndGame;
            if (this->score == 50) {
                // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
                // anything bad.
                Audio_PlayFanfare(NA_BGM_GET_ITEM);
                Interface_SetPerfectLetters(play, PERFECT_LETTERS_TYPE_1);
            }
        }
    }
}

void EnSyatekiMan_Town_EndGame(EnSyatekiMan* this, PlayState* play) {
    if (this->shootingGameState == SG_GAME_STATE_RUNNING) {
        this->octorokFlags = 0;
        if ((this->talkWaitTimer <= 0) && !play->interfaceCtx.perfectLettersOn) {
            Flags_SetAllTreasure(play, this->score);
            this->talkWaitTimer = 15;
            if ((HS_GET_TOWN_SHOOTING_GALLERY_HIGH_SCORE() < this->score) || (this->score == 50)) {
                if (HS_GET_TOWN_SHOOTING_GALLERY_HIGH_SCORE() < this->score) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE)) {
                        // You got a new record!
                        Message_StartTextbox(play, 0x407, &this->actor);
                        this->prevTextId = 0x407;
                    } else if (this->score == 50) {
                        // No way! That was perfect!
                        Message_StartTextbox(play, 0x405, &this->actor);
                        this->prevTextId = 0x405;
                    } else {
                        // You got a new record!
                        Message_StartTextbox(play, 0x407, &this->actor);
                        this->prevTextId = 0x407;
                    }
                } else if (this->score == 50) {
                    // That was perfect!
                    Message_StartTextbox(play, 0x406, &this->actor);
                    this->prevTextId = 0x406;
                }

                HS_SET_TOWN_SHOOTING_GALLERY_HIGH_SCORE(this->score);
                this->shootingGameState = SG_GAME_STATE_ENDED;
            } else {
                if (CURRENT_DAY != 3) {
                    // You got [score]? Oh, that's too bad...
                    Message_StartTextbox(play, 0x401, &this->actor);
                    this->prevTextId = 0x401;
                } else {
                    // You got [score]? Too bad...
                    Message_StartTextbox(play, 0x403, &this->actor);
                    this->prevTextId = 0x403;
                }

                this->shootingGameState = SG_GAME_STATE_ONE_MORE_GAME;
            }

            this->actionFunc = EnSyatekiMan_Town_Talk;
        } else {
            this->talkWaitTimer--;
        }
    }

    if (this->talkWaitTimer < 5) {
        play->bButtonAmmoPlusOne = -10;
    }
}

void EnSyatekiMan_Blink(EnSyatekiMan* this) {
    switch (this->blinkTimer) {
        case 1:
            this->eyeIndex = 2;
            break;

        case 2:
            this->eyeIndex = 1;
            break;

        case 40:
            this->blinkTimer = 0;

        default:
            this->eyeIndex = 0;
            break;
    }

    this->blinkTimer++;
}

void EnSyatekiMan_Update(Actor* thisx, PlayState* play) {
    EnSyatekiMan* this = (EnSyatekiMan*)thisx;

    this->actionFunc(this, play);
    EnSyatekiMan_Blink(this);
    this->actor.focus.pos.y = 70.0f;
    Actor_SetFocus(&this->actor, 70.0f);
    if (this->shootingGameState != SG_GAME_STATE_RUNNING) {
        SkelAnime_Update(&this->skelAnime);
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    }
}

s32 EnSyatekiMan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSyatekiMan* this = (EnSyatekiMan*)thisx;

    if ((play->sceneId == SCENE_SYATEKI_MIZU) && (limbIndex == BURLY_GUY_LIMB_HEAD)) {
        *dList = gTownShootingGalleryManHeadDL;
    }

    if (limbIndex == BURLY_GUY_LIMB_HEAD) {
        Matrix_Translate(3000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZS(this->headRot.x, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_Translate(-3000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    } else if (limbIndex == BURLY_GUY_LIMB_TORSO) {
        Matrix_RotateXS(-this->torsoRot.y, MTXMODE_APPLY);
    }

    return false;
}

void EnSyatekiMan_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnSyatekiMan* this = (EnSyatekiMan*)thisx;
    Vec3f sFocusOffset = { 1600.0f, 0.0f, 0.0f };

    if (limbIndex == BURLY_GUY_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnSyatekiMan_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gSwampShootingGalleryManEyeOpenTex,
        gSwampShootingGalleryManEyeHalfTex,
        gSwampShootingGalleryManEyeHalfTex,
    };
    EnSyatekiMan* this = (EnSyatekiMan*)thisx;
    s32 pad;

    if (play->sceneId == SCENE_SYATEKI_MIZU) {
        sEyeTextures[0] = gTownShootingGalleryManEyeOpenTex;
        sEyeTextures[1] = gTownShootingGalleryManEyeClosedTex;
        sEyeTextures[2] = gTownShootingGalleryManEyeClosedTex;
    } else {
        sEyeTextures[0] = gSwampShootingGalleryManEyeOpenTex;
        sEyeTextures[1] = gSwampShootingGalleryManEyeHalfTex;
        sEyeTextures[2] = gSwampShootingGalleryManEyeHalfTex;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSyatekiMan_OverrideLimbDraw, EnSyatekiMan_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
