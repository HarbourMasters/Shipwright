/*
 * File: z_en_poh.c
 * Overlay: ovl_En_Poh
 * Description: Graveyard Poe
 */

#include "z_en_poh.h"
#include "objects/object_poh/object_poh.h"
#include "objects/object_po_composer/object_po_composer.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_12)

void EnPoh_Init(Actor* thisx, PlayState* play);
void EnPoh_Destroy(Actor* thisx, PlayState* play);
void EnPoh_Update(Actor* thisx, PlayState* play);

void EnPoh_UpdateLiving(Actor* thisx, PlayState* play);
void EnPoh_UpdateDead(Actor* thisx, PlayState* play);
void EnPoh_DrawRegular(Actor* thisx, PlayState* play);
void EnPoh_DrawComposer(Actor* thisx, PlayState* play);
void EnPoh_DrawSoul(Actor* thisx, PlayState* play);

void func_80ADEAC4(EnPoh* this, PlayState* play);
void EnPoh_Idle(EnPoh* this, PlayState* play);
void func_80ADEC9C(EnPoh* this, PlayState* play);
void EnPoh_Attack(EnPoh* this, PlayState* play);
void func_80ADEECC(EnPoh* this, PlayState* play);
void func_80ADF894(EnPoh* this, PlayState* play);
void EnPoh_ComposerAppear(EnPoh* this, PlayState* play);
void func_80ADEF38(EnPoh* this, PlayState* play);
void func_80ADF15C(EnPoh* this, PlayState* play);
void func_80ADF574(EnPoh* this, PlayState* play);
void func_80ADF5E0(EnPoh* this, PlayState* play);
void EnPoh_Disappear(EnPoh* this, PlayState* play);
void EnPoh_Appear(EnPoh* this, PlayState* play);
void EnPoh_Death(EnPoh* this, PlayState* play);
void func_80ADFE28(EnPoh* this, PlayState* play);
void func_80ADFE80(EnPoh* this, PlayState* play);
void func_80AE009C(EnPoh* this, PlayState* play);
void EnPoh_TalkRegular(EnPoh* this, PlayState* play);
void EnPoh_TalkComposer(EnPoh* this, PlayState* play);

static s16 D_80AE1A50 = 0;

const ActorInit En_Poh_InitVars = {
    ACTOR_EN_POH,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnPoh),
    (ActorFunc)EnPoh_Init,
    (ActorFunc)EnPoh_Destroy,
    (ActorFunc)EnPoh_Update,
    NULL,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
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
    { 20, 40, 20, { 0, 0, 0 } },
};

static ColliderJntSphElementInit D_80AE1AA0[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 18, { { 0, 1400, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    D_80AE1AA0,
};

static CollisionCheckInfoInit sColChkInfoInit = { 4, 25, 50, 40 };

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

static EnPohInfo sPoeInfo[2] = {
    {
        { 255, 170, 255 },
        { 100, 0, 150 },
        18,
        5,
        248,
        &gPoeDisappearAnim,
        &gPoeFloatAnim,
        &gPoeDamagedAnim,
        &gPoeFleeAnim,
        gPoeLanternDL,
        gPoeBurnDL,
        gPoeSoulDL,
    },
    {
        { 255, 255, 170 },
        { 0, 150, 0 },
        9,
        1,
        244,
        &gPoeComposerDisappearAnim,
        &gPoeComposerFloatAnim,
        &gPoeComposerDamagedAnim,
        &gPoeComposerFleeAnim,
        gPoeComposerLanternDL,
        gPoeComposerBurnDL,
        gPoeComposerSoulDL,
    },
};

static Color_RGBA8 D_80AE1B4C = { 75, 20, 25, 255 };
static Color_RGBA8 D_80AE1B50 = { 80, 110, 90, 255 };
static Color_RGBA8 D_80AE1B54 = { 90, 85, 50, 255 };
static Color_RGBA8 D_80AE1B58 = { 100, 90, 100, 255 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f D_80AE1B60 = { 0.0f, 3.0f, 0.0f };
static Vec3f D_80AE1B6C = { 0.0f, 0.0f, 0.0f };

void EnPoh_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnItem00* collectible;
    EnPoh* this = (EnPoh*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    Collider_InitJntSph(play, &this->colliderSph);
    Collider_SetJntSph(play, &this->colliderSph, &this->actor, &sJntSphInit, &this->colliderSphItem);
    this->colliderSph.elements[0].dim.worldSphere.radius = 0;
    this->colliderSph.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->colliderSph.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->colliderSph.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
    Collider_InitCylinder(play, &this->colliderCyl);
    Collider_SetCylinder(play, &this->colliderCyl, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->unk_194 = 0;
    this->unk_195 = 32;
    this->visibilityTimer = Rand_S16Offset(700, 300);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                            255, 255, 255, 0);
    if (this->actor.params >= 4) {
        this->actor.params = EN_POH_NORMAL;
    }
    if (this->actor.params == EN_POH_RUPEE) {
        D_80AE1A50++;
        if (D_80AE1A50 >= 3) {
            Actor_Kill(&this->actor);
        } else {
            collectible = Item_DropCollectible(play, &this->actor.world.pos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (collectible != NULL) {
                collectible->actor.speedXZ = 0.0f;
            }
        }
    } else if (this->actor.params == EN_POH_FLAT) {
        if (Flags_GetSwitch(play, 0x28) || Flags_GetSwitch(play, 0x9)) {
            Actor_Kill(&this->actor);
        } else {
            Flags_SetSwitch(play, 0x28);
        }
    } else if (this->actor.params == EN_POH_SHARP) {
        if (Flags_GetSwitch(play, 0x29) || Flags_GetSwitch(play, 0x9)) {
            Actor_Kill(&this->actor);
        } else {
            Flags_SetSwitch(play, 0x29);
        }
    }
    if (this->actor.params < EN_POH_SHARP) {
        this->objectIdx = Object_GetIndex(&play->objectCtx, OBJECT_POH);
        this->infoIdx = EN_POH_INFO_NORMAL;
        this->actor.naviEnemyId = 0x44;
    } else {
        this->objectIdx = Object_GetIndex(&play->objectCtx, OBJECT_PO_COMPOSER);
        this->infoIdx = EN_POH_INFO_COMPOSER;
        this->actor.naviEnemyId = 0x43;
    }
    this->info = &sPoeInfo[this->infoIdx];
    if (this->objectIdx < 0) {
        Actor_Kill(&this->actor);
    }
}

void EnPoh_Destroy(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    Collider_DestroyJntSph(play, &this->colliderSph);
    Collider_DestroyCylinder(play, &this->colliderCyl);
    if (this->actor.params == EN_POH_RUPEE) {
        D_80AE1A50--;
    }
}

void func_80ADE114(EnPoh* this) {
    Animation_PlayLoop(&this->skelAnime, this->info->idleAnim);
    this->unk_198 = Rand_S16Offset(2, 3);
    this->actionFunc = func_80ADEAC4;
    this->actor.speedXZ = 0.0f;
}

void EnPoh_SetupIdle(EnPoh* this) {
    Animation_PlayLoop(&this->skelAnime, this->info->idleAnim2);
    this->unk_198 = Rand_S16Offset(15, 3);
    this->actionFunc = EnPoh_Idle;
}

void func_80ADE1BC(EnPoh* this) {
    Animation_PlayLoop(&this->skelAnime, this->info->idleAnim2);
    this->actionFunc = func_80ADEC9C;
    this->unk_198 = 0;
    this->actor.speedXZ = 2.0f;
}

void EnPoh_SetupAttack(EnPoh* this) {
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_MorphToLoop(&this->skelAnime, &gPoeAttackAnim, -6.0f);
    } else {
        Animation_PlayLoop(&this->skelAnime, &gPoeComposerAttackAnim);
    }
    this->unk_198 = 12;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = EnPoh_Attack;
}

void func_80ADE28C(EnPoh* this) {
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gPoeDamagedAnim, -6.0f);
    } else {
        Animation_PlayOnce(&this->skelAnime, &gPoeComposerDamagedAnim);
    }
    if (this->colliderCyl.info.acHitInfo->toucher.dmgFlags & 0x0001F824) {
        this->actor.world.rot.y = this->colliderCyl.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, this->colliderCyl.base.ac) + 0x8000;
    }
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->actor.speedXZ = 5.0f;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0x10);
    this->actionFunc = func_80ADEECC;
}

void func_80ADE368(EnPoh* this) {
    Animation_MorphToLoop(&this->skelAnime, this->info->fleeAnim, -5.0f);
    this->actor.speedXZ = 5.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->unk_198 = 200;
    this->actionFunc = func_80ADF894;
}

void EnPoh_SetupInitialAction(EnPoh* this) {
    this->lightColor.a = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Animation_PlayOnceSetSpeed(&this->skelAnime, &gPoeAppearAnim, 0.0f);
        this->actionFunc = func_80ADEF38;
    } else {
        Animation_PlayOnceSetSpeed(&this->skelAnime, &gPoeComposerAppearAnim, 1.0f);
        this->actor.world.pos.y = this->actor.home.pos.y + 20.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
        this->actionFunc = EnPoh_ComposerAppear;
    }
}

void func_80ADE48C(EnPoh* this) {
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_198 = 0;
    this->actor.naviEnemyId = 0xFF;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actionFunc = func_80ADF15C;
}

void func_80ADE4C8(EnPoh* this) {
    Animation_PlayOnce(&this->skelAnime, this->info->idleAnim2);
    this->actionFunc = func_80ADF574;
    this->actor.speedXZ = -5.0f;
}

void func_80ADE514(EnPoh* this) {
    Animation_PlayLoop(&this->skelAnime, this->info->idleAnim);
    this->unk_19C = this->actor.world.rot.y + 0x8000;
    this->actionFunc = func_80ADF5E0;
    this->actor.speedXZ = 0.0f;
}

void EnPoh_SetupDisappear(EnPoh* this) {
    this->unk_194 = 32;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = EnPoh_Disappear;
}

void EnPoh_SetupAppear(EnPoh* this) {
    this->unk_194 = 0;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = EnPoh_Appear;
}

void EnPoh_SetupDeath(EnPoh* this, PlayState* play) {
    this->actor.update = EnPoh_UpdateDead;
    this->actor.draw = EnPoh_DrawSoul;
    this->actor.shape.shadowDraw = NULL;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.flags |= ACTOR_FLAG_4;
    this->actor.gravity = -1.0f;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.world.pos.y -= 15.0f;
    if (this->infoIdx != EN_POH_INFO_COMPOSER) {
        this->actor.shape.rot.x = -0x8000;
    }
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, 8);
    this->unk_198 = 60;
    this->actionFunc = EnPoh_Death;
}

void func_80ADE6D4(EnPoh* this) {
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->visibilityTimer = 0;
    this->actor.shape.rot.y = 0;
    this->lightColor.r = 0;
    this->lightColor.a = 0;
    this->actor.shape.yOffset = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    if (this->actor.params >= EN_POH_SHARP) {
        this->lightColor.g = 200;
        this->lightColor.b = 0;
    } else {
        this->lightColor.g = 0;
        this->lightColor.b = 200;
    }
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.0f;
    this->actor.shape.rot.x = 0;
    this->actor.home.pos.y = this->actor.world.pos.y;
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_METAL_BOX_BOUND);
    this->actionFunc = func_80ADFE28;
}

void EnPoh_Talk(EnPoh* this, PlayState* play) {
    this->actor.home.pos.y = this->actor.world.pos.y;
    Actor_SetFocus(&this->actor, -10.0f);
    this->colliderCyl.dim.radius = 13;
    this->colliderCyl.dim.height = 30;
    this->colliderCyl.dim.yShift = 0;
    this->colliderCyl.dim.pos.x = this->actor.world.pos.x;
    this->colliderCyl.dim.pos.y = this->actor.world.pos.y - 20.0f;
    this->colliderCyl.dim.pos.z = this->actor.world.pos.z;
    this->colliderCyl.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    if (this->actor.params == EN_POH_FLAT || this->actor.params == EN_POH_SHARP) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            this->actor.textId = 0x5000;
        } else if (!Flags_GetSwitch(play, 0xA) && !Flags_GetSwitch(play, 0xB)) {
            this->actor.textId = 0x500F;
        } else if ((this->actor.params == EN_POH_FLAT && Flags_GetSwitch(play, 0xA)) ||
                   (this->actor.params == EN_POH_SHARP && Flags_GetSwitch(play, 0xB))) {
            this->actor.textId = 0x5013;
        } else {
            this->actor.textId = 0x5012;
        }
    } else {
        this->actor.textId = 0x5005;
    }
    this->unk_198 = 200;
    this->unk_195 = 32;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actionFunc = func_80ADFE80;
}

void func_80ADE950(EnPoh* this, s32 arg1) {
    if (arg1) {
        Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
    }
    this->actionFunc = func_80AE009C;
}

void func_80ADE998(EnPoh* this) {
    this->actionFunc = EnPoh_TalkRegular;
    this->actor.home.pos.y = this->actor.world.pos.y - 15.0f;
}

void func_80ADE9BC(EnPoh* this) {
    this->actionFunc = EnPoh_TalkComposer;
}

void EnPoh_MoveTowardsPlayerHeight(EnPoh* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_StepToF(&this->actor.world.pos.y, player->actor.world.pos.y, 1.0f);
    this->actor.world.pos.y += 2.5f * Math_SinS(this->unk_195 * 0x800);
    if (this->unk_195 != 0) {
        this->unk_195 -= 1;
    }
    if (this->unk_195 == 0) {
        this->unk_195 = 32;
    }
}

void func_80ADEA5C(EnPoh* this) {
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 400.0f) {
        this->unk_19C = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }
    Math_ScaledStepToS(&this->actor.world.rot.y, this->unk_19C, 0x71C);
}

void func_80ADEAC4(EnPoh* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_198 != 0) {
        this->unk_198--;
    }
    EnPoh_MoveTowardsPlayerHeight(this, play);
    if (this->actor.xzDistToPlayer < 200.0f) {
        func_80ADE1BC(this);
    } else if (this->unk_198 == 0) {
        EnPoh_SetupIdle(this);
    }
    if (this->lightColor.a == 255) {
        func_8002F974(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void EnPoh_Idle(EnPoh* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speedXZ, 1.0f, 0.2f);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_198 != 0) {
        this->unk_198--;
    }
    func_80ADEA5C(this);
    EnPoh_MoveTowardsPlayerHeight(this, play);
    if (this->actor.xzDistToPlayer < 200.0f && this->unk_198 < 19) {
        func_80ADE1BC(this);
    } else if (this->unk_198 == 0) {
        if (Rand_ZeroOne() < 0.1f) {
            func_80ADE514(this);
        } else {
            func_80ADE114(this);
        }
    }
    if (this->lightColor.a == 255) {
        func_8002F974(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void func_80ADEC9C(EnPoh* this, PlayState* play) {
    Player* player;
    s16 facingDiff;

    player = GET_PLAYER(play);
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    facingDiff = this->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    if (facingDiff >= 0x3001) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (facingDiff < -0x3000) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C);
    }
    EnPoh_MoveTowardsPlayerHeight(this, play);
    if (this->actor.xzDistToPlayer > 280.0f) {
        EnPoh_SetupIdle(this);
    } else if (this->unk_198 == 0 && this->actor.xzDistToPlayer < 140.0f &&
               !Player_IsFacingActor(&this->actor, 0x2AAA, play)) {
        EnPoh_SetupAttack(this);
    }
    if (this->lightColor.a == 255) {
        func_8002F974(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

void EnPoh_Attack(EnPoh* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_KANTERA);
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
    }
    EnPoh_MoveTowardsPlayerHeight(this, play);
    if (this->unk_198 >= 10) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xE38);
    } else if (this->unk_198 == 9) {
        this->actor.speedXZ = 5.0f;
        this->skelAnime.playSpeed = 2.0f;
    } else if (this->unk_198 == 0) {
        EnPoh_SetupIdle(this);
        this->unk_198 = 23;
    }
}

void func_80ADEECC(EnPoh* this, PlayState* play) {
    Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.colChkInfo.health != 0) {
            func_80ADE368(this);
        } else {
            func_80ADE48C(this);
        }
    }
}

void func_80ADEF38(EnPoh* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->lightColor.a = 255;
        this->visibilityTimer = Rand_S16Offset(700, 300);
        this->actor.flags |= ACTOR_FLAG_0;
        EnPoh_SetupIdle(this);
    } else if (this->skelAnime.curFrame > 10.0f) {
        this->lightColor.a = ((this->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    }
    if (this->skelAnime.playSpeed < 0.5f && this->actor.xzDistToPlayer < 280.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnPoh_ComposerAppear(EnPoh* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->lightColor.a = 255;
        this->visibilityTimer = Rand_S16Offset(700, 300);
        this->actor.flags |= ACTOR_FLAG_0;
        EnPoh_SetupIdle(this);
    } else {
        this->lightColor.a = CLAMP_MAX((s32)(this->skelAnime.curFrame * 25.5f), 255);
    }
}

void func_80ADF15C(EnPoh* this, PlayState* play) {
    Vec3f vec;
    f32 multiplier;
    f32 newScale;
    s32 pad;
    s32 pad1;

    this->unk_198++;
    if (this->unk_198 < 8) {
        if (this->unk_198 < 5) {
            vec.y = Math_SinS((this->unk_198 * 0x1000) - 0x4000) * 23.0f + (this->actor.world.pos.y + 40.0f);
            multiplier = Math_CosS((this->unk_198 * 0x1000) - 0x4000) * 23.0f;
            vec.x = Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier +
                    this->actor.world.pos.x;
            vec.z = Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier +
                    this->actor.world.pos.z;
        } else {
            vec.y = (this->actor.world.pos.y + 40.0f) + (15.0f * (this->unk_198 - 5));
            vec.x =
                Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.x;
            vec.z =
                Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.z;
        }
        EffectSsDeadDb_Spawn(play, &vec, &D_80AE1B60, &D_80AE1B6C, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        vec.x = (this->actor.world.pos.x + this->actor.world.pos.x) - vec.x;
        vec.z = (this->actor.world.pos.z + this->actor.world.pos.z) - vec.z;
        EffectSsDeadDb_Spawn(play, &vec, &D_80AE1B60, &D_80AE1B6C, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        vec.x = this->actor.world.pos.x;
        vec.z = this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &vec, &D_80AE1B60, &D_80AE1B6C, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, 1);
        if (this->unk_198 == 1) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_EXTINCT);
        }
    } else if (this->unk_198 == 28) {
        EnPoh_SetupDeath(this, play);
    } else if (this->unk_198 >= 19) {
        newScale = (28 - this->unk_198) * 0.001f;
        this->actor.world.pos.y += 5.0f;
        this->actor.scale.z = newScale;
        this->actor.scale.y = newScale;
        this->actor.scale.x = newScale;
    }
    if (this->unk_198 == 18) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

void func_80ADF574(EnPoh* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        EnPoh_SetupIdle(this);
        this->unk_198 = 23;
    } else {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
        this->actor.shape.rot.y += 0x1000;
    }
}

void func_80ADF5E0(EnPoh* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Math_ScaledStepToS(&this->actor.world.rot.y, this->unk_19C, 1820) != 0) {
        EnPoh_SetupIdle(this);
    }
    if (this->actor.xzDistToPlayer < 200.0f) {
        func_80ADE1BC(this);
    }
    EnPoh_MoveTowardsPlayerHeight(this, play);
}

void EnPoh_Disappear(EnPoh* this, PlayState* play) {
    if (this->unk_194 != 0) {
        this->unk_194--;
    }
    this->actor.world.rot.y += 0x1000;
    EnPoh_MoveTowardsPlayerHeight(this, play);
    this->lightColor.a = this->unk_194 * 7.96875f;
    if (this->unk_194 == 0) {
        this->visibilityTimer = Rand_S16Offset(100, 50);
        EnPoh_SetupIdle(this);
    }
}

void EnPoh_Appear(EnPoh* this, PlayState* play) {
    this->unk_194++;
    this->actor.world.rot.y -= 0x1000;
    EnPoh_MoveTowardsPlayerHeight(this, play);
    this->lightColor.a = this->unk_194 * 7.96875f;
    if (this->unk_194 == 32) {
        this->visibilityTimer = Rand_S16Offset(700, 300);
        this->unk_194 = 0;
        EnPoh_SetupIdle(this);
    }
}

void func_80ADF894(EnPoh* this, PlayState* play) {
    f32 multiplier;

    SkelAnime_Update(&this->skelAnime);
    multiplier = Math_SinS(this->unk_195 * 0x800) * 3.0f;
    this->actor.world.pos.x -= multiplier * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.z += multiplier * Math_SinS(this->actor.shape.rot.y);
    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0x71C);
    EnPoh_MoveTowardsPlayerHeight(this, play);
    if (this->unk_198 == 0 || this->actor.xzDistToPlayer > 250.0f) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        EnPoh_SetupIdle(this);
    }
    func_8002F974(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void EnPoh_Death(EnPoh* this, PlayState* play) {
    s32 objId;

    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    if (this->actor.bgCheckFlags & 1) {
        objId = (this->infoIdx == EN_POH_INFO_COMPOSER) ? OBJECT_PO_COMPOSER : OBJECT_POH;
        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 6.0f, 0, 1, 1, 15, objId, 10,
                                 this->info->lanternDisplayList);
        func_80ADE6D4(this);
    } else if (this->unk_198 == 0) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 10.0f, 4);
}

void func_80ADFA90(EnPoh* this, s32 arg1) {
    f32 multiplier;

    this->lightColor.a = CLAMP(this->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = this->lightColor.a * (1.0f / 255);
        this->actor.scale.x = this->actor.scale.z = 0.0056000002f * multiplier + 0.0014000001f;
        this->actor.scale.y = (0.007f - 0.007f * multiplier) + 0.007f;
    } else {
        multiplier = 1.0f;
        this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = this->lightColor.a * (0.007f / 0xFF);
        this->actor.world.pos.y = this->actor.home.pos.y + (1.0f / 17.0f) * this->lightColor.a;
    }
    this->lightColor.r = this->info->lightColor.r * multiplier;
    this->lightColor.g = this->info->lightColor.g * multiplier;
    this->lightColor.b = this->info->lightColor.b * multiplier;
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->info->lightColor.r, this->info->lightColor.g,
                              this->info->lightColor.b, this->lightColor.a * (200.0f / 255));
}

void func_80ADFE28(EnPoh* this, PlayState* play) {
    this->actor.home.pos.y += 2.0f;
    func_80ADFA90(this, 20);
    if (this->lightColor.a == 255) {
        EnPoh_Talk(this, play);
    }
}

void func_80ADFE80(EnPoh* this, PlayState* play) {
    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->actor.params >= EN_POH_SHARP) {
            func_80ADE9BC(this);
        } else {
            func_80ADE998(this);
        }
        return;
    }
    if (this->unk_198 == 0) {
        func_80ADE950(this, 1);
        this->actor.flags &= ~ACTOR_FLAG_16;
        return;
    }
    if (this->colliderCyl.base.ocFlags1 & OC1_HIT) {
        this->actor.flags |= ACTOR_FLAG_16;
        func_8002F2F4(&this->actor, play);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_16;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCyl.base);
    }
    this->actor.world.pos.y = Math_SinS(this->unk_195 * 0x800) * 5.0f + this->actor.home.pos.y;
    if (this->unk_195 != 0) {
        this->unk_195 -= 1;
    }
    if (this->unk_195 == 0) {
        this->unk_195 = 32;
    }
    this->colliderCyl.dim.pos.y = this->actor.world.pos.y - 20.0f;
    Actor_SetFocus(&this->actor, -10.0f);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->info->lightColor.r, this->info->lightColor.g,
                              this->info->lightColor.b, this->lightColor.a * (200.0f / 255));
}

void func_80AE009C(EnPoh* this, PlayState* play) {
    func_80ADFA90(this, -13);
    if (this->lightColor.a == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnPoh_TalkRegular(EnPoh* this, PlayState* play) {
    if (this->actor.textId != 0x5005) {
        func_80ADFA90(this, -13);
    } else {
        func_8002F974(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            Audio_StopSfxByPosAndId(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (play->msgCtx.choiceIndex == 0) {
                if (Inventory_HasEmptyBottle()) {
                    this->actor.textId = 0x5008;
                    Item_Give(play, ITEM_POE);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_BIG_GET);
                } else {
                    this->actor.textId = 0x5006;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
                }
            } else {
                this->actor.textId = 0x5007;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH);
            }
            Message_ContinueTextbox(play, this->actor.textId);
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        func_80ADE950(this, 0);
    }
}

void EnPoh_TalkComposer(EnPoh* this, PlayState* play) {
    func_8002F974(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (Message_ShouldAdvance(play)) {
            if (play->msgCtx.choiceIndex == 0) {
                if (!Flags_GetSwitch(play, 0xB) && !Flags_GetSwitch(play, 0xA)) {
                    this->actor.textId = 0x5010;
                } else {
                    this->actor.textId = 0x5014;
                }
                Message_ContinueTextbox(play, this->actor.textId);
            } else {
                if (this->actor.params == EN_POH_SHARP) {
                    Flags_SetSwitch(play, 0xB);
                } else {
                    Flags_SetSwitch(play, 0xA);
                }
                func_80ADE950(this, 1);
            }
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        if (this->actor.textId == 0x5000) {
            Flags_SetSwitch(play, 9);
        }
        func_80ADE950(this, 1);
    }
}

void func_80AE032C(EnPoh* this, PlayState* play) {
    if (this->colliderCyl.base.acFlags & AC_HIT) {
        this->colliderCyl.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD);
            } else {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DAMAGE);
            }
            func_80ADE28C(this);
        }
    }
}

void EnPoh_UpdateVisibility(EnPoh* this) {
    if (this->actionFunc != EnPoh_Appear && this->actionFunc != EnPoh_Disappear && this->actionFunc != func_80ADEF38 &&
        this->actionFunc != EnPoh_ComposerAppear) {
        if (this->visibilityTimer != 0) {
            this->visibilityTimer--;
        }
        if (this->lightColor.a == 255) {
            if (this->actor.isTargeted) {
                this->unk_194++;
                this->unk_194 = CLAMP_MAX(this->unk_194, 20);
            } else {
                this->unk_194 = 0;
            }
            if ((this->unk_194 == 20 || this->visibilityTimer == 0) &&
                (this->actionFunc == func_80ADEAC4 || this->actionFunc == EnPoh_Idle ||
                 this->actionFunc == func_80ADEC9C || this->actionFunc == func_80ADF894 ||
                 this->actionFunc == func_80ADF5E0)) {
                EnPoh_SetupDisappear(this);
            }
        } else if (this->lightColor.a == 0 && this->visibilityTimer == 0 &&
                   (this->actionFunc == func_80ADEAC4 || this->actionFunc == EnPoh_Idle ||
                    this->actionFunc == func_80ADEC9C || this->actionFunc == func_80ADF5E0)) {
            EnPoh_SetupAppear(this);
        }
    }
}

void EnPoh_Update(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectIdx)) {
        this->actor.objBankIndex = this->objectIdx;
        this->actor.update = EnPoh_UpdateLiving;
        Actor_SetObjectDependency(play, &this->actor);
        if (this->infoIdx == EN_POH_INFO_NORMAL) {
            SkelAnime_Init(play, &this->skelAnime, &gPoeSkel, &gPoeFloatAnim, this->jointTable, this->morphTable,
                           21);
            this->actor.draw = EnPoh_DrawRegular;
        } else {
            SkelAnime_InitFlex(play, &this->skelAnime, &gPoeComposerSkel, &gPoeComposerFloatAnim, this->jointTable,
                               this->morphTable, 12);
            this->actor.draw = EnPoh_DrawComposer;
            this->colliderSph.elements[0].dim.limb = 9;
            this->colliderSph.elements[0].dim.modelSphere.center.y *= -1;
            this->actor.shape.rot.y = this->actor.world.rot.y = -0x4000;
            this->colliderCyl.dim.radius = 20;
            this->colliderCyl.dim.height = 55;
            this->colliderCyl.dim.yShift = 15;
        }
        this->actor.flags &= ~ACTOR_FLAG_4;
        EnPoh_SetupInitialAction(this);
    }
}

void func_80AE067C(EnPoh* this) {
    s16 temp_var;

    if (this->actionFunc == EnPoh_Attack) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 255);
        this->lightColor.g = CLAMP_MIN((s16)(this->lightColor.g - 5), 50);
        temp_var = this->lightColor.b - 5;
        this->lightColor.b = CLAMP_MIN(temp_var, 0);
    } else if (this->actionFunc == func_80ADF894) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 80);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        temp_var = this->lightColor.b + 5;
        this->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (this->actionFunc == func_80ADEECC) {
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
        if (this->lightColor.b >= 211) {
            temp_var = this->lightColor.b - 5;
            this->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->lightColor.b + 5;
            this->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void func_80AE089C(EnPoh* this) {
    f32 rand;

    if ((this->actionFunc == func_80ADEF38 || this->actionFunc == EnPoh_ComposerAppear) &&
        this->skelAnime.curFrame < 12.0f) {
        this->envColor.r = this->envColor.g = this->envColor.b = (s16)(this->skelAnime.curFrame * 16.66f) + 55;
        this->envColor.a = this->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = Rand_ZeroOne();
        this->envColor.r = (s16)(rand * 30.0f) + 225;
        this->envColor.g = (s16)(rand * 100.0f) + 155;
        this->envColor.b = (s16)(rand * 160.0f) + 95;
        this->envColor.a = 200;
    }
}

void EnPoh_UpdateLiving(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;
    s32 pad;
    Vec3f vec;
    s32 sp38;

    if (this->colliderSph.base.atFlags & AT_HIT) {
        this->colliderSph.base.atFlags &= ~AT_HIT;
        func_80ADE4C8(this);
    }
    func_80AE032C(this, play);
    EnPoh_UpdateVisibility(this);
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    if (this->actionFunc == EnPoh_Attack && this->unk_198 < 10) {
        this->actor.flags |= ACTOR_FLAG_24;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSph.base);
    }
    Collider_UpdateCylinder(&this->actor, &this->colliderCyl);
    if ((this->colliderCyl.base.acFlags & AC_ON) && this->lightColor.a == 255) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCyl.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCyl.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSph.base);
    Actor_SetFocus(&this->actor, 42.0f);
    if (this->actionFunc != func_80ADEECC && this->actionFunc != func_80ADF574) {
        if (this->actionFunc == func_80ADF894) {
            this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
        } else {
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }
    vec.x = this->actor.world.pos.x;
    vec.y = this->actor.world.pos.y + 20.0f;
    vec.z = this->actor.world.pos.z;
    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &sp38, &this->actor, &vec);
    func_80AE089C(this);
    this->actor.shape.shadowAlpha = this->lightColor.a;
}

s32 EnPoh_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfxP) {
    EnPoh* this = (EnPoh*)thisx;

    if ((this->lightColor.a == 0 || limbIndex == this->info->unk_6) ||
        (this->actionFunc == func_80ADF15C && this->unk_198 >= 2)) {
        *dList = NULL;
    } else if (this->actor.params == EN_POH_FLAT && limbIndex == 0xA) {
        // Replace Sharp's head with Flat's
        *dList = gPoeComposerFlatHeadDL;
    }
    if (limbIndex == 0x13 && this->infoIdx == EN_POH_INFO_NORMAL) {
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
    }
    return false;
}

void EnPoh_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoh* this = (EnPoh*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->colliderSph);
    if (this->actionFunc == func_80ADF15C && this->unk_198 >= 2 && limbIndex == this->info->unk_7) {
        gSPMatrix((*gfxP)++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, this->info->burnDisplayList);
    }
    if (limbIndex == this->info->unk_6) {
        if (this->actionFunc == func_80ADF15C && this->unk_198 >= 19 && 0.0f != this->actor.scale.x) {
            f32 mtxScale = 0.01f / this->actor.scale.x;
            Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_Get(&this->unk_368);
        if (this->actionFunc == func_80ADF15C && this->unk_198 == 27) {
            this->actor.world.pos.x = this->unk_368.xw;
            this->actor.world.pos.y = this->unk_368.yw;
            this->actor.world.pos.z = this->unk_368.zw;
        }
        Lights_PointGlowSetInfo(&this->lightInfo, this->colliderSph.elements[0].dim.worldSphere.center.x,
                                this->colliderSph.elements[0].dim.worldSphere.center.y,
                                this->colliderSph.elements[0].dim.worldSphere.center.z, this->envColor.r,
                                this->envColor.g, this->envColor.b, this->envColor.a * (200.0f / 255));
    }
}

void EnPoh_DrawRegular(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80AE067C(this);
    func_80093D18(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);
    if (this->lightColor.a == 255 || this->lightColor.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, D_80116280 + 2);
        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->lightColor.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, D_80116280);
        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
    Matrix_Put(&this->unk_368);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnPoh_DrawComposer(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;
    Color_RGBA8* sp90;
    Color_RGBA8* phi_t0;

    OPEN_DISPS(play->state.gfxCtx);
    func_80AE067C(this);
    if (this->actor.params == EN_POH_SHARP) {
        sp90 = &D_80AE1B4C;
        phi_t0 = &D_80AE1B54;
    } else {
        sp90 = &D_80AE1B50;
        phi_t0 = &D_80AE1B58;
    }
    if (this->lightColor.a == 255 || this->lightColor.a == 0) {
        func_80093D18(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   Gfx_EnvColor(play->state.gfxCtx, sp90->r, sp90->g, sp90->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   Gfx_EnvColor(play->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0C, D_80116280 + 2);
        POLY_OPA_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw,
                                           &this->actor, POLY_OPA_DISP);
    } else {
        func_80093D18(play->state.gfxCtx);
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_EnvColor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   Gfx_EnvColor(play->state.gfxCtx, sp90->r, sp90->g, sp90->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0B,
                   Gfx_EnvColor(play->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, EnPoh_OverrideLimbDraw, EnPoh_PostLimbDraw,
                                           &this->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
    Matrix_Put(&this->unk_368);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp90->r, sp90->g, sp90->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnPoh_UpdateDead(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc != EnPoh_Death) {
        this->visibilityTimer++;
    }
    func_80AE089C(this);
}

void EnPoh_DrawSoul(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc == EnPoh_Death) {
        func_80093D18(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
        Lights_PointGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, this->envColor.r, this->envColor.g, this->envColor.b, 200);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
        if (this->infoIdx == EN_POH_INFO_COMPOSER) {
            Color_RGBA8* envColor = (this->actor.params == EN_POH_SHARP) ? &D_80AE1B4C : &D_80AE1B50;
            s32 pad;

            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, envColor->r, envColor->g, envColor->b, 255);
            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
        }
    } else {
        func_80093D84(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                    (this->visibilityTimer * this->info->unk_8) % 512U, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, this->info->primColor.r, this->info->primColor.g,
                        this->info->primColor.b, this->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 255);
        Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, this->info->soulDisplayList);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
