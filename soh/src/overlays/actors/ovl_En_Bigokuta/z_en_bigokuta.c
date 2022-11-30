#include "z_en_bigokuta.h"
#include "objects/object_bigokuta/object_bigokuta.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnBigokuta_Init(Actor* thisx, PlayState* play);
void EnBigokuta_Destroy(Actor* thisx, PlayState* play);
void EnBigokuta_Update(Actor* thisx, PlayState* play);
void EnBigokuta_Draw(Actor* thisx, PlayState* play);

void func_809BD318(EnBigokuta* this);
void func_809BD3E0(EnBigokuta* this);
void func_809BDF34(EnBigokuta* this, PlayState* play);
void func_809BD84C(EnBigokuta* this, PlayState* play);
void func_809BD8DC(EnBigokuta* this, PlayState* play);
void func_809BDAE8(EnBigokuta* this, PlayState* play);
void func_809BDB90(EnBigokuta* this, PlayState* play);
void func_809BDC08(EnBigokuta* this, PlayState* play);
void func_809BE3E4(EnBigokuta* this, PlayState* play);
void func_809BE4A4(EnBigokuta* this, PlayState* play);
void func_809BE518(EnBigokuta* this, PlayState* play);
void func_809BCF68(EnBigokuta* this, PlayState* play);
void func_809BDFC8(EnBigokuta* this, PlayState* play);
void func_809BE26C(EnBigokuta* this, PlayState* play);
void func_809BE180(EnBigokuta* this, PlayState* play);
void func_809BE058(EnBigokuta* this, PlayState* play);
void func_809BD1C8(EnBigokuta* this, PlayState* play);

static Color_RGBA8 sEffectPrimColor = { 255, 255, 255, 255 };
static Color_RGBA8 sEffectEnvColor = { 100, 255, 255, 255 };
static Vec3f sEffectPosAccel = { 0.0f, 0.0f, 0.0f };

const ActorInit En_Bigokuta_InitVars = {
    ACTOR_EN_BIGOKUTA,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BIGOKUTA,
    sizeof(EnBigokuta),
    (ActorFunc)EnBigokuta_Init,
    (ActorFunc)EnBigokuta_Destroy,
    (ActorFunc)EnBigokuta_Update,
    (ActorFunc)EnBigokuta_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK1,
            { 0x20000000, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_HARD,
            BUMP_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 45, -30 }, 75 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementInit),
    sJntSphElementInit,
};

static ColliderCylinderInit sCylinderInit[] = {
    { {
          COLTYPE_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEMTYPE_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          TOUCH_ON | TOUCH_SFX_HARD,
          BUMP_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { 30, 0, 12 } } },
    { {
          COLTYPE_HARD,
          AT_ON | AT_TYPE_ENEMY,
          AC_ON | AC_HARD | AC_TYPE_PLAYER,
          OC1_ON | OC1_TYPE_ALL,
          OC2_TYPE_1,
          COLSHAPE_CYLINDER,
      },
      {
          ELEMTYPE_UNK1,
          { 0x20000000, 0x00, 0x08 },
          { 0xFFCFFFE7, 0x00, 0x00 },
          TOUCH_ON | TOUCH_SFX_HARD,
          BUMP_ON,
          OCELEM_ON,
      },
      { 50, 100, 0, { -30, 0, 12 } } },
};

static CollisionCheckInfoInit sColChkInfoInit[] = { 4, 130, 120, 200 };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x59, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 33, ICHAIN_STOP),
};

// possibly color data
static s32 sUnused[] = { 0xFFFFFFFF, 0x969696FF };

void EnBigokuta_Init(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_bigokuta_Skel_006BC0, &object_bigokuta_Anim_0014B8,
                       this->jointTable, this->morphTable, 20);

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, &this->element);

    this->collider.elements->dim.worldSphere.radius = this->collider.elements->dim.modelSphere.radius;

    for (i = 0; i < ARRAY_COUNT(sCylinderInit); i++) {
        Collider_InitCylinder(play, &this->cylinder[i]);
        Collider_SetCylinder(play, &this->cylinder[i], &this->actor, &sCylinderInit[i]);
    }

    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, sColChkInfoInit);

    this->unk_194 = 1;

    if (this->actor.params == 0) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        func_809BD318(this);
    } else {
        func_809BD3E0(this);
        this->unk_19A = 0;
        this->unk_196 = 1;
        this->actor.home.pos.y = -1025.0f;
    }
}

void EnBigokuta_Destroy(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;

    Collider_DestroyJntSph(play, &this->collider);
    for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
        Collider_DestroyCylinder(play, &this->cylinder[i]);
    }
}

void func_809BCE3C(EnBigokuta* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y + this->unk_194 * -0x4000;
    this->actor.world.pos.x = Math_SinS(this->actor.world.rot.y) * 263.0f + this->actor.home.pos.x;
    this->actor.world.pos.z = Math_CosS(this->actor.world.rot.y) * 263.0f + this->actor.home.pos.z;
}

void func_809BCEBC(EnBigokuta* this, PlayState* play) {
    Vec3f pos;
    f32 yDistFromHome = this->actor.world.pos.y - this->actor.home.pos.y;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.home.pos.y + 3.0f;
    pos.z = this->actor.world.pos.z;

    if (((play->gameplayFrames % 7) == 0) && (yDistFromHome <= 0.0f) && (yDistFromHome > -100.0f)) {
        EffectSsGRipple_Spawn(play, &pos, 800, 1300, 0);
    }
}

void func_809BCF68(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;
    s16 rot;

    if (play->gameplayFrames & 1) {
        rot = Rand_S16Offset(0x1200, 0xC00) + this->actor.shape.rot.y - this->unk_194 * 0xA00;
    } else {
        rot = this->actor.shape.rot.y - this->unk_194 * 0xA00 - Rand_S16Offset(0x1200, 0xC00);
    }
    if (this->actionFunc != func_809BE4A4) {
        if (this->actionFunc == func_809BE3E4 || (play->gameplayFrames & 2)) {
            effectPos.x = this->actor.world.pos.x - Math_SinS(rot) * 80.0f;
            effectPos.z = this->actor.world.pos.z - Math_CosS(rot) * 80.0f;
            effectPos.y = this->actor.home.pos.y + 1.0f;
            EffectSsGRipple_Spawn(play, &effectPos, 100, 500, 0);
        } else {
            effectPos.x = this->actor.world.pos.x - Math_SinS(rot) * 120.0f;
            effectPos.z = this->actor.world.pos.z - Math_CosS(rot) * 120.0f;
            effectPos.y = this->actor.home.pos.y + 5.0f;
        }
    } else {
        effectPos.x = this->actor.world.pos.x - Math_SinS(rot) * 50.0f;
        effectPos.z = this->actor.world.pos.z - Math_CosS(rot) * 50.0f;
        effectPos.y = this->actor.home.pos.y + 1.0f;
        EffectSsGRipple_Spawn(play, &effectPos, 100, 500, 0);
    }
    EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 800);
    if (this->actionFunc != func_809BE4A4) {
        func_8002F974(&this->actor, NA_SE_EN_DAIOCTA_SPLASH - SFX_FLAG);
    }
}

void func_809BD1C8(EnBigokuta* this, PlayState* play) {
    s32 i;
    Vec3f effectPos;

    effectPos.y = this->actor.world.pos.y;

    for (i = 0; i < 4; i++) {
        effectPos.x = ((i >= 2) ? 1 : -1) * 60.0f + this->actor.world.pos.x;
        effectPos.z = ((i & 1) ? 1 : -1) * 60.0f + this->actor.world.pos.z;
        EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 2000);
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_LAND_BIG);
    func_80033E88(&this->actor, play, 0xA, 8);
}

void func_809BD2E4(EnBigokuta* this) {
    Actor* parent = this->actor.parent;

    if (parent != NULL) {
        Math_ScaledStepToS(&parent->world.rot.y, this->unk_19A, 0x10);
    }
}

void func_809BD318(EnBigokuta* this) {
    Animation_PlayLoop(&this->skelAnime, &object_bigokuta_Anim_0014B8);
    this->unk_19A = 0;
    this->actor.home.pos.y = -1025.0f;
    this->unk_196 = 121;
    this->actionFunc = func_809BD84C;
}

void func_809BD370(EnBigokuta* this) {
    this->unk_196 = 21;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actionFunc = func_809BD8DC;
}

void func_809BD3AC(EnBigokuta* this) {
    this->actor.world.pos.x = this->actor.home.pos.x + 263.0f;
    this->unk_196 = 10;
    this->actionFunc = func_809BDAE8;
    this->actor.world.pos.y = this->actor.home.pos.y;
}

void func_809BD3E0(EnBigokuta* this) {
    this->unk_196 = 40;
    this->actionFunc = func_809BDB90;
}

void func_809BD3F8(EnBigokuta* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_bigokuta_Anim_001CA4, -5.0f);
    this->unk_196 = 350;
    this->unk_198 = 80;
    this->unk_19A = this->unk_194 * -0x200;
    func_809BCE3C(this);
    this->cylinder[0].base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = func_809BDC08;
}

void func_809BD47C(EnBigokuta* this) {
    this->unk_196 = 16;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.colorFilterTimer = 0;
    this->actionFunc = func_809BDF34;
}

void func_809BD4A4(EnBigokuta* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_bigokuta_Anim_0014B8, -5.0f);
    this->unk_195 = true;
    this->actor.world.rot.x = this->actor.shape.rot.y + 0x8000;
    this->unk_19A = this->unk_194 * 0x200;
    this->collider.base.acFlags &= ~AC_ON;
    this->cylinder[0].base.atFlags |= AT_ON;
    this->actionFunc = func_809BDFC8;
}

void func_809BD524(EnBigokuta* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_bigokuta_Anim_000D1C, -5.0f);
    this->unk_196 = 80;
    this->unk_19A = 0;
    this->cylinder[0].base.atFlags |= AT_ON;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_MAHI);
    if (this->collider.elements->info.acHitInfo->toucher.dmgFlags & 1) {
        this->unk_195 = true;
        this->unk_196 = 20;
    } else {
        this->unk_195 = false;
        this->unk_196 = 80;
    }
    Actor_SetColorFilter(&this->actor, 0, 255, 0, this->unk_196);
    this->actionFunc = func_809BE058;
}

void func_809BD5E0(EnBigokuta* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_bigokuta_Anim_000444, -5.0f);
    this->unk_196 = 24;
    this->unk_19A = 0;
    this->cylinder[0].base.atFlags &= ~AT_ON;
    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 24);
    this->actionFunc = func_809BE180;
}
void func_809BD658(EnBigokuta* this) {

    Animation_MorphToPlayOnce(&this->skelAnime, &object_bigokuta_Anim_000A74, -5.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_DEAD2);
    this->unk_196 = 38;
    this->unk_198 = 10;
    this->actionFunc = func_809BE26C;
}

void func_809BD6B8(EnBigokuta* this) {
    if (!this->unk_195) {
        if (Rand_ZeroOne() < 0.5f) {
            this->unk_196 = 24;
        } else {
            this->unk_196 = 28;
        }
    } else {
        if (ABS(this->actor.shape.rot.y - this->actor.yawTowardsPlayer) >= 0x4000) {
            this->unk_196 = 4;
        } else {
            this->unk_196 = 0;
        }
    }
    this->unk_19A = 0;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = func_809BE3E4;
}

void func_809BD768(EnBigokuta* this) {
    this->unk_194 = Rand_ZeroOne() < 0.5f ? -1 : 1;
    this->unk_19A = 0;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->cylinder[0].base.atFlags &= ~AT_ON;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_SINK);
    this->actionFunc = func_809BE4A4;
}

void func_809BD7F0(EnBigokuta* this, PlayState* play) {
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&GET_PLAYER(play)->actor, &this->actor.home.pos);
    this->actor.shape.rot.y = this->actor.world.rot.y + (this->unk_194 * 0x4000);
    func_809BCE3C(this);
    this->actionFunc = func_809BE518;
}

void func_809BD84C(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    this->unk_196--;

    if (this->unk_196 == 13 || this->unk_196 == -20) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if (this->unk_196 == 1) {
        func_800F5ACC(NA_BGM_MINI_BOSS);
    }
    if (this->actor.params == 1) {
        func_809BD370(this);
    }
}

void func_809BD8DC(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;

    this->unk_196--;

    if (this->unk_196 >= 0) {
        Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x + 263.0f, 263.0f / 21);

        if (this->unk_196 < 14) {
            this->actor.world.pos.y = sinf(this->unk_196 * (M_PI / 28)) * 200.0f + this->actor.home.pos.y;
        } else {
            this->actor.world.pos.y =
                sinf((this->unk_196 - 7) * (M_PI / 14)) * 130.0f + (this->actor.home.pos.y + 70.0f);
        }
        if (this->unk_196 == 0) {
            effectPos.x = this->actor.world.pos.x + 40.0f;
            effectPos.y = this->actor.world.pos.y;
            effectPos.z = this->actor.world.pos.z - 70.0f;
            EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 2000);
            effectPos.x = this->actor.world.pos.x - 40.0f;
            EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 2000);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_LAND_WATER);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOLON_LAND_BIG);
            func_800AA000(0.0f, 0xFF, 0x14, 0x96);
        }
    } else if (this->unk_196 < -1) {
        this->actor.world.pos.y = this->actor.home.pos.y - (sinf((this->unk_196 + 1) * (M_PI / 10)) * 20.0f);
        if (this->unk_196 == -10) {
            func_809BD3AC(this);
        }
    }
}

void func_809BDAE8(EnBigokuta* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y + 0x4000, 0x400)) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        if (this->unk_196 == 0) {
            func_809BCE3C(this);
            this->actor.home.pos.y = this->actor.world.pos.y;
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            this->actor.params = 2;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
            func_809BD3E0(this);
        }
    }
}

void func_809BDB90(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 0) {
        if (this->actor.params == 3) {
            func_800F5ACC(NA_BGM_MINI_BOSS);
        }
        func_809BD3F8(this);
    }
}

void func_809BDC08(EnBigokuta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 phi_v0;
    s16 pad;
    s16 phi_v1;
    Vec3f sp28;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_OCTAROCK_BUBLE);
    }

    if (this->unk_196 < 0) {
        this->actor.shape.rot.y += this->unk_194 * 0x200;
        func_809BCE3C(this);
        this->unk_196++;
        if (this->unk_196 == 0) {
            this->unk_196 = 350;
        }
        func_809BCF68(this, play);
        return;
    }

    phi_v1 = (Actor_WorldDistXZToPoint(&player->actor, &this->actor.home.pos) - 180.0f) * (8.0f / 15);
    func_8002DBD0(&this->actor, &sp28, &player->actor.world.pos);
    if (fabsf(sp28.x) > 263.0f || ((sp28.z > 0.0f) && !Actor_IsFacingPlayer(&this->actor, 0x1B00) &&
                                   !Player_IsFacingActor(&this->actor, 0x2000, play))) {
        phi_v1 -= 0x80;
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
    }

    if ((this->actor.xzDistToPlayer < 250.0f) && !Actor_IsFacingPlayer(&this->actor, 0x6000)) {
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
        if (this->actor.xzDistToPlayer < 180.0f) {
            phi_v1 += 0x20;
        }
    } else {
        this->unk_198 = 80;
    }
    if (this->actor.colChkInfo.health == 1) {
        phi_v1 = (phi_v1 + 0x130) * 1.1f;
    } else {
        phi_v1 += 0x130;
    }
    this->actor.shape.rot.y += phi_v1 * this->unk_194;
    func_809BCE3C(this);
    func_809BCF68(this, play);
    if (this->unk_198 == 0) {
        func_809BD768(this);
    } else if (this->unk_196 == 0) {
        func_809BD4A4(this);
    } else if (this->unk_195) {
        phi_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (phi_v0 < 0) {
            phi_v0 = -phi_v0;
        }
        if (phi_v0 < 0x4100 && phi_v0 > 0x3F00) {
            if (Rand_ZeroOne() < 0.6f) {
                this->unk_196 = 0;
                func_809BD4A4(this);
            }
            this->unk_195 = false;
        }
    }
}

void func_809BDF34(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    this->actor.world.pos.y = (sinf(this->unk_196 * (M_PI / 16)) * 100.0f) + this->actor.home.pos.y;
    if (this->unk_196 == 0) {
        func_809BD1C8(this, play);
        func_809BD3F8(this);
    }
}

void func_809BDFC8(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 20) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_VOICE);
    }
    if ((this->unk_196 == 0) && Math_ScaledStepToS(&this->actor.shape.rot.y, this->actor.world.rot.x, 0x800)) {
        this->unk_194 = -this->unk_194;
        func_809BD3F8(this);
    }
}

void func_809BE058(EnBigokuta* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 speedXZ;

    if (this->unk_196 != 0) {
        this->unk_196--;
    }

    SkelAnime_Update(&this->skelAnime);

    if ((this->collider.base.ocFlags1 & OC1_HIT) || (this->cylinder[0].base.ocFlags1 & OC1_HIT) ||
        (this->cylinder[1].base.ocFlags1 & OC1_HIT)) {
        speedXZ = CLAMP_MIN(player->actor.speedXZ, 1.0f);
        if (!(this->collider.base.ocFlags1 & OC1_HIT)) {
            this->cylinder[0].base.ocFlags1 &= ~OC1_HIT;
            this->cylinder[1].base.ocFlags1 &= ~OC1_HIT;
            speedXZ *= -1.0f;
        }
        player->actor.world.pos.x -= speedXZ * Math_SinS(this->actor.shape.rot.y);
        player->actor.world.pos.z -= speedXZ * Math_CosS(this->actor.shape.rot.y);
    }
    if (this->unk_196 == 0) {
        func_809BD6B8(this);
    }
}

void func_809BE180(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    if (this->unk_196 == 0) {
        if (this->actor.colChkInfo.health != 0) {
            func_809BD4A4(this);
        } else {
            func_809BD658(this);
        }
    } else if (this->unk_196 >= 8) {
        this->actor.shape.rot.y += this->unk_194 * 0x200;
        this->actor.world.pos.y = sinf((this->unk_196 - 8) * (M_PI / 16)) * 100.0f + this->actor.home.pos.y;
        func_809BCE3C(this);
        if (this->unk_196 == 8) {
            func_809BD1C8(this, play);
        }
    }
}
void func_809BE26C(EnBigokuta* this, PlayState* play) {
    Vec3f effectPos;

    if (this->unk_196 != 0) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        if (this->unk_196 >= 10) {
            this->actor.shape.rot.y += 0x2000;
        }
    } else if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
        if (this->unk_198 == 6) {
            effectPos.x = this->actor.world.pos.x;
            effectPos.y = this->actor.world.pos.y + 150.0f;
            effectPos.z = this->actor.world.pos.z;
            func_8002829C(play, &effectPos, &sEffectPosAccel, &sEffectPosAccel, &sEffectPrimColor,
                          &sEffectEnvColor, 1200, 20);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_OCTAROCK_DEAD2);
        }
        if (this->unk_198 == 0 && Math_StepToF(&this->actor.scale.y, 0.0f, 0.001f)) {
            Flags_SetClear(play, this->actor.room);
            Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON0);
            func_8005ACFC(play->cameraPtrs[MAIN_CAM], 4);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 50, NA_SE_EN_OCTAROCK_BUBLE);
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xB0);
            Actor_Kill(&this->actor);
            gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_BIG_OCTO]++;
        }
    }
}

void func_809BE3E4(EnBigokuta* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->unk_196 != 0) {
        if (this->unk_196 != 0) {
            this->unk_196--;
        }
        this->actor.shape.rot.y += 0x2000;
    }
    if (this->unk_196 == 0) {
        if ((s16)(this->actor.shape.rot.y - this->actor.world.rot.y) > 0) {
            this->unk_194 = 1;
        } else {
            this->unk_194 = -1;
        }

        func_809BD3F8(this);
        if (!this->unk_195) {
            this->unk_196 = -40;
        }
    }
    func_809BCF68(this, play);
}

void func_809BE4A4(EnBigokuta* this, PlayState* play) {
    this->actor.world.pos.y -= 10.0f;
    this->actor.shape.rot.y += 0x2000;
    if (this->actor.world.pos.y < (this->actor.home.pos.y + -200.0f)) {
        func_809BD7F0(this, play);
    }
    func_809BCF68(this, play);
}

void func_809BE518(EnBigokuta* this, PlayState* play) {
    if (Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 10.0f)) {
        this->actor.flags |= ACTOR_FLAG_0;
        func_809BD3F8(this);
    }
}

void func_809BE568(EnBigokuta* this) {
    s32 i;
    f32 sin = Math_SinS(this->actor.shape.rot.y);
    f32 cos = Math_CosS(this->actor.shape.rot.y);

    this->collider.elements->dim.worldSphere.center.x =
        (this->collider.elements->dim.modelSphere.center.z * sin) +
        (this->actor.world.pos.x + (this->collider.elements->dim.modelSphere.center.x * cos));
    this->collider.elements->dim.worldSphere.center.z =
        (this->actor.world.pos.z + (this->collider.elements->dim.modelSphere.center.z * cos)) -
        (this->collider.elements->dim.modelSphere.center.x * sin);
    this->collider.elements->dim.worldSphere.center.y =
        this->collider.elements->dim.modelSphere.center.y + this->actor.world.pos.y;

    for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
        this->cylinder[i].dim.pos.x =
            this->actor.world.pos.x + sCylinderInit[i].dim.pos.z * sin + sCylinderInit[i].dim.pos.x * cos;
        this->cylinder[i].dim.pos.z =
            this->actor.world.pos.z + sCylinderInit[i].dim.pos.z * cos - sCylinderInit[i].dim.pos.x * sin;
        this->cylinder[i].dim.pos.y = this->actor.world.pos.y;
    }
}

void func_809BE798(EnBigokuta* this, PlayState* play) {
    s16 effectRot;
    s16 yawDiff;

    if ((this->cylinder[0].base.atFlags & AT_HIT) || (this->cylinder[1].base.atFlags & AT_HIT) ||
        (this->collider.base.atFlags & AT_HIT)) {
        this->cylinder[0].base.atFlags &= ~AT_HIT;
        this->cylinder[1].base.atFlags &= ~AT_HIT;
        this->collider.base.atFlags &= ~AT_HIT;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
        if (yawDiff > 0x4000) {
            effectRot = 0x4000;
        } else if (yawDiff > 0) {
            effectRot = 0x6000;
        } else if (yawDiff < -0x4000) {
            effectRot = -0x4000;
        } else {
            effectRot = -0x6000;
        }
        func_8002F71C(play, &this->actor, 10.0f, this->actor.world.rot.y + effectRot, 5.0f);
        if (this->actionFunc == func_809BDC08) {
            func_809BD4A4(this);
            this->unk_196 = 40;
        } else if (this->actionFunc == func_809BE3E4) {
            if ((effectRot * this->unk_194) > 0) {
                this->unk_194 = 0 - this->unk_194;
                this->unk_196 += 4;
            }
        }
    }
}

void EnBigokuta_UpdateDamage(EnBigokuta* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (this->actor.colChkInfo.damageEffect == 1) {
                if (this->actionFunc != func_809BE058) {
                    func_809BD524(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == 0xF) {
                func_809BD47C(this);
            } else if (!Actor_IsFacingPlayer(&this->actor, 0x4000)) {
                if (Actor_ApplyDamage(&this->actor) == 0) { // Dead
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_DEAD);
                    Enemy_StartFinishingBlow(play, &this->actor);
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DAIOCTA_DAMAGE);
                }
                func_809BD5E0(this);
            }
        }
    }
}

void EnBigokuta_Update(Actor* thisx, PlayState* play2) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    s32 i;
    PlayState* play = play2;

    func_809BE798(this, play);
    EnBigokuta_UpdateDamage(this, play);
    this->actionFunc(this, play);
    func_809BD2E4(this);
    func_809BE568(this);
    Camera_ChangeSetting(play->cameraPtrs[MAIN_CAM], CAM_SET_BIG_OCTO);
    func_8005AD1C(play->cameraPtrs[MAIN_CAM], 4);

    if (this->cylinder[0].base.atFlags & AT_ON) {
        if (this->actionFunc != func_809BE058) {
            for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->cylinder[i].base);
            }
            this->actor.flags |= ACTOR_FLAG_24;
        } else {
            for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->cylinder[i].base);
            }
        }
        for (i = 0; i < ARRAY_COUNT(this->cylinder); i++) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->cylinder[i].base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_SetFocus(&this->actor, this->actor.scale.y * 25.0f * 100.0f);
    func_809BCEBC(this, play);
}

s32 EnBigokuta_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    EnBigokuta* this = (EnBigokuta*)thisx;
    u8 intensity;
    f32 temp_f0;
    s32 temp_hi;

    if (limbIndex == 15) {
        if (this->actionFunc == func_809BE058 || this->actionFunc == func_809BE180) {
            OPEN_DISPS(play->state.gfxCtx);

            if (this->actionFunc == func_809BE058) {
                temp_hi = this->unk_196 % 12;
                if (temp_hi >= 8) {
                    temp_f0 = (12 - temp_hi) * (M_PI / 8);
                } else {
                    temp_f0 = temp_hi * (M_PI / 16);
                }
            } else {
                temp_hi = this->unk_196 % 6;
                if (temp_hi >= 4) {
                    temp_f0 = (6 - temp_hi) * (M_PI / 4);
                } else {
                    temp_f0 = temp_hi * (M_PI / 8);
                }
            }

            temp_f0 = sinf(temp_f0) * 125.0f + 130.0f;
            gDPPipeSync(POLY_OPA_DISP++);

            gDPSetEnvColor(POLY_OPA_DISP++, temp_f0, temp_f0, temp_f0, 255);
            CLOSE_DISPS(play->state.gfxCtx);
        }
    } else if (limbIndex == 10) {
        OPEN_DISPS(play->state.gfxCtx);
        if (this->actionFunc == func_809BE26C) {
            intensity = this->unk_196 * (255.0f / 38);
        } else {
            intensity = 255;
        }
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, intensity, intensity, intensity, intensity);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == 17 && this->actionFunc == func_809BE26C) {
        if (this->unk_198 < 5) {
            Matrix_Scale((this->unk_198 * 0.2f * 0.25f) + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        } else if (this->unk_198 < 8) {
            temp_f0 = (this->unk_198 - 5) * (1.0f / 12);
            Matrix_Scale(1.25f - temp_f0, 1.0f + temp_f0, 1.0f + temp_f0, MTXMODE_APPLY);
        } else {
            temp_f0 = ((this->unk_198 - 8) * 0.125f);
            Matrix_Scale(1.0f, 1.25f - temp_f0, 1.25f - temp_f0, MTXMODE_APPLY);
        }
    }
    return false;
}

void EnBigokuta_Draw(Actor* thisx, PlayState* play) {
    EnBigokuta* this = (EnBigokuta*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->actionFunc != func_809BE26C) || (this->unk_196 != 0) || (this->unk_198 != 0)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        if (this->unk_196 & 1) {
            if ((this->actionFunc == func_809BE180 && this->unk_196 >= 8) ||
                (this->actionFunc == func_809BE26C && this->unk_196 >= 10)) {
                f32 rotX = Rand_ZeroOne() * (M_PI * 2.0f);
                f32 rotY = Rand_ZeroOne() * (M_PI * 2.0f);

                Matrix_RotateY(rotY, MTXMODE_APPLY);
                Matrix_RotateX(rotX, MTXMODE_APPLY);
                Matrix_Scale(0.78999996f, 1.3f, 0.78999996f, MTXMODE_APPLY);
                Matrix_RotateX(-rotX, MTXMODE_APPLY);
                Matrix_RotateY(-rotY, MTXMODE_APPLY);
            }
        }
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, EnBigokuta_OverrideLimbDraw, NULL, this);
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (this->actor.scale.y * (255 / 0.033f)));
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, NULL, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
