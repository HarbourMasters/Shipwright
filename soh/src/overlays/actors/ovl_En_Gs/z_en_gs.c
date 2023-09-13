/*
 * File: z_en_gs.c
 * Overlay: ovl_En_Gs
 * Description: Gossip Stone
 */

#include "z_en_gs.h"
#include "objects/object_gs/object_gs.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnGs_Init(Actor* thisx, PlayState* play);
void EnGs_Destroy(Actor* thisx, PlayState* play);
void EnGs_Update(Actor* thisx, PlayState* play);
void EnGs_Draw(Actor* thisx, PlayState* play);

void func_80A4F734(EnGs* this, PlayState* play);
void func_80A4F700(EnGs* this, PlayState* play);

void func_80A4F77C(EnGs* this);

const ActorInit En_Gs_InitVars = {
    ACTOR_EN_GS,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GS,
    sizeof(EnGs),
    (ActorFunc)EnGs_Init,
    (ActorFunc)EnGs_Destroy,
    (ActorFunc)EnGs_Update,
    (ActorFunc)EnGs_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 21, 48, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0xE),
    /* Explosive     */ DMG_ENTRY(0, 0xC),
    /* Boomerang     */ DMG_ENTRY(0, 0xE),
    /* Normal arrow  */ DMG_ENTRY(0, 0xE),
    /* Hammer swing  */ DMG_ENTRY(0, 0xD),
    /* Hookshot      */ DMG_ENTRY(0, 0xE),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xF),
    /* Master sword  */ DMG_ENTRY(0, 0xF),
    /* Giant's Knife */ DMG_ENTRY(0, 0xF),
    /* Fire arrow    */ DMG_ENTRY(0, 0xB),
    /* Ice arrow     */ DMG_ENTRY(0, 0xB),
    /* Light arrow   */ DMG_ENTRY(0, 0xB),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xB),
    /* Ice magic     */ DMG_ENTRY(0, 0xB),
    /* Light magic   */ DMG_ENTRY(0, 0xB),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void EnGs_Init(Actor* thisx, PlayState* play) {
    EnGs* this = (EnGs*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    CollisionCheck_SetInfo2(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);

    thisx->targetMode = 6;
    this->unk_1D8 = thisx->world.pos;
    this->actionFunc = func_80A4F734;
    this->unk_1B4[0].x = 1.0f;
    this->unk_1B4[0].y = 1.0f;
    this->unk_1B4[0].z = 1.0f;
    this->unk_1B4[1].x = 1.0f;
    this->unk_1B4[1].y = 1.0f;
    this->unk_1B4[1].z = 1.0f;
}

void EnGs_Destroy(Actor* thisx, PlayState* play) {
}

s32 func_80A4E3EC(EnGs* this, PlayState* play) {
    s32 ret = 2;
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                switch (this->actor.textId) {
                    case 0x2054:
                        this->actor.textId = (this->actor.params & 0xFF) + 0x400;
                        ret = 1;
                        break;
                    default:
                        ret = 0;
                        break;
                }
            }
            break;
    }
    return ret;
}

void func_80A4E470(EnGs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    bREG(15) = 0;
    if (this->actor.xzDistToPlayer <= 100.0f) {
        bREG(15) = 1;
        if (this->unk_19D == 0) {
            player->stateFlags2 |= 0x800000;
            if (player->stateFlags2 & 0x1000000) {
                func_8010BD58(play, OCARINA_ACTION_FREE_PLAY);
                this->unk_19D |= 1;
            }

        } else if (this->unk_19D & 1) {
            if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
                if ((play->msgCtx.unk_E3F2 == OCARINA_SONG_SARIAS) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_EPONAS) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_LULLABY) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_SUNS) ||
                    (play->msgCtx.unk_E3F2 == OCARINA_SONG_TIME)) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0, FAIRY_HEAL_TIMED, true);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                } else if (play->msgCtx.unk_E3F2 == OCARINA_SONG_STORMS) {
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0, FAIRY_HEAL_BIG, true);
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                }
                this->unk_19D = 0;
                Flags_SetSwitch(play, (this->actor.params >> 8) & 0x3F);
            } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
                player->stateFlags2 |= 0x800000;
            }
        }
    }
}

void func_80A4E648(EnGs* this, PlayState* play) {
    s16 sp26;
    s16 sp24;

    if (this->unk_19C == 1) {
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_19C = 2;
    } else if (this->unk_19C == 2) {
        this->unk_19C = func_80A4E3EC(this, play);
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->unk_19C = 2;
    } else {
        Actor_GetScreenPos(play, &this->actor, &sp26, &sp24);
        if ((sp26 >= 0) && (sp26 <= SCREEN_WIDTH) && (sp24 >= 0) && (sp24 <= SCREEN_HEIGHT) && (this->unk_19C != 3)) {
            if (func_8002F2CC(&this->actor, play, 40.0f) == 1) {
                if (IS_RANDO) {
                    // if we're rando'd, always use the non-mask text id
                    this->actor.textId = 0x2053;
                } else {
                    if (Player_GetMask(play) == PLAYER_MASK_TRUTH) {
                        this->actor.textId = 0x2054;
                    } else {
                        this->actor.textId = 0x2053;
                    }
                }
            }
        }
    }
}

f32 func_80A4E754(EnGs* this, PlayState* play, f32* arg2, f32* arg3, u16* arg4, f32 arg5, f32 arg6, f32 arg7,
                  s32 arg8, s32 arg9) {
    f32 sp2C = Math_SmoothStepToF(arg2, *arg3, arg5, arg6, arg7);

    if (arg9 == 0) {
        sp2C = Math_SmoothStepToF(arg2, *arg3, arg5, arg6, arg7);
        this->unk_1B4[0].x = 1.0f + (sinf((((*arg4 % arg8) * (1.0f / arg8)) * 360.0f) * (M_PI / 180.0f)) * *arg2);
        this->unk_1B4[0].y = 1.0f - (sinf((((*arg4 % arg8) * (1.0f / arg8)) * 360.0f) * (M_PI / 180.0f)) * *arg2);
        *arg4 += 1;
    }
    return sp2C;
}

void func_80A4E910(EnGs* this, PlayState* play) {
    if (this->unk_19F == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_200 = 0;
        this->unk_19F = 1;
        this->unk_1E8 = 0.5f;
        this->unk_1EC = 0.0f;
    } else if ((this->unk_19F == 1) && (func_80A4E754(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200,
                                                      0.8f, 0.007f, 0.001f, 7, 0) == 0.0f)) {
        if (!Play_InCsMode(play)) {
            Message_StartTextbox(play, 0x71B1, NULL);
        }
        this->unk_19C = 0;
        this->actionFunc = func_80A4F734;
    }
}

void func_80A4EA08(EnGs* this, PlayState* play) {
    if (this->unk_19F == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_1E8 = 0.3f;
        this->unk_1EC = 0.0f;
        this->unk_200 = 0;
        this->unk_19F = 1;
    } else if (this->unk_19F == 1) {
        this->unk_1A0[0].z = (((this->unk_200 % 8) / 8.0f) * 360.0f) * (0x10000 / 360.0f);
        this->unk_1A0[1].z = -this->unk_1A0[0].z;
        if (func_80A4E754(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200, 0.8f, 0.005f, 0.001f, 7,
                          0) == 0.0f) {
            this->unk_19C = 0;
            this->actionFunc = func_80A4F734;
        }
    }
}

void func_80A4EB3C(EnGs* this, PlayState* play) {
    f32 ret;

    if (this->unk_19F == 0) {
        this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
        this->unk_1EC = -0.8f;
        this->unk_19F++;
    } else if (this->unk_19F == 1) {
        ret = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 1.0f, 0.4f, 0.001f);
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (ret == 0.0f) {
            this->unk_200 = 0;
            this->unk_19F++;
        }
    } else if (this->unk_19F == 2) {
        this->unk_200++;
        if (this->unk_200 >= 100) {
            this->unk_19F++;
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 0.0f;
        }
    } else if (this->unk_19F == 3) {
        ret = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 1.0f, 0.5f, 0.001f);
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (ret == 0.0f) {
            this->unk_1E8 = 0.5f;
            this->unk_1EC = 0.0f;
            this->unk_200 = 0;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->unk_19F++;
        }
    } else if (this->unk_19F == 4) {
        if (func_80A4E754(this, play, &this->unk_1E8, &this->unk_1EC, &this->unk_200, 1.0f, 0.03f, 0.001f, 5, 0) ==
            0.0f) {
            this->unk_19C = 0;
            this->actionFunc = func_80A4F734;
        }
    }
}

void func_80A4ED34(EnGs* this, PlayState* play) {
    static Color_RGBA8 flashRed = { 255, 50, 50, 0 };
    static Color_RGBA8 flashBlue = { 50, 50, 255, 0 };
    static Color_RGBA8 baseWhite = { 255, 255, 255, 0 };
    static Vec3f dustAccel = { 0.0f, -0.3f, 0.0f };
    static Color_RGBA8 dustPrim = { 200, 200, 200, 128 };
    static Color_RGBA8 dustEnv = { 100, 100, 100, 0 };
    static Vec3f bomb2Velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f bomb2Accel = { 0.0f, 0.0f, 0.0f };
    u8 i;
    Vec3f dustPos;
    Vec3f dustVelocity;
    Vec3f bomb2Pos;

    if (this->unk_19F == 0) {
        this->unk_200 = 40;
        this->unk_19F++;
    }

    if (this->unk_19F == 1) {
        if ((u32)this->unk_200-- <= 0) {
            this->unk_200 = 80;
            this->unk_19E |= 4;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 2) {
        this->unk_200--;
        Color_RGBA8_Copy(&this->flashColor, &baseWhite);
        if ((this->unk_200 < 80) && ((this->unk_200 % 20) < 8)) {
            if (this->unk_200 < 20) {
                Color_RGBA8_Copy(&this->flashColor, &flashRed);
                if ((this->unk_200 % 20) == 7) {
                    Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_E, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                }
            } else {
                Color_RGBA8_Copy(&this->flashColor, &flashBlue);
                if ((this->unk_200 % 20) == 7) {
                    Audio_PlaySoundGeneral(NA_SE_SY_WARNING_COUNT_N, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                }
            }
        }
        if (this->unk_200 <= 0) {
            this->unk_200 = 0;
            this->unk_19E &= ~4;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 3) {
        for (i = 0; i < 3; i++) {
            dustVelocity.x = Rand_CenteredFloat(15.0f);
            dustVelocity.y = Rand_ZeroFloat(-1.0f);
            dustVelocity.z = Rand_CenteredFloat(15.0f);
            dustPos.x = this->actor.world.pos.x + (dustVelocity.x + dustVelocity.x);
            dustPos.y = this->actor.world.pos.y + 7.0f;
            dustPos.z = this->actor.world.pos.z + (dustVelocity.z + dustVelocity.z);
            func_8002836C(play, &dustPos, &dustVelocity, &dustAccel, &dustPrim, &dustEnv,
                          (s16)Rand_ZeroFloat(50.0f) + 200, 40, 15);
        }

        func_8002F974(&this->actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        if (this->unk_200++ >= 40) {
            this->unk_19E |= 0x10;
            this->actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->actor.uncullZoneForward = 12000.0f;

            this->actor.gravity = 0.3f;
            this->unk_19F++;
        }
    }

    if (this->unk_19F == 4) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, 3);
        if (this->actor.bgCheckFlags & 0x18) {
            bomb2Pos.x = this->actor.world.pos.x;
            bomb2Pos.y = this->actor.world.pos.y;
            bomb2Pos.z = this->actor.world.pos.z;
            Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
            EffectSsBomb2_SpawnLayered(play, &bomb2Pos, &bomb2Velocity, &bomb2Accel, 100, 20);
            this->unk_200 = 10;
            this->unk_19E |= 8;
            this->actionFunc = func_80A4F700;
        } else {
            func_8002F974(&this->actor, NA_SE_EV_STONE_LAUNCH - SFX_FLAG);
        }

        Actor_MoveForward(&this->actor);
        if (this->actor.yDistToPlayer < -12000.0f) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80A4F13C(EnGs* this, PlayState* play) {
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    s32 tmp;
    s16 tmp2 = this->unk_1A0[0].y;

    if (this->unk_19F == 0) {
        this->unk_1E8 = 0.0f;
        this->unk_1EC = 90.0f;
        this->unk_1F0 = 0.1f;
        this->unk_1F4 = 2.0f;
        this->unk_1F8 = 0;
        this->unk_19F = 1;
    }
    if (this->unk_19F == 1) {
        Math_SmoothStepToF(&this->unk_1F0, this->unk_1F4, 1.0f, 0.1f, 0.001f);
        tmpf1 = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 1.0f, this->unk_1F0, 0.001f);
        this->unk_1A0[0].y += (s32)(this->unk_1E8 * (0x10000 / 360.0f));
        if (tmpf1 == 0.0f) {
            this->unk_200 = 0;
            this->unk_19F = 2;
        }
    }
    if (this->unk_19F == 2) {
        this->unk_1A0[0].y += (s32)(this->unk_1E8 * (0x10000 / 360.0f));
        if (this->unk_200++ > 40) {
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 1.5f;
            this->unk_1F0 = this->unk_1B4[1].y - 1.0f;
            this->unk_1F4 = -0.3f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_STONE_GROW_UP);
            this->unk_19F = 3;
        }
    }
    if (this->unk_19F == 3) {
        this->unk_1A0[0].y += 0x4000;
        tmpf1 = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 0.8f, 0.2f, 0.001f);
        Math_SmoothStepToF(&this->unk_1F0, this->unk_1F4, 0.8f, 0.2f, 0.001f);
        this->unk_1B4[0].x = this->unk_1F0 + 1.0f;
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        if (tmpf1 == 0.0f) {
            this->unk_1E8 = 2.0f * M_PI / 9.0000002;
            this->unk_1EC = M_PI / 9.0000002;
            this->unk_19F = 4;
        }
    }
    if (this->unk_19F == 4) {
        tmpf1 = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 0.8f, 16384.0f, 3640.0f);
        this->unk_1A0[0].y += (s16)this->unk_1E8;
        if (tmpf1 == 0.0f) {

            tmp = this->unk_1A0[0].y;
            if (tmp > 0) {
                tmp += 0xFFFF0000;
            }
            this->unk_1E8 = tmp;

            this->unk_1EC = 0.0f;
            this->unk_19F = 5;
        }
    }
    if (this->unk_19F == 5) {
        tmp = this->unk_1A0[0].y;
        if (tmp > 0) {
            tmp += 0xFFFF0001;
        }
        this->unk_1E8 = tmp;
        tmpf1 = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 0.8f, 3640.0f, 0.001f);
        this->unk_1A0[0].y = this->unk_1E8;
        if (tmpf1 == 0.0f) {
            this->unk_1E8 = this->unk_1B4[0].y - 1.0f;
            this->unk_1EC = 0;
            this->unk_1A0[0].y = 0;
            this->unk_200 = 0;
            this->unk_1F0 = this->unk_1B4[0].x - 1.0f;
            this->unk_1F4 = 0;
            this->unk_1F8 = 0.5f;
            this->unk_1FC = 0;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->unk_19F = 6;
        }
    }
    if (this->unk_19F == 6) {
        tmpf1 = Math_SmoothStepToF(&this->unk_1E8, this->unk_1EC, 0.8f, 0.1f, 0.001f);
        tmpf2 = Math_SmoothStepToF(&this->unk_1F0, this->unk_1F4, 0.8f, 0.1f, 0.001f);
        tmpf3 = Math_SmoothStepToF(&this->unk_1F8, this->unk_1FC, 0.8f, 0.02f, 0.001f);
        this->unk_1B4[0].x = this->unk_1F0 + 1.0f;
        this->unk_1B4[0].y = this->unk_1E8 + 1.0f;
        this->unk_1B4[0].x += sinf((((this->unk_200 % 10) * 0.1f) * 360.0f) * (M_PI / 180.0f)) * this->unk_1F8;
        this->unk_1B4[0].y += sinf((((this->unk_200 % 10) * 0.1f) * 360.0f) * (M_PI / 180.0f)) * this->unk_1F8;
        this->unk_200++;
        if ((tmpf1 == 0.0f) && (tmpf2 == 0.0f) && (tmpf3 == 0.0f)) {
            this->unk_19C = 0;
            this->actionFunc = func_80A4F734;
        }
    }
    if ((u16)this->unk_1A0[0].y < (u16)tmp2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_STONE_ROLLING);
    }
}

void func_80A4F700(EnGs* this, PlayState* play) {
    if (this->unk_200-- <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80A4F734(EnGs* this, PlayState* play) {
    if (!Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {
        func_80A4E470(this, play);
    }
}

void func_80A4F77C(EnGs* this) {
    static Vec3s D_80A4FE34 = { 0, 0, 0 };
    static Vec3f D_80A4FE3C = { 1.0f, 1.0f, 1.0f };
    s32 i;

    for (i = 0; i < 3; i++) {
        this->unk_1A0[i] = D_80A4FE34;
    }

    for (i = 0; i < 2; i++) {
        this->unk_1B4[i] = D_80A4FE3C;
    }

    this->unk_19C = 3;
}

void EnGs_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGs* this = (EnGs*)thisx;

    Actor_SetFocus(&this->actor, 23.0f);
    if (!(this->unk_19E & 0x10)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->unk_19F = 0;
            this->collider.base.acFlags &= ~AC_HIT;

            switch (this->actor.colChkInfo.damageEffect) {
                case 15:
                    this->unk_19E |= 1;
                    func_80A4F77C(this);
                    this->actionFunc = func_80A4E910;
                    break;
                case 14:
                    this->unk_19E |= 1;
                    func_80A4F77C(this);
                    this->actionFunc = func_80A4EA08;
                    break;
                case 13:
                    this->unk_19E |= 1;
                    func_80A4F77C(this);
                    this->actionFunc = func_80A4EB3C;
                    break;
                case 12:
                    this->unk_19E |= 2;
                    func_80A4F77C(this);
                    this->actionFunc = func_80A4ED34;
                    break;
                case 11:
                    this->unk_19E |= 1;
                    func_80A4F77C(this);
                    this->actionFunc = func_80A4F13C;
                    break;
            }
        }
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    this->actionFunc(this, play);
    func_80A4E648(this, play);
}

void EnGs_Draw(Actor* thisx, PlayState* play) {
    EnGs* this = (EnGs*)thisx;
    s32 tmp;
    u32 frames;

    if (!(this->unk_19E & 8)) {
        OPEN_DISPS(play->state.gfxCtx);

        frames = play->gameplayFrames;
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        Matrix_Push();
        if (this->unk_19E & 1) {
            Matrix_RotateY(this->unk_1A0[0].y * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateX(this->unk_1A0[0].x * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateZ(this->unk_1A0[0].z * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_Scale(this->unk_1B4[0].x, this->unk_1B4[0].y, this->unk_1B4[0].z, MTXMODE_APPLY);
            Matrix_RotateY(this->unk_1A0[1].y * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateX(this->unk_1A0[1].x * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateZ(this->unk_1A0[1].z * (M_PI / 0x8000), MTXMODE_APPLY);
        }

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneMaterialDL);

        if (this->unk_19E & 4) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->flashColor.r, this->flashColor.g, this->flashColor.b,
                            this->flashColor.a);
        } else {
            if (CVarGetInteger("gCosmetics.World_GossipStone.Changed", 0)) {
                Color_RGB8 color = CVarGetColor24("gCosmetics.World_GossipStone.Value", (Color_RGB8){255, 255, 255});
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, color.r, color.g, color.b, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            }
        }

        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneDL);
        gSPDisplayList(POLY_OPA_DISP++, gGossipStoneSquishedDL);

        Matrix_Pop();
        if (this->unk_19E & 2) {
            Gfx_SetupDL_25Xlu(play->state.gfxCtx);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(0.05f, -0.05f, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(
                POLY_XLU_DISP++, 0x08,
                Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, -frames * 0x14, 0x20, 0x80));
            gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
