/*
 * File: z_en_bom.c
 * Overlay: ovl_En_Bom
 * Description: Bomb / Powder Keg
 */

#include "z_en_bom.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnBom_Init(Actor* thisx, PlayState* play);
void EnBom_Destroy(Actor* thisx, PlayState* play);
void EnBom_Update(Actor* thisx, PlayState* play);
void EnBom_Draw(Actor* thisx, PlayState* play);

void EnBom_Move(EnBom* this, PlayState* play);
void EnBom_WaitForRelease(EnBom* this, PlayState* play);

void func_80872648(PlayState* play, Vec3f* arg1);
void func_808726DC(PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, s32 arg4);
void EnBom_DrawKeg(PlayState* play, s32 arg1);

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ s16 rotY;
    /* 0x1A */ s16 rotX;
} PowderKegFuseSegment; // size = 0x1C

PowderKegFuseSegment sPowderKegFuseSegments[16];

ActorProfile En_Bom_Profile = {
    /**/ ACTOR_EN_BOM,
    /**/ ACTORCAT_EXPLOSIVES,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnBom),
    /**/ EnBom_Init,
    /**/ EnBom_Destroy,
    /**/ EnBom_Update,
    /**/ EnBom_Draw,
};

static f32 enBomScales[] = { 0.01f, 0.03f };

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00013828, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 11, 14, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit1[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000008, 0x00, 0x02 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit1 = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit1),
    sJntSphElementsInit1,
};

static ColliderJntSphElementInit sJntSphElementsInit2[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x80000008, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit2 = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit2),
    sJntSphElementsInit2,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_STOP),
};

void EnBom_Init(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;
    s32 params;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 700.0f, ActorShadow_DrawCircle, 16.0f);
    this->actor.colChkInfo.mass = 200;
    this->actor.colChkInfo.cylRadius = 5;
    this->actor.colChkInfo.cylHeight = 10;

    this->flashSpeedScale = 7;
    this->isPowderKeg = ENBOM_GET_1(&this->actor);
    if (this->isPowderKeg) {
        play->actorCtx.flags |= ACTORCTX_FLAG_0;
        this->timer = gSaveContext.powderKegTimer;
    } else {
        this->timer = 70;
    }

    Collider_InitCylinder(play, &this->collider1);
    Collider_InitJntSph(play, &this->collider2);
    Collider_SetCylinder(play, &this->collider1, &this->actor, &sCylinderInit);

    if (!this->isPowderKeg) {
        Collider_SetJntSph(play, &this->collider2, &this->actor, &sJntSphInit1, this->collider2Elements);
        this->collider1.dim.radius = 6;
        this->collider1.dim.height = 11;
    } else {
        Collider_SetJntSph(play, &this->collider2, &this->actor, &sJntSphInit2, this->collider2Elements);
        this->collider1.dim.radius = 20;
        this->collider1.dim.height = 36;
        func_80872648(play, &this->actor.world.pos);
    }

    this->collider2Elements[0].base.atDmgInfo.damage += ENBOM_GET_FF00(thisx);
    this->actor.shape.rot.z &= 0xFF;
    if (ENBOM_GET_80(&this->actor)) {
        this->actor.shape.rot.z |= 0xFF00;
    }

    this->collider2.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider2.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->collider2.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;

    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = EnBom_WaitForRelease;
        this->actor.room = -1;
        Actor_SetScale(&this->actor, enBomScales[this->isPowderKeg]);
    } else {
        this->actionFunc = EnBom_Move;
        gSaveContext.powderKegTimer = 0;
    }
}

void EnBom_Destroy(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;

    Collider_DestroyJntSph(play, &this->collider2);
    Collider_DestroyCylinder(play, &this->collider1);
    if (this->isPowderKeg) {
        play->actorCtx.flags &= ~ACTORCTX_FLAG_0;
    }
}

void EnBom_Move(EnBom* this, PlayState* play) {
    static Vec3f D_80872E68[] = {
        { 2.0f, -6.0f, -0.3f },
        { 1.5f, -5.0f, -0.6f },
        { 0.2f, -6.0f, -0.1f },
    };

    // if bomb has a parent actor, the bomb hasnt been released yet
    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = EnBom_WaitForRelease;
        this->actor.room = -1;
        return;
    }

    if ((this->actor.velocity.y > 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
        this->actor.velocity.y = -this->actor.velocity.y;
    }

    // rebound bomb off the wall it hits
    if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        s16 yDiff = BINANG_SUB(this->actor.wallYaw, this->actor.world.rot.y);

        if (ABS_ALT(yDiff) > 0x4000) {
            this->actor.world.rot.y =
                BINANG_SUB(this->actor.wallYaw - this->actor.world.rot.y + this->actor.wallYaw, 0x8000);
        }

        Actor_PlaySfx(&this->actor, this->isPowderKeg ? NA_SE_EV_PUT_DOWN_WOODBOX : NA_SE_EV_BOMB_BOUND);
        Actor_MoveWithGravity(&this->actor);
        this->actor.speed *= 0.7f;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Math_StepToF(&this->actor.speed, 0.0f, 0.08f);
    } else {
        Vec3f* sp58;
        FloorType floorType = SurfaceType_GetFloorType(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        Vec3f slopeNormal;
        s16 downwardSlopeYaw;
        f32 sp40;
        f32 sp3C;
        f32 sp38;

        sp58 = &D_80872E68[this->isPowderKeg];

        if (floorType == FLOOR_TYPE_5) {
            sp58 = &D_80872E68[2];
        }

        if ((floorType == FLOOR_TYPE_4) || (floorType == FLOOR_TYPE_14) || (floorType == FLOOR_TYPE_15)) {
            s16 sp36;

            Math_ApproachF(&this->actor.shape.yOffset, 0.0f, 0.1f, 50.0f);
            sp36 = this->actor.shape.shadowAlpha;
            Math_ApproachS(&sp36, 0, 1, 20);
            this->actor.shape.shadowAlpha = sp36;
        } else {
            Math_ApproachF(&this->actor.shape.yOffset, 700.0f, 1.0f, 700.0f);
        }

        sp40 = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
        sp3C = Math_CosS(this->actor.world.rot.y) * this->actor.speed;
        Actor_GetSlopeDirection(this->actor.floorPoly, &slopeNormal, &downwardSlopeYaw);

        sp40 += 3.0f * slopeNormal.x;
        sp3C += 3.0f * slopeNormal.z;
        sp38 = sqrtf(SQ(sp40) + SQ(sp3C));

        if ((sp38 < this->actor.speed) || (SurfaceType_GetFloorEffect(&play->colCtx, this->actor.floorPoly,
                                                                      this->actor.floorBgId) == FLOOR_EFFECT_1)) {
            if (sp38 > 16.0f) {
                this->actor.speed = 16.0f;
            } else {
                this->actor.speed = sp38;
            }
            this->actor.world.rot.y = Math_Atan2S_XY(sp3C, sp40);
        }

        if (!Math_StepToF(&this->actor.speed, 0.0f, sp58->x)) {
            s16 temp = this->actor.world.rot.y;
            s32 pad;

            if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->actor.shape.rot.y)) > 0x4000) {
                temp = BINANG_ROT180(temp);
            }
            Math_ScaledStepToS(&this->actor.shape.rot.y, temp, this->actor.speed * 100.0f);
            this->unk_1FA += TRUNCF_BINANG(this->actor.speed * 800.0f);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_PlaySfx(&this->actor, this->isPowderKeg ? NA_SE_EV_TRE_BOX_BOUND : NA_SE_EV_BOMB_BOUND);
            if (this->actor.velocity.y < sp58->y) {
                if ((floorType == FLOOR_TYPE_4) || (floorType == FLOOR_TYPE_14) || (floorType == FLOOR_TYPE_15)) {
                    this->actor.velocity.y = 0.0f;
                } else {
                    this->actor.velocity.y *= sp58->z;
                }
                this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            }
        } else if (this->timer >= 4) {
            Actor_OfferCarry(&this->actor, play);
        }
    }

    Actor_MoveWithGravity(&this->actor);
}

void EnBom_WaitForRelease(EnBom* this, PlayState* play) {
    // if parent pointer is NULL, the bomb has been released
    if (Actor_HasNoParent(&this->actor, play)) {
        this->actionFunc = EnBom_Move;
        this->actor.room = play->roomCtx.curRoom.num;
        this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        Math_Vec3s_ToVec3f(&this->actor.prevPos, &this->actor.home.rot);
        if (this->isPowderKeg) {
            gSaveContext.powderKegTimer = 0;
        }
        EnBom_Move(this, play);
    } else {
        Math_Vec3f_ToVec3s(&this->actor.home.rot, &this->actor.parent->world.pos);
        if (this->isPowderKeg) {
            gSaveContext.powderKegTimer = this->timer;
        }
    }
    Math_ScaledStepToS(&this->unk_1FA, 0, 0x7D0);
}

void EnBom_Explode(EnBom* this, PlayState* play) {
    static s16 D_80872E8C[] = { 100, 200 };
    static Color_RGBA8 D_80872E90 = { 185, 140, 70, 255 };
    static Color_RGBA8 D_80872E94 = { 255, 255, 255, 255 };
    s32 i;
    FloorType floorType;
    f32 temp_f20;
    s32 pad;
    f32 spCC;
    Vec3f spC0;
    Vec3f spB4;
    CollisionPoly* spB0;
    s32 bgId;
    Vec3f spA0;
    Vec3f sp94;
    Vec3f sp88;
    Color_RGBA8 sp84;
    Color_RGBA8 sp80;

    if (this->collider2.elements[0].dim.modelSphere.radius == 0) {
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        Rumble_Request(this->actor.xzDistToPlayer, 255, 20, 150);
    }

    this->collider2.elements[0].dim.worldSphere.radius = D_80872E8C[this->isPowderKeg];
    if (this->timer == 7) {
        this->collider2.base.atFlags &= ~AT_TYPE_ENEMY;
    }

    if (this->actor.params == BOMB_TYPE_EXPLOSION) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
    }

    if (play->envCtx.adjLightSettings.light1Color[0] != 0) {
        play->envCtx.adjLightSettings.light1Color[0] -= 25;
    }

    if (play->envCtx.adjLightSettings.light1Color[1] != 0) {
        play->envCtx.adjLightSettings.light1Color[1] -= 25;
    }

    if (play->envCtx.adjLightSettings.light1Color[2] != 0) {
        play->envCtx.adjLightSettings.light1Color[2] -= 25;
    }

    if (play->envCtx.adjLightSettings.ambientColor[0] != 0) {
        play->envCtx.adjLightSettings.ambientColor[0] -= 25;
    }

    if (play->envCtx.adjLightSettings.ambientColor[1] != 0) {
        play->envCtx.adjLightSettings.ambientColor[1] -= 25;
    }

    if (play->envCtx.adjLightSettings.ambientColor[2] != 0) {
        play->envCtx.adjLightSettings.ambientColor[2] -= 25;
    }

    if (this->timer == 0) {
        func_80123590(play, &this->actor);
        Actor_Kill(&this->actor);
    }

    if ((this->timer & 1) == 0) {
        spCC = Rand_ZeroFloat(M_PIf);

        for (i = 0; i < 15; i++) {
            Matrix_RotateYF(((2.0f * (i * M_PIf)) / 15.0f) + spCC, MTXMODE_NEW);
            Matrix_MultVecZ((10 - this->timer) * 300.0f * 0.1f, &spC0);
            spB4.x = this->actor.world.pos.x + spC0.x;
            spB4.y = this->actor.world.pos.y + 500.0f;
            spB4.z = this->actor.world.pos.z + spC0.z;
            if (BgCheck_EntityRaycastFloor3(&play->colCtx, &spB0, &bgId, &spB4) != BGCHECK_Y_MIN) {
                floorType = SurfaceType_GetFloorType(&play->colCtx, spB0, bgId);
                temp_f20 = BgCheck_EntityRaycastFloor1(&play->colCtx, &spB0, &spB4);

                if ((floorType == FLOOR_TYPE_4) || (floorType == FLOOR_TYPE_15) || (floorType == FLOOR_TYPE_14)) {
                    if (floorType == FLOOR_TYPE_4) {
                        sp84 = D_80872E90;
                        sp80 = D_80872E90;
                    } else {
                        sp84 = D_80872E94;
                        sp80 = D_80872E94;
                    }
                    Matrix_MultVecZ(5.0f, &sp94);
                    sp88.x = sp88.z = 0.0f;
                    sp94.y = 2.0f;
                    sp88.y = 0.2f;

                    spA0.x = spB4.x;
                    spA0.y = temp_f20;
                    spA0.z = spB4.z;

                    temp_f20 = Rand_ZeroFloat(250.0f);
                    func_800B0F18(play, &spA0, &sp94, &sp88, &sp84, &sp80, temp_f20 + 950.0f, 10,
                                  Rand_ZeroFloat(5.0f) + 14.0f);
                }
            }
        }
    }
}

static s16 sQuakeY[] = { 3, 5 };
static s16 sQuakeDurations[] = { 10, 15 };

void EnBom_Update(Actor* thisx, PlayState* play) {
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f bomb2Accel = { 0.0f, 0.1f, 0.0f }; // unused
    Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f dustAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 dustColor = { 255, 255, 255, 255 };
    EnBom* this = (EnBom*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_2) {
        return;
    }

    if (Player_GetMask(play) == PLAYER_MASK_GIANT) {
        Actor_Kill(thisx);
        return;
    }

    if (this->unk_1FC != 0) {
        this->unk_1FC--;
        Math_ApproachZeroF(&thisx->speed, 1.0f, 1.0f);
        Actor_MoveWithGravity(thisx);
        Actor_UpdateBgCheckInfo(play, thisx, 35.0f, 10.0f, 36.0f, UPDBGCHECKINFO_FLAG_4);
        if (this->unk_1FC == 0) {
            if (this->isPowderKeg) {
                gSaveContext.powderKegTimer = 0;
            }
            Actor_Kill(thisx);
        }
    } else {
        thisx->gravity = -1.2f;
        if (this->timer != 0) {
            if (!this->isPowderKeg || (EnBom_Explode == this->actionFunc) || !Play_InCsMode(play)) {
                this->timer--;
            }
        }

        if ((!this->isPowderKeg && (this->timer == 67)) || (this->isPowderKeg && (this->timer <= 2400))) {
            Actor_PlaySfx(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
            Actor_SetScale(thisx, enBomScales[this->isPowderKeg]);
        }

        if ((thisx->xzDistToPlayer >= 20.0f) || (fabsf(thisx->playerHeightRel) >= 80.0f)) {
            this->unk_1F8 = 1;
        }

        this->actionFunc(this, play);

        Actor_UpdateBgCheckInfo(play, thisx, 35.0f, 10.0f, 36.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                    UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
        if (thisx->params == BOMB_TYPE_BODY) {
            static Vec3us D_80872ED4[] = {
                { 40, 20, 100 },
                { 300, 60, 600 },
            };
            Vec3us* sp60 = &D_80872ED4[this->isPowderKeg];

            // spawn spark effect on even frames
            dustAccel.y = 0.2f;
            Math_Vec3f_Copy(&effPos, &thisx->home.pos);
            if ((play->gameplayFrames % 2) == 0) {
                EffectSsGSpk_SpawnFuse(play, thisx, &effPos, &effVelocity, &effAccel);
            }
            if (this->isPowderKeg) {
                Audio_PlaySfx_AtPosWithChannelIO(&thisx->projectedPos, NA_SE_IT_BIG_BOMB_IGNIT - SFX_FLAG,
                                                 (this->flashSpeedScale == 7)   ? 0
                                                 : (this->flashSpeedScale == 3) ? 1
                                                                                : 2);
            } else {
                Actor_PlaySfx(thisx, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);
            }

            effPos.y += 3.0f;
            func_800B0DE0(play, &effPos, &effVelocity, &dustAccel, &dustColor, &dustColor, 50, 5);

            if ((this->collider1.base.acFlags & AC_HIT) ||
                ((this->collider1.base.ocFlags1 & OC1_HIT) && ((this->collider1.base.oc->category == ACTORCAT_ENEMY) ||
                                                               (this->collider1.base.oc->category == ACTORCAT_BOSS)))) {
                this->timer = 0;
                thisx->shape.rot.z = 0;
            } else {
                // if a lit stick touches the bomb, set timer to 100
                // these bombs never have a timer over 70, so this isn't used
                if ((this->timer > 100) && Player_IsBurningStickInRange(play, &thisx->world.pos, 30.0f, 50.0f)) {
                    this->timer = 100;
                }
            }

            dustAccel.y = 0.2f;
            effPos = thisx->world.pos;
            effPos.y += 10.0f;

            // double bomb flash speed and adjust red color at certain times during the countdown
            if ((this->timer == sp60->x) || (this->timer == sp60->y) || (this->timer == 3)) {
                thisx->shape.rot.z = 0;
                this->flashSpeedScale >>= 1;
            }

            if ((this->timer < sp60->z) && (this->timer & (this->flashSpeedScale + 1))) {
                Math_ApproachF(&this->unk_1F4, 140.0f, 1.0f, 140.0f / this->flashSpeedScale);
            } else {
                Math_ApproachZeroF(&this->unk_1F4, 1.0f, 140.0f / this->flashSpeedScale);
            }

            if (this->timer < 3) {
                Actor_SetScale(thisx, thisx->scale.x + 0.002f);
            }

            if (this->timer == 0) {
                effPos = thisx->world.pos;
                effPos.y += 10.0f;
                if (Actor_HasParent(thisx, play)) {
                    effPos.y += 30.0f;
                }
                //! @note Assumes `isPowderKeg` values aligns with clearTag params.
                //! Here, 0/1 are small/large explosions respectively.
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y - 10.0f, effPos.z, 0, 0, 0,
                            CLEAR_TAG_PARAMS(this->isPowderKeg));
                Actor_RequestQuakeAndRumble(thisx, play, sQuakeY[this->isPowderKeg],
                                            sQuakeDurations[this->isPowderKeg]);
                play->envCtx.adjLightSettings.light1Color[0] = play->envCtx.adjLightSettings.light1Color[1] =
                    play->envCtx.adjLightSettings.light1Color[2] = 250;
                play->envCtx.adjLightSettings.ambientColor[0] = play->envCtx.adjLightSettings.ambientColor[1] =
                    play->envCtx.adjLightSettings.ambientColor[2] = 250;
                Camera_AddQuake(&play->mainCamera, 2, 11, 8);
                thisx->params = BOMB_TYPE_EXPLOSION;
                this->timer = 10;
                thisx->flags |= (ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION);
                this->actionFunc = EnBom_Explode;
                if (this->isPowderKeg) {
                    gSaveContext.powderKegTimer = 0;
                    Actor_PlaySfx(thisx, NA_SE_IT_BIG_BOMB_EXPLOSION);
                } else {
                    Actor_PlaySfx(thisx, NA_SE_IT_BOMB_EXPLOSION);
                }
            }
        }

        Actor_SetFocus(thisx, 20.0f);

        if (thisx->params <= BOMB_TYPE_BODY) {
            Collider_UpdateCylinder(thisx, &this->collider1);

            // if link is not holding the bomb anymore and bump conditions are met, subscribe to OC
            if (!Actor_HasParent(thisx, play) && (this->unk_1F8 != 0)) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
            }
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider1.base);
        }

        if ((enBomScales[this->isPowderKeg] <= thisx->scale.x) && (thisx->params != BOMB_TYPE_EXPLOSION)) {
            if (thisx->depthInWater >= 20.0f) {
                Vec3f effPos;

                effPos.x = thisx->world.pos.x;
                effPos.y = thisx->world.pos.y + thisx->depthInWater;
                effPos.z = thisx->world.pos.z;
                EffectSsGRipple_Spawn(play, &effPos, 70, 500, 0);
                EffectSsGRipple_Spawn(play, &effPos, 70, 500, 10);
                effPos.y += 10.0f;
                EffectSsGSplash_Spawn(play, &effPos, NULL, NULL, 1, 500);
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y, effPos.z, 0, 0, 1,
                            CLEAR_TAG_PARAMS(CLEAR_TAG_SMOKE));
                SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 30, NA_SE_IT_BOMB_UNEXPLOSION);
                this->unk_1F4 = 0.0f;
                thisx->velocity.y = (KREG(83) * 0.1f) + -2.0f;
                thisx->gravity = (KREG(84) * 0.1f) + -0.5f;
                this->unk_1FC = KREG(81) + 10;
            } else if (thisx->bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
                thisx->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
                Actor_PlaySfx(thisx, NA_SE_EV_BOMB_DROP_WATER);
            }
        }
    }
}

static Vec3f D_80872EE0 = { 0.0f, 800.0f, 0.0f };
static Vec3f D_80872EEC = { -750.0f, 0.0f, 0.0f };
static Vec3f D_80872EF8 = { -800.0f, 0.0f, 0.0f };
static Vec3f D_80872F04 = { 0.0f, 0.0f, 0.0f };

#include "overlays/ovl_En_Bom/ovl_En_Bom.h"

void EnBom_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBom* this = (EnBom*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actor.params == BOMB_TYPE_BODY) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        Collider_UpdateSpheres(0, &this->collider2);

        if (!this->isPowderKeg) {
            func_800B8050(&this->actor, play, 0);
            Matrix_MultVec3f(&D_80872EE0, &this->actor.home.pos);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gBombCapDL);

            if (GameInteractor_Should(VB_APPLY_BOMB_BILLBOARDING, true, this->actor.id)) {
                Matrix_ReplaceRotation(&play->billboardMtxF);
            }
            Matrix_RotateXS(0x4000, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, (s8)this->unk_1F4, 0, 40, 255);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, (s8)this->unk_1F4, 0, 40, 255);
            gSPDisplayList(POLY_OPA_DISP++, gBombBodyDL);
        } else {
            Vec3f sp58;
            Vec3f sp4C;

            if (this->unk_1FA != 0) {
                s16 sp4A = this->actor.world.rot.y - this->actor.shape.rot.y;
                f32 sp44 = (1000.0f / Math_CosS(ABS_ALT((s16)(this->unk_1FA % 10922)) - 0x1555)) + -1000.0f;

                Matrix_RotateYS(sp4A, MTXMODE_APPLY);
                Matrix_Translate(0.0f, sp44, 0.0f, MTXMODE_APPLY);
                Matrix_RotateXS(this->unk_1FA, MTXMODE_APPLY);
                Matrix_RotateYS(-sp4A, MTXMODE_APPLY);
            }

            Matrix_MultVec3f(&D_80872EEC, &this->actor.home.pos);
            Matrix_MultVec3f(&D_80872EF8, &sp58);
            Matrix_MultVec3f(&D_80872F04, &sp4C);

            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gPowderKegBarrelDL);
            gSPDisplayList(POLY_OPA_DISP++, gPowderKegGoronSkullDL);

            func_808726DC(play, &this->actor.home.pos, &sp58, &sp4C, this->timer);
            EnBom_DrawKeg(play, this->timer);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80872648(PlayState* play, Vec3f* arg1) {
    PowderKegFuseSegment* fuseSegmentPtr = &sPowderKegFuseSegments[0];
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sPowderKegFuseSegments); i++, fuseSegmentPtr++) {
        Math_Vec3f_Copy(&fuseSegmentPtr->pos, arg1);
        Math_Vec3f_Copy(&fuseSegmentPtr->velocity, &gZeroVec3f);
        fuseSegmentPtr->rotY = 0;
        fuseSegmentPtr->rotX = 0x4000;
    }
}

void func_808726DC(PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, s32 arg4) {
    s32 i;
    f32 temp_f20;
    Vec3f spCC;
    Vec3f spC0;
    PowderKegFuseSegment* fuseSegmentPtr = &sPowderKegFuseSegments[0];
    PowderKegFuseSegment* fuseSegmentPtr2 = &sPowderKegFuseSegments[1];
    f32 temp_f26 = Math_Vec3f_DistXYZ(arg3, arg1);
    s32 spB0;
    f32 temp_f2;
    f32 distXZ;

    Math_Vec3f_Copy(&fuseSegmentPtr->pos, arg1);
    Math_Vec3f_Diff(arg2, arg1, &spCC);

    fuseSegmentPtr->rotY = Math_Atan2S_XY(spCC.z, spCC.x);
    distXZ = sqrtf(SQXZ(spCC));
    fuseSegmentPtr->rotX = Math_Atan2S_XY(distXZ, spCC.y);

    spB0 = (arg4 / 240) + 1;

    for (i = 0; i < spB0; i++, fuseSegmentPtr++, fuseSegmentPtr2++, arg4 -= 240) {
        f32 phi_f22;
        CollisionPoly* spA0;
        s32 bgId;
        Vec3f sp90;

        if (arg4 >= 240) {
            phi_f22 = 8.0f;
        } else {
            phi_f22 = (arg4 % 240) * (1.0f / 240) * 8.0f;
        }

        Math_Vec3f_Sum(&fuseSegmentPtr2->pos, &fuseSegmentPtr2->velocity, &fuseSegmentPtr2->pos);
        temp_f20 = Math_Vec3f_DistXYZAndStoreDiff(arg3, &fuseSegmentPtr2->pos, &spCC);
        if (temp_f20 < temp_f26) {
            if (temp_f20 == 0.0f) {
                spCC.x = 0.0f;
                spCC.y = temp_f26;
                spCC.z = 0.0f;
            } else {
                temp_f20 = temp_f26 / temp_f20;
                spCC.x *= temp_f20;
                spCC.y *= temp_f20;
                spCC.z *= temp_f20;
            }
            Math_Vec3f_Sum(arg3, &spCC, &fuseSegmentPtr2->pos);
        }

        if (Math_Vec3f_DistXYZAndStoreDiff(&fuseSegmentPtr->pos, &fuseSegmentPtr2->pos, &spCC) == 0.0f) {
            spCC.x = 0.0f;
            spCC.y = phi_f22;
            spCC.z = 0.0f;
        }

        fuseSegmentPtr2->rotY = Math_Atan2S_XY(spCC.z, spCC.x);
        distXZ = sqrtf(SQXZ(spCC));
        fuseSegmentPtr2->rotX = Math_Atan2S_XY(distXZ, spCC.y);

        fuseSegmentPtr2->rotY =
            (s16)CLAMP(BINANG_SUB(fuseSegmentPtr2->rotY, fuseSegmentPtr->rotY), -8000, 8000) + fuseSegmentPtr->rotY;
        fuseSegmentPtr2->rotX =
            (s16)CLAMP(BINANG_SUB(fuseSegmentPtr2->rotX, fuseSegmentPtr->rotX), -8000, 8000) + fuseSegmentPtr->rotX;

        temp_f20 = Math_CosS(fuseSegmentPtr2->rotX) * phi_f22;
        spC0.x = Math_SinS(fuseSegmentPtr2->rotY) * temp_f20;
        spC0.z = Math_CosS(fuseSegmentPtr2->rotY) * temp_f20;
        spC0.y = Math_SinS(fuseSegmentPtr2->rotX) * phi_f22;

        Math_Vec3f_Sum(&fuseSegmentPtr->pos, &spC0, &fuseSegmentPtr2->pos);
        Math_Vec3f_Copy(&sp90, &fuseSegmentPtr2->pos);

        sp90.y += 50.0f;

        temp_f2 = BgCheck_EntityRaycastFloor3(&play->colCtx, &spA0, &bgId, &sp90) - fuseSegmentPtr2->pos.y;
        if (temp_f2 >= 0.0f) {
            spC0.y += temp_f2;
            if (phi_f22 < spC0.y) {
                spC0.y = phi_f22;
                temp_f2 = 0.0f;
            } else {
                temp_f2 = sqrtf(SQ(phi_f22) - SQ(spC0.y));
            }

            if (temp_f20 == 0.0f) {
                spC0.x = temp_f2;
            } else {
                temp_f2 /= temp_f20;
                spC0.x *= temp_f2;
                spC0.z *= temp_f2;
            }

            Math_Vec3f_Sum(&fuseSegmentPtr->pos, &spC0, &fuseSegmentPtr2->pos);
            Math_Vec3f_Copy(&fuseSegmentPtr2->velocity, &gZeroVec3f);
        }

        fuseSegmentPtr2->velocity.y += -1.0f;
        if (fuseSegmentPtr2->velocity.y < -10.0f) {
            fuseSegmentPtr2->velocity.y = -10.0f;
        }
    }

    Math_Vec3f_Copy(arg1, &fuseSegmentPtr->pos);
}

// 2S2H [Port] See comment in the draw function.
#define dgPowderKegFuseTexLarger "__OTR__overlays/ovl_En_Bom/gPowderKegFuseTexLarger"
static const ALIGN_ASSET(2) char gPowderKegFuseTexLarger[] = dgPowderKegFuseTexLarger;

void EnBom_DrawKeg(PlayState* play, s32 timer) {
    s32 temp_s5;
    s32 i;
    PowderKegFuseSegment* fuseSegmentPtr = &sPowderKegFuseSegments[0];
    PowderKegFuseSegment* fuseSegmentPtr2;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(fuseSegmentPtr->pos.x, fuseSegmentPtr->pos.y, fuseSegmentPtr->pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(fuseSegmentPtr->rotX, fuseSegmentPtr->rotY, 0, MTXMODE_APPLY);
    Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    // 2S2H [Port] This was originally a static DL in the OTR file. The normal texture is too small to be rendered by
    // Fast3D and causes a crash. This adds a custom texture in the OTR file and changes the DList to use the new size.
    // At some point this should be done in the GFX patcher.
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
    gSPSetGeometryMode(POLY_OPA_DISP++, G_LIGHTING);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEI_PRIM2);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gPowderKegFuseTexLarger, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 1, 1, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 200, 0, 255);
    gSPVertex(POLY_OPA_DISP++, gPowderKegFuseVtx, 4, 0);

    temp_s5 = (timer / 240) + 1;
    fuseSegmentPtr2 = &sPowderKegFuseSegments[1];
    // Bentodo the fuse is not visible enough to see if this is broken. Verify once the long timer crash is fixed.
    for (i = 1; i < temp_s5; i++, fuseSegmentPtr2++) {
        Matrix_Translate(fuseSegmentPtr2->pos.x, fuseSegmentPtr2->pos.y, fuseSegmentPtr2->pos.z, MTXMODE_NEW);
        Matrix_RotateZYX(fuseSegmentPtr2->rotX, fuseSegmentPtr2->rotY, 0, MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        if ((i % 2) == 0) {
            gSPDisplayList(POLY_OPA_DISP++, gPowderKegFuseModel1DL);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, gPowderKegFuseModel2DL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
