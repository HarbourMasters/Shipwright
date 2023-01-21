/*
 * File: z_en_bw.c
 * Overlay: ovl_En_Bw
 * Description: Torch slug
 */

#include "z_en_bw.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_bw/object_bw.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4)

void EnBw_Init(Actor* thisx, PlayState* play);
void EnBw_Destroy(Actor* thisx, PlayState* play);
void EnBw_Update(Actor* thisx, PlayState* play);
void EnBw_Draw(Actor* thisx, PlayState* play);
void EnBw_Reset(void);

void func_809CE884(EnBw* this, PlayState* play);
void func_809CE9A8(EnBw* this);
void func_809CEA24(EnBw* this, PlayState* play);
void func_809CF72C(EnBw* this);
void func_809CF7AC(EnBw* this, PlayState* play);
void func_809CF8F0(EnBw* this);
void func_809CF984(EnBw* this, PlayState* play);
void func_809CFBA8(EnBw* this);
void func_809CFC4C(EnBw* this, PlayState* play);
void func_809CFF10(EnBw* this);
void func_809CFF98(EnBw* this, PlayState* play);
void func_809D00F4(EnBw* this);
void func_809D014C(EnBw* this, PlayState* play);
void func_809D01CC(EnBw* this);
void func_809D0268(EnBw* this, PlayState* play);
void func_809D03CC(EnBw* this);
void func_809D0424(EnBw* this, PlayState* play);

const ActorInit En_Bw_InitVars = {
    ACTOR_EN_BW,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BW,
    sizeof(EnBw),
    (ActorFunc)EnBw_Init,
    (ActorFunc)EnBw_Destroy,
    (ActorFunc)EnBw_Update,
    (ActorFunc)EnBw_Draw,
    (ActorResetFunc)EnBw_Reset,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { 30, 65, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
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
    { 30, 35, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(2, 0xF),
    /* Ice arrow     */ DMG_ENTRY(4, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xE),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
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

s32 sSlugGroup = 0;

void EnBw_SetupAction(EnBw* this, EnBwActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnBw_Init(Actor* thisx, PlayState* play) {
    EnBw* this = (EnBw*)thisx;

    Actor_SetScale(&this->actor, 0.012999999f);
    this->actor.naviEnemyId = 0x23;
    this->actor.gravity = -2.0f;
    SkelAnime_Init(play, &this->skelAnime, &object_bw_Skel_0020F0, &object_bw_Anim_000228, this->jointTable,
                   this->morphTable, 12);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 40.0f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.colChkInfo.health = 6;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.focus.pos = this->actor.world.pos;
    func_809CE9A8(this);
    this->color1.a = this->color1.r = 255;
    this->color1.g = this->color1.b = 0;
    this->unk_248 = 0.6f;
    this->unk_221 = 3;
    Collider_InitCylinder(play, &this->collider1);
    //! this->collider2 should have Init called on it, but it doesn't matter since the heap is zeroed before use.
    Collider_SetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
    Collider_SetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);
    this->unk_236 = this->actor.world.rot.y;
    this->actor.params = sSlugGroup;
    sSlugGroup = (sSlugGroup + 1) & 3;
}

void EnBw_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBw* this = (EnBw*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
}

void func_809CE884(EnBw* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    this->unk_222 -= 250;
    this->actor.scale.x = 0.013f + Math_SinF(this->unk_222 * 0.001f) * 0.0069999998f;
    this->actor.scale.y = 0.013f - Math_SinF(this->unk_222 * 0.001f) * 0.0069999998f;
    this->actor.scale.z = 0.013f + Math_SinF(this->unk_222 * 0.001f) * 0.0069999998f;
    if (this->unk_222 == 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        func_809CE9A8(this);
    }
}

void func_809CE9A8(EnBw* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_bw_Anim_000228, -2.0f);
    this->unk_220 = 2;
    this->unk_222 = Rand_ZeroOne() * 200.0f + 200.0f;
    this->unk_232 = 0;
    this->actor.speedXZ = 0.0f;
    EnBw_SetupAction(this, func_809CEA24);
}

void func_809CEA24(EnBw* this, PlayState* play) {
    CollisionPoly* sp74 = NULL;
    Vec3f sp68;
    u32 sp64 = 0;
    s16 sp62;
    s16 sp60;
    f32 sp5C;
    f32 sp58;
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->unk_244 = this->unk_250 + 0.1f;
    sp58 = Math_CosF(this->unk_240);
    this->unk_240 += this->unk_244;
    if (this->unk_24C < 0.8f) {
        this->unk_24C += 0.1f;
    }
    this->actor.scale.x = 0.013f - Math_SinF(this->unk_240) * (this->unk_24C * 0.004f);
    this->actor.scale.y = 0.013f - Math_SinF(this->unk_240) * (this->unk_24C * 0.004f);
    this->actor.scale.z = 0.013f + Math_SinF(this->unk_240) * (this->unk_24C * 0.004f);
    sp5C = Math_CosF(this->unk_240);
    if (this->unk_232 == 0) {
        if (ABS(sp58) < ABS(sp5C)) {
            this->unk_232++;
        }
    } else {
        if (ABS(sp58) > ABS(sp5C)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_WALK);
            this->unk_232 = 0;
        }
    }
    sp5C *= this->unk_24C * (10.0f * this->unk_244);
    this->actor.speedXZ = ABS(sp5C);
    if (this->unk_221 != 1) {
        sp58 = Math_SinF(this->unk_240);
        sp60 = ABS(sp58) * 85.0f;
        this->color1.g = sp60;
    }
    if ((((play->gameplayFrames % 4) == (u32)this->actor.params) && (this->actor.speedXZ != 0.0f) &&
         (sp64 = BgCheck_AnyLineTest2(&play->colCtx, &this->actor.world.pos, &this->unk_264, &sp68, &sp74, 1, 0, 0,
                                      1))) ||
        (this->unk_222 == 0)) {
        if (sp74 != NULL) {
            sp74 = SEGMENTED_TO_VIRTUAL(sp74);
            sp62 = Math_FAtan2F(sp74->normal.x, sp74->normal.z) * ((f32)0x8000 / M_PI);
        } else {
            sp62 = this->actor.world.rot.y + 0x8000;
        }
        if ((this->unk_236 != sp62) || (sp64 == 0)) {
            if (BgCheck_AnyLineTest2(&play->colCtx, &this->unk_270, &this->unk_288, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 2;
            }
            if (BgCheck_AnyLineTest2(&play->colCtx, &this->unk_270, &this->unk_27C, &sp68, &sp74, 1, 0, 0, 1)) {
                sp64 |= 4;
            }
            switch (sp64) {
                case 0:
                    this->unk_236 += this->unk_238;
                case 1:
                    if (this->unk_221 == 3) {
                        if (play->gameplayFrames & 2) {
                            this->unk_238 = 0x4000;
                        } else {
                            this->unk_238 = -0x4000;
                        }
                    } else {
                        if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0.0f) {
                            this->unk_238 = 0x4000;
                        } else {
                            this->unk_238 = -0x4000;
                        }
                        if (this->unk_221 == 1) {
                            this->unk_238 = -this->unk_238;
                        }
                    }
                    break;
                case 2:
                    this->unk_236 += this->unk_238;
                case 3:
                    this->unk_238 = 0x4000;
                    break;
                case 4:
                    this->unk_236 += this->unk_238;
                case 5:
                    this->unk_238 = -0x4000;
                    break;
                case 7:
                    this->unk_238 = 0;
                    break;
            }
            if (sp64 != 6) {
                this->unk_236 = sp62;
            }
            this->unk_222 = (Rand_ZeroOne() * 200.0f) + 200.0f;
        }
    } else if ((this->actor.speedXZ != 0.0f) && (this->actor.bgCheckFlags & 8)) {
        if (this->unk_236 != this->actor.wallYaw) {
            sp64 = 1;
            this->unk_236 = this->actor.wallYaw;
            if (this->unk_221 == 3) {
                if (play->gameplayFrames & 0x20) {
                    this->unk_238 = 0x4000;
                } else {
                    this->unk_238 = -0x4000;
                }
                this->actor.bgCheckFlags &= ~8;
                this->unk_222 = (Rand_ZeroOne() * 20.0f) + 160.0f;
            } else {
                if ((s16)(this->actor.yawTowardsPlayer - this->unk_236) >= 0) {
                    this->unk_238 = 0x4000;
                } else {
                    this->unk_238 = -0x4000;
                }
                if (this->unk_221 == 1) {
                    this->unk_238 = -this->unk_238;
                }
            }
        } else if (this->unk_221 == 0) {
            sp64 = BgCheck_AnyLineTest2(&play->colCtx, &this->actor.world.pos, &player->actor.world.pos, &sp68,
                                        &sp74, 1, 0, 0, 1);
            if (sp64 != 0) {
                sp74 = SEGMENTED_TO_VIRTUAL(sp74);
                sp60 = Math_FAtan2F(sp74->normal.x, sp74->normal.z) * ((f32)0x8000 / M_PI);
                if (this->unk_236 != sp60) {
                    if ((s16)(this->actor.yawTowardsPlayer - sp60) >= 0) {
                        this->unk_238 = 0x4000;
                    } else {
                        this->unk_238 = -0x4000;
                    }
                    this->unk_236 = sp60;
                }
            }
        }
    }
    this->unk_222--;
    if (this->unk_224 != 0) {
        this->unk_224--;
    }
    if ((this->unk_234 == 0) &&
        !Actor_TestFloorInDirection(&this->actor, play, 50.0f, this->unk_236 + this->unk_238)) {
        if (this->unk_238 != 0x4000) {
            this->unk_238 = 0x4000;
        } else {
            this->unk_238 = -0x4000;
        }
    }
    switch (this->unk_221) {
        case 3:
            Math_SmoothStepToF(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if ((this->unk_224 == 0) && (this->actor.xzDistToPlayer < 200.0f) &&
                (ABS(this->actor.yDistToPlayer) < 50.0f) && Actor_IsFacingPlayer(&this->actor, 0x1C70)) {
                func_809CF72C(this);
            } else {
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speedXZ * 1000.0f, 0);
            }
            break;
        case 0:
            Math_SmoothStepToF(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
            if (sp64 == 0) {
                Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1,
                                   this->actor.speedXZ * 1000.0f, 0);
                if ((this->actor.xzDistToPlayer < 90.0f) && (this->actor.yDistToPlayer < 50.0f) &&
                    Actor_IsFacingPlayer(&this->actor, 0x1554) &&
                    Actor_TestFloorInDirection(&this->actor, play, 71.24802f, this->actor.yawTowardsPlayer)) {
                    func_809CF8F0(this);
                }
            } else {
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speedXZ * 1000.0f, 0);
            }
            if ((this->unk_224 == 0) || (ABS(this->actor.yDistToPlayer) > 60.0f) || (player2->stateFlags1 & 0x6000)) {
                this->unk_221 = 3;
                this->unk_224 = 150;
                this->unk_250 = 0.0f;
            }
            break;
        case 1:
            if (((sp64 == 0) && !(this->actor.bgCheckFlags & 8)) || Actor_IsFacingPlayer(&this->actor, 0x1C70)) {
                if (Actor_IsFacingPlayer(&this->actor, 0x1C70)) {
                    this->unk_238 = -this->unk_238;
                }
                Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x8000, 1,
                                   this->actor.speedXZ * 1000.0f, 0);
            } else {
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_236 + this->unk_238, 1,
                                   this->actor.speedXZ * 1000.0f, 0);
            }
            if (this->unk_224 <= 200) {
                sp60 = Math_SinS(this->unk_224 * (0x960 - this->unk_224)) * 55.0f;
                this->color1.r = 255 - ABS(sp60);
                sp60 = Math_SinS(this->unk_224 * (0x960 - this->unk_224)) * 115.0f;
                this->color1.g = ABS(sp60) + 85;
                sp60 = Math_SinS(0x960 - this->unk_224) * 255.0f;
                this->color1.b = ABS(sp60);
                if (this->unk_224 == 0) {
                    this->unk_221 = 3;
                    this->unk_250 = 0.0f;
                }
            }
            break;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void func_809CF72C(EnBw* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_bw_Anim_0021A0, -2.0f);
    this->unk_220 = 3;
    this->unk_221 = 0;
    this->unk_250 = 0.6f;
    this->unk_222 = 20;
    this->unk_224 = 0xBB8;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_AIM);
    EnBw_SetupAction(this, func_809CF7AC);
}

void func_809CF7AC(EnBw* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_222 > 0) {
            this->unk_222--;
        } else {
            this->unk_222 = 100;
        }
    }
    if (this->unk_222 >= 20) {
        this->unk_224 -= 250;
    }
    this->actor.scale.x = 0.013f - Math_SinF(this->unk_224 * 0.001f) * 0.0034999999f;
    this->actor.scale.y = 0.013f + Math_SinF(this->unk_224 * 0.001f) * 0.0245f;
    this->actor.scale.z = 0.013f - Math_SinF(this->unk_224 * 0.001f) * 0.0034999999f;
    if (this->unk_224 == 0) {
        func_809CE9A8(this);
        this->unk_224 = 200;
    }
}

void func_809CF8F0(EnBw* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_bw_Anim_002250, -1.0f);
    this->actor.speedXZ = 7.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->unk_220 = 4;
    this->unk_222 = 1000;
    this->actor.velocity.y = 11.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STAL_JUMP);
    this->actor.flags |= ACTOR_FLAG_24;
    EnBw_SetupAction(this, func_809CF984);
}

void func_809CF984(EnBw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 floorPolyType;

    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    this->unk_222 += 250;
    this->actor.scale.x = 0.013f - Math_SinF(this->unk_222 * 0.001f) * 0.0034999999f;
    this->actor.scale.y = 0.013f + Math_SinF(this->unk_222 * 0.001f) * 0.0245f;
    this->actor.scale.z = 0.013f - Math_SinF(this->unk_222 * 0.001f) * 0.0034999999f;
    if (this->collider1.base.atFlags & AT_HIT) {
        this->collider1.base.atFlags &= ~AT_HIT;
        this->actor.speedXZ = -6.0f;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if ((&player->actor == this->collider1.base.at) && !(this->collider1.base.atFlags & AT_BOUNCED)) {
            Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & 3) {
        floorPolyType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
        if ((floorPolyType == 2) || (floorPolyType == 3) || (floorPolyType == 9)) {
            Actor_Kill(&this->actor);
            return;
        }
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        this->unk_222 = 3000;
        this->actor.flags &= ~ACTOR_FLAG_24;
        this->actor.speedXZ = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        EnBw_SetupAction(this, func_809CE884);
    }
}

void func_809CFBA8(EnBw* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_bw_Anim_002250, -1.0f);
    this->unk_220 = 5;
    this->unk_222 = 1000;
    this->unk_260 = 0.0f;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 11.0f;
    this->unk_25C = Rand_ZeroOne() * 0.25f + 1.0f;
    this->unk_224 = 0xBB8;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    EnBw_SetupAction(this, func_809CFC4C);
}

void func_809CFC4C(EnBw* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, 0x7FFF, 1, 0xFA0, 0);
    Math_SmoothStepToF(&this->unk_248, 0.0f, 1.0f, 0.05f, 0.0f);
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & 3) {
        if ((play->gameplayFrames % 0x80) == 0) {
            this->unk_25C = (Rand_ZeroOne() * 0.25f) + 0.7f;
        }
        this->unk_221 = 4;
        this->unk_258 += this->unk_25C;
        Math_SmoothStepToF(&this->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
        if (this->actor.bgCheckFlags & 2) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        }
        if (this->unk_224 != 0) {
            this->unk_224 -= 250;
            this->actor.scale.x = 0.013f + Math_SinF(this->unk_224 * 0.001f) * 0.0069999998f;
            this->actor.scale.y = 0.013f - Math_SinF(this->unk_224 * 0.001f) * 0.0069999998f;
            this->actor.scale.z = 0.013f + Math_SinF(this->unk_224 * 0.001f) * 0.0069999998f;
        }
        this->unk_222--;
        if (this->unk_222 == 0) {
            func_809CFF10(this);
        }
    } else {
        this->color1.r -= 8;
        this->color1.g += 32;
        if (this->color1.r < 200) {
            this->color1.r = 200;
        }
        if (this->color1.g > 200) {
            this->color1.g = 200;
        }
        if (this->color1.b > 235) {
            this->color1.b = 255;
        } else {
            this->color1.b += 40;
        }
        if (this->actor.shape.yOffset < 1000.0f) {
            this->actor.shape.yOffset += 200.0f;
        }
    }
}

void func_809CFF10(EnBw* this) {
    Animation_MorphToLoop(&this->skelAnime, &object_bw_Anim_002250, -1.0f);
    this->unk_220 = 6;
    this->unk_222 = 1000;
    this->unk_221 = 3;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 11.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_REVERSE);
    this->actor.bgCheckFlags &= ~1;
    EnBw_SetupAction(this, func_809CFF98);
}

void func_809CFF98(EnBw* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, 0, 1, 0xFA0, 0);
    Math_SmoothStepToF(&this->unk_248, 0.6f, 1.0f, 0.05f, 0.0f);
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & 3) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        this->unk_222 = 0xBB8;
        this->unk_250 = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        EnBw_SetupAction(this, func_809CE884);
    }
    if (this->color1.r < 247) {
        this->color1.r += 8;
    } else {
        this->color1.r = 255;
    }
    if (this->color1.g < 32) {
        this->color1.g = 0;
    } else {
        this->color1.g -= 32;
    }
    if (this->color1.b < 40) {
        this->color1.b = 0;
    } else {
        this->color1.b -= 40;
    }
    if (this->actor.shape.yOffset > 0.0f) {
        this->actor.shape.yOffset -= 200.0f;
    }
}

void func_809D00F4(EnBw* this) {
    this->unk_220 = 0;
    this->unk_222 = 40;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->actor.speedXZ = 0.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_DEAD);
    EnBw_SetupAction(this, func_809D014C);
    gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_TORCH_SLUG]++;
}

void func_809D014C(EnBw* this, PlayState* play) {
    if (this->unk_230) {
        this->actor.scale.x += 0.0002f;
        this->actor.scale.y -= 0.0002f;
        this->actor.scale.z += 0.0002f;
    }
    this->actor.shape.shadowAlpha = this->color1.a -= 6;
    this->unk_222--;
    if (this->unk_222 <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_809D01CC(EnBw* this) {
    this->unk_220 = 1;
    this->actor.speedXZ = 0.0f;
    this->unk_25C = (Rand_ZeroOne() * 0.25f) + 1.0f;
    this->unk_260 = 0.0f;
    if (this->damageEffect == 0xE) {
        this->iceTimer = 0x50;
    }
    this->unk_222 = (this->actor.colorFilterParams & 0x4000) ? 25 : 80;
    EnBw_SetupAction(this, func_809D0268);
}

void func_809D0268(EnBw* this, PlayState* play) {
    this->color1.r -= 1;
    this->color1.g += 4;
    this->color1.b += 5;
    if (this->color1.r < 200) {
        this->color1.r = 200;
    }
    if (this->color1.g > 200) {
        this->color1.g = 200;
    }
    if (this->color1.b > 230) {
        this->color1.b = 230;
    }
    if (this->actor.colorFilterParams & 0x4000) {
        if ((play->gameplayFrames % 0x80) == 0) {
            this->unk_25C = 0.5f + Rand_ZeroOne() * 0.25f;
        }
        this->unk_258 += this->unk_25C;
        Math_SmoothStepToF(&this->unk_260, 0.075f, 1.0f, 0.005f, 0.0f);
    }
    this->unk_222--;
    if (this->unk_222 == 0) {
        func_809CE9A8(this);
        this->color1.r = this->color1.g = 200;
        this->color1.b = 255;
        this->unk_224 = 0x258;
        this->unk_221 = 1;
        this->unk_250 = 0.7f;
        this->unk_236++;
    }
}

void func_809D03CC(EnBw* this) {
    this->actor.speedXZ = 0.0f;
    if (this->damageEffect == 0xE) {
        this->iceTimer = 32;
    }
    this->unk_23C = this->actor.colorFilterTimer;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    EnBw_SetupAction(this, func_809D0424);
}

void func_809D0424(EnBw* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        this->unk_23C = 0;
        if (this->actor.colChkInfo.health != 0) {
            if ((this->unk_220 != 5) && (this->unk_220 != 6)) {
                func_809CE9A8(this);
                this->color1.r = this->color1.g = 200;
                this->color1.b = 255;
                this->unk_224 = 0x258;
                this->unk_221 = 1;
                this->unk_250 = 0.7f;
                this->unk_236++;
            } else if (this->unk_220 != 5) {
                EnBw_SetupAction(this, func_809CFF98);
            } else {
                func_809CFF10(this);
            }
        } else {
            if (func_800355E4(play, &this->collider2.base)) {
                this->unk_230 = 0;
                this->actor.scale.y -= 0.009f;
                Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
            } else {
                this->unk_230 = 1;
            }
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
            func_809D00F4(this);
        }
    }
}

void func_809D0584(EnBw* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & 0x10) && (this->actor.bgCheckFlags & 1)) {
        this->unk_230 = 0;
        this->actor.scale.y -= 0.009f;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0, false);
        func_809D00F4(this);
    } else {
        if (this->collider2.base.acFlags & AC_HIT) {
            this->collider2.base.acFlags &= ~AC_HIT;
            if ((this->actor.colChkInfo.damageEffect == 0) || (this->unk_220 == 6)) {
                return;
            }
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            Actor_SetDropFlag(&this->actor, &this->collider2.info, 0);
            if ((this->damageEffect == 1) || (this->damageEffect == 0xE)) {
                if (this->unk_23C == 0) {
                    Actor_ApplyDamage(&this->actor);
                    Actor_SetColorFilter(&this->actor, 0, 0x78, 0, 0x50);
                    func_809D03CC(this);
                    this->unk_248 = 0.0f;
                }
                return;
            }
            if (this->unk_248 == 0.0f) {
                Actor_ApplyDamage(&this->actor);
            }
            if (((this->unk_221 == 1) || (this->unk_221 == 4)) && (this->actor.colChkInfo.health == 0)) {
                if (this->unk_220 != 0) {
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
                    if (func_800355E4(play, &this->collider2.base)) {
                        this->unk_230 = 0;
                        this->actor.scale.y -= 0.009f;
                        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 30.0f, 11, 4.0f, 0, 0,
                                                 false);
                    } else {
                        this->unk_230 = 1;
                    }
                    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
                    func_809D00F4(this);
                }
            } else if ((this->unk_220 != 1) && (this->unk_220 != 6)) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEWALK_DAMAGE);
                Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 8);
                if (this->unk_220 != 5) {
                    func_809D01CC(this);
                }
                this->unk_248 = 0.0f;
            }
        }
        if ((play->actorCtx.unk_02 != 0) && (this->actor.xzDistToPlayer <= 400.0f) &&
            (this->actor.bgCheckFlags & 1)) {
            if (this->unk_220 == 5) {
                this->unk_23C = 0;
                func_809CFF10(this);
            } else if (this->unk_220 != 0) {
                this->unk_23C = 0;
                func_809CFBA8(this);
            }
        }
    }
}

void EnBw_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBw* this = (EnBw*)thisx;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s32 pad[3]; // probably an unused Vec3f
    Color_RGBA8 sp50 = { 255, 200, 0, 255 };
    Color_RGBA8 sp4C = { 255, 80, 0, 255 };
    Color_RGBA8 sp48 = { 0, 0, 0, 255 };
    Color_RGBA8 sp44 = { 0, 0, 0, 220 };

    func_809D0584(this, play);
    if (thisx->colChkInfo.damageEffect != 6) {
        this->actionFunc(this, play);
        if (this->unk_23C == 0) {
            this->unk_23A = (this->unk_23A + 4) & 0x7F;
        }
        if ((play->gameplayFrames & this->unk_221) == 0) {
            accel.y = -3.0f;
            velocity.x = Rand_CenteredFloat(this->unk_248 * 24.0f);
            velocity.y = this->unk_248 * 30.0f;
            velocity.z = Rand_CenteredFloat(this->unk_248 * 24.0f);
            accel.x = velocity.x * -0.075f;
            accel.z = velocity.z * -0.075f;
            func_8002836C(play, &thisx->world.pos, &velocity, &accel, &sp50, &sp4C, 0x3C, 0, 0x14);
        }
        if (this->unk_248 <= 0.4f) {
            this->collider1.info.toucher.effect = 0;
            if (((play->gameplayFrames & 1) == 0) && (this->unk_220 < 5) && (this->unk_23C == 0)) {
                accel.y = -0.1f;
                velocity.x = Rand_CenteredFloat(4.0f);
                velocity.y = Rand_CenteredFloat(2.0f) + 6.0f;
                velocity.z = Rand_CenteredFloat(4.0f);
                accel.x = velocity.x * -0.1f;
                accel.z = velocity.z * -0.1f;
                Rand_CenteredFloat(4.0f);
                Rand_CenteredFloat(4.0f);
                sp48.a = this->color1.a;
                if (sp48.a >= 30) {
                    sp44.a = sp48.a - 30;
                } else {
                    sp44.a = 0;
                }
                func_8002836C(play, &thisx->world.pos, &velocity, &accel, &sp48, &sp44, 0xB4, 0x28,
                              20.0f - (this->unk_248 * 40.0f));
            }
        } else {
            this->collider1.info.toucher.effect = 1;
        }

        this->unk_234 = Actor_TestFloorInDirection(thisx, play, 50.0f, thisx->world.rot.y);
        if ((this->unk_220 == 4) || (this->unk_220 == 6) || (this->unk_220 == 5) || (this->unk_220 == 1) ||
            (this->unk_234 != 0)) {
            Actor_MoveForward(thisx);
        }
        Actor_UpdateBgCheckInfo(play, thisx, 20.0f, 30.0f, 21.0f, 0x1F);
    }
    Collider_UpdateCylinder(thisx, &this->collider2);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider2.base);
    if ((this->unk_220 != 0) && ((thisx->colorFilterTimer == 0) || !(thisx->colorFilterParams & 0x4000))) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
    }
    if ((this->unk_221 != 1) && (this->unk_220 < 5) && (this->unk_248 > 0.4f)) {
        Collider_UpdateCylinder(thisx, &this->collider1);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
    }
    thisx->focus.pos = thisx->world.pos;
    thisx->focus.pos.y += 5.0f;
}

s32 EnBw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnBw* this = (EnBw*)thisx;

    if (limbIndex == 1) {
        gSPSegment((*gfx)++, 0x09,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x20, 1, 0, this->unk_23A, 0x20, 0x20));
        if ((this->unk_220 == 1) || (this->unk_220 == 5)) {
            Matrix_Push();
            Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            Matrix_RotateX(this->unk_258 * 0.115f, MTXMODE_APPLY);
            Matrix_RotateY(this->unk_258 * 0.13f, MTXMODE_APPLY);
            Matrix_RotateZ(this->unk_258 * 0.1f, MTXMODE_APPLY);
            Matrix_Scale(1.0f - this->unk_260, 1.0f + this->unk_260, 1.0f - this->unk_260, MTXMODE_APPLY);
            Matrix_RotateZ(-(this->unk_258 * 0.1f), MTXMODE_APPLY);
            Matrix_RotateY(-(this->unk_258 * 0.13f), MTXMODE_APPLY);
            Matrix_RotateX(-(this->unk_258 * 0.115f), MTXMODE_APPLY);
            gSPMatrix((*gfx)++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList((*gfx)++, *dList);
            Matrix_Pop();
            return 1;
        }
    }
    return 0;
}

static Vec3f sIceOffsets[] = {
    { 20.0f, 10.0f, 0.0f },  { -20.0f, 10.0f, 0.0f }, { 0.0f, 10.0f, -25.0f }, { 10.0f, 0.0f, 15.0f },
    { 10.0f, 0.0f, -15.0f }, { 0.0f, 10.0f, 25.0f },  { -10.0f, 0.0f, 15.0f }, { -10.0f, 0.0f, -15.0f },
};

void EnBw_Draw(Actor* thisx, PlayState* play2) {
    Vec3f spAC = { 0.0f, 0.0f, 0.0f };
    PlayState* play = play2;
    EnBw* this = (EnBw*)thisx;
    Vec3f icePos;
    s32 iceIndex;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->color1.a == 0xFF) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, this->color1.r, this->color1.g, this->color1.b, this->color1.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnBw_OverrideLimbDraw, NULL, this, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 0, 0, this->color1.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->color1.r, this->color1.g, this->color1.b, this->color1.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, &D_80116280[0]);
        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnBw_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);
    }

    if (((play->gameplayFrames + 1) % 4) == (u32)thisx->params) {
        spAC.z = thisx->scale.z * 375000.0f;
        Matrix_MultVec3f(&spAC, &this->unk_264);
        spAC.z = thisx->scale.z * 150000.0f;
        Matrix_MultVec3f(&spAC, &this->unk_270);
        spAC.x = thisx->scale.x * 550000.0f;
        Matrix_MultVec3f(&spAC, &this->unk_288);
        spAC.x = -spAC.x;
        Matrix_MultVec3f(&spAC, &this->unk_27C);
    }

    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y + ((thisx->scale.y - 0.013f) * 1000.0f), thisx->world.pos.z,
                     MTXMODE_NEW);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                (play->gameplayFrames * -20) % 0x200, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
    Matrix_Scale(this->unk_248 * 0.01f, this->unk_248 * 0.01f, this->unk_248 * 0.01f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    if (this->iceTimer != 0) {
        thisx->colorFilterTimer++;
        this->iceTimer--;
        if ((this->iceTimer & 3) == 0) {
            iceIndex = this->iceTimer >> 2;

            icePos.x = sIceOffsets[iceIndex].x + thisx->world.pos.x;
            icePos.y = sIceOffsets[iceIndex].y + thisx->world.pos.y;
            icePos.z = sIceOffsets[iceIndex].z + thisx->world.pos.z;
            EffectSsEnIce_SpawnFlyingVec3f(play, thisx, &icePos, 0x96, 0x96, 0x96, 0xFA, 0xEB, 0xF5, 0xFF, 1.3f);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBw_Reset(void) {
    sSlugGroup = 0;
}