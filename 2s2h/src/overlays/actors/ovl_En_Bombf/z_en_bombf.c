/*
 * File: z_en_bombf.c
 * Overlay: ovl_En_Bombf
 * Description: Bomb flower
 */

#include "z_en_bombf.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_bombf/object_bombf.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBombf_Init(Actor* thisx, PlayState* play2);
void EnBombf_Destroy(Actor* thisx, PlayState* play);
void EnBombf_Update(Actor* thisx, PlayState* play);
void EnBombf_Draw(Actor* thisx, PlayState* play);

void EnBombf_SetupAction(EnBombf* this, EnBombfActionFunc actionFunc);
void func_808AEAB8(EnBombf* this, s16 arg1);
void func_808AEAE0(EnBombf* this, PlayState* play);
void func_808AEE3C(EnBombf* this, PlayState* play);
void func_808AEF68(EnBombf* this, PlayState* play);
void func_808AEFD4(EnBombf* this, PlayState* play);

ActorProfile En_Bombf_Profile = {
    /**/ ACTOR_EN_BOMBF,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOMBF,
    /**/ sizeof(EnBombf),
    /**/ EnBombf_Init,
    /**/ EnBombf_Destroy,
    /**/ EnBombf_Update,
    /**/ EnBombf_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00013A28, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 9, 18, 10, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000008, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void EnBombf_SetupAction(EnBombf* this, EnBombfActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnBombf_Init(Actor* thisx, PlayState* play2) {
    f32 yOffset = 0.0f;
    PlayState* play = play2;
    EnBombf* this = (EnBombf*)thisx;

    Actor_SetScale(thisx, 0.01f);
    this->unk_1F8 = 1;

    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_InitJntSph(play, &this->colliderJntSph);
    Collider_SetCylinder(play, &this->colliderCylinder, thisx, &sCylinderInit);
    Collider_SetJntSph(play, &this->colliderJntSph, thisx, &sJntSphInit, this->colliderJntSphElements);

    if (ENBOMBF_GET(thisx) == ENBOMBF_0) {
        yOffset = 1000.0f;
    }

    ActorShape_Init(&thisx->shape, yOffset, ActorShadow_DrawCircle, 12.0f);

    thisx->focus.pos = thisx->world.pos;
    thisx->colChkInfo.cylRadius = 10;
    thisx->colChkInfo.cylHeight = 10;
    thisx->attentionRangeType = ATTENTION_RANGE_0;

    if (ENBOMBF_GET(thisx) == ENBOMBF_0) {
        this->timer = 140;
        this->unk_1FE = 15;
        thisx->gravity = -1.5f;
        Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_EXPLOSIVES);
        thisx->colChkInfo.mass = 200;
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        EnBombf_SetupAction(this, func_808AEE3C);
    } else {
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
        this->unk_1FC = 1;
        this->unk_204 = 1.0f;
        func_808AEAB8(this, ENBOMBF_GET(thisx));
    }

    thisx->cullingVolumeScale += 31000.0f;
    thisx->cullingVolumeDistance += 31000.0f;
}

void EnBombf_Destroy(Actor* thisx, PlayState* play) {
    EnBombf* this = (EnBombf*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroyJntSph(play, &this->colliderJntSph);
}

void func_808AEAB8(EnBombf* this, s16 arg1) {
    EnBombf_SetupAction(this, func_808AEAE0);
}

void func_808AEAE0(EnBombf* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    EnBombf* bombf;
    Player* player = GET_PLAYER(play);
    s32 pad3;

    if (this->unk_204 >= 1.0f) {
        if (Actor_HasParent(&this->actor, play)) {
            bombf = (EnBombf*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOMBF, this->actor.world.pos.x,
                                          this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ENBOMBF_0);
            if (bombf != NULL) {
                func_800B8C20(&this->actor, &bombf->actor, play);
                this->timer = 180;
                this->unk_204 = 0.0f;
                Actor_PlaySfx(&this->actor, NA_SE_PL_PULL_UP_ROCK);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            } else {
                player->actor.child = NULL;
                player->heldActor = NULL;
                player->interactRangeActor = NULL;
                this->actor.parent = NULL;
                player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
            }
        } else if ((this->colliderCylinder.base.acFlags & AC_HIT) &&
                   ((this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & 0x13828) ||
                    ((this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & 0x200) &&
                     (player->transformation == PLAYER_FORM_GORON) && (player->actor.speed > 15.0f)))) {
            this->colliderCylinder.base.acFlags &= ~AC_HIT;
            if (this->colliderCylinder.base.ac->category != ACTORCAT_BOSS) {
                bombf = (EnBombf*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOMBF, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ENBOMBF_0);
                if (bombf != NULL) {
                    bombf->unk_1F8 = 1;
                    bombf->timer = 0;
                    this->timer = 180;
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                    this->unk_204 = 0.0f;
                }
            }
        } else {
            if (Player_IsBurningStickInRange(play, &this->actor.world.pos, 30.0f, 50.0f)) {
                bombf = (EnBombf*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOMBF, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ENBOMBF_0);
                if (bombf != NULL) {
                    bombf->timer = 100;
                    this->timer = 180;
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                    this->unk_204 = 0.0f;
                }
            } else {
                if (!Actor_HasParent(&this->actor, play)) {
                    Actor_OfferCarry(&this->actor, play);
                    return;
                }
                player->actor.child = NULL;
                player->heldActor = NULL;
                player->interactRangeActor = NULL;
                this->actor.parent = NULL;
                player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
                this->actor.world.pos = this->actor.home.pos;
            }
        }
    } else {
        if (this->timer == 0) {
            this->unk_204 += 0.05f;
            if (this->unk_204 >= 1.0f) {
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
        }

        if (Actor_HasParent(&this->actor, play)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            this->actor.parent = NULL;
            player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
            this->actor.world.pos = this->actor.home.pos;
        }
    }
}

void func_808AEE3C(EnBombf* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->unk_204 = 0.0f;
        EnBombf_SetupAction(this, func_808AEF68);
        this->actor.room = -1;
        return;
    }

    this->unk_204 = 1.0f;
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Math_SmoothStepToF(&this->actor.speed, 0.0f, 1.0f, 0.025f, 0.0f);
        return;
    }

    Math_SmoothStepToF(&this->actor.speed, 0.0f, 1.0f, 1.5f, 0.0f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_PlaySfx_SurfaceBomb(play, &this->actor);
        if (this->actor.velocity.y < -6.0f) {
            this->actor.velocity.y *= -0.3f;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        }
    } else if (this->timer >= 4) {
        Actor_OfferCarry(&this->actor, play);
    }
}

void func_808AEF68(EnBombf* this, PlayState* play) {
    if (Actor_HasNoParent(&this->actor, play)) {
        EnBombf_SetupAction(this, func_808AEE3C);
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        func_808AEE3C(this, play);
    } else {
        this->actor.velocity.y = 0.0f;
    }
}

void func_808AEFD4(EnBombf* this, PlayState* play) {
    if (this->colliderJntSph.elements[0].dim.modelSphere.radius == 0) {
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        Rumble_Request(this->actor.xzDistToPlayer, 255, 20, 150);
    }

    this->colliderJntSph.elements[0].dim.modelSphere.radius = 100;
    this->colliderJntSph.elements[0].dim.worldSphere.radius = 100;

    if (ENBOMBF_GET(&this->actor) == ENBOMBF_1) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderJntSph.base);
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
        Player* player = GET_PLAYER(play);

        if ((player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (&this->actor == player->heldActor)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        }
        Actor_Kill(&this->actor);
    }
}

void EnBombf_Update(Actor* thisx, PlayState* play) {
    Vec3f sp8C = { 0.0f, 0.0f, 0.0f };
    Vec3f sp80 = { 0.0f, 0.1f, 0.0f };
    Vec3f sp74 = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f sp5C = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 sp58 = { 255, 255, 255, 255 };
    EnBombf* this = (EnBombf*)thisx;
    s32 pad;

    if ((this->unk_1F8 != 0) && (this->timer != 0)) {
        this->timer--;
    }

    if ((this->unk_1FC == 0) && !Actor_HasParent(&this->actor, play) &&
        ((this->actor.xzDistToPlayer >= 20.0f) || (fabsf(this->actor.playerHeightRel) >= 80.0f))) {
        this->unk_1FC = 1;
    }

    this->actionFunc(this, play);

    if (ENBOMBF_GET(&this->actor) == ENBOMBF_0) {
        Actor_MoveWithGravity(&this->actor);
    }

    if (this->actor.gravity != 0.0f) {
        DREG(6) = 1;
        Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 10.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                    UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
        DREG(6) = 0;
    }

    if (ENBOMBF_GET(&this->actor) == ENBOMBF_0) {
        if ((this->actor.velocity.y > 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
            this->actor.velocity.y = -this->actor.velocity.y;
        }

        if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
            s16 yDiff = BINANG_SUB(this->actor.wallYaw, this->actor.world.rot.y);

            if (ABS_ALT(yDiff) > 0x4000) {
                this->actor.world.rot.y =
                    BINANG_SUB(this->actor.wallYaw - this->actor.world.rot.y + this->actor.wallYaw, 0x8000);
            }
            Actor_PlaySfx(&this->actor, NA_SE_EV_BOMB_BOUND);
            Actor_MoveWithGravity(&this->actor);
            DREG(6) = 1;
            Actor_UpdateBgCheckInfo(play, &this->actor, 5.0f, 10.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                        UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
            DREG(6) = 0;
            this->actor.speed *= 0.7f;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        }

        if ((this->colliderCylinder.base.acFlags & AC_HIT) ||
            ((this->colliderCylinder.base.ocFlags1 & OC1_HIT) && (this->colliderCylinder.base.oc->category == 5))) {
            this->unk_1F8 = 1;
            this->timer = 0;
        } else if ((this->timer > 100) && Player_IsBurningStickInRange(play, &this->actor.world.pos, 30.0f, 50.0f)) {
            this->timer = 100;
        }

        if (this->unk_1F8 != 0) {
            sp5C.y = 0.2f;
            effPos = this->actor.world.pos;
            effPos.y += 25.0f;

            if (this->timer < 127) {
                if ((play->gameplayFrames % 2) == 0) {
                    EffectSsGSpk_SpawnFuse(play, &this->actor, &effPos, &sp8C, &sp74);
                }
                Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);
                effPos.y += 3.0f;
                func_800B0DE0(play, &effPos, &sp8C, &sp5C, &sp58, &sp58, 0x32, 5);
            }

            if ((this->timer == 3) || (this->timer == 30) || (this->timer == 50) || (this->timer == 70)) {
                this->unk_1FE >>= 1;
            }

            if ((this->timer < 100) && (this->timer & (this->unk_1FE + 1))) {
                Math_SmoothStepToF(&this->unk_200, 150.0f, 1.0f, 150.0f / this->unk_1FE, 0.0f);
            } else {
                Math_SmoothStepToF(&this->unk_200, 0.0f, 1.0f, 150.0f / this->unk_1FE, 0.0f);
            }

            if (this->timer < 3) {
                Actor_SetScale(&this->actor, this->actor.scale.x + 0.002f);
            }

            if (this->timer == 0) {
                effPos = this->actor.world.pos;
                effPos.y += 10.0f;

                if (Actor_HasParent(&this->actor, play)) {
                    effPos.y += 30.0f;
                }

                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y, effPos.z, 0, 0, 0,
                            CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_EXPLOSION));
                Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);

                play->envCtx.adjLightSettings.light1Color[0] = play->envCtx.adjLightSettings.light1Color[1] =
                    play->envCtx.adjLightSettings.light1Color[2] = 250;
                play->envCtx.adjLightSettings.ambientColor[0] = play->envCtx.adjLightSettings.ambientColor[1] =
                    play->envCtx.adjLightSettings.ambientColor[2] = 250;

                Camera_AddQuake(&play->mainCamera, 2, 11, 8);

                this->actor.params = ENBOMBF_1;
                this->timer = 10;
                this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;

                EnBombf_SetupAction(this, func_808AEFD4);
            }
        }
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 10.0f;

    if (ENBOMBF_GET(&this->actor) <= ENBOMBF_0) {
        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
        if ((this->unk_204 >= 1.0f) && (this->unk_1FC != 0)) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }

    if ((this->actor.scale.x >= 0.01f) && (ENBOMBF_GET(&this->actor) != ENBOMBF_1)) {
        if (this->actor.depthInWater >= 20.0f) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_IT_BOMB_UNEXPLOSION);
            Actor_Kill(&this->actor);
            return;
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            Actor_PlaySfx(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

Gfx* func_808AF86C(GraphicsContext* gfxCtx, PlayState* play) {
    Gfx* gfxHead = GRAPH_ALLOC(gfxCtx, 5 * sizeof(Gfx));
    Gfx* gfx = gfxHead;

    Matrix_ReplaceRotation(&play->billboardMtxF);

    MATRIX_FINALIZE_AND_LOAD(gfx++, gfxCtx);
    gSPEndDisplayList(gfx++);

    return gfxHead;
}

void EnBombf_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombf* this = (EnBombf*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (ENBOMBF_GET(&this->actor) <= ENBOMBF_0) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        if (ENBOMBF_GET(&this->actor) != ENBOMBF_0) {
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gBombFlowerLeavesDL);
            gSPDisplayList(POLY_OPA_DISP++, gBombFlowerBaseLeavesDL);

            Matrix_Translate(0.0f, 1000.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(this->unk_204, this->unk_204, this->unk_204, MTXMODE_APPLY);
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 200, 255, 200, 255);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, (s8)this->unk_200, 20, 10, 0);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        {
            Gfx* gfx = func_808AF86C(play->state.gfxCtx, play);

            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gfx));
        }

        gSPDisplayList(POLY_OPA_DISP++, gBombFlowerBombAndSparkDL);
    } else {
        Collider_UpdateSpheres(0, &this->colliderJntSph);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
