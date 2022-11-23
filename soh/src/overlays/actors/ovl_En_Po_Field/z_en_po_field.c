/*
 * File: z_en_po_field.c
 * Overlay: ovl_En_Po_Field
 * Description: Field Poe
 */

#include "z_en_po_field.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_po_field/object_po_field.h"

#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_12)

void EnPoField_Init(Actor* thisx, PlayState* play);
void EnPoField_Destroy(Actor* thisx, PlayState* play);
void EnPoField_Update(Actor* thisx, PlayState* play);
void EnPoField_Draw(Actor* thisx, PlayState* play);
void EnPoField_Reset(void);

void EnPoField_UpdateDead(Actor* thisx, PlayState* play);
void EnPoField_DrawSoul(Actor* thisx, PlayState* play);

void EnPoField_SetupWaitForSpawn(EnPoField* this, PlayState* play);
void EnPoField_WaitForSpawn(EnPoField* this, PlayState* play);
void EnPoField_Appear(EnPoField* this, PlayState* play);
void EnPoField_CirclePlayer(EnPoField* this, PlayState* play);
void EnPoField_Damage(EnPoField* this, PlayState* play);
void EnPoField_Flee(EnPoField* this, PlayState* play);
void EnPoField_Death(EnPoField* this, PlayState* play);
void EnPoField_Disappear(EnPoField* this, PlayState* play);
void EnPoField_SoulIdle(EnPoField* this, PlayState* play);
void func_80AD587C(EnPoField* this, PlayState* play);
void func_80AD58D4(EnPoField* this, PlayState* play);
void EnPoField_SoulDisappear(EnPoField* this, PlayState* play);
void EnPoField_SoulInteract(EnPoField* this, PlayState* play);
void EnPoField_SpawnFlame(EnPoField* this);

const ActorInit En_Po_Field_InitVars = {
    ACTOR_EN_PO_FIELD,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PO_FIELD,
    sizeof(EnPoField),
    (ActorFunc)EnPoField_Init,
    (ActorFunc)EnPoField_Destroy,
    (ActorFunc)EnPoField_Update,
    (ActorFunc)EnPoField_Draw,
    (ActorResetFunc)EnPoField_Reset,
};

static ColliderCylinderInit D_80AD7080 = {
    {
        COLTYPE_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 50, 20, { 0, 0, 0 } },
};

static ColliderCylinderInit D_80AD70AC = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit D_80AD70D8 = { 4, 25, 50, 40 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

s32 sEnPoFieldNumSpawned = 0;

static Vec3f sFieldMiddle = { -1000.0f, 0.0f, 6500.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f D_80AD7114 = { 0.0f, 3.0f, 0.0f };

static Vec3f D_80AD7120 = { 0.0f, 0.0f, 0.0f };

static EnPoFieldInfo sPoFieldInfo[2] = {
    { { 255, 170, 255 }, { 100, 0, 150 }, { 255, 85, 0 }, 248, gPoeFieldSoulTex },
    { { 255, 255, 170 }, { 255, 200, 0 }, { 160, 0, 255 }, 241, gBigPoeSoulTex },
};

static Vec3f D_80AD714C = { 0.0f, 1400.0f, 0.0f };

Vec3s sEnPoFieldSpawnPositions[10];
u8 sEnPoFieldSpawnSwitchFlags[10];
static MtxF sLimb7Mtx;

void EnPoField_Init(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    s32 pad;

    if (sEnPoFieldNumSpawned != 10) {
        sEnPoFieldSpawnPositions[sEnPoFieldNumSpawned].x = this->actor.world.pos.x;
        sEnPoFieldSpawnPositions[sEnPoFieldNumSpawned].y = this->actor.world.pos.y;
        sEnPoFieldSpawnPositions[sEnPoFieldNumSpawned].z = this->actor.world.pos.z;
        sEnPoFieldSpawnSwitchFlags[sEnPoFieldNumSpawned] = this->actor.params & 0xFF;
        sEnPoFieldNumSpawned++;
    }
    if (sEnPoFieldNumSpawned >= 2) {
        this->actor.params = 0xFF;
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &gPoeFieldSkel, &gPoeFieldFloatAnim, this->jointTable, this->morphTable,
                   10);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &D_80AD7080);
    Collider_InitCylinder(play, &this->flameCollider);
    Collider_SetCylinder(play, &this->flameCollider, &this->actor, &D_80AD70AC);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &D_80AD70D8);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                            255, 255, 255, 0);
    this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
    EnPoField_SetupWaitForSpawn(this, play);
}

void EnPoField_Destroy(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->actor.params != 0xFF) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
        Collider_DestroyCylinder(play, &this->flameCollider);
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void EnPoField_SetupWaitForSpawn(EnPoField* this, PlayState* play) {
    this->actor.update = EnPoField_Update;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
    this->actor.shape.rot.x = 0;
    Lights_PointSetColorAndRadius(&this->lightInfo, 0, 0, 0, 0);
    this->actionTimer = 200;
    Actor_SetScale(&this->actor, 0.0f);
    this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_16);
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_ALL;
    this->actor.colChkInfo.health = D_80AD70D8.health;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actionFunc = EnPoField_WaitForSpawn;
}

void EnPoField_SetupAppear(EnPoField* this) {
    Animation_PlayOnce(&this->skelAnime, &gPoeFieldAppearAnim);
    this->actor.draw = EnPoField_Draw;
    this->lightColor.r = 255;
    this->lightColor.g = 255;
    this->lightColor.b = 210;
    this->lightColor.a = 0;
    this->actor.shape.shadowAlpha = 0;
    this->actor.shape.yOffset = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
    this->actor.home.pos.y = this->actor.world.pos.y;
    if (this->actor.params == EN_PO_FIELD_BIG) {
        this->actor.speedXZ = 12.0f;
        this->collider.dim.radius = 35;
        this->collider.dim.height = 100;
        this->collider.dim.yShift = 10;
        this->actor.shape.shadowScale = 45.0f;
        this->scaleModifier = 0.014f;
        this->actor.naviEnemyId = 0x5A;
    } else {
        this->actor.speedXZ = 0.0f;
        this->collider.dim.radius = D_80AD7080.dim.radius;
        this->collider.dim.height = D_80AD7080.dim.height;
        this->collider.dim.yShift = D_80AD7080.dim.yShift;
        this->actor.shape.shadowScale = 37.0f;
        this->scaleModifier = 0.01f;
        this->actor.naviEnemyId = 0x5C;
    }
    this->actionFunc = EnPoField_Appear;
}

void EnPoField_SetupCirclePlayer(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Animation_PlayLoop(&this->skelAnime, &gPoeFieldFloatAnim);
    this->collider.base.acFlags |= AC_ON;
    this->scaleModifier = this->actor.xzDistToPlayer;
    Math_Vec3f_Copy(&this->actor.home.pos, &player->actor.world.pos);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (this->actionFunc != EnPoField_Damage) {
        this->actor.flags |= ACTOR_FLAG_0;
        this->actionTimer = 600;
        this->unk_194 = 32;
    }
    this->actionFunc = EnPoField_CirclePlayer;
}

void EnPoField_SetupFlee(EnPoField* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeFieldFleeAnim, -5.0f);
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = EnPoField_Flee;
    this->actor.speedXZ = 12.0f;
    if (this->actionFunc != EnPoField_Damage) {
        this->actor.flags |= ACTOR_FLAG_0;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
        this->actionTimer = 2000;
        this->unk_194 = 32;
    }
}

void EnPoField_SetupDamage(EnPoField* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gPoeFieldDamagedAnim, -6.0f);
    if (this->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac) + 0x8000;
    }
    this->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    this->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 16);
    this->actionFunc = EnPoField_Damage;
}

void EnPoField_SetupDeath(EnPoField* this) {
    this->actionTimer = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.naviEnemyId = 0xFF;
    if (this->flameTimer >= 20) {
        this->flameTimer = 19;
    }
    this->actionFunc = EnPoField_Death;
}

void EnPoField_SetupDisappear(EnPoField* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeFieldDisappearAnim, -6.0f);
    this->actionTimer = 16;
    this->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    this->actionFunc = EnPoField_Disappear;
}

void EnPoField_SetupSoulIdle(EnPoField* this, PlayState* play) {
    this->actor.update = EnPoField_UpdateDead;
    this->actor.draw = EnPoField_DrawSoul;
    this->actor.shape.shadowDraw = NULL;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.gravity = -1.0f;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.shape.rot.x = -0x8000;
    this->actionTimer = 60;
    this->actor.world.pos.y -= 15.0f;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actionFunc = EnPoField_SoulIdle;
}

void func_80AD42B0(EnPoField* this) {
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->actor.shape.rot.y = 0;
    this->lightColor.a = 0;
    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.home.pos.y = this->actor.world.pos.y;
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_METAL_BOX_BOUND);
    if (this->actor.params == EN_PO_FIELD_BIG) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
    this->actionFunc = func_80AD587C;
}

void func_80AD4384(EnPoField* this) {
    this->actor.home.pos.y = this->actor.world.pos.y;
    Actor_SetFocus(&this->actor, -10.0f);
    this->collider.dim.radius = 13;
    this->collider.dim.height = 30;
    this->collider.dim.yShift = 0;
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y - 20.0f;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    this->actor.textId = 0x5005;
    this->actionTimer = 400;
    this->unk_194 = 32;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actionFunc = func_80AD58D4;
}

void EnPoField_SetupSoulDisappear(EnPoField* this) {
    this->actionFunc = EnPoField_SoulDisappear;
}

void EnPoField_SetupInteractWithSoul(EnPoField* this) {
    this->actionFunc = EnPoField_SoulInteract;
    this->actor.home.pos.y = this->actor.world.pos.y - 15.0f;
}

void EnPoField_CorrectYPos(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_194 == 0) {
        this->unk_194 = 32;
    }
    if (this->unk_194 != 0) {
        this->unk_194 -= 1;
    }
    if (this->actor.floorHeight == BGCHECK_Y_MIN) {
        EnPoField_SetupDisappear(this);
        return;
    }
    Math_ApproachF(
        &this->actor.home.pos.y,
        ((player->actor.world.pos.y > this->actor.floorHeight) ? player->actor.world.pos.y : this->actor.floorHeight) +
            13.0f,
        0.2f, 5.0f);
    this->actor.world.pos.y = Math_SinS(this->unk_194 * 0x800) * 13.0f + this->actor.home.pos.y;
}

f32 EnPoField_SetFleeSpeed(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 speed = ((player->stateFlags1 & 0x800000) && player->rideActor != NULL) ? player->rideActor->speedXZ : 12.0f;

    if (this->actor.xzDistToPlayer < 300.0f) {
        this->actor.speedXZ = speed * 1.5f + 2.0f;
    } else if (this->actor.xzDistToPlayer < 400.0f) {
        this->actor.speedXZ = speed * 1.25f + 2.0f;
    } else if (this->actor.xzDistToPlayer < 500.0f) {
        this->actor.speedXZ = speed + 2.0f;
    } else {
        this->actor.speedXZ = 12.0f;
    }
    this->actor.speedXZ = CLAMP_MIN(this->actor.speedXZ, 12.0f);
}

void EnPoField_WaitForSpawn(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 spawnDist;
    s32 i;
    s32 bgId;

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actionTimer == 0) {
        for (i = 0; i < sEnPoFieldNumSpawned; i++) {
            if (fabsf(sEnPoFieldSpawnPositions[i].x - player->actor.world.pos.x) < 150.0f &&
                fabsf(sEnPoFieldSpawnPositions[i].z - player->actor.world.pos.z) < 150.0f) {
                if (Flags_GetSwitch(play, sEnPoFieldSpawnSwitchFlags[i])) {
                    if (player->stateFlags1 & 0x800000) { // Player riding Epona
                        return;
                    } else {
                        this->actor.params = EN_PO_FIELD_SMALL;
                        spawnDist = 300.0f;
                    }
                } else if (player->stateFlags1 & 0x800000 || Rand_ZeroOne() < 0.4f) {
                    this->actor.params = EN_PO_FIELD_BIG;
                    this->spawnFlagIndex = i;
                    spawnDist = 480.0f;
                } else {
                    this->actor.params = EN_PO_FIELD_SMALL;
                    spawnDist = 300.0f;
                }
                this->actor.world.pos.x = Math_SinS(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.x;
                this->actor.world.pos.z = Math_CosS(player->actor.shape.rot.y) * spawnDist + player->actor.world.pos.z;
                this->actor.world.pos.y = player->actor.world.pos.y + 1000.0f;
                this->actor.world.pos.y = BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &bgId,
                                                                      &this->actor, &this->actor.world.pos);
                if (this->actor.world.pos.y != BGCHECK_Y_MIN) {
                    this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, &player->actor);
                    EnPoField_SetupAppear(this);
                } else {
                    return;
                }
            }
        }
    }
}

void EnPoField_Appear(EnPoField* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->lightColor.a = 255;
        Actor_SetScale(&this->actor, this->scaleModifier);
        if (this->actor.params == EN_PO_FIELD_BIG) {
            EnPoField_SetupFlee(this);
        } else {
            EnPoField_SetupCirclePlayer(this, play);
        }
    } else if (this->skelAnime.curFrame > 10.0f) {
        this->lightColor.a = ((this->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    } else {
        this->actor.scale.x += this->scaleModifier * 0.1f;
        this->actor.scale.y = this->actor.scale.x;
        this->actor.scale.z = this->actor.scale.x;
    }
    this->actor.shape.shadowAlpha = this->lightColor.a;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.params == EN_PO_FIELD_BIG) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
        EnPoField_SetFleeSpeed(this, play);
    }
}

void EnPoField_CirclePlayer(EnPoField* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 temp_v1 = 16 - this->unk_194;

    SkelAnime_Update(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (ABS(temp_v1) < 16) {
        this->actor.world.rot.y += 512.0f * fabsf(Math_SinS(this->unk_194 * 0x800));
    }
    Math_ApproachF(&this->scaleModifier, 180.0f, 0.5f, 10.0f);
    Math_ApproachF(&this->actor.home.pos.x, player->actor.world.pos.x, 0.2f, 6.0f);
    Math_ApproachF(&this->actor.home.pos.z, player->actor.world.pos.z, 0.2f, 6.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 1, 0x800, 0x200);
    if (this->actor.home.pos.x - player->actor.world.pos.x > 100.0f) {
        this->actor.home.pos.x = player->actor.world.pos.x + 100.0f;
    } else if (this->actor.home.pos.x - player->actor.world.pos.x < -100.0f) {
        this->actor.home.pos.x = player->actor.world.pos.x + -100.0f;
    }
    if (this->actor.home.pos.z - player->actor.world.pos.z > 100.0f) {
        this->actor.home.pos.z = player->actor.world.pos.z + 100.0f;
    } else if (this->actor.home.pos.z - player->actor.world.pos.z < -100.0f) {
        this->actor.home.pos.z = player->actor.world.pos.z + -100.0f;
    }
    this->actor.world.pos.x = this->actor.home.pos.x - (Math_SinS(this->actor.world.rot.y) * this->scaleModifier);
    this->actor.world.pos.z = this->actor.home.pos.z - (Math_CosS(this->actor.world.rot.y) * this->scaleModifier);
    if (this->actionTimer == 0) {
        EnPoField_SetupDisappear(this);
    } else {
        EnPoField_SpawnFlame(this);
    }
    EnPoField_CorrectYPos(this, play);
    func_8002F974(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void EnPoField_Flee(EnPoField* this, PlayState* play) {
    f32 temp_f6;
    s16 phi_t0;

    SkelAnime_Update(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (Actor_WorldDistXZToPoint(&this->actor, &sFieldMiddle) > 3000.0f) {
        phi_t0 = (s16)(this->actor.yawTowardsPlayer - Actor_WorldYawTowardPoint(&this->actor, &sFieldMiddle) - 0x8000) *
                 0.2f;
    } else {
        phi_t0 = 0;
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer - phi_t0, 6, 0x400);
    EnPoField_SetFleeSpeed(this, play);
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    temp_f6 = Math_SinS(this->actionTimer * 0x800) * 3.0f;
    this->actor.world.pos.x -= temp_f6 * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.z += temp_f6 * Math_SinS(this->actor.shape.rot.y);
    if (this->actionTimer == 0 || this->actor.xzDistToPlayer > 1500.0f) {
        EnPoField_SetupDisappear(this);
    } else {
        EnPoField_CorrectYPos(this, play);
    }
    func_8002F974(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void EnPoField_Damage(EnPoField* this, PlayState* play) {
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            EnPoField_SetupDeath(this);
        } else if (this->actor.params == EN_PO_FIELD_BIG) {
            EnPoField_SetupFlee(this);
        } else {
            EnPoField_SetupCirclePlayer(this, play);
        }
    }
}

void EnPoField_Death(EnPoField* this, PlayState* play) {
    Vec3f sp6C;
    f32 sp68;
    s32 pad;
    s32 pad1;
    f32 temp_f0;

    this->actionTimer++;
    if (this->actionTimer < 8) {
        if (this->actionTimer < 5) {
            sp6C.y = Math_SinS(this->actionTimer * 0x1000 - 0x4000) * 23.0f + (this->actor.world.pos.y + 40.0f);
            sp68 = Math_CosS(this->actionTimer * 0x1000 - 0x4000) * 23.0f;
            sp6C.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * sp68 + this->actor.world.pos.x;
            sp6C.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * sp68 + this->actor.world.pos.z;
        } else {
            sp6C.y = this->actor.world.pos.y + 40.0f + 15.0f * (this->actionTimer - 5);
            sp6C.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.x;
            sp6C.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.z;
        }
        EffectSsDeadDb_Spawn(play, &sp6C, &D_80AD7114, &D_80AD7120, this->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        sp6C.x = (this->actor.world.pos.x + this->actor.world.pos.x) - sp6C.x;
        sp6C.z = (this->actor.world.pos.z + this->actor.world.pos.z) - sp6C.z;
        EffectSsDeadDb_Spawn(play, &sp6C, &D_80AD7114, &D_80AD7120, this->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        sp6C.x = this->actor.world.pos.x;
        sp6C.z = this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &sp6C, &D_80AD7114, &D_80AD7120, this->actionTimer * 10 + 80, 0, 255, 255, 255,
                             255, 0, 0, 255, 1, 9, 1);
        if (this->actionTimer == 1) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_EXTINCT);
            if (this->actor.params == EN_PO_FIELD_BIG) {
                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_BIG]++;
            } else {
                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE]++;
            }
        }
    } else if (this->actionTimer == 28) {
        EnPoField_SetupSoulIdle(this, play);
    } else if (this->actionTimer >= 19) {
        temp_f0 = (28 - this->actionTimer) * 0.001f;
        this->actor.world.pos.y += 5.0f;
        this->actor.scale.z = temp_f0;
        this->actor.scale.y = temp_f0;
        this->actor.scale.x = temp_f0;
    }
    if (this->actionTimer == 18) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

void EnPoField_Disappear(EnPoField* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    this->actor.shape.rot.y += 0x1000;
    this->lightColor.a = this->actionTimer * 15.9375f;
    this->actor.shape.shadowAlpha = this->lightColor.a;
    if (this->actionTimer == 0) {
        EnPoField_SetupWaitForSpawn(this, play);
    }
}

void EnPoField_SoulIdle(EnPoField* this, PlayState* play) {
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actor.bgCheckFlags & 1) {
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 6.0f, 0, 1, 1, 15, OBJECT_PO_FIELD, 10,
                                 gPoeFieldLanternDL);
        func_80AD42B0(this);
    } else if (this->actionTimer == 0) {
        EnPoField_SetupWaitForSpawn(this, play);
    }
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, 4);
}

void EnPoField_SoulUpdateProperties(EnPoField* this, s32 arg1) {
    EnPoFieldInfo* info = &sPoFieldInfo[this->actor.params];
    f32 multiplier;

    this->lightColor.a = CLAMP(this->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = this->lightColor.a * (1.0f / 255);
        this->actor.scale.x = this->actor.scale.z = 0.0056000003f * multiplier + 0.0014000001f;
        this->actor.scale.y = 0.007f - 0.007f * multiplier + 0.007f;
    } else {
        multiplier = 1.0f;
        this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = this->lightColor.a * (0.007f / 255);
        this->actor.world.pos.y = this->actor.home.pos.y + ((1.0f / 17.0f) * this->lightColor.a);
    }
    this->lightColor.r = info->lightColor.r * multiplier;
    this->lightColor.g = info->lightColor.g * multiplier;
    this->lightColor.b = info->lightColor.b * multiplier;
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, info->lightColor.r, info->lightColor.g, info->lightColor.b,
                              this->lightColor.a * (200.0f / 255));
}

void func_80AD587C(EnPoField* this, PlayState* play) {
    this->actor.home.pos.y += 2.0f;
    EnPoField_SoulUpdateProperties(this, 20);
    if (this->lightColor.a == 255) {
        func_80AD4384(this);
    }
}

void func_80AD58D4(EnPoField* this, PlayState* play) {
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        EnPoField_SetupInteractWithSoul(this);
        return;
    }
    if (this->actionTimer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
        this->actor.flags &= ~ACTOR_FLAG_16;
        EnPoField_SetupSoulDisappear(this);
        return;
    }
    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->actor.flags |= ACTOR_FLAG_16;
        func_8002F2F4(&this->actor, play);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_16;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    this->actor.world.pos.y = Math_SinS(this->unk_194 * 0x800) * 5.0f + this->actor.home.pos.y;
    if (this->unk_194 != 0) {
        this->unk_194 -= 1;
    }
    if (this->unk_194 == 0) {
        this->unk_194 = 32;
    }
    this->collider.dim.pos.y = this->actor.world.pos.y - 20.0f;
    Actor_SetFocus(&this->actor, -10.0f);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->lightInfo.params.point.color[0],
                              this->lightInfo.params.point.color[1], this->lightInfo.params.point.color[2],
                              this->lightColor.a * (200.0f / 255));
}

void EnPoField_SoulDisappear(EnPoField* this, PlayState* play) {
    EnPoField_SoulUpdateProperties(this, -13);
    if (this->lightColor.a == 0) {
        EnPoField_SetupWaitForSpawn(this, play);
    }
}

void EnPoField_SoulInteract(EnPoField* this, PlayState* play) {
    if (this->actor.textId != 0x5005) {
        EnPoField_SoulUpdateProperties(this, -13);
    } else {
        func_8002F974(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (play->msgCtx.choiceIndex == 0) {
                if (Inventory_HasEmptyBottle()) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_BIG_GET);
                    if (this->actor.params == 0) {
                        Item_Give(play, ITEM_POE);
                        this->actor.textId = 0x5008;
                    } else {
                        this->actor.textId = 0x508F;
                        Item_Give(play, ITEM_BIG_POE);
                        Flags_SetSwitch(play, sEnPoFieldSpawnSwitchFlags[this->spawnFlagIndex]);
                    }
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
                    this->actor.textId = 0x5006;
                }
            } else {
                this->actor.textId = 0x5007;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
            }
            Message_ContinueTextbox(play, this->actor.textId);
            return;
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnPoField_SetupSoulDisappear(this);
    }
}

void EnPoField_TestForDamage(EnPoField* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DAMAGE);
            }
            EnPoField_SetupDamage(this);
        }
    }
}

void EnPoField_SpawnFlame(EnPoField* this) {
    if (this->flameTimer == 0) {
        this->flamePosition.x = this->lightInfo.params.point.x;
        this->flamePosition.y = this->lightInfo.params.point.y;
        this->flamePosition.z = this->lightInfo.params.point.z;
        this->flameTimer = 70;
        this->flameRotation = this->actor.shape.rot.y;
    }
}

void EnPoField_UpdateFlame(EnPoField* this, PlayState* play) {
    if (this->flameTimer != 0) {
        if (this->flameTimer != 0) {
            this->flameTimer--;
        }
        if (this->flameCollider.base.atFlags & AT_HIT) {
            this->flameCollider.base.atFlags &= ~AT_HIT;
            this->flameTimer = 19;
        }
        if (this->flameTimer < 20) {
            Math_StepToF(&this->flameScale, 0.0f, 0.00015f);
            return;
        }
        if (Math_StepToF(&this->flameScale, 0.003f, 0.0006f) != 0) {
            this->flamePosition.x += 2.5f * Math_SinS(this->flameRotation);
            this->flamePosition.z += 2.5f * Math_CosS(this->flameRotation);
        }
        this->flameCollider.dim.pos.x = this->flamePosition.x;
        this->flameCollider.dim.pos.y = this->flamePosition.y;
        this->flameCollider.dim.pos.z = this->flamePosition.z;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->flameCollider.base);
    }
}

void EnPoField_DrawFlame(EnPoField* this, PlayState* play) {
    f32 sp4C;
    s32 pad;

    if (this->flameTimer != 0) {
        OPEN_DISPS(play->state.gfxCtx);
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    (play->gameplayFrames * -20) % 512, 32, 128));
        sp4C = this->flameScale * 85000.0f;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, sp4C);
        Matrix_Translate(this->flamePosition.x, this->flamePosition.y, this->flamePosition.z, MTXMODE_NEW);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000) * (M_PI / 0x8000), MTXMODE_APPLY);
        if (this->flameTimer >= 20) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            Matrix_Scale(this->flameScale, this->flameScale, this->flameScale, MTXMODE_APPLY);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, sp4C, 0, 0, 0);
            Matrix_Scale((this->flameScale * 0.7f) + 0.00090000004f, (0.003f - this->flameScale) + 0.003f, 0.003f,
                         MTXMODE_APPLY);
        }
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80AD619C(EnPoField* this) {
    s16 temp_var;

    if (this->actionFunc == EnPoField_Flee) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 80);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        temp_var = this->lightColor.b + 5;
        this->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (this->actionFunc == EnPoField_Damage) {
        if (this->actor.colorFilterTimer & 2) {
            this->lightColor.r = 0;
            this->lightColor.g = 0;
            this->lightColor.b = 0;
        } else {
            this->lightColor.r = 80;
            this->lightColor.g = 255;
            this->lightColor.b = 225;
        }
    } else {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 255);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        if (this->lightColor.b > 210) {
            temp_var = this->lightColor.b - 5;
            this->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->lightColor.b + 5;
            this->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void func_80AD6330(EnPoField* this) {
    f32 rand;

    if (this->actionFunc == EnPoField_Appear && this->skelAnime.curFrame < 12.0f) {
        this->soulColor.r = this->soulColor.g = this->soulColor.b = (s16)(this->skelAnime.curFrame * 16.66f) + 55;
        this->soulColor.a = this->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = Rand_ZeroOne();
        this->soulColor.r = (s16)(rand * 30.0f) + 225;
        this->soulColor.g = (s16)(rand * 100.0f) + 155;
        this->soulColor.b = (s16)(rand * 160.0f) + 95;
        this->soulColor.a = 200;
    }
}

void EnPoField_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoField* this = (EnPoField*)thisx;

    EnPoField_TestForDamage(this, play);
    this->actionFunc(this, play);
    EnPoField_UpdateFlame(this, play);
    if (this->actionFunc == EnPoField_Flee || this->actionFunc == EnPoField_Damage ||
        this->actionFunc == EnPoField_Appear) {
        Actor_MoveForward(&this->actor);
    }
    if (this->actionFunc != EnPoField_WaitForSpawn) {
        Actor_SetFocus(&this->actor, 42.0f);
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 27.0f, 60.0f, 4);
        func_80AD619C(this);
        func_80AD6330(this);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 EnPoField_OverrideLimbDraw2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx, Gfx** gfxP) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->lightColor.a == 0 || limbIndex == 7 || (this->actionFunc == EnPoField_Death && this->actionTimer >= 2)) {
        *dList = NULL;
    } else if (this->actor.params == EN_PO_FIELD_BIG) {
        if (limbIndex == 1) {
            *dList = gBigPoeFaceDL;
        } else if (limbIndex == 8) {
            *dList = gBigPoeCloakDL;
        } else if (limbIndex == 9) {
            *dList = gBigPoeBodyDL;
        }
    }
    if (this->actionFunc == EnPoField_Disappear && limbIndex == 7) {
        Matrix_Scale(this->actionTimer / 16.0f, this->actionTimer / 16.0f, this->actionTimer / 16.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnPoField_PostLimDraw2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoField* this = (EnPoField*)thisx;

    if (this->actionFunc == EnPoField_Death && this->actionTimer >= 2 && limbIndex == 8) {
        gSPMatrix((*gfxP)++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, gPoeFieldBurnDL);
    }
    if (limbIndex == 7) {
        Vec3f vec;
        Matrix_MultVec3f(&D_80AD714C, &vec);
        if (this->actionFunc == EnPoField_Death && this->actionTimer >= 19 && this->actor.scale.x != 0.0f) {
            f32 mtxScale = 0.01f / this->actor.scale.x;
            Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_Get(&sLimb7Mtx);
        if (this->actionFunc == EnPoField_Death && this->actionTimer == 27) {
            this->actor.world.pos.x = sLimb7Mtx.xw;
            this->actor.world.pos.y = sLimb7Mtx.yw;
            this->actor.world.pos.z = sLimb7Mtx.zw;
        }
        Lights_PointGlowSetInfo(&this->lightInfo, vec.x, vec.y, vec.z, this->soulColor.r, this->soulColor.g,
                                this->soulColor.b, this->soulColor.a * (200.0f / 255));
    }
}

void EnPoField_Draw(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    EnPoFieldInfo* info = &sPoFieldInfo[this->actor.params];

    if (this->actionFunc != EnPoField_WaitForSpawn) {
        OPEN_DISPS(play->state.gfxCtx);
        func_80093D18(play->state.gfxCtx);
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(play->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        if (this->lightColor.a == 255 || this->lightColor.a == 0) {
            gSPSegment(POLY_OPA_DISP++, 0x08,
                       Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                    this->lightColor.a));
            gSPSegment(POLY_OPA_DISP++, 0x0C, D_80116280 + 2);
            POLY_OPA_DISP =
                SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                               EnPoField_OverrideLimbDraw2, EnPoField_PostLimDraw2, &this->actor, POLY_OPA_DISP);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                    this->lightColor.a));
            gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
            POLY_XLU_DISP =
                SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                               EnPoField_OverrideLimbDraw2, EnPoField_PostLimDraw2, &this->actor, POLY_XLU_DISP);
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, this->soulColor.r, this->soulColor.g, this->soulColor.b, 255);
        Matrix_Put(&sLimb7Mtx);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
        CLOSE_DISPS(play->state.gfxCtx);
    }
    EnPoField_DrawFlame(this, play);
}

void EnPoField_UpdateDead(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc == EnPoField_SoulIdle) {
        func_80AD6330(this);
    }
    EnPoField_UpdateFlame(this, play);
}

void EnPoField_DrawSoul(Actor* thisx, PlayState* play) {
    EnPoField* this = (EnPoField*)thisx;
    s32 pad;
    EnPoFieldInfo* info = &sPoFieldInfo[this->actor.params];

    OPEN_DISPS(play->state.gfxCtx);
    if (this->actionFunc == EnPoField_SoulIdle) {
        func_80093D18(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(play->state.gfxCtx, info->envColor.r, info->envColor.g, info->envColor.b, 255));
        Lights_PointGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, this->soulColor.r, this->soulColor.g, this->soulColor.b, 200);
        gDPSetEnvColor(POLY_OPA_DISP++, this->soulColor.r, this->soulColor.g, this->soulColor.b, 255);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternDL);
        gSPDisplayList(POLY_OPA_DISP++, gPoeFieldLanternTopDL);
    } else {
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (play->gameplayFrames * info->unk_9) & 0x1FF, 0x20, 0x80));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(info->soulTexture));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, info->primColor.r, info->primColor.g, info->primColor.b,
                        this->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 255);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gPoeFieldSoulDL);
    }
    CLOSE_DISPS(play->state.gfxCtx);
    EnPoField_DrawFlame(this, play);
}

void EnPoField_Reset(void) {
    sEnPoFieldNumSpawned = 0;

    memset(sEnPoFieldSpawnPositions, 0, sizeof(sEnPoFieldSpawnPositions));
    memset(sEnPoFieldSpawnSwitchFlags, 0, sizeof(sEnPoFieldSpawnSwitchFlags));
}