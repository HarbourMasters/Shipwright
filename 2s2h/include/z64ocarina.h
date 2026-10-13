#ifndef Z64OCARINA_H
#define Z64OCARINA_H

#include "PR/ultratypes.h"

typedef enum OcarinaSongId {
    /*  0 */ OCARINA_SONG_SONATA,
    /*  1 */ OCARINA_SONG_GORON_LULLABY,
    /*  2 */ OCARINA_SONG_NEW_WAVE,
    /*  3 */ OCARINA_SONG_ELEGY,
    /*  4 */ OCARINA_SONG_OATH,
    /*  5 */ OCARINA_SONG_SARIAS,
    /*  6 */ OCARINA_SONG_TIME,
    /*  7 */ OCARINA_SONG_HEALING,
    /*  8 */ OCARINA_SONG_EPONAS,
    /*  9 */ OCARINA_SONG_SOARING,
    /* 10 */ OCARINA_SONG_STORMS,
    /* 11 */ OCARINA_SONG_SUNS,
    /* 12 */ OCARINA_SONG_INVERTED_TIME,
    /* 13 */ OCARINA_SONG_DOUBLE_TIME,
    /* 14 */ OCARINA_SONG_GORON_LULLABY_INTRO,
    /* 15 */ OCARINA_SONG_WIND_FISH_HUMAN, // "Ballad of the Wind Fish"
    /* 16 */ OCARINA_SONG_WIND_FISH_GORON,
    /* 17 */ OCARINA_SONG_WIND_FISH_ZORA,
    /* 18 */ OCARINA_SONG_WIND_FISH_DEKU,
    /* 19 */ OCARINA_SONG_EVAN_PART1,
    /* 20 */ OCARINA_SONG_EVAN_PART2,
    /* 21 */ OCARINA_SONG_ZELDAS_LULLABY,
    /* 22 */ OCARINA_SONG_SCARECROW_SPAWN,
    /* 23 */ OCARINA_SONG_TERMINA_WALL,
    /* 24 */ OCARINA_SONG_MAX,
    /* 24 */ OCARINA_SONG_SCARECROW_LONG = OCARINA_SONG_MAX // anything larger than 24 is considered the long scarecrow's song
} OcarinaSongId;

typedef enum OcarinaSongActionId {
    /* 0x00 */ OCARINA_ACTION_0, // acts like free play but never set
    /* 0x01 */ OCARINA_ACTION_FREE_PLAY,
    /* 0x02 */ OCARINA_ACTION_DEMONSTRATE_SONATA, // Song demonstrated/previewed by another actor/teacher
    /* 0x03 */ OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY,
    /* 0x04 */ OCARINA_ACTION_DEMONSTRATE_NEW_WAVE,
    /* 0x05 */ OCARINA_ACTION_DEMONSTRATE_ELEGY,
    /* 0x06 */ OCARINA_ACTION_DEMONSTRATE_OATH,
    /* 0x07 */ OCARINA_ACTION_DEMONSTRATE_SARIAS,
    /* 0x08 */ OCARINA_ACTION_DEMONSTRATE_TIME,
    /* 0x09 */ OCARINA_ACTION_DEMONSTRATE_HEALING,
    /* 0x0A */ OCARINA_ACTION_DEMONSTRATE_EPONAS,
    /* 0x0B */ OCARINA_ACTION_DEMONSTRATE_SOARING,
    /* 0x0C */ OCARINA_ACTION_DEMONSTRATE_STORMS,
    /* 0x0D */ OCARINA_ACTION_DEMONSTRATE_SUNS,
    /* 0x0E */ OCARINA_ACTION_DEMONSTRATE_INVERTED_TIME,
    /* 0x0F */ OCARINA_ACTION_DEMONSTRATE_DOUBLE_TIME,
    /* 0x10 */ OCARINA_ACTION_DEMONSTRATE_GORON_LULLABY_INTRO,
    /* 0x11 */ OCARINA_ACTION_11,
    /* 0x12 */ OCARINA_ACTION_PROMPT_SONATA, // User plays back song through inputing 1 note at a time, as a tutorial. Each note is verified.
    /* 0x13 */ OCARINA_ACTION_PROMPT_GORON_LULLABY,
    /* 0x14 */ OCARINA_ACTION_PROMPT_NEW_WAVE,
    /* 0x15 */ OCARINA_ACTION_PROMPT_ELEGY,
    /* 0x16 */ OCARINA_ACTION_PROMPT_OATH,
    /* 0x17 */ OCARINA_ACTION_PROMPT_SARIAS,
    /* 0x18 */ OCARINA_ACTION_PROMPT_TIME,
    /* 0x19 */ OCARINA_ACTION_PROMPT_HEALING,
    /* 0x1A */ OCARINA_ACTION_PROMPT_EPONAS,
    /* 0x1B */ OCARINA_ACTION_PROMPT_SOARING,
    /* 0x1C */ OCARINA_ACTION_PROMPT_STORMS,
    /* 0x1D */ OCARINA_ACTION_PROMPT_SUNS,
    /* 0x1E */ OCARINA_ACTION_PROMPT_INVERTED_TIME,
    /* 0x1F */ OCARINA_ACTION_PROMPT_DOUBLE_TIME,
    /* 0x20 */ OCARINA_ACTION_PROMPT_GORON_LULLABY_INTRO,
    /* 0x21 */ OCARINA_ACTION_21,
    /* 0x22 */ OCARINA_ACTION_CHECK_SONATA, // Once the entire song is played, checks for a specific song. Broken as playing any dungeon song will softlock.
    /* 0x23 */ OCARINA_ACTION_CHECK_GORON_LULLABY,
    /* 0x24 */ OCARINA_ACTION_CHECK_NEW_WAVE,
    /* 0x25 */ OCARINA_ACTION_CHECK_ELEGY,
    /* 0x26 */ OCARINA_ACTION_CHECK_OATH,
    /* 0x27 */ OCARINA_ACTION_CHECK_SARIAS,
    /* 0x28 */ OCARINA_ACTION_CHECK_TIME,
    /* 0x29 */ OCARINA_ACTION_CHECK_HEALING,
    /* 0x2A */ OCARINA_ACTION_CHECK_EPONAS,
    /* 0x2B */ OCARINA_ACTION_CHECK_SOARING,
    /* 0x2C */ OCARINA_ACTION_CHECK_STORMS,
    /* 0x2D */ OCARINA_ACTION_CHECK_SUNS,
    /* 0x2E */ OCARINA_ACTION_CHECK_INVERTED_TIME,
    /* 0x2F */ OCARINA_ACTION_CHECK_DOUBLE_TIME,
    /* 0x30 */ OCARINA_ACTION_CHECK_GORON_LULLABY_INTRO,
    /* 0x31 */ OCARINA_ACTION_CHECK_SCARECROW_SPAWN, // TODO: Verify (may include more)
    /* 0x32 */ OCARINA_ACTION_FREE_PLAY_DONE, 
    /* 0x33 */ OCARINA_ACTION_SCARECROW_LONG_RECORDING,
    /* 0x34 */ OCARINA_ACTION_SCARECROW_LONG_DEMONSTRATION,
    /* 0x35 */ OCARINA_ACTION_SCARECROW_SPAWN_RECORDING,
    /* 0x36 */ OCARINA_ACTION_SCARECROW_SPAWN_DEMONSTRATION,
    /* 0x37 */ OCARINA_ACTION_37,
    /* 0x38 */ OCARINA_ACTION_CHECK_NOTIME,
    /* 0x39 */ OCARINA_ACTION_CHECK_NOTIME_DONE,
    /* 0x3A */ OCARINA_ACTION_3A, // Monkey Instrument Prompt
    /* 0x3B */ OCARINA_ACTION_3B,
    /* 0x3C */ OCARINA_ACTION_3C,
    /* 0x3D */ OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_FIRST_HALF, // Evan Part 1 demonstrate first half (Japa)
    /* 0x3E */ OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_FIRST_HALF, // Evan Part 2 demonstrate first half (Japa)
    /* 0x3F */ OCARINA_ACTION_DEMONSTRATE_EVAN_PART1_SECOND_HALF, // Evan Part 1 demonstrate second half (Diary)
    /* 0x40 */ OCARINA_ACTION_DEMONSTRATE_EVAN_PART2_SECOND_HALF, // Evan Part 2 demonstrate second half (Diary)
    /* 0x41 */ OCARINA_ACTION_PROMPT_EVAN_PART1_SECOND_HALF, // Evan Part 1 prompt second half (Japa)
    /* 0x42 */ OCARINA_ACTION_PROMPT_EVAN_PART2_SECOND_HALF, // Evan Part 2 prompt second half (Japa)
    /* 0x43 */ OCARINA_ACTION_PROMPT_WIND_FISH_HUMAN,
    /* 0x44 */ OCARINA_ACTION_PROMPT_WIND_FISH_GORON,
    /* 0x45 */ OCARINA_ACTION_PROMPT_WIND_FISH_ZORA,
    /* 0x46 */ OCARINA_ACTION_PROMPT_WIND_FISH_DEKU,
    /* 0x47 */ OCARINA_ACTION_TIMED_PROMPT_SONATA, // Playing back a particular song, but must also time the notes properly. Not fully implemented
    /* 0x48 */ OCARINA_ACTION_TIMED_PROMPT_GORON_LULLABY,
    /* 0x49 */ OCARINA_ACTION_TIMED_PROMPT_NEW_WAVE,
    /* 0x4A */ OCARINA_ACTION_TIMED_PROMPT_ELEGY,
    /* 0x4B */ OCARINA_ACTION_TIMED_PROMPT_OATH,
    /* 0x4C */ OCARINA_ACTION_TIMED_PROMPT_SARIAS,
    /* 0x4D */ OCARINA_ACTION_TIMED_PROMPT_TIME,
    /* 0x4E */ OCARINA_ACTION_TIMED_PROMPT_HEALING,
    /* 0x4F */ OCARINA_ACTION_TIMED_PROMPT_EPONAS,
    /* 0x50 */ OCARINA_ACTION_TIMED_PROMPT_SOARING,
    /* 0x51 */ OCARINA_ACTION_TIMED_PROMPT_STORMS
} OcarinaSongActionId;

typedef enum OcarinaMode {
    /* 0x00 */ OCARINA_MODE_NONE,
    /* 0x01 */ OCARINA_MODE_ACTIVE,
    /* 0x02 */ OCARINA_MODE_WARP,
    /* 0x03 */ OCARINA_MODE_EVENT,
    /* 0x04 */ OCARINA_MODE_END,
    /* 0x05 */ OCARINA_MODE_PLAYED_TIME, // Played Time
    /* 0x06 */ OCARINA_MODE_PLAYED_HEALING,
    /* 0x07 */ OCARINA_MODE_PLAYED_EPONAS,
    /* 0x08 */ OCARINA_MODE_PLAYED_SOARING,
    /* 0x09 */ OCARINA_MODE_PLAYED_STORMS,
    /* 0x0A */ OCARINA_MODE_PLAYED_SUNS,
    /* 0x0B */ OCARINA_MODE_PLAYED_INVERTED_TIME,
    /* 0x0C */ OCARINA_MODE_PLAYED_DOUBLE_TIME,
    /* 0x0D */ OCARINA_MODE_PLAYED_SCARECROW_SPAWN,
    /* 0x0E */ OCARINA_MODE_E,
    /* 0x0F */ OCARINA_MODE_F,
    /* 0x10 */ OCARINA_MODE_10,
    /* 0x11 */ OCARINA_MODE_11, // End memory/scarecrow from OoT
    /* 0x12 */ OCARINA_MODE_PROCESS_SOT, // Song of Time related
    /* 0x13 */ OCARINA_MODE_PROCESS_INVERTED_TIME, // Inverted Song of Time related
    /* 0x14 */ OCARINA_MODE_14,
    /* 0x15 */ OCARINA_MODE_PROCESS_DOUBLE_TIME, // Song of Double Time related
    /* 0x16 */ OCARINA_MODE_APPLY_SOT, // Apply
    /* 0x17 */ OCARINA_MODE_17,
    /* 0x18 */ OCARINA_MODE_APPLY_INV_SOT_FAST,
    /* 0x19 */ OCARINA_MODE_APPLY_INV_SOT_SLOW,
    /* 0x1A */ OCARINA_MODE_APPLY_DOUBLE_SOT,
    /* 0x1B */ OCARINA_MODE_1B,
    /* 0x1C */ OCARINA_MODE_WARP_TO_GREAT_BAY_COAST,
    /* 0x1D */ OCARINA_MODE_WARP_TO_ZORA_CAPE,
    /* 0x1E */ OCARINA_MODE_WARP_TO_SNOWHEAD,
    /* 0x1F */ OCARINA_MODE_WARP_TO_MOUNTAIN_VILLAGE,
    /* 0x20 */ OCARINA_MODE_WARP_TO_SOUTH_CLOCK_TOWN,
    /* 0x21 */ OCARINA_MODE_WARP_TO_MILK_ROAD,
    /* 0x22 */ OCARINA_MODE_WARP_TO_WOODFALL,
    /* 0x23 */ OCARINA_MODE_WARP_TO_SOUTHERN_SWAMP,
    /* 0x24 */ OCARINA_MODE_WARP_TO_IKANA_CANYON,
    /* 0x25 */ OCARINA_MODE_WARP_TO_STONE_TOWER,
    /* 0x26 */ OCARINA_MODE_WARP_TO_ENTRANCE,
    /* 0x27 */ OCARINA_MODE_PROCESS_RESTRICTED_SONG, // Variants of Song of Time, Soaring, or Elegy restricted
    /* 0x28 */ OCARINA_MODE_28,
    /* 0x29 */ OCARINA_MODE_29,
    /* 0x2A */ OCARINA_MODE_PLAYED_FULL_EVAN_SONG
} OcarinaMode;

typedef enum OcarinaButtonIndex {
    /*  0 */ OCARINA_BTN_A,
    /*  1 */ OCARINA_BTN_C_DOWN,
    /*  2 */ OCARINA_BTN_C_RIGHT,
    /*  3 */ OCARINA_BTN_C_LEFT,
    /*  4 */ OCARINA_BTN_C_UP,
    /*  5 */ OCARINA_BTN_C_RIGHT_OR_C_LEFT,
    /* -1 */ OCARINA_BTN_INVALID = 0xFF
} OcarinaButtonIndex;

// Uses scientific pitch notation relative to middle C
// https://en.wikipedia.org/wiki/Scientific_pitch_notation
typedef enum OcarinaPitch {
    /* 0x0 */ OCARINA_PITCH_C4,
    /* 0x1 */ OCARINA_PITCH_DFLAT4,
    /* 0x2 */ OCARINA_PITCH_D4,
    /* 0x3 */ OCARINA_PITCH_EFLAT4,
    /* 0x4 */ OCARINA_PITCH_E4,
    /* 0x5 */ OCARINA_PITCH_F4,
    /* 0x6 */ OCARINA_PITCH_GFLAT4,
    /* 0x7 */ OCARINA_PITCH_G4,
    /* 0x8 */ OCARINA_PITCH_AFLAT4,
    /* 0x9 */ OCARINA_PITCH_A4,
    /* 0xA */ OCARINA_PITCH_BFLAT4,
    /* 0xB */ OCARINA_PITCH_B4,
    /* 0xC */ OCARINA_PITCH_C5,
    /* 0xD */ OCARINA_PITCH_DFLAT5,
    /* 0xE */ OCARINA_PITCH_D5,
    /* 0xF */ OCARINA_PITCH_EFLAT5,
    /*  -1 */ OCARINA_PITCH_NONE = 0xFF
} OcarinaPitch;

typedef enum OcarinaInstrumentId {
    /*  0 */ OCARINA_INSTRUMENT_OFF,
    /*  1 */ OCARINA_INSTRUMENT_DEFAULT,
    /*  2 */ OCARINA_INSTRUMENT_FEMALE_VOICE,
    /*  3 */ OCARINA_INSTRUMENT_WHISTLING_FLUTE,
    /*  4 */ OCARINA_INSTRUMENT_HARP,
    /*  5 */ OCARINA_INSTRUMENT_IKANA_KING,
    /*  6 */ OCARINA_INSTRUMENT_TATL, // Sounds like bells
    /*  7 */ OCARINA_INSTRUMENT_GORON_DRUMS,
    /*  8 */ OCARINA_INSTRUMENT_ZORA_GUITAR,
    /*  9 */ OCARINA_INSTRUMENT_DEKU_PIPES,
    /* 10 */ OCARINA_INSTRUMENT_MONKEY,
    /* 11 */ OCARINA_INSTRUMENT_DEKU_TRUMPET, // Pull out ocarina for captured monkey
    /* 12 */ OCARINA_INSTRUMENT_ELDER_GORON_DRUMS,
    /* 13 */ OCARINA_INSTRUMENT_PIANO,
    /* 14 */ OCARINA_INSTRUMENT_BASS_GUITAR,
    /* 15 */ OCARINA_INSTRUMENT_BABY_SINGING,
    /* 16 */ OCARINA_INSTRUMENT_AMPLIFIED_GUITAR // Related to WEEKEVENTREG_41_20
} OcarinaInstrumentId;

typedef enum OcarinaRecordingState {
    /*  0 */ OCARINA_RECORD_OFF,
    /*  1 */ OCARINA_RECORD_SCARECROW_LONG,
    /*  2 */ OCARINA_RECORD_SCARECROW_SPAWN,
    /* -1 */ OCARINA_RECORD_REJECTED = 0xFF
} OcarinaRecordingState;

typedef enum OcarinaError {
    /*    0 */ OCARINA_ERROR_NONE,
    /*    1 */ OCARINA_ERROR_1,
    /*    2 */ OCARINA_ERROR_2,
    /*    3 */ OCARINA_ERROR_3,
    /*    4 */ OCARINA_ERROR_BEND,
    /*    5 */ OCARINA_ERROR_5,
    /* 0xFF */ OCARINA_ERROR_INACTIVE = 0xFF // Song not available
} OcarinaError;

/**
 * bFlat4Flag Note:
 * Flag for resolving whether (pitch = OCARINA_PITCH_BFLAT4) 
 * gets mapped to either C_RIGHT and C_LEFT
 * 
 * This is required as C_RIGHT and C_LEFT are the only notes
 * that map to two semitones apart (OCARINA_PITCH_A4 and OCARINA_PITCH_B4)
 *      0x40 - BTN_Z is pressed to lower note by a semitone
 *      0x80 - BTN_R is pressed to raise note by a semitone
 */ 
#define OCARINA_BUTTON_MASK_PITCH 0x3F
#define OCARINA_BUTTON_MASK_FLAG 0xC0
#define OCARINA_BUTTON_FLAG_BFLAT_LOWER 0x40
#define OCARINA_BUTTON_FLAG_BFLAT_RAISE 0x80

typedef struct {
    /* 0x0 */ u8 pitch; // number of semitones above middle C
    /* 0x2 */ u16 length; // number of frames the note is sustained
    /* 0x4 */ u8 volume;
    /* 0x5 */ u8 vibrato;
    /* 0x6 */ s8 bend; // frequency multiplicative offset from the pitch defined by pitch
    /* 0x7 */ u8 bFlat4Flag; // See note above
} OcarinaNote;  // size = 0x8

typedef struct {
    /* 0x0 */ u8 numButtons;
    /* 0x1 */ u8 buttonIndex[8];
} OcarinaSongButtons; // size = 0x9

typedef struct OcarinaStaff {
    /* 0x0 */ u8 buttonIndex;
    /* 0x1 */ u8 state;   // original name: "status"
    /* 0x2 */ u8 pos;     // original name: "locate"
} OcarinaStaff; // size = 0x3

void AudioOcarina_SetSongStartingPos(void);
void AudioOcarina_StartAtSongStartingPos(u32 ocarinaFlags);
void AudioOcarina_StartForSongCheck(u32 ocarinaFlags, u8 ocarinaStaffPlayingPosStart);
void AudioOcarina_StartWithSongNoteLengths(u32 ocarinaFlags);
void AudioOcarina_StartDefault(u32 ocarinaFlags);
u8 func_8019B5AC(void);
void AudioOcarina_ResetAndReadInput(void);
void AudioOcarina_SetOcarinaDisableTimer(u8 unused, u8 timer);
void AudioOcarina_SetInstrument(u8 ocarinaInstrumentId);
void AudioOcarina_SetPlaybackSong(s8 songIndexPlusOne, u8 playbackState);
void AudioOcarina_SetRecordingState(u8 recordingState);
OcarinaStaff* AudioOcarina_GetRecordingStaff(void);
OcarinaStaff* AudioOcarina_GetPlayingStaff(void);
OcarinaStaff* AudioOcarina_GetPlaybackStaff(void);
void AudioOcarina_TerminaWallGenerateNotes(void);
void AudioOcarina_PlayLongScarecrowSong(void);

extern u8* gScarecrowSpawnSongPtr;
extern OcarinaSongButtons gOcarinaSongButtons[OCARINA_SONG_MAX];

#endif
