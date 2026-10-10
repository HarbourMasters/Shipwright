/*
 * File: z_en_boom.c
 * Overlay: ovl_En_Boom
 * Description: Zora boomerangs
 */

#include "z_en_boom.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnBoom_Init(Actor* thisx, PlayState* play);
void EnBoom_Destroy(Actor* thisx, PlayState* play);
void EnBoom_Update(Actor* thisx, PlayState* play);
void EnBoom_Draw(Actor* thisx, PlayState* play);

void EnBoom_SetupAction(EnBoom* this, EnBoomActionFunc actionFunc);
void func_808A2918(EnBoom* this, PlayState* play);

ActorProfile En_Boom_Profile = {
    /**/ ACTOR_EN_BOOM,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBoom),
    /**/ EnBoom_Init,
    /**/ EnBoom_Destroy,
    /**/ EnBoom_Update,
    /**/ EnBoom_Draw,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000010, 0x00, 0x02 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_VEC3S(shape.rot, 0, ICHAIN_STOP),
};

void EnBoom_SetupAction(EnBoom* this, EnBoomActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void func_808A24DC(EnBoom* this, PlayState* play) {
    WaterBox* waterBox;
    f32 sp50 = this->actor.world.pos.y;
    u16 sp4E = this->actor.bgCheckFlags & BGCHECKFLAG_WATER;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp50, &waterBox) &&
        (this->actor.world.pos.y < sp50)) {
        Vec3f sp40;

        this->actor.bgCheckFlags |= BGCHECKFLAG_WATER;
        sp40.x = this->actor.world.pos.x;
        sp40.y = this->actor.world.pos.y - 20.0f;
        sp40.z = this->actor.world.pos.z;
        EffectSsBubble_Spawn(play, &sp40, 20.0f, 10.0f, 20.0f, 0.13f);
    } else {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) &&
        ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) != sp4E)) {
        Vec3f sp34;

        Math_Vec3f_Diff(&this->actor.world.pos, &this->actor.prevPos, &sp34);
        if (sp34.y != 0.0f) {
            f32 temp_fv0 = sqrtf(SQ(sp34.x) + SQ(sp34.z));

            if (temp_fv0 != 0.0f) {
                temp_fv0 = (((sp50 - this->actor.prevPos.y) / sp34.y) * temp_fv0) / temp_fv0;
            }

            sp34.x = this->actor.prevPos.x + (sp34.x * temp_fv0);
            sp34.y = sp50;
            sp34.z = this->actor.prevPos.z + (sp34.z * temp_fv0);
            EffectSsGSplash_Spawn(play, &sp34, NULL, NULL, 0, 300);
        }

        Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_INTO_WATER_L);

        EffectSsGRipple_Spawn(play, &sp34, 100, 500, 0);
        EffectSsGRipple_Spawn(play, &sp34, 100, 500, 4);
        EffectSsGRipple_Spawn(play, &sp34, 100, 500, 8);
    }

    this->actor.bgCheckFlags |= BGCHECKFLAG_WATER_TOUCH;
}

void EnBoom_Init(Actor* thisx, PlayState* play) {
    static u8 D_808A3068[4] = { 255, 255, 100, 255 };
    static u8 D_808A306C[4] = { 255, 255, 100, 64 };
    static u8 D_808A3070[4] = { 255, 255, 100, 0 };
    static u8 D_808A3074[4] = { 255, 255, 100, 0 };
    s32 pad;
    EnBoom* this = (EnBoom*)thisx;
    EffectBlureInit1 sp30;
    s32 i;

    this->actor.room = -1;
    Actor_ProcessInitChain(&this->actor, sInitChain);

    Color_RGBA8 trailColor =
        CosmeticEditor_GetChangedColor(255, 255, 100, 255, COSMETIC_ID("Trails.ZoraBoomerangTrail"));

    for (i = 0; i < 4; i++) {
        sp30.p1StartColor[i] = (i < 3) ? ((i == 0)   ? trailColor.r
                                          : (i == 1) ? trailColor.g
                                                     : trailColor.b)
                                       : D_808A3068[i];
        sp30.p2StartColor[i] = (i < 3) ? ((i == 0)   ? trailColor.r
                                          : (i == 1) ? trailColor.g
                                                     : trailColor.b)
                                       : D_808A306C[i];
        sp30.p1EndColor[i] = (i < 3) ? ((i == 0)   ? trailColor.r
                                        : (i == 1) ? trailColor.g
                                                   : trailColor.b)
                                     : D_808A3070[i];
        sp30.p2EndColor[i] = (i < 3) ? ((i == 0)   ? trailColor.r
                                        : (i == 1) ? trailColor.g
                                                   : trailColor.b)
                                     : D_808A3074[i];
    }

    sp30.elemDuration = 8;
    sp30.unkFlag = 0;
    sp30.calcMode = 0;

    Effect_Add(play, &this->effectIndex, EFFECT_BLURE1, 0, 0, &sp30);

    Collider_InitQuad(play, &this->collider);
    Collider_SetQuad(play, &this->collider, &this->actor, &sQuadInit);
    EnBoom_SetupAction(this, func_808A2918);
}

void EnBoom_Destroy(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;
    Player* player = GET_PLAYER(play);
    Actor* otherZoraBoomerangActor;

    if (player != NULL) {
        Effect_Destroy(play, this->effectIndex);
        Collider_DestroyQuad(play, &this->collider);

        otherZoraBoomerangActor = this->actor.child;
        if (otherZoraBoomerangActor != NULL) {
            otherZoraBoomerangActor->parent = NULL;
            player->zoraBoomerangActor = otherZoraBoomerangActor;
        } else {
            otherZoraBoomerangActor = this->actor.parent;
            if (otherZoraBoomerangActor != NULL) {
                otherZoraBoomerangActor->child = NULL;
            } else {
                player->zoraBoomerangActor = NULL;
                player->stateFlags1 &= ~PLAYER_STATE1_ZORA_BOOMERANG_THROWN;
            }
        }
        player->stateFlags3 |= PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT;
    }
}

void func_808A2918(EnBoom* this, PlayState* play) {
    Actor* targetActor;
    Player* player = GET_PLAYER(play);
    s32 sp74;
    s16 sp72;
    s16 sp70;
    s16 sp6E;
    s16 sp6C;
    Actor* temp;
    f32 sp64;
    f32 distXYZ;

    targetActor = this->moveTo;

    if (targetActor != NULL) {
        sp72 = Actor_WorldYawTowardPoint(&this->actor, &targetActor->focus.pos);
        sp70 = this->actor.world.rot.y - sp72;
        sp6E = Actor_WorldPitchTowardPoint(&this->actor, &targetActor->focus.pos);
        sp6C = this->actor.world.rot.x - sp6E;

        sp64 = (200.0f - Math_Vec3f_DistXYZ(&this->actor.world.pos, &targetActor->focus.pos)) * 0.005f;
        if (sp64 < 0.12f) {
            sp64 = 0.12f;
        }

        if ((&player->actor != targetActor) &&
            ((targetActor->update == NULL) || ((this->unk_1CF < 0) && (ABS_ALT(sp70) > 0x4000)))) {
            this->moveTo = NULL;
        } else {
            if (this->unk_1CE != 0) {
                this->unk_1CE--;
                this->actor.world.rot.y += (this->actor.params == ZORA_BOOMERANG_LEFT) ? 0x190 : -0x190;
            } else {
                if (this->unk_1CF > 0) {
                    this->unk_1CF--;
                } else {
                    this->unk_1CF = -1;
                }

                Math_ScaledStepToS(&this->actor.world.rot.y, sp72,
                                   (this->unk_1CF > 0) ? 0x3E8 : TRUNCF_BINANG(ABS_ALT(sp70) * sp64));
            }

            Math_ScaledStepToS(&this->actor.world.rot.x, sp6E, ABS_ALT(sp6C) * sp64);
        }
    }

    Actor_SetSpeeds(&this->actor, 12.0f);
    Actor_MoveWithGravity(&this->actor);
    func_808A24DC(this, play);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_IT_BOOMERANG_FLY - SFX_FLAG);

    if ((this->collider.base.atFlags & AT_HIT) && (((this->collider.base.at->id == ACTOR_EN_ITEM00) &&
                                                    (this->collider.base.at->params != ITEM00_HEART_CONTAINER) &&
                                                    (this->collider.base.at->params != ITEM00_HEART_PIECE)) ||
                                                   (this->collider.base.at->id == ACTOR_EN_SI))) {
        this->unk_1C8 = this->collider.base.at;
        if (this->collider.base.at->id == ACTOR_EN_SI) {
            this->collider.base.at->flags |= ACTOR_FLAG_HOOKSHOT_ATTACHED;
        }
    }

    if (DECR(this->unk_1CC) != 0) {
        s32 bgId;
        Vec3f sp50;
        s32 pad;

        sp74 = BgCheck_EntityLineTest1(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &sp50,
                                       &this->actor.wallPoly, true, true, true, true, &bgId);
        if (sp74) {
            if (func_800B90AC(play, &this->actor, this->actor.wallPoly, bgId, &sp50)) {
                sp74 = false;
            } else {
                CollisionCheck_SpawnShieldParticlesMetal(play, &sp50);
            }
        }

        if (sp74) {
            this->actor.world.rot.x = -this->actor.world.rot.x;
            this->actor.world.rot.y += 0x8000;
            this->moveTo = &player->actor;
            this->unk_1CC = 0;
        }
    }

    if (this->unk_1CC <= 16) {
        distXYZ = Math_Vec3f_DistXYZ(&this->actor.world.pos, &player->actor.focus.pos);

        if (&player->actor != this->moveTo) {
            if (this->moveTo == 0) {
                this->unk_1CE = 8;
            }
            this->moveTo = &player->actor;
        }

        if (distXYZ < 40.0f) {
            targetActor = this->unk_1C8;
            if (targetActor != NULL) {
                Math_Vec3f_Copy(&targetActor->world.pos, &player->actor.world.pos);
                if (targetActor->id == ACTOR_EN_ITEM00) {
                    targetActor->gravity = -0.9f;
                    targetActor->bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
                } else {
                    targetActor->flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
                }
            }
            Actor_Kill(&this->actor);
        }
    }

    targetActor = this->unk_1C8;
    if (targetActor != NULL) {
        if (targetActor->update == NULL) {
            this->unk_1C8 = NULL;
        } else {
            Math_Vec3f_Copy(&targetActor->world.pos, &this->actor.world.pos);
        }
    }
}

void EnBoom_Update(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;
    Player* player = GET_PLAYER(play);
    Actor* actor;

    if (!(player->stateFlags1 & PLAYER_STATE1_20000000)) {
        this->actionFunc(this, play);

        if (((actor = this->actor.child) != NULL) || ((actor = this->actor.parent) != NULL)) {
            this->actor.focus.pos.x = (this->actor.world.pos.x + actor->world.pos.x) * 0.5f;
            this->actor.focus.pos.y = (this->actor.world.pos.y + actor->world.pos.y) * 0.5f;
            this->actor.focus.pos.z = (this->actor.world.pos.z + actor->world.pos.z) * 0.5f;
        } else {
            Actor_SetFocus(&this->actor, 0.0f);
        }

        if (this->actor.params != ZORA_BOOMERANG_LEFT) {
            this->unk_1CD--;
        } else {
            this->unk_1CD++;
        }
    }
}

typedef struct {
    /* 0x00 */ Gfx* unk_00;
    /* 0x04 */ Vec3f unk_04;
    /* 0x10 */ Vec3f unk_10;
} EnBoomStruct; // size = 0x1C

EnBoomStruct D_808A3078[] = {
    { gameplay_keep_DL_06FE20, { -960.0f, 0.0f, 0.0f }, { 960.0f, 0.0f, 0.0f } },
    { gameplay_keep_DL_06FF68, { -960.0f, 0.0f, 0.0f }, { 960.0f, 0.0f, 0.0f } },
};

void EnBoom_Draw(Actor* thisx, PlayState* play) {
    EnBoom* this = (EnBoom*)thisx;
    EnBoomStruct* sp58 = &D_808A3078[this->actor.params];
    Vec3f sp4C;
    Vec3f sp40;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS((this->actor.params != ZORA_BOOMERANG_LEFT) ? 0x1F40 : -0x1F40, MTXMODE_APPLY);
    Matrix_RotateXS(this->actor.world.rot.x, MTXMODE_APPLY);
    Matrix_MultVec3f(&sp58->unk_04, &sp4C);
    Matrix_MultVec3f(&sp58->unk_10, &sp40);

    if (func_80126440(play, &this->collider, &this->weaponInfo, &sp4C, &sp40)) {
        EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &sp4C, &sp40);
    }

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_RotateYS(this->unk_1CD * 0x2EE0, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, sp58->unk_00);

    CLOSE_DISPS(play->state.gfxCtx);
}
