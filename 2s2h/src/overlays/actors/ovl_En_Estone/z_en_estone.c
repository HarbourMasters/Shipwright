/*
 * File: z_en_estone.c
 * Overlay: ovl_En_Estone
 * Description: Rubble from Eyegore's attacks
 */

#include "z_en_estone.h"
#include "objects/object_eg/object_eg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnEstone_Init(Actor* thisx, PlayState* play);
void EnEstone_Destroy(Actor* thisx, PlayState* play);
void EnEstone_Update(Actor* thisx, PlayState* play2);
void EnEstone_Draw(Actor* thisx, PlayState* play2);

void EnEstone_Active(EnEstone* this, PlayState* play);
void EnEstone_Inactive(EnEstone* this, PlayState* play);
void EnEstone_SpawnEffect(EnEstone* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 lifetime);
void EnEstone_UpdateEffects(EnEstone* this, PlayState* play);
void EnEstone_DrawEffects(EnEstone* this, PlayState* play);

ActorProfile En_Estone_Profile = {
    /**/ ACTOR_EN_ESTONE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_EG,
    /**/ sizeof(EnEstone),
    /**/ EnEstone_Init,
    /**/ EnEstone_Destroy,
    /**/ EnEstone_Update,
    /**/ EnEstone_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 30, 30, -10, { 0, 0, 0 } },
};

void EnEstone_Init(Actor* thisx, PlayState* play) {
    EnEstone* this = (EnEstone*)thisx;
    Vec3f accel;
    Vec3f velocity;
    f32 scale;
    s32 i;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 15.0f);
    this->rotVel.x = this->rotVel.y = this->rotVel.z = Rand_CenteredFloat(1.0f) * 20.0f;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.world.rot.y += TRUNCF_BINANG(Rand_CenteredFloat(0x2710));
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.params == ENESTONE_TYPE_LARGE) {
        this->actor.speed = Rand_ZeroFloat(5.0f) + 2.0f;
        this->scale = (Rand_ZeroFloat(1.0f) * 0.005f) + 0.005f;
        this->actor.velocity.y = Rand_ZeroFloat(10.0f) + 15.0f;
        this->actor.gravity = -2.0f;
    } else { // ENESTONE_TYPE_SMALL
        this->actor.speed = Rand_ZeroFloat(3.0f) + 1.0f;
        this->scale = (Rand_ZeroFloat(1.0f) * 0.003f) + 0.003f;
        this->actor.velocity.y = Rand_ZeroFloat(5.0f) + 7.0f;
        this->actor.gravity = -1.0f;
    }

    this->collider.dim.radius = this->scale * 3000.0f;
    this->collider.dim.height = this->scale * 3000.0f;
    this->collider.dim.yShift = this->scale * -1300.0f;

    for (i = 0; i < 7; i++) {
        accel.x = 0.5f * (Rand_ZeroOne() - 0.5f);
        accel.y = -1.0f;
        accel.z = 0.5f * (Rand_ZeroOne() - 0.5f);

        velocity.x = 2.0f * (Rand_ZeroOne() - 0.5f);
        velocity.y = 8.0f + (Rand_ZeroOne() * 10.0f);
        velocity.z = 2.0f * (Rand_ZeroOne() - 0.5f);

        scale = (Rand_ZeroFloat(1.0f) * 0.001f) + 0.001f;

        EnEstone_SpawnEffect(this, &this->actor.world.pos, &velocity, &accel, scale, 70);
    }
    this->timer = 5;
    this->inactive = false;
    Actor_SetScale(&this->actor, 1.0f);
    this->actionFunc = EnEstone_Active;
}

void EnEstone_Destroy(Actor* thisx, PlayState* play) {
    EnEstone* this = (EnEstone*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnEstone_Active(EnEstone* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f accel;
    Vec3f velocity;
    s32 i;

    if ((this->actor.floorHeight == -10000.0f) || ((player->actor.world.pos.y - 200.0f) > this->actor.world.pos.y)) {
        Actor_Kill(&this->actor);
        return;
    }
    if (((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y < 0.0f)) ||
        (this->collider.base.atFlags & AT_BOUNCED)) {
        this->collider.base.atFlags &= ~AT_BOUNCED;
        if (this->actor.params == ENESTONE_TYPE_LARGE) {
            for (i = 0; i < 2; i++) {
                accel.x = 2.0f * (Rand_ZeroOne() - 0.5f);
                accel.y = -1.0f;
                accel.z = 2.0f * (Rand_ZeroOne() - 0.5f);
                velocity.x = 3.0f * (Rand_ZeroOne() - 0.5f);
                velocity.y = 5.0f + (Rand_ZeroOne() * 10.0f);
                velocity.z = 3.0f * (Rand_ZeroOne() - 0.5f);
                EnEstone_SpawnEffect(this, &this->actor.world.pos, &velocity, &accel,
                                     0.003f + (0.002f * Rand_ZeroFloat(1.0f)), 20);
            }
        }
        Math_Vec3f_Copy(&velocity, &this->actor.world.pos);
        velocity.y = this->actor.floorHeight;
        Actor_SpawnFloorDustRing(play, &this->actor, &velocity, 0.0f, 10, 6.0f, 50, 30, true);
        this->actor.velocity.y = this->actor.gravity = 0.0f;
        this->actor.speed *= 0.3f;
        this->actor.shape.shadowScale = 0.0f;
        this->inactive = true;
        this->timer = 50;
        Actor_PlaySfx(&this->actor, NA_SE_EV_WALL_BROKEN);
        this->actionFunc = EnEstone_Inactive;
    } else {
        this->actor.shape.shadowScale = 4500.0f * this->scale;
        Actor_SetScale(&this->actor, this->scale);
    }
}

void EnEstone_Inactive(EnEstone* this, PlayState* play) {
    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnEstone_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnEstone* this = (EnEstone*)thisx;

    DECR(this->timer);

    this->actionFunc(this, play);

    this->rot.x -= this->rotVel.x;
    this->rot.y -= this->rotVel.y;
    this->rot.z -= this->rotVel.z;

    Actor_MoveWithGravity(&this->actor);
    if ((this->timer == 0) && !this->inactive) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 50.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    }
    if (!this->inactive) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
    EnEstone_UpdateEffects(this, play);
}

void EnEstone_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnEstone* this = (EnEstone*)thisx;

    if (this->inactive != true) {
        Matrix_Push();

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateXFApply(DEG_TO_RAD(this->rot.x));
        Matrix_RotateYF(DEG_TO_RAD(this->rot.y), MTXMODE_APPLY);
        Matrix_RotateZF(DEG_TO_RAD(this->rot.z), MTXMODE_APPLY);
        Matrix_Scale(this->scale, this->scale, this->scale, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gEyegoreStoneDL);

        CLOSE_DISPS(play->state.gfxCtx);

        Matrix_Pop();
    }
    EnEstone_DrawEffects(this, play);
}

void EnEstone_SpawnEffect(EnEstone* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 lifetime) {
    EnEstoneEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isActive) {
            effect->isActive = true;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->scale = scale;
            effect->timer = lifetime;
            effect->rot.x = Rand_CenteredFloat(0x7530);
            effect->rot.y = Rand_CenteredFloat(0x7530);
            effect->rot.z = Rand_CenteredFloat(0x7530);
            break;
        }
    }
}

void EnEstone_UpdateEffects(EnEstone* this, PlayState* play) {
    EnEstoneEffect* effect = this->effects;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            effect->rot.x += 0x100;
            effect->rot.z += 0x130;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.y += effect->accel.y;
            if ((effect->pos.y < this->actor.floorHeight) && (effect->velocity.y < 0.0f)) {
                effect->timer = 0;
                Actor_SpawnFloorDustRing(play, &this->actor, &effect->pos, 0.0f, 5, 3.0f, 30, 15, true);
            }
            if (effect->timer != 0) {
                effect->timer--;
            } else {
                effect->isActive = false;
            }
        }
    }
}

void EnEstone_DrawEffects(EnEstone* this, PlayState* play) {
    EnEstoneEffect* effect = this->effects;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isActive) {
            FrameInterpolation_RecordOpenChild(effect, i);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
            Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);
            Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}
