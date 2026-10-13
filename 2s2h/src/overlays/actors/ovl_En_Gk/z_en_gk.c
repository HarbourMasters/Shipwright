/*
 * File: z_en_gk.c
 * Overlay: ovl_En_Gk
 * Description: Goron Elder's Son
 */

#include "z_en_gk.h"

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnGk_Init(Actor* thisx, PlayState* play);
void EnGk_Destroy(Actor* thisx, PlayState* play);
void EnGk_Update(Actor* thisx, PlayState* play);
void EnGk_Draw(Actor* thisx, PlayState* play);

void func_80B51698(EnGk* this, PlayState* play);
void func_80B51760(EnGk* this, PlayState* play);
void func_80B51970(EnGk* this, PlayState* play);
void func_80B51B40(EnGk* this, PlayState* play);
void func_80B51D9C(EnGk* this, PlayState* play);
void func_80B51EA4(EnGk* this, PlayState* play);
void func_80B51FD0(EnGk* this, PlayState* play);
void func_80B5202C(EnGk* this, PlayState* play);
void func_80B5216C(EnGk* this, PlayState* play);
void func_80B521E8(EnGk* this, PlayState* play);
void func_80B5227C(EnGk* this, PlayState* play);
void func_80B52340(EnGk* this, PlayState* play);
void func_80B52430(EnGk* this, PlayState* play);
void func_80B5253C(EnGk* this, PlayState* play);
void func_80B525E0(EnGk* this, PlayState* play);
void func_80B52654(EnGk* this, PlayState* play);

ActorProfile En_Gk_Profile = {
    /**/ ACTOR_EN_GK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GK,
    /**/ sizeof(EnGk),
    /**/ EnGk_Init,
    /**/ EnGk_Destroy,
    /**/ EnGk_Update,
    /**/ EnGk_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 24, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 24, 32, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum {
    /*  -1 */ ENGK_ANIM_NONE = -1,
    /* 0x0 */ ENGK_ANIM_0,
    /* 0x1 */ ENGK_ANIM_1,
    /* 0x2 */ ENGK_ANIM_2,
    /* 0x3 */ ENGK_ANIM_3,
    /* 0x4 */ ENGK_ANIM_4,
    /* 0x5 */ ENGK_ANIM_5,
    /* 0x6 */ ENGK_ANIM_6,
    /* 0x7 */ ENGK_ANIM_7,
    /* 0x8 */ ENGK_ANIM_8,
    /* 0x9 */ ENGK_ANIM_9,
    /* 0xA */ ENGK_ANIM_10,
    /* 0xB */ ENGK_ANIM_11,
    /* 0xC */ ENGK_ANIM_12,
    /* 0xD */ ENGK_ANIM_MAX
} EnGkAnimation;

static AnimationInfo sAnimationInfo[ENGK_ANIM_MAX] = {
    { &object_gk_Anim_00787C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_0
    { &object_gk_Anim_007DC4, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // ENGK_ANIM_1
    { &object_gk_Anim_0092C0, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_2
    { &object_gk_Anim_005EDC, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_3
    { &object_gk_Anim_009638, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_4
    { &object_gk_Anim_008774, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_5
    { &object_gk_Anim_00AE34, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_6
    { &object_gk_Anim_00BD90, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // ENGK_ANIM_7
    { &object_gk_Anim_00C308, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_8
    { &object_gk_Anim_009858, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // ENGK_ANIM_9
    { &object_gk_Anim_009D88, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_10
    { &object_gk_Anim_00A21C, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // ENGK_ANIM_11
    { &object_gk_Anim_00AAEC, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // ENGK_ANIM_12
};

Color_RGBA8 D_80B533A0 = { 255, 255, 255, 255 };
Color_RGBA8 D_80B533A4 = { 50, 150, 150, 0 };

u16 func_80B50410(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->sceneId == SCENE_17SETUGEN2) {
        if (player->transformation == PLAYER_FORM_GORON) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_40_80)) {
                switch (this->unk_31C) {
                    case 0xE7A:
                        return 0xE7B;

                    case 0xE7B:
                        return 0xE7C;

                    case 0xE7C:
                        return 0xE7D;

                    case 0xE7D:
                        return 0xE7E;

                    case 0xE7E:
                        return 0xE7F;

                    case 0xE7F:
                        SET_WEEKEVENTREG(WEEKEVENTREG_40_80);
                        this->unk_1E4 |= 1;
                        return 0xE80;

                    default:
                        return 0xE7A;
                }
            } else {
                this->unk_1E4 |= 1;
                return 0xE81;
            }
        } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_01)) {
            switch (this->unk_31C) {
                case 0xE82:
                    return 0xE83;

                case 0xE83:
                    return 0xE7D;

                case 0xE7D:
                    return 0xE7E;

                case 0xE7E:
                    return 0xE7F;

                case 0xE7F:
                    SET_WEEKEVENTREG(WEEKEVENTREG_41_01);
                    this->unk_1E4 |= 1;
                    return 0xE80;

                default:
                    return 0xE82;
            }
        } else {
            this->unk_1E4 |= 1;
            return 0xE81;
        }
    } else if (play->sceneId == SCENE_GORONRACE) {
        if (player->transformation == PLAYER_FORM_GORON) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_04)) {
                if (this->unk_31C == 0xE88) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE) || Inventory_HasEmptyBottle()) {
                        return 0xE89;
                    }
                    SET_WEEKEVENTREG(WEEKEVENTREG_41_04);
                    this->unk_1E4 |= 1;
                    return 0xE94;
                }
                return 0xE88;
            }

            if ((this->unk_31C == 0xE8D) || (this->unk_31C == 0xE98)) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE) || Inventory_HasEmptyBottle()) {
                    return 0xE89;
                }
                SET_WEEKEVENTREG(WEEKEVENTREG_41_04);
                this->unk_1E4 |= 1;
                return 0xE94;
            }

            if (this->unk_1E4 & 0x10) {
                return 0xE8D;
            }
            return 0xE98;
        }

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_41_02)) {
            switch (this->unk_31C) {
                case 0xE85:
                    return 0xE86;

                case 0xE86:
                    SET_WEEKEVENTREG(WEEKEVENTREG_41_02);
                    this->unk_1E4 |= 1;
                    return 0xE87;

                default:
                    return 0xE85;
            }
        } else {
            this->unk_1E4 |= 1;
            return 0xE87;
        }
    }

    return 0;
}

u16 func_80B50710(EnGk* this) {
    switch (this->unk_31C) {
        case 0xE8E:
            SET_WEEKEVENTREG(WEEKEVENTREG_41_04);
            this->unk_1E4 &= ~0x10;
            this->unk_1E4 |= 1;
            return 0xE8F;

        case 0xE8A:
            return 0xE8B;

        case 0xE8B:
            SET_WEEKEVENTREG(WEEKEVENTREG_41_04);
            this->unk_1E4 |= 0x10;
            this->unk_1E4 |= 1;
            return 0xE8C;

        default:
            return 0;
    }
}

void func_80B507A0(EnGk* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 30.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
}

s32 func_80B50854(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(this->unk_1E4 & 0x40)) {
        if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
            this->unk_1E4 |= 0x40;
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
        this->unk_1E4 &= ~0x40;
    }

    if ((player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
        (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY)) {
        Flags_SetSwitch(play, ENGK_GET_SWITCH_FLAG(&this->actor));
        this->animIndex = ENGK_ANIM_3;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_3);
        this->actionFunc = func_80B521E8;
        return true;
    }
    return false;
}

void func_80B50954(EnGk* this) {
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLONKID_WALK);
    }
}

void func_80B509A8(EnGk* this, PlayState* play) {
    Vec3f sp4C;
    s16 phi_s1 = 0;

    this->unk_300.x += 2.0f * Rand_Centered();
    this->unk_300.y += Rand_ZeroOne();
    this->unk_300.z += 2.0f * Rand_Centered();

    this->unk_30C.x += 2.0f * Rand_Centered();
    this->unk_30C.y += Rand_ZeroOne();
    this->unk_30C.z += 2.0f * Rand_Centered();

    if (this->animIndex == ENGK_ANIM_0) {
        sp4C.x = 0.0f;
        sp4C.z = 0.0f;
        sp4C.y = -1.2f;
        phi_s1 = 25;
    } else if ((this->animIndex == ENGK_ANIM_2) || (this->animIndex == ENGK_ANIM_9) ||
               (this->animIndex == ENGK_ANIM_10) || (this->animIndex == ENGK_ANIM_11)) {
        sp4C.x = 0.0f;
        sp4C.z = 0.0f;
        sp4C.y = -0.5f;
        phi_s1 = 10;
    }

    EffectSsDtBubble_SpawnCustomColor(play, &this->unk_2E8, &this->unk_300, &sp4C, &D_80B533A0, &D_80B533A4,
                                      Rand_S16Offset(15, 15), phi_s1, false);
    EffectSsDtBubble_SpawnCustomColor(play, &this->unk_2F4, &this->unk_30C, &sp4C, &D_80B533A0, &D_80B533A4,
                                      Rand_S16Offset(15, 15), phi_s1, false);
}

void func_80B50B38(EnGk* this, PlayState* play) {
    s16 i;
    s16 temp;

    switch (this->animIndex) {
        case ENGK_ANIM_0:
            this->unk_2E0 = 3;
            for (i = 0; i < 2; i++) {
                func_80B509A8(this, play);
            }
            break;

        case ENGK_ANIM_2:
            this->unk_2E0 = 3;
            func_80B509A8(this, play);
            break;

        case ENGK_ANIM_1:
        case ENGK_ANIM_3:
        case ENGK_ANIM_12:
            this->unk_2E0 = 2;
            break;

        case ENGK_ANIM_9:
        case ENGK_ANIM_10:
        case ENGK_ANIM_11:
            this->unk_2E0 = 3;
            func_80B509A8(this, play);
            break;

        default:
            temp = this->unk_2E2;
            temp--;
            if (temp >= 3) {
                this->unk_2E0 = 0;
                this->unk_2E2 = temp;
            } else if (temp == 0) {
                this->unk_2E0 = 2;
                this->unk_2E2 = (s32)(Rand_ZeroOne() * 60.0f) + 20;
            } else {
                this->unk_2E0 = 1;
                this->unk_2E2 = temp;
            }
            break;
    }
}

s32 EnGk_HasReachedPoint(EnGk* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

f32 func_80B50E14(Path* path, s32 arg1, Vec3f* arg2, Vec3s* arg3) {
    Vec3s* points;
    Vec3f sp20;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[arg1];
        sp20.x = points[0].x;
        sp20.y = points[0].y;
        sp20.z = points[0].z;
    }
    arg3->y = Math_Vec3f_Yaw(arg2, &sp20);
    arg3->x = Math_Vec3f_Pitch(arg2, &sp20);
    return sp20.y - arg2->y;
}

s32 func_80B50ED4(s16 arg0, s16 arg1, Vec3f* arg2, Vec3s* arg3, s32 arg4, s32 arg5) {
    Vec3f sp7C;
    Vec3f sp70 = gZeroVec3f;
    Vec3s sp68;
    MtxF sp28;

    Matrix_MultVec3f(&sp70, &sp7C);
    Matrix_Get(&sp28);
    Matrix_MtxFToYXZRot(&sp28, &sp68, false);

    *arg2 = sp7C;

    if (!arg4 && !arg5) {
        arg3->x = sp68.x;
        arg3->y = sp68.y;
        arg3->z = sp68.z;
        return true;
    }

    if (arg5) {
        sp68.z = arg0;
        sp68.y = arg1;
    }

    Math_SmoothStepToS(&arg3->x, sp68.x, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&arg3->y, sp68.y, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&arg3->z, sp68.z, 3, 0x2AA8, 0xB6);

    return true;
}

s32 func_80B5100C(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    Vec3f sp34;

    Math_SmoothStepToS(&this->unk_320, (this->actor.yawTowardsPlayer - this->unk_324) - this->actor.shape.rot.y, 4,
                       0x2AA8, 1);
    this->unk_320 = CLAMP(this->unk_320, -0x1FFE, 0x1FFE);

    Math_SmoothStepToS(&this->unk_324, (this->actor.yawTowardsPlayer - this->unk_320) - this->actor.shape.rot.y, 4,
                       0x2AA8, 1);
    this->unk_324 = CLAMP(this->unk_324, -0x1C70, 0x1C70);

    sp40 = player->actor.world.pos;
    sp40.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;

    sp34 = this->actor.world.pos;
    sp34.y += 70.0f;

    Math_SmoothStepToS(&this->unk_31E, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_322, 4, 0x2AA8, 1);
    this->unk_31E = CLAMP(this->unk_31E, -0x1C70, 0x1C70);

    Math_SmoothStepToS(&this->unk_322, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_31E, 4, 0x2AA8, 1);
    this->unk_322 = CLAMP(this->unk_322, -0x1C70, 0x1C70);
    return true;
}

s32 func_80B5123C(EnGk* this, PlayState* play) {
    if (DECR(this->unk_34E) != 0) {
        this->unk_31E = 0;
        this->unk_320 = 0;
        this->unk_1E4 &= ~8;
        this->unk_322 = 0;
        this->unk_324 = 0;
        return true;
    }

    if (Actor_IsFacingAndNearPlayer(&this->actor, 300.0f, 0x7FF8)) {
        this->unk_1E4 |= 8;
        func_80B5100C(this, play);
    } else {
        if (this->unk_1E4 & 8) {
            this->unk_34E = 20;
        }
        this->unk_1E4 &= ~8;
        this->unk_31E = 0;
        this->unk_320 = 0;
        this->unk_322 = 0;
        this->unk_324 = 0;
    }
    return true;
}

void func_80B51308(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->csAnimIndex].animation);

    if ((this->animIndex == ENGK_ANIM_7) && (curFrame == endFrame)) {
        this->animIndex = ENGK_ANIM_8;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_8);
    }
}

void func_80B51398(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[9].animation);

    if ((this->animIndex == ENGK_ANIM_9) && (curFrame == endFrame)) {
        this->animIndex = ENGK_ANIM_10;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_10);
    }
}

void func_80B51410(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.xzDistToPlayer < 100.0f) {
        if ((player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
            (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY_INTRO)) {
            this->unk_1E4 |= 0x20;
        }

        if (CutsceneManager_IsNext(this->csId)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_GOLONKID_SOB_TALK);
            CutsceneManager_Start(this->csId, &this->actor);
            this->unk_1E4 &= ~0x20;
            return;
        }

        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }

        if (this->unk_1E4 & 0x20) {
            CutsceneManager_Queue(this->csId);
        }
    } else {
        this->unk_1E4 &= ~0x20;
    }
}

void func_80B51510(EnGk* this, PlayState* play) {
    s32 pad;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_479)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_479);

        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->csAnimIndex = ENGK_ANIM_0;
                    this->animIndex = ENGK_ANIM_0;
                    break;

                case 2:
                    this->csAnimIndex = ENGK_ANIM_2;
                    this->animIndex = ENGK_ANIM_2;
                    break;

                case 3:
                    this->csAnimIndex = ENGK_ANIM_7;
                    this->animIndex = ENGK_ANIM_7;
                    break;

                case 4:
                    this->csAnimIndex = ENGK_ANIM_3;
                    this->animIndex = ENGK_ANIM_3;
                    break;

                case 5:
                    this->csAnimIndex = ENGK_ANIM_1;
                    this->animIndex = ENGK_ANIM_1;
                    this->unk_1E4 |= 0x80;
                    this->actionFunc = func_80B5227C;
                    break;

                case 6:
                    this->csAnimIndex = ENGK_ANIM_12;
                    this->animIndex = ENGK_ANIM_12;
                    break;

                case 7:
                    Flags_SetSwitch(play, ENGK_GET_SWITCH_FLAG(&this->actor));
                    break;

                default:
                    break;
            }
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->csAnimIndex);
        }

        if (this->csAnimIndex == ENGK_ANIM_7) {
            func_80B51308(this, play);
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        this->actor.shape.yOffset = 0.0f;
    } else {
        this->cueId = 99;
    }
}

void func_80B51698(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        switch (this->animIndex) {
            case ENGK_ANIM_0:
                this->animIndex = ENGK_ANIM_2;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_2);
                this->actionFunc = func_80B5216C;
                break;

            case ENGK_ANIM_2:
                this->animIndex = ENGK_ANIM_0;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_0);
                this->actionFunc = func_80B5202C;
                break;

            default:
                break;
        }
    }
}

void func_80B51760(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame;

    if (this->animIndex == ENGK_ANIM_11) {
        endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);
        if (curFrame == endFrame) {
            this->animIndex = ENGK_ANIM_5;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else if (this->animIndex == ENGK_ANIM_10) {
        endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);
        if (curFrame == endFrame) {
            this->animIndex = ENGK_ANIM_11;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else {
        if (Flags_GetSwitch(play, ENGK_GET_SWITCH_FLAG(&this->actor))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_40_40);
            this->actionFunc = func_80B51D9C;
            return;
        }

        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->unk_1E4 |= 4;
            this->unk_31C = func_80B50410(this, play);
            Message_StartTextbox(play, this->unk_31C, &this->actor);
            this->actionFunc = func_80B51970;
            if (this->unk_31C == 0xE81) {
                this->animIndex = ENGK_ANIM_0;
                this->unk_1E4 |= 2;
            }
        } else if (((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) &&
                   (gSaveContext.save.entrance != ENTRANCE(GORON_RACETRACK, 1))) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }

        if (this->unk_1E4 & 4) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
            this->actor.world.rot.y = this->actor.shape.rot.y;
        }
    }
}

void func_80B51970(EnGk* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_DONE) || (talkState == TEXT_STATE_EVENT)) && Message_ShouldAdvance(play)) {
        if ((this->unk_31C == 0xE84) || (this->unk_31C == 0xE99)) {
            CutsceneManager_Stop(this->csId);
            this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
            CutsceneManager_Queue(this->csId);
            this->actionFunc = func_80B51D9C;
            return;
        }

        if (this->unk_1E4 & 1) {
            this->unk_1E4 &= ~1;
            this->unk_1E4 &= ~4;
            if (this->animIndex == ENGK_ANIM_10) {
                this->unk_1E4 &= ~2;
            }
            this->actionFunc = func_80B51760;
            return;
        }

        this->unk_31C = func_80B50410(this, play);
        Message_StartTextbox(play, this->unk_31C, &this->actor);
        if (this->unk_31C == 0xE80) {
            this->unk_1E4 |= 2;
        } else if (this->unk_31C == 0xE89) {
            this->actionFunc = func_80B51B40;
        }
    }

    if (this->unk_1E4 & 2) {
        Audio_PlaySfx_AtFixedPos(&this->actor.projectedPos, NA_SE_EN_GOLON_KID_CRY - SFX_FLAG);
        if (this->animIndex != ENGK_ANIM_10) {
            if (this->animIndex != ENGK_ANIM_9) {
                this->animIndex = ENGK_ANIM_9;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_9);
            } else {
                func_80B51398(this, play);
            }
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->animIndex == ENGK_ANIM_6) {
        func_80B50954(this);
    }
}

void func_80B51B40(EnGk* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play)) {
            if (this->unk_1E4 & 1) {
                this->unk_1E4 &= ~1;
                this->unk_1E4 &= ~4;

                if (this->animIndex == ENGK_ANIM_10) {
                    this->unk_1E4 &= ~2;
                }

                if (this->unk_31C == 0xE8F) {
                    play->nextEntrance = ENTRANCE(GORON_RACETRACK, 1);
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    Magic_Add(play, MAGIC_FILL_TO_CAPACITY);
                } else {
                    this->actionFunc = func_80B51760;
                }
                return;
            }

            this->unk_31C = func_80B50710(this);
            Message_StartTextbox(play, this->unk_31C, &this->actor);
            if (this->unk_31C == 0xE8C) {
                this->unk_1E4 |= 2;
            }
        }
    } else if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Audio_PlaySfx_MessageDecide();
                this->unk_31C = 0xE8E;
                Message_StartTextbox(play, this->unk_31C, &this->actor);
                break;

            case 1:
                Audio_PlaySfx_MessageCancel();
                this->unk_31C = 0xE8A;
                Message_StartTextbox(play, this->unk_31C, &this->actor);
                break;

            default:
                break;
        }
    }

    if (this->unk_1E4 & 2) {
        Audio_PlaySfx_AtFixedPos(&this->actor.projectedPos, NA_SE_EN_GOLON_KID_CRY - SFX_FLAG);
        if (this->animIndex != ENGK_ANIM_10) {
            if (this->animIndex != ENGK_ANIM_9) {
                this->animIndex = ENGK_ANIM_9;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_9);
            } else {
                func_80B51398(this, play);
            }
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80B51D9C(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCsAndSetFlag(this->csId, &this->actor);
        if (this->unk_1E4 & 4) {
            this->unk_1E4 &= ~4;
            this->animIndex = ENGK_ANIM_6;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_6);
            this->actionFunc = func_80B51EA4;
        } else {
            this->unk_1E4 |= 4;
            if (player->transformation == PLAYER_FORM_GORON) {
                this->unk_31C = 0xE84;
            } else {
                this->unk_31C = 0xE99;
            }
            Message_StartTextbox(play, this->unk_31C, &this->actor);
            this->actionFunc = func_80B51970;
        }
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
    func_80B50954(this);
}

void func_80B51EA4(EnGk* this, PlayState* play) {
    Vec3s sp38;
    s16 sp36;

    if (this->path != NULL) {
        func_80B50E14(this->path, this->unk_1EC, &this->actor.world.pos, &sp38);
        Math_SmoothStepToS(&this->actor.world.rot.y, sp38.y, 5, 0x1000, 0x100);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        sp36 = this->actor.shape.rot.y - sp38.y;
        if (EnGk_HasReachedPoint(this, this->path, this->unk_1EC)) {
            if (this->unk_1EC >= (this->path->count - 1)) {
                CutsceneManager_Stop(this->csId);
                Actor_Kill(&this->actor);
            } else {
                this->unk_1EC++;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            sp38.y = this->actor.wallYaw;
        }

        if (ABS_ALT(sp36) < 0x2AAA) {
            Math_ApproachF(&this->actor.speed, 3.0f, 0.2f, 0.5f);
        }
        Actor_MoveWithGravity(&this->actor);
    }
    func_80B50954(this);
}

void func_80B51FD0(EnGk* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
        if (this->unk_1E4 & 2) {
            if (GameInteractor_Should(VB_PLAY_GORON_CHILD_CRY, true)) {
                Audio_PlaySfx_AtFixedPos(&this->actor.projectedPos, NA_SE_EN_GOLON_KID_CRY - SFX_FLAG);
            }
        } else {
            this->unk_1E4 |= 2;
        }
    }
}

void func_80B5202C(EnGk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->csCtx.state == CS_STATE_IDLE) {
        func_80B51410(this, play);
    }

    if (!func_80B50854(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_80);
            this->actionFunc = func_80B51698;
        } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            if (player->transformation == PLAYER_FORM_GORON) {
                this->actor.textId = 0xE74;
            } else {
                this->actor.textId = 0xE76;
            }
        }

        if (this->unk_1E4 & 2) {
            if ((play->msgCtx.ocarinaMode != OCARINA_MODE_ACTIVE) && (play->msgCtx.ocarinaMode != OCARINA_MODE_EVENT) &&
                (play->csCtx.state == CS_STATE_IDLE)) {
                if (GameInteractor_Should(VB_PLAY_GORON_CHILD_CRY, true)) {
                    Audio_PlaySfx_AtFixedPos(&this->actor.projectedPos, NA_SE_EN_GOLON_KID_CRY - SFX_FLAG);
                }
            }
        } else {
            this->unk_1E4 |= 2;
        }
    }
}

void func_80B5216C(EnGk* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->actionFunc = func_80B51698;
    }
}

void func_80B521E8(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        this->animIndex = ENGK_ANIM_1;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_1);
        this->actionFunc = func_80B5227C;
    }
}

void func_80B5227C(EnGk* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == (endFrame - 1)) {
        func_800B14D4(play, 20.0f, &this->actor.home.pos);
        this->unk_350 = 60;
        if (!(this->unk_1E4 & 0x80)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON);
        }
        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SIT_IMT);
        this->unk_350 = 0x4000;
        this->actionFunc = func_80B52654;
    }
}

void func_80B52340(EnGk* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_1E4 |= 4;
        if (CHECK_EVENTINF(EVENTINF_11)) {
            this->unk_31C = 0xE90;
            this->actionFunc = func_80B52430;
        } else {
            this->unk_31C = 0xE93;
            this->actionFunc = func_80B52430;
        }
        Message_StartTextbox(play, this->unk_31C, &this->actor);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80B52430(EnGk* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        switch (this->unk_31C) {
            case 0xE93:
                this->unk_31C = 0xE89;
                Message_StartTextbox(play, this->unk_31C, &this->actor);
                this->actionFunc = func_80B51B40;
                return;

            case 0xE90:
                this->unk_31C = 0xE91;
                Message_StartTextbox(play, this->unk_31C, &this->actor);
                return;

            case 0xE91:
                this->actionFunc = func_80B5253C;
                return;

            case 0xE92:
                this->unk_1E4 &= ~4;
                this->actionFunc = func_80B51760;
                return;

            default:
                break;
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void func_80B5253C(EnGk* this, PlayState* play) {
    s32 getItemId;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE)) {
        getItemId = GI_GOLD_DUST_2;
    } else {
        getItemId = GI_GOLD_DUST;
    }

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (getItemId == GI_GOLD_DUST) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE);
        }
        this->actionFunc = func_80B525E0;
    } else {
        Actor_OfferGetItem(&this->actor, play, getItemId, 300.0f, 300.0f);
    }
}

void func_80B525E0(EnGk* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_31C = 0xE92;
        Message_StartTextbox(play, this->unk_31C, &this->actor);
        this->actionFunc = func_80B52430;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void func_80B52654(EnGk* this, PlayState* play) {
    this->unk_350 += 0x400;
    if ((this->unk_1E4 & 0x80) && (play->csCtx.curFrame == 250)) {
        SET_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON);
    }

    this->unk_354 = Math_SinS(this->unk_350) * 0.006f * 0.06f;
    this->actor.scale.y = 0.006f - this->unk_354;
    this->actor.scale.z = 0.006f - this->unk_354;
    this->actor.scale.x = 0.006f + this->unk_354;
    if (this->unk_350 == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GOLONKID_SNORE);
    }
}

void EnGk_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGk* this = (EnGk*)thisx;

    SkelAnime_InitFlex(play, &this->skelAnime, &object_gk_Skel_0079C0, &object_gk_Anim_00787C, this->jointTable,
                       this->morphTable, OBJECT_GK_LIMB_MAX);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->unk_2E2 = 20;
    this->unk_2E0 = 0;
    this->unk_1E4 = 0;
    this->unk_354 = 0.006f;
    Actor_SetScale(&this->actor, 0.006f);
    this->actor.gravity = -1.0f;

    if (ENGK_GET_F(&this->actor) == ENGK_F_1) {
        this->animIndex = ENGK_ANIM_5;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_5);
        if (play->sceneId == SCENE_17SETUGEN2) {
            if (Flags_GetSwitch(play, ENGK_GET_SWITCH_FLAG(&this->actor))) {
                Actor_Kill(&this->actor);
                return;
            }
            this->csId = this->actor.csId;
            this->path = SubS_GetPathByIndex(play, ENGK_GET_PATH_INDEX(&this->actor), ENGK_PATH_INDEX_NONE);
            this->actionFunc = func_80B51760;
        } else if (play->sceneId == SCENE_GORONRACE) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
                if (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 1)) {
                    this->actionFunc = func_80B51760;
                } else if (gSaveContext.save.entrance == ENTRANCE(GORON_RACETRACK, 2)) {
                    this->actionFunc = func_80B52340;
                } else {
                    this->actionFunc = func_80B51760;
                }
            } else {
                Actor_Kill(&this->actor);
            }
        } else {
            Actor_Kill(&this->actor);
        }
    } else if (ENGK_GET_F(&this->actor) == ENGK_F_2) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
            this->actionFunc = func_80B51FD0;
            this->actor.draw = NULL;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        } else {
            Actor_Kill(&this->actor);
        }
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
        this->animIndex = ENGK_ANIM_0;
        this->csId = this->actor.csId;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENGK_ANIM_0);
        this->actionFunc = func_80B5202C;
    } else {
        this->actionFunc = func_80B52654;
    }
}

void EnGk_Destroy(Actor* thisx, PlayState* play) {
    EnGk* this = (EnGk*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnGk_Update(Actor* thisx, PlayState* play) {
    EnGk* this = (EnGk*)thisx;

    this->actionFunc(this, play);

    if ((ENGK_GET_F(&this->actor) == ENGK_F_1) ||
        ((ENGK_GET_F(&this->actor) == ENGK_F_0) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON))) {
        func_80B507A0(this, play);
        SkelAnime_Update(&this->skelAnime);
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
        if (ENGK_GET_F(&this->actor) == ENGK_F_1) {
            func_80B5123C(this, play);
        }
        func_80B50B38(this, play);
    }

    if (ENGK_GET_F(&this->actor) == ENGK_F_0) {
        func_80B51510(this, play);
    }
}

s32 EnGk_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    if (limbIndex == OBJECT_GK_LIMB_10) {
        Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnGk_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnGk* this = (EnGk*)thisx;
    Vec3f sp58 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    Vec3f sp40 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp34 = { 1100.0f, -100.0f, 0.0f };
    Vec3f sp28 = { 1100.0f, -100.0f, 0.0f };
    f32 phi_f0;

    sp34.x = 1100.0f;
    sp34.z = 500.0f;

    sp28.x = 1100.0f;
    sp28.z = -500.0f;

    if (this->animIndex == ENGK_ANIM_0) {
        sp34.y = -100.0f;
        sp28.y = -100.0f;

        sp40.x = 2.0f;
        sp40.y = 5.8f;
        sp40.z = 0.0f;

        sp4C = sp40;
        sp4C.x *= -1.0f;
    } else if ((this->animIndex == ENGK_ANIM_2) || (this->animIndex == ENGK_ANIM_9) ||
               (this->animIndex == ENGK_ANIM_10) || (this->animIndex == ENGK_ANIM_11)) {
        sp34.y = 400.0f;
        sp28.y = 400.0f;

        sp40.x = 1.0f;
        sp40.y = 0.8f;
        sp40.z = 0.0f;

        sp4C = sp40;
        sp4C.x *= -1.0f;
    }

    if (limbIndex == OBJECT_GK_LIMB_11) {
        if (ENGK_GET_F(&this->actor) == ENGK_F_1) {
            phi_f0 = 100.0f;
        } else {
            phi_f0 = 50.0f;
        }

        this->actor.focus.pos.x = this->actor.world.pos.x;
        this->actor.focus.pos.y = this->actor.world.pos.y + phi_f0;
        this->actor.focus.pos.z = this->actor.world.pos.z;

        Matrix_MultVec3f(&sp58, &this->actor.focus.pos);
        Matrix_MultVec3f(&sp34, &this->unk_2E8);
        Matrix_MultVec3f(&sp28, &this->unk_2F4);

        Matrix_Push();

        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVec3f(&sp4C, &this->unk_30C);
        Matrix_MultVec3f(&sp40, &this->unk_300);

        Matrix_Pop();
    }
}

void EnGk_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnGk* this = (EnGk*)thisx;
    s32 phi_v0;
    s32 phi_v1;

    switch (limbIndex) {
        case OBJECT_GK_LIMB_12:
            if (this->unk_1E4 & 8) {
                phi_v1 = true;
            } else {
                phi_v1 = false;
            }

            if (this->unk_34E != 0) {
                phi_v0 = true;
            } else {
                phi_v0 = false;
            }
            func_80B50ED4(this->unk_31E + this->unk_322 + 0x4000,
                          this->unk_320 + this->unk_324 + this->actor.shape.rot.y + 0x4000, &this->unk_328,
                          &this->unk_334, phi_v0, phi_v1);

            Matrix_Pop();

            Matrix_Translate(this->unk_328.x, this->unk_328.y, this->unk_328.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_334.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_334.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_334.z, MTXMODE_APPLY);

            Matrix_Push();
            break;

        case OBJECT_GK_LIMB_09:
            if (this->unk_1E4 & 8) {
                phi_v1 = true;
            } else {
                phi_v1 = false;
            }

            if (this->unk_34E != 0) {
                phi_v0 = true;
            } else {
                phi_v0 = false;
            }
            func_80B50ED4(this->unk_322 + 0x4000, this->unk_324 + this->actor.shape.rot.y + 0x4000, &this->unk_33C,
                          &this->unk_348, phi_v0, phi_v1);

            Matrix_Pop();

            Matrix_Translate(this->unk_33C.x, this->unk_33C.y, this->unk_33C.z, MTXMODE_NEW);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_RotateYS(this->unk_348.y, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk_348.x, MTXMODE_APPLY);
            Matrix_RotateZS(this->unk_348.z, MTXMODE_APPLY);

            Matrix_Push();
            break;

        default:
            break;
    }
}

TexturePtr D_80B533E4[] = {
    object_gk_Tex_00F720,
    object_gk_Tex_00FF20,
    object_gk_Tex_010720,
    object_gk_Tex_010F20,
};

void EnGk_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGk* this = (EnGk*)thisx;
    Vec3f pos;
    Vec3f scale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if ((ENGK_GET_F(&this->actor) == ENGK_F_0) && CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
        Matrix_RotateXS(-0x4000, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_gk_DL_006688);
        gSPDisplayList(POLY_OPA_DISP++, object_gk_DL_006680);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        pos = this->actor.world.pos;
        scale.x = 0.2f;
        scale.y = 0.2f;
        scale.z = 0.2f;

        func_800BC620(&pos, &scale, 255, play);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80B533E4[this->unk_2E0]));

        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnGk_OverrideLimbDraw, EnGk_PostLimbDraw,
                                       EnGk_TransformLimbDraw, &this->actor);

        if (ENGK_GET_F(&this->actor) != ENGK_F_2) {
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);
            if ((this->animIndex == ENGK_ANIM_0) || (this->animIndex == ENGK_ANIM_1) ||
                (this->animIndex == ENGK_ANIM_2) || (this->animIndex == ENGK_ANIM_3) ||
                (this->animIndex == ENGK_ANIM_4)) {
                pos.x = this->actor.world.pos.x - 15.0f;
                pos.y = this->actor.world.pos.y;
                pos.z = this->actor.world.pos.z;

                scale.x = 0.2f;
                scale.y = 0.2f;
                scale.z = 0.2f;
            } else {
                pos = this->actor.world.pos;
                scale.x = 0.2f;
                scale.y = 0.2f;
                scale.z = 0.2f;
            }

            func_800BC620(&pos, &scale, 255, play);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
