
/*
 * File: z_en_reeba.c
 * Overlay: ovl_En_Reeba
 * Description: Leever
 */

#include "z_en_reeba.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "vt.h"
#include "objects/object_reeba/object_reeba.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_LOCKON)

void EnReeba_Init(Actor* thisx, PlayState* play);
void EnReeba_Destroy(Actor* thisx, PlayState* play);
void EnReeba_Update(Actor* thisx, PlayState* play);
void EnReeba_Draw(Actor* thisx, PlayState* play);

void func_80AE4F40(EnReeba* this, PlayState* play);
void func_80AE5054(EnReeba* this, PlayState* play);
void func_80AE5270(EnReeba* this, PlayState* play);
void func_80AE5688(EnReeba* this, PlayState* play);
void func_80AE56E0(EnReeba* this, PlayState* play);
void func_80AE538C(EnReeba* this, PlayState* play);
void func_80AE53AC(EnReeba* this, PlayState* play);
void func_80AE5E48(EnReeba* this, PlayState* play);
void func_80AE5854(EnReeba* this, PlayState* play);
void func_80AE5C38(EnReeba* this, PlayState* play);
void func_80AE5938(EnReeba* this, PlayState* play);
void func_80AE5A9C(EnReeba* this, PlayState* play);

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xE),
    /* Slingshot     */ DMG_ENTRY(1, 0xE),
    /* Explosive     */ DMG_ENTRY(2, 0xE),
    /* Boomerang     */ DMG_ENTRY(1, 0xC),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xE),
    /* Hookshot      */ DMG_ENTRY(2, 0xD),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(4, 0xE),
    /* Giant's Knife */ DMG_ENTRY(6, 0xE),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0x3),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x3),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(2, 0xE),
    /* Giant spin    */ DMG_ENTRY(8, 0xE),
    /* Master spin   */ DMG_ENTRY(4, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x1),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

const ActorInit En_Reeba_InitVars = {
    ACTOR_EN_REEBA,
    ACTORCAT_MISC,
    FLAGS,
    OBJECT_REEBA,
    sizeof(EnReeba),
    (ActorFunc)EnReeba_Init,
    (ActorFunc)EnReeba_Destroy,
    (ActorFunc)EnReeba_Update,
    (ActorFunc)EnReeba_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x08, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

void EnReeba_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;
    s32 surfaceType;

    this->actor.naviEnemyId = 0x47;
    this->actor.targetMode = 3;
    this->actor.gravity = -3.5f;
    this->actor.focus.pos = this->actor.world.pos;
    SkelAnime_Init(play, &this->skelanime, &object_reeba_Skel_001EE8, &object_reeba_Anim_0001E4, this->jointTable,
                   this->morphTable, 18);
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 4;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->isBig = this->actor.params;
    this->scale = 0.04f;

    if (this->isBig) {
        this->collider.dim.radius = 35;
        this->collider.dim.height = 45;
        this->scale *= 1.5f;
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ リーバぼす登場 ☆☆☆☆☆ %f\n" VT_RST, this->scale);
        this->actor.colChkInfo.health = 20;
        this->collider.info.toucher.effect = 4;
        this->collider.info.toucher.damage = 16;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
    }

    this->actor.shape.yOffset = this->unk_284 = this->scale * -27500.0f;
    ActorShape_Init(&this->actor.shape, this->actor.shape.yOffset, ActorShadow_DrawCircle, 0.0f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 60.0f, 60.0f, 0x1D);

    surfaceType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if ((surfaceType != 4) && (surfaceType != 7)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionfunc = func_80AE4F40;
}

void EnReeba_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    if (this->actor.parent != NULL) {
        EnEncount1* spawner = (EnEncount1*)this->actor.parent;

        if (spawner->actor.update != NULL) {
            if (spawner->curNumSpawn > 0) {
                spawner->curNumSpawn--;
            }
            if (this->isBig) {
                spawner->bigLeever = NULL;
                spawner->timer = 600;
            }
        }
    }

    ResourceMgr_UnregisterSkeleton(&this->skelanime);
}

void func_80AE4F40(EnReeba* this, PlayState* play) {
    f32 frames = Animation_GetLastFrame(&object_reeba_Anim_0001E4);
    Player* player = GET_PLAYER(play);
    s16 playerSpeed;

    Animation_Change(&this->skelanime, &object_reeba_Anim_0001E4, 2.0f, 0.0f, frames, ANIMMODE_LOOP, -10.0f);

    playerSpeed = fabsf(player->linearVelocity);
    this->unk_278 = 20 - playerSpeed * 2;
    if (this->unk_278 < 0) {
        this->unk_278 = 2;
    }
    if (this->unk_278 > 20) {
        this->unk_278 = 20;
    }

    this->actor.flags &= ~ACTOR_FLAG_NO_LOCKON;
    this->actor.world.pos.y = this->actor.floorHeight;

    if (this->isBig) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_BIG_APPEAR);
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_APPEAR);
    }

    this->actionfunc = func_80AE5054;
}

void func_80AE5054(EnReeba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerLinearVel;

    SkelAnime_Update(&this->skelanime);

    if ((play->gameplayFrames % 4) == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1,
                                 8.0f, 500, 10, true);
    }

    if (this->unk_278 == 0) {
        Math_ApproachF(&this->actor.shape.shadowScale, 12.0f, 1.0f, 1.0f);
        if (this->actor.shape.yOffset < 0.0f) {
            Math_ApproachZeroF(&this->actor.shape.yOffset, 1.0f, this->unk_288);
            Math_ApproachF(&this->unk_288, 300.0f, 1.0f, 5.0f);
        } else {
            this->unk_288 = 0.0f;
            this->actor.shape.yOffset = 0.0f;
            playerLinearVel = player->linearVelocity;

            switch (this->unk_280) {
                case 0:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    break;
                case 1:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer + (800.0f * playerLinearVel);
                    break;
                case 2:
                case 3:
                    this->actor.world.rot.y =
                        this->actor.yawTowardsPlayer +
                        (player->actor.shape.rot.y - this->actor.yawTowardsPlayer) * (playerLinearVel * 0.15f);
                    break;
                case 4:
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer - (800.0f * playerLinearVel);
                    break;
            }

            if (this->isBig) {
                this->actionfunc = func_80AE538C;
            } else {
                this->unk_272 = 130;
                this->actor.speedXZ = Rand_ZeroFloat(4.0f) + 6.0f;
                this->actionfunc = func_80AE5270;
            }
        }
    }
}

void func_80AE5270(EnReeba* this, PlayState* play) {
    s32 surfaceType;

    SkelAnime_Update(&this->skelanime);

    if (this->actor.shape.shadowScale < 12.0f) {
        Math_ApproachF(&this->actor.shape.shadowScale, 12.0f, 3.0f, 1.0f);
    }

    surfaceType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if ((surfaceType != 4) && (surfaceType != 7)) {
        this->actor.speedXZ = 0.0f;
        this->actionfunc = func_80AE5688;
    } else if ((this->unk_272 == 0) || (this->actor.xzDistToPlayer < 30.0f) || (this->actor.xzDistToPlayer > 400.0f) ||
               (this->actor.bgCheckFlags & 8)) {
        this->actionfunc = func_80AE5688;
    } else if (this->unk_274 == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_MOVE);
        this->unk_274 = 10;
    }
}

void func_80AE538C(EnReeba* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE;
    this->actionfunc = func_80AE53AC;
}

void func_80AE53AC(EnReeba* this, PlayState* play) {
    f32 speed;
    s16 yawDiff;
    s16 yaw;
    s32 surfaceType;

    SkelAnime_Update(&this->skelanime);

    if (this->actor.shape.shadowScale < 12.0f) {
        Math_ApproachF(&this->actor.shape.shadowScale, 12.0f, 3.0f, 1.0f);
    }

    surfaceType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

    if (((surfaceType != 4) && (surfaceType != 7)) || (this->actor.xzDistToPlayer > 400.0f) ||
        (this->actor.bgCheckFlags & 8)) {
        this->actionfunc = func_80AE5688;
    } else {
        if ((this->actor.xzDistToPlayer < 70.0f) && (this->unk_270 == 0)) {
            this->unk_270 = 30;
        }

        speed = (this->actor.xzDistToPlayer - 20.0f) / ((Rand_ZeroOne() * 50.0f) + 150.0f);
        this->actor.speedXZ += speed * 1.8f;
        if (this->actor.speedXZ >= 3.0f) {
            this->actor.speedXZ = 3.0f;
        }
        if (this->actor.speedXZ < -3.0f) {
            this->actor.speedXZ = -3.0f;
        }

        yawDiff = (this->unk_270 == 0) ? this->actor.yawTowardsPlayer : -this->actor.yawTowardsPlayer;
        yawDiff -= this->actor.world.rot.y;
        yaw = (yawDiff > 0) ? ((yawDiff / 31.0f) + 10.0f) : ((yawDiff / 31.0f) - 10.0f);
        this->actor.world.rot.y += yaw * 2.0f;

        if (this->unk_274 == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_MOVE);
            this->unk_274 = 20;
        }
    }
}

void func_80AE561C(EnReeba* this, PlayState* play) {
    Math_ApproachZeroF(&this->actor.speedXZ, 1.0f, 0.3f);

    if (this->unk_272 == 0) {
        if (this->isBig) {
            this->actionfunc = func_80AE538C;
        } else {
            this->actionfunc = func_80AE5688;
        }
    }
}

void func_80AE5688(EnReeba* this, PlayState* play) {
    this->unk_27E = 0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    this->actor.flags |= ACTOR_FLAG_NO_LOCKON;
    this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
    this->actionfunc = func_80AE56E0;
}

void func_80AE56E0(EnReeba* this, PlayState* play) {
    Math_ApproachZeroF(&this->actor.shape.shadowScale, 1.0f, 0.3f);
    Math_ApproachZeroF(&this->actor.speedXZ, 0.1f, 0.3f);
    SkelAnime_Update(&this->skelanime);

    if ((this->unk_284 + 10.0f) <= this->actor.shape.yOffset) {
        if ((play->gameplayFrames % 4) == 0) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1,
                                     8.0f, 500, 10, true);
        }

        Math_ApproachF(&this->actor.shape.yOffset, this->unk_284, 1.0f, this->unk_288);
        Math_ApproachF(&this->unk_288, 300.0f, 1.0f, 5.0f);
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80AE57F0(EnReeba* this, PlayState* play) {
    this->unk_276 = 14;
    this->actor.speedXZ = -8.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
    this->actionfunc = func_80AE5854;
}

void func_80AE5854(EnReeba* this, PlayState* play) {
    SkelAnime_Update(&this->skelanime);

    if (this->actor.speedXZ < 0.0f) {
        this->actor.speedXZ += 1.0f;
    }

    if (this->unk_276 == 0) {
        if (this->isBig) {
            this->unk_270 = 30;
            this->actionfunc = func_80AE538C;
        } else {
            this->actionfunc = func_80AE5688;
        }
    }
}

void func_80AE58EC(EnReeba* this, PlayState* play) {
    this->unk_278 = 14;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speedXZ = -8.0f;
    this->actor.flags |= ACTOR_FLAG_NO_LOCKON;
    this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
    this->actionfunc = func_80AE5938;
}

void func_80AE5938(EnReeba* this, PlayState* play) {
    Vec3f pos;
    f32 scale;

    if (this->unk_278 != 0) {
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ += 1.0f;
        }
    } else {
        this->actor.speedXZ = 0.0f;

        if ((this->unk_27E == 4) || (this->actor.colChkInfo.health != 0)) {
            if (this->unk_27E == 2) {
                pos.x = this->actor.world.pos.x + Rand_CenteredFloat(20.0f);
                pos.y = this->actor.world.pos.y + Rand_CenteredFloat(20.0f);
                pos.z = this->actor.world.pos.z + Rand_CenteredFloat(20.0f);
                scale = 3.0f;

                if (this->isBig) {
                    scale = 6.0f;
                }

                EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255, scale);
            }

            this->unk_278 = 66;
            this->actionfunc = func_80AE5E48;
        } else {
            this->unk_278 = 30;
            this->actionfunc = func_80AE5A9C;
        }
    }
}

void func_80AE5A9C(EnReeba* this, PlayState* play) {
    Vec3f pos;
    f32 scale;

    if (this->unk_278 != 0) {
        if ((this->unk_27E == 2) && ((this->unk_278 & 0xF) == 0)) {
            pos.x = this->actor.world.pos.x + Rand_CenteredFloat(20.0f);
            pos.y = this->actor.world.pos.y + Rand_CenteredFloat(20.0f);
            pos.z = this->actor.world.pos.z + Rand_CenteredFloat(20.0f);

            scale = 3.0f;
            if (this->isBig) {
                scale = 6.0f;
            }

            EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &pos, 150, 150, 150, 250, 235, 245, 255, scale);
        }
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_DEAD);
        Enemy_StartFinishingBlow(play, &this->actor);
        this->actionfunc = func_80AE5C38;
        GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
    }
}

void func_80AE5BC4(EnReeba* this, PlayState* play) {
    this->actor.speedXZ = -8.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
    this->unk_278 = 14;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->actionfunc = func_80AE5C38;
}

void func_80AE5C38(EnReeba* this, PlayState* play) {
    Vec3f pos;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };

    if (this->unk_278 != 0) {
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ += 1.0f;
        }
    } else {
        this->actor.speedXZ = 0.0f;
        Math_ApproachZeroF(&this->scale, 0.1f, 0.01f);

        if (this->scale < 0.01f) {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y;
            pos.z = this->actor.world.pos.z;

            velocity.y = 4.0f;

            EffectSsDeadDb_Spawn(play, &pos, &velocity, &accel, 120, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);

            if (!this->isBig) {
                Item_DropCollectibleRandom(play, &this->actor, &pos, 0xE0);
            } else {
                Item_DropCollectibleRandom(play, &this->actor, &pos, 0xC0);
            }

            if (this->actor.parent != NULL) {
                EnEncount1* spawner = (EnEncount1*)this->actor.parent;

                if ((spawner->actor.update != NULL) && !this->isBig) {
                    if (spawner->killCount < 10) {
                        spawner->killCount++;
                    }
                    // "How many are dead?"
                    osSyncPrintf("\n\n");
                    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 何匹ＤＥＡＤ？ ☆☆☆☆☆%d\n" VT_RST, spawner->killCount);
                    osSyncPrintf("\n\n");
                }

                Actor_Kill(&this->actor);
                GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
            }
        }
    }
}

void func_80AE5E48(EnReeba* this, PlayState* play) {
    if (this->unk_278 < 37) {
        this->actor.shape.rot.x = Rand_CenteredFloat(3000.0f);
        this->actor.shape.rot.z = Rand_CenteredFloat(3000.0f);

        if (this->unk_278 == 0) {
            if (this->isBig) {
                this->actionfunc = func_80AE538C;
            } else {
                this->actionfunc = func_80AE5688;
            }
        }
    }
}

void func_80AE5EDC(EnReeba* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if ((this->actionfunc != func_80AE5C38) && (this->actionfunc != func_80AE5854)) {
            this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
            this->unk_27E = 0;

            switch (this->actor.colChkInfo.damageEffect) {
                case 11: // none
                case 12: // boomerang
                    if ((this->actor.colChkInfo.health > 1) && (this->unk_27E != 4)) {
                        this->unk_27E = 4;
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0x50);
                        this->actionfunc = func_80AE58EC;
                        break;
                    }
                case 13: // hookshot/longshot
                    if ((this->actor.colChkInfo.health > 2) && (this->unk_27E != 4)) {
                        this->unk_27E = 4;
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 0x50);
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                        this->actionfunc = func_80AE58EC;
                        break;
                    }
                case 14:
                    this->unk_27C = 6;
                    Actor_ApplyDamage(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_DEAD);
                        Enemy_StartFinishingBlow(play, &this->actor);
                        this->actionfunc = func_80AE5BC4;
                    } else {
                        if (this->actionfunc == func_80AE5E48) {
                            this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
                        }
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIVA_DAMAGE);
                        this->actionfunc = func_80AE57F0;
                    }
                    break;
                case 3: // ice arrows/ice magic
                    Actor_ApplyDamage(&this->actor);
                    this->unk_27C = 2;
                    this->unk_27E = 2;
                    Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 80);
                    this->actionfunc = func_80AE58EC;
                    break;
                case 1: // unknown
                    if (this->unk_27E != 4) {
                        this->unk_27E = 4;
                        Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 80);
                        this->actionfunc = func_80AE58EC;
                    }
                    break;
            }
        }
    }
}

void EnReeba_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnReeba* this = (EnReeba*)thisx;
    Player* player = GET_PLAYER(play);

    func_80AE5EDC(this, play);
    this->actionfunc(this, play);
    Actor_SetScale(&this->actor, this->scale);

    if (this->unk_270 != 0) {
        this->unk_270--;
    }

    if (this->unk_272 != 0) {
        this->unk_272--;
    }

    if (this->unk_278 != 0) {
        this->unk_278--;
    }

    if (this->unk_274 != 0) {
        this->unk_274--;
    }

    if (this->unk_276 != 0) {
        this->unk_276--;
    }

    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 60.0f, 60.0f, 0x1D);

    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;

        if ((this->actionfunc == func_80AE5270) || (this->actionfunc == func_80AE53AC)) {
            this->actor.speedXZ = 8.0f;
            this->actor.world.rot.y *= -1.0f;
            this->unk_272 = 14;
            this->actionfunc = func_80AE561C;
            return;
        }
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if ((this->collider.base.at == &player->actor) && !this->isBig && (this->actionfunc != func_80AE56E0)) {
            this->actionfunc = func_80AE5688;
        }
    }

    this->actor.focus.pos = this->actor.world.pos;

    if (!this->isBig) {
        this->actor.focus.pos.y += 15.0f;
    } else {
        this->actor.focus.pos.y += 30.0f;
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);

    if ((this->actor.shape.yOffset >= -700.0f) && (this->actor.colChkInfo.health > 0) &&
        (this->actionfunc != func_80AE56E0)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

        if (!(this->actor.shape.yOffset < 0.0f)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

            if ((this->actionfunc == func_80AE5270) || (this->actionfunc == func_80AE53AC)) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void EnReeba_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnReeba* this = (EnReeba*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if (this->isBig) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 155, 55, 255, 255);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0x0, 0x01, 255, 255, 255, 255);
    }

    SkelAnime_DrawSkeletonOpa(play, &this->skelanime, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx);

    if (BREG(0)) {
        Vec3f debugPos;

        debugPos.x = (Math_SinS(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.x;
        debugPos.y = this->actor.world.pos.y + 20.0f;
        debugPos.z = (Math_CosS(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.z;
        DebugDisplay_AddObject(debugPos.x, debugPos.y, debugPos.z, this->actor.world.rot.x, this->actor.world.rot.y,
                               this->actor.world.rot.z, 1.0f, 1.0f, 1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
}
