/*
 * File: z_en_okarina_tag.c
 * Overlay: ovl_En_Okarina_Tag
 * Description:
 */

#include "z_en_okarina_tag.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnOkarinaTag_Init(Actor* thisx, PlayState* play);
void EnOkarinaTag_Destroy(Actor* thisx, PlayState* play);
void EnOkarinaTag_Update(Actor* thisx, PlayState* play);

void func_8093E518(EnOkarinaTag* this, PlayState* play);
void func_8093E68C(EnOkarinaTag* this, PlayState* play);

ActorProfile En_Okarina_Tag_Profile = {
    /**/ ACTOR_EN_OKARINA_TAG,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnOkarinaTag),
    /**/ EnOkarinaTag_Init,
    /**/ EnOkarinaTag_Destroy,
    /**/ EnOkarinaTag_Update,
    /**/ NULL,
};

void EnOkarinaTag_Destroy(Actor* thisx, PlayState* play) {
}

void EnOkarinaTag_Init(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;
    f32 zRot = 0.0f;
    s32 i = 0;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk148 = ENOKARINATAG_GET_F800(thisx);
    this->unk14A = ENOKARINATAG_GET_780(thisx);
    this->switchFlag = ENOKARINATAG_GET_SWITCH_FLAG(thisx);
    if (this->actor.world.rot.z > 0) {
        zRot = this->actor.world.rot.z;
        while (zRot > 10.0f) {
            zRot -= 10.0f;
            i++;
        }
    }
    this->unk154 = zRot * 50.0f;
    this->unk158 = i * 50.0f;
    if (this->switchFlag == ENOKARINATAG_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if (this->unk14A == 0xF) {
        this->unk14A = -1;
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actionFunc = func_8093E518;
}

void func_8093E518(EnOkarinaTag* this, PlayState* play) {
    f32 xzRange;
    f32 yRange;
    s16 yDiff;
    u16 var_v1;

    if (this->switchFlag > SWITCH_FLAG_NONE) {
        if (this->unk148 == 0) {
            if (Flags_GetSwitch(play, this->switchFlag)) {
                return;
            }
        }
        if (this->unk148 == 1) {
            if (!Flags_GetSwitch(play, this->switchFlag)) {
                return;
            }
        }
    }
    var_v1 = this->unk14A;
    if (var_v1 == 6) {
        var_v1 = 0xA;
        if (!gSaveContext.save.saveInfo.scarecrowSpawnSongSet) {
            return;
        }
    }
    if (this->unk14A == -1) {
        var_v1 = 0;
    }
    if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_CHECK_HEALING + var_v1);
        this->actionFunc = func_8093E68C;
    } else {
        yDiff = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));
        if (yDiff >= 0x4300) {
            this->unk150 = 0;
            return;
        }
        xzRange = this->unk154;
        this->unk150 = 1;
        if (xzRange == 0.0f) {
            xzRange = 50000.0f;
        }
        yRange = this->unk158;
        if (yRange == 0.0f) {
            yRange = 50000.0f;
        }
        Actor_OfferOcarinaInteraction(&this->actor, play, xzRange, yRange);
    }
}

void func_8093E68C(EnOkarinaTag* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) {
        this->actionFunc = func_8093E518;
    } else {
        if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) ||
            ((this->unk14A == -1) && ((play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_TIME) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_HEALING) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_EPONAS) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SOARING) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SUNS) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_STORMS) ||
                                      (play->msgCtx.ocarinaMode == OCARINA_MODE_F)))) {
            if (this->switchFlag > SWITCH_FLAG_NONE) {
                switch (this->unk148) {
                    case 0:
                        Flags_SetSwitch(play, this->switchFlag);
                        break;

                    case 1:
                        Flags_UnsetSwitch(play, this->switchFlag);
                        break;

                    case 2:
                        if (Flags_GetSwitch(play, this->switchFlag)) {
                            Flags_UnsetSwitch(play, this->switchFlag);
                        } else {
                            Flags_SetSwitch(play, this->switchFlag);
                        }
                        break;

                    default:
                        break;
                }
            }
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
            Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = func_8093E518;
        }
    }
}

void EnOkarinaTag_Update(Actor* thisx, PlayState* play) {
    EnOkarinaTag* this = (EnOkarinaTag*)thisx;

    this->actionFunc(this, play);
}
