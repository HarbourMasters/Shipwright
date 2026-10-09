/* Header taken as placeholder from OoT sister project */
/* Confirm as needed */

#ifndef REGS_H
#define REGS_H

#include "ultra64.h"
#include "unk.h"


#define REG_GROUPS 29 // number of REG groups, i.e. REG, SREG, OREG, etc.
#define REG_PAGES 6
#define REG_PER_PAGE 16
#define REG_PER_GROUP REG_PAGES * REG_PER_PAGE

typedef struct RegEditor {
    /* 0x00 */ u8  regPage; // 0: no page selected (reg editor is not active); 1: first page; `REG_PAGES`: last page
    /* 0x01 */ u8  regGroup; // Indexed from 0 to `REG_GROUPS`-1. Each group has its own character to identify it.
    /* 0x02 */ u8  regCur; // Selected reg, indexed from 0 as the page start
    /* 0x03 */ u8  dPadInputPrev;
    /* 0x04 */ u32 inputRepeatTimer;
    /* 0x08 */ UNK_TYPE1 pad_08[0xC];
    /* 0x14 */ s16 data[REG_GROUPS * REG_PER_GROUP]; // Accessed through *REG macros
} RegEditor; // size = 0x15D4

void Regs_Init(void);

extern RegEditor* gRegEditor;

#define BASE_REG(n, r) (gRegEditor->data[n * REG_PER_GROUP + r])

#define  REG(r) BASE_REG(0, r)
#define SREG(r) BASE_REG(1, r)
#define OREG(r) BASE_REG(2, r)
#define PREG(r) BASE_REG(3, r)
#define QREG(r) BASE_REG(4, r)
#define MREG(r) BASE_REG(5, r)
#define YREG(r) BASE_REG(6, r)
#define DREG(r) BASE_REG(7, r)
#define UREG(r) BASE_REG(8, r)
#define IREG(r) BASE_REG(9, r)
#define ZREG(r) BASE_REG(10, r)
#define CREG(r) BASE_REG(11, r)
#define NREG(r) BASE_REG(12, r)
#define KREG(r) BASE_REG(13, r)
#define XREG(r) BASE_REG(14, r)
#define cREG(r) BASE_REG(15, r)
#define sREG(r) BASE_REG(16, r)
#define iREG(r) BASE_REG(17, r)
#define WREG(r) BASE_REG(18, r)
#define AREG(r) BASE_REG(19, r)
#define VREG(r) BASE_REG(20, r)
#define HREG(r) BASE_REG(21, r)
#define GREG(r) BASE_REG(22, r)
#define mREG(r) BASE_REG(23, r)
#define nREG(r) BASE_REG(24, r)
#define BREG(r) BASE_REG(25, r)
#define dREG(r) BASE_REG(26, r)
#define kREG(r) BASE_REG(27, r)
#define bREG(r) BASE_REG(28, r)

/* TODO: There are still a few OoT defines here that need confirmation */

#define R_ENV_DISABLE_DBG                 REG(9)
#define R_TIME_SPEED                      REG(15)
#define R_DECELERATE_RATE                 REG(43)
#define R_RUN_SPEED_LIMIT                 REG(45)

#define R_ENABLE_ARENA_DBG                SREG(0) // Same as OoT
#define R_ROOM_IMAGE_NODRAW_FLAGS         SREG(25)
#define R_UPDATE_RATE                     SREG(30)
#define R_VI_MODE_EDIT_STATE              SREG(48)
#define R_VI_MODE_EDIT_WIDTH              SREG(49)
#define R_VI_MODE_EDIT_HEIGHT             SREG(50)
#define R_VI_MODE_EDIT_ULY_ADJ            SREG(51)
#define R_VI_MODE_EDIT_LRY_ADJ            SREG(52)
#define R_VI_MODE_EDIT_ULX_ADJ            SREG(53)
#define R_VI_MODE_EDIT_LRX_ADJ            SREG(54)
#define R_FB_FILTER_TYPE                  SREG(80)
#define R_FB_FILTER_PRIM_COLOR(c)         SREG(81 + c)
#define R_FB_FILTER_A                     SREG(84)
#define R_FB_FILTER_ENV_COLOR(c)          SREG(85 + c)
#define R_PICTO_PHOTO_STATE               SREG(89)
#define R_MOTION_BLUR_ALPHA               SREG(90)
#define R_MOTION_BLUR_ENABLED             SREG(91)
#define R_MOTION_BLUR_PRIORITY_ALPHA      SREG(92)
#define R_MOTION_BLUR_PRIORITY_ENABLED    SREG(93)
#define R_PAUSE_BG_PRERENDER_STATE        SREG(94) // PauseBgPreRenderState enum

#define R_PLAY_FILL_SCREEN_ON             MREG(64)
#define R_PLAY_FILL_SCREEN_R              MREG(65)
#define R_PLAY_FILL_SCREEN_G              MREG(66)
#define R_PLAY_FILL_SCREEN_B              MREG(67)
#define R_PLAY_FILL_SCREEN_ALPHA          MREG(68)

#define R_PAUSE_WORLD_MAP_YAW              YREG(24)
#define R_PAUSE_WORLD_MAP_Y_OFFSET         YREG(25)
#define R_PAUSE_WORLD_MAP_DEPTH            YREG(26)
#define R_PICTO_FOCUS_BORDER_TOPLEFT_X     YREG(32)
#define R_PICTO_FOCUS_BORDER_TOPLEFT_Y     YREG(33)
#define R_PICTO_FOCUS_BORDER_TOPRIGHT_X    YREG(34)
#define R_PICTO_FOCUS_BORDER_TOPRIGHT_Y    YREG(35)
#define R_PICTO_FOCUS_BORDER_BOTTOMLEFT_X  YREG(36)
#define R_PICTO_FOCUS_BORDER_BOTTOMLEFT_Y  YREG(37)
#define R_PICTO_FOCUS_BORDER_BOTTOMRIGHT_X YREG(38)
#define R_PICTO_FOCUS_BORDER_BOTTOMRIGHT_Y YREG(39)
#define R_PICTO_FOCUS_ICON_X               YREG(40)
#define R_PICTO_FOCUS_ICON_Y               YREG(41)
#define R_PICTO_FOCUS_TEXT_X               YREG(42)
#define R_PICTO_FOCUS_TEXT_Y               YREG(43)
#define R_PAUSE_DBG_QUEST_CURSOR_ON        YREG(69)
#define R_PAUSE_DBG_QUEST_CURSOR_X         YREG(70)
#define R_PAUSE_DBG_QUEST_CURSOR_Y         YREG(71)

#define R_MAGIC_FILL_COLOR(i)             ZREG(0 + i)
#define R_C_BTN_COLOR(i)                  ZREG(39 + i)
#define R_B_BTN_COLOR(i)                  ZREG(43 + i)
#define R_START_LABEL_DD(i)               ZREG(48 + i)
#define R_START_LABEL_Y(i)                ZREG(51 + i)
#define R_START_LABEL_X(i)                ZREG(54 + i)
#define R_C_UP_BTN_X                      ZREG(62)
#define R_C_UP_BTN_Y                      ZREG(63)
#define R_START_BTN_X                     ZREG(68)
#define R_START_BTN_Y                     ZREG(69)
#define R_ITEM_BTN_X(i)                   ZREG(70 + i)
#define R_ITEM_BTN_Y(i)                   ZREG(74 + i)
#define R_ITEM_BTN_DD(i)                  ZREG(78 + i)
#define R_ITEM_ICON_X(i)                  ZREG(82 + i)
#define R_ITEM_ICON_Y(i)                  ZREG(86 + i)
#define R_ITEM_ICON_DD(i)                 ZREG(90 + i)

#define R_MAGIC_DBG_SET_UPGRADE             XREG(4)
#define R_A_BTN_Y                           XREG(16)
#define R_A_BTN_X                           XREG(17)
#define R_A_ICON_Y                          XREG(19)
#define R_A_ICON_X                          XREG(20)
#define R_A_BTN_COLOR(i)                    XREG(22 + i)
#define R_A_BTN_Y_OFFSET                    XREG(31)
#define R_MAGIC_CONSUME_TIMER_GIANTS_MASK   XREG(41)
#define R_THREE_DAY_CLOCK_Y_POS             XREG(43) // TODO: Test
#define R_THREE_DAY_CLOCK_SUN_MOON_CUTOFF   XREG(44)
#define R_THREE_DAY_CLOCK_HOUR_DIGIT_CUTOFF XREG(45)
#define R_PAUSE_DBG_MAP_CLOUD_ON            XREG(50)
#define R_PAUSE_DBG_MAP_CLOUD_X             XREG(52)
#define R_PAUSE_DBG_MAP_CLOUD_Y             XREG(53)
#define R_MOON_CRASH_TIMER_Y                XREG(80)
#define R_MOON_CRASH_TIMER_X                XREG(81)
#define R_PAUSE_OWL_WARP_ALPHA              XREG(87)
#define R_STORY_FILL_SCREEN_ALPHA           XREG(91)
#define R_PLAYER_FLOOR_REVERSE_INDEX        XREG(94) // stores what floor the player is on
#define R_MINIMAP_DISABLED                  XREG(95)

#define R_ENV_LIGHT1_DIR(i)               cREG(3 + (i))
#define R_ENV_LIGHT2_DIR(i)               cREG(6 + (i))

#define R_TRANS_FADE_FLASH_ALPHA_STEP     iREG(50) // Set to a negative number to start the flash
#define R_ROOM_CULL_DEBUG_MODE            iREG(86)
#define R_ROOM_CULL_NUM_ENTRIES           iREG(87)
#define R_ROOM_CULL_USED_ENTRIES          iREG(88)
#define R_ROOM_CULL_DEBUG_TARGET          iREG(89)

#define R_B_LABEL_DD                      WREG(0)
#define R_OW_MINIMAP_X                    WREG(29)
#define R_OW_MINIMAP_Y                    WREG(30)
#define R_B_LABEL_X(i)                    WREG(40 + i)
#define R_B_LABEL_Y(i)                    WREG(43 + i)
#define R_DGN_MINIMAP_X                   WREG(68)
#define R_DGN_MINIMAP_Y                   WREG(69)

#define R_MAP_INDEX                       VREG(11)
#define R_MAP_TEX_INDEX_BASE              VREG(12)
#define R_MAP_TEX_INDEX                   VREG(13)
#define R_COMPASS_SCALE_X                 VREG(14)
#define R_COMPASS_SCALE_Y                 VREG(15)
#define R_COMPASS_OFFSET_X                VREG(16)
#define R_COMPASS_OFFSET_Y                VREG(17)
#define R_MINIMAP_COLOR(i)                VREG(18 + i)
#define R_ITEM_AMMO_X(i)                  VREG(64 + i)
#define R_ITEM_AMMO_Y(i)                  VREG(68 + i)
#define R_ITEM_ICON_WIDTH(i)              VREG(76 + i)
#define R_ITEM_BTN_WIDTH(i)               VREG(80 + i)

// Name inferred from OoT. Set to true to manually set play->csCtx.script
#define R_USE_DEBUG_CUTSCENE              dREG(95)

#endif
