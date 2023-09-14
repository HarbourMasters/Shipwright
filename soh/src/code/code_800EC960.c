#include <libultraship/libultra.h>
#include "global.h"
#include "soh/Enhancements/audio/AudioEditor.h"

// TODO: can these macros be shared between files? code_800F9280 seems to use
// versions without any casts...
#define Audio_DisableSeq(playerIdx, fadeOut) Audio_QueueCmdS32(0x83000000 | ((u8)playerIdx << 16), fadeOut)
#define Audio_StartSeq(playerIdx, fadeTimer, seqId) \
    Audio_QueueSeqCmd(0x00000000 | ((u8)playerIdx << 24) | ((u8)(fadeTimer) << 0x10) | (u16)seqId)
#define Audio_SeqCmd7(playerIdx, a, b) \
    Audio_QueueSeqCmd(0x70000000 | ((u8)playerIdx << 0x18) | ((u8)a << 0x10) | (u8)(b))
#define Audio_SeqCmdC(playerIdx, a, b, c) \
    Audio_QueueSeqCmd(0xC0000000 | ((u8)playerIdx << 24) | ((u8)a << 16) | ((u8)b << 8) | ((u8)(c)))
#define Audio_SeqCmdA(playerIdx, a) Audio_QueueSeqCmd(0xA0000000 | ((u8)playerIdx << 24) | ((u16)(a)))
#define Audio_SeqCmd1(playerIdx, a) Audio_QueueSeqCmd(0x100000FF | ((u8)playerIdx << 24) | ((u8)(a) << 16))
#define Audio_SeqCmdB(playerIdx, a, b, c) \
    Audio_QueueSeqCmd(0xB0000000 | ((u8)playerIdx << 24) | ((u8)a << 16) | ((u8)b << 8) | ((u8)c))
#define Audio_SeqCmdB40(playerIdx, a, b) Audio_QueueSeqCmd(0xB0004000 | ((u8)playerIdx << 24) | ((u8)a << 16) | ((u8)b))
#define Audio_SeqCmd6(playerIdx, a, b, c) \
    Audio_QueueSeqCmd(0x60000000 | ((u8)playerIdx << 24) | ((u8)(a) << 16) | ((u8)b << 8) | ((u8)c))
#define Audio_SeqCmdE0(playerIdx, a) Audio_QueueSeqCmd(0xE0000000 | ((u8)playerIdx << 24) | ((u8)a))
#define Audio_SeqCmdE01(playerIdx, a) Audio_QueueSeqCmd(0xE0000100 | ((u8)playerIdx << 24) | ((u16)a))
#define Audio_SeqCmd8(playerIdx, a, b, c) \
    Audio_QueueSeqCmd(0x80000000 | ((u8)playerIdx << 24) | ((u8)a << 16) | ((u8)b << 8) | ((u8)c))
#define Audio_SeqCmdF(playerIdx, a) Audio_QueueSeqCmd(0xF0000000 | ((u8)playerIdx << 24) | ((u8)a))

typedef struct {
    /* 0x0 */ f32 vol;
    /* 0x4 */ f32 freqScale;
    /* 0x8 */ s8 reverb;
    /* 0x9 */ s8 panSigned;
    /* 0xA */ s8 stereoBits;
    /* 0xB */ u8 filter;
    /* 0xC */ u8 unk_0C;
} SfxPlayerState;

typedef struct {
    /* 0x0 */ f32 value;
    /* 0x4 */ f32 target;
    /* 0x8 */ f32 step;
    /* 0xC */ s32 remainingFrames;
} FreqLerp;

typedef struct {
    /* 0x0 */ u16 playerIO;
    /* 0x2 */ u16 channelMask;
    /* 0x4 */ u8 channelIO[3 * 33 + 1];
} NatureAmbienceDataIO; // size = 0x68

typedef enum {
    /* 0x0 */ PAGE_NON,
    /* 0x1 */ PAGE_SOUND_CONTROL,
    /* 0x2 */ PAGE_SPEC_INFO, // unused
    /* 0x3 */ PAGE_HEAP_INFO,
    /* 0x4 */ PAGE_GROUP_TRACK_INFO, // unused
    /* 0x5 */ PAGE_SUB_TRACK_INFO,
    /* 0x6 */ PAGE_CHANNEL_INFO, // unused
    /* 0x7 */ PAGE_INTERFACE_INFO,
    /* 0x8 */ PAGE_SFX_SWAP,
    /* 0x9 */ PAGE_BLOCK_CHANGE_BGM,
    /* 0xA */ PAGE_NATURAL_SOUND_CONTROL, // unused
    /* 0xB */ PAGE_OCARINA_TEST,
    /* 0xC */ PAGE_SFX_PARAMETER_CHANGE,
    /* 0xD */ PAGE_SCROLL_PRINT,
    /* 0xE */ PAGE_FREE_AREA,
    /* 0xF */ PAGE_MAX
} AudioDebugPage;

/** bit field of songs that can be played
 * 0x0800 storms
 * 0x0400 song of time
 * 0x0200 suns
 * 0x0100 lullaby
 * 0x0080 epona
 * 0x0040 sarias
 * 0x0020 prelude
 * 0x0010 nocturne
 * 0x0008 requiem
 * 0x0004 serenade
 * 0x0002 bolero
 * 0x0001 minuet
 */

#define SCROLL_PRINT_BUF_SIZE 25

#define SFX_PLAYER_CHANNEL_OCARINA 13

u8 gIsLargeSoundBank[7] = { 0, 0, 0, 1, 0, 0, 0 };

// Only the first row of these is supported by sequence 0. (gSfxChannelLayout is always 0.)
u8 gChannelsPerBank[4][7] = {
    { 3, 2, 3, 3, 2, 1, 2 },
    { 3, 2, 2, 2, 2, 2, 2 },
    { 3, 2, 2, 2, 2, 2, 2 },
    { 4, 1, 0, 0, 2, 2, 2 },
};
u8 gUsedChannelsPerBank[4][7] = {
    { 3, 2, 3, 2, 2, 1, 1 },
    { 3, 1, 1, 1, 2, 1, 1 },
    { 3, 1, 1, 1, 2, 1, 1 },
    { 2, 1, 0, 0, 1, 1, 1 },
};

f32 D_801305B0 = 0.7950898f;
s8 D_801305B4 = 35;
s8 D_801305B8 = 20;
s8 D_801305BC = 30;
s8 D_801305C0 = 20;
f32 sBehindScreenZ[2] = { -15.0f, -65.0f };
u8 sAudioIncreasingTranspose = 0;
u8 gMorphaTransposeTable[16] = { 0, 0, 0, 1, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8 };
u8 sPrevChargeLevel = 0;
f32 D_801305E4[4] = { 1.0f, 1.12246f, 1.33484f, 1.33484f }; // 2**({0, 2, 5, 5}/12)
f32 D_801305F4 = 1.0f;
u8 D_801305F8[8] = { 127, 80, 75, 73, 70, 68, 65, 60 };
u8 D_80130600 = 0;
s8 D_80130604 = 2;
s8 D_80130608 = 0;
s8 sAudioCutsceneFlag = 0;
s8 sSpecReverb = 0;
s8 sAudioEnvReverb = 0;
s8 sAudioCodeReverb = 0;
u8 sPrevSeqMode = 0;
f32 sAudioEnemyDist = 0.0f;
s8 sAudioEnemyVol = 127;
u16 sPrevMainBgmSeqId = NA_BGM_DISABLED;
u8 D_8013062C = 0;
u8 D_80130630 = NA_BGM_GENERAL_SFX;
u32 sNumFramesStill = 0;
u32 sNumFramesMoving = 0;
u8 sAudioBaseFilter = 0;
u8 sAudioExtraFilter = 0;
u8 sAudioBaseFilter2 = 0;
u8 sAudioExtraFilter2 = 0;
Vec3f* sSariaBgmPtr = NULL;
f32 D_80130650 = 2000.0f;
u8 sSeqModeInput = 0;
u8 sSeqFlags[0x6F] = {
    0x2,  // NA_BGM_GENERAL_SFX
    0x1,  // NA_BGM_NATURE_BACKGROUND
    0,    // NA_BGM_FIELD_LOGIC
    0,    // NA_BGM_FIELD_INIT
    0,    // NA_BGM_FIELD_DEFAULT_1
    0,    // NA_BGM_FIELD_DEFAULT_2
    0,    // NA_BGM_FIELD_DEFAULT_3
    0,    // NA_BGM_FIELD_DEFAULT_4
    0,    // NA_BGM_FIELD_DEFAULT_5
    0,    // NA_BGM_FIELD_DEFAULT_6
    0,    // NA_BGM_FIELD_DEFAULT_7
    0,    // NA_BGM_FIELD_DEFAULT_8
    0,    // NA_BGM_FIELD_DEFAULT_9
    0,    // NA_BGM_FIELD_DEFAULT_A
    0,    // NA_BGM_FIELD_DEFAULT_B
    0,    // NA_BGM_FIELD_ENEMY_INIT
    0,    // NA_BGM_FIELD_ENEMY_1
    0,    // NA_BGM_FIELD_ENEMY_2
    0,    // NA_BGM_FIELD_ENEMY_3
    0,    // NA_BGM_FIELD_ENEMY_4
    0,    // NA_BGM_FIELD_STILL_1
    0,    // NA_BGM_FIELD_STILL_2
    0,    // NA_BGM_FIELD_STILL_3
    0,    // NA_BGM_FIELD_STILL_4
    0x21, // NA_BGM_DUNGEON
    0x10, // NA_BGM_KAKARIKO_ADULT
    0,    // NA_BGM_ENEMY
    0x88, // NA_BGM_BOSS
    0x1,  // NA_BGM_INSIDE_DEKU_TREE
    0,    // NA_BGM_MARKET
    0,    // NA_BGM_TITLE
    0x20, // NA_BGM_LINK_HOUSE
    0,    // NA_BGM_GAME_OVER
    0,    // NA_BGM_BOSS_CLEAR
    0x2,  // NA_BGM_ITEM_GET
    0x4,  // NA_BGM_OPENING_GANON
    0x2,  // NA_BGM_HEART_GET
    0x2,  // NA_BGM_OCA_LIGHT
    0x1,  // NA_BGM_JABU_JABU
    0x10, // NA_BGM_KAKARIKO_KID
    0,    // NA_BGM_GREAT_FAIRY
    0,    // NA_BGM_ZELDA_THEME
    0x1,  // NA_BGM_FIRE_TEMPLE
    0x2,  // NA_BGM_OPEN_TRE_BOX
    0x1,  // NA_BGM_FOREST_TEMPLE
    0,    // NA_BGM_COURTYARD
    0x80, // NA_BGM_GANON_TOWER
    0,    // NA_BGM_LONLON
    0x80, // NA_BGM_GORON_CITY
    0,    // NA_BGM_FIELD_MORNING
    0x2,  // NA_BGM_SPIRITUAL_STONE
    0x2,  // NA_BGM_OCA_BOLERO
    0x2,  // NA_BGM_OCA_MINUET
    0x2,  // NA_BGM_OCA_SERENADE
    0x2,  // NA_BGM_OCA_REQUIEM
    0x2,  // NA_BGM_OCA_NOCTURNE
    0x88, // NA_BGM_MINI_BOSS
    0x2,  // NA_BGM_SMALL_ITEM_GET
    0,    // NA_BGM_TEMPLE_OF_TIME
    0x2,  // NA_BGM_EVENT_CLEAR
    0x11, // NA_BGM_KOKIRI
    0x2,  // NA_BGM_OCA_FAIRY_GET
    0x1,  // NA_BGM_SARIA_THEME
    0x1,  // NA_BGM_SPIRIT_TEMPLE
    0,    // NA_BGM_HORSE
    0,    // NA_BGM_HORSE_GOAL
    0,    // NA_BGM_INGO
    0x2,  // NA_BGM_MEDALLION_GET
    0x2,  // NA_BGM_OCA_SARIA
    0x2,  // NA_BGM_OCA_EPONA
    0x2,  // NA_BGM_OCA_ZELDA
    0x2,  // NA_BGM_OCA_SUNS
    0x2,  // NA_BGM_OCA_TIME
    0x2,  // NA_BGM_OCA_STORM
    0,    // NA_BGM_NAVI_OPENING
    0,    // NA_BGM_DEKU_TREE_CS
    0,    // NA_BGM_WINDMILL
    0,    // NA_BGM_HYRULE_CS
    0x20, // NA_BGM_MINI_GAME
    0,    // NA_BGM_SHEIK
    0x10, // NA_BGM_ZORA_DOMAIN
    0x2,  // NA_BGM_APPEAR
    0,    // NA_BGM_ADULT_LINK
    0,    // NA_BGM_MASTER_SWORD
    0x4,  // NA_BGM_INTRO_GANON
    0x20, // NA_BGM_SHOP
    0x40, // NA_BGM_CHAMBER_OF_SAGES
    0x40, // NA_BGM_FILE_SELECT
    0x1,  // NA_BGM_ICE_CAVERN
    0x2,  // NA_BGM_DOOR_OF_TIME
    0x2,  // NA_BGM_OWL
    0x1,  // NA_BGM_SHADOW_TEMPLE
    0x1,  // NA_BGM_WATER_TEMPLE
    0x2,  // NA_BGM_BRIDGE_TO_GANONS
    0,    // NA_BGM_OCARINA_OF_TIME
    0x11, // NA_BGM_GERUDO_VALLEY
    0,    // NA_BGM_POTION_SHOP
    0,    // NA_BGM_KOTAKE_KOUME
    0x80, // NA_BGM_ESCAPE
    0,    // NA_BGM_UNDERGROUND
    0x80, // NA_BGM_GANON_BATTLE_1
    0x80, // NA_BGM_GANON_BATTLE_2
    0,    // NA_BGM_END_DEMO
    0,    // NA_BGM_STAFF_1
    0,    // NA_BGM_STAFF_2
    0,    // NA_BGM_STAFF_3
    0,    // NA_BGM_STAFF_4
    0,    // NA_BGM_FIRE_BOSS
    0x8,  // NA_BGM_TIMED_MINI_GAME
    0,    // NA_BGM_VARIOUS_SFX
    1,    // NA_BGM_CUSTOM_SEQ
};

s8 sSpecReverbs[20] = { 0, 0, 0, 0, 0, 0, 0, 40, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

NatureAmbienceDataIO sNatureAmbienceDataIO[20] = {
    // NATURE_ID_GENERAL_NIGHT
    {
        0xC0FF, // PlayerIO Data
        0xC0FE, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CROWS_CAWS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_1_BEND_PITCH(0),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_ENTRANCE
    {
        0xC0FB, // PlayerIO Data
        0xC0FA, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_KAKARIKO_REGION
    {
        0xC001, // PlayerIO Data
        0x4000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(48),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_RUINS
    {
        0xC005, // PlayerIO Data
        0x4000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_HOWLING_WIND),
            NATURE_IO_STREAM_0_PORT3(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(48),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_KOKIRI_REGION
    {
        0xC01F, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(47),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_OWL_HOOT),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_CAWING_BIRD),
            NATURE_IO_CRITTER_1_BEND_PITCH(0),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(0),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(44),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(63),
            NATURE_IO_CRITTER_3_NUM_LAYERS(1),
            NATURE_IO_CRITTER_3_PORT5(44),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_NIGHT
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_06
    {
        0xC0FB, // PlayerIO Data
        0xC0FA, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_GANONS_LAIR
    {
        0x8001, // PlayerIO Data
        0x0,    // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_HOWLING_WIND),
            NATURE_IO_STREAM_0_PORT3(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_08
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_09
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_WASTELAND
    {
        0xC001, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_SCREECHING_WIND),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_COLOSSUS
    {
        0xC02F, // PlayerIO Data
        0xC02E, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_SCREECHING_WIND),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_CALL),
            NATURE_IO_CRITTER_1_BEND_PITCH(112),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(127),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(0),
            NATURE_IO_CRITTER_4_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_DEATH_MOUNTAIN_TRAIL
    {
        0xC07F, // PlayerIO Data
        0xC07E, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(112),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SONG),
            NATURE_IO_CRITTER_2_BEND_PITCH(127),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(16),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_LOUD_CHIRPING),
            NATURE_IO_CRITTER_3_BEND_PITCH(0),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(0),
            NATURE_IO_CRITTER_4_NUM_LAYERS(0),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(0),
            NATURE_IO_CRITTER_5_NUM_LAYERS(0),
            NATURE_IO_CRITTER_5_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0D
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0E
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0F
    {
        0xC01F, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_0_BEND_PITCH(80),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(8),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_1_BEND_PITCH(80),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_LOUD_CHIRPING),
            NATURE_IO_CRITTER_2_BEND_PITCH(0),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(0),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(96),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_10
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_11
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_12
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_NONE
    // While there is data for this natureAmbienceId, it is identical to previous entries
    // and the game treats it as no nature ambience
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },
};

u32 sOcarinaAllowedBtnMask = (BTN_A | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT);
s32 sOcarinaD5BtnMap = BTN_CUP;
s32 sOcarinaB4BtnMap = BTN_CLEFT;
s32 sOcarinaA4BtnMap = BTN_CRIGHT;
s32 sOcarinaF4BtnMap = BTN_CDOWN;
s32 sOcarinaD4BtnMap = BTN_A;
u8 sOcarinaInpEnabled = 0;
s8 D_80130F10 = 0; // "OCA", ocarina active?
u8 sCurOcarinaBtnVal = 0xFF;
u8 sPrevOcarinaNoteVal = 0;
u8 sCurOcarinaBtnIdx = 0; // note index?
u8 sLearnSongLastBtn = 0;
f32 D_80130F24 = 1.0f;
f32 D_80130F28 = 87.0f / 127.0f;
s8 D_80130F2C = 0; // pitch?
s8 D_80130F30 = 0x57;
s8 D_80130F34 = 0;
u8 sPlaybackState = 0; // 80130F38
u32 D_80130F3C = 0;    // "SEQ"
u32 sNotePlaybackTimer = 0;
u16 sPlaybackNotePos = 0;
u16 sStaffPlaybackPos = 0;
u16 D_80130F4C = 0;
u8 sDisplayedNoteValue = 0xFF; // Note to display on screen?
u8 sNotePlaybackVolume = 0;
u8 sNotePlaybackVibrato = 0;
s8 sNotePlaybackTone = 0;
f32 sNormalizedNotePlaybackTone = 1.0f;
f32 sNormalizedNotePlaybackVolume = 1.0f;
s32 D_80130F68 = 0;
u8 sOcarinaNoteValues[5] = { 2, 5, 9, 11, 14 };
u8 sOcaMinigameAppendPos = 0;
u8 sOcaMinigameEndPos = 0;
u8 sOcaMinigameNoteCnts[] = { 5, 6, 8 };

OcarinaNote sOcarinaSongs[OCARINA_SONG_MAX][20] = {
    // Minuet
    {
        { 2, 0, 18, 86, 0, 0, 0 },
        { 14, 0, 18, 92, 0, 0, 0 },
        { 11, 0, 72, 86, 0, 0, 0 },
        { 9, 0, 18, 80, 0, 0, 0 },
        { 11, 0, 18, 88, 0, 0, 0 },
        { 9, 0, 144, 86, 0, 0, 0 },
        { 0xFF, 0, 0, 86, 0, 0, 0 },
    },

    // Bolero
    {
        { 5, 0, 15, 80, 0, 0, 0 },
        { 2, 0, 15, 72, 0, 0, 0 },
        { 5, 0, 15, 84, 0, 0, 0 },
        { 2, 0, 15, 76, 0, 0, 0 },
        { 9, 0, 15, 84, 0, 0, 0 },
        { 5, 0, 15, 74, 0, 0, 0 },
        { 9, 0, 15, 78, 0, 0, 0 },
        { 5, 0, 135, 66, 0, 0, 0 },
        { 0xFF, 0, 0, 66, 0, 0, 0 },
    },

    // Serenade
    {
        { 2, 0, 36, 60, 0, 0, 0 },
        { 5, 0, 36, 78, 0, 0, 0 },
        { 9, 0, 33, 82, 0, 0, 0 },
        { 0xFF, 0, 3, 82, 0, 0, 0 },
        { 9, 0, 36, 84, 0, 0, 0 },
        { 11, 0, 144, 90, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Requiem
    {
        { 2, 0, 45, 88, 0, 0, 0 },
        { 5, 0, 23, 86, 0, 0, 0 },
        { 2, 0, 22, 84, 0, 0, 0 },
        { 9, 0, 45, 86, 0, 0, 0 },
        { 5, 0, 45, 94, 0, 0, 0 },
        { 2, 0, 180, 94, 0, 0, 0 },
        { 0xFF, 0, 0, 94, 0, 0, 0 },
    },

    // Nocturne
    {
        { 11, 0, 36, 88, 0, 0, 0 },
        { 9, 0, 33, 84, 0, 0, 0 },
        { 0xFF, 0, 3, 84, 0, 0, 0 },
        { 9, 0, 18, 82, 0, 0, 0 },
        { 2, 0, 18, 60, 0, 0, 0 },
        { 11, 0, 18, 90, 0, 0, 0 },
        { 9, 0, 18, 88, 0, 0, 0 },
        { 5, 0, 144, 96, 0, 0, 0 },
        { 0xFF, 0, 0, 96, 0, 0, 0 },
    },

    // Prelude
    {
        { 14, 0, 15, 84, 0, 0, 0 },
        { 9, 0, 45, 88, 0, 0, 0 },
        { 14, 0, 15, 88, 0, 0, 0 },
        { 9, 0, 15, 82, 0, 0, 0 },
        { 11, 0, 15, 86, 0, 0, 0 },
        { 14, 0, 60, 90, 0, 0, 0 },
        { 0xFF, 0, 75, 90, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Sarias
    {
        { 5, 0, 17, 84, 0, 0, 0 },
        { 9, 0, 17, 88, 0, 0, 0 },
        { 11, 0, 34, 80, 0, 0, 0 },
        { 5, 0, 17, 84, 0, 0, 0 },
        { 9, 0, 17, 88, 0, 0, 0 },
        { 11, 0, 136, 80, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Epona
    {
        { 14, 0, 18, 84, 0, 0, 0 },
        { 11, 0, 18, 88, 0, 0, 0 },
        { 9, 0, 72, 80, 0, 0, 0 },
        { 14, 0, 18, 84, 0, 0, 0 },
        { 11, 0, 18, 88, 0, 0, 0 },
        { 9, 0, 144, 80, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Lullaby
    {
        { 11, 0, 51, 84, 0, 0, 0 },
        { 14, 0, 25, 88, 0, 0, 0 },
        { 9, 0, 78, 80, 0, 0, 0 },
        { 11, 0, 51, 84, 0, 0, 0 },
        { 14, 0, 25, 88, 0, 0, 0 },
        { 9, 0, 100, 80, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Suns
    {
        { 9, 0, 12, 84, 0, 0, 0 },
        { 5, 0, 13, 88, 0, 0, 0 },
        { 14, 0, 29, 80, 2, 0, 0 },
        { 0xFF, 0, 9, 84, 0, 0, 0 },
        { 9, 0, 12, 84, 0, 0, 0 },
        { 5, 0, 13, 88, 0, 0, 0 },
        { 14, 0, 120, 80, 3, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Song of Time
    {
        { 9, 0, 32, 84, 0, 0, 0 },
        { 2, 0, 65, 88, 0, 0, 0 },
        { 5, 0, 33, 80, 0, 0, 0 },
        { 9, 0, 32, 84, 0, 0, 0 },
        { 2, 0, 65, 88, 0, 0, 0 },
        { 5, 0, 99, 80, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Storms
    {
        { 2, 0, 11, 84, 0, 0, 0 },
        { 5, 0, 11, 88, 0, 0, 0 },
        { 14, 0, 45, 80, 0, 0, 0 },
        { 2, 0, 11, 84, 0, 0, 0 },
        { 5, 0, 11, 88, 0, 0, 0 },
        { 14, 0, 90, 80, 0, 0, 0 },
        { 0xFF, 0, 0, 90, 0, 0, 0 },
    },

    // Scarecrow
    {
        { 2, 0, 3, 0, 0, 0, 0 },
        { 0xFF, 0, 0, 255, 0, 0, 0 },
    },

    // Lost Woods Memory Game
    {
        { 2, 0, 3, 0, 0, 0, 0 },
        { 0xFF, 0, 0, 0, 0, 0, 0 },
    },
};

OcarinaNote* sPlaybackSong = sOcarinaSongs[0];
u8 sFrogsSongNotes[14] = {
    OCARINA_NOTE_D4, OCARINA_NOTE_B4, OCARINA_NOTE_A4, OCARINA_NOTE_F4, OCARINA_NOTE_B4,
    OCARINA_NOTE_A4, OCARINA_NOTE_F4, OCARINA_NOTE_D4, OCARINA_NOTE_F4, OCARINA_NOTE_D4,
    OCARINA_NOTE_F4, OCARINA_NOTE_A4, OCARINA_NOTE_B4, OCARINA_NOTE_D4,
};
u8* gFrogsSongPtr = sFrogsSongNotes;
u8 sRecordingState = 0;
u8 sRecordSongPos = 0;
u32 D_80131860 = 0;
u8 D_80131864 = 0;
u8 D_80131868 = 0;
u8 D_8013186C = 0;
s8 D_80131870 = 0;
u8 D_80131874 = 0;
u8 D_80131878 = 0;
u8 D_8013187C = 0;
u8 sOcarinaDropInputTimer = 0;

OcarinaNote sPierresSong[108] = {
    { 0xFF, 0, 0, 0, 0, 0, 0 },
    { 0xFF, 0, 0, 0, 0, 0, 0 },
};
OcarinaNote* gScarecrowCustomSongPtr = sPierresSong;

u8* gScarecrowSpawnSongPtr = (u8*)&sOcarinaSongs[OCARINA_SONG_SCARECROW];
OcarinaNote* D_80131BEC = sOcarinaSongs[OCARINA_SONG_MEMORY_GAME];
u8 sNoteValueIndexMap[16] = { 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 5, 3, 3, 4, 4, 4 };

OcarinaSongInfo gOcarinaSongNotes[OCARINA_SONG_MAX] = {
    // Minuet
    { 6,
      {
          OCARINA_NOTE_D4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
      } },
    // Bolero
    { 8,
      {
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
      } },
    // Serenade
    { 5,
      {
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
      } },
    // Requiem
    { 6,
      {
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D4,
      } },
    // Nocturne
    { 7,
      {
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
      } },
    // Prelude
    { 6,
      {
          OCARINA_NOTE_D5,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_D5,
      } },
    // Sarias
    { 6,
      {
          OCARINA_NOTE_F4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
      } },
    // Epona
    { 6,
      {
          OCARINA_NOTE_D5,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_A4,
      } },
    // Lullaby
    { 6,
      {
          OCARINA_NOTE_B4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_B4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_A4,
      } },
    // Suns
    { 6,
      {
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D5,
      } },
    // Song of Time
    { 6,
      {
          OCARINA_NOTE_A4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_A4,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
      } },
    // Storms
    { 6,
      {
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D5,
          OCARINA_NOTE_D4,
          OCARINA_NOTE_F4,
          OCARINA_NOTE_D5,
      } },
    // Scarecrow
    { 8, { 0, 0, 0, 0, 0, 0, 0, 0 } },
    // Lost Woods Memory Game
    { 0, { 0, 0, 0, 0, 0, 0, 0, 0 } },
};
// clang-format on

/**
 * BSS
 */
u32 sAudioUpdateStartTime; // 8016B7A0
u32 sAudioUpdateEndTime;
f32 D_8016B7A8;
f32 D_8016B7AC;
f32 D_8016B7B0;
f32 D_8016B7B4;
FreqLerp sRiverFreqScaleLerp;
FreqLerp sWaterfallFreqScaleLerp;
f32 D_8016B7D8;
s8 D_8016B7DC;
f32 D_8016B7E0;
u16 D_8016B7E4;
struct {
    s8 str[5];
    u16 num;
} sAudioScrPrtBuf[SCROLL_PRINT_BUF_SIZE];
u8 D_8016B8B0;
u8 D_8016B8B1;
u8 D_8016B8B2;
u8 D_8016B8B3;
u8 sAudioGanonDistVol;
SfxPlayerState sSfxChannelState[0x10];

char sBinToStrBuf[0x20];
u8 D_8016B9D8;
u8 sAudioSpecPeakNumNotes[0x12];
u8 D_8016B9F2;
u8 D_8016B9F3;
u8 D_8016B9F4;
u16 D_8016B9F6;

OcarinaStaff sPlayingStaff;
OcarinaStaff sDisplayedStaff;
OcarinaStaff sRecordingStaff;
u32 D_8016BA04;
typedef struct {
    s8 x;
    s8 y;
} OcarinaStick;
OcarinaStick sCurOcaStick;
u32 sCurOcarinaBtnPress;
u32 D_8016BA10;
u32 sPrevOcarinaBtnPress;
s32 D_8016BA18;
s32 D_8016BA1C;
u8 sCurOcarinaSong[8];
u8 sOcarinaSongAppendPos;
u8 sOcarinaHasStartedSong;
u8 sOcarinaSongNoteStartIdx;
u8 sOcarinaSongCnt;
u16 sOcarinaAvailSongs;
u8 sStaffPlayingPos;
u16 sLearnSongPos[0x10];
u16 D_8016BA50[0x10];
u16 D_8016BA70[0x10];
u8 sLearnSongExpectedNote[0x10];
OcarinaNote D_8016BAA0;
u8 sAudioHasMalonBgm;
f32 sAudioMalonBgmDist;

// Start debug bss
u32 sDebugPadHold;
u32 sDebugPadBtnLast;
u32 sDebugPadPress;
s32 sAudioUpdateTaskStart;
s32 sAudioUpdateTaskEnd;

void PadMgr_RequestPadData(PadMgr* padmgr, Input* inputs, s32 mode);

void Audio_StepFreqLerp(FreqLerp* lerp);
void func_800F56A8(void);
void Audio_PlayNatureAmbienceSequence(u8 natureAmbienceId);
s32 Audio_SetGanonDistVol(u8 targetVol);
void Audio_PlayFanfare_Rando(GetItemEntry getItem);

// Right stick as virtual C buttons
#define RSTICK_UP    0x100000
#define RSTICK_DOWN  0x200000
#define RSTICK_LEFT  0x400000
#define RSTICK_RIGHT 0x800000

// Function originally not called, so repurposing for control mapping
void Audio_OcaUpdateBtnMap(bool customControls, bool dpad, bool rStick) {
    if (customControls) {
        sOcarinaD5BtnMap = CVarGetInteger("gOcarinaD5BtnMap", BTN_CUP);
        sOcarinaB4BtnMap = CVarGetInteger("gOcarinaB4BtnMap", BTN_CLEFT);
        sOcarinaA4BtnMap = CVarGetInteger("gOcarinaA4BtnMap", BTN_CRIGHT);
        sOcarinaF4BtnMap = CVarGetInteger("gOcarinaF4BtnMap", BTN_CDOWN);
        sOcarinaD4BtnMap = CVarGetInteger("gOcarinaD4BtnMap", BTN_A);
    } else {
        sOcarinaD5BtnMap = BTN_CUP;
        sOcarinaB4BtnMap = BTN_CLEFT;
        sOcarinaA4BtnMap = BTN_CRIGHT;
        sOcarinaF4BtnMap = BTN_CDOWN;
        sOcarinaD4BtnMap = BTN_A;
    }

    if (dpad) {
        sOcarinaD5BtnMap |= BTN_DUP;
        sOcarinaB4BtnMap |= BTN_DLEFT;
        sOcarinaA4BtnMap |= BTN_DRIGHT;
        sOcarinaF4BtnMap |= BTN_DDOWN;
    }

    if (rStick) {
        sOcarinaD5BtnMap |= RSTICK_UP;
        sOcarinaB4BtnMap |= RSTICK_LEFT;
        sOcarinaA4BtnMap |= RSTICK_RIGHT;
        sOcarinaF4BtnMap |= RSTICK_DOWN;
    }

    sOcarinaAllowedBtnMask = (
        sOcarinaD5BtnMap |
        sOcarinaB4BtnMap |
        sOcarinaA4BtnMap |
        sOcarinaF4BtnMap |
        sOcarinaD4BtnMap
    );
}

void Audio_GetOcaInput(void) {
    Input inputs[4];
    Input* input = &inputs[0];
    u32 sp18;

    sp18 = sCurOcarinaBtnPress;
    PadMgr_RequestPadData(&gPadMgr, inputs, 0);
    sCurOcarinaBtnPress = input->cur.button;
    sPrevOcarinaBtnPress = sp18;
    sCurOcaStick.x = input->rel.stick_x;
    sCurOcaStick.y = input->rel.stick_y;

    s8 rstick_x = input->cur.right_stick_x;
    s8 rstick_y = input->cur.right_stick_y;
    const s8 sensitivity = 64;
    if (rstick_x > sensitivity) {
        sCurOcarinaBtnPress |= RSTICK_RIGHT;
    }
    if (rstick_x < -sensitivity) {
        sCurOcarinaBtnPress |= RSTICK_LEFT;
    }
    if (rstick_y > sensitivity) {
        sCurOcarinaBtnPress |= RSTICK_UP;
    }
    if (rstick_y < -sensitivity) {
        sCurOcarinaBtnPress |= RSTICK_DOWN;
    }
}

f32 Audio_OcaAdjStick(s8 inp) {
    s8 inpAdj;
    f32 ret;

    if (inp > 0x40) {
        inpAdj = 127;
    } else if (inp < -0x40) {
        inpAdj = -128;
    } else if (inp >= 0) {
        inpAdj = (inp * 127) / 64;
    } else {
        inpAdj = (inp * 128) / 64;
    }
    ret = gBendPitchTwoSemitonesFrequencies[inpAdj + 128];
    return ret;
}

u8 Audio_OcaGetPlayingState(void) {
    u8 ret;

    if (D_80131878 != 0) {
        ret = D_80131878 - 1;
        D_80131878 = 0;
    } else if (D_80130F3C != 0) {
        ret = 0xFE;
    } else {
        ret = 0xFF;
    }

    return ret;
}

u8 Audio_OcaMapNoteValue(u8 arg0) {
    u8 temp_v1;

    temp_v1 = sNoteValueIndexMap[arg0 & 0x3F];
    if (temp_v1 == 5) {
        if (arg0 & 0x80) {
            return 2;
        }
        return 3;
    }
    return temp_v1;
}

void func_800ECB7C(u8 songIdx) {
    u8 savedSongIdx;
    u8 scarecrowSongIdx;
    u8 noteIdx;

    savedSongIdx = 0;
    scarecrowSongIdx = 0;
    while (savedSongIdx < 8 && scarecrowSongIdx < 0x10) {
        noteIdx = sOcarinaSongs[songIdx][scarecrowSongIdx++].noteIdx;
        if (noteIdx != 0xFF) {
            gOcarinaSongNotes[OCARINA_SONG_SCARECROW].notesIdx[savedSongIdx++] = sNoteValueIndexMap[noteIdx];
        }
    }
}

// start ocarina.
void func_800ECC04(u16 flg) {
    u8 i;

    if ((sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume != 0xFF) && ((flg & 0xFFF) == 0xFFF)) {
        flg |= 0x1000;
    }

    if ((flg == 0xCFFF) && (sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume != 0xFF)) {
        flg = 0xDFFF;
    }

    if ((flg == 0xFFF) && (sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume != 0xFF)) {
        flg = 0x1FFF;
    }

    if (flg != 0xFFFF) {
        D_80130F3C = 0x80000000 + (u32)flg;
        sOcarinaSongNoteStartIdx = 0;
        sOcarinaSongCnt = 0xE;
        if (flg != 0xA000) {
            sOcarinaSongCnt--;
        }
        sOcarinaAvailSongs = flg & 0x3FFF;
        D_8013187C = 8;
        sOcarinaHasStartedSong = 0;
        D_80131878 = 0;
        sStaffPlayingPos = 0;
        sPlayingStaff.state = Audio_OcaGetPlayingState();
        sOcarinaInpEnabled = 1;
        D_80130F4C = 0;
        for (i = 0; i < 0xE; i++) {
            sLearnSongPos[i] = 0;
            D_8016BA50[i] = 0;
            D_8016BA70[i] = 0;
            sLearnSongExpectedNote[i] = 0;
        }

        if (flg & 0x8000) {
            D_8013187C = 0;
        }

        if (flg & 0x4000) {
            sOcarinaSongAppendPos = 0;
        }

        if (flg & 0xD000) {
            func_800ECB7C(OCARINA_SONG_SCARECROW);
        }
    } else {
        D_80130F3C = 0;
        sOcarinaInpEnabled = 0;
    }
}

void func_800ECDBC(void) {
    if (sCurOcarinaBtnVal != 0xFF && sOcarinaHasStartedSong == 0) {
        sOcarinaHasStartedSong = 1;
        sLearnSongLastBtn = 0xFF;
    }
}

void func_800ECDF8(void) {
    u16 sh;
    u16 pad;
    u8 inputChanged = 0;
    u16 pad2;
    s8 sp57 = 0;
    u8 i;
    OcarinaNote* prevNote;
    OcarinaNote* note;

    func_800ECDBC();

    if (sOcarinaHasStartedSong) {
        if ((D_80130F2C < 0 ? -D_80130F2C : D_80130F2C) >= 0x15) {
            D_80130F3C = 0;
            return;
        }

        // clang-format off
        if (sPrevOcarinaNoteVal == sCurOcarinaBtnVal || sCurOcarinaBtnVal == 0xFF) { inputChanged = 1; }
        // clang-format on

        for (i = sOcarinaSongNoteStartIdx; i < sOcarinaSongCnt; i++) {
            sh = 1 << i;
            if (sOcarinaAvailSongs & sh) {
                D_8016BA50[i] = D_8016BA70[i] + 0x12;
                if (inputChanged) {
                    // (pointless if check, this is always true)
                    if ((D_8016BA50[i] >= D_8016BA70[i] - 0x12) && (D_8016BA50[i] >= D_8016BA70[i] + 0x12) &&
                        (sOcarinaSongs[i][sLearnSongPos[i]].unk_02 == 0) &&
                        (sLearnSongLastBtn == sLearnSongExpectedNote[i])) {
                        D_80131878 = i + 1;
                        sOcarinaInpEnabled = 0;
                        D_80130F3C = 0;
                    }
                } else if (D_8016BA50[i] >= (D_8016BA70[i] - 0x12)) {
                    if (sLearnSongLastBtn != 0xFF) {
                        if (sLearnSongLastBtn == sLearnSongExpectedNote[i]) {
                            if (i == 12) {
                                D_8016BA50[i] = 0;
                            }
                        } else {
                            sOcarinaAvailSongs ^= sh;
                        }
                    }

                    prevNote = &sOcarinaSongs[i][sLearnSongPos[i]];
                    note = &sOcarinaSongs[i][++sLearnSongPos[i]];
                    D_8016BA70[i] = prevNote->unk_02;
                    sLearnSongExpectedNote[i] = prevNote->noteIdx;

                    if (sCurOcarinaBtnVal != sLearnSongExpectedNote[i]) {
                        sOcarinaAvailSongs ^= sh;
                    }
                    while (prevNote->noteIdx == note->noteIdx ||
                           (note->noteIdx == OCARINA_NOTE_INVALID && note->unk_02 != 0)) {
                        D_8016BA70[i] += note->unk_02;
                        prevNote = &sOcarinaSongs[i][sLearnSongPos[i]];
                        note = &sOcarinaSongs[i][sLearnSongPos[i] + 1];
                        sLearnSongPos[i]++;
                    }
                } else if (D_8016BA50[i] < 0xA) {
                    sp57 = -1;
                    D_8016BA50[i] = 0;
                    sLearnSongLastBtn = sCurOcarinaBtnVal;
                } else {
                    sOcarinaAvailSongs ^= sh;
                }
            }

            if (sOcarinaAvailSongs == 0 && sStaffPlayingPos >= D_8013187C) {
                sOcarinaInpEnabled = 0;
                if (CHECK_BTN_ANY(D_80130F3C, BTN_B) && sCurOcarinaBtnVal == sOcarinaSongs[i][0].noteIdx) {
                    D_80130F4C = D_80130F3C;
                }
                D_80130F3C = 0;
                return;
            }
        }

        if (!inputChanged) {
            sLearnSongLastBtn = sCurOcarinaBtnVal;
            sStaffPlayingPos += sp57 + 1;
        }
    }
}

void func_800ED200(void) {
    u32 temp_v0;
    u8 i;
    u8 j;
    u8 k;

    u32 disableSongBtnMap;
    if (CVarGetInteger("gCustomOcarinaControls", 0)) {
        disableSongBtnMap = CVarGetInteger("gOcarinaDisableBtnMap", BTN_L);
    } else {
        disableSongBtnMap = BTN_L;
    }

    if (
        CHECK_BTN_ANY(sCurOcarinaBtnPress, disableSongBtnMap)
        && CHECK_BTN_ANY(sCurOcarinaBtnPress, sOcarinaAllowedBtnMask)
    ) {
        func_800ECC04((u16)D_80130F3C);
        return;
    }

    func_800ECDBC();

    if (sOcarinaHasStartedSong) {
        if ((sPrevOcarinaNoteVal != sCurOcarinaBtnVal) && (sCurOcarinaBtnVal != 0xFF)) {
            sStaffPlayingPos++;
            if (sStaffPlayingPos >= 9) {
                sStaffPlayingPos = 1;
            }

            if (sOcarinaSongAppendPos == 8) {
                for (i = 0; i < 7; i++) {
                    sCurOcarinaSong[i] = sCurOcarinaSong[i + 1];
                }
            } else {
                sOcarinaSongAppendPos++;
            }

            if ((D_80130F2C < 0 ? -D_80130F2C : D_80130F2C) >= 0x15) {
                sCurOcarinaSong[sOcarinaSongAppendPos - 1] = 0xFF;
            } else {
                sCurOcarinaSong[sOcarinaSongAppendPos - 1] = sCurOcarinaBtnVal;
            }

            for (i = sOcarinaSongNoteStartIdx; i < sOcarinaSongCnt; i++) {
                if (sOcarinaAvailSongs & (u16)(1 << i)) {
                    for (j = 0, k = 0;
                         j < gOcarinaSongNotes[i].len && k == 0 && sOcarinaSongAppendPos >= gOcarinaSongNotes[i].len;) {
                        temp_v0 = sCurOcarinaSong[(sOcarinaSongAppendPos - gOcarinaSongNotes[i].len) + j];
                        if (temp_v0 == sOcarinaNoteValues[gOcarinaSongNotes[i].notesIdx[j]]) {
                            j++;
                        } else {
                            k++;
                        }
                    }

                    if (j == gOcarinaSongNotes[i].len) {
                        D_80131878 = i + 1;
                        sOcarinaInpEnabled = 0;
                        D_80130F3C = 0;
                    }
                }
            }
        }
    }
}

void func_800ED458(s32 arg0) {
    u32 phi_v1_2;
    bool customControls = CVarGetInteger("gCustomOcarinaControls", 0);
    bool dpad = CVarGetInteger("gDpadOcarina", 0);
    bool rStick = CVarGetInteger("gRStickOcarina", 0);

    if (D_80130F3C != 0 && sOcarinaDropInputTimer != 0) {
        sOcarinaDropInputTimer--;
        if (!CVarGetInteger("gDpadNoDropOcarinaInput", 0)) {
            return;
        }
    }

    if ((D_8016BA10 == 0) ||
        ((D_8016BA10 & sOcarinaAllowedBtnMask) != (sCurOcarinaBtnPress & sOcarinaAllowedBtnMask))) {
        D_8016BA10 = 0;
        sCurOcarinaBtnVal = 0xFF;
        sCurOcarinaBtnIdx = 0xFF;
        phi_v1_2 = (sCurOcarinaBtnPress & sOcarinaAllowedBtnMask) & (sPrevOcarinaBtnPress & sOcarinaAllowedBtnMask);
        if (!(D_8016BA18 & phi_v1_2) && (sCurOcarinaBtnPress != 0)) {
            D_8016BA18 = sCurOcarinaBtnPress;
        } else {
            D_8016BA18 &= phi_v1_2;
        }

        Audio_OcaUpdateBtnMap(customControls, dpad, rStick);
        if (D_8016BA18 & sOcarinaD4BtnMap) {
            osSyncPrintf("Presss NA_KEY_D4 %08x\n", sOcarinaD4BtnMap);
            sCurOcarinaBtnVal = 2;
            sCurOcarinaBtnIdx = 0;
        } else if (D_8016BA18 & sOcarinaF4BtnMap) {
            osSyncPrintf("Presss NA_KEY_F4 %08x\n", sOcarinaF4BtnMap);
            sCurOcarinaBtnVal = 5;
            sCurOcarinaBtnIdx = 1;
        } else if (D_8016BA18 & sOcarinaA4BtnMap) {
            osSyncPrintf("Presss NA_KEY_A4 %08x\n", sOcarinaA4BtnMap);
            sCurOcarinaBtnVal = 9;
            sCurOcarinaBtnIdx = 2;
        } else if (D_8016BA18 & sOcarinaB4BtnMap) {
            osSyncPrintf("Presss NA_KEY_B4 %08x\n", sOcarinaA4BtnMap);
            sCurOcarinaBtnVal = 0xB;
            sCurOcarinaBtnIdx = 3;
        } else if (D_8016BA18 & sOcarinaD5BtnMap) {
            osSyncPrintf("Presss NA_KEY_D5 %08x\n", sOcarinaD5BtnMap);
            sCurOcarinaBtnVal = 0xE;
            sCurOcarinaBtnIdx = 4;
        }

        u32 noteSharpBtnMap;
        if (customControls) {
            noteSharpBtnMap = CVarGetInteger("gOcarinaSharpBtnMap", BTN_R);
        } else {
            noteSharpBtnMap = BTN_R;
        }
        if ((sCurOcarinaBtnVal != 0xFF) && (sCurOcarinaBtnPress & noteSharpBtnMap) && (sRecordingState != 2)) {
            sCurOcarinaBtnIdx += 0x80;
            sCurOcarinaBtnVal++;
        }

        u32 noteFlatBtnMap;
        if (customControls) {
            noteFlatBtnMap = CVarGetInteger("gOcarinaFlatBtnMap", BTN_Z);
        } else {
            noteFlatBtnMap = BTN_Z;
        }
        if ((sCurOcarinaBtnVal != 0xFF) && (sCurOcarinaBtnPress & noteFlatBtnMap) && (sRecordingState != 2)) {
            sCurOcarinaBtnIdx += 0x40;
            sCurOcarinaBtnVal--;
        }

        if (sRecordingState != 2) {
            D_80130F2C = sCurOcaStick.y;
            D_80130F24 = Audio_OcaAdjStick(D_80130F2C);

            D_80130F34 = (sCurOcaStick.x < 0 ? -sCurOcaStick.x : sCurOcaStick.x) >> 2;
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD06, D_80130F34);
        } else {
            D_80130F2C = 0;
            D_80130F24 = 1.0f;
        }

        if ((sCurOcarinaBtnVal != 0xFF) && (sPrevOcarinaNoteVal != sCurOcarinaBtnVal)) {
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD07, D_80130F10 - 1);
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD05, sCurOcarinaBtnVal);
            Audio_PlaySoundGeneral(NA_SE_OC_OCARINA, &D_801333D4, 4, &D_80130F24, &D_80130F28, &D_801333E8);
        } else if ((sPrevOcarinaNoteVal != 0xFF) && (sCurOcarinaBtnVal == 0xFF)) {
            Audio_StopSfxById(NA_SE_OC_OCARINA);
        }
    }
}

void func_800ED848(u8 inputEnabled) {
    sOcarinaInpEnabled = inputEnabled;
}

void Audio_OcaSetInstrument(u8 arg0) {
    if (D_80130F10 == arg0) {
        return;
    }

    u16 sfxEditorId = arg0 + 0x81;
    u16 newArg0 = AudioEditor_GetReplacementSeq(sfxEditorId);
    if (newArg0 != sfxEditorId) {
        gAudioContext.seqReplaced[SEQ_PLAYER_SFX] = 1;
        arg0 = newArg0 - 0x81;
    }

    Audio_SeqCmd8(SEQ_PLAYER_SFX, 1, SFX_PLAYER_CHANNEL_OCARINA, arg0);
    D_80130F10 = arg0;
    if (arg0 == 0) {
        sCurOcarinaBtnPress = 0;
        sPrevOcarinaBtnPress = 0;
        D_8016BA18 = 0;
        D_8016BA10 = 0xFFFF;
        func_800ED458(0);
        Audio_StopSfxById(NA_SE_OC_OCARINA);
        Audio_SetSoundBanksMute(0);
        sPlaybackState = 0;
        sStaffPlaybackPos = 0;
        sOcarinaInpEnabled = 0;
        D_80130F3C = 0;
        Audio_ClearBGMMute(SFX_PLAYER_CHANNEL_OCARINA);
    } else {
        sCurOcarinaBtnPress = 0;
        Audio_GetOcaInput();
        D_8016BA10 = sCurOcarinaBtnPress;
        Audio_QueueSeqCmdMute(SFX_PLAYER_CHANNEL_OCARINA);
    }
}

void Audio_OcaSetSongPlayback(s8 songIdxPlusOne, s8 playbackState) {
    if (songIdxPlusOne == 0) {
        sPlaybackState = 0;
        Audio_StopSfxById(NA_SE_OC_OCARINA);
        return;
    }

    if (songIdxPlusOne < 0xF) {
        sPlaybackSong = sOcarinaSongs[songIdxPlusOne - 1];
    } else {
        sPlaybackSong = sPierresSong;
    }

    sPlaybackState = playbackState;
    sNotePlaybackTimer = 0;
    sDisplayedNoteValue = 0xFF;
    sPlaybackNotePos = 0;
    sStaffPlaybackPos = 0;
    while (sPlaybackSong[sPlaybackNotePos].noteIdx == OCARINA_NOTE_INVALID) {
        sPlaybackNotePos++;
    }
}

void Audio_OcaPlayback(void) {
    u32 noteTimerStep;
    u32 nextNoteTimerStep = 0;

    if (sPlaybackState != 0) {
        if (sStaffPlaybackPos == 0) {
            noteTimerStep = 3;
        } else {
            noteTimerStep = D_8016BA04 - D_80130F68;
        }

        if (noteTimerStep < sNotePlaybackTimer) {
            sNotePlaybackTimer -= noteTimerStep;
        } else {
            nextNoteTimerStep = noteTimerStep - sNotePlaybackTimer;
            sNotePlaybackTimer = 0;
        }

        if (sNotePlaybackTimer == 0) {

            sNotePlaybackTimer = sPlaybackSong[sPlaybackNotePos].unk_02;

            if (sPlaybackNotePos == 1) {
                sNotePlaybackTimer++;
            }

            if (sNotePlaybackTimer == 0) {
                sPlaybackState--;
                if (sPlaybackState != 0) {
                    sPlaybackNotePos = 0;
                    sStaffPlaybackPos = 0;
                    sDisplayedNoteValue = 0xFF;
                } else {
                    Audio_StopSfxById(NA_SE_OC_OCARINA);
                }
                return;
            } else {
                sNotePlaybackTimer -= nextNoteTimerStep;
            }

            if (sNotePlaybackVolume != sPlaybackSong[sPlaybackNotePos].volume) {
                sNotePlaybackVolume = sPlaybackSong[sPlaybackNotePos].volume;
                sNormalizedNotePlaybackVolume = sNotePlaybackVolume / 127.0f;
            }

            if (sNotePlaybackVibrato != sPlaybackSong[sPlaybackNotePos].vibrato) {
                sNotePlaybackVibrato = sPlaybackSong[sPlaybackNotePos].vibrato;
                Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD06, sNotePlaybackVibrato);
            }

            if (sNotePlaybackTone != sPlaybackSong[sPlaybackNotePos].tone) {
                sNotePlaybackTone = sPlaybackSong[sPlaybackNotePos].tone;
                sNormalizedNotePlaybackTone = Audio_OcaAdjStick(sNotePlaybackTone);
            }

            if ((sPlaybackSong[sPlaybackNotePos].volume == sPlaybackSong[sPlaybackNotePos - 1].volume &&
                 (sPlaybackSong[sPlaybackNotePos].vibrato == sPlaybackSong[sPlaybackNotePos - 1].vibrato) &&
                 (sPlaybackSong[sPlaybackNotePos].tone == sPlaybackSong[sPlaybackNotePos - 1].tone))) {
                sDisplayedNoteValue = 0xFE;
            }

            if (sDisplayedNoteValue != sPlaybackSong[sPlaybackNotePos].noteIdx) {
                u8 tmp = sPlaybackSong[sPlaybackNotePos].noteIdx;

                if (tmp == 0xA) {
                    sDisplayedNoteValue = tmp + sPlaybackSong[sPlaybackNotePos].semitone;
                } else {
                    sDisplayedNoteValue = tmp;
                }

                if (sDisplayedNoteValue != 0xFF) {
                    sStaffPlaybackPos++;
                    Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD07, D_80130F10 - 1);
                    Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD05, sDisplayedNoteValue & 0x3F);
                    Audio_PlaySoundGeneral(NA_SE_OC_OCARINA, &D_801333D4, 4, &sNormalizedNotePlaybackTone,
                                           &sNormalizedNotePlaybackVolume, &D_801333E8);
                } else {
                    Audio_StopSfxById(NA_SE_OC_OCARINA);
                }
            }
            sPlaybackNotePos++;
        }
    }
}

void func_800EDD68(u8 arg0) {
    u16 i;
    u16 i2;
    u16 pad;
    u8 lastNote;
    OcarinaNote* note;
    u8 j;
    u8 k;
    s32 t;
    OcarinaNote* song;

    if (sRecordingState == 1) {
        song = gScarecrowCustomSongPtr;
    } else {
        song = D_80131BEC;
    }
    song[sRecordSongPos].noteIdx = D_80131864;
    song[sRecordSongPos].unk_02 = D_8016BA04 - D_80131860;
    song[sRecordSongPos].volume = D_80131868;
    song[sRecordSongPos].vibrato = D_8013186C;
    song[sRecordSongPos].tone = D_80131870;
    song[sRecordSongPos].semitone = D_80131874 & 0xC0;
    D_80131864 = sCurOcarinaBtnVal;
    D_80131868 = D_80130F30;
    D_8013186C = D_80130F34;
    D_80131870 = D_80130F2C;
    D_80131874 = sCurOcarinaBtnIdx;
    sRecordSongPos++;

    if ((sRecordSongPos != 107) && (arg0 == 0)) {
        return;
    }

    i = sRecordSongPos;
    lastNote = 0xFF;
    while (i != 0 && lastNote == 0xFF) {
        i--;
        lastNote = song[i].noteIdx;
    }


    if (sRecordSongPos != (i + 1)) {
        sRecordSongPos = i + 2;
        song[sRecordSongPos - 1].unk_02 = 0;
    }

    song[sRecordSongPos].unk_02 = 0;

    if (sRecordingState == 2) {
        if (sStaffPlayingPos >= 8) {
            for (i = 0; i < sRecordSongPos; i++) {
                song[i] = song[i + 1];
            }

            func_800ECB7C(OCARINA_SONG_MEMORY_GAME);

            for (i = 0; i < OCARINA_SONG_SCARECROW; i++) {
                for (j = 0; j < 9 - gOcarinaSongNotes[i].len; j++) {
                    for (k = 0;
                         k < gOcarinaSongNotes[i].len && k + j < 8 &&
                         gOcarinaSongNotes[i].notesIdx[k] == gOcarinaSongNotes[OCARINA_SONG_SCARECROW].notesIdx[k + j];
                         k++) {
                        ;
                    }

                    if (k == gOcarinaSongNotes[i].len) {
                        sRecordingState = 0xFF;
                        sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume = 0xFF;
                        return;
                    }
                }
            }

            i = 1;
            while (i < 8) {
                if (gOcarinaSongNotes[OCARINA_SONG_SCARECROW].notesIdx[0] !=
                    gOcarinaSongNotes[OCARINA_SONG_SCARECROW].notesIdx[i]) {
                    i = 9;
                } else {
                    i++;
                }
            }

            if (i == 8) {
                sRecordingState = 0xFF;
                sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume = 0xFF;
                return;
            }

            for (i = 0; i < sRecordSongPos; i++) {
                sOcarinaSongs[OCARINA_SONG_SCARECROW][i] = sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][i];
            }

            sOcarinaInpEnabled = 0;
        } else {
            sOcarinaSongs[OCARINA_SONG_SCARECROW][1].volume = 0xFF;
        }
    }
    sRecordingState = 0;
}

// start custom song?
/**
 * recordingState = 1, start long scarecrows song
 * recordingState = 0, end
 * recordingState = 2, also scarecrows song
 */
void Audio_OcaSetRecordingState(u8 recordingState) {
    if ((u32)recordingState == sRecordingState) {
        return;
    }

    if (recordingState != 0) {
        D_80131860 = D_8016BA04;
        D_80131864 = 0xFF;
        D_80131868 = 0x57;
        D_8013186C = 0;
        D_80131870 = 0;
        D_80131874 = 0;
        sRecordSongPos = 0;
        sOcarinaInpEnabled = 1;
        sStaffPlayingPos = 0;
        D_8016BAA0 = sPierresSong[1];
    } else {
        if (sRecordSongPos == 0) {
            sPierresSong[1] = D_8016BAA0;
        } else {
            if (sRecordingState == 2) {
                sStaffPlayingPos = 1;
            }

            func_800EDD68(1);
        }

        sOcarinaInpEnabled = 0;
        sStaffPlayingPos = 0;
    }

    sRecordingState = recordingState;
}

void Audio_OcaUpdateRecordingStaff(void) {
    sRecordingStaff.state = sRecordingState;
    sRecordingStaff.pos = sStaffPlayingPos;
    if (sRecordingState == 0xFF) {
        sRecordingState = 0;
    }
}

void Audio_OcaUpdatePlayingStaff(void) {
    sPlayingStaff.noteIdx = sCurOcarinaBtnIdx & 0x3F;
    sPlayingStaff.state = Audio_OcaGetPlayingState();
    sPlayingStaff.pos = sStaffPlayingPos;
}

void Audio_OcaUpdateDisplayedStaff(void) {
    if ((sDisplayedNoteValue & 0x3F) < 0x10) {
        sDisplayedStaff.noteIdx = Audio_OcaMapNoteValue(sDisplayedNoteValue);
    }

    sDisplayedStaff.state = sPlaybackState;

    if (sPlaybackSong != sPierresSong) {
        sDisplayedStaff.pos = sStaffPlaybackPos;
    } else if (sStaffPlaybackPos == 0) {
        sDisplayedStaff.pos = 0;
    } else {
        sDisplayedStaff.pos = ((sStaffPlaybackPos - 1) % 8) + 1;
    }
}

OcarinaStaff* Audio_OcaGetRecordingStaff(void) {
    return &sRecordingStaff;
}

OcarinaStaff* Audio_OcaGetPlayingStaff(void) {
    if (sPlayingStaff.state < 0xFE) {
        D_80130F3C = 0;
    }
    return &sPlayingStaff;
}

OcarinaStaff* Audio_OcaGetDisplayingStaff(void) {
    return &sDisplayedStaff;
}

void func_800EE404(void) {
    s32 noteChanged;

    if ((sRecordingState != 0) && ((D_8016BA04 - D_80131860) >= 3)) {
        noteChanged = false;
        if (D_80131864 != sCurOcarinaBtnVal) {
            if (sCurOcarinaBtnVal != 0xFF) {
                sRecordingStaff.noteIdx = sCurOcarinaBtnIdx & 0x3F;
                sStaffPlayingPos++;
            } else if ((sRecordingState == 2) && (sStaffPlayingPos == 8)) {
                func_800EDD68(1);
                return;
            }

            if (sStaffPlayingPos > 8) {
                if (sRecordingState == 2) {
                    // notes played are over 8 and in recording mode.
                    func_800EDD68(1);
                    return;
                }
                sStaffPlayingPos = true;
            }

            noteChanged = true;
        } else if (D_80131868 != D_80130F30) {
            noteChanged = true;
        } else if (D_8013186C != D_80130F34) {
            noteChanged = true;
        } else if (D_80131870 != D_80130F2C) {
            noteChanged = true;
        }

        if (noteChanged) {
            func_800EDD68(0);
            D_80131860 = D_8016BA04;
        }
    }
}

void Audio_OcaMemoryGameStart(u8 minigameRound) {
    u8 i;

    if (minigameRound > 2) {
        minigameRound = 2;
    }

    sOcaMinigameAppendPos = 0;
    sOcaMinigameEndPos = sOcaMinigameNoteCnts[minigameRound];

    for (i = 0; i < 3; i++) {
        Audio_OcaMemoryGameGenNote();
    }
}

s32 Audio_OcaMemoryGameGenNote(void) {
    u32 rnd;
    u8 rndNote;

    if (sOcaMinigameAppendPos == sOcaMinigameEndPos) {
        return 1;
    }

    rnd = Audio_NextRandom();
    rndNote = sOcarinaNoteValues[rnd % 5];

    if (sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos - 1].noteIdx == rndNote) {
        rndNote = sOcarinaNoteValues[(rnd + 1) % 5];
    }

    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].noteIdx = rndNote;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].unk_02 = 0x2D;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].volume = 0x50;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].vibrato = 0;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].tone = 0;

    sOcaMinigameAppendPos++;

    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].noteIdx = 0xFF;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos].unk_02 = 0;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos + 1].noteIdx = 0xFF;
    sOcarinaSongs[OCARINA_SONG_MEMORY_GAME][sOcaMinigameAppendPos + 1].unk_02 = 0;
    return 0;
}

// input update?
void func_800EE6F4(void) {
    D_8016BA04 = gAudioContext.totalTaskCnt;
    if (D_80130F10 != 0) {
        if (sOcarinaInpEnabled == 1) {
            Audio_GetOcaInput();
        }
        if ((sPlaybackState == 0) && (sOcarinaInpEnabled == 1)) {
            func_800ED458(0);
        }
        if (D_80130F3C != 0) {
            if (D_80130F3C & 0x4000) {
                func_800ED200();
            } else {
                func_800ECDF8();
            }
        }

        Audio_OcaPlayback();
        D_80130F68 = D_8016BA04;

        if (sPlaybackState == 0) {
            func_800EE404();
        }

        if ((D_80130F3C != 0) && (sPrevOcarinaNoteVal != sCurOcarinaBtnVal)) {
            sOcarinaDropInputTimer = 1;
        }

        sPrevOcarinaNoteVal = sCurOcarinaBtnVal;
    }

    Audio_OcaUpdatePlayingStaff();
    Audio_OcaUpdateDisplayedStaff();
    Audio_OcaUpdateRecordingStaff();
}

void func_800EE824(void) {
    static u8 D_80131C80 = 0;
    static u8 D_80131C84 = 1;
    static u16 D_80131C88 = 1200;

    switch (D_80131C80) {
        case 0:
            if (D_80131C88-- == 0) {
                if (D_80131C84 < 7) {
                    D_80131C80++;
                } else {
                    D_80131C80 = 3;
                    Audio_OcaSetInstrument(0);
                }
                D_80131C88 = 1200;
            }
            break;
        case 1:
            Audio_SetSoundBanksMute(0);
            Audio_OcaSetInstrument(D_80131C84);
            Audio_OcaSetSongPlayback(OCARINA_SONG_SCARECROW_LONG + 1, 1);
            D_80131C84++;
            D_80131C80++;
            break;
        case 2:
            if (Audio_OcaGetDisplayingStaff()->state == 0) {
                D_80131C80 = 0;
            }
            break;
    }
}

void func_800EE930(void) {
    sPlayingStaff.noteIdx = OCARINA_NOTE_INVALID;
    sPlayingStaff.state = 0xFF;
    sPlayingStaff.pos = 0;
    sDisplayedStaff.noteIdx = OCARINA_NOTE_INVALID;
    sDisplayedStaff.state = 0;
    sDisplayedStaff.pos = 0;
    sRecordingStaff.noteIdx = OCARINA_NOTE_INVALID;
    sRecordingStaff.state = 0xFF;
    sRecordingStaff.pos = 0;
    sOcarinaDropInputTimer = 0;
}

f32 D_80131C8C = 0.0f;

// === Audio Debugging ===

// These variables come between in-function statics in func_800EE824 and Audio_SplitBgmChannels

f32 sAudioUpdateDuration = 0.0f;
f32 sAudioUpdateDurationMax = 0.0f;
u8 sAudioDebugEverOpened = 0;
u8 sAudioSfxMuted = 0;
u8 sAudioDebugPage = 0;
u8 sAudioSndContSel = 0;
u8 sAudioDebugTextColor = 7;
char sAudioDebugPageNames[15][23] = {
    "Non",
    "Sound Control",
    "Spec Info",
    "Heap Info",
    "Grp Track Info",
    "Sub Track Info",
    "Channel Info",
    "Interface Info",
    "SE Flag Swap",
    "Block Change BGM",
    "Natural Sound Control",
    "Ocarina Test",
    "SE Parameter Change",
    "Scroll Print",
    "Free Area",
};
u16 sAudioSndContWork[11] = { 0 };
u16 sAudioSndContWorkLims[11] = { 128, 128, 7, 512, 4, 2, 16, 32, 2, 2, 2 };
char sSoundBankNames[7][11] = { "PLAYER", "ITEM", "ENVIROMENT", "ENEMY", "SYSTEM", "OCARINA", "VOICE" };
char sSoundModeNames[5][10] = { "W-STEREO", "HEADPHONE", "3D SOUND", "MONO", "" };
s8 sAudioIntInfoX = 0;
s8 sAudioIntInfoY = 0;
s8 sAudioIntInfoSel = 0;
s8 sAudioIntInfoBankPage[7] = { 0, 0, 2, 2, 0, 0, 0 };
u8 sAudioScrPrtSel = 0;
u8 sAudioScrPrtInd = 0;
u8 sAudioScrPrtOverflow = 0;
s8 sAudioScrPrtX = 26;
s8 sAudioScrPrtY = 1;
u8 sAudioScrPrtWork[11] = { 1, 19, 6, 0, 0, 0, 0, 0, 0, 0, 1 };
u8 sAudioScrPrtWorkLims[11] = { 2, SCROLL_PRINT_BUF_SIZE, 8, 2, 2, 2, 2, 2, 2, 2, 2 };
u8 sAudioSubTrackInfoSpec = 0;
u8 sAudioSfxSwapIsEditing = 0;
u8 sAudioSfxSwapSel = 0;
u8 sAudioSfxSwapNibbleSel = 0;
char sAudioSfxSwapModeNames[2][5] = { "SWAP", "ADD" };
u8 sAudioSfxParamChgSel = 0;
u8 sAudioSfxParamChgBitSel = 0;
u16 sAudioSfxParamChgWork[4] = { 0 };
u8 sAudioSubTrackInfoPlayerSel = SEQ_PLAYER_BGM_MAIN;
u8 sAudioSubTrackInfoChannelSel = 0;
u8 sSeqPlayerPeakNumLayers[20] = { 0 };
char sAudioSceneNames[3][2] = { "A", "S", "X" };
u8 sAudioBlkChgBgmWork[2] = { 0 };
u8 sAudioBlkChgBgmSel = 0;
char sBoolStrs[3][5] = { "OFF", "ON", "STBY" };
u8 sAudioNatureFailed = false;
u8 sPeakNumNotes = 0;

void AudioDebug_SetInput(void) {
    Input inputs[4];
    u32 btn;

    PadMgr_RequestPadData(&gPadMgr, inputs, 0);
    btn = inputs[3].cur.button;
    sDebugPadHold = btn & 0xFFFF;
    sDebugPadPress = (btn ^ sDebugPadBtnLast) & btn;
    sDebugPadBtnLast = btn;
}

char* AudioDebug_ToStringBinary(u32 num, u8 bits) {
    u8 i;
    u32 flg = 1;

    for (i = 0; i < bits; flg *= 2, i++) {
        if (num & flg) {
            sBinToStrBuf[bits - i - 1] = '1';
        } else {
            sBinToStrBuf[bits - i - 1] = '0';
        }
    }

    sBinToStrBuf[bits] = '\0';
    return sBinToStrBuf;
}

void AudioDebug_Draw(GfxPrint* printer) {
    s32 pad[3];
    u8 i;
    u8 j;
    u8 ctr;
    u8 ctr2;
    s8 k;
    s8 k2;
    s8 ind;
    u8 numEnabledNotes = 0;
    char digitStr[2] = "1";

#define SETCOL_COMMON(v, r, g, b) \
    GfxPrint_SetColor(printer, ((v & 4) >> 2) * (r), ((v & 2) >> 1) * (g), (v & 1) * (b), 255)
#define SETCOL(r, g, b) SETCOL_COMMON(sAudioDebugTextColor, r, g, b)
#define SETCOL_SCROLLPRINT(r, g, b) SETCOL_COMMON(sAudioScrPrtWork[2], r, g, b)

    sAudioDebugEverOpened = true;
    GfxPrint_SetPos(printer, 3, 2);
    SETCOL(255, 255, 255);
    GfxPrint_Printf(printer, "Audio Debug Mode");

    GfxPrint_SetPos(printer, 3, 3);
    GfxPrint_Printf(printer, "- %s -", sAudioDebugPageNames[sAudioDebugPage]);

    for (i = 0; i < gAudioSpecs[gAudioSpecId].numNotes; i++) {
        if (gAudioContext.notes[i].noteSubEu.bitField0.enabled == 1) {
            numEnabledNotes++;
        }
    }

    if (sPeakNumNotes < numEnabledNotes) {
        sPeakNumNotes = numEnabledNotes;
    }
    if (sAudioSpecPeakNumNotes[gAudioSpecId] < numEnabledNotes) {
        sAudioSpecPeakNumNotes[gAudioSpecId] = numEnabledNotes;
    }

    if (sAudioScrPrtWork[0] != 0) {
        GfxPrint_SetPos(printer, sAudioScrPrtX, sAudioScrPrtY);
        SETCOL_SCROLLPRINT(200, 200, 200);
        GfxPrint_Printf(printer, "Audio ScrPrt");

        ind = sAudioScrPrtInd;
        for (k = 0; k < sAudioScrPrtWork[1] + 1; k++) {
            if (ind == 0) {
                if (sAudioScrPrtOverflow == 1) {
                    ind = SCROLL_PRINT_BUF_SIZE - 1;
                } else {
                    k = sAudioScrPrtWork[1] + 1; // "break;"
                }
            } else {
                ind--;
            }
            if (k != sAudioScrPrtWork[1] + 1) {
                if ((ind % 5) != 0) {
                    SETCOL_SCROLLPRINT(180, 180, 180);
                } else {
                    SETCOL_SCROLLPRINT(120, 120, 120);
                }
                GfxPrint_SetPos(printer, 2 + sAudioScrPrtX, sAudioScrPrtY + sAudioScrPrtWork[1] + 1 - k);
                GfxPrint_Printf(printer, "%s", sAudioScrPrtBuf[ind].str);

                GfxPrint_SetPos(printer, 7 + sAudioScrPrtX, sAudioScrPrtY + sAudioScrPrtWork[1] + 1 - k);
                GfxPrint_Printf(printer, "%04X", sAudioScrPrtBuf[ind].num);
            }
        }
    }

    switch (sAudioDebugPage) {
        case PAGE_NON:
            GfxPrint_SetPos(printer, 3, 4);
            SETCOL(255, 64, 64);
            GfxPrint_Printf(printer, "BGM CANCEL:%s", sBoolStrs[sAudioSndContWork[5]]);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "SE MUTE:%s", sBoolStrs[sAudioSfxMuted]);

            GfxPrint_SetPos(printer, 18, 4);
            SETCOL(255, 255, 255);
            GfxPrint_Printf(printer, "PUSH CONT-4 A-BTN");

            ind = (s8)sAudioSndContWork[2];
            i = gSoundBanks[ind][0].next;
            j = 0;
            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "SE HANDLE:%s", sSoundBankNames[ind]);

            while (i != 0xFF) {
                GfxPrint_SetPos(printer, 3, 7 + j++);
                GfxPrint_Printf(printer, "%02x %04x %02x %08x", i, gSoundBanks[ind][i].sfxId, gSoundBanks[ind][i].state,
                                gSoundBanks[ind][i].priority);
                i = gSoundBanks[ind][i].next;
            }
            break;

        case PAGE_SOUND_CONTROL:
            GfxPrint_SetPos(printer, 2, 4 + sAudioSndContSel);
            SETCOL(127, 255, 127);
            GfxPrint_Printf(printer, "*");

            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "Seq 0  : %2x", sAudioSndContWork[0]);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "Seq 1  : %2x", sAudioSndContWork[1]);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "SE HD  : %2x %s", sAudioSndContWork[2], sSoundBankNames[sAudioSndContWork[2]]);

            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "SE No. :%3x", sAudioSndContWork[3]);

            GfxPrint_SetPos(printer, 3, 8);
            GfxPrint_Printf(printer, "S-Out  : %2x %s", sAudioSndContWork[4], sSoundModeNames[sAudioSndContWork[4]]);

            GfxPrint_SetPos(printer, 3, 9);
            GfxPrint_Printf(printer, "BGM Ent: %2x", sAudioSndContWork[5]);

            GfxPrint_SetPos(printer, 3, 10);
            GfxPrint_Printf(printer, "Spec   : %2x", sAudioSndContWork[6]);

            GfxPrint_SetPos(printer, 3, 11);
            GfxPrint_Printf(printer, "Na Snd : %2x", sAudioSndContWork[7]);

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "Cam Wt : %s", sBoolStrs[sAudioSndContWork[8]]);

            GfxPrint_SetPos(printer, 3, 13);
            GfxPrint_Printf(printer, "Lnk Wt : %s", sBoolStrs[sAudioSndContWork[9]]);

            GfxPrint_SetPos(printer, 3, 14);
            GfxPrint_Printf(printer, "SE Ent : %2x", sAudioSndContWork[10]);
            break;

        case PAGE_INTERFACE_INFO:
            ind = 0;
            for (k = 0; k < 7; k++) {
                if (k == sAudioIntInfoSel) {
                    SETCOL(255, 127, 127);
                } else {
                    SETCOL(255, 255, 255);
                }
                GfxPrint_SetPos(printer, 2 + sAudioIntInfoX, 4 + ind + sAudioIntInfoY);
                GfxPrint_Printf(printer, "%s <%d>", sSoundBankNames[k], sAudioIntInfoBankPage[k]);

                for (k2 = 0; k2 < gChannelsPerBank[gSfxChannelLayout][k]; k2++) {
#define entryIndex (gActiveSounds[k][k2].entryIndex)
#define entry (&gSoundBanks[k][entryIndex])
#define chan (gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[entry->channelIdx])
                    GfxPrint_SetPos(printer, 2 + sAudioIntInfoX, 5 + ind + sAudioIntInfoY);
                    if (sAudioIntInfoBankPage[k] == 1) {
                        if ((entryIndex != 0xFF) &&
                            ((entry->state == SFX_STATE_PLAYING_1) || (entry->state == SFX_STATE_PLAYING_2))) {
                            GfxPrint_Printf(printer, "%2X %5d %5d %5d %02X %04X %04X", entryIndex, (s32)*entry->posX,
                                            (s32)*entry->posY, (s32)*entry->posZ, entry->sfxImportance,
                                            entry->sfxParams, entry->sfxId);
                        } else {
                            GfxPrint_Printf(printer, "FF ----- ----- ----- -- ---- ----");
                        }
                    } else if (sAudioIntInfoBankPage[k] == 2) {
                        if ((entryIndex != 0xFF) &&
                            ((entry->state == SFX_STATE_PLAYING_1) || (entry->state == SFX_STATE_PLAYING_2))) {
                            GfxPrint_Printf(printer, "%2X %5d %5d %5d %3d %3d %04X", entryIndex, (s32)*entry->posX,
                                            (s32)*entry->posY, (s32)*entry->posZ, (s32)(chan->volume * 127.1f),
                                            chan->newPan, entry->sfxId);
                        } else {
                            GfxPrint_Printf(printer, "FF ----- ----- ----- --- --- ----");
                        }
                    } else if (sAudioIntInfoBankPage[k] == 3) {
                        if ((entryIndex != 0xFF) &&
                            ((entry->state == SFX_STATE_PLAYING_1) || (entry->state == SFX_STATE_PLAYING_2))) {
                            GfxPrint_Printf(printer, "%2X %5d %5d %5d %3d %3d %04X", entryIndex, (s32)*entry->posX,
                                            (s32)*entry->posY, (s32)*entry->posZ, (s32)(chan->freqScale * 100.0f),
                                            chan->reverb, entry->sfxId);
                        } else {
                            GfxPrint_Printf(printer, "FF ----- ----- ----- --- --- ----");
                        }
                    } else if (sAudioIntInfoBankPage[k] == 4) {
                        if ((entryIndex != 0xFF) &&
                            ((entry->state == SFX_STATE_PLAYING_1) || (entry->state == SFX_STATE_PLAYING_2))) {
                            GfxPrint_Printf(printer, "%2X %04X", entryIndex, entry->sfxId);
                        } else {
                            GfxPrint_Printf(printer, "FF ----");
                        }
                    }
#undef entryIndex
#undef entry
#undef chan

                    if (sAudioIntInfoBankPage[k] != 0) {
                        ind++;
                    }
                }
                ind++;
            }
            break;

        case PAGE_SCROLL_PRINT:
            GfxPrint_SetPos(printer, 2, 4 + sAudioScrPrtSel);
            SETCOL(255, 255, 255);
            GfxPrint_Printf(printer, "*");

            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "Swicth  : %d", sAudioScrPrtWork[0]);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "Lines   : %d", sAudioScrPrtWork[1] + 1);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "Color   : %d", sAudioScrPrtWork[2]);

            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "%s  : %d", sSoundBankNames[0], sAudioScrPrtWork[3]);

            GfxPrint_SetPos(printer, 3, 8);
            GfxPrint_Printf(printer, "%s    : %d", sSoundBankNames[1], sAudioScrPrtWork[4]);

            GfxPrint_SetPos(printer, 3, 9);
            GfxPrint_Printf(printer, "ENVRONM : %d", sAudioScrPrtWork[5]);

            GfxPrint_SetPos(printer, 3, 10);
            GfxPrint_Printf(printer, "%s   : %d", sSoundBankNames[3], sAudioScrPrtWork[6]);

            GfxPrint_SetPos(printer, 3, 11);
            GfxPrint_Printf(printer, "%s  : %d", sSoundBankNames[4], sAudioScrPrtWork[7]);

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "%s : %d", sSoundBankNames[5], sAudioScrPrtWork[8]);

            GfxPrint_SetPos(printer, 3, 13);
            GfxPrint_Printf(printer, "%s    : %d", sSoundBankNames[6], sAudioScrPrtWork[9]);

            GfxPrint_SetPos(printer, 3, 14);
            GfxPrint_Printf(printer, "SEQ ENT : %d", sAudioScrPrtWork[10]);
            break;

        case PAGE_SFX_SWAP:
            GfxPrint_SetPos(printer, 3, 4);
            SETCOL(255, 255, 255);
            if (gAudioSfxSwapOff) {
                GfxPrint_Printf(printer, "SWAP OFF");
            }

            if (sAudioSfxSwapIsEditing == 0) {
                SETCOL(255, 255, 255);
            } else {
                SETCOL(127, 127, 127);
            }
            GfxPrint_SetPos(printer, 2, 6 + sAudioSfxSwapSel);
            GfxPrint_Printf(printer, "*");

            ctr = sAudioSfxSwapNibbleSel;
            if (sAudioSfxSwapNibbleSel >= 4) {
                ctr++;
            }
            if (sAudioSfxSwapIsEditing == 1) {
                SETCOL(255, 255, 255);
                GfxPrint_SetPos(printer, 3 + ctr, 5);
                GfxPrint_Printf(printer, "V");
            }

            for (i = 0; i < 10; i++) {
                if (i == sAudioSfxSwapSel) {
                    if (sAudioSfxSwapIsEditing == 0) {
                        SETCOL(192, 192, 192);
                    } else {
                        SETCOL(255, 255, 255);
                    }
                } else if (sAudioSfxSwapIsEditing == 0) {
                    SETCOL(144, 144, 144);
                } else {
                    SETCOL(96, 96, 96);
                }
                GfxPrint_SetPos(printer, 3, 6 + i);
                GfxPrint_Printf(printer, "%04x %04x %s", gAudioSfxSwapSource[i], gAudioSfxSwapTarget[i],
                                sAudioSfxSwapModeNames[gAudioSfxSwapMode[i]]);
            }
            break;

        case PAGE_SUB_TRACK_INFO:
            GfxPrint_SetPos(printer, 3, 4);
            SETCOL(255, 255, 255);
            GfxPrint_Printf(printer, "Group Track:%d", sAudioSubTrackInfoPlayerSel);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "Sub Track  :%d", sAudioSubTrackInfoChannelSel);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "TRK NO. ");

            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "ENTRY   ");

            GfxPrint_SetPos(printer, 3, 8);
            GfxPrint_Printf(printer, "MUTE    ");

            GfxPrint_SetPos(printer, 3, 9);
            GfxPrint_Printf(printer, "OPENNOTE");

            ctr2 = 0;
            for (i = 0; i < 16; i++) {
                if (i == sAudioSubTrackInfoChannelSel) {
                    SETCOL(255, 255, 255);
                } else {
                    SETCOL(200, 200, 200);
                }
                GfxPrint_SetPos(printer, 15 + i, 6);
                GfxPrint_Printf(printer, "%1X", i);

                GfxPrint_SetPos(printer, 15 + i, 7);
                if (gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel].channels[i]->enabled) {
                    GfxPrint_Printf(printer, "O");
                } else {
                    GfxPrint_Printf(printer, "X");
                }

                GfxPrint_SetPos(printer, 15 + i, 8);
                if (gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel].channels[i]->stopSomething2) {
                    GfxPrint_Printf(printer, "O");
                } else {
                    GfxPrint_Printf(printer, "X");
                }

                GfxPrint_SetPos(printer, 15 + i, 9);
                ctr = 0;
                for (j = 0; j < 4; j++) {
                    if (gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel].channels[i]->layers[j] != NULL) {
                        ctr++;
                    }
                }

                GfxPrint_Printf(printer, "%1X", ctr);
                ctr2 += ctr;
            }

            SETCOL(255, 255, 255);
            if (sSeqPlayerPeakNumLayers[sAudioSubTrackInfoPlayerSel] < ctr2) {
                sSeqPlayerPeakNumLayers[sAudioSubTrackInfoPlayerSel] = ctr2;
            }
            GfxPrint_SetPos(printer, 16 + i, 9);
            GfxPrint_Printf(printer, "%2d,%2d", ctr2, sSeqPlayerPeakNumLayers[sAudioSubTrackInfoPlayerSel]);

            GfxPrint_SetPos(printer, 3, 11);
            GfxPrint_Printf(printer, "VOL     ");

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "E VOL   ");

            GfxPrint_SetPos(printer, 3, 13);
            GfxPrint_Printf(printer, "BANK ID ");

            GfxPrint_SetPos(printer, 3, 14);
            GfxPrint_Printf(printer, "PROG    ");

            GfxPrint_SetPos(printer, 3, 15);
            GfxPrint_Printf(printer, "PAN    ");

            GfxPrint_SetPos(printer, 3, 16);
            GfxPrint_Printf(printer, "PANPOW  ");

            GfxPrint_SetPos(printer, 3, 17);
            GfxPrint_Printf(printer, "FXMIX   ");

            GfxPrint_SetPos(printer, 3, 18);
            GfxPrint_Printf(printer, "PRIO    ");

            GfxPrint_SetPos(printer, 3, 19);
            GfxPrint_Printf(printer, "VIB PIT ");

            GfxPrint_SetPos(printer, 3, 20);
            GfxPrint_Printf(printer, "VIB DEP ");

            GfxPrint_SetPos(printer, 3, 21);
            GfxPrint_Printf(printer, "TUNE    ");

            GfxPrint_SetPos(printer, 3, 22);
            GfxPrint_Printf(printer, "TUNE    ");

            for (i = 0; i < 8; i++) {
                GfxPrint_SetPos(printer, 15 + 3 * i, 22);
                GfxPrint_Printf(printer, "%02X ",
                                (u8)gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->soundScriptIO[i]);
            }

            if (gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel].channels[sAudioSubTrackInfoChannelSel]->enabled) {
                GfxPrint_SetPos(printer, 15, 11);
                GfxPrint_Printf(printer, "%d",
                                (u8)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                         .channels[sAudioSubTrackInfoChannelSel]
                                         ->volume *
                                     127.1));

                GfxPrint_SetPos(printer, 15, 12);
                GfxPrint_Printf(printer, "%d",
                                (u8)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                         .channels[sAudioSubTrackInfoChannelSel]
                                         ->volumeScale *
                                     127.1));

                GfxPrint_SetPos(printer, 15, 13);
                GfxPrint_Printf(printer, "%X",
                                gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->fontId);

                ctr = (u8)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                               .channels[sAudioSubTrackInfoChannelSel]
                               ->instOrWave);

                if (ctr == 0) {
                    ctr2 = 0x7F;
                } else if (ctr < 0x80) {
                    ctr2 = ctr - 1;
                } else {
                    ctr2 = ctr;
                }

                GfxPrint_SetPos(printer, 15, 14);
                GfxPrint_Printf(printer, "%d", ctr2);

                GfxPrint_SetPos(printer, 15, 15);
                GfxPrint_Printf(printer, "%d",
                                gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->newPan);

                GfxPrint_SetPos(printer, 15, 16);
                GfxPrint_Printf(printer, "%d",
                                gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->panChannelWeight);

                GfxPrint_SetPos(printer, 15, 17);
                GfxPrint_Printf(printer, "%d",
                                gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->reverb);

                GfxPrint_SetPos(printer, 15, 18);
                GfxPrint_Printf(printer, "%d",
                                gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                    .channels[sAudioSubTrackInfoChannelSel]
                                    ->notePriority);

                GfxPrint_SetPos(printer, 15, 19);
                GfxPrint_Printf(printer, "%d",
                                (u8)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                         .channels[sAudioSubTrackInfoChannelSel]
                                         ->vibratoRateTarget /
                                     32));

                GfxPrint_SetPos(printer, 15, 20);
                GfxPrint_Printf(printer, "%d",
                                (u8)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                         .channels[sAudioSubTrackInfoChannelSel]
                                         ->vibratoExtentTarget /
                                     8));

                GfxPrint_SetPos(printer, 15, 21);
                GfxPrint_Printf(printer, "%d",
                                (u16)(gAudioContext.seqPlayers[sAudioSubTrackInfoPlayerSel]
                                          .channels[sAudioSubTrackInfoChannelSel]
                                          ->freqScale *
                                      100));
            }
            break;

        case PAGE_HEAP_INFO:
            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "TOTAL  %d", D_8014A6C4.heapSize);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "DRIVER %05X / %05X",
                            gAudioContext.notesAndBuffersPool.cur - gAudioContext.notesAndBuffersPool.start,
                            gAudioContext.notesAndBuffersPool.size);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(
                printer, "AT-SEQ %02X-%02X (%05X-%05X / %05X)", (u8)gAudioContext.seqCache.temporary.entries[0].id,
                (u8)gAudioContext.seqCache.temporary.entries[1].id, gAudioContext.seqCache.temporary.entries[0].size,
                gAudioContext.seqCache.temporary.entries[1].size, gAudioContext.seqCache.temporary.pool.size);

            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(
                printer, "AT-BNK %02X-%02X (%05X-%05X / %05X)", (u8)gAudioContext.fontCache.temporary.entries[0].id,
                (u8)gAudioContext.fontCache.temporary.entries[1].id, gAudioContext.fontCache.temporary.entries[0].size,
                gAudioContext.fontCache.temporary.entries[1].size, gAudioContext.fontCache.temporary.pool.size);

            GfxPrint_SetPos(printer, 3, 8);
            GfxPrint_Printf(printer, "ST-SEQ %02Xseqs  (%05X / %06X)", gAudioContext.seqCache.persistent.numEntries,
                            gAudioContext.seqCache.persistent.pool.cur - gAudioContext.seqCache.persistent.pool.start,
                            gAudioContext.seqCache.persistent.pool.size);

            for (k = 0; (u32)k < gAudioContext.seqCache.persistent.numEntries; k++) {
                GfxPrint_SetPos(printer, 3 + 3 * k, 9);
                GfxPrint_Printf(printer, "%02x", gAudioContext.seqCache.persistent.entries[k].id);
            }

            GfxPrint_SetPos(printer, 3, 10);
            GfxPrint_Printf(printer, "ST-BNK %02Xbanks (%05X / %06X)", gAudioContext.fontCache.persistent.numEntries,
                            gAudioContext.fontCache.persistent.pool.cur - gAudioContext.fontCache.persistent.pool.start,
                            gAudioContext.fontCache.persistent.pool.size);

            for (k = 0; (u32)k < gAudioContext.fontCache.persistent.numEntries; k++) {
                GfxPrint_SetPos(printer, 3 + 3 * k, 11);
                GfxPrint_Printf(printer, "%02x", gAudioContext.fontCache.persistent.entries[k].id);
            }

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "E-MEM  %05X / %05X",
                            gAudioContext.permanentPool.cur - gAudioContext.permanentPool.start,
                            gAudioContext.permanentPool.size);
            break;

        case PAGE_BLOCK_CHANGE_BGM:
            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "BGM No.    %02X", sAudioBlkChgBgmWork[0]);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "SCENE SET  %02X %s", sAudioBlkChgBgmWork[1],
                            sAudioSceneNames[sAudioBlkChgBgmWork[1]]);

            SETCOL(0x64, 255, 0x64);
            GfxPrint_SetPos(printer, 2, 4 + sAudioBlkChgBgmSel);
            GfxPrint_Printf(printer, "*");

            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "NEXT SCENE %02X %s",
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[2],
                            sAudioSceneNames[(u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[2]]);

            GfxPrint_SetPos(printer, 3, 8);
            GfxPrint_Printf(printer, "NOW SCENE  %02X %s",
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[4],
                            sAudioSceneNames[(u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[4]]);

            GfxPrint_SetPos(printer, 3, 9);
            GfxPrint_Printf(printer, "NOW BLOCK  %02X",
                            (gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[5] + 1) & 0xFF);

            GfxPrint_SetPos(printer, 3, 11);
            GfxPrint_Printf(printer, "PORT");

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "%02X %02X %02X %02X",
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[0],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[1],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[2],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[3]);

            GfxPrint_SetPos(printer, 3, 13);
            GfxPrint_Printf(printer, "%02X %02X %02X %02X",
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[4],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[5],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[6],
                            (u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[7]);
            break;

        case PAGE_OCARINA_TEST:
            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "SEQ INFO  : %2d %02x %d", sDisplayedStaff.noteIdx, sDisplayedStaff.state,
                            sDisplayedStaff.pos);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "PLAY INFO : %2d %02x %d", sPlayingStaff.noteIdx, sPlayingStaff.state,
                            sPlayingStaff.pos);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "8note REC POINTER : %08x", gScarecrowSpawnSongPtr);

            ctr = 0;
            for (j = 0; j < 4; j++) {
                for (i = 0; i < 8; i++) {
                    GfxPrint_SetPos(printer, 3 + 3 * i, 7 + j);
                    GfxPrint_Printf(printer, "%02x", gScarecrowSpawnSongPtr[ctr++]);
                }
            }

            GfxPrint_SetPos(printer, 3, 24);
            GfxPrint_Printf(printer, "OCA:%02x SEQ:%04x PLAY:%02x REC:%02x", D_80130F10, D_80130F3C, sPlaybackState,
                            sRecordingState);
            break;

        case PAGE_SFX_PARAMETER_CHANGE:
            GfxPrint_SetPos(printer, 2, 4 + sAudioSfxParamChgSel);
            SETCOL(127, 255, 127);
            GfxPrint_Printf(printer, "*");

            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 4);
            GfxPrint_Printf(printer, "SE HD  : %02x %s", sAudioSfxParamChgWork[0],
                            sSoundBankNames[sAudioSfxParamChgWork[0]]);

            GfxPrint_SetPos(printer, 3, 5);
            GfxPrint_Printf(printer, "SE No. : %02x", sAudioSfxParamChgWork[1]);

            GfxPrint_SetPos(printer, 20, 6);
            GfxPrint_Printf(printer, "       : %04x",
                            gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].params);

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(
                printer, "SE SW    %s",
                AudioDebug_ToStringBinary(gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].params, 16));

            SETCOL(127, 255, 127);
            digitStr[0] = (char)('0' + ((gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].params >>
                                         (15 - sAudioSfxParamChgBitSel)) &
                                        1));
            GfxPrint_SetPos(printer, 12 + sAudioSfxParamChgBitSel, 6);
            GfxPrint_Printf(printer, "%s", digitStr);

            SETCOL(255, 255, 255);
            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "SE PR  : %02x",
                            gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].importance);
            break;

        case PAGE_FREE_AREA:
            GfxPrint_SetPos(printer, 3, 4);
            SETCOL(255, 255, 255);
            GfxPrint_Printf(printer, "env_fx %d code_fx %d SPEC %d", sAudioEnvReverb, sAudioCodeReverb, gAudioSpecId);

            if (sAudioUpdateTaskStart == sAudioUpdateTaskEnd) {
                sAudioUpdateDuration = OS_CYCLES_TO_NSEC(sAudioUpdateEndTime - sAudioUpdateStartTime) / (1e9f / 20);
                if (sAudioUpdateDurationMax < sAudioUpdateDuration) {
                    sAudioUpdateDurationMax = sAudioUpdateDuration;
                }
            }

            GfxPrint_SetPos(printer, 3, 6);
            GfxPrint_Printf(printer, "SOUND GAME FRAME NOW %f", sAudioUpdateDuration);

            GfxPrint_SetPos(printer, 3, 7);
            GfxPrint_Printf(printer, "SOUND GAME FRAME MAX %f", sAudioUpdateDurationMax);

            GfxPrint_SetPos(printer, 3, 9);
            GfxPrint_Printf(printer, "SWITCH BGM MODE %d %d %d (FLAG %d)", sPrevSeqMode, sNumFramesStill,
                            sNumFramesMoving, sSeqModeInput);

            GfxPrint_SetPos(printer, 3, 10);
            GfxPrint_Printf(printer, "ENEMY DIST %f VOL %3d", sAudioEnemyDist, sAudioEnemyVol);

            GfxPrint_SetPos(printer, 3, 11);
            GfxPrint_Printf(printer, "GANON DIST VOL %3d", sAudioGanonDistVol);

            GfxPrint_SetPos(printer, 3, 12);
            GfxPrint_Printf(printer, "DEMO FLAG %d", sAudioCutsceneFlag);

            GfxPrint_SetPos(printer, 3, 12);
            if (sAudioHasMalonBgm == true) {
                GfxPrint_Printf(printer, "MARON BGM DIST %f", sAudioMalonBgmDist);
                sAudioHasMalonBgm = false;
            }

            GfxPrint_SetPos(printer, 3, 23);
            if (sAudioNatureFailed != false) {
                GfxPrint_Printf(printer, "NATURE FAILED %01x", sAudioNatureFailed);
            }

            GfxPrint_SetPos(printer, 3, 24);
            if (sSariaBgmPtr != 0) {
                GfxPrint_Printf(printer, "SARIA BGM PTR %08x", sSariaBgmPtr);
            }

            GfxPrint_SetPos(printer, 3, 25);
            GfxPrint_Printf(printer, "POLI %d(%d)", sPeakNumNotes, numEnabledNotes);

            for (i = 0; i < 11; i++) {
                GfxPrint_SetPos(printer, 3 + 3 * i, 26);
                GfxPrint_Printf(printer, "%d", sAudioSpecPeakNumNotes[i]);
            }
            break;
    }
#undef SETCOL_COMMON
#undef SETCOL
#undef SETCOL_SCROLLPRINT
}

void AudioDebug_ProcessInput_SndCont(void) {
    u16 step = 1;

    if (CHECK_BTN_ANY(sDebugPadHold, BTN_CDOWN)) {
        if (sAudioSndContWorkLims[sAudioSndContSel] >= 16) {
            step = 16;
        }
    } else if (CHECK_BTN_ANY(sDebugPadHold, BTN_CLEFT)) {
        if (sAudioSndContWorkLims[sAudioSndContSel] >= 16) {
            step = 8;
        }
    } else if (CHECK_BTN_ANY(sDebugPadHold, BTN_CUP)) {
        sAudioSndContWork[sAudioSndContSel] = 0;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
        if (sAudioSndContSel > 0) {
            sAudioSndContSel--;
        } else {
            sAudioSndContSel = 10;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
        if (sAudioSndContSel < 10) {
            sAudioSndContSel++;
        } else {
            sAudioSndContSel = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
        if (sAudioSndContWork[sAudioSndContSel] >= step) {
            if (1) {
                sAudioSndContWork[sAudioSndContSel] -= step;
            }
        } else {
            sAudioSndContWork[sAudioSndContSel] += sAudioSndContWorkLims[sAudioSndContSel] - step;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
        if (sAudioSndContWork[sAudioSndContSel] + step < sAudioSndContWorkLims[sAudioSndContSel]) {
            sAudioSndContWork[sAudioSndContSel] += step;
        } else {
            sAudioSndContWork[sAudioSndContSel] += step - sAudioSndContWorkLims[sAudioSndContSel];
        }
    }

    if (sAudioSndContSel == 8) {
        if (sAudioSndContWork[sAudioSndContSel] != 0) {
            Audio_SetExtraFilter(0x20);
        } else {
            Audio_SetExtraFilter(0);
        }
    }

    if (sAudioSndContSel == 9) {
        if (sAudioSndContWork[sAudioSndContSel] != 0) {
            Audio_SetBaseFilter(0x20);
        } else {
            Audio_SetBaseFilter(0);
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
        switch (sAudioSndContSel) {
            case 0:
            case 1:
                Audio_StartSeq(sAudioSndContSel, 0, sAudioSndContWork[sAudioSndContSel]);
                break;
            case 2:
            case 3:
                Audio_PlaySoundGeneral(((sAudioSndContWork[2] << 12) & 0xFFFF) + sAudioSndContWork[3] + SFX_FLAG,
                                       &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                break;
            case 4:
                func_800F6700(sAudioSndContWork[sAudioSndContSel]);
                break;
            case 5:
                Audio_SeqCmdE01(SEQ_PLAYER_BGM_MAIN, sAudioSndContWork[sAudioSndContSel]);
                break;
            case 6:
                Audio_SeqCmdF(SEQ_PLAYER_BGM_MAIN, sAudioSndContWork[sAudioSndContSel]);
                sAudioSubTrackInfoSpec = sAudioSndContWork[6];
                if (sAudioSubTrackInfoPlayerSel > gAudioSpecs[sAudioSubTrackInfoSpec].numSequencePlayers - 1) {
                    sAudioSubTrackInfoPlayerSel = gAudioSpecs[sAudioSubTrackInfoSpec].numSequencePlayers - 1;
                }
                break;
            case 7:
                Audio_PlayNatureAmbienceSequence(sAudioSndContWork[sAudioSndContSel]);
                break;
            case 8:
            case 9:
                break;
            case 10:
                Audio_SetSoundBanksMute(sAudioSndContWork[sAudioSndContSel] * 0x7F);
                break;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
        switch (sAudioSndContSel) {
            case 0:
            case 1:
                Audio_SeqCmd1(sAudioSndContSel, 0);
                break;
            case 7:
                Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, 0);
                break;
            case 2:
            case 3:
                Audio_StopSfxByBank(sAudioSndContWork[2]);
                break;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CDOWN)) {
        if (sAudioSndContSel == 0) {
            func_800F595C(sAudioSndContWork[sAudioSndContSel]);
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CRIGHT)) {
        if (sAudioSndContSel == 0) {
            func_800F5ACC(sAudioSndContWork[sAudioSndContSel]);
        }
    }
}

void AudioDebug_ProcessInput_IntInfo(void) {
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CUP)) {
        sAudioIntInfoY--;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CDOWN)) {
        sAudioIntInfoY++;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CLEFT)) {
        sAudioIntInfoX--;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CRIGHT)) {
        sAudioIntInfoX++;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
        sAudioIntInfoX = 0;
        sAudioIntInfoY = 0;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP) && sAudioIntInfoSel > 0) {
        sAudioIntInfoSel--;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN) && sAudioIntInfoSel < 6) {
        sAudioIntInfoSel++;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT) && sAudioIntInfoBankPage[sAudioIntInfoSel] > 0) {
        sAudioIntInfoBankPage[sAudioIntInfoSel]--;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT) && sAudioIntInfoBankPage[sAudioIntInfoSel] < 4) {
        sAudioIntInfoBankPage[sAudioIntInfoSel]++;
    }
}

void AudioDebug_ProcessInput_ScrPrt(void) {
    if (sAudioScrPrtWork[0] != 0) {
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_CUP)) {
            sAudioScrPrtY--;
        }
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_CDOWN)) {
            sAudioScrPrtY++;
        }
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_CLEFT)) {
            sAudioScrPrtX--;
        }
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_CRIGHT)) {
            sAudioScrPrtX++;
        }
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
            sAudioScrPrtX = 26;
            sAudioScrPrtY = 1;
            sAudioScrPrtWork[2] = 6;
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
            sAudioScrPrtInd = 0;
            sAudioScrPrtOverflow = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
        if (sAudioScrPrtSel > 0) {
            sAudioScrPrtSel--;
        } else {
            sAudioScrPrtSel = 10;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
        if (sAudioScrPrtSel < 10) {
            sAudioScrPrtSel++;
        } else {
            sAudioScrPrtSel = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
        if (sAudioScrPrtWork[sAudioScrPrtSel] > 0) {
            sAudioScrPrtWork[sAudioScrPrtSel]--;
        } else {
            sAudioScrPrtWork[sAudioScrPrtSel] = sAudioScrPrtWorkLims[sAudioScrPrtSel] - 1;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
        if (sAudioScrPrtWork[sAudioScrPrtSel] < sAudioScrPrtWorkLims[sAudioScrPrtSel] - 1) {
            sAudioScrPrtWork[sAudioScrPrtSel]++;
        } else {
            sAudioScrPrtWork[sAudioScrPrtSel] = 0;
        }
    }

    D_801333F0 = sAudioScrPrtWork[3] + (sAudioScrPrtWork[4] * 2) + (sAudioScrPrtWork[5] * 4) +
                 (sAudioScrPrtWork[6] * 8) + (sAudioScrPrtWork[7] * 0x10) + (sAudioScrPrtWork[8] * 0x20);
}

void AudioDebug_ProcessInput_SfxSwap(void) {
    s16 step;
    u16 val;
    u8 prev;

    if (!sAudioSfxSwapIsEditing) {
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
            if (sAudioSfxSwapSel > 0) {
                sAudioSfxSwapSel--;
            } else {
                sAudioSfxSwapSel = 9;
            }
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
            if (sAudioSfxSwapSel < 9) {
                sAudioSfxSwapSel++;
            } else {
                sAudioSfxSwapSel = 0;
            }
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
            sAudioSfxSwapIsEditing = true;
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
            gAudioSfxSwapSource[sAudioSfxSwapSel] = 0;
            gAudioSfxSwapTarget[sAudioSfxSwapSel] = 0;
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_START)) {
            if (sAudioSfxSwapSel != 0) {
                prev = sAudioSfxSwapSel - 1;
            } else {
                prev = 9;
            }
            gAudioSfxSwapSource[sAudioSfxSwapSel] = gAudioSfxSwapSource[prev];
            gAudioSfxSwapTarget[sAudioSfxSwapSel] = gAudioSfxSwapTarget[prev];
        }
    } else {
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
            if (sAudioSfxSwapNibbleSel > 0) {
                sAudioSfxSwapNibbleSel--;
            } else {
                sAudioSfxSwapNibbleSel = 7;
            }
        }
        if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
            if (sAudioSfxSwapNibbleSel < 7) {
                sAudioSfxSwapNibbleSel++;
            } else {
                sAudioSfxSwapNibbleSel = 0;
            }
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP) || CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
            if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
                step = CHECK_BTN_ANY(sDebugPadHold, BTN_CUP) ? 8 : 1;
            }

            if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
                step = CHECK_BTN_ANY(sDebugPadHold, BTN_CUP) ? -8 : -1;
            }

            if (sAudioSfxSwapNibbleSel < 4) {
                val = gAudioSfxSwapSource[sAudioSfxSwapSel] >> ((3 - sAudioSfxSwapNibbleSel) * 4);
                val = (val + step) & 0xF;
                gAudioSfxSwapSource[sAudioSfxSwapSel] =
                    (gAudioSfxSwapSource[sAudioSfxSwapSel] & ((0xF << ((3 - sAudioSfxSwapNibbleSel) * 4)) ^ 0xFFFF)) +
                    (val << ((3 - sAudioSfxSwapNibbleSel) * 4));
            } else {
                val = gAudioSfxSwapTarget[sAudioSfxSwapSel] >> ((7 - sAudioSfxSwapNibbleSel) * 4);
                val = (val + step) & 0xF;
                gAudioSfxSwapTarget[sAudioSfxSwapSel] =
                    (gAudioSfxSwapTarget[sAudioSfxSwapSel] & ((0xF << ((7 - sAudioSfxSwapNibbleSel) * 4)) ^ 0xFFFF)) +
                    (val << ((7 - sAudioSfxSwapNibbleSel) * 4));
            }
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
            sAudioSfxSwapIsEditing = false;
        }

        if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
            if (sAudioSfxSwapNibbleSel < 4) {
                gAudioSfxSwapSource[sAudioSfxSwapSel] = 0;
            } else {
                gAudioSfxSwapTarget[sAudioSfxSwapSel] = 0;
            }
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CLEFT)) {
        gAudioSfxSwapOff ^= 1;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CDOWN)) {
        gAudioSfxSwapMode[sAudioSfxSwapSel] ^= 1;
    }
}

void AudioDebug_ProcessInput_SubTrackInfo(void) {
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
        if (sAudioSubTrackInfoPlayerSel != 0) {
            sAudioSubTrackInfoPlayerSel--;
        } else {
            sAudioSubTrackInfoPlayerSel = gAudioSpecs[sAudioSubTrackInfoSpec].numSequencePlayers - 1;
        }
    }
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
        if (sAudioSubTrackInfoPlayerSel < gAudioSpecs[sAudioSubTrackInfoSpec].numSequencePlayers - 1) {
            sAudioSubTrackInfoPlayerSel++;
        } else {
            sAudioSubTrackInfoPlayerSel = SEQ_PLAYER_BGM_MAIN;
        }
    }
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
        sAudioSubTrackInfoChannelSel = (sAudioSubTrackInfoChannelSel - 1) & 0xF;
    }
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
        sAudioSubTrackInfoChannelSel = (sAudioSubTrackInfoChannelSel + 1) & 0xF;
    }
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_START)) {
        sSeqPlayerPeakNumLayers[sAudioSubTrackInfoPlayerSel] = SEQ_PLAYER_BGM_MAIN;
    }
}

void AudioDebug_ProcessInput_HeapInfo(void) {
}

void AudioDebug_ProcessInput_BlkChgBgm(void) {
    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
        if (sAudioBlkChgBgmSel > 0) {
            sAudioBlkChgBgmSel--;
        } else {
            sAudioBlkChgBgmSel = 1;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
        if (sAudioBlkChgBgmSel <= 0) {
            sAudioBlkChgBgmSel++;
        } else {
            sAudioBlkChgBgmSel = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
        sAudioBlkChgBgmWork[sAudioBlkChgBgmSel]--;
        if (sAudioBlkChgBgmSel == 1) {
            Audio_SetSequenceMode(sAudioBlkChgBgmWork[1]);
            ; // might be a fake match?
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
        sAudioBlkChgBgmWork[sAudioBlkChgBgmSel]++;
        if (sAudioBlkChgBgmSel == 1) {
            Audio_SetSequenceMode(sAudioBlkChgBgmWork[1]);
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
        Audio_QueueCmdS8(MK_CMD(0x46, SEQ_PLAYER_BGM_MAIN, 0x00, 0x00), sAudioBlkChgBgmWork[1]);
        Audio_QueueSeqCmd(sAudioBlkChgBgmWork[0] | 0x10000);
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
        Audio_QueueSeqCmd(0x100100FF);
    }
}

void AudioDebug_ProcessInput_OcaTest(void) {
}

void AudioDebug_ProcessInput_SfxParamChg(void) {
    s32 step;
    u16 sfx;

    if (CHECK_BTN_ANY(sDebugPadHold, BTN_CLEFT)) {
        step = 8;
    } else {
        step = 1;
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DUP)) {
        if (sAudioSfxParamChgSel > 0) {
            sAudioSfxParamChgSel--;
        } else {
            sAudioSfxParamChgSel = 3;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DDOWN)) {
        if (sAudioSfxParamChgSel < 3) {
            sAudioSfxParamChgSel++;
        } else {
            sAudioSfxParamChgSel = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DLEFT)) {
        if (sAudioSfxParamChgSel < 2) {
            if (sAudioSfxParamChgSel == 0) {
                if (sAudioSfxParamChgWork[sAudioSfxParamChgSel] > 0) {
                    sAudioSfxParamChgWork[sAudioSfxParamChgSel]--;
                } else {
                    sAudioSfxParamChgWork[sAudioSfxParamChgSel] = sAudioSndContWorkLims[2] - 1;
                }
            } else {
                sAudioSfxParamChgWork[sAudioSfxParamChgSel] -= step;
                sAudioSfxParamChgWork[sAudioSfxParamChgSel] &= 0x1FF;
            }
        } else if (sAudioSfxParamChgSel == 3) {
            gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].importance -= step;
        } else {
            sAudioSfxParamChgBitSel = (sAudioSfxParamChgBitSel - 1) & 0xF;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_DRIGHT)) {
        if (sAudioSfxParamChgSel < 2) {
            if (sAudioSfxParamChgSel == 0) {
                if (sAudioSfxParamChgWork[sAudioSfxParamChgSel] < (sAudioSndContWorkLims[2] - 1)) {
                    sAudioSfxParamChgWork[sAudioSfxParamChgSel]++;
                } else {
                    sAudioSfxParamChgWork[sAudioSfxParamChgSel] = 0;
                }
            } else {
                sAudioSfxParamChgWork[sAudioSfxParamChgSel] += step;
                sAudioSfxParamChgWork[sAudioSfxParamChgSel] &= 0x1FF;
            }
        } else if (sAudioSfxParamChgSel == 3) {
            gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].importance += step;
        } else {
            sAudioSfxParamChgBitSel = (sAudioSfxParamChgBitSel + 1) & 0xF;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
        sfx = (u16)(sAudioSfxParamChgWork[0] << 12) + sAudioSfxParamChgWork[1] + SFX_FLAG;
        Audio_PlaySoundGeneral(sfx, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
        Audio_StopSfxByBank(sAudioSfxParamChgWork[0]);
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CDOWN)) {
        if (sAudioSfxParamChgSel == 2) {
            gSoundParams[sAudioSfxParamChgWork[0]][sAudioSfxParamChgWork[1]].params ^=
                (1 << (0xF - sAudioSfxParamChgBitSel));
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_CUP)) {
        if (sAudioSfxParamChgSel < 2) {
            sAudioSfxParamChgWork[sAudioSfxParamChgSel] = 0;
        }
    }
}

void AudioDebug_ScrPrt(const s8* str, u16 num) {
    u8 i = 0;

    sAudioScrPrtBuf[sAudioScrPrtInd].num = num;

    while (str[i] != 0) {
        sAudioScrPrtBuf[sAudioScrPrtInd].str[i] = str[i];
        i++;
    }

    while (i < 5) {
        sAudioScrPrtBuf[sAudioScrPrtInd].str[i] = 0;
        i++;
    }

    if (sAudioScrPrtInd < 25 - 1) {
        sAudioScrPrtInd++;
    } else {
        sAudioScrPrtInd = 0;
        sAudioScrPrtOverflow = 1;
    }
}

void AudioDebug_ProcessInput(void) {
    if (!sAudioDebugEverOpened) {
        return;
    }

    if (sAudioSfxMuted) {
        Audio_SetSoundBanksMute(0x6F);
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_L)) {
        if (sAudioDebugPage < PAGE_MAX - 1) {
            sAudioDebugPage++;
        } else {
            sAudioDebugPage = 0;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_R)) {
        if (sAudioDebugPage > 0) {
            sAudioDebugPage--;
        } else {
            sAudioDebugPage = PAGE_MAX - 1;
        }
    }

    if (CHECK_BTN_ANY(sDebugPadPress, BTN_Z)) {
        sAudioDebugTextColor++;
        sAudioDebugTextColor &= 7;
    }

    switch (sAudioDebugPage) {
        case PAGE_NON:
            if (CHECK_BTN_ANY(sDebugPadPress, BTN_A)) {
                sAudioSndContWork[5] ^= 1;
                Audio_SeqCmdE01(SEQ_PLAYER_BGM_MAIN, sAudioSndContWork[5]);
                if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) != NA_BGM_NATURE_AMBIENCE) {
                    Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, 0);
                }
                Audio_SeqCmd1(SEQ_PLAYER_FANFARE, 0);
                Audio_SeqCmd1(SEQ_PLAYER_BGM_SUB, 0);
            }

            if (CHECK_BTN_ANY(sDebugPadPress, BTN_B)) {
                sAudioSfxMuted ^= 1;
                if (!sAudioSfxMuted) {
                    Audio_SetSoundBanksMute(0);
                }
            }
            break;
        case PAGE_SOUND_CONTROL:
            AudioDebug_ProcessInput_SndCont();
            break;
        case PAGE_INTERFACE_INFO:
            AudioDebug_ProcessInput_IntInfo();
            break;
        case PAGE_SCROLL_PRINT:
            AudioDebug_ProcessInput_ScrPrt();
            break;
        case PAGE_SFX_SWAP:
            AudioDebug_ProcessInput_SfxSwap();
            break;
        case PAGE_SUB_TRACK_INFO:
            AudioDebug_ProcessInput_SubTrackInfo();
            break;
        case PAGE_HEAP_INFO:
            AudioDebug_ProcessInput_HeapInfo();
            break;
        case PAGE_BLOCK_CHANGE_BGM:
            AudioDebug_ProcessInput_BlkChgBgm();
            break;
        case PAGE_OCARINA_TEST:
            AudioDebug_ProcessInput_OcaTest();
            break;
        case PAGE_SFX_PARAMETER_CHANGE:
            AudioDebug_ProcessInput_SfxParamChg();
            break;
        case PAGE_FREE_AREA:
        default:
            break;
    }

    D_8013340C = sAudioScrPrtWork[10];
}

void func_800F4A70(void);
void func_800F5CF8(void);

void func_800F3054(void) {
    if (func_800FAD34() == 0) {
        sAudioUpdateTaskStart = gAudioContext.totalTaskCnt;
        sAudioUpdateStartTime = osGetTime();
        func_800EE6F4();
        Audio_StepFreqLerp(&sRiverFreqScaleLerp);
        Audio_StepFreqLerp(&sWaterfallFreqScaleLerp);
        func_800F4A70();
        func_800F56A8();
        func_800F5CF8();
        if (gAudioSpecId == 7) {
            Audio_ClearSariaBgm();
        }
        Audio_ProcessSoundRequests();
        Audio_ProcessSeqCmds();
        func_800F8F88();
        func_800FA3DC();
        AudioDebug_SetInput();
        AudioDebug_ProcessInput();
        Audio_ScheduleProcessCmds();
        sAudioUpdateTaskEnd = gAudioContext.totalTaskCnt;
        sAudioUpdateEndTime = osGetTime();
    }
}

void func_800F3138(UNK_TYPE arg0) {
}

void func_800F3140(UNK_TYPE arg0, UNK_TYPE arg1) {
}

void func_800F314C(s8 arg0) {
    Audio_QueueCmdS32(0x82 << 24 | SEQ_PLAYER_BGM_MAIN << 16 | (((u8)arg0 & 0xFF) << 8), 1);
}

f32 Audio_ComputeSoundVolume(u8 bankId, u8 entryIdx) {
    SoundBankEntry* bankEntry = &gSoundBanks[bankId][entryIdx];
    f32 minDist;
    f32 baseDist;
    f32 ret;

    if (bankEntry->sfxParams & 0x2000) {
        return 1.0f;
    }

    if (bankEntry->dist > 10000.0f) {
        ret = 0.0f;
    } else {
        switch (bankEntry->sfxParams & 3) {
            case 1:
                baseDist = 10000.0f / 15.0f;
                break;
            case 2:
                baseDist = 10000.0f / 10.5f;
                break;
            case 3:
                baseDist = 10000.0f / 2.6f;
                break;
            default:
                baseDist = 10000.0f / 20.0f;
                break;
        }

        minDist = baseDist / 5.0f;

        // Volume grows as inverse square of distance. Linearly approximate
        // the inverse part, then square.
        if (bankEntry->dist < minDist) {
            ret = 1.0f;
        } else if (bankEntry->dist < baseDist) {
            ret = ((((baseDist - minDist) - (bankEntry->dist - minDist)) / (baseDist - minDist)) * 0.19f) + 0.81f;
        } else {
            ret = (1.0f - ((bankEntry->dist - baseDist) / (10000.0f - baseDist))) * 0.81f;
        }
        ret = SQ(ret);
    }

    return ret;
}

s8 Audio_ComputeSoundReverb(u8 bankId, u8 entryIdx, u8 channelIdx) {
    s8 distAdd = 0;
    s32 scriptAdd = 0;
    SoundBankEntry* entry = &gSoundBanks[bankId][entryIdx];
    s32 reverb;

    if (!(entry->sfxParams & 0x1000)) {
        if (entry->dist < 2500.0f) {
            distAdd = *entry->posZ > 0.0f ? (entry->dist / 2500.0f) * 70.0f : (entry->dist / 2500.0f) * 91.0f;
        } else {
            distAdd = 70;
        }
    }

    if (IS_SEQUENCE_CHANNEL_VALID(gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx])) {
        scriptAdd = gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx]->soundScriptIO[1];
        if (gAudioContext.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx]->soundScriptIO[1] < 0) {
            scriptAdd = 0;
        }
    }

    reverb = *entry->reverbAdd + distAdd + scriptAdd;
    if ((bankId != BANK_OCARINA) || !((entry->sfxId & 0x1FF) < 2)) {
        reverb += sAudioEnvReverb + sAudioCodeReverb + sSpecReverb;
    }

    if (reverb > 0x7F) {
        reverb = 0x7F;
    }

    return reverb;
}

s8 Audio_ComputeSoundPanSigned(f32 x, f32 z, u8 token) {
    f32 absX;
    f32 absZ;
    f32 pan;

    if (x < 0) {
        absX = -x;
    } else {
        absX = x;
    }
    if (z < 0) {
        absZ = -z;
    } else {
        absZ = z;
    }

    if (absX > 8000.0f) {
        absX = 8000.0f;
    }

    if (absZ > 8000.0f) {
        absZ = 8000.0f;
    }

    if ((x == 0.0f) && (z == 0.0f)) {
        pan = 0.5f;
    } else if (absZ <= absX) {
        pan = (16000.0f - absX) / (3.3f * (16000.0f - absZ));
        if (x >= 0.0f) {
            pan = 1.0f - pan;
        }
    } else {
        pan = (x / (5.0769234f * absZ)) + 0.5f; // about 66 / 13
    }

    if (absZ < 50.0f) {
        if (absX < 50.0f) {
            pan = ((pan - 0.5f) * SQ(absX / 50.0f)) + 0.5f;
        }
    }
    return (s8)((pan * 127.0f) + 0.5f);
}

f32 Audio_ComputeSoundFreqScale(u8 bankId, u8 entryIdx) {
    s32 phi_v0 = 0;
    SoundBankEntry* entry = &gSoundBanks[bankId][entryIdx];
    f32 unk1C;
    f32 freq = 1.0f;

    if (entry->sfxParams & 0x4000) {
        freq = 1.0f - ((gAudioContext.audioRandom & 0xF) / 192.0f);
    }

    switch (bankId) {
        case BANK_PLAYER:
        case BANK_ITEM:
        case BANK_VOICE:
            if (sAudioBaseFilter2 != 0) {
                phi_v0 = 1;
            }
            break;
        case BANK_ENV:
        case BANK_ENEMY:
            if (sAudioExtraFilter2 != 0) {
                phi_v0 = 1;
            }
            break;
        case BANK_SYSTEM:
        case BANK_OCARINA:
            break;
    }

    if (phi_v0 == 1) {
        if (!(entry->sfxParams & 0x800)) {
            freq *= (1.0293 - ((gAudioContext.audioRandom & 0xF) / 144.0f));
        }
    }

    unk1C = entry->dist;
    if (!(entry->sfxParams & 0x2000)) {
        if (!(entry->sfxParams & 0x8000)) {
            if (unk1C >= 10000.0f) {
                freq += 0.2f;
            } else {
                freq += (0.2f * (unk1C / 10000.0f));
            }
        }
    }

    if (entry->sfxParams & 0xC0) {
        freq += (entry->unk_2F / 192.0f);
    }

    return freq;
}

u8 func_800F37B8(f32 behindScreenZ, SoundBankEntry* arg1, s8 arg2) {
    s8 phi_v0;
    u8 phi_v1;
    f32 phi_f0;
    f32 phi_f12;

    if (*arg1->posZ < behindScreenZ) {
        phi_v0 = arg2 < 65 ? arg2 : 0x7F - arg2;

        if (phi_v0 < 30) {
            phi_v1 = 0;
        } else {
            phi_v1 = (((phi_v0 & 0xFFFF) * 10) - 300) / 34;
            if (phi_v1 != 0) {
                phi_v1 = 0x10 - phi_v1;
            }
        }
    } else {
        phi_v1 = 0;
    }

    if (phi_v1 == 0) {
        if (arg1->sfxParams & 0x200) {
            phi_v1 = 0xF;
        }
    }

    switch (arg1->sfxParams & 3) {
        case 1:
            phi_f0 = 12.0f;
            break;
        case 2:
            phi_f0 = 9.0f;
            break;
        case 3:
            phi_f0 = 6.0f;
            break;
        default:
            phi_f0 = 15.0f;
            break;
    }

    phi_f12 = CLAMP_MAX(arg1->dist, 10000.0f / 5.2f);

    return (phi_v1 * 0x10) + (u8)((phi_f0 * phi_f12) / (10000.0f / 5.2f));
}

s8 func_800F3990(f32 arg0, u16 sfxParams) {
    s8 ret = 0;

    if (arg0 >= 0.0f) {
        if (arg0 > 625.0f) {
            ret = 127;
        } else {
            ret = (arg0 / 625.0f) * 126.0f;
        }
    }
    return ret | 1;
}

void Audio_SetSoundProperties(u8 bankId, u8 entryIdx, u8 channelIdx) {
    f32 vol = 1.0f;
    s8 volS8;
    s8 reverb = 0;
    f32 freqScale = 1.0f;
    s8 panSigned = 0x40;
    u8 stereoBits = 0;
    u8 filter = 0;
    s8 sp38 = 0;
    f32 behindScreenZ;
    u8 baseFilter = 0;
    SoundBankEntry* entry = &gSoundBanks[bankId][entryIdx];

    switch (bankId) {
        case BANK_PLAYER:
        case BANK_ITEM:
        case BANK_ENV:
        case BANK_ENEMY:
        case BANK_VOICE:
            if (D_80130604 == 2) {
                sp38 = func_800F3990(*entry->posY, entry->sfxParams);
            }
            // fallthrough
        case BANK_OCARINA:
            entry->dist = sqrtf(entry->dist);
            vol = Audio_ComputeSoundVolume(bankId, entryIdx) * *entry->vol;
            reverb = Audio_ComputeSoundReverb(bankId, entryIdx, channelIdx);
            panSigned = Audio_ComputeSoundPanSigned(*entry->posX, *entry->posZ, entry->token);
            freqScale = Audio_ComputeSoundFreqScale(bankId, entryIdx) * *entry->freqScale;
            if (D_80130604 == 2) {
                behindScreenZ = sBehindScreenZ[(entry->sfxParams & 0x400) >> 10];
                if (!(entry->sfxParams & 0x800)) {
                    if (*entry->posZ < behindScreenZ) {
                        stereoBits = 0x10;
                    }

                    if ((sSfxChannelState[channelIdx].stereoBits ^ stereoBits) & 0x10) {
                        if (panSigned < 0x40) {
                            stereoBits = sSfxChannelState[channelIdx].stereoBits ^ 0x14;
                        } else {
                            stereoBits = sSfxChannelState[channelIdx].stereoBits ^ 0x18;
                        }
                    } else {
                        stereoBits = sSfxChannelState[channelIdx].stereoBits;
                    }
                }
            }
            if (sAudioBaseFilter != 0) {
                if ((bankId == BANK_ITEM) || (bankId == BANK_PLAYER) || (bankId == BANK_VOICE)) {
                    baseFilter = sAudioBaseFilter;
                }
            }

            if ((baseFilter | sAudioExtraFilter) != 0) {
                filter = (baseFilter | sAudioExtraFilter);
            } else if (D_80130604 == 2 && (entry->sfxParams & 0x2000) == 0) {
                filter = func_800F37B8(behindScreenZ, entry, panSigned);
            }
            break;
        case BANK_SYSTEM:
            break;
    }

    if (sSfxChannelState[channelIdx].vol != vol) {
        volS8 = (u8)(vol * 127.0f);
        sSfxChannelState[channelIdx].vol = vol;
    } else {
        volS8 = -1;
    }

    // CHAN_UPD_SCRIPT_IO (slot 2, sets volume)
    Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8) | 2, volS8);
    if (reverb != sSfxChannelState[channelIdx].reverb) {
        Audio_QueueCmdS8(0x5 << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), reverb);
        sSfxChannelState[channelIdx].reverb = reverb;
    }
    if (freqScale != sSfxChannelState[channelIdx].freqScale) {
        Audio_QueueCmdF32(0x4 << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), freqScale);
        sSfxChannelState[channelIdx].freqScale = freqScale;
    }
    if (stereoBits != sSfxChannelState[channelIdx].stereoBits) {
        Audio_QueueCmdS8(0xE << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), stereoBits | 0x10);
        sSfxChannelState[channelIdx].stereoBits = stereoBits;
    }
    if (filter != sSfxChannelState[channelIdx].filter) {
        // CHAN_UPD_SCRIPT_IO (slot 3, sets filter)
        Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8) | 3, filter);
        sSfxChannelState[channelIdx].filter = filter;
    }
    if (sp38 != sSfxChannelState[channelIdx].unk_0C) {
        // CHAN_UPD_UNK_0F
        Audio_QueueCmdS8(0xC << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), 0x10);
        // CHAN_UPD_UNK_20
        Audio_QueueCmdU16(0xD << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), ((u16)(sp38) << 8) + 0xFF);
        sSfxChannelState[channelIdx].unk_0C = sp38;
    }
    if (panSigned != sSfxChannelState[channelIdx].panSigned) {
        Audio_QueueCmdS8(0x3 << 24 | SEQ_PLAYER_SFX << 16 | (channelIdx << 8), panSigned);
        sSfxChannelState[channelIdx].panSigned = panSigned;
    }
}

void Audio_ResetSfxChannelState(void) {
    u8 i;
    SfxPlayerState* state;

    for (i = 0; i < 16; i++) {
        state = &sSfxChannelState[i];
        state->vol = 1.0f;
        state->freqScale = 1.0f;
        state->reverb = 0;
        state->panSigned = 0x40;
        state->stereoBits = 0;
        state->filter = 0xFF;
        state->unk_0C = 0xFF;
    }

    sSfxChannelState[SFX_PLAYER_CHANNEL_OCARINA].unk_0C = 0;
    sPrevSeqMode = 0;
    sAudioCodeReverb = 0;
}

// Function to play "get-item" fanfares according to the type of item obtained (used in rando)
// Longer fanfares for medallions/stones/songs are behind the Cvar
void Audio_PlayFanfare_Rando(GetItemEntry getItem) {
    s32 temp1;
    s16 getItemId = getItem.getItemId;
    s16 itemId = getItem.itemId;

    if (getItem.modIndex == MOD_NONE) {
        if (((itemId >= ITEM_RUPEE_GREEN) && (itemId <= ITEM_RUPEE_GOLD)) || (itemId == ITEM_HEART)) {
            Audio_PlaySoundGeneral(NA_SE_SY_GET_BOXITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        } else {
            if (itemId == ITEM_HEART_CONTAINER ||
                ((itemId == ITEM_HEART_PIECE_2) && ((gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000))) {
                temp1 = NA_BGM_HEART_GET | 0x900;
            } else {
                temp1 = (itemId == ITEM_HEART_PIECE_2) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
            }
            // If we get a skulltula token or the "WINNER" heart, play "get small item"
            // Also make sure "WINNER" heart is not the 4th heart piece.
            if (itemId == ITEM_SKULL_TOKEN || (getItemId == GI_HEART_PIECE_WIN && itemId == ITEM_HEART_PIECE_2 &&
                (gSaveContext.inventory.questItems & 0xF0000000) != 0x40000000)) {
                temp1 = NA_BGM_SMALL_ITEM_GET | 0x900;
            }
            // If the setting is toggled on and we get special quest items (longer fanfares):
            if (CVarGetInteger("gRandoQuestItemFanfares", 0) != 0) {
                // If we get a medallion, play the "get a medallion" fanfare
                if ((itemId >= ITEM_MEDALLION_FOREST) && (itemId <= ITEM_MEDALLION_LIGHT)) {
                    temp1 = NA_BGM_MEDALLION_GET | 0x900;
                }
                // If it's a Spiritual Stone, play the "get a spiritual stone" fanfare
                if ((itemId >= ITEM_KOKIRI_EMERALD) && (itemId <= ITEM_ZORA_SAPPHIRE)) {
                    temp1 = NA_BGM_SPIRITUAL_STONE | 0x900;
                }
                // If the item we're getting is a song, play the "learned a song" fanfare
                if ((itemId >= ITEM_SONG_MINUET) && (itemId <= ITEM_SONG_STORMS)) {
                    temp1 = NA_BGM_OCA_FAIRY_GET | 0x900;
                }
            }
            Audio_PlayFanfare(temp1);
        } 
    } else if (getItem.modIndex == MOD_RANDOMIZER) {
        if ((itemId >= RG_BOTTLE_WITH_RED_POTION && itemId <= RG_BOTTLE_WITH_BIG_POE) || 
            (itemId >= RG_DEKU_TREE_MAP && itemId <= RG_GANONS_CASTLE_SMALL_KEY)) {
            temp1 = NA_BGM_ITEM_GET | 0x900;
        } else {
            // Just in case nothing else matches.
            temp1 = NA_BGM_ITEM_GET | 0x900;
        }
        Audio_PlayFanfare(temp1);
    }
}

void func_800F3F3C(u8 arg0) {
    if (gSoundBankMuted[0] != 1) {
        Audio_StartSeq(SEQ_PLAYER_BGM_SUB, 0, NA_BGM_VARIOUS_SFX);
        Audio_SeqCmd8(SEQ_PLAYER_BGM_SUB, 0, 0, arg0);
    }
}

f32 func_800F3F84(f32 arg0) {
    f32 ret = 1.0f;

    if (arg0 > 6.0f) {
        D_8016B7A8 = 1.0f;
        D_8016B7B0 = 1.1f;
    } else {
        ret = arg0 / 6.0f;
        D_8016B7A8 = (ret * 0.22500002f) + 0.775f;
        D_8016B7B0 = (ret * 0.2f) + 0.9f;
    }
    return ret;
}

void func_800F4010(Vec3f* pos, u16 sfxId, f32 arg2) {
    f32 sp24;
    f32 phi_f0;
    u8 phi_v0;
    u16 sfxId2;

    D_80131C8C = arg2;
    sp24 = func_800F3F84(arg2);
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_8016B7B0, &D_8016B7A8, &D_801333E8);

    if ((sfxId & 0xF0) == 0xB0) {
        phi_f0 = 0.3f;
        phi_v0 = 1;
        sp24 = 1.0f;
    } else {
        phi_f0 = 1.1f;
        phi_v0 = gAudioContext.audioRandom % 2;
    }

    if ((phi_f0 < arg2) && (phi_v0 != 0)) {
        if ((sfxId & 0x80) != 0) {
            sfxId2 = NA_SE_PL_METALEFFECT_ADULT;
        } else {
            sfxId2 = NA_SE_PL_METALEFFECT_KID;
        }
        D_8016B7AC = (sp24 * 0.7) + 0.3;
        Audio_PlaySoundGeneral(sfxId2, pos, 4, &D_8016B7B0, &D_8016B7AC, &D_801333E8);
    }
}

void func_800F4138(Vec3f* pos, u16 sfxId, f32 arg2) {
    func_800F3F84(arg2);
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_8016B7B0, &D_8016B7A8, &D_801333E8);
}

void func_800F4190(Vec3f* pos, u16 sfxId) {
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_801305B0, &D_801333E0, &D_801305B4);
}
void Audio_PlaySoundRandom(Vec3f* pos, u16 baseSfxId, u8 randLim) {
    u8 offset = Audio_NextRandom() % randLim;

    Audio_PlaySoundGeneral(baseSfxId + offset, pos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void func_800F4254(Vec3f* pos, u8 level) {
    level &= 3;
    if (level != sPrevChargeLevel) {
        D_801305F4 = D_801305E4[level];
        switch (level) {
            case 1:
                Audio_PlaySoundGeneral(NA_SE_PL_SWORD_CHARGE, pos, 4, &D_801305F4, &D_801333E0, &D_801333E8);
                break;
            case 2:
                Audio_PlaySoundGeneral(NA_SE_PL_SWORD_CHARGE, pos, 4, &D_801305F4, &D_801333E0, &D_801333E8);
                break;
        }

        sPrevChargeLevel = level;
    }

    if (level != 0) {
        Audio_PlaySoundGeneral(NA_SE_IT_SWORD_CHARGE - SFX_FLAG, pos, 4, &D_801305F4, &D_801333E0, &D_801333E8);
    }
}

void func_800F436C(Vec3f* pos, u16 sfxId, f32 arg2) {
    if (arg2 < 0.75f) {
        D_8016B7D8 = ((arg2 / 0.75f) * 0.25f) + 0.5f;
    } else {
        D_8016B7D8 = arg2;
    }

    if (D_8016B7D8 > 0.5f) {
        Audio_PlaySoundGeneral(sfxId, pos, 4, &D_8016B7D8, &D_801333E0, &D_801333E8);
    }
}

void func_800F4414(Vec3f* pos, u16 sfxId, f32 arg2) {
    D_801305B8--;
    if (D_801305B8 == 0) {
        Audio_PlaySoundGeneral(sfxId, pos, 4, &D_8016B7D8, &D_801333E0, &D_801333E8);

        if (arg2 > 2.0f) {
            arg2 = 2.0f;
        }
        D_801305B8 = (s8)((D_801305C0 - D_801305BC) * (1.0f - arg2)) + D_801305C0;
    }
}

void func_800F44EC(s8 arg0, s8 arg1) {
    D_801305B8 = 1;
    D_801305BC = arg1;
    D_801305C0 = arg0;
}

void func_800F4524(Vec3f* pos, u16 sfxId, s8 arg2) {
    D_8016B7DC = arg2;
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_801333E0, &D_801333E0, &D_8016B7DC);
}

void func_800F4578(Vec3f* pos, u16 sfxId, f32 arg2) {
    D_8016B7E0 = arg2;
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_801333E0, &D_8016B7E0, &D_801333E8);
}

void func_800F45D0(f32 arg0) {
    func_800F4414(&D_801333D4, NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG, arg0);
    func_800F436C(&D_801333D4, 0, (0.15f * arg0) + 1.4f);
}

void Audio_PlaySoundRiver(Vec3f* pos, f32 freqScale) {
    if (!Audio_IsSfxPlaying(NA_SE_EV_RIVER_STREAM - SFX_FLAG)) {
        sRiverFreqScaleLerp.value = freqScale;
    } else if (freqScale != sRiverFreqScaleLerp.value) {
        sRiverFreqScaleLerp.target = freqScale;
        sRiverFreqScaleLerp.remainingFrames = 40;
        sRiverFreqScaleLerp.step = (sRiverFreqScaleLerp.target - sRiverFreqScaleLerp.value) / 40;
    }
    Audio_PlaySoundGeneral(NA_SE_EV_RIVER_STREAM - SFX_FLAG, pos, 4, &sRiverFreqScaleLerp.value, &D_801333E0,
                           &D_801333E8);
}

void Audio_PlaySoundWaterfall(Vec3f* pos, f32 freqScale) {
    if (!Audio_IsSfxPlaying(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG)) {
        sWaterfallFreqScaleLerp.value = freqScale;
    } else if (freqScale != sWaterfallFreqScaleLerp.value) {
        sWaterfallFreqScaleLerp.target = freqScale;
        sWaterfallFreqScaleLerp.remainingFrames = 40;
        sWaterfallFreqScaleLerp.step = (sWaterfallFreqScaleLerp.target - sWaterfallFreqScaleLerp.value) / 40;
    }
    Audio_PlaySoundGeneral(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG, pos, 4, &sWaterfallFreqScaleLerp.value,
                           &sWaterfallFreqScaleLerp.value, &D_801333E8);
}

void Audio_StepFreqLerp(FreqLerp* lerp) {
    if (lerp->remainingFrames != 0) {
        lerp->remainingFrames--;
        if (lerp->remainingFrames != 0) {
            lerp->value += lerp->step;
        } else {
            lerp->value = lerp->target;
        }
    }
}

void func_800F47BC(void) {
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 1, 0, 10);
    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 1, 0, 10);
}

void func_800F47FC(void) {
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 1, 0x7F, 3);
    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 1, 0x7F, 3);
}

void func_800F483C(u8 targetVol, u8 volFadeTimer) {
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 0, targetVol, volFadeTimer);
}

void func_800F4870(u8 arg0) {
    s8 pan;
    u8 i;

    pan = 0;
    if (arg0 == 0) {
        pan = 0x7F;
    }

    for (i = 0; i < 16; i++) {
        // CHAN_UPD_PAN_UNSIGNED
        Audio_QueueCmdS8(
            _SHIFTL(0x7, 24, 8) | _SHIFTL(SEQ_PLAYER_BGM_MAIN, 16, 8) | _SHIFTL(i, 8, 8) | _SHIFTL(0, 0, 8), pan);
    }

    if (arg0 == 7) {
        D_80130600 = 2;
    } else {
        Audio_SetGanonDistVol(D_801305F8[arg0 & 7]);
    }
}

// (name derived from debug strings, should probably update. used in ganon/ganon_boss scenes)
s32 Audio_SetGanonDistVol(u8 targetVol)
{
    u8 phi_v0;
    u16 phi_v0_2;
    u8 i;

    if (sAudioGanonDistVol != targetVol) {
        Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 0, targetVol, 2);
        if (targetVol < 0x40) {
            phi_v0 = 0x10;
        } else {
            phi_v0 = (((targetVol - 0x40) >> 2) + 1) << 4;
        }

        Audio_SeqCmd8(SEQ_PLAYER_BGM_MAIN, 4, 15, phi_v0);
        for (i = 0; i < 0x10; i++) {
            if (gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[i] != &gAudioContext.sequenceChannelNone) {
                if ((u8)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[i]->soundScriptIO[5] != 0xFF) {
                    // this looks like some kind of macro?
                    phi_v0_2 =
                        ((u16)gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[i]->soundScriptIO[5] - targetVol) +
                        0x7F;
                    if (phi_v0_2 >= 0x80) {
                        phi_v0_2 = 0x7F;
                    }
                    // CHAN_UPD_REVERB
                    Audio_QueueCmdS8(_SHIFTL(0x5, 24, 8) | _SHIFTL(SEQ_PLAYER_BGM_MAIN, 16, 8) | _SHIFTL(i, 8, 8) |
                                         _SHIFTL(0, 0, 8),
                                     (u8)phi_v0_2);
                }
            }
        }
        sAudioGanonDistVol = targetVol;
    }
    return -1;
}

void func_800F4A54(u8 arg0) {
    D_8016B8B0 = arg0;
    D_8016B8B2 = 1;
}

void func_800F4A70(void) {
    if (D_8016B8B2 == 1) {
        if (D_8016B8B1 != D_8016B8B0) {
            Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 0, D_8016B8B0, 0xA);
            D_8016B8B1 = D_8016B8B0;
            D_8016B8B3 = 1;
        }
        D_8016B8B2 = 0;
    } else if (D_8016B8B3 == 1 && D_80130608 == 0) {
        Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 0, 0x7F, 0xA);
        D_8016B8B1 = 0x7F;
        D_8016B8B3 = 0;
    }

    if (D_80130600 != 0) {
        D_80130600--;
        if (D_80130600 == 0) {
            Audio_SetGanonDistVol(D_801305F8[7]);
        }
    }
}

void Audio_PlaySoundIncreasinglyTransposed(Vec3f* pos, s16 sfxId, u8* semitones) {
    Audio_PlaySoundGeneral(sfxId, pos, 4, &gNoteFrequencies[semitones[sAudioIncreasingTranspose] + 39], &D_801333E0,
                           &D_801333E8);

    if (sAudioIncreasingTranspose < 15) {
        sAudioIncreasingTranspose++;
    }
}

void Audio_ResetIncreasingTranspose(void) {
    sAudioIncreasingTranspose = 0;
}

void Audio_PlaySoundTransposed(Vec3f* pos, u16 sfxId, s8 semitone) {
    Audio_PlaySoundGeneral(sfxId, pos, 4, &gNoteFrequencies[semitone + 39], &D_801333E0, &D_801333E8);
}

void func_800F4C58(Vec3f* pos, u16 sfxId, u8 arg2) {
    u8 phi_s1 = 0;
    u8 i;
    u8 bankId;

    bankId = SFX_BANK_SHIFT(sfxId);
    for (i = 0; i < bankId; i++) {
        phi_s1 += gChannelsPerBank[gSfxChannelLayout][i];
    }

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        if ((gActiveSounds[bankId][i].entryIndex != 0xFF) &&
            (sfxId == gSoundBanks[bankId][gActiveSounds[bankId][i].entryIndex].sfxId)) {
            Audio_QueueCmdS8(
                _SHIFTL(0x6, 24, 8) | _SHIFTL(SEQ_PLAYER_SFX, 16, 8) | _SHIFTL(phi_s1, 8, 8) | _SHIFTL(6, 0, 8), arg2);
        }
        phi_s1++;
    }
    Audio_PlaySoundGeneral(sfxId, pos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void func_800F4E30(Vec3f* pos, f32 arg1) {
    f32 phi_f22;
    s8 phi_s4;
    u8 i;

    if (sSariaBgmPtr == NULL) {
        sSariaBgmPtr = pos;
        D_80130650 = arg1;
    } else if (pos != sSariaBgmPtr) {
        if (arg1 < D_80130650) {
            sSariaBgmPtr = pos;
            D_80130650 = arg1;
        }
    } else {
        D_80130650 = arg1;
    }

    if (sSariaBgmPtr->x > 100.0f) {
        phi_s4 = 0x7F;
    } else if (sSariaBgmPtr->x < -100.0f) {
        phi_s4 = 0;
    } else {
        phi_s4 = ((sSariaBgmPtr->x / 100.0f) * 64.0f) + 64.0f;
    }

    if (D_80130650 > 400.0f) {
        phi_f22 = 0.1f;
    } else if (D_80130650 < 120.0f) {
        phi_f22 = 1.0f;
    } else {
        phi_f22 = ((1.0f - ((D_80130650 - 120.0f) / 280.0f)) * 0.9f) + 0.1f;
    }

    for (i = 0; i < 0x10; i++) {
        if (i != 9) {
            Audio_SeqCmd6(SEQ_PLAYER_BGM_MAIN, 2, i, (127.0f * phi_f22));
            Audio_QueueCmdS8(0x3 << 24 | SEQ_PLAYER_BGM_MAIN << 16 | ((u8)((u32)i) << 8), phi_s4);
        }
    }
}

void Audio_ClearSariaBgm(void) {
    if (sSariaBgmPtr != NULL) {
        sSariaBgmPtr = NULL;
    }
}

void Audio_ClearSariaBgmAtPos(Vec3f* pos) {
    if (sSariaBgmPtr == pos) {
        sSariaBgmPtr = NULL;
    }
}

/**
 * Turns on and off channels from both bgm players in a way that splits
 * equally between the two bgm channels. Split based on note priority
 */
void Audio_SplitBgmChannels(s8 volSplit) {
    u8 volume;
    u8 notePriority;
    u16 channelBits;
    u8 bgmPlayers[2] = { SEQ_PLAYER_BGM_MAIN, SEQ_PLAYER_BGM_SUB };
    u8 channelIdx;
    u8 i;

    if ((func_800FA0B4(SEQ_PLAYER_FANFARE) == NA_BGM_DISABLED) &&
        (func_800FA0B4(SEQ_PLAYER_BGM_SUB) != NA_BGM_LONLON)) {
        for (i = 0; i < ARRAY_COUNT(bgmPlayers); i++) {
            if (i == 0) {
                // Main Bgm SeqPlayer
                volume = volSplit;
            } else {
                // Sub Bgm SeqPlayer
                volume = 0x7F - volSplit;
            }

            if (volume > 100) {
                notePriority = 11;
            } else if (volume < 20) {
                notePriority = 2;
            } else {
                notePriority = ((volume - 20) / 10) + 2;
            }

            channelBits = 0;
            for (channelIdx = 0; channelIdx < 16; channelIdx++) {
                if (notePriority > gAudioContext.seqPlayers[bgmPlayers[i]].channels[channelIdx]->notePriority) {
                    // If the note currently playing in the channel is a high enough priority,
                    // then keep the channel on by setting a channelBit
                    // If this condition fails, then the channel will be shut off
                    channelBits += (1 << channelIdx);
                }
            }

            Audio_SeqCmdA(bgmPlayers[i], channelBits);
        }
    }
}

void Audio_PlaySariaBgm(Vec3f* pos, u16 seqId, u16 distMax) {
    f32 absY;
    f32 dist;
    u8 vol;
    f32 prevDist;

    if (D_8016B9F3 != 0) {
        D_8016B9F3--;
        return;
    }

    dist = sqrtf(SQ(pos->z) + SQ(pos->x));
    if (sSariaBgmPtr == NULL) {
        sSariaBgmPtr = pos;
        func_800F5E18(SEQ_PLAYER_BGM_SUB, seqId, 0, 7, 2);
    } else {
        prevDist = sqrtf(SQ(sSariaBgmPtr->z) + SQ(sSariaBgmPtr->x));
        if (dist < prevDist) {
            sSariaBgmPtr = pos;
        } else {
            dist = prevDist;
        }
    }

    if (pos->y < 0.0f) {
        absY = -pos->y;
    } else {
        absY = pos->y;
    }

    if ((distMax / 15.0f) < absY) {
        vol = 0;
    } else if (dist < distMax) {
        vol = (1.0f - (dist / distMax)) * 127.0f;
    } else {
        vol = 0;
    }

    if (seqId != NA_BGM_GREAT_FAIRY) {
        Audio_SplitBgmChannels(vol);
    }

    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 3, vol, 0);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 3, 0x7F - vol, 0);
}

void Audio_ClearSariaBgm2(void) {
    sSariaBgmPtr = NULL;
}

void func_800F5510(u16 seqId) {
    func_800F5550(seqId);
    func_800F5E18(SEQ_PLAYER_BGM_MAIN, seqId, 0, 0, 1);
}

void func_800F5550(u16 seqId) {
    u8 sp27 = 0;
    u16 nv;

    if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) != NA_BGM_WINDMILL) {
        if (func_800FA0B4(SEQ_PLAYER_BGM_SUB) == NA_BGM_LONLON) {
            func_800F9474(SEQ_PLAYER_BGM_SUB, 0);
            Audio_QueueCmdS32(0xF8000000, 0);
        }

        if ((sSeqFlags[D_80130630] & 0x20) && sSeqFlags[(seqId & 0xFF) & 0xFF] & 0x10) {

            if ((D_8013062C & 0x3F) != 0) {
                sp27 = 0x1E;
            }

            func_800F5E18(SEQ_PLAYER_BGM_MAIN, seqId, sp27, 7, D_8013062C);

            D_8013062C = 0;
        } else {
            nv = (sSeqFlags[(seqId & 0xFF) & 0xFF] & 0x40) ? 1 : 0xFF;
            func_800F5E18(SEQ_PLAYER_BGM_MAIN, seqId, 0, 7, nv);
            if (!(sSeqFlags[seqId] & 0x20)) {
                D_8013062C = 0xC0;
            }
        }
        D_80130630 = seqId & 0xFF;
    }
}

void func_800F56A8(void) {
    u16 temp_v0;
    u8 bvar;

    temp_v0 = func_800FA0B4(SEQ_PLAYER_BGM_MAIN);
    bvar = temp_v0 & 0xFF;
    if ((temp_v0 != NA_BGM_DISABLED) && (sSeqFlags[bvar] & 0x10)) {
        if (D_8013062C != 0xC0) {
            D_8013062C = gAudioContext.seqPlayers[SEQ_PLAYER_BGM_MAIN].soundScriptIO[3];
        } else {
            D_8013062C = 0;
        }
    }
}

void func_800F5718(void) {
    if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) != NA_BGM_WINDMILL) {
        Audio_StartSeq(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_WINDMILL);
    }
}

void func_800F574C(f32 arg0, u8 arg2) {
    if (arg0 == 1.0f) {
        Audio_SeqCmdB40(SEQ_PLAYER_BGM_MAIN, arg2, 0);
    } else {
        Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0x30, arg2, arg0 * 100.0f);
    }
    Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0xA0, arg2, arg0 * 100.0f);
}

void func_800F5918(void) {
    if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_TIMED_MINI_GAME && func_800FA11C(0, 0xF0000000)) {
        Audio_SeqCmdB(SEQ_PLAYER_BGM_MAIN, 5, 0, 0xD2);
    }
}

void func_800F595C(u16 arg0) {
    u8 arg0b = arg0 & 0xFF;

    if (sSeqFlags[arg0b] & 2) {
        Audio_PlayFanfare(arg0);
    } else if (sSeqFlags[arg0b] & 4) {
        Audio_StartSeq(SEQ_PLAYER_FANFARE, 0, arg0);

    } else {
        func_800F5E18(SEQ_PLAYER_BGM_MAIN, arg0, 0, 7, -1);
        Audio_SeqCmd1(SEQ_PLAYER_FANFARE, 0);
    }
}

void func_800F59E8(u16 arg0) {
    u8 arg0b = arg0 & 0xFF;

    if (sSeqFlags[arg0b] & 2) {
        Audio_SeqCmd1(SEQ_PLAYER_FANFARE, 0);
    } else if (sSeqFlags[arg0b] & 4) {
        Audio_SeqCmd1(SEQ_PLAYER_FANFARE, 0);
    } else {
        Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, 0);
    }
}

s32 func_800F5A58(u8 arg0) {
    u8 phi_a1 = 0;

    if (sSeqFlags[arg0 & 0xFF] & 2) {
        phi_a1 = 1;
    } else if (sSeqFlags[arg0 & 0xFF] & 4) {
        phi_a1 = 1;
    }

    if (arg0 == (u8)func_800FA0B4(phi_a1)) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Plays a sequence on the main bgm player, but stores the previous sequence to return to later
 * Designed for the mini-boss sequence, but also used by mini-game 2 sequence
 */
void func_800F5ACC(u16 seqId) {
    u16 curSeqId = func_800FA0B4(SEQ_PLAYER_BGM_MAIN);

    if ((curSeqId & 0xFF) != NA_BGM_GANON_TOWER && (curSeqId & 0xFF) != NA_BGM_ESCAPE && curSeqId != seqId) {
        Audio_SetSequenceMode(SEQ_MODE_IGNORE);
        if (curSeqId != NA_BGM_DISABLED) {
            sPrevMainBgmSeqId = curSeqId;
        } else {
            osSyncPrintf("Middle Boss BGM Start not stack \n");
        }

        Audio_StartSeq(SEQ_PLAYER_BGM_MAIN, 0, seqId);
    }
}

// based on func_800F5ACC
void PreviewSequence(u16 seqId) {
    u16 curSeqId = func_800FA0B4(SEQ_PLAYER_BGM_MAIN);

    if ((curSeqId & 0xFF) != NA_BGM_GANON_TOWER && (curSeqId & 0xFF) != NA_BGM_ESCAPE && curSeqId != seqId) {
        Audio_SetSequenceMode(SEQ_MODE_IGNORE);
        if (curSeqId != NA_BGM_DISABLED) {
            sPrevMainBgmSeqId = curSeqId;
        } else {
            osSyncPrintf("Middle Boss BGM Start not stack \n");
        }

        Audio_QueuePreviewSeqCmd(seqId);
    }
}

/**
 * Restores the previous sequence to the main bgm player before func_800F5ACC was called
 */
void func_800F5B58(void) {
    if ((func_800FA0B4(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) && (sPrevMainBgmSeqId != NA_BGM_DISABLED) &&
        (sSeqFlags[func_800FA0B4(SEQ_PLAYER_BGM_MAIN) & 0xFF] & 8)) {
        if (sPrevMainBgmSeqId == NA_BGM_DISABLED) {
            Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, 0);
        } else {
            Audio_StartSeq(SEQ_PLAYER_BGM_MAIN, 0, sPrevMainBgmSeqId);
        }

        sPrevMainBgmSeqId = NA_BGM_DISABLED;
    }
}

/**
 * Plays the nature ambience sequence on the main bgm player, but stores the previous sequence to return to later
 */
void func_800F5BF0(u8 natureAmbienceId) {
    u16 curSeqId = func_800FA0B4(SEQ_PLAYER_BGM_MAIN);

    if (curSeqId != NA_BGM_NATURE_AMBIENCE) {
        sPrevMainBgmSeqId = curSeqId;
    }

    Audio_PlayNatureAmbienceSequence(natureAmbienceId);
}

/**
 * Restores the previous sequence to the main bgm player before func_800F5BF0 was called
 */
void func_800F5C2C(void) {
    if (sPrevMainBgmSeqId != NA_BGM_DISABLED) {
        Audio_StartSeq(SEQ_PLAYER_BGM_MAIN, 0, sPrevMainBgmSeqId);
    }
    sPrevMainBgmSeqId = NA_BGM_DISABLED;
}

void Audio_PlayFanfare(u16 seqId)
{
    u16 curSeqId;
    u32 outNumFonts;
    u8* curFontId;
    u8* requestedFontId;

    curSeqId = func_800FA0B4(SEQ_PLAYER_FANFARE);

    // Although seqIds are u16, there is no fanfare that is above 0xFF
    // Sometimes the game will add 0x900 to a requested fanfare ID
    // The `& 0xFF` here is to strip off this 0x900 and get the original fanfare ID
    // when getting the sound font data for the sequence
    curFontId = func_800E5E84(curSeqId & 0xFF, &outNumFonts);
    requestedFontId = func_800E5E84(seqId & 0xFF, &outNumFonts);

	if (!curFontId || !requestedFontId) {
		// disable BGM, we're about to null deref!
		D_8016B9F4 = 1;
	} else {
		if ((curSeqId == NA_BGM_DISABLED) || (*curFontId == *requestedFontId)) {
			D_8016B9F4 = 1;
		} else {
			D_8016B9F4 = 5;
			Audio_SeqCmd1(SEQ_PLAYER_FANFARE, 0);
		}
	}
    D_8016B9F6 = seqId;
}

void func_800F5CF8(void) {
    u16 sp26;
    u16 pad;
    u16 sp22;

    if (D_8016B9F4 != 0) {
        D_8016B9F4--;
        if (D_8016B9F4 == 0) {
            Audio_QueueCmdS32(0xE3000000, SEQUENCE_TABLE);
            Audio_QueueCmdS32(0xE3000000, FONT_TABLE);
            func_800FA0B4(SEQ_PLAYER_BGM_MAIN);
            sp26 = func_800FA0B4(SEQ_PLAYER_FANFARE);
            sp22 = func_800FA0B4(SEQ_PLAYER_BGM_SUB);
            if (sp26 == NA_BGM_DISABLED) {
                Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 1, 0, 5);
                Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 1, 0, 5);
                Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0x80, 1, 0xA);
                Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0x83, 1, 0xA);
                Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0x90, 0, 0);
                if (sp22 != NA_BGM_LONLON) {
                    Audio_SeqCmdC(SEQ_PLAYER_FANFARE, 0x93, 0, 0);
                }
            }
            Audio_StartSeq(SEQ_PLAYER_FANFARE, 1, D_8016B9F6);
            Audio_SeqCmdA(0, 0xFFFF);
            if (sp22 != NA_BGM_LONLON) {
                Audio_SeqCmdA(SEQ_PLAYER_BGM_SUB, 0xFFFF);
            }
        }
    }
}

void func_800F5E18(u8 playerIdx, u16 seqId, u8 fadeTimer, s8 arg3, s8 arg4) {
    Audio_SeqCmd7(playerIdx, arg3, arg4);
    Audio_StartSeq(playerIdx, fadeTimer, seqId);
}

void Audio_SetSequenceMode(u8 seqMode) {
    s32 volumeFadeInTimer;
    u16 seqId;
    u8 volumeFadeOutTimer;

    sSeqModeInput = seqMode;
    if (sPrevMainBgmSeqId == NA_BGM_DISABLED) {
        if (sAudioCutsceneFlag) {
            seqMode = SEQ_MODE_IGNORE;
        }

        seqId = D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254;

        if (seqId == NA_BGM_FIELD_LOGIC && func_800FA0B4(SEQ_PLAYER_BGM_SUB) == (NA_BGM_ENEMY | 0x800)) {
            seqMode = SEQ_MODE_IGNORE;
        }

        if ((seqId == NA_BGM_DISABLED) || (sSeqFlags[(u8)(seqId & 0xFF)] & 1) ||
            ((sPrevSeqMode & 0x7F) == SEQ_MODE_ENEMY)) {
            if (seqMode != (sPrevSeqMode & 0x7F)) {
                if (seqMode == SEQ_MODE_ENEMY) {
                    // Start playing enemy bgm
                    if (D_8016E750[SEQ_PLAYER_BGM_SUB].volScales[1] - sAudioEnemyVol < 0) {
                        volumeFadeInTimer = -(D_8016E750[SEQ_PLAYER_BGM_SUB].volScales[1] - sAudioEnemyVol);
                    } else {
                        volumeFadeInTimer = D_8016E750[SEQ_PLAYER_BGM_SUB].volScales[1] - sAudioEnemyVol;
                    }

                    Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 3, sAudioEnemyVol, volumeFadeInTimer);
                    Audio_StartSeq(SEQ_PLAYER_BGM_SUB, 10, NA_BGM_ENEMY | 0x800);

                    if (seqId != NA_BGM_NATURE_AMBIENCE) {
                        Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 3, (0x7F - sAudioEnemyVol) & 0xFF, 0xA);
                        Audio_SplitBgmChannels(sAudioEnemyVol);
                    }
                } else if ((sPrevSeqMode & 0x7F) == SEQ_MODE_ENEMY) {
                    // Stop playing enemy bgm
                    Audio_SeqCmd1(SEQ_PLAYER_BGM_SUB, 10);
                    if (seqMode == SEQ_MODE_IGNORE) {
                        volumeFadeOutTimer = 0;
                    } else {
                        volumeFadeOutTimer = 10;
                    }

                    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 3, 0x7F, volumeFadeOutTimer);
                    Audio_SplitBgmChannels(0);
                }

                sPrevSeqMode = seqMode + 0x80;
            }
        } else {
            // Hyrule Field will play slightly different bgm music depending on whether player is standing
            // still or moving. This is the logic to determine the transition between those two states
            if (seqMode == SEQ_MODE_DEFAULT) {
                if (sPrevSeqMode == SEQ_MODE_STILL) {
                    sNumFramesMoving = 0;
                }
                sNumFramesStill = 0;
                sNumFramesMoving++;
            } else {
                sNumFramesStill++;
            }

            if (seqMode == SEQ_MODE_STILL && sNumFramesStill < 30 && sNumFramesMoving > 20) {
                seqMode = SEQ_MODE_DEFAULT;
            }

            sPrevSeqMode = seqMode;
            Audio_SeqCmd7(SEQ_PLAYER_BGM_MAIN, 2, seqMode);
        }
    }
}

void Audio_SetBgmEnemyVolume(f32 dist) {
    f32 adjDist;

    if (sPrevSeqMode == (0x80 | SEQ_MODE_ENEMY)) {
        if (dist != sAudioEnemyDist) {
            if (dist < 150.0f) {
                adjDist = 0.0f;
            } else if (dist > 500.0f) {
                adjDist = 350.0f;
            } else {
                adjDist = dist - 150.0f;
            }

            sAudioEnemyVol = ((350.0f - adjDist) * 127.0f) / 350.0f;
            Audio_SetVolScale(SEQ_PLAYER_BGM_SUB, 3, sAudioEnemyVol, 10);
            if (D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254 != NA_BGM_NATURE_AMBIENCE) {
                Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 3, (0x7F - sAudioEnemyVol), 10);
            }
        }
        if (D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254 != NA_BGM_NATURE_AMBIENCE) {
            Audio_SplitBgmChannels(sAudioEnemyVol);
        }
    }
    sAudioEnemyDist = dist;
}

void func_800F6268(f32 dist, u16 arg1) {
    s8 pad;
    s8 phi_v1;
    s16 temp_a0;

    sAudioHasMalonBgm = true;
    sAudioMalonBgmDist = dist;
    if (D_8016B9F2 == 0) {
        temp_a0 = (s8)(func_800FA0B4(SEQ_PLAYER_BGM_MAIN) & 0xFF);
        if (temp_a0 == (arg1 & 0xFF)) {
            if ((arg1 & 0xFF) == NA_BGM_LONLON) {

                if (dist > 2000.0f) {
                    phi_v1 = 127;
                } else if (dist < 200.0f) {
                    phi_v1 = 0;
                } else {
                    phi_v1 = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
                }
                // Transition volume of channels 0, 1 and 13 on seq player 0 over 3 frames
                Audio_SeqCmd6(SEQ_PLAYER_BGM_MAIN, 3, 0, 127 - phi_v1);
                Audio_SeqCmd6(SEQ_PLAYER_BGM_MAIN, 3, 1, 127 - phi_v1);
                Audio_SeqCmd6(SEQ_PLAYER_BGM_MAIN, 3, 13, phi_v1);
                if (D_8016B9D8 == 0) {
                    D_8016B9D8++;
                }
            }
        } else if ((temp_a0 == NA_BGM_NATURE_AMBIENCE) && ((arg1 & 0xFF) == NA_BGM_LONLON)) {
            temp_a0 = (s8)(func_800FA0B4(SEQ_PLAYER_BGM_SUB) & 0xFF);
            if ((temp_a0 != (arg1 & 0xFF)) && (D_8016B9D8 < 10)) {
                func_800F5E18(SEQ_PLAYER_BGM_SUB, NA_BGM_LONLON, 0, 0, 0);
                Audio_SeqCmdA(SEQ_PLAYER_BGM_SUB, 0xFFFC);
                D_8016B9D8 = 10;
            }

            if (dist > 2000.0f) {
                phi_v1 = 127;
            } else if (dist < 200.0f) {
                phi_v1 = 0;
            } else {
                phi_v1 = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
            }
            // Transition volume of channels 0 and 1 on seq player 0 over 3 frames
            Audio_SeqCmd6(SEQ_PLAYER_BGM_SUB, 3, 0, 127 - phi_v1);
            Audio_SeqCmd6(SEQ_PLAYER_BGM_SUB, 3, 1, 127 - phi_v1);
        }

        if (D_8016B9D8 < 10) {
            D_8016B9D8++;
        }
    }
}

void func_800F64E0(u8 arg0) {
    D_80130608 = arg0;
    if (arg0 != 0) {
        Audio_PlaySoundGeneral(NA_SE_SY_WIN_OPEN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        Audio_QueueCmdS32(0xF1000000, 0);
    } else {
        Audio_PlaySoundGeneral(NA_SE_SY_WIN_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        Audio_QueueCmdS32(0xF2000000, 0);
    }
}

void func_800F6584(u8 arg0) {
    u8 playerIdx;
    u16 sp34;

    D_8016B9F2 = arg0;
    if ((func_800FA0B4(SEQ_PLAYER_BGM_MAIN) & 0xFF) == NA_BGM_LONLON) {
        playerIdx = SEQ_PLAYER_BGM_MAIN;
        sp34 = 0;
    } else if ((func_800FA0B4(SEQ_PLAYER_BGM_SUB) & 0xFF) == NA_BGM_LONLON) {
        playerIdx = SEQ_PLAYER_BGM_SUB;
        sp34 = 0xFFFC;
    } else {
        return;
    }

    if (arg0 != 0) {
        Audio_SeqCmd6(playerIdx, 1, 0, 0);
        Audio_SeqCmd6(playerIdx, 1, 1, 0);
        if (playerIdx == SEQ_PLAYER_BGM_SUB) {
            Audio_SeqCmdA(playerIdx, sp34 | 3);
        }
    } else {
        if (playerIdx == SEQ_PLAYER_BGM_SUB) {
            func_800F5E18(SEQ_PLAYER_BGM_SUB, NA_BGM_LONLON, 0, 0, 0);
        }
        Audio_SeqCmd6(playerIdx, 1, 0, 0x7F);
        Audio_SeqCmd6(playerIdx, 1, 1, 0x7F);
        if (playerIdx == SEQ_PLAYER_BGM_SUB) {
            Audio_SeqCmdA(playerIdx, sp34);
        }
    }
}

void Audio_SetEnvReverb(s8 reverb) {
    sAudioEnvReverb = reverb & 0x7F;
}

void Audio_SetCodeReverb(s8 reverb) {
    if (reverb != 0) {
        sAudioCodeReverb = reverb & 0x7F;
    }
}

void func_800F6700(s8 arg0) {
    s8 sp1F = 0;

    switch (arg0) {
        case 0:
            sp1F = 0;
            D_80130604 = 0;
            break;
        case 1:
            sp1F = 3;
            D_80130604 = 3;
            break;
        case 2:
            sp1F = 1;
            D_80130604 = 1;
            break;
        case 3:
            sp1F = 0;
            D_80130604 = 2;
            break;
    }

    Audio_SeqCmdE0(SEQ_PLAYER_BGM_MAIN, sp1F);
}

void Audio_SetBaseFilter(u8 filter) {
    if (sAudioBaseFilter != filter) {
        if (filter == 0) {
            Audio_StopSfxById(NA_SE_PL_IN_BUBBLE);
        } else if (sAudioBaseFilter == 0) {
            Audio_PlaySoundGeneral(NA_SE_PL_IN_BUBBLE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
    sAudioBaseFilter = filter;
    sAudioBaseFilter2 = filter;
}

void Audio_SetExtraFilter(u8 filter) {
    u32 t;
    u8 i;

    sAudioExtraFilter2 = filter;
    sAudioExtraFilter = filter;
    if (D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254 == NA_BGM_NATURE_AMBIENCE) {
        for (i = 0; i < 16; i++) {
            t = i;
            // CHAN_UPD_SCRIPT_IO (seq player 0, all channels, slot 6)
            Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_BGM_MAIN << 16 | ((t & 0xFF) << 8) | 6, filter);
        }
    }
}

void Audio_SetCutsceneFlag(s8 flag) {
    sAudioCutsceneFlag = flag;
}

void Audio_PlaySoundGeneralIfNotInCutscene(u16 sfxId, Vec3f* pos, u8 arg2, f32* freqScale, f32* arg4, s8* reverbAdd) {
    if (!sAudioCutsceneFlag) {
        Audio_PlaySoundGeneral(sfxId, pos, arg2, freqScale, arg4, reverbAdd);
    }
}

void Audio_PlaySoundIfNotInCutscene(u16 sfxId) {
    Audio_PlaySoundGeneralIfNotInCutscene(sfxId, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void func_800F6964(u16 arg0) {
    s32 skip;
    u8 i;

    Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, (arg0 * 3) / 2);
    Audio_SeqCmd1(SEQ_PLAYER_FANFARE, (arg0 * 3) / 2);
    for (i = 0; i < 0x10; i++) {
        skip = false;
        switch (i) {
            case 11:
            case 12:
                if (gAudioSpecId == 10) {
                    skip = true;
                }
                break;
            case 13:
                skip = true;
                break;
        }

        if (!skip) {
            Audio_SeqCmd6(SEQ_PLAYER_SFX, arg0 >> 1, i, 0);
        }
    }

    Audio_SeqCmd1(SEQ_PLAYER_BGM_SUB, (arg0 * 3) / 2);
}

void func_800F6AB0(u16 arg0) {
    Audio_SeqCmd1(SEQ_PLAYER_BGM_MAIN, arg0);
    Audio_SeqCmd1(SEQ_PLAYER_FANFARE, arg0);
    Audio_SeqCmd1(SEQ_PLAYER_BGM_SUB, arg0);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 3, 0x7F, 0);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 1, 0x7F, 0);
}

void func_800F6B3C(void) {
    func_800F9280(SEQ_PLAYER_SFX, 0, 0xFF, 5);
}

void Audio_DisableAllSeq(void) {
    Audio_DisableSeq(SEQ_PLAYER_BGM_MAIN, 0);
    Audio_DisableSeq(SEQ_PLAYER_FANFARE, 0);
    Audio_DisableSeq(SEQ_PLAYER_SFX, 0);
    Audio_DisableSeq(SEQ_PLAYER_BGM_SUB, 0);
    Audio_ScheduleProcessCmds();
}

s8 func_800F6BB8(void) {
    return func_800E6680();
}

void func_800F6BDC(void) {
    Audio_DisableAllSeq();
    Audio_ScheduleProcessCmds();
    while (true) {
        if (!func_800F6BB8()) {
            return;
        }
    }
}

void Audio_PreNMI(void) {
    Audio_PreNMIInternal();
}

void func_800F6C34(void) {
    sPrevSeqMode = 0;
    D_8016B7A8 = 1.0f;
    D_8016B7B0 = 1.0f;
    sAudioBaseFilter = 0;
    sAudioExtraFilter = 0;
    sAudioBaseFilter2 = 0;
    sAudioExtraFilter2 = 0;
    Audio_OcaSetInstrument(0);
    sRiverFreqScaleLerp.remainingFrames = 0;
    sWaterfallFreqScaleLerp.remainingFrames = 0;
    sRiverFreqScaleLerp.value = 1.0f;
    sWaterfallFreqScaleLerp.value = 1.0f;
    D_8016B7D8 = 1.0f;
    D_8016B8B0 = 0x7F;
    D_8016B8B1 = 0x7F;
    D_8016B8B2 = 0;
    D_8016B8B3 = 0;
    sAudioGanonDistVol = 0xFF;
    D_8016B9D8 = 0;
    sSpecReverb = sSpecReverbs[gAudioSpecId];
    D_80130608 = 0;
    sPrevMainBgmSeqId = NA_BGM_DISABLED;
    Audio_QueueCmdS8(0x46 << 24 | SEQ_PLAYER_BGM_MAIN << 16, -1);
    sSariaBgmPtr = NULL;
    D_8016B9F4 = 0;
    D_8016B9F3 = 1;
    D_8016B9F2 = 0;
}

void Audio_SetNatureAmbienceChannelIO(u8 channelIdxRange, u8 port, u8 val) {
    u8 firstChannelIdx;
    u8 lastChannelIdx;
    u8 channelIdx;

    if ((D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254 != NA_BGM_NATURE_AMBIENCE) && func_800FA11C(1, 0xF00000FF)) {
        sAudioNatureFailed = true;
        return;
    }

    // channelIdxRange = 01 on port 1
    if (((channelIdxRange << 8) + port) == ((NATURE_CHANNEL_CRITTER_0 << 8) + CHANNEL_IO_PORT_1)) {
        if (func_800FA0B4(SEQ_PLAYER_BGM_SUB) != NA_BGM_LONLON) {
            D_8016B9D8 = 0;
        }
    }

    firstChannelIdx = channelIdxRange >> 4;
    lastChannelIdx = channelIdxRange & 0xF;

    if (firstChannelIdx == 0) {
        firstChannelIdx = channelIdxRange & 0xF;
    }

    for (channelIdx = firstChannelIdx; channelIdx <= lastChannelIdx; channelIdx++) {
        Audio_SeqCmd8(SEQ_PLAYER_BGM_MAIN, port, channelIdx, val);
    }
}

void Audio_StartNatureAmbienceSequence(u16 playerIO, u16 channelMask) {
    u8 channelIdx;

    if (func_800FA0B4(SEQ_PLAYER_BGM_MAIN) == NA_BGM_WINDMILL) {
        func_800F3F3C(0xF);
        return;
    }

    Audio_SeqCmd7(SEQ_PLAYER_BGM_MAIN, 0, 1);
    Audio_SeqCmd7(SEQ_PLAYER_BGM_MAIN, 4, playerIO >> 8);
    Audio_SeqCmd7(SEQ_PLAYER_BGM_MAIN, 5, playerIO & 0xFF);
    Audio_SetVolScale(SEQ_PLAYER_BGM_MAIN, 0, 0x7F, 1);

    channelIdx = false;
    if (D_80133408 != 0) {
        channelIdx = true;
        Audio_SeqCmdE01(SEQ_PLAYER_BGM_MAIN, 0);
    }

    Audio_StartSeq(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_NATURE_AMBIENCE);

    if (channelIdx) {
        Audio_SeqCmdE01(SEQ_PLAYER_BGM_MAIN, 1);
    }

    for (channelIdx = 0; channelIdx < 16; channelIdx++) {
        if (!(channelMask & (1 << channelIdx)) && (playerIO & (1 << channelIdx))) {
            Audio_SeqCmd8(SEQ_PLAYER_BGM_MAIN, CHANNEL_IO_PORT_1, channelIdx, 1);
        }
    }
}

void Audio_PlayNatureAmbienceSequence(u8 natureAmbienceId) {
    u8 i = 0;
    u8 channelIdx;
    u8 port;
    u8 val;

    if ((D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254 == NA_BGM_DISABLED) ||
        !(sSeqFlags[((u8)D_8016E750[SEQ_PLAYER_BGM_MAIN].unk_254) & 0xFF] & 0x80)) {

        Audio_StartNatureAmbienceSequence(sNatureAmbienceDataIO[natureAmbienceId].playerIO,
                                          sNatureAmbienceDataIO[natureAmbienceId].channelMask);

        while ((sNatureAmbienceDataIO[natureAmbienceId].channelIO[i] != 0xFF) && (i < 100)) {
            // Probably a fake match, using Audio_SeqCmd8 doesn't work.
            channelIdx = sNatureAmbienceDataIO[natureAmbienceId].channelIO[i++];
            port = sNatureAmbienceDataIO[natureAmbienceId].channelIO[i++];
            val = sNatureAmbienceDataIO[natureAmbienceId].channelIO[i++];
            Audio_QueueSeqCmd(0x80000000 | (SEQ_PLAYER_BGM_MAIN << 24) | (port << 0x10) | (channelIdx << 8) | val);
        }

        Audio_SeqCmd8(SEQ_PLAYER_BGM_MAIN, CHANNEL_IO_PORT_7, NATURE_CHANNEL_UNK, D_80130604);
    }
}

void Audio_Init(void) {
    AudioLoad_Init(NULL, 0);
}

void Audio_InitSound(void) {
    func_800F6C34();
    func_800EE930();
    Audio_ResetSfxChannelState();
    func_800FAEB4();
    Audio_ResetSounds();
    func_800F9280(SEQ_PLAYER_SFX, 0, 0x70, 10);
}

void func_800F7170(void) {
    func_800F9280(SEQ_PLAYER_SFX, 0, 0x70, 1);
    Audio_QueueCmdS32(0xF2000000, 1);
    Audio_ScheduleProcessCmds();
    Audio_QueueCmdS32(0xF8000000, 0);
}

void func_800F71BC(s32 arg0) {
    D_80133418 = 1;
    func_800F6C34();
    func_800EE930();
    Audio_ResetSfxChannelState();
    func_800FADF8();
    Audio_ResetSounds();
}

void func_800F7208(void) {
    func_800FADF8();
    Audio_QueueCmdS32(0xF2000000, 1);
    func_800F6C34();
    Audio_ResetSfxChannelState();
    func_800F9280(SEQ_PLAYER_SFX, 0, 0x70, 1);
}
