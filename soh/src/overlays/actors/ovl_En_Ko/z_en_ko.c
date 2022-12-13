/*
 * File: z_en_ko.c
 * Overlay: ovl_En_Ko
 * Description: Kokiri children, and Fado
 */

#include "z_en_ko.h"
#include "objects/object_fa/object_fa.h"
#include "objects/object_os_anime/object_os_anime.h"
#include "objects/object_km1/object_km1.h"
#include "objects/object_kw1/object_kw1.h"
#include "vt.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

#define ENKO_TYPE (this->actor.params & 0xFF)
#define ENKO_PATH ((this->actor.params & 0xFF00) >> 8)

void EnKo_Init(Actor* thisx, PlayState* play);
void EnKo_Destroy(Actor* thisx, PlayState* play);
void EnKo_Update(Actor* thisx, PlayState* play);
void EnKo_Draw(Actor* thisx, PlayState* play);

void func_80A99048(EnKo* this, PlayState* play);
void func_80A995CC(EnKo* this, PlayState* play);
void func_80A99384(EnKo* this, PlayState* play);
void func_80A99438(EnKo* this, PlayState* play);
void func_80A99504(EnKo* this, PlayState* play);
void func_80A99560(EnKo* this, PlayState* play);

s32 func_80A98ECC(EnKo* this, PlayState* play);

const ActorInit En_Ko_InitVars = {
    ACTOR_EN_KO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnKo),
    (ActorFunc)EnKo_Init,
    (ActorFunc)EnKo_Destroy,
    (ActorFunc)EnKo_Update,
    (ActorFunc)EnKo_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static void* sFaEyes[] = { gFaEyeOpenTex, gFaEyeHalfTex, gFaEyeClosedTex, NULL };
static void* sKw1Eyes[] = { gKw1EyeOpenTex , gKw1EyeHalfTex,
                            gKw1EyeClosedTex, NULL };

typedef struct {
    /* 0x0 */ s16 objectId;
    /* 0x4 */ Gfx* dList;
    /* 0x8 */ void** eyeTextures;
} EnKoHead; // size = 0xC

static EnKoHead sHead[] = {
    { OBJECT_KM1, gKm1DL , NULL },
    { OBJECT_KW1, object_kw1_DL_002C10, sKw1Eyes },
    { OBJECT_FA, gFaDL, sFaEyes },
};

typedef struct {
    /* 0x0 */ s16 objectId;
    /* 0x4 */ FlexSkeletonHeader* flexSkeletonHeader;
} EnKoSkeleton; // size = 0x8

static EnKoSkeleton sSkeleton[2] = {
    { OBJECT_KM1, gKm1Skel, /* 0x060000F0 */ },
    { OBJECT_KW1, gKw1Skel, /* 0x060000F0 */ },
};

typedef enum {
    /*  0 */ ENKO_ANIM_0,
    /*  1 */ ENKO_ANIM_1,
    /*  2 */ ENKO_ANIM_2,
    /*  3 */ ENKO_ANIM_3,
    /*  4 */ ENKO_ANIM_4,
    /*  5 */ ENKO_ANIM_5,
    /*  6 */ ENKO_ANIM_6,
    /*  7 */ ENKO_ANIM_7,
    /*  8 */ ENKO_ANIM_8,
    /*  9 */ ENKO_ANIM_9,
    /* 10 */ ENKO_ANIM_10,
    /* 11 */ ENKO_ANIM_11,
    /* 12 */ ENKO_ANIM_12,
    /* 13 */ ENKO_ANIM_13,
    /* 14 */ ENKO_ANIM_14,
    /* 15 */ ENKO_ANIM_15,
    /* 16 */ ENKO_ANIM_16,
    /* 17 */ ENKO_ANIM_17,
    /* 18 */ ENKO_ANIM_18,
    /* 19 */ ENKO_ANIM_19,
    /* 20 */ ENKO_ANIM_20,
    /* 21 */ ENKO_ANIM_21,
    /* 22 */ ENKO_ANIM_22,
    /* 23 */ ENKO_ANIM_23,
    /* 24 */ ENKO_ANIM_24,
    /* 25 */ ENKO_ANIM_25,
    /* 26 */ ENKO_ANIM_26,
    /* 27 */ ENKO_ANIM_27,
    /* 28 */ ENKO_ANIM_28,
    /* 29 */ ENKO_ANIM_29,
    /* 30 */ ENKO_ANIM_30,
    /* 31 */ ENKO_ANIM_31,
    /* 32 */ ENKO_ANIM_32,
    /* 33 */ ENKO_ANIM_33
} EnKoAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &gObjOsAnim_8F6C, 1.0f, 2.0f, 14.0f, ANIMMODE_LOOP_PARTIAL, 0.0f },
    { &gObjOsAnim_8F6C, 0.0f, 1.0f, 1.0f, ANIMMODE_LOOP_PARTIAL, 0.0f },
    { &gObjOsAnim_9B64, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },
    { &gObjOsAnim_9B64, 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f },
    { &gObjOsAnim_9B64, 0.0f, 2.0f, 2.0f, ANIMMODE_ONCE, 0.0f },
    { &gObjOsAnim_62DC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_62DC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gObjOsAnim_5808, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    { &gObjOsAnim_7830, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_8178, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_65E0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_879C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7FFC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_80B4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_91AC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_6F9C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7064, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7120, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7F38, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7D94, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_6EE0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_98EC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_90EC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_982C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_9274, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_99A4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_9028, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7E64, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_7454, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gObjOsAnim_8F6C, 0.0f, 1.0f, 1.0f, ANIMMODE_LOOP_PARTIAL, -8.0f },
    { &gObjOsAnim_7D94, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gObjOsAnim_879C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gObjOsAnim_6A60, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    { &gObjOsAnim_7830, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

static u8 sOsAnimeLookup[13][5] = {
    /* ENKO_TYPE_CHILD_0    */ { ENKO_ANIM_8, ENKO_ANIM_9, ENKO_ANIM_9, ENKO_ANIM_14, ENKO_ANIM_11 },
    /* ENKO_TYPE_CHILD_1    */ { ENKO_ANIM_2, ENKO_ANIM_12, ENKO_ANIM_2, ENKO_ANIM_13, ENKO_ANIM_13 },
    /* ENKO_TYPE_CHILD_2    */ { ENKO_ANIM_11, ENKO_ANIM_11, ENKO_ANIM_11, ENKO_ANIM_15, ENKO_ANIM_9 },
    /* ENKO_TYPE_CHILD_3    */ { ENKO_ANIM_0, ENKO_ANIM_16, ENKO_ANIM_16, ENKO_ANIM_17, ENKO_ANIM_18 },
    /* ENKO_TYPE_CHILD_4    */ { ENKO_ANIM_19, ENKO_ANIM_19, ENKO_ANIM_20, ENKO_ANIM_10, ENKO_ANIM_9 },
    /* ENKO_TYPE_CHILD_5    */ { ENKO_ANIM_3, ENKO_ANIM_3, ENKO_ANIM_3, ENKO_ANIM_3, ENKO_ANIM_3 },
    /* ENKO_TYPE_CHILD_6    */ { ENKO_ANIM_4, ENKO_ANIM_22, ENKO_ANIM_22, ENKO_ANIM_4, ENKO_ANIM_23 },
    /* ENKO_TYPE_CHILD_7    */ { ENKO_ANIM_24, ENKO_ANIM_16, ENKO_ANIM_16, ENKO_ANIM_25, ENKO_ANIM_16 },
    /* ENKO_TYPE_CHILD_8    */ { ENKO_ANIM_26, ENKO_ANIM_15, ENKO_ANIM_15, ENKO_ANIM_26, ENKO_ANIM_15 },
    /* ENKO_TYPE_CHILD_9    */ { ENKO_ANIM_3, ENKO_ANIM_3, ENKO_ANIM_3, ENKO_ANIM_27, ENKO_ANIM_27 },
    /* ENKO_TYPE_CHILD_10   */ { ENKO_ANIM_2, ENKO_ANIM_2, ENKO_ANIM_2, ENKO_ANIM_2, ENKO_ANIM_22 },
    /* ENKO_TYPE_CHILD_11   */ { ENKO_ANIM_14, ENKO_ANIM_14, ENKO_ANIM_14, ENKO_ANIM_14, ENKO_ANIM_14 },
    /* ENKO_TYPE_CHILD_FADO */ { ENKO_ANIM_5, ENKO_ANIM_5, ENKO_ANIM_5, ENKO_ANIM_5, ENKO_ANIM_5 },
};

typedef struct {
    /* 0x0 */ u8 headId;
    /* 0x1 */ u8 bodyId;
    /* 0x4 */ Color_RGBA8 tunicColor;
    /* 0x8 */ u8 legsId;
    /* 0xC */ Color_RGBA8 bootsColor;
} EnKoModelInfo; // size = 0x10

typedef enum {
    /* 0 */ KO_BOY,
    /* 1 */ KO_GIRL,
    /* 2 */ KO_FADO
} KokiriGender;

static EnKoModelInfo sModelInfo[] = {
    /* ENKO_TYPE_CHILD_0    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_1    */ { KO_GIRL, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
    /* ENKO_TYPE_CHILD_2    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_3    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_4    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_5    */ { KO_GIRL, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
    /* ENKO_TYPE_CHILD_6    */ { KO_GIRL, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
    /* ENKO_TYPE_CHILD_7    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_8    */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_9    */ { KO_GIRL, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
    /* ENKO_TYPE_CHILD_10   */ { KO_GIRL, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
    /* ENKO_TYPE_CHILD_11   */ { KO_BOY, KO_BOY, { 0, 130, 70, 255 }, KO_BOY, { 110, 170, 20, 255 } },
    /* ENKO_TYPE_CHILD_FADO */ { KO_FADO, KO_GIRL, { 70, 190, 60, 255 }, KO_GIRL, { 100, 30, 0, 255 } },
};

typedef struct {
    /* 0x0 */ s8 targetMode;
    /* 0x4 */ f32 lookDist; // extended by collider radius
    /* 0x8 */ f32 appearDist;
} EnKoInteractInfo; // size = 0xC

static EnKoInteractInfo sInteractInfo[] = {
    /* ENKO_TYPE_CHILD_0    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_1    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_2    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_3    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_4    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_5    */ { 1, 30.0f, 240.0f },
    /* ENKO_TYPE_CHILD_6    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_7    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_8    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_9    */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_10   */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_11   */ { 6, 30.0f, 180.0f },
    /* ENKO_TYPE_CHILD_FADO */ { 6, 30.0f, 180.0f },
};

s32 EnKo_AreObjectsAvailable(EnKo* this, PlayState* play) {
    u8 headId = sModelInfo[ENKO_TYPE].headId;
    u8 bodyId = sModelInfo[ENKO_TYPE].bodyId;
    u8 legsId = sModelInfo[ENKO_TYPE].legsId;

    this->legsObjectBankIdx = Object_GetIndex(&play->objectCtx, sSkeleton[legsId].objectId);
    if (this->legsObjectBankIdx < 0) {
        return false;
    }

    this->bodyObjectBankIdx = Object_GetIndex(&play->objectCtx, sSkeleton[bodyId].objectId);
    if (this->bodyObjectBankIdx < 0) {
        return false;
    }

    this->headObjectBankIdx = Object_GetIndex(&play->objectCtx, sHead[headId].objectId);
    if (this->headObjectBankIdx < 0) {
        return false;
    }
    return true;
}

s32 EnKo_AreObjectsLoaded(EnKo* this, PlayState* play) {
    if (!Object_IsLoaded(&play->objectCtx, this->legsObjectBankIdx)) {
        return false;
    }
    if (!Object_IsLoaded(&play->objectCtx, this->bodyObjectBankIdx)) {
        return false;
    }
    if (!Object_IsLoaded(&play->objectCtx, this->headObjectBankIdx)) {
        return false;
    }
    return true;
}

s32 EnKo_IsOsAnimeAvailable(EnKo* this, PlayState* play) {
    this->osAnimeBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_OS_ANIME);
    if (this->osAnimeBankIndex < 0) {
        return false;
    }
    return true;
}

s32 EnKo_IsOsAnimeLoaded(EnKo* this, PlayState* play) {
    if (!Object_IsLoaded(&play->objectCtx, this->osAnimeBankIndex)) {
        return false;
    }
    return true;
}

u16 func_80A96FD0(PlayState* play, Actor* thisx) {
    EnKo* this = (EnKo*)thisx;
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_FADO:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x10DA;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x10D9;
            }
            return (gSaveContext.infTable[11] & 0x80) ? 0x10D8 : 0x10D7;
        case ENKO_TYPE_CHILD_0:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x1025;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1042;
            }
            return 0x1004;
        case ENKO_TYPE_CHILD_1:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x1023;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1043;
            }
            if (gSaveContext.infTable[1] & 0x4000) {
                return 0x1006;
            }
            return 0x1005;
        case ENKO_TYPE_CHILD_2:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x1022;
            }
            return 0x1007;
        case ENKO_TYPE_CHILD_3:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x1021;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1044;
            }
            if (gSaveContext.infTable[2] & 4) {
                return 0x1009;
            }
            return 0x1008;
        case ENKO_TYPE_CHILD_4:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x1097;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1042;
            }
            if (gSaveContext.infTable[2] & 0x10) {
                return 0x100B;
            }
            return 0x100A;
        case ENKO_TYPE_CHILD_5:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x10B0;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1043;
            }
            if (gSaveContext.infTable[2] & 0x40) {
                return 0x100D;
            }
            return 0x100C;
        case ENKO_TYPE_CHILD_6:
            if (gSaveContext.eventChkInf[4] & 1) {
                return 0x10B5;
            }
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x1043;
            }
            if (gSaveContext.infTable[2] & 0x100) {
                return 0x1019;
            }
            return 0x100E;
        case ENKO_TYPE_CHILD_7:
            return 0x1035;
        case ENKO_TYPE_CHILD_8:
            return 0x1038;
        case ENKO_TYPE_CHILD_9:
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x104B;
            }
            return 0x103C;
        case ENKO_TYPE_CHILD_10:
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                return 0x104C;
            }
            return 0x103D;
        case ENKO_TYPE_CHILD_11:
            return 0x103E;
    }
    return 0;
}

u16 func_80A97338(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);
    EnKo* this = (EnKo*)thisx;

    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_FADO:
            player->exchangeItemId = EXCH_ITEM_ODD_POTION;
            return 0x10B9;
        case ENKO_TYPE_CHILD_0:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1072;
            }
            if (gSaveContext.infTable[4] & 2) {
                return 0x1056;
            }
            return 0x1055;
        case ENKO_TYPE_CHILD_1:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1073;
            }
            return 0x105A;
        case ENKO_TYPE_CHILD_2:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1074;
            }
            if (gSaveContext.infTable[4] & 0x80) {
                return 0x105E;
            }
            return 0x105D;
        case ENKO_TYPE_CHILD_3:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1075;
            }
            return 0x105B;
        case ENKO_TYPE_CHILD_4:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1076;
            }
            return 0x105F;
        case ENKO_TYPE_CHILD_5:
            return 0x1057;
        case ENKO_TYPE_CHILD_6:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1077;
            }
            if (gSaveContext.infTable[5] & 2) {
                return 0x1059;
            }
            return 0x1058;
        case ENKO_TYPE_CHILD_7:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x1079;
            }
            return 0x104E;
        case ENKO_TYPE_CHILD_8:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x107A;
            }
            if (gSaveContext.infTable[5] & 0x200) {
                return 0x1050;
            }
            return 0x104F;
        case ENKO_TYPE_CHILD_9:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x107B;
            }
            return 0x1051;
        case ENKO_TYPE_CHILD_10:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x107C;
            }
            return 0x1052;
        case ENKO_TYPE_CHILD_11:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                return 0x107C;
            }
            if (gSaveContext.infTable[6] & 2) {
                return 0x1054;
            }
            return 0x1053;
        default:
            return 0;
    }
}

u16 func_80A97610(PlayState* play, Actor* thisx) {
    u16 faceReaction;
    EnKo* this = (EnKo*)thisx;

    if (ENKO_TYPE == ENKO_TYPE_CHILD_0 || ENKO_TYPE == ENKO_TYPE_CHILD_2 || ENKO_TYPE == ENKO_TYPE_CHILD_3 ||
        ENKO_TYPE == ENKO_TYPE_CHILD_4 || ENKO_TYPE == ENKO_TYPE_CHILD_7 || ENKO_TYPE == ENKO_TYPE_CHILD_8 ||
        ENKO_TYPE == ENKO_TYPE_CHILD_11) {
        faceReaction = Text_GetFaceReaction(play, 0x13);
    }
    if (ENKO_TYPE == ENKO_TYPE_CHILD_1 || ENKO_TYPE == ENKO_TYPE_CHILD_5 || ENKO_TYPE == ENKO_TYPE_CHILD_6 ||
        ENKO_TYPE == ENKO_TYPE_CHILD_9 || ENKO_TYPE == ENKO_TYPE_CHILD_10) {
        faceReaction = Text_GetFaceReaction(play, 0x14);
    }
    if (ENKO_TYPE == ENKO_TYPE_CHILD_FADO) {
        faceReaction = Text_GetFaceReaction(play, 0x12);
    }
    if (faceReaction != 0) {
        return faceReaction;
    }
    if (LINK_IS_ADULT) {
        return func_80A97338(play, thisx);
    }
    return func_80A96FD0(play, thisx);
}

s16 func_80A97738(PlayState* play, Actor* thisx) {
    EnKo* this = (EnKo*)thisx;

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x1005:
                    gSaveContext.infTable[1] |= 0x4000;
                    break;
                case 0x1008:
                    gSaveContext.infTable[2] |= 0x4;
                    break;
                case 0x100A:
                    gSaveContext.infTable[2] |= 0x10;
                    break;
                case 0x100C:
                    gSaveContext.infTable[2] |= 0x40;
                    break;
                case 0x100E:
                    gSaveContext.infTable[2] |= 0x100;
                    break;
                case 0x104F:
                    gSaveContext.infTable[5] |= 0x200;
                    break;
                case 0x1053:
                    gSaveContext.infTable[6] |= 2;
                    break;
                case 0x1055:
                    gSaveContext.infTable[4] |= 2;
                    break;
                case 0x1058:
                    gSaveContext.infTable[5] |= 2;
                    break;
                case 0x105D:
                    gSaveContext.infTable[4] |= 0x80;
                    break;
                case 0x10D7:
                    gSaveContext.infTable[11] |= 0x80;
                    break;
                case 0x10BA:
                    return 1;
            }
            return 0;
        case TEXT_STATE_DONE_FADING:
            switch (this->actor.textId) {
                case 0x10B7:
                case 0x10B8:
                    if (this->unk_210 == 0) {
                        Audio_PlaySoundGeneral(NA_SE_SY_TRE_BOX_APPEAR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        this->unk_210 = 1;
                    }
            }
            return 1;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (this->actor.textId) {
                    case 0x1035:
                        this->actor.textId = (play->msgCtx.choiceIndex == 0) ? 0x1036 : 0x1037;
                        Message_ContinueTextbox(play, this->actor.textId);
                        break;
                    case 0x1038:
                        this->actor.textId = (play->msgCtx.choiceIndex != 0)
                                                 ? (play->msgCtx.choiceIndex == 1) ? 0x103A : 0x103B
                                                 : 0x1039;
                        Message_ContinueTextbox(play, this->actor.textId);
                        break;
                    case 0x103E:
                        this->actor.textId = (play->msgCtx.choiceIndex == 0) ? 0x103F : 0x1040;
                        Message_ContinueTextbox(play, this->actor.textId);
                        break;
                    case 0x10B7:
                        gSaveContext.infTable[11] |= 0x1000;

                    case 0x10B8:
                        this->actor.textId = (play->msgCtx.choiceIndex == 0) ? 0x10BA : 0x10B9;
                        return (play->msgCtx.choiceIndex == 0) ? 2 : 1;
                }
                return 1;
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                return 3;
            }
    }
    return 1;
}

s32 EnKo_GetForestQuestState(EnKo* this) {
    s32 result;

    if (!LINK_IS_ADULT) {
        // Obtained Zelda's Letter
        if (gSaveContext.eventChkInf[4] & 1) {
            return ENKO_FQS_CHILD_SARIA;
        }
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
            return ENKO_FQS_CHILD_STONE;
        }
        return ENKO_FQS_CHILD_START;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        result = ENKO_FQS_ADULT_SAVED;
    } else {
        result = ENKO_FQS_ADULT_ENEMY;
    }
    return result;
}

f32 func_80A97BC0(EnKo* this) {
    f32 D_80A9A62C[13][5] = {
        /* ENKO_TYPE_CHILD_0    */ { 0.0f, 0.0f, 0.0f, -30.0f, -20.0f },
        /* ENKO_TYPE_CHILD_1    */ { 0.0f, 0.0f, 0.0f, -20.0f, -10.0f },
        /* ENKO_TYPE_CHILD_2    */ { 0.0f, 0.0f, 0.0f, -30.0f, -20.0f },
        /* ENKO_TYPE_CHILD_3    */ { -10.0f, 10.0f, 10.0f, -10.0f, -30.0f },
        /* ENKO_TYPE_CHILD_4    */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_5    */ { 0.0f, 0.0f, 0.0f, -20.0f, -20.0f },
        /* ENKO_TYPE_CHILD_6    */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_7    */ { 10.0f, 10.0f, 10.0f, -60.0f, -20.0f },
        /* ENKO_TYPE_CHILD_8    */ { -10.0f, -10.0f, -20.0f, -30.0f, -30.0f },
        /* ENKO_TYPE_CHILD_9    */ { -10.0f, -10.0f, -10.0f, -40.0f, -40.0f },
        /* ENKO_TYPE_CHILD_10   */ { 0.0f, 0.0f, 0.0f, -10.0f, -20.0f },
        /* ENKO_TYPE_CHILD_11   */ { -10.0f, -10.0f, -20.0f, -30.0f, -30.0f },
        /* ENKO_TYPE_CHILD_FADO */ { 0.0f, 0.0f, 0.0f, -20.0f, -20.0f },
    };

    if (LINK_IS_ADULT && ENKO_TYPE == ENKO_TYPE_CHILD_FADO) {
        return -20.0f;
    }
    return D_80A9A62C[ENKO_TYPE][EnKo_GetForestQuestState(this)];
}

u8 func_80A97C7C(EnKo* this) {
    u8 D_80A9A730[13][5] = {
        /* ENKO_TYPE_CHILD_0    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_1    */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_2    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_3    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_4    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_5    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_6    */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_7    */ { 1, 1, 1, 0, 1 },
        /* ENKO_TYPE_CHILD_8    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_9    */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_10   */ { 1, 1, 1, 1, 1 },
        /* ENKO_TYPE_CHILD_11   */ { 0, 0, 0, 0, 0 },
        /* ENKO_TYPE_CHILD_FADO */ { 1, 1, 1, 1, 1 },
    };

    return D_80A9A730[ENKO_TYPE][EnKo_GetForestQuestState(this)];
}

s32 EnKo_IsWithinTalkAngle(EnKo* this) {
    s16 yawDiff;
    s16 yawDiffAbs;
    s32 result;

    yawDiff = this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y;
    yawDiffAbs = ABS(yawDiff);

    if (yawDiffAbs < 0x3FFC) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

s32 func_80A97D68(EnKo* this, PlayState* play) {
    s16 arg3;

    if (this->unk_1E8.unk_00 != 0) {
        if ((this->skelAnime.animation == &gObjOsAnim_6A60) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_32);
        }
        arg3 = 2;
    } else {
        if ((this->skelAnime.animation == &gObjOsAnim_7830) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_33);
        }
        arg3 = 1;
    }
    func_80034A14(&this->actor, &this->unk_1E8, 2, arg3);
    return EnKo_IsWithinTalkAngle(this);
}

s32 func_80A97E18(EnKo* this, PlayState* play) {
    s16 arg3;

    func_80034F54(play, this->unk_2E4, this->unk_304, 16);
    if (EnKo_IsWithinTalkAngle(this) == true) {
        arg3 = 2;
    } else {
        arg3 = 1;
    }
    if (this->unk_1E8.unk_00 != 0) {
        arg3 = 4;
    } else if (this->lookDist < this->actor.xzDistToPlayer) {
        arg3 = 1;
    }
    func_80034A14(&this->actor, &this->unk_1E8, 2, arg3);
    return 1;
}

s32 func_80A97EB0(EnKo* this, PlayState* play) {
    s16 arg3;
    s32 result;

    func_80034F54(play, this->unk_2E4, this->unk_304, 16);
    result = EnKo_IsWithinTalkAngle(this);
    arg3 = (result == true) ? 2 : 1;
    func_80034A14(&this->actor, &this->unk_1E8, 2, arg3);
    return result;
}

s32 func_80A97F20(EnKo* this, PlayState* play) {
    func_80034F54(play, this->unk_2E4, this->unk_304, 16);
    func_80034A14(&this->actor, &this->unk_1E8, 2, 4);
    return 1;
}

s32 func_80A97F70(EnKo* this, PlayState* play) {
    s16 arg3;

    if (this->unk_1E8.unk_00 != 0) {
        if ((this->skelAnime.animation == &gObjOsAnim_8F6C) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_29);
        }
        func_80034F54(play, this->unk_2E4, this->unk_304, 16);
        arg3 = 2;
    } else {
        if ((this->skelAnime.animation == &gObjOsAnim_7D94) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_30);
        }
        arg3 = 1;
    }
    func_80034A14(&this->actor, &this->unk_1E8, 5, arg3);
    return EnKo_IsWithinTalkAngle(this);
}

s32 func_80A98034(EnKo* this, PlayState* play) {
    s16 arg3;
    s32 result;

    if (this->unk_1E8.unk_00 != 0) {
        if ((this->skelAnime.animation == &gObjOsAnim_8F6C) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_29);
        }
        func_80034F54(play, this->unk_2E4, this->unk_304, 16);
        result = EnKo_IsWithinTalkAngle(this);
        arg3 = (result == true) ? 2 : 1;
    } else {
        if ((this->skelAnime.animation == &gObjOsAnim_879C) == false) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_31);
        }
        arg3 = 1;
        result = EnKo_IsWithinTalkAngle(this);
    }
    func_80034A14(&this->actor, &this->unk_1E8, 5, arg3);
    return result;
}

// Same as func_80A97F20
s32 func_80A98124(EnKo* this, PlayState* play) {
    func_80034F54(play, this->unk_2E4, this->unk_304, 16);
    func_80034A14(&this->actor, &this->unk_1E8, 2, 4);
    return 1;
}

s32 func_80A98174(EnKo* this, PlayState* play) {
    if (this->unk_1E8.unk_00 != 0) {
        if (Animation_OnFrame(&this->skelAnime, 18.0f)) {
            this->skelAnime.playSpeed = 0.0f;
        }
    } else if (this->skelAnime.playSpeed != 1.0f) {
        this->skelAnime.playSpeed = 1.0f;
    }
    if (this->skelAnime.playSpeed == 0.0f) {
        func_80034F54(play, this->unk_2E4, this->unk_304, 16);
    }
    func_80034A14(&this->actor, &this->unk_1E8, 2, (this->skelAnime.playSpeed == 0.0f) ? 2 : 1);
    return EnKo_IsWithinTalkAngle(this);
}

s32 EnKo_ChildStart(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return func_80A97D68(this, play);
        case ENKO_TYPE_CHILD_1:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_2:
            return func_80A98034(this, play);
        case ENKO_TYPE_CHILD_3:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_4:
            return func_80A97F70(this, play);
        case ENKO_TYPE_CHILD_5:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_6:
            return func_80A97F20(this, play);
        case ENKO_TYPE_CHILD_7:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_8:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_9:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_10:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_11:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return func_80A97E18(this, play);
    }
}

s32 EnKo_ChildStone(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return func_80A98124(this, play);
        case ENKO_TYPE_CHILD_1:
            return func_80A98124(this, play);
        case ENKO_TYPE_CHILD_2:
            return func_80A98034(this, play);
        case ENKO_TYPE_CHILD_3:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_4:
            return func_80A97F70(this, play);
        case ENKO_TYPE_CHILD_5:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_6:
            return func_80A97F20(this, play);
        case ENKO_TYPE_CHILD_7:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_8:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_9:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_10:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_11:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return func_80A97E18(this, play);
    }
}

s32 EnKo_ChildSaria(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return func_80A98124(this, play);
        case ENKO_TYPE_CHILD_1:
            return func_80A98124(this, play);
        case ENKO_TYPE_CHILD_2:
            return func_80A98034(this, play);
        case ENKO_TYPE_CHILD_3:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_4:
            return func_80A98174(this, play);
        case ENKO_TYPE_CHILD_5:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_6:
            return func_80A97F20(this, play);
        case ENKO_TYPE_CHILD_7:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_8:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_9:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_10:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_11:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return func_80A97E18(this, play);
    }
}

s32 EnKo_AdultEnemy(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_1:
            return func_80A98124(this, play);
        case ENKO_TYPE_CHILD_2:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_3:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_4:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_5:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_6:
            return func_80A97F20(this, play);
        case ENKO_TYPE_CHILD_7:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_8:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_9:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_10:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_11:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return func_80A97E18(this, play);
    }
}

s32 EnKo_AdultSaved(EnKo* this, PlayState* play) {
    switch (ENKO_TYPE) {
        case ENKO_TYPE_CHILD_0:
            return func_80A98034(this, play);
        case ENKO_TYPE_CHILD_1:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_2:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_3:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_4:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_5:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_6:
            return func_80A97F20(this, play);
        case ENKO_TYPE_CHILD_7:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_8:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_9:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_10:
            return func_80A97E18(this, play);
        case ENKO_TYPE_CHILD_11:
            return func_80A97EB0(this, play);
        case ENKO_TYPE_CHILD_FADO:
            return func_80A97E18(this, play);
    }
}
void func_80A9877C(EnKo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((play->csCtx.state != 0) || (gDbgCamEnabled != 0)) {
        this->unk_1E8.unk_18 = play->view.eye;
        this->unk_1E8.unk_14 = 40.0f;
        if (ENKO_TYPE != ENKO_TYPE_CHILD_0) {
            func_80034A14(&this->actor, &this->unk_1E8, 2, 2);
        }
    } else {
        this->unk_1E8.unk_18 = player->actor.world.pos;
        this->unk_1E8.unk_14 = func_80A97BC0(this);
        if ((func_80A98ECC(this, play) == 0) && (this->unk_1E8.unk_00 == 0)) {
            return;
        }
    }
    if (func_800343CC(play, &this->actor, &this->unk_1E8.unk_00, this->lookDist, func_80A97610, func_80A97738) &&
        ENKO_TYPE == ENKO_TYPE_CHILD_FADO && play->sceneNum == SCENE_SPOT10) {
        this->actor.textId = INV_CONTENT(ITEM_TRADE_ADULT) > ITEM_ODD_POTION ? 0x10B9 : 0x10DF;

        if (func_8002F368(play) == ENKO_TYPE_CHILD_9) {
            this->actor.textId = (gSaveContext.infTable[11] & 0x1000) ? 0x10B8 : 0x10B7;
            this->unk_210 = 0;
        }
        player->actor.textId = this->actor.textId;
    }
}

// Checks if the Kokiri should spawn based on quest progress
s32 EnKo_CanSpawn(EnKo* this, PlayState* play) {
    switch (play->sceneNum) {
        case SCENE_SPOT04:
            if (ENKO_TYPE >= ENKO_TYPE_CHILD_7 && ENKO_TYPE != ENKO_TYPE_CHILD_FADO) {
                return false;
            }
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) && LINK_IS_ADULT) {
                return false;
            }
            return true;
        case SCENE_KOKIRI_HOME:
            if (ENKO_TYPE != ENKO_TYPE_CHILD_7 && ENKO_TYPE != ENKO_TYPE_CHILD_8 && ENKO_TYPE != ENKO_TYPE_CHILD_11) {
                return false;
            } else {
                return true;
            }
        case SCENE_KOKIRI_HOME3:
            if (LINK_IS_ADULT && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                if (ENKO_TYPE != ENKO_TYPE_CHILD_1 && ENKO_TYPE != ENKO_TYPE_CHILD_9) {
                    return false;
                } else {
                    return true;
                }
            }
            if (ENKO_TYPE != ENKO_TYPE_CHILD_9) {
                return false;
            } else {
                return true;
            }
        case SCENE_KOKIRI_HOME4:
            if (LINK_IS_ADULT && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                if (ENKO_TYPE != ENKO_TYPE_CHILD_0 && ENKO_TYPE != ENKO_TYPE_CHILD_4) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case SCENE_KOKIRI_HOME5:
            if (LINK_IS_ADULT && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                if (ENKO_TYPE != ENKO_TYPE_CHILD_6) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }

        case SCENE_KOKIRI_SHOP:
            if (LINK_IS_ADULT && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                if (ENKO_TYPE != ENKO_TYPE_CHILD_5 && ENKO_TYPE != ENKO_TYPE_CHILD_10) {
                    return false;
                } else {
                    return true;
                }
            } else if (ENKO_TYPE != ENKO_TYPE_CHILD_10) {
                return false;
            } else {
                return true;
            }

        case SCENE_SPOT10:
            if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE)) {
                // To explain the logic because Fado and Grog are linked:
                // - If you have Cojiro, then spawn Grog and not Fado.
                // - If you don't have Cojiro but do have Odd Potion, spawn Fado and not Grog.
                // - If you don't have either, spawn Grog if you haven't traded the Odd Mushroom.
                // - If you don't have either but have traded the mushroom, don't spawn either.
                if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_COJIRO)) {
                    return false;
                } else {
                    return PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_ODD_POTION);
                }
            } else {
                return (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_ODD_POTION) ? true : false;
            }
        default:
            return false;
    }
}

void EnKo_Blink(EnKo* this) {
    void** eyeTextures;
    s32 headId;

    if (DECR(this->blinkTimer) == 0) {
        headId = sModelInfo[ENKO_TYPE].headId;
        this->eyeTextureIndex++;
        eyeTextures = sHead[headId].eyeTextures;
        if (eyeTextures != NULL && eyeTextures[this->eyeTextureIndex] == NULL) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTextureIndex = 0;
        }
    }
}

void func_80A98CD8(EnKo* this) {
    s32 type = ENKO_TYPE;
    EnKoInteractInfo* info = &sInteractInfo[type];

    this->actor.targetMode = info->targetMode;
    this->lookDist = info->lookDist;
    this->lookDist += this->collider.dim.radius;
    this->appearDist = info->appearDist;
}

// Used to fetch actor animation?
s32 EnKo_GetForestQuestState2(EnKo* this) {
    if (LINK_IS_ADULT) {
        return CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) ? ENKO_FQS_ADULT_SAVED : ENKO_FQS_ADULT_ENEMY;
    }
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        return (gSaveContext.eventChkInf[4] & 1) ? ENKO_FQS_CHILD_SARIA : ENKO_FQS_CHILD_STONE;
    }
    return ENKO_FQS_CHILD_START;
}

void func_80A98DB4(EnKo* this, PlayState* play) {
    f32 dist;

    if (play->sceneNum != SCENE_SPOT10 && play->sceneNum != SCENE_SPOT04) {
        this->modelAlpha = 255.0f;
        return;
    }
    if (play->csCtx.state != 0 || gDbgCamEnabled != 0) {
        dist = Math_Vec3f_DistXYZ(&this->actor.world.pos, &play->view.eye) * 0.25f;
    } else {
        dist = this->actor.xzDistToPlayer;
    }

    if (CVar_GetS32("gDisableKokiriDrawDistance", 0) != 0) {
        this->appearDist = 32767.0f;
        Math_SmoothStepToF(&this->modelAlpha, (this->appearDist < dist) ? 0.0f : 255.0f, 0.3f, 40.0f, 1.0f);
        f32 test = this->appearDist;
    } else {
        this->appearDist = 180.0f;
        Math_SmoothStepToF(&this->modelAlpha, (this->appearDist < dist) ? 0.0f : 255.0f, 0.3f, 40.0f, 1.0f);
    }
    
    if (this->modelAlpha < 10.0f) {
        this->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        this->actor.flags |= ACTOR_FLAG_0;
    }
}

s32 func_80A98ECC(EnKo* this, PlayState* play) {
    if (play->sceneNum == SCENE_SPOT10 && ENKO_TYPE == ENKO_TYPE_CHILD_FADO) {
        return func_80A97E18(this, play);
    }
    switch (EnKo_GetForestQuestState(this)) {
        case ENKO_FQS_CHILD_START:
            return EnKo_ChildStart(this, play);
        case ENKO_FQS_CHILD_STONE:
            return EnKo_ChildStone(this, play);
        case ENKO_FQS_CHILD_SARIA:
            return EnKo_ChildSaria(this, play);
        case ENKO_FQS_ADULT_ENEMY:
            return EnKo_AdultEnemy(this, play);
        case ENKO_FQS_ADULT_SAVED:
            return EnKo_AdultSaved(this, play);
    }
}

void EnKo_Init(Actor* thisx, PlayState* play) {
    EnKo* this = (EnKo*)thisx;

    if (ENKO_TYPE >= ENKO_TYPE_CHILD_MAX || !EnKo_IsOsAnimeAvailable(this, play) ||
        !EnKo_AreObjectsAvailable(this, play)) {
        Actor_Kill(thisx);
    }
    if (!EnKo_CanSpawn(this, play)) {
        Actor_Kill(thisx);
    }
    this->actionFunc = func_80A99048;
}

void EnKo_Destroy(Actor* thisx, PlayState* play) {
    EnKo* this = (EnKo*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80A99048(EnKo* this, PlayState* play) {
    if (EnKo_IsOsAnimeLoaded(this, play) && EnKo_AreObjectsLoaded(this, play)) {
        this->actor.flags &= ~ACTOR_FLAG_4;
        this->actor.objBankIndex = this->legsObjectBankIdx;
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->actor.objBankIndex].segment);
        SkelAnime_InitFlex(play, &this->skelAnime, sSkeleton[sModelInfo[ENKO_TYPE].legsId].flexSkeletonHeader,
                           NULL, this->jointTable, this->morphTable, 16);
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->osAnimeBankIndex].segment);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
        if (ENKO_TYPE == ENKO_TYPE_CHILD_7) {
            // "Angle Z"
            osSyncPrintf(VT_BGCOL(BLUE) "  アングルＺ->(%d)\n" VT_RST, this->actor.shape.rot.z);
            if (LINK_IS_ADULT && !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
                if (this->actor.shape.rot.z != 1) {
                    Actor_Kill(&this->actor);
                    return;
                }
            } else if (this->actor.shape.rot.z != 0) {
                Actor_Kill(&this->actor);
                return;
            }
        }
        if (ENKO_TYPE == ENKO_TYPE_CHILD_5) {
            this->collider.base.ocFlags1 |= 0x40;
        }
        this->forestQuestState = EnKo_GetForestQuestState2(this);
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, sOsAnimeLookup[ENKO_TYPE][this->forestQuestState]);
        Actor_SetScale(&this->actor, 0.01f);
        func_80A98CD8(this);
        this->modelAlpha = 0.0f;
        this->path = Path_GetByIndex(play, ENKO_PATH, 0xFF);
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 3);
        if (ENKO_TYPE == ENKO_TYPE_CHILD_3) {
            if (!gSaveContext.n64ddFlag) {
                if (!CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
                    this->collider.dim.height += 200;
                    this->actionFunc = func_80A995CC;
                    return;
                }
            } else {
                if (!Flags_GetEventChkInf(7)) {
                    this->collider.dim.height += 200;
                    this->actionFunc = func_80A995CC;
                    return;
                }
            }
            Path_CopyLastPoint(this->path, &this->actor.world.pos);
        }
        this->actionFunc = func_80A99384;
    }
}

void func_80A99384(EnKo* this, PlayState* play) {
    if (ENKO_TYPE == ENKO_TYPE_CHILD_FADO && this->unk_1E8.unk_00 != 0 && this->actor.textId == 0x10B9) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_7);
        this->actionFunc = func_80A99438;
    } else if (ENKO_TYPE == ENKO_TYPE_CHILD_FADO && this->unk_1E8.unk_00 == 2) {
        this->actionFunc = func_80A99504;
        play->msgCtx.stateTimer = 4;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    }
}

void func_80A99438(EnKo* this, PlayState* play) {
    if (ENKO_TYPE == ENKO_TYPE_CHILD_FADO && this->unk_1E8.unk_00 == 2) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_6);
        this->actionFunc = func_80A99504;
        play->msgCtx.stateTimer = 4;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    } else if (this->unk_1E8.unk_00 == 0 || this->actor.textId != 0x10B9) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENKO_ANIM_6);
        this->actionFunc = func_80A99384;
    }
}

void func_80A99504(EnKo* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80A99560;
    } else {
        if (gSaveContext.n64ddFlag) {
            GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_TRADE_ODD_POTION, GI_SAW);
            Randomizer_ConsumeAdultTradeItem(play, ITEM_ODD_POTION);
            GiveItemEntryFromActor(&this->actor, play, itemEntry, 120.0f, 10.0f);
        } else {
            s32 itemId = GI_SAW;
            func_8002F434(&this->actor, play, itemId, 120.0f, 10.0f);
        }
    }
}

void func_80A99560(EnKo* this, PlayState* play) {
    if (this->unk_1E8.unk_00 == 3) {
        this->actor.textId = 0x10B9;
        Message_ContinueTextbox(play, this->actor.textId);
        this->unk_1E8.unk_00 = 1;
        gSaveContext.itemGetInf[3] |= 2;
        this->actionFunc = func_80A99384;
    }
}

void func_80A995CC(EnKo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_f2;
    f32 phi_f0;
    s16 homeYawToPlayer = Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos);

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.x += 80.0f * Math_SinS(homeYawToPlayer);
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.z += 80.0f * Math_CosS(homeYawToPlayer);
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

    if (this->unk_1E8.unk_00 == 0 || !this->actor.isTargeted) {
        temp_f2 = fabsf((f32)this->actor.yawTowardsPlayer - homeYawToPlayer) * 0.001f * 3.0f;
        if (temp_f2 < 1.0f) {
            this->skelAnime.playSpeed = 1.0f;
        } else {
            phi_f0 = CLAMP_MAX(temp_f2, 3.0f);
            this->skelAnime.playSpeed = phi_f0;
        }
    } else {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnKo_Update(Actor* thisx, PlayState* play) {
    ColliderCylinder* collider;
    EnKo* this = (EnKo*)thisx;
    s32 pad;

    if (this->actionFunc != func_80A99048) {
        if ((s32)this->modelAlpha != 0) {
            gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->osAnimeBankIndex].segment);
            SkelAnime_Update(&this->skelAnime);
            func_80A98DB4(this, play);
            EnKo_Blink(this);
        } else {
            func_80A98DB4(this, play);
        }
    }
    if (this->unk_1E8.unk_00 == 0) {
        Actor_MoveForward(&this->actor);
    }
    if (func_80A97C7C(this)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
        this->actor.gravity = -1.0f;
    } else {
        this->actor.gravity = 0.0f;
    }

    this->actionFunc(this, play);

    func_80A9877C(this, play);
    collider = &this->collider;
    Collider_UpdateCylinder(&this->actor, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
}

s32 EnKo_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfx) {
    EnKo* this = (EnKo*)thisx;
    void* eyeTexture;
    Vec3s sp40;
    u8 headId;
    s32 pad;

    if (limbIndex == 15) {
        gSPSegment((*gfx)++, 0x06, play->objectCtx.status[this->headObjectBankIdx].segment);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->headObjectBankIdx].segment);

        headId = sModelInfo[ENKO_TYPE].headId;
        *dList = sHead[headId].dList;
        if (sHead[headId].eyeTextures != NULL) {
            eyeTexture = sHead[headId].eyeTextures[this->eyeTextureIndex];
            gSPSegment((*gfx)++, 0x0A, SEGMENTED_TO_VIRTUAL(eyeTexture));
        }
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->legsObjectBankIdx].segment);
    }
    if (limbIndex == 8) {
        sp40 = this->unk_1E8.unk_0E;
        Matrix_RotateX(BINANG_TO_RAD(-sp40.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp40.x), MTXMODE_APPLY);
    }
    if (limbIndex == 15) {
        Matrix_Translate(1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        sp40 = this->unk_1E8.unk_08;
        Matrix_RotateX(BINANG_TO_RAD(sp40.y), MTXMODE_APPLY);
        Matrix_RotateZ(BINANG_TO_RAD(sp40.x), MTXMODE_APPLY);
        Matrix_Translate(-1200.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8 || limbIndex == 9 || limbIndex == 12) {
        rot->y += Math_SinS(this->unk_2E4[limbIndex]) * 200.0f;
        rot->z += Math_CosS(this->unk_304[limbIndex]) * 200.0f;
    }
    return false;
}

void EnKo_PostLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    PlayState* play = play2;
    EnKo* this = (EnKo*)thisx;
    Vec3f D_80A9A774 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 7) {
        gSPSegment((*gfx)++, 0x06, play->objectCtx.status[this->bodyObjectBankIdx].segment);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->bodyObjectBankIdx].segment);
    }
    if (limbIndex == 15) {
        Matrix_MultVec3f(&D_80A9A774, &this->actor.focus.pos);
    }
}

Gfx* EnKo_SetEnvColor(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dList = Graph_Alloc(gfxCtx, sizeof(Gfx) * 2);

    gDPSetEnvColor(dList, r, g, b, a);
    gSPEndDisplayList(dList + 1);
    return dList;
}

void EnKo_Draw(Actor* thisx, PlayState* play) {
    EnKo* this = (EnKo*)thisx;
    Color_RGBA8 tunicColor = sModelInfo[ENKO_TYPE].tunicColor;
    Color_RGBA8 bootsColor = sModelInfo[ENKO_TYPE].bootsColor;

    if (CVar_GetS32("gCosmetics.NPC_Kokiri.Changed", 0)) {
        tunicColor = CVar_GetRGBA("gCosmetics.NPC_Kokiri.Value", sModelInfo[ENKO_TYPE].tunicColor);
        bootsColor = CVar_GetRGBA("gCosmetics.NPC_Kokiri.Value", sModelInfo[ENKO_TYPE].bootsColor);
    }

    this->actor.shape.shadowAlpha = this->modelAlpha;

    OPEN_DISPS(play->state.gfxCtx);
    if ((s16)this->modelAlpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   EnKo_SetEnvColor(play->state.gfxCtx, tunicColor.r, tunicColor.g, tunicColor.b, 255));
        gSPSegment(POLY_OPA_DISP++, 0x09,
                   EnKo_SetEnvColor(play->state.gfxCtx, bootsColor.r, bootsColor.g, bootsColor.b, 255));
        func_80034BA0(play, &this->skelAnime, EnKo_OverrideLimbDraw, EnKo_PostLimbDraw, &this->actor,
                      this->modelAlpha);
    } else if ((s16)this->modelAlpha != 0) {
        tunicColor.a = this->modelAlpha;
        bootsColor.a = this->modelAlpha;
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   EnKo_SetEnvColor(play->state.gfxCtx, tunicColor.r, tunicColor.g, tunicColor.b, tunicColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   EnKo_SetEnvColor(play->state.gfxCtx, bootsColor.r, bootsColor.g, bootsColor.b, bootsColor.a));
        func_80034CC4(play, &this->skelAnime, EnKo_OverrideLimbDraw, EnKo_PostLimbDraw, &this->actor,
                      this->modelAlpha);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
