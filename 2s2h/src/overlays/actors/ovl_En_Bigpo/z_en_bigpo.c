/*
 * File: z_en_bigpo.c
 * Overlay: ovl_En_Bigpo
 * Description: Big Poe. Found under Dampe's house and in Beneath the Well
 */

#include "z_en_bigpo.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_bigpo/object_bigpo.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_IGNORE_QUAKE)

void EnBigpo_Init(Actor* thisx, PlayState* play2);
void EnBigpo_Destroy(Actor* thisx, PlayState* play2);
void EnBigpo_Update(Actor* thisx, PlayState* play);
void EnBigpo_UpdateFire(Actor* thisx, PlayState* play);

void EnBigpo_InitWellBigpo(EnBigpo* this);
void EnBigpo_HitStun(EnBigpo* this);
void EnBigpo_InitDampeMainPo(EnBigpo* this);
void EnBigpo_ChangeToFireCounting(EnBigpo* this);
void EnBigpo_InitHiddenFire(EnBigpo* this);
void EnBigpo_SetupFireRevealed(EnBigpo* this);

void EnBigpo_SetupSpawnCutscene(EnBigpo* this);
void EnBigpo_SpawnCutsceneStage1(EnBigpo* this, PlayState* play);
void EnBigpo_SpawnCutsceneStage2(EnBigpo* this, PlayState* play);
void EnBigpo_SpawnCutsceneStage3(EnBigpo* this);
void EnBigpo_SpawnCutsceneStage4(EnBigpo* this, PlayState* play);
void EnBigpo_SpawnCutsceneStage5(EnBigpo* this);
void EnBigpo_SpawnCutsceneStage6(EnBigpo* this, PlayState* play);
void EnBigpo_SpawnCutsceneStage7(EnBigpo* this);
void EnBigpo_SpawnCutsceneStage8(EnBigpo* this, PlayState* play);

s32 EnBigpo_ApplyDamage(EnBigpo* this, PlayState* play);
void EnBigpo_LowerCutsceneSubCamera(EnBigpo* this, PlayState* play);
void EnBigpo_WellWaitForProximity(EnBigpo* this, PlayState* play);
void EnBigpo_WaitCutsceneQueue(EnBigpo* this, PlayState* play);
void EnBigpo_SetupWarpOut(EnBigpo* this);
void EnBigpo_WarpingOut(EnBigpo* this, PlayState* play);
void EnBigpo_SetupWarpIn(EnBigpo* this, PlayState* play);
void EnBigpo_WarpingIn(EnBigpo* this, PlayState* play);
void EnBigpo_SetupIdleFlying(EnBigpo* this);
void EnBigpo_IdleFlying(EnBigpo* this, PlayState* play);
void EnBigpo_SetupSpinUp(EnBigpo* this);
void EnBigpo_SpinningUp(EnBigpo* this, PlayState* play);
void EnBigpo_SetupSpinAttack(EnBigpo* this);
void EnBigpo_SpinAttack(EnBigpo* this, PlayState* play);
void EnBigpo_SetupSpinDown(EnBigpo* this);
void EnBigpo_SpinningDown(EnBigpo* this, PlayState* play);
void EnBigpo_CheckHealth(EnBigpo* this, PlayState* play);
void EnBigpo_SetupDeath(EnBigpo* this);
void EnBigpo_BurnAwayDeath(EnBigpo* this, PlayState* play);
void EnBigpo_SetupLanternDrop(EnBigpo* this, PlayState* play);
void EnBigpo_LanternFalling(EnBigpo* this, PlayState* play);
void EnBigpo_SpawnScoopSoul(EnBigpo* this);
void EnBigpo_ScoopSoulAppearing(EnBigpo* this, PlayState* play);
void EnBigpo_SetupScoopSoulIdle(EnBigpo* this);
void EnBigpo_ScoopSoulIdle(EnBigpo* this, PlayState* play);
void EnBigpo_SetupScoopSoulLeaving(EnBigpo* this);
void EnBigpo_ScoopSoulFadingAway(EnBigpo* this, PlayState* play);
void EnBigpo_Die(EnBigpo* this, PlayState* play);
void EnBigpo_SelectRandomFireLocations(EnBigpo* this, PlayState* play);
void EnBigpo_FireCounting(EnBigpo* this, PlayState* play);
void EnBigpo_SetupFlameCirclePositions(EnBigpo* this, PlayState* play);
void EnBigpo_SetupFlameCircleCutscene(EnBigpo* this);
void EnBigpo_FlameCircleCutscene(EnBigpo* this, PlayState* play);
void EnBigpo_UpdateColor(EnBigpo* this);
void EnBigpo_FlickerLanternLight(EnBigpo* this);
void EnBigpo_SetupRevealedFireIdle(EnBigpo* this);
void EnBigpo_RevealedFireIdle(EnBigpo* this, PlayState* play);
void EnBigpo_DoNothing(EnBigpo* this, PlayState* play);
void EnBigpo_WaitingForDampe(EnBigpo* this, PlayState* play);
void EnBigpo_RevealedFireGrowing(EnBigpo* this, PlayState* play);

// draw funcs
void EnBigpo_DrawMainBigpo(Actor* thisx, PlayState* play);
void EnBigpo_DrawScoopSoul(Actor* thisx, PlayState* play);
void EnBigpo_DrawLantern(Actor* thisx, PlayState* play);
void EnBigpo_DrawCircleFlames(Actor* thisx, PlayState* play);
void EnBigpo_RevealedFire(Actor* thisx, PlayState* play);

ActorProfile En_Bigpo_Profile = {
    /**/ ACTOR_EN_BIGPO,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BIGPO,
    /**/ sizeof(EnBigpo),
    /**/ EnBigpo_Init,
    /**/ EnBigpo_Destroy,
    /**/ EnBigpo_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE | AC_TYPE_PLAYER,
        OC1_NONE | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 35, 100, 10, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 35, 100, 50 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_BIG_POE, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 3200, ICHAIN_STOP),
};

// used in the burning death actionfunc
static Vec3f D_80B6506C = { 0.0f, 3.0f, 0.0f };

static s8 sLimbToBodyParts[BIG_POE_LIMB_MAX] = {
    BODYPART_NONE,      // BIG_POE_LIMB_NONE
    BIG_POE_BODYPART_4, // BIG_POE_LIMB_FACE
    BODYPART_NONE,      // BIG_POE_LIMB_LEFT_UPPER_ARM
    BIG_POE_BODYPART_0, // BIG_POE_LIMB_LEFT_FOREARM
    BODYPART_NONE,      // BIG_POE_LIMB_RIGHT_UPPER_ARM
    BIG_POE_BODYPART_1, // BIG_POE_LIMB_RIGHT_FOREARM
    BODYPART_NONE,      // BIG_POE_LIMB_RIGHT_HAND
    BIG_POE_BODYPART_2, // BIG_POE_LIMB_LANTERN
    BIG_POE_BODYPART_5, // BIG_POE_LIMB_HAT_AND_CLOAK
    BIG_POE_BODYPART_3, // BIG_POE_LIMB_LOWER_ROBE
};

// used in limbdraw
static Vec3f D_80B65084[] = {
    { 2000.0f, 4000.0f, 0.0f },      // BIG_POE_BODYPART_6
    { -1000.0f, 1500.0f, -2000.0f }, // BIG_POE_BODYPART_7
    { -1000.0f, 1500.0f, 2000.0f },  // BIG_POE_BODYPART_8
};

void EnBigpo_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBigpo* this = (EnBigpo*)thisx;
    EnBigpoFireEffect* firesPtr;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    this->switchFlag = BIG_POE_GET_SWITCH_FLAG(thisx);
    thisx->params &= 0xFF;
    if (thisx->params == BIG_POE_TYPE_POSSIBLE_FIRE) {
        if (Flags_GetSwitch(play, this->switchFlag)) {
            Actor_Kill(&this->actor);
            return;
        }

        thisx->update = Actor_Noop;
        EnBigpo_InitHiddenFire(this);
        return;
    }

    SkelAnime_Init(play, &this->skelAnime, &gBigPoeSkel, &gBigPoeFloatAnim, this->jointTable, this->morphTable,
                   BIG_POE_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        firesPtr = &this->fires[i];
        firesPtr->light = LightContext_InsertLight(play, &play->lightCtx, &firesPtr->info);

        Lights_PointNoGlowSetInfo(&firesPtr->info, thisx->home.pos.x, thisx->home.pos.y, thisx->home.pos.z, 255, 255,
                                  255, 0);
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 45.0f);
    thisx->bgCheckFlags |= BGCHECKFLAG_PLAYER_400;
    this->savedHeight = thisx->home.pos.y + 100.0f;
    this->mainColor.r = 255;
    this->mainColor.g = 255;
    this->mainColor.b = 210;
    this->mainColor.a = 0; // fully invisible

    if ((this->switchFlag != BIG_POE_SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
    }

    if (thisx->params == BIG_POE_TYPE_REGULAR) {             // the well poe, starts immediately
        thisx->flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED; // always update OFF
        this->storePrevBgm = true;
        EnBigpo_InitWellBigpo(this);
    } else if (thisx->params == BIG_POE_TYPE_SUMMONED) { // dampe type
        EnBigpo_InitDampeMainPo(this);
    }
}

void EnBigpo_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBigpo* this = (EnBigpo*)thisx;
    s32 fireCount;

    if ((thisx->params != BIG_POE_TYPE_POSSIBLE_FIRE) && (thisx->params != BIG_POE_TYPE_CHOSEN_FIRE) &&
        (thisx->params != BIG_POE_TYPE_REVEALED_FIRE) && (thisx->params != BIG_POE_TYPE_UNK5)) {
        // if NOT a fire type, *BIG_POE_TYPE_REGULAR and BIG_POE_TYPE_SUMMONED (combat types only)
        for (fireCount = 0; fireCount < ARRAY_COUNT(this->fires); fireCount++) {
            LightContext_RemoveLight(play, &play->lightCtx, this->fires[fireCount].light);
        }
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnBigpo_RotateSpawnCutsceneFires(EnBigpo* this) {
    EnBigpoFireEffect* firePtr;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        firePtr = &this->fires[i];
        firePtr->pos.x = (Math_SinS(this->actor.shape.rot.y) * this->fireRadius) + this->actor.world.pos.x;
        firePtr->pos.z = (Math_CosS(this->actor.shape.rot.y) * this->fireRadius) + this->actor.world.pos.z;
        this->actor.shape.rot.y += 0x5555;
    }
}

void EnBigpo_UpdateSpin(EnBigpo* this) {
    s16 oldYaw = this->actor.shape.rot.y;

    this->actor.shape.rot.y += this->angularVelocity;
    if ((oldYaw < 0) && (this->actor.shape.rot.y > 0)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL); // spinning sfx during spin attack
    }
}

/*
 * Lowers the position/eye of the camera during the Big Poe spawn cutscene
 */
void EnBigpo_LowerCutsceneSubCamera(EnBigpo* this, PlayState* play) {
    Camera* subCam;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        subCam = Play_GetCamera(play, this->subCamId);
        subCam->eye.y -= this->actor.velocity.y;
        if (this->actor.velocity.y > 0.0f) {
            subCam->eye.x -= 1.5f * Math_SinS(this->actor.yawTowardsPlayer);
            subCam->eye.z -= 1.5f * Math_CosS(this->actor.yawTowardsPlayer);
        }
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCam->eye);
    }
}

void EnBigpo_InitWellBigpo(EnBigpo* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBigpo_WellWaitForProximity;
    this->fireRadius = 200.0f;
}

void EnBigpo_WellWaitForProximity(EnBigpo* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 200.0f) {
        EnBigpo_SetupSpawnCutscene(this);
    }
}

void EnBigpo_SetupSpawnCutscene(EnBigpo* this) {
    CutsceneManager_Queue(this->actor.csId);
    this->actionFunc = EnBigpo_WaitCutsceneQueue;
}

void EnBigpo_WaitCutsceneQueue(EnBigpo* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_Start(this->actor.csId, &this->actor);
        Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_WAIT);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        if (this->actor.params == BIG_POE_TYPE_REGULAR) { // and SUMMONED, got switched earlier
            EnBigpo_SpawnCutsceneStage1(this, play);
        } else { // BIG_POE_TYPE_REVEALED_FIRE
            EnBigpo_SetupFlameCirclePositions(this, play);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

/*
 * stage 1: fires are set to draw, size set to tiny, camera pointed
 */
void EnBigpo_SpawnCutsceneStage1(EnBigpo* this, PlayState* play) {
    s32 i;

    this->actor.draw = EnBigpo_DrawCircleFlames;
    this->actor.shape.rot.y = BINANG_ROT180(this->actor.yawTowardsPlayer);
    EnBigpo_RotateSpawnCutsceneFires(this);

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        this->fires[i].pos.y = this->actor.world.pos.y;
    }

    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.015f;
    this->actor.scale.z = 0.0f;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f subCamEye;

        subCamEye.x = ((this->actor.world.pos.x - this->fires[0].pos.x) * 1.8f) + this->actor.world.pos.x;
        subCamEye.y = this->actor.world.pos.y + 150.0f;
        subCamEye.z = ((this->actor.world.pos.z - this->fires[0].pos.z) * 1.8f) + this->actor.world.pos.z;
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCamEye);
    }
    this->actionFunc = EnBigpo_SpawnCutsceneStage2;
}

/*
 * stage 2: fires are growing to full size
 */
void EnBigpo_SpawnCutsceneStage2(EnBigpo* this, PlayState* play) {
    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.001f)) {
        EnBigpo_SpawnCutsceneStage3(this);
    }
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = ((0.01f - this->actor.scale.x) * 0.5f) + 0.01f;
}

/*
 * stage 3: switch to fires rotating
 */
void EnBigpo_SpawnCutsceneStage3(EnBigpo* this) {
    this->angularVelocity = 0x1000;
    this->actionFunc = EnBigpo_SpawnCutsceneStage4;
    this->fireRadius = 200.0f;
    this->actor.velocity.y = 0.0f;
}

/*
 * stage 4: fires are circling inward toward each other
 */
void EnBigpo_SpawnCutsceneStage4(EnBigpo* this, PlayState* play) {
    s32 i;

    if (Math_StepToF(&this->fireRadius, 30.0f, 5.0f)) {
        this->angularVelocity += 0x80;
        this->actor.velocity.y += 0.25f;
    }
    this->actor.shape.rot.y += this->angularVelocity;
    EnBigpo_RotateSpawnCutsceneFires(this);

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        this->fires[i].pos.y += this->actor.velocity.y;
    }

    this->actor.world.pos.y += this->actor.velocity.y;
    EnBigpo_LowerCutsceneSubCamera(this, play);
    if (this->actor.velocity.y >= 4.0f) {
        EnBigpo_SpawnCutsceneStage5(this);
    }
}

/*
 * stage 5: fires have touched, they start to rise,
 *          big poe starts to visibly appear
 */
void EnBigpo_SpawnCutsceneStage5(EnBigpo* this) {
    Animation_PlayLoop(&this->skelAnime, &gBigPoeAwakenStretchAnim);
    this->actor.draw = EnBigpo_DrawMainBigpo;
    Actor_SetScale(&this->actor, 0.014f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
    this->actionFunc = EnBigpo_SpawnCutsceneStage6;
}

/*
 * stage 6:  big poe becoming more visible in the flames
 *           and flames dissapearing
 */
void EnBigpo_SpawnCutsceneStage6(EnBigpo* this, PlayState* play) {
    s32 i;
    s32 alphaPlus; // color alpha + 10

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y += this->angularVelocity;
    alphaPlus = this->mainColor.a + 10; // decrease transparency
    EnBigpo_RotateSpawnCutsceneFires(this);
    if (alphaPlus >= 90) {
        this->angularVelocity -= 0x80;
        this->actor.velocity.y -= 0.25f;
        if (alphaPlus >= 180) {
            Math_ScaledStepToS(&this->actor.world.rot.y, 0, 0x180);
        }
    }
    this->actor.world.pos.y += this->actor.velocity.y;

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        this->fires[i].pos.y += this->actor.velocity.y;
    }

    EnBigpo_LowerCutsceneSubCamera(this, play);
    if (alphaPlus >= 255) {
        this->mainColor.a = 255; // fully visible
        EnBigpo_SpawnCutsceneStage7(this);
    } else {
        this->mainColor.a = alphaPlus;
    }
}

/*
 * stage 7:  big poe now fully visible
 */
void EnBigpo_SpawnCutsceneStage7(EnBigpo* this) {
    this->idleTimer = 15;
    if (this->storePrevBgm == false) {
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
        this->storePrevBgm = true;
    }
    this->actionFunc = EnBigpo_SpawnCutsceneStage8;
}

/*
 * stage 8:  count 15 frames, animating, then start dampe cutscene if hes here
 *           also sets the main camera to align with the subCamera
 *           and switches back from the subCamera back to the main camera
 */
void EnBigpo_SpawnCutsceneStage8(EnBigpo* this, PlayState* play) {
    Actor* dampe;
    Camera* subCam;

    SkelAnime_Update(&this->skelAnime);
    this->idleTimer--;
    if (this->idleTimer == 0) {
        subCam = Play_GetCamera(play, this->subCamId);
        Play_SetCameraAtEye(play, CAM_ID_MAIN, &subCam->at, &subCam->eye);
        this->subCamId = SUB_CAM_ID_DONE;
        if (this->actor.params == BIG_POE_TYPE_SUMMONED) {
            dampe = SubS_FindActor(play, NULL, ACTORCAT_NPC, ACTOR_EN_TK);
            if (dampe != NULL) {
                // if dampe exists, switch to viewing his running away cutscene
                dampe->params = this->actor.csId;
            } else {
                CutsceneManager_Stop(this->actor.csId);
            }
        } else { // BIG_POE_TYPE_REGULAR
            CutsceneManager_Stop(this->actor.csId);
        }
        Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_END);
        EnBigpo_SetupIdleFlying(this); // setup idle flying
    }
}

void EnBigpo_SetupWarpOut(EnBigpo* this) {
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->angularVelocity = 0x2000;
    this->idleTimer = 32;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    this->actionFunc = EnBigpo_WarpingOut;
}

void EnBigpo_WarpingOut(EnBigpo* this, PlayState* play) {
    DECR(this->idleTimer);
    this->actor.shape.rot.y += this->angularVelocity;
    if (this->idleTimer < 16) {
        Math_ScaledStepToS(&this->angularVelocity, 0, 0x200);
    }
    this->mainColor.a = this->idleTimer * (255.0f / 32.0f);
    if (this->idleTimer == 0) {
        this->mainColor.a = 0; // fully invisible
        EnBigpo_SetupWarpIn(this, play);
    }
}

void EnBigpo_SetupWarpIn(EnBigpo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distance = CLAMP_MIN(this->actor.xzDistToPlayer, 200.0f);
    s16 randomYaw = ((s32)Rand_Next() >> 0x14) + this->actor.yawTowardsPlayer;

    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
    Animation_PlayLoop(&this->skelAnime, &gBigPoeAwakenStretchAnim);
    this->angularVelocity = 0x2000;
    this->actor.world.pos.x = (Math_SinS(randomYaw) * distance) + player->actor.world.pos.x;
    this->actor.world.pos.z = (Math_CosS(randomYaw) * distance) + player->actor.world.pos.z;
    this->actionFunc = EnBigpo_WarpingIn;
}

void EnBigpo_WarpingIn(EnBigpo* this, PlayState* play) {
    this->idleTimer++;
    this->actor.shape.rot.y -= this->angularVelocity;
    if (this->idleTimer >= 16) {
        // after 16th frame, start slowing rotation
        Math_ScaledStepToS(&this->angularVelocity, 0, 0x200);
    }

    this->mainColor.a = this->idleTimer * (255.0f / 32.0f);
    if (this->idleTimer == 32) {
        this->mainColor.a = 255; // fully visible
        if (this->storePrevBgm == false) {
            Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            this->storePrevBgm = true;
        }
        EnBigpo_SetupIdleFlying(this);
    }
}

void EnBigpo_SetupIdleFlying(EnBigpo* this) {
    Animation_MorphToLoop(&this->skelAnime, &gBigPoeFloatAnim, -5.0f);
    // if poe missed attack, idle 4 seconds, otherwise its reappearing: attack immediately
    this->idleTimer = (this->actionFunc == EnBigpo_SpinningDown) ? 80 : 0;
    this->hoverHeightCycleTimer = 40;
    this->actor.velocity.y = 0.0f;
    this->savedHeight = this->actor.world.pos.y;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBigpo_IdleFlying;
}

void EnBigpo_IdleFlying(EnBigpo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    DECR(this->idleTimer);

    // flight position calculations
    this->hoverHeightCycleTimer = (this->hoverHeightCycleTimer == 0) ? 40 : (this->hoverHeightCycleTimer - 1);
    Math_StepToF(&this->savedHeight, player->actor.world.pos.y + 100.0f, 1.5f);
    this->actor.world.pos.y = (Math_SinF(this->hoverHeightCycleTimer * (M_PIf / 20)) * 10.0f) + this->savedHeight;
    Math_StepToF(&this->actor.speed, 3.0f, 0.2f);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 300.0f) {
        this->unk208 = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->unk208, 0x200) && (Rand_ZeroOne() < 0.075f)) {
        this->unk208 += (s16)(((Rand_Next() >> 0x14) + 0x1000) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1));
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (this->idleTimer == 0) {
        // poe's break is done, time to attack
        EnBigpo_SetupSpinUp(this);
    }
}

void EnBigpo_SetupSpinUp(EnBigpo* this) {
    this->collider.base.colMaterial = COL_MATERIAL_METAL;
    this->collider.base.acFlags |= AC_HARD;
    this->collider.elem.acDmgInfo.dmgFlags &= ~0x8000;
    this->collider.base.atFlags |= AT_ON;
    this->angularVelocity = 0x800;
    this->actionFunc = EnBigpo_SpinningUp;
    this->actor.speed = 0.0f;
}

void EnBigpo_SpinningUp(EnBigpo* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->angularVelocity += 0x200;
    EnBigpo_UpdateSpin(this);
    if (this->angularVelocity >= 0x3C00) {
        EnBigpo_SetupSpinAttack(this);
    }
}

void EnBigpo_SetupSpinAttack(EnBigpo* this) {
    // set flying direction at player (not spinning direction)
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actionFunc = EnBigpo_SpinAttack;
}

void EnBigpo_SpinAttack(EnBigpo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speed, 10.0f, 1.0f);
    Math_SmoothStepToF(&this->actor.world.pos.y, player->actor.world.pos.y, 0.3f, 7.5f, 1.0f);
    EnBigpo_UpdateSpin(this);
    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    // because acFlags AC_HARD and COL_MATERIAL_METAL, if we hit it means we contacted as attack
    if ((this->collider.base.atFlags & AT_HIT) ||
        ((ABS_ALT(yawDiff) > 0x4000) && (this->actor.xzDistToPlayer > 50.0f))) {
        // hit the player OR the poe has missed and flew past player
        EnBigpo_SetupSpinDown(this);
    }
}

/*
 * if po misses, has to spin down before idle flying
 */
void EnBigpo_SetupSpinDown(EnBigpo* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = EnBigpo_SpinningDown;
}

void EnBigpo_SpinningDown(EnBigpo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToF(&this->actor.world.pos.y, player->actor.world.pos.y + 100.0f, 0.3f, 5.0f, 1.0f);
    Math_StepToF(&this->actor.speed, 0.0f, 0.2f);
    if (Math_ScaledStepToS(&this->angularVelocity, 0, 0x200)) {
        // spin down complete, re-allow hittable
        this->collider.base.colMaterial = COL_MATERIAL_HIT3;
        this->collider.base.acFlags &= ~AC_HARD;
        this->collider.elem.acDmgInfo.dmgFlags |= 0x8000;
        EnBigpo_SetupIdleFlying(this);
    }
    EnBigpo_UpdateSpin(this);
}

/*
 * hit by player, stunned
 * called by EnBigpo_ApplyDamage
 */
void EnBigpo_HitStun(EnBigpo* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gBigPoeShockAnim, -6.0f);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->collider.base.acFlags &= ~AC_ON;
    func_800BE504(&this->actor, &this->collider);
    this->actionFunc = EnBigpo_CheckHealth;
    this->actor.speed = 5.0f;
}

/*
 * check if just damaged or dead
 */
void EnBigpo_CheckHealth(EnBigpo* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            EnBigpo_SetupDeath(this);
        } else {
            EnBigpo_SetupWarpOut(this);
        }
    }
}

void EnBigpo_SetupDeath(EnBigpo* this) {
    this->idleTimer = 0;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.hintId = TATL_HINT_ID_NONE;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->actionFunc = EnBigpo_BurnAwayDeath;
}

/*
 * from [red damaged poe] to [burning up poe] to [shinking into the lantern]
 */
void EnBigpo_BurnAwayDeath(EnBigpo* this, PlayState* play) {
    Vec3f tempVec;
    f32 unkTemp2; // dont really know what these unktemps are doing
    s16 camYaw;
    s16 unkTemp;
    s16 modifiedTimer;

    this->idleTimer++;
    if (this->idleTimer < 8) {
        camYaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800;
        if (this->idleTimer < 5) {
            unkTemp = (this->idleTimer * 0x1000) - 0x4000;
            tempVec.y = (((Math_SinS(unkTemp) * 23.0f) + 40.0f) * (1400.0f * 0.001f)) + this->actor.world.pos.y;
            unkTemp2 = Math_CosS(unkTemp) * 32.2f;
            tempVec.x = (Math_SinS(camYaw) * unkTemp2) + this->actor.world.pos.x;
            tempVec.z = (Math_CosS(camYaw) * unkTemp2) + this->actor.world.pos.z;

        } else {
            tempVec.y = this->actor.world.pos.y + ((40.0f + (15.0f * (this->idleTimer - 5))) * (1400.0f * 0.001f));
            tempVec.x = (Math_SinS(camYaw) * 32.2f) + this->actor.world.pos.x;
            tempVec.z = (Math_CosS(camYaw) * 32.2f) + this->actor.world.pos.z;
        }

        // not sure what we're turning this into, but its based on the timer
        modifiedTimer = ((this->idleTimer * 10) + 80) * (1400.0f * 0.001f);
        func_800B3030(play, &tempVec, &D_80B6506C, &gZeroVec3f, modifiedTimer, 0, 2);
        tempVec.x = (2.0f * this->actor.world.pos.x) - tempVec.x;
        tempVec.z = (2.0f * this->actor.world.pos.z) - tempVec.z;
        func_800B3030(play, &tempVec, &D_80B6506C, &gZeroVec3f, modifiedTimer, 0, 2);
        tempVec.x = this->actor.world.pos.x;
        tempVec.z = this->actor.world.pos.z;
        func_800B3030(play, &tempVec, &D_80B6506C, &gZeroVec3f, modifiedTimer, 0, 2);

    } else if (this->idleTimer >= 28) {
        EnBigpo_SetupLanternDrop(this, play);

    } else if (this->idleTimer >= 19) {
        this->actor.scale.x = ((28 - this->idleTimer) * 0.014f) * 0.1f;
        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
        this->actor.world.pos.y += 5.0f;
    }

    if (this->idleTimer < 18) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG); // burning sfx
    }
    if (this->idleTimer == 18) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_WIZ_DISAPPEAR);
    }
}

void EnBigpo_SetupLanternDrop(EnBigpo* this, PlayState* play) {
    this->actor.draw = EnBigpo_DrawLantern;
    this->actor.shape.shadowDraw = NULL;
    this->actor.world.pos.x = this->drawMtxF.xw;
    this->actor.world.pos.y = this->drawMtxF.yw;
    this->actor.world.pos.z = this->drawMtxF.zw;

    Actor_SetScale(&this->actor, 0.014f);
    this->actor.gravity = -1.0f;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.shape.rot.x = -0x8000;
    this->actor.velocity.y = 0.0f;
    this->actor.world.pos.y -= 15.0f;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE); // targetable OFF, enemy music OFF
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_400;
    this->actionFunc = EnBigpo_LanternFalling;
}

void EnBigpo_LanternFalling(EnBigpo* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND || this->actor.floorHeight == BGCHECK_Y_MIN) {
        if (this->switchFlag != BIG_POE_SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }

        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 6.0f, 0, 1, 1, 15, OBJECT_BIGPO, 10,
                                 gBigPoeLanternFallingDL);
        EnBigpo_SpawnScoopSoul(this);
    }
}

void EnBigpo_AdjustPoAlpha(EnBigpo* this, s32 alphaDiff) {
    s32 newAlpha = this->mainColor.a + alphaDiff;
    f32 lowerAlpha;
    f32 newXYScale;

    this->mainColor.a = (newAlpha < 0) ? 0 : ((newAlpha > 255) ? 255 : newAlpha);

    lowerAlpha = this->mainColor.a * (1.0f / 255.0f);
    if (alphaDiff < 0) {
        newXYScale = (0.0056000003f * lowerAlpha) + 0.0014000001f;
        this->actor.scale.x = newXYScale;
        this->actor.scale.z = newXYScale;
        this->actor.scale.y = (0.007f - (0.007f * lowerAlpha)) + 0.007f;
    } else {
        Actor_SetScale(&this->actor, lowerAlpha * 0.007f);
        this->actor.world.pos.y = this->savedHeight + (lowerAlpha * 15.0f);
        lowerAlpha = 1.0f;
    }

    this->mainColor.r = 255.0f * lowerAlpha;
    this->mainColor.g = 200.0f * lowerAlpha;
    this->mainColor.b = 0;
}

void EnBigpo_SpawnScoopSoul(EnBigpo* this) {
    this->actor.draw = EnBigpo_DrawScoopSoul;
    this->actor.shape.yOffset = 0.0f;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.y = 0;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->mainColor.a = 0; // fully invisible
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.0f;
    this->savedHeight = this->actor.world.pos.y;
    Actor_PlaySfx(&this->actor, NA_SE_EV_METAL_BOX_BOUND); // misnamed?
    this->actionFunc = EnBigpo_ScoopSoulAppearing;
}

void EnBigpo_ScoopSoulAppearing(EnBigpo* this, PlayState* play) {
    this->savedHeight += 2.0f;
    EnBigpo_AdjustPoAlpha(this, 20); // increase visibility
    if (this->mainColor.a == 255) {  // fully visible
        EnBigpo_SetupScoopSoulIdle(this);
    }
}

void EnBigpo_SetupScoopSoulIdle(EnBigpo* this) {
    this->savedHeight = this->actor.world.pos.y;
    Actor_SetFocus(&this->actor, -10.0f);
    this->idleTimer = 400; // 20 seconds
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBigpo_ScoopSoulIdle;
}

void EnBigpo_ScoopSoulIdle(EnBigpo* this, PlayState* play) {
    DECR(this->idleTimer);
    if (Actor_HasParent(&this->actor, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->idleTimer == 0) {
        // took too long, soul is leaving
        Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH);
        EnBigpo_SetupScoopSoulLeaving(this);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MAX, 35.0f, 60.0f);
        this->actor.world.pos.y = (Math_SinF(this->idleTimer * (M_PIf / 20)) * 5.0f) + this->savedHeight;
    }
}

void EnBigpo_SetupScoopSoulLeaving(EnBigpo* this) {
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
    this->actionFunc = EnBigpo_ScoopSoulFadingAway;
}

void EnBigpo_ScoopSoulFadingAway(EnBigpo* this, PlayState* play) {
    EnBigpo_AdjustPoAlpha(this, -13);
    if (this->mainColor.a == 0) { // fully invisible
        Actor_Kill(&this->actor);
    }
}

void EnBigpo_InitDampeMainPo(EnBigpo* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBigpo_SelectRandomFireLocations;
}

/*
 * dampe fires are in 3/N random locations, here we pick them randomly
 */
void EnBigpo_SelectRandomFireLocations(EnBigpo* this, PlayState* play) {
    Actor* enemyPtr;
    EnBigpo* randomFirePo;
    s32 fireIndex;
    s32 randomIndex;
    s32 fireCount = 0;

    // count the number of possible fires we can find (4 in vanilla)
    for (enemyPtr = GET_FIRST_ENEMY(play); enemyPtr != NULL; enemyPtr = enemyPtr->next) {
        if ((enemyPtr->id == ACTOR_EN_BIGPO) && (enemyPtr->params == BIG_POE_TYPE_POSSIBLE_FIRE)) {
            fireCount++;
        }
    }

    // if not enough fires exist, just starting fighting immediately
    if (fireCount < ARRAY_COUNT(this->fires)) {
        this->actor.draw = EnBigpo_DrawMainBigpo;
        Actor_SetScale(&this->actor, 0.014f);
        EnBigpo_SetupWarpIn(this, play);
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
        this->actor.world.pos.y += 100.0f;
        return;
    }

    // for available possiblefires, pick three to be random fires
    for (fireIndex = 0; fireIndex < ARRAY_COUNT(this->fires); fireIndex++, fireCount--) {
        enemyPtr = GET_FIRST_ENEMY(play);
        randomIndex = ((s32)Rand_ZeroFloat(fireCount)) % fireCount;

        while (enemyPtr != NULL) {
            if ((enemyPtr->id == ACTOR_EN_BIGPO) && (enemyPtr->params == BIG_POE_TYPE_POSSIBLE_FIRE)) {
                if (randomIndex == 0) {
                    randomFirePo = (EnBigpo*)enemyPtr;
                    randomFirePo->actor.params = BIG_POE_TYPE_CHOSEN_FIRE;
                    Math_Vec3f_Copy(&this->fires[fireIndex].pos, &randomFirePo->actor.world.pos);
                    randomFirePo->actor.parent = (Actor*)this;
                    randomFirePo->actor.update = EnBigpo_UpdateFire;
                    Actor_ChangeCategory(play, &play->actorCtx, &randomFirePo->actor, ACTORCAT_PROP);
                    randomFirePo->unk20C = fireIndex;
                    randomFirePo->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                    // make invisible by size: 0
                    Actor_SetScale(&randomFirePo->actor, 0);

                    if (this->actor.child == NULL) {
                        this->actor.child = &randomFirePo->actor;
                    } else {
                        randomFirePo->actor.child = this->actor.child;
                        this->actor.child = &randomFirePo->actor;
                    }
                    break;
                }

                randomIndex--;
            }
            enemyPtr = enemyPtr->next;
        }
    }

    // remove unused fires
    for (enemyPtr = GET_FIRST_ENEMY(play); enemyPtr != NULL; enemyPtr = enemyPtr->next) {
        if ((enemyPtr->id == ACTOR_EN_BIGPO) && (enemyPtr->params == BIG_POE_TYPE_POSSIBLE_FIRE)) {
            randomFirePo = (EnBigpo*)enemyPtr;
            randomFirePo->actionFunc = EnBigpo_Die;
            randomFirePo->actor.update = EnBigpo_UpdateFire;
        }
    }

    EnBigpo_ChangeToFireCounting(this);
}

void EnBigpo_ChangeToFireCounting(EnBigpo* this) {
    this->actionFunc = EnBigpo_FireCounting;
}

/*
 * count fires already found by Dampe,
 * once enough: spawn big poe for fight
 */
void EnBigpo_FireCounting(EnBigpo* this, PlayState* play) {
    EnBigpo* firePo;
    s32 activatedFireCount = 0;

    for (firePo = (EnBigpo*)this->actor.child; firePo; firePo = (EnBigpo*)firePo->actor.child) {
        if ((firePo->actor.params == BIG_POE_TYPE_REVEALED_FIRE) && (firePo->actionFunc == EnBigpo_RevealedFireIdle)) {
            activatedFireCount++;
        }
    }

    if (activatedFireCount == ARRAY_COUNT(this->fires)) { // all fires found
        EnBigpo_SetupSpawnCutscene(this);
    }
}

void EnBigpo_SetupFlameCirclePositions(EnBigpo* this, PlayState* play) {
    EnBigpo* firePo;
    Vec3f subCamEye;

    this->idleTimer = 39;
    for (firePo = (EnBigpo*)this->actor.child; firePo; firePo = (EnBigpo*)firePo->actor.child) {
        EnBigpo_SetupFlameCircleCutscene(firePo);
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        subCamEye.x = (Math_SinS(this->actor.yawTowardsPlayer) * 360.0f) + this->actor.world.pos.x;
        subCamEye.y = this->actor.world.pos.y + 150.0f;
        subCamEye.z = (Math_CosS(this->actor.yawTowardsPlayer) * 360.0f) + this->actor.world.pos.z;
        Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &subCamEye);
    }

    this->actionFunc = EnBigpo_DoNothing;
}

void EnBigpo_DoNothing(EnBigpo* this, PlayState* play) {
}

void EnBigpo_InitHiddenFire(EnBigpo* this) {
    this->actor.draw = NULL;
    this->actionFunc = EnBigpo_WaitingForDampe;
}

/*
 * idle until dampe finds this file by
 * changing this file params from BIG_POE_TYPE_POSSIBLE_FIRE into BIG_POE_TYPE_REVEALED_FIRE
 */
void EnBigpo_WaitingForDampe(EnBigpo* this, PlayState* play) {
    if (this->actor.params == BIG_POE_TYPE_REVEALED_FIRE) {
        EnBigpo_SetupFireRevealed(this);
    }
}

void EnBigpo_Die(EnBigpo* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void EnBigpo_SetupFireRevealed(EnBigpo* this) {
    this->actor.draw = EnBigpo_RevealedFire;
    this->idleTimer = 15;
    this->actionFunc = EnBigpo_RevealedFireGrowing;
}

void EnBigpo_RevealedFireGrowing(EnBigpo* this, PlayState* play) {
    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.0005f)) {
        this->idleTimer--;
        if (this->idleTimer == 0) {
            EnBigpo_SetupRevealedFireIdle(this);
        }
    }
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = ((0.01f - this->actor.scale.x) * 0.5f) + 0.01f;
}

void EnBigpo_SetupRevealedFireIdle(EnBigpo* this) {
    this->idleTimer = 10000; // 8 minutes until the fire leaves
    this->actionFunc = EnBigpo_RevealedFireIdle;
}

void EnBigpo_RevealedFireIdle(EnBigpo* this, PlayState* play) {
    if (this->idleTimer > 0) {
        if (this->idleTimer == 0) {
            //! @bug: unreachable code
            this->actor.params = BIG_POE_TYPE_UNK5;
        }
    } else {
        if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.001f)) {
            this->actor.params = BIG_POE_TYPE_CHOSEN_FIRE;
            EnBigpo_InitHiddenFire(this);
        }
        this->actor.scale.z = this->actor.scale.x;
        this->actor.scale.y = ((0.01f - this->actor.scale.x) * 0.5f) + 0.01f;
    }
}

void EnBigpo_SetupFlameCircleCutscene(EnBigpo* this) {
    s16 flameHeight;

    this->idleTimer = 40; // 2 seconds
    flameHeight = this->actor.parent->yawTowardsPlayer + this->unk20C * 0x5555;
    this->actor.home.pos.x = Math_SinS(flameHeight) * 30.0f + this->actor.parent->world.pos.x;
    this->actor.home.pos.y = this->actor.parent->world.pos.y;
    this->actor.home.pos.z = Math_CosS(flameHeight) * 30.0f + this->actor.parent->world.pos.z;
    this->actionFunc = EnBigpo_FlameCircleCutscene;
}

/*
 * every frame, swirl the flames toward big poe as summoned
 */
void EnBigpo_FlameCircleCutscene(EnBigpo* this, PlayState* play) {
    Vec3f posDiff;
    f32 magnitude;

    this->idleTimer--;
    if (this->idleTimer == 0) {
        EnBigpo* parentPoh = (EnBigpo*)this->actor.parent;

        Flags_SetSwitch(play, this->switchFlag);
        Math_Vec3f_Copy(&parentPoh->fires[this->unk20C].pos, &this->actor.world.pos);
        Actor_Kill(&this->actor);
        if (this->unk20C == 0) {
            parentPoh->actor.draw = EnBigpo_DrawCircleFlames;
            Actor_SetScale(&parentPoh->actor, 0.01f);
            EnBigpo_SpawnCutsceneStage3(parentPoh);
            parentPoh->fireRadius = 30.0f;
        }
    } else {
        Math_Vec3f_Diff(&this->actor.world.pos, &this->actor.home.pos, &posDiff);
        magnitude = Math3D_Vec3fMagnitude(&posDiff);
        if (magnitude > 0.0001f) {
            Math_Vec3f_Scale(&posDiff, 1.0f / magnitude);
        }
        magnitude /= this->idleTimer;
        this->actor.world.pos.x -= magnitude * posDiff.x;
        this->actor.world.pos.y -= magnitude * posDiff.y;
        this->actor.world.pos.z -= magnitude * posDiff.z;
    }
}

void EnBigpo_UpdateColor(EnBigpo* this) {
    s32 bplus5;
    s32 bminus5;

    if (this->actionFunc == EnBigpo_CheckHealth) {
        if (this->actor.colorFilterTimer & 2) {
            this->mainColor.r = 0;
            this->mainColor.g = 0;
            this->mainColor.b = 0;
        } else {
            this->mainColor.r = 80; // teal? what about the red health?
            this->mainColor.g = 255;
            this->mainColor.b = 225;
        }
    } else {
        this->mainColor.r = CLAMP_MAX(this->mainColor.r + 5, 255);
        this->mainColor.g = CLAMP_MAX(this->mainColor.g + 5, 255);

        // this might be a triple ternary but it matches and is easier to read spread out
        bplus5 = this->mainColor.b + 5;
        if (this->mainColor.b > 210) {
            bminus5 = this->mainColor.b - 5;
            if (bminus5 < 210) {
                this->mainColor.b = 210;
            } else {
                this->mainColor.b = bminus5;
            }
        } else {
            if (bplus5 > 210) {
                this->mainColor.b = 210;
            } else {
                this->mainColor.b = bplus5;
            }
        }
    }
}

void EnBigpo_FlickerLanternLight(EnBigpo* this) {
    f32 rand = Rand_ZeroOne();

    this->lanternColor.r = ((s32)(rand * 30.0f)) + 225;
    this->lanternColor.g = ((s32)(rand * 100.0f)) + 155;
    this->lanternColor.b = ((s32)(rand * 160.0f)) + 95;
    this->lanternColor.a = ((s32)(rand * 30.0f)) + 220;
}

s32 EnBigpo_ApplyDamage(EnBigpo* this, PlayState* play) {
    if ((this->collider.base.acFlags & AC_HIT) && !(this->collider.base.acFlags & AC_HARD)) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.colChkInfo.damageEffect == 0xF) {
            return true;
        }

        if (Actor_ApplyDamage(&this->actor) == 0) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DEAD);
            Enemy_StartFinishingBlow(play, &this->actor);
            if (this->actor.params == BIG_POE_TYPE_SUMMONED) { // dampe type
                Audio_RestorePrevBgm();
            }
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_DAMAGE);
        }

        // light arrows
        if (this->actor.colChkInfo.damageEffect == 4) {
            this->drawDmgEffAlpha = 4.0f;
            this->drawDmgEffScale = 1.0f;
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                        this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                        CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
        EnBigpo_HitStun(this);
        return true;
    }
    return false;
}

void EnBigpo_Update(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    s32 pad;
    ColliderCylinder* thisCollider;

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->hoverHeightCycleTimer = 0;
        this->savedHeight = this->actor.world.pos.y;
    }

    if (EnBigpo_ApplyDamage(this, play) == 0) {
        if (this->actor.isLockedOn && (this->actionFunc != EnBigpo_WarpingOut) &&
            !(this->collider.base.acFlags & AC_HARD) && (this->actor.category == ACTORCAT_ENEMY)) {
            this->unk20C++;
        } else {
            this->unk20C = 0;
        }
        if (this->unk20C == 40) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_LAUGH);
            EnBigpo_SetupWarpOut(this);
        }
    }

    this->actionFunc(this, play);
    if ((this->actionFunc != EnBigpo_SpawnCutsceneStage6) && (this->actionFunc != EnBigpo_SpawnCutsceneStage4)) {
        Actor_MoveWithGravity(&this->actor);
    }
    if (this->actionFunc == EnBigpo_LanternFalling) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 27.0f, 60.0f, UPDBGCHECKINFO_FLAG_4);
    }

    if (this->actor.draw == EnBigpo_DrawScoopSoul) {
        Actor_SetFocus(&this->actor, -10.0f);
    } else {
        Actor_SetFocus(&this->actor, 42.0f);
    }

    EnBigpo_UpdateColor(this);
    EnBigpo_FlickerLanternLight(this);

    this->actor.shape.shadowAlpha = this->mainColor.a;
    thisCollider = &this->collider;
    Collider_UpdateCylinder(&this->actor, thisCollider);
    if (this->collider.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &thisCollider->base);
    }
    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &thisCollider->base);
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &thisCollider->base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
        if (this->mainColor.a != 255) { // NOT fully visible
            if (this->mainColor.a * (1.0f / 255.0f) < this->mainColor.a) {
                this->drawDmgEffAlpha = this->mainColor.a * (1.0f / 255.0f);
            }
        }
        this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.5f;
        this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 1.0f);
    }
}

/*
 * alt update func: the revealed fires under dampe's house
 */
void EnBigpo_UpdateFire(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;

    this->actor.shape.rot.y = BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)));
    this->actionFunc(this, play);
}

s32 EnBigpo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                             Gfx** gfx) {
    EnBigpo* this = (EnBigpo*)thisx;
    // not fully invisible
    if ((this->mainColor.a == 0) || (limbIndex == BIG_POE_LIMB_LANTERN) ||
        ((this->actionFunc == EnBigpo_BurnAwayDeath) && (this->idleTimer >= 2))) {
        *dList = NULL;
    }
    return false;
}

void EnBigpo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnBigpo* this = (EnBigpo*)thisx;
    s8 bodyPartIndex;
    Vec3f* v1ptr; // todo: figure out better names
    Vec3f* v2ptr;
    Vec3f unusedVec;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->actionFunc == EnBigpo_BurnAwayDeath) && (this->idleTimer >= 2) &&
        (limbIndex == BIG_POE_LIMB_HAT_AND_CLOAK)) {
        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx);
        gSPDisplayList((*gfx)++, &gBigPoeCrispyBodyDL);
    }

    if (limbIndex == BIG_POE_LIMB_LANTERN) {
        // we scale the vec3f... then do nothing with it?
        Matrix_MultVecY(1400.0f, &unusedVec);
        if ((this->actionFunc == EnBigpo_BurnAwayDeath) && (this->idleTimer > 18)) {
            if (this->actor.scale.x != 0.0f) {
                Matrix_Scale(0.014f / this->actor.scale.x, 0.014f / this->actor.scale.x, 0.014f / this->actor.scale.x,
                             MTXMODE_APPLY);
            }
        }
        Matrix_Get(&this->drawMtxF);
    }

    bodyPartIndex = sLimbToBodyParts[limbIndex];
    if (bodyPartIndex != BODYPART_NONE) {
        if (bodyPartIndex <= BIG_POE_BODYPART_2) {
            Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
        } else if (bodyPartIndex == BIG_POE_BODYPART_3) {
            Matrix_MultVecX(3000.0f, &this->bodyPartsPos[bodyPartIndex]);
        } else if (bodyPartIndex == BIG_POE_BODYPART_4) {
            Matrix_MultVecY(-2000.0f, &this->bodyPartsPos[bodyPartIndex]);
        } else {
            v2ptr = &this->bodyPartsPos[bodyPartIndex + 1];
            v1ptr = D_80B65084;
            // BIG_POE_BODYPART_5
            Matrix_MultVecX(-4000.0f, &this->bodyPartsPos[bodyPartIndex]);

            for (i = bodyPartIndex + 1; i < BIG_POE_BODYPART_MAX; i++) {
                Matrix_MultVec3f(v1ptr, v2ptr);
                v2ptr++;
                v1ptr++;
            }
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigpo_DrawMainBigpo(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    Gfx* dispHead;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->mainColor.a == 255) || (this->mainColor.a == 0)) {
        // fully visible OR fully transparent
        dispHead = POLY_OPA_DISP;
        gSPDisplayList(dispHead, gSetupDLs[SETUPDL_25]);
        gSPSegment(&dispHead[1], 0x0C, &D_801AEFA0); // empty display list for no transparency
        gSPSegment(&dispHead[2], 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->mainColor.r, this->mainColor.g, this->mainColor.b,
                                this->mainColor.a));
        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnBigpo_OverrideLimbDraw, EnBigpo_PostLimbDraw, &this->actor, &dispHead[3]);

    } else {
        dispHead = POLY_XLU_DISP;
        gSPDisplayList(dispHead, gSetupDLs[SETUPDL_25]);
        gSPSegment(&dispHead[1], 0x0C, &D_801AEF88); // transparency display list
        gSPSegment(&dispHead[2], 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->mainColor.r, this->mainColor.g, this->mainColor.b,
                                this->mainColor.a));
        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnBigpo_OverrideLimbDraw, EnBigpo_PostLimbDraw, &this->actor, &dispHead[3]);
    }

    // 71.428566f might be 500/7 context unknown
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, BIG_POE_BODYPART_MAX,
                            this->actor.scale.x * 71.428566f * this->drawDmgEffScale, 0.0f, this->drawDmgEffAlpha,
                            ACTOR_DRAW_DMGEFF_LIGHT_ORBS);

    Matrix_Put(&this->drawMtxF);
    EnBigpo_DrawLantern(&this->actor, play);
    if (this->actionFunc == EnBigpo_SpawnCutsceneStage6) {
        EnBigpo_DrawCircleFlames(&this->actor, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigpo_DrawScoopSoul(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (play->gameplayFrames * -15) % 512,
                                  0x20, 0x80, 0, 0, 0, -15));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, this->mainColor.a);

    gDPSetEnvColor(POLY_XLU_DISP++, this->mainColor.r, this->mainColor.g, this->mainColor.b, 255);

    Lights_PointNoGlowSetInfo(&this->fires[0].info, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->mainColor.r, this->mainColor.g, this->mainColor.b,
                              this->mainColor.a * 2);

    Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, &gBigPoeSoulDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigpo_DrawLantern(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    f32 magnitude;
    f32 magnitude2;
    Gfx* dispHead;
    Vec3f vec1;
    Vec3f vec2;
    Camera* cam = GET_ACTIVE_CAM(play);

    if (cam != NULL) {
        Math_Vec3f_Diff(&cam->eye, &cam->at, &vec1);
        magnitude = Math3D_Vec3fMagnitude(&vec1);
        magnitude2 = (magnitude > 1.0f) ? (20.0f / magnitude) : (20.0f);
        Math_Vec3f_Scale(&vec1, magnitude2);
    } else {
        Math_Vec3f_Copy(&vec1, &gZeroVec3f);
    }

    OPEN_DISPS(play->state.gfxCtx);

    // fully visible OR fully transparent
    if ((this->mainColor.a == 255) || (this->mainColor.a == 0)) {
        Scene_SetRenderModeXlu(play, 0, 1);
        dispHead = POLY_OPA_DISP;
    } else {
        Scene_SetRenderModeXlu(play, 1, 2);
        dispHead = POLY_XLU_DISP;
    }

    gSPDisplayList(&dispHead[0], gSetupDLs[SETUPDL_25]);

    gSPSegment(&dispHead[1], 0x0A, Gfx_EnvColor(play->state.gfxCtx, 160, 0, 255, this->mainColor.a));

    Matrix_MultVecY(1400.0f, &vec2);
    Lights_PointGlowSetInfo(&this->fires[0].info, vec2.x + vec1.x, vec2.y + vec1.y, vec2.z + vec1.z,
                            this->lanternColor.r, this->lanternColor.g, this->lanternColor.b, this->lanternColor.a);

    gDPSetEnvColor(&dispHead[2], this->lanternColor.r, this->lanternColor.g, this->lanternColor.b, this->mainColor.a);

    MATRIX_FINALIZE_AND_LOAD(&dispHead[3], play->state.gfxCtx);

    gSPDisplayList(&dispHead[4], &gBigPoeLanternMainDL);

    gSPDisplayList(&dispHead[5], &gBigPoeLanternPurpleTopDL);

    // fully transparent OR fully invisible
    if ((this->mainColor.a == 255) || (this->mainColor.a == 0)) {
        POLY_OPA_DISP = &dispHead[6];
    } else {
        POLY_XLU_DISP = &dispHead[6];
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigpo_DrawCircleFlames(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    s32 pad[3];
    s16 fireRadius;
    MtxF* mtfxPtr;
    s32 i;

    mtfxPtr = Matrix_GetCurrent();

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_RotateYS(BINANG_ROT180(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_NEW);
    if (this->actionFunc == EnBigpo_SpawnCutsceneStage6) {
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        fireRadius = 500;
    } else {
        Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
        fireRadius = TRUNCF_BINANG(thisx->scale.x * 500.0f * 100.0f);
    }
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (play->gameplayFrames * -20) % 512,
                                  0x20, 0x80, 0, 0, 0, -20));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, 255 - this->mainColor.a);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);

    for (i = 0; i < ARRAY_COUNT(this->fires); i++) {
        EnBigpoFireEffect* firePtr = &this->fires[i];

        Lights_PointNoGlowSetInfo(&this->fires[i].info, this->fires[i].pos.x, this->fires[i].pos.y,
                                  this->fires[i].pos.z, 170, 255, 255, fireRadius);
        mtfxPtr->xw = firePtr->pos.x;
        mtfxPtr->yw = firePtr->pos.y;
        mtfxPtr->zw = firePtr->pos.z;

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigpo_RevealedFire(Actor* thisx, PlayState* play) {
    EnBigpo* this = (EnBigpo*)thisx;
    EnBigpo* parent = (EnBigpo*)thisx->parent;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, (play->gameplayFrames * -20) % 512,
                                  0x20, 0x80, 0, 0, 0, -20));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);

    Lights_PointNoGlowSetInfo(&parent->fires[this->unk20C].info, thisx->world.pos.x, thisx->world.pos.y,
                              thisx->world.pos.z, 170, 255, 255, (s32)(thisx->scale.x * 500.0f * 100.0f));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
