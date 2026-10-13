/*
 * File: z_en_bombal.c
 * Overlay: ovl_En_Bombal
 * Description: Bombers - Majora Balloon
 */

#include "z_en_bombal.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "assets/objects/object_fusen/object_fusen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBombal_Init(Actor* thisx, PlayState* play);
void EnBombal_Destroy(Actor* thisx, PlayState* play);
void EnBombal_Update(Actor* thisx, PlayState* play);
void EnBombal_Draw(Actor* thisx, PlayState* play);

void func_80C05B3C(EnBombal* this, PlayState* play);
void func_80C05C44(EnBombal* this, PlayState* play);
void func_80C05DE8(EnBombal* this, PlayState* play);
void func_80C05B24(EnBombal* this);
void EnBombal_InitEffects(EnBombal* this, Vec3f* pos, s16 fadeDelay);
void EnBombal_UpdateEffects(EnBombal* this, PlayState* play);
void EnBombal_DrawEffects(EnBombal* this, PlayState* play);

ActorProfile En_Bombal_Profile = {
    /**/ ACTOR_EN_BOMBAL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_FUSEN,
    /**/ sizeof(EnBombal),
    /**/ EnBombal_Init,
    /**/ EnBombal_Destroy,
    /**/ EnBombal_Update,
    /**/ EnBombal_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x004138B0, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 60, 90, -50, { 0, 0, 0 } },
};

void EnBombal_Init(Actor* thisx, PlayState* play) {
    EnBombal* this = (EnBombal*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    this->actor.colChkInfo.mass = 0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.colChkInfo.health = 1;
    this->scale = 0.1f;
    this->csId = this->actor.csId;
    func_80C05B24(this);
}

void EnBombal_Destroy(Actor* thisx, PlayState* play) {
    EnBombal* this = (EnBombal*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80C05B24(EnBombal* this) {
    this->isPopped = false;
    this->actionFunc = func_80C05B3C;
}

void func_80C05B3C(EnBombal* this, PlayState* play) {
    Player* player;

    this->oscillationAngle += 1500.0f;
    this->actor.velocity.y = Math_SinS(this->oscillationAngle);
    Math_ApproachF(&this->scale, 0.1f, 0.3f, 0.01f);

    if (play->msgCtx.msgLength == 0) {
        if (this->collider.base.acFlags & AC_HIT) {
            player = GET_PLAYER(play);
            this->collider.base.acFlags &= ~AC_HIT;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) &&
                !CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
                player->stateFlags1 |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
                this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
            }
            this->actionFunc = func_80C05C44;
        }
    }
}

void func_80C05C44(EnBombal* this, PlayState* play) {
    s32 phi_s0 = false;
    s32 i;
    Vec3f effPos;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) &&
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csId);
            return;
        }

        if (!CutsceneManager_IsNext(this->csId)) {
            CutsceneManager_Queue(this->csId);
        } else {
            CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
            phi_s0 = true;
        }
    } else {
        phi_s0 = true;
    }

    if (phi_s0) {
        Math_Vec3f_Copy(&effPos, &this->actor.world.pos);
        effPos.y += 60.0f;
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y, effPos.z, 255, 255, 200,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_EXPLOSION));

        for (i = 0; i < 100; i++) {
            EnBombal_InitEffects(this, &effPos, 10);
        }

        SET_WEEKEVENTREG(WEEKEVENTREG_83_04);
        Actor_PlaySfx(&this->actor, NA_SE_EV_MUJURA_BALLOON_BROKEN);
        this->timer = 30;
        this->isPopped = true;
        this->actionFunc = func_80C05DE8;
    }
}

void func_80C05DE8(EnBombal* this, PlayState* play) {
    if (this->timer == 0) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
            CutsceneManager_Stop(this->csId);
        }
        Actor_Kill(&this->actor);
        return;
    }

    if (this->timer < 10) {
        this->actor.colChkInfo.health = 0;
    }
}

void EnBombal_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombal* this = (EnBombal*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;

    Actor_SetFocus(&this->actor, 30.0f);
    Actor_SetScale(&this->actor, this->scale);

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    EnBombal_UpdateEffects(this, play);

    if (!this->isPopped) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnBombal_Draw(Actor* thisx, PlayState* play) {
    EnBombal* this = (EnBombal*)thisx;

    if (this->isPopped != true) {
        Gfx_DrawDListOpa(play, gMajoraBalloonDL);
        Gfx_DrawDListOpa(play, gMajoraBalloonKnotDL);
    }
    EnBombal_DrawEffects(this, play);
}

void EnBombal_InitEffects(EnBombal* this, Vec3f* pos, s16 fadeDelay) {
    s16 i;
    EnBombalEffect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->pos = *pos;
            effect->alphaFadeDelay = fadeDelay;
            effect->alpha = 255;

            effect->accel.x = (Rand_ZeroOne() - 0.5f) * 10.0f;
            effect->accel.y = (Rand_ZeroOne() - 0.5f) * 10.0f;
            effect->accel.z = (Rand_ZeroOne() - 0.5f) * 10.0f;

            effect->velocity.x = Rand_ZeroOne() - 0.5f;
            effect->velocity.y = Rand_ZeroOne() - 0.5f;
            effect->velocity.z = Rand_ZeroOne() - 0.5f;

            effect->scale = (Rand_ZeroFloat(1.0f) * 0.5f) + 2.0f;
            return;
        }
    }
}

void EnBombal_UpdateEffects(EnBombal* this, PlayState* play) {
    s32 i;
    EnBombalEffect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (effect->alphaFadeDelay != 0) {
                effect->alphaFadeDelay--;
            } else {
                effect->alpha -= 10;
                if (effect->alpha < 10) {
                    effect->isEnabled = 0;
                }
            }
        }
    }
}

void EnBombal_DrawEffects(EnBombal* this, PlayState* play) {
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    EnBombalEffect* effect = this->effects;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Opa(gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);

            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(&gSun1Tex));

            gSPDisplayList(POLY_XLU_DISP++, &gSunSparkleMaterialDL);

            gDPPipeSync(POLY_XLU_DISP++);

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 255, effect->alpha);

            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZF(DEG_TO_RAD(play->state.frames * 20.0f), MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, &gSunSparkleModelDL);
        }
    }
    CLOSE_DISPS(gfxCtx);
}
