/*
 * File: z_boss_02.c
 * Overlay: ovl_Boss_02
 * Description: Twinmold
 */

#include "z_boss_02.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "overlays/actors/ovl_En_Tanron5/z_en_tanron5.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Boss02_Init(Actor* thisx, PlayState* play);
void Boss02_Destroy(Actor* thisx, PlayState* play);
void Boss02_Twinmold_Update(Actor* thisx, PlayState* play);
void Boss02_Twinmold_Draw(Actor* thisx, PlayState* play2);
void Boss02_Reset(void);

void func_809DAA74(Boss02* this, PlayState* play);
void func_809DAA98(Boss02* this, PlayState* play);
void func_809DAAA8(Boss02* this, PlayState* play);
void func_809DAB78(Boss02* this, PlayState* play);
void Boss02_Tail_Update(Actor* thisx, PlayState* play);
void Boss02_BattleHandler_Update(Actor* thisx, PlayState* play);
void Boss02_BattleHandler_Draw(Actor* thisx, PlayState* play);
void Boss02_UpdateEffects(PlayState* play);
void Boss02_DrawEffects(PlayState* play);
void Boss02_HandleGiantsMaskCutscene(Boss02* this, PlayState* play);
void func_809DEAC4(Boss02* this, PlayState* play);

typedef enum {
    /*  0 */ GIANTS_MASK_CS_STATE_WAITING_FOR_MASK, // Starts the cutscene when the player puts on/takes off the mask.
    /*  1 */ GIANTS_MASK_CS_STATE_MASK_ON,          // Controls the cutscene where the player puts the mask on.
    /*  2 */ GIANTS_MASK_CS_STATE_MASK_ON_SKIPPED,  // Moves to the "done" state when the mask-on cutscene is skipped.
    /* 10 */ GIANTS_MASK_CS_STATE_MASK_OFF = 10,    // Controls the cutscene where the player takes the mask off.
    /* 11 */ GIANTS_MASK_CS_STATE_MASK_OFF_SKIPPED, // Moves to the "done" state when the mask-off cutscene is skipped.
    /* 20 */ GIANTS_MASK_CS_STATE_DONE = 20         // Stops the cutscene and resets the player's scale back to normal.
} GiantsMaskCsState;

typedef enum {
    /* 0 */ GIANTS_MASK_CS_FLASH_STATE_NOT_STARTED,    // Does nothing at all.
    /* 1 */ GIANTS_MASK_CS_FLASH_STATE_STARTED,        // Plays the "mask flash" SFX and initializes a white FillScreen.
    /* 2 */ GIANTS_MASK_CS_FLASH_STATE_INCREASE_ALPHA, // Increases the alpha for the FillScreen on every frame.
    /* 3 */ GIANTS_MASK_CS_FLASH_STATE_DECREASE_ALPHA  // Decreases the alpha for the FillScreen on every frame.
} GiantsMaskCsFlashState;

/**
 * If set to true, the player can skip the cutscene of putting on the Giant's Mask
 * by pressing A, B, or any C-button. If false, they must watch it in full.
 */
u8 sCanSkipMaskOnCs;

/**
 * If set to true, the player can skip the cutscene of taking off the Giant's Mask
 * by pressing A, B, or any C-button. If false, they must watch it in full.
 */
u8 sCanSkipMaskOffCs;

/**
 * True when the player is giant-sized (i.e. wearing the Giant's Mask), false when the player is normal-sized.
 */
u8 sIsInGiantMode;

Boss02* sRedTwinmold;
Boss02* sBlueTwinmold;
Boss02* sTwinmoldBattleHandler;
u8 sTwinmoldMusicStartTimer;
DoorWarp1* sBlueWarp;
TwinmoldEffect sTwinmoldEffects[TWINMOLD_EFFECT_COUNT];

static DamageTable sBlueTwinmoldDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xF),
    /* Fire arrow     */ DMG_ENTRY(5, 0x2),
    /* Ice arrow      */ DMG_ENTRY(1, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(2, 0xF),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

static DamageTable sRedTwinmoldDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(2, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0x2),
    /* Ice arrow      */ DMG_ENTRY(5, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(2, 0xF),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

ActorProfile Boss_02_Profile = {
    /**/ ACTOR_BOSS_02,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS02,
    /**/ sizeof(Boss02),
    /**/ Boss02_Init,
    /**/ Boss02_Destroy,
    /**/ Boss02_Twinmold_Update,
    /**/ Boss02_Twinmold_Draw,
    /**/ Boss02_Reset,
};

/**
 * Multiplies the scale differently depending on whether the player is wearing the Giant's Mask or not.
 * When the player is wearing the Giant's Mask, this value is smaller to make the player seem larger.
 */
static f32 sGiantModeScaleFactor = 1.0f;

s16 D_809DF5B4[] = {
    0, 195, 190, 185, 180, 175, 170, 165, 160, 155, 150, 145, 140, 135, 130, 125, 120, 115, 110, 105, 100, 95, 90, 0,
};

s16 D_809DF5E4[] = {
    0, 196, 192, 188, 184, 180, 176, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132, 128, 124, 120, 116, 112,
};

static ColliderJntSphElementInit sJntSphElementsInit1[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 2, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 3, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 4, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 5, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 6, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 7, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 8, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 9, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 10, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 11, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 12, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 14, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 16, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 17, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 18, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 19, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 20, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 21, { { 0, 0, 0 }, 85 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 130 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit1 = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit1),
    sJntSphElementsInit1,
};

static ColliderJntSphElementInit sJntSphElementsInit2[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 130 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 22, { { 0, 0, 0 }, 110 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit2 = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit2),
    sJntSphElementsInit2,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x04, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 150, 200, 0, { 0, 0, 0 } },
};

Vec3f D_809DF9C0[] = {
    { 0.0f, -200.0f, 1000.0f },    { 0.0f, 500.0f, 1000.0f },        { 0.0f, 1000.0f, 1000.0f },
    { 1000.0f, 500.0f, 1000.0f },  { 1000.0f, 1000.0f, -1000.0f },   { -1000.0f, 500.0f, -1000.0f },
    { -1000.0f, 500.0f, 1000.0f }, { -1000.0f, -1000.0f, -1000.0f }, { -1000.0f, -1000.0f, -1000.0f },
};

Vec3f D_809DFA2C[] = {
    { 0.0f, -200.0f, -800.0f },  { 0.0f, 800.0f, -800.0f },   { 800.0f, 300.0f, -800.0f },
    { -800.0f, 800.0f, 0.0f },   { -800.0f, -1000.0f, 0.0f }, { -800.0f, -1000.0f, 0.0f },
    { -800.0f, -1000.0f, 0.0f }, { -800.0f, -1000.0f, 0.0f }, { -800.0f, -1000.0f, 0.0f },
};

void Boss02_FillScreen(PlayState* play, u8 red, u8 green, u8 blue, u8 alpha) {
    R_PLAY_FILL_SCREEN_ON = true;
    R_PLAY_FILL_SCREEN_R = red;
    R_PLAY_FILL_SCREEN_G = green;
    R_PLAY_FILL_SCREEN_B = blue;
    R_PLAY_FILL_SCREEN_ALPHA = alpha;
}

void Boss02_SetFillScreenAlpha(PlayState* play, u8 alpha) {
    R_PLAY_FILL_SCREEN_ALPHA = alpha;
}

void Boss02_StopFillScreen(PlayState* play) {
    R_PLAY_FILL_SCREEN_ON = false;
}

void Boss02_SpawnEffectSand(TwinmoldEffect* effects, Vec3f* pos, f32 scale) {
    s16 i;

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effects++) {
        if (effects->type == TWINMOLD_EFFECT_NONE) {
            effects->type = TWINMOLD_EFFECT_SAND;
            effects->pos = *pos;
            effects->velocity.x = Rand_CenteredFloat(30.0f);
            effects->velocity.y = Rand_ZeroFloat(7.0f) + 7.0f;
            effects->velocity.z = Rand_CenteredFloat(30.0f);
            effects->accel.y = -0.3f;
            effects->scale = scale;
            effects->alpha = 255;
            effects->timer = 0;
            effects->targetScale = 2.0f * scale;
            effects->accel.x = effects->accel.z = 0.0f;
            effects->epoch++;
            break;
        }
    }
}

void Boss02_SpawnEffectFragment(TwinmoldEffect* effects, Vec3f* pos) {
    s16 i;

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effects++) {
        if (effects->type == TWINMOLD_EFFECT_NONE) {
            effects->type = TWINMOLD_EFFECT_FRAGMENT;
            effects->pos = *pos;
            effects->timer = Rand_ZeroFloat(20.0f);
            effects->velocity.x = Rand_CenteredFloat(50.0f);
            effects->velocity.y = Rand_CenteredFloat(50.0f);
            effects->velocity.z = Rand_CenteredFloat(50.0f);
            effects->accel.z = 0.0f;
            effects->accel.x = 0.0f;
            effects->accel.y = -1.5f;
            effects->scale = Rand_ZeroFloat(0.04f) + 0.02f;
            effects->rotY = Rand_ZeroFloat(32767.0f);
            effects->rotX = Rand_ZeroFloat(32767.0f);
            effects->epoch++;
            break;
        }
    }
}

void Boss02_SpawnEffectFlash(TwinmoldEffect* effects, Vec3f* pos) {
    s16 i;

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effects++) {
        if ((effects->type == TWINMOLD_EFFECT_NONE) || (effects->type == TWINMOLD_EFFECT_FRAGMENT)) {
            effects->type = TWINMOLD_EFFECT_FLASH;
            effects->pos = *pos;
            Math_Vec3f_Copy(&effects->velocity, &gZeroVec3f);
            Math_Vec3f_Copy(&effects->accel, &gZeroVec3f);
            effects->alpha = 255;
            effects->scale = 0.0f;
            effects->epoch++;
            break;
        }
    }
}

void func_809DA50C(s32 arg0, ColliderJntSph* collider, Vec3f* arg2) {
    collider->elements[arg0].dim.worldSphere.center.x = arg2->x;
    collider->elements[arg0].dim.worldSphere.center.y = arg2->y;
    collider->elements[arg0].dim.worldSphere.center.z = arg2->z;
    collider->elements[arg0].dim.worldSphere.radius =
        collider->elements[arg0].dim.modelSphere.radius * collider->elements[arg0].dim.scale * sGiantModeScaleFactor;
}

void Boss02_Init(Actor* thisx, PlayState* play) {
    Boss02* this = (Boss02*)thisx;
    s32 i;
    s32 pad[2];

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) &&
        (TWINMOLD_GET_TYPE(&this->actor) == TWINMOLD_TYPE_RED)) {
        sBlueWarp = (DoorWarp1*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, 60.0f,
                                                   0.0f, 0, 0, 0, ENDOORWARP1_FF_1);
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 30.0f, -150.0f, 0, 1, 0, BHEART_PARAM_NORMAL);
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_10;
    this->subCamUp.z = this->subCamUp.x = 0.0f;
    this->subCamUp.y = 1.0f;
    if (TWINMOLD_GET_TYPE(&this->actor) == TWINMOLD_TYPE_BATTLE_HANDLER) {
        sTwinmoldBattleHandler = this;
        play->specialEffects = (void*)sTwinmoldEffects;
        this->actor.update = Boss02_BattleHandler_Update;
        this->actor.draw = Boss02_BattleHandler_Draw;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->playerScale = 0.01f;
        if ((KREG(64) != 0) || CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_TWINMOLD) || (sBlueWarp != NULL)) {
            this->unk_1D20 = 0;
            sTwinmoldMusicStartTimer = KREG(15) + 20;
        } else {
            this->unk_1D20 = 1;
        }
        R_MAGIC_CONSUME_TIMER_GIANTS_MASK = KREG(14) + 20;
        this->giantModeScaleFactor = 1.0f;
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_TANRON5, 0.0f, 1000.0f, 0.0f, 0, 0, 0,
                           TWINMOLD_PROP_PARAMS(TWINMOLD_PROP_TYPE_SPAWN_HANDLER));
    } else if (TWINMOLD_GET_TYPE(&this->actor) == TWINMOLD_TYPE_TAIL) {
        this->actor.update = Boss02_Tail_Update;
        this->actor.draw = NULL;
        this->actor.hintId = TATL_HINT_ID_TWINMOLD;
    } else {
        if (TWINMOLD_GET_TYPE(&this->actor) != TWINMOLD_TYPE_BLUE) {
            this->actor.params = TWINMOLD_TYPE_RED;
            Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_02, 0.0f, 0.0f, 0.0f, 0, 0, 0, TWINMOLD_TYPE_BATTLE_HANDLER);
            sRedTwinmold = this;
            sBlueTwinmold =
                (Boss02*)Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_02, this->actor.world.pos.x,
                                     this->actor.world.pos.y, this->actor.world.pos.z, this->actor.world.rot.x,
                                     this->actor.world.rot.y, this->actor.world.rot.z, TWINMOLD_TYPE_BLUE);
            sRedTwinmold->actor.colChkInfo.damageTable = &sRedTwinmoldDamageTable;
            sBlueTwinmold->actor.colChkInfo.damageTable = &sBlueTwinmoldDamageTable;
            sRedTwinmold->otherTwinmold = sBlueTwinmold;
            sBlueTwinmold->otherTwinmold = sRedTwinmold;
        }

        this->actor.colChkInfo.mass = MASS_HEAVY;
        this->actor.colChkInfo.health = 20;
        Actor_SetScale(&this->actor, 0.01f);
        SkelAnime_Init(play, &this->skelAnime, &gTwinmoldHeadSkel, &gTwinmoldHeadFlyAnim, this->jointTable,
                       this->morphTable, TWINMOLD_HEAD_LIMB_MAX);
        Collider_InitAndSetJntSph(play, &this->colliderSphere1, &this->actor, &sJntSphInit1,
                                  this->colliderSphere1Elements);
        Collider_InitAndSetJntSph(play, &this->colliderSphere2, &this->actor, &sJntSphInit2,
                                  this->colliderSphere2Elements);
        Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);

        if (sBlueWarp != NULL) {
            func_809DAA74(this, play);
        } else {
            func_809DAAA8(this, play);
        }

        for (i = 0; i < ARRAY_COUNT(this->unk_01BC); i++) {
            this->unk_01BC[i].x = this->actor.world.pos.x;
            this->unk_01BC[i].y = this->actor.world.pos.y;
            this->unk_01BC[i].z = this->actor.world.pos.z;
        }

        this->unk_014C = Rand_ZeroFloat(1000.0f);
        this->unk_1678 = 22;
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BOSS_02, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                           TWINMOLD_TYPE_TAIL);
    }
}

void Boss02_Destroy(Actor* thisx, PlayState* play) {
}

void func_809DAA74(Boss02* this, PlayState* play) {
    this->actionFunc = func_809DAA98;
    this->actor.world.pos.y = -3000.0f;
}

void func_809DAA98(Boss02* this, PlayState* play) {
}

void func_809DAAA8(Boss02* this, PlayState* play) {
    this->actionFunc = func_809DAB78;
    Animation_MorphToLoop(&this->skelAnime, &gTwinmoldHeadFlyAnim, 0.0f);
    if (sTwinmoldBattleHandler->unk_1D20 != 0) {
        this->unk_0144 = 10;
    } else {
        this->unk_0144 = 100;
        this->unk_01A8 = 25.0f;
        sTwinmoldBattleHandler->unk_1D7E = 100;
        this->actor.world.pos.x = 0.0f;
        if (sRedTwinmold == this) {
            this->actor.world.pos.z = -1000.0f;
        } else {
            this->actor.world.pos.z = 1400.0f;
        }
    }
    this->actor.world.pos.y = -500.0f;
}

Color_RGBA8 D_809DFA98 = { 185, 140, 70, 255 };

void func_809DAB78(Boss02* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    CollisionPoly* spDC;
    Vec3f spD0;
    f32 spCC;
    f32 spC8;
    f32 spC4;
    s32 i;
    f32 temp_f0;
    f32 phi_f2;
    s16 temp_s0;
    s16 temp_s2;
    Boss02* otherTwinmold = this->otherTwinmold;
    Vec3f spA4;
    f32 spA0;
    f32 sp9C;
    Vec3f sp90;
    CollisionPoly* sp8C;
    s32 bgId;
    Vec3f sp7C;
    Vec3f sp70;
    Vec3f sp64;

    spCC = this->unk_01B0.x - this->actor.world.pos.x;
    spC8 = this->unk_01B0.y - this->actor.world.pos.y;
    spC4 = this->unk_01B0.z - this->actor.world.pos.z;

    if ((this->unk_0144 != 10) && (this->unk_0144 <= 20)) {
        SkelAnime_Update(&this->skelAnime);
        temp_s0 = Math_Atan2S(spCC, spC4);
        temp_s2 = Math_Atan2S(spC8, sqrtf(SQ(spCC) + SQ(spC4)));
        Math_ApproachS(&this->actor.world.rot.y, temp_s0, 0x14, this->unk_0164);
        Math_ApproachS(&this->actor.shape.rot.x, temp_s2, 0x14, this->unk_0164);
        Math_ApproachS(&this->unk_0198, this->unk_019A, 1, 0x20);
        this->unk_0196 += this->unk_0198;
        Math_ApproachF(&this->unk_019C, this->unk_01A0, 0.1f, 100.0f);
        this->unk_01A4 = Math_SinS(this->unk_0196) * this->unk_019C;
        this->actor.world.rot.x = this->actor.shape.rot.x + this->unk_01A4;

        if (!(this->unk_014C & 0x1F) && (sTwinmoldBattleHandler->unk_1D20 == 0)) {
            this->unk_01A0 = Rand_ZeroFloat(0x1000) + 0x800;
            this->unk_019A = Rand_ZeroFloat(0x400) + 0x200;
        }

        if (this->unk_0195 != 0) {
            Math_ApproachF(&this->unk_0164, this->unk_0168 * 1.25f, 1.0f, 62.5f);
        } else {
            Math_ApproachF(&this->unk_0164, this->unk_0168, 1.0f, 50.0f);
        }

        this->unk_0168 = 2000.0f;
        if (this->unk_0195 != 0) {
            this->actor.speed = this->unk_01A8 * sGiantModeScaleFactor * 1.25f;
            this->skelAnime.playSpeed = 2.0f;
        } else {
            this->actor.speed = this->unk_01A8 * sGiantModeScaleFactor;
        }

        Actor_UpdateVelocityWithoutGravity(&this->actor);
        Actor_UpdatePos(&this->actor);

        spD0 = this->actor.world.pos;
        if (sIsInGiantMode) {
            spD0.y = 5000.0f;
        } else {
            spD0.y = 2000.0f;
        }

        temp_f0 = BgCheck_EntityRaycastFloor1(&play->colCtx, &spDC, &spD0);
        if (((this->unk_017C.y < temp_f0) && (temp_f0 <= this->unk_0188.y)) ||
            ((temp_f0 < this->unk_017C.y) && (this->unk_0188.y <= temp_f0))) {
            this->unk_0170 = this->unk_017C;
            this->unk_0170.y = temp_f0;
            this->unk_016C = 120;
            Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_ROAR_OLD);
        }

        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        if (this->unk_0195 != 0) {
            this->actor.world.rot.z = Math_SinS(this->unk_014C * 0x1200) * 0xE00;
        } else {
            this->actor.world.rot.z = Math_SinS(this->unk_014C * 0xC00) * 0xE00;
        }

        this->unk_014E++;
        if (this->unk_014E >= ARRAY_COUNT(this->unk_01BC)) {
            this->unk_014E = 0;
        }

        this->unk_01BC[this->unk_014E].x = this->actor.world.pos.x;
        this->unk_01BC[this->unk_014E].y = this->actor.world.pos.y;
        this->unk_01BC[this->unk_014E].z = this->actor.world.pos.z;

        this->unk_0B1C[this->unk_014E].x = BINANG_TO_RAD_ALT(this->actor.world.rot.x);
        this->unk_0B1C[this->unk_014E].y = BINANG_TO_RAD_ALT(this->actor.world.rot.y);
        this->unk_0B1C[this->unk_014E].z = BINANG_TO_RAD_ALT(this->actor.world.rot.z);
    }

    if ((this->unk_0144 < 10) && (otherTwinmold->unk_0144 >= 20)) {
        this->unk_01B0.y = -1000.0f * sGiantModeScaleFactor;
        if (sIsInGiantMode) {
            this->unk_01B0.y += 3150.0f;
        }
        this->unk_0144 = 3;
        this->unk_0146[0] = 50;
    }

    switch (this->unk_0144) {
        default:
            return;

        case 0:
            if ((TWINMOLD_GET_TYPE(&this->actor) == TWINMOLD_TYPE_RED) && (Rand_ZeroOne() < 0.75f)) {
                this->actor.world.pos.x = player->actor.world.pos.x;
                this->actor.world.pos.z = player->actor.world.pos.z;
                this->actor.world.pos.y = player->actor.world.pos.y - (600.0f * sGiantModeScaleFactor);
            } else {
                this->actor.world.pos.x = Rand_CenteredFloat(5000.0f * sGiantModeScaleFactor);
                this->actor.world.pos.z = Rand_CenteredFloat(5000.0f * sGiantModeScaleFactor);
                this->actor.world.pos.y = -500.0f * sGiantModeScaleFactor;
                if (sIsInGiantMode) {
                    this->actor.world.pos.y += 3150.0f;
                }
            }

            if ((fabsf(this->actor.world.pos.x) < (500.0f * sGiantModeScaleFactor)) &&
                (fabsf(this->actor.world.pos.z) < (500.0f * sGiantModeScaleFactor))) {
                return;
            }

        case 100:
            this->actor.shape.rot.x = 0x4000;
            this->unk_01B0.x = this->actor.world.pos.x;
            this->unk_01B0.y = this->actor.world.pos.y + (1000.0f * sGiantModeScaleFactor);
            this->unk_01B0.z = this->actor.world.pos.z;
            this->unk_0146[0] = 100;
            this->unk_0144 = 1;

        case 1:
            if (this->unk_0146[0] == 0) {
                if (Rand_ZeroOne() < 0.3f) {
                    this->unk_0144 = 5;
                    this->unk_0146[0] = 150;
                } else {
                    this->unk_0144 = 2;
                    this->unk_01B0.x = Rand_CenteredFloat(3000.0f * sGiantModeScaleFactor);
                    this->unk_01B0.z = Rand_CenteredFloat(3000.0f * sGiantModeScaleFactor);
                    if ((fabsf(this->unk_01B0.x) < (500.0f * sGiantModeScaleFactor)) &&
                        (fabsf(this->unk_01B0.z) < (500.0f * sGiantModeScaleFactor))) {
                        this->unk_01B0.x = 500.0f;
                        this->unk_01B0.z = 500.0f;
                    }
                    this->unk_01B0.y =
                        Rand_ZeroFloat(800.0f * sGiantModeScaleFactor) + (200.0f * sGiantModeScaleFactor);
                    if (sIsInGiantMode) {
                        this->unk_01B0.y += 3150.0f;
                    }
                }
                this->unk_0164 = 0.0f;
            }
            return;

        case 2:
            if (this->unk_0195 != 0) {
                phi_f2 = 700.0f;
            } else {
                phi_f2 = 500.0f;
            }
            if (sqrtf(SQ(spCC) + SQ(spC8) + SQ(spC4)) < (phi_f2 * sGiantModeScaleFactor)) {
                this->unk_0144 = 3;
                this->unk_01B0.y = -3000.0f * sGiantModeScaleFactor;
                if (sIsInGiantMode) {
                    this->unk_01B0.y += 3150.0f;
                }
                this->unk_0146[0] = 150;
                this->unk_0164 = 0.0f;
            }
            return;

        case 3:
            if (this->unk_0146[0] == 0) {
                this->unk_0144 = 0;
            }
            return;

        case 5:
            this->unk_01B0.x = player->actor.world.pos.x;
            this->unk_01B0.y = player->actor.world.pos.y + (100.0f * sGiantModeScaleFactor);
            this->unk_01B0.z = player->actor.world.pos.z;
            if (this->unk_0146[0] == 0) {
                this->unk_0144 = 3;
                this->unk_01B0.x = Rand_CenteredFloat(500.0f * sGiantModeScaleFactor) + this->actor.world.pos.x;
                this->unk_01B0.y = -3000.0f * sGiantModeScaleFactor;
                if (sIsInGiantMode) {
                    this->unk_01B0.y += 3150.0f;
                }
                this->unk_01B0.z = Rand_CenteredFloat(500.0f * sGiantModeScaleFactor) + this->actor.world.pos.z;
                this->unk_0146[0] = 150;
                this->unk_0164 = 0.0f;
            }
            return;

        case 10:
            if (this->unk_1678 != 0) {
                this->unk_019A = 0x500;
                this->unk_01A0 = 0x1200;
            } else {
                this->colliderCylinder.dim.radius = 150.0f * sGiantModeScaleFactor;
                this->colliderCylinder.dim.height = 200.0f * sGiantModeScaleFactor;
                this->colliderCylinder.dim.yShift = 0;
                Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
            }
            return;

        case 11:
            if (this == sRedTwinmold) {
                this->unk_01B0.x = D_809DF9C0[this->unk_1D1A].x;
                this->unk_01B0.y = D_809DF9C0[this->unk_1D1A].y;
                this->unk_01B0.z = D_809DF9C0[this->unk_1D1A].z;
            } else {
                this->unk_01B0.x = D_809DFA2C[this->unk_1D1A].x;
                this->unk_01B0.y = D_809DFA2C[this->unk_1D1A].y;
                this->unk_01B0.z = D_809DFA2C[this->unk_1D1A].z;
            }
            this->actor.shape.rot.x = 0x4000;
            this->unk_0144 = 12;
            this->actor.world.pos.x = this->unk_01B0.x;
            this->actor.world.pos.y = -200.0f;
            this->actor.world.pos.z = this->unk_01B0.z;
            this->unk_01A8 = 25.0f;
            return;

        case 12:
            if (sqrtf(SQ(spCC) + SQ(spC8) + SQ(spC4)) < 500.0f) {
                this->unk_1D1A++;
                this->unk_0164 = 0.0f;
                if (this->unk_1D1A > 8) {
                    this->unk_1D1A = 8;
                }
                if (this == sRedTwinmold) {
                    this->unk_01B0.x = D_809DF9C0[this->unk_1D1A].x;
                    this->unk_01B0.y = D_809DF9C0[this->unk_1D1A].y;
                    this->unk_01B0.z = D_809DF9C0[this->unk_1D1A].z;
                } else {
                    this->unk_01B0.x = D_809DFA2C[this->unk_1D1A].x;
                    this->unk_01B0.y = D_809DFA2C[this->unk_1D1A].y;
                    this->unk_01B0.z = D_809DFA2C[this->unk_1D1A].z;
                }
            }
            return;

        case 20:
            this->unk_0152 = 15;
            if ((s16)(BREG(71) + 140) < this->unk_0146[1]) {
                if (this->unk_0146[0] == 0) {
                    Matrix_RotateYS(Math_Atan2S(-player->actor.world.pos.x, -player->actor.world.pos.z), MTXMODE_NEW);
                    Matrix_MultVecZ(1500.0f * sGiantModeScaleFactor, &spA4);
                    this->unk_0146[0] = 50;
                    this->unk_01B0.x = player->actor.world.pos.x + spA4.x;
                    this->unk_01B0.y =
                        Rand_CenteredFloat(500.0f * sGiantModeScaleFactor) + (600.0f * sGiantModeScaleFactor);
                    if (sIsInGiantMode) {
                        this->unk_01B0.y += 3150.0f;
                    }
                    this->unk_01B0.z = player->actor.world.pos.z + spA4.z;
                }
                this->unk_0168 = 3000.0f;
            } else {
                this->unk_01B0.y += 10.0f * sGiantModeScaleFactor;
                this->unk_0168 = 5000.0f;
            }
            this->unk_019A = 0x1000;
            this->unk_01A8 = 25.0f;
            this->unk_01A0 = 0x1800;
            if (this->unk_0146[1] == 0) {
                this->unk_015C = 1;
                this->unk_0144 = 21;
                this->unk_0146[0] = 20;
                this->unk_0152 = 0;
                sTwinmoldBattleHandler->unk_1D20 = 102;
                sTwinmoldBattleHandler->subCamAtVel = 0.0f;
                Audio_PlaySfx(NA_SE_EN_INBOSS_DEAD_PRE2_OLD);
            } else if (!(this->unk_0146[1] & 0xF) && (Rand_ZeroOne() < 0.5f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_DAMAGE_OLD);
            }
            return;

        case 21:
            this->unk_01A8 = 0.0f;
            this->actor.speed = 0.0f;
            if (this->unk_0146[0] == 0) {
                this->unk_0146[0] = 3;

                for (i = 0; i < 35; i++) {
                    Boss02_SpawnEffectFragment(play->specialEffects, &this->unk_147C[this->unk_1678]);
                }

                Boss02_SpawnEffectFlash(play->specialEffects, &this->unk_147C[this->unk_1678]);
                Audio_PlaySfx(NA_SE_EV_EXPLOSION);

                this->unk_1678--;
                if (this->unk_1678 <= 0) {
                    this->unk_0144 = 22;
                    this->actor.gravity = -1.0f * sGiantModeScaleFactor;
                    this->actor.velocity.y = 0.0f;
                    this->actor.terminalVelocity = -1000.0f * sGiantModeScaleFactor;
                    this->unk_0164 = Rand_CenteredFloat(0.05f);

                    spCC = player->actor.world.pos.x - this->actor.world.pos.x;
                    spC4 = player->actor.world.pos.z - this->actor.world.pos.z;
                    if (sqrtf(SQ(spCC) + SQ(spC4)) < (400.0f * sGiantModeScaleFactor)) {
                        this->actor.speed = 15.0f * sGiantModeScaleFactor;
                    }

                    spCC = this->actor.world.pos.x;
                    spC4 = this->actor.world.pos.z;
                    if (sqrtf(SQ(spCC) + SQ(spC4)) < (400.0f * sGiantModeScaleFactor)) {
                        this->actor.speed = 15.0f * sGiantModeScaleFactor;
                    }

                    if (otherTwinmold->unk_0144 >= 10) {
                        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_BOSS | SEQ_FLAG_ASYNC);
                    }

                    Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_DEAD_OLD);
                }
            }
            return;

        case 22:
            i = (this->unk_014E + 196) % ARRAY_COUNT(this->unk_01BC);

            Math_Vec3f_Copy(&this->unk_01BC[i], &this->actor.world.pos);
            this->unk_0B1C[i].y += this->unk_0164;
            Math_ApproachF(&this->unk_0B1C[i].x, -(M_PIf / 2), 0.1f, 0.07f);
            Actor_MoveWithGravity(&this->actor);
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 150.0f, 100.0f, UPDBGCHECKINFO_FLAG_4);

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->unk_0144 = 23;
                this->actor.speed = 0.0f;
                this->unk_0170 = this->unk_017C;
                this->unk_016C = 30;
                this->unk_0170.y = this->actor.floorHeight;
                sTwinmoldBattleHandler->unk_1D20 = 103;
                sTwinmoldBattleHandler->unk_1D1C = 0;
                sTwinmoldBattleHandler->unk_0146[0] = 15;
                sTwinmoldBattleHandler->unk_0150 = 0;
                Audio_PlaySfx(NA_SE_EV_LIGHTNING);

                for (i = 0; i < 30; i++) {
                    Boss02_SpawnEffectFragment(play->specialEffects, &this->unk_0170);
                }

                this->unk_0146[0] = 10;
            }
            break;

        case 23:
            i = (this->unk_014E + 196) % ARRAY_COUNT(this->unk_01BC);
            Math_Vec3f_Copy(&this->unk_01BC[i], &this->actor.world.pos);
            Math_ApproachF(&this->unk_0B1C[i].x, -(M_PIf / 2), 0.05f, 0.07f);

            if (this->unk_0146[0] & 1) {
                sp9C = Rand_ZeroFloat(M_PIf);

                for (i = 0; i < 15; i++) {
                    Matrix_RotateYF(((2.0f * (i * M_PIf)) / 15.0f) + sp9C, MTXMODE_NEW);
                    Matrix_MultVecZ((10 - this->unk_0146[0]) * (sGiantModeScaleFactor * 300.0f) * 0.1f, &sp90);
                    spD0.x = this->unk_0170.x + sp90.x;
                    spD0.y = this->unk_0170.y + (1000.0f * sGiantModeScaleFactor);
                    spD0.z = this->unk_0170.z + sp90.z;
                    if (BgCheck_EntityRaycastFloor3(&play->colCtx, &sp8C, &bgId, &spD0) != BGCHECK_Y_MIN) {
                        spA0 = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp8C, &spD0);
                        Matrix_MultVecZ(5.0f * sGiantModeScaleFactor, &sp70);
                        sp70.y = 2.0f * sGiantModeScaleFactor;
                        sp64.y = 0.3f * sGiantModeScaleFactor;
                        sp64.z = 0.0f;
                        sp64.x = 0.0f;
                        sp7C.x = spD0.x;
                        sp7C.y = spA0;
                        sp7C.z = spD0.z;
                        func_800B0F18(play, &sp7C, &sp70, &sp64, &D_809DFA98, &D_809DFA98,
                                      (Rand_ZeroFloat(500.0f) + 1200.0f) * sGiantModeScaleFactor,
                                      20.0f * sGiantModeScaleFactor, Rand_ZeroFloat(5.0f) + 14.0f);
                    }
                }
            }
    }

    this->colliderCylinder.dim.radius = 150.0f * sGiantModeScaleFactor;
    this->colliderCylinder.dim.height = 200.0f * sGiantModeScaleFactor;
    this->colliderCylinder.dim.yShift = 0;
    Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder.base);
}

void func_809DBFB4(Boss02* this, PlayState* play) {
    Boss02* otherTwinmold = this->otherTwinmold;
    s32 temp;
    s32 i;
    u8 damage;

    if (this->unk_0154 == 0) {
        if (sIsInGiantMode) {
            temp = 0;
        } else {
            temp = 1;
        }

        for (i = temp; i < ARRAY_COUNT(this->colliderSphere2Elements); i++) {
            if (this->colliderSphere2.elements[i].base.acElemFlags & ACELEM_HIT) {
                this->colliderSphere2.elements[i].base.acElemFlags &= ~ACELEM_HIT;
                this->unk_0154 = 15;
                this->unk_0156 = 15;

                if (i == 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_DAMAGE_OLD);
                    this->unk_015C = 1;
                } else {
                    Audio_PlaySfx_AtPos(&this->unk_167C, NA_SE_EN_INBOSS_DAMAGE_OLD);
                    this->unk_015C = 10;
                }

                if (sIsInGiantMode) {
                    this->unk_0152 = 15;
                } else {
                    this->unk_0152 = 12;
                }

                damage = this->actor.colChkInfo.damage;

                if (otherTwinmold->unk_0144 < 20) {
                    this->actor.colChkInfo.health -= damage;

                    if ((s8)this->actor.colChkInfo.health <= 0) {
                        Actor_Kill(this->actor.child);
                        this->actor.child = NULL;
                        Enemy_StartFinishingBlow(play, &this->actor);
                        this->skelAnime.playSpeed = 2.0f;
                        this->unk_0144 = 20;

                        if (otherTwinmold->unk_0144 >= 10) {
                            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
                            GameInteractor_ExecuteOnBossDefeated(this->actor.id); // 2S2H Time Splits
                        } else {
                            otherTwinmold->unk_0195 = 1;
                        }

                        this->unk_0146[1] = BREG(70) + 170;
                        this->unk_0146[0] = 0;
                        this->actor.shape.rot.x = 0x4000;

                        if (!sIsInGiantMode) {
                            if (this->actor.world.pos.y < -200.0f) {
                                this->actor.world.pos.y = -200.0f;
                            }
                        } else if (this->actor.world.pos.y < 3130.0f) {
                            this->actor.world.pos.y = 3130.0f;
                        }

                        sTwinmoldBattleHandler->unk_1D20 = 100;

                        if (this == sRedTwinmold) {
                            sTwinmoldBattleHandler->unk_0194 = 0;
                        } else {
                            sTwinmoldBattleHandler->unk_0194 = 1;
                        }
                    }
                }
            }
        }
    }
}

void Boss02_Tail_Update(Actor* thisx, PlayState* play) {
    Boss02* this = (Boss02*)thisx;
    s32 pad;
    Vec3f pos;
    CollisionPoly* outPoly;

    this->unk_014C++;
    if ((this->unk_014C + this->actor.params) & 1) {
        pos = this->actor.world.pos;
        this->actor.focus.pos = this->actor.world.pos;
        if (sIsInGiantMode) {
            pos.y = 5000.0f;
        } else {
            pos.y = 2000.0f;
        }

        if ((this->actor.focus.pos.y < BgCheck_EntityRaycastFloor1(&play->colCtx, &outPoly, &pos)) || sIsInGiantMode) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        } else {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }
    }
}

void Boss02_Twinmold_Update(Actor* thisx, PlayState* play) {
    Vec3f sp3C;
    Boss02* this = (Boss02*)thisx;
    s32 pad;
    s16 i;

    if (!sIsInGiantMode) {
        Actor_SetScale(&this->actor, 0.6f);
    } else {
        Actor_SetScale(&this->actor, 0.060000001f);
    }

    if (sTwinmoldBattleHandler->giantsMaskCsState == GIANTS_MASK_CS_STATE_WAITING_FOR_MASK) {
        for (i = 0; i < ARRAY_COUNT(this->unk_0146); i++) {
            if (this->unk_0146[i] != 0) {
                this->unk_0146[i]--;
            }
        }

        if (this->unk_016C != 0) {
            this->unk_016C--;
        }

        if (this->unk_0154 != 0) {
            this->unk_0154--;
        }

        if (this->unk_0156 != 0) {
            this->unk_0156--;
        }

        if (this->unk_0152 != 0) {
            this->unk_0152--;
        }

        this->unk_014C++;

        this->actionFunc(this, play);

        if (this->unk_016C != 0) {
            if ((this->unk_016C == 60) && (this->unk_0144 < 20)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_ROAR_OLD);
            }
            Actor_PlaySfx(&this->actor, NA_SE_EN_INBOSS_SAND_OLD - SFX_FLAG);

            if (this->unk_0144 > 20) {
                sp3C.x = Rand_CenteredFloat(100.0f * sGiantModeScaleFactor) + this->unk_0170.x;
                sp3C.y = Rand_CenteredFloat(50.0f * sGiantModeScaleFactor) + this->unk_0170.y;
                sp3C.z = Rand_CenteredFloat(100.0f * sGiantModeScaleFactor) + this->unk_0170.z;
                Boss02_SpawnEffectSand(play->specialEffects, &sp3C, Rand_ZeroFloat(3.0f) + 6.0f);
            }

            if ((this->unk_014C % 2) == 0) {
                sp3C.x = Rand_CenteredFloat(100.0f * sGiantModeScaleFactor) + this->unk_0170.x;
                sp3C.y = Rand_CenteredFloat(50.0f * sGiantModeScaleFactor) + this->unk_0170.y;
                sp3C.z = Rand_CenteredFloat(100.0f * sGiantModeScaleFactor) + this->unk_0170.z;
                Boss02_SpawnEffectSand(play->specialEffects, &sp3C, Rand_ZeroFloat(3.0f) + 6.0f);
            }
        }

        if ((this->unk_0144 < 20) && (this->unk_0144 != 10)) {
            func_809DBFB4(this, play);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere1.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSphere1.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere1.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere2.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSphere2.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere2.base);
        }

        switch (this->unk_015C) {
            case 1:
                this->unk_015A = 0;
                this->unk_0158 = this->unk_015A;
                this->unk_015C++;

            case 2:
                this->unk_015A += 2;
                if (this->unk_015A > 20) {
                    this->unk_015C++;
                }
                this->unk_0156 |= 4;
                break;

            case 3:
                this->unk_0158 += 2;
                if (this->unk_0158 > 20) {
                    this->unk_015C = 0;
                }
                this->unk_0156 |= 4;
                break;

            case 10:
                this->unk_015A = 21;
                this->unk_0158 = this->unk_015A;
                this->unk_015C++;

            case 11:
                this->unk_0158 -= 2;
                if (this->unk_0158 <= 0) {
                    this->unk_0158 = 0;
                    this->unk_015C++;
                }
                this->unk_0156 |= 4;
                break;

            case 12:
                this->unk_015A -= 2;
                if (this->unk_015A <= 0) {
                    this->unk_015A = 0;
                    this->unk_015C = 0;
                }
                this->unk_0156 |= 4;
                break;
        }
    }
}

void Boss02_BattleHandler_Update(Actor* thisx, PlayState* play) {
    Boss02* this = (Boss02*)thisx;

    this->giantModeScaleFactor = sGiantModeScaleFactor;
    play->envCtx.sandstormState = SANDSTORM_D;

    if (sBlueWarp != NULL) {
        this->fogNear = KREG(23) + -15.0f;
        D_801F4E30 = 0;
        play->envCtx.lightSettingOverride = 1;
        play->skyboxId = SKYBOX_NORMAL_SKY;
    } else if (!sIsInGiantMode) {
        this->fogNear = 0.0f;
        D_801F4E30 = this->unk_1D7C;
        play->envCtx.lightSettingOverride = 0;
        play->skyboxId = SKYBOX_2;
    } else {
        this->fogNear = KREG(23) + -15.0f;
        D_801F4E30 = ((KREG(24) * 0.1f) + 1.0f) * this->unk_1D7C;
        play->envCtx.lightSettingOverride = 1;
        play->skyboxId = SKYBOX_NORMAL_SKY;
    }

    Math_ApproachS(&this->unk_1D7C, this->unk_1D7E, 1, 3);

    if (this->unk_0146[0] != 0) {
        this->unk_0146[0]--;
    }

    if (sBlueWarp == NULL) {
        if (sTwinmoldMusicStartTimer != 0) {
            sTwinmoldMusicStartTimer--;
            if (sTwinmoldMusicStartTimer == 0) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
            }
        }
    }

    Boss02_UpdateEffects(play);
    func_809DEAC4(this, play);
    Boss02_HandleGiantsMaskCutscene(this, play);
}

Gfx* D_809DFA9C[] = {
    gTwinmoldBodySegment1DL,  gTwinmoldBodySegment2DL,  gTwinmoldBodySegment3DL,  gTwinmoldBodySegment4DL,
    gTwinmoldBodySegment5DL,  gTwinmoldBodySegment6DL,  gTwinmoldBodySegment7DL,  gTwinmoldBodySegment8DL,
    gTwinmoldBodySegment9DL,  gTwinmoldBodySegment10DL, gTwinmoldBodySegment11DL, gTwinmoldBodySegment12DL,
    gTwinmoldBodySegment13DL, gTwinmoldBodySegment14DL, gTwinmoldBodySegment15DL, gTwinmoldBodySegment16DL,
    gTwinmoldBodySegment17DL, gTwinmoldBodySegment18DL, gTwinmoldBodySegment19DL, gTwinmoldBodySegment20DL,
    gTwinmoldBodySegment21DL, gTwinmoldBodyTailDL,
};

Vec3f D_809DFAF4 = { -10000.0f, -100000.0f, -100000.0f };

void Boss02_Twinmold_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss02* this = (Boss02*)thisx;
    s32 i;
    s32 idx;
    Mtx* mtxHead = GRAPH_ALLOC(play->state.gfxCtx, 23 * sizeof(Mtx));
    Mtx* mtx = mtxHead;
    s32 phi_v0;
    f32 phi_f12;
    f32 spAC;
    f32 spA8;
    f32 spA4;
    f32 spA0;
    f32 sp9C;
    f32 sp98;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (TWINMOLD_GET_TYPE(&this->actor) == TWINMOLD_TYPE_RED) {
        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gTwinmoldRedSkinTex));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gTwinmoldBlueSkinTex));
    }

    gSPSegment(POLY_OPA_DISP++, 0x0D, mtxHead);

    if (!sIsInGiantMode) {
        sp98 = -500.0f;
    } else {
        sp98 = 3100.0f;
    }

    sp9C = this->unk_0152 * (M_PIf / 4) * (1.0f / 15);
    if (this->unk_0144 < 20) {
        spAC = Math_SinS(this->unk_014C * 0x2200) * sp9C * 0.5f;
        spA8 = Math_CosS(this->unk_014C * 0x3200) * sp9C;
    } else {
        spAC = Math_SinS(this->unk_014C * 0x1000) * sp9C * 0.5f;
        spA8 = Math_CosS(this->unk_014C * 0x1500) * sp9C;
    }

    idx = (this->unk_014E + 196) % ARRAY_COUNT(this->unk_01BC);
    Matrix_Translate(this->unk_01BC[idx].x, this->unk_01BC[idx].y, this->unk_01BC[idx].z, MTXMODE_NEW);
    Matrix_RotateYF(this->unk_0B1C[idx].y + spA8, MTXMODE_APPLY);
    Matrix_RotateXFApply(spAC - this->unk_0B1C[idx].x);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 0.0f, 250.0f, MTXMODE_APPLY);
    Matrix_MultVecZ(150.0f, &this->unk_147C[0]);
    this->unk_0188 = this->unk_017C;
    Matrix_MultVecZ(300.0f, &this->unk_017C);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->unk_017C);

    if ((this->unk_0156 & 1) && (this->unk_0158 == 0)) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 0x384, 0x44B);
    }

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    spA4 = 0.0f;
    spA0 = 0.0f;
    for (i = 0; i < ARRAY_COUNT(D_809DFA9C); i++, mtx++) {
        if (this->unk_0195 != 0) {
            phi_v0 = (D_809DF5E4[i + 1] + this->unk_014E) % ARRAY_COUNT(this->unk_01BC);
        } else {
            phi_v0 = (D_809DF5B4[i + 1] + this->unk_014E) % ARRAY_COUNT(this->unk_01BC);
        }

        if ((i == 21) && (this->unk_0144 < 20)) {
            spA4 = spAC;
            spA0 = spA8;
        }

        Matrix_Translate(this->unk_01BC[phi_v0].x, this->unk_01BC[phi_v0].y, this->unk_01BC[phi_v0].z, MTXMODE_NEW);
        Matrix_RotateYF(this->unk_0B1C[phi_v0].y + spA0, MTXMODE_APPLY);
        Matrix_RotateXFApply(spA4 - this->unk_0B1C[phi_v0].x);
        Matrix_RotateZF(this->unk_0B1C[phi_v0].z, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYF(M_PIf / 2, MTXMODE_APPLY);
        Matrix_RotateXFApply(-(M_PIf / 2));
        Matrix_ToMtx(mtx);

        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        if ((this->unk_0156 & 1) && (i >= this->unk_0158) && (this->unk_015A >= i)) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 0x384, 0x44B);
        }

        if (i < this->unk_1678) {
            if (sp98 < this->unk_01BC[phi_v0].y) {
                gSPDisplayList(POLY_OPA_DISP++, D_809DFA9C[i]);
            }
            POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        }

        if (i == 21) {
            phi_f12 = 600.0f;
        } else {
            phi_f12 = 200.0f;
        }

        Matrix_MultVecX(phi_f12, &this->unk_147C[i + 1]);

        if (i == 21) {
            Actor* child = this->actor.child;

            if (child != NULL) {
                Matrix_MultVecX(500.0f, &this->actor.child->world.pos);
            }

            func_809DA50C(1, &this->colliderSphere2, &this->unk_147C[i + 1]);
            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &this->unk_147C[i + 1], &this->unk_167C,
                                         &this->actor.projectedW);
        } else {
            func_809DA50C(i, &this->colliderSphere1, &this->unk_147C[i + 1]);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (!sIsInGiantMode) {
        func_809DA50C(0, &this->colliderSphere2, &D_809DFAF4);
        func_809DA50C(21, &this->colliderSphere1, &this->unk_147C[0]);
    } else {
        func_809DA50C(0, &this->colliderSphere2, &this->unk_147C[0]);
        func_809DA50C(21, &this->colliderSphere1, &D_809DFAF4);
    }
}

void Boss02_BattleHandler_Draw(Actor* thisx, PlayState* play) {
    Boss02_DrawEffects(play);
}

void Boss02_UpdateEffects(PlayState* play) {
    TwinmoldEffect* effect = (TwinmoldEffect*)play->specialEffects;
    f32 floorY;
    s16 i;

    if (!sIsInGiantMode) {
        floorY = 0.0f;
    } else {
        floorY = 3150.0f;
    }

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type) {
            effect->timer++;
            effect->pos.x += effect->velocity.x * sGiantModeScaleFactor;
            effect->pos.y += effect->velocity.y * sGiantModeScaleFactor;
            effect->pos.z += effect->velocity.z * sGiantModeScaleFactor;
            effect->velocity.y += effect->accel.y;

            if (effect->type < TWINMOLD_EFFECT_FRAGMENT) {
                Math_ApproachF(&effect->scale, effect->targetScale, 0.1f, 0.1f);
                if (effect->type == TWINMOLD_EFFECT_BLACK_DUST) {
                    effect->alpha -= 18;
                } else {
                    effect->alpha -= 15;
                }

                if (effect->alpha <= 0) {
                    effect->type = TWINMOLD_EFFECT_NONE;
                }
            } else if (effect->type == TWINMOLD_EFFECT_FRAGMENT) {
                effect->rotX += 0x1000;
                effect->rotY += 0x1500;
                if ((effect->pos.y < floorY) || (effect->timer > 50)) {
                    effect->type = TWINMOLD_EFFECT_NONE;
                }
            } else if (effect->type == TWINMOLD_EFFECT_FLASH) {
                Math_ApproachF(&effect->scale, 80.0f, 0.2f, 20.0f);
                effect->alpha -= 15;
                if (effect->alpha <= 0) {
                    effect->type = TWINMOLD_EFFECT_NONE;
                }
            }
        }
    }
}

void Boss02_DrawEffects(PlayState* play) {
    u8 flag = false;
    TwinmoldEffect* effect = (TwinmoldEffect*)play->specialEffects;
    s16 i;
    u8 alpha;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_SAND) {
            // Here and below, key by effect type merged with epoch
            FrameInterpolation_RecordOpenChild(effect, (effect->epoch << 4) | effect->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                gSPDisplayList(POLY_XLU_DISP++, gTwinmoldDustMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 185, 140, 70, 128);
                flag++;
            }

            alpha = effect->alpha;
            if (effect->alpha > 255) {
                alpha = 255;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 185, 140, 70, alpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, effect->timer + (i * 3), (effect->timer + (i * 3)) * 5,
                                          32, 64, 1, 0, 0, 32, 32, 1, 5, 0, 0));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale * sGiantModeScaleFactor, effect->scale * sGiantModeScaleFactor, 1.0f,
                         MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gTwinmoldDustModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = (TwinmoldEffect*)play->specialEffects;
    for (i = 0, flag = false; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_FRAGMENT) {
            FrameInterpolation_RecordOpenChild(effect, (effect->epoch << 4) | effect->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                gDPSetCombineLERP(POLY_OPA_DISP++, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE,
                                  0, SHADE, 0, PRIMITIVE, 0);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x01, 100, 100, 120, 255);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateYS(effect->rotY, MTXMODE_APPLY);
            Matrix_RotateXS(effect->rotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale * sGiantModeScaleFactor, effect->scale * sGiantModeScaleFactor,
                         effect->scale * sGiantModeScaleFactor, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gEffFragments1DL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = (TwinmoldEffect*)play->specialEffects;
    for (i = 0, flag = false; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_FLASH) {
            FrameInterpolation_RecordOpenChild(effect, (effect->epoch << 4) | effect->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) { //! @bug - dev forgot to set flag to 1, should only apply to first entry?
                gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (u8)effect->alpha);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale * sGiantModeScaleFactor, effect->scale * sGiantModeScaleFactor, 1.0f,
                         MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    effect = (TwinmoldEffect*)play->specialEffects;
    for (i = 0, flag = false; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_BLACK_DUST) {
            FrameInterpolation_RecordOpenChild(effect, (effect->epoch << 4) | effect->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                gSPDisplayList(POLY_XLU_DISP++, gTwinmoldDustMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 30, 30, 30, 128);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 30, 30, 30, (u8)effect->alpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, effect->timer + (i * 3), (effect->timer + (i * 3)) * 5,
                                          32, 64, 1, 0, 0, 32, 32, 1, 5, 0, 0));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale * sGiantModeScaleFactor, effect->scale * sGiantModeScaleFactor, 1.0f,
                         MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gTwinmoldDustModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * This function is primarily responsible for handling the cutscene where the player puts on
 * or takes off the Giant's Mask. It does some manual camera control, adjusts the player's
 * scale, switches between the "Normal" and "Giant" variations of the arena, and creates a
 * "flash" when the cutscene ends by filling the screen with white. In addition, this function
 * is responsible for updating the EnvironmentContext's fogNear variable, which it does even
 * when the Giant's Mask cutscene is not playing.
 */
void Boss02_HandleGiantsMaskCutscene(Boss02* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;
    Actor* actorIter;
    Vec3f subCamEyeOffset;
    u8 switchArena = false;
    f32 scale;
    s16 alpha;

    this->giantsMaskCsTimer++;

    switch (this->giantsMaskCsState) {
        case GIANTS_MASK_CS_STATE_WAITING_FOR_MASK:
            if (player->stateFlags1 & PLAYER_STATE1_100) {
                Cutscene_StartManual(play, &play->csCtx);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
                Play_EnableMotionBlur(150);
                this->giantsMaskCsTimer = 0;
                this->subCamAtVel = 0.0f;
                this->subCamUpRotZScale = 0.0f;
                if (!sIsInGiantMode) {
                    this->giantsMaskCsState = GIANTS_MASK_CS_STATE_MASK_ON;
                    this->subCamEyeOffsetY = 10.0f;
                    this->subCamDistZFromPlayer = 60.0f;
                    this->subCamAtOffsetY = 23.0f;
                    this->playerScale = 0.01f;
                    this->fogNear = 0.0f;
                    goto maskOn; // Jumps to this label to prevent the player from skipping the cutscene immediately.
                } else {
                    this->giantsMaskCsState = GIANTS_MASK_CS_STATE_MASK_OFF;
                    this->subCamEyeOffsetY = 10.0f;
                    this->subCamDistZFromPlayer = 200.0f;
                    this->subCamAtOffsetY = 273.0f;
                    this->playerScale = 0.1f;
                    this->fogNear = -100.0f;
                    switchArena = true;
                    goto maskOff; // Jumps to this label to prevent the player from skipping the cutscene immediately.
                }
            }
            break;

        case GIANTS_MASK_CS_STATE_MASK_ON:
            if ((this->giantsMaskCsTimer < 80) && sCanSkipMaskOnCs &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT | BTN_DPAD_EQUIP)) {
                this->giantsMaskCsState++;
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_STARTED;
                this->giantsMaskCsTimer = 0;
                break;
            }

        maskOn:
            if (this->giantsMaskCsTimer >= 50) {
                if (this->giantsMaskCsTimer == (u32)(BREG(43) + 60)) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_GIANT);
                }

                // Makes the camera quickly move away from the player and look upwards as the player
                // increases in size. Also decreases the fogNear from 0.0f to -100.0f.
                Math_ApproachF(&this->subCamDistZFromPlayer, 200.0f, 0.1f, this->subCamAtVel * 640.0f);
                Math_ApproachF(&this->subCamAtOffsetY, 273.0f, 0.1f, this->subCamAtVel * 150.0f);
                Math_ApproachF(&this->playerScale, 0.1f, 0.2f, this->subCamAtVel * 0.1f);
                Math_ApproachF(&this->fogNear, -100.0f, 1.0f, this->subCamAtVel * 100.0f);
                Math_ApproachF(&this->subCamAtVel, 1.0f, 1.0f, 0.001f);
            } else {
                // Makes the camera slowly approach the player.
                Math_ApproachF(&this->subCamDistZFromPlayer, 30.0f, 0.1f, 1.0f);
            }

            // Makes the camera roll side-to-side at the start of the cutscene, and stops
            // the camera from rolling after 51 frames have passed.
            if (this->giantsMaskCsTimer > 50) {
                Math_ApproachZeroF(&this->subCamUpRotZScale, 1.0f, 0.06f);
            } else {
                Math_ApproachF(&this->subCamUpRotZScale, 0.4f, 1.0f, 0.02f);
            }

            if (this->giantsMaskCsTimer == 107) {
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_STARTED;
            }

            if (this->giantsMaskCsTimer > 120) {
                switchArena = true;
                sCanSkipMaskOnCs = true;
                goto done;
            }
            break;

        case GIANTS_MASK_CS_STATE_MASK_ON_SKIPPED:
            if (this->giantsMaskCsTimer >= 8) {
                switchArena = true;
                goto done;
            }
            break;

        case GIANTS_MASK_CS_STATE_MASK_OFF:
            if ((this->giantsMaskCsTimer < 30) && sCanSkipMaskOffCs &&
                CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button,
                              BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT | BTN_DPAD_EQUIP)) {
                this->giantsMaskCsState++;
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_STARTED;
                this->giantsMaskCsTimer = 0;
                break;
            }

        maskOff:
            if (this->giantsMaskCsTimer != 0) {
                if (this->giantsMaskCsTimer == (u32)(BREG(44) + 10)) {
                    Audio_PlaySfx(NA_SE_PL_TRANSFORM_NORAML);
                }

                // Makes the camera move slowly towards the player and look downwards as the player
                // decreases in size. Also increases the fogNear from -100.0f to 0.0f.
                Math_ApproachF(&this->subCamDistZFromPlayer, 60.0f, 0.1f, this->subCamAtVel * 640.0f);
                Math_ApproachF(&this->subCamAtOffsetY, 23.0f, 0.1f, this->subCamAtVel * 150.0f);
                Math_ApproachF(&this->playerScale, 0.01f, 0.1f, 0.003f);
                Math_ApproachF(&this->fogNear, 0.0f, 1.0f, this->subCamAtVel * 100.0f);
                Math_ApproachF(&this->subCamAtVel, 2.0f, 1.0f, 0.01f);
            }

            if (this->giantsMaskCsTimer == 42) {
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_STARTED;
            }

            if (this->giantsMaskCsTimer > 50) {
                sCanSkipMaskOffCs = true;
                goto done;
            }
            break;

        case GIANTS_MASK_CS_STATE_MASK_OFF_SKIPPED:
            if (this->giantsMaskCsTimer >= 8) {
                goto done;
            }
            break;

        done:
        case GIANTS_MASK_CS_STATE_DONE:
            // Ends the cutscene and restores the player's scale to its "original" value of 0.1f.
            // This might seem strange in the case where the player has put on the Giant's Mask,
            // since they seem so large after the cutscene ends. However, the player is still
            // their normal size; the arena and everything in it has shrunk instead.
            this->giantsMaskCsState = GIANTS_MASK_CS_STATE_WAITING_FOR_MASK;
            func_80169AFC(play, this->subCamId, 0);
            this->subCamId = SUB_CAM_ID_DONE;
            Cutscene_StopManual(play, &play->csCtx);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            player->stateFlags1 &= ~PLAYER_STATE1_100;
            this->playerScale = 0.01f;
            Play_DisableMotionBlur();
            break;
    }

    if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
        Actor_SetScale(&player->actor, this->playerScale * 1.5f);
    } else {
        Actor_SetScale(&player->actor, this->playerScale);
    }

    play->envCtx.adjLightSettings.fogNear = this->fogNear;

    // Switches from the "normal" variation of the arena to the "giant" variation (or vice versa)
    // at the appropriate point in the Giant's Mask cutscene.
    if (switchArena) {
        sIsInGiantMode = true - sIsInGiantMode;
        if (!sIsInGiantMode) {
            sGiantModeScaleFactor = 1.0f;
        } else {
            sGiantModeScaleFactor = 0.1f;
        }

        this->giantModeScaleFactor = sGiantModeScaleFactor;

        if (!sIsInGiantMode) {
            // In this case, the player is switching from the "giant" arena to the "normal" arena.
            // The "giant" arena sits 3150 units above the "normal" arena, so this moves the player,
            // Twinmold, and the blue warp down and scales their world coordinates upwards to be
            // appropriate for the larger arena.
            if (sBlueWarp != NULL) {
                sBlueWarp->unk_203 = 0;
                sBlueWarp->unk_204 = 1.0f;
                sBlueWarp->dyna.actor.world.pos.y = 60.0f;
            }

            player->actor.world.pos.x *= 10.0f;
            player->actor.world.pos.y -= 3150.0f;
            player->actor.world.pos.y *= 10.0f;
            player->fallStartHeight = player->actor.world.pos.y;
            player->actor.world.pos.z *= 10.0f;

            // If the blue warp exists, and if the player is within 60 units of its center after switching
            // to the "normal" arena, this code will move the player to a fixed point next to the blue warp.
            // This prevents the player from instantly activating the blue warp after switching arenas, and
            // it relies on the blue warp's XZ-coordinates being (0, 0) as they are in Twinmold's boss room.
            if ((sBlueWarp != NULL) && ((SQ(player->actor.world.pos.z) + SQ(player->actor.world.pos.x)) < SQ(60.0f))) {
                player->actor.world.pos.z = 60.0f;
                player->actor.world.pos.x = 60.0f;
            }

            sRedTwinmold->actor.world.pos.x *= 10.0f;
            sRedTwinmold->actor.world.pos.y -= 3150.0f;
            sRedTwinmold->actor.world.pos.y *= 10.0f;
            sRedTwinmold->actor.world.pos.z *= 10.0f;

            sRedTwinmold->unk_0170.x *= 10.0f;
            sRedTwinmold->unk_0170.y -= 3150.0f;
            sRedTwinmold->unk_0170.y *= 10.0f;
            sRedTwinmold->unk_0170.z *= 10.0f;

            for (i = 0; i < ARRAY_COUNT(this->unk_01BC); i++) {
                sRedTwinmold->unk_01BC[i].x *= 10.0f;
                sRedTwinmold->unk_01BC[i].y -= 3150.0f;
                sRedTwinmold->unk_01BC[i].y *= 10.0f;
                sRedTwinmold->unk_01BC[i].z *= 10.0f;
            }

            sRedTwinmold->unk_01B0.x *= 10.0f;
            sRedTwinmold->unk_01B0.y -= 3150.0f;
            sRedTwinmold->unk_01B0.y *= 10.0f;
            sRedTwinmold->unk_01B0.z *= 10.0f;

            sRedTwinmold->unk_017C.y -= 3150.0f;
            sRedTwinmold->unk_017C.y *= 10.0f;
            sRedTwinmold->unk_0188.y -= 3150.0f;
            sRedTwinmold->unk_0188.y *= 10.0f;

            sBlueTwinmold->actor.world.pos.x *= 10.0f;
            sBlueTwinmold->actor.world.pos.y -= 3150.0f;
            sBlueTwinmold->actor.world.pos.y *= 10.0f;
            sBlueTwinmold->actor.world.pos.z *= 10.0f;

            sBlueTwinmold->unk_0170.x *= 10.0f;
            sBlueTwinmold->unk_0170.y -= 3150.0f;
            sBlueTwinmold->unk_0170.y *= 10.0f;
            sBlueTwinmold->unk_0170.z *= 10.0f;

            for (i = 0; i < ARRAY_COUNT(this->unk_01BC); i++) {
                sBlueTwinmold->unk_01BC[i].x *= 10.0f;
                sBlueTwinmold->unk_01BC[i].y -= 3150.0f;
                sBlueTwinmold->unk_01BC[i].y *= 10.0f;
                sBlueTwinmold->unk_01BC[i].z *= 10.0f;
            }

            sBlueTwinmold->unk_01B0.x *= 10.0f;
            sBlueTwinmold->unk_01B0.y -= 3150.0f;
            sBlueTwinmold->unk_01B0.y *= 10.0f;
            sBlueTwinmold->unk_01B0.z *= 10.0f;

            sBlueTwinmold->unk_017C.y -= 3150.0f;
            sBlueTwinmold->unk_017C.y *= 10.0f;
            sBlueTwinmold->unk_0188.y -= 3150.0f;
            sBlueTwinmold->unk_0188.y *= 10.0f;

        } else {
            // In this case, the player is switching from the "normal" arena to the "giant" arena.
            // The "giant" arena sits 3150 units above the "normal" arena, so this moves the player,
            // Twinmold, and the blue warp up and scales their world coordinates downwards to be
            // appropriate for the smaller arena.
            if (sBlueWarp != NULL) {
                sBlueWarp->unk_203 = 1;
                sBlueWarp->unk_204 = 0.1f;
                sBlueWarp->dyna.actor.world.pos.y = 3155.0f;
            }

            player->actor.world.pos.x *= 0.1f;
            player->actor.world.pos.y *= 0.1f;
            player->actor.world.pos.y += 3150.0f;
            player->fallStartHeight = player->actor.world.pos.y;
            player->actor.world.pos.z *= 0.1f;

            sRedTwinmold->actor.world.pos.x *= 0.1f;
            sRedTwinmold->actor.world.pos.y *= 0.1f;
            sRedTwinmold->actor.world.pos.y += 3150.0f;
            sRedTwinmold->actor.world.pos.z *= 0.1f;

            sRedTwinmold->unk_0170.x *= 0.1f;
            sRedTwinmold->unk_0170.y *= 0.1f;
            sRedTwinmold->unk_0170.y += 3150.0f;
            sRedTwinmold->unk_0170.z *= 0.1f;

            for (i = 0; i < ARRAY_COUNT(this->unk_01BC); i++) {
                sRedTwinmold->unk_01BC[i].x *= 0.1f;
                sRedTwinmold->unk_01BC[i].y *= 0.1f;
                sRedTwinmold->unk_01BC[i].y += 3150.0f;
                sRedTwinmold->unk_01BC[i].z *= 0.1f;
            }

            sRedTwinmold->unk_01B0.x *= 0.1f;
            sRedTwinmold->unk_01B0.y *= 0.1f;
            sRedTwinmold->unk_01B0.y += 3150.0f;
            sRedTwinmold->unk_01B0.z *= 0.1f;

            sRedTwinmold->unk_017C.y *= 0.1f;
            sRedTwinmold->unk_017C.y += 3150.0f;
            sRedTwinmold->unk_0188.y *= 0.1f;
            sRedTwinmold->unk_0188.y += 3150.0f;

            sBlueTwinmold->actor.world.pos.x *= 0.1f;
            sBlueTwinmold->actor.world.pos.y *= 0.1f;
            sBlueTwinmold->actor.world.pos.y += 3150.0f;
            sBlueTwinmold->actor.world.pos.z *= 0.1f;

            sBlueTwinmold->unk_0170.x *= 0.1f;
            sBlueTwinmold->unk_0170.y *= 0.1f;
            sBlueTwinmold->unk_0170.y += 3150.0f;
            sBlueTwinmold->unk_0170.z *= 0.1f;

            for (i = 0; i < ARRAY_COUNT(this->unk_01BC); i++) {
                sBlueTwinmold->unk_01BC[i].x *= 0.1f;
                sBlueTwinmold->unk_01BC[i].y *= 0.1f;
                sBlueTwinmold->unk_01BC[i].y += 3150.0f;
                sBlueTwinmold->unk_01BC[i].z *= 0.1f;
            }

            sBlueTwinmold->unk_01B0.x *= 0.1f;
            sBlueTwinmold->unk_01B0.y *= 0.1f;
            sBlueTwinmold->unk_01B0.y += 3150.0f;
            sBlueTwinmold->unk_01B0.z *= 0.1f;

            sBlueTwinmold->unk_017C.y *= 0.1f;
            sBlueTwinmold->unk_017C.y += 3150.0f;
            sBlueTwinmold->unk_0188.y *= 0.1f;
            sBlueTwinmold->unk_0188.y += 3150.0f;
        }

        player->actor.home.pos = player->actor.world.pos;
        player->actor.prevPos = player->actor.world.pos;

        // Kills the existing instance of Twinmold's arena and spawns a new instance with the appropriate
        // scale and at the appropriate height.
        actorIter = play->actorCtx.actorLists[ACTORCAT_BG].first;
        while (actorIter != NULL) {
            if (actorIter->id == ACTOR_BG_INIBS_MOVEBG) {
                Actor_Kill(actorIter);
                break;
            }

            actorIter = actorIter->next;
        }
        {
            f32 arenaPosY = sIsInGiantMode ? 3150.0f : 0.0f;

            Actor_Spawn(&play->actorCtx, play, ACTOR_BG_INIBS_MOVEBG, 0, arenaPosY, 0, 0, 0, 0, sIsInGiantMode);
        }

        // Moves the props in Twinmold's arena and the Heart Container 3150 units up or down
        // so they're in the appropriate arena and scales them accordingly.
        actorIter = play->actorCtx.actorLists[ACTORCAT_BOSS].first;
        while (actorIter != NULL) {
            if ((actorIter->id == ACTOR_EN_TANRON5) || (actorIter->id == ACTOR_ITEM_B_HEART)) {
                if (!sIsInGiantMode) {
                    actorIter->world.pos.y -= 3150.0f;
                    actorIter->world.pos.y *= 10.0f;

                    actorIter->floorHeight -= 3150.0f;
                    actorIter->floorHeight *= 10.0f;

                    scale = 10.0f;
                } else {
                    actorIter->world.pos.y *= 0.1f;
                    actorIter->world.pos.y += 3150.0f;

                    actorIter->floorHeight *= 0.1f;
                    actorIter->floorHeight += 3150.0f;

                    scale = 0.1f;
                }

                actorIter->world.pos.x *= scale;
                actorIter->world.pos.z *= scale;

                actorIter->speed *= scale;

                actorIter->velocity.x *= scale;
                actorIter->velocity.y *= scale;
                actorIter->velocity.z *= scale;

                actorIter->gravity *= scale;
                actorIter->terminalVelocity *= scale;

                actorIter->scale.x *= scale;
                actorIter->scale.y *= scale;
                actorIter->scale.z *= scale;

                if (actorIter->id == ACTOR_ITEM_B_HEART) {
                    ((ItemBHeart*)actorIter)->baseScale *= scale;
                }
            }

            actorIter = actorIter->next;
        }
    }

    switch (this->giantsMaskCsFlashState) {
        case GIANTS_MASK_CS_FLASH_STATE_NOT_STARTED:
            break;

        case GIANTS_MASK_CS_FLASH_STATE_STARTED:
            this->giantsMaskCsFlashAlpha = 0;
            Boss02_FillScreen(play, 255, 255, 255, 0);
            this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_INCREASE_ALPHA;
            Audio_PlaySfx(NA_SE_SY_TRANSFORM_MASK_FLASH);
            // fallthrough
        case GIANTS_MASK_CS_FLASH_STATE_INCREASE_ALPHA:
            this->giantsMaskCsFlashAlpha += 40;
            if (this->giantsMaskCsFlashAlpha >= 400) {
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_DECREASE_ALPHA;
            }

            alpha = this->giantsMaskCsFlashAlpha;
            alpha = CLAMP_MAX(alpha, 255);
            Boss02_SetFillScreenAlpha(play, alpha);
            break;

        case GIANTS_MASK_CS_FLASH_STATE_DECREASE_ALPHA:
            this->giantsMaskCsFlashAlpha -= 40;
            if (this->giantsMaskCsFlashAlpha <= 0) {
                this->giantsMaskCsFlashAlpha = 0;
                this->giantsMaskCsFlashState = GIANTS_MASK_CS_FLASH_STATE_NOT_STARTED;
                Boss02_StopFillScreen(play);
            } else {
                alpha = this->giantsMaskCsFlashAlpha;
                alpha = CLAMP_MAX(alpha, 255);
                Boss02_SetFillScreenAlpha(play, alpha);
            }
            break;
    }

    if ((this->giantsMaskCsState != GIANTS_MASK_CS_STATE_WAITING_FOR_MASK) && (this->subCamId != SUB_CAM_ID_DONE)) {
        Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(this->subCamDistZFromPlayer, &subCamEyeOffset);

        this->subCamEye.x = player->actor.world.pos.x + subCamEyeOffset.x;
        this->subCamEye.y = player->actor.world.pos.y + subCamEyeOffset.y + this->subCamEyeOffsetY;
        this->subCamEye.z = player->actor.world.pos.z + subCamEyeOffset.z;

        this->subCamAt.x = player->actor.world.pos.x;
        this->subCamAt.y = player->actor.world.pos.y + this->subCamAtOffsetY;
        this->subCamAt.z = player->actor.world.pos.z;

        this->subCamUpRotZ = Math_SinS(this->giantsMaskCsTimer * 1512) * this->subCamUpRotZScale;
        Matrix_RotateZF(this->subCamUpRotZ, MTXMODE_APPLY);
        Matrix_MultVecY(1.0f, &this->subCamUp);
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
        ShrinkWindow_Letterbox_SetSizeTarget(27);
    }
}

void func_809DEAC4(Boss02* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Boss02* sp68;
    Vec3f subCamEye;
    f32 subCamEyeOffset = 0.0f;

    this->unk_1D1C++;
    if (this->unk_0194 == 0) {
        sp68 = sRedTwinmold;
    } else {
        sp68 = sBlueTwinmold;
    }

    switch (this->unk_1D20) {
        case 0:
            break;

        case 1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) ||
                ((u32)(KREG(13) + 15) >= this->unk_1D1C)) {
                break;
            }
            Cutscene_StartManual(play, &play->csCtx);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->unk_1D20 = 2;
            this->unk_1D1C = 0;

        case 2:
            player->actor.shape.rot.y = -0x8000;
            player->actor.world.rot.y = player->actor.shape.rot.y;
            this->subCamEye.x = player->actor.world.pos.x - 20.0f;
            this->subCamEye.y = (Player_GetHeight(player) + player->actor.world.pos.y) - 29.0f;
            this->subCamEye.z = player->actor.world.pos.z - 50;

            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = (Player_GetHeight(player) + player->actor.world.pos.y) - 17.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_1D1C >= 30) {
                if (this->unk_1D1C == 30) {
                    Rumble_Override(0.0f, 50, 200, 1);
                }
                this->unk_0150 += 0x4000;
                subCamEyeOffset = (Math_SinS(this->unk_0150) * (BREG(19) + 5)) * 0.1f;
                Matrix_RotateZF(Math_SinS(this->unk_1D1C * 0x3000) * ((KREG(28) * 0.001f) + 0.017f), MTXMODE_NEW);
                Matrix_MultVecY(1.0f, &this->subCamUp);
                Audio_PlaySfx_2(NA_SE_EV_EARTHQUAKE_LAST - SFX_FLAG);
            }

            if (this->unk_1D1C == 20) {
                this->unk_1D7E = 100;
            }

            if (this->unk_1D1C == 45) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
                sTwinmoldMusicStartTimer = KREG(91) + 43;
            }

            if (this->unk_1D1C == 85) {
                sRedTwinmold->unk_0144 = 11;
                sRedTwinmold->unk_014C = 0;
            }

            if (this->unk_1D1C == 92) {
                Rumble_Override(0.0f, 255, 30, 100);
            }

            if (this->unk_1D1C == 100) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_115);
            }

            if (this->unk_1D1C == 112) {
                this->unk_1D20 = 10;
                this->unk_1D1C = 0;
            }
            break;

        case 10:
            if (this->unk_1D1C < 310) {
                this->subCamAtNext = sp68->actor.world.pos;
                this->subCamEye.x = (sp68->actor.world.pos.x * 50.0f) * 0.001f;
                this->subCamEye.y = (sp68->actor.world.pos.y * 50.0f) * 0.001f;
                if (this->subCamEye.y < 100.0f) {
                    this->subCamEye.y = 100.0f;
                }
                this->subCamEye.z = (sp68->actor.world.pos.z * 100) * 0.001f;
            }

            Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.1f, 3000.0f);
            Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.1f, 3000.0f);
            Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.1f, 3000.0f);

            if (this->unk_1D1C == 100) {
                sBlueTwinmold->unk_0144 = 11;
                sBlueTwinmold->unk_014C = 0;
                this->unk_0194 = 1;
            }

            if (this->unk_1D1C == (u32)(KREG(92) + 125)) {
                TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                       Lib_SegmentedToVirtual(gTwinmoldTitleCardTex), 160, 180, 128, 40);
            }

            if (this->unk_1D1C == (u32)(BREG(27) + 335)) {
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->unk_1D20 = 0;
                sRedTwinmold->unk_0144 = sBlueTwinmold->unk_0144 = 3;
                sRedTwinmold->unk_0146[0] = sBlueTwinmold->unk_0146[0] = 60;
                SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_TWINMOLD);
            }
            break;

        case 100:
            if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
                Cutscene_StartManual(play, &play->csCtx);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
                this->unk_1D20 = 101;
                this->unk_1D1C = 0;
                this->subCamAtVel = 1.0f;
                this->subCamAt = sp68->actor.world.pos;
            } else {
                break;
            }

        case 101:
        case 102:
        case_10x:
            if ((this->unk_1D20 == 101) || (this->unk_1D20 == 103)) {
                this->subCamAtNext = sp68->actor.world.pos;
            } else {
                this->subCamAtNext = sp68->unk_147C[sp68->unk_1678];
            }

            this->subCamEye.x = player->actor.world.pos.x;
            this->subCamEye.y = player->actor.world.pos.y + 100.0f;
            if (!sIsInGiantMode) {
                if (this->subCamEye.y < 100.0f) {
                    this->subCamEye.y = 100.0f;
                }
            } else if (this->subCamEye.y < 3160.0f) {
                this->subCamEye.y = 3160.0f;
            }

            if (this->subCamAtNext.y < (100.0f * sGiantModeScaleFactor)) {
                this->subCamAtNext.y = (100.0f * sGiantModeScaleFactor);
            }

            this->subCamEye.z = player->actor.world.pos.z;
            Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.3f, this->subCamAtVel * 500.0f);
            Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.3f, this->subCamAtVel * 500.0f);
            Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.3f, this->subCamAtVel * 500.0f);
            Math_ApproachF(&this->subCamAtVel, 1.0f, 1.0f, 0.02f);
            break;

        case 103:
            this->unk_0150 += 0x4000;
            subCamEyeOffset = Math_SinS(this->unk_0150);
            subCamEyeOffset = (subCamEyeOffset * this->unk_0146[0]) * 1.5f;
            if (this->unk_1D1C == 30) {
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->unk_1D20 = 0;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                sp68->unk_0144 = 10;
                if ((sRedTwinmold->unk_0144 >= 10) && (sBlueTwinmold->unk_0144 >= 10)) {
                    f32 phi_f0;

                    this->unk_1D7E = 0;
                    if (!sIsInGiantMode) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 30.0f, -150.0f, 0, 1, 0,
                                    BHEART_PARAM_NORMAL);
                        phi_f0 = 60.0f;
                    } else {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 3153.0f, -15.0f, 0, 1, 0,
                                    BHEART_PARAM_SMALL);
                        phi_f0 = 3155.0f;
                    }
                    sBlueWarp = (DoorWarp1*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1,
                                                               0.0f, phi_f0, 0.0f, 0, 0, 0, ENDOORWARP1_FF_1);

                    if (!sIsInGiantMode) {
                        sBlueWarp->unk_203 = 0;
                        sBlueWarp->unk_204 = 1.0f;
                    } else {
                        sBlueWarp->unk_203 = 1;
                        sBlueWarp->unk_204 = 0.1f;
                    }
                }
            }
            goto case_10x;
    }

    if ((this->unk_1D20 != 0) && (this->subCamId != SUB_CAM_ID_DONE)) {
        subCamEye = this->subCamEye;
        subCamEye.y += subCamEyeOffset * sGiantModeScaleFactor;
        Play_SetCameraAtEyeUp(play, this->subCamId, &this->subCamAt, &subCamEye, &this->subCamUp);
        this->subCamUp.z = this->subCamUp.x = 0.0f;
        this->subCamUp.y = 1.0f;
        ShrinkWindow_Letterbox_SetSizeTarget(27);
    }
}

void Boss02_Reset(void) {
    sCanSkipMaskOnCs = false;
    sCanSkipMaskOffCs = false;
    sIsInGiantMode = false;
    sRedTwinmold = NULL;
    sBlueTwinmold = NULL;
    sTwinmoldBattleHandler = NULL;
    sTwinmoldMusicStartTimer = 0;
    sBlueWarp = NULL;
    sGiantModeScaleFactor = 1.0f;

    for (int i = 0; i < TWINMOLD_EFFECT_COUNT; i++) {
        sTwinmoldEffects[i].type = TWINMOLD_EFFECT_NONE;
        sTwinmoldEffects[i].epoch = 0;
    }
}
