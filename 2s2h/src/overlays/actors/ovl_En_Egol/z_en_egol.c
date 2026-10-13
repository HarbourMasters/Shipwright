/*
 * File: z_en_egol.c
 * Overlay: ovl_En_Egol
 * Description: Eyegore
 */

#include "z_en_egol.h"
#include "objects/object_eg/object_eg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Estone/z_en_estone.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

typedef enum {
    /*  0 */ EYEGORE_ACTION_WAIT,
    /*  1 */ EYEGORE_ACTION_STAND,
    /*  2 */ EYEGORE_ACTION_WALK,
    /*  3 */ EYEGORE_ACTION_RETREAT,
    /*  4 */ EYEGORE_ACTION_LASER,
    /*  5 */ EYEGORE_ACTION_SLAM,
    /*  6 */ EYEGORE_ACTION_PUNCH,
    /*  7 */ EYEGORE_ACTION_SLAM_WAIT,
    /*  8 */ EYEGORE_ACTION_STUNNED,
    /*  9 */ EYEGORE_ACTION_STUN_END,
    /* 10 */ EYEGORE_ACTION_SLAM_END,
    /* 11 */ EYEGORE_ACTION_DAMAGED,
    /* 12 */ EYEGORE_ACTION_STOP,
    /* 13 */ EYEGORE_ACTION_DYING,
    /* 14 */ EYEGORE_ACTION_DEAD
} EnEgolAction;

typedef enum {
    /* 0 */ EYEGORE_EFFECT_IMPACT,
    /* 1 */ EYEGORE_EFFECT_PIECE_LARGE,
    /* 2 */ EYEGORE_EFFECT_PIECE_SMALL,
    /* 3 */ EYEGORE_EFFECT_DEBRIS
} EnEgolEffectType;

typedef enum {
    /* 0 */ EYEGORE_LASER_OFF,
    /* 1 */ EYEGORE_LASER_START,
    /* 2 */ EYEGORE_LASER_CHARGING,
    /* 3 */ EYEGORE_LASER_FIRE,
    /* 7 */ EYEGORE_LASER_ON = 7
} EnEgolLaserState;

void EnEgol_Init(Actor* thisx, PlayState* play);
void EnEgol_Destroy(Actor* thisx, PlayState* play);
void EnEgol_Update(Actor* thisx, PlayState* play);
void EnEgol_Draw(Actor* thisx, PlayState* play2);

void EnEgol_SetupWait(EnEgol* this);
void EnEgol_Wait(EnEgol* this, PlayState* play);
void EnEgol_SetupStand(EnEgol* this);
void EnEgol_Stand(EnEgol* this, PlayState* play);
void EnEgol_SetupWalk(EnEgol* this);
void EnEgol_Walk(EnEgol* this, PlayState* play);
void EnEgol_SetupRetreat(EnEgol* this);
void EnEgol_Retreat(EnEgol* this, PlayState* play);
void EnEgol_SetupLaser(EnEgol* this);
void EnEgol_Laser(EnEgol* this, PlayState* play);
void EnEgol_SetupStop(EnEgol* this);
void EnEgol_Stop(EnEgol* this, PlayState* play);
void EnEgol_SetupSlam(EnEgol* this);
void EnEgol_Slam(EnEgol* this, PlayState* play);
void EnEgol_SetupPunch(EnEgol* this);
void EnEgol_Punch(EnEgol* this, PlayState* play);
void EnEgol_SetupSlamWait(EnEgol* this);
void EnEgol_SlamWait(EnEgol* this, PlayState* play);
void EnEgol_SetupStunned(EnEgol* this);
void EnEgol_Stunned(EnEgol* this, PlayState* play);
void EnEgol_SetupStunEnd(EnEgol* this);
void EnEgol_StunEnd(EnEgol* this, PlayState* play);
void EnEgol_SetupSlamEnd(EnEgol* this);
void EnEgol_SlamEnd(EnEgol* this, PlayState* play);
void EnEgol_SetupDamaged(EnEgol* this);
void EnEgol_Damaged(EnEgol* this, PlayState* play);
void EnEgol_StartDeathCutscene(EnEgol* this, PlayState* play);
void EnEgol_Death(EnEgol* this, PlayState* play);

void EnEgol_SpawnEffect(EnEgol* this, Vec3f* pos, Vec3s* rot, s16 lifetime, f32 scale, s16 type);
void EnEgol_UpdateEffects(EnEgol* this, PlayState* play);
void EnEgol_DrawEffects(EnEgol* this, PlayState* play);

static ColliderJntSphElementInit sEyeJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { EYEGORE_LIMB_EYE_COLLIDER, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphElementInit sBodySphElementsInit[6] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { EYEGORE_LIMB_LEFT_HAND, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { EYEGORE_LIMB_RIGHT_HAND, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { EYEGORE_LIMB_HEAD, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { EYEGORE_LIMB_LEFT_SHOULDER, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { EYEGORE_LIMB_RIGHT_SHOULDER, { { 0, 0, 0 }, 0 }, 1 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { EYEGORE_LIMB_FAULDS, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphInit sBodyJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sBodySphElementsInit),
    sBodySphElementsInit,
};

static ColliderJntSphInit sEyeJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sEyeJntSphElementsInit),
    sEyeJntSphElementsInit,
};

static ColliderQuadInit sLaserQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x07, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum {
    /* 0x0 */ EYEGORE_DMGEFF_IMMUNE_0,
    /* 0x4 */ EYEGORE_DMGEFF_LIGHT_ARROW = 4,
    /* 0xE */ EYEGORE_DMGEFF_NONE = 0xE,
    /* 0xF */ EYEGORE_DMGEFF_IMMUNE_F
} EnEgolDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Deku Stick     */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Horse trample  */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Explosives     */ DMG_ENTRY(2, EYEGORE_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Hookshot       */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Goron pound    */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Fire arrow     */ DMG_ENTRY(2, EYEGORE_DMGEFF_NONE),
    /* Ice arrow      */ DMG_ENTRY(2, EYEGORE_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(2, EYEGORE_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EYEGORE_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Zora barrier   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Normal shield  */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Light ray      */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Thrown object  */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EYEGORE_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_F),
    /* Sword beam     */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Normal Roll    */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Unblockable    */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EYEGORE_DMGEFF_IMMUNE_0),
    /* Powder Keg     */ DMG_ENTRY(1, EYEGORE_DMGEFF_IMMUNE_F),
};

static Color_RGB8 sEyePrimColors[] = {
    { 175, 255, 255 }, // light cyan
    { 195, 245, 10 },  // lime green
    { 255, 55, 205 },  // magenta
};

static Color_RGB8 sEyeEnvColors[] = {
    { 255, 115, 155 }, // pink
    { 40, 155, 155 },  // dark cyan
    { 215, 255, 255 }, // light cyan
};

static Color_RGB8 sLightOrbColors[] = {
    { 255, 255, 255 }, // white
    { 255, 255, 255 }, // white
};

ActorProfile En_Egol_Profile = {
    /**/ ACTOR_EN_EGOL,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_EG,
    /**/ sizeof(EnEgol),
    /**/ EnEgol_Init,
    /**/ EnEgol_Destroy,
    /**/ EnEgol_Update,
    /**/ EnEgol_Draw,
};

typedef enum {
    /*  0 */ EYEGORE_ANIM_STAND,
    /*  1 */ EYEGORE_ANIM_WALK,
    /*  2 */ EYEGORE_ANIM_SLAM,
    /*  3 */ EYEGORE_ANIM_SLAM_WAIT,
    /*  4 */ EYEGORE_ANIM_SLAM_END,
    /*  5 */ EYEGORE_ANIM_DAMAGED,
    /*  6 */ EYEGORE_ANIM_DEATH,
    /*  7 */ EYEGORE_ANIM_LASER,
    /*  8 */ EYEGORE_ANIM_LASER_END, // unused
    /*  9 */ EYEGORE_ANIM_STUNNED,
    /* 10 */ EYEGORE_ANIM_STUN_END,
    /* 11 */ EYEGORE_ANIM_RETREAT,
    /* 12 */ EYEGORE_ANIM_SIT,
    /* 13 */ EYEGORE_ANIM_LEFT_PUNCH,
    /* 14 */ EYEGORE_ANIM_RIGHT_PUNCH,
    /* 15 */ EYEGORE_ANIM_MAX
} EnEgolAnimation;

static AnimationHeader* sAnimations[EYEGORE_ANIM_MAX] = {
    &gEyegoreStandAnim,          // EYEGORE_ANIM_STAND
    &gEyegoreWalkAnim,           // EYEGORE_ANIM_WALK
    &gEyegoreSlamAnim,           // EYEGORE_ANIM_SLAM
    &gEyegoreSlamWaitAnim,       // EYEGORE_ANIM_SLAM_WAIT
    &gEyegoreSlamEndAnim,        // EYEGORE_ANIM_SLAM_END
    &gEyegoreDamagedAnim,        // EYEGORE_ANIM_DAMAGED
    &gEyegoreDeathAnim,          // EYEGORE_ANIM_DEATH
    &gEyegoreLaserAnim,          // EYEGORE_ANIM_LASER
    &gEyegoreUnusedLaserEndAnim, // EYEGORE_ANIM_LASER_END
    &gEyegoreStunnedAnim,        // EYEGORE_ANIM_STUNNED
    &gEyegoreStunEndAnim,        // EYEGORE_ANIM_STUN_END
    &gEyegoreRetreatAnim,        // EYEGORE_ANIM_RETREAT
    &gEyegoreSitAnim,            // EYEGORE_ANIM_SIT
    &gEyegoreLeftPunchAnim,      // EYEGORE_ANIM_LEFT_PUNCH
    &gEyegoreRightPunchAnim,     // EYEGORE_ANIM_RIGHT_PUNCH
};

static u8 sAnimationModes[EYEGORE_ANIM_MAX] = {
    ANIMMODE_ONCE, // EYEGORE_ANIM_STAND
    ANIMMODE_LOOP, // EYEGORE_ANIM_WALK
    ANIMMODE_ONCE, // EYEGORE_ANIM_SLAM
    ANIMMODE_ONCE, // EYEGORE_ANIM_SLAM_WAIT
    ANIMMODE_ONCE, // EYEGORE_ANIM_SLAM_END
    ANIMMODE_ONCE, // EYEGORE_ANIM_DAMAGED
    ANIMMODE_ONCE, // EYEGORE_ANIM_DEATH
    ANIMMODE_ONCE, // EYEGORE_ANIM_LASER
    ANIMMODE_ONCE, // EYEGORE_ANIM_LASER_END
    ANIMMODE_ONCE, // EYEGORE_ANIM_STUNNED
    ANIMMODE_ONCE, // EYEGORE_ANIM_STUN_END
    ANIMMODE_LOOP, // EYEGORE_ANIM_RETREAT
    ANIMMODE_ONCE, // EYEGORE_ANIM_SIT
    ANIMMODE_ONCE, // EYEGORE_ANIM_LEFT_PUNCH
    ANIMMODE_ONCE, // EYEGORE_ANIM_RIGHT_PUNCH
};

void EnEgol_ChangeAnim(EnEgol* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], 0.0f);
}

void EnEgol_FootstepEffects(EnEgol* this, PlayState* play, f32 leftFootFrame, f32 rightFootFrame) {
    if (Animation_OnFrame(&this->skelAnime, leftFootFrame) || Animation_OnFrame(&this->skelAnime, rightFootFrame)) {
        Vec3f spawnPos;
        Player* player = GET_PLAYER(play);
        s32 quakeYOffset;
        s32 pad;

        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_WALK);
        quakeYOffset = 4 - (s32)(fabsf(player->actor.world.pos.y - this->actor.world.pos.y) * 0.02f);
        if (quakeYOffset > 4) {
            quakeYOffset = 4;
        } else if (quakeYOffset < 1) {
            quakeYOffset = 1;
        }

        if (player->stateFlags3 != PLAYER_STATE3_1000000) {
            Actor_RequestQuakeAndRumble(&this->actor, play, quakeYOffset, 2);
        }
        if (Animation_OnFrame(&this->skelAnime, leftFootFrame)) {
            Math_Vec3f_Copy(&spawnPos, &this->leftFootPos);
            spawnPos.y = this->actor.floorHeight;
            Actor_SpawnFloorDustRing(play, &this->actor, &spawnPos, 0.0f, 10, 6.0f, 50, 30, true);
        } else {
            Math_Vec3f_Copy(&spawnPos, &this->rightFootPos);
            spawnPos.y = this->actor.floorHeight;
            Actor_SpawnFloorDustRing(play, &this->actor, &spawnPos, 0.0f, 10, 6.0f, 50, 30, true);
        }
    }
}

void EnEgol_DestroyBlocks(EnEgol* this, PlayState* play, Vec3f pos1, Vec3f pos2) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (prop != NULL) {
        if (prop->id != ACTOR_EN_EGBLOCK) {
            prop = prop->next;
            continue;
        }
        if (prop->colChkInfo.health > 0) {
            Vec3f blockTo1;
            Vec3f blockTo2;
            f32 dist1;
            f32 dist2;

            blockTo1.x = pos1.x - prop->world.pos.x;
            blockTo1.y = pos1.y - prop->world.pos.y;
            blockTo1.z = pos1.z - prop->world.pos.z;

            blockTo2.x = pos2.x - prop->world.pos.x;
            blockTo2.y = pos2.y - prop->world.pos.y;
            blockTo2.z = pos2.z - prop->world.pos.z;

            dist1 = sqrtf(SQXZ(blockTo1));
            dist2 = sqrtf(SQXZ(blockTo2));

            if ((dist1 < 40.0f) || (dist2 < 40.0f)) {
                dist1 = sqrtf(SQ(blockTo1.y));
                dist2 = sqrtf(SQ(blockTo2.y));
                if ((dist1 < 40.0f) || (dist2 < 40.0f)) {
                    prop->colChkInfo.health = 0;
                    Actor_PlaySfx(&this->actor, NA_SE_EV_WALL_BROKEN);
                    break;
                }
            }
        }
        prop = prop->next;
    }
}

void EnEgol_GetWaypoint(EnEgol* this) {
    if ((this->pathIndex != PATH_INDEX_NONE) && (this->path != NULL) &&
        !SubS_CopyPointFromPath(this->path, this->waypoint, &this->waypointPos)) {
        Actor_Kill(&this->actor);
    }
}

#define EYEGORE_SET_SPH_DIM(element, centerX, centerY, centerZ, radiusV, scaleV) \
    element.dim.modelSphere.radius = radiusV;                                    \
    element.dim.scale = scaleV;                                                  \
    element.dim.modelSphere.center.x = centerX;                                  \
    element.dim.modelSphere.center.y = centerY;                                  \
    element.dim.modelSphere.center.z = centerZ

void EnEgol_Init(Actor* thisx, PlayState* play) {
    EnEgol* this = (EnEgol*)thisx;

    this->actor.gravity = -2.0f;
    Actor_SetScale(&this->actor, 0.015f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 60.0f);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.hintId = TATL_HINT_ID_EYEGORE;
    this->actor.colChkInfo.health = 8;

    SkelAnime_InitFlex(play, &this->skelAnime, &gEyegoreSkel, &gEyegoreStandAnim, this->jointTable, this->morphTable,
                       EYEGORE_LIMB_MAX);

    Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sBodyJntSphInit, this->bodyElements);
    Collider_InitAndSetJntSph(play, &this->eyeCollider, &this->actor, &sEyeJntSphInit, this->eyeElements);
    Collider_InitAndSetQuad(play, &this->laserCollider, &this->actor, &sLaserQuadInit);

    this->pathIndex = EYEGORE_GET_PATH_INDEX(&this->actor);
    if (this->pathIndex == EYEGORE_PATH_INDEX_NONE) {
        this->pathIndex = PATH_INDEX_NONE;
        Actor_Kill(&this->actor);
        return;
    }

    this->wakeupRange = this->actor.world.rot.x * 20.0f;
    this->actor.world.rot.x = 0;
    if (this->wakeupRange < 0.0f) {
        this->wakeupRange = 200.0f;
    }

    this->minLaserRange = this->actor.world.rot.z * 20.0f;
    this->actor.world.rot.z = 0;
    if (this->minLaserRange < 0.0f) {
        this->minLaserRange = 200.0f;
    }

    this->path = SubS_GetPathByIndex(play, this->pathIndex, EYEGORE_PATH_INDEX_NONE);

    EYEGORE_SET_SPH_DIM(this->eyeCollider.elements[0], 500, 0, 0, 26, 1.0f);

    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[0], 1800, 100, 0, 20, 1.0f);
    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[1], 1800, 100, 0, 20, 1.0f);
    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[2], 1000, -500, 0, 35, 1.0f);
    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[3], 200, 200, 0, 25, 1.0f);
    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[4], 300, 200, 0, 25, 1.0f);
    EYEGORE_SET_SPH_DIM(this->bodyCollider.elements[5], 2100, -300, 0, 37, 1.0f);

    this->switchFlag = EYEGORE_GET_SWITCH_FLAG(&this->actor);
    if (this->switchFlag == EYEGORE_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }
    EnEgol_SetupWait(this);
}

void EnEgol_Destroy(Actor* thisx, PlayState* play) {
    EnEgol* this = (EnEgol*)thisx;

    Collider_DestroyJntSph(play, &this->bodyCollider);
    Collider_DestroyJntSph(play, &this->eyeCollider);
    Collider_DestroyQuad(play, &this->laserCollider);
}

void EnEgol_SetupWait(EnEgol* this) {
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->action = EYEGORE_ACTION_WAIT;
    this->actionFunc = EnEgol_Wait;
}

void EnEgol_Wait(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s16 angleToFacing = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

    if ((curFrame >= this->animEndFrame) && (ABS_ALT(angleToFacing) < 0x3000) &&
        (this->actor.xzDistToPlayer < this->wakeupRange)) {
        EnEgol_SetupStand(this);
    }
}

void EnEgol_SetupStand(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_STAND);
    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    this->action = EYEGORE_ACTION_STAND;
    Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_STAND);
    this->actionFunc = EnEgol_Stand;
}

void EnEgol_Stand(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnEgol_SetupWalk(this);
    }
}

void EnEgol_SetupWalk(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_WALK);
    this->laserCount = 0;
    EnEgol_GetWaypoint(this);
    this->action = EYEGORE_ACTION_WALK;
    this->actionFunc = EnEgol_Walk;
}

void EnEgol_Walk(EnEgol* this, PlayState* play) {
    s16 angleToFacing = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);
    Vec3f spawnPos;

    Math_SmoothStepToS(&this->headRot, 0, 1, 0x3E8, 0);
    Math_Vec3f_Copy(&spawnPos, &gZeroVec3f);
    if (curFrame >= this->animEndFrame) {
        this->laserCount++;
        if (this->laserCount > 0) {
            this->laserCount = 0;
            if (this->actor.xzDistToPlayer > this->minLaserRange) {
                EnEgol_SetupLaser(this);
                return;
            }
        }
    }
    if ((this->actor.world.pos.y - 50.0f) <= player->actor.world.pos.y) {
        if (Animation_OnFrame(&this->skelAnime, 24.0f)) {
            Math_Vec3f_Copy(&spawnPos, &this->rightFootPos);
            spawnPos.y = this->actor.floorHeight + 2.0f;
            EnEgol_SpawnEffect(this, &spawnPos, &gZeroVec3s, 100, 0.005f, EYEGORE_EFFECT_IMPACT);
        }
        if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
            Math_Vec3f_Copy(&spawnPos, &this->leftFootPos);
            spawnPos.y = this->actor.floorHeight + 2.0f;
            EnEgol_SpawnEffect(this, &spawnPos, &gZeroVec3s, 100, 0.005f, EYEGORE_EFFECT_IMPACT);
        }
    }
    if ((ABS_ALT(angleToFacing) < 0x3000) && (this->actor.xzDistToPlayer < 100.0f)) {
        EnEgol_SetupSlam(this);
    } else {
        s16 angleBehind;

        if (!((this->skelAnime.curFrame == 0.0f) ||
              ((9.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 15.0f)) ||
              ((24.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 29.0f)))) {
            Math_ApproachF(&this->actor.world.pos.x, this->waypointPos.x, 0.5f,
                           fabsf(Math_SinS(this->actor.world.rot.y) * 4.0f));
            Math_ApproachF(&this->actor.world.pos.z, this->waypointPos.z, 0.5f,
                           fabsf(Math_CosS(this->actor.world.rot.y) * 4.0f));
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos), 1,
                           0x7D0, 0);
        EnEgol_FootstepEffects(this, play, 9.0f, 24.0f);
        angleBehind = this->actor.world.rot.y - this->actor.yawTowardsPlayer + 0x8000;
        if (ABS_ALT(angleBehind) < 0x2000) {
            this->waypoint--;
            if (this->waypoint < 0) {
                this->waypoint = 0;
            }
            EnEgol_SetupStop(this);
        } else {
            f32 dx = this->actor.world.pos.x - this->waypointPos.x;
            f32 dz = this->actor.world.pos.z - this->waypointPos.z;

            if (sqrtf(SQ(dx) + SQ(dz)) < 4.0f) {
                if (this->path != NULL) {
                    this->waypoint++;
                    if (this->waypoint >= this->path->count) {
                        this->waypoint -= 2;
                        if (this->waypoint < 0) {
                            this->waypoint = 0;
                        }
                        EnEgol_SetupStop(this);
                    }
                    EnEgol_GetWaypoint(this);
                }
            }
            if (this->actor.wallBgId != BGCHECK_SCENE) {
                EnEgol_DestroyBlocks(this, play, this->rightFootPos, this->leftFootPos);
            }
        }
    }
}

void EnEgol_SetupRetreat(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_RETREAT);
    this->laserCount = 0;
    EnEgol_GetWaypoint(this);
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
    this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    this->action = EYEGORE_ACTION_RETREAT;
    this->actionFunc = EnEgol_Retreat;
}

void EnEgol_Retreat(EnEgol* this, PlayState* play) {
    s16 angleToFacing = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
    Vec3f toWaypoint;
    Vec3f spawnPos;
    Player* player = GET_PLAYER(play);

    if ((ABS_ALT(angleToFacing) < 0x3000) && (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 50.0f) &&
        (this->actor.xzDistToPlayer < 100.0f) && (player->invincibilityTimer == 0)) {
        func_800B8D50(play, &this->actor, 2.0f, (s32)Rand_CenteredFloat(0x2000) + this->actor.world.rot.y, 5.0f, 0x10);
    }
    Math_Vec3f_Copy(&spawnPos, &gZeroVec3f);
    if ((this->actor.world.pos.y - 50.0f) <= player->actor.world.pos.y) {
        if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
            Math_Vec3f_Copy(&spawnPos, &this->rightFootPos);
            spawnPos.y = this->actor.floorHeight + 2.0f;
            EnEgol_SpawnEffect(this, &spawnPos, &gZeroVec3s, 100, 0.005f, EYEGORE_EFFECT_IMPACT);
        }
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            Math_Vec3f_Copy(&spawnPos, &this->leftFootPos);
            spawnPos.y = this->actor.floorHeight + 2.0f;
            EnEgol_SpawnEffect(this, &spawnPos, &gZeroVec3s, 100, 0.005f, EYEGORE_EFFECT_IMPACT);
        }
    }
    Math_ApproachF(&this->actor.world.pos.x, this->waypointPos.x, 0.5f,
                   fabsf(Math_SinS(this->actor.world.rot.y) * 10.0f));
    Math_ApproachF(&this->actor.world.pos.z, this->waypointPos.z, 0.5f,
                   fabsf(Math_CosS(this->actor.world.rot.y) * 10.0f));
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos), 1, 0x7D0,
                       0);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y + 0x8000, 1, 0x7D0, 0);
    EnEgol_FootstepEffects(this, play, 5.0f, 0.0f);
    toWaypoint.x = this->actor.world.pos.x - this->waypointPos.x;
    toWaypoint.z = this->actor.world.pos.z - this->waypointPos.z;
    if (sqrtf(SQXZ(toWaypoint)) < 4.0f) {
        this->waypoint--;
        if (this->waypoint < 0) {
            this->waypoint = 0;
            EnEgol_SetupStop(this);
        }
        EnEgol_GetWaypoint(this);
    }
}

void EnEgol_SetupLaser(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_LASER);
    this->actionTimer = 0;
    this->laserCount = 0;
    this->action = EYEGORE_ACTION_LASER;
    this->actionFunc = EnEgol_Laser;
}

void EnEgol_Laser(EnEgol* this, PlayState* play) {
    static s16 sLaserAngles[3] = { 0x1F40, 0xBB8, 0x7D0 };
    s32 pad1;
    Vec3f stonePos;
    s32 pad2;
    CollisionPoly* colPoly;
    Vec3f hitPos;
    s32 bgId;

    if ((ABS_ALT((s16)(this->actor.world.rot.y - this->actor.yawTowardsPlayer)) < 0x3000) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        this->chargingLaser = false;
        this->chargeLevel = 0;
        this->actionTimer = 0;
        this->laserState = EYEGORE_LASER_OFF;
        Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
        EnEgol_SetupSlam(this);
    } else if (this->actor.xzDistToPlayer < this->minLaserRange) {
        this->chargingLaser = false;
        this->chargeLevel = 0;
        this->actionTimer = 0;
        this->laserState = EYEGORE_LASER_OFF;
        Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
        EnEgol_SetupWalk(this);
    } else {
        if (this->chargingLaser) {
            switch (this->chargeLevel) {
                case 0:
                    this->waitTimer = 10;
                    this->chargeLevel++;
                    break;

                case 1:
                    Math_ApproachF(&this->chargeLightScale, 8.0f, 0.5f, 2.5f);
                    if (this->waitTimer == 0) {
                        this->waitTimer = 10;
                        this->chargeLevel++;
                    }
                    break;

                case 2:
                    Math_ApproachF(&this->chargeLightScale, 1.0f, 0.5f, 1.0f);
                    if (this->waitTimer == 0) {
                        this->chargeLevel = 0;
                        this->laserState = EYEGORE_LASER_FIRE;
                        this->chargingLaser = false;
                        this->chargeLightScale = 0.0f;
                    }
                    break;

                default:
                    break;
            }
            this->chargeLightRot += 0x7D0;
        }
        if ((this->laserState != EYEGORE_LASER_OFF) || (this->laserCount != 0)) {
            Math_SmoothStepToS(&this->headRot, -0x2710, 5, 0x1F4, 5);
        } else {
            Math_SmoothStepToS(&this->headRot, 0, 5, 0x1F4, 5);
        }
        if (this->laserState == EYEGORE_LASER_OFF) {
            if (this->laserCount >= 3) {
                EnEgol_SetupWalk(this);
            } else {
                this->laserRot.x = sLaserAngles[this->laserCount];
                this->laserScaleTarget.z = 0.0f;
                this->laserScale.z = 0.0f;
                if (this->laserCount == 0) {
                    this->laserState = EYEGORE_LASER_START;
                } else {
                    this->laserState = EYEGORE_LASER_FIRE;
                    this->laserScaleTarget.x = 0.03f;
                    this->laserScaleTarget.y = 0.03f;
                }
                this->laserCount++;
            }
        } else if (this->laserState >= EYEGORE_LASER_FIRE) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_BEAM - SFX_FLAG);
            if (this->laserState != EYEGORE_LASER_OFF) {
                EnEgol_DestroyBlocks(this, play, this->laserCollider.dim.quad[0], this->laserCollider.dim.quad[1]);
            }
            //! @bug this should check walls, too
            if ((this->actionTimer == 0) && BgCheck_EntityLineTest1(&play->colCtx, &this->laserCollider.dim.quad[3],
                                                                    &this->laserCollider.dim.quad[1], &hitPos, &colPoly,
                                                                    false, true, false, true, &bgId)) {
                Vec3s rotToNorm;
                f32 nx;
                f32 ny;
                f32 nz;
                s32 pad3;
                s32 i;
                s32 quakeYOffset;
                Player* player = GET_PLAYER(play);

                nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
                ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
                nz = COLPOLY_GET_NORMAL(colPoly->normal.z);

                /*! @bug The following is supposed to calculate the rotation from vertical to the collision poly normal.
                 * However, the calculation is performed incorrectly. The correct calculation is
                 * rotToNorm.x = Math_FAtan2F(nz, ny) * 0x8000 / M_PI;
                 * rotToNorm.z = Math_FAtan2F(-nx, sqrtf(1.0f - SQ(nx))) * 0x8000 / M_PI;
                 */
                rotToNorm.x = RAD_TO_BINANG_ALT2(-Math_FAtan2F(-nz * ny, 1.0f));
                rotToNorm.z = RAD_TO_BINANG_ALT2(Math_FAtan2F(-nx * ny, 1.0f));

                if ((this->actor.world.pos.y - 50.0f) <= player->actor.world.pos.y) {
                    EnEgol_SpawnEffect(this, &hitPos, &rotToNorm, 100, 0.02f, EYEGORE_EFFECT_IMPACT);
                }
                quakeYOffset = 4 - (s32)(fabsf(player->actor.world.pos.y - this->actor.world.pos.y) * 0.02f);
                if (quakeYOffset > 4) {
                    quakeYOffset = 4;
                } else if (quakeYOffset < 1) {
                    quakeYOffset = 1;
                }
                if (player->stateFlags3 != PLAYER_STATE3_1000000) {
                    Actor_RequestQuakeAndRumble(&this->actor, play, quakeYOffset, 2);
                }
                Actor_PlaySfx(&this->actor, NA_SE_EV_EXPLOSION);
                func_800B31BC(play, &hitPos, 40, -2, 255, 20);
                func_800BBFB0(play, &hitPos, 6.0f, 2, 120, 20, true);

                if ((this->actor.world.pos.y - 50.0f <= player->actor.world.pos.y) &&
                    (this->actor.floorBgId == BGCHECK_SCENE)) {
                    Math_Vec3f_Copy(&stonePos, &hitPos);
                    stonePos.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
                    stonePos.z += Math_CosS(this->actor.world.rot.y) * 60.0f;
                    for (i = 0; i < 3; i++) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ESTONE, stonePos.x, stonePos.y, stonePos.z, 0,
                                    this->actor.world.rot.y, 0, ENESTONE_TYPE_SMALL);
                    }
                }
                if (this->actor.world.pos.y - 50.0f <= player->actor.world.pos.y) {
                    for (i = 0; i < 10; i++) {
                        EnEgol_SpawnEffect(this, &hitPos, &gZeroVec3s, 30, (Rand_ZeroFloat(1.0f) * 0.1f) + 0.2f,
                                           EYEGORE_EFFECT_DEBRIS);
                    }
                }
                this->actionTimer = 1;
            }
            if (this->actionTimer != 0) {
                this->actionTimer++;
                if (this->actionTimer >= 5) {
                    this->laserState = EYEGORE_LASER_OFF;
                    Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
                    Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
                    this->actionTimer = 0;
                }
            }
            Math_ApproachF(&this->laserScale.x, this->laserScaleTarget.x, 0.5f, 0.5f);
            Math_ApproachF(&this->laserScale.y, this->laserScaleTarget.y, 0.5f, 0.5f);
            Math_ApproachF(&this->laserScale.z, this->laserScaleTarget.z, 0.5f, 0.5f);
        }
    }
}

void EnEgol_SetupStop(EnEgol* this) {
    if (!this->isRetreating) {
        EnEgol_ChangeAnim(this, EYEGORE_ANIM_LASER);
    } else {
        EnEgol_ChangeAnim(this, EYEGORE_ANIM_STUN_END);
    }
    this->action = EYEGORE_ACTION_STOP;
    this->actionFunc = EnEgol_Stop;
}

void EnEgol_Stop(EnEgol* this, PlayState* play) {
    s32 isHome = false;
    f32 curFrame = this->skelAnime.curFrame;
    s16 angleToHome;

    EnEgol_FootstepEffects(this, play, 28.0f, 13.0f);
    if (this->isRetreating) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 5, 0xBB8, 5);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        angleToHome = this->actor.world.rot.y - this->actor.home.rot.y;
    }
    if (!this->isRetreating) {
        isHome = true;
    } else if (ABS_ALT(angleToHome) < 0x64) {
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.home.rot.y;
        isHome = true;
    }
    if (isHome && (curFrame >= this->animEndFrame)) {
        this->isRetreating ^= true;
        if (!this->isRetreating) {
            EnEgol_ChangeAnim(this, EYEGORE_ANIM_SIT);
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_SIT);
            this->laserCount = 0;
            this->actionFunc = EnEgol_Wait;
        } else {
            EnEgol_SetupRetreat(this);
        }
    }
}

void EnEgol_SetupSlam(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_SLAM);
    this->action = EYEGORE_ACTION_SLAM;
    this->actionFunc = EnEgol_Slam;
}

void EnEgol_Slam(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->headRot, 0, 1, 0x3E8, 0);
    if (Animation_OnFrame(&this->skelAnime, 17.0f)) {
        s32 i;
        s32 spawnCount;
        Player* player = GET_PLAYER(play);
        Vec3f spawnPos;
        f32 dyToPlayer = fabsf(player->actor.world.pos.y - this->actor.world.pos.y);
        s32 quakeYOffset;

        Actor_PlaySfx(&this->actor, NA_SE_EV_EXPLOSION);
        quakeYOffset = 4 - (s32)(dyToPlayer * 0.02f);
        if (quakeYOffset > 4) {
            quakeYOffset = 4;
        } else if (quakeYOffset < 1) {
            quakeYOffset = 1;
        }
        Actor_RequestQuakeAndRumble(&this->actor, play, quakeYOffset, 2);
        if (this->actor.floorBgId == BGCHECK_SCENE) {
            Math_Vec3f_Copy(&spawnPos, &this->actor.world.pos);
            spawnPos.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
            spawnPos.y = this->actor.floorHeight;
            spawnPos.z += Math_CosS(this->actor.world.rot.y) * 60.0f;
            spawnCount = Rand_S16Offset(1, 3);
            for (i = 0; i < spawnCount; i++) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ESTONE, spawnPos.x, spawnPos.y, spawnPos.z, 0,
                            this->actor.world.rot.y, 0, ENESTONE_TYPE_LARGE);
            }
            Actor_SpawnFloorDustRing(play, &this->actor, &spawnPos, 30.0f, 30, 10.0f, 100, 30, true);
            Math_Vec3f_Copy(&spawnPos, &this->actor.world.pos);
            spawnPos.x += Math_SinS(this->actor.world.rot.y) * 55.0f;
            spawnPos.y = this->actor.floorHeight + 2.0f;
            spawnPos.z += Math_CosS(this->actor.world.rot.y) * 55.0f;
            EnEgol_SpawnEffect(this, &spawnPos, &gZeroVec3s, 100, 0.03f, EYEGORE_EFFECT_IMPACT);
        }
        EnEgol_DestroyBlocks(this, play, this->rightHandPos, this->leftHandPos);
    }
    if (curFrame >= this->animEndFrame) {
        EnEgol_SetupSlamWait(this);
    } else if ((this->skelAnime.curFrame <= 17.0f) && (this->skelAnime.curFrame >= 10.0f)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
    }
}

void EnEgol_SetupPunch(EnEgol* this) {
    this->bodyCollider.elements[0].dim.modelSphere.radius = 40;
    this->bodyCollider.elements[1].dim.modelSphere.radius = 40;
    this->hitPlayer = false;
    this->action = EYEGORE_ACTION_PUNCH;
    this->actionFunc = EnEgol_Punch;
}

void EnEgol_Punch(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->headRot, 0, 1, 0x3E8, 0);
    if (curFrame >= this->animEndFrame) {
        this->bodyCollider.elements[0].dim.modelSphere.radius = 20;
        this->bodyCollider.elements[1].dim.modelSphere.radius = 20;
        EnEgol_SetupWalk(this);
    } else if (!this->hitPlayer) {
        if ((this->bodyCollider.elements[0].base.atElemFlags & ATELEM_HIT) ||
            (this->bodyCollider.elements[1].base.atElemFlags & ATELEM_HIT)) {
            this->hitPlayer = true;
            func_800B8D50(play, &this->actor, 10.0f, this->actor.home.rot.y, 10.0f, 0);
        }
        if (!(this->bodyCollider.base.atFlags & AT_BOUNCED)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
        }
    }
}

void EnEgol_SetupSlamWait(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_SLAM_WAIT);
    this->actionTimer = 0;
    this->eyeCollider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK1;
    this->action = EYEGORE_ACTION_SLAM_WAIT;
    this->actionFunc = EnEgol_SlamWait;
}

void EnEgol_SlamWait(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        this->actionTimer++;
        if (this->actionTimer > 20) {
            this->eyeCollider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK2;
            EnEgol_SetupSlamEnd(this);
        }
    }
}

void EnEgol_SetupStunned(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_STUNNED);
    this->actionTimer = 0;
    this->eyeCollider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK1;
    this->bodyCollider.elements[0].dim.modelSphere.radius = 0;
    this->bodyCollider.elements[1].dim.modelSphere.radius = 0;
    this->action = EYEGORE_ACTION_STUNNED;
    this->actionFunc = EnEgol_Stunned;
}

void EnEgol_Stunned(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        this->actionTimer++;
        if (this->actionTimer > 80) {
            this->eyeCollider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK2;
            EnEgol_SetupStunEnd(this);
        }
    }
}

void EnEgol_SetupStunEnd(EnEgol* this) {
    this->bodyCollider.elements[0].dim.modelSphere.radius = 20;
    this->bodyCollider.elements[1].dim.modelSphere.radius = 20;
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_STUN_END);
    this->action = EYEGORE_ACTION_STUN_END;
    this->actionFunc = EnEgol_StunEnd;
}

void EnEgol_StunEnd(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnEgol_SetupWalk(this);
    }
}

void EnEgol_SetupSlamEnd(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_SLAM_END);
    this->actionTimer = 0;
    this->action = EYEGORE_ACTION_SLAM_END;
    this->actionFunc = EnEgol_SlamEnd;
}

void EnEgol_SlamEnd(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnEgol_SetupWalk(this);
    }
}

void EnEgol_SetupDamaged(EnEgol* this) {
    EnEgol_ChangeAnim(this, EYEGORE_ANIM_DAMAGED);
    this->bodyCollider.elements[0].dim.modelSphere.radius = 20;
    this->bodyCollider.elements[1].dim.modelSphere.radius = 20;
    this->action = EYEGORE_ACTION_DAMAGED;
    this->actionFunc = EnEgol_Damaged;
}

void EnEgol_Damaged(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        if (this->actor.colChkInfo.health > 0) {
            EnEgol_SetupWalk(this);
        } else {
            Enemy_StartFinishingBlow(play, &this->actor);
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DEAD);
            this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
            this->actionFunc = EnEgol_StartDeathCutscene;
        }
    }
}

void EnEgol_StartDeathCutscene(EnEgol* this, PlayState* play) {
    if (!CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_Queue(this->actor.csId);
    } else {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        this->subCamFovTarget = 60.0f;
        EnEgol_ChangeAnim(this, EYEGORE_ANIM_DEATH);
        this->action = EYEGORE_ACTION_DYING;
        this->actionFunc = EnEgol_Death;
        this->actor.shape.shadowScale = 0.0f;
    }
}

void EnEgol_Death(EnEgol* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f atOffset;

    this->subCamAt.x = 0.0f;
    this->subCamAt.y = 60.0f;
    this->subCamAt.z = 260.0f;
    Math_Vec3f_Copy(&atOffset, &this->subCamAt);
    OLib_Vec3fAdd(&this->actor.world, &atOffset, &this->subCamAt, OLIB_ADD_OFFSET);
    this->subCamEye.x = this->actor.world.pos.x;
    this->subCamEye.y = this->actor.world.pos.y + 70.0f;
    this->subCamEye.z = this->actor.world.pos.z;
    Math_ApproachF(&this->subCamFov, this->subCamFovTarget, 0.3f, 10.0f);
    Play_SetCameraAtEye(play, this->subCamId, &this->subCamEye, &this->subCamAt);
    Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    if ((this->action == EYEGORE_ACTION_DEAD) && (this->waitTimer == 1)) {
        if (this->switchFlag > SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        CutsceneManager_Stop(this->actor.csId);
        Actor_Kill(&this->actor);
    } else {
        if (Animation_OnFrame(&this->skelAnime, 46.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_EXPLOSION);
            Actor_RequestQuakeAndRumble(&this->actor, play, 10, 5);
        }
        if ((curFrame >= this->animEndFrame) && (this->action != EYEGORE_ACTION_DEAD)) {
            s32 i;

            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_EXPLOSION));
            this->waitTimer = 30;
            Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
            this->action = EYEGORE_ACTION_DEAD;
            for (i = 0; i < 20; i++) {
                EnEgol_SpawnEffect(this, &this->actor.world.pos, &gZeroVec3s, 10.0f + Rand_ZeroFloat(20.0f),
                                   0.01f + (0.02f * Rand_ZeroFloat(1.0f)), (i & 1) + EYEGORE_EFFECT_PIECE_LARGE);
            }
        }
    }
}

typedef enum {
    /* 0 */ EYEGORE_HIT_NONE,
    /* 1 */ EYEGORE_HIT_DAMAGE,
    /* 2 */ EYEGORE_HIT_IMMUNE // Makes hitmarks, but no reaction
} EyegoreHitReaction;

void EnEgol_CollisionCheck(EnEgol* this, PlayState* play) {
    s16 angleToFacing = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
    s32 reaction = EYEGORE_HIT_NONE;

    if ((this->action == EYEGORE_ACTION_LASER) && (this->laserCollider.base.atFlags & AT_BOUNCED)) {
        this->laserCollider.base.atFlags &= ~AT_HIT;
        this->laserCollider.base.atFlags &= ~AT_BOUNCED;
        this->actionTimer = 0;
        this->laserState = EYEGORE_LASER_OFF;
        Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
        Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
        EnEgol_SetupWalk(this);
    }
    if (this->eyeCollider.elements[0].base.acElemFlags & ACELEM_HIT) {
        reaction = EYEGORE_HIT_IMMUNE;
        switch (this->actor.colChkInfo.damageEffect) {
            case EYEGORE_DMGEFF_LIGHT_ARROW:
                if (ABS_ALT(angleToFacing) < 0x3000) {
                    if ((this->action == EYEGORE_ACTION_STUNNED) || (this->action == EYEGORE_ACTION_SLAM_WAIT)) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                    this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        this->dmgEffectTimer = 20;
                        reaction = EYEGORE_HIT_DAMAGE;
                    } else if ((this->action >= EYEGORE_ACTION_WALK) && (this->action <= EYEGORE_ACTION_STUN_END) &&
                               !this->isRetreating) {
                        reaction = EYEGORE_HIT_NONE;
                        this->chargingLaser = false;
                        this->chargeLevel = 0;
                        this->actionTimer = 0;
                        this->laserState = EYEGORE_LASER_OFF;
                        Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
                        Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
                        Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
                        Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
                        Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
                        Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA,
                                             40);
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        EnEgol_SetupStunned(this);
                    }
                }
                break;

            case EYEGORE_DMGEFF_NONE:
                if ((this->action == EYEGORE_ACTION_SLAM_WAIT) || (this->action == EYEGORE_ACTION_STUNNED)) {
                    reaction = EYEGORE_HIT_DAMAGE;
                }
                break;

            default:
                break;
        }
    }
    if (reaction == EYEGORE_HIT_DAMAGE) {
        Actor_ApplyDamage(&this->actor);
        CollisionCheck_BlueBlood(play, NULL, &this->eyePos);
        CollisionCheck_BlueBlood(play, NULL, &this->eyePos);
        CollisionCheck_BlueBlood(play, NULL, &this->eyePos);
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 25);
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DAMAGE);
        EnEgol_SetupDamaged(this);
    } else if (reaction == EYEGORE_HIT_IMMUNE) {
        Vec3f hitPos;

        hitPos.x = this->eyeCollider.elements[0].base.acDmgInfo.hitPos.x;
        hitPos.y = this->eyeCollider.elements[0].base.acDmgInfo.hitPos.y;
        hitPos.z = this->eyeCollider.elements[0].base.acDmgInfo.hitPos.z;
        Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
        EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &hitPos);
        CollisionCheck_SpawnShieldParticlesMetal(play, &hitPos);
    }
}

void EnEgol_Update(Actor* thisx, PlayState* play) {
    EnEgol* this = (EnEgol*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (!((this->action == EYEGORE_ACTION_PUNCH) || (this->action == EYEGORE_ACTION_RETREAT) ||
          (this->action >= EYEGORE_ACTION_DYING))) {
        s16 angleToFacing = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;
        s16 angleBehind = this->actor.world.rot.y - this->actor.yawTowardsPlayer + 0x8000;

        if ((ABS_ALT(angleToFacing) > 0x1888) && (ABS_ALT(angleBehind) > 0x2000) &&
            (this->actor.xzDistToPlayer < 100.0f)) {
            if (angleToFacing < 0) {
                if (this->animIndex != EYEGORE_ANIM_LEFT_PUNCH) {
                    EnEgol_ChangeAnim(this, EYEGORE_ANIM_LEFT_PUNCH);
                }
            } else if (this->animIndex != EYEGORE_ANIM_RIGHT_PUNCH) {
                EnEgol_ChangeAnim(this, EYEGORE_ANIM_RIGHT_PUNCH);
            }
            this->chargingLaser = false;
            this->chargeLevel = 0;
            this->actionTimer = 0;
            this->laserState = EYEGORE_LASER_OFF;
            Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
            Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
            Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
            Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
            Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
            Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
            EnEgol_SetupPunch(this);
        }
    }
    if (this->action != EYEGORE_ACTION_WAIT) {
        SkelAnime_Update(&this->skelAnime);
    }
    if (this->laserState == EYEGORE_LASER_OFF) {
        Math_ApproachZeroF(&this->laserLightScale, 0.5f, 0.5f);
    } else if (this->laserState >= EYEGORE_LASER_FIRE) {
        Math_ApproachF(&this->laserLightScale, 1.0f, 0.5f, 0.5f);
    }
    Actor_SetScale(&this->actor, 0.015f);

    if (!((this->action == EYEGORE_ACTION_STOP) || (this->action == EYEGORE_ACTION_RETREAT))) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
    this->texScroll += 12;
    DECR(this->waitTimer);
    DECR(this->eyeShutTimer);
    DECR(this->dmgEffectTimer);

    EnEgol_UpdateEffects(this, play);
    this->actionFunc(this, play);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->eyePos);
    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
    if (!((this->action == EYEGORE_ACTION_SLAM_WAIT) || (this->action == EYEGORE_ACTION_WAIT) ||
          (this->action == EYEGORE_ACTION_RETREAT) || (this->action == EYEGORE_ACTION_STAND) ||
          (this->action == EYEGORE_ACTION_LASER) || (this->action == EYEGORE_ACTION_STUNNED) ||
          (this->action == EYEGORE_ACTION_DAMAGED) || (this->action == EYEGORE_ACTION_DYING))) {

        this->pupilRot.x = Math_Vec3f_Pitch(&this->eyePos, &player->actor.world.pos);
        this->pupilRot.y = -Math_Vec3f_Yaw(&this->eyePos, &player->actor.world.pos);

        this->pupilRot.y += this->actor.world.rot.y;

        if (this->pupilRot.x > 0x7D0) {
            this->pupilRot.x = 0x7D0;
        } else if (this->pupilRot.x < -0x7D0) {
            this->pupilRot.x = -0x7D0;
        }
        if (this->pupilRot.y > 0x1770) {
            this->pupilRot.y = 0x1770;
        } else if (this->pupilRot.y < -0x1770) {
            this->pupilRot.y = -0x1770;
        }
    } else {
        this->pupilRot.x = this->pupilRot.y = 0;
    }
    if (this->eyeShutTimer == 0) {
        if (((this->action == EYEGORE_ACTION_WALK) || (this->action == EYEGORE_ACTION_LASER)) && !this->isRetreating) {
            Actor* projectile = func_800BC270(play, &this->actor, 80.0f, 0x138B0);
            // last argument is DMG_ZORA_BOOMERANG | DMG_HOOKSHOT | DMG_NORMAL_ARROW | DMG_FIRE_ARROW | DMG_ICE_ARROW |
            // DMG_LIGHT_ARROW | DMG_DEKU_BUBBLE

            if ((projectile != NULL) &&
                !((projectile->id == ACTOR_EN_ARROW) && (projectile->params == ARROW_TYPE_LIGHT))) {
                this->eyelidRotTarget = 0xFA0;
                this->eyeShutTimer = 20;
                if (this->action == EYEGORE_ACTION_LASER) {
                    this->chargingLaser = false;
                    this->chargeLevel = 0;
                    this->actionTimer = 0;
                    this->laserState = EYEGORE_LASER_OFF;
                    Math_Vec3f_Copy(&this->laserScale, &gZeroVec3f);
                    Math_Vec3f_Copy(&this->laserScaleTarget, &gZeroVec3f);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[1], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[3], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[0], &this->laserBase);
                    Math_Vec3f_Copy(&this->laserCollider.dim.quad[2], &this->laserBase);
                    EnEgol_SetupWalk(this);
                }
            }
        } else {
            this->eyelidRotTarget = 0;
        }
    } else if (this->eyeShutTimer == 1) {
        this->eyelidRotTarget = 0;
    }
    Math_SmoothStepToS(&this->eyelidRot, this->eyelidRotTarget, 1, 0x7D0, 0);
    EnEgol_CollisionCheck(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 50.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    if (this->action != EYEGORE_ACTION_DEAD) {
        //! @bug This should be ||, not &&. As is, the check always succeeds.
        if (!((this->action == EYEGORE_ACTION_DAMAGED) && (this->action == EYEGORE_ACTION_DYING))) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->eyeCollider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    }
    if (this->laserState >= EYEGORE_LASER_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->laserCollider.base);
    }
}

s32 EnEgol_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnEgol* this = (EnEgol*)thisx;

    if (limbIndex == EYEGORE_LIMB_HEAD) {
        rot->z += this->headRot;
    }
    if (limbIndex == EYEGORE_LIMB_UPPER_EYELID) {
        rot->z += this->eyelidRot;
    }
    if (limbIndex == EYEGORE_LIMB_LOWER_EYELID) {
        rot->z += -1 * this->eyelidRot;
    }
    if (limbIndex == EYEGORE_LIMB_PUPIL) {
        rot->y += this->pupilRot.y;
        rot->z += this->pupilRot.x;
    }
    if (limbIndex == EYEGORE_LIMB_EYEBALL) {
        s32 eyeColorIndex = 0;

        OPEN_DISPS(play->state.gfxCtx);

        if ((this->action == EYEGORE_ACTION_DAMAGED) || (this->action == EYEGORE_ACTION_DYING)) {
            eyeColorIndex = 2;
        } else if ((this->action == EYEGORE_ACTION_STUNNED) || (this->action == EYEGORE_ACTION_SLAM_WAIT)) {
            eyeColorIndex = 1;
        }
        this->eyePrimColor.r = sEyePrimColors[eyeColorIndex].r;
        this->eyePrimColor.g = sEyePrimColors[eyeColorIndex].g;
        this->eyePrimColor.b = sEyePrimColors[eyeColorIndex].b;
        this->eyeEnvColor.r = sEyeEnvColors[eyeColorIndex].r;
        this->eyeEnvColor.g = sEyeEnvColors[eyeColorIndex].g;
        this->eyeEnvColor.b = sEyeEnvColors[eyeColorIndex].b;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0xFF, (u8)this->eyePrimColor.r, (u8)this->eyePrimColor.g,
                        (u8)this->eyePrimColor.b, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, (u8)this->eyeEnvColor.r, (u8)this->eyeEnvColor.g, (u8)this->eyeEnvColor.b, 255);
        gSPSegment(POLY_OPA_DISP++, 0x09, func_8012CB28(play->state.gfxCtx, 0, this->texScroll));

        CLOSE_DISPS(play->state.gfxCtx);
    }
    return false;
}

void EnEgol_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnEgol* this = (EnEgol*)thisx;
    Vec3f footOffset = { 1000.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == EYEGORE_LIMB_LEFT_HAND) {
        Matrix_MultVec3f(&gZeroVec3f, &this->leftHandPos);
    }
    if (limbIndex == EYEGORE_LIMB_RIGHT_HAND) {
        Matrix_MultVec3f(&gZeroVec3f, &this->rightHandPos);
    }
    if (limbIndex == EYEGORE_LIMB_LASER_ATTACH) {
        Matrix_MultVec3f(&gZeroVec3f, &this->eyePos);
    }
    if ((limbIndex == EYEGORE_LIMB_LASER_ATTACH) && (this->laserState >= EYEGORE_LASER_FIRE)) {
        Vec3f laserLengthZ = { 0.0f, 0.0f, 0.0f };
        Vec3f laserVec;
        f32 targetScale;
        f32 targetHeight;

        Matrix_MultVec3f(&zeroVec, &this->laserBase);
        this->laserCollider.dim.quad[3].x =
            this->laserBase.x + Math_SinS(this->actor.world.rot.y + 0x4000) * this->laserScale.x * 77.0f;
        this->laserCollider.dim.quad[3].y = this->laserBase.y;
        this->laserCollider.dim.quad[3].z =
            this->laserBase.z + Math_CosS(this->actor.world.rot.y + 0x4000) * this->laserScale.x * 77.0f;
        this->laserCollider.dim.quad[2].x =
            this->laserBase.x + Math_SinS(this->actor.world.rot.y + 0xC000) * this->laserScale.x * 77.0f;
        this->laserCollider.dim.quad[2].y = this->laserBase.y;
        this->laserCollider.dim.quad[2].z =
            this->laserBase.z + Math_CosS(this->actor.world.rot.y + 0xC000) * this->laserScale.x * 77.0f;

        targetScale = this->laserScale.z;
        targetHeight = this->laserCollider.dim.quad[3].y;

        while ((targetHeight + 20.0f > this->actor.floorHeight) && (targetScale < 10.0f)) {
            Matrix_Push();
            Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(this->laserRot.x, MTXMODE_APPLY);
            laserLengthZ.z = targetScale * 700.0f;
            Matrix_MultVec3f(&laserLengthZ, &laserVec);
            Matrix_Pop();
            targetHeight = this->laserCollider.dim.quad[3].y + laserVec.y;
            targetScale += 0.01f;
        }
        this->laserScaleTarget.z = targetScale;
        Matrix_Push();
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);
        Matrix_RotateXS(this->laserRot.x, MTXMODE_APPLY);
        laserLengthZ.z = this->laserScale.z * 700.0f;
        Matrix_MultVec3f(&laserLengthZ, &laserVec);
        Matrix_Pop();
        this->laserCollider.dim.quad[1].x = this->laserCollider.dim.quad[3].x + laserVec.x;
        this->laserCollider.dim.quad[1].y = this->laserCollider.dim.quad[3].y + laserVec.y;
        this->laserCollider.dim.quad[1].z = this->laserCollider.dim.quad[3].z + laserVec.z;
        this->laserCollider.dim.quad[0].x = this->laserCollider.dim.quad[2].x + laserVec.x;
        this->laserCollider.dim.quad[0].y = this->laserCollider.dim.quad[2].y + laserVec.y;
        this->laserCollider.dim.quad[0].z = this->laserCollider.dim.quad[2].z + laserVec.z;
        Collider_SetQuadVertices(&this->laserCollider, &this->laserCollider.dim.quad[0],
                                 &this->laserCollider.dim.quad[1], &this->laserCollider.dim.quad[2],
                                 &this->laserCollider.dim.quad[3]);
        this->laserState = EYEGORE_LASER_ON;
    }
    if ((limbIndex == EYEGORE_LIMB_LASER_ATTACH) && (this->laserState == EYEGORE_LASER_START)) {
        this->chargingLaser = true;
        this->laserState = EYEGORE_LASER_CHARGING;
        this->laserScaleTarget.x = 0.03f;
        this->laserScaleTarget.y = 0.03f;
    }
    if (limbIndex == EYEGORE_LIMB_LEFT_FOOT) {
        Matrix_MultVec3f(&footOffset, &this->leftFootPos);
    }
    if (limbIndex == EYEGORE_LIMB_RIGHT_FOOT) {
        Matrix_MultVec3f(&footOffset, &this->rightFootPos);
    }
    if ((limbIndex == EYEGORE_LIMB_HEAD) || (limbIndex == EYEGORE_LIMB_LEFT_SHOULDER) ||
        (limbIndex == EYEGORE_LIMB_LEFT_ARM) || (limbIndex == EYEGORE_LIMB_LEFT_HAND) ||
        (limbIndex == EYEGORE_LIMB_RIGHT_SHOULDER) || (limbIndex == EYEGORE_LIMB_RIGHT_ARM) ||
        (limbIndex == EYEGORE_LIMB_RIGHT_HAND) || (limbIndex == EYEGORE_LIMB_UPPER_EYELID) ||
        (limbIndex == EYEGORE_LIMB_LOWER_EYELID) || (limbIndex == EYEGORE_LIMB_HIPS) ||
        (limbIndex == EYEGORE_LIMB_LEFT_SHIN) || (limbIndex == EYEGORE_LIMB_RIGHT_SHIN)) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= EYEGORE_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }
    Collider_UpdateSpheres(limbIndex, &this->bodyCollider);
    Collider_UpdateSpheres(limbIndex, &this->eyeCollider);
}

void EnEgol_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnEgol* this = (EnEgol*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->action != EYEGORE_ACTION_DEAD) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gEyegoreEyeLaserTexAnim));
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnEgol_OverrideLimbDraw, EnEgol_PostLimbDraw, &this->actor);
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    }
    if (this->dmgEffectTimer != 0) {
        f32 drawDmgEffAlpha = 0.05f * this->dmgEffectTimer;

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EYEGORE_BODYPART_MAX, 0.8f, 0.8f,
                                drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);
    }
    if (this->laserState >= EYEGORE_LASER_FIRE) {
        this->laserRot.y = this->actor.world.rot.y;
        gSPSegment(POLY_OPA_DISP++, 0x08, func_8012CB28(play->state.gfxCtx, 0, this->texScroll));
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        Matrix_Translate(this->laserBase.x, this->laserBase.y, this->laserBase.z, MTXMODE_NEW);
        Matrix_RotateZYX(this->laserRot.x, this->laserRot.y, 0, MTXMODE_APPLY);
        Matrix_Scale(this->laserScale.x, this->laserScale.y, this->laserScale.z, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gEyegoreLaserDL);
    }
    if (((this->action == EYEGORE_ACTION_LASER) && (this->laserState >= EYEGORE_LASER_FIRE)) || this->chargingLaser) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
        if (!this->chargingLaser) {
            s32 i;
            f32 laserLightScaleMod;
            f32 laserLightAlpha;

            gDPSetEnvColor(POLY_XLU_DISP++, 155, 255, 255, 128);
            Matrix_Translate(this->eyePos.x, this->eyePos.y, this->eyePos.z, MTXMODE_NEW);
            Matrix_Scale(this->laserLightScale, this->laserLightScale, this->laserLightScale, MTXMODE_APPLY);

            laserLightScaleMod = 10.0f;
            laserLightAlpha = 80.0f;
            for (i = 0; i < ARRAY_COUNT(sLightOrbColors); i++) {
                FrameInterpolation_RecordOpenChild(&sLightOrbColors[i], i);
                Matrix_Push();
                Matrix_Scale(laserLightScaleMod, laserLightScaleMod, laserLightScaleMod, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, sLightOrbColors[i].r, sLightOrbColors[i].g, sLightOrbColors[i].b,
                                laserLightAlpha);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
                Matrix_Pop();
                laserLightScaleMod = 3.0f;
                laserLightAlpha = 200.0f;
                FrameInterpolation_RecordCloseChild();
            }
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 155, 255, 255, 128);
            Matrix_Translate(this->eyePos.x, this->eyePos.y, this->eyePos.z, MTXMODE_NEW);
            Matrix_Scale(this->chargeLightScale, this->chargeLightScale, this->chargeLightScale, MTXMODE_APPLY);
            Matrix_Push();
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateZS(this->chargeLightRot, MTXMODE_APPLY);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            Matrix_Pop();
        }
    }
    EnEgol_DrawEffects(this, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnEgol_SpawnEffect(EnEgol* this, Vec3f* pos, Vec3s* rot, s16 lifetime, f32 scale, s16 type) {
    EnEgolEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isActive) {
            effect->isActive = true;
            effect->scale = scale;
            effect->pos = *pos;
            effect->timer = lifetime;
            effect->alpha = 255;
            effect->type = type;
            effect->rot.x = Rand_CenteredFloat(0x7530);
            effect->rot.y = Rand_CenteredFloat(0x7530);
            effect->rot.z = Rand_CenteredFloat(0x7530);
            if ((effect->type == EYEGORE_EFFECT_PIECE_LARGE) || (effect->type == EYEGORE_EFFECT_PIECE_SMALL)) {
                effect->accel.y = -1.0f;
                effect->velocity.x = 4.0f * (Rand_ZeroOne() - 0.5f);
                effect->velocity.y = 10.0f + (10.0f * Rand_ZeroOne());
                effect->velocity.z = 4.0f * (Rand_ZeroOne() - 0.5f);
                break;
            } else if (effect->type == EYEGORE_EFFECT_DEBRIS) {
                effect->accel.y = -1.0f;
                effect->velocity.x = 0.5f * (Rand_ZeroOne() - 0.5f);
                effect->velocity.y = 5.0f + (5.0f * Rand_ZeroOne());
                effect->velocity.z = 0.5f * (Rand_ZeroOne() - 0.5f);
                effect->timer = 20.0f + Rand_ZeroFloat(10.0f);
                break;
            } else if (effect->type == EYEGORE_EFFECT_IMPACT) {
                effect->rot.x = rot->x;
                effect->rot.y = 0;
                effect->rot.z = rot->z;
                break;
            }
        }
    }
}

void EnEgol_UpdateEffects(EnEgol* this, PlayState* play) {
    EnEgolEffect* effect = this->effects;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            if (this->action == EYEGORE_ACTION_DYING) {
                effect->timer = 0;
            }
            if (effect->type != EYEGORE_EFFECT_IMPACT) {
                effect->rot.x += 0x100;
                effect->rot.z += 0x130;

                effect->pos.x += effect->velocity.x;
                effect->pos.y += effect->velocity.y;
                effect->pos.z += effect->velocity.z;

                effect->velocity.y += effect->accel.y;
            }
            if (effect->timer != 0) {
                effect->timer--;
            } else if (effect->type == EYEGORE_EFFECT_IMPACT) {
                effect->alpha -= 10;
                if (effect->alpha < 10) {
                    effect->isActive = false;
                }
            } else {
                effect->isActive = false;
            }
        }
    }
}

void EnEgol_DrawEffects(EnEgol* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnEgolEffect* effect = this->effects;
    s16 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            FrameInterpolation_RecordOpenChild(effect, effect->type);
            FrameInterpolation_IgnoreActorMtx();
            Matrix_Push();

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
            Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);
            Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            switch (effect->type) {
                case EYEGORE_EFFECT_IMPACT:
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 155, 155, 155, 255);
                    Matrix_Translate(0.0f, 50.0f, 0.0f, MTXMODE_APPLY);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, effect->alpha);
                    gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectImpactDL);
                    break;

                case EYEGORE_EFFECT_PIECE_LARGE:
                case EYEGORE_EFFECT_PIECE_SMALL:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, effect->alpha);
                    if (effect->type == EYEGORE_EFFECT_PIECE_LARGE) {
                        gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectLargeBodyPieceDL);
                    } else {
                        gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectSmallBodyPieceDL);
                    }
                    break;

                case EYEGORE_EFFECT_DEBRIS:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
                    gSPDisplayList(POLY_OPA_DISP++, gEyegoreEffectSolidDebrisDL);
                    break;

                default:
                    break;
                    FrameInterpolation_RecordCloseChild();
            }
            Matrix_Pop();
        }
    }

    CLOSE_DISPS(gfxCtx);
}
