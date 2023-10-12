/*
 * File: z_en_arrow.c
 * Overlay: ovl_En_Arrow
 * Description: Arrow, Deku Seed, and Deku Nut Projectile
 */

#include "z_en_arrow.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_gi_nuts/object_gi_nuts.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnArrow_Init(Actor* thisx, PlayState* play);
void EnArrow_Destroy(Actor* thisx, PlayState* play);
void EnArrow_Update(Actor* thisx, PlayState* play);
void EnArrow_Draw(Actor* thisx, PlayState* play);

void EnArrow_Shoot(EnArrow* this, PlayState* play);
void EnArrow_Fly(EnArrow* this, PlayState* play);
void func_809B45E0(EnArrow* this, PlayState* play);
void func_809B4640(EnArrow* this, PlayState* play);

const ActorInit En_Arrow_InitVars = {
    ACTOR_EN_ARROW,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnArrow),
    (ActorFunc)EnArrow_Init,
    (ActorFunc)EnArrow_Destroy,
    (ActorFunc)EnArrow_Update,
    (ActorFunc)EnArrow_Draw,
    NULL,
};

static ColliderQuadInit sColliderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000020, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(minVelocityY, -150, ICHAIN_STOP),
};

void EnArrow_SetupAction(EnArrow* this, EnArrowActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnArrow_Init(Actor* thisx, PlayState* play) {
    static EffectBlureInit2 blureNormal = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 255, 170, 255 }, { 0, 150, 0, 0 }, 0,
    };
    static EffectBlureInit2 blureFire = {
        0, 4, 0, { 0, 255, 200, 255 }, { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 200, 0, 255 }, { 255, 0, 0, 0 }, 0,
    };
    static EffectBlureInit2 blureIce = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 170, 255, 255, 255 }, { 0, 100, 255, 0 }, 0,
    };
    static EffectBlureInit2 blureLight = {
        0, 4, 0, { 0, 255, 200, 255 },   { 0, 255, 255, 255 }, { 0, 255, 200, 0 }, { 0, 255, 255, 0 }, 16,
        0, 1, 0, { 255, 255, 170, 255 }, { 255, 255, 0, 0 }, 0,
    };
    static u32 dmgFlags[] = {
        0x00000800, 0x00000020, 0x00000020, 0x00000800, 0x00001000,
        0x00002000, 0x00010000, 0x00004000, 0x00008000, 0x00000004,
    };
    EnArrow* this = (EnArrow*)thisx;

    if (CVarGetInteger("gCosmetics.Arrows_NormalPrimary.Changed", 0)) {
        blureNormal.altEnvColor = CVarGetColor("gCosmetics.Arrows_NormalPrimary.Value", (Color_RGBA8){ 0, 150, 0, 0 });
    } else {
        blureNormal.altEnvColor = (Color_RGBA8){ 0, 150, 0, 0 };
    }
    if (CVarGetInteger("gCosmetics.Arrows_NormalSecondary.Changed", 0)) {
        blureNormal.altPrimColor = CVarGetColor("gCosmetics.Arrows_NormalSecondary.Value", (Color_RGBA8){ 255, 255, 170, 255 });
    } else {
        blureNormal.altPrimColor = (Color_RGBA8){ 255, 255, 170, 255 };
    }

    if (CVarGetInteger("gCosmetics.Arrows_FirePrimary.Changed", 0)) {
        blureFire.altEnvColor = CVarGetColor("gCosmetics.Arrows_FirePrimary.Value", (Color_RGBA8){ 255, 200, 0, 0 });
    } else {
        blureFire.altEnvColor = (Color_RGBA8){ 255, 200, 0, 0 };
    }
    if (CVarGetInteger("gCosmetics.Arrows_FireSecondary.Changed", 0)) {
        blureFire.altPrimColor = CVarGetColor("gCosmetics.Arrows_FireSecondary.Value", (Color_RGBA8){ 255, 0, 0, 255 });
    } else {
        blureFire.altPrimColor = (Color_RGBA8){ 255, 0, 0, 255 };
    }

    if (CVarGetInteger("gCosmetics.Arrows_IcePrimary.Changed", 0)) {
        blureIce.altEnvColor = CVarGetColor("gCosmetics.Arrows_IcePrimary.Value", (Color_RGBA8){ 0, 0, 255, 255 });
    } else {
        blureIce.altEnvColor = (Color_RGBA8){ 0, 0, 255, 255 };
    }
    if (CVarGetInteger("gCosmetics.Arrows_IceSecondary.Changed", 0)) {
        blureIce.altPrimColor = CVarGetColor("gCosmetics.Arrows_IceSecondary.Value", (Color_RGBA8){ 170, 255, 255, 0 });
    } else {
        blureIce.altPrimColor = (Color_RGBA8){ 170, 255, 255, 0 };
    }

    if (CVarGetInteger("gCosmetics.Arrows_LightPrimary.Changed", 0)) {
        blureLight.altEnvColor = CVarGetColor("gCosmetics.Arrows_LightPrimary.Value", (Color_RGBA8){ 255, 255, 0, 255 });
    } else {
        blureLight.altEnvColor = (Color_RGBA8){ 255, 255, 0, 255 };
    }
    if (CVarGetInteger("gCosmetics.Arrows_LightSecondary.Changed", 0)) {
        blureLight.altPrimColor = CVarGetColor("gCosmetics.Arrows_LightSecondary.Value", (Color_RGBA8){ 255, 255, 170, 0 });
    } else {
        blureLight.altPrimColor = (Color_RGBA8){ 255, 255, 170, 0 };
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);

    if (this->actor.params == ARROW_CS_NUT) {
        this->isCsNut = true;
        this->actor.params = ARROW_NUT;
    }

    if (this->actor.params <= ARROW_SEED) {

        if (this->actor.params <= ARROW_0E) {
            SkelAnime_Init(play, &this->skelAnime, &gArrowSkel, &gArrow2Anim, NULL, NULL, 0);
        }

        if (this->actor.params <= ARROW_NORMAL) {
            if (this->actor.params == ARROW_NORMAL_HORSE) {
                blureNormal.elemDuration = 4;
            } else {
                blureNormal.elemDuration = 16;
            }

            Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blureNormal);

        } else if (this->actor.params == ARROW_FIRE) {

            Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blureFire);

        } else if (this->actor.params == ARROW_ICE) {

            Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blureIce);

        } else if (this->actor.params == ARROW_LIGHT) {

            Effect_Add(play, &this->effectIndex, EFFECT_BLURE2, 0, 0, &blureLight);
        }

        Collider_InitQuad(play, &this->collider);
        Collider_SetQuad(play, &this->collider, &this->actor, &sColliderInit);

        if (this->actor.params <= ARROW_NORMAL) {
            this->collider.info.toucherFlags &= ~0x18;
            this->collider.info.toucherFlags |= 0;
        }

        if (this->actor.params < 0) {
            this->collider.base.atFlags = (AT_ON | AT_TYPE_ENEMY);
        } else if (this->actor.params <= ARROW_SEED) {
            this->collider.info.toucher.dmgFlags = dmgFlags[this->actor.params];
            LOG_HEX("this->at_info.cl_elem.at_btl_info.at_type", this->collider.info.toucher.dmgFlags);
        }
    }

    EnArrow_SetupAction(this, EnArrow_Shoot);
}

void EnArrow_Destroy(Actor* thisx, PlayState* play) {
    EnArrow* this = (EnArrow*)thisx;

    if (this->actor.params <= ARROW_LIGHT) {
        Effect_Delete(play, this->effectIndex);
    }

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyQuad(play, &this->collider);

    if ((this->hitActor != NULL) && (this->hitActor->update != NULL)) {
        this->hitActor->flags &= ~ACTOR_FLAG_DRAGGED_BY_ARROW;
    }
}

void EnArrow_Shoot(EnArrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.parent == NULL) {
        if ((this->actor.params != ARROW_NUT) && (player->unk_A73 == 0)) {
            Actor_Kill(&this->actor);
            return;
        }

        switch (this->actor.params) {
            case ARROW_SEED:
                Player_PlaySfx(&player->actor, NA_SE_IT_SLING_SHOT);
                break;

            case ARROW_NORMAL_LIT:
            case ARROW_NORMAL_HORSE:
            case ARROW_NORMAL:
                Player_PlaySfx(&player->actor, NA_SE_IT_ARROW_SHOT);
                break;

            case ARROW_FIRE:
            case ARROW_ICE:
            case ARROW_LIGHT:
                Player_PlaySfx(&player->actor, NA_SE_IT_MAGIC_ARROW_SHOT);
                break;
        }

        EnArrow_SetupAction(this, EnArrow_Fly);
        Math_Vec3f_Copy(&this->unk_210, &this->actor.world.pos);

        if (this->actor.params >= ARROW_SEED) {
            func_8002D9A4(&this->actor, 80.0f);
            this->timer = 15;
            this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
        } else {
            func_8002D9A4(&this->actor, 150.0f);
            this->timer = 12;
        }
    }
}

void func_809B3CEC(PlayState* play, EnArrow* this) {
    EnArrow_SetupAction(this, func_809B4640);
    Animation_PlayOnce(&this->skelAnime, &gArrow1Anim);
    this->actor.world.rot.y += (s32)(24576.0f * (Rand_ZeroOne() - 0.5f)) + 0x8000;
    this->actor.velocity.y += (this->actor.speedXZ * (0.4f + (0.4f * Rand_ZeroOne())));
    this->actor.speedXZ *= (0.04f + 0.3f * Rand_ZeroOne());
    this->timer = 50;
    this->actor.gravity = -1.5f;
}

void EnArrow_CarryActor(EnArrow* this, PlayState* play) {
    CollisionPoly* hitPoly;
    Vec3f posDiffLastFrame;
    Vec3f actorNextPos;
    Vec3f hitPos;
    f32 temp_f12;
    f32 scale;
    s32 bgId;

    Math_Vec3f_Diff(&this->actor.world.pos, &this->unk_210, &posDiffLastFrame);

    temp_f12 = ((this->actor.world.pos.x - this->hitActor->world.pos.x) * posDiffLastFrame.x) +
               ((this->actor.world.pos.y - this->hitActor->world.pos.y) * posDiffLastFrame.y) +
               ((this->actor.world.pos.z - this->hitActor->world.pos.z) * posDiffLastFrame.z);

    if (!(temp_f12 < 0.0f)) {
        scale = Math3D_Vec3fMagnitudeSq(&posDiffLastFrame);

        if (!(scale < 1.0f)) {
            scale = temp_f12 / scale;
            Math_Vec3f_Scale(&posDiffLastFrame, scale);
            Math_Vec3f_Sum(&this->hitActor->world.pos, &posDiffLastFrame, &actorNextPos);

            if (BgCheck_EntityLineTest1(&play->colCtx, &this->hitActor->world.pos, &actorNextPos, &hitPos,
                                        &hitPoly, true, true, true, true, &bgId)) {
                this->hitActor->world.pos.x = hitPos.x + ((actorNextPos.x <= hitPos.x) ? 1.0f : -1.0f);
                this->hitActor->world.pos.y = hitPos.y + ((actorNextPos.y <= hitPos.y) ? 1.0f : -1.0f);
                this->hitActor->world.pos.z = hitPos.z + ((actorNextPos.z <= hitPos.z) ? 1.0f : -1.0f);
            } else {
                Math_Vec3f_Copy(&this->hitActor->world.pos, &actorNextPos);
            }
        }
    }
}

void EnArrow_Fly(EnArrow* this, PlayState* play) {
    CollisionPoly* hitPoly;
    s32 bgId;
    Vec3f hitPoint;
    Vec3f posCopy;
    s32 atTouched;
    u16 sfxId;
    Actor* hitActor;
    Vec3f sp60;
    Vec3f sp54;

    if (DECR(this->timer) == 0) {
        Actor_Kill(&this->actor);
        return;
    }

    if (this->timer < 7.2000003f) {
        this->actor.gravity = -0.4f;
    }

    atTouched = (this->actor.params != ARROW_NORMAL_LIT) && (this->actor.params <= ARROW_SEED) &&
                (this->collider.base.atFlags & AT_HIT);

    if (atTouched || this->touchedPoly) {
        if (this->actor.params >= ARROW_SEED) {
            if (atTouched) {
                this->actor.world.pos.x = (this->actor.world.pos.x + this->actor.prevPos.x) * 0.5f;
                this->actor.world.pos.y = (this->actor.world.pos.y + this->actor.prevPos.y) * 0.5f;
                this->actor.world.pos.z = (this->actor.world.pos.z + this->actor.prevPos.z) * 0.5f;
            }

            if (this->actor.params == ARROW_NUT) {
                iREG(50) = -1;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_FIRE1, this->actor.world.pos.x,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
                sfxId = NA_SE_IT_DEKU;
            } else {
                sfxId = NA_SE_IT_SLING_REFLECT;
            }

            EffectSsStone1_Spawn(play, &this->actor.world.pos, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, sfxId);
            Actor_Kill(&this->actor);
        } else {
            EffectSsHitMark_SpawnCustomScale(play, 0, 150, &this->actor.world.pos);

            if (atTouched && (this->collider.info.atHitInfo->elemType != ELEMTYPE_UNK4)) {
                hitActor = this->collider.base.at;

                if ((hitActor->update != NULL) && (!(this->collider.base.atFlags & AT_BOUNCED)) &&
                    (hitActor->flags & ACTOR_FLAG_ARROW_DRAGGABLE)) {
                    this->hitActor = hitActor;
                    EnArrow_CarryActor(this, play);
                    Math_Vec3f_Diff(&hitActor->world.pos, &this->actor.world.pos, &this->unk_250);
                    hitActor->flags |= ACTOR_FLAG_DRAGGED_BY_ARROW;
                    this->collider.base.atFlags &= ~AT_HIT;
                    this->actor.speedXZ /= 2.0f;
                    this->actor.velocity.y /= 2.0f;
                } else {
                    this->hitFlags |= 1;
                    this->hitFlags |= 2;

                    if (this->collider.info.atHitInfo->bumperFlags & 2) {
                        this->actor.world.pos.x = this->collider.info.atHitInfo->bumper.hitPos.x;
                        this->actor.world.pos.y = this->collider.info.atHitInfo->bumper.hitPos.y;
                        this->actor.world.pos.z = this->collider.info.atHitInfo->bumper.hitPos.z;
                    }

                    func_809B3CEC(play, this);
                    Audio_PlayActorSound2(&this->actor, NA_SE_IT_ARROW_STICK_CRE);
                }
            } else if (this->touchedPoly) {
                EnArrow_SetupAction(this, func_809B45E0);
                Animation_PlayOnce(&this->skelAnime, &gArrow2Anim);

                if (this->actor.params >= ARROW_NORMAL_LIT) {
                    this->timer = 60;
                } else {
                    this->timer = 20;
                }

                Audio_PlayActorSound2(&this->actor, NA_SE_IT_ARROW_STICK_OBJ);
                this->hitFlags |= 1;
            }
        }
    } else {
        Math_Vec3f_Copy(&this->unk_210, &this->actor.world.pos);
        Actor_MoveForward(&this->actor);

        if ((this->touchedPoly =
                 BgCheck_ProjectileLineTest(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &hitPoint,
                                            &this->actor.wallPoly, true, true, true, true, &bgId))) {
            func_8002F9EC(play, &this->actor, this->actor.wallPoly, bgId, &hitPoint);
            Math_Vec3f_Copy(&posCopy, &this->actor.world.pos);
            Math_Vec3f_Copy(&this->actor.world.pos, &hitPoint);
        }

        if (this->actor.params <= ARROW_0E) {
            this->actor.shape.rot.x = Math_Atan2S(this->actor.speedXZ, -this->actor.velocity.y);
        }
    }

    if (this->hitActor != NULL) {
        if (this->hitActor->update != NULL) {
            Math_Vec3f_Sum(&this->unk_210, &this->unk_250, &sp60);
            Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_250, &sp54);

            if (BgCheck_EntityLineTest1(&play->colCtx, &sp60, &sp54, &hitPoint, &hitPoly, true, true, true, true,
                                        &bgId)) {
                this->hitActor->world.pos.x = hitPoint.x + ((sp54.x <= hitPoint.x) ? 1.0f : -1.0f);
                this->hitActor->world.pos.y = hitPoint.y + ((sp54.y <= hitPoint.y) ? 1.0f : -1.0f);
                this->hitActor->world.pos.z = hitPoint.z + ((sp54.z <= hitPoint.z) ? 1.0f : -1.0f);
                Math_Vec3f_Diff(&this->hitActor->world.pos, &this->actor.world.pos, &this->unk_250);
                this->hitActor->flags &= ~ACTOR_FLAG_DRAGGED_BY_ARROW;
                this->hitActor = NULL;
            } else {
                Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_250, &this->hitActor->world.pos);
            }

            if (this->touchedPoly && (this->hitActor != NULL)) {
                this->hitActor->flags &= ~ACTOR_FLAG_DRAGGED_BY_ARROW;
                this->hitActor = NULL;
            }
        } else {
            this->hitActor = NULL;
        }
    }
}

void func_809B45E0(EnArrow* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (DECR(this->timer) == 0) {
        Actor_Kill(&this->actor);
    }
}

void func_809B4640(EnArrow* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveForward(&this->actor);

    if (DECR(this->timer) == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnArrow_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnArrow* this = (EnArrow*)thisx;
    Player* player = GET_PLAYER(play);

    if (this->isCsNut || ((this->actor.params >= ARROW_NORMAL_LIT) && (player->unk_A73 != 0)) ||
        !Player_InBlockingCsMode(play, player)) {
        this->actionFunc(this, play);
    }

    if ((this->actor.params >= ARROW_FIRE) && (this->actor.params <= ARROW_0E)) {
        s16 elementalActorIds[] = { ACTOR_ARROW_FIRE, ACTOR_ARROW_ICE,  ACTOR_ARROW_LIGHT,
                                    ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE, ACTOR_ARROW_FIRE };

        if (this->actor.child == NULL) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, elementalActorIds[this->actor.params - 3],
                               this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
        }
    } else if (this->actor.params == ARROW_NORMAL_LIT) {
        static Vec3f velocity = { 0.0f, 0.5f, 0.0f };
        static Vec3f accel = { 0.0f, 0.5f, 0.0f };
        static Color_RGBA8 primColor = { 255, 255, 100, 255 };
        static Color_RGBA8 envColor = { 255, 50, 0, 0 };
        // spawn dust for the flame
        func_8002836C(play, &this->unk_21C, &velocity, &accel, &primColor, &envColor, 100, 0, 8);
    }
}

void func_809B4800(EnArrow* this, PlayState* play) {
    static Vec3f D_809B4E88 = { 0.0f, 400.0f, 1500.0f };
    static Vec3f D_809B4E94 = { 0.0f, -400.0f, 1500.0f };
    static Vec3f D_809B4EA0 = { 0.0f, 0.0f, -300.0f };
    Vec3f sp44;
    Vec3f sp38;
    s32 addBlureVertex;

    Matrix_MultVec3f(&D_809B4EA0, &this->unk_21C);

    if (EnArrow_Fly == this->actionFunc) {
        Matrix_MultVec3f(&D_809B4E88, &sp44);
        Matrix_MultVec3f(&D_809B4E94, &sp38);

        if (this->actor.params <= ARROW_SEED) {
            addBlureVertex = this->actor.params <= ARROW_LIGHT;

            if (this->hitActor == NULL) {
                addBlureVertex &= func_80090480(play, &this->collider, &this->weaponInfo, &sp44, &sp38);
            } else {
                if (addBlureVertex) {
                    if ((sp44.x == this->weaponInfo.tip.x) && (sp44.y == this->weaponInfo.tip.y) &&
                        (sp44.z == this->weaponInfo.tip.z) && (sp38.x == this->weaponInfo.base.x) &&
                        (sp38.y == this->weaponInfo.base.y) && (sp38.z == this->weaponInfo.base.z)) {
                        addBlureVertex = false;
                    }
                }
            }

            if (addBlureVertex) {
                EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &sp44, &sp38);
            }
        }
    }
}

void EnArrow_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnArrow* this = (EnArrow*)thisx;
    u8 alpha;
    f32 scale;

    if (this->actor.params <= ARROW_0E) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        SkelAnime_DrawLod(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, this,
                          (this->actor.projectedPos.z < MREG(95)) ? 0 : 1);
    } else if (this->actor.speedXZ != 0.0f) {
        alpha = (Math_CosS(this->timer * 5000) * 127.5f) + 127.5f;

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL_25Xlu2(play->state.gfxCtx);

        if (this->actor.params == ARROW_SEED) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, alpha);
            scale = 50.0f;
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 12, 0, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 0, alpha);
            scale = 150.0f;
        }

        Matrix_Push();
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        // redundant check because this is contained in an if block for non-zero speed
        Matrix_RotateZ(
            (this->actor.speedXZ == 0.0f) ? 0.0f : ((play->gameplayFrames & 0xFF) * 4000) * (M_PI / 0x8000),
            MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                    G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
        Matrix_Pop();
        Matrix_RotateY(this->actor.world.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        CLOSE_DISPS(play->state.gfxCtx);
    }

    func_809B4800(this, play);
}
