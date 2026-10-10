/*
 * File: z_en_gamelupy.c
 * Overlay: ovl_En_Gamelupy
 * Description: Deku Scrub Playground Rupees
 */

#include "z_en_gamelupy.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnGamelupy_Init(Actor* thisx, PlayState* play);
void EnGamelupy_Destroy(Actor* thisx, PlayState* play);
void EnGamelupy_Update(Actor* thisx, PlayState* play);
void EnGamelupy_Draw(Actor* thisx, PlayState* play);

void EnGamelupy_FindSharedMemory(EnGamelupy* this, PlayState* play);
void EnGamelupy_Idle(EnGamelupy* this, PlayState* play);
void EnGamelupy_Collected(EnGamelupy* this, PlayState* play);
void EnGamelupy_SetupFindSharedMemory(EnGamelupy* this);
void EnGamelupy_SetupIdle(EnGamelupy* this);
void EnGamelupy_SetupCollected(EnGamelupy* this);

ActorProfile En_Gamelupy_Profile = {
    /**/ ACTOR_EN_GAMELUPY,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnGamelupy),
    /**/ EnGamelupy_Init,
    /**/ EnGamelupy_Destroy,
    /**/ EnGamelupy_Update,
    /**/ EnGamelupy_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static TexturePtr sRupeeTextures[] = {
    gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeeOrangeTex, gRupeePurpleTex,
};

static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };

static Color_RGBA8 sEnvColor = { 100, 200, 0, 255 };

void EnGamelupy_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGamelupy* this = (EnGamelupy*)thisx;

    Actor_SetScale(&this->actor, 0.03f);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
    this->actor.shape.yOffset = 700.0f;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.gravity = -0.5f;
    this->actor.shape.rot.y = Rand_Next();
    this->collectedTimer = 0;
    this->sparklesAngle = 0;
    this->sparklesAngleStep = 0x7D0;
    if (EN_GAMELUPY_GET_TYPE(&this->actor) == ENGAMELUPY_TYPE_BLUE) {
        this->type = ENGAMELUPY_TYPE_BLUE;
    } else {
        this->type = ENGAMELUPY_TYPE_GREEN;
    }
    EnGamelupy_SetupFindSharedMemory(this);
}

void EnGamelupy_Destroy(Actor* thisx, PlayState* play) {
    EnGamelupy* this = (EnGamelupy*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGamelupy_SpawnSparkles(EnGamelupy* this, PlayState* play) {
    Vec3f pos;
    Vec3f accel;
    Vec3f velocity;

    pos = this->actor.world.pos;
    velocity.x = Math_SinS(this->sparklesAngle) * 3.0f;
    velocity.y = 5.5f;
    velocity.z = Math_CosS(this->sparklesAngle) * 3.0f;
    accel.x = -0.05f * velocity.x;
    accel.y = -0.4f;
    accel.z = -0.05f * velocity.z;
    EffectSsKirakira_SpawnDispersed(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, 3000, 40);
    this->sparklesAngle += this->sparklesAngleStep;
}

void EnGamelupy_SetupFindSharedMemory(EnGamelupy* this) {
    this->actionFunc = EnGamelupy_FindSharedMemory;
}

void EnGamelupy_FindSharedMemory(EnGamelupy* this, PlayState* play) {
    s16* minigameScore = Actor_FindSharedMemoryEntry(play, ACTOR_EN_GAMELUPY);

    if (minigameScore != NULL) {
        this->minigameScore = minigameScore;
        EnGamelupy_SetupIdle(this);
    }
}

void EnGamelupy_SetupIdle(EnGamelupy* this) {
    this->actionFunc = EnGamelupy_Idle;
}

void EnGamelupy_Idle(EnGamelupy* this, PlayState* play) {
    if (GameInteractor_Should(VB_COLLECT_PLAYGROUND_RUPEE, this->collider.base.ocFlags1 & OC1_HIT, this)) {
        *this->minigameScore += ENGAMELUPY_POINTS;
        if (this->type == ENGAMELUPY_TYPE_BLUE) {
            Rupees_ChangeBy(5);
        } else {
            Rupees_ChangeBy(1);
        }
        EnGamelupy_SetupCollected(this);
    }
    this->actor.shape.rot.y += 0x1F4;
}

void EnGamelupy_SetupCollected(EnGamelupy* this) {
    this->collectedTimer = 0;
    this->actor.gravity = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_SY_GET_RUPY);
    this->actionFunc = EnGamelupy_Collected;
}

void EnGamelupy_Collected(EnGamelupy* this, PlayState* play) {
    f32 scale;
    Player* player = GET_PLAYER(play);

    if (this->collectedTimer > 30) {
        Actor_Kill(&this->actor);
    } else {
        this->collectedTimer++;
        this->actor.world.pos = player->actor.world.pos;
        this->actor.world.pos.y += 40.0f;
        scale = (30.0f - this->collectedTimer) * 0.001f;
        Actor_SetScale(&this->actor, scale);
        EnGamelupy_SpawnSparkles(this, play);
    }
    this->actor.shape.rot.y += 0x3E8;
}

void EnGamelupy_UpdateCollision(EnGamelupy* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnGamelupy_Update(Actor* thisx, PlayState* play) {
    EnGamelupy* this = (EnGamelupy*)thisx;

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 32.0f, 30.0f, 0.0f, UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8);
    EnGamelupy_UpdateCollision(this, play);
}

void EnGamelupy_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGamelupy* this = (EnGamelupy*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sRupeeTextures[this->type]));
    gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
