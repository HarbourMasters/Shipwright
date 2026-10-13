/*
 * File: z_en_fall.c
 * Overlay: ovl_En_Fall
 * Description: The moon and related effects, along with the Moon's Tear that falls from its eye.
 *
 * This actor handles mutliple types of moon as well as a variety of effects. Specifically, it handles:
 * - Moon: A high-detail moon, used in Termina Field, the Clock Tower, and a few cutscenes.
 * - StoppedClosedMouthMoon: A high-detail moon, used in the Clock Tower after the Giants stop the moon from crashing.
 * - StoppedOpenMouthMoon: A high-detail open-mouthed moon, used in the cutscene prior to warping to the moon.
 * - CrashingMoon: An enlarged high-detail moon, used for when the moon is crashing.
 * - LodMoon: A low-detail moon, used everywhere else.
 * - MoonsTear: The Moon's Tear that can be seen falling from the telescope, along with its associated fire trail.
 * - Fireball: The ball of fire that surrounds the moon when it is crashing.
 * - RisingDebris: The debris that rises from the ground as the moon is crashing.
 * - FireRing: The ring of fire that expands outward when the moon is almost done crashing.
 */

#include "z_en_fall.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_fall/object_fall.h"
#include "objects/object_fall2/object_fall2.h"
#include "objects/object_lodmoon/object_lodmoon.h"
#include "objects/object_moonston/object_moonston.h"

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define FLAG_FIRE_BALL_INTENSIFIES (1 << 0)
#define FLAG_FIRE_RING_APPEARS (1 << 1)

void EnFall_Init(Actor* thisx, PlayState* play);
void EnFall_Destroy(Actor* thisx, PlayState* play);
void EnFall_Update(Actor* thisx, PlayState* play);
void EnFall_Reset(void);

void EnFall_Setup(EnFall* this, PlayState* play);
void EnFall_CrashingMoon_PerformCutsceneActions(EnFall* this, PlayState* play);
void EnFall_StoppedOpenMouthMoon_PerformCutsceneActions(EnFall* this, PlayState* play);
void EnFall_StoppedClosedMouthMoon_PerformCutsceneActions(EnFall* this, PlayState* play);
void EnFall_ClockTowerOrTitleScreenMoon_PerformCutsceneActions(EnFall* this, PlayState* play);
void EnFall_Moon_PerformDefaultActions(EnFall* this, PlayState* play);
void EnFall_MoonsTear_Fall(EnFall* this, PlayState* play);
void EnFall_Fireball_Update(Actor* thisx, PlayState* play);
void EnFall_RisingDebris_Update(Actor* thisx, PlayState* play);
void EnFall_FireRing_Update(Actor* thisx, PlayState* play);
void EnFall_Moon_Draw(Actor* thisx, PlayState* play);
void EnFall_OpenMouthMoon_Draw(Actor* thisx, PlayState* play);
void EnFall_LodMoon_DrawWithoutLerp(Actor* thisx, PlayState* play);
void EnFall_LodMoon_DrawWithLerp(Actor* thisx, PlayState* play);
void EnFall_Fireball_Draw(Actor* thisx, PlayState* play);
void EnFall_RisingDebris_Draw(Actor* thisx, PlayState* play);
void EnFall_FireRing_Draw(Actor* thisx, PlayState* play);
void EnFall_MoonsTear_Draw(Actor* thisx, PlayState* play);

typedef struct {
    /* 0x00 */ u8 modelIndex;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3s rot;
} EnFallDebrisEffect; // size = 0x24

#define EN_FALL_DEBRIS_EFFECT_COUNT 50

EnFallDebrisEffect debrisEffects[EN_FALL_DEBRIS_EFFECT_COUNT];

ActorProfile En_Fall_Profile = {
    /**/ ACTOR_EN_FALL,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnFall),
    /**/ EnFall_Init,
    /**/ EnFall_Destroy,
    /**/ EnFall_Update,
    /**/ NULL,
    /**/ EnFall_Reset,
};

/**
 * Sets the scale of the moon depending on the current day. On the Final Day,
 * it also moves the moon closer to the ground depending on the current time.
 */
void EnFall_Moon_AdjustScaleAndPosition(EnFall* this, PlayState* play) {
    u16 currentTime = CURRENT_TIME;
    u16 dayStartTime = this->dayStartTime;
    f32 finalDayRelativeHeight;

    if (currentTime < dayStartTime) {
        finalDayRelativeHeight = 1.0f - (((f32)dayStartTime - (f32)currentTime) * (1.0f / 0x10000));
    } else {
        finalDayRelativeHeight = ((f32)currentTime - (f32)dayStartTime) * (1.0f / 0x10000);
    }
    switch (CURRENT_DAY) {
        case 0:
            Actor_SetScale(&this->actor, this->scale * 1.2f);
            this->actor.world.pos.y = this->actor.home.pos.y;
            break;

        case 1:
            Actor_SetScale(&this->actor, this->scale * 2.4f);
            this->actor.world.pos.y = this->actor.home.pos.y;
            break;

        case 2:
            Actor_SetScale(&this->actor, this->scale * 3.6f);
            this->actor.world.pos.y = this->actor.home.pos.y;
            break;

        case 3:
            Actor_SetScale(&this->actor, this->scale * 3.6f);
            if (EN_FALL_TYPE(&this->actor) == EN_FALL_TYPE_LODMOON_INVERTED_STONE_TOWER) {
                this->actor.world.pos.y =
                    this->actor.home.pos.y + (finalDayRelativeHeight * 6700.0f * (this->scale * 6.25f));
            } else {
                this->actor.world.pos.y =
                    this->actor.home.pos.y - (finalDayRelativeHeight * 6700.0f * (this->scale * 6.25f));
            }
            break;

        default:
            break;
    }
}

void EnFall_RisingDebris_ResetEffects(EnFall* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(debrisEffects); i++) {
        debrisEffects[i].modelIndex = 3;
    }
    this->activeDebrisEffectCount = 0;
}

void EnFall_Init(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;
    s32 objectSlot;

    this->eyeGlowIntensity = 0.0f;
    this->flags = 0;

    // This system is basically never used in the final game; all instances
    // of EnFall will use the default case.
    switch (EN_FALL_SCALE(&this->actor)) {
        case 1:
            this->scale = 0.08f;
            break;

        case 2:
            this->scale = 0.04f;
            break;

        case 3:
            this->scale = 0.02f;
            break;

        case 4:
            this->scale = 0.01f;
            break;

        default:
            this->scale = 0.16f;
            break;
    }

    switch (EN_FALL_TYPE(&this->actor)) {
        case EN_FALL_TYPE_LODMOON_NO_LERP:
        case EN_FALL_TYPE_LODMOON:
        case EN_FALL_TYPE_LODMOON_INVERTED_STONE_TOWER:
            objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_LODMOON);
            break;

        case EN_FALL_TYPE_MOONS_TEAR:
            objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_MOONSTON);
            break;

        case EN_FALL_TYPE_STOPPED_MOON_OPEN_MOUTH:
            objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_FALL2);
            break;

        default:
            objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_FALL);
            break;
    }

    if (objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }
    this->objectSlot = objectSlot;
    this->actionFunc = EnFall_Setup;
}

void EnFall_Destroy(Actor* thisx, PlayState* play) {
}

/**
 * Finds the Termina Field moon so the Moon's Tear can spawn in the correct place.
 */
Actor* EnFall_MoonsTear_GetTerminaFieldMoon(PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (itemAction != NULL) {
        if (itemAction->id == ACTOR_EN_FALL && EN_FALL_TYPE(itemAction) == EN_FALL_TYPE_TERMINA_FIELD_MOON) {
            return itemAction;
        }
        itemAction = itemAction->next;
    }
    return NULL;
}

void EnFall_Setup(EnFall* this, PlayState* play) {
    Actor* moon;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        this->actionFunc = EnFall_Moon_PerformDefaultActions;
        switch (EN_FALL_TYPE(&this->actor)) {
            case EN_FALL_TYPE_TITLE_SCREEN_MOON:
                this->actor.draw = EnFall_Moon_Draw;
                this->actionFunc = EnFall_ClockTowerOrTitleScreenMoon_PerformCutsceneActions;
                Actor_SetScale(&this->actor, this->scale);
                break;

            case EN_FALL_TYPE_STOPPED_MOON_CLOSED_MOUTH:
                this->actor.draw = EnFall_Moon_Draw;
                this->actionFunc = EnFall_StoppedClosedMouthMoon_PerformCutsceneActions;
                Actor_SetScale(&this->actor, this->scale * 3.0f);
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_25_02)) {
                    Actor_Kill(&this->actor);
                }
                break;

            case EN_FALL_TYPE_CLOCK_TOWER_MOON:
                this->actionFunc = EnFall_ClockTowerOrTitleScreenMoon_PerformCutsceneActions;
                Actor_SetScale(&this->actor, this->scale * 3.0f);
                this->actor.draw = EnFall_Moon_Draw;
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_02)) {
                    Actor_Kill(&this->actor);
                }
                break;

            case EN_FALL_TYPE_CRASHING_MOON:
                this->actor.draw = EnFall_Moon_Draw;
                Actor_SetScale(&this->actor, this->scale * 5.3999996f);
                this->actionFunc = EnFall_CrashingMoon_PerformCutsceneActions;
                break;

            case EN_FALL_TYPE_CRASH_FIRE_BALL:
                this->actor.update = EnFall_Fireball_Update;
                this->actor.draw = EnFall_Fireball_Draw;
                this->scale = 1.0f;
                this->actor.shape.rot.z = 0;
                this->fireballIntensity = 0.0f;
                this->fireballAlpha = 100;
                this->actor.shape.rot.x = this->actor.shape.rot.z;
                break;

            case EN_FALL_TYPE_CRASH_RISING_DEBRIS:
                this->actor.update = EnFall_RisingDebris_Update;
                this->actor.draw = EnFall_RisingDebris_Draw;
                this->scale = 1.0f;
                EnFall_RisingDebris_ResetEffects(this);
                Actor_SetScale(&this->actor, 1.0f);
                this->actor.shape.rot.x = 0;
                break;

            case EN_FALL_TYPE_LODMOON_NO_LERP:
                this->actor.draw = EnFall_LodMoon_DrawWithoutLerp;
                this->dayStartTime = CLOCK_TIME(6, 0);
                this->currentDay = CURRENT_DAY;
                EnFall_Moon_AdjustScaleAndPosition(this, play);
                break;

            case EN_FALL_TYPE_LODMOON:
            case EN_FALL_TYPE_LODMOON_INVERTED_STONE_TOWER:
                this->actor.draw = EnFall_LodMoon_DrawWithLerp;
                this->dayStartTime = CLOCK_TIME(6, 0);
                this->currentDay = CURRENT_DAY;
                EnFall_Moon_AdjustScaleAndPosition(this, play);
                break;

            case EN_FALL_TYPE_MOONS_TEAR:
                this->actor.update = EnFall_Update;
                this->actor.draw = NULL;
                this->actionFunc = EnFall_MoonsTear_Fall;
                Actor_SetScale(&this->actor, 0.02f);
                if (!(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
                    Actor_Kill(&this->actor);
                }
                moon = EnFall_MoonsTear_GetTerminaFieldMoon(play);
                this->actor.child = moon;
                if (moon == NULL) {
                    Actor_Kill(&this->actor);
                }
                break;

            case EN_FALL_TYPE_STOPPED_MOON_OPEN_MOUTH:
                this->actor.draw = NULL;
                this->actionFunc = EnFall_StoppedOpenMouthMoon_PerformCutsceneActions;
                Actor_SetScale(&this->actor, this->scale * 3.0f);
                break;

            case EN_FALL_TYPE_CRASH_FIRE_RING:
                this->actor.update = EnFall_FireRing_Update;
                this->actor.draw = EnFall_FireRing_Draw;
                Actor_SetScale(&this->actor, 0.2f);
                break;

            default:
                // used for EN_FALL_TYPE_TERMINA_FIELD_MOON and anything else that isn't in the enum
                this->actor.draw = EnFall_Moon_Draw;
                this->dayStartTime = CLOCK_TIME(6, 0);
                this->currentDay = CURRENT_DAY;
                EnFall_Moon_AdjustScaleAndPosition(this, play);
                break;
        }
    }
}

// 2S2H [Port] Moved static vars out of function scope so they can be cleared in actor reset
static s32 sGiantsCutsceneState = 0;

void EnFall_CrashingMoon_HandleGiantsCutscene(EnFall* this, PlayState* play) {

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 1) && (play->csCtx.scriptIndex == 0)) {
        switch (sGiantsCutsceneState) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE) {
                    sGiantsCutsceneState += 2;
                }
                break;

            case 2:
                if (GameInteractor_Should(VB_MEET_MOON_REQUIREMENTS, CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) &&
                                                                         CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD))) {
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_93_04)) {
                        if (CutsceneManager_IsNext(12)) {
                            CutsceneManager_Start(12, &this->actor);
                            sGiantsCutsceneState++;
                        } else {
                            CutsceneManager_Queue(12);
                        }
                    } else if (CutsceneManager_IsNext(11)) {
                        CutsceneManager_Start(11, &this->actor);
                        SET_WEEKEVENTREG(WEEKEVENTREG_93_04);
                        sGiantsCutsceneState++;
                    } else {
                        CutsceneManager_Queue(11);
                    }
                } else if (play->csCtx.curFrame > 1600) {
                    play->nextEntrance = ENTRANCE(CLOCK_TOWER_ROOFTOP, 0);
                    gSaveContext.nextCutsceneIndex = 0xFFF2;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_BLACK;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                    sGiantsCutsceneState = 9;
                }
                break;

            case 9:
                play->csCtx.curFrame--;
                break;

            default:
                break;
        }
    }
}

void EnFall_CrashingMoon_PerformCutsceneActions(EnFall* this, PlayState* play) {
    EnFall_CrashingMoon_HandleGiantsCutscene(this, play);
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133)) {
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133) &&
            play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id == 1) {
            this->actor.draw = NULL;
        } else {
            this->actor.draw = EnFall_Moon_Draw;
            if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133) &&
                play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id == 2) {
                Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133));
            }
        }
    } else {
        this->actor.draw = NULL;
    }
}

void EnFall_StoppedOpenMouthMoon_PerformCutsceneActions(EnFall* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133)) {
        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id) {
            case 3:
                if (this->eyeGlowIntensity == 0.0f) {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_MOON_EYE_FLASH);
                }
                this->eyeGlowIntensity += 1 / 30.0f;
                if (this->eyeGlowIntensity > 1.0f) {
                    this->eyeGlowIntensity = 1.0f;
                }
                break;

            case 4:
                this->actor.draw = EnFall_OpenMouthMoon_Draw;
                break;

            default:
                break;
        }
    }
}

void EnFall_StoppedClosedMouthMoon_PerformCutsceneActions(EnFall* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133)) {
        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id) {
            case 2:
                Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133));
                break;

            case 4:
                this->actor.draw = NULL;
                break;

            default:
                break;
        }
    }

    if ((play->sceneId == SCENE_OKUJOU) && (gSaveContext.sceneLayer == 2)) {
        switch (play->csCtx.scriptIndex) {
            case 0:
                switch (play->csCtx.curFrame) {
                    case 1060:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM1);
                        break;

                    case 1089:
                        Actor_PlaySfx(&this->actor, NA_SE_EV_MOON_CRY);
                        break;

                    case 1303:
                        Actor_PlaySfx(&this->actor, NA_SE_EV_SLIP_MOON);
                        break;

                    default:
                        break;
                }
                if (play->csCtx.curFrame >= 1145) {
                    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FALL_POWER - SFX_FLAG);
                }
                break;

            case 1:
                switch (play->csCtx.curFrame) {
                    case 561:
                        Actor_PlaySfx(&this->actor, NA_SE_EN_MOON_SCREAM1);
                        break;

                    case 590:
                        Actor_PlaySfx(&this->actor, NA_SE_EV_MOON_CRY);
                        break;

                    case 737:
                        Actor_PlaySfx(&this->actor, NA_SE_EV_SLIP_MOON);
                        break;

                    default:
                        break;
                }
                if (play->csCtx.curFrame >= 650) {
                    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FALL_POWER - SFX_FLAG);
                }
                break;

            default:
                break;
        }
    }
}

void EnFall_ClockTowerOrTitleScreenMoon_PerformCutsceneActions(EnFall* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->sceneId == SCENE_OKUJOU)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_MOON_FALL - SFX_FLAG);
    }
}

/**
 * Used by the Moon in Termina Field, as well as all LodMoons.
 */
void EnFall_Moon_PerformDefaultActions(EnFall* this, PlayState* play) {
    u16 currentDay;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133)) {
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133) &&
            play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id == 1) {
            this->actor.draw = NULL;
        } else {
            Actor_SetScale(&this->actor, this->scale * 3.6f);
            this->actor.draw = EnFall_Moon_Draw;
            if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_133) &&
                play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133)]->id == 2) {
                Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_133));
            }
        }
    } else {
        if (this->actor.draw == NULL) {
            this->actor.draw = EnFall_Moon_Draw;
        }
        currentDay = CURRENT_DAY;
        if ((u16)this->currentDay != (u32)currentDay) {
            this->currentDay = currentDay;
            this->dayStartTime = CURRENT_TIME;
        }
        EnFall_Moon_AdjustScaleAndPosition(this, play);
    }
}

void EnFall_MoonsTear_Initialize(EnFall* this) {
    s32 pad[2];

    this->actor.draw = EnFall_MoonsTear_Draw;
    if (this->actor.child != NULL) {
        // The focus of the child needs to be shifted in EnFall_Moon_Draw first,
        // otherwise the tear will spawn in the wrong place.
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.child->focus.pos);
    }
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.world.rot.x = Math_Vec3f_Pitch(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.speed = Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos) / 82.0f;
    this->actor.shape.rot.x = this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnFall_MoonsTear_DoNothing(EnFall* this, PlayState* play) {
}

void EnFall_MoonsTear_Fall(EnFall* this, PlayState* play) {
    s32 pad;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_517) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_517)]->id == 2) &&
        (this->actor.draw == NULL)) {
        EnFall_MoonsTear_Initialize(this);
    }

    if (this->actor.draw != NULL) {
        if (Math_Vec3f_StepTo(&this->actor.world.pos, &this->actor.home.pos, this->actor.speed) <= 0.0f) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_GORON_BOUND_1);
            SET_WEEKEVENTREG(WEEKEVENTREG_74_80);
            SET_WEEKEVENTREG(WEEKEVENTREG_74_20);
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_TEST, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, -2);
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_EXPLOSION));
            this->actor.draw = NULL;
            this->actionFunc = EnFall_MoonsTear_DoNothing;
        } else {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_MOONSTONE_FALL - SFX_FLAG);
        }
    }
}

void EnFall_Update(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;

    this->actionFunc(this, play);
}

static u8 sAlphaTableIndices[] = {
    4, 4, 0, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0, 3, 0, 1, 1, 1, 4, 0, 4, 0, 1, 1, 1, 3, 0, 3, 0, 1, 1, 1, 4, 4, 1,
    1, 0, 4, 4, 0, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0, 3, 3, 0, 0, 1, 1, 1, 1, 1, 4, 0, 4, 4, 0, 4, 4, 1, 1, 1, 1,
    1, 1, 3, 3, 0, 0, 3, 3, 0, 0, 1, 1, 1, 1, 1, 1, 4, 4, 0, 4, 0, 1, 1, 1, 3, 0, 3, 3, 0, 0, 1, 1, 1, 1, 1,
    1, 4, 4, 0, 4, 4, 0, 1, 1, 1, 1, 1, 1, 3, 3, 0, 0, 0, 0, 0, 2, 2, 1, 1, 0, 0, 2, 2, 0, 1, 1, 0, 0, 1, 0,
    2, 0, 0, 1, 0, 2, 0, 0, 2, 1, 2, 0, 1, 0, 1, 0, 0, 1, 2, 2, 0, 0, 2, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0,
    0, 0, 2, 0, 2, 2, 0, 1, 1, 0, 0, 1, 0, 0, 1, 2, 2, 0, 0, 0, 2, 2, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0,
};

/**
 * Updates the alpha for every vertex in the fire ball so that some parts of
 * the sphere are more transparent or opaque than others.
 */
void EnFall_Fireball_SetPerVertexAlpha(f32 fireballAlpha) {
    s32 pad;
    u8 perVertexAlphaTable[5];
    Vtx* vertices = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(gMoonFireballVtx));
    s32 i;

    if (fireballAlpha > 1.0f) {
        fireballAlpha = 1.0f;
    }
    perVertexAlphaTable[0] = 0;
    perVertexAlphaTable[1] = (s8)(255.0f * fireballAlpha);
    perVertexAlphaTable[2] = (s8)(155.0f * fireballAlpha);
    perVertexAlphaTable[3] = (s8)(104.0f * fireballAlpha);
    perVertexAlphaTable[4] = (s8)(54.0f * fireballAlpha);

    for (i = 0; i < ARRAY_COUNT(sAlphaTableIndices); i++, vertices++) {
        vertices->v.cn[3] = perVertexAlphaTable[sAlphaTableIndices[i]];
    }
}

void EnFall_Fireball_Update(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 0) && (play->csCtx.scriptIndex == 2)) {
        play->skyboxCtx.rot.y -= 0.05f;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_450)) {
        this->actor.draw = EnFall_Fireball_Draw;
        if (this->flags & FLAG_FIRE_BALL_INTENSIFIES) {
            this->fireballIntensity += 0.01f;
            if (this->fireballIntensity > 1.0f) {
                this->fireballIntensity = 1.0f;
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, 450));

        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, 450)]->id) {
            default:
                this->actor.draw = NULL;
                this->fireballAlpha = 0;
                break;

            case 2:
                if (this->fireballAlpha < 100) {
                    this->fireballAlpha += 4;
                }
                if (this->fireballAlpha > 100) {
                    this->fireballAlpha = 100;
                }
                EnFall_Fireball_SetPerVertexAlpha(this->fireballAlpha * 0.01f);
                break;

            case 3:
                if (this->fireballAlpha > 0) {
                    this->fireballAlpha -= 2;
                }
                if (this->fireballAlpha < 0) {
                    this->fireballAlpha = 0;
                }
                EnFall_Fireball_SetPerVertexAlpha(this->fireballAlpha * 0.01f);
                break;

            case 4:
                this->flags |= FLAG_FIRE_BALL_INTENSIFIES;
                break;

            case 5:
                break;
        }
    } else {
        this->actor.draw = NULL;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_450) && (this->fireballAlpha > 0)) {
        Audio_PlaySfx_2(NA_SE_EV_MOON_FALL_LAST - SFX_FLAG);
    }
    Actor_SetScale(&this->actor, this->scale * 1.74f);
}

void EnFall_RisingDebris_UpdateEffects(EnFall* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(debrisEffects); i++) {
        if (debrisEffects[i].modelIndex < 3) {
            debrisEffects[i].pos.x += debrisEffects[i].velocity.x;
            debrisEffects[i].pos.y += debrisEffects[i].velocity.y;
            debrisEffects[i].pos.z += debrisEffects[i].velocity.z;
            debrisEffects[i].rot.x += 0x64;
            debrisEffects[i].rot.y += 0xC8;
            debrisEffects[i].rot.z += 0x12C;
            if ((this->actor.world.pos.y + 3000.0f) < debrisEffects[i].pos.y) {
                debrisEffects[i].modelIndex = 3;
                this->activeDebrisEffectCount--;
            }
        }
    }
}

s32 EnFall_RisingDebris_InitializeEffect(EnFall* this) {
    s16 angle;
    s32 i;
    f32 scale;

    for (i = 0; i < ARRAY_COUNT(debrisEffects); i++) {
        if (debrisEffects[i].modelIndex >= 3) {
            debrisEffects[i].modelIndex = (s32)Rand_ZeroFloat(3.0f);
            debrisEffects[i].pos.x = this->actor.world.pos.x;
            debrisEffects[i].pos.y = this->actor.world.pos.y;
            debrisEffects[i].pos.z = this->actor.world.pos.z;
            angle = Rand_CenteredFloat(0x10000);
            scale = (1.0f - (Rand_ZeroFloat(1.0f) * Rand_ZeroFloat(1.0f))) * 3000.0f;
            debrisEffects[i].pos.x += Math_SinS(angle) * scale;
            debrisEffects[i].pos.z += Math_CosS(angle) * scale;
            debrisEffects[i].velocity.x = 0.0f;
            debrisEffects[i].velocity.z = 0.0f;
            debrisEffects[i].velocity.y = 80.0f;
            debrisEffects[i].rot.x = Rand_CenteredFloat(0x10000);
            debrisEffects[i].rot.y = Rand_CenteredFloat(0x10000);
            debrisEffects[i].rot.z = Rand_CenteredFloat(0x10000);
            this->activeDebrisEffectCount++;
            return true;
        }
    }

    return false;
}

void EnFall_RisingDebris_Update(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_451)) {
        if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_451) &&
            play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_451)]->id == 2) {
            EnFall_RisingDebris_UpdateEffects(this);
            EnFall_RisingDebris_InitializeEffect(this);
        } else if (this->activeDebrisEffectCount != 0) {
            EnFall_RisingDebris_ResetEffects(this);
        }
    } else if (thisx->home.rot.x != 0) {
        EnFall_RisingDebris_UpdateEffects(this);
        EnFall_RisingDebris_InitializeEffect(this);
    }
}

void EnFall_FireRing_Update(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_450) &&
        play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_450)]->id == 5) {
        if (!(this->flags & FLAG_FIRE_RING_APPEARS)) {
            Actor_PlaySfx(&this->actor, NA_SE_IT_DM_RING_EXPLOSION);
        }
        this->flags |= FLAG_FIRE_RING_APPEARS;
    }
    if (this->flags & FLAG_FIRE_RING_APPEARS) {
        this->fireRingAlpha += 1 / 30.0f;
        if (this->fireRingAlpha > 1.0f) {
            this->fireRingAlpha = 1.0f;
        }
        if (this->actor.scale.x < 18.0f) {
            this->actor.scale.x += 0.2f;
        }
        this->actor.scale.z = this->actor.scale.x;
        this->actor.scale.y = Math_SinS(this->fireWallYScale) * 5.0f;
        if (this->fireWallYScale < 0x4000) {
            this->fireWallYScale += 0x147;
        }
    }
}

/**
 * Used for all closed-mouth high-detail moons, including
 * StoppedClosedMouthMoon and CrashingMoon.
 */
void EnFall_Moon_Draw(Actor* thisx, PlayState* play) {
    // This offsets the moon's focus so that the Moon's Tear actually falls
    // out of its eye when looking at it through the telescope.
    static Vec3f sFocusOffset[] = { 1800.0f, 1000.0f, 4250.0f };
    EnFall* this = (EnFall*)thisx;
    s32 primColor;

    OPEN_DISPS(play->state.gfxCtx);

    CosmeticEditor_ApplySillyMoonScale();

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Matrix_MultVec3f(sFocusOffset, &this->actor.focus.pos);

    primColor = (this->eyeGlowIntensity * 200.0f) + 40.0f;
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, primColor, primColor, primColor, 255);

    gSPDisplayList(POLY_OPA_DISP++, gMoonDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_OpenMouthMoon_Draw(Actor* thisx, PlayState* play) {
    EnFall* this = (EnFall*)thisx;
    s32 primColor;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    primColor = (this->eyeGlowIntensity * 200.0f) + 40.0f;
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, primColor, primColor, primColor, 255);

    gSPDisplayList(POLY_OPA_DISP++, gOpenMouthMoonDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_LodMoon_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFall* this = (EnFall*)thisx;
    s32 primColor;

    OPEN_DISPS(play->state.gfxCtx);

    CosmeticEditor_ApplySillyMoonScale();

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 20, 25, 30, 0, 0x3E7, 0x3200);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    gSPLoadGeometryMode(POLY_OPA_DISP++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);

    primColor = (this->eyeGlowIntensity * 200.0f) + 40.0f;
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, primColor, primColor, primColor, 255);

    gSPDisplayList(POLY_OPA_DISP++, gLodmoonEyesDL);
    gSPLoadGeometryMode(POLY_OPA_DISP++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH);
    gSPDisplayList(POLY_OPA_DISP++, gLodmoonMoonDL);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_LodMoon_DrawWithoutLerp(Actor* thisx, PlayState* play) {
    EnFall_LodMoon_Draw(thisx, play);
}

/**
 * If the moon is more than 9000 units away from the eye, this will lerp it
 * to be 9000 units away before drawing it.
 */
void EnFall_LodMoon_DrawWithLerp(Actor* thisx, PlayState* play) {
    f32 distanceToEye = Actor_WorldDistXYZToPoint(thisx, &play->view.eye);
    f32 scale;
    Vec3f translation;

    if (distanceToEye > 9000.0f) {
        scale = 9000.0f / distanceToEye;
        translation.x = (-(play->view.eye.x - thisx->world.pos.x) * scale) + play->view.eye.x;
        translation.y = (-(play->view.eye.y - thisx->world.pos.y) * scale) + play->view.eye.y;
        translation.z = (-(play->view.eye.z - thisx->world.pos.z) * scale) + play->view.eye.z;
        Matrix_Translate(translation.x, translation.y, translation.z, MTXMODE_NEW);
        Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(thisx->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(thisx->shape.rot.z, MTXMODE_APPLY);
    }

    EnFall_LodMoon_Draw(thisx, play);
}

void EnFall_Fireball_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFall* this = (EnFall*)thisx;
    u32 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    this->fireballYTexScroll1 += (s32)(4.0f + (this->fireballIntensity * 12.0f));
    this->fireballYTexScroll2 += (s32)(2.0f + (this->fireballIntensity * 6.0f));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80,
                    (s32)(((1.0f - this->fireballIntensity) * 160.0f) + (255.0f * this->fireballIntensity)),
                    (s32)((70.0f * (1.0f - this->fireballIntensity)) + (255.0f * this->fireballIntensity)),
                    (s32)(70.0f * (1.0f - this->fireballIntensity)), 255);
    gDPSetEnvColor(
        POLY_XLU_DISP++, (s32)(((1.0f - this->fireballIntensity) * 50.0f) + (200.0f * this->fireballIntensity)),
        (s32)(20.0f * (1.0f - this->fireballIntensity)), (s32)(20.0f * (1.0f - this->fireballIntensity)), 255);

    // For the glowing sphere of fire
    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, gameplayFrames, -this->fireballYTexScroll2, 64, 64, 1,
                                  -gameplayFrames, -this->fireballYTexScroll1, 64, 64, 1,
                                  -(2.0f + (this->fireballIntensity * 6.0f)), -1,
                                  -(4.0f + (this->fireballIntensity * 12.0f))));

    // For the "flecks" of fire around the fireball
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, gameplayFrames * 2, -this->fireballYTexScroll1, 64, 64, 1,
                                  -gameplayFrames * 2, -this->fireballYTexScroll1, 64, 64, 2,
                                  -(4.0f + (this->fireballIntensity * 12.0f)), -2,
                                  -(4.0f + (this->fireballIntensity * 12.0f))));

    gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gSPDisplayList(POLY_XLU_DISP++, gMoonFireballDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_RisingDebris_Draw(Actor* thisx, PlayState* play) {
    static Gfx* sDebrisModelDLists[] = { gMoonDebrisModel1DL, gMoonDebrisModel2DL, gMoonDebrisModel3DL };
    EnFall* this = (EnFall*)thisx;
    f32 scale = this->scale * 0.06f;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gMoonDebrisMaterialDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    for (i = 0; i < ARRAY_COUNT(debrisEffects); i++) {
        if (debrisEffects[i].modelIndex < 3) {
            Matrix_Translate(debrisEffects[i].pos.x, debrisEffects[i].pos.y, debrisEffects[i].pos.z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_RotateZYX(debrisEffects[i].rot.x, debrisEffects[i].rot.y, debrisEffects[i].rot.z, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, sDebrisModelDLists[debrisEffects[i].modelIndex]);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_FireRing_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFall* this = (EnFall*)thisx;

    if (!(this->fireRingAlpha <= 0.0f)) {
        if (this->fireRingAlpha > 1.0f) {
            this->fireRingAlpha = 1.0f;
        }

        OPEN_DISPS(play->state.gfxCtx);

        AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(gMoonFireRingTexAnim));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetColorDither(POLY_XLU_DISP++, G_CD_NOISE);
        gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (s32)(this->fireRingAlpha * 255.0f));

        gSPDisplayList(POLY_XLU_DISP++, gMoonFireRingDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnFall_MoonsTear_Draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gFallingMoonsTearTexAnim));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gFallingMoonsTearDL);

    Matrix_Scale(3.0f, 3.0f, 6.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gFallingMoonsTearFireDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFall_Reset(void) {
    sGiantsCutsceneState = 0;
}
