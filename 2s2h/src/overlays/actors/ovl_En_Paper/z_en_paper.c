/**
 * @file z_en_paper.c
 * Overlay: ovl_En_Paper
 * Description: Tingle confetti group
 *
 * Complex actor for its length, that creates and manages a group of confetti effects. Each is a triangle of a
 * particular colour, that rotates around a fixed axis at a randomly-chosen fixed velocity, and is subject to gravity,
 * drag and wind forces.
 */

#include "z_en_paper.h"
#include "objects/object_bal/object_bal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnPaper_Init(Actor* thisx, PlayState* play);
void EnPaper_Destroy(Actor* thisx, PlayState* play);
void EnPaper_Update(Actor* thisx, PlayState* play);
void EnPaper_Draw(Actor* thisx, PlayState* play);

void EnPaper_SetupSpreadConfettiGroup(EnPaper* this);
void EnPaper_SpreadConfettiGroup(EnPaper* this, PlayState* play);
void EnPaper_SetupFlyConfettiGroup(EnPaper* this);
void EnPaper_FlyConfettiGroup(EnPaper* this, PlayState* play);
void EnPaper_InitConfettiPiece(EnPaper* this, EnPaperConfetto* piece);
void EnPaper_FlyConfettiPiece(EnPaper* this, EnPaperConfetto* piece);

ActorProfile En_Paper_Profile = {
    /**/ ACTOR_EN_PAPER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_BAL,
    /**/ sizeof(EnPaper),
    /**/ EnPaper_Init,
    /**/ EnPaper_Destroy,
    /**/ EnPaper_Update,
    /**/ EnPaper_Draw,
};

static Vec3f sUnitVecZ = { 0.0f, 0.0f, 1.0f };

void EnPaper_Init(Actor* thisx, PlayState* play) {
    EnPaper* this = (EnPaper*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    this->timer = 70;
    this->windForce = sUnitVecZ;
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnPaper_SetupSpreadConfettiGroup(this);
}

void EnPaper_Destroy(Actor* thisx, PlayState* play) {
}

void EnPaper_SetupSpreadConfettiGroup(EnPaper* this) {
    this->actionFunc = EnPaper_SpreadConfettiGroup;
}

/**
 * Set up the pieces of confetti.
 */
void EnPaper_SpreadConfettiGroup(EnPaper* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->pieces); i++) {
        EnPaper_InitConfettiPiece(this, &this->pieces[i]);
    }

    EnPaper_SetupFlyConfettiGroup(this);
}

void EnPaper_SetupFlyConfettiGroup(EnPaper* this) {
    this->actionFunc = EnPaper_FlyConfettiGroup;
}

/**
 * Main action function for the group.
 */
void EnPaper_FlyConfettiGroup(EnPaper* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->pieces); i++) {
        EnPaper_FlyConfettiPiece(this, &this->pieces[i]);
    }
}

void EnPaper_InitConfettiPiece(EnPaper* this, EnPaperConfetto* piece) {
    // Pick rotation axis randomly (significantly biased towards the z = 0 plane)
    Matrix_RotateZYX(Rand_Next(), Rand_Next(), Rand_Next(), MTXMODE_NEW);
    Matrix_MultVec3f(&sUnitVecZ, &piece->rotAxis);

    // copy actor position and distribute uniformly in a cube of side 2 around it
    piece->pos = this->actor.world.pos;
    piece->pos.x += Rand_Centered() * 4.0f;
    piece->pos.y += Rand_Centered() * 4.0f;
    piece->pos.z += Rand_Centered() * 4.0f;

    // copy actor velocity and distrbute uniformly in a cuboid with sides 9, 6, 9 with actor.velocity in the middle
    // of the base.
    piece->velocity = this->actor.velocity;
    piece->velocity.x += Rand_Centered() * 9.0f;
    piece->velocity.y += Rand_ZeroOne() * 6.0f;
    piece->velocity.z += Rand_Centered() * 9.0f;

    // Choose random starting angle and angular velocity
    piece->angle = Rand_Next();
    piece->angVel = (Rand_Next() >> 4) + (0x10000 / 180);

    // Rotate the unit Z-vector by the random starting axis and angle
    Matrix_RotateAxisS(piece->angle, &piece->rotAxis, MTXMODE_NEW);
    Matrix_MultVec3f(&sUnitVecZ, &piece->normal);
}

/**
 * Main falling function for a single piece, handles dynamics.
 *
 * Position \f$ \mathbf{x} \f$ essentially satisfies discretised version of the equation
 *
 * \f[
 *     \ddot{\mathbf{x}} = -0.2 \mathbf{n} (\dot{\mathbf{x}} + \mathbf{W}) + \mathbf{g} ,
 * \f]
 *
 * where
 *
 * - \f$ \mathbf{n} \f$ is the unit normal to the confetti triangle's plane
 * - \f$ \mathbf{W} \f$ is the random wind pressure
 * - \f$ \mathbf{g} \f$ is gravity
 *
 * and the product is componentwise: the normal is being used as a cross-sectional area measure rather than vectorially.
 */
void EnPaper_FlyConfettiPiece(EnPaper* this, EnPaperConfetto* piece) {
    f32 cos = Math_CosS(piece->angle);
    f32 sin = Math_SinS(piece->angle);
    f32 versin = 1.0f - cos;

    if (piece->pos.y < (this->actor.floorHeight - 40.0f)) {
        return;
    }

    // acceleration due to gravity
    piece->velocity.y += this->actor.gravity;

    // drag and wind force: normal is used to simulate cross-section size of piece, although
    piece->velocity.x -= 0.2f * fabsf(piece->normal.x) * (piece->velocity.x + this->windForce.x);
    piece->velocity.y -= 0.2f * fabsf(piece->normal.y) * (piece->velocity.y + this->windForce.y);
    piece->velocity.z -= 0.2f * fabsf(piece->normal.z) * (piece->velocity.z + this->windForce.z);

    // rotate around axis
    piece->angle += piece->angVel;

    // move
    piece->pos.x += piece->velocity.x;
    piece->pos.y += piece->velocity.y;
    piece->pos.z += piece->velocity.z;

    // Rotate unit Z vector about `axis` by `angle` to get forward direction. This is the same calculation as at the
    // bottom of EnPaper_InitConfettiPiece(), but done manually instead of using any matrix functions.
    piece->normal.x = (versin * piece->rotAxis.x * piece->rotAxis.z) - (piece->rotAxis.y * sin);
    piece->normal.y = (versin * piece->rotAxis.y * piece->rotAxis.z) + (piece->rotAxis.x * sin);
    piece->normal.z = (versin * piece->rotAxis.z * piece->rotAxis.z) + cos;
}

#define WIND_PITCH_BOUND (0x10000 / 12)

/**
 * Sets the wind force for the whole group, using the shape.rot from the previous frame, and picks a new random one
 * for the next frame.
 */
void EnPaper_UpdateWind(EnPaper* this) {
    f32 strength = (Rand_Centered() * 4.0f) + 6.0f;
    f32 cosX;

    this->windForce.y = Math_SinS(this->actor.shape.rot.x) * -strength;
    cosX = Math_CosS(this->actor.shape.rot.x) * -strength;
    this->windForce.x = Math_SinS(this->actor.shape.rot.y) * cosX;
    this->windForce.z = Math_CosS(this->actor.shape.rot.y) * cosX;

    // New random wind direction. A uniform distribution of the angles in spherical coordinates is not uniformly
    // distributed on the sphere, so this is biased more towards up and down.
    this->actor.shape.rot.x += (s16)(Rand_Next() >> 8);
    this->actor.shape.rot.y += (s16)(Rand_Next() >> 6);

    // Essentially a clamp
    if (ABS_ALT(this->actor.shape.rot.x) > WIND_PITCH_BOUND) {
        if (this->actor.shape.rot.x > 0) {
            this->actor.shape.rot.x = WIND_PITCH_BOUND;
        } else {
            this->actor.shape.rot.x = -WIND_PITCH_BOUND;
        }
    }
}

void EnPaper_Update(Actor* thisx, PlayState* play) {
    EnPaper* this = (EnPaper*)thisx;

    this->actionFunc(this, play);
    EnPaper_UpdateWind(this);

    if (this->timer == 0) {
        Actor_Kill(&this->actor);
        return;
    }

    this->timer--;
}

void EnPaper_Draw(Actor* thisx, PlayState* play) {
    EnPaper* this = (EnPaper*)thisx;
    EnPaperConfetto* piece = this->pieces;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, SHADE, 0, 0, 0, SHADE, PRIMITIVE, 0, COMBINED, 0, 0, 0, 0, COMBINED);
    gSPLoadGeometryMode(POLY_OPA_DISP++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH);

    for (i = 0; i < ARRAY_COUNT(this->pieces); i++, piece++) {
        // colour the first 30 white and 10 each of the other 3 colours
        if (i == 0) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        } else if (i == 30) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 32, 32, 255);
        } else if (i == 40) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 64, 128, 255, 255);
        } else if (i == 50) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 64, 255, 128, 255);
        }

        if ((this->actor.floorHeight - 40.0f) < piece->pos.y) {
            Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx));

            Mtx_SetTranslationRotationScaleMtx(mtx, this->actor.scale.x, this->actor.scale.y, this->actor.scale.z,
                                               piece->angle, piece->rotAxis.x, piece->rotAxis.y, piece->rotAxis.z,
                                               piece->pos.x, piece->pos.y, piece->pos.z);

            gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gTingleConfettiDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
