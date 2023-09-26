/*
 * File: z_boss_va.c
 * Overlay: ovl_Boss_Va
 * Description: Barinade
 */

#include "z_boss_va.h"

#include <string.h>

#include "textures/boss_title_cards/object_bv.h"
#include "objects/object_bv/object_bv.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#include "soh/frame_interpolation.h"
#include "soh/Enhancements/boss-rush/BossRush.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

#define GET_BODY(this) ((BossVa*)(this)->actor.parent)
#define vaGorePulse offset.x
#define vaGorePulseRate offset.y
#define vaBariUnused headRot
#define vaCamRotMod headRot.x
#define vaBodySpinRate headRot.y

#define PHASE_2 3
#define PHASE_3 9
#define PHASE_4 15
#define PHASE_DEATH 18

typedef struct BossVaEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x26 */ u16 timer;
    /* 0x28 */ s16 mode;
    /* 0x2A */ Vec3s rot;
    /* 0x30 */ s16 primColor[4];
    /* 0x38 */ s16 envColor[4];
    /* 0x40 */ f32 scale;
    /* 0x44 */ f32 scaleMod;
    /* 0x48 */ Vec3f offset;
    /* 0x54 */ struct BossVa* parent;
    u32 epoch;
} BossVaEffect; // size = 0x58

typedef enum {
    /* 0 */ VA_NONE,
    /* 1 */ VA_LARGE_SPARK,
    /* 2 */ VA_BLAST_SPARK,
    /* 3 */ VA_SMALL_SPARK,
    /* 4 */ VA_SPARK_BALL,
    /* 5 */ VA_ZAP_CHARGE,
    /* 6 */ VA_BLOOD,
    /* 7 */ VA_TUMOR,
    /* 8 */ VA_GORE
} BossVaEffectType;

typedef enum {
    /* 1 */ SPARK_TETHER = 1,
    /* 2 */ SPARK_BARI,
    /* 3 */ SPARK_BLAST,
    /* 4 */ SPARK_UNUSED,
    /* 5 */ SPARK_BODY,
    /* 6 */ SPARK_LINK
} BossVaSparkMode;

typedef enum {
    /* 0 */ BLOOD_DROPLET,
    /* 1 */ BLOOD_SPLATTER,
    /* 2 */ BLOOD_SPOT
} BossVaBloodMode;

typedef enum {
    /* 0 */ TUMOR_UNUSED,
    /* 1 */ TUMOR_BODY,
    /* 2 */ TUMOR_ARM
} BossVaTumorMode;

typedef enum {
    /* 0 */ GORE_PERMANENT,
    /* 1 */ GORE_FLOOR,
    /* 2 */ GORE_FADING
} BossVaGoreMode;

typedef enum {
    /* -5 */ INTRO_UNUSED_START = -5,
    /* -4 */ INTRO_START,
    /* -3 */ INTRO_LOOK_DOOR,
    /* -2 */ INTRO_CLOSE_DOOR,
    /* -1 */ INTRO_DOOR_SHUT,
    /*  0 */ INTRO_CRACKLE,
    /*  1 */ INTRO_SPAWN_BARI,
    /*  2 */ INTRO_LOOK_BARI,
    /*  3 */ INTRO_REVERSE_CAMERA,
    /*  4 */ INTRO_SUPPORT_CAMERA,
    /*  5 */ INTRO_BODY_SOUND,
    /*  6 */ INTRO_LOOK_SUPPORT,
    /*  7 */ INTRO_UNUSED_CALL_BARI,
    /*  8 */ INTRO_CALL_BARI,
    /*  9 */ INTRO_ATTACH_BARI,
    /* 10 */ INTRO_TITLE,
    /* 11 */ INTRO_BRIGHTEN,
    /* 12 */ INTRO_FINISH,
    /* 13 */ BOSSVA_BATTLE,
    /* 14 */ DEATH_START,
    /* 15 */ DEATH_BODY_TUMORS,
    /* 16 */ DEATH_ZAPPER_1,
    /* 17 */ DEATH_ZAPPER_2,
    /* 18 */ DEATH_ZAPPER_3,
    /* 19 */ DEATH_SHELL_BURST,
    /* 20 */ DEATH_CORE_TUMORS,
    /* 21 */ DEATH_CORE_DEAD,
    /* 22 */ DEATH_CORE_BURST,
    /* 23 */ DEATH_MUSIC,
    /* 24 */ DEATH_FINISH
} BossVaCutscene;

void BossVa_Init(Actor* thisx, PlayState* play);
void BossVa_Destroy(Actor* thisx, PlayState* play);
void BossVa_Update(Actor* thisx, PlayState* play);
void BossVa_Draw(Actor* thisx, PlayState* play);
void BossVa_Reset(void);

void BossVa_UpdateEffects(PlayState* play);
void BossVa_DrawEffects(BossVaEffect* effect, PlayState* play);
void BossVa_DrawDoor(PlayState* play, s16 scale);

void BossVa_SetupIntro(BossVa* this);
void BossVa_SetupBodyPhase1(BossVa* this);
void BossVa_SetupBodyPhase2(BossVa* this, PlayState* play);
void BossVa_SetupBodyPhase3(BossVa* this);
void BossVa_SetupBodyPhase4(BossVa* this, PlayState* play);
void BossVa_SetupBodyDeath(BossVa* this, PlayState* play);

void BossVa_SetupSupportIntro(BossVa* this, PlayState* play);
void BossVa_SetupSupportAttached(BossVa* this, PlayState* play);
void BossVa_SetupSupportCut(BossVa* this, PlayState* play);

void BossVa_SetupZapperIntro(BossVa* this, PlayState* play);
void BossVa_SetupZapperAttack(BossVa* this, PlayState* play);
void BossVa_SetupZapperEnraged(BossVa* this, PlayState* play);
void BossVa_SetupZapperDamaged(BossVa* this, PlayState* play);
void BossVa_SetupZapperHold(BossVa* this, PlayState* play);

void BossVa_SetupStump(BossVa* this, PlayState* play);

void BossVa_SetupDoor(BossVa* this, PlayState* play);

void BossVa_SetupBariIntro(BossVa* this, PlayState* play);
void BossVa_SetupBariPhase2Attack(BossVa* this, PlayState* play);
void BossVa_SetupBariPhase3Attack(BossVa* this, PlayState* play);
void BossVa_SetupBariPhase3Stunned(BossVa* this, PlayState* play);
void BossVa_SetupBariDeath(BossVa* this);

void BossVa_BodyIntro(BossVa* this, PlayState* play);
void BossVa_BodyPhase1(BossVa* this, PlayState* play);
void BossVa_BodyPhase2(BossVa* this, PlayState* play);
void BossVa_BodyPhase3(BossVa* this, PlayState* play);
void BossVa_BodyPhase4(BossVa* this, PlayState* play);
void BossVa_BodyDeath(BossVa* this, PlayState* play);

void BossVa_SupportIntro(BossVa* this, PlayState* play);
void BossVa_SupportAttached(BossVa* this, PlayState* play);
void BossVa_SupportCut(BossVa* this, PlayState* play);

void BossVa_ZapperIntro(BossVa* this, PlayState* play);
void BossVa_ZapperAttack(BossVa* this, PlayState* play);
void BossVa_ZapperEnraged(BossVa* this, PlayState* play);
void BossVa_ZapperDamaged(BossVa* this, PlayState* play);
void BossVa_ZapperHold(BossVa* this, PlayState* play);
void BossVa_ZapperDeath(BossVa* this, PlayState* play);

void BossVa_Stump(BossVa* this, PlayState* play);

void BossVa_Door(BossVa* this, PlayState* play);

void BossVa_BariIntro(BossVa* this, PlayState* play);
void BossVa_BariPhase3Attack(BossVa* this, PlayState* play);
void BossVa_BariPhase2Attack(BossVa* this, PlayState* play);
void BossVa_BariPhase3Stunned(BossVa* this, PlayState* play);
void BossVa_BariDeath(BossVa* this, PlayState* play);

void BossVa_SpawnBloodSplatter(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale);
void BossVa_SpawnGore(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale);
void BossVa_SpawnSpark(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode);
void BossVa_SpawnZapperCharge(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* pos, Vec3s* rot,
                              s16 scale, u8 mode);
void BossVa_SpawnTumor(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode);
void BossVa_SpawnSparkBall(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale,
                           u8 mode);
void BossVa_SpawnBloodDroplets(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 scale, s16 phase,
                               s16 yaw);
void BossVa_Tumor(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode,
                  f32 range, u8 fixed);

const ActorInit Boss_Va_InitVars = {
    ACTOR_BOSS_VA,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_BV,
    sizeof(BossVa),
    (ActorFunc)BossVa_Init,
    (ActorFunc)BossVa_Destroy,
    (ActorFunc)BossVa_Update,
    (ActorFunc)BossVa_Draw,
    (ActorResetFunc)BossVa_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFEF, 0x03, 0x08 },
        { 0x00000010, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 85, 120, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInitSupport[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000010, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInitSupport = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInitSupport),
    sJntSphElementsInitSupport,
};

static ColliderJntSphElementInit sJntSphElementsInitBari[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x03, 0x04 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInitBari = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInitBari),
    sJntSphElementsInitBari,
};

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x20000000, 0x03, 0x04 },
        { 0x00000010, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL | TOUCH_UNK7,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f sInitPosOffsets[] = {
    { 0.0f, 175.35f, 0.0f },       // Support 1
    { 0.0f, 175.35f, 0.0f },       // Support 2
    { 0.0f, 175.35f, 0.0f },       // Support 3
    { 120.0f, 103.425f, -67.0f },  // Zapper 1
    { 0.0f, 103.425f, 140.0f },    // Zapper 2
    { -120.0f, 103.425f, -70.0f }, // Zapper 3
    { -2.0f, 16.0f, 50.0f },       // Upper Bari 1
    { 48.0f, 16.0f, 15.0f },       // Upper Bari 2
    { 25.0f, 16.0f, -36.0f },      // Upper Bari 3
    { -29.0f, 16.0f, -36.0f },     // Upper Bari 4
    { -63.0f, 16.0f, 22.0f },      // Upper Bari 5
    { 0.0f, -10.0f, -64.0f },      // Lower Bari 1
    { 63.0f, -10.0f, -22.0f },     // Lower Bari 2
    { 35.0f, -10.0f, 46.0f },      // Lower Bari 3
    { -36.0f, -10.0f, 46.0f },     // Lower Bari 4
    { -49.0f, -10.0f, -17.0f },    // Lower Bari 5
    { 0.0f, 160.0f, 370.0f },      // Upper Bari 1 CS
    { 65.0f, 35.0f, 370.0f },      // Upper Bari 2 CS
    { 80.0f, 70.0f, -130.0f },     // Upper Bari 3 CS
    { -160.0f, 100.0f, -130.0f },  // Upper Bari 4 CS
    { -150.0f, 130.0f, 0.0f },     // Upper Bari 5 CS
    { 230.0f, 0.0f, 0.0f },        // Lower Bari 1 CS
    { 60.0f, 140.0f, 0.0f },       // Lower Bari 2 CS
    { 0.0f, 40.0f, 270.0f },       // Lower Bari 3 CS
    { -100.0f, 10.0f, 200.0f },    // Lower Bari 4 CS
    { -90.0f, 70.0f, -310.0f },    // Lower Bari 5 CS
};

static Vec3s sInitRot[] = {
    { 0x1FFE, 0x0000, 0x0000 }, { 0x1FFE, 0x5550, 0x0000 }, { 0x1FFE, 0xAAB0, 0x0000 }, { 0xD558, 0x5550, 0x0000 },
    { 0xD558, 0x0000, 0x0000 }, { 0xD558, 0xAAB0, 0x0000 }, { 0x2AA8, 0xFCCC, 0x0000 }, { 0x2AA8, 0x3330, 0x0000 },
    { 0x2AA8, 0x6660, 0x0000 }, { 0x2AA8, 0x99A0, 0x0000 }, { 0x2AA8, 0xCCD0, 0x0000 }, { 0x4C98, 0x81D0, 0x0000 },
    { 0x4C98, 0x4F70, 0x0000 }, { 0x4C98, 0x1758, 0x0000 }, { 0x4C98, 0xE8A8, 0x0000 }, { 0x4C98, 0xB648, 0x0000 },
};

static Vec3f sWarpPos[] = {
    { 10.0f, 0.0f, 30.0f },
    { 260.0f, 0.0f, -470.0f },
    { -240.0f, 0.0f, -470.0f },
};

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xE),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0xD),
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

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static u8 sKillBari = 0;
static u8 sBodyBari[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static s16 sCsCamera = 0;

static BossVaEffect sVaEffects[400];
static u8 sBodyState;
static u8 sFightPhase;
static s8 sCsState;
static Vec3f sCameraEye;
static Vec3f sCameraAt;
static Vec3f sCameraNextEye;
static Vec3f sCameraNextAt;
static Vec3f sCameraEyeMaxVel;
static Vec3f sCameraAtMaxVel;
static s16 sDoorState;
static u8 sPhase3StopMoving;
static Vec3s sZapperRot;
static u16 sPhase2Timer;
static s8 sPhase4HP;

void BossVa_SetupAction(BossVa* this, BossVaActionFunc func) {
    this->actionFunc = func;
}

void BossVa_AttachToBody(BossVa* this) {
    BossVa* vaBody = GET_BODY(this);

    Matrix_Translate(vaBody->actor.world.pos.x, vaBody->actor.world.pos.y, vaBody->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(vaBody->actor.shape.rot.x, 0, vaBody->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_MultVec3f(&sInitPosOffsets[this->actor.params], &this->actor.world.pos);

    switch (this->actor.params) {
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            if (!this->onCeiling) {
                this->actor.shape.rot.x = sInitRot[this->actor.params].x + vaBody->actor.shape.rot.x;
                this->actor.shape.rot.y = sInitRot[this->actor.params].y;
                this->actor.shape.rot.z = sInitRot[this->actor.params].z + vaBody->actor.shape.rot.z;
            }
            break;

        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            this->actor.shape.rot.y = sInitRot[this->actor.params].y;
            this->actor.shape.rot.x = (sInitRot[this->actor.params].x +
                                       (s16)(Math_CosS(sInitRot[this->actor.params].y) * vaBody->actor.shape.rot.x)) -
                                      (s16)(Math_SinS(sInitRot[this->actor.params].y) * vaBody->actor.shape.rot.z);
            this->actor.shape.rot.z = (s16)(Math_CosS(sInitRot[this->actor.params].y) * vaBody->actor.shape.rot.z) +
                                      (sInitRot[this->actor.params].z +
                                       (s16)(Math_SinS(sInitRot[this->actor.params].y) * vaBody->actor.shape.rot.x));
            break;
    }

    this->actor.world.rot = this->actor.shape.rot;
    this->actor.shape.yOffset = GET_BODY(this)->actor.shape.yOffset;
}

void BossVa_BloodDroplets(PlayState* play, Vec3f* pos, s16 phase, s16 yaw) {
    s32 i;
    Vec3f spawnPos;

    for (i = 2; i > 0; i--) {
        spawnPos.x = Rand_CenteredFloat(10.0f) + pos->x;
        spawnPos.y = pos->y - (Rand_ZeroOne() * 15.0f);
        spawnPos.z = Rand_CenteredFloat(10.0f) + pos->z;
        BossVa_SpawnBloodDroplets(play, sVaEffects, &spawnPos, 65, phase, yaw);
    }
}

void BossVa_BloodSplatter(PlayState* play, BossVaEffect* src, s16 yaw, s16 scale, s32 count) {
    s32 i;
    Vec3f pos;

    for (i = count; i > 0; i--) {
        pos.x = Rand_CenteredFloat(10.0f) + src->pos.x;
        pos.y = src->pos.y - (Rand_ZeroOne() * 15.0f);
        pos.z = Rand_CenteredFloat(10.0f) + src->pos.z;
        BossVa_SpawnBloodSplatter(play, sVaEffects, &pos, (s16)Rand_CenteredFloat(0x6590) + yaw, scale);
    }
}

void BossVa_Gore(PlayState* play, BossVaEffect* src, s16 yaw, s16 scale) {
    s32 i;
    Vec3f pos;

    for (i = (sCsState <= DEATH_SHELL_BURST) ? 2 : 1; i > 0; i--) {
        pos.x = Rand_CenteredFloat(10.0f) + src->pos.x;
        pos.y = Rand_CenteredFloat(10.0f) + src->pos.y;
        pos.z = Rand_CenteredFloat(10.0f) + src->pos.z;
        BossVa_SpawnGore(play, sVaEffects, &pos, (s16)Rand_CenteredFloat(0x6590) + yaw, scale);
    }
}

void BossVa_Spark(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode,
                  f32 range, u8 fixed) {
    s32 i;
    s16 index;
    Vec3f offset;

    for (i = count; i > 0; i--) {
        if (!fixed) {
            index = Rand_ZeroOne() * (range - 0.6f);
        } else {
            index = range - 0.6f;
        }
        offset.x = Rand_CenteredFloat(xzSpread) + this->effectPos[index].x - this->actor.world.pos.x;
        offset.y = Rand_CenteredFloat(ySpread) + this->effectPos[index].y - this->actor.world.pos.y;
        offset.z = Rand_CenteredFloat(xzSpread) + this->effectPos[index].z - this->actor.world.pos.z;
        BossVa_SpawnSpark(play, sVaEffects, this, &offset, scale, mode);
    }
}

void BossVa_Tumor(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode,
                  f32 range, u8 fixed) {
    s16 index;
    s32 i;
    Vec3f offset;

    for (i = count; i > 0; i--) {
        if (!fixed) {
            index = Rand_ZeroOne() * (range - 0.6f);
        } else {
            index = range - 0.6f;
        }

        offset.x = Rand_CenteredFloat(xzSpread) + this->effectPos[index].x - this->actor.world.pos.x;
        offset.y = Rand_CenteredFloat(ySpread) + this->effectPos[index].y - this->actor.world.pos.y;
        offset.z = Rand_CenteredFloat(xzSpread) + this->effectPos[index].z - this->actor.world.pos.z;
        BossVa_SpawnTumor(play, sVaEffects, this, &offset, scale, mode);
    }
}

void BossVa_SetSparkEnv(PlayState* play) {
    play->envCtx.adjAmbientColor[0] = 0xA;
    play->envCtx.adjAmbientColor[1] = 0xA;
    play->envCtx.adjAmbientColor[2] = 0xA;
    play->envCtx.adjLight1Color[0] = 0x73;
    play->envCtx.adjLight1Color[1] = 0x41;
    play->envCtx.adjLight1Color[2] = 0x64;
    play->envCtx.adjFogColor[0] = 0x78;
    play->envCtx.adjFogColor[1] = 0x78;
    play->envCtx.adjFogColor[2] = 0x46;
}

void BossVa_SetDeathEnv(PlayState* play) {
    play->envCtx.adjFogColor[0] = 0xDC;
    play->envCtx.adjFogColor[1] = 0xDC;
    play->envCtx.adjFogColor[2] = 0x96;
    play->envCtx.adjFogNear = -0x3E8;
    play->envCtx.adjFogFar = -0x384;
    play->envCtx.adjAmbientColor[0] = 0xC8;
    play->envCtx.adjAmbientColor[1] = 0xC8;
    play->envCtx.adjAmbientColor[2] = 0xC8;
    play->envCtx.adjLight1Color[0] = 0xD7;
    play->envCtx.adjLight1Color[1] = 0xA5;
    play->envCtx.adjLight1Color[2] = 0xC8;
    play->envCtx.screenFillColor[0] = 0xDC;
    play->envCtx.screenFillColor[1] = 0xDC;
    play->envCtx.screenFillColor[2] = 0x96;
    play->envCtx.screenFillColor[3] = 0x64;
}

EnBoom* BossVa_FindBoomerang(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_MISC].head;

    while (actorIt != NULL) {
        if (actorIt->id != ACTOR_EN_BOOM) {
            actorIt = actorIt->next;
            continue;
        }
        return (EnBoom*)actorIt;
    }
    return NULL;
}

void BossVa_KillBari(BossVa* this, PlayState* play) {
    s32 i;
    s16 scale;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    for (i = 7; i >= 0; i--) {
        pos.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
        pos.y =
            Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y));
        pos.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
        velocity.y = Rand_ZeroOne() + 1.0f;
        scale = Rand_S16Offset(80, 100);
        if (Rand_ZeroOne() < 0.7f) {
            EffectSsDtBubble_SpawnColorProfile(play, &pos, &velocity, &accel, scale, 25, 2, 1);
        } else {
            EffectSsDtBubble_SpawnColorProfile(play, &pos, &velocity, &accel, scale, 25, 0, 1);
        }
    }

    sFightPhase++;
    BossVa_SetupBariDeath(this);
}

void BossVa_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossVa* this = (BossVa*)thisx;
    s32 i;
    s16 warpId;

    Actor_SetScale(&this->actor, 0.1f);
    this->actor.targetMode = 5;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    switch (this->actor.params) {
        case BOSSVA_BODY:
            //sFightPhase = 0;
            //sBodyState = 1;
            SkelAnime_Init(play, &this->skelAnime, &gBarinadeBodySkel, &gBarinadeBodyAnim, NULL, NULL, 0);
            this->actor.flags |= ACTOR_FLAG_PLAY_HIT_SFX;
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            SkelAnime_InitFlex(play, &this->skelAnime, &gBarinadeSupportSkel, &gBarinadeSupportAttachedAnim, NULL,
                               NULL, 0);
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            SkelAnime_InitFlex(play, &this->skelAnime, &gBarinadeZapperSkel, &gBarinadeZapperIdleAnim, NULL, NULL,
                               0);
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            SkelAnime_InitFlex(play, &this->skelAnime, &gBarinadeStumpSkel, &gBarinadeStumpAnim, NULL, NULL, 0);
            break;
        default:
            this->actor.flags |= ACTOR_FLAG_PLAY_HIT_SFX;
            SkelAnime_Init(play, &this->skelAnime, &gBarinadeBariSkel, &gBarinadeBariAnim, NULL, NULL, 0);
            this->actor.shape.yOffset = 400.0f;
            break;
        case BOSSVA_DOOR:
            break;
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->onCeiling = false;
    this->actor.naviEnemyId = 0x14;

    switch (this->actor.params) {
        case BOSSVA_BODY:
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA, 0.0f, 80.0f, 400.0f, 0, 0,
                               0, BOSSVA_DOOR);
            if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
                warpId = ACTOR_EN_RU1;
                if (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
                    warpId = ACTOR_DOOR_WARP1;
                }
                Actor_Spawn(&play->actorCtx, play, warpId, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0,
                            0, true); //! params could be WARP_DUNGEON_CHILD however this can also spawn Ru1
                Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x + 160.0f,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
                sDoorState = 100;
                Actor_Kill(&this->actor);
            } else {
                this->actor.colChkInfo.damageTable = sDamageTable;
                sPhase2Timer = 0xFFFF;
                if (Flags_GetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE)) {
                    sCsState = INTRO_CALL_BARI;
                    sDoorState = 100;
                    func_8002DF54(play, &this->actor, 1);
                    play->envCtx.screenFillColor[0] = 0xDC;
                    play->envCtx.screenFillColor[1] = 0xDC;
                    play->envCtx.screenFillColor[2] = 0xBE;
                    play->envCtx.screenFillColor[3] = 0xD2;
                    func_80064520(play, &play->csCtx);
                    sCsCamera = Play_CreateSubCamera(play);
                    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                    Play_ChangeCameraStatus(play, sCsCamera, CAM_STAT_ACTIVE);
                    sCameraNextEye.x = sCameraEye.x = 140.0f;
                    sCameraNextEye.y = sCameraEye.y = 205.0f;
                    sCameraNextEye.z = sCameraEye.z = -20.0f;
                    sCameraNextAt.x = sCameraAt.x = 10.0f;
                    sCameraNextAt.y = sCameraAt.y = 50.0f;
                    sCameraNextAt.z = sCameraAt.z = -220.0f;
                    Play_CameraSetAtEye(play, sCsCamera, &sCameraAt, &sCameraEye);
                    this->timer = 20;

                    for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
                        Actor_SpawnAsChild(
                            &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                            sInitPosOffsets[i].x + this->actor.world.pos.x,
                            sInitPosOffsets[i].y + this->actor.world.pos.y,
                            sInitPosOffsets[i].z + this->actor.world.pos.z, sInitRot[i].x + this->actor.world.rot.x,
                            sInitRot[i].y + this->actor.world.rot.y, sInitRot[i].z + this->actor.world.rot.z, i);
                    }

                    sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

                } else {
                    sCsState = INTRO_START;
                    sDoorState = 5;
                }

                this->zapHeadPos.x = 1.0f;
                Collider_InitCylinder(play, &this->colliderBody);
                Collider_SetCylinder(play, &this->colliderBody, &this->actor, &sCylinderInit);

                for (i = BOSSVA_ZAPPER_3; i >= BOSSVA_SUPPORT_1; i--) {
                    Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                        sInitPosOffsets[i].x + this->actor.world.pos.x, sInitPosOffsets[i].y + this->actor.world.pos.y,
                        sInitPosOffsets[i].z + this->actor.world.pos.z, sInitRot[i].x + this->actor.world.rot.x,
                        sInitRot[i].y + this->actor.world.rot.y, sInitRot[i].z + this->actor.world.rot.z, i);
                }

                memset((u8*)sVaEffects, 0, ARRAY_COUNT(sVaEffects) * sizeof(BossVaEffect));
                if (sCsState < BOSSVA_BATTLE) {
                    BossVa_SetupIntro(this);
                } else {
                    BossVa_SetupBodyPhase1(this);
                }
            }
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            Collider_InitJntSph(play, &this->colliderSph);
            Collider_SetJntSph(play, &this->colliderSph, &this->actor, &sJntSphInitSupport, this->elements);
            if (sCsState < BOSSVA_BATTLE) {
                BossVa_SetupSupportIntro(this, play);
            } else {
                BossVa_SetupSupportAttached(this, play);
            }
            this->onCeiling++;
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            Collider_InitQuad(play, &this->colliderLightning);
            Collider_SetQuad(play, &this->colliderLightning, &this->actor, &sQuadInit);
            if (sCsState < BOSSVA_BATTLE) {
                BossVa_SetupZapperIntro(this, play);
            } else {
                BossVa_SetupZapperAttack(this, play);
            }
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            BossVa_SetupStump(this, play);
            break;
        case BOSSVA_DOOR:
            BossVa_SetupDoor(this, play);
            break;
        default:
            Collider_InitJntSph(play, &this->colliderSph);
            Collider_SetJntSph(play, &this->colliderSph, &this->actor, &sJntSphInitBari, this->elements);
            Collider_InitQuad(play, &this->colliderLightning);
            Collider_SetQuad(play, &this->colliderLightning, &this->actor, &sQuadInit);
            this->unk_1D8.x = 1.0f;
            this->unk_1D8.y = 1.0f;
            if (sCsState < BOSSVA_BATTLE) {
                BossVa_SetupBariIntro(this, play);
            } else if (sFightPhase >= PHASE_3) {
                BossVa_SetupBariPhase3Attack(this, play);
            } else {
                BossVa_SetupBariPhase2Attack(this, play);
            }
            break;
    }
}

void BossVa_Destroy(Actor* thisx, PlayState* play) {
    BossVa* this = (BossVa*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyJntSph(play, &this->colliderSph);
    Collider_DestroyCylinder(play, &this->colliderBody);
}

void BossVa_SetupIntro(BossVa* this) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeBodyAnim);

    Animation_Change(&this->skelAnime, &gBarinadeBodyAnim, 1.0f, lastFrame, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->actor.shape.yOffset = -450.0f;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_BodyIntro);
}

void BossVa_BodyIntro(BossVa* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;

    switch (sCsState) {
        case INTRO_UNUSED_START:
            this->timer--;
            if (this->timer == 0) {
                sCsState = INTRO_CLOSE_DOOR;
                this->timer = 10;
            }
            break;
        case INTRO_START:
            play->envCtx.screenFillColor[0] = 0xDC;
            play->envCtx.screenFillColor[1] = 0xDC;
            play->envCtx.screenFillColor[2] = 0xBE;
            play->envCtx.screenFillColor[3] = 0xD2;
            func_8002DF54(play, &this->actor, 8);
            player->actor.world.rot.y = player->actor.shape.rot.y = 0x7FFF;
            sCsState++;
            break;
        case INTRO_LOOK_DOOR:
            func_80064520(play, &play->csCtx);
            if (sCsCamera == SUBCAM_FREE) {
                sCsCamera = Play_CreateSubCamera(play);
            }
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCsCamera, CAM_STAT_ACTIVE);

            sCameraNextEye.x = sCameraEye.x = 13.0f;
            sCameraNextEye.y = sCameraEye.y = 124.0f;
            sCameraNextEye.z = sCameraEye.z = 167.0f;

            sCameraNextAt.x = sCameraAt.x = player->actor.world.pos.x;
            sCameraNextAt.y = sCameraAt.y = player->actor.world.pos.y;
            sCameraNextAt.z = sCameraAt.z = player->actor.world.pos.z;

            sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

            this->timer = 10;
            sCsState++;
            break;
        case INTRO_CLOSE_DOOR:
            this->timer--;
            if (this->timer == 0) {
                func_8002DF54(play, &this->actor, 2);
                sCsState++;
                this->timer = 30;
            }
            break;
        case INTRO_DOOR_SHUT:
            this->timer--;
            if (this->timer == 0) {
                sCsState++;
            }
            if (Rand_ZeroOne() < 0.1f) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
            }
            break;
        case INTRO_CRACKLE:
            func_8002DF54(play, &this->actor, 1);
            sCsState++;
            break;
        case INTRO_SPAWN_BARI:
            func_80064520(play, &play->csCtx);
            if (sCsCamera == SUBCAM_FREE) {
                sCsCamera = Play_CreateSubCamera(play);
            }
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCsCamera, CAM_STAT_ACTIVE);

            sCameraNextEye.x = sCameraEye.x = 13.0f;
            sCameraNextEye.y = sCameraEye.y = 124.0f;
            sCameraNextEye.z = sCameraEye.z = 167.0f;

            sCameraNextAt.x = sCameraAt.x = player->actor.world.pos.x;
            sCameraNextAt.y = sCameraAt.y = player->actor.world.pos.y;
            sCameraNextAt.z = sCameraAt.z = player->actor.world.pos.z;

            sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

            for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
                Actor_SpawnAsChild(
                    &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                    sInitPosOffsets[i].x + this->actor.world.pos.x, sInitPosOffsets[i].y + this->actor.world.pos.y,
                    sInitPosOffsets[i].z + this->actor.world.pos.z, sInitRot[i].x + this->actor.world.rot.x,
                    sInitRot[i].y + this->actor.world.rot.y, sInitRot[i].z + this->actor.world.rot.z, i);
            }

            this->timer = 90;
            sCsState++;
            break;
        case INTRO_REVERSE_CAMERA:
            sCameraNextEye.x = -92.0f;
            sCameraNextEye.y = 22.0f;
            sCameraNextEye.z = 360.0f;
            sCameraNextAt.x = 63.0f;
            sCameraNextAt.y = 104.0f;
            sCameraNextAt.z = 248.0f;
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 7.0f, 0.3f, 0.7f, 0.05f);
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;
            sCameraEyeMaxVel.y = sCameraEyeMaxVel.z;
            sCameraAtMaxVel = sCameraEyeMaxVel;

            this->timer--;
            if (this->timer == 0) {
                sCsState++;
                this->timer = 60;
            }
            break;
        case INTRO_SUPPORT_CAMERA:
            sCameraNextEye.x = sCameraEye.x = 140.0f;
            sCameraNextEye.y = sCameraEye.y = 205.0f;
            sCameraNextEye.z = sCameraEye.z = -20.0f;

            sCameraNextAt.x = sCameraAt.x = 10.0f;
            sCameraNextAt.y = sCameraAt.y = 247.0f;
            sCameraNextAt.z = sCameraAt.z = -220.0f;

            sCsState++;
            this->timer = 1;
            break;
        case INTRO_BODY_SOUND:
            sCameraNextAt.x = 10.0f;
            sCameraNextAt.y = 247.0f;
            sCameraNextAt.z = -220.0f;
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 7.0f, 0.3f, 0.7f, 0.05f);
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;
            sCameraEyeMaxVel.y = sCameraEyeMaxVel.z;
            sCameraAtMaxVel = sCameraEyeMaxVel;

            this->timer--;
            if (this->timer == 0) {
                sCsState++;
                this->timer = 40;
            }
            break;
        case INTRO_LOOK_SUPPORT:
            this->timer--;
            if (this->timer == 0) {
                sCameraNextAt.x = 10.0f;
                sCameraNextAt.y = 50.0f;
                sCameraNextAt.z = -220.0f;

                sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

                sCsState++;
                sCsState++;
                this->timer = 20;
            }
            break;
        case INTRO_CALL_BARI:
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 14.0f, 0.3f, 1.0f, 0.25f);

            sCameraEyeMaxVel.y = sCameraEyeMaxVel.x * 0.7f;
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;

            sCameraAtMaxVel = sCameraEyeMaxVel;
            sCameraAtMaxVel.z = sCameraAtMaxVel.z * 1.75f;

            this->timer--;
            if (this->timer == 0) {
                sCsState++;
                this->timer = 7500;
                this->unk_1F2 = 0;
            }
            break;
        case INTRO_ATTACH_BARI:
            for (i = 10; i >= 1; i--) {
                if (sBodyBari[i - 1]) {
                    if (sBodyBari[i - 1] == 1) {
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_STICK);
                        BossVa_SetSparkEnv(play);
                        if (this->onCeiling == 0) {
                            this->onCeiling = 2; // Not used by body
                        }
                    } else if (sBodyBari[i - 1] == 2) {
                        BossVa_Spark(play, this, 6, 140, 50.0f, 30.0f, SPARK_BARI, i, true);
                    }

                    if (sBodyBari[i - 1] <= 2) {
                        sBodyBari[i - 1]++;
                    }
                }
            }
            Math_SmoothStepToS(&this->unk_1F2, 0x280, 1, 0x32, 0);
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 14.0f, 0.3f, 1.0f, 0.25f);
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;
            sCameraAtMaxVel = sCameraEyeMaxVel;
            if (this->timer >= 45000) {
                play->envCtx.unk_BF = 1;
                func_8002DF54(play, &this->actor, 8);
            } else if (this->timer >= 35000) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS);
            }

            this->timer += this->unk_1F2;
            if (this->timer >= 65536) {
                sCameraEyeMaxVel.y = sCameraAtMaxVel.y = 9.8f;
                sCsState++;

                sCameraNextEye.x = 10.0f;
                sCameraNextEye.z = 0.0f;

                sCameraNextAt.x = 10.0f;
                sCameraNextAt.y = 140.0f;
                sCameraNextAt.z = -200.0f;

                if (!Flags_GetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE)) {
                    TitleCard_InitBossName(play, &play->actorCtx.titleCtx,
                                           SEGMENTED_TO_VIRTUAL(gBarinadeTitleCardENGTex), 160, 180, 128, 40, true);
                }

                if (Rand_ZeroOne() < 0.1f) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }

                this->timer = 40;
            } else {
                sCameraEyeMaxVel.y = 1.6f;
                sCameraNextEye.y = 5.0f;
                sCameraNextEye.x = Math_SinS(this->timer) * 200.0f;
                sCameraNextEye.z = (Math_CosS(this->timer) * 200.0f) + -200.0f;
            }
            break;
        case INTRO_TITLE:
            BossVa_Spark(play, this, 3, 140, 50.0f, 30.0f, SPARK_BARI, 10.0f, false);
            this->timer--;
            if (this->timer == 0) {
                sCsState++;
                this->timer = 45;
            }
            break;
        case INTRO_BRIGHTEN:
            BossVa_Spark(play, this, 3, 140, 50.0f, 30.0f, SPARK_BARI, 10.0f, false);
            this->timer--;
            if (this->timer == 0) {
                sCsState++;
                this->timer = 11;
            }
            break;
        case INTRO_FINISH:
            this->timer--;
            if (this->timer == 0) {
                Play_ClearCamera(play, sCsCamera);
                sCsCamera = 0;
                func_80064534(play, &play->csCtx);
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
                func_8002DF54(play, &this->actor, 7);
                sCsState++;
                Flags_SetEventChkInf(EVENTCHKINF_BEGAN_BARINA_BATTLE);
                player->actor.shape.rot.y = player->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
            }
            break;
        case BOSSVA_BATTLE:
            BossVa_SetupBodyPhase1(this);
            break;
    }

    if (sCsState >= INTRO_BODY_SOUND) {
        func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
        if ((sCsState >= INTRO_CALL_BARI) && ((play->gameplayFrames % 4) == 0)) {
            BossVa_Spark(play, this, 1, 100, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
        }
    }

    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(Math_SinS(this->unk_1B0) * 50.0f) + 150;
    if ((sCsCamera != 0) && (sCsState <= INTRO_TITLE)) {
        Math_SmoothStepToF(&sCameraEye.x, sCameraNextEye.x, 0.3f, sCameraEyeMaxVel.x, 0.075f);
        Math_SmoothStepToF(&sCameraEye.y, sCameraNextEye.y, 0.3f, sCameraEyeMaxVel.y, 0.075f);
        Math_SmoothStepToF(&sCameraEye.z, sCameraNextEye.z, 0.3f, sCameraEyeMaxVel.z, 0.075f);
        Math_SmoothStepToF(&sCameraAt.x, sCameraNextAt.x, 0.3f, sCameraAtMaxVel.x, 0.075f);
        Math_SmoothStepToF(&sCameraAt.y, sCameraNextAt.y, 0.3f, sCameraAtMaxVel.y, 0.075f);
        Math_SmoothStepToF(&sCameraAt.z, sCameraNextAt.z, 0.3f, sCameraAtMaxVel.z, 0.075f);
        Play_CameraSetAtEye(play, sCsCamera, &sCameraAt, &sCameraEye);
    }
}

void BossVa_SetupBodyPhase1(BossVa* this) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeBodyAnim);

    Animation_Change(&this->skelAnime, &gBarinadeBodyAnim, 1.0f, lastFrame, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->actor.shape.yOffset = -450.0f;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->timer = 25;
    sBodyState = 0x80;
    BossVa_SetupAction(this, BossVa_BodyPhase1);
}

void BossVa_BodyPhase1(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(Math_SinS(this->unk_1B0) * 50.0f) + 150;
    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            sBodyState &= (u8)~0x80;
        }
    }

    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            func_8002F71C(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
        }
    }

    if (sBodyState & 0x7F) {
        this->skelAnime.curFrame = 0.0f;
        Actor_SetColorFilter(&this->actor, 0, 255, 0, 12);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_DAMAGE);
    }

    if (SkelAnime_Update(&this->skelAnime) && (sFightPhase >= PHASE_2)) {
        BossVa_SetupBodyPhase2(this, play);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 1, 0xC8, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->actor.world.rot.z, 1, 0xC8, 0);
    this->unk_1AC += 0xC31;
    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        BossVa_Spark(play, this, 1, 100, 50.0f, 10.0f, SPARK_BARI, 10.0f, false);
    }

    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    Collider_UpdateCylinder(&this->actor, &this->colliderBody);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
    func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
}

void BossVa_SetupBodyPhase2(BossVa* this, PlayState* play) {
    s32 i;

    sFightPhase++;
    for (i = BOSSVA_BARI_UPPER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                           sInitPosOffsets[i].x + this->actor.world.pos.x,
                           sInitPosOffsets[i].y + this->actor.world.pos.y,
                           sInitPosOffsets[i].z + this->actor.world.pos.z, sInitRot[i].x + this->actor.world.rot.x,
                           sInitRot[i].y + this->actor.world.rot.y, sInitRot[i].z + this->actor.world.rot.z, i);
    }

    this->invincibilityTimer = 0;
    this->actor.flags |= ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_BodyPhase2);
}

void BossVa_BodyPhase2(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp48;

    if (this->actor.colorFilterTimer == 0) {
        sPhase2Timer++;
        if ((this->invincibilityTimer != 0) && (this->actor.colorFilterParams & 0x4000)) {
            Actor_SetColorFilter(&this->actor, 0, 255, 0, 160);
            this->actor.colorFilterTimer = this->invincibilityTimer;
        } else {
            this->colliderBody.info.bumper.dmgFlags = 0x10;
        }
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->colliderBody.base.acFlags &= ~AC_HIT;

        if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
            sPhase2Timer &= 0xFE00;
            Actor_SetColorFilter(&this->actor, 0, 255, 0, 160);
            this->colliderBody.info.bumper.dmgFlags = 0xFC00712;
        } else {
            sKillBari++;
            if ((this->actor.colorFilterTimer != 0) && !(this->actor.colorFilterParams & 0x4000)) {
                this->invincibilityTimer = this->actor.colorFilterTimer - 5;
                if (this->invincibilityTimer > 160) {
                    this->invincibilityTimer = 0;
                }
            }

            Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 12);
        }

        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_FAINT);
    }

    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;

        sPhase2Timer = (sPhase2Timer + 0x18) & 0xFFF0;
        if (this->colliderBody.base.at == &player->actor) {
            func_8002F71C(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }

    if ((sPhase2Timer > 10) && !(sPhase2Timer & 7) && (this->actor.speedXZ == 1.0f)) {
        sp48 = this->actor.world.pos;
        sp48.y += 310.0f + (this->actor.shape.yOffset * this->actor.scale.y);
        sp48.x += -10.0f;
        sp48.z += 220.0f;
        BossVa_SpawnSparkBall(play, sVaEffects, this, &sp48, 4, 0);
    }

    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x, 1, 0xC8, 0);
    Math_SmoothStepToS(&this->actor.shape.rot.z, this->actor.world.rot.z, 1, 0xC8, 0);
    Math_SmoothStepToF(&this->actor.shape.yOffset, -1000.0f, 1.0f, 20.0f, 0.0f);
    if (!(sPhase2Timer & 0x100)) {
        this->actor.flags |= ACTOR_FLAG_TARGETABLE;
        this->actor.speedXZ = 1.0f;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
        this->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skelAnime) && (sFightPhase >= PHASE_3)) {
        BossVa_SetupBodyPhase3(this);
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        BossVa_Spark(play, this, 1, 100, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;

    Collider_UpdateCylinder(&this->actor, &this->colliderBody);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->actor.colorFilterTimer == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
    }

    if ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    }

    func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
}

void BossVa_SetupBodyPhase3(BossVa* this) {
    this->colliderBody.info.bumper.dmgFlags = 0x10;
    this->actor.speedXZ = 0.0f;
    sPhase3StopMoving = false;
    BossVa_SetupAction(this, BossVa_BodyPhase3);
}

void BossVa_BodyPhase3(BossVa* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 sp62;

    sp62 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(Math_SinS(this->unk_1B0) * 50.0f) + 150;
    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            func_8002F71C(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            this->actor.world.rot.y += (s16)Rand_CenteredFloat(0x2EE0) + 0x8000;
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->skelAnime.curFrame = 0.0f;
        Actor_SetColorFilter(&this->actor, 0, 255, 0, 12);
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_FAINT);
        sBodyState = 1;
        this->timer = 131;
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    } else {
        sBodyState = 0;
        if (this->timer == 0) {
            if (Math_SmoothStepToS(&this->vaBodySpinRate, 0xFA0, 1, 0x12C, 0) == 0) {
                if (this->actor.speedXZ == 0.0f) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                }
                Math_SmoothStepToF(&this->actor.speedXZ, 3.0f, 1.0f, 0.15f, 0.0f);
            }
            this->actor.flags |= ACTOR_FLAG_TARGETABLE;
        } else {
            this->timer--;
            if (this->timer < 35) {
                sBodyState = 0x80;
            }
            Math_SmoothStepToS(&this->vaBodySpinRate, 0, 1, 0x12C, 0);
            Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.2f, 0.0f);
            Math_SmoothStepToF(&this->actor.shape.yOffset, -1420.0f, 1.0f, 30.0f, 0.0f);
        }
    }

    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) >= 400.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, sp62, 1, 0x3E8, 0);
    } else if (player->invincibilityTimer != 0) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 0x12C, 0);
    } else if ((play->gameplayFrames & 0x80) == 0) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x12C, 0);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, sp62, 1, 0x258, 0);
    }

    if (sPhase3StopMoving) {
        this->actor.speedXZ = 0.0f;
    }

    Actor_MoveForward(&this->actor);
    if (SkelAnime_Update(&this->skelAnime) && (sFightPhase >= PHASE_4)) {
        BossVa_SetupBodyPhase4(this, play);
    }

    this->actor.shape.rot.y += this->vaBodySpinRate;
    if (sFightPhase == PHASE_3) {
        Math_SmoothStepToF(&this->actor.shape.yOffset, -450.0f, 1.0f, 15.0f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->actor.shape.yOffset, -810.0f, 1.0f, 15.0f, 0.0f);
    }

    if ((this->actor.shape.yOffset >= -500.0f) && (sFightPhase == PHASE_3)) {
        for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_LOWER_1; i--) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                               sInitPosOffsets[i].x + this->actor.world.pos.x,
                               sInitPosOffsets[i].y + this->actor.world.pos.y,
                               sInitPosOffsets[i].z + this->actor.world.pos.z, sInitRot[i].x + this->actor.world.rot.x,
                               sInitRot[i].y + this->actor.world.rot.y, sInitRot[i].z + this->actor.world.rot.z, i);
        }
        sFightPhase++;
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        BossVa_Spark(play, this, 1, 0x64, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 20.0f;
    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    Collider_UpdateCylinder(&this->actor, &this->colliderBody);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->timer == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    }

    func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
}

void BossVa_SetupBodyPhase4(BossVa* this, PlayState* play) {
    this->unk_1AC = 0;
    this->actor.flags |= ACTOR_FLAG_TARGETABLE;
    this->vaBodySpinRate = this->unk_1AC;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->timer2 = (s16)(Rand_ZeroOne() * 150.0f) + 300;
    sBodyState = 1;
    sPhase4HP = 4;
    if (this->actor.shape.yOffset != 0.0f) {
        this->timer = -30;
    }

    this->colliderBody.dim.radius = 55;
    BossVa_SetupAction(this, BossVa_BodyPhase4);
}

void BossVa_BodyPhase4(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 tmpf1;
    EnBoom* boomerang;

    this->unk_1B0 = (this->unk_1B0 + (s16)((sFightPhase - PHASE_4 + 1) * 1000.0f)) + 0xCE4;
    this->bodyGlow = (s16)(Math_SinS(this->unk_1B0) * 50.0f) + 150;
    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            func_8002F71C(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            this->actor.world.rot.y += (s16)Rand_CenteredFloat(0x2EE0) + 0x8000;
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->colliderBody.base.acFlags &= ~AC_HIT;
        this->skelAnime.curFrame = 0.0f;
        if (this->timer >= 0) {
            if (this->invincibilityTimer == 0) {
                this->invincibilityTimer = 8;
                if (this->actor.colChkInfo.damageEffect != 1) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_DAMAGE);
                    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 12);
                    sPhase4HP -= this->actor.colChkInfo.damage;
                    if (sPhase4HP <= 0) {
                        this->timer = 0;
                        sFightPhase++;
                        sPhase4HP += 3;
                        if (sFightPhase >= PHASE_DEATH) {
                            BossVa_SetupBodyDeath(this, play);
                            Enemy_StartFinishingBlow(play, &this->actor);
                            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_DEFEAT_BARINADE] = GAMEPLAYSTAT_TOTAL_TIME;
                            BossRush_HandleCompleteBoss(play);
                            return;
                        }
                        this->actor.speedXZ = -10.0f;
                        this->timer = -170 - (s16)(Rand_ZeroOne() * 150.0f);
                    }
                } else {
                    this->timer = (s16)Rand_CenteredFloat(40.0f) + 160;
                    this->vaBodySpinRate = 0;
                    this->actor.speedXZ = 0.0f;
                    Actor_SetColorFilter(&this->actor, 0, 125, 0, 255);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_FAINT);
                }
            }
        } else if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
            boomerang = (EnBoom*)this->colliderBody.base.ac;
            boomerang->returnTimer = 0;
            boomerang->moveTo = &player->actor;
            boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
            Audio_PlayActorSound2(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
        }
    } else if ((this->timer2 == 0) && (this->actor.shape.yOffset == 0.0f)) {
        this->timer = -220 - (s16)(Rand_ZeroOne() * 200.0f);
    } else if (this->timer2 != 0) {
        this->timer2--;
    }

    SkelAnime_Update(&this->skelAnime);
    if (this->timer == 0) {
        Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 1.0f, ((sFightPhase - PHASE_4 + 1) * 5.0f) + 10.0f, 0.0f);
        if (Math_SmoothStepToS(&this->vaBodySpinRate, (s16)((sFightPhase - PHASE_4 + 1) * 500.0f) + 0xFA0, 1, 0x12C,
                               0) == 0) {
            if (this->actor.speedXZ == 0.0f) {
                this->actor.colorFilterTimer = 0;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                this->timer2 = (s16)(Rand_ZeroOne() * 150.0f) + 300;
            }
            Math_SmoothStepToF(&this->actor.speedXZ, ((sFightPhase - PHASE_4 + 1) * 1.5f) + 4.0f, 1.0f, 0.25f, 0.0f);
        }
        this->colliderBody.info.bumper.dmgFlags = 0x10;
    } else {
        Math_SmoothStepToS(&this->vaBodySpinRate, 0, 1, 0x96, 0);
        if (this->timer > 0) {
            if ((player->stateFlags1 & 0x4000000) && (this->timer > 35)) {
                this->timer = 35;
            }
            Math_SmoothStepToF(&this->actor.shape.yOffset, -480.0f, 1.0f, 30.0f, 0.0f);
            this->colliderBody.info.bumper.dmgFlags = 0xFC00712;
            this->timer--;
        } else {
            if ((player->stateFlags1 & 0x4000000) && (this->timer < -60)) {
                this->timer = -59;
            }
            if ((play->gameplayFrames % 4) == 0) {
                BossVa_Spark(play, this, 2, 0x64, 220.0f, 5.0f, SPARK_BODY, 12.0f, true);
            }
            if (this->timer < -30) {
                if (this->actor.speedXZ > 0.0f) {
                    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
                }
                Math_SmoothStepToF(&this->actor.shape.yOffset, -1400.0f, 1.0f, 60.0f, 0.0f);
            } else {
                if (this->actor.speedXZ == 0.0f) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
                    this->timer2 = (s16)(Rand_ZeroOne() * 150.0f) + 330;
                }
                Math_SmoothStepToS(&this->vaBodySpinRate, 0xFA0, 1, 0x1F4, 0);
                tmpf1 = sFightPhase - PHASE_4 + 1;
                Math_SmoothStepToF(&this->actor.speedXZ, (tmpf1 + tmpf1) + 4.0f, 1.0f, 0.25f, 0.0f);
                Math_SmoothStepToF(&this->actor.shape.yOffset, 0.0f, 1.0f, 20.0f, 0.0f);
            }
            this->timer++;
        }
    }

    this->actor.shape.rot.y += this->vaBodySpinRate;
    if (this->actor.speedXZ < 0.0f) {
        Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;
    if (this->actor.bgCheckFlags & 8) {
        this->actor.bgCheckFlags &= ~8;
        this->actor.world.rot.y = (s16)Rand_CenteredFloat(30 * (0x10000 / 360)) + this->actor.wallYaw;
    }

    if (sFightPhase <= PHASE_4) {
        if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) >= 400.0f) {
            Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos),
                               1, 0x5DC, 0);
        } else if (player->invincibilityTimer != 0) {
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 0x12C, 0);
        } else if ((play->gameplayFrames & 0x80) == 0) {
            Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1,
                               (s16)((sFightPhase - PHASE_4 + 1) * 100.0f) + 0x64, 0);
        }
    }

    Actor_MoveForward(&this->actor);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 60.0f;
    if (((play->gameplayFrames % 2) == 0) && (this->timer == 0)) {
        BossVa_Spark(play, this, 2, 125, 40.0f, 10.0f, SPARK_BODY, 10.0f, false);
        BossVa_Spark(play, this, 1, 100, 15.0f, 10.0f, SPARK_BARI, 11.0f, true);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 70.0f, 0.0f, 1);
    Collider_UpdateCylinder(&this->actor, &this->colliderBody);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->invincibilityTimer == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->vaBodySpinRate > 0x3E8) || (this->actor.shape.yOffset < -1200.0f)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
    }
    func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
    if (this->invincibilityTimer != 0) {
        this->invincibilityTimer--;
        sBodyState = (sBodyState & 0x80) | 2;
    } else {
        sBodyState = (sBodyState & 0x80) | 1;
    }
}

void BossVa_SetupBodyDeath(BossVa* this, PlayState* play) {
    func_800F436C(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
    this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE);
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    this->vaCamRotMod = 0xC31;
    sCsState = DEATH_START;
    this->actor.speedXZ = 0.0f;
    this->unk_1A8 = 0.0f;
    Flags_SetClear(play, play->roomCtx.curRoom.num);
    BossVa_SetupAction(this, BossVa_BodyDeath);
}

void BossVa_BodyDeath(BossVa* this, PlayState* play) {
    s32 i;
    Camera* camera = Play_GetCamera(play, 0);
    s32 sp7C;
    Player* player = GET_PLAYER(play);
    s16 tmp16;

    switch (sCsState) {
        case DEATH_START:
            func_8002DF54(play, &this->actor, 1);
            func_80064520(play, &play->csCtx);
            sCsCamera = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCsCamera, CAM_STAT_ACTIVE);

            sCameraNextAt.x = this->actor.world.pos.x;
            sCameraNextAt.y = this->actor.world.pos.y;
            sCameraNextAt.z = this->actor.world.pos.z;

            sCameraAt = camera->at;

            sCameraNextEye = sCameraEye = camera->eye;

            sCameraNextEye.y = 40.0f;
            sCameraNextAt.y = 140.0f;

            sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

            this->unk_1AC = Math_Vec3f_Yaw(&sCameraEye, &sCameraNextAt) - 0x100;
            this->unk_1B0 = 15;
            play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
                play->envCtx.screenFillColor[2] = 0xFF;
            play->envCtx.screenFillColor[3] = 0;
            play->envCtx.fillScreen = true;
            sCsState++;
        case DEATH_BODY_TUMORS:
            this->unk_1AC += 0x100;
            sCameraNextEye.x = (Math_SinS(this->unk_1AC) * (160.0f + this->unk_1A8)) + sCameraNextAt.x;
            sCameraNextEye.z = (Math_CosS(this->unk_1AC) * (160.0f + this->unk_1A8)) + sCameraNextAt.z;
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 16.0f, 0.4f, 1.5f, 0.5f);
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;
            sCameraEyeMaxVel.y = sCameraEyeMaxVel.x * 0.5f;
            sCameraAtMaxVel = sCameraEyeMaxVel;
            tmp16 = Rand_CenteredFloat(0.5f) + ((sCameraEyeMaxVel.x * 0.5f) + 0.6f);
            if (((play->gameplayFrames % 4) == 0) && (this->unk_1B0 != 0)) {
                for (i = 6; i > 1; i--) {
                    BossVa_Tumor(play, this, 1, tmp16, 0.0f, 0.0f, TUMOR_BODY, i, true);
                }

                BossVa_Tumor(play, this, 1, tmp16, 0.0f, 0.0f, TUMOR_BODY, 11.0f, true);
                this->unk_1B0--;
            }

            if (this->unk_1B0 == 0) {
                sCsState++;

                sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;
            }
            break;
        case DEATH_CORE_DEAD:
            this->unk_1AC += 0x1862;
            this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.12f) + 1.0f;
            this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.075f) + 1.0f;
            if (!this->isDead) {
                this->burst++;
                this->isDead++;
                this->timer = 30;
                sCsState++;
                EffectSsDeadSound_SpawnStationary(play, &this->actor.projectedPos, NA_SE_EN_BALINADE_DEAD, 1, 1,
                                                  0x28);
                this->onCeiling = 2; // Not used by body
                BossVa_SetDeathEnv(play);
                func_8002DF54(play, &this->actor, 8);
            }
            break;
        case DEATH_CORE_BURST:
            if (this->timer == 13) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
            }

            this->timer--;
            if (this->timer == 0) {
                sCameraNextAt.x = this->actor.world.pos.x;
                sCameraNextAt.y = this->actor.world.pos.y + 30.0f;
                sCameraNextAt.z = this->actor.world.pos.z;

                sCameraNextEye.x = (Math_SinS(player->actor.shape.rot.y) * -130.0f) + player->actor.world.pos.x;
                sCameraNextEye.z = (Math_CosS(player->actor.shape.rot.y) * -130.0f) + player->actor.world.pos.z;
                sCameraNextEye.y = player->actor.world.pos.y + 55.0f;

                sCameraAtMaxVel = sCameraEyeMaxVel = sZeroVec;

                sCsState++;
                this->timer = 133;
            }
            break;
        case DEATH_MUSIC:
            Math_SmoothStepToF(&sCameraEyeMaxVel.x, 1.5f, 0.3f, 0.05f, 0.015f);
            sCameraEyeMaxVel.z = sCameraEyeMaxVel.x;
            sCameraEyeMaxVel.y = sCameraEyeMaxVel.z;
            sCameraAtMaxVel = sCameraEyeMaxVel;

            this->timer--;
            if (this->timer == 0) {
                Play_ClearCamera(play, sCsCamera);
                sCsCamera = 0;
                func_80064534(play, &play->csCtx);
                Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);

                camera->eyeNext = camera->eye = sCameraEye;

                camera->at = sCameraAt;

                func_8002DF54(play, &this->actor, 7);
                sCsState++;

                if (!IS_BOSS_RUSH) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
                }

                for (i = 2, sp7C = 2; i > 0; i--) {
                    if (Math_Vec3f_DistXYZ(&sWarpPos[i], &player->actor.world.pos) <
                        Math_Vec3f_DistXYZ(&sWarpPos[i - 1], &player->actor.world.pos)) {
                        sp7C = i - 1;
                    }
                }

                if (!IS_BOSS_RUSH) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_RU1, sWarpPos[sp7C].x, sWarpPos[sp7C].y,
                                sWarpPos[sp7C].z, 0, 0, 0, 0, true);
                } else {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, sWarpPos[sp7C].x, sWarpPos[sp7C].y,
                                sWarpPos[sp7C].z, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                }
            }
        case DEATH_FINISH:
            Rand_CenteredFloat(0.5f);
            play->envCtx.fillScreen = false;
            break;
    }

    if (sCsCamera != 0) {
        Math_SmoothStepToF(&sCameraEye.x, sCameraNextEye.x, 0.3f, sCameraEyeMaxVel.x, 0.15f);
        Math_SmoothStepToF(&sCameraEye.y, sCameraNextEye.y, 0.3f, sCameraEyeMaxVel.y, 0.15f);
        Math_SmoothStepToF(&sCameraEye.z, sCameraNextEye.z, 0.3f, sCameraEyeMaxVel.z, 0.15f);
        Math_SmoothStepToF(&sCameraAt.x, sCameraNextAt.x, 0.3f, sCameraAtMaxVel.x, 0.15f);
        Math_SmoothStepToF(&sCameraAt.y, sCameraNextAt.y, 0.3f, sCameraAtMaxVel.y, 0.15f);
        Math_SmoothStepToF(&sCameraAt.z, sCameraNextAt.z, 0.3f, sCameraAtMaxVel.z, 0.15f);
        Play_CameraSetAtEye(play, sCsCamera, &sCameraAt, &sCameraEye);
    }

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToF(&this->actor.shape.yOffset, -480.0f, 1.0f, 30.0f, 0.0f);
    Math_SmoothStepToS(&this->vaBodySpinRate, 0, 1, 0xC8, 0);
    Math_SmoothStepToS(&this->vaCamRotMod, 0, 1, 0xC8, 0);
    Math_SmoothStepToS(&this->bodyGlow, 200, 1, 10, 0);
    if (play->envCtx.screenFillColor[3] != 0) {
        play->envCtx.screenFillColor[3] -= 50;
    }

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    this->actor.shape.rot.y += this->vaBodySpinRate;
    this->unk_1AC += this->vaCamRotMod;

    this->unk_1A0 = (Math_CosS(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (Math_SinS(this->unk_1AC) * 0.05f) + 1.0f;
}

void BossVa_SetupSupportIntro(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeSupportAttachedAnim, 0.0f, 0.0f,
                     Animation_GetLastFrame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP_INTERP, 0.0f);
    this->timer = 0;
    BossVa_SetupAction(this, BossVa_SupportIntro);
}

void BossVa_SupportIntro(BossVa* this, PlayState* play) {
    BossVa_AttachToBody(this);
    if (sCsState == BOSSVA_BATTLE) {
        BossVa_SetupSupportAttached(this, play);
    } else if (sCsState >= INTRO_REVERSE_CAMERA) {
        this->timer++;
        if ((this->timer % 2) == 0) {
            BossVa_Spark(play, this, 2, 90, 5.0f, 0.0f, SPARK_BODY, ((this->timer & 0x20) >> 5) + 1, true);
        }

        SkelAnime_Update(&this->skelAnime);
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        if (Rand_ZeroOne() < 0.1f) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
        }
    }
}

void BossVa_SetupSupportAttached(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeSupportAttachedAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP, 0.0f);
    this->timer = this->actor.params * 10;
    BossVa_SetupAction(this, BossVa_SupportAttached);
}

void BossVa_SupportAttached(BossVa* this, PlayState* play) {
    this->timer++;
    if (sBodyState & 0x7F) {
        Actor_SetColorFilter(&this->actor, 0, 255, 0, 12);
        if (Rand_ZeroOne() > 0.5f) {
            Animation_Change(&this->skelAnime, &gBarinadeSupportDamage1Anim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gBarinadeSupportDamage1Anim), ANIMMODE_ONCE, 0.0f);
        } else {
            Animation_Change(&this->skelAnime, &gBarinadeSupportDamage2Anim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gBarinadeSupportDamage2Anim), ANIMMODE_ONCE, 0.0f);
        }
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_Change(&this->skelAnime, &gBarinadeSupportAttachedAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP, 0.0f);
    }

    BossVa_AttachToBody(this);
    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (this->colliderSph.base.acFlags & AC_HIT) {
        BossVa_SetupSupportCut(this, play);
    } else {
        if (this->actor.colorFilterTimer == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSph.base);
        }

        if ((this->timer % 2) == 0) {
            BossVa_Spark(play, this, 1, 100, 5.0f, 0.0f, SPARK_BODY, ((this->timer & 0x20) >> 5) + 1, true);
        }
    }
}

void BossVa_SetupSupportCut(BossVa* this, PlayState* play) {
    s32 stumpParams = this->actor.params + BOSSVA_STUMP_1;

    sBodyState++;
    sFightPhase++;
    Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_VA, this->armTip.x, this->armTip.y + 20.0f, this->armTip.z,
                0, this->actor.shape.rot.y, 0, stumpParams, true);
    Camera_AddQuake(&play->mainCamera, 2, 11, 8);
    this->burst = false;
    this->timer2 = 0;
    BossVa_SetupAction(this, BossVa_SupportCut);
}

void BossVa_SupportCut(BossVa* this, PlayState* play) {
    BossVa* vaBody = GET_BODY(this);
    f32 lastFrame;

    BossVa_AttachToBody(this);

    if (this->onCeiling) {
        lastFrame = Animation_GetLastFrame(&gBarinadeSupportCutAnim);
        this->onCeiling = false;
        this->timer = (s32)(Rand_ZeroOne() * 10.0f) + 5;
        SkelAnime_Free(&this->skelAnime, play);
        SkelAnime_InitFlex(play, &this->skelAnime, &gBarinadeCutSupportSkel, &gBarinadeSupportCutAnim, NULL, NULL,
                           0);
        Animation_Change(&this->skelAnime, &gBarinadeSupportCutAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
        sBodyState = 0;
        GET_BODY(this)->actor.shape.yOffset -= 60.0f;

        switch (this->actor.params) {
            case BOSSVA_SUPPORT_1:
                GET_BODY(this)->actor.world.rot.x += 0x4B0;
                break;
            case BOSSVA_SUPPORT_2:
                GET_BODY(this)->actor.world.rot.x -= 0x258;
                GET_BODY(this)->actor.world.rot.z -= 0x4E2;
                break;
            case BOSSVA_SUPPORT_3:
                GET_BODY(this)->actor.world.rot.x -= 0x258;
                GET_BODY(this)->actor.world.rot.z += 0x4E2;
                break;
        }
    }

    Math_SmoothStepToS(&this->headRot.x, vaBody->vaBodySpinRate * -3, 1, 0x4B0, 0);
    if (SkelAnime_Update(&this->skelAnime)) {
        lastFrame = Animation_GetLastFrame(&gBarinadeSupportDetachedAnim);
        Animation_Change(&this->skelAnime, &gBarinadeSupportDetachedAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                         0.0f);
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    }

    if ((this->timer == 0) && (sCsState < DEATH_START)) {
        this->timer = (s32)(Rand_ZeroOne() * 10.0f) + 10;
        BossVa_BloodDroplets(play, &this->armTip, this->headRot.x, this->actor.shape.rot.y);
    }

    if (sCsState >= DEATH_START) {
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 0.0f, 0.3f, 0.25f, 0.125f);
    }

    switch (sCsState) {
        case DEATH_SHELL_BURST:
            sCameraEye = sCameraNextEye;
            sCameraAt = sCameraNextAt;
            Math_SmoothStepToF(&sCameraEye.x, sCameraNextAt.x, 1.0f, 10.0f, 0.0f);
            Math_SmoothStepToF(&sCameraEye.z, sCameraNextAt.z, 1.0f, 10.0f, 0.0f);
            sCameraEye.y += 20.0f;
            sCsState++;

        case DEATH_CORE_TUMORS:
        case DEATH_CORE_DEAD:
        case DEATH_CORE_BURST:
            if (!this->burst) {
                if ((play->gameplayFrames % 2) != 0) {
                    BossVa_Tumor(play, this, 1, (s16)Rand_CenteredFloat(5.0f) + 6, 7.0f, 5.0f, TUMOR_ARM,
                                 (this->timer2 >> 3) + 1, true);
                }

                this->timer2++;
                if (this->timer2 >= 32) {
                    this->burst++;
                    this->isDead = true;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BREAK2);
                    if (this->actor.params == BOSSVA_SUPPORT_3) {
                        sCsState++;
                    }
                }
            } else {
                this->timer2--;
                if (this->timer2 == 0) {
                    Actor_Kill(&this->actor);
                }
            }
            break;
    }

    this->timer--;
}

void BossVa_SetupStump(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeStumpAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBarinadeStumpAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_Stump);
}

void BossVa_Stump(BossVa* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (Rand_ZeroOne() < 0.3f)) {
        this->skelAnime.curFrame -= Rand_ZeroOne() * 3.0f;
    }

    if (sCsState >= DEATH_START) {
        Actor_Kill(&this->actor);
    }
}

void BossVa_SetupZapperIntro(BossVa* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeZapperIdleAnim);

    Animation_Change(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_ZapperIntro);
}

void BossVa_ZapperIntro(BossVa* this, PlayState* play) {
    BossVa_AttachToBody(this);

    switch (sCsState) {
        case INTRO_TITLE:
        case INTRO_BRIGHTEN:
        case INTRO_FINISH:
            SkelAnime_Update(&this->skelAnime);
            break;
        case BOSSVA_BATTLE:
            BossVa_SetupZapperAttack(this, play);
            break;
    }

    Math_SmoothStepToS(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x2EE, 0);
    Math_SmoothStepToS(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x2EE, 0);
}

void BossVa_SetupZapperAttack(BossVa* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeZapperIdleAnim);

    Animation_Change(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_ZapperAttack);
}

void BossVa_ZapperAttack(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Actor* boomTarget;
    s16 yaw;
    s16 sp98;
    s16 sp96;
    s16 sp94;
    s16 tmp17;
    s16 sp90 = 0x1F4;
    s16 sp8E;
    u32 sp88;
    Vec3f sp7C;
    s32 pad3;
    f32 sp74;
    s32 i;
    s16 sp6E;
    s16 sp6C;
    f32 sp68;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    s16 sp5A;
    s16 sp58;
    s16 sp56;
    s16 sp54;
    f32 sp50;

    boomerang = BossVa_FindBoomerang(play);

    if ((boomerang == NULL) || (boomerang->moveTo == NULL) || (boomerang->moveTo == &player->actor)) {
        sp7C = player->actor.world.pos;
        sp7C.y += 10.0f;
        sp8E = 0x3E80;
    } else {
        sp74 = R_UPDATE_RATE * 0.5f;
        sp8E = 0x4650;

        boomTarget = boomerang->moveTo;
        sp7C = boomerang->actor.world.pos;
        sp6C = boomerang->actor.world.rot.y;
        sp56 = boomerang->actor.world.rot.x;

        for (i = boomerang->returnTimer; i >= 3; i--) {
            sp6E = Math_Vec3f_Yaw(&sp7C, &boomTarget->focus.pos);
            sp5A = sp6C - sp6E;
            sp58 = Math_Vec3f_Pitch(&sp7C, &boomTarget->focus.pos);
            sp54 = sp56 - sp58;

            sp50 = (200.0f - Math_Vec3f_DistXYZ(&sp7C, &boomTarget->focus.pos)) * 0.005f;
            if (sp50 < 0.12f) {
                sp50 = 0.12f;
            }

            if (sp5A < 0) {
                sp5A = -sp5A;
            }

            if (sp54 < 0) {
                sp54 = -sp54;
            }

            Math_ScaledStepToS(&sp6C, sp6E, sp5A * sp50);
            Math_ScaledStepToS(&sp56, sp58, sp54 * sp50);

            sp68 = -Math_SinS(sp56) * 12.0f;
            sp5C = Math_CosS(sp56) * 12.0f;
            sp64 = Math_SinS(sp6C) * sp5C;
            sp60 = Math_CosS(sp6C);
            sp7C.x += sp64 * sp74;
            sp7C.y += sp68 * sp74;
            sp7C.z += sp60 * sp5C * sp74;
        }
        sp90 = 0x3E80;
    }

    SkelAnime_Update(&this->skelAnime);
    BossVa_AttachToBody(this);
    if (sFightPhase >= PHASE_4) {
        BossVa_SetupZapperEnraged(this, play);
        return;
    }

    if (sBodyState & 0x7F) {
        BossVa_SetupZapperDamaged(this, play);
        return;
    }

    if ((sFightPhase < PHASE_4) && (GET_BODY(this)->actor.speedXZ != 0.0f)) {
        BossVa_SetupZapperHold(this, play);
        return;
    }

    sp98 = Math_Vec3f_Yaw(&sp7C, &this->armTip);
    tmp17 = sp98 - this->actor.shape.rot.y;

    if ((sp8E >= ABS(tmp17) || this->burst) && !(sBodyState & 0x80) && !(player->stateFlags1 & 0x04000000)) {

        if (!this->burst) {
            sp94 = sp98 - this->actor.shape.rot.y;

            if (ABS(sp94) > 0x1770) {
                sp94 = (sp94 > 0) ? 0x1770 : -0x1770;
            }

            tmp17 = Math_SmoothStepToS(&this->unk_1E6, sp94, 1, 0x6D6, 0);
            sp88 = ABS(tmp17);

            sp94 = sp98 - sp94;

            if (ABS(sp94) > 0x1770) {
                sp94 = (sp94 > 0) ? 0x1770 : -0x1770;
            }

            tmp17 = Math_SmoothStepToS(&this->unk_1EC, sp94, 1, 0x6D6, 0);
            sp88 += ABS(tmp17);

            yaw = Math_Vec3f_Yaw(&this->zapHeadPos, &sp7C);
            tmp17 = Math_SmoothStepToS(&this->unk_1F2, yaw - 0x4000, 1, 0x9C4, 0);
            sp88 += ABS(tmp17);

            sp96 = this->actor.shape.rot.x + this->skelAnime.jointTable[1].z + this->skelAnime.jointTable[2].z +
                   this->skelAnime.jointTable[3].z + this->skelAnime.jointTable[4].z + this->skelAnime.jointTable[5].z;

            yaw = Math_Vec3f_Pitch(&sp7C, &this->zapNeckPos);
            tmp17 = Math_SmoothStepToS(&this->unk_1EA, yaw - sp96, 1, 0xFA0, 0);
            sp88 += ABS(tmp17);

            yaw = Math_Vec3f_Pitch(&this->zapHeadPos, &sp7C);
            tmp17 = Math_SmoothStepToS(&this->unk_1F0, -yaw, 1, 0xFA0, 0);
            sp88 += ABS(tmp17);

            this->skelAnime.playSpeed = 0.0f;
            if (Math_SmoothStepToF(&this->skelAnime.curFrame, 0.0f, 1.0f, 2.0f, 0.0f) == 0.0f) {
                if (sp88 < sp90) {
                    this->timer2 = 0;
                    this->burst++;
                    this->unk_1D8 = sp7C;
                }

                if (Rand_ZeroOne() < 0.1f) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }
            }
        }
    } else {
        if (this->burst || (this->timer2 < 0)) {
            if (this->colliderLightning.base.atFlags & AT_HIT) {
                if (this->timer2 > 0) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_HIT_RINK);
                    BossVa_SetSparkEnv(play);
                    this->timer2 = -1;
                    GET_BODY(this)->onCeiling = 6; // not used by body
                }
            } else if (this->timer2 > 0) {
                this->timer2 = 0;
            }

            if ((this->timer2 < 0) && (player->stateFlags1 & 0x4000000)) {
                BossVa_Spark(play, this, 1, 30, 0.0f, 0.0f, SPARK_LINK, 0.0f, true);
            }
        }

        Math_SmoothStepToS(&this->unk_1E6, 0, 1, 0x6D6, 0);
        Math_SmoothStepToS(&this->unk_1EC, 0, 1, 0x6D6, 0);
        Math_SmoothStepToS(&this->unk_1EA, 0, 1, 0x6D6, 0);
        Math_SmoothStepToS(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x6D6, 0);
        Math_SmoothStepToS(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x6D6, 0);
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        this->burst = false;
    }

    if (this->burst && (this->burst != 2)) { // burst can never be 2
        if (this->timer2 >= 32) {
            if (this->timer2 == 32) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_THUNDER);
            }
            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 5.0f, true);
            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 6.0f, true);
            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 7.0f, true);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderLightning.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderLightning.base);
        } else {
            BossVa_Spark(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 3) + 1, true);
            if (this->timer2 == 30) {
                BossVa_SetSparkEnv(play);
            }
            if (this->timer2 == 20) {
                Vec3f sp44 = this->zapHeadPos;

                BossVa_SpawnZapperCharge(play, sVaEffects, this, &sp44, &this->headRot, 100, 0);
            }
        }

        this->timer2++;
        if (this->timer2 >= 40) {
            this->burst = false;
        }
    }
}

void BossVa_SetupZapperDamaged(BossVa* this, PlayState* play) {
    if (Rand_ZeroOne() > 0.5f) {
        Animation_Change(&this->skelAnime, &gBarinadeZapperDamage1Anim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gBarinadeZapperDamage1Anim), ANIMMODE_ONCE_INTERP, 4.0f);
    } else {
        Animation_Change(&this->skelAnime, &gBarinadeZapperDamage2Anim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gBarinadeZapperDamage2Anim), ANIMMODE_ONCE_INTERP, 4.0f);
    }

    Actor_SetColorFilter(&this->actor, 0, 255, 0, 12);
    this->burst = false;
    BossVa_SetupAction(this, BossVa_ZapperDamaged);
}

void BossVa_ZapperDamaged(BossVa* this, PlayState* play) {
    BossVa_AttachToBody(this);
    Math_SmoothStepToS(&this->unk_1E6, 0, 1, 0xFA0, 0);
    Math_SmoothStepToS(&this->unk_1E4, 0, 1, 0xFA0, 0);
    Math_SmoothStepToS(&this->unk_1EC, 0, 1, 0xFA0, 0);
    Math_SmoothStepToS(&this->unk_1EA, 0, 1, 0xFA0, 0);
    Math_SmoothStepToS(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x2EE, 0);
    Math_SmoothStepToS(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x2EE, 0);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (sFightPhase >= PHASE_4) {
            BossVa_SetupZapperEnraged(this, play);
        } else {
            BossVa_SetupZapperAttack(this, play);
        }
    }
}

void BossVa_SetupZapperDeath(BossVa* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeZapperIdleAnim);

    Animation_Change(&this->skelAnime, &gBarinadeZapperIdleAnim, Rand_ZeroOne() + 0.25f, Rand_ZeroOne() * 3.0f,
                     lastFrame, ANIMMODE_LOOP_INTERP, -6.0f);
    this->burst = false;
    this->timer2 = (this->actor.params * -6) + 18;
    this->unk_1B0 = 0;
    BossVa_SetupAction(this, BossVa_ZapperDeath);
}

void BossVa_ZapperDeath(BossVa* this, PlayState* play) {
    f32 sp3C = 55.0f;
    f32 tmpf1;
    f32 tmpf2;

    BossVa_AttachToBody(this);
    if (((play->gameplayFrames % 32) == 0) && (sCsState <= DEATH_BODY_TUMORS)) {
        this->unk_1E8 = Rand_CenteredFloat(0x4000);
        this->unk_1EE = Rand_CenteredFloat(0x4000);
        this->unk_1F4 = (s16)Rand_CenteredFloat(0x4000) + this->actor.shape.rot.y - this->actor.shape.rot.x;
    } else {
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 0.0f, 1.0f, 0.025f, 0.0f);
    }

    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->unk_1E6, this->unk_1E8, 1, (s16)Rand_CenteredFloat(500.0f) + 0x1F4, 0);
    Math_SmoothStepToS(&this->unk_1E4, 0, 1, 0x1F4, 0);
    Math_SmoothStepToS(&this->unk_1EC, this->unk_1EE, 1, (s16)Rand_CenteredFloat(500.0f) + 0x1F4, 0);
    Math_SmoothStepToS(&this->unk_1EA, 0, 1, 0x1F4, 0);
    Math_SmoothStepToS(&this->unk_1F2, this->unk_1F4, 1, (s16)Rand_CenteredFloat(500.0f) + 0x1F4, 0);

    switch (sCsState) {
        case DEATH_ZAPPER_2:
            sp3C = -55.0f;
        case DEATH_ZAPPER_1:
        case DEATH_ZAPPER_3:
            if (!this->burst) {
                if (((this->actor.params == BOSSVA_ZAPPER_1) && (this->timer2 < 16)) ||
                    ((this->actor.params == BOSSVA_ZAPPER_2) && (this->timer2 < 24)) ||
                    (this->actor.params == BOSSVA_ZAPPER_3)) {

                    if ((this->timer2 % 2) == 0 && (this->timer2 >= 0)) {
                        if (this->timer2 < 8) {
                            BossVa_Tumor(play, this, 1, (s16)Rand_CenteredFloat(5.0f) + 0xD, 0.0f, 0.0f, TUMOR_ARM,
                                         0.6f, true);
                        } else {
                            BossVa_Tumor(play, this, 1, (s16)Rand_CenteredFloat(5.0f) + 6, 0.0f, 7.0f, TUMOR_ARM,
                                         (this->timer2 >> 3) + 1, true);
                        }

                        BossVa_Spark(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 3) + 1, true);
                    }

                    this->timer2++;
                    if (this->timer2 >= 32) {
                        this->burst++;
                        this->isDead = true;
                        BossVa_SetDeathEnv(play);
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BREAK2);
                    }
                } else {
                    this->burst++;
                    this->isDead = true;
                    this->timer2 = 32;
                    sCsState++;
                }

                if ((this->actor.params - BOSSVA_ZAPPER_1 + DEATH_ZAPPER_1) == sCsState) {
                    sCameraAt.x = this->zapNeckPos.x;
                    sCameraEye.y = sCameraAt.y = this->zapNeckPos.y;
                    sCameraAt.z = this->zapNeckPos.z;
                    sCameraEye.x = (Math_CosS(-(this->actor.shape.rot.y + this->unk_1B0)) * sp3C) + this->zapNeckPos.x;
                    sCameraEye.z = (Math_SinS(-(this->actor.shape.rot.y + this->unk_1B0)) * sp3C) + this->zapNeckPos.z;
                    this->unk_1B0 += 0x15E;
                }
            } else {
                this->timer2--;
                if (this->timer2 == 0) {
                    if (this->actor.params == BOSSVA_ZAPPER_3) {
                        sCsState++;
                    }
                    Actor_Kill(&this->actor);
                }
            }
            break;
    }
}

void BossVa_SetupZapperEnraged(BossVa* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(&gBarinadeZapperIdleAnim);

    Animation_Change(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->burst = false;
    BossVa_SetupAction(this, BossVa_ZapperEnraged);
}

void BossVa_ZapperEnraged(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 tmp16;
    s16 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 yaw;
    u32 sp60;
    Vec3f sp54 = player->actor.world.pos;

    sp54.y += 10.0f;
    SkelAnime_Update(&this->skelAnime);
    BossVa_AttachToBody(this);
    if (sFightPhase >= PHASE_DEATH) {
        BossVa_SetupZapperDeath(this, play);
        return;
    }

    if (sBodyState & 0x7E) {
        BossVa_SetupZapperDamaged(this, play);
        return;
    }

    sp54.y += 25.0;

    sp6C = Math_Vec3f_Yaw(&sp54, &this->armTip);
    tmp16 = sp6C - this->actor.shape.rot.y;

    if ((ABS(tmp16) <= 0x4650 || this->burst) && !(sBodyState & 0x80) && !(player->stateFlags1 & 0x04000000)) {
        if (!this->burst) {

            sp68 = sp6C - this->actor.shape.rot.y;
            if (ABS(sp68) > 0x1770) {
                sp68 = (sp68 > 0) ? 0x1770 : -0x1770;
            }

            tmp16 = Math_SmoothStepToS(&this->unk_1E6, sp68, 1, 0xDAC, 0);
            sp60 = ABS(tmp16);

            sp68 = sp6C - sp68;
            if (ABS(sp68) > 0x1770) {
                sp68 = sp68 > 0 ? 0x1770 : -0x1770;
            }

            tmp16 = Math_SmoothStepToS(&this->unk_1EC, sp68, 1, 0xDAC, 0);
            sp60 += ABS(tmp16);

            yaw = Math_Vec3f_Yaw(&this->zapHeadPos, &sp54);
            tmp16 = Math_SmoothStepToS(&this->unk_1F2, yaw - 0x4000, 1, 0xEA6, 0);
            sp60 += ABS(tmp16);

            sp6A = this->actor.shape.rot.x + this->skelAnime.jointTable[1].x + this->skelAnime.jointTable[2].x +
                   this->skelAnime.jointTable[3].x + this->skelAnime.jointTable[4].x + this->skelAnime.jointTable[5].x;

            yaw = Math_Vec3f_Pitch(&sp54, &this->zapNeckPos);
            tmp16 = Math_SmoothStepToS(&this->unk_1EA, yaw - sp6A, 1, 0x1B58, 0);
            sp60 += ABS(tmp16);

            yaw = Math_Vec3f_Pitch(&this->zapHeadPos, &sp54);
            tmp16 = Math_SmoothStepToS(&this->unk_1F0, -yaw, 1, 0x1B58, 0);
            sp60 += ABS(tmp16);

            this->skelAnime.playSpeed = 0.0f;
            if ((Math_SmoothStepToF(&this->skelAnime.curFrame, 0.0f, 1.0f, 3.0f, 0.0f) == 0.0f) && (sp60 < 0x258)) {
                this->timer2 = 0;
                this->burst++;
                this->unk_1D8 = sp54;
                if (Rand_ZeroOne() < 0.1f) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }
            }
        }
    } else {
        if (this->burst || (this->timer2 < 0)) {
            if (this->colliderLightning.base.atFlags & AT_HIT) {
                if (this->timer2 > 0) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_HIT_RINK);
                    BossVa_SetSparkEnv(play);
                    this->timer2 = -1;
                    GET_BODY(this)->onCeiling = 6; // not used by body
                }
            } else if (this->timer2 > 0) {
                this->timer2 = 0;
            }

            if ((this->timer2 < 0) && (player->stateFlags1 & 0x4000000)) {
                BossVa_Spark(play, this, 1, 30, 0.0f, 0, SPARK_LINK, 0.0f, true);
            }
        }

        Math_SmoothStepToS(&this->unk_1E6, 0, 1, 0xEA6, 0);
        Math_SmoothStepToS(&this->unk_1EC, 0, 1, 0xEA6, 0);
        Math_SmoothStepToS(&this->unk_1EA, 0, 1, 0xEA6, 0);
        Math_SmoothStepToS(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0xEA6, 0);
        Math_SmoothStepToS(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0xEA6, 0);
        Math_SmoothStepToF(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        this->burst = false;
    }

    if (this->burst && (this->burst != 2)) { // burst can never be 2
        if (this->timer2 >= 16) {
            if (this->timer2 == 18) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_THUNDER);
            }

            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 5.0f, true);
            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 6.0f, true);
            BossVa_Spark(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 7.0f, true);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderLightning.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderLightning.base);
        } else {
            BossVa_Spark(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 1) + 1, true);
            if (this->timer2 == 14) {
                BossVa_SetSparkEnv(play);
            }
            if (this->timer2 == 4) {
                Vec3f sp48 = this->zapHeadPos;

                BossVa_SpawnZapperCharge(play, sVaEffects, this, &sp48, &this->headRot, 100, 0);
            }
        }

        this->timer2++;
        if (this->timer2 >= 24) {
            this->burst = false;
        }
    }
}

void BossVa_SetupZapperHold(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeZapperDamage2Anim, 0.0f, 0.0f,
                     Animation_GetLastFrame(&gBarinadeZapperDamage2Anim), ANIMMODE_ONCE_INTERP, -6.0f);
    this->burst = false;
    BossVa_SetupAction(this, BossVa_ZapperHold);
}

void BossVa_ZapperHold(BossVa* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    BossVa_AttachToBody(this);
    Math_SmoothStepToS(&this->unk_1E6, 0, 1, 0x1770, 0);
    Math_SmoothStepToS(&this->unk_1E4, 0, 1, 0x1770, 0);
    Math_SmoothStepToS(&this->unk_1EC, 0, 1, 0x1770, 0);
    Math_SmoothStepToS(&this->unk_1EA, 0, 1, 0x1770, 0);
    Math_SmoothStepToS(&this->unk_1F2, this->actor.shape.rot.y - 0x4000, 1, 0x2710, 0);
    Math_SmoothStepToS(&this->unk_1F0, this->skelAnime.jointTable[7].z - 0x1388, 1, 0x1770, 0);
    if (GET_BODY(this)->actor.speedXZ == 0.0f) {
        BossVa_SetupZapperAttack(this, play);
    }
}

void BossVa_SetupBariIntro(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->unk_1A0 = 60.0f;
    this->unk_1A4 = Rand_ZeroOne() * 360.0f;
    this->timer2 = 64;
    this->unk_1F0 = 120;
    this->unk_1A8 = 0.0f;
    this->actor.world.pos.x = sInitPosOffsets[this->actor.params + 10].x + this->actor.home.pos.x;
    this->actor.world.pos.y = sInitPosOffsets[this->actor.params + 10].y + this->actor.home.pos.y;
    this->actor.world.pos.z = sInitPosOffsets[this->actor.params + 10].z + this->actor.home.pos.z;
    this->timer = 45;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_BariIntro);
}

void BossVa_BariIntro(BossVa* this, PlayState* play) {
    Vec3f sp54 = this->actor.home.pos;
    f32 sp50 = 40.0f;
    s16 sp4E;
    s16 tmp;

    if (this->actor.home.pos.y >= 0.0f) {
        sp54.y += 25.0f;
    }

    this->unk_1A4 += Rand_ZeroOne() * 0.25f;

    switch (sCsState) {
        case INTRO_LOOK_BARI:
            if (this->actor.params == BOSSVA_BARI_UPPER_1) {
                func_8002DF54(play, &this->actor, 1);
                if (Math_SmoothStepToF(&this->actor.world.pos.y, 60.0f, 0.3f, 1.0f, 0.15f) == 0.0f) {
                    this->timer--;
                    if (this->timer == 0) {
                        sCsState++;
                    }
                }
            }
            this->actor.shape.rot.x = 0;
            break;
        case INTRO_REVERSE_CAMERA:
        case INTRO_SUPPORT_CAMERA:
        case INTRO_BODY_SOUND:
        case INTRO_LOOK_SUPPORT:
            if (this->actor.params != BOSSVA_BARI_UPPER_1) {
                Math_SmoothStepToF(&this->actor.world.pos.y,
                                   sInitPosOffsets[this->actor.params + 10].y + this->actor.home.pos.y, 0.3f, 1.0f,
                                   0.15f);
                this->actor.world.pos.x += (Math_SinF(this->unk_1A4 * 0.25f) * 0.5f);
            } else {
                Math_SmoothStepToF(&this->actor.world.pos.y, 60.0f, 0.3f, 1.0f, 0.15f);
            }
            this->actor.world.pos.y += Math_SinF(this->unk_1A4) * (2.0f - Math_SinF(this->unk_1A4));
            break;
        case INTRO_CALL_BARI:
        case INTRO_ATTACH_BARI:
            if ((this->timer2 > 15) && (this->timer < 0)) {
                Math_SmoothStepToF(&this->actor.world.pos.x, sp54.x, 1.0f, 6.5f, 0.0f);
                Math_SmoothStepToF(&this->actor.world.pos.y, sp54.y, 1.0f, 6.5f, 0.0f);
                Math_SmoothStepToF(&this->actor.world.pos.z, sp54.z, 1.0f, 6.5f, 0.0f);

                sp50 = Math_Vec3f_DistXYZ(&sp54, &this->actor.world.pos);
                if (sp50 <= 60.0f) {
                    tmp = Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.home.rot.x, 1, 0x7D0, 0);
                    sp4E = ABS(tmp);

                    tmp = Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 1, 0x7D0, 0);
                    sp4E += ABS(tmp);

                    if ((sp50 == 0.0f) && (sp4E == 0)) {
                        if (!this->isDead) {
                            if (this->actor.params >= BOSSVA_BARI_LOWER_1) {
                                if (this->actor.params == BOSSVA_BARI_LOWER_1) {
                                    sBodyBari[0]++;
                                } else {
                                    sBodyBari[this->actor.params - BOSSVA_BARI_UPPER_1]++;
                                }
                            } else {
                                sBodyBari[this->actor.params - BOSSVA_BARI_UPPER_1 + 1]++;
                            }
                            this->timer = -30;
                            this->isDead++;
                        } else {
                            this->timer++;
                            if (this->timer == 0) {
                                Actor_Kill(&this->actor);
                            }
                        }
                        return;
                    }
                }
            }
        case INTRO_UNUSED_CALL_BARI:
            this->timer--;
            if (this->timer == 0) {
                this->timer2 = 0;
            } else {
                func_80035844(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos, &this->actor.world.rot, false);
                this->unk_1A0 = Math_Vec3f_DistXYZ(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos);
                if (sp50 > 30.0f) {
                    BossVa_Spark(play, this, 1, 80, 15.0f, 0.0f, SPARK_BARI, 1.0f, true);
                }
            }
            break;
        case BOSSVA_BATTLE:
            this->timer++;
            if (this->timer == 0) {
                Actor_Kill(&this->actor);
            }
            return;
        case INTRO_TITLE:
        case INTRO_BRIGHTEN:
        case INTRO_FINISH:
            break;
    }

    if (((play->gameplayFrames % 4) == 0) && (sCsState < INTRO_ATTACH_BARI)) {
        BossVa_Spark(play, this, 1, 70, 25.0f, 20.0f, SPARK_BARI, 2.0f, true);
    }

    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }
}

void BossVa_SetupBariPhase3Attack(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->timer2 = 0x80;
    this->unk_1F0 = 0x78;
    this->unk_1A0 = 60.0f;
    this->unk_1A8 = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_BariPhase3Attack);
}

void BossVa_BariPhase3Attack(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Vec3f sp54 = GET_BODY(this)->unk_1D8;
    s16 sp52;
    s32 pad;

    this->unk_1A4 += Rand_ZeroOne() * 0.5f;
    sp52 = this->timer2 & 0x1FF;

    if ((play->gameplayFrames % 128) == 0) {
        this->vaBariUnused.x = (s16)(Rand_ZeroOne() * 100.0f) + 100;
    }

    Math_SmoothStepToS(&this->vaBariUnused.z, this->vaBariUnused.x, 1, 0x1E, 0);
    this->vaBariUnused.y += this->vaBariUnused.z;
    if ((this->colliderLightning.base.atFlags & AT_HIT) || (this->colliderSph.base.atFlags & AT_HIT)) {
        if ((this->colliderLightning.base.at == &player->actor) || (this->colliderSph.base.at == &player->actor)) {
            func_8002F71C(play, &this->actor, 8.0f, GET_BODY(this)->actor.yawTowardsPlayer, 8.0f);
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            this->colliderSph.base.at = NULL;
            this->colliderLightning.base.at = NULL;
        }

        this->colliderLightning.base.atFlags &= ~AT_HIT;
        this->colliderSph.base.atFlags &= ~AT_HIT;
    }

    if (this->colliderSph.base.acFlags & AC_HIT) {
        this->colliderSph.base.acFlags &= ~AC_HIT;
        if ((this->colliderSph.base.ac->id == ACTOR_EN_BOOM) && (sp52 >= 128)) {
            boomerang = (EnBoom*)this->colliderSph.base.ac;
            boomerang->returnTimer = 0;
            boomerang->moveTo = &player->actor;
            boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
            Audio_PlayActorSound2(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
        }
    }

    this->actor.world.pos.x = (Math_SinS(this->actor.world.rot.y) * this->unk_1A0) + sp54.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.world.rot.y) * this->unk_1A0) + sp54.z;
    Math_SmoothStepToF(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.pos.y += 2.0f * Math_SinF(this->unk_1A4);
    this->actor.world.rot.x = Math_Vec3f_Pitch(&sp54, &this->actor.world.pos);
    Math_SmoothStepToF(&this->unk_1A0, 160.0f, 1.0f, 2.0f, 0.0f);
    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 0x5DC, 0);
    if (!(this->timer2 & 0x200)) {
        this->unk_1AC = 0xBB8;
    } else {
        this->unk_1AC = -0xBB8;
    }

    if (sp52 >= 128) {
        BossVa_Spark(play, this, 1, 75, 15.0f, 7.0f, SPARK_TETHER, 1.0f, true);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSph.base);
        sPhase3StopMoving = false;
    } else {
        sPhase3StopMoving = true;
    }

    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderLightning.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        Math_SmoothStepToS(&this->unk_1F0, 0x78, 1, 0xA, 0);
    }

    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    this->actor.world.rot.y += this->unk_1AC;
    if (sBodyState & 0x7F) {
        BossVa_SetupBariPhase3Stunned(this, play);
    }
}

void BossVa_SetupBariPhase2Attack(BossVa* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->timer2 = 0x40;
    this->unk_1F0 = 0x78;
    this->unk_1A0 = 60.0f;
    this->unk_1A8 = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_BariPhase2Attack);
}

void BossVa_BariPhase2Attack(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Vec3f sp54 = GET_BODY(this)->unk_1D8;
    s16 sp52;
    s16 sp50;
    f32 sp4C;
    s32 pad;

    this->unk_1A4 += Rand_ZeroOne() * 0.5f;
    sp52 = this->timer2 & 0x1FF;
    if ((play->gameplayFrames % 128) == 0) {
        this->vaBariUnused.x = (s16)(Rand_ZeroOne() * 100.0f) + 100;
    }

    sp50 = (sFightPhase * 70) - 280;
    Math_SmoothStepToS(&this->vaBariUnused.z, this->vaBariUnused.x, 1, 0x1E, 0);
    this->vaBariUnused.y += this->vaBariUnused.z;
    if (sKillBari != 0) {
        sKillBari--;
        BossVa_KillBari(this, play);
        return;
    }

    if ((this->colliderLightning.base.atFlags & AT_HIT) || (this->colliderSph.base.atFlags & AT_HIT)) {
        if ((this->colliderLightning.base.at == &player->actor) || (this->colliderSph.base.at == &player->actor)) {
            func_8002F71C(play, &this->actor, 8.0f, GET_BODY(this)->actor.yawTowardsPlayer, 8.0f);
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
            this->colliderSph.base.at = NULL;
            this->colliderLightning.base.at = NULL;
        }

        this->colliderLightning.base.atFlags &= ~AT_HIT;
        this->colliderSph.base.atFlags &= ~AT_HIT;
    }

    Math_SmoothStepToF(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.rot.x = Math_Vec3f_Pitch(&sp54, &this->actor.world.pos);
    if ((play->gameplayFrames % 8) == 0) {
        Math_SmoothStepToS(&this->unk_1F0, 0x28, 1, 0xA, 0);
        BossVa_Spark(play, this, 1, this->unk_1F0, 25.0f, 20.0f, 2, 2.0f, true);
    }

    if (!(sPhase2Timer & 0x100) && (GET_BODY(this)->actor.colorFilterTimer == 0)) {
        sp4C = 200.0f;
        BossVa_Spark(play, this, 1, 125, 15.0f, 7.0f, SPARK_TETHER, 1.0f, true);
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
        if (this->actor.params & 1) {
            sp4C = -200.0f;
        }

        Math_SmoothStepToF(&this->unk_1A0, (Math_SinS(sPhase2Timer * 0x190) * sp4C) + 320.0f, 1.0f, 10.0f, 0.0f);
        Math_SmoothStepToS(&this->unk_1AC, sp50 + 0x1F4, 1, 0x3C, 0);
        this->actor.world.pos.y += 2.0f * Math_SinF(this->unk_1A4);
        if (this->colliderSph.base.acFlags & AC_HIT) {
            this->colliderSph.base.acFlags &= ~AC_HIT;

            if ((this->colliderSph.base.ac->id == ACTOR_EN_BOOM) && (sp52 >= 64)) {
                boomerang = (EnBoom*)this->colliderSph.base.ac;
                boomerang->returnTimer = 0;
                boomerang->moveTo = &player->actor;
                boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
                Audio_PlayActorSound2(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
            }
        }

        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderLightning.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSph.base);
    } else {
        this->actor.flags |= ACTOR_FLAG_TARGETABLE;
        Math_SmoothStepToS(&this->unk_1AC, sp50 + 150, 1, 0x3C, 0);
        if (GET_BODY(this)->actor.colorFilterTimer == 0) {
            Math_SmoothStepToF(&this->unk_1A0, 180.0f, 1.0f, 1.5f, 0.0f);
        } else {
            this->unk_1AC = 0;
            if (this->actor.colorFilterTimer == 0) {
                Actor_SetColorFilter(&this->actor, 0, 255, 0x2000, GET_BODY(this)->actor.colorFilterTimer);
            }
        }

        this->actor.world.pos.y += Math_SinF(this->unk_1A4) * 4.0f;
        if (this->colliderSph.base.acFlags & AC_HIT) {
            BossVa_KillBari(this, play);
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 0x5DC, 0);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        Math_SmoothStepToS(&this->unk_1F0, 0x78, 1, 0xA, 0);
    }

    if (Rand_ZeroOne() < 0.1f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (GET_BODY(this)->actor.colorFilterTimer == 0) {
        if (!(this->timer2 & 0x400)) {
            this->actor.world.rot.y += this->unk_1AC;
        } else {
            this->actor.world.rot.y -= this->unk_1AC;
        }

        this->actor.world.pos.x = (Math_SinS(this->actor.world.rot.y) * this->unk_1A0) + sp54.x;
        this->actor.world.pos.z = (Math_CosS(this->actor.world.rot.y) * this->unk_1A0) + sp54.z;
    }
}

void BossVa_SetupBariPhase3Stunned(BossVa* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_TARGETABLE;
    this->timer = GET_BODY(this)->timer;
    Actor_SetColorFilter(&this->actor, 0, 255, 0x2000, this->timer);
    BossVa_SetupAction(this, BossVa_BariPhase3Stunned);
}

void BossVa_BariPhase3Stunned(BossVa* this, PlayState* play) {
    s32 sp44_pad;
    Vec3f sp40 = GET_BODY(this)->unk_1D8;

    this->actor.world.rot.x = Math_Vec3f_Pitch(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos);
    if (this->colliderSph.base.acFlags & AC_HIT) {
        BossVa_KillBari(this, play);
        return;
    }

    this->unk_1A4 += Rand_ZeroOne() * 0.5f;
    Math_SmoothStepToF(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.pos.y += Math_SinF(this->unk_1A4) * 3.0f;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        Math_SmoothStepToS(&this->unk_1F0, 0x28, 1, 0xA, 0);
        BossVa_Spark(play, this, 1, this->unk_1F0, 25.0f, 20.0f, SPARK_BARI, 2.0f, true);
    }

    this->timer--;
    this->actor.world.rot.x = Math_Vec3f_Pitch(&sp40, &this->actor.world.pos);
    if (this->timer <= 0) {
        if (this->timer == 0) {
            this->timer2 = 0;
        } else {
            BossVa_Spark(play, this, 1, 85, 15.0f, 0.0f, SPARK_TETHER, 1.0f, true);
            if (this->timer2 >= 0x10) {
                this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                this->timer2 = 0x80;
                BossVa_SetupAction(this, BossVa_BariPhase3Attack);
            }
        }
    }
}

void BossVa_SetupBariDeath(BossVa* this) {
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->timer = 30;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BALINADE_BL_DEAD);
    this->isDead++;
    BossVa_SetupAction(this, BossVa_BariDeath);
}

void BossVa_BariDeath(BossVa* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

void BossVa_SetupDoor(BossVa* this, PlayState* play) {
    if (sCsState >= INTRO_SPAWN_BARI) {
        sDoorState = 100;
    }
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    BossVa_SetupAction(this, BossVa_Door);
}

void BossVa_Door(BossVa* this, PlayState* play) {
    if (sDoorState == 29) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_BUYODOOR_CLOSE);
    }

    if (sCsState <= INTRO_DOOR_SHUT) {
        if (sDoorState < 100) {
            sDoorState += 8;
        } else {
            sDoorState = 100;
        }
    }
}

void BossVa_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossVa* this = (BossVa*)thisx;
    EnBoom* boomerang;
    s32 i;

    this->actionFunc(this, play);

    switch (this->actor.params) {
        case BOSSVA_BODY:
            if (this->colliderBody.base.acFlags & AC_HIT) {
                this->colliderBody.base.acFlags &= ~AC_HIT;
                if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
                    boomerang = (EnBoom*)this->colliderBody.base.ac;
                    boomerang->returnTimer = 0;
                }
            }

            BossVa_UpdateEffects(play);

            for (i = 2; i >= 0; i--) {
                if ((play->envCtx.adjAmbientColor[i] - 1) > 0) {
                    play->envCtx.adjAmbientColor[i] -= 1;
                } else {
                    play->envCtx.adjAmbientColor[i] = 0;
                }

                if ((play->envCtx.adjLight1Color[i] - 10) > 0) {
                    play->envCtx.adjLight1Color[i] -= 10;
                } else {
                    play->envCtx.adjLight1Color[i] = 0;
                }

                if ((play->envCtx.adjFogColor[i] - 10) > 0) {
                    play->envCtx.adjFogColor[i] -= 10;
                } else {
                    play->envCtx.adjFogColor[i] = 0;
                }
            }

            if (this->onCeiling > 0) {
                this->onCeiling--; // not used by body
            }
            break;

        default:
            this->timer2++;
            this->actor.focus.pos = this->actor.world.pos;
            this->actor.focus.pos.y += 45.0f;
            this->unk_1D8.y = (Math_CosS(this->timer2 * 0xFA4) * 0.24f) + 0.76f;
            this->unk_1D8.x = (Math_SinS(this->timer2 * 0xFA4) * 0.2f) + 1.0f;
            break;

        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
        case BOSSVA_DOOR:
            break;
    }
}

s32 BossVa_BodyOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    BossVa* this = (BossVa*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 20) {
        gDPPipeSync(POLY_OPA_DISP++);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 8, 16, 1, 0,
                                    (play->gameplayFrames * -2) % 64, 16, 16));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->bodyGlow);
        Matrix_RotateX(-M_PI / 2, MTXMODE_APPLY);
    } else if ((limbIndex >= 10) && (limbIndex < 20)) {
        rot->x -= 0x4000;
        *dList = NULL;
    } else if (limbIndex == 6) {
        Matrix_Scale(this->unk_1A4, this->unk_1A4, this->unk_1A4, MTXMODE_APPLY);
    } else if (limbIndex == 61) {
        Matrix_Scale(this->unk_1A0, this->unk_1A0, this->unk_1A0, MTXMODE_APPLY);
    } else if (limbIndex == 7) {
        rot->x -= 0xCCC;
    }

    CLOSE_DISPS(play->state.gfxCtx);
    return false;
}

void BossVa_BodyPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp78 = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 6) {
        if (sFightPhase < PHASE_3) {
            sp78.x = -1000.0f;
        } else {
            sp78.x = 200.0f;
        }
        Matrix_MultVec3f(&sp78, &this->unk_1D8);
    } else if ((limbIndex >= 10) && (limbIndex < 20) && (sBodyBari[limbIndex - 10] != 0)) {
        if (((limbIndex >= 16) || (limbIndex == 10)) && (sFightPhase <= PHASE_3)) {
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008BB8);
        } else if ((limbIndex >= 11) && (sFightPhase <= PHASE_2)) {
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008BB8);
        }

        if (sCsState >= DEATH_START) {
            sp78.x = Rand_CenteredFloat(530.0f);
            sp78.y = Rand_CenteredFloat(530.0f);
            sp78.z = -60.0f;
        }
        Matrix_MultVec3f(&sp78, &this->effectPos[limbIndex - 10]);
    } else if (limbIndex == 25) {
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (play->gameplayFrames * 10) % 128, 16, 32, 1, 0,
                                    (play->gameplayFrames * 5) % 128, 16, 32));
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008D70);
    } else if ((*dList != NULL) && (limbIndex >= 29) && (limbIndex < 56)) {
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, *dList);
    } else if ((limbIndex == 24) && (sCsState < DEATH_START)) {
        sp78.x = (this->actor.shape.yOffset + 450.0f) + -140.0f;
        Matrix_MultVec3f(&sp78, &this->unk_280);
        sp78.x = 200.0f;
        Matrix_MultVec3f(&sp78, &this->unk_274);
    }

    if ((limbIndex == 7) && (sCsState >= DEATH_START)) {
        sp78.x = Rand_CenteredFloat(320.0f) + -250.0f;
        sp78.y = Rand_CenteredFloat(320.0f);
        sp78.z = Rand_CenteredFloat(320.0f);

        if (sp78.y < 0.0f) {
            sp78.y -= 150.0f;
        } else {
            sp78.y += 150.0f;
        }

        if (sp78.z < 0.0f) {
            sp78.z -= 150.0f;
        } else {
            sp78.z += 150.0f;
        }
        Matrix_MultVec3f(&sp78, &this->unk_274);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 BossVa_SupportOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void* thisx) {
    BossVa* this = (BossVa*)thisx;

    if (!this->onCeiling && (limbIndex == 4)) {
        rot->z += this->headRot.x;
    }
    return false;
}

void BossVa_SupportPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp20 = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    if (this->onCeiling) {
        switch (limbIndex) {
            case 4:
                Matrix_MultVec3f(&sZeroVec, &this->actor.focus.pos);
                Collider_UpdateSpheres(0, &this->colliderSph);
                break;
            case 7:
                Matrix_MultVec3f(&sZeroVec, &this->armTip);
                sp20.x = ((this->timer & 0x1F) >> 1) * -40.0f;
                sp20.y = ((this->timer & 0x1F) >> 1) * -7.0f;
                Matrix_MultVec3f(&sp20, &this->effectPos[0]);
                break;
            case 9:
                sp20.x = ((this->timer & 0x1F) >> 1) * -60.0f;
                sp20.y = ((this->timer & 0x1F) >> 1) * -45.0f;
                Matrix_MultVec3f(&sp20, &this->effectPos[1]);
                break;
        }
    } else {
        switch (limbIndex) {
            case 5:
                Matrix_MultVec3f(&sZeroVec, &this->armTip);
                break;
            case 8:
                sp20.x = (this->timer2 & 7) * 90.0f;
                Matrix_MultVec3f(&sp20, &this->effectPos[2]);
                break;
            case 9:
                sp20.x = (this->timer2 & 7) * 50.0f;
                Matrix_MultVec3f(&sp20, &this->effectPos[1]);
                break;
            case 10:
                sp20.x = (this->timer2 & 7) * 46.0f;
                Matrix_MultVec3f(&sp20, &this->effectPos[0]);
                break;
        }
    }
}

s32 BossVa_ZapperOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    BossVa* this = (BossVa*)thisx;
    MtxF zapperMtx;

    switch (limbIndex) {
        case 4:
            rot->y += this->unk_1E6;
            rot->z += this->unk_1E4;
            break;
        case 5:
            rot->y += this->unk_1EC;
            rot->z += this->unk_1EA;
            break;
        case 7:
            Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
            Matrix_Get(&zapperMtx);
            Matrix_MtxFToZYXRotS(&zapperMtx, &sZapperRot, false);
            Matrix_RotateX(-sZapperRot.x * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateY(-sZapperRot.y * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateZ(-sZapperRot.z * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateY(this->unk_1F2 * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateZ(this->unk_1F0 * (M_PI / 0x8000), MTXMODE_APPLY);
            pos->x = pos->y = pos->z = 0.0f;
            rot->x = rot->y = rot->z = 0;
            break;
    }
    return false;
}

void BossVa_ZapperPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp70 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp64 = { 15.0f, 0.0f, 0.0f };
    Vec3f sp58 = { -15.0f, 0.0f, 0.0f };
    Vec3f sp4C = { 15.0f, 0.0f, 0.0f };
    Vec3f sp40 = { -15.0f, 0.0f, 0.0f };
    s16 sp3E;
    s16 sp3C;

    switch (limbIndex) {
        case 3:
            sp70.x = (this->timer2 & 7) * 30.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[0]);
            break;

        case 4:
            Matrix_MultVec3f(&sZeroVec, &this->armTip);
            sp70.x = (this->timer2 & 7) * 30.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[1]);
            break;

        case 5:
            Matrix_MultVec3f(&sZeroVec, &this->zapNeckPos);
            sp70.x = (this->timer2 & 7) * 46.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[2]);
            break;

        case 7:
            Matrix_MultVec3f(&sZeroVec, &this->zapHeadPos);
            sp70.x = (this->timer2 & 7) * 46.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[3]);
            sp70.x = 20.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[9]);
            func_80035844(&this->effectPos[9], &this->unk_1D8, &this->headRot, false);
            sp3E = this->headRot.x;
            sp3C = this->headRot.y;
            Matrix_Push();
            Matrix_Translate(this->effectPos[9].x, this->effectPos[9].y, this->effectPos[9].z, MTXMODE_NEW);
            Matrix_RotateZYX(sp3E, sp3C, 0, MTXMODE_APPLY);
            sp70.x = 0.0f;
            if (sFightPhase >= PHASE_4) {
                sp70.z = ((this->timer2 - 16) & 7) * 120.0f;
            } else {
                sp70.z = ((this->timer2 - 32) & 0xF) * 80.0f;
            }
            sp4C.z = sp40.z = sp70.z += 40.0f;
            sp70.z += 50.0f;
            Matrix_MultVec3f(&sp70, &this->effectPos[4]);
            if (sFightPhase >= PHASE_4) {
                sp70.z -= 33.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
                Matrix_MultVec3f(&sp70, &this->effectPos[6]);
                sp70.z -= 33.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
            } else {
                sp70.z -= 22.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
                Matrix_MultVec3f(&sp70, &this->effectPos[6]);
                sp70.z -= 22.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
            }
            Matrix_MultVec3f(&sp70, &this->effectPos[5]);
            Matrix_MultVec3f(&sp64, &this->colliderLightning.dim.quad[1]);
            Matrix_MultVec3f(&sp58, &this->colliderLightning.dim.quad[0]);
            Matrix_MultVec3f(&sp4C, &this->colliderLightning.dim.quad[3]);
            Matrix_MultVec3f(&sp40, &this->colliderLightning.dim.quad[2]);
            Collider_SetQuadVertices(&this->colliderLightning, &this->colliderLightning.dim.quad[0],
                                     &this->colliderLightning.dim.quad[1], &this->colliderLightning.dim.quad[2],
                                     &this->colliderLightning.dim.quad[3]);
            Matrix_Pop();
            break;
    }
}

s32 BossVa_BariOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                void* thisx) {
    BossVa* this = (BossVa*)thisx;

    switch (limbIndex) {
        case 2:
            *dList = NULL;
            break;
        case 3:
            Matrix_Scale(this->unk_1D8.x, 1.0f, this->unk_1D8.x, MTXMODE_APPLY);
            break;
        case 4:
            Matrix_Scale(1.0f, this->unk_1D8.y, 1.0f, MTXMODE_APPLY);
            break;
    }
    return false;
}

void BossVa_BariPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (play->gameplayFrames * 10) % 32, 16, 32, 1, 0,
                                    (play->gameplayFrames * -5) % 32, 16, 32));
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_000FA0);
    } else if ((limbIndex == 3) || (limbIndex == 4)) {
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, *dList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void BossVa_Draw(Actor* thisx, PlayState* play) {
    s16* paramsPtr; // This stack slot is almost certainly actually play2, but can't make it match
    BossVa* this = (BossVa*)thisx;
    Vec3f spBC;
    Vec3f spB0 = { 0.0f, 45.0f, 0.0f };
    Vec3f spA4 = { 0.4f, 0.4f, 0.4f };
    Vec3f sp98 = { 15.0f, 40.0f, 0.0f };
    Vec3f sp8C = { -15.0f, 40.0f, 0.0f };
    Vec3f sp80 = { 15.0f, 40.0f, 0.0f };
    Vec3f sp74 = { -15.0f, 40.0f, 0.0f };
    Color_RGBA8 unused = { 250, 250, 230, 200 };

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    paramsPtr = &this->actor.params;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    switch (this->actor.params) {
        case BOSSVA_BODY:
            if (play->envCtx.adjFogNear != 0) {
                play->envCtx.adjFogNear += 0x15E;
                if (play->envCtx.adjFogNear > 0) {
                    play->envCtx.adjFogNear = 0;
                }
            }

            if (play->envCtx.adjFogFar != 0) {
                play->envCtx.adjFogFar += 0x15E;
                if (play->envCtx.adjFogFar > 0) {
                    play->envCtx.adjFogFar = 0;
                }
            }

            if (!this->isDead) {
                gSPSegment(POLY_OPA_DISP++, 0x08,
                           Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 8, 16, 1, 0,
                                            (play->gameplayFrames * -10) % 16, 16, 16));
                gSPSegment(POLY_OPA_DISP++, 0x09,
                           Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (play->gameplayFrames * -10) % 32, 16,
                                            0x20, 1, 0, (play->gameplayFrames * -5) % 32, 16, 32));
                SkelAnime_DrawSkeletonOpa(play, &this->skelAnime,
                                  BossVa_BodyOverrideLimbDraw, BossVa_BodyPostLimbDraw, this);
            }
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            if (!this->isDead) {
                SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, BossVa_SupportOverrideLimbDraw,
                                      BossVa_SupportPostLimbDraw, this);
            }
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            if (!this->isDead) {
                SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, BossVa_ZapperOverrideLimbDraw,
                                      BossVa_ZapperPostLimbDraw, this);
            }
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, NULL, NULL);
            break;
        default:
            if (!this->isDead) {
                SkelAnime_DrawSkeletonOpa(play, &this->skelAnime,
                                  BossVa_BariOverrideLimbDraw, BossVa_BariPostLimbDraw, this);
                Collider_UpdateSpheres(0, &this->colliderSph);
                if (sCsState < BOSSVA_BATTLE) {
                    spBC = GET_BODY(this)->actor.world.pos;
                } else {
                    spBC = GET_BODY(this)->unk_1D8;
                }
                Matrix_MultVec3f(&sZeroVec, &this->effectPos[1]);
                Matrix_Push();
                Matrix_Translate(spBC.x, spBC.y, spBC.z, MTXMODE_NEW);
                Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
                sp80.z = sp74.z = this->unk_1A0;
                spB0.z = (this->timer2 & 0xF) * (this->unk_1A0 * 0.0625f);
                Matrix_MultVec3f(&spB0, &this->effectPos[0]);
                Matrix_MultVec3f(&sp98, &this->colliderLightning.dim.quad[1]);
                Matrix_MultVec3f(&sp8C, &this->colliderLightning.dim.quad[0]);
                Matrix_MultVec3f(&sp80, &this->colliderLightning.dim.quad[3]);
                Matrix_MultVec3f(&sp74, &this->colliderLightning.dim.quad[2]);
                Collider_SetQuadVertices(&this->colliderLightning, &this->colliderLightning.dim.quad[0],
                                         &this->colliderLightning.dim.quad[1], &this->colliderLightning.dim.quad[2],
                                         &this->colliderLightning.dim.quad[3]);
                Matrix_Pop();
                spBC = this->actor.world.pos;
                spBC.y += 9.0f;
                if (this->actor.colorFilterTimer != 0) {
                    func_80026A6C(play);
                }
                func_80033C30(&spBC, &spA4, 0xFF, play);
                if (this->actor.colorFilterTimer != 0) {
                    Color_RGBA8 blue = { 0, 0, 255, 255 };

                    func_80026860(play, &blue, this->actor.colorFilterTimer, this->actor.colorFilterParams & 0xFF);
                }
            }
            break;
        case BOSSVA_DOOR:
            break;
    }

    if (*paramsPtr == BOSSVA_BODY) {
        BossVa_DrawEffects(sVaEffects, play);
    } else if (*paramsPtr == BOSSVA_DOOR) {
        BossVa_DrawDoor(play, sDoorState);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static s32 sUnkValue = 0x009B0000; // Unreferenced? Possibly a color

void BossVa_UpdateEffects(PlayState* play) {
    BossVaEffect* effect = sVaEffects;
    Player* player = GET_PLAYER(play);
    s16 spB6;
    s16 i;
    f32 floorY;
    s32 padAC;
    s16 pitch;
    s16 yaw;
    BossVa* refActor2;
    BossVa* refActor;
    Vec3f sp94;
    CollisionPoly* sp90;
    f32 pad8C;
    Vec3f sp80;
    CollisionPoly* sp7C;
    f32 pad78;
    f32 pad74;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type != VA_NONE) {
            effect->timer--;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if ((effect->type == VA_LARGE_SPARK) || (effect->type == VA_SMALL_SPARK)) {
                refActor = effect->parent;

                effect->rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x2000;
                effect->rot.y += (s16)(Rand_ZeroOne() * 0x2710) + 0x2000;

                if ((effect->mode == SPARK_TETHER) || (effect->mode == SPARK_UNUSED)) {
                    pitch = effect->rot.x - Math_Vec3f_Pitch(&refActor->actor.world.pos, &GET_BODY(refActor)->unk_1D8);
                    pad8C = Math_SinS(refActor->actor.world.rot.y);
                    effect->pos.x = refActor->actor.world.pos.x - (effect->offset.x * pad8C);
                    pad74 = Math_CosS(refActor->actor.world.rot.y);
                    effect->pos.z = refActor->actor.world.pos.z - (effect->offset.x * pad74);
                    pad78 = Math_CosS(-pitch);
                    effect->pos.y = (effect->offset.y * pad78) + refActor->actor.world.pos.y;
                } else if ((effect->mode == SPARK_BARI) || (effect->mode == SPARK_BODY)) {
                    effect->pos.x = effect->offset.x + refActor->actor.world.pos.x;
                    effect->pos.y = effect->offset.y + refActor->actor.world.pos.y;
                    effect->pos.z = effect->offset.z + refActor->actor.world.pos.z;
                } else {
                    spB6 = Rand_ZeroFloat(ARRAY_COUNT(player->bodyPartsPos) - 0.1f);
                    effect->pos.x = player->bodyPartsPos[spB6].x + Rand_CenteredFloat(10.0f);
                    effect->pos.y = player->bodyPartsPos[spB6].y + Rand_CenteredFloat(15.0f);
                    effect->pos.z = player->bodyPartsPos[spB6].z + Rand_CenteredFloat(10.0f);
                }

                if (effect->timer < 100) {
                    effect->primColor[3] -= 50;
                    if (effect->primColor[3] < 0) {
                        effect->primColor[3] = 0;
                        effect->timer = 0;
                        effect->type = VA_NONE;
                    }
                }
            }

            if (effect->type == VA_BLAST_SPARK) {
                effect->rot.z += (s16)(Rand_ZeroOne() * 0x4E20) + 0x4000;
                if (effect->timer < 100) {
                    effect->primColor[3] -= 50;
                    if (effect->primColor[3] < 0) {
                        effect->primColor[3] = 0;
                        effect->timer = 0;
                        effect->type = VA_NONE;
                    }
                }
            }

            if (effect->type == VA_SPARK_BALL) {
                refActor2 = effect->parent;

                effect->rot.z += (s16)(Rand_ZeroOne() * 0x2710) + 0x24A8;
                effect->pos.x = effect->offset.x + refActor2->actor.world.pos.x;
                effect->pos.y =
                    refActor2->actor.world.pos.y + 310.0f + (refActor2->actor.shape.yOffset * refActor2->actor.scale.y);
                effect->pos.z = effect->offset.z + refActor2->actor.world.pos.z;
                effect->mode = (effect->mode + 1) & 7;

                if (effect->timer < 100) {
                    effect->primColor[3] -= 50;
                    if (effect->primColor[3] < 0) {
                        effect->primColor[3] = 0;
                        effect->timer = 0;
                        effect->type = VA_NONE;
                    }
                }
            }

            if (effect->type == VA_ZAP_CHARGE) {
                effect->mode = (effect->mode + 1) & 7;
                effect->primColor[3] -= 20;
                if (effect->primColor[3] <= 0) {
                    effect->primColor[3] = 0;
                    effect->timer = 0;
                    effect->type = VA_NONE;
                }
            }

            if (effect->type == VA_BLOOD) {
                if (effect->mode < BLOOD_SPOT) {
                    sp94 = effect->pos;
                    sp94.y -= effect->velocity.y + 4.0f;
                    floorY = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp90, &sp94);
                    if ((sp90 != NULL) && (effect->pos.y <= floorY)) {
                        effect->mode = BLOOD_SPOT;
                        effect->pos.y = floorY + 1.0f;
                        if (sCsState <= DEATH_SHELL_BURST) {
                            effect->timer = 80;
                        } else {
                            effect->timer = 60000;
                        }

                        effect->accel = effect->velocity = sZeroVec;
                    }
                    if (!effect->timer) {
                        effect->type = VA_NONE;
                    }
                } else {
                    if (effect->timer < 20) {
                        effect->envColor[3] = effect->timer * 5;
                        effect->primColor[3] = effect->timer * 10;
                    } else if (effect->timer > 50000) {
                        effect->timer++;
                    }
                }

                if (!effect->timer) {
                    effect->type = VA_NONE;
                }
            }

            if (effect->type == VA_GORE) {
                if (effect->mode == GORE_PERMANENT) {
                    sp80 = effect->pos;
                    sp80.y -= effect->velocity.y + 4.0f;
                    effect->rot.x += 0x1770;
                    floorY = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp7C, &sp80);
                    if ((sp7C != NULL) && (effect->pos.y <= floorY)) {
                        effect->mode = GORE_FLOOR;
                        effect->timer = 30;
                        effect->pos.y = floorY + 1.0f;
                        effect->accel = effect->velocity = sZeroVec;
                        effect->rot.x = -0x4000;
                    }

                    if (!effect->timer) {
                        effect->type = VA_NONE;
                    }

                } else if (effect->mode == GORE_FADING) {
                    if (effect->timer == 0) {
                        effect->type = VA_NONE;
                    }

                } else {
                    Math_SmoothStepToF(&effect->scaleMod, 0.075f, 1.0f, 0.005f, 0.0f);
                    Math_SmoothStepToF(&effect->vaGorePulseRate, 0.0f, 0.6f, 0.005f, 0.0013f);
                    if ((play->gameplayFrames % 4) == 0) {
                        Math_SmoothStepToS(&effect->primColor[0], 95, 1, 1, 0);
                    }
                }
                effect->vaGorePulse += effect->vaGorePulseRate;
            }

            if (effect->type == VA_TUMOR) {
                refActor = effect->parent;

                effect->rot.z += 0x157C;
                effect->envColor[3] = (s16)(Math_SinS(effect->rot.z) * 50.0f) + 80;
                Math_SmoothStepToF(&effect->scale, effect->scaleMod, 1.0f, 0.01f, 0.005f);
                effect->pos.x = effect->offset.x + refActor->actor.world.pos.x;
                effect->pos.y = effect->offset.y + refActor->actor.world.pos.y;
                effect->pos.z = effect->offset.z + refActor->actor.world.pos.z;

                switch (effect->mode) {
                    case TUMOR_UNUSED:
                        if (effect->timer == 0) {
                            yaw = Math_Vec3f_Yaw(&refActor->actor.world.pos, &effect->pos);
                            effect->type = VA_NONE;
                            BossVa_BloodSplatter(play, effect, yaw, effect->scale * 4500.0f, 1);
                            BossVa_Gore(play, effect, yaw, effect->scale * 1.2f);
                        }
                        break;
                    case TUMOR_BODY:
                    case TUMOR_ARM:
                        if (refActor->burst) {
                            effect->type = VA_NONE;
                            yaw = Math_Vec3f_Yaw(&refActor->actor.world.pos, &effect->pos);
                            BossVa_BloodSplatter(play, effect, yaw, effect->scale * 4500.0f, 1);
                            BossVa_Gore(play, effect, yaw, effect->scale * 1.2f);
                        }
                        break;
                }
            }
        }
    }
}

void BossVa_DrawEffects(BossVaEffect* effect, PlayState* play) {
    static void* sSparkBallTex[] = {
        gBarinadeSparkBall1Tex, gBarinadeSparkBall2Tex, gBarinadeSparkBall3Tex, gBarinadeSparkBall4Tex,
        gBarinadeSparkBall5Tex, gBarinadeSparkBall6Tex, gBarinadeSparkBall7Tex, gBarinadeSparkBall8Tex,
    };
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 flag = 0;
    BossVaEffect* effectHead = effect;
    Camera* camera = Play_GetCamera(play, sCsCamera);

    OPEN_DISPS(gfxCtx);

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_LARGE_SPARK) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 130, 130, 30, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0156A0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 230, 230, 230, effect->primColor[3]);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_Scale(effect->scale * 0.0185f, effect->scale * 0.0185f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_015710);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_SPARK_BALL) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_011738);
                flag++;
            }
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sSparkBallTex[effect->mode]));
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->primColor[0], effect->primColor[1], effect->primColor[2],
                            effect->primColor[3]);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, effect->envColor[0], effect->envColor[1], effect->envColor[2],
                           effect->envColor[3]);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_011768);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_BLOOD) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_009430);
                gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gEffBubble1Tex));
                flag++;
            }

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 0, effect->envColor[3]);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 150, 0, effect->primColor[3]);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            if (effect->mode == BLOOD_SPOT) {
                Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
            } else {
                Matrix_ReplaceRotation(&play->billboardMtxF);
            }

            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gDPPipeSync(POLY_XLU_DISP++);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_009468);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_TUMOR) {
            BossVa* parent = effect->parent;

            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Opa(play->state.gfxCtx);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, effect->envColor[3]);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_0128B8);
                flag++;
            }

            if ((effect->mode != TUMOR_BODY) || ((Math_Vec3f_DistXZ(&camera->eye, &effect->pos) -
                                                  Math_Vec3f_DistXZ(&camera->eye, &parent->actor.world.pos)) < 10.0f)) {
                Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012948);
            }
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_GORE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Opa(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012BA0);
                flag++;
            }

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, effect->primColor[3]);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, effect->primColor[0], effect->primColor[1], effect->primColor[2],
                            effect->primColor[3]);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateZYX(effect->rot.x, effect->rot.y, 0, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateX(effect->offset.x * 0.115f, MTXMODE_APPLY);
            Matrix_RotateY(effect->offset.x * 0.13f, MTXMODE_APPLY);
            Matrix_RotateZ(effect->offset.x * 0.1f, MTXMODE_APPLY);
            Matrix_Scale(1.0f - effect->scaleMod, effect->scaleMod + 1.0f, 1.0f - effect->scaleMod, MTXMODE_APPLY);
            Matrix_RotateZ(-(effect->offset.x * 0.1f), MTXMODE_APPLY);
            Matrix_RotateY(-(effect->offset.x * 0.13f), MTXMODE_APPLY);
            Matrix_RotateX(-(effect->offset.x * 0.115f), MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012C50);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_ZAP_CHARGE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0135B0);
                flag++;
            }

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 50, effect->primColor[3]);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->primColor[3]);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateZYX(effect->rot.x, effect->rot.y, 0, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_013638);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_BLAST_SPARK) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu2(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 130, 130, 30, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0156A0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 230, 230, 230, effect->primColor[3]);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_Scale(effect->scale * 0.02f, effect->scale * 0.02f, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_015710);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = effectHead;
    for (i = 0, flag = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_SMALL_SPARK) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (!flag) {
                Gfx_SetupDL_25Xlu(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008F08);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->primColor[3]);
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_RotateY((effect->rot.y / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008F70);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void BossVa_SpawnSpark(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale,
                       u8 mode) {
    Player* player = GET_PLAYER(play);
    s16 index;
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    Vec3f tempVec;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_LARGE_SPARK;
            effect->parent = this;
            effect->pos = pos;
            effect->timer = (s16)(Rand_ZeroOne() * 10.0f) + 111;
            effect->velocity = effect->accel = sZeroVec;
            effect->mode = mode;
            effect->epoch++;

            switch (mode) {
                case SPARK_UNUSED:
                    effect->type = VA_SMALL_SPARK;
                case SPARK_TETHER:
                    tempVec = *offset;
                    tempVec.x += this->actor.world.pos.x;
                    tempVec.z += this->actor.world.pos.z;
                    effect->offset.x = Math_Vec3f_DistXZ(&this->actor.world.pos, &tempVec);
                    effect->rot.x = Math_Vec3f_Pitch(&this->actor.world.pos, &GET_BODY(this)->unk_1D8);
                    break;

                case SPARK_BODY:
                    effect->type = VA_SMALL_SPARK;
                case SPARK_BARI:
                    effect->offset.x = offset->x;
                    effect->offset.z = offset->z;
                    break;

                case SPARK_BLAST:
                    effect->type = VA_BLAST_SPARK;
                    effect->pos.x = offset->x + this->actor.world.pos.x;
                    effect->pos.y = offset->y + this->actor.world.pos.y;
                    effect->pos.z = offset->z + this->actor.world.pos.z;
                    effect->timer = 111;
                    break;

                case SPARK_LINK:
                    effect->type = VA_SMALL_SPARK;
                    index = Rand_ZeroFloat(ARRAY_COUNT(player->bodyPartsPos) - 0.1f);
                    effect->pos.x = player->bodyPartsPos[index].x + Rand_CenteredFloat(10.0f);
                    effect->pos.y = player->bodyPartsPos[index].y + Rand_CenteredFloat(15.0f);
                    effect->pos.z = player->bodyPartsPos[index].z + Rand_CenteredFloat(10.0f);
                    break;
            }

            effect->offset.y = offset->y;
            effect->scale = (Rand_ZeroFloat(scale) + scale) * 0.01f;
            effect->primColor[3] = 255;
            break;
        }
    }
}

void BossVa_SpawnSparkBall(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale,
                           u8 mode) {
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_SPARK_BALL;
            effect->parent = this;

            effect->pos = pos;

            effect->velocity = effect->accel = sZeroVec;

            effect->mode = 0;
            effect->offset.x = offset->x;
            effect->offset.z = offset->z;
            effect->offset.y = offset->y;
            effect->timer = (s16)(Rand_ZeroOne() * 10.0f) + 111;
            effect->primColor[0] = effect->primColor[1] = effect->primColor[2] = effect->primColor[3] = 230;
            effect->envColor[0] = 0;
            effect->envColor[1] = 100;
            effect->envColor[2] = 220;
            effect->envColor[3] = 160;

            effect->scale = (Rand_ZeroFloat(scale) + scale) * 0.01f;
            return;
        }
    }
}

void BossVa_SpawnBloodDroplets(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 scale, s16 phase,
                               s16 yaw) {
    s32 i;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 xzVel;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_BLOOD;
            effect->pos = *pos;
            effect->mode = BLOOD_DROPLET;

            xzVel = Math_SinS(phase) * 6.0f;
            velocity.x = Rand_CenteredFloat(1.0f) + (-Math_SinS(yaw) * xzVel);
            velocity.z = Rand_CenteredFloat(1.0f) + (-Math_CosS(yaw) * xzVel);

            effect->velocity = velocity;

            accel.y = Rand_CenteredFloat(0.3f) - 1.0f;
            effect->accel = accel;

            effect->timer = 20;
            effect->envColor[3] = 100;
            effect->primColor[3] = 200;
            effect->scale = (Rand_ZeroFloat(scale) + scale) * 0.01f;
            break;
        }
    }
}

void BossVa_SpawnBloodSplatter(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale) {
    s32 i;
    f32 xzVel;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_BLOOD;
            effect->pos = *pos;

            effect->mode = BLOOD_SPLATTER;

            xzVel = Rand_ZeroOne() * 7.0f;
            velocity.x = Math_SinS(yaw) * xzVel;
            velocity.y = Rand_CenteredFloat(4.0f) + 4.0f;
            velocity.z = Math_CosS(yaw) * xzVel;
            effect->velocity = velocity;

            accel.y = Rand_CenteredFloat(0.3f) - 1.0f;
            effect->accel = accel;

            if (sCsState <= DEATH_SHELL_BURST) {
                effect->timer = 20;
            } else {
                effect->timer = 60;
            }
            effect->envColor[3] = 100;
            effect->primColor[3] = 200;
            effect->scale = scale * 0.01f;
            break;
        }
    }
}

void BossVa_SpawnTumor(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale,
                       u8 mode) {
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_TUMOR;
            effect->parent = this;
            effect->pos = pos;

            effect->velocity = effect->accel = sZeroVec;

            effect->mode = mode;
            effect->rot.z = 0;

            effect->offset.x = offset->x;
            effect->offset.z = offset->z;
            effect->offset.y = offset->y;

            effect->timer = (s16)(Rand_ZeroOne() * 10.0f) + 10;
            effect->envColor[3] = 100;
            effect->scaleMod = scale * 0.01f;
            effect->scale = 0.0f;

            if (((i % 4) == 0) || (mode == 2)) {
                Audio_PlaySoundGeneral(NA_SE_EN_BALINADE_BREAK, &effect->pos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            break;
        }
    }
}

void BossVa_SpawnGore(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale) {
    s32 i;
    f32 xzVel;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_GORE;
            effect->pos = *pos;
            effect->scaleMod = 0.0f;

            xzVel = (Rand_ZeroOne() * 4.0f) + 4.0f;
            velocity.x = Math_SinS(yaw) * xzVel;
            velocity.y = Rand_CenteredFloat(8.0f);
            velocity.z = Math_CosS(yaw) * xzVel;
            effect->velocity = velocity;

            accel.y = Rand_CenteredFloat(0.3f) - 1.0f;
            effect->accel = accel;

            effect->timer = 20;
            if (sCsState <= DEATH_SHELL_BURST) {
                effect->mode = GORE_FADING;
            } else {
                effect->mode = GORE_PERMANENT;
            }

            effect->primColor[3] = effect->envColor[0] = effect->envColor[1] = effect->envColor[2] =
                effect->envColor[3] = 255;

            effect->primColor[0] = 155;
            effect->primColor[1] = effect->primColor[2] = 55;

            effect->rot.x = Rand_CenteredFloat(0x10000);
            effect->rot.y = Rand_CenteredFloat(0x10000);
            effect->scale = (Rand_ZeroFloat(scale) + scale) * 0.01f;
            effect->vaGorePulseRate = (Rand_ZeroOne() * 0.25f) + 0.9f;
            break;
        }
    }
}

void BossVa_SpawnZapperCharge(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* pos, Vec3s* rot,
                              s16 scale, u8 mode) {
    Vec3f unused = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < ARRAY_COUNT(sVaEffects); i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_ZAP_CHARGE;
            effect->parent = this;
            effect->pos = *pos;

            effect->velocity = effect->accel = sZeroVec;

            effect->mode = mode;
            effect->rot.x = rot->x + 0x4000;
            effect->rot.y = rot->y;
            effect->timer = (s16)(Rand_ZeroOne() * 10.0f) + 10;
            effect->primColor[3] = 240;
            effect->scale = scale * 0.01f;
            break;
        }
    }
}

void BossVa_DrawDoor(PlayState* play, s16 scale) {
    static Gfx* doorPieceDispList[] = {
        gBarinadeDoorPiece1DL, gBarinadeDoorPiece2DL, gBarinadeDoorPiece3DL, gBarinadeDoorPiece4DL,
        gBarinadeDoorPiece5DL, gBarinadeDoorPiece6DL, gBarinadeDoorPiece7DL, gBarinadeDoorPiece8DL,
    };
    static s16 doorPieceLength[] = { 836, 900, 836, 1016, 800, 1016, 836, 900 };
    MtxF doorMtx;
    f32 yScale;
    f32 segAngle = 0.0f;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(0.0f, 80.0f, 400.0f, MTXMODE_NEW);
    Matrix_RotateY(M_PI, MTXMODE_APPLY);
    yScale = (scale * 0.01f) * 0.1f;
    Matrix_Scale(0.1f, yScale, 0.1f, MTXMODE_APPLY);

    if (yScale != 0.0f) {
        yScale = 0.1f / yScale;
    } else {
        yScale = 0.0f;
    }

    Matrix_Get(&doorMtx);

    for (i = 0; i < 8; i++, segAngle -= M_PI / 4) {
        FrameInterpolation_RecordOpenChild("Barinade Door", i);

        Matrix_Put(&doorMtx);
        Matrix_RotateZ(segAngle, MTXMODE_APPLY);
        Matrix_Translate(0.0f, doorPieceLength[i] * yScale, 0.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, doorPieceDispList[i]);

        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void BossVa_Reset(void) {
    sKillBari = 0;
    sCsCamera = 0;
    memset(sVaEffects, 0, sizeof(sVaEffects));
    sBodyState = 0;
    sFightPhase = 0;
    sCsState = 0;
    sDoorState = 0;
    sPhase3StopMoving = 0;
    sZapperRot.x = 0;
    sZapperRot.y = 0;
    sZapperRot.z = 0;    
    sPhase2Timer = 0;
    sPhase4HP = 0;
    for (u8 i = 0; i < ARRAY_SIZE(sBodyBari); i++) {
        sBodyBari[i] = 0;
    }
}
