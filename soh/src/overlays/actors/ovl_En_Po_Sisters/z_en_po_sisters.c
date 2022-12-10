/*
 * File: z_en_po_sisters.c
 * Overlay: ovl_En_Po_Sisters
 * Description: Forest Temple Four Poe Sisters
 */

#include "z_en_po_sisters.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_po_sisters/object_po_sisters.h"
#include "soh/frame_interpolation.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_9 | ACTOR_FLAG_12 | ACTOR_FLAG_14)

void EnPoSisters_Init(Actor* thisx, PlayState* play);
void EnPoSisters_Destroy(Actor* thisx, PlayState* play);
void EnPoSisters_Update(Actor* thisx, PlayState* play);
void EnPoSisters_Draw(Actor* thisx, PlayState* play);
void EnPoSisters_Reset(void);

void func_80ADA094(EnPoSisters* this, PlayState* play);
void func_80ADA4A8(EnPoSisters* this, PlayState* play);
void func_80ADA530(EnPoSisters* this, PlayState* play);
void func_80ADA6A0(EnPoSisters* this, PlayState* play);
void func_80ADA7F0(EnPoSisters* this, PlayState* play);
void func_80ADA8C0(EnPoSisters* this, PlayState* play);
void func_80ADA9E8(EnPoSisters* this, PlayState* play);
void func_80ADAAA4(EnPoSisters* this, PlayState* play);
void func_80ADAC70(EnPoSisters* this, PlayState* play);
void func_80ADAD54(EnPoSisters* this, PlayState* play);
void func_80ADAE6C(EnPoSisters* this, PlayState* play);
void func_80ADAFC0(EnPoSisters* this, PlayState* play);
void func_80ADB17C(EnPoSisters* this, PlayState* play);
void func_80ADB2B8(EnPoSisters* this, PlayState* play);
void func_80ADB338(EnPoSisters* this, PlayState* play);
void func_80ADB9F0(EnPoSisters* this, PlayState* play);
void func_80ADB4B0(EnPoSisters* this, PlayState* play);
void func_80ADB51C(EnPoSisters* this, PlayState* play);
void func_80ADB770(EnPoSisters* this, PlayState* play);
void func_80ADBB6C(EnPoSisters* this, PlayState* play);
void func_80ADBBF4(EnPoSisters* this, PlayState* play);
void func_80ADBC88(EnPoSisters* this, PlayState* play);
void func_80ADBD38(EnPoSisters* this, PlayState* play);
void func_80ADBD8C(EnPoSisters* this, PlayState* play);
void func_80ADBEE8(EnPoSisters* this, PlayState* play);
void func_80ADBF58(EnPoSisters* this, PlayState* play);

void func_80AD9AA8(EnPoSisters* this, PlayState* play);
void func_80AD9C24(EnPoSisters* this, PlayState* play);

void func_80AD9D44(EnPoSisters* this);

static Color_RGBA8 D_80ADD6F0[4] = {
    { 255, 170, 255, 255 },
    { 255, 200, 0, 255 },
    { 0, 170, 255, 255 },
    { 170, 255, 0, 255 },
};

static Color_RGBA8 D_80ADD700[4] = {
    { 100, 0, 255, 255 },
    { 255, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 0, 150, 0, 255 },
};

const ActorInit En_Po_Sisters_InitVars = {
    ACTOR_EN_PO_SISTERS,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_PO_SISTERS,
    sizeof(EnPoSisters),
    (ActorFunc)EnPoSisters_Init,
    (ActorFunc)EnPoSisters_Destroy,
    (ActorFunc)EnPoSisters_Update,
    (ActorFunc)EnPoSisters_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x4FC7FFEA, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 18, 60, 15, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 25, 60, 40 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
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
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 D_80ADD784 = 0;

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 6000, ICHAIN_STOP),
};

static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };

static s16 D_80ADD79C[4] = { 0xB000, 0xD000, 0x5000, 0x3000 };

static Vec3s D_80ADD7A4[4] = {
    { -22, 337, -1704 },
    { -431, 879, -3410 },
    { 549, 879, -3410 },
    { 1717, 515, -1340 },
};

static Vec3f D_80ADD7BC = { 120.0f, 250.0f, -1420.0f };

static Gfx* D_80ADD7C8[4] = {
    gPoeSistersMegBodyDL,
    gPoeSistersJoelleBodyDL,
    gPoeSistersBethBodyDL,
    gPoeSistersAmyBodyDL,
};

static Gfx* D_80ADD7D8[4] = {
    gPoeSistersMegFaceDL,
    gPoeSistersJoelleFaceDL,
    gPoeSistersBethFaceDL,
    gPoSistersAmyFaceDL,
};

static Color_RGBA8 D_80ADD7E8[4] = {
    { 80, 0, 100, 0 },
    { 80, 15, 0, 0 },
    { 0, 70, 50, 0 },
    { 70, 70, 0, 0 },
};

static Vec3f D_80ADD7F8 = { 1000.0f, -1700.0f, 0.0f };

void EnPoSisters_Init(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 pad;

    this->epoch++;

    // Skip Poe Intro Cutscene
    if (gSaveContext.n64ddFlag && thisx->params == 4124 && !Randomizer_GetSettingValue(RSK_ENABLE_GLITCH_CUTSCENES)) {
        Flags_SetSwitch(play, 0x1B);
        Actor_Kill(thisx);
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    SkelAnime_Init(play, &this->skelAnime, &gPoeSistersSkel, &gPoeSistersSwayAnim, this->jointTable,
                   this->morphTable, 12);
    this->unk_22E.r = 255;
    this->unk_22E.g = 255;
    this->unk_22E.b = 210;
    this->unk_22E.a = 255;
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, 0,
                            0, 0, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->unk_194 = (thisx->params >> 8) & 3;
    this->actor.naviEnemyId = this->unk_194 + 0x50;
    this->unk_195 = (thisx->params >> 0xA) & 3;
    this->unk_196 = 32;
    this->unk_197 = 20;
    this->unk_198 = 1;
    this->unk_199 = 32;
    this->unk_294 = 110.0f;
    this->actor.flags &= ~ACTOR_FLAG_0;
    if (this->actor.params & 0x1000) {
        func_80ADA094(this, play);
    } else if (this->unk_194 == 0) {
        if (this->unk_195 == 0) {
            this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
            func_80AD9AA8(this, play);
        } else {
            this->actor.flags &= ~(ACTOR_FLAG_9 | ACTOR_FLAG_14);
            this->collider.info.elemType = ELEMTYPE_UNK4;
            this->collider.info.bumper.dmgFlags |= 1;
            this->collider.base.ocFlags1 = OC1_NONE;
            func_80AD9C24(this, NULL);
        }
    } else {
        func_80AD9D44(this);
    }
    this->actor.params &= 0x3F;
}

void EnPoSisters_Destroy(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    if (this->unk_194 == 0 && this->unk_195 == 0) {
        func_800F5B58();
    }
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AD9240(EnPoSisters* this, s32 arg1, Vec3f* arg2) {
    f32 temp_f20 = SQ(arg1) * 0.1f;
    Vec3f* vec;
    s32 i;

    for (i = 0; i < this->unk_198; i++) {
        vec = &this->unk_234[i];
        vec->x = arg2->x + Math_SinS((s16)(this->actor.shape.rot.y + (this->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->z = arg2->z + Math_CosS((s16)(this->actor.shape.rot.y + (this->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->y = arg2->y + arg1;
    }
}

void func_80AD9368(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->unk_19A = Rand_S16Offset(2, 3);
    this->actionFunc = func_80ADA4A8;
    this->actor.speedXZ = 0.0f;
}

void func_80AD93C4(EnPoSisters* this) {
    if (this->actionFunc != func_80ADA6A0) {
        Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    }
    this->unk_19A = Rand_S16Offset(0xF, 3);
    this->unk_199 |= 7;
    this->actionFunc = func_80ADA530;
}

void func_80AD943C(EnPoSisters* this) {
    this->actionFunc = func_80ADA6A0;
}

void func_80AD944C(EnPoSisters* this) {
    if (this->unk_22E.a != 0) {
        this->collider.base.colType = COLTYPE_METAL;
        this->collider.base.acFlags |= AC_HARD;
    }
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    this->actor.speedXZ = 0.0f;
    this->unk_19A = Animation_GetLastFrame(&gPoeSistersAttackAnim) * 3 + 3;
    this->unk_199 &= ~2;
    this->actionFunc = func_80ADA7F0;
}

void func_80AD94E0(EnPoSisters* this) {
    this->actor.speedXZ = 5.0f;
    if (this->unk_194 == 0) {
        this->collider.base.colType = COLTYPE_METAL;
        this->collider.base.acFlags |= AC_HARD;
        Animation_MorphToLoop(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    }
    this->unk_19A = 5;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->unk_199 |= 8;
    this->actionFunc = func_80ADA8C0;
}

void func_80AD9568(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    if (this->unk_194 != 0) {
        this->collider.base.colType = COLTYPE_HIT3;
        this->collider.base.acFlags &= ~AC_HARD;
    }
    this->actionFunc = func_80ADA9E8;
}

void func_80AD95D8(EnPoSisters* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gPoeSistersDamagedAnim, -3.0f);
    if (this->collider.base.ac != NULL) {
        this->actor.world.rot.y = (this->collider.info.acHitInfo->toucher.dmgFlags & 0x1F824)
                                      ? this->collider.base.ac->world.rot.y
                                      : Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac) + 0x8000;
    }
    if (this->unk_194 != 0) {
        this->actor.speedXZ = 10.0f;
    }
    this->unk_199 &= ~0xB;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0x10);
    this->actionFunc = func_80ADAAA4;
}

void func_80AD96A4(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFleeAnim, -3.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->unk_19A = 5;
    this->unk_199 |= 0xB;
    this->actor.speedXZ = 5.0f;
    this->actionFunc = func_80ADAC70;
}

void func_80AD9718(EnPoSisters* this) {
    Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    this->actor.speedXZ = 0.0f;
    this->unk_19C = 100;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_199 &= ~5;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
    this->actionFunc = func_80ADAD54;
}

void func_80AD97C8(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 sp20;

    if (this->unk_195 == 0 || this->actionFunc != func_80ADAAA4) {
        if ((player->swordState == 0 || player->meleeWeaponAnimation >= 24) &&
            player->actor.world.pos.y - player->actor.floorHeight < 1.0f) {
            Math_StepToF(&this->unk_294, 110.0f, 3.0f);
        } else {
            Math_StepToF(&this->unk_294, 170.0f, 10.0f);
        }
        sp20 = this->unk_294;
    } else if (this->unk_195 != 0) {
        sp20 = this->actor.parent->xzDistToPlayer;
    }
    this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.z;
}

void func_80AD98F4(EnPoSisters* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    if (this->unk_194 == 0) {
        this->unk_294 = 110.0f;
        func_80AD97C8(this, play);
        this->unk_22E.a = 0;
        this->actor.draw = EnPoSisters_Draw;
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    this->unk_19A = 15;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
    this->unk_199 &= ~1;
    this->actionFunc = func_80ADAE6C;
}

void func_80AD99D4(EnPoSisters* this, PlayState* play) {
    this->unk_19A = 0;
    this->actor.speedXZ = 0.0f;
    this->actor.world.pos.y += 42.0f;
    this->actor.shape.yOffset = -6000.0f;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->unk_199 = 0;
    this->actionFunc = func_80ADAFC0;
    OnePointCutscene_Init(play, 3190, 999, &this->actor, MAIN_CAM);
}

void func_80AD9A54(EnPoSisters* this, PlayState* play) {
    this->unk_19A = 0;
    this->actor.world.pos.y = this->unk_234[0].y;
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x80);
    this->actionFunc = func_80ADB17C;
}

// Meg spawning fakes
void func_80AD9AA8(EnPoSisters* this, PlayState* play) {
    Actor* actor1 = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x400, true);
    Actor* actor2 = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x800, true);
    Actor* actor3 = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0xC00, true);
    s32 pad;
    s32 pad1;

    if (actor1 == NULL || actor2 == NULL || actor3 == NULL) {
        if (actor1 != NULL) {
            Actor_Kill(actor1);
        }
        if (actor2 != NULL) {
            Actor_Kill(actor2);
        }
        if (actor3 != NULL) {
            Actor_Kill(actor3);
        }
        Actor_Kill(&this->actor);
    } else {
        actor3->parent = &this->actor;
        actor2->parent = &this->actor;
        actor1->parent = &this->actor;
        Animation_PlayLoop(&this->skelAnime, &gPoeSistersMegCryAnim);
        this->unk_198 = 0;
        this->unk_199 = 160;
        this->actionFunc = func_80ADB2B8;
    }
}

void func_80AD9C24(EnPoSisters* this, PlayState* play) {
    Vec3f vec;

    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->unk_19C = 100;
    this->unk_199 = 32;
    this->collider.base.colType = COLTYPE_HIT3;
    this->collider.base.acFlags &= ~AC_HARD;
    if (play != NULL) {
        vec.x = this->actor.world.pos.x;
        vec.y = this->actor.world.pos.y + 45.0f;
        vec.z = this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &vec, &sZeroVector, &sZeroVector, 150, 0, 255, 255, 255, 155, 150, 150, 150, 1,
                             9, 0);
    }
    Lights_PointSetColorAndRadius(&this->lightInfo, 0, 0, 0, 0);
    this->actionFunc = func_80ADB338;
}

void func_80AD9D44(EnPoSisters* this) {
    if (this->unk_194 == 3) {
        Animation_PlayOnce(&this->skelAnime, &gPoeSistersAppearDisappearAnim);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
    } else {
        Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    }
    this->unk_22E.a = 0;
    this->unk_199 = 32;
    this->actionFunc = func_80ADB9F0;
}

void func_80AD9DF0(EnPoSisters* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gPoeSistersAppearDisappearAnim, -5.0f);
    this->unk_198 = 1;
    this->unk_199 &= ~0x80;
    this->actionFunc = func_80ADB4B0;
    OnePointCutscene_Init(play, 3180, 156, &this->actor, MAIN_CAM);
}

void func_80AD9E60(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_19A = Animation_GetLastFrame(&gPoeSistersFloatAnim) * 7 + 7;
    if (this->actor.parent != NULL) {
        this->actor.world.pos = this->actor.parent->world.pos;
        this->actor.shape.rot.y = this->actor.parent->shape.rot.y;
    } else {
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->unk_19A++;
    }
    if (this->unk_195 == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
    }
    this->actionFunc = func_80ADB51C;
}

void func_80AD9F1C(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_22E.a = 255;
    this->unk_19A = 300;
    this->unk_19C = 3;
    this->unk_199 |= 9;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actionFunc = func_80ADB770;
}

void func_80AD9F90(EnPoSisters* this) {
    if (this->unk_194 == 1) {
        this->actor.home.pos.x = -632.0f;
        this->actor.home.pos.z = -3440.0f;
    } else {
        this->actor.home.pos.x = 752.0f;
        this->actor.home.pos.z = -3440.0f;
    }
    Animation_PlayLoop(&this->skelAnime, &gPoeSistersFloatAnim);
    this->unk_199 |= 0xA;
    this->actionFunc = func_80ADBB6C;
    this->actor.speedXZ = 5.0f;
}

void func_80ADA028(EnPoSisters* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->unk_22E.a = 255;
    this->unk_199 |= 0x15;
    this->actor.flags |= ACTOR_FLAG_0;
    this->actionFunc = func_80ADBBF4;
    this->actor.speedXZ = 0.0f;
}

void func_80ADA094(EnPoSisters* this, PlayState* play) {
    D_80ADD784 = 0;
    this->unk_22E.a = 0;
    this->unk_199 = 128;
    this->unk_19A = 50;
    this->unk_234[0] = this->actor.home.pos;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    this->actionFunc = func_80ADBC88;
}

void func_80ADA10C(EnPoSisters* this) {
    s32 i;

    this->unk_198 = ARRAY_COUNT(this->unk_234);
    for (i = 0; i < ARRAY_COUNT(this->unk_234); i++) {
        this->unk_234[i] = this->unk_234[0];
    }
    this->actionFunc = func_80ADBD38;
}

void func_80ADA1B8(EnPoSisters* this) {
    Animation_Change(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 0.833f, 0.0f,
                     Animation_GetLastFrame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    if (this->unk_194 == 0 || this->unk_194 == 1) {
        this->unk_19A = 40;
    } else {
        this->unk_19A = 76;
    }
    this->unk_198 = 0;
    D_80ADD784 = 0;
    this->actionFunc = func_80ADBD8C;
}

void func_80ADA25C(EnPoSisters* this) {
    Animation_PlayLoop(&this->skelAnime, &gPoeSistersSwayAnim);
    this->unk_198 = 8;
    this->unk_19A = 32;
    func_80AD9240(this, this->unk_19A, &this->actor.home.pos);
    this->actionFunc = func_80ADBEE8;
}

void func_80ADA2BC(EnPoSisters* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_198 = 0;
    this->unk_199 = 40;
    this->unk_19A = 90;
    this->unk_196 = 32;
    this->actor.world.rot.y = D_80ADD79C[this->unk_194];
    this->actor.home.pos.y = this->actor.world.pos.y;
    if (this->unk_194 == 0) {
        Flags_SetSwitch(play, 0x1B);
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_FLAME_IGNITION);
    this->actionFunc = func_80ADBF58;
}

void func_80ADA35C(EnPoSisters* this, PlayState* play) {
    f32 targetY;
    Player* player = GET_PLAYER(play);

    if (this->actionFunc == func_80ADBF58) {
        targetY = this->actor.home.pos.y;
    } else if (this->unk_194 == 0 || this->unk_194 == 3) {
        targetY = player->actor.world.pos.y + 5.0f;
    } else {
        targetY = 832.0f;
    }
    Math_ApproachF(&this->actor.world.pos.y, targetY, 0.5f, 3.0f);
    if (!this->unk_196) {
        this->unk_196 = 32;
    }
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    this->actor.world.pos.y += (2.0f + 0.5f * Rand_ZeroOne()) * Math_SinS(this->unk_196 * 0x800);
    if (this->unk_22E.a == 255 && this->actionFunc != func_80ADA8C0 && this->actionFunc != func_80ADA7F0) {
        if (this->actionFunc == func_80ADAC70) {
            func_8002F974(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
        } else {
            func_8002F974(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
        }
    }
}

void func_80ADA4A8(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->unk_19A == 0 || this->actor.xzDistToPlayer < 200.0f) {
        func_80AD93C4(this);
    }
}

void func_80ADA530(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->actor.speedXZ, 1.0f, 0.2f);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->actor.xzDistToPlayer < 200.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD943C(this);
    } else if (this->unk_19A == 0 && Math_StepToF(&this->actor.speedXZ, 0.0f, 0.2f) != 0) {
        func_80AD9368(this);
    }
    if (this->actor.bgCheckFlags & 8) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x71C);
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 300.0f) {
        Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           0x71C);
    }
}

void func_80ADA6A0(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp_v0;

    SkelAnime_Update(&this->skelAnime);
    temp_v0 = this->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    Math_StepToF(&this->actor.speedXZ, 2.0f, 0.2f);
    if (temp_v0 > 0x3000) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (temp_v0 < -0x3000) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C);
    }
    if (this->actor.xzDistToPlayer < 160.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD944C(this);
    } else if (this->actor.xzDistToPlayer > 240.0f) {
        func_80AD93C4(this);
    }
}

void func_80ADA7F0(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    this->actor.shape.rot.y += 384.0f * ((this->skelAnime.endFrame + 1.0f) * 3.0f - this->unk_19A);
    if (this->unk_19A == 18 || this->unk_19A == 7) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_ROLL);
    }
    if (this->unk_19A == 0) {
        func_80AD94E0(this);
    }
}

void func_80ADA8C0(EnPoSisters* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    this->actor.shape.rot.y += (384.0f * this->skelAnime.endFrame) * 3.0f;
    if (this->unk_19A == 0 && ABS((s16)(this->actor.shape.rot.y - this->actor.world.rot.y)) < 0x1000) {
        if (this->unk_194 != 0) {
            this->collider.base.colType = COLTYPE_HIT3;
            this->collider.base.acFlags &= ~AC_HARD;
            func_80AD93C4(this);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(this, play);
        }
    }
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_ROLL);
    }
}

void func_80ADA9E8(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y -= (this->actor.speedXZ * 10.0f) * 128.0f;
    if (Math_StepToF(&this->actor.speedXZ, 0.0f, 0.1f) != 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->unk_194 != 0) {
            func_80AD93C4(this);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(this, play);
        }
    }
}

void func_80ADAAA4(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && !(this->actor.flags & ACTOR_FLAG_15)) {
        if (this->actor.colChkInfo.health != 0) {
            if (this->unk_194 != 0) {
                func_80AD96A4(this);
            } else if (this->unk_195 != 0) {
                func_80AD9C24(this, NULL);
            } else {
                func_80AD9C24(this, play);
            }
        } else {
            func_80AD99D4(this, play);
        }
    }
    if (this->unk_195 != 0) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.parent->shape.rot.y,
                           (this->unk_195 == 2) ? 0x800 : 0x400);
        this->unk_22E.a = ((this->skelAnime.endFrame - this->skelAnime.curFrame) * 255.0f) / this->skelAnime.endFrame;
        this->actor.world.pos.y = this->actor.parent->world.pos.y;
        func_80AD97C8(this, play);
    } else if (this->unk_194 != 0) {
        Math_StepToF(&this->actor.speedXZ, 0.0f, 0.5f);
    }
}

void func_80ADAC70(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1820);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->actor.bgCheckFlags & 8) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->unk_199 |= 2;
        func_80AD9718(this);
    } else if (this->unk_19A == 0 && 240.0f < this->actor.xzDistToPlayer) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_80AD93C4(this);
    }
}

void func_80ADAD54(EnPoSisters* this, PlayState* play) {
    s32 endFrame;

    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_22E.a = 0;
        this->collider.info.bumper.dmgFlags = 0x00060001;
        func_80AD93C4(this);
    } else {
        endFrame = this->skelAnime.endFrame;
        this->unk_22E.a = ((endFrame - this->skelAnime.curFrame) * 255.0f) / endFrame;
    }
}

void func_80ADAE6C(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_22E.a = 255;
        if (this->unk_194 != 0) {
            this->unk_199 |= 1;
            this->collider.info.bumper.dmgFlags = 0x4FC7FFEA;
            if (this->unk_19A != 0) {
                this->unk_19A--;
            }
            if (this->unk_19A == 0) {
                this->unk_197 = 20;
                func_80AD93C4(this);
            }
        } else {
            func_80AD9F1C(this);
        }
    } else {
        this->unk_22E.a = (this->skelAnime.curFrame * 255.0f) / this->skelAnime.endFrame;
        if (this->unk_194 == 0) {
            func_80AD97C8(this, play);
        }
    }
}

void func_80ADAFC0(EnPoSisters* this, PlayState* play) {
    s32 i;

    this->unk_19A++;
    this->unk_198 = CLAMP_MAX(this->unk_198 + 1, 8);
    for (i = this->unk_198 - 1; i > 0; i--) {
        this->unk_234[i] = this->unk_234[i - 1];
    }
    this->unk_234[0].x =
        (Math_SinS((this->actor.shape.rot.y + this->unk_19A * 0x3000) - 0x4000) * (3000.0f * this->actor.scale.x)) +
        this->actor.world.pos.x;
    this->unk_234[0].z =
        (Math_CosS((this->actor.shape.rot.y + this->unk_19A * 0x3000) - 0x4000) * (3000.0f * this->actor.scale.x)) +
        this->actor.world.pos.z;
    if (this->unk_19A < 8) {
        this->unk_234[0].y = this->unk_234[1].y - 9.0f;
    } else {
        this->unk_234[0].y = this->unk_234[1].y + 2.0f;
        if (this->unk_19A >= 16) {
            if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.001f) != 0) {
                func_80AD9A54(this, play);
            }
            this->actor.scale.z = this->actor.scale.x;
            this->actor.scale.y = this->actor.scale.x;
        }
    }
    if (this->unk_19A == 16) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

void func_80ADB17C(EnPoSisters* this, PlayState* play) {
    this->unk_19A++;
    if (this->unk_19A == 64) {
        Flags_SetSwitch(play, this->actor.params);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_FLAME_IGNITION);
        if (this->unk_194 == 0) {
            Flags_UnsetSwitch(play, 0x1B);
        }
        play->envCtx.unk_BF = 0xFF;
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        Actor_Kill(&this->actor);
    } else if (this->unk_19A < 32) {
        func_80AD9240(this, this->unk_19A, &this->actor.world.pos);
    } else {
        func_80AD9240(this, 64 - this->unk_19A, &this->actor.world.pos);
    }
    if (this->unk_19A == 32) {
        this->actor.world.pos.x = D_80ADD7A4[this->unk_194].x;
        this->actor.world.pos.y = D_80ADD7A4[this->unk_194].y;
        this->actor.world.pos.z = D_80ADD7A4[this->unk_194].z;
    }
}

void func_80ADB2B8(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.xzDistToPlayer < 130.0f) {
        func_80AD9DF0(this, play);
    }
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_CRY);
    }
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
}

void func_80ADB338(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnPoSisters* realMeg = (EnPoSisters*)this->actor.parent;

    if (this->unk_195 == 0) {
        if (Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) < 600.0f) {
            if (this->unk_19C != 0) {
                this->unk_19C--;

                // Force Meg to respawn instantly after getting hit
                if (gSaveContext.n64ddFlag) {
                    this->unk_19C = 0;
                }
            }
        } else {
            this->unk_19C = 100;
        }
        if (this->unk_19C == 0) {
            this->actor.shape.rot.y = (s32)(4.0f * Rand_ZeroOne()) * 0x4000 + this->actor.yawTowardsPlayer;
            this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            func_80AD98F4(this, play);
        }
    } else {
        if (realMeg->actionFunc == func_80ADB51C) {
            this->actor.draw = EnPoSisters_Draw;
            func_80AD9E60(this);
        } else if (realMeg->actionFunc == func_80ADAE6C) {
            this->actor.shape.rot.y = this->actor.parent->shape.rot.y + this->unk_195 * 0x4000;
            this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            func_80AD98F4(this, play);
        } else if (realMeg->actionFunc == func_80ADAFC0) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80ADB4B0(EnPoSisters* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_80AD9E60(this);
    }
    func_80AD97C8(this, play);
    this->actor.world.pos.y += 1.0f;
    Actor_SetFocus(&this->actor, 40.0f);
}

void func_80ADB51C(EnPoSisters* this, PlayState* play) {
    f32 temp_f2;
    s16 phi_v0;
    s16 phi_a2;
    u8 temp;

    SkelAnime_Update(&this->skelAnime);
    temp_f2 = this->skelAnime.endFrame * 0.5f;
    this->unk_22E.a = (fabsf(temp_f2 - this->skelAnime.curFrame) * 255.0f) / temp_f2;
    if (this->unk_19A != 0) {
        this->unk_19A -= 1;
    }
    if (this->unk_19A == 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y += 0x4000 * (s32)(Rand_ZeroOne() * 4.0f);
        if (this->unk_195 == 0) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
        func_80AD9F1C(this);
    } else {
        this->actor.world.pos.y += 0.1f;
        temp = this->unk_195;
        if (temp != 0) {
            if (this->unk_19A > 90) {
                phi_v0 = 1;
                phi_a2 = 64;
            } else if (this->unk_19A > 70) {
                phi_v0 = 0;
                phi_a2 = 64;
            } else if (this->unk_19A > 55) {
                phi_v0 = 1;
                phi_a2 = 96;
            } else if (this->unk_19A > 40) {
                phi_v0 = 0;
                phi_a2 = 96;
            } else {
                phi_v0 = 1;
                phi_a2 = 256;
            }
            if (this->unk_195 == 2) {
                phi_a2 *= 2;
            }
            Math_ScaledStepToS(&this->actor.shape.rot.y,
                               this->actor.parent->shape.rot.y + (this->unk_195 * 0x4000) * phi_v0, phi_a2);
        } else if (this->unk_19A == 70 || this->unk_19A == 40) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
        }
    }
    func_80AD97C8(this, play);
    Actor_SetFocus(&this->actor, 40.0f);
}

void func_80ADB770(EnPoSisters* this, PlayState* play) {
    s32 temp_v0;
    s32 phi_a0;

    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->unk_19C > 0) {
        if (this->unk_19A >= 16) {
            SkelAnime_Update(&this->skelAnime);
            if (this->unk_195 == 0) {
                if (ABS((s16)(16 - this->unk_196)) < 14) {
                    this->actor.shape.rot.y +=
                        (0x580 - (this->unk_19C * 0x180)) * fabsf(Math_SinS(this->unk_196 * 0x800));
                }
                if (this->unk_19A >= 284 || this->unk_19A < 31) {
                    this->unk_199 |= 0x40;
                } else {
                    this->unk_199 &= ~0x40;
                }
            } else {
                this->actor.shape.rot.y = (s16)(this->actor.parent->shape.rot.y + (this->unk_195 * 0x4000));
            }
        }
    }
    if (this->unk_195 == 0) {
        if (this->unk_19A >= 284 || (this->unk_19A < 31 && this->unk_19A >= 16)) {
            this->unk_199 |= 0x40;
        } else {
            this->unk_199 &= ~0x40;
        }
    }
    if (Actor_WorldDistXZToPoint(&GET_PLAYER(play)->actor, &this->actor.home.pos) > 600.0f) {
        this->unk_199 &= ~0x40;
        func_80AD9C24(this, play);
    } else if (this->unk_19A == 0) {
        if (this->unk_195 == 0) {
            func_80AD94E0(this);
        } else {
            func_80AD9C24(this, play);
        }
    } else if (this->unk_195 != 0) {
        EnPoSisters* realMeg = (EnPoSisters*)this->actor.parent;

        if (realMeg->actionFunc == func_80ADAAA4) {
            func_80AD95D8(this);
        }
    } else if (this->unk_19C == 0) {
        this->unk_19C = -15;
    } else if (this->unk_19C < 0) {
        this->unk_19C++;
        if (this->unk_19C == 0) {
            func_80AD94E0(this);
        }
    }
    func_80AD97C8(this, play);
}

void func_80ADB9F0(EnPoSisters* this, PlayState* play) {
    f32 div;

    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_22E.a = 255;
        if (this->unk_194 == 3) {
            this->actor.flags |= ACTOR_FLAG_0;
            this->actor.home.pos.x = 1992.0f;
            this->actor.home.pos.z = -1440.0f;
            this->unk_199 |= 0x18;
            func_80AD93C4(this);
        } else {
            func_80AD9F90(this);
        }
    } else {
        div = this->skelAnime.curFrame / this->skelAnime.endFrame;
        this->unk_22E.a = 255.0f * div;
    }
    if (this->unk_194 != 3 && Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
    }
    Actor_SetFocus(&this->actor, 40.0f);
}

void func_80ADBB6C(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 10.0f) {
        func_80ADA028(this);
    } else {
        Math_ScaledStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos),
                           1820);
    }
}

void func_80ADBBF4(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1820);
    if (this->actor.xzDistToPlayer < 240.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        func_80AD93C4(this);
    }
}

void func_80ADBC88(EnPoSisters* this, PlayState* play) {
    if (D_80ADD784 != 0 || !Player_InCsMode(play)) {
        if (this->unk_19A != 0) {
            this->unk_19A--;
        }
        if (this->unk_19A == 30) {
            if (this->unk_194 == 0) {
                OnePointCutscene_Init(play, 3140, 999, NULL, MAIN_CAM);
            }
            D_80ADD784 = 1;
        }
        if (this->unk_19A == 0) {
            func_80ADA10C(this);
        }
    }
    func_8002F974(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
}

void func_80ADBD38(EnPoSisters* this, PlayState* play) {
    this->unk_19A++;
    func_80AD9240(this, this->unk_19A, &this->actor.home.pos);
    if (this->unk_19A == 32) {
        func_80ADA1B8(this);
    }
}

void func_80ADBD8C(EnPoSisters* this, PlayState* play) {
    this->unk_19A--;
    if (this->unk_19A == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_APPEAR);
        this->unk_199 &= ~0x80;
    }
    if (this->unk_19A <= 0) {
        if (SkelAnime_Update(&this->skelAnime)) {
            this->unk_22E.a = 255;
            D_80ADD784 |= (1 << this->unk_194);
        } else {
            this->unk_22E.a = (this->skelAnime.curFrame * 255.0f) / this->skelAnime.endFrame;
        }
    }
    if (D_80ADD784 == 15) {
        func_80ADA25C(this);
    }
}

void func_80ADBEE8(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    func_80AD9240(this, this->unk_19A, &this->actor.home.pos);
    if (this->unk_19A == 0) {
        func_80ADA2BC(this, play);
    }
}

void func_80ADBF58(EnPoSisters* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->unk_19A--;
    Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 0x500);
    if (this->unk_19A == 0 && this->unk_194 == 0) {
        play->envCtx.unk_BF = 4;
    }
    if (this->unk_19A < 0) {
        Math_StepToF(&this->actor.speedXZ, 5.0f, 0.2f);
    }
    if (this->unk_19A == -70 && this->unk_194 == 1) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &D_80ADD7BC, 40, NA_SE_EN_PO_LAUGH);
    }
    if (this->unk_19A < -120) {
        Actor_Kill(&this->actor);
    }
}

void func_80ADC034(EnPoSisters* this, PlayState* play) {
    if (this->actor.isTargeted && this->unk_22E.a == 255) {
        if (this->unk_197 != 0) {
            this->unk_197--;
        }
    } else {
        this->unk_197 = 20;
    }
    if (this->unk_22E.a == 0) {
        if (this->unk_19C != 0) {
            this->unk_19C--;
        }
    }
    if (this->actionFunc != func_80ADA7F0 && this->actionFunc != func_80ADA8C0 && this->actionFunc != func_80ADAAA4) {
        if (this->unk_197 == 0) {
            func_80AD9718(this);
        } else if (this->unk_19C == 0 && this->unk_22E.a == 0) {
            func_80AD98F4(this, play);
        }
    }
}

void func_80ADC10C(EnPoSisters* this, PlayState* play) {
    Vec3f sp24;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.info, 1);
        if (this->unk_195 != 0) {
            ((EnPoSisters*)this->actor.parent)->unk_19C--;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_LAUGH2);
            func_80AD9C24(this, play);
            if (Rand_ZeroOne() < 0.2f) {
                sp24.x = this->actor.world.pos.x;
                sp24.y = this->actor.world.pos.y;
                sp24.z = this->actor.world.pos.z;
                Item_DropCollectible(play, &sp24, ITEM00_ARROWS_SMALL);
            }
        } else if (this->collider.base.colType == 9 ||
                   (this->actor.colChkInfo.damageEffect == 0 && this->actor.colChkInfo.damage == 0)) {
            if (this->unk_194 == 0) {
                this->actor.freezeTimer = 0;
            }
        } else if (this->actor.colChkInfo.damageEffect == 0xF) {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            this->unk_199 |= 2;
            func_80AD98F4(this, play);
        } else if (this->unk_194 == 0 && this->actor.colChkInfo.damageEffect == 0xE &&
                   this->actionFunc == func_80ADB770) {
            if (this->unk_19C == 0) {
                this->unk_19C = -45;
            }
        } else {
            if (Actor_ApplyDamage(&this->actor) != 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_DAMAGE);
            } else {
                Enemy_StartFinishingBlow(play, &this->actor);
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_PO_SISTER_DEAD);
                gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_POE_SISTERS]++;
            }
            func_80AD95D8(this);
        }
    }
}

void EnPoSisters_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoSisters* this = (EnPoSisters*)thisx;
    s16 temp;
    
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_80AD9568(this);
    }
    func_80ADC10C(this, play);
    if (this->unk_199 & 4) {
        func_80ADC034(this, play);
    }
    this->actionFunc(this, play);
    if (this->unk_199 & 0x1F) {
        if (this->unk_199 & 8) {
            func_80ADA35C(this, play);
        }
        Actor_MoveForward(&this->actor);

        if (this->unk_199 & 0x10) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 0.0f, 5);
        } else {
            Vec3f vec;
            s32 sp34;

            vec.x = this->actor.world.pos.x;
            vec.y = this->actor.world.pos.y + 10.0f;
            vec.z = this->actor.world.pos.z;
            this->actor.floorHeight =
                BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &sp34, &this->actor, &vec);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        if (this->actionFunc == func_80ADA8C0 || this->actionFunc == func_80ADA7F0) {
            this->unk_198++;
            this->unk_198 = CLAMP_MAX(this->unk_198, 8);
        } else if (this->actionFunc != func_80ADAFC0) {
            temp = this->unk_198 - 1;
            this->unk_198 = CLAMP_MIN(temp, 1);
        }
        if (this->actionFunc == func_80ADA8C0) {
            this->actor.flags |= ACTOR_FLAG_24;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->unk_199 & 1) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != func_80ADB338) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
        Actor_SetFocus(&this->actor, 40.0f);
        if (this->actionFunc == func_80ADAC70) {
            this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
        } else if (this->unk_199 & 2) {
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }
}

void func_80ADC55C(EnPoSisters* this) {
    s16 temp_var;

    if (this->skelAnime.animation == &gPoeSistersAttackAnim) {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 255);
        this->unk_22E.g = CLAMP_MIN((s16)(this->unk_22E.g - 5), 50);
        temp_var = this->unk_22E.b - 5;
        this->unk_22E.b = CLAMP_MIN(temp_var, 0);
    } else if (this->skelAnime.animation == &gPoeSistersFleeAnim) {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 80);
        this->unk_22E.g = CLAMP_MAX((s16)(this->unk_22E.g + 5), 255);
        temp_var = this->unk_22E.b + 5;
        this->unk_22E.b = CLAMP_MAX(temp_var, 225);
    } else if (this->skelAnime.animation == &gPoeSistersDamagedAnim) {
        if (this->actor.colorFilterTimer & 2) {
            this->unk_22E.r = 0;
            this->unk_22E.g = 0;
            this->unk_22E.b = 0;
        } else {
            this->unk_22E.r = 80;
            this->unk_22E.g = 255;
            this->unk_22E.b = 225;
        }
    } else {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 255);
        this->unk_22E.g = CLAMP_MAX((s16)(this->unk_22E.g + 5), 255);
        if (this->unk_22E.b > 210) {
            temp_var = this->unk_22E.b - 5;
            this->unk_22E.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->unk_22E.b + 5;
            this->unk_22E.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

s32 EnPoSisters_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx, Gfx** gfxP) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    Color_RGBA8* color;

    if (limbIndex == 1 && (this->unk_199 & 0x40)) {
        if (this->unk_19A >= 284) {
            rot->x += (this->unk_19A * 0x1000) + 0xFFEE4000;
        } else {
            rot->x += (this->unk_19A * 0x1000) + 0xFFFF1000;
        }
    }
    if (this->unk_22E.a == 0 || limbIndex == 8 || (this->actionFunc == func_80ADAFC0 && this->unk_19A >= 8)) {
        *dList = NULL;
    } else if (limbIndex == 9) {
        *dList = D_80ADD7C8[this->unk_194];
    } else if (limbIndex == 10) {
        *dList = D_80ADD7D8[this->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, this->unk_22E.r, this->unk_22E.g, this->unk_22E.b, this->unk_22E.a);
    } else if (limbIndex == 11) {
        color = &D_80ADD7E8[this->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, color->r, color->g, color->b, this->unk_22E.a);
    }
    return false;
}

void EnPoSisters_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx,
                              Gfx** gfxP) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 i;
    s32 pad;

    if (this->actionFunc == func_80ADAFC0 && this->unk_19A >= 8 && limbIndex == 9) {
        gSPMatrix((*gfxP)++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfxP)++, gPoSistersBurnDL);
    }
    if (limbIndex == 8 && this->actionFunc != func_80ADB2B8) {
        if (this->unk_199 & 0x20) {
            for (i = this->unk_198 - 1; i > 0; i--) {
                this->unk_234[i] = this->unk_234[i - 1];
            }
            Matrix_MultVec3f(&D_80ADD7F8, &this->unk_234[0]);
        } else if (this->actionFunc == func_80ADBD8C) {
            Matrix_MultVec3f(&D_80ADD7F8, &this->actor.home.pos);
        }
        if (this->unk_198 > 0) {
            Color_RGBA8* color = &D_80ADD6F0[this->unk_194];
            f32 temp_f2 = Rand_ZeroOne() * 0.3f + 0.7f;

            if (this->actionFunc == func_80ADB17C || this->actionFunc == func_80ADBD38 ||
                this->actionFunc == func_80ADBEE8) {
                Lights_PointNoGlowSetInfo(&this->lightInfo, this->unk_234[0].x, this->unk_234[0].y + 15.0f,
                                          this->unk_234[0].z, color->r * temp_f2, color->g * temp_f2,
                                          color->b * temp_f2, 200);
            } else {
                Lights_PointGlowSetInfo(&this->lightInfo, this->unk_234[0].x, this->unk_234[0].y + 15.0f,
                                        this->unk_234[0].z, color->r * temp_f2, color->g * temp_f2, color->b * temp_f2,
                                        200);
            }
        } else {
            Lights_PointSetColorAndRadius(&this->lightInfo, 0, 0, 0, 0);
        }
        if (!(this->unk_199 & 0x80)) {
            Matrix_Get(&this->unk_2F8);
        }
    }
}

void EnPoSisters_Draw(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    u8 phi_s5 = 0;
    f32 phi_f20;
    s32 i;
    u8 spE7 = 0;
    Color_RGBA8* temp_s1 = &D_80ADD700[this->unk_194];
    Color_RGBA8* temp_s7 = &D_80ADD6F0[this->unk_194];
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);
    func_80ADC55C(this);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if (this->unk_22E.a == 255 || this->unk_22E.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->unk_22E.r, this->unk_22E.g, this->unk_22E.b, this->unk_22E.a);
        gSPSegment(POLY_OPA_DISP++, 0x09, D_80116280 + 2);
        POLY_OPA_DISP =
            SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                           EnPoSisters_OverrideLimbDraw, EnPoSisters_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->unk_22E.a);
        gSPSegment(POLY_XLU_DISP++, 0x09, D_80116280);
        POLY_XLU_DISP =
            SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                           EnPoSisters_OverrideLimbDraw, EnPoSisters_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }
    if (!(this->unk_199 & 0x80)) {
        Matrix_Put(&this->unk_2F8);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gPoSistersTorchDL);
    }
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (play->gameplayFrames * -20) % 512, 0x20, 0x80));
    gDPSetEnvColor(POLY_XLU_DISP++, temp_s1->r, temp_s1->g, temp_s1->b, temp_s1->a);
    if (this->actionFunc == func_80ADB17C) {
        if (this->unk_19A < 32) {
            phi_s5 = ((32 - this->unk_19A) * 255) / 32;
            phi_f20 = 0.0056000003f;
        } else {
            phi_s5 = (this->unk_19A * 255 - 8160) / 32;
            phi_f20 = 0.0027f;
        }
    } else if (this->actionFunc == func_80ADBD38) {
        phi_s5 = ((32 - this->unk_19A) * 255) / 32;
        phi_f20 = 0.0027f;
    } else if (this->actionFunc == func_80ADBEE8) {
        phi_s5 = ((32 - this->unk_19A) * 255) / 32;
        phi_f20 = 0.0035f;
    } else if (this->actionFunc == func_80ADBC88) {
        //! @bug uninitialised spE7
        phi_s5 = spE7;
        phi_f20 = 0.0027f;
    } else {
        phi_s5 = spE7;
        phi_f20 = this->actor.scale.x * 0.5f;
    }
    for (i = 0; i < this->unk_198; i++) {
        if (this->actionFunc != func_80ADB17C && this->actionFunc != func_80ADBD38 &&
            this->actionFunc != func_80ADBEE8) {
            phi_s5 = -i * 31 + 248;
        }
        FrameInterpolation_RecordOpenChild(this, this->epoch + i * 25);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, temp_s7->r, temp_s7->g, temp_s7->b, phi_s5);
        Matrix_Translate(this->unk_234[i].x, this->unk_234[i].y, this->unk_234[i].z, MTXMODE_NEW);
        Matrix_RotateZYX(0, (s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000), 0, MTXMODE_APPLY);
        if (this->actionFunc == func_80ADAFC0) {
            phi_f20 = (this->unk_19A - i) * 0.025f + 0.5f;
            phi_f20 = CLAMP(phi_f20, 0.5f, 0.8f) * 0.007f;
        }
        Matrix_Scale(phi_f20, phi_f20, phi_f20, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        FrameInterpolation_RecordCloseChild();
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnPoSisters_Reset(void) {
    D_80ADD784 = 0;
}