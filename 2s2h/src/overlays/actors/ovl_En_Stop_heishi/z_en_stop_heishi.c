/*
 * File: z_en_stop_heishi.c
 * Overlay: ovl_En_Stop_heishi
 * Description: Clock Town - Gate-Blocking Soldier
 */

#include "z_en_stop_heishi.h"
#include "z64quake.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnStopheishi_Init(Actor* thisx, PlayState* play);
void EnStopheishi_Destroy(Actor* thisx, PlayState* play);
void EnStopheishi_Update(Actor* thisx, PlayState* play);
void EnStopheishi_Draw(Actor* thisx, PlayState* play);

void func_80AE7F34(EnStopheishi* this, PlayState* play);
void func_80AE85C4(EnStopheishi* this, PlayState* play);
void func_80AE7E9C(EnStopheishi* this);
void func_80AE854C(EnStopheishi* this, PlayState* play);
void func_80AE795C(EnStopheishi* this, PlayState* play);

ActorProfile En_Stop_heishi_Profile = {
    /**/ ACTOR_EN_STOP_HEISHI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SDN,
    /**/ sizeof(EnStopheishi),
    /**/ EnStopheishi_Init,
    /**/ EnStopheishi_Destroy,
    /**/ EnStopheishi_Update,
    /**/ EnStopheishi_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 50, 260, 0, { 0, 0, 0 } },
};

static u16 sLeaveMessages[] = {
    0x0516, 0x0517, 0x051A, 0x0000, 0x051C, 0x0000, 0x051E, 0x0000, 0x0520, 0x0521, 0x0524, 0x0000, 0x0526, 0x0000,
    0x0528, 0x0000, 0x052A, 0x052B, 0x052E, 0x0000, 0x0530, 0x0000, 0x0532, 0x0000, 0x0534, 0x0535, 0x0538, 0x0000,
    0x053A, 0x0000, 0x053C, 0x0000, 0x0518, 0x0519, 0x051B, 0x0000, 0x051D, 0x0000, 0x051F, 0x0000, 0x0522, 0x0523,
    0x0525, 0x0000, 0x0527, 0x0000, 0x0529, 0x0000, 0x052C, 0x052D, 0x052F, 0x0000, 0x0531, 0x0000, 0x0533, 0x0000,
    0x0536, 0x0537, 0x0539, 0x0000, 0x053B, 0x0000, 0x053D, 0x0000, 0x0514, 0x0000, 0x0560, 0x0000, 0x0562, 0x0000,
    0x0564, 0x0000, 0x0515, 0x0000, 0x0561, 0x0000, 0x0563, 0x0000, 0x0565, 0x0000,
};

static u16 sThirdDayLeaveMessages[] = {
    0x0540, 0x0541, 0x0542, 0x0000, 0x0543, 0x0000, 0x0543, 0x0000, 0x0547, 0x0548, 0x0549, 0x0000, 0x054A, 0x0000,
    0x054B, 0x0000, 0x054F, 0x0550, 0x0551, 0x0000, 0x0552, 0x0000, 0x0553, 0x0000, 0x0557, 0x0558, 0x0559, 0x0000,
    0x055A, 0x0000, 0x055A, 0x0000, 0x0544, 0x0545, 0x0546, 0x0000, 0x0546, 0x0000, 0x0546, 0x0000, 0x054C, 0x054D,
    0x054E, 0x0000, 0x054E, 0x0000, 0x054E, 0x0000, 0x0554, 0x0555, 0x0556, 0x0000, 0x0556, 0x0000, 0x0556, 0x0000,
    0x055B, 0x055C, 0x055D, 0x0000, 0x055D, 0x0000, 0x055D, 0x0000, 0x053E, 0x0000, 0x053E, 0x0000, 0x053E, 0x0000,
    0x053E, 0x0000, 0x053F, 0x0000, 0x053F, 0x0000, 0x053F, 0x0000, 0x053F, 0x0000,
};

typedef enum {
    /* 0 */ SOLDIER_ANIM_LOOK_DOWN,
    /* 1 */ SOLDIER_ANIM_COME_UP_HERE,
    /* 2 */ SOLDIER_ANIM_STAND_HAND_ON_HIP,
    /* 3 */ SOLDIER_ANIM_STAND_LOOK_DOWN,
    /* 4 */ SOLDIER_ANIM_4,
    /* 5 */ SOLDIER_ANIM_5,
    /* 6 */ SOLDIER_ANIM_6,
    /* 7 */ SOLDIER_ANIM_STAND_HAND_ON_CHEST,
    /* 8 */ SOLDIER_ANIM_MAX
} SoldierAnimation;

static AnimationHeader* sAnimations[SOLDIER_ANIM_MAX] = {
    &gSoldierLookDownAnim,             // SOLDIER_ANIM_LOOK_DOWN
    &gSoldierComeUpHereAnim,           // SOLDIER_ANIM_COME_UP_HERE
    &gSoldierStandHandOnHipAnim,       // SOLDIER_ANIM_STAND_HAND_ON_HIP
    &gSoldierStandAndLookDownAnim,     // SOLDIER_ANIM_STAND_LOOK_DOWN
    &object_sdn_Anim_0057BC,           // SOLDIER_ANIM_4
    &object_sdn_Anim_005D28,           // SOLDIER_ANIM_5
    &object_sdn_Anim_0064C0,           // SOLDIER_ANIM_6
    &gSoldierStandWithHandOnChestAnim, // SOLDIER_ANIM_STAND_HAND_ON_CHEST
};

void EnStopheishi_Init(Actor* thisx, PlayState* play) {
    EnStopheishi* this = (EnStopheishi*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gSoldierSkel, &gSoldierStandHandOnHipAnim, this->jointTable,
                       this->morphTable, SOLDIER_LIMB_MAX);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_276 = ENSTOPHEISHI_GET_F000(&this->actor);
    this->switchFlag = ENSTOPHEISHI_GET_SWITCH_FLAG(&this->actor);
    this->unk_288 = (this->actor.world.rot.z * 40.0f) + 50.0f;
    this->actor.world.rot.z = 0;
    if (this->switchFlag == ENSTOPHEISHI_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.gravity = -3.0f;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->rotYTarget = this->actor.world.rot.y;
    this->headTurnTimer2 = 6;
    func_80AE7E9C(this);
}

void EnStopheishi_Destroy(Actor* thisx, PlayState* play) {
    EnStopheishi* this = (EnStopheishi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnStopHeishi_ChangeAnim(EnStopheishi* this, s32 animIndex) {
    s32 animMode;
    f32 morphFrames;

    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    animMode = ANIMMODE_ONCE;
    morphFrames = -10.0f;
    if ((animIndex >= SOLDIER_ANIM_STAND_HAND_ON_HIP) && (animIndex != SOLDIER_ANIM_4)) {
        animMode = ANIMMODE_LOOP;
    }
    if (animIndex == SOLDIER_ANIM_5) {
        morphFrames = 0.0f;
    }
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f, this->animEndFrame, animMode, morphFrames);
}

void EnStopheishi_UpdateHeadNormal(EnStopheishi* this, PlayState* play) {
    s32 yawDiff;
    Player* player;
    Vec3f playerPos;

    yawDiff = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));

    this->headRotXTarget = 0;

    if (this->actor.xzDistToPlayer < 200.0f) {
        player = GET_PLAYER(play);
        if (yawDiff < 0x4E20) {
            this->headRotXTarget = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
            if (this->headRotXTarget > 0x2710) {
                this->headRotXTarget = 0x2710;
            } else if (this->headRotXTarget < -0x2710) {
                this->headRotXTarget = -0x2710;
            }
        }

        Math_Vec3f_Copy(&playerPos, &player->actor.world.pos);

        if ((player->transformation == PLAYER_FORM_DEKU) || (player->transformation == PLAYER_FORM_HUMAN)) {
            playerPos.y += 57.0f + BREG(34);
        } else {
            playerPos.y += 70.0f + BREG(35);
        }
        this->pitchToPlayer = Math_Vec3f_Pitch(&this->actor.focus.pos, &playerPos);
    }
}

void EnStopHeishi_UpdateHeadThirdDay(EnStopheishi* this) {
    if (this->headTurnTimer1 != 0) {
        this->headTurnTimer1--;
    }
    this->pitchToPlayer = -0xBB8;
    this->headRotXTarget = 0;
    if (this->headTurnTimer2 < 6) {
        this->headRotXTarget = 0x1770;
        if (((u32)this->headTurnTimer2 % 8) == 0) {
            this->headRotXTarget = -0x1770;
        }
        if (this->headTurnTimer1 == 0) {
            this->headTurnTimer2++;
            this->headTurnTimer1 = 10;
            if (this->headTurnTimer2 >= 6) {
                this->headTurnTimer1 = 50;
            }
        }
    } else if ((this->headTurnTimer1 == 0) && (Quake_GetNumActiveQuakes() != 0) && (this->headTurnTimer2 >= 6)) {
        this->headTurnTimer2 = 0;
    }
}

void func_80AE77D4(EnStopheishi* this) {
    this->unk_274 = 0;
    this->maxMoveStep = 0.0f;
    this->rotYTarget = this->actor.home.rot.y;
    if (this->timer == 1) {
        this->pitchToPlayer = 0;
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_5);
    }
    if ((this->timer == 0) && (this->unk_264 == 0)) {
        if ((fabsf(this->actor.world.pos.x - this->actor.home.pos.x) > 3.0f) ||
            (fabsf(this->actor.world.pos.z - this->actor.home.pos.z) > 3.0f)) {
            this->skelAnime.playSpeed = 3.0f;
            Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, 5.0f);
            Math_ApproachF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.3f, 5.0f);
        } else {
            this->unk_264 = 1;
        }
    }
    if ((this->animIndex != SOLDIER_ANIM_STAND_HAND_ON_HIP) && (this->animIndex != SOLDIER_ANIM_STAND_LOOK_DOWN) &&
        (this->animIndex != SOLDIER_ANIM_STAND_HAND_ON_CHEST) && (this->unk_264 != 0)) {
        this->skelAnime.playSpeed = 1.0f;
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
        if ((gSaveContext.save.day != 3) && gSaveContext.save.isNight) {
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_LOOK_DOWN);
        }
        if (gSaveContext.save.day == 3) {
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_CHEST);
        }
    }
}

void func_80AE795C(EnStopheishi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f newPos;
    f32 xDiff;
    f32 zDiff;
    f32 playSpeed;
    s16 yaw;
    s16 rotY;

    if (ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.home.rot.y)) < 0x2000) {
        this->rotYTarget = this->actor.yawTowardsPlayer;
    }
    playSpeed =
        fabsf((f32)this->actor.yawTowardsPlayer - Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos)) *
        0.01f * 5.0f;
    if (this->unk_274 != 4) {
        this->skelAnime.playSpeed = CLAMP(playSpeed, 1.0f, 5.0f);
    }
    yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos);
    xDiff = fabsf(this->actor.home.pos.x - player->actor.world.pos.x);
    zDiff = fabsf(this->actor.home.pos.z - player->actor.world.pos.z);
    Math_Vec3f_Copy(&newPos, &this->actor.home.pos);
    rotY = this->actor.home.rot.y;

    if (rotY == -0x8000) {
        if (yaw > 0) {
            xDiff = Math_SinS(this->actor.home.rot.y + 0xC000) * xDiff;
            zDiff = Math_CosS(this->actor.home.rot.y + 0xC000) * zDiff;
        } else {
            xDiff = Math_SinS(this->actor.home.rot.y + 0x4000) * xDiff;
            zDiff = Math_CosS(this->actor.home.rot.y + 0x4000) * zDiff;
        }
    } else if (yaw < rotY) {
        xDiff = Math_SinS(this->actor.home.rot.y + 0xC000) * xDiff;
        zDiff = Math_CosS(this->actor.home.rot.y + 0xC000) * zDiff;
    } else {
        xDiff = Math_SinS(this->actor.home.rot.y + 0x4000) * xDiff;
        zDiff = Math_CosS(this->actor.home.rot.y + 0x4000) * zDiff;
    }

    if (this->unk_288 < xDiff) {
        xDiff = this->unk_288;
    } else if (xDiff < -this->unk_288) {
        xDiff = -this->unk_288;
    }
    if (this->unk_288 < zDiff) {
        zDiff = this->unk_288;
    } else if (zDiff < -this->unk_288) {
        zDiff = -this->unk_288;
    }
    newPos.x += xDiff;
    newPos.z += zDiff;
    Math_ApproachF(&this->actor.world.pos.x, newPos.x, 0.5f, this->maxMoveStep);
    Math_ApproachF(&this->actor.world.pos.z, newPos.z, 0.5f, this->maxMoveStep);
    Math_ApproachF(&this->maxMoveStep, 50.0f, 0.3f, 1.0f);

    switch (this->unk_274) {
        case 0:
            if (gSaveContext.save.day != 3) {
                EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_4);
                this->unk_274 = 2;
            } else {
                EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
                this->unk_274 = 1;
            }
            break;

        case 1:
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_4);
            this->unk_274 = 2;
            break;

        case 2:
            if (curFrame >= this->animEndFrame) {
                EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_5);
                this->unk_274 = 3;
            }
            break;

        case 3:
            if ((fabsf(this->actor.world.pos.x - newPos.x) < 2.0f) &&
                (fabsf(this->actor.world.pos.z - newPos.z) < 2.0f)) {
                this->skelAnime.playSpeed = 1.0f;
                EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_6);
                this->unk_274 = 4;
            }
            break;

        case 4:
            if ((fabsf(this->actor.world.pos.x - newPos.x) > 4.0f) ||
                (fabsf(this->actor.world.pos.z - newPos.z) > 4.0f)) {
                EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_5);
                this->unk_274 = 3;
            }
            break;
    }
    this->unk_264 = 0;
    this->timer = 20;
    this->pitchToPlayer = 0;
    if (this->unk_276 == 3) {
        Math_Vec3f_Copy(&newPos, &player->actor.world.pos);
        if ((player->transformation == PLAYER_FORM_DEKU) || (player->transformation == PLAYER_FORM_HUMAN)) {
            newPos.y += 77.0f + BREG(37);
            this->pitchToPlayer = Math_Vec3f_Pitch(&this->actor.focus.pos, &newPos);
        }
    }
}

void func_80AE7E9C(EnStopheishi* this) {
    EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
    if ((gSaveContext.save.day != 3) && gSaveContext.save.isNight) {
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_LOOK_DOWN);
    }
    if (gSaveContext.save.day == 3) {
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_CHEST);
    }
    this->unk_274 = 0;
    this->disableCollider = false;
    this->actionFunc = func_80AE7F34;
    this->actor.speed = 0.0f;
}

void func_80AE7F34(EnStopheishi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp32;
    s16 phi_a2;
    s32 yawDiffAbs;
    s16 yawDiff;
    f32 xDiff;
    f32 zDiff;

    SkelAnime_Update(&this->skelAnime);
    if ((this->animIndex == SOLDIER_ANIM_5) && ((TRUNCF_BINANG(this->skelAnime.curFrame) % 2) != 0)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SOLDIER_WALK);
    }
    if (gSaveContext.save.day != 3) {
        EnStopheishi_UpdateHeadNormal(this, play);
    } else {
        EnStopHeishi_UpdateHeadThirdDay(this);
    }
    Math_SmoothStepToS(&this->actor.world.rot.y, this->rotYTarget, 1, 0x1388, 0);

    sp32 = 0;
    if (gSaveContext.save.isNight) {
        sp32 = 0x20;
    }

    phi_a2 = 0;
    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            if ((STOLEN_ITEM_1 == SLOT_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_1 == ITEM_SWORD_KOKIRI) ||
                (STOLEN_ITEM_1 == ITEM_SWORD_RAZOR) || (STOLEN_ITEM_1 == ITEM_SWORD_GILDED) ||
                (STOLEN_ITEM_2 == SLOT_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_2 == ITEM_SWORD_KOKIRI) ||
                (STOLEN_ITEM_2 == ITEM_SWORD_RAZOR) || (STOLEN_ITEM_2 == ITEM_SWORD_GILDED)) {
                if (this->unk_265 != 0) {
                    phi_a2 = 1;
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_12_20)) {
                phi_a2 = 1;
            }
            break;

        case PLAYER_FORM_DEKU:
            phi_a2 = 4;
            break;

        case PLAYER_FORM_ZORA:
            phi_a2 = 3;
            break;

        case PLAYER_FORM_GORON:
            phi_a2 = 2;
            break;

        // 2S2H [FD Enhancement] - Guard treats FD as if Human has already talked
        case PLAYER_FORM_FIERCE_DEITY:
            phi_a2 = 1;
            break;
    }

    if (((phi_a2 == 1) || (phi_a2 == 2) || (phi_a2 == 3)) &&
        (((this->animIndex == SOLDIER_ANIM_4)) || (this->animIndex == SOLDIER_ANIM_5) ||
         (this->animIndex == SOLDIER_ANIM_6))) {
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
        if ((gSaveContext.save.day != 3) && gSaveContext.save.isNight) {
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_LOOK_DOWN);
        }
        if (gSaveContext.save.day == 3) {
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_CHEST);
        }
    }

    if ((phi_a2 == 0) || (phi_a2 == 4)) {
        xDiff = this->actor.home.pos.x - player->actor.world.pos.x;
        zDiff = this->actor.home.pos.z - player->actor.world.pos.z;
        this->collider.dim.radius = 50;
        this->collider.dim.height = 260;
        if (sqrtf(SQ(xDiff) + SQ(zDiff)) < 240.0f) {
            func_80AE795C(this, play);
        } else {
            func_80AE77D4(this);
        }
    } else {
        this->collider.dim.radius = 15;
        this->collider.dim.height = 60;
    }
    if (phi_a2 != 4) {
        this->messageIndex = this->unk_276 * 8;
        this->messageIndex += sp32;
        this->messageIndex += phi_a2 * 2;
    } else {
        this->messageIndex = 64;
        if (sp32 != 0) {
            this->messageIndex = 72;
        }
        this->messageIndex += this->unk_276 * 2;
    }
    if (gSaveContext.save.day != 3) {
        this->actor.textId = sLeaveMessages[this->messageIndex];
        this->unk_27E = sLeaveMessages[this->messageIndex + 1];
    } else {
        this->actor.textId = sThirdDayLeaveMessages[this->messageIndex];
        this->unk_27E = sThirdDayLeaveMessages[this->messageIndex + 1];
    }
    if (this->unk_27E != 0) {
        if ((STOLEN_ITEM_1 == ITEM_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_1 == ITEM_SWORD_KOKIRI) ||
            (STOLEN_ITEM_1 == ITEM_SWORD_RAZOR) || (STOLEN_ITEM_1 == ITEM_SWORD_GILDED) ||
            (STOLEN_ITEM_2 == ITEM_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_2 == ITEM_SWORD_KOKIRI) ||
            (STOLEN_ITEM_2 == ITEM_SWORD_RAZOR) || (STOLEN_ITEM_2 == ITEM_SWORD_GILDED)) {
            if (this->actor.textId == 0x516) {
                this->actor.textId = 0x56C;
            } else if (this->actor.textId == 0x520) {
                this->actor.textId = 0x56E;
            } else if (this->actor.textId == 0x52A) {
                this->actor.textId = 0x570;
            } else if (this->actor.textId == 0x534) {
                this->actor.textId = 0x572;
            } else if (this->actor.textId == 0x518) {
                this->actor.textId = 0x56D;
            } else if (this->actor.textId == 0x522) {
                this->actor.textId = 0x56F;
            } else if (this->actor.textId == 0x52C) {
                this->actor.textId = 0x571;
            } else if (this->actor.textId == 0x536) {
                this->actor.textId = 0x573;
            }
        } else if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
            if (this->actor.textId == 0x516) {
                this->actor.textId = 0x55E;
            } else if (this->actor.textId == 0x520) {
                this->actor.textId = 0x566;
            } else if (this->actor.textId == 0x52A) {
                this->actor.textId = 0x568;
            } else if (this->actor.textId == 0x534) {
                this->actor.textId = 0x56A;
            } else if (this->actor.textId == 0x518) {
                this->actor.textId = 0x55F;
            } else if (this->actor.textId == 0x522) {
                this->actor.textId = 0x567;
            } else if (this->actor.textId == 0x52C) {
                this->actor.textId = 0x569;
            } else if (this->actor.textId == 0x536) {
                this->actor.textId = 0x56B;
            }
        }
    }
    yawDiff = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
    yawDiffAbs = ABS_ALT(yawDiff);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->skelAnime.playSpeed = 1.0f;
        func_80AE854C(this, play);
    } else if (yawDiffAbs < 0x4BB9) {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80AE854C(EnStopheishi* this, PlayState* play) {
    if (((this->unk_265 != 0) || CHECK_WEEKEVENTREG(WEEKEVENTREG_12_20)) &&
        (this->animIndex != SOLDIER_ANIM_STAND_HAND_ON_HIP)) {
        EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
    }
    this->pitchToPlayer = 0;
    this->unk_274 = 0;
    this->disableCollider = true;
    this->actionFunc = func_80AE85C4;
}

void func_80AE85C4(EnStopheishi* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnStopheishi_UpdateHeadNormal(this, play);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_27E != 0) {
            this->actor.textId = this->unk_27E;
            Message_ContinueTextbox(play, this->actor.textId);
            EnStopHeishi_ChangeAnim(this, SOLDIER_ANIM_STAND_HAND_ON_HIP);
            SET_WEEKEVENTREG(WEEKEVENTREG_12_20);
            this->unk_265 = 1;
            this->unk_27E = 0;
        } else {
            Message_CloseTextbox(play);
            func_80AE7E9C(this);
        }
    }
}

void EnStopheishi_Update(Actor* thisx, PlayState* play) {
    EnStopheishi* this = (EnStopheishi*)thisx;
    s32 pad;

    if (this->timer != 0) {
        this->timer--;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_SmoothStepToS(&this->headRotX, this->headRotXTarget, 1, 0x7D0, 0);
    Math_SmoothStepToS(&this->headRotZ, this->pitchToPlayer, 1, 0x7D0, 0);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.cullingVolumeDistance = 500.0f;
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->headWorldPos);
    Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
    if (!this->disableCollider) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnStopheishi_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnStopheishi* this = (EnStopheishi*)thisx;

    if (limbIndex == SOLDIER_LIMB_HEAD) {
        rot->x += this->headRotX;
        rot->z += this->headRotZ;
    }
    return false;
}

void EnStopheishi_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnStopheishi* this = (EnStopheishi*)thisx;

    if (limbIndex == SOLDIER_LIMB_HEAD) {
        Matrix_MultVec3f(&gZeroVec3f, &this->headWorldPos);
    }
}

void EnStopheishi_Draw(Actor* thisx, PlayState* play) {
    EnStopheishi* this = (EnStopheishi*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnStopheishi_OverrideLimbDraw, EnStopheishi_PostLimbDraw, &this->actor);
}
