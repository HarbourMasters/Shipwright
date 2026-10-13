/*
 * File: z_bg_ikana_dharma.c
 * Overlay: ovl_Bg_Ikana_Dharma
 * Description: Stone Tower Temple - Punchable Pillar Segments
 */

#include "z_bg_ikana_dharma.h"
#include "z64quake.h"
#include "assets/objects/object_ikana_obj/object_ikana_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgIkanaDharma_Init(Actor* thisx, PlayState* play2);
void BgIkanaDharma_Destroy(Actor* thisx, PlayState* play);
void BgIkanaDharma_Update(Actor* thisx, PlayState* play);
void BgIkanaDharma_Draw(Actor* thisx, PlayState* play);
void BgIkanaDharma_Reset(void);

void BgIkanaDharma_SetupWaitForHit(BgIkanaDharma* this);
void BgIkanaDharma_WaitForHit(BgIkanaDharma* this, PlayState* play);
void BgIkanaDharma_SetupStartCutscene(BgIkanaDharma* this);
void BgIkanaDharma_StartCutscene(BgIkanaDharma* this, PlayState* play);
void BgIkanaDharma_SetupWaitForCutsceneToEnd(BgIkanaDharma* this);
void BgIkanaDharma_WaitForCutsceneToEnd(BgIkanaDharma* this, PlayState* play);

ActorProfile Bg_Ikana_Dharma_Profile = {
    /**/ ACTOR_BG_IKANA_DHARMA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKANA_OBJ,
    /**/ sizeof(BgIkanaDharma),
    /**/ BgIkanaDharma_Init,
    /**/ BgIkanaDharma_Destroy,
    /**/ BgIkanaDharma_Update,
    /**/ BgIkanaDharma_Draw,
    /**/ BgIkanaDharma_Reset,
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
        { 0x00000000, 0x00, 0x00 },
        { 0x00000100, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 98, 10, 25, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 320, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 320, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1100, ICHAIN_STOP),
};

static BgIkanaDharma* sFirstHitBgIkanaDharma;

void BgIkanaDharma_SpawnEffects(BgIkanaDharma* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    s32 i;
    f32 initialRadius = this->dyna.actor.scale.x * 200.0f;
    f32 initialY = this->dyna.actor.scale.y * 50.0f;

    for (i = 0; i < 4; i++) {
        f32 speed = (Rand_ZeroOne() * 5.0f) + 5.0f;
        s16 angle = (Rand_Next() >> 0x12) + this->dyna.actor.world.rot.y + 0x6000;
        f32 dirX = Math_SinS(angle);
        f32 dirZ = Math_CosS(angle);

        pos.x = (dirX * initialRadius) + this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y + initialY;
        pos.z = (dirZ * initialRadius) + this->dyna.actor.world.pos.z;
        velocity.x = speed * dirX;
        velocity.y = Rand_ZeroOne() + 0.5f;
        velocity.z = speed * dirZ;
        accel.x = velocity.x * -0.05f;
        accel.y = -0.15f;
        accel.z = velocity.z * -0.05f;
        EffectSsKirakira_SpawnSmallYellow(play, &pos, &velocity, &accel);
    }
}

void BgIkanaDharma_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgIkanaDharma* this = (BgIkanaDharma*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.scale.x = 0.3f;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.3f;
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gStoneTowerTemplePunchablePillarCol);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    if (!BGIKANADHARMA_IS_CHILD(&this->dyna.actor)) {
        f32 segmentY = this->dyna.actor.world.pos.y;
        s32 numSegments = BGIKANADHARMA_NUM_SEGMENTS(&this->dyna.actor);
        s32 i;

        for (i = 0; i < numSegments; i++) {
            segmentY += 60.0f;
            Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_BG_IKANA_DHARMA, this->dyna.actor.world.pos.x,
                                          segmentY, this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                                          this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z,
                                          BGIKANADHARMA_PARAMS(0, true, 0), this->dyna.actor.csId,
                                          this->dyna.actor.halfDaysBits, NULL);
        }

        this->dyna.actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
    }

    BgIkanaDharma_SetupWaitForHit(this);
}

void BgIkanaDharma_Destroy(Actor* thisx, PlayState* play) {
    BgIkanaDharma* this = (BgIkanaDharma*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
    if (sFirstHitBgIkanaDharma == this) {
        sFirstHitBgIkanaDharma = NULL;
    }
}

void BgIkanaDharma_SetupWaitForHit(BgIkanaDharma* this) {
    this->actionFunc = BgIkanaDharma_WaitForHit;
    this->dyna.actor.speed = 0.0f;
}

void BgIkanaDharma_WaitForHit(BgIkanaDharma* this, PlayState* play) {
    s32 wasHit = (this->collider.base.acFlags & AC_HIT) != 0;
    Player* player = GET_PLAYER(play);
    BgIkanaDharma* this2 = this;
    s32 tempAngle1;
    s32 tempAngle2;

    if (wasHit) {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    if (wasHit && (sFirstHitBgIkanaDharma == NULL)) {
        sFirstHitBgIkanaDharma = this2;
        Flags_SetSwitch(play, BGIKANADHARMA_GET_SWITCH_FLAG(&this->dyna.actor));
        tempAngle1 = BINANG_ADD(this->dyna.actor.yawTowardsPlayer, 0x8000);
        tempAngle2 = (BINANG_SUB(player->actor.shape.rot.y, tempAngle1) >> 1);
        this->dyna.actor.world.rot.y = tempAngle1 + tempAngle2 + 0xF000;
        this->dyna.actor.speed = 20.0f;
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_DARUMA_VANISH);
        BgIkanaDharma_SetupStartCutscene(this);
    } else if (CHECK_FLAG_ALL(this->dyna.actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
               (sFirstHitBgIkanaDharma == NULL) && (this->dyna.actor.xzDistToPlayer < 420.0f)) {
        tempAngle1 = BINANG_SUB(this->dyna.actor.yawTowardsPlayer, player->actor.shape.rot.y);
        tempAngle1 = ABS_ALT(tempAngle1);

        if (tempAngle1 > 0x4000) {
            Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void BgIkanaDharma_SetupStartCutscene(BgIkanaDharma* this) {
    CutsceneManager_Queue(this->dyna.actor.csId);
    this->actionFunc = BgIkanaDharma_StartCutscene;
}

void BgIkanaDharma_StartCutscene(BgIkanaDharma* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        BgIkanaDharma_SetupWaitForCutsceneToEnd(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgIkanaDharma_SetupWaitForCutsceneToEnd(BgIkanaDharma* this) {
    this->cutsceneFramesRemaining = 10;
    this->actionFunc = BgIkanaDharma_WaitForCutsceneToEnd;
}

void BgIkanaDharma_WaitForCutsceneToEnd(BgIkanaDharma* this, PlayState* play) {
    if (this->cutsceneFramesRemaining > 0) {
        this->cutsceneFramesRemaining--;

        if (this->cutsceneFramesRemaining == 0) {
            if (sFirstHitBgIkanaDharma == this) {
                sFirstHitBgIkanaDharma = NULL;
            }

            CutsceneManager_Stop(this->dyna.actor.csId);
        }
    }

    if (Math_StepToF(&this->dyna.actor.scale.y, 0.0f, 1.0f / 300.0f)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    this->dyna.actor.scale.x = this->dyna.actor.scale.y * 3.0f;
    this->dyna.actor.scale.z = this->dyna.actor.scale.y * 3.0f;
    BgIkanaDharma_SpawnEffects(this, play);
}

void BgIkanaDharma_Update(Actor* thisx, PlayState* play) {
    BgIkanaDharma* this = (BgIkanaDharma*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc == BgIkanaDharma_WaitForHit) {
        DynaPolyActor* actorBelow;
        s32 pad[2];

        func_800B4AEC(play, &this->dyna.actor, 30.0f);
        actorBelow = DynaPoly_GetActor(&play->colCtx, this->dyna.actor.floorBgId);
        if (actorBelow == NULL) {
            Actor_MoveWithGravity(&this->dyna.actor);
            Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
            if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
                s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

                Quake_SetSpeed(quakeIndex, 21536);
                Quake_SetPerturbations(quakeIndex, 4, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 12);

                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            }
        } else {
            if (actorBelow->actor.id == ACTOR_BG_IKANA_DHARMA) {
                this->dyna.actor.world.pos.y = actorBelow->actor.world.pos.y + 60.0f;
            } else {
                this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            }

            this->dyna.actor.velocity.y = 0.0f;
        }
    } else {
        f32 wallCheckRadius = this->dyna.actor.scale.x * 300.0f;
        wallCheckRadius = CLAMP_MIN(wallCheckRadius, 2.0f);

        Actor_MoveWithGravity(&this->dyna.actor);
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, wallCheckRadius, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }

    Actor_SetFocus(&this->dyna.actor, 40.0f);
}

void BgIkanaDharma_Draw(Actor* thisx, PlayState* play) {
    BgIkanaDharma* this = (BgIkanaDharma*)thisx;

    Gfx_DrawDListOpa(play, gStoneTowerTemplePunchablePillarDL);
}

void BgIkanaDharma_Reset(void) {
    sFirstHitBgIkanaDharma = NULL;
}
