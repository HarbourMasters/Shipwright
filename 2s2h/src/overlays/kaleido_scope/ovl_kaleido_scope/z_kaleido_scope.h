#ifndef Z_KALEIDO_SCOPE_H
#define Z_KALEIDO_SCOPE_H

#include "global.h"
#include "z64pause_menu.h"

#define PAUSE_ITEM_NONE 999

#define PAUSE_CURSOR_PAGE_LEFT 10
#define PAUSE_CURSOR_PAGE_RIGHT 11

// Direction of pause pages
#define PAUSE_ITEM_X (0)
#define PAUSE_ITEM_Z (-1)
#define PAUSE_MAP_X (1)
#define PAUSE_MAP_Z (0)
#define PAUSE_QUEST_X (0)
#define PAUSE_QUEST_Z (1)
#define PAUSE_MASK_X (-1)
#define PAUSE_MASK_Z (0)

#define PAUSE_EYE_DIST (64.0f)

typedef enum PauseEquipCButton {
    /* 0 */ PAUSE_EQUIP_C_LEFT,
    /* 1 */ PAUSE_EQUIP_C_DOWN,
    /* 2 */ PAUSE_EQUIP_C_RIGHT
} PauseEquipCButton;

// #region 2S2H [Dpad]
typedef enum {
    /* 3 */ PAUSE_EQUIP_D_RIGHT = 3,
    /* 4 */ PAUSE_EQUIP_D_LEFT,
    /* 5 */ PAUSE_EQUIP_D_DOWN,
    /* 6 */ PAUSE_EQUIP_D_UP
} PauseEquipDButton;
// #endregion

typedef enum EquipState {
    /* 0 */ EQUIP_STATE_MAGIC_ARROW_GROW_ORB,
    /* 1 */ EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT,
    /* 2 */ EQUIP_STATE_MAGIC_ARROW_HOVER_OVER_BOW_SLOT,
    /* 3 */ EQUIP_STATE_MOVE_TO_C_BTN
} EquipState;

typedef enum VtxPage {
    /* 0 */ VTX_PAGE_MASK,
    /* 1 */ VTX_PAGE_ITEM,
    /* 2 */ VTX_PAGE_MAP_DUNGEON,
    /* 3 */ VTX_PAGE_QUEST,
    /* 4 */ VTX_PAGE_MAP_WORLD,
    /* 5 */ VTX_PAGE_PROMPT,
    /* 6 */ VTX_PAGE_MAX
} VtxPage;

#define VTX_PAGE_MASK_QUADS 0        // VTX_PAGE_MASK
#define VTX_PAGE_ITEM_QUADS 0        // VTX_PAGE_ITEM
#define VTX_PAGE_MAP_DUNGEON_QUADS 6 // VTX_PAGE_MAP_DUNGEON
#define VTX_PAGE_QUEST_QUADS 0       // VTX_PAGE_QUEST
#define VTX_PAGE_MAP_WORLD_QUADS 36  // VTX_PAGE_MAP_WORLD
#define VTX_PAGE_SAVE_QUADS 5        // VTX_PAGE_PROMPT

#define PAGE_BG_COLS 3
#define PAGE_BG_ROWS 5
#define PAGE_BG_QUADS (PAGE_BG_COLS * PAGE_BG_ROWS)
#define PAGE_BG_QUAD_WIDTH 80
#define PAGE_BG_QUAD_HEIGHT 32
#define PAGE_BG_QUAD_TEX_WIDTH 80
#define PAGE_BG_QUAD_TEX_HEIGHT 32

// === ITEM === //

#define ITEM_GRID_CELL_WIDTH 32
#define ITEM_GRID_CELL_HEIGHT 32
#define ITEM_GRID_QUAD_MARGIN 2
#define ITEM_GRID_QUAD_WIDTH (ITEM_GRID_CELL_WIDTH - (2 * ITEM_GRID_QUAD_MARGIN))
#define ITEM_GRID_QUAD_HEIGHT (ITEM_GRID_CELL_HEIGHT - (2 * ITEM_GRID_QUAD_MARGIN))
#define ITEM_GRID_QUAD_TEX_SIZE 32 // both width and height
#define ITEM_GRID_QUAD_ENLARGE_OFFSET 2

#define ITEM_GRID_SELECTED_QUAD_MARGIN (-2)
#define ITEM_GRID_SELECTED_QUAD_WIDTH (ITEM_GRID_QUAD_WIDTH - (2 * ITEM_GRID_SELECTED_QUAD_MARGIN))
#define ITEM_GRID_SELECTED_QUAD_HEIGHT (ITEM_GRID_QUAD_HEIGHT - (2 * ITEM_GRID_SELECTED_QUAD_MARGIN))
#define ITEM_GRID_SELECTED_QUAD_TEX_SIZE 32 // both width and height

typedef enum ItemPageQuad {
    // 0..59 are the 15 background textures
    /*  0 */ QUAD_ITEM_PAGE_BG_FIRST,
    /* 14 */ QUAD_ITEM_PAGE_BG_LAST = PAGE_BG_QUADS - 1,
    // VTX_PAGE_ITEM_QUADS is 0
    /* 15 */ QUAD_ITEM_PAGE_MAX
} ItemPageQuad;

typedef enum ItemQuad {
    // 0..23 are the ITEM_GRID_ROWS*ITEM_GRID_COLS item grid
    // The values follow the `InventorySlot` enum
    /*  0 */ QUAD_ITEM_GRID_FIRST,
    /* 23 */ QUAD_ITEM_GRID_LAST = ITEM_GRID_ROWS * ITEM_GRID_COLS - 1,
    // White-box Markers indicating the currently equipped items
    /* 24 */ QUAD_ITEM_GRID_SELECTED_C_LEFT,
    /* 25 */ QUAD_ITEM_GRID_SELECTED_C_DOWN,
    /* 26 */ QUAD_ITEM_GRID_SELECTED_C_RIGHT,
    /* 27 */ QUAD_ITEM_MAX
} ItemQuad;

// === MASK === //

#define MASK_GRID_CELL_WIDTH 32
#define MASK_GRID_CELL_HEIGHT 32
#define MASK_GRID_QUAD_MARGIN 2
#define MASK_GRID_QUAD_WIDTH (MASK_GRID_CELL_WIDTH - (2 * MASK_GRID_QUAD_MARGIN))
#define MASK_GRID_QUAD_HEIGHT (MASK_GRID_CELL_HEIGHT - (2 * MASK_GRID_QUAD_MARGIN))
#define MASK_GRID_QUAD_TEX_SIZE 32 // both width and height
#define MASK_GRID_QUAD_ENLARGE_OFFSET 2

#define MASK_GRID_SELECTED_QUAD_MARGIN (-2)
#define MASK_GRID_SELECTED_QUAD_WIDTH (MASK_GRID_QUAD_WIDTH - (2 * MASK_GRID_SELECTED_QUAD_MARGIN))
#define MASK_GRID_SELECTED_QUAD_HEIGHT (MASK_GRID_QUAD_HEIGHT - (2 * MASK_GRID_SELECTED_QUAD_MARGIN))
#define MASK_GRID_SELECTED_QUAD_TEX_SIZE 32 // both width and height


typedef enum MaskPageQuad {
    // 0..59 are the 15 background textures
    /*  0 */ QUAD_MASK_PAGE_BG_FIRST,
    /* 14 */ QUAD_MASK_PAGE_BG_LAST = PAGE_BG_QUADS - 1,
    // VTX_PAGE_MASK_QUADS is 0
    /* 15 */ QUAD_MASK_PAGE_MAX
} MaskPageQuad;

typedef enum MaskQuad {
    // 0..23 are the MASK_GRID_ROWS*MASK_GRID_COLS item grid
    // The values follow the `InventorySlot` enum offset by ITEM_NUM_SLOTS
    /*  0 */ QUAD_MASK_GRID_FIRST,
    /* 23 */ QUAD_MASK_GRID_LAST = MASK_GRID_ROWS * MASK_GRID_COLS - 1,
    // Markers indicating the currently equipped items
    /* 24 */ QUAD_MASK_GRID_SELECTED_C_LEFT,
    /* 25 */ QUAD_MASK_GRID_SELECTED_C_DOWN,
    /* 26 */ QUAD_MASK_GRID_SELECTED_C_RIGHT,
    /* 27 */ QUAD_MASK_MAX
} MaskQuad;

// === QUEST === //

typedef enum QuestPageQuad {
    // 0..59 are the 15 background textures
    /*  0 */ QUAD_QUEST_PAGE_BG_FIRST,
    /* 14 */ QUAD_QUEST_PAGE_BG_LAST = PAGE_BG_QUADS - 1,
    // VTX_PAGE_QUEST_QUADS is 0
    /* 15 */ QUAD_QUEST_PAGE_MAX
} QuestPageQuad;

typedef enum QuestQuad {
    // 0 to 24 matches the `QuestItem` enum
    // Notes showing the correct song
    /* 25 */ QUAD_QUEST_SONG_NOTE_A1 = QUEST_HEART_PIECE + 1,
    /* 26 */ QUAD_QUEST_SONG_NOTE_A2,
    /* 27 */ QUAD_QUEST_SONG_NOTE_A3,
    /* 28 */ QUAD_QUEST_SONG_NOTE_A4,
    /* 29 */ QUAD_QUEST_SONG_NOTE_A5,
    /* 30 */ QUAD_QUEST_SONG_NOTE_A6,
    /* 31 */ QUAD_QUEST_SONG_NOTE_A7,
    /* 32 */ QUAD_QUEST_SONG_NOTE_A8,
    // Notes appearing when playing
    /* 33 */ QUAD_QUEST_SONG_NOTE_B1,
    /* 34 */ QUAD_QUEST_SONG_NOTE_B2,
    /* 35 */ QUAD_QUEST_SONG_NOTE_B3,
    /* 36 */ QUAD_QUEST_SONG_NOTE_B4,
    /* 37 */ QUAD_QUEST_SONG_NOTE_B5,
    /* 38 */ QUAD_QUEST_SONG_NOTE_B6,
    /* 39 */ QUAD_QUEST_SONG_NOTE_B7,
    /* 40 */ QUAD_QUEST_SONG_NOTE_B8,
    /* 41 */ QUAD_QUEST_MAX
} QuestQuad;

// === DUNGEON MAP === //

typedef enum DungeonMapPageQuad {
    // 0..59 are the 15 background textures
    /*  0 */ QUAD_MAP_PAGE_DUNGEON_BG_FIRST,
    /* 14 */ QUAD_MAP_PAGE_DUNGEON_BG_LAST = PAGE_BG_QUADS - 1,
    /* 15 */ QUAD_MAP_PAGE_DUNGEON_TITLE,
    /* 16 */ QUAD_MAP_PAGE_DUNGEON_BOSS_KEY,
    /* 17 */ QUAD_MAP_PAGE_DUNGEON_COMPASS,
    /* 18 */ QUAD_MAP_PAGE_DUNGEON_MAP,
    /* 19 */ QUAD_MAP_PAGE_DUNGEON_STRAY_FAIRY_GLOWING_CIRCLE,
    /* 20 */ QUAD_MAP_PAGE_DUNGEON_UNUSED,
    /* 21 */ QUAD_MAP_PAGE_DUNGEON_MAX
} DungeonMapPageQuad;

// === WORLD MAP === //

#define WORLD_MAP_IMAGE_WIDTH 216
#define WORLD_MAP_IMAGE_HEIGHT 128
#define WORLD_MAP_IMAGE_FRAG_HEIGHT ((TMEM_SIZE / 2) / (WORLD_MAP_IMAGE_WIDTH * G_IM_SIZ_8b_BYTES))
#define WORLD_MAP_IMAGE_FRAG_NUM (((WORLD_MAP_IMAGE_HEIGHT - 1) / WORLD_MAP_IMAGE_FRAG_HEIGHT) + 1)
#define WORLD_MAP_NUM_CLOUDS 15

typedef enum WorldMapPageQuad {
    // 0..59 are the 15 background textures
    /*  0 */ QUAD_MAP_PAGE_WORLD_BG_FIRST,
    /* 14 */ QUAD_MAP_PAGE_WORLD_BG_LAST = PAGE_BG_QUADS - 1,
    // Clouds covering the world map
    /* 15 */ QUAD_MAP_PAGE_WORLD_CLOUDS_FIRST,
    /* 29 */ QUAD_MAP_PAGE_WORLD_CLOUDS_LAST = QUAD_MAP_PAGE_WORLD_CLOUDS_FIRST + WORLD_MAP_NUM_CLOUDS - 1,
    // Markers for regions
    /* 30 */ QUAD_MAP_PAGE_WORLD_REGION_FIRST,
    /* 40 */ QUAD_MAP_PAGE_WORLD_REGION_LAST = QUAD_MAP_PAGE_WORLD_REGION_FIRST + REGION_MAX - 1,
    // Markers for owl warps
    /* 41 */ QUAD_MAP_PAGE_WORLD_WARP_FIRST,
    /* 50 */ QUAD_MAP_PAGE_WORLD_WARP_LAST = QUAD_MAP_PAGE_WORLD_WARP_FIRST + OWL_WARP_ENTRANCE - 1,
    // Fragments of the world map
    /* 51 */ QUAD_MAP_PAGE_WORLD_IMAGE_FIRST,
    /* 65 */ QUAD_MAP_PAGE_WORLD_IMAGE_LAST = QUAD_MAP_PAGE_WORLD_IMAGE_FIRST + WORLD_MAP_IMAGE_FRAG_NUM - 1,
    /* 66 */ QUAD_MAP_PAGE_WORLD_MAX
} WorldMapPageQuad;

// === INFO PANEL === //

/**
 * infoPanelVtx
 * 
 * infoPanelVtx[0] name panel left texture
 * infoPanelVtx[4] name panel right texture
 * infoPanelVtx[8] Z Button icon
 * infoPanelVtx[12] R Button icon
 * infoPanelVtx[16] A button icon (or name segment)
 * infoPanelVtx[20] pause-to-decide texture
 * infoPanelVtx[24] (unused) oot remnant of Gold Skulltula Icon Texture
 */

#define PAUSE_NAME_COLOR_SET_WHITE 0
#define PAUSE_NAME_COLOR_SET_GREY 1

// === CURSOR === //

/**
 * cursorVtx
 * 
 * cursorVtx[0] cursor circle
 * cursorVtx[4] (unused) oot remnant of top-right corner of cursor
 * cursorVtx[8] (unused) oot remnant of bottom-left corner of cursor
 * cursorVtx[12] (unused) oot remnant of bottom-right corner of cursor
 * cursorVtx[16] equipping item icon (while moving)
 */

#define PAUSE_CURSOR_COLOR_SET_WHITE 0
#define PAUSE_CURSOR_COLOR_SET_YELLOW 2
#define PAUSE_CURSOR_COLOR_SET_BLUE 4

// To be used for Item-Page cursor and Mask-Page cursor
typedef enum PauseMoveCursorResult {
    /* 0 */ PAUSE_CURSOR_RESULT_NONE,
    /* 1 */ PAUSE_CURSOR_RESULT_SLOT,
    /* 2 */ PAUSE_CURSOR_RESULT_SPECIAL_POS
} PauseMoveCursorResult;

typedef enum PauseCursorQuad {
    /* 0 */ PAUSE_QUAD_CURSOR_0,
    /* 1 */ PAUSE_QUAD_CURSOR_1,
    /* 2 */ PAUSE_QUAD_CURSOR_2,
    /* 3 */ PAUSE_QUAD_CURSOR_3,
    /* 4 */ PAUSE_QUAD_CURSOR_4,
    /* 5 */ PAUSE_QUAD_CURSOR_MAX
} PauseCursorQuad;

// === PROMPT === //

/**
 * promptPageVtx (unused in MM, inferred from OoT)
 * 
 * // PAGE_BG_QUADS
 * promptPageVtx[0] prompt page background texture:  row 0, column 0
 * promptPageVtx[4] prompt page background texture:  row 1, column 0
 * promptPageVtx[8] prompt page background texture:  row 2, column 0
 * promptPageVtx[12] prompt page background texture: row 3, column 0
 * promptPageVtx[16] prompt page background texture: row 4, column 0
 * promptPageVtx[20] prompt page background texture: row 0, column 1
 * promptPageVtx[24] prompt page background texture: row 1, column 1
 * promptPageVtx[28] prompt page background texture: row 2, column 1
 * promptPageVtx[32] prompt page background texture: row 3, column 1
 * promptPageVtx[36] prompt page background texture: row 4, column 1
 * promptPageVtx[40] prompt page background texture: row 0, column 2
 * promptPageVtx[44] prompt page background texture: row 1, column 2
 * promptPageVtx[48] prompt page background texture: row 2, column 2
 * promptPageVtx[52] prompt page background texture: row 3, column 2
 * promptPageVtx[56] prompt page background texture: row 4, column 2
 * 
 * // QUAD_PROMPT_MAX
 * promptPageVtx[60] message
 * promptPageVtx[64] cursor left
 * promptPageVtx[68] cursor right
 * promptPageVtx[72] choice (yes)
 * promptPageVtx[76] choice (no)
 */

#define PAUSE_PROMPT_YES 0
#define PAUSE_PROMPT_NO 4

typedef enum PromptQuad {
    /* 0 */ QUAD_PROMPT_MESSAGE,
    /* 1 */ QUAD_PROMPT_CURSOR_LEFT,
    /* 2 */ QUAD_PROMPT_CURSOR_RIGHT,
    /* 3 */ QUAD_PROMPT_CHOICE_YES,
    /* 4 */ QUAD_PROMPT_CHOICE_NO,
    /* 5 */ QUAD_PROMPT_MAX
} PromptQuad;

// NES
void KaleidoScope_MoveCursorToSpecialPos(PlayState* play, s16 cursorSpecialPos);
void KaleidoScope_DrawTexQuadRGBA32(GraphicsContext* gfxCtx, TexturePtr texture, u16 width, u16 height, u16 point);
void KaleidoScope_SetView(PauseContext* pauseCtx, f32 eyeX, f32 eyeY, f32 eyeZ);
void KaleidoScope_MoveCursorFromSpecialPos(PlayState* play);

// Map
void KaleidoScope_DrawDungeonMap(PlayState* play);
void KaleidoScope_UpdateDungeonCursor(PlayState* play);
void KaleidoScope_DrawWorldMap(PlayState* play);
void KaleidoScope_UpdateWorldMapCursor(PlayState* play);

// Collect
void KaleidoScope_DrawQuestStatus(PlayState* play);
void KaleidoScope_UpdateQuestCursor(PlayState* play);

// Item
void KaleidoScope_SetCursorVtxPos(PauseContext* pauseCtx, u16 vtxIndex, Vtx* vtx);
void KaleidoScope_DrawItemSelect(PlayState* play);
void KaleidoScope_UpdateItemCursor(PlayState* play);
void KaleidoScope_UpdateItemEquip(PlayState* play);

// Mask
void KaleidoScope_DrawMaskSelect(PlayState* play);
void KaleidoScope_UpdateMaskCursor(PlayState* play);
void KaleidoScope_UpdateMaskEquip(PlayState* play);

// Prompt
void KaleidoScope_UpdatePrompt(PlayState* play);

// Debug
void KaleidoScope_DrawInventoryEditor(PlayState* play);
void KaleidoScope_UpdateInventoryEditor(PlayState* play);

extern s16 gVtxPageMapWorldQuadsWidth[];
extern s16 gVtxPageMapWorldQuadsHeight[];

#endif
