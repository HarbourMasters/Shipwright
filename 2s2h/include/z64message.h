#ifndef Z64MESSAGE_H
#define Z64MESSAGE_H

#include "PR/ultratypes.h"
#include "z64view.h"
#include "unk.h"
#include "z64bombers_notebook.h"

struct Actor;
struct MessageTableEntry;
struct OcarinaStaff;
struct PlayState;

typedef enum TextBoxType {
    /* 0x00 */ TEXTBOX_TYPE_0,
    /* 0x01 */ TEXTBOX_TYPE_1,
    /* 0x02 */ TEXTBOX_TYPE_2,
    /* 0x03 */ TEXTBOX_TYPE_3,
    /* 0x04 */ TEXTBOX_TYPE_4,
    /* 0x05 */ TEXTBOX_TYPE_5,
    /* 0x06 */ TEXTBOX_TYPE_6,
    /* 0x07 */ TEXTBOX_TYPE_7,
    /* 0x08 */ TEXTBOX_TYPE_8,
    /* 0x09 */ TEXTBOX_TYPE_9,
    /* 0x0A */ TEXTBOX_TYPE_A,
    /* 0x0B */ TEXTBOX_TYPE_B,
    /* 0x0C */ TEXTBOX_TYPE_C,
    /* 0x0D */ TEXTBOX_TYPE_D,
    /* 0x0E */ TEXTBOX_TYPE_E,
    /* 0x0F */ TEXTBOX_TYPE_F,
    /* 0x10 */ TEXTBOX_TYPE_MAX
} TextBoxType;

#define TEXTBOX_ENDTYPE_DEFAULT    0x00

#define TEXTBOX_ENDTYPE_TWO_CHOICE    0x10
#define TEXTBOX_ENDTYPE_THREE_CHOICE    0x11
#define TEXTBOX_ENDTYPE_12    0x12

#define TEXTBOX_ENDTYPE_NEXT    0x20

#define TEXTBOX_ENDTYPE_PERSISTENT    0x30

#define TEXTBOX_ENDTYPE_EVENT    0x40
#define TEXTBOX_ENDTYPE_PAUSE_MENU    0x41
#define TEXTBOX_ENDTYPE_EVENT2    0x42

#define TEXTBOX_ENDTYPE_FADE_NORMAL    0x50
#define TEXTBOX_ENDTYPE_FADE_SKIPPABLE    0x52
#define TEXTBOX_ENDTYPE_FADE_STAGES_1    0x55
#define TEXTBOX_ENDTYPE_FADE_STAGES_2    0x56
#define TEXTBOX_ENDTYPE_FADE_STAGES_3    0x57

#define TEXTBOX_ENDTYPE_INPUT_BANK    0x60
#define TEXTBOX_ENDTYPE_INPUT_DOGGY_RACETRACK_BET    0x61
#define TEXTBOX_ENDTYPE_INPUT_BOMBER_CODE    0x62
#define TEXTBOX_ENDTYPE_INPUT_LOTTERY_CODE    0x63
#define TEXTBOX_ENDTYPE_64    0x64

typedef enum MessageMode {
    /* 0x00 */ MSGMODE_NONE,
    /* 0x01 */ MSGMODE_TEXT_START,
    /* 0x02 */ MSGMODE_TEXT_BOX_GROWING,
    /* 0x03 */ MSGMODE_TEXT_STARTING,
    /* 0x04 */ MSGMODE_TEXT_NEXT_MSG,
    /* 0x05 */ MSGMODE_TEXT_CONTINUING,
    /* 0x06 */ MSGMODE_TEXT_DISPLAYING,
    /* 0x07 */ MSGMODE_TEXT_AWAIT_INPUT,
    /* 0x08 */ MSGMODE_TEXT_DELAYED_BREAK,
    /* 0x09 */ MSGMODE_9,
    /* 0x0A */ MSGMODE_OCARINA_STARTING,
    /* 0x0B */ MSGMODE_SONG_DEMONSTRATION_STARTING,
    /* 0x0C */ MSGMODE_SONG_PROMPT_STARTING,
    /* 0x0D */ MSGMODE_OCARINA_PLAYING,
    /* 0x0E */ MSGMODE_E,
    /* 0x0F */ MSGMODE_OCARINA_FAIL,
    /* 0x10 */ MSGMODE_OCARINA_FAIL_NO_TEXT,
    /* 0x11 */ MSGMODE_OCARINA_NOTES_DROP,
    /* 0x12 */ MSGMODE_SONG_PLAYED,
    /* 0x13 */ MSGMODE_SETUP_DISPLAY_SONG_PLAYED,
    /* 0x14 */ MSGMODE_DISPLAY_SONG_PLAYED,
    /* 0x15 */ MSGMODE_DISPLAY_SONG_PLAYED_TEXT_BEGIN,
    /* 0x16 */ MSGMODE_16,
    /* 0x17 */ MSGMODE_17,
    /* 0x18 */ MSGMODE_18,
    /* 0x19 */ MSGMODE_19,
    /* 0x1A */ MSGMODE_SONG_DEMONSTRATION,
    /* 0x1B */ MSGMODE_SONG_DEMONSTRATION_DONE,
    /* 0x1C */ MSGMODE_SONG_PROMPT,
    /* 0x1D */ MSGMODE_SONG_PROMPT_SUCCESS,
    /* 0x1E */ MSGMODE_SONG_PROMPT_FAIL,
    /* 0x1F */ MSGMODE_SONG_PROMPT_NOTES_DROP,
    /* 0x20 */ MSGMODE_OCARINA_AWAIT_INPUT,
    /* 0x21 */ MSGMODE_21,
    /* 0x22 */ MSGMODE_22,
    /* 0x23 */ MSGMODE_23,
    /* 0x24 */ MSGMODE_24,
    /* 0x25 */ MSGMODE_25,
    /* 0x26 */ MSGMODE_26,
    /* 0x27 */ MSGMODE_SCARECROW_LONG_RECORDING_START,
    /* 0x28 */ MSGMODE_SCARECROW_LONG_RECORDING_ONGOING,
    /* 0x29 */ MSGMODE_SCARECROW_LONG_DEMONSTRATION,
    /* 0x2A */ MSGMODE_SCARECROW_SPAWN_RECORDING_START,
    /* 0x2B */ MSGMODE_SCARECROW_SPAWN_RECORDING_ONGOING,
    /* 0x2C */ MSGMODE_SCARECROW_SPAWN_RECORDING_FAILED,
    /* 0x2D */ MSGMODE_SCARECROW_SPAWN_RECORDING_DONE,
    /* 0x2E */ MSGMODE_SCARECROW_SPAWN_DEMONSTRATION,
    /* 0x2F */ MSGMODE_2F,
    /* 0x30 */ MSGMODE_30,
    /* 0x31 */ MSGMODE_31,
    /* 0x32 */ MSGMODE_32,
    /* 0x33 */ MSGMODE_33,
    /* 0x34 */ MSGMODE_34,
    /* 0x35 */ MSGMODE_35,
    /* 0x36 */ MSGMODE_36,
    /* 0x37 */ MSGMODE_37,
    /* 0x38 */ MSGMODE_38,
    /* 0x39 */ MSGMODE_39,
    /* 0x3A */ MSGMODE_3A,
    /* 0x3B */ MSGMODE_3B,
    /* 0x3C */ MSGMODE_3C,
    /* 0x3D */ MSGMODE_3D,
    /* 0x3E */ MSGMODE_3E,
    /* 0x3F */ MSGMODE_3F,
    /* 0x40 */ MSGMODE_40,
    /* 0x41 */ MSGMODE_TEXT_AWAIT_NEXT,
    /* 0x42 */ MSGMODE_TEXT_DONE,
    /* 0x43 */ MSGMODE_TEXT_CLOSING,
    /* 0x44 */ MSGMODE_PAUSED, // Causes the message system to do nothing until external code sets a new message mode or calls a public function
    /* 0x45 */ MSGMODE_SCENE_TITLE_CARD_FADE_IN_BACKGROUND, // Scene Title Card
    /* 0x46 */ MSGMODE_SCENE_TITLE_CARD_FADE_IN_TEXT,
    /* 0x47 */ MSGMODE_SCENE_TITLE_CARD_DISPLAYING,
    /* 0x48 */ MSGMODE_SCENE_TITLE_CARD_FADE_OUT_TEXT,
    /* 0x49 */ MSGMODE_SCENE_TITLE_CARD_FADE_OUT_BACKGROUND,
    /* 0x4A */ MSGMODE_NEW_CYCLE_0, // End of cycle?
    /* 0x4B */ MSGMODE_NEW_CYCLE_1,
    /* 0x4C */ MSGMODE_NEW_CYCLE_2,
    /* 0x4D */ MSGMODE_OWL_SAVE_0,
    /* 0x4E */ MSGMODE_OWL_SAVE_1,
    /* 0x4F */ MSGMODE_OWL_SAVE_2
} MessageMode;

typedef enum FaceReactionSet {
    /* 0x11 */ FACE_REACTION_SET_JIM = 0x11,
    /* 0x12 */ FACE_REACTION_SET_BOMBERS,
    /* 0x15 */ FACE_REACTION_SET_BOMBERS_HIDEOUT_GUARD = 0x15,
    /* 0x2D */ FACE_REACTION_SET_TREASURE_CHEST_SHOP_GAL = 0x2D,
    /* 0x2F */ FACE_REACTION_SET_CURIOSITY_SHOP_MAN = 0x2F,
    /* 0x30 */ FACE_REACTION_SET_TOWN_SHOOTING_GALLERY_MAN,
    /* 0x31 */ FACE_REACTION_SET_SWAMP_SHOOTING_GALLERY_MAN,
    /* 0x36 */ FACE_REACTION_SET_PART_TIMER = 0x36
} FaceReactionSet;

u16 Text_GetFaceReaction(struct PlayState* play, FaceReactionSet reactionSet);

typedef enum TextState {
    /*  0 */ TEXT_STATE_NONE,
    /*  1 */ TEXT_STATE_NEXT,
    /*  2 */ TEXT_STATE_CLOSING,
    /*  3 */ TEXT_STATE_FADING,
    /*  4 */ TEXT_STATE_CHOICE,
    /*  5 */ TEXT_STATE_EVENT,
    /*  6 */ TEXT_STATE_DONE,
    /*  7 */ TEXT_STATE_SONG_DEMO_DONE,
    /*  8 */ TEXT_STATE_8,
    /*  9 */ TEXT_STATE_9,
    /* 10 */ TEXT_STATE_AWAITING_NEXT,
    /* 11 */ TEXT_STATE_11,
    /* 12 */ TEXT_STATE_12,
    /* 13 */ TEXT_STATE_13,
    /* 14 */ TEXT_STATE_INPUT_RUPEES,
    /* 15 */ TEXT_STATE_INPUT_BOMBER_CODE,
    /* 16 */ TEXT_STATE_PAUSE_MENU,
    /* 17 */ TEXT_STATE_INPUT_LOTTERY_CODE,
    /* 18 */ TEXT_STATE_18
} TextState;

#define FONT_CHAR_TEX_WIDTH 16
#define FONT_CHAR_TEX_HEIGHT 16
//! TODO: Make this use `sizeof(AnyFontTextureSymbol)`
#define FONT_CHAR_TEX_SIZE ((16 * 16) / 2) // 16x16 I4 texture

// TODO: should Font be in its own header or is it fine to have it here?
// Font textures are loaded into here
typedef struct {
    /* 0x00000 */ union {
        u8 charBuf[2][FONT_CHAR_TEX_SIZE * 120];
        u64 force_structure_alignment_charTex;
    };
    /* 0x07800 */ union {
        u8 iconBuf[FONT_CHAR_TEX_SIZE];
        u64 force_structure_alignment_icon;
    };
    /* 0x07880 */ union {
        u8 fontBuf[FONT_CHAR_TEX_SIZE * 320];
        u64 force_structure_alignment_font;
    };
    /* 0x11880 */ union {
        unsigned char schar[1280]; // msgBuf
        u16 wchar[640];   // msgBufWide
        u64 force_structure_alignment_msg;
    } msgBuf;
    /* 0x11D80 */ uintptr_t messageStart;
    /* 0x11D84 */ uintptr_t messageEnd;
    /* 0x11D88 */ u8 unk_11D88; // current Char Buffer ?
} Font; // size = 0x11D90

typedef enum {
    /* 0 */ TEXTBOX_SEG_ICON,
    /* 1 */ TEXTBOX_SEG_TYPE,
    /* 1 */ TEXTBOX_SEG_BG_1,
    /* 1 */ TEXTBOX_SEG_BG_2,
    /* 4 */ TEXTBOX_SEG_MAX
} TextboxSegmentIndex;

typedef struct MessageContext {
    /* 0x00000 */ View view;
    /* 0x00168 */ Font font;
    /* 0x11EF8 */ char** textboxSegment;
    /* 0x11EFC */ UNK_TYPE1 unk11EFC[0x4];
    /* 0x11F00 */ struct OcarinaStaff* ocarinaStaff;
    /* 0x11F04 */ u16 currentTextId;
    /* 0x11F06 */ u16 choiceTextId; // s16?
    /* 0x11F08 */ u16 unk11F08;
    /* 0x11F0A */ u8 textBoxType;
    /* 0x11F0B */ u8 textBoxPos;
    /* 0x11F0C */ u8 unk11F0C;
    /* 0x11F10 */ s32 msgLength;
    /* 0x11F14 */ u16 nextTextId;
    /* 0x11F16 */ u16 itemId;
    /* 0x11F18 */ u8 unk11F18;
    /* 0x11F1A */ s16 unk11F1A[3];
    /* 0x11F20 */ UNK_TYPE1 unk11F20[0x2];
    /* 0x11F22 */ u8 msgMode;
    /* 0x11F23 */ UNK_TYPE1 unk11F23;
    /* 0x11F24 */ union {
        char schar[200];
        u16 wchar[100];
    } decodedBuffer;
    /* 0x11FEC */ u16 msgBufPos;
    /* 0x11FEE */ u16 textDrawPos;
    /* 0x11FF0 */ u16 decodedTextLen;
    /* 0x11FF2 */ u16 textUnskippable;
    /* 0x11FF4 */ s16 textPosX;
    /* 0x11FF6 */ s16 textPosY;
    /* 0x11FF8 */ s16 unk11FF8;
    /* 0x11FFA */ s16 unk11FFA;
    /* 0x11FFC */ s16 unk11FFC;
    /* 0x11FFE */ s16 unk11FFE[0x3];
    /* 0x12004 */ s16 textboxXTarget;
    /* 0x12006 */ s16 textboxYTarget;
    /* 0x12008 */ s16 unk12008;
    /* 0x1200A */ s16 unk1200A;
    /* 0x1200C */ s16 unk1200C;
    /* 0x1200E */ s16 unk1200E;
    /* 0x12010 */ s16 unk12010;
    /* 0x12012 */ s16 unk12012;
    /* 0x12014 */ s16 unk12014;
    /* 0x12014 */ s16 unk12016;
    /* 0x12018 */ s16 textColorR;
    /* 0x1201A */ s16 textColorG;
    /* 0x1201C */ s16 textColorB;
    /* 0x1201E */ s16 textColorAlpha;
    /* 0x12020 */ u8 textboxEndType;
    /* 0x12021 */ u8 choiceIndex;
    /* 0x12022 */ u8 choiceNum;
    /* 0x12023 */ u8 stateTimer;
    /* 0x12024 */ u16 textDelayTimer;
    /* 0x12026 */ u16 textDelay;
    /* 0x12028 */ u16 songPlayed;
    /* 0x1202A */ u16 ocarinaMode;
    /* 0x1202C */ u16 ocarinaAction;
    /* 0x1202E */ u16 lastPlayedSong;
    /* 0x12030 */ s16 unk_12030;
    /* 0x12032 */ UNK_TYPE1 unk_12032[0x2];
    /* 0x12034 */ s16 textboxColorRed;
    /* 0x12036 */ s16 textboxColorGreen;
    /* 0x12038 */ s16 textboxColorBlue;
    /* 0x1203A */ s16 textboxColorAlphaTarget;
    /* 0x1203C */ s16 textboxColorAlphaCurrent;
    /* 0x1203E */ s16 unk1203E;
    /* 0x12040 */ struct Actor* talkActor;
    /* 0x12044 */ s16 unk12044;
    /* 0x12046 */ s16 blockSunsSong;
    /* 0x12048 */ u8 ocarinaButtonIndex;
    /* 0x1204A */ s16 ocarinaButtonsPosY[5];
    /* 0x12054 */ s16 unk12054[6]; // First, second and third digits in lottery code guess
    /* 0x1205A */ UNK_TYPE1 unk12060[0x8];
    /* 0x12068 */ s16 textboxX;
    /* 0x1206A */ s16 textboxY;
    /* 0x1206C */ s32 unk1206C;
    /* 0x12070 */ s32 unk12070;
    /* 0x12074 */ s32 unk12074;
    /* 0x12078 */ s32 rupeesSelected; // Used for bank and doggy racetrack bet
    /* 0x1207C */ s32 rupeesTotal; // Used for bank and doggy racetrack bet
    /* 0x12080 */ struct MessageTableEntry* messageTable;
    /* 0x12084 */ struct MessageTableEntry* messageTableNES;
    /* 0x12088 */ UNK_TYPE1 unk12088[0x4];
    /* 0x1208C */ struct MessageTableEntry* messageTableCredits;
    /* 0x12090 */ s16 textIsCredits;
    /* 0x12092 */ s16 messageHasSetSfx;
    /* 0x12094 */ u8 textboxSkipped;
    /* 0x12098 */ f32 textCharScale;
    /* 0x1209C */ s16 textFade;
    /* 0x120A0 */ s32 ocarinaAvailableSongs;
    /* 0x120A4 */ s16 stickAdjX;
    /* 0x120A6 */ s16 stickAdjY;
    /* 0x120A8 */ s16 stickXRepeatState;
    /* 0x120AA */ s16 stickYRepeatState;
    /* 0x120AC */ s16 stickXRepeatTimer;
    /* 0x120AE */ s16 stickYRepeatTimer;
    /* 0x120B0 */ u8 ocarinaSongEffectActive;
    /* 0x120B1 */ u8 bombersNotebookEventQueueCount;
    /* 0x120B2 */ u8 bombersNotebookEventQueue[10];
    /* 0x120BC */ u16 hudVisibility;
    /* 0x120BE */ s16 unk120BE;
    /* 0x120C0 */ s16 unk120C0;
    /* 0x120C2 */ s16 unk120C2;
    /* 0x120C4 */ s32 unk120C4;
    /* 0x120C8 */ s16 unk120C8;
    /* 0x120CA */ s16 unk120CA;
    /* 0x120CC */ s16 unk120CC;
    /* 0x120CE */ s16 unk120CE;
    /* 0x120D0 */ s16 unk120D0;
    /* 0x120D2 */ s16 unk120D2;
    /* 0x120D4 */ s16 unk120D4;
    /* 0x120D6 */ s16 unk120D6;
    /* 0x120D8 */ s16 unk120D8;
    /* 0x120DA */ UNK_TYPE1 unk_120DA[0x6];
} MessageContext; // size = 0x120E0

bool Message_ShouldAdvance(struct PlayState* play);
void Message_CloseTextbox(struct PlayState* play);
void Message_DrawTextChar(struct PlayState* play, TexturePtr texture, Gfx** gfxP);
void Message_DrawItemIcon(struct PlayState* play, Gfx** gfxP);
void Message_HandleOcarina(struct PlayState* play);
void Message_LoadItemIcon(struct PlayState* play, u16 itemId, s16 arg2);
void Message_DecodeHeader(struct PlayState* play);
void func_801514B0(struct PlayState* play, u16 arg1, u8 arg2);
void Message_StartTextbox(struct PlayState* play, u16 textId, Actor* actor);
void Message_ContinueTextbox(struct PlayState* play, u16 textId);
void Message_DisplaySceneTitleCard(struct PlayState* play, u16 textId);
void Message_BombersNotebookQueueEvent(struct PlayState* play, u8 event);
void Message_DisplayOcarinaStaff(struct PlayState* play, u16 ocarinaAction);
void Message_DisplayOcarinaStaffBlockSunsSong(struct PlayState* play, u16 ocarinaAction);
u8 Message_GetState(MessageContext* msgCtx);
void Message_Draw(struct PlayState* play);
void Message_Update(struct PlayState* play);
void Message_Init(struct PlayState* play);
void Message_FindMessageNES(struct PlayState* play, u16 textId);
void Message_DrawTextNES(struct PlayState* play, Gfx** gfxP, u16 textDrawPos);
void Message_DecodeNES(struct PlayState* play);
void Message_FindCreditsMessage(struct PlayState* play, u16 textId);
void Message_DrawTextCredits(struct PlayState* play, Gfx** gfxP);
void Message_DecodeCredits(struct PlayState* play);

extern u8 gPageSwitchNextButtonStatus[6][5];
extern u16 gBombersNotebookWeekEventFlags[BOMBERS_NOTEBOOK_EVENT_MAX];
extern s16 gOcarinaSongItemMap[];

#endif
