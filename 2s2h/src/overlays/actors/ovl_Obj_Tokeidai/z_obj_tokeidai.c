/*
 * File: z_obj_tokeidai.c
 * Overlay: ovl_Obj_Tokeidai
 * Description: Components of the Clock Tower. Also used for wall clocks. The specific components are:
 * - ClockFace: The decorated face of the clock that spins every hour.
 * - MinuteRing: The ring around the clock face that spins every two minutes.
 * - ExteriorGear: The two gears on the side of the tower that spin every two minutes.
 * - SunAndMoonPanel: A panel with a sun and moon image that tells whether it is day or night.
 * - Counterweight: The spinning weight on top of the tower that emits a spotlight at night.
 * - StaircaseToRooftop: The staircase to tower's rooftop that opens after midnight on the Final Day.
 * - TerminaFieldWalls: The walls of the Clock Tower visible from Termina Field.
 * - UnusedWall: A single wall of the Clock Tower, unused in the final game.
 *
 * Sometimes, the ClockFace, MinuteRing, and SunAndMoonPanel are treated as a single clock entity.
 * This is called Clock if it applies to both the Clock Tower clock and wall clocks, and
 * TowerClock/WallClock, respectively, if it only applies to one of them.
 *
 * On the midnight of the Final Day, the Clock Tower opens up, and the various pieces of it
 * are moved around. This actor is responsible for managing all of this movement, which is handled
 * via the series of action functions labeled with TowerOpening.
 *
 * All components have at least one of these three actions:
 * - Idle: The "default" action, and for some components the only action.
 *       The component is either completely still, or it's telling the time, depending on the component.
 * - OpenedIdle: The action for certain Clock Tower components after midnight on the Final Day.
 *       The TowerClock, ExteriorGears, and Counterweight no longer tell time and are moved to a different position.
 *       Called "opened" because it corresponds to when the staircase to the tower's rooftop is opened up.
 * - Collapse: The action for certain Clock Tower components when the moon crashes into Termina.
 *       This action is responsible for making the component fall during the moon crash cutscene.
 */

#include "z_obj_tokeidai.h"
#include "objects/object_obj_tokeidai/object_obj_tokeidai.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define GET_CURRENT_CLOCK_HOUR(this) ((s32)TIME_TO_HOURS_F((this)->clockTime))
#define GET_CURRENT_CLOCK_MINUTE(this) ((s32)((this)->clockTime * (360 * 2.0f / 0x10000)) % 30)
#define GET_CLOCK_FACE_ROTATION(currentClockHour) (TRUNCF_BINANG(currentClockHour * (0x10000 / 24.0f)))
#define GET_MINUTE_RING_OR_EXTERIOR_GEAR_ROTATION(currentClockMinute) \
    (TRUNCF_BINANG(currentClockMinute * (0x10000 * 12.0f / 360)))

void ObjTokeidai_Init(Actor* thisx, PlayState* play);
void ObjTokeidai_Destroy(Actor* thisx, PlayState* play);
void ObjTokeidai_Update(Actor* thisx, PlayState* play);
void ObjTokeidai_Draw(Actor* thisx, PlayState* play);

void ObjTokeidai_DoNothing(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_TowerClock_OpenedIdle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_ExteriorGear_OpenedIdle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_Counterweight_OpenedIdle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_TowerClock_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_WallClock_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_ExteriorGear_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_Counterweight_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_TerminaFieldWalls_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_StaircaseToRooftop_Idle(ObjTokeidai* this, PlayState* play);
void ObjTokeidai_SetupTowerOpening(ObjTokeidai* this);
void ObjTokeidai_Clock_Draw(Actor* thisx, PlayState* play);
void ObjTokeidai_Counterweight_Draw(Actor* thisx, PlayState* play);
void ObjTokeidai_ExteriorGear_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Tokeidai_Profile = {
    /**/ ACTOR_OBJ_TOKEIDAI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_OBJ_TOKEIDAI,
    /**/ sizeof(ObjTokeidai),
    /**/ ObjTokeidai_Init,
    /**/ ObjTokeidai_Destroy,
    /**/ ObjTokeidai_Update,
    /**/ ObjTokeidai_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 3300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1100, ICHAIN_STOP),
};

/**
 * Returns the angle necessary to show the correct side of
 * the sun and moon panel based on the time of day. The actual
 * angle can differ from the target angle if the panel is in
 * the middle of rotating.
 */
s32 ObjTokeidai_GetTargetSunMoonPanelRotation(void) {
    if (gSaveContext.save.isNight) {
        return 0x8000;
    }
    return 0;
}

void ObjTokeidai_SetupClockOrExteriorGear(ObjTokeidai* this) {
    s32 currentClockMinute = GET_CURRENT_CLOCK_MINUTE(this);

    this->clockMinute = currentClockMinute;
    this->minuteRingOrExteriorGearRotation = GET_MINUTE_RING_OR_EXTERIOR_GEAR_ROTATION(currentClockMinute);
    this->minuteRingOrExteriorGearAngularVelocity = 0x3C;
    this->minuteRingOrExteriorGearRotationTimer = 0;
}

void ObjTokeidai_Clock_Init(ObjTokeidai* this) {
    s32 currentClockHour;

    ObjTokeidai_SetupClockOrExteriorGear(this);
    currentClockHour = GET_CURRENT_CLOCK_HOUR(this);
    this->clockHour = currentClockHour;
    this->clockFaceRotation = GET_CLOCK_FACE_ROTATION(currentClockHour);
    this->clockFaceAngularVelocity = 0;
    this->clockFaceRotationTimer = 0;
    this->sunMoonPanelAngularVelocity = 0;
    this->sunMoonPanelRotation = ObjTokeidai_GetTargetSunMoonPanelRotation();
}

void ObjTokeidai_ExteriorGear_Init(ObjTokeidai* this, PlayState* play) {
    this->actor.draw = ObjTokeidai_ExteriorGear_Draw;
    this->opaDList = gClockTowerExteriorGearDL;
    ObjTokeidai_SetupClockOrExteriorGear(this);

    if (((play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) ||
        ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0))) {
        ObjTokeidai_SetupTowerOpening(this);
    } else if (((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) || (CURRENT_DAY >= 4)) {
        this->actionFunc = ObjTokeidai_ExteriorGear_OpenedIdle;
        this->actor.world.pos.y += this->actor.scale.y * 1900.0f;
        this->actor.shape.yOffset = 1500.0f;
        SET_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED);
    } else {
        this->actionFunc = ObjTokeidai_ExteriorGear_Idle;
    }
}

void ObjTokeidai_TowerClock_Init(ObjTokeidai* this, PlayState* play) {
    this->actor.draw = ObjTokeidai_Clock_Draw;
    ObjTokeidai_Clock_Init(this);

    if (((play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) ||
        ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0))) {
        ObjTokeidai_SetupTowerOpening(this);
    } else if (((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) || (CURRENT_DAY >= 4)) {
        this->actor.world.pos.y += (this->actor.scale.y * 5191.0f) - 50.0f;
        this->actor.world.pos.x += Math_SinS(this->actor.world.rot.y) * this->actor.scale.z * 1791.0f;
        this->actor.world.pos.z += -Math_CosS(this->actor.world.rot.y) * this->actor.scale.z * 1791.0f;

        this->clockFaceZTranslation = -0x140;
        this->actor.shape.rot.x = -0x4000;

        this->actor.home.pos = this->actor.world.pos;
        this->actor.home.pos.y -= 1178.0f;
        this->actor.home.pos.z += 13.0f;

        this->actionFunc = ObjTokeidai_TowerClock_OpenedIdle;
    } else {
        this->actionFunc = ObjTokeidai_TowerClock_Idle;
    }
}

void ObjTokeidai_Counterweight_Init(ObjTokeidai* this, PlayState* play) {
    s32 type;

    this->actor.draw = ObjTokeidai_Counterweight_Draw;
    this->opaDList = gClockTowerCounterweightDL;
    this->xluDList = gClockTowerSpotlightDL;
    if (gSaveContext.save.isNight) {
        this->spotlightIntensity = 100;
    } else {
        this->spotlightIntensity = 0;
    }

    if (((play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) ||
        ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0))) {
        this->spotlightIntensity = 0;
        ObjTokeidai_SetupTowerOpening(this);

        if (this->actor.child == NULL) {
            type = OBJ_TOKEIDAI_TYPE(&this->actor);
            if ((type == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_CLOCK_TOWN) ||
                (type == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_TERMINA_FIELD)) {
                this->actor.child = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HANABI, this->actor.world.pos.x,
                                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
            }
        }

        if (this->actor.child != NULL) {
            if (OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_TERMINA_FIELD) {
                this->actor.child->home.rot.x = 0x384;
            } else {
                this->actor.child->home.rot.x = 0x12C;
            }
        }
    } else if (((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) || (CURRENT_DAY >= 4)) {
        this->spotlightIntensity = 0;
        this->actor.world.pos.y += this->actor.scale.y * -2160.0f;
        this->actor.world.pos.x += Math_SinS(this->actor.world.rot.y) * this->actor.scale.z * 5400.0f;
        this->actor.world.pos.z += -Math_CosS(this->actor.world.rot.y) * this->actor.scale.z * 5400.0f;
        this->actor.shape.rot.x = -0x4000;
        this->actionFunc = ObjTokeidai_Counterweight_OpenedIdle;
    } else {
        this->actionFunc = ObjTokeidai_Counterweight_Idle;
    }
}

void ObjTokeidai_Init(Actor* thisx, PlayState* play) {
    ObjTokeidai* this = (ObjTokeidai*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actionFunc = ObjTokeidai_DoNothing;
    this->opaDList = NULL;
    this->xluDList = NULL;
    this->xRotation = 0;
    this->yTranslation = 0;
    this->clockFaceZTranslation = 0;
    this->clockTime = CURRENT_TIME;
    this->actor.home.rot.x = 0;

    switch (OBJ_TOKEIDAI_TYPE(&this->actor)) {
        case OBJ_TOKEIDAI_TYPE_EXTERIOR_GEAR_TERMINA_FIELD:
            Actor_SetScale(&this->actor, 0.15f);
            ObjTokeidai_ExteriorGear_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_EXTERIOR_GEAR_CLOCK_TOWN:
            ObjTokeidai_ExteriorGear_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_UNUSED_WALL:
            this->opaDList = gClockTowerUnusedTowerWallDL;
            break;

        case OBJ_TOKEIDAI_TYPE_TOWER_WALLS_TERMINA_FIELD:
            Actor_SetScale(&this->actor, 1.0f);
            this->opaDList = gClockTowerTerminaFieldWallsDL;
            this->actionFunc = ObjTokeidai_TerminaFieldWalls_Idle;
            break;

        case OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD:
            Actor_SetScale(&this->actor, 0.15f);
            ObjTokeidai_TowerClock_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_CLOCK_TOWN:
            ObjTokeidai_TowerClock_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_WALL_CLOCK:
            Actor_SetScale(&this->actor, 0.02f);
            this->actor.draw = ObjTokeidai_Clock_Draw;
            ObjTokeidai_Clock_Init(this);
            this->actionFunc = ObjTokeidai_WallClock_Idle;
            break;

        case OBJ_TOKEIDAI_TYPE_SMALL_WALL_CLOCK:
            Actor_SetScale(&this->actor, 0.01f);
            this->actor.draw = ObjTokeidai_Clock_Draw;
            ObjTokeidai_Clock_Init(this);
            this->actionFunc = ObjTokeidai_WallClock_Idle;
            break;

        case OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_TERMINA_FIELD:
            Actor_SetScale(&this->actor, 0.15f);
            ObjTokeidai_Counterweight_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_CLOCK_TOWN:
            ObjTokeidai_Counterweight_Init(this, play);
            break;

        case OBJ_TOKEIDAI_TYPE_STAIRCASE_TO_ROOFTOP:
            this->opaDList = gClockTowerStaircaseToRooftopDL;
            this->xluDList = gClockTowerEmptyDL;
            this->actionFunc = ObjTokeidai_StaircaseToRooftop_Idle;
            break;

        default:
            break;
    }
}

void ObjTokeidai_Destroy(Actor* thisx, PlayState* play) {
}

void ObjTokeidai_RotateOnMinuteChange(ObjTokeidai* this, s32 playSfx) {
    s32 currentClockMinute = GET_CURRENT_CLOCK_MINUTE(this);

    if (currentClockMinute != this->clockMinute) {
        if ((this->minuteRingOrExteriorGearRotationTimer == 8) && playSfx) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_SECOND_HAND);
        }

        if (this->minuteRingOrExteriorGearRotationTimer > 8) {
            // This actually performs the rotation to the next minute
            // for the minute ring or exterior gear.
            this->minuteRingOrExteriorGearAngularVelocity += 0x3C;
            this->minuteRingOrExteriorGearRotation += this->minuteRingOrExteriorGearAngularVelocity;
        } else {
            // This makes the minute ring or exterior gear wiggle in place for a bit
            // before rotating to the next position.
            if ((this->minuteRingOrExteriorGearRotationTimer & 3) == 0) {
                this->minuteRingOrExteriorGearRotation += 0x5A;
            }
            if ((this->minuteRingOrExteriorGearRotationTimer & 3) == 1) {
                this->minuteRingOrExteriorGearRotation -= 0x5A;
            }
        }

        this->minuteRingOrExteriorGearRotationTimer++;

        if (((currentClockMinute == 15) && (this->minuteRingOrExteriorGearRotation < 0)) ||
            ((currentClockMinute != 15) &&
             this->minuteRingOrExteriorGearRotation > GET_MINUTE_RING_OR_EXTERIOR_GEAR_ROTATION(currentClockMinute))) {
            this->minuteRingOrExteriorGearRotation = GET_MINUTE_RING_OR_EXTERIOR_GEAR_ROTATION(currentClockMinute);
            this->clockMinute = currentClockMinute;
            this->minuteRingOrExteriorGearAngularVelocity = 0x5A;
            this->minuteRingOrExteriorGearRotationTimer = 0;
        }
    }
}

void ObjTokeidai_ExteriorGear_Collapse(ObjTokeidai* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.world.pos.y < 0.0f)) {
        this->actionFunc = ObjTokeidai_DoNothing;
    } else {
        this->actor.shape.rot.x += 0x50;
        this->actor.shape.rot.z += 0x50;
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    }
}

void ObjTokeidai_ExteriorGear_OpenedIdle(ObjTokeidai* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
        play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 2) {
        this->actionFunc = ObjTokeidai_ExteriorGear_Collapse;
        this->actor.speed = this->actor.scale.y * 5.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.terminalVelocity = this->actor.scale.y * -50.0f;
        this->actor.gravity = this->actor.scale.y * -5.0f;
    }
}

/**
 * This, used in conjunction with the following function, makes the
 * tower's clock slide off the tower and spin through the air when the moon crashes.
 */
void ObjTokeidai_TowerClock_Fall(ObjTokeidai* this, PlayState* play) {
    this->actor.shape.rot.x += this->fallingClockFaceAngularVelocity;
    if (this->fallingClockFaceAngularVelocity > 0xA0) {
        this->fallingClockFaceAngularVelocity -= 5;
    }

    this->actor.world.pos.z += 4.0f;
    Actor_MoveWithGravity(&this->actor);
    if (this->actor.world.pos.y < 0.0f) {
        this->actionFunc = ObjTokeidai_DoNothing;
    }
}

/**
 * This makes the tower's clock slide off as the moon crashes into it. It's
 * the tower clock's equivalent to other components' Collapse functions.
 */
void ObjTokeidai_TowerClock_SlideOff(ObjTokeidai* this, PlayState* play) {
    f32 cos;
    f32 sin;
    Actor* thisx = &this->actor;

    if (this->slidingClockFaceAngle < 0x4000) {
        this->slidingClockFaceAngle += 0x28;
    }
    if (this->slidingClockFaceAngle > 0x800) {
        this->aerialClockFaceSpeed += 4;
    }

    if (this->aerialClockFaceSpeed < 0x80) {
        thisx->shape.rot.x = this->slidingClockFaceAngle - 0x4000;
        this->fallingClockFaceAngularVelocity = 0x28;
    } else {
        if (thisx->shape.rot.x < -0x1000) {
            thisx->shape.rot.x += this->fallingClockFaceAngularVelocity;
            if (this->fallingClockFaceAngularVelocity < 0x1E0) {
                this->fallingClockFaceAngularVelocity += 0xA;
            }
        } else {
            thisx->shape.rot.x += this->fallingClockFaceAngularVelocity;
            this->actionFunc = ObjTokeidai_TowerClock_Fall;
            thisx->terminalVelocity = -7.5f;
            thisx->gravity = -0.75f;
            thisx->velocity.y = -2.0f;
        }
    }

    sin = Math_SinS(this->slidingClockFaceAngle);
    cos = Math_CosS(this->slidingClockFaceAngle);
    thisx->world.pos.y = (1178.0f * cos) - (this->aerialClockFaceSpeed * sin) + thisx->home.pos.y;
    thisx->world.pos.z = (1178.0f * sin) + (this->aerialClockFaceSpeed * cos) + thisx->home.pos.z;
}

void ObjTokeidai_TowerClock_OpenedIdle(ObjTokeidai* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 1)) {
        this->actionFunc = ObjTokeidai_TowerClock_SlideOff;
        this->slidingClockFaceAngle = 0;
        this->aerialClockFaceSpeed = -0xD;
    }
}

void ObjTokeidai_Counterweight_Collapse(ObjTokeidai* this, PlayState* play) {
    if (this->actor.world.pos.y < 0.0f) {
        this->actionFunc = ObjTokeidai_DoNothing;
    } else {
        this->xRotation += 0x64;
        Actor_MoveWithGravity(&this->actor);
    }
}

void ObjTokeidai_Counterweight_OpenedIdle(ObjTokeidai* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 3)) {
        this->actionFunc = ObjTokeidai_Counterweight_Collapse;
        this->xRotation = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.terminalVelocity = this->actor.scale.y * -50.0f;
        this->actor.gravity = this->actor.scale.y * -5.0f;
    }
}

void ObjTokeidai_TerminaFieldWalls_Collapse(ObjTokeidai* this, PlayState* play) {
    if (this->actor.shape.rot.x < 0x4000) {
        this->actor.shape.rot.x += 0x28;
    }
}

void ObjTokeidai_TerminaFieldWalls_Idle(ObjTokeidai* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 1)) {
        this->actionFunc = ObjTokeidai_TerminaFieldWalls_Collapse;
    }
}

void ObjTokeidai_TowerOpening_EndCutscene(ObjTokeidai* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 5)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED);
        if (((play->sceneId == SCENE_CLOCKTOWER) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) ||
            ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0))) {
            Audio_SetCutsceneFlag(false);
            gSaveContext.save.cutsceneIndex = 0;
            gSaveContext.nextCutsceneIndex = 0;
            gSaveContext.respawnFlag = 2;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_RETURN].entrance;
            play->transitionType = TRANS_TYPE_FADE_BLACK;
            if (gSaveContext.respawn[RESPAWN_MODE_RETURN].playerParams ==
                PLAYER_PARAMS(0xFF, PLAYER_START_MODE_TELESCOPE)) {
                gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE;
            } else {
                gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            }
        }
        this->actionFunc = ObjTokeidai_DoNothing;
    }
}

void ObjTokeidai_TowerOpening_FinishOpening(ObjTokeidai* this, PlayState* play) {
    s32 type;

    if (this->clockFaceZTranslation > -320) {
        // Recess the clock face before ending the cutscene, since the Termina
        // Field version of the cutscene looks right at it.
        this->clockFaceZTranslation -= 10;
    } else {
        type = OBJ_TOKEIDAI_TYPE(&this->actor);
        if ((type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_CLOCK_TOWN) ||
            (type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD)) {
            ObjTokeidai_TowerOpening_EndCutscene(this, play);
        } else {
            this->actionFunc = ObjTokeidai_DoNothing;
            if (this->actor.child != NULL) {
                this->actor.child->home.rot.x = 0;
            }
        }
    }
}

void ObjTokeidai_TowerOpening_Wait(ObjTokeidai* this, PlayState* play) {
    if (this->openingWaitTimer > 0) {
        this->openingWaitTimer--;
    } else {
        this->actionFunc = ObjTokeidai_TowerOpening_FinishOpening;
        this->openingWaitTimer = 0;
    }
}

void ObjTokeidai_TowerOpening_DropCounterweight(ObjTokeidai* this, PlayState* play) {
    this->xRotation += this->counterweightAngularVelocity;
    if (this->xRotation < 0x4000) {
        this->counterweightAngularVelocity += this->counterweightRotationalAcceleration;
        return;
    }

    this->counterweightRotationalAcceleration = 0x14;
    this->xRotation = 0x4000;
    switch (this->boundCount) {
        case 0:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_BOUND_0);
            break;

        case 1:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_BOUND_1);
            break;

        case 2:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_BOUND_2);
            break;

        default:
            break;
    }
    this->boundCount++;

    if (this->counterweightAngularVelocity > 0x190) {
        // This condition is met for the first bound, causing the counterweight
        // to rebound upwards quickly.
        this->counterweightAngularVelocity = -0xC8;
    } else if (this->counterweightAngularVelocity > 0x32) {
        // This condition is met for the second bound, causing the counterweight
        // to rebound upwards slowly.
        this->counterweightAngularVelocity = -(this->counterweightAngularVelocity >> 1);
    } else {
        // This condition is met for the third bound, causing the counterweight
        // to stop moving.
        this->actionFunc = ObjTokeidai_TowerOpening_Wait;
        this->openingWaitTimer = 10;
    }
}

void ObjTokeidai_TowerOpening_FinishRaise(ObjTokeidai* this, PlayState* play) {
    s32 type;

    if (this->settleTimer > 0) {
        // Jiggle the parts of the tower up and down to give it the appearance
        // of "settling" into place.
        if (this->settleTimer & 1) {
            this->yTranslation = this->settleAmount + 3400;
        } else {
            this->yTranslation = 3400 - this->settleAmount;
            if (this->settleAmount > 0) {
                this->settleAmount -= 4;
            }
        }
        this->settleTimer--;
    } else {
        type = OBJ_TOKEIDAI_TYPE(&this->actor);
        if ((type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_CLOCK_TOWN) ||
            (type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_FALL);
        }
        this->yTranslation = 3400;
        this->actionFunc = ObjTokeidai_TowerOpening_DropCounterweight;
        this->counterweightAngularVelocity = 0;
        this->counterweightRotationalAcceleration = 0xA;
        this->boundCount = 0;
    }
}

void ObjTokeidai_TowerOpening_RaiseTower(ObjTokeidai* this, PlayState* play) {
    s32 type;

    if (this->yTranslation < 3400) {
        type = OBJ_TOKEIDAI_TYPE(&this->actor);
        this->yTranslation += 25;
        if ((type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_CLOCK_TOWN) ||
            (type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_CLOCK_TOWER_UP - SFX_FLAG);
        }
    } else {
        type = OBJ_TOKEIDAI_TYPE(&this->actor);
        if ((type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_CLOCK_TOWN) ||
            (type == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_CLOCK_TOWER_STOP);
        }
        this->yTranslation = 3400;
        this->actionFunc = ObjTokeidai_TowerOpening_FinishRaise;
        this->settleTimer = 10;
        this->settleAmount = 20;
    }
}

void ObjTokeidai_TowerOpening_Start(ObjTokeidai* this, PlayState* play) {
    if ((Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_132) &&
         (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_132)]->id == 4)) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED)) {
        this->actionFunc = ObjTokeidai_TowerOpening_RaiseTower;
    }
}

/**
 * Sets up the sequence where the Clock Tower opens up
 * on the midnight of the Final Day.
 */
void ObjTokeidai_SetupTowerOpening(ObjTokeidai* this) {
    this->actionFunc = ObjTokeidai_TowerOpening_Start;
    this->clockFaceAngularVelocity = 0;
    this->clockFaceRotationTimer = 0;
    this->yTranslation = 0;
    this->xRotation = 0;
    this->clockFaceZTranslation = 0;
}

void ObjTokeidai_DoNothing(ObjTokeidai* this, PlayState* play) {
}

void ObjTokeidai_StaircaseToRooftop_Idle(ObjTokeidai* this, PlayState* play) {
    if ((((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) || (CURRENT_DAY >= 4)) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED)) {
        this->actor.draw = ObjTokeidai_Draw;
    } else {
        this->actor.draw = NULL;
    }
}

s32 ObjTokeidai_IsPostFirstCycleFinalHours(ObjTokeidai* this, PlayState* play) {
    if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
        return false;
    }
    if ((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        ObjTokeidai_SetupTowerOpening(this);
        return true;
    }
    return false;
}

void ObjTokeidai_RotateOnHourChange(ObjTokeidai* this, PlayState* play) {
    s32 currentClockHour = GET_CURRENT_CLOCK_HOUR(this);

    if (currentClockHour != this->clockHour) {
        if (this->clockFaceRotationTimer > 12) {
            // This actually performs the rotation to the next hour
            // for the clock face.
            this->clockFaceAngularVelocity += 0xA;
            this->clockFaceRotation += this->clockFaceAngularVelocity;
        } else {
            // This makes the clock face wiggle in place for a bit
            // before rotating to the next position.
            if ((this->clockFaceRotationTimer & 3) == 0) {
                this->clockFaceRotation += 0x3C;
            }
            if ((this->clockFaceRotationTimer & 3) == 1) {
                this->clockFaceRotation -= 0x3C;
            }
        }

        this->clockFaceRotationTimer++;

        if (((currentClockHour == 12) && (this->clockFaceRotation < 0)) ||
            ((currentClockHour != 12) && (this->clockFaceRotation > GET_CLOCK_FACE_ROTATION(currentClockHour)))) {
            this->clockFaceRotation = GET_CLOCK_FACE_ROTATION(currentClockHour);
            this->clockHour = currentClockHour;
            this->clockFaceAngularVelocity = 0;
            this->clockFaceRotationTimer = 0;
        }
    }

    // If the sun and moon panel doesn't have the target rotation (e.g., the time of day
    // just changed), rotate it until it matches the target.
    if (this->sunMoonPanelRotation != ObjTokeidai_GetTargetSunMoonPanelRotation()) {
        if (this->clockHour == 6) {
            this->sunMoonPanelAngularVelocity += 0x222;
            this->sunMoonPanelRotation += this->sunMoonPanelAngularVelocity;
            if (this->sunMoonPanelRotation > 0x10000) {
                this->sunMoonPanelRotation = ObjTokeidai_GetTargetSunMoonPanelRotation();
                this->sunMoonPanelAngularVelocity = 0;
            }
        }
        if (this->clockHour == 18) {
            this->sunMoonPanelAngularVelocity += 0x222;
            this->sunMoonPanelRotation += this->sunMoonPanelAngularVelocity;
            if (this->sunMoonPanelRotation > 0x8000) {
                this->sunMoonPanelRotation = ObjTokeidai_GetTargetSunMoonPanelRotation();
                this->sunMoonPanelAngularVelocity = 0;
            }
        }
    }
}

void ObjTokeidai_TowerClock_Idle(ObjTokeidai* this, PlayState* play) {
    if ((CURRENT_DAY == 3) && (this->clockHour < 6) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        this->actor.draw = ObjTokeidai_Clock_Draw;
        ObjTokeidai_SetupTowerOpening(this);
        SET_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED);
        return;
    }

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.home.rot.x = 1;
        this->clockTime += 3;
        this->actor.draw = ObjTokeidai_Clock_Draw;
    } else {
        if (!(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) &&
            (OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_TOWER_CLOCK_TERMINA_FIELD) &&
            (CutsceneManager_GetCurrentCsId() == CS_ID_NONE)) {
            this->actor.draw = NULL;
        }
        this->clockTime = CURRENT_TIME;
        if (this->actor.home.rot.x != 0) {
            ObjTokeidai_Clock_Init(this);
            this->actor.home.rot.x = 0;
        }
    }

    if ((CURRENT_DAY != 3) || (CURRENT_TIME >= CLOCK_TIME(6, 0))) {
        ObjTokeidai_RotateOnMinuteChange(this, true);
    }
    ObjTokeidai_RotateOnHourChange(this, play);
}

void ObjTokeidai_WallClock_Idle(ObjTokeidai* this, PlayState* play) {
    this->clockTime = CURRENT_TIME;
    ObjTokeidai_RotateOnMinuteChange(this, true);
    ObjTokeidai_RotateOnHourChange(this, play);
}

void ObjTokeidai_ExteriorGear_Idle(ObjTokeidai* this, PlayState* play) {
    if (ObjTokeidai_IsPostFirstCycleFinalHours(this, play)) {
        this->actor.draw = ObjTokeidai_ExteriorGear_Draw;
    } else {
        if (play->csCtx.state != CS_STATE_IDLE) {
            this->actor.home.rot.x = 1;
            this->clockTime += 3;
            this->actor.draw = ObjTokeidai_ExteriorGear_Draw;
        } else {
            if (!(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) &&
                (OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_EXTERIOR_GEAR_TERMINA_FIELD) &&
                (CutsceneManager_GetCurrentCsId() == CS_ID_NONE)) {
                this->actor.draw = NULL;
            }
            this->clockTime = CURRENT_TIME;
            if (this->actor.home.rot.x != 0) {
                ObjTokeidai_SetupClockOrExteriorGear(this);
                this->actor.home.rot.x = 0;
            }
        }
        ObjTokeidai_RotateOnMinuteChange(this, false);
    }
}

void ObjTokeidai_Counterweight_Idle(ObjTokeidai* this, PlayState* play) {
    s32 type;

    if (ObjTokeidai_IsPostFirstCycleFinalHours(this, play)) {
        this->spotlightIntensity = 0;

        if (this->actor.child == NULL) {
            type = OBJ_TOKEIDAI_TYPE(&this->actor);
            if ((type == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_CLOCK_TOWN) ||
                (type == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_TERMINA_FIELD)) {
                this->actor.child = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_HANABI, this->actor.world.pos.x,
                                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
            }
        }

        if (this->actor.child != NULL) {
            if (OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_COUNTERWEIGHT_TERMINA_FIELD) {
                this->actor.child->home.rot.x = 0x384;
            } else {
                this->actor.child->home.rot.x = 0x12C;
            }
        }
    } else {
        this->actor.shape.rot.y -= (s16)(0x40 * CVarGetFloat(CVAR_COSMETIC("Silly.ClockTowerSpeed"), 1.0f));
        if (gSaveContext.save.isNight) {
            if (this->spotlightIntensity < 100) {
                this->spotlightIntensity += 4;
            }
        } else if (this->spotlightIntensity > 0) {
            this->spotlightIntensity -= 4;
        }
    }
}

void ObjTokeidai_Update(Actor* thisx, PlayState* play) {
    ObjTokeidai* this = (ObjTokeidai*)thisx;
    this->actionFunc(this, play);
}

/**
 * Used for TerminaFieldWalls StaircaseToRooftop, and UnusedWall
 */
void ObjTokeidai_Draw(Actor* thisx, PlayState* play) {
    ObjTokeidai* this = (ObjTokeidai*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->opaDList != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, this->opaDList);
    }

    if (this->xluDList != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, this->xluDList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjTokeidai_Clock_Draw(Actor* thisx, PlayState* play) {
    ObjTokeidai* this = (ObjTokeidai*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Translate(0.0f, this->yTranslation, 0.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -1791.0f, MTXMODE_APPLY);
    Matrix_RotateXS(-this->xRotation, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 1791.0f, MTXMODE_APPLY);

    Matrix_Push();
    Matrix_RotateZS(-this->minuteRingOrExteriorGearRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gClockTowerMinuteRingDL);
    Matrix_Pop();

    Matrix_Translate(0.0f, 0.0f, this->clockFaceZTranslation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gClockTowerClockCenterAndHandDL);

    Matrix_RotateZS(-this->clockFaceRotation * 2, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    if (OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_WALL_CLOCK ||
        OBJ_TOKEIDAI_TYPE(&this->actor) == OBJ_TOKEIDAI_TYPE_SMALL_WALL_CLOCK) {
        gSPDisplayList(POLY_OPA_DISP++, gWallClockClockFaceDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gClockTowerClockFaceDL);
    }

    Matrix_Translate(0.0f, -1112.0f, -19.6f, MTXMODE_APPLY);
    Matrix_RotateYS(this->sunMoonPanelRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gClockTowerSunAndMoonPanelDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjTokeidai_Counterweight_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    u32 gameplayFrames = play->gameplayFrames;
    ObjTokeidai* this = (ObjTokeidai*)thisx;

    Matrix_RotateYS(-this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, this->yTranslation, 0.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -5480.0f, 80.0f, MTXMODE_APPLY);
    Matrix_RotateXS(-this->xRotation, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 5480.0f, -80.0f, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    // For scrolling the spotlight's mask texture down the length of the light beam.
    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, -gameplayFrames, 0, 0x20, 0x20, 0, 0, -1, 0));

    // Draws the counterweight
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->opaDList);

    // Draws the spotlight
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 255, 255, 235, 180, (s32)(this->spotlightIntensity * 2.55f));
    gSPDisplayList(POLY_XLU_DISP++, this->xluDList);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjTokeidai_ExteriorGear_Draw(Actor* thisx, PlayState* play) {
    ObjTokeidai* this = (ObjTokeidai*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, this->yTranslation, 0.0f, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, -1791.0f, MTXMODE_APPLY);
    Matrix_RotateYS(-this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateXS(-this->xRotation, MTXMODE_APPLY);
    Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 1791.0f, MTXMODE_APPLY);
    Matrix_RotateZS(this->minuteRingOrExteriorGearRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gClockTowerExteriorGearDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
