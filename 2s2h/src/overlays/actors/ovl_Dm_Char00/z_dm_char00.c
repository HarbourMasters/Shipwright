/*
 * File: z_dm_char00.c
 * Overlay: ovl_Dm_Char00
 * Description: Tatl and Tael (cutscene)
 */

#include "z_dm_char00.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_delf/object_delf.h"
#include "z64animation.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmChar00_Init(Actor* thisx, PlayState* play);
void DmChar00_Destroy(Actor* thisx, PlayState* play);
void DmChar00_Update(Actor* thisx, PlayState* play);
void DmChar00_Draw(Actor* thisx, PlayState* play2);

void func_80AA67F8(DmChar00* this, PlayState* play);
void func_80AA695C(DmChar00* this, PlayState* play);

ActorProfile Dm_Char00_Profile = {
    /**/ ACTOR_DM_CHAR00,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_DELF,
    /**/ sizeof(DmChar00),
    /**/ DmChar00_Init,
    /**/ DmChar00_Destroy,
    /**/ DmChar00_Update,
    /**/ DmChar00_Draw,
};

typedef enum {
    /*   -1 */ DMCHAR00_ANIM_NONE = -1,
    /* 0x00 */ DMCHAR00_ANIM_0,
    /* 0x01 */ DMCHAR00_ANIM_1,
    /* 0x02 */ DMCHAR00_ANIM_2,
    /* 0x03 */ DMCHAR00_ANIM_3,
    /* 0x04 */ DMCHAR00_ANIM_4,
    /* 0x05 */ DMCHAR00_ANIM_5,
    /* 0x06 */ DMCHAR00_ANIM_6,
    /* 0x07 */ DMCHAR00_ANIM_7,
    /* 0x08 */ DMCHAR00_ANIM_8,
    /* 0x09 */ DMCHAR00_ANIM_9,
    /* 0x0A */ DMCHAR00_ANIM_10,
    /* 0x0B */ DMCHAR00_ANIM_11,
    /* 0x0C */ DMCHAR00_ANIM_12,
    /* 0x0D */ DMCHAR00_ANIM_13,
    /* 0x0E */ DMCHAR00_ANIM_14,
    /* 0x0F */ DMCHAR00_ANIM_15,
    /* 0x10 */ DMCHAR00_ANIM_16,
    /* 0x11 */ DMCHAR00_ANIM_17,
    /* 0x12 */ DMCHAR00_ANIM_18,
    /* 0x13 */ DMCHAR00_ANIM_19,
    /* 0x14 */ DMCHAR00_ANIM_20,
    /* 0x15 */ DMCHAR00_ANIM_21,
    /* 0x16 */ DMCHAR00_ANIM_22,
    /* 0x17 */ DMCHAR00_ANIM_23,
    /* 0x18 */ DMCHAR00_ANIM_24,
    /* 0x19 */ DMCHAR00_ANIM_25,
    /* 0x1A */ DMCHAR00_ANIM_26,
    /* 0x1B */ DMCHAR00_ANIM_27,
    /* 0x1C */ DMCHAR00_ANIM_28,
    /* 0x1D */ DMCHAR00_ANIM_29,
    /* 0x1E */ DMCHAR00_ANIM_30,
    /* 0x1F */ DMCHAR00_ANIM_31,
    /* 0x20 */ DMCHAR00_ANIM_32,
    /* 0x21 */ DMCHAR00_ANIM_33,
    /* 0x22 */ DMCHAR00_ANIM_34,
    /* 0x23 */ DMCHAR00_ANIM_35,
    /* 0x24 */ DMCHAR00_ANIM_36,
    /* 0x25 */ DMCHAR00_ANIM_37,
    /* 0x26 */ DMCHAR00_ANIM_38,
    /* 0x27 */ DMCHAR00_ANIM_39,
    /* 0x28 */ DMCHAR00_ANIM_40,
    /* 0x29 */ DMCHAR00_ANIM_41,
    /* 0x2A */ DMCHAR00_ANIM_42,
    /* 0x2B */ DMCHAR00_ANIM_43,
    /* 0x2C */ DMCHAR00_ANIM_44,
    /* 0x2D */ DMCHAR00_ANIM_45,
    /* 0x2E */ DMCHAR00_ANIM_46,
    /* 0x2F */ DMCHAR00_ANIM_47,
    /* 0x30 */ DMCHAR00_ANIM_48,
    /* 0x31 */ DMCHAR00_ANIM_49,
    /* 0x32 */ DMCHAR00_ANIM_50,
    /* 0x33 */ DMCHAR00_ANIM_51,
    /* 0x34 */ DMCHAR00_ANIM_52,
    /* 0x35 */ DMCHAR00_ANIM_53,
    /* 0x36 */ DMCHAR00_ANIM_54,
    /* 0x37 */ DMCHAR00_ANIM_55,
    /* 0x38 */ DMCHAR00_ANIM_56,
    /* 0x39 */ DMCHAR00_ANIM_57,
    /* 0x3A */ DMCHAR00_ANIM_58,
    /* 0x3B */ DMCHAR00_ANIM_59,
    /* 0x3C */ DMCHAR00_ANIM_60,
    /* 0x3D */ DMCHAR00_ANIM_61,
    /* 0x3E */ DMCHAR00_ANIM_62,
    /* 0x3F */ DMCHAR00_ANIM_63,
    /* 0x40 */ DMCHAR00_ANIM_64,
    /* 0x41 */ DMCHAR00_ANIM_65,
    /* 0x42 */ DMCHAR00_ANIM_66,
    /* 0x43 */ DMCHAR00_ANIM_67,
    /* 0x44 */ DMCHAR00_ANIM_68,
    /* 0x45 */ DMCHAR00_ANIM_69,
    /* 0x46 */ DMCHAR00_ANIM_70,
    /* 0x47 */ DMCHAR00_ANIM_71,
    /* 0x48 */ DMCHAR00_ANIM_72,
    /* 0x49 */ DMCHAR00_ANIM_73,
    /* 0x4A */ DMCHAR00_ANIM_74,
    /* 0x4B */ DMCHAR00_ANIM_75,
    /* 0x4C */ DMCHAR00_ANIM_76,
    /* 0x4D */ DMCHAR00_ANIM_77,
    /* 0x4E */ DMCHAR00_ANIM_78,
    /* 0x4F */ DMCHAR00_ANIM_79,
    /* 0x50 */ DMCHAR00_ANIM_80,
    /* 0x51 */ DMCHAR00_ANIM_81,
    /* 0x52 */ DMCHAR00_ANIM_82,
    /* 0x53 */ DMCHAR00_ANIM_83,
    /* 0x54 */ DMCHAR00_ANIM_84,
    /* 0x55 */ DMCHAR00_ANIM_MAX
} DmChar00Animation;

static AnimationInfo sAnimationInfo[DMCHAR00_ANIM_MAX] = {
    { &gameplay_keep_Anim_02B2E8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },        // DMCHAR00_ANIM_0
    { &gameplay_keep_Anim_029140, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },        // DMCHAR00_ANIM_1
    { &object_delf_Anim_004FF4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_2
    { &object_delf_Anim_0053A4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_3
    { &object_delf_Anim_005B68, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_4
    { &object_delf_Anim_006328, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_5
    { &object_delf_Anim_00FB30, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_6
    { &object_delf_Anim_010590, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_7
    { &object_delf_Anim_005B68, 1.0f, 37.0f, 56.0f, ANIMMODE_LOOP_PARTIAL, 0.0f }, // DMCHAR00_ANIM_8
    { &object_delf_Anim_006328, 1.0f, 37.0f, 56.0f, ANIMMODE_LOOP_PARTIAL, 0.0f }, // DMCHAR00_ANIM_9
    { &object_delf_Anim_00E024, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_10
    { &object_delf_Anim_00F0D0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_11
    { &object_delf_Anim_011C70, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_12
    { &object_delf_Anim_011FE0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_13
    { &object_delf_Anim_011088, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_14
    { &object_delf_Anim_0118FC, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_15
    { &object_delf_Anim_012388, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_16
    { &object_delf_Anim_012738, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_17
    { &object_delf_Anim_013BE0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_18
    { &object_delf_Anim_013E80, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_19
    { &object_delf_Anim_007B04, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_20
    { &object_delf_Anim_0080A4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_21
    { &object_delf_Anim_00859C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_22
    { &object_delf_Anim_008A44, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_23
    { &object_delf_Anim_00EB18, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_24
    { &object_delf_Anim_00EBB0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_25
    { &object_delf_Anim_00EE30, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_26
    { &object_delf_Anim_012BBC, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_27
    { &object_delf_Anim_012EF8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_28
    { &object_delf_Anim_00C248, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_29
    { &object_delf_Anim_00C588, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_30
    { &object_delf_Anim_00BDFC, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_31
    { &object_delf_Anim_00C0BC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_32
    { &object_delf_Anim_013040, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_33
    { &object_delf_Anim_013378, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_34
    { &object_delf_Anim_0091BC, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_35
    { &object_delf_Anim_0094B0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_36
    { &object_delf_Anim_009C4C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_37
    { &object_delf_Anim_009F40, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_38
    { &object_delf_Anim_009010, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_39
    { &object_delf_Anim_0097C4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_40
    { &object_delf_Anim_00A7B4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_41
    { &object_delf_Anim_00C588, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },        // DMCHAR00_ANIM_42
    { &object_delf_Anim_004FF4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -10.0f },        // DMCHAR00_ANIM_43
    { &object_delf_Anim_011C70, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },        // DMCHAR00_ANIM_44
    { &object_delf_Anim_006B28, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_45
    { &object_delf_Anim_014190, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_46
    { &object_delf_Anim_01447C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_47
    { &object_delf_Anim_003FAC, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_48
    { &object_delf_Anim_004798, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_49
    { &object_delf_Anim_0042E8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_50
    { &object_delf_Anim_004C44, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_51
    { &object_delf_Anim_01088C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_52
    { &object_delf_Anim_01498C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_53
    { &object_delf_Anim_014E9C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_54
    { &object_delf_Anim_00CEE0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_55
    { &object_delf_Anim_00D260, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_56
    { &object_delf_Anim_00C690, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_57
    { &object_delf_Anim_00C790, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_58
    { &object_delf_Anim_00AF78, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_59
    { &object_delf_Anim_00B2B8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_60
    { &object_delf_Anim_00BCD8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_61
    { &object_delf_Anim_006D74, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_62
    { &object_delf_Anim_00706C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_63
    { &object_delf_Anim_0072B0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_64
    { &object_delf_Anim_007570, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_65
    { &object_delf_Anim_00A2A4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_66
    { &object_delf_Anim_00A4B4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_67
    { &object_delf_Anim_006614, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_68
    { &object_delf_Anim_015114, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_69
    { &object_delf_Anim_00CCD4, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_70
    { &object_delf_Anim_00046C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_71
    { &object_delf_Anim_00065C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_72
    { &object_delf_Anim_00377C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_73
    { &object_delf_Anim_003148, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_74
    { &object_delf_Anim_001410, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_75
    { &object_delf_Anim_0010F8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_76
    { &object_delf_Anim_00199C, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_77
    { &object_delf_Anim_003BB4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_78
    { &object_delf_Anim_001754, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_79
    { &object_delf_Anim_0008E0, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_80
    { &object_delf_Anim_000A94, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_81
    { &object_delf_Anim_001ED4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_82
    { &object_delf_Anim_001B28, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // DMCHAR00_ANIM_83
    { &object_delf_Anim_000E44, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // DMCHAR00_ANIM_84
};

Color_RGBAf D_80AA77A8[] = {
    { 250.0f, 255.0f, 230.0f, 255.0f },
    { 63.0f, 18.0f, 93.0f, 255.0f },
    { 255.0f, 235.0f, 220.0f, 255.0f },
};

Color_RGBAf D_80AA77D8[] = {
    { 220.0f, 160.0f, 80.0f, 255.0f },
    { 250.0f, 40.0f, 10.0f, 255.0f },
    { 255.0f, 235.0f, 220.0f, 255.0f },
};

Vec3f D_80AA7808 = { 0.0f, 0.0f, 0.0f };

void DmChar00_ChangeAnim(SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
    f32 endFrame;

    animInfo += animIndex;

    if (animInfo->frameCount < 0.0f) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

void func_80AA561C(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame + 20) {
            case 503:
            case 926:
            case 979:
            case 1222:
            case 1682:
            case 2194:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 1858:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_ATTACK);
                break;

            case 2043:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
                break;

            default:
                break;
        }
    } else {
        switch (play->csCtx.curFrame + 20) {
            case 503:
            case 926:
            case 979:
            case 1234:
            case 1687:
            case 2194:
            case 2210:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            case 2043:
                Actor_PlaySfx(&this->actor, NA_SE_EV_MONDO_SURPRISE);
                break;

            default:
                break;
        }
    }
}

void func_80AA5720(DmChar00* this, PlayState* play) {
    if ((DMCHAR00_GET(&this->actor) == DMCHAR00_0) && (play->csCtx.curFrame == 505)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SPOT_LIGHT_OPEN);
    }
}

void func_80AA575C(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 474:
            case 489:
            case 495:
            case 505:
            case 854:
            case 860:
            case 866:
            case 872:
            case 877:
            case 882:
            case 887:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_ATTACK);
                break;

            case 520:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BELL_SPIT);
                break;

            case 774:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
                break;

            case 904:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BELL_SIGH);
                break;

            case 813:
            case 972:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BELL_BRAKE);
                break;

            default:
                break;
        }
    } else if (play->csCtx.curFrame == 660) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_MONDO_SURPRISE);
    }
}

void func_80AA5890(DmChar00* this, PlayState* play) {
    if ((DMCHAR00_GET(&this->actor) == DMCHAR00_0) && (play->csCtx.curFrame == 20)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
    }
}

void func_80AA58CC(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        if (play->csCtx.curFrame == 568) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
        }
    } else {
        switch (play->csCtx.curFrame) {
            case 375:
            case 479:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            case 534:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_ATTACK);
                break;

            default:
                break;
        }
    }
}

void func_80AA5950(DmChar00* this, PlayState* play) {
}

void func_80AA5960(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        if (play->csCtx.curFrame == 280) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
        }
    } else {
        switch (play->csCtx.curFrame) {
            case 87:
            case 190:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            case 244:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_ATTACK);
                break;

            default:
                break;
        }
    }
}

void func_80AA59E4(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 125:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 1832:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                Actor_PlaySfx(&this->actor, NA_SE_SY_WHITE_OUT_INTO_MOON);
                break;

            default:
                break;
        }
    } else if (play->csCtx.curFrame == 125) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
    }
}

void func_80AA5A6C(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 44:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 891:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                Actor_PlaySfx(&this->actor, NA_SE_SY_WHITE_OUT_INTO_MOON);
                break;

            default:
                break;
        }
    } else if (play->csCtx.curFrame == 44) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
    }
}

void func_80AA5AF4(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 352:
            case 401:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 440:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 550:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BELL_BRAKE);
                break;

            default:
                break;
        }
    } else {
        switch (play->csCtx.curFrame) {
            case 362:
            case 401:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            case 454:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            default:
                break;
        }

        if ((play->csCtx.curFrame >= 500) && (play->csCtx.curFrame < 602)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SHIVER - SFX_FLAG);
        }
    }
}

void func_80AA5BF8(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 762:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_SHOT_DASH);
                break;
            case 797:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            default:
                break;
        }
    } else {
        switch (play->csCtx.curFrame) {
            case 762:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_SHOT_DASH);
                break;

            case 797:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            default:
                break;
        }
    }

    if ((this->animIndex == DMCHAR00_ANIM_53) && Animation_OnFrame(&this->skelAnime, 16.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
    }
}

void func_80AA5CD4(DmChar00* this, PlayState* play) {
    if ((DMCHAR00_GET(&this->actor) != DMCHAR00_0) && (play->csCtx.curFrame == 467)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
    }
}

void func_80AA5D10(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 8:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
                break;

            case 130:
                Actor_PlaySfx(&this->actor, NA_SE_EV_NAVY_VANISH);
                break;

            default:
                break;
        }
    }
}

void func_80AA5D6C(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 2:
            case 166:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 31:
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_SURPRISE);
                break;

            default:
                break;
        }
    }
}

void func_80AA5DC8(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_0) {
        switch (play->csCtx.curFrame) {
            case 233:
            case 415:
            case 593:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 130:
                Actor_PlaySfx(&this->actor, NA_SE_EV_NAVY_VANISH);
                break;

            default:
                break;
        }
    }
}

void func_80AA5E2C(DmChar00* this, PlayState* play) {
    if (DMCHAR00_GET(&this->actor) == DMCHAR00_1) {
        switch (play->csCtx.curFrame) {
            case 32:
                Actor_PlaySfx(&this->actor, NA_SE_EV_WHITE_FAIRY_DASH);
                break;

            case 42:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BLACK_FAIRY_DASH);
                break;

            case 192:
                Actor_PlaySfx(&this->actor, NA_SE_EV_BELL_BRAKE);
                break;

            case 215:
                Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_INTO_WEED);
                break;

            default:
                break;
        }
    }
}

void func_80AA5EBC(DmChar00* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        switch (play->sceneId) {
            case SCENE_LOST_WOODS:
                if (gSaveContext.sceneLayer == 1) {
                    func_80AA561C(this, play);
                }
                break;

            case SCENE_OPENINGDAN:
                if (gSaveContext.sceneLayer == 0) {
                    if (play->csCtx.scriptIndex == 0) {
                        func_80AA5720(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        func_80AA575C(this, play);
                    } else if (play->csCtx.scriptIndex == 2) {
                        func_80AA5890(this, play);
                    }
                }
                break;

            case SCENE_OKUJOU:
                if (gSaveContext.sceneLayer == 0) {
                    if (play->csCtx.scriptIndex == 0) {
                        func_80AA58CC(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        func_80AA5950(this, play);
                    } else if (play->csCtx.scriptIndex == 2) {
                        func_80AA5960(this, play);
                    }
                } else if (gSaveContext.sceneLayer == 2) {
                    if (play->csCtx.scriptIndex == 0) {
                        func_80AA59E4(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        func_80AA5A6C(this, play);
                    }
                }
                break;

            case SCENE_00KEIKOKU:
                if (gSaveContext.sceneLayer == 3) {
                    if (play->csCtx.scriptIndex == 0) {
                        func_80AA5AF4(this, play);
                    } else if (play->csCtx.scriptIndex == 2) {
                        func_80AA5E2C(this, play);
                    }
                } else if (gSaveContext.sceneLayer == 7) {
                    if (play->csCtx.scriptIndex == 0) {
                        func_80AA5BF8(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        func_80AA5CD4(this, play);
                    }
                }
                break;

            case SCENE_MITURIN:
                if ((gSaveContext.sceneLayer == 0) && (play->csCtx.scriptIndex == 1)) {
                    func_80AA5DC8(this, play);
                }
                break;

            case SCENE_INSIDETOWER:
                if ((gSaveContext.sceneLayer == 0) && (play->csCtx.scriptIndex == 0)) {
                    func_80AA5D10(this, play);
                }
                break;

            case SCENE_PIRATE:
                if ((gSaveContext.sceneLayer == 0) && (play->csCtx.scriptIndex == 0)) {
                    func_80AA5D6C(this, play);
                }
                break;

            default:
                break;
        }
    }
}

void DmChar00_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DmChar00* this = (DmChar00*)thisx;

    if ((play->sceneId == SCENE_LOST_WOODS) && !Cutscene_IsPlaying(play)) {
        Actor_Kill(thisx);
    }

    this->unk_240 = D_80AA77A8[DMCHAR00_GET(thisx)];
    this->unk_250 = D_80AA77D8[DMCHAR00_GET(thisx)];

    thisx->lockOnArrowOffset = 3000.0f;
    this->cueId = 99;
    this->unk_262 = DMCHAR00_GET_F800(thisx);

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_Init(play, &this->skelAnime, gameplay_keep_Skel_02AF58, &gameplay_keep_Anim_029140, this->jointTable,
                   this->morphTable, FAIRY_LIMB_MAX);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 15.0f);
    DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[DMCHAR00_ANIM_0], 0);
    Actor_SetScale(thisx, 0.01f);
    this->actionFunc = func_80AA67F8;
}

void DmChar00_Destroy(Actor* thisx, PlayState* play) {
}

void DmChar00_HandleCutscene(DmChar00* this, PlayState* play) {
    u16 cueType = CS_CMD_ACTOR_CUE_113 + DMCHAR00_GET(&this->actor);
    s32 cueChannel;
    s32 pad;

    if (Cutscene_IsCueInChannel(play, cueType)) {
        cueChannel = Cutscene_GetCueChannel(play, cueType);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
                this->cueId = play->csCtx.actorCues[cueChannel]->id;

                switch (play->csCtx.actorCues[cueChannel]->id) {
                    case 0x1:
                        this->animIndex = DMCHAR00_ANIM_0;
                        break;

                    case 0x2:
                        this->animIndex = DMCHAR00_ANIM_2;
                        break;

                    case 0x3:
                        this->animIndex = DMCHAR00_ANIM_3;
                        break;

                    case 0x4:
                        this->animIndex = DMCHAR00_ANIM_4;
                        break;

                    case 0x5:
                        this->animIndex = DMCHAR00_ANIM_5;
                        break;

                    case 0x6:
                        this->animIndex = DMCHAR00_ANIM_6;
                        break;

                    case 0x7:
                        this->animIndex = DMCHAR00_ANIM_7;
                        break;

                    case 0x8:
                        this->animIndex = DMCHAR00_ANIM_20;
                        break;

                    case 0x9:
                        this->animIndex = DMCHAR00_ANIM_21;
                        break;

                    case 0xA:
                        this->animIndex = DMCHAR00_ANIM_14;
                        break;

                    case 0xB:
                        this->animIndex = DMCHAR00_ANIM_15;
                        break;

                    case 0xC:
                        this->animIndex = DMCHAR00_ANIM_18;
                        break;

                    case 0xD:
                        this->animIndex = DMCHAR00_ANIM_25;
                        break;

                    case 0xE:
                        this->animIndex = DMCHAR00_ANIM_27;
                        break;

                    case 0xF:
                        this->animIndex = DMCHAR00_ANIM_24;
                        break;

                    case 0x10:
                        this->animIndex = DMCHAR00_ANIM_10;
                        break;

                    case 0x11:
                        this->animIndex = DMCHAR00_ANIM_12;
                        break;

                    case 0x12:
                        this->animIndex = DMCHAR00_ANIM_13;
                        break;

                    case 0x13:
                        this->animIndex = DMCHAR00_ANIM_29;
                        break;

                    case 0x14:
                        this->animIndex = DMCHAR00_ANIM_31;
                        break;

                    case 0x15:
                        this->animIndex = DMCHAR00_ANIM_33;
                        break;

                    case 0x16:
                        Actor_Kill(&this->actor);
                        break;

                    case 0x17:
                        this->animIndex = DMCHAR00_ANIM_35;
                        break;

                    case 0x18:
                        this->animIndex = DMCHAR00_ANIM_37;
                        break;

                    case 0x19:
                        this->animIndex = DMCHAR00_ANIM_39;
                        break;

                    case 0x1A:
                        this->animIndex = DMCHAR00_ANIM_41;
                        break;

                    case 0x1B:
                        this->animIndex = DMCHAR00_ANIM_45;
                        break;

                    case 0x1C:
                        this->animIndex = DMCHAR00_ANIM_46;
                        break;

                    case 0x1E:
                        this->animIndex = DMCHAR00_ANIM_48;
                        break;

                    case 0x1F:
                        this->animIndex = DMCHAR00_ANIM_50;
                        break;

                    case 0x20:
                        this->animIndex = DMCHAR00_ANIM_52;
                        break;

                    case 0x21:
                        this->animIndex = DMCHAR00_ANIM_53;
                        break;

                    case 0x23:
                        this->animIndex = DMCHAR00_ANIM_55;
                        break;

                    case 0x24:
                        this->animIndex = DMCHAR00_ANIM_57;
                        break;

                    case 0x25:
                        this->animIndex = DMCHAR00_ANIM_58;
                        break;

                    case 0x26:
                        this->animIndex = DMCHAR00_ANIM_59;
                        break;

                    case 0x27:
                        this->animIndex = DMCHAR00_ANIM_61;
                        break;

                    case 0x28:
                        this->animIndex = DMCHAR00_ANIM_62;
                        break;

                    case 0x29:
                        this->animIndex = DMCHAR00_ANIM_64;
                        break;

                    case 0x2A:
                        this->animIndex = DMCHAR00_ANIM_66;
                        break;

                    case 0x2B:
                        this->animIndex = DMCHAR00_ANIM_68;
                        break;

                    case 0x2C:
                        this->animIndex = DMCHAR00_ANIM_69;
                        break;

                    case 0x2D:
                        this->animIndex = DMCHAR00_ANIM_70;
                        break;

                    case 0x2E:
                        this->animIndex = DMCHAR00_ANIM_67;
                        break;

                    case 0x2F:
                        this->animIndex = DMCHAR00_ANIM_71;
                        break;

                    case 0x30:
                        this->animIndex = DMCHAR00_ANIM_73;
                        break;

                    case 0x31:
                        this->animIndex = DMCHAR00_ANIM_74;
                        break;

                    case 0x32:
                        this->animIndex = DMCHAR00_ANIM_75;
                        break;

                    case 0x33:
                        this->animIndex = DMCHAR00_ANIM_77;
                        break;

                    case 0x34:
                        this->animIndex = DMCHAR00_ANIM_78;
                        break;

                    case 0x35:
                        this->animIndex = DMCHAR00_ANIM_79;
                        break;

                    case 0x36:
                        this->animIndex = DMCHAR00_ANIM_80;
                        break;

                    case 0x37:
                        this->animIndex = DMCHAR00_ANIM_82;
                        break;

                    case 0x38:
                        this->animIndex = DMCHAR00_ANIM_83;
                        break;

                    default:
                        this->animIndex = DMCHAR00_ANIM_0;
                        break;
                }
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    } else {
        this->cueId = 99;
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->animIndex) {
            case DMCHAR00_ANIM_4:
            case DMCHAR00_ANIM_5:
                this->animIndex += 4;
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                this->skelAnime.curFrame = 37.0f;
                break;

            case DMCHAR00_ANIM_14:
            case DMCHAR00_ANIM_15:
            case DMCHAR00_ANIM_20:
            case DMCHAR00_ANIM_21:
                this->animIndex += 2;
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            case DMCHAR00_ANIM_10:
            case DMCHAR00_ANIM_18:
            case DMCHAR00_ANIM_25:
            case DMCHAR00_ANIM_27:
            case DMCHAR00_ANIM_29:
            case DMCHAR00_ANIM_31:
            case DMCHAR00_ANIM_33:
            case DMCHAR00_ANIM_35:
            case DMCHAR00_ANIM_37:
            case DMCHAR00_ANIM_39:
            case DMCHAR00_ANIM_43:
            case DMCHAR00_ANIM_46:
            case DMCHAR00_ANIM_48:
            case DMCHAR00_ANIM_50:
            case DMCHAR00_ANIM_53:
            case DMCHAR00_ANIM_55:
            case DMCHAR00_ANIM_59:
            case DMCHAR00_ANIM_62:
            case DMCHAR00_ANIM_64:
            case DMCHAR00_ANIM_66:
            case DMCHAR00_ANIM_71:
            case DMCHAR00_ANIM_75:
            case DMCHAR00_ANIM_80:
            case DMCHAR00_ANIM_83:
                this->animIndex += 1;
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            case DMCHAR00_ANIM_45:
                this->animIndex = DMCHAR00_ANIM_19;
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            case DMCHAR00_ANIM_70:
            case DMCHAR00_ANIM_77:
                this->animIndex = DMCHAR00_ANIM_0;
                DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            default:
                break;
        }
    }
}

void func_80AA67F8(DmChar00* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((play->csCtx.state == CS_STATE_IDLE) && (gSaveContext.sceneLayer == 0) && (play->csCtx.scriptIndex == 1)) {
        if (this->animIndex != DMCHAR00_ANIM_42) {
            this->animIndex = DMCHAR00_ANIM_42;
            DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        }

        Math_SmoothStepToF(&this->actor.world.pos.x, 0.0f, 0.5f, 0.5f, 0.001f);
        Math_SmoothStepToF(&this->actor.world.pos.y, 30.0f, 0.5f, 0.5f, 0.001f);
        Math_SmoothStepToF(&this->actor.world.pos.z, -560.0f, 0.5f, 20.0f, 0.001f);

        if (player->actor.world.pos.z < -625.0f) {
            this->animIndex = DMCHAR00_ANIM_43;
            DmChar00_ChangeAnim(&this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            this->actionFunc = func_80AA695C;
            this->skelAnime.playSpeed = 1.5f;
        }
    }
}

void func_80AA695C(DmChar00* this, PlayState* play) {
    if (this->animIndex == DMCHAR00_ANIM_44) {
        Math_SmoothStepToF(&this->actor.world.pos.x, 0.0f, 0.5f, 0.5f, 0.001f);
        Math_SmoothStepToF(&this->actor.world.pos.y, 30.0f, 0.5f, 0.5f, 0.001f);
        Math_SmoothStepToF(&this->actor.world.pos.z, -680.0f, 0.5f, 10.0f, 0.001f);
    }
}

void DmChar00_Update(Actor* thisx, PlayState* play) {
    DmChar00* this = (DmChar00*)thisx;

    SkelAnime_Update(&this->skelAnime);

    this->actionFunc(this, play);

    this->unk_262++;
    func_80AA5EBC(this, play);
    DmChar00_HandleCutscene(this, play);
}

s32 DmChar00_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                              Gfx** gfx) {
    DmChar00* this = (DmChar00*)thisx;
    f32 sp28;
    Vec3f sp1C;

    if (limbIndex == FAIRY_LIMB_6) {
        sp28 = ((Math_SinS(this->unk_262 * 0x1000) * 0.1f) + 1.0f) * 0.012f * (this->actor.scale.x * (1.0f / 0.008f));
        Matrix_MultVec3f(&D_80AA7808, &sp1C);
        Matrix_Translate(sp1C.x, sp1C.y, sp1C.z, MTXMODE_NEW);
        Matrix_Scale(sp28, sp28, sp28, MTXMODE_APPLY);
    }
    return false;
}

void DmChar00_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DmChar00* this = (DmChar00*)thisx;
    s32 phi_a0;
    s32 pad;
    Gfx* gfx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Gfx));

    if ((play->csCtx.state == CS_STATE_IDLE) &&
        ((play->sceneId != SCENE_OPENINGDAN) || (gSaveContext.sceneLayer != 0) || (play->roomCtx.curRoom.num != 0) ||
         (play->csCtx.scriptIndex != 1) || (DMCHAR00_GET(&this->actor) != DMCHAR00_0))) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL27_Xlu(play->state.gfxCtx);

    //! FAKE:
    if (1) {}

    phi_a0 = (this->unk_262 * 50) % 512U;
    if (phi_a0 >= 256) {
        phi_a0 = 511 - phi_a0;
    }

    gSPSegment(POLY_XLU_DISP++, 0x08, &gfx[0]);

    gDPPipeSync(gfx++);
    gDPSetPrimColor(gfx++, 0, 0x01, (u8)this->unk_240.r, (u8)this->unk_240.g, (u8)this->unk_240.b,
                    (u8)(this->unk_240.a * 1));
    gDPSetRenderMode(gfx++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
    gSPEndDisplayList(gfx);

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(s8)this->unk_250.r, (u8)(s8)this->unk_250.g, (u8)(s8)this->unk_250.b,
                   (u8)(s8)((f32)phi_a0 * 1));
    gDPSetDither(POLY_XLU_DISP++, G_CD_BAYER);

    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   DmChar00_OverrideLimbDraw, NULL, &this->actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}
