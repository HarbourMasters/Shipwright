/*
 * File: z_en_elforg.c
 * Overlay: ovl_En_Elforg
 * Description: Stray Fairy
 */

#include "z_en_elforg.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnElforg_Init(Actor* thisx, PlayState* play);
void EnElforg_Destroy(Actor* thisx, PlayState* play);
void EnElforg_Update(Actor* thisx, PlayState* play);
void EnElforg_Draw(Actor* thisx, PlayState* play);

void EnElforg_TrappedByBubble(EnElforg* this, PlayState* play);
void EnElforg_TurnInFairy(EnElforg* this, PlayState* play);
void EnElforg_FreeFloatingFairyFountain(EnElforg* this, PlayState* play);
void EnElforg_FreeFloating(EnElforg* this, PlayState* play);
void EnElforg_SetupTrappedByEnemy(EnElforg* this, PlayState* play);
void EnElforg_HiddenByCollider(EnElforg* this, PlayState* play);

ActorProfile En_Elforg_Profile = {
    /**/ ACTOR_EN_ELFORG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnElforg),
    /**/ EnElforg_Init,
    /**/ EnElforg_Destroy,
    /**/ EnElforg_Update,
    /**/ EnElforg_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 16, 32, 0, { 0, 0, 0 } },
};

void EnElforg_InitializeParams(EnElforg* this) {
    this->actor.speed = 1.0f;
    this->targetSpeedXZ = 1.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.world.rot.y = Rand_CenteredFloat(0x10000);
    this->timer = 0;
    this->secondaryTimer = Rand_ZeroFloat(100.0f);
    this->actor.shape.yOffset = 0.0f;
    this->skelAnime.curFrame = (s32)Rand_ZeroFloat(5.0f);
}

void EnElforg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnElforg* this = (EnElforg*)thisx;

    Actor_SetScale(thisx, 0.01f);
    this->strayFairyFlags = 0;
    this->direction = 0;
    SkelAnime_InitFlex(play, &this->skelAnime, &gStrayFairySkel, &gStrayFairyFlyingAnim, this->jointTable,
                       this->jointTable, STRAY_FAIRY_LIMB_MAX);
    this->skelAnime.playSpeed = 1.0f;
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    thisx->shape.shadowAlpha = 255;

    switch (STRAY_FAIRY_TYPE(thisx)) {
        case STRAY_FAIRY_TYPE_CLOCK_TOWN:
            if (GameInteractor_Should(VB_KILL_CLOCK_TOWN_STRAY_FAIRY, CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80), this)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case STRAY_FAIRY_TYPE_COLLECTIBLE:
            if (Flags_GetCollectible(play, STRAY_FAIRY_GET_FLAG(thisx))) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN:
        case STRAY_FAIRY_TYPE_BUBBLE:
        case STRAY_FAIRY_TYPE_CHEST:
        case STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN:
            break;

        default:
            if (Flags_GetSwitch(play, STRAY_FAIRY_GET_FLAG(thisx))) {
                Actor_Kill(thisx);
                return;
            }
            break;
    }

    if (Map_IsInDungeonOrBossScene(play)) {
        this->area = gSaveContext.dungeonSceneSharedIndex + STRAY_FAIRY_AREA_WOODFALL;
    } else {
        this->area = STRAY_FAIRY_GET_NON_DUNGEON_AREA(thisx);
    }

    switch (STRAY_FAIRY_TYPE(thisx)) {
        case STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN:
            EnElforg_InitializeParams(this);
            this->actionFunc = EnElforg_FreeFloatingFairyFountain;
            this->targetSpeedXZ = Rand_ZeroFloat(2.0f) + 1.0f;
            this->targetDistanceFromHome = Rand_ZeroFloat(100.0f) + 50.0f;
            break;

        case STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN:
            EnElforg_InitializeParams(this);
            this->actionFunc = EnElforg_TurnInFairy;
            this->secondaryTimer = 60;
            break;

        case STRAY_FAIRY_TYPE_BUBBLE:
            this->timer = 0;
            this->actionFunc = EnElforg_TrappedByBubble;
            break;

        case STRAY_FAIRY_TYPE_ENEMY:
            this->actionFunc = EnElforg_SetupTrappedByEnemy;
            EnElforg_SetupTrappedByEnemy(this, play);
            thisx->draw = NULL;
            break;

        case STRAY_FAIRY_TYPE_COLLIDER:
            this->actionFunc = EnElforg_HiddenByCollider;
            thisx->draw = NULL;
            Collider_InitAndSetCylinder(play, &this->collider, thisx, &sCylinderInit);
            Collider_UpdateCylinder(thisx, &this->collider);
            break;

        default:
            EnElforg_InitializeParams(this);
            this->actionFunc = EnElforg_FreeFloating;
            break;
    }

    thisx->shape.rot.y = 0;
}

void EnElforg_Destroy(Actor* thisx, PlayState* play) {
    EnElforg* this = (EnElforg*)thisx;

    if (STRAY_FAIRY_TYPE(&this->actor) == STRAY_FAIRY_TYPE_COLLIDER) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnElforg_SpawnSparkles(EnElforg* this, PlayState* play, s32 life) {
    static Vec3f sVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f sAccel = { 0.0f, -0.025f, 0.0f };
    static Color_RGBA8 sPrimColors[] = {
        { 255, 235, 220, 255 }, { 255, 220, 220, 255 }, { 220, 255, 220, 255 },
        { 220, 220, 255, 255 }, { 255, 255, 200, 255 },
    };
    static Color_RGBA8 sEnvColors[] = {
        { 255, 150, 0, 255 }, { 255, 0, 0, 255 }, { 0, 255, 0, 255 }, { 0, 0, 255, 255 }, { 255, 255, 0, 255 },
    };
    Vec3f pos;
    s32 pad;
    s32 index;

    pos.x = Rand_CenteredFloat(6.0f) + this->actor.world.pos.x;
    pos.y = (Rand_ZeroOne() * 6.0f) + this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y);
    pos.z = Rand_CenteredFloat(6.0f) + this->actor.world.pos.z;
    index = (this->area < STRAY_FAIRY_AREA_CLOCK_TOWN || this->area >= STRAY_FAIRY_AREA_MAX)
                ? STRAY_FAIRY_AREA_CLOCK_TOWN
                : this->area;
    EffectSsKirakira_SpawnDispersed(play, &pos, &sVelocity, &sAccel, &sPrimColors[index], &sEnvColors[index], 1000,
                                    life);
}

void EnElforg_ApproachTargetYPosition(EnElforg* this, Vec3f* targetPos) {
    f32 yDifference = targetPos->y - this->actor.world.pos.y;

    if (fabsf(yDifference) < this->actor.speed) {
        this->actor.world.pos.y = targetPos->y;
    } else if (yDifference > 0.0f) {
        this->actor.world.pos.y += this->actor.speed;
    } else {
        this->actor.world.pos.y -= this->actor.speed;
    }
}

void EnElforg_ApproachTargetSpeedXZ(EnElforg* this) {
    if (this->actor.speed > this->targetSpeedXZ) {
        this->actor.speed *= 0.9f;
    } else if (this->actor.speed < (this->targetSpeedXZ - 0.1f)) {
        this->actor.speed += 0.1f;
    } else {
        this->actor.speed = this->targetSpeedXZ;
    }
}

void EnElforg_MoveToTargetFairyFountain(EnElforg* this, Vec3f* homePos) {
    s32 pad[2];
    f32 xzDistance;
    f32 zDifference;
    f32 xDifference;
    s16 angleAdjustment;
    s16 targetAngle;

    this->actor.shape.yOffset += 100.0f * Math_SinS(this->timer * 0x200);
    EnElforg_ApproachTargetYPosition(this, homePos);
    xDifference = this->actor.world.pos.x - homePos->x;
    zDifference = this->actor.world.pos.z - homePos->z;
    targetAngle = Math_Atan2S_XY(-zDifference, -xDifference);
    xzDistance = sqrtf(SQ(xDifference) + SQ(zDifference));

    if ((this->targetDistanceFromHome + 10.0f) < xzDistance) {
        angleAdjustment = 0x1000;
    } else if ((this->targetDistanceFromHome - 10.0f) > xzDistance) {
        angleAdjustment = 0x6000;
    } else {
        angleAdjustment = 0x4000;
    }

    targetAngle += angleAdjustment;
    Math_SmoothStepToS(&this->actor.world.rot.y, targetAngle, 2, 0xFA0, 0x3E8);
    EnElforg_ApproachTargetSpeedXZ(this);
    Actor_MoveWithGravity(&this->actor);
}

/**
 * Adjust's the Stray Fairy's speed, rotation, and y-position to bring it
 * closer to targetPos. Since it doesn't directly point the fairy towards
 * its target, it can take strange paths or even "orbit" the target.
 */
void EnElforg_MoveToTarget(EnElforg* this, Vec3f* targetPos) {
    s16 targetAngle;

    this->actor.shape.yOffset += 100.0f * Math_SinS(this->timer * 0x200);
    EnElforg_ApproachTargetYPosition(this, targetPos);
    targetAngle = Math_Atan2S_XY(-(this->actor.world.pos.z - targetPos->z), -(this->actor.world.pos.x - targetPos->x));

    if (this->targetSpeedXZ > 2.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, targetAngle, 2, 0x400, 0x100);
    } else {
        // If the speed is below a threshold, deliberately mess up the target
        // angle so the fairy "orbits" the target instead of reaching it.
        targetAngle += 0x2000;
        Math_SmoothStepToS(&this->actor.world.rot.y, targetAngle, 10, 0x200, 0x80);
    }

    EnElforg_ApproachTargetSpeedXZ(this);
    Actor_MoveWithGravity(&this->actor);
}

void func_80ACCBB8(EnElforg* this, PlayState* play) {
    play->actorCtx.flags |= ACTORCTX_FLAG_3;
}

void EnElforg_TrappedByBubble(EnElforg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((this->actor.parent == NULL) || (this->actor.parent->update == NULL)) {
        EnElforg_InitializeParams(this);
        this->actionFunc = EnElforg_FreeFloating;
    } else {
        this->actor.shape.yOffset += 10.0f * Math_SinS(this->timer * 0x200);
        this->actor.world.pos = this->actor.parent->world.pos;
        this->actor.world.pos.y += 12.0f;
    }

    func_80ACCBB8(this, play);
}

void EnElforg_TurnInFairy(EnElforg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xzDistToPlayer;
    s16 rotationTemp;
    s16 newAngle;
    s32 pad;

    // This code makes the fairy briefly circle the player before
    // flying towards the fountain's center.
    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.yOffset *= 0.9f;
    this->actor.speed = 5.0f;
    EnElforg_ApproachTargetYPosition(this, &player->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    xzDistToPlayer = this->actor.xzDistToPlayer;
    if (xzDistToPlayer < 0.0f) {
        xzDistToPlayer = 10.0f;
    }

    newAngle = 0x28000 / xzDistToPlayer;
    Math_SmoothStepToF(&xzDistToPlayer, 40.0f, 0.2f, 100.0f, 1.0f);
    rotationTemp = this->actor.yawTowardsPlayer - newAngle;
    this->actor.world.pos.x = player->actor.world.pos.x - (Math_SinS(rotationTemp) * xzDistToPlayer);
    this->actor.world.pos.z = player->actor.world.pos.z - (Math_CosS(rotationTemp) * xzDistToPlayer);
    EnElforg_SpawnSparkles(this, play, 16);

    if (this->secondaryTimer > 0) {
        this->secondaryTimer--;
    } else {
        this->actor.world.rot.y = rotationTemp + 0x4000;
        this->timer = 0;
        this->secondaryTimer = Rand_ZeroFloat(100.0f);
        this->actor.shape.yOffset = 0.0f;
        this->targetSpeedXZ = 3.0f;
        this->targetDistanceFromHome = 50.0f;
        this->actionFunc = EnElforg_FreeFloatingFairyFountain;
        this->strayFairyFlags &= ~STRAY_FAIRY_FLAG_CIRCLES_QUICKLY_IN_FOUNTAIN;
    }
}

void EnElforg_QuicklyCircleFairyFountain(EnElforg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnElforg_MoveToTargetFairyFountain(this, &this->actor.home.pos);

    if (this->secondaryTimer <= 30) {
        this->actionFunc = EnElforg_TurnInFairy;
    }

    this->secondaryTimer--;
}

void EnElforg_FreeFloatingFairyFountain(EnElforg* this, PlayState* play) {
    s32 pad;

    if (this->strayFairyFlags & STRAY_FAIRY_FLAG_MOVES_QUICKLY_TO_HOME) {
        // This happens when "turning in" the last batch of Stray Fairies to
        // a Fairy Fountain. The ones being "turned in" will fly very
        // quickly to the center of the fountain.
        if (this->targetSpeedXZ < 8.0f) {
            this->targetSpeedXZ += 0.1f;
        }

        if (this->targetDistanceFromHome > 0.0f) {
            this->targetDistanceFromHome -= 2.0f;
        }
    } else if ((this->timer & 127) == 127) {
        if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) > 150.0f) {
            this->targetSpeedXZ = 5.0f;
        } else {
            this->targetSpeedXZ = Rand_ZeroFloat(2.0f) + 1.0f;
        }

        this->targetDistanceFromHome = Rand_ZeroFloat(100.0f) + 50.0f;
    }

    SkelAnime_Update(&this->skelAnime);
    EnElforg_MoveToTargetFairyFountain(this, &this->actor.home.pos);

    if (this->strayFairyFlags & STRAY_FAIRY_FLAG_CIRCLES_QUICKLY_IN_FOUNTAIN) {
        // A small number of fairies will do this when the player walks into
        // the center of the fountain to be healed.
        this->actionFunc = EnElforg_QuicklyCircleFairyFountain;
    }

    if (this->strayFairyFlags & STRAY_FAIRY_FLAG_SPARKLES_AND_SHRINKS) {
        // This happens right before the Great Fairy appears once all
        // Stray Fairies are saved.
        if (STRAY_FAIRY_SPARKLE_COUNT(&this->actor) > 0) {
            EnElforg_SpawnSparkles(this, play, 10);
            STRAY_FAIRY_SPARKLE_COUNT(&this->actor)--;
        }

        Actor_SetScale(&this->actor, this->actor.scale.x * 0.9f);
        if (this->actor.scale.x < 0.001f) {
            Actor_Kill(&this->actor);
        }
    }
}

void EnElforg_CirclePlayer(EnElforg* this, PlayState* play) {
    s32 pad;
    Actor* playerActor = &GET_PLAYER(play)->actor;
    Player* player = GET_PLAYER(play);
    f32 orbitRadius;

    if (GET_PLAYER_FORM == PLAYER_FORM_GORON) {
        orbitRadius = 40.0f;
    } else {
        orbitRadius = 20.0f;
    }

    this->actor.world.pos.x = (Math_SinS(this->timer * 0x1000) * orbitRadius) + playerActor->world.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->timer * 0x1000) * orbitRadius) + playerActor->world.pos.z;
    this->actor.world.pos.y = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y;

    EnElforg_SpawnSparkles(this, play, 16);
}

void EnElforg_FairyCollected(EnElforg* this, PlayState* play) {
    EnElforg_CirclePlayer(this, play);

    if (this->timer > 80) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_CHIBI_FAIRY_HEAL - SFX_FLAG);
}

void EnElforg_SetupFairyCollected(EnElforg* this, PlayState* play) {
    Actor* playerActor = &GET_PLAYER(play)->actor;
    Player* player = GET_PLAYER(play);

    this->actor.world.pos.x = playerActor->world.pos.x;
    this->actor.world.pos.y = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y;
    this->actor.world.pos.z = playerActor->world.pos.z;
    this->actionFunc = EnElforg_FairyCollected;
    this->timer = 0;
    this->secondaryTimer = 0;
    this->actor.shape.yOffset = 0.0f;
}

void EnElforg_ClockTownFairyCollected(EnElforg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnElforg_CirclePlayer(this, play);

    player->actor.freezeTimer = 100;
    player->stateFlags1 |= PLAYER_STATE1_20000000;

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        player->actor.freezeTimer = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20000000;
        Actor_Kill(&this->actor);
        SET_WEEKEVENTREG(WEEKEVENTREG_08_80);
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        return;
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_CHIBI_FAIRY_HEAL - SFX_FLAG);
    if (CutsceneManager_GetCurrentCsId() != CS_ID_GLOBAL_TALK) {
        if (CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)) {
            CutsceneManager_Start(CS_ID_GLOBAL_TALK, &this->actor);
        } else {
            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        }
    }
}

void EnElforg_FreeFloating(EnElforg* this, PlayState* play) {
    Vec3f pos;
    f32 scaledYDistance;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    if (Player_GetMask(play) == PLAYER_MASK_GREAT_FAIRY) {
        pos = player->bodyPartsPos[PLAYER_BODYPART_WAIST];
        this->targetSpeedXZ = 5.0f;
        EnElforg_MoveToTarget(this, &pos);
    } else {
        this->targetSpeedXZ = 1.0f;
        EnElforg_MoveToTarget(this, &this->actor.home.pos);
    }

    scaledYDistance = this->actor.playerHeightRel - (this->actor.shape.yOffset * this->actor.scale.y);

    if (!Player_InCsMode(play)) {
        if ((this->actor.xzDistToPlayer < 30.0f) && (scaledYDistance < 12.0f) && (scaledYDistance > -68.0f)) {
            EnElforg_SetupFairyCollected(this, play);
            Health_ChangeBy(play, 0x30);

            switch (STRAY_FAIRY_TYPE(&this->actor)) {
                case STRAY_FAIRY_TYPE_COLLECTIBLE:
                    Flags_SetCollectible(play, STRAY_FAIRY_GET_FLAG(&this->actor));
                    break;

                case STRAY_FAIRY_TYPE_CHEST:
                    Flags_SetTreasure(play, STRAY_FAIRY_GET_FLAG(&this->actor));
                    break;

                default:
                    Flags_SetSwitch(play, STRAY_FAIRY_GET_FLAG(&this->actor));
                    break;
            }

            if (GameInteractor_Should(VB_GIVE_ITEM_FROM_ELFORG, true, this)) {
                if (STRAY_FAIRY_TYPE(&this->actor) == STRAY_FAIRY_TYPE_CLOCK_TOWN) {
                    player->actor.freezeTimer = 100;
                    player->stateFlags1 |= PLAYER_STATE1_20000000;
                    // Bring me back to North Clock Town!
                    Message_StartTextbox(play, 0x579, NULL);
                    this->actionFunc = EnElforg_ClockTownFairyCollected;
                    CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
                    return;
                }

                if (Map_IsInDungeonOrBossScene(play)) {
                    gSaveContext.save.saveInfo.inventory.strayFairies[gSaveContext.dungeonSceneSharedIndex]++;
                    // You found a Stray Fairy!
                    Message_StartTextbox(play, 0x11, NULL);
                    if (gSaveContext.save.saveInfo.inventory
                            .strayFairies[(void)0, gSaveContext.dungeonSceneSharedIndex] >=
                        STRAY_FAIRY_SCATTERED_TOTAL) {
                        // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't
                        // seem to do anything bad.
                        Audio_PlayFanfare(NA_BGM_GET_ITEM);
                    }
                }
            }
        }
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
        func_80ACCBB8(this, play);

        if (Player_GetMask(play) == PLAYER_MASK_GREAT_FAIRY) {
            if (!(this->strayFairyFlags & STRAY_FAIRY_FLAG_GREAT_FAIRYS_MASK_EQUIPPED)) {
                Audio_PlaySfx(NA_SE_SY_FAIRY_MASK_SUCCESS);
            }

            this->strayFairyFlags |= STRAY_FAIRY_FLAG_GREAT_FAIRYS_MASK_EQUIPPED;
        } else {
            this->strayFairyFlags &= ~STRAY_FAIRY_FLAG_GREAT_FAIRYS_MASK_EQUIPPED;
        }
    }
}

/**
 * This finds the enemy that is "holding" the Stray Fairy hostage. When
 * this enemy is killed, the Stray Fairy will spawn. This function only
 * works if the enemy and the Stray Fairy have the exact same home
 * coordinates when the Stray Fairy first spawns.
 */
Actor* EnElforg_GetHoldingEnemy(EnElforg* this, PlayState* play) {
    Actor* enemy;

    for (enemy = play->actorCtx.actorLists[ACTORCAT_ENEMY].first; enemy != NULL; enemy = enemy->next) {
        if ((enemy->home.pos.x == this->actor.home.pos.x) && (enemy->home.pos.y == this->actor.home.pos.y) &&
            (enemy->home.pos.z == this->actor.home.pos.z)) {
            return enemy;
        }
    }

    return NULL;
}

void EnElforg_TrappedByEnemy(EnElforg* this, PlayState* play) {
    f32 posTemp;

    if (this->enemy->update == NULL) {
        EnElforg_InitializeParams(this);
        this->actionFunc = EnElforg_FreeFloating;
        if (GameInteractor_Should(VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY, true, this)) {
            this->actor.draw = EnElforg_Draw;
        }
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHIBI_FAIRY_SAVED);
    } else {
        // The enemy is still alive, so have the Stray Fairy
        // track the enemy in case it's moving around.
        posTemp = this->enemy->world.pos.x;
        this->actor.world.pos.x = posTemp;
        this->actor.home.pos.x = posTemp;
        posTemp = this->enemy->world.pos.y + 30.0f;
        this->actor.world.pos.y = posTemp;
        this->actor.home.pos.y = posTemp;
        posTemp = this->enemy->world.pos.z;
        this->actor.world.pos.z = posTemp;
        this->actor.home.pos.z = posTemp;
    }

    func_80ACCBB8(this, play);
}

void EnElforg_SetupTrappedByEnemy(EnElforg* this, PlayState* play) {
    Actor* enemy = EnElforg_GetHoldingEnemy(this, play);

    if ((enemy != NULL) && (enemy->update != NULL)) {
        this->actionFunc = EnElforg_TrappedByEnemy;
        this->enemy = enemy;
    }
}

void EnElforg_HiddenByCollider(EnElforg* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        EnElforg_InitializeParams(this);
        this->actionFunc = EnElforg_FreeFloating;
        if (GameInteractor_Should(VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY, true, this)) {
            this->actor.draw = EnElforg_Draw;
        }
        this->actor.world.pos.y += 40.0f;
        this->actor.home.pos.y += 40.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHIBI_FAIRY_SAVED);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    func_80ACCBB8(this, play);
}

void EnElforg_Update(Actor* thisx, PlayState* play) {
    EnElforg* this = (EnElforg*)thisx;

    this->actionFunc(this, play);

    if ((this->timer == 0) && (this->secondaryTimer > 0)) {
        this->secondaryTimer--;
    } else {
        this->timer++;
    }

    if (this->direction < 0) {
        this->direction++;
        if (this->direction == 0) {
            this->direction = Rand_ZeroFloat(20.0f) + 20.0f;
        }
    } else if (this->direction > 0) {
        this->direction--;
    } else {
        this->direction = -Rand_ZeroFloat(20.0f) - 20.0f;
    }
}

s32 EnElforg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                              Gfx** gfx) {
    EnElforg* this = (EnElforg*)thisx;

    if (this->direction < 0) {
        if (limbIndex == STRAY_FAIRY_LIMB_LEFT_FACING_HEAD) {
            *dList = NULL;
        }
    } else if (limbIndex == STRAY_FAIRY_LIMB_RIGHT_FACING_HEAD) {
        *dList = NULL;
    }

    return false;
}

void EnElforg_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnElforg* this = (EnElforg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    CosmeticEditor_SetStrayFairyMaterial(this->area);

    switch (this->area) {
        case STRAY_FAIRY_AREA_WOODFALL:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyWoodfallTexAnim));
            break;

        case STRAY_FAIRY_AREA_SNOWHEAD:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairySnowheadTexAnim));
            break;

        case STRAY_FAIRY_AREA_GREAT_BAY:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyGreatBayTexAnim));
            break;

        case STRAY_FAIRY_AREA_STONE_TOWER:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyStoneTowerTexAnim));
            break;

        default: // STRAY_FAIRY_AREA_CLOCK_TOWN
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gStrayFairyClockTownTexAnim));
            break;
    }

    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

    POLY_XLU_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnElforg_OverrideLimbDraw, NULL, &this->actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}
