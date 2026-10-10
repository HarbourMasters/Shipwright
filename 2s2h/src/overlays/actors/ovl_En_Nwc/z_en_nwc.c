/*
 * File: z_en_nwc.c
 * Overlay: ovl_En_Nwc
 * Description: Cucco chick
 *
 *  This actor requires object_niw to be present for the transformation during Breman Mask March
 *  EnHs (Grog) must also be present to maintain the count of the EnNwc in the march
 */

#include "z_en_nwc.h"
#include "overlays/actors/ovl_En_Niw/z_en_niw.h"
#include "objects/object_nwc/object_nwc.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnNwc_Init(Actor* thisx, PlayState* play);
void EnNwc_Destroy(Actor* thisx, PlayState* play);
void EnNwc_Update(Actor* thisx, PlayState* play);
void EnNwc_Draw(Actor* thisx, PlayState* play);

void EnNwc_LoadNiwSkeleton(EnNwc* this, PlayState* play);
void EnNwc_CrowAtTheEnd(EnNwc* this, PlayState* play);
void EnNwc_Follow(EnNwc* this, PlayState* play);
void EnNwc_HopForward(EnNwc* this, PlayState* play);
void EnNwc_RunAway(EnNwc* this, PlayState* play);
void EnNwc_Turn(EnNwc* this, PlayState* play);
void EnNwc_CheckForBreman(EnNwc* this, PlayState* play);

void EnNwc_DrawAdultBody(Actor* thisx, PlayState* play);
EnHs* EnNwc_FindGrog(PlayState* play);

typedef enum EnNwcState {
    /* -1 */ NWC_STATE_NIW_LOADED = -1, // set after loading object_niw
    /*  0 */ NWC_STATE_CHECK_BREMAN,    // checking for breman mask
    /*  1 */ NWC_STATE_TURNING,         // turning to face a new direction to explore
    /*  2 */ NWC_STATE_HOPPING_FORWARD, // hopping to go explore
    /*  3 */ NWC_STATE_FOLLOWING,       // following the player
    /*  4 */ NWC_STATE_RUNNING          // running from the player after failed breman march
} EnNwcState;

ActorProfile En_Nwc_Profile = {
    /**/ ACTOR_EN_NWC,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NWC,
    /**/ sizeof(EnNwc),
    /**/ EnNwc_Init,
    /**/ EnNwc_Destroy,
    /**/ EnNwc_Update,
    /**/ EnNwc_Draw,
};

void EnNwc_Init(Actor* thisx, PlayState* play) {
    s32 niwObjectSlot;
    EnNwc* this = (EnNwc*)thisx;

    niwObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_NIW);
    if (niwObjectSlot <= OBJECT_SLOT_NONE) {
        // niw object does not exist, we need it for tranformation, despawn
        Actor_Kill(&this->actor);
        return;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_08)) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, NIW_TYPE_REGULAR);
        Actor_Kill(&this->actor);
        return;
    }

    this->niwObjectSlot = niwObjectSlot;
    this->nwcObjectIndex = this->actor.objectSlot;
    this->grog = EnNwc_FindGrog(play);

    this->footRotY = this->footRotZ = 0;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnNwc_LoadNiwSkeleton;
    this->hasGrownUp = false;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 6.0f);
    this->actor.velocity.y = 0.0f;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;
}

void EnNwc_Destroy(Actor* thisx, PlayState* play) {
    EnNwc* this = (EnNwc*)thisx;
}

void EnNwc_SpawnDust(EnNwc* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 80, 80, 80, 255 };
    Vec3f pos;
    Vec3f vec5;
    Vec3f velocity;
    Vec3f accel;
    s16 yaw;
    s16 pitch;
    Vec3f eye = GET_ACTIVE_CAM(play)->eye;
    s32 i;

    yaw = Math_Vec3f_Yaw(&eye, &this->actor.world.pos);
    pitch = -Math_Vec3f_Pitch(&eye, &this->actor.world.pos);
    vec5.x = this->actor.world.pos.x - 5.0f * Math_SinS(yaw) * Math_CosS(pitch);
    vec5.y = this->actor.world.pos.y - 5.0f * Math_SinS(pitch);
    vec5.z = this->actor.world.pos.z - 5.0f * Math_CosS(yaw) * Math_CosS(pitch);

    for (i = 0; i < 5; i++) {
        velocity.x = Rand_CenteredFloat(4.0f);
        velocity.y = Rand_CenteredFloat(4.0f);
        velocity.z = Rand_CenteredFloat(4.0f);
        accel.x = -velocity.x * 0.1f;
        accel.y = -velocity.y * 0.1f;
        accel.z = -velocity.z * 0.1f;
        pos.x = vec5.x + velocity.x;
        pos.y = vec5.y + velocity.y;
        pos.z = vec5.z + velocity.z;

        func_800B0F80(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, 300, 30, 10);
    }
}

EnHs* EnNwc_FindGrog(PlayState* play) {
    Actor* grogSearch = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (grogSearch != NULL) {
        if (grogSearch->id == ACTOR_EN_HS) {
            return (EnHs*)grogSearch;
        }
        grogSearch = grogSearch->next;
    }

    return NULL;
}

s32 EnNwc_PlayerReleasedBremanMarch(EnNwc* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // Weird: home.rot.x holds count of chicks having transformed into adult.
    // Weird: Its incremented by 1 unlike chicks following, so it should max at 10.
    if (this->grog->actor.home.rot.x >= 20) {
        return false;
    }

    if (player->stateFlags3 & PLAYER_STATE3_20000000) {
        return false;
    }

    return true;
}

/**
 * Summary: Checks 1) if grog exists, 2) player is using breman mask, 3) and within range.
 *  Used to identify if the chick should be captured by breman mask.
 */
s32 EnNwc_IsFound(EnNwc* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->grog == NULL) {
        return false;
    }

    if ((player->stateFlags3 & PLAYER_STATE3_20000000) && (this->actor.xzDistToPlayer < 100.0f)) {
        return true;
    }

    return false;
}

void EnNwc_ChangeState(EnNwc* this, s16 newState) {
    this->actor.speed = 0.0f;
    switch (newState) {
        case NWC_STATE_CHECK_BREMAN:
            this->stateTimer = 10;
            this->actionFunc = EnNwc_CheckForBreman;
            break;

        case NWC_STATE_TURNING:
            this->stateTimer = Rand_ZeroFloat(20.0f) + 15.0f;
            this->actionFunc = EnNwc_Turn;
            this->fallingRotY = TRUNCF_BINANG(Rand_CenteredFloat(0x10000));
            break;

        case NWC_STATE_HOPPING_FORWARD:
            this->stateTimer = Rand_ZeroFloat(20.0f) + 15.0f;
            this->actionFunc = EnNwc_HopForward;
            break;

        case NWC_STATE_FOLLOWING:
            this->actionFunc = EnNwc_Follow;
            this->transformTimer = 0;
            this->randomRot = TRUNCF_BINANG(Rand_CenteredFloat(0x2710));
            break;

        case NWC_STATE_RUNNING:
            //! FAKE: & 0xFFFF
            this->actor.world.rot.y = this->actor.home.rot.z * 0x3000 & 0xFFFF;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->stateTimer = Rand_ZeroFloat(40.0f) + 120.0f;
            this->actionFunc = EnNwc_RunAway;
            break;

        default:
            break;
    }

    this->state = newState;
}

/**
 * Summary: Changes the current actor state
 *     If previously random behavior -> check if breman mask is active
 *     If previously checking for breman -> select random (NWC_STATE_TURNING, NWC_STATE_HOPPING_FORWARD)
 */
void EnNwc_ToggleState(EnNwc* this) {
    this->actor.speed = 0.0f;
    if (this->state != NWC_STATE_CHECK_BREMAN) {
        EnNwc_ChangeState(this, NWC_STATE_CHECK_BREMAN);
    } else {
        EnNwc_ChangeState(this, Rand_ZeroFloat(2.0f) + 1.0f);
    }
}

void EnNwc_CheckFound(EnNwc* this, PlayState* play) {
    if (EnNwc_IsFound(this, play)) {
        u8 currentChickCount = (this->grog->actor.home.rot.z / 2);

        if (currentChickCount > 9) {
            currentChickCount = 9;
        }

        // save our current chick order
        this->actor.home.rot.z = this->grog->actor.home.rot.z + 1;

        // if < 10 chicks, increment grog's chick counter
        if (this->grog->actor.home.rot.z < 20) {
            this->grog->actor.home.rot.z += 2;
        }

        EnNwc_ChangeState(this, NWC_STATE_FOLLOWING);
        Audio_PlaySfx_AtPosWithAllChannelsIO(&gSfxDefaultPos, NA_SE_SY_CHICK_JOIN_CHIME, currentChickCount);
    }
}

void EnNwc_LoadNiwSkeleton(EnNwc* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->niwObjectSlot)) {
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->niwObjectSlot].segment);

        SkelAnime_InitFlex(play, &this->niwSkeleton, &gNiwSkel, &gNiwIdleAnim, this->jointTable, this->morphTable,
                           NIW_LIMB_MAX);
        Animation_Change(&this->niwSkeleton, &gNiwIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gNiwIdleAnim),
                         ANIMMODE_LOOP, 0.0f);

        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->nwcObjectIndex].segment);
        this->state = NWC_STATE_NIW_LOADED;
        EnNwc_ToggleState(this);
    }
}

void EnNwc_CrowAtTheEnd(EnNwc* this, PlayState* play) {
    // I guess grog handles the scene transit?
    Math_SmoothStepToS(&this->upperBodyRotY, 0x2710, 2, 0x1B58, 0x3E8);
    Math_SmoothStepToS(&this->footRotZ, 0, 2, 0x1B58, 0x3E8);
    Math_SmoothStepToS(&this->footRotY, 0x36B0, 2, 0x1B58, 0x3E8);
}

/**
 * Summary: Controls the Chick when following Breman Mask.
 *
 * ActionFunc for NWC Type: NWC_STATE_FOLLOWING
 */
void EnNwc_Follow(EnNwc* this, PlayState* play) {
    Vec3f* chickCoords = this->grog->nwcPos;
    Vec3f targetVector;
    s32 pad;
    s16 newRotY;

    this->stateTimer++;
    if (this->hasGrownUp & 1) {
        s16 targetUpperBodyRot = 0;
        s16 targetFootRot = 0;

        if (this->actor.speed > 0.0f) {
            if (this->stateTimer & 4) {
                targetFootRot = 0x1B58;
                targetUpperBodyRot = -0x2710;
            }
            if ((this->stateTimer & 3) == 3) {
                this->actor.velocity.y = 2.0f; // hop up and down
            }
        }
        Math_SmoothStepToS(&this->footRotZ, targetFootRot, 2, 0x1B58, 0x3E8);
        Math_SmoothStepToS(&this->upperBodyRotY, targetUpperBodyRot, 2, 0x1B58, 0x3E8);

    } else { // NOT grown up
        if (((this->stateTimer & 3) == 3) && ((this->stateTimer & 0x14) != 0)) {
            this->actor.velocity.y = 2.0f; // hop up and down
        }
        if ((this->stateTimer & 0x1B) == 24) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICK_SONG);
        }
    }

    if ((this->grog->actor.home.rot.z >= 20) && // all 10 chicks have been found
        !(this->hasGrownUp & 1)) {
        this->transformTimer += 2;
        if (this->transformTimer >= (s16)((this->actor.home.rot.z * 0x1E) + 0x1E)) {
            // it is our turn to transform
            this->hasGrownUp |= 1;
            this->grog->actor.home.rot.x += 2; // increment grog's adult tranformation counter
            EnNwc_SpawnDust(this, play);
            Actor_SetScale(&this->actor, 0.002f);
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHICK_TO_CHICKEN);
        }
    }

    Math_Vec3f_Diff(&chickCoords[this->actor.home.rot.z], &this->actor.world.pos, &targetVector);
    if (SQXZ(targetVector) < SQ(5.0f)) { // too close to keep moving, stop
        this->actor.speed = 0.0f;

        // first nwc in the line follows player, the rest follow the previous one
        if (this->actor.home.rot.z == 1) {
            newRotY = this->actor.yawTowardsPlayer;
        } else {
            // for some reason the array is 10 * 2, incremented by 2, so this is "index - 1"
            newRotY = Math_Vec3f_Yaw(&this->actor.world.pos, &chickCoords[this->actor.home.rot.z - 2]);
        }

    } else { // not too close: keep moving
        this->randomRot += TRUNCF_BINANG(Rand_CenteredFloat(0x5DC));
        if (this->randomRot > 0x1388) {
            this->randomRot = 0x1388;
        } else if (this->randomRot < -0x1388) {
            this->randomRot = -0x1388;
        }
        this->actor.speed = 2.0f;
        newRotY = Math_Vec3f_Yaw(&this->actor.world.pos, &chickCoords[this->actor.home.rot.z]) + this->randomRot;
    }

    this->actor.world.rot.y = newRotY;
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0xBB8, 0xC8);

    if (EnNwc_PlayerReleasedBremanMarch(this, play)) {
        this->grog->actor.home.rot.x = 0; // reset adult count
        this->grog->actor.home.rot.z = 0; // reset chick follow count

        EnNwc_ChangeState(this, NWC_STATE_RUNNING);

        if (this->hasGrownUp & 1) {
            EnNwc_SpawnDust(this, play);
        }

        this->hasGrownUp &= ~1;
        Actor_SetScale(&this->actor, 0.01f);
    }

    if (this->actor.scale.x < 0.01f) {
        this->actor.scale.x += 0.002f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
    }

    if (this->grog->actor.home.rot.x >= 20) { // all chicks have turned into adult cucco, stop and crow
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
        this->actionFunc = EnNwc_CrowAtTheEnd;
        this->actor.speed = 0.0f;
        Actor_SetScale(&this->actor, 0.01f);
    }
}

/**
 * Summary: Chick is Walking (Hopping) in a straight line.
 *
 * ActionFunc for NWC Type: NWC_STATE_HOPPING_FORWARD
 */
void EnNwc_HopForward(EnNwc* this, PlayState* play) {
    if (DECR(this->stateTimer) == 0) {
        EnNwc_ToggleState(this);
        return;
    }

    if ((this->stateTimer & 3) == 3) {
        this->actor.velocity.y = 2.0f; // hop up and down
    }
    if ((this->stateTimer & 0xB) == 8) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICK_SONG);
    }

    // they only move forward while off the ground, which gives the visual of them hopping to move
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    } else {
        this->actor.speed = 2.0f;
    }
}

/**
 * Summary: Chick is Running Away from player (Breman Mask was dropped).
 *
 * ActionFunc for NWC Type: NWC_STATE_RUNNING
 */
void EnNwc_RunAway(EnNwc* this, PlayState* play) {
    if (DECR(this->stateTimer) == 0) {
        EnNwc_ToggleState(this);
        return;
    }

    if ((this->stateTimer & 3) == 3) {
        this->actor.velocity.y = 2.0f; // hop up and down
    }
    if ((this->stateTimer & 0xB) == 8) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICK_SONG);
    }

    this->actor.speed = 2.0f;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        EnNwc_ToggleState(this);
    }
}

/**
 * Summary: Chick is turning to face a new direction.
 *
 * ActionFunc for NWC Type: NWC_STATE_TURNING
 */
void EnNwc_Turn(EnNwc* this, PlayState* play) {
    if (DECR(this->stateTimer) == 0) {
        EnNwc_ToggleState(this);
        return;
    }

    if ((this->stateTimer & 7) == 7) {
        this->actor.velocity.y = 2.0f; // vertical hop
    }

    if ((this->stateTimer & 0xB) == 8) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHICK_SONG);
    }

    // they only rotate when off the ground, giving the visual that they turn by hopping
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->fallingRotY, 0xA, 0x1000, 0x800);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

/**
 * Summary: Chick is standing still.
 *          Looking around, can see the player with the Breman Mask
 *
 * ActionFunc for NWC Type: NWC_STATE_CHECK_BREMAN
 */
void EnNwc_CheckForBreman(EnNwc* this, PlayState* play) {
    if (DECR(this->stateTimer) == 0) {
        EnNwc_ToggleState(this);
    }

    EnNwc_CheckFound(this, play);
}

void EnNwc_Update(Actor* thisx, PlayState* play) {
    EnNwc* this = (EnNwc*)thisx;

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);

    if (this->hasGrownUp & 1) {
        this->actor.objectSlot = this->niwObjectSlot;
        this->actor.draw = EnNwc_DrawAdultBody;
        this->actor.shape.shadowScale = 15.0f;
    } else {
        this->actor.objectSlot = this->nwcObjectIndex;
        this->actor.draw = EnNwc_Draw;
        this->actor.shape.shadowScale = 6.0f;
    }

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(0x3C, 0x3C);
    }

    if ((this->blinkTimer == 1) || (this->blinkTimer == 3)) {
        this->blinkState = true;
    } else {
        this->blinkState = false;
    }
}

void EnNwc_Draw(Actor* thisx, PlayState* play) {
    TexturePtr eyeTextures[] = { gNwcEyeOpenTex, gNwcEyeClosedTex };
    EnNwc* this = (EnNwc*)thisx;
    Gfx* dispHead;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    dispHead = POLY_OPA_DISP;

    gSPSegment(&dispHead[0], 0x08, Lib_SegmentedToVirtual(eyeTextures[this->blinkState]));

    MATRIX_FINALIZE_AND_LOAD(&dispHead[1], play->state.gfxCtx);

    gSPDisplayList(&dispHead[2], &gNwcBodyDL);

    POLY_OPA_DISP = &dispHead[3];

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnNwc_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnNwc* this = (EnNwc*)thisx;

    if (limbIndex == NIW_LIMB_UPPER_BODY) {
        rot->y += this->upperBodyRotY;
    }
    if ((limbIndex == NIW_LIMB_RIGHT_WING_ROOT) || (limbIndex == NIW_LIMB_LEFT_WING_ROOT)) {
        rot->y += this->footRotY;
        rot->z += this->footRotZ;
    }

    return false;
}

void EnNwc_DrawAdultBody(Actor* thisx, PlayState* play) {
    EnNwc* this = (EnNwc*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->niwSkeleton.skeleton, this->niwSkeleton.jointTable, this->niwSkeleton.dListCount,
                          EnNwc_OverrideLimbDraw, NULL, &this->actor);
}
