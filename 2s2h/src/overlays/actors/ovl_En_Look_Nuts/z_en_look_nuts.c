/*
 * File: z_en_look_nuts.c
 * Overlay: ovl_En_Look_Nuts
 * Description: Deku Palace - Patrolling Deku Guard
 */

#include "z_en_look_nuts.h"
#include "overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnLookNuts_Init(Actor* thisx, PlayState* play);
void EnLookNuts_Destroy(Actor* thisx, PlayState* play);
void EnLookNuts_Update(Actor* thisx, PlayState* play);
void EnLookNuts_Draw(Actor* thisx, PlayState* play);
void EnLookNuts_Reset(void);

void EnLookNuts_SetupPatrol(EnLookNuts* this);
void EnLookNuts_Patrol(EnLookNuts* this, PlayState* play);
void EnLookNuts_SetupStandAndWait(EnLookNuts* this);
void EnLookNuts_StandAndWait(EnLookNuts* this, PlayState* play);
void EnLookNuts_RunToPlayer(EnLookNuts* this, PlayState* play);
void EnLookNuts_SetupSendPlayerToSpawn(EnLookNuts* this);
void EnLookNuts_SendPlayerToSpawn(EnLookNuts* this, PlayState* play);

ActorProfile En_Look_Nuts_Profile = {
    /**/ ACTOR_EN_LOOK_NUTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNK,
    /**/ sizeof(EnLookNuts),
    /**/ EnLookNuts_Init,
    /**/ EnLookNuts_Destroy,
    /**/ EnLookNuts_Update,
    /**/ EnLookNuts_Draw,
    /**/ EnLookNuts_Reset,
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
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

s32 D_80A6862C = false;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0xF),
    /* Deku Stick     */ DMG_ENTRY(1, 0xF),
    /* Horse trample  */ DMG_ENTRY(1, 0xF),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xF),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0xF),
    /* Hookshot       */ DMG_ENTRY(1, 0xE),
    /* Goron punch    */ DMG_ENTRY(1, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(1, 0xF),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(1, 0xF),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0xF),
    /* Zora barrier   */ DMG_ENTRY(1, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(1, 0xF),
    /* Normal Roll    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0xF),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0xF),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

typedef enum {
    /* 0x01 */ PALACE_GUARD_PATROLLING,
    /* 0x01 */ PALACE_GUARD_WAITING,
    /* 0x02 */ PALACE_GUARD_RUNNING_TO_PLAYER,
    /* 0x03 */ PALACE_GUARD_CAUGHT_PLAYER
} PalaceGuardState;

void EnLookNuts_Init(Actor* thisx, PlayState* play) {
    EnLookNuts* this = (EnLookNuts*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuPalaceGuardSkel, &gDekuPalaceGuardDigAnim, this->jointTable,
                   this->morphTable, DEKU_PALACE_GUARD_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->pathIndex = LOOKNUTS_GET_PATH_INDEX(&this->actor);
    this->switchFlag = LOOKNUTS_GET_SWITCH_FLAG(&this->actor);
    this->spawnIndex = LOOKNUTS_GET_SPAWN_INDEX(&this->actor);

    if (this->switchFlag == LOOKNUTS_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (this->pathIndex == LOOKNUTS_PATH_INDEX_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->state = PALACE_GUARD_PATROLLING;
    EnLookNuts_SetupPatrol(this);
}

void EnLookNuts_Destroy(Actor* thisx, PlayState* play) {
    EnLookNuts* this = (EnLookNuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnLookNuts_SetupPatrol(EnLookNuts* this) {
    Animation_Change(&this->skelAnime, &gDekuPalaceGuardWalkAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuPalaceGuardWalkAnim), ANIMMODE_LOOP, -10.0f);
    this->state = PALACE_GUARD_PATROLLING;
    this->actionFunc = EnLookNuts_Patrol;
}

void EnLookNuts_Patrol(EnLookNuts* this, PlayState* play) {
    f32 sp34 = 0.0f;
    f32 sp30;

    SkelAnime_Update(&this->skelAnime);
    if (Play_InCsMode(play)) {
        this->actor.speed = 0.0f;
        return;
    }

    this->actor.speed = 2.0f;
    if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
    }

    if (D_80A6862C) {
        Math_ApproachZeroF(&this->actor.speed, 0.3f, 1.0f);
        return;
    }

    this->path = SubS_GetPathByIndex(play, this->pathIndex, LOOKNUTS_PATH_INDEX_NONE);
    if (this->path != NULL) {
        sp34 = SubS_GetDistSqAndOrientPath(this->path, this->waypointIndex, &this->actor.world.pos, &sp30);
    }

    //! @bug sp30 is uninitialised if path == NULL. Fix by enclosing everything in the path NULL check.
    if (sp30 < 10.0f) {
        if (this->path != NULL) {
            this->waypointIndex++;
            if (this->waypointIndex >= this->path->count) {
                this->waypointIndex = 0;
            }
            if (Rand_ZeroOne() < 0.6f) {
                EnLookNuts_SetupStandAndWait(this);
                return;
            }
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, sp34, 1, 0x1388, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnLookNuts_SetupStandAndWait(EnLookNuts* this) {
    Animation_Change(&this->skelAnime, &gDekuPalaceGuardWalkAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuPalaceGuardWalkAnim), ANIMMODE_ONCE, -10.0f);
    this->waitTimer = Rand_S16Offset(1, 3);
    this->headRotTarget.y = 10000.0f;

    if (Rand_ZeroOne() < 0.5f) {
        this->headRotTarget.y = -10000.0f;
    }
    this->eventTimer = 10;
    this->state = PALACE_GUARD_WAITING;
    this->actionFunc = EnLookNuts_StandAndWait;
}

// Patrol Guards will stand in place and wait for a maximum of ~12 frames.
void EnLookNuts_StandAndWait(EnLookNuts* this, PlayState* play) {
    s16 randOffset;

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 0.3f, 1.0f);
    if (!Play_InCsMode(play) && !D_80A6862C && (this->eventTimer == 0)) {
        this->eventTimer = 10;
        switch (this->waitTimer) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                this->waitTimer++;
                this->headRotTarget.y *= -1.0f;
                break;

            case 5:
                this->headRotTarget.y = 0.0f;
                randOffset = Rand_S16Offset(1, 2);
                this->eventTimer = 0;
                this->waitTimer += randOffset;
                break;

            case 6:
                if (fabsf(this->headRotTarget.y - this->headRot.y) < 10.0f) {
                    this->waitTimer = 10;
                    this->headRotTarget.x = 4000.0f;
                    this->eventTimer = 5;
                }
                break;

            case 7:
                if (fabsf(this->headRotTarget.y - this->headRot.y) < 10.0f) {
                    this->headRotTarget.z = 4000.0f;
                    this->waitTimer++;
                }
                break;

            case 8:
                this->waitTimer = 10;
                this->eventTimer = 20;
                this->headRotTarget.z = -8000.0f;
                break;

            case 10:
                Math_Vec3f_Copy(&this->headRotTarget, &gZeroVec3f);
                this->waitTimer = 11;
                break;

            case 11:
                if ((fabsf(this->headRot.x) < 30.0f) && (fabsf(this->headRot.y) < 30.0f) &&
                    (fabsf(this->headRot.z) < 30.0f)) {
                    this->waitTimer = 12;
                }
                break;

            default:
                break;
        }
        if (this->waitTimer == 12) {
            this->waitTimer = 0;
            EnLookNuts_SetupPatrol(this);
        }
    }
}

void EnLookNuts_DetectedPlayer(EnLookNuts* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gDekuPalaceGuardWalkAnim, 2.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuPalaceGuardWalkAnim), ANIMMODE_LOOP, -10.0f);
    this->state = PALACE_GUARD_RUNNING_TO_PLAYER;
    this->eventTimer = 300;
    Message_StartTextbox(play, 0x833, &this->actor);
    this->actionFunc = EnLookNuts_RunToPlayer;
}

void EnLookNuts_RunToPlayer(EnLookNuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
    }

    this->actor.speed = 4.0f;
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    if ((this->actor.xzDistToPlayer < 70.0f) || (this->eventTimer == 0)) {
        this->actor.speed = 0.0f;
        EnLookNuts_SetupSendPlayerToSpawn(this);
    }
}

void EnLookNuts_SetupSendPlayerToSpawn(EnLookNuts* this) {
    Animation_Change(&this->skelAnime, &gDekuPalaceGuardWalkAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuPalaceGuardWalkAnim), ANIMMODE_ONCE, -10.0f);
    this->state = PALACE_GUARD_CAUGHT_PLAYER;
    this->actionFunc = EnLookNuts_SendPlayerToSpawn;
}

void EnLookNuts_SendPlayerToSpawn(EnLookNuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        play->nextEntrance = Entrance_CreateFromSpawn(this->spawnIndex);
        gSaveContext.nextCutsceneIndex = 0;
        Scene_SetExitFade(play);
        play->transitionTrigger = TRANS_TRIGGER_START;
        SET_WEEKEVENTREG(WEEKEVENTREG_17_04);
    }
}

void EnLookNuts_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnLookNuts* this = (EnLookNuts*)thisx;

    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState >= 3) {
            this->eyeState = 0;
            this->blinkTimer = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }
    this->actionFunc(this, play);
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->eventTimer != 0) {
        this->eventTimer--;
    }
    Actor_MoveWithGravity(&this->actor);
    if (!D_80A6862C) {
        if ((this->state < 2) && (this->actor.xzDistToPlayer < 320.0f) && (this->actor.playerHeightRel < 80.0f)) {
            Vec3f effectVelocityOffset = { 0.0f, 0.0f, 0.0f };
            Vec3f effectPos;
            Vec3f effectVelocity;

            Math_Vec3f_Copy(&effectPos, &this->actor.world.pos);
            effectPos.x += Math_SinS(this->actor.world.rot.y + TRUNCF_BINANG(this->headRot.y)) * 10.0f;
            effectPos.y += 30.0f;
            effectPos.z += Math_CosS(this->actor.world.rot.y + TRUNCF_BINANG(this->headRot.y)) * 10.0f;
            Matrix_Push();
            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
            effectVelocityOffset.z = 20.0f;
            Matrix_MultVec3f(&effectVelocityOffset, &effectVelocity);
            Matrix_Pop();
            if (!this->isPlayerDetected) {
                s16 effectFlags = SOLDERSRCHBALL_INVISIBLE;

                if (GameInteractor_Should(VB_DEKU_GUARD_SHOW_SEARCH_BALLS, gSaveContext.save.isNight)) {
                    effectFlags = 0;
                }

                if (Player_GetMask(play) != PLAYER_MASK_STONE) {
                    EffectSsSolderSrchBall_Spawn(play, &effectPos, &effectVelocity, &gZeroVec3f, 50,
                                                 &this->isPlayerDetected, effectFlags);
                }
            }

            if ((this->isPlayerDetected == true) || (this->actor.xzDistToPlayer < 20.0f)) {
                Player* player = GET_PLAYER(play);

                if (!(player->stateFlags3 & PLAYER_STATE3_100) && !Play_InCsMode(play)) {
                    Math_Vec3f_Copy(&this->headRotTarget, &gZeroVec3f);
                    this->state = PALACE_GUARD_RUNNING_TO_PLAYER;
                    Audio_PlaySfx(NA_SE_SY_FOUND);
                    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_26);
                    D_80A6862C = true;
                    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED);
                    this->actor.gravity = 0.0f;
                    EnLookNuts_DetectedPlayer(this, play);
                } else {
                    this->isPlayerDetected = false;
                }
            }
        }
        Math_ApproachF(&this->headRot.x, this->headRotTarget.x, 1.0f, 3000.0f);
        Math_ApproachF(&this->headRot.y, this->headRotTarget.y, 1.0f, 6000.0f);
        Math_ApproachF(&this->headRot.z, this->headRotTarget.z, 1.0f, 2000.0f);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

static TexturePtr sEyeTextures[] = {
    gDekuPalaceGuardEyeOpenTex,
    gDekuPalaceGuardEyeHalfTex,
    gDekuPalaceGuardEyeClosedTex,
};

void EnLookNuts_Draw(Actor* thisx, PlayState* play) {
    EnLookNuts* this = (EnLookNuts*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeState]));
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnLookNuts_Reset(void) {
    D_80A6862C = 0;
}
