#ifndef Z64INTERFACE_H
#define Z64INTERFACE_H

#include "ultra64.h"
#include "z64view.h"

// TODO extract this information from the texture definitions themselves
// 8x8 IA8
#define AMMO_DIGIT_TEX_WIDTH  8
#define AMMO_DIGIT_TEX_HEIGHT 8
#define AMMO_DIGIT_TEX_SIZE (AMMO_DIGIT_TEX_WIDTH * AMMO_DIGIT_TEX_HEIGHT)

// TODO extract this information from the texture definitions themselves
// 32x32 RGBA32
#define ICON_ITEM_TEX_WIDTH  32
#define ICON_ITEM_TEX_HEIGHT 32
#define ICON_ITEM_TEX_SIZE ((ICON_ITEM_TEX_WIDTH * ICON_ITEM_TEX_HEIGHT) * 4)

// TODO extract this information from the texture definitions themselves
// 48x16 I4
#define DO_ACTION_TEX_WIDTH  48
#define DO_ACTION_TEX_HEIGHT 16
#define DO_ACTION_TEX_SIZE ((DO_ACTION_TEX_WIDTH * DO_ACTION_TEX_HEIGHT) / 2)

// This do action label slot contains the current do action text
#define DO_ACTION_A_SLOT_ACTIVE 0
// This do action label slot contains the next do action text for the duration of the A button switching animation
#define DO_ACTION_A_SLOT_NEXT   1

// Do action texture offset for the A button's active do action text
#define DO_ACTION_OFFSET_A_ACTIVE       (0 * DO_ACTION_TEX_SIZE)
// Do action texture offset for the A button's next do action text
#define DO_ACTION_OFFSET_A_NEXT         (1 * DO_ACTION_TEX_SIZE)
// Do action texture offset for the start button in the pause menu
#define DO_ACTION_OFFSET_START          (2 * DO_ACTION_TEX_SIZE)
// Do action texture offset for the B button for player actions (masks, transformations)
#define DO_ACTION_OFFSET_B_PLAYER       (3 * DO_ACTION_TEX_SIZE)
// Do action texture offset for the B button for the interface (pause menu, pictograph box, ocarina)
#define DO_ACTION_OFFSET_B_INTERFACE    (4 * DO_ACTION_TEX_SIZE)
// Following the do action textures is extra space for the day number used by the 3 day clock
#define DO_ACTION_OFFSET_DAY_NUMBER     (5 * DO_ACTION_TEX_SIZE)

// TODO extract this information from the texture definitions themselves
// 48x26 IA8
#define WEEK_STATIC_TEX_WIDTH 48
#define WEEK_STATIC_TEX_HEIGHT 27
#define WEEK_STATIC_TEX_SIZE ((WEEK_STATIC_TEX_WIDTH * WEEK_STATIC_TEX_HEIGHT))

typedef enum {
    /* -1 */ EQUIP_SLOT_NONE = -1,
    /*  0 */ EQUIP_SLOT_B,
    /*  1 */ EQUIP_SLOT_C_LEFT,
    /*  2 */ EQUIP_SLOT_C_DOWN,
    /*  3 */ EQUIP_SLOT_C_RIGHT,
    /*  4 */ EQUIP_SLOT_A,
    /*  5 */ EQUIP_SLOT_MAX
} EquipSlot;

// #region 2S2H
typedef enum {
    /* -1 */ EQUIP_SLOT_D_NONE = -1,
    /*  0 */ EQUIP_SLOT_D_RIGHT,
    /*  1 */ EQUIP_SLOT_D_LEFT,
    /*  2 */ EQUIP_SLOT_D_DOWN,
    /*  3 */ EQUIP_SLOT_D_UP,
    /*  4 */ EQUIP_SLOT_D_MAX
} DpadEquipSlot;
// #endregion

typedef enum {
    /* 0 */ A_BTN_STATE_IDLE,               // Uses ACTIVE slot
    /* 1 */ A_BTN_STATE_CHANGE_1_UNPAUSED,  // Uses ACTIVE slot
    /* 2 */ A_BTN_STATE_CHANGE_2_UNPAUSED,  // Uses NEXT slot
    /* 3 */ A_BTN_STATE_CHANGE_1_PAUSED,    // Uses ACTIVE slot
    /* 4 */ A_BTN_STATE_CHANGE_2_PAUSED     // Uses NEXT slot
} AButtonState;

typedef enum {
    /*   -1 */ DO_ACTION_UNDEFINED = -1, // 2S2H [Port] DO_ACTION_UNDEFINED added so comparisons between the enum type and -1 would work on all systems
    /* 0x00 */ DO_ACTION_ATTACK = 0,
    /* 0x01 */ DO_ACTION_CHECK,
    /* 0x02 */ DO_ACTION_ENTER,
    /* 0x03 */ DO_ACTION_RETURN,
    /* 0x04 */ DO_ACTION_OPEN,
    /* 0x05 */ DO_ACTION_JUMP,
    /* 0x06 */ DO_ACTION_DECIDE,
    /* 0x07 */ DO_ACTION_DIVE,
    /* 0x08 */ DO_ACTION_FASTER,
    /* 0x09 */ DO_ACTION_THROW,
    /* 0x0A */ DO_ACTION_NONE,  // in do_action_static, the texture at this position is NAVI, however this value is in practice the "No Action" value
    /* 0x0B */ DO_ACTION_CLIMB,
    /* 0x0C */ DO_ACTION_DROP,
    /* 0x0D */ DO_ACTION_DOWN,
    /* 0x0E */ DO_ACTION_QUIT,
    /* 0x0F */ DO_ACTION_SPEAK,
    /* 0x10 */ DO_ACTION_NEXT,
    /* 0x11 */ DO_ACTION_GRAB,
    /* 0x12 */ DO_ACTION_STOP,
    /* 0x13 */ DO_ACTION_PUTAWAY,
    /* 0x14 */ DO_ACTION_REEL,
    /* 0x15 */ DO_ACTION_INFO,
    /* 0x16 */ DO_ACTION_WARP,
    /* 0x17 */ DO_ACTION_SNAP,
    /* 0x18 */ DO_ACTION_EXPLODE,
    /* 0x19 */ DO_ACTION_DANCE,
    /* 0x1A */ DO_ACTION_MARCH,
    /* 0x1B */ DO_ACTION_1,
    /* 0x1C */ DO_ACTION_2,
    /* 0x1D */ DO_ACTION_3,
    /* 0x1E */ DO_ACTION_4,
    /* 0x1F */ DO_ACTION_5,
    /* 0x20 */ DO_ACTION_6,
    /* 0x21 */ DO_ACTION_7,
    /* 0x22 */ DO_ACTION_8,
    /* 0x23 */ DO_ACTION_CURL,
    /* 0x24 */ DO_ACTION_SURFACE,
    /* 0x25 */ DO_ACTION_SWIM,
    /* 0x26 */ DO_ACTION_PUNCH,
    /* 0x27 */ DO_ACTION_POUND,
    /* 0x28 */ DO_ACTION_HOOK,
    /* 0x29 */ DO_ACTION_SHOOT,
    /* 0x2A */ DO_ACTION_MAX,
    /* 0x2A */ TATL_STATE_2A = DO_ACTION_MAX,
    /* 0x2B */ TATL_STATE_2B,
    /* 0x2C */ TATL_STATE_2C
} DoAction;

typedef enum {
    /*  0 */ MINIGAME_STATE_NONE,
    /*  1 */ MINIGAME_STATE_COUNTDOWN_SETUP_3,
    /*  2 */ MINIGAME_STATE_COUNTDOWN_3,
    /*  3 */ MINIGAME_STATE_COUNTDOWN_SETUP_2,
    /*  4 */ MINIGAME_STATE_COUNTDOWN_2,
    /*  5 */ MINIGAME_STATE_COUNTDOWN_SETUP_1,
    /*  6 */ MINIGAME_STATE_COUNTDOWN_1,
    /*  7 */ MINIGAME_STATE_COUNTDOWN_SETUP_GO,
    /*  8 */ MINIGAME_STATE_COUNTDOWN_GO,
    /* 20 */ MINIGAME_STATE_NO_COUNTDOWN_SETUP = 20,
    /* 21 */ MINIGAME_STATE_NO_COUNTDOWN,
    /* 30 */ MINIGAME_STATE_PLAYING = 30
} MinigameState;

typedef enum {
    /* 0 */ PERFECT_LETTERS_TYPE_OFF,
    /* 1 */ PERFECT_LETTERS_TYPE_1,
    /* 2 */ PERFECT_LETTERS_TYPE_2,
    /* 3 */ PERFECT_LETTERS_TYPE_3
} PerfectLettersType;

typedef enum {
    /* 0 */ PERFECT_LETTERS_STATE_OFF,
    /* 1 */ PERFECT_LETTERS_STATE_INIT,
    /* 2 */ PERFECT_LETTERS_STATE_ENTER,
    /* 3 */ PERFECT_LETTERS_STATE_STATIONARY, // Display for type 1
    /* 4 */ PERFECT_LETTERS_STATE_SPREAD, // Exit for type 1
    /* 5 */ PERFECT_LETTERS_STATE_DISPLAY,
    /* 6 */ PERFECT_LETTERS_STATE_EXIT
} PerfectLettersState;

#define PERFECT_LETTERS_NUM_LETTERS 8
#define PERFECT_LETTERS_ANGLE_PER_LETTER (0x10000 / PERFECT_LETTERS_NUM_LETTERS)

typedef enum {
    /* 0 */ STORY_DMA_IDLE,
    /* 1 */ STORY_DMA_LOADING,
    /* 2 */ STORY_DMA_DONE
} StoryLoadStatus;

typedef enum {
    /* 0 */ STORY_STATE_OFF,
    /* 1 */ STORY_STATE_INIT,
    /* 2 */ STORY_STATE_IDLE,
    /* 3 */ STORY_STATE_DESTROY,
    /* 4 */ STORY_STATE_SETUP_IDLE,
    /* 5 */ STORY_STATE_FADE_OUT,
    /* 6 */ STORY_STATE_FADE_IN
} StoryState;

typedef enum {
    /* 0 */ STORY_TYPE_MASK_FESTIVAL,
    /* 1 */ STORY_TYPE_GIANTS_LEAVING
} StoryType;

typedef enum {
    /* 0 */ DO_ACTION_SEG_A,
    /* 1 */ DO_ACTION_SEG_B,
    /* 2 */ DO_ACTION_SEG_START,
    /* 3 */ DO_ACTION_SEG_CLOCK,
    /* 4 */ DO_ACTION_SEG_MAX
} DoActionSegmentIndex;

typedef struct {
    char* mainTex;
    char* subTex;
} ActionLabel;

// #region 2S2H
typedef struct {
    s16 dRightAlpha;
    s16 dLeftAlpha;
    s16 dDownAlpha;
    s16 dUpAlpha;
} DpadInterface;

typedef struct {
    DpadInterface dpad;
} ShipInterfaceContext;
// #endregion

typedef struct {
    /* 0x000 */ View view;
    /* 0x168 */ Vtx* actionVtx;
    /* 0x16C */ Vtx* beatingHeartVtx;
    /* 0x170 */ u8* parameterSegment;
    /* 0x174 */ ActionLabel* doActionSegment;
    /* 0x178 */ char** iconItemSegment;
    /* 0x17C */ u8* mapSegment;
    /* 0x180 */ UNK_TYPE4 unk_180; // unused segment?
    /* 0x184 */ DmaRequest dmaRequest;
    /* 0x1A4 */ UNK_TYPE1 unk_1A4[0x40]; // likely 2 DmaRequest
    /* 0x1E4 */ OSMesgQueue loadQueue;
    /* 0x1FC */ OSMesg loadMsg;
    /* 0x200 */ Viewport viewport;
    /* 0x210 */ s16 aButtonState;
    /* 0x212 */ u16 aButtonDoActionDelayed; // Like aButtonDoAction, but does not update until the A button animation is complete
    /* 0x214 */ u16 aButtonDoAction;
    /* 0x218 */ f32 aButtonRoll;
    /* 0x21C */ s16 bButtonPlayerDoActionActive;
    /* 0x21E */ s16 bButtonPlayerDoAction;
    /* 0x220 */ s16 tatlCalling;
    /* 0x222 */ s16 bButtonInterfaceDoActionActive;
    /* 0x224 */ s16 bButtonInterfaceDoAction;
    /* 0x226 */ s16 lifeColorChange;
    /* 0x228 */ s16 lifeColorChangeDirection;
    /* 0x22A */ s16 beatingHeartPrim[3];
    /* 0x230 */ s16 beatingHeartEnv[3];
    /* 0x236 */ s16 heartsPrimR[2];
    /* 0x23A */ s16 heartsPrimG[2];
    /* 0x23E */ s16 heartsPrimB[2];
    /* 0x242 */ s16 heartsEnvR[2];
    /* 0x246 */ s16 heartsEnvG[2];
    /* 0x24A */ s16 heartsEnvB[2];
    /* 0x24E */ s16 health;
    /* 0x250 */ s16 healthTimer;
    /* 0x252 */ s16 lifeSizeChange;
    /* 0x254 */ s16 lifeSizeChangeDirection; // 1 means shrinking, 0 growing
    /* 0x256 */ UNK_TYPE2 unk_256;
    /* 0x258 */ s16 magicConsumptionTimer; // For certain magic states, 1 unit of magic is consumed every time the timer reaches 0
    /* 0x25A */ u8 numHorseBoosts;
    /* 0x25C */ u16 minigamePoints; // Points to add to the minigame score. Reset to 0 every frame.
    /* 0x25E */ u16 minigameHiddenPoints; // Points to add to the secondary set of minigame points not displayed. Reset to 0 every frame.
    /* 0x260 */ u16 minigameAmmo;
    /* 0x262 */ u16 minigameUnusedPoints; // Associated with other minigame points, unused
    /* 0x264 */ s16 screenFillAlpha;
    /* 0x266 */ s16 aAlpha;
    /* 0x268 */ s16 bAlpha;
    /* 0x26A */ s16 cLeftAlpha;
    /* 0x26C */ s16 cDownAlpha;
    /* 0x26E */ s16 cRightAlpha;
    /* 0x270 */ s16 healthAlpha;
    /* 0x272 */ s16 magicAlpha;
    /* 0x274 */ s16 minimapAlpha;
    /* 0x276 */ s16 startAlpha;
    /* 0x278 */ s16 unk_278; // Set in Map_Init, never read
    /* 0x27A */ s16 dungeonSceneIndex;
    /* 0x27C */ s16 mapRoomNum;
    /* 0x27E */ u8 fishingHUDControl; // Left over from OoT
    /* 0x27F */ UNK_TYPE1 unk_27F;
    /* 0x280 */ u8 minigameState;
    /* 0x282 */ s16 minigameCountdownAlpha;
    /* 0x284 */ s16 minigameCountdownScale;
    /* 0x286 */ s16 perfectLettersOn;
    /* 0x288 */ s16 perfectLettersType;
    /* 0x28A */ s16 perfectLettersState[PERFECT_LETTERS_NUM_LETTERS];
    /* 0x29A */ u16 perfectLettersAngles[PERFECT_LETTERS_NUM_LETTERS]; // Angle that follows the projectory of an ellipse
    /* 0x2AA */ s16 perfectLettersOffsetX[PERFECT_LETTERS_NUM_LETTERS];
    /* 0x2BC */ f32 perfectLettersSemiAxisX[PERFECT_LETTERS_NUM_LETTERS];
    /* 0x2DC */ f32 perfectLettersSemiAxisY[PERFECT_LETTERS_NUM_LETTERS];
    /* 0x2FC */ s16 perfectLettersPrimColor[4];
    /* 0x304 */ s16 perfectLettersCount;
    /* 0x306 */ s16 perfectLettersUnused;
    /* 0x308 */ s16 perfectLettersColorIndex;
    /* 0x30A */ s16 perfectLettersColorTimer;
    /* 0x30C */ s16 perfectLettersTimer;
    struct {
        /* 0x30E */ u8 hGauge;
        /* 0x30F */ u8 bButton;
        /* 0x310 */ u8 aButton;
        /* 0x311 */ u8 tradeItems; // Includes deed trade quest, anju/kafei trade quest, bottles, and ocarina
        /* 0x312 */ u8 songOfTime;
        /* 0x313 */ u8 songOfDoubleTime;
        /* 0x314 */ u8 invSongOfTime;
        /* 0x315 */ u8 songOfSoaring;
        /* 0x316 */ u8 songOfStorms;
        /* 0x317 */ u8 masks;
        /* 0x318 */ u8 pictoBox;
        /* 0x319 */ u8 all;     // "another"; enables all item restrictions
    } restrictions; // size = 0xC
    /* 0x31A */ u8 storyState;
    /* 0x31B */ u8 storyType;
    /* 0x31C */ u8 storyDmaStatus;
    /* 0x320 */ OSMesgQueue storyMsgQueue;
    /* 0x338 */ OSMesg storyMsgBuf;
    /* 0x33C */ u8* storySegment;
    /* 0x340 */ uintptr_t storyAddr;
    /* 0x344 */ size_t storySize;
    /* 2S2H  */ ShipInterfaceContext shipInterface;
} InterfaceContext; // size = 0x348

Gfx* Gfx_DrawTexRectRGBA16(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy);
Gfx* Gfx_DrawTexRectIA8(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy);
Gfx* Gfx_DrawTexRectI8(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop, s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy);
Gfx* Gfx_DrawTexQuadIA8(Gfx* gfx, TexturePtr texture, s16 textureWidth, s16 textureHeight, u16 point);
Gfx* Gfx_DrawTexQuad4b(Gfx* gfx, TexturePtr texture, s32 fmt, s16 textureWidth, s16 textureHeight, u16 point);

void Interface_StartTimer(s16 timerId, s16 seconds);
void Interface_StartPostmanTimer(s16 seconds, s16 bunnyHoodState);
void Interface_NewDay(struct PlayState* play, s32 day);
void Interface_SetHudVisibility(u16 hudVisibility);
void Interface_UpdateButtonsPart2(struct PlayState* play);
void Interface_SetSceneRestrictions(struct PlayState* play);
void Interface_InitMinigame(struct PlayState* play);
// #region 2S2H [Dpad]
void Interface_Dpad_LoadItemIconImpl(struct PlayState* play, u8 btn);
void Interface_Dpad_LoadItemIcon(struct PlayState* play, u8 btn);
// #endregion
void Interface_LoadItemIconImpl(struct PlayState* play, u8 btn);
void Interface_LoadItemIcon(struct PlayState* play, u8 btn);
void Interface_UpdateButtonsAlt(struct PlayState* play, u16 flag);
u8 Item_Give(struct PlayState* play, u8 item);
u8 Item_CheckObtainability(u8 item);
void Inventory_DeleteItem(s16 item, s16 slot);
void Inventory_UnequipItem(s16 item);
s32 Inventory_ReplaceItem(struct PlayState* play, u8 oldItem, u8 newItem);
void Inventory_UpdateDeitySwordEquip(struct PlayState* play) ;
s32 Inventory_HasEmptyBottle(void);
s32 Inventory_HasItemInBottle(u8 item);
// #region 2S2H - Enhancements
// [Dpad]
void Inventory_Dpad_UpdateBottleItem(struct PlayState* play, u8 item, u8 btn);
// [Autosave]
void Interface_DrawAutosaveIcon(struct PlayState* play, uint16_t opacity);
// #endregion
void Inventory_UpdateBottleItem(struct PlayState* play, u8 item, u8 btn);
s32 Inventory_ConsumeFairy(struct PlayState* play);
void Inventory_UpdateItem(struct PlayState* play, s16 slot, s16 item);
void Interface_SetAButtonDoAction(struct PlayState* play, u16 aButtonDoAction);
void Interface_SetBButtonPlayerDoAction(struct PlayState* play, s16 bButtonDoAction);
void Interface_SetTatlCall(struct PlayState* play, u16 tatlCallState);
void Interface_SetBButtonInterfaceDoAction(struct PlayState* play, s16 bButtonDoAction);
s32 Health_ChangeBy(struct PlayState* play, s16 healthChange);
void Health_GiveHearts(s16 hearts);
void Rupees_ChangeBy(s16 rupeeChange);
void Inventory_ChangeAmmo(s16 item, s16 ammoChange);
void Magic_Add(struct PlayState* play, s16 magicToAdd);
void Magic_Reset(struct PlayState* play);
s32 Magic_Consume(struct PlayState* play, s16 magicToConsume, s16 type);
void Interface_SetPerfectLetters(struct PlayState* play, s16 perfectLettersType);
void Interface_StartMoonCrash(struct PlayState* play);
void Interface_Draw(struct PlayState* play);
void Interface_Update(struct PlayState* play);
void Interface_Destroy(struct PlayState* play);
void Interface_Init(struct PlayState* play);

#endif
