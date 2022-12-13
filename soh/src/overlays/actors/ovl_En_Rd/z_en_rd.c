#include "z_en_rd.h"
#include "objects/object_rd/object_rd.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_10)

void EnRd_Init(Actor* thisx, PlayState* play);
void EnRd_Destroy(Actor* thisx, PlayState* play);
void EnRd_Update(Actor* thisx, PlayState* play);
void EnRd_Draw(Actor* thisx, PlayState* play);

void func_80AE269C(EnRd* this);
void func_80AE2744(EnRd* this, PlayState* play);
void func_80AE2970(EnRd* this);
void func_80AE2A10(EnRd* this, PlayState* play);
void func_80AE2C1C(EnRd* this, PlayState* play);
void func_80AE2F50(EnRd* this, PlayState* play);
void func_80AE2FD0(EnRd* this, PlayState* play);
void func_80AE31DC(EnRd* this);
void func_80AE3260(EnRd* this, PlayState* play);
void func_80AE33F0(EnRd* this);
void func_80AE392C(EnRd* this);
void func_80AE39D4(EnRd* this);
void func_80AE3454(EnRd* this, PlayState* play);
void func_80AE37BC(EnRd* this);
void func_80AE3834(EnRd* this, PlayState* play);
void func_80AE3978(EnRd* this, PlayState* play);
void func_80AE3A54(EnRd* this, PlayState* play);
void func_80AE3B18(EnRd* this, PlayState* play);
void func_80AE3C98(EnRd* this, PlayState* play);
void func_80AE3ECC(EnRd* this, PlayState* play);

const ActorInit En_Rd_InitVars = {
    ACTOR_EN_RD,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_RD,
    sizeof(EnRd),
    (ActorFunc)EnRd_Init,
    (ActorFunc)EnRd_Destroy,
    (ActorFunc)EnRd_Update,
    (ActorFunc)EnRd_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0xE),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(3, 0xD),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

static Vec3f D_80AE4918 = { 0.0f, 0.0f, 0.0f };

// I'm guessing these are primitive and environment colors that go unused
static Color_RGBA8 D_80AE4924 = { 200, 200, 255, 255 };
static Color_RGBA8 D_80AE4928 = { 0, 0, 255, 0 };

static Vec3f D_80AE492C = { 0.0f, 0.0f, 0.0f };
static Color_RGBA8 D_80AE4938 = { 200, 200, 255, 255 };
static Color_RGBA8 D_80AE493C = { 0, 0, 255, 0 };

static Vec3f D_80AE4940 = { 300.0f, 0.0f, 0.0f };
static Vec3f D_80AE494C = { 300.0f, 0.0f, 0.0f };
static Vec3f D_80AE4958 = { 0.25f, 0.25f, 0.25f };

void EnRd_SetupAction(EnRd* this, EnRdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnRd_Init(Actor* thisx, PlayState* play) {
    EnRd* this = (EnRd*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    thisx->targetMode = 0;
    thisx->colChkInfo.damageTable = &sDamageTable;
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    this->unk_310 = this->unk_30E = 0;
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 50.0f;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.health = 8;
    this->unk_314 = this->unk_31D = 0xFF;
    this->unk_312 = (thisx->params & 0xFF00) >> 8;

    if (thisx->params & 0x80) {
        thisx->params |= 0xFF00;
    } else {
        thisx->params &= 0xFF;
    }

    if (thisx->params >= -1) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gRedeadSkel, &gGibdoRedeadIdleAnim,
                           this->jointTable, this->morphTable, 26);
        thisx->naviEnemyId = 0x2A;
    } else {
        SkelAnime_InitFlex(play, &this->skelAnime, &gGibdoSkel, &gGibdoRedeadIdleAnim,
                           this->jointTable, this->morphTable, 26);
        thisx->naviEnemyId = 0x2D;
    }

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);

    if (thisx->params >= -2) {
        func_80AE269C(this);
    } else {
        func_80AE2970(this);
    }

    SkelAnime_Update(&this->skelAnime);

    if (thisx->params == 3) {
        thisx->flags |= ACTOR_FLAG_7;
    }
}

void EnRd_Destroy(Actor* thisx, PlayState* play) {
    EnRd* this = (EnRd*)thisx;

    if (gSaveContext.sunsSongState != SUNSSONG_INACTIVE) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AE2630(PlayState* play, Actor* thisx, s32 arg2) {
    Actor* enemyIt = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;

    while (enemyIt != NULL) {
        if ((enemyIt->id != ACTOR_EN_RD) || (enemyIt == thisx) || (enemyIt->params < 0)) {
            enemyIt = enemyIt->next;
            continue;
        }

        if (arg2 != 0) {
            enemyIt->parent = thisx;
        } else if (enemyIt->parent == thisx) {
            enemyIt->parent = NULL;
        }
        enemyIt = enemyIt->next;
    }
}

void func_80AE269C(EnRd* this) {
    if (this->actor.params != 2) {
        Animation_MorphToLoop(&this->skelAnime, &gGibdoRedeadIdleAnim, -6.0f);
    } else {
        Animation_PlayLoop(&this->skelAnime, &gGibdoRedeadSobbingAnim);
    }

    this->unk_31B = 0;
    this->unk_30C = (Rand_ZeroOne() * 10.0f) + 5.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnRd_SetupAction(this, func_80AE2744);
}

void func_80AE2744(EnRd* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x64, 0);
    Math_SmoothStepToS(&this->unk_310, 0, 1, 0x64, 0);

    if ((this->actor.params == 2) && (0.0f == this->skelAnime.curFrame)) {
        if (Rand_ZeroOne() >= 0.5f) {
            Animation_PlayLoop(&this->skelAnime, &gGibdoRedeadSobbingAnim);
        } else {
            Animation_PlayLoop(&this->skelAnime, &gGibdoRedeadWipingTearsAnim);
        }
    } else {
        this->unk_30C--;
        if (this->unk_30C == 0) {
            this->unk_30C = (Rand_ZeroOne() * 10.0f) + 10.0f;
            this->skelAnime.curFrame = 0.0f;
        }
    }

    if (this->actor.parent != NULL) {
        if (this->unk_305 == 0) {
            if (this->actor.params != 2) {
                func_80AE31DC(this);
            } else {
                func_80AE392C(this);
            }
        }
    } else {
        if (this->unk_305 != 0) {
            if (this->actor.params != 2) {
                func_80AE37BC(this);
            } else {
                func_80AE392C(this);
            }
        }

        this->unk_305 = 0;

        if (this->actor.xzDistToPlayer <= 150.0f && func_8002DDE4(play)) {
            // Add a height check to redeads/gibdos freeze when Enemy Randomizer is on.
            // Without the height check, redeads/gibdos can freeze the player from insane distances in
            // vertical rooms (like the first room in Deku Tree), making these rooms nearly unplayable.
            s8 enemyRando = CVar_GetS32("gRandomizedEnemies", 0);
            if (!enemyRando || (enemyRando && this->actor.yDistToPlayer <= 100.0f && this->actor.yDistToPlayer >= -100.0f)) {
                if ((this->actor.params != 2) && (this->unk_305 == 0)) {
                    func_80AE37BC(this);
                } else {
                    func_80AE392C(this);
                }
            }
        }
    }

    if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void func_80AE2970(EnRd* this) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadIdleAnim, 0, 0, Animation_GetLastFrame(&gGibdoRedeadIdleAnim),
                     ANIMMODE_LOOP, -6.0f);
    this->unk_31B = 11;
    this->unk_30C = 6;
    this->actor.shape.rot.x = -0x4000;
    this->actor.gravity = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.speedXZ = 0.0f;
    EnRd_SetupAction(this, func_80AE2A10);
}

// Rising out of coffin
void func_80AE2A10(EnRd* this, PlayState* play) {
    if (this->actor.shape.rot.x != -0x4000) {
        Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 0x7D0, 0);
        if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 0.3f, 2.0f, 0.3f) == 0.0f) {
            this->actor.gravity = -3.5f;
            func_80AE269C(this);
        }
    } else {
        if (this->actor.world.pos.y == this->actor.home.pos.y) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_CRY);
        }
        if (Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 50.0f, 0.3f, 2.0f, 0.3f) == 0.0f) {
            if (this->unk_30C != 0) {
                this->unk_30C--;
                Math_SmoothStepToF(&this->actor.speedXZ, 6.0f, 0.3f, 1.0f, 0.3f);
            } else if (Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 0.3f, 1.0f, 0.3f) == 0.0f) {
                Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 0x7D0, 0);
            }
        }
    }
}

void func_80AE2B90(EnRd* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadWalkAnim, 1.0f, 4.0f,
                     Animation_GetLastFrame(&gGibdoRedeadWalkAnim), ANIMMODE_LOOP_INTERP, -4.0f);
    this->actor.speedXZ = 0.4f;
    this->unk_31B = 4;
    EnRd_SetupAction(this, func_80AE2C1C);
}

void func_80AE2C1C(EnRd* this, PlayState* play) {
    Vec3f sp44 = D_80AE4918;
    Color_RGBA8 sp40 = D_80AE4924;
    Color_RGBA8 sp3C = D_80AE4928;
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 sp32 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->unk_30E - this->unk_310;

    this->skelAnime.playSpeed = this->actor.speedXZ;
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA, 0);
    Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x64, 0);
    Math_SmoothStepToS(&this->unk_310, 0, 1, 0x64, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);

    if (Actor_WorldDistXYZToPoint(&player->actor, &this->actor.home.pos) >= 150.0f) {
        func_80AE2F50(this, play);
    }

    if ((ABS(sp32) < 0x1554) && (Actor_WorldDistXYZToActor(&this->actor, &player->actor) <= 150.0f)) {
        if (!(player->stateFlags1 & 0x2C6080) && !(player->stateFlags2 & 0x80)) {
            if (this->unk_306 == 0) {
                if (!(this->unk_312 & 0x80)) {
                    player->actor.freezeTimer = 40;
                    func_8008EEAC(play, &this->actor);
                    GET_PLAYER(play)->unk_684 = &this->actor;
                    func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
                }
                this->unk_306 = 0x3C;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_AIM);
            }
        } else {
            func_80AE2F50(this, play);
        }
    }

    if (this->unk_307 != 0) {
        this->unk_307--;
    }

    if (!this->unk_307 && (Actor_WorldDistXYZToActor(&this->actor, &player->actor) <= 45.0f) &&
        Actor_IsFacingPlayer(&this->actor, 0x38E3)) {
        player->actor.freezeTimer = 0;
        if (play->grabPlayer(play, player)) {
            this->actor.flags &= ~ACTOR_FLAG_0;
            func_80AE33F0(this);
        }
    } else if (this->actor.params > 0) {
        if (this->actor.parent != NULL) {
            func_80AE31DC(this);
        } else {
            this->unk_305 = 0;
        }
    }

    if ((this->skelAnime.curFrame == 10.0f) || (this->skelAnime.curFrame == 22.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void func_80AE2F50(EnRd* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadWalkAnim, 0.5f, 0, Animation_GetLastFrame(&gGibdoRedeadWalkAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->unk_31B = 2;
    EnRd_SetupAction(this, func_80AE2FD0);
}

void func_80AE2FD0(EnRd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 targetY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);

    if (Actor_WorldDistXYZToPoint(&this->actor, &this->actor.home.pos) >= 5.0f) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, targetY, 1, 0x1C2, 0);
    } else {
        this->actor.speedXZ = 0.0f;
        if (Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 1, 0x1C2, 0) == 0) {
            if (this->actor.params != 2) {
                func_80AE269C(this);
            } else {
                func_80AE39D4(this);
            }
        }
    }

    Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x64, 0);
    Math_SmoothStepToS(&this->unk_310, 0, 1, 0x64, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);

    if (!(player->stateFlags1 & 0x2C6080) && !(player->stateFlags2 & 0x80) &&
        (Actor_WorldDistXYZToPoint(&player->actor, &this->actor.home.pos) < 150.0f)) {
        this->actor.targetMode = 0;
        func_80AE2B90(this, play);
    } else if (this->actor.params > 0) {
        if (this->actor.parent != NULL) {
            func_80AE31DC(this);
        } else {
            this->unk_305 = 0;
        }
    }

    if (this->skelAnime.curFrame == 10.0f || this->skelAnime.curFrame == 22.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void func_80AE31DC(EnRd* this) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadWalkAnim, 0.5f, 0, Animation_GetLastFrame(&gGibdoRedeadWalkAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->unk_31B = 3;
    this->unk_305 = 1;
    EnRd_SetupAction(this, func_80AE3260);
}

void func_80AE3260(EnRd* this, PlayState* play) {
    if (this->actor.parent != NULL) {
        s32 pad;
        s16 targetY;
        Vec3f thisPos = this->actor.parent->world.pos;

        targetY = Actor_WorldYawTowardPoint(&this->actor, &thisPos);

        Math_SmoothStepToS(&this->actor.shape.rot.y, targetY, 1, 0xFA, 0);

        if (Actor_WorldDistXYZToPoint(&this->actor, &thisPos) >= 45.0f) {
            this->actor.speedXZ = 0.4f;
        } else {
            this->actor.speedXZ = 0.0f;

            if (this->actor.params != 2) {
                func_80AE269C(this);
            } else {
                func_80AE39D4(this);
            }
        }

        Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x64, 0);
        Math_SmoothStepToS(&this->unk_310, 0, 1, 0x64, 0);
    } else {
        func_80AE2B90(this, play);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    SkelAnime_Update(&this->skelAnime);

    if (this->skelAnime.curFrame == 10.0f || this->skelAnime.curFrame == 22.0f) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_WALK);
    } else if ((play->gameplayFrames & 0x5F) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_CRY);
    }
}

void func_80AE33F0(EnRd* this) {
    Animation_PlayOnce(&this->skelAnime, &gGibdoRedeadGrabStartAnim);
    this->unk_30C = this->unk_304 = 0;
    this->unk_319 = 200;
    this->unk_31B = 8;
    this->actor.speedXZ = 0.0f;
    EnRd_SetupAction(this, func_80AE3454);
}

void func_80AE3454(EnRd* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_304++;
    }

    switch (this->unk_304) {
        case 1:
            Animation_PlayLoop(&this->skelAnime, &gGibdoRedeadGrabAttackAnim);
            this->unk_304++;
            play->damagePlayer(play, -8);
            func_800AA000(this->actor.xzDistToPlayer, 0xFF, 1, 0xC);
            this->unk_319 = 20;
        case 0:
            Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x5DC, 0);
            Math_SmoothStepToS(&this->unk_310, 0, 1, 0x5DC, 0);
        case 2:
            if (!(player->stateFlags2 & 0x80)) {
                Animation_Change(&this->skelAnime, &gGibdoRedeadGrabEndAnim, 0.5f, 0.0f,
                                 Animation_GetLastFrame(&gGibdoRedeadGrabEndAnim), ANIMMODE_ONCE_INTERP, 0.0f);
                this->unk_304++;
                this->unk_31B = 4;
                return;
            }

            if (!LINK_IS_ADULT) {
                Math_SmoothStepToF(&this->actor.shape.yOffset, -1500.0f, 1.0f, 150.0f, 0.0f);
            }

            Math_SmoothStepToF(&this->actor.world.pos.x,
                               (Math_SinS(player->actor.shape.rot.y) * -25.0f) + player->actor.world.pos.x, 1.0f, 10.0f,
                               0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.y, player->actor.world.pos.y, 1.0f, 10.0f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.z,
                               (Math_CosS(player->actor.shape.rot.y) * -25.0f) + player->actor.world.pos.z, 1.0f, 10.0f,
                               0.0f);
            Math_SmoothStepToS(&this->actor.shape.rot.y, player->actor.shape.rot.y, 1, 0x1770, 0);

            if (this->skelAnime.curFrame == 0.0f) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_ATTACK);
            }
            this->unk_319--;

            if (this->unk_319 == 0) {
                play->damagePlayer(play, -8);
                func_800AA000(this->actor.xzDistToPlayer, 0xF0, 1, 0xC);
                this->unk_319 = 20;
                func_8002F7DC(&player->actor, NA_SE_VO_LI_DAMAGE_S + player->ageProperties->unk_92);
            }
            break;
        case 3:
            if (!LINK_IS_ADULT) {
                Math_SmoothStepToF(&this->actor.shape.yOffset, 0, 1.0f, 400.0f, 0.0f);
            }
            break;
        case 4:
            if (!LINK_IS_ADULT) {
                Math_SmoothStepToF(&this->actor.shape.yOffset, 0, 1.0f, 400.0f, 0.0f);
            }
            this->actor.targetMode = 0;
            this->actor.flags |= ACTOR_FLAG_0;
            this->unk_306 = 0xA;
            this->unk_307 = 0xF;
            func_80AE2B90(this, play);
            break;
    }
}

void func_80AE37BC(EnRd* this) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadLookBackAnim, 0.0f, 0.0f,
                     Animation_GetLastFrame(&gGibdoRedeadLookBackAnim), ANIMMODE_ONCE, 0.0f);
    this->unk_31B = 7;
    EnRd_SetupAction(this, func_80AE3834);
}

void func_80AE3834(EnRd* this, PlayState* play) {
    Vec3f sp34 = D_80AE492C;
    Color_RGBA8 sp30 = D_80AE4938;
    Color_RGBA8 sp2C = D_80AE493C;
    Player* player = GET_PLAYER(play);
    s16 temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y - this->unk_30E - this->unk_310;

    if (ABS(temp_v0) < 0x2008) {
        if (!(this->unk_312 & 0x80)) {
            player->actor.freezeTimer = 60;
            func_800AA000(this->actor.xzDistToPlayer, 0xFF, 0x14, 0x96);
            func_8008EEAC(play, &this->actor);
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_AIM);
        func_80AE2B90(this, play);
    }
}

void func_80AE392C(EnRd* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGibdoRedeadStandUpAnim, -4.0f);
    this->unk_31B = 5;
    EnRd_SetupAction(this, func_80AE3978);
}

void func_80AE3978(EnRd* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.parent != NULL) {
            func_80AE31DC(this);
        } else {
            func_80AE37BC(this);
        }
    }
}

void func_80AE39D4(EnRd* this) {
    Animation_Change(&this->skelAnime, &gGibdoRedeadStandUpAnim, -1.0f, Animation_GetLastFrame(&gGibdoRedeadStandUpAnim),
                     0.0f, ANIMMODE_ONCE, -4.0f);
    this->unk_31B = 6;
    EnRd_SetupAction(this, func_80AE3A54);
}

void func_80AE3A54(EnRd* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_80AE269C(this);
    }
}

void func_80AE3A8C(EnRd* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGibdoRedeadDamageAnim, -6.0f);

    if (this->actor.bgCheckFlags & 1) {
        this->actor.speedXZ = -2.0f;
    }

    this->actor.flags |= ACTOR_FLAG_0;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_DAMAGE);
    this->unk_31B = 9;
    EnRd_SetupAction(this, func_80AE3B18);
}

void func_80AE3B18(EnRd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.speedXZ < 0.0f) {
        this->actor.speedXZ += 0.15f;
    }

    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x12C, 0);
    Math_SmoothStepToS(&this->unk_310, 0, 1, 0x12C, 0);
    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;

        if (this->actor.parent != NULL) {
            func_80AE31DC(this);
        } else if (Actor_WorldDistXYZToPoint(&player->actor, &this->actor.home.pos) >= 150.0f) {
            func_80AE2F50(this, play);
        } else {
            func_80AE2B90(this, play);
        }

        this->unk_31D = 0xFF;
    }
}

void func_80AE3C20(EnRd* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGibdoRedeadDeathAnim, -1.0f);
    this->unk_31B = 10;
    this->unk_30C = 300;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_REDEAD_DEAD);
    EnRd_SetupAction(this, func_80AE3C98);
    if (this->actor.params >= -1) {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_REDEAD]++;
    } else {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_GIBDO]++;
    }
}

void func_80AE3C98(EnRd* this, PlayState* play) {
    if (this->actor.category != ACTORCAT_PROP) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    }

    Math_SmoothStepToS(&this->unk_30E, 0, 1, 0x7D0, 0);
    Math_SmoothStepToS(&this->unk_310, 0, 1, 0x7D0, 0);

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_30C == 0) {
            if (!Flags_GetSwitch(play, this->unk_312 & 0x7F)) {
                Flags_SetSwitch(play, this->unk_312 & 0x7F);
            }
            if (this->unk_314 != 0) {
                if (this->unk_314 == 0xB4) {
                    func_80AE2630(play, &this->actor, 0);
                }
                this->actor.scale.y -= 0.000075f;
                this->unk_314 -= 5;
            } else {
                Actor_Kill(&this->actor);
            }
        } else {
            this->unk_30C--;
        }
    } else if (((s32)this->skelAnime.curFrame == 33) || ((s32)this->skelAnime.curFrame == 40)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
}

void func_80AE3DE4(EnRd* this) {
    this->unk_31B = 1;
    this->actor.speedXZ = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (gSaveContext.sunsSongState != SUNSSONG_INACTIVE) {
        this->unk_318 = 1;
        this->unk_316 = 0x258;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIGHT_ARROW_HIT);
        Actor_SetColorFilter(&this->actor, -0x8000, -0x7F38, 0, 0xFF);
    } else if (this->unk_31C == 1) {
        Actor_SetColorFilter(&this->actor, 0, 0xC8, 0, 0x50);
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_LIGHT_ARROW_HIT);
        Actor_SetColorFilter(&this->actor, -0x8000, 0xC8, 0, 0x50);
    }
    EnRd_SetupAction(this, func_80AE3ECC);
}

void func_80AE3ECC(EnRd* this, PlayState* play) {
    if ((this->unk_318 != 0) && (this->unk_316 != 0)) {
        this->unk_316--;
        if (this->unk_316 >= 0xFF) {
            Actor_SetColorFilter(&this->actor, -0x8000, 0xC8, 0, 0xFF);
        }
        if (this->unk_316 == 0) {
            this->unk_318 = 0;
            gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
        }
    }

    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            func_80AE2630(play, &this->actor, 1);
            func_80AE3C20(this);
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
        } else {
            func_80AE3A8C(this);
        }
    }
}

void func_80AE3F9C(EnRd* this, PlayState* play) {
    s16 temp1;
    s16 temp2;
    s16 temp3;

    temp1 = this->actor.yawTowardsPlayer - (s16)(this->actor.shape.rot.y + this->unk_310);
    temp2 = CLAMP(temp1, -500, 500);

    temp1 -= this->unk_30E;
    temp3 = CLAMP(temp1, -500, 500);

    if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0) {
        this->unk_310 += ABS(temp2);
        this->unk_30E += ABS(temp3);
    } else {
        this->unk_310 -= ABS(temp2);
        this->unk_30E -= ABS(temp3);
    }

    this->unk_310 = CLAMP(this->unk_310, -18783, 18783);
    this->unk_30E = CLAMP(this->unk_30E, -9583, 9583);
}

void func_80AE4114(EnRd* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((gSaveContext.sunsSongState != SUNSSONG_INACTIVE) && (this->actor.shape.rot.x == 0) && (this->unk_318 == 0) &&
        (this->unk_31B != 9) && (this->unk_31B != 10) && (this->unk_31B != 1)) {
        func_80AE3DE4(this);
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->unk_31C = this->actor.colChkInfo.damageEffect;

        if (this->unk_31B != 11) {
            Actor_SetDropFlag(&this->actor, &this->collider.info, 1);
            if (player->unk_844 != 0) {
                this->unk_31D = player->unk_845;
            }

            if ((this->unk_31C != 0) && (this->unk_31C != 6)) {
                if (((this->unk_31C == 1) || (this->unk_31C == 13)) && (this->unk_31B != 1)) {
                    Actor_ApplyDamage(&this->actor);
                    func_80AE3DE4(this);
                    return;
                }

                this->unk_318 = 0;
                this->unk_316 = 0;

                if (this->unk_31C == 0xE) {
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0x50);
                    this->unk_31A = 0x28;
                } else {
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
                }

                Actor_ApplyDamage(&this->actor);
                if (this->actor.colChkInfo.health == 0) {
                    func_80AE2630(play, &this->actor, 1);
                    func_80AE3C20(this);
                    Item_DropCollectibleRandom(play, 0, &this->actor.world.pos, 0x90);
                } else {
                    func_80AE3A8C(this);
                }
            }
        }
    }
}

void EnRd_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRd* this = (EnRd*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    func_80AE4114(this, play);

    if (gSaveContext.sunsSongState != SUNSSONG_INACTIVE && this->unk_318 == 0) {
        gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    }

    if (this->unk_31C != 6 && ((this->unk_31B != 11) || (this->unk_31C != 14))) {
        if (this->unk_306 != 0) {
            this->unk_306--;
        }

        this->actionFunc(this, play);
        if (this->unk_31B != 8 && this->actor.speedXZ != 0.0f) {
            Actor_MoveForward(&this->actor);
        }

        if ((this->actor.shape.rot.x == 0) && (this->unk_31B != 8) && (this->actor.speedXZ != 0.0f)) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 20.0f, 35.0f, 0x1D);
        }

        if (this->unk_31B == 7) {
            func_80AE3F9C(this, play);
        }
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 50.0f;

    if ((this->actor.colChkInfo.health > 0) && (this->unk_31B != 8)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        if ((this->unk_31B != 9) || ((player->unk_844 != 0) && (player->unk_845 != this->unk_31D))) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 EnRd_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnRd* this = (EnRd*)thisx;

    if (limbIndex == 23) {
        rot->y += this->unk_30E;
    } else if (limbIndex == 12) {
        rot->y += this->unk_310;
    }
    return false;
}

void EnRd_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Vec3f sp2C = D_80AE4940;
    EnRd* this = (EnRd*)thisx;
    s32 idx = -1;
    Vec3f destPos;

    if ((this->unk_31A != 0) || ((this->actor.colorFilterTimer != 0) && (this->actor.colorFilterParams & 0x4000))) {
        switch (limbIndex - 1) {
            case 23:
                idx = 0;
                break;
            case 0:
                idx = 1;
                break;
            case 21:
                idx = 2;
                break;
            case 17:
                idx = 3;
                break;
            case 13:
                idx = 4;
                break;
            case 24:
                idx = 5;
                break;
            case 8:
                idx = 6;
                break;
            case 3:
                idx = 7;
                break;
            case 10:
                idx = 8;
                break;
            case 5:
                idx = 9;
                break;
        }

        if (idx >= 0) {
            Matrix_MultVec3f(&sp2C, &destPos);
            this->firePos[idx].x = destPos.x;
            this->firePos[idx].y = destPos.y;
            this->firePos[idx].z = destPos.z;
        }
    }
}

void EnRd_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRd* this = (EnRd*)thisx;
    Vec3f thisPos = thisx->world.pos;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_314 == 0xFF) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->unk_314);
        gSPSegment(POLY_OPA_DISP++, 8, &D_80116280[2]);
        POLY_OPA_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, EnRd_OverrideLimbDraw, EnRd_PostLimbDraw, this,
                                           POLY_OPA_DISP);
        func_80033C30(&thisPos, &D_80AE4958, 255, play);
        if (this->unk_31A != 0) {
            thisx->colorFilterTimer++;
            this->unk_31A--;
            if (this->unk_31A % 4 == 0) {
                EffectSsEnFire_SpawnVec3s(play, thisx, &this->firePos[this->unk_31A >> 2], 0x4B, 0, 0,
                                          (this->unk_31A >> 2));
            }
        }
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_314);
        gSPSegment(POLY_XLU_DISP++, 8, &D_80116280[0]);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                               this->skelAnime.dListCount, EnRd_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);

        func_80033C30(&thisPos, &D_80AE4958, this->unk_314, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
