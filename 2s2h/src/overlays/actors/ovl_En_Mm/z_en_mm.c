/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Rock Sirloin
 */

#include "z_en_mm.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMm_Init(Actor* thisx, PlayState* play);
void EnMm_Destroy(Actor* thisx, PlayState* play);
void EnMm_Update(Actor* thisx, PlayState* play);
void EnMm_Draw(Actor* thisx, PlayState* play);

void func_80965D3C(EnMm* this, PlayState* play);
void func_80965DB4(EnMm* this, PlayState* play);
void func_8096611C(EnMm* this, PlayState* play);
void EnMm_SetupAction(EnMm* this, EnMmActionFunc actionFunc);

ActorProfile En_Mm_Profile = {
    /**/ ACTOR_EN_MM,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnMm),
    /**/ EnMm_Init,
    /**/ EnMm_Destroy,
    /**/ EnMm_Update,
    /**/ EnMm_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0x01000202, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 6, 30, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 270, ICHAIN_STOP),
};

void EnMm_SetupAction(EnMm* this, EnMmActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void func_80965BBC(EnMm* this) {
    EnMm_SetupAction(this, func_8096611C);
    this->actor.room = -1;
    if (this->actor.parent->id == ACTOR_PLAYER) {
        gSaveContext.unk_1014 = 1;
    }
}

void EnMm_Init(Actor* thisx, PlayState* play) {
    EnMm* this = (EnMm*)thisx;
    EnMmActionFunc action;

    if ((this->actor.params >= 0) && (!CHECK_WEEKEVENTREG(WEEKEVENTREG_37_10) ||
                                      CHECK_WEEKEVENTREG(WEEKEVENTREG_37_08) || (gSaveContext.unk_1014 != 0))) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 50.0f, ActorShadow_DrawCircle, 1.2f);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    if (this->actor.parent != NULL) {
        func_80965BBC(this);
        return;
    }
    if (this->actor.csId > CS_ID_NONE) {
        action = func_80965D3C;
    } else {
        action = func_80965DB4;
    }
    EnMm_SetupAction(this, action);
}

void EnMm_Destroy(Actor* thisx, PlayState* play) {
    EnMm* this = (EnMm*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80965D3C(EnMm* this, PlayState* play) {
    s16 csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_StartWithPlayerCs(csId, &this->actor);
        EnMm_SetupAction(this, func_80965DB4);
    } else {
        CutsceneManager_Queue(csId);
    }
}

void func_80965DB4(EnMm* this, PlayState* play) {
    s16 direction;
    Vec3f slopeNormal;
    s16 downwardSlopeYaw;
    f32 temp_f14;
    f32 temp_f12;
    f32 temp_f2;
    s32 angle;
    s32 pad;

    if (Actor_HasParent(&this->actor, play)) {
        func_80965BBC(this);
        Actor_PlaySfx(&this->actor, NA_SE_PL_PULL_UP_ROCK);
    } else {
        if ((this->actor.velocity.y > 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
            this->actor.velocity.y = 0.0f;
        }

        if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            angle = BINANG_SUB(this->actor.world.rot.y, BINANG_ROT180(this->actor.wallYaw));
            this->actor.world.rot.y += BINANG_SUB(0x8000, (s16)(angle * 2));
            this->actor.speed *= 0.5f;
            CollisionCheck_SpawnShieldParticles(play, &this->actor.world.pos);
            Actor_PlaySfx(&this->actor, NA_SE_EV_HUMAN_BOUND);
        }

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Math_StepToF(&this->actor.speed, 0.0f, 0.08f);
        } else {
            temp_f14 = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
            temp_f12 = Math_CosS(this->actor.world.rot.y) * this->actor.speed;
            Actor_GetSlopeDirection(this->actor.floorPoly, &slopeNormal, &downwardSlopeYaw);
            temp_f14 += 3.0f * slopeNormal.x;
            temp_f12 += 3.0f * slopeNormal.z;
            temp_f2 = sqrtf(SQ(temp_f14) + SQ(temp_f12));

            if ((temp_f2 < this->actor.speed) ||
                (SurfaceType_GetFloorEffect(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ==
                 FLOOR_EFFECT_1)) {
                this->actor.speed = CLAMP_MAX(temp_f2, 16.0f);
                this->actor.world.rot.y = Math_Atan2S_XY(temp_f12, temp_f14);
            }

            if (!Math_StepToF(&this->actor.speed, 0.0f, 1.0f)) {
                direction = this->actor.world.rot.y;
                if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->actor.shape.rot.y)) > 0x4000) {
                    direction = BINANG_ROT180(direction);
                }
                Math_ScaledStepToS(&this->actor.shape.rot.y, direction, this->actor.speed * 100.0f);
                this->unk_190 += TRUNCF_BINANG(this->actor.speed * 800.0f);
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
                if (this->actor.velocity.y < -6.0f) {
                    this->actor.velocity.y *= -0.3f;
                    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                }

                Actor_PlaySfx(&this->actor, NA_SE_EV_HUMAN_BOUND);
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_NONE, 50.0f, 30.0f);
            }
        }

        Actor_MoveWithGravity(&this->actor);
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_8096611C(EnMm* this, PlayState* play) {
    if (Actor_HasNoParent(&this->actor, play)) {
        EnMm_SetupAction(this, func_80965DB4);
        this->actor.room = play->roomCtx.curRoom.num;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        Math_Vec3s_ToVec3f(&this->actor.prevPos, &this->actor.home.rot);
        gSaveContext.unk_1014 = 0;
    } else {
        Math_Vec3f_ToVec3s(&this->actor.home.rot, &this->actor.parent->world.pos);
    }
    Math_ScaledStepToS(&this->unk_190, 0, 0x7D0);
}

void EnMm_Update(Actor* thisx, PlayState* play) {
    EnMm* this = (EnMm*)thisx;

    Collider_ResetCylinderAC(play, &this->collider.base);
    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 10.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    Actor_SetFocus(&this->actor, 20.0f);
}

void EnMm_Draw(Actor* thisx, PlayState* play) {
    EnMm* this = (EnMm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->unk_190 != 0) {
        s16 rotY = this->actor.world.rot.y - this->actor.shape.rot.y;

        Matrix_RotateYS(rotY, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_190, MTXMODE_APPLY);
        Matrix_RotateYS(-rotY, MTXMODE_APPLY);
    }
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_055628);

    CLOSE_DISPS(play->state.gfxCtx);
}
