#include "global.h"
#include "z64voice.h"

#include "GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <libultraship/bridge/audiobridge.h>

typedef struct {
    /* 0x0 */ s8 x;
    /* 0x1 */ s8 y;
} OcarinaControlStick; // size = 0x2

typedef struct {
    /* 0x0 */ Vec3f* pos;
    /* 0x4 */ f32 freqScale;
    /* 0x8 */ s8 reverbAdd;
} SfxSettings; // size = 0xC

typedef struct {
    /* 0x0 */ f32 volume;
    /* 0x4 */ f32 freqScale;
    /* 0x8 */ s8 reverb;
    /* 0x9 */ s8 panSigned;
    /* 0xA */ s8 stereoBits;
    /* 0xB */ u8 filter;
    /* 0xC */ u8 combFilterGain;
    /* 0xD */ u8 zVolume;
} SfxChannelState; // size = 0x10

typedef enum {
    /* 0x0 */ SFX_CHANNEL_PLAYER0, // SfxPlayerBank
    /* 0x1 */ SFX_CHANNEL_PLAYER1,
    /* 0x2 */ SFX_CHANNEL_PLAYER2,
    /* 0x3 */ SFX_CHANNEL_ITEM0, // SfxItemBank
    /* 0x4 */ SFX_CHANNEL_ITEM1,
    /* 0x5 */ SFX_CHANNEL_ENV0, // SfxEnvironmentBank
    /* 0x6 */ SFX_CHANNEL_ENV1,
    /* 0x7 */ SFX_CHANNEL_ENV2,
    /* 0x8 */ SFX_CHANNEL_ENEMY0, // SfxEnemyBank
    /* 0x9 */ SFX_CHANNEL_ENEMY1,
    /* 0xA */ SFX_CHANNEL_ENEMY2,
    /* 0xB */ SFX_CHANNEL_SYSTEM0, // SfxSystemBank
    /* 0xC */ SFX_CHANNEL_SYSTEM1,
    /* 0xD */ SFX_CHANNEL_OCARINA, // SfxOcarinaBank
    /* 0xE */ SFX_CHANNEL_VOICE0,  // SfxVoiceBank
    /* 0xF */ SFX_CHANNEL_VOICE1
} SfxChannelIndex; // seqPlayerIndex = 2

// Global IO ports for sequences, 8 global ports per seqPlayer
typedef enum {
    /* 0x0 */ SEQ_PLAYER_IO_PORT_0,
    /* 0x1 */ SEQ_PLAYER_IO_PORT_1,
    /* 0x2 */ SEQ_PLAYER_IO_PORT_2,
    /* 0x3 */ SEQ_PLAYER_IO_PORT_3,
    /* 0x4 */ SEQ_PLAYER_IO_PORT_4,
    /* 0x5 */ SEQ_PLAYER_IO_PORT_5,
    /* 0x6 */ SEQ_PLAYER_IO_PORT_6,
    /* 0x7 */ SEQ_PLAYER_IO_PORT_7
} SeqPlayerIOPort;

typedef struct {
    /* 0x0 */ f32 value;
    /* 0x4 */ f32 target;
    /* 0x8 */ f32 step;
    /* 0xC */ s32 remainingFrames;
} FreqLerp; // size = 0x10

s32 AudioOcarina_MemoryGameNextNote(void);

void AudioSfx_ProcessSfxSettings(void);
void Audio_UpdateSfxVolumeTransition(void);
void Audio_StepFreqLerp(FreqLerp* lerp);

s32 Audio_SetGanonsTowerBgmVolume(u8 targetVolume);

void Audio_StartMorningSceneSequence(u16 seqId);
void Audio_StartSceneSequence(u16 seqId);
void Audio_PlaySequenceWithSeqPlayerIO(s8 seqPlayerIndex, u16 seqId, u8 fadeInDuration, s8 ioPort, u8 ioData);
void Audio_SetSfxReverbIndexExceptOcarinaBank(u8 reverbIndex);
void Audio_MuteBgmPlayersForFanfare(void);
void Audio_PlayAmbience(u8 ambienceId);
void Audio_SetSfxVolumeExceptSystemAndOcarinaBanks(u8 volume);

void Audio_UpdateRiverSoundVolumes(void);
void Audio_UpdateObjSoundProperties(void);
void Audio_UpdateObjSoundFanfare(void);
void Audio_UpdateSubBgmAtPos(void);
void Audio_UpdateSequenceAtPos(void);
void Audio_UpdateSceneSequenceResumePoint(void);
void Audio_UpdateFanfare(void);
void Audio_UpdatePauseState(void);
void Audio_ResetRequestedSceneSeqId(void);

// Sfx bss
SfxSettings sSfxSettings[8];
u8 sSfxSettingsFlags;
f32 sTwoSemitonesLoweredFreq;
s8 sSfxIncreasedReverb;
f32 sSfxSyncedVolume;
f32 sSfxSyncedVolumeForMetalEffects;
f32 sSfxSyncedFreq;
FreqLerp sRiverFreqScaleLerp;
FreqLerp sWaterfallFreqScaleLerp;
f32 sSfxAdjustedFreq;
s8 sSfxCustomReverb;
u8 sRiverSoundMainBgmVol;
u8 sRiverSoundMainBgmCurrentVol;
u8 sRiverSoundMainBgmLower;
u8 sRiverSoundMainBgmRestore;
u8 sGanonsTowerVol;
f32* sSfxVolumeCur;
f32 sSfxVolumeTarget;
f32 sSfxVolumeRate;
u16 sRequestedSceneSeqId;
SfxChannelState sSfxChannelState[SEQ_NUM_CHANNELS];

// Sequence bss
u8 sRomaniSingingTimer;
u8 sRomaniSingingDisabled;
u8 sRiverSoundBgmTimer;
u8 sFanfareState;
u16 sFanfareSeqId;
u8 sMuteOnlySfxAndAmbienceSeq;
u8 sAllPlayersMutedExceptSystemAndOcarina;

typedef enum {
    /* 0 */ AUDIO_PAUSE_STATE_CLOSED,
    /* 1 */ AUDIO_PAUSE_STATE_CLOSING,
    /* 2 */ AUDIO_PAUSE_STATE_OPEN
} AudioPauseState;

u8 sAudioPauseState;
u8 sSpatialSeqIsActive[4];
u8 sSequenceFilter[8 * 4];
u8 sIsFinalHoursOrSoaring;
u16 sObjSoundFanfareSeqId;
u8 sObjSoundFanfareRequested;
Vec3f sObjSoundFanfarePos;
u8 sObjSoundPlayerIndex;
Vec3f sObjSoundPos;
s16 sObjSoundFlags;
f32 sObjSoundMinDist;
f32 sObjSoundMaxDist;
f32 sObjSoundMaxVol;
f32 sObjSoundMinVol;
Vec3f sSpatialSeqNoFilterPos;
Vec3f sSpatialSeqFilterPos;
f32 sSpatialSeqMaxDist;
u16 sSpatialSeqSeqId;
u8 sSpatialSeqFlags;
u8 D_801FD432;
u8 sSpatialSubBgmFadeTimer;
u8 D_801FD434;
u8 sSpatialSeqPlayerIndex;
u8 sSpatialSeqFadeTimer;
u16 sPrevAmbienceSeqId;

// AudioOcarina bss
OcarinaStaff sPlayingStaff;
OcarinaStaff sPlaybackStaff;
OcarinaStaff sRecordingStaff;
u32 sOcarinaUpdateTaskStart;
OcarinaControlStick sOcarinaInputStickRel;
u32 sOcarinaInputButtonCur;
u32 sOcarinaInputButtonStart;
u32 sOcarinaInputButtonPrev;
s32 sOcarinaInputButtonPress;
u8 sOcarinaDisableTimer;
u8 sOcarinaUnused;
u8 sOcarinaHasStartedSong;
u8 sFirstOcarinaSongIndex;
u8 sLastOcarinaSongIndex;
u32 sOcarinaAvailableSongFlags;
u8 sOcarinaStaffPlayingPos;
u16 sMusicStaffPos[OCARINA_SONG_MAX];
u16 sMusicStaffCurHeldLength[OCARINA_SONG_MAX];
u16 sMusicStaffExpectedLength[OCARINA_SONG_MAX];
u8 sMusicStaffExpectedPitch[OCARINA_SONG_MAX]; // Next required pitch in song playback
u8 D_801FD518[OCARINA_SONG_MAX];
u32 D_801FD530[OCARINA_SONG_MAX];
OcarinaNote sScarecrowsLongSongSecondNote;
u16 sCustomSequencePc;

// Sfx Data
// If the Sfx Bank has more than 255 values (max value of u8)
u8 gIsLargeSfxBank[] = {
    true, false, true, true, false, false, true,
};
u8 D_801D6608[] = {
    true, true, true, true, true, false, true,
};
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
f32 sGiantsMaskFreq = 0.89167805f; // Around 2 semitones down in pitch
s8 sGiantsMaskReverbAdd = 0x40;
f32 sWaterWheelVolume = 0.65f;
f32 gSfxVolume = 1.0f;
s8 sSfxTimer = 20;
s8 sSfxTimerLerpRange2 = 30;
s8 sSfxTimerLerpRange1 = 20;
f32 sBehindScreenZ[2] = { -15.0f, -65.0f }; // Unused Remnant of OoT
u8 sAudioIncreasingTranspose = 0;           // Remnant of OoT, only unsed in unused functions
u8 gMorphaTransposeTable[16] = { 0, 0, 0, 1, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8 }; // Unused Remnant of OoT
u8 sPrevChargeLevel = 0;
f32 sChargeLevelsSfxFreq[] = { 1.0f, 1.12246f, 1.33484f, 1.33484f };
f32 sCurChargeLevelSfxFreq = 1.0f;
u8 sGanonsTowerLevelsVol[] = {
    127, 80, 75, 73, 70, 68, 65, 60 // volumes
};
u8 sEnterGanonsTowerTimer = 0;
u16 sSfxVolumeDuration = 0;

// System Data
s8 sSoundMode = SOUNDMODE_STEREO;
s8 sAudioPauseMenuOpenOrClose = SFX_PAUSE_MENU_CLOSE;
s8 sAudioCutsceneFlag = false;
s8 sSpecReverb = 0;
s8 sAudioEnvReverb = 0;
s8 sAudioCodeReverb = 0;

// Sequence Data
u8 sPrevSeqMode = 0;
f32 sBgmEnemyDist = 0.0f;
s8 sBgmEnemyVolume = 127;
u16 sPrevMainBgmSeqId = NA_BGM_DISABLED;

#define SEQ_RESUME_POINT_NONE 0xC0
u8 sSeqResumePoint = 0;
u16 sPrevSceneSeqId = NA_BGM_GENERAL_SFX;

u32 sNumFramesStill = 0;
u32 sNumFramesMoving = 0;
u8 sAudioBaseFilter = 0;
u8 sAudioExtraFilter = 0;
u8 sAudioBaseFilter2 = 0;
u8 sAudioExtraFilter2 = 0;
s8 gUnderwaterSfxReverbAdd = 0;
Vec3f* sRiverSoundBgmPos = NULL;
f32 sRiverSoundXZDistToPlayer = 2000.0f;
u16 sObjSoundMainBgmSeqId = NA_BGM_GENERAL_SFX;

// Weights distance strongest by depth (z), weaker by left/right screen (x), and weakest by top/bottom screen (y)
#define SEQ_SCREEN_WEIGHTED_DIST(projectedPos) \
    (sqrtf(SQ((projectedPos)->z) + ((SQ((projectedPos)->x) / 4.0f) + (SQ((projectedPos)->y) / 6.0f))))

// Allows enemy bgm
#define SEQ_FLAG_ENEMY (1 << 0)

#define SEQ_FLAG_FANFARE (1 << 1)
#define SEQ_FLAG_FANFARE_KAMARO (1 << 2)

// required for Audio_RestorePrevBgm to restore a sequence after Audio_PlayBgm_StorePrevBgm
#define SEQ_FLAG_RESTORE (1 << 3)

/**
 * These two sequence flags work together to implement a “resume playing from where you left off” system for scene
 * sequences when leaving and returning to a scene. For a scene to resume playing from the point where it left off, it
 * must have `SEQ_FLAG_RESUME` attached to it. Then, if the scene changes and the new scene sequence contain
 * `SEQ_FLAG_RESUME_PREV`, the point from the previous scene sequence will be stored. Then, when returning to the
 * scene with the sequence `SEQ_FLAG_RESUME`, then the sequence will resume playing from where it left off.
 *
 * TODO: Confirm which sequences properly implement this system after sequence extraction
 */
#define SEQ_FLAG_RESUME (1 << 4)
#define SEQ_FLAG_RESUME_PREV (1 << 5)

/**
 * Will write a value of 1 to ioPort 7 when called through the scene. How it's used depends on the sequence:
 * NA_BGM_FILE_SELECT - ioPort 7 skips the harp intro when a value of 1 is written to it.
 */
#define SEQ_FLAG_SKIP_HARP_INTRO (1 << 6)

// Unused, repurposed for SubBgm
#define SEQ_FLAG_NO_AMBIENCE (1 << 7)

u8 sSeqFlags[] = {
    SEQ_FLAG_FANFARE | SEQ_FLAG_ENEMY, // NA_BGM_GENERAL_SFX
    SEQ_FLAG_ENEMY,                    // NA_BGM_AMBIENCE
    SEQ_FLAG_ENEMY,                    // NA_BGM_TERMINA_FIELD
    SEQ_FLAG_RESTORE,                  // NA_BGM_CHASE
    0,                                 // NA_BGM_MAJORAS_THEME
    0,                                 // NA_BGM_CLOCK_TOWER
    SEQ_FLAG_ENEMY,                    // NA_BGM_STONE_TOWER_TEMPLE
    SEQ_FLAG_ENEMY,                    // NA_BGM_INV_STONE_TOWER_TEMPLE
    SEQ_FLAG_FANFARE,                  // NA_BGM_FAILURE_0
    SEQ_FLAG_FANFARE,                  // NA_BGM_FAILURE_1
    0,                                 // NA_BGM_HAPPY_MASK_SALESMAN
    0,                                 // NA_BGM_SONG_OF_HEALING
    SEQ_FLAG_ENEMY,                    // NA_BGM_SWAMP_REGION
    0,                                 // NA_BGM_ALIEN_INVASION
    SEQ_FLAG_FANFARE,                  // NA_BGM_SWAMP_CRUISE
    0,                                 // NA_BGM_SHARPS_CURSE
    SEQ_FLAG_ENEMY,                    // NA_BGM_GREAT_BAY_REGION
    SEQ_FLAG_ENEMY,                    // NA_BGM_IKANA_REGION
    0,                                 // NA_BGM_DEKU_PALACE
    SEQ_FLAG_ENEMY,                    // NA_BGM_MOUNTAIN_REGION
    0,                                 // NA_BGM_PIRATES_FORTRESS
    0,                                 // NA_BGM_CLOCK_TOWN_DAY_1
    0,                                 // NA_BGM_CLOCK_TOWN_DAY_2
    0,                                 // NA_BGM_CLOCK_TOWN_DAY_3
    SEQ_FLAG_SKIP_HARP_INTRO,          // NA_BGM_FILE_SELECT
    SEQ_FLAG_RESUME,                   // NA_BGM_CLEAR_EVENT
    0,                                 // NA_BGM_ENEMY
    SEQ_FLAG_RESTORE,                  // NA_BGM_BOSS
    SEQ_FLAG_ENEMY,                    // NA_BGM_WOODFALL_TEMPLE
    0,                                 // NA_BGM_CLOCK_TOWN_MAIN_SEQUENCE
    0,                                 // NA_BGM_OPENING
    SEQ_FLAG_RESUME_PREV,              // NA_BGM_INSIDE_A_HOUSE
    SEQ_FLAG_FANFARE,                  // NA_BGM_GAME_OVER
    0,                                 // NA_BGM_CLEAR_BOSS
    SEQ_FLAG_FANFARE,                  // NA_BGM_GET_ITEM
    SEQ_FLAG_FANFARE,                  // NA_BGM_CLOCK_TOWN_DAY_2_PTR
    SEQ_FLAG_FANFARE,                  // NA_BGM_GET_HEART
    SEQ_FLAG_RESTORE,                  // NA_BGM_TIMED_MINI_GAME
    0,                                 // NA_BGM_GORON_RACE
    0,                                 // NA_BGM_MUSIC_BOX_HOUSE
    0,                                 // NA_BGM_FAIRY_FOUNTAIN
    0,                                 // NA_BGM_ZELDAS_LULLABY
    0,                                 // NA_BGM_ROSA_SISTERS
    SEQ_FLAG_FANFARE,                  // NA_BGM_OPEN_CHEST
    0,                                 // NA_BGM_MARINE_RESEARCH_LAB
    SEQ_FLAG_SKIP_HARP_INTRO,          // NA_BGM_GIANTS_THEME
    0,                                 // NA_BGM_SONG_OF_STORMS
    0,                                 // NA_BGM_ROMANI_RANCH
    0,                                 // NA_BGM_GORON_VILLAGE
    0,                                 // NA_BGM_MAYORS_OFFICE
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_EPONA
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_SUNS
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_TIME
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_STORM
    SEQ_FLAG_RESUME,                   // NA_BGM_ZORA_HALL
    SEQ_FLAG_FANFARE,                  // NA_BGM_GET_NEW_MASK
    SEQ_FLAG_RESTORE,                  // NA_BGM_MINI_BOSS
    SEQ_FLAG_FANFARE,                  // NA_BGM_GET_SMALL_ITEM
    0,                                 // NA_BGM_ASTRAL_OBSERVATORY
    SEQ_FLAG_ENEMY,                    // NA_BGM_CAVERN
    SEQ_FLAG_RESUME | SEQ_FLAG_ENEMY,  // NA_BGM_MILK_BAR
    SEQ_FLAG_FANFARE,                  // NA_BGM_ZELDA_APPEAR
    0,                                 // NA_BGM_SARIAS_SONG
    0,                                 // NA_BGM_GORON_GOAL
    0,                                 // NA_BGM_HORSE
    0,                                 // NA_BGM_HORSE_GOAL
    0,                                 // NA_BGM_INGO
    0,                                 // NA_BGM_KOTAKE_POTION_SHOP
    SEQ_FLAG_RESUME_PREV,              // NA_BGM_SHOP
    SEQ_FLAG_FANFARE,                  // NA_BGM_OWL
    SEQ_FLAG_RESUME_PREV,              // NA_BGM_SHOOTING_GALLERY
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_SOARING
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_HEALING
    SEQ_FLAG_FANFARE,                  // NA_BGM_INVERTED_SONG_OF_TIME
    SEQ_FLAG_FANFARE,                  // NA_BGM_SONG_OF_DOUBLE_TIME
    SEQ_FLAG_FANFARE,                  // NA_BGM_SONATA_OF_AWAKENING
    SEQ_FLAG_FANFARE,                  // NA_BGM_GORON_LULLABY
    SEQ_FLAG_FANFARE,                  // NA_BGM_NEW_WAVE_BOSSA_NOVA
    SEQ_FLAG_FANFARE,                  // NA_BGM_ELEGY_OF_EMPTINESS
    SEQ_FLAG_FANFARE,                  // NA_BGM_OATH_TO_ORDER
    0,                                 // NA_BGM_SWORD_TRAINING_HALL
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_LULLABY_INTRO
    SEQ_FLAG_FANFARE,                  // NA_BGM_LEARNED_NEW_SONG
    SEQ_FLAG_FANFARE,                  // NA_BGM_BREMEN_MARCH
    SEQ_FLAG_FANFARE,                  // NA_BGM_BALLAD_OF_THE_WIND_FISH
    SEQ_FLAG_RESTORE,                  // NA_BGM_SONG_OF_SOARING
    0,                                 // NA_BGM_MILK_BAR_DUPLICATE
    0,                                 // NA_BGM_FINAL_HOURS
    SEQ_FLAG_FANFARE,                  // NA_BGM_MIKAU_RIFF
    SEQ_FLAG_FANFARE,                  // NA_BGM_MIKAU_FINALE
    0,                                 // NA_BGM_FROG_SONG
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_SONATA
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_LULLABY
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_NEW_WAVE
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_ELEGY
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_OATH
    0,                                 // NA_BGM_MAJORAS_LAIR
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_LULLABY_INTRO_PTR
    SEQ_FLAG_FANFARE,                  // NA_BGM_OCARINA_GUITAR_BASS_SESSION
    SEQ_FLAG_FANFARE,                  // NA_BGM_PIANO_SESSION
    SEQ_FLAG_FANFARE,                  // NA_BGM_INDIGO_GO_SESSION
    SEQ_FLAG_ENEMY,                    // NA_BGM_SNOWHEAD_TEMPLE
    SEQ_FLAG_ENEMY,                    // NA_BGM_GREAT_BAY_TEMPLE
    SEQ_FLAG_FANFARE,                  // NA_BGM_NEW_WAVE_SAXOPHONE
    SEQ_FLAG_FANFARE,                  // NA_BGM_NEW_WAVE_VOCAL
    0,                                 // NA_BGM_MAJORAS_WRATH
    0,                                 // NA_BGM_MAJORAS_INCARNATION
    0,                                 // NA_BGM_MAJORAS_MASK
    SEQ_FLAG_FANFARE,                  // NA_BGM_BASS_PLAY
    SEQ_FLAG_FANFARE,                  // NA_BGM_DRUMS_PLAY
    SEQ_FLAG_FANFARE,                  // NA_BGM_PIANO_PLAY
    SEQ_FLAG_ENEMY,                    // NA_BGM_IKANA_CASTLE
    0,                                 // NA_BGM_GATHERING_GIANTS
    SEQ_FLAG_FANFARE_KAMARO,           // NA_BGM_KAMARO_DANCE
    0,                                 // NA_BGM_CREMIA_CARRIAGE
    SEQ_FLAG_FANFARE,                  // NA_BGM_KEATON_QUIZ
    0,                                 // NA_BGM_END_CREDITS_1
    0,                                 // NA_BGM_OPENING_LOOP
    0,                                 // NA_BGM_TITLE_THEME
    SEQ_FLAG_FANFARE,                  // NA_BGM_DUNGEON_APPEAR
    SEQ_FLAG_FANFARE,                  // NA_BGM_WOODFALL_CLEAR
    SEQ_FLAG_FANFARE,                  // NA_BGM_SNOWHEAD_CLEAR
    0,                                 //
    0,                                 // NA_BGM_INTO_THE_MOON
    0,                                 // NA_BGM_GOODBYE_GIANT
    0,                                 // NA_BGM_TATL_AND_TAEL
    0,                                 // NA_BGM_MOONS_DESTRUCTION
    0,                                 // NA_BGM_END_CREDITS_SECOND_HALF
};

s8 sSpecReverbs[] = {
    0, 0, 0, 0, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

#define AMBIENCE_CHANNEL_PROPERTIES_ENTRIES_MAX 33

typedef struct {
    /* 0x0 */ u16 initChannelMask;     // bitwise flag for 16 channels, channels to initialize
    /* 0x2 */ u16 initMuteChannelMask; // bitwise flag for 16 channels, channels to mute upon initialization
    /* 0x4 */ u8 channelProperties[3 * AMBIENCE_CHANNEL_PROPERTIES_ENTRIES_MAX + 1];
} AmbienceDataIO; // size = 0x68

AmbienceDataIO sAmbienceData[20] = {
    // AMBIENCE_ID_00
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_09),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_10),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_03),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_00),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_5_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_01
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_16),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_12),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_15),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_06),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_5_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_02
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(48),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_10),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(48),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(32),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_03),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(0),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_5_PORT5(63),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_03
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_09),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_10),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(32),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(32),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_14),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_00),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_5_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_04
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_02),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_12),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_13),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_02),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_5_PORT5(48),

            // Channel 14
            AMBIENCE_IO_RAIN_PORT4(63),

            // End
            AMBIENCE_IO_ENTRIES_END,

        },
    },

    // AMBIENCE_ID_05
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_09),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(64),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_10),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_13),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_3_PORT5(63),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_00),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_5_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_06
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) |
         (1 << AMBIENCE_CHANNEL_CRITTER_5) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) | (1 << AMBIENCE_CHANNEL_CRITTER_2) |
         (1 << AMBIENCE_CHANNEL_CRITTER_3) | (1 << AMBIENCE_CHANNEL_CRITTER_4) | (1 << AMBIENCE_CHANNEL_CRITTER_5) |
         (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_11),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_0_PORT5(48),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_15),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_1_PORT5(63),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_11),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(48),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(16),

            // Channel 4
            AMBIENCE_IO_CRITTER_3_TYPE(AMBIENCE_CRITTER_14),
            AMBIENCE_IO_CRITTER_3_BEND_PITCH(48),
            AMBIENCE_IO_CRITTER_3_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            AMBIENCE_IO_CRITTER_4_TYPE(AMBIENCE_CRITTER_11),
            AMBIENCE_IO_CRITTER_4_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_4_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_4_PORT5(32),

            // Channel 6
            AMBIENCE_IO_CRITTER_5_TYPE(AMBIENCE_CRITTER_02),
            AMBIENCE_IO_CRITTER_5_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_5_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_5_PORT5(48),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_07
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_08
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_09
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_16),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(2),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_12),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(0),
            AMBIENCE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_15),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0A
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_6) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_1),
            AMBIENCE_IO_STREAM_0_PORT3(8),

            // Channel 7
            AMBIENCE_IO_CRITTER_6_TYPE(AMBIENCE_CRITTER_11),
            AMBIENCE_IO_CRITTER_6_BEND_PITCH(112),
            AMBIENCE_IO_CRITTER_6_NUM_LAYERS(2),
            AMBIENCE_IO_CRITTER_6_PORT5(32),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0B
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_CRITTER_2) | (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_3),
            AMBIENCE_IO_STREAM_0_PORT3(8),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_00),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(2),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            AMBIENCE_IO_CRITTER_2_TYPE(AMBIENCE_CRITTER_06),
            AMBIENCE_IO_CRITTER_2_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_2_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_2_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0C
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_CRITTER_1) |
         (1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_00),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // Channel 2
            AMBIENCE_IO_CRITTER_1_TYPE(AMBIENCE_CRITTER_01),
            AMBIENCE_IO_CRITTER_1_BEND_PITCH(127),
            AMBIENCE_IO_CRITTER_1_NUM_LAYERS(3),
            AMBIENCE_IO_CRITTER_1_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0D
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0E
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_0F
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_10
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_11
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_12
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            // Channel 0
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),

            // End
            AMBIENCE_IO_ENTRIES_END,
        },
    },

    // AMBIENCE_ID_13
    {
        // Init channel mask
        ((1 << AMBIENCE_CHANNEL_STREAM_0) | (1 << AMBIENCE_CHANNEL_CRITTER_0) | (1 << AMBIENCE_CHANNEL_RAIN) |
         (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        // Init mute channel mask
        ((1 << AMBIENCE_CHANNEL_RAIN) | (1 << AMBIENCE_CHANNEL_LIGHTNING)),
        {
            AMBIENCE_IO_STREAM_0_TYPE(AMBIENCE_STREAM_0),
            AMBIENCE_IO_STREAM_0_PORT3(0),

            // Channel 1
            AMBIENCE_IO_CRITTER_0_TYPE(AMBIENCE_CRITTER_04),
            AMBIENCE_IO_CRITTER_0_BEND_PITCH(0),
            AMBIENCE_IO_CRITTER_0_NUM_LAYERS(1),
            AMBIENCE_IO_CRITTER_0_PORT5(16),

            // End
            AMBIENCE_IO_ENTRIES_END,

        },
    },
};

// AudioOcarina Data
u8 sIsOcarinaInputEnabled = false;
s8 sOcarinaInstrumentId = OCARINA_INSTRUMENT_OFF;
u8 sCurOcarinaPitch = OCARINA_PITCH_NONE;
u8 sPrevOcarinaPitch = OCARINA_PITCH_C4;
u8 sCurOcarinaButtonIndex = OCARINA_BTN_A;
u8 sMusicStaffPrevPitch = OCARINA_PITCH_C4;
f32 sCurOcarinaBendFreq = 1.0f;
f32 sDefaultOcarinaVolume = 87.0f / 127.0f;
s8 sCurOcarinaBendIndex = 0;
s8 sCurOcarinaVolume = 87;
s8 sCurOcarinaVibrato = 0;
u8 sPlaybackState = 0;
u8 sPlaybackStaffStopPos = 0xFF;
u8 sPlaybackStaffStartPos = 0xFF;
u32 sOcarinaFlags = 0;
s32 sPlaybackNoteTimer = 0;
u16 sPlaybackNotePos = 0;
u16 sPlaybackStaffPos = 0;
u32 sPrevOcarinaWithMusicStaffFlags = 0; // Stores sOcarinaFlags but never used
u8 sPlaybackPitch = OCARINA_PITCH_NONE;  // Pitch + PitchFlags
u8 sNotePlaybackVolume = 0;
u8 sNotePlaybackVibrato = 0;
s8 sNotePlaybackBend = 0;
f32 sRelativeNotePlaybackBend = 1.0f;
f32 sRelativeNotePlaybackVolume = 1.0f;
u32 sOcarinaPlaybackTaskStart = 0;
u32 sOcarinaWallCounter = 0;
u8 sCurOcarinaSongWithoutMusicStaff[8] = {
    OCARINA_PITCH_C4, OCARINA_PITCH_C4, OCARINA_PITCH_C4, OCARINA_PITCH_C4,
    OCARINA_PITCH_C4, OCARINA_PITCH_C4, OCARINA_PITCH_C4, OCARINA_PITCH_C4,
};
u8 sOcarinaWithoutMusicStaffPos = 0;
u8 sOcarinaSongStartingPos = 0;

u8 sButtonToPitchMap[5] = {
    OCARINA_PITCH_D4, // OCARINA_BTN_A
    OCARINA_PITCH_F4, // OCARINA_BTN_C_DOWN
    OCARINA_PITCH_A4, // OCARINA_BTN_C_RIGHT
    OCARINA_PITCH_B4, // OCARINA_BTN_C_LEFT
    OCARINA_PITCH_D5, // OCARINA_BTN_C_UP
};

u8 sOcarinaMemoryGameAppendPos = 0;
u8 sOcarinaMemoryGameEndPos = 0;
u8 sOcarinaMemoryGameNumNotes[] = { 5, 6, 8 };
OcarinaNote sOcarinaSongNotes[OCARINA_SONG_MAX][20] = {
    // OCARINA_SONG_SONATA
    {
        { OCARINA_PITCH_D5, 19, 92, 0, 0, 0 },
        { OCARINA_PITCH_B4, 19, 90, 0, 0, 0 },
        { OCARINA_PITCH_D5, 19, 90, 0, 0, 0 },
        { OCARINA_PITCH_B4, 38, 90, 0, 0, 0 },
        { OCARINA_PITCH_D4, 39, 92, 0, 0, 0 },
        { OCARINA_PITCH_A4, 76, 89, 0, 0, 0 },
        { OCARINA_PITCH_D4, 77, 82, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 86, 0, 0, 0 },
    },

    // OCARINA_SONG_GORON_LULLABY
    {
        { OCARINA_PITCH_D4, 41, 80, 0, 0, 0 },
        { OCARINA_PITCH_A4, 40, 72, 0, 0, 0 },
        { OCARINA_PITCH_B4, 39, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 42, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 40, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 39, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 41, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 80, 76, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 40, 76, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_NEW_WAVE
    {
        { OCARINA_PITCH_B4, 64, 74, 0, 0, 0 },
        { OCARINA_PITCH_D5, 13, 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, 12, 90, 0, 0, 0 },
        { OCARINA_PITCH_A4, 78, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 12, 76, 0, 0, 0 },
        { OCARINA_PITCH_B4, 13, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 114, 76, 6, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_ELEGY
    {
        { OCARINA_PITCH_A4, 85, 93, 0, 0, 0 },
        { OCARINA_PITCH_B4, 43, 91, 0, 0, 0 },
        { OCARINA_PITCH_A4, 43, 93, 0, 0, 0 },
        { OCARINA_PITCH_F4, 21, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 21, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 43, 101, 0, 0, 0 },
        { OCARINA_PITCH_B4, 85, 95, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 94, 0, 0, 0 },
    },

    // OCARINA_SONG_OATH
    {
        { OCARINA_PITCH_A4, 97, 104, 0, 0, 0 },
        { OCARINA_PITCH_F4, 48, 88, 0, 0, 0 },
        { OCARINA_PITCH_D4, 49, 78, 0, 0, 0 },
        { OCARINA_PITCH_F4, 49, 78, 0, 0, 0 },
        { OCARINA_PITCH_A4, 48, 94, 0, 0, 0 },
        { OCARINA_PITCH_D5, 97, 100, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 96, 0, 0, 0 },

    },

    // OCARINA_SONG_SARIAS
    {
        { OCARINA_PITCH_F4, 17, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 17, 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, 34, 80, 0, 0, 0 },
        { OCARINA_PITCH_F4, 17, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 17, 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, 136, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_TIME
    {
        { OCARINA_PITCH_A4, 32, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 65, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 33, 80, 0, 0, 0 },
        { OCARINA_PITCH_A4, 32, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 65, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 99, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_HEALING
    {
        { OCARINA_PITCH_B4, 32, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 33, 69, 0, 0, 0 },
        { OCARINA_PITCH_B4, 32, 94, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 121, 86, 2, 0, 0 },
        { OCARINA_PITCH_NONE, 10, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_EPONAS
    {
        { OCARINA_PITCH_D5, 18, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 72, 80, 0, 0, 0 },
        { OCARINA_PITCH_D5, 18, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 144, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SOARING
    {
        { OCARINA_PITCH_F4, 18, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 80, 0, 0, 0 },
        { OCARINA_PITCH_D5, 36, 94, 0, 0, 0 },
        { OCARINA_PITCH_F4, 18, 73, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 76, 0, 0, 0 },
        { OCARINA_PITCH_D5, 108, 96, 2, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_STORMS
    {
        { OCARINA_PITCH_D4, 11, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 11, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 45, 80, 0, 0, 0 },
        { OCARINA_PITCH_D4, 11, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 11, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 90, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SUNS
    {
        { OCARINA_PITCH_A4, 12, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 13, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 29, 80, 2, 0, 0 },
        { OCARINA_PITCH_NONE, 9, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 12, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 13, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 120, 80, 3, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_INVERTED_TIME
    {
        { OCARINA_PITCH_F4, 32, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 65, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 80, 0, 0, 0 },
        { OCARINA_PITCH_F4, 32, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 65, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 99, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_DOUBLE_TIME
    {
        { OCARINA_PITCH_A4, 29, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 30, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 29, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 30, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 29, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 99, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
    },

    // OCARINA_SONG_GORON_LULLABY_INTRO
    {
        { OCARINA_PITCH_D4, 32, 78, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 90, 0, 0, 0 },
        { OCARINA_PITCH_B4, 33, 87, 0, 0, 0 },
        { OCARINA_PITCH_D4, 32, 92, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 86, 0, 0, 0 },
        { OCARINA_PITCH_B4, 130, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_WIND_FISH_HUMAN
    {
        { OCARINA_PITCH_D5, 89, 80, 0, 0, 0 },
        { OCARINA_PITCH_A4, 41, 72, 0, 0, 0 },
        { OCARINA_PITCH_B4, 22, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 91, 76, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 30, 66, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_WIND_FISH_GORON
    {
        { OCARINA_PITCH_D4, 52, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_D4, 8, 72, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_D4, 30, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_F4, 34, 76, 0, 0, 0 },
        { OCARINA_PITCH_D4, 52, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_D4, 8, 72, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_D4, 30, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 66, 0, 0, 0 },
        { OCARINA_PITCH_A4, 34, 76, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_WIND_FISH_ZORA
    {
        { OCARINA_PITCH_D5, 11, 80, 0, 0, 0 },
        { OCARINA_PITCH_A4, 11, 72, 0, 0, 0 },
        { OCARINA_PITCH_F4, 11, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 100, 76, 0, 0, 0 },
        { OCARINA_PITCH_D5, 11, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 11, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 11, 72, 0, 0, 0 },
        { OCARINA_PITCH_F4, 100, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_WIND_FISH_DEKU
    {
        { OCARINA_PITCH_A4, 54, 80, 0, 0, 0 },
        { OCARINA_PITCH_D4, 77, 72, 0, 0, 0 },
        { OCARINA_PITCH_F4, 19, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 20, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 78, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_EVAN_PART1
    {
        { OCARINA_PITCH_A4, 33, 100, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 92, 0, 0, 0 },
        { OCARINA_PITCH_A4, 37, 104, 0, 0, 0 },
        { OCARINA_PITCH_F4, 24, 100, 0, 0, 0 },
        { OCARINA_PITCH_D4, 70, 97, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 96, 0, 0, 0 },
        { OCARINA_PITCH_D4, 12, 93, 0, 0, 0 },
        { OCARINA_PITCH_F4, 12, 100, 0, 0, 0 },
        { OCARINA_PITCH_A4, 12, 62, 0, 0, 0 },
        { OCARINA_PITCH_D4, 170, 91, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_EVAN_PART2
    {
        { OCARINA_PITCH_B4, 33, 107, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 100, 0, 0, 0 },
        { OCARINA_PITCH_B4, 37, 104, 0, 0, 0 },
        { OCARINA_PITCH_A4, 24, 97, 0, 0, 0 },
        { OCARINA_PITCH_F4, 70, 104, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 104, 0, 0, 0 },
        { OCARINA_PITCH_F4, 12, 90, 0, 0, 0 },
        { OCARINA_PITCH_A4, 12, 96, 0, 0, 0 },
        { OCARINA_PITCH_B4, 12, 81, 0, 0, 0 },
        { OCARINA_PITCH_F4, 170, 66, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_ZELDAS_LULLABY
    {
        { OCARINA_PITCH_B4, 51, 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, 25, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 78, 80, 0, 0, 0 },
        { OCARINA_PITCH_B4, 51, 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, 25, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 100, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SCARECROW_SPAWN
    {
        { OCARINA_PITCH_D4, 3, 0, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 255, 0, 0, 0 },
    },

    // OCARINA_SONG_TERMINA_WALL
    {
        { OCARINA_PITCH_D4, 3, 0, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 255, 0, 0, 0 },
    },
};

OcarinaNote sOoTOcarinaSongNotes[9][20] = {
    // Minuet
    {
        { OCARINA_PITCH_D4, 18, 86, 0, 0, 0 },
        { OCARINA_PITCH_D5, 18, 92, 0, 0, 0 },
        { OCARINA_PITCH_B4, 72, 86, 0, 0, 0 },
        { OCARINA_PITCH_A4, 18, 80, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 144, 86, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 86, 0, 0, 0 },
    },

    // Bolero
    {
        { OCARINA_PITCH_F4, 15, 80, 0, 0, 0 },
        { OCARINA_PITCH_D4, 15, 72, 0, 0, 0 },
        { OCARINA_PITCH_F4, 15, 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, 15, 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, 15, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 15, 74, 0, 0, 0 },
        { OCARINA_PITCH_A4, 15, 78, 0, 0, 0 },
        { OCARINA_PITCH_F4, 135, 66, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // Serenade
    {
        { OCARINA_PITCH_D4, 36, 60, 0, 0, 0 },
        { OCARINA_PITCH_F4, 36, 78, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 82, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 82, 0, 0, 0 },
        { OCARINA_PITCH_A4, 36, 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, 144, 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // Requiem
    {
        { OCARINA_PITCH_D4, 45, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 23, 86, 0, 0, 0 },
        { OCARINA_PITCH_D4, 22, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 45, 86, 0, 0, 0 },
        { OCARINA_PITCH_F4, 45, 94, 0, 0, 0 },
        { OCARINA_PITCH_D4, 180, 94, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 94, 0, 0, 0 },
    },

    // Nocturne
    {
        { OCARINA_PITCH_B4, 36, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 33, 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 3, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 18, 82, 0, 0, 0 },
        { OCARINA_PITCH_D4, 18, 60, 0, 0, 0 },
        { OCARINA_PITCH_B4, 18, 90, 0, 0, 0 },
        { OCARINA_PITCH_A4, 18, 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, 144, 96, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 96, 0, 0, 0 },
    },

    // Prelude
    {
        { OCARINA_PITCH_D5, 15, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 45, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 15, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 15, 82, 0, 0, 0 },
        { OCARINA_PITCH_B4, 15, 86, 0, 0, 0 },
        { OCARINA_PITCH_D5, 60, 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 75, 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // Sarias
    {
        { OCARINA_PITCH_F4, 17, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 17, 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, 34, 80, 0, 0, 0 },
        { OCARINA_PITCH_F4, 17, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 17, 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, 136, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // Zeldas Lullaby
    {
        { OCARINA_PITCH_B4, 51, 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, 25, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 78, 80, 0, 0, 0 },
        { OCARINA_PITCH_B4, 51, 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, 25, 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, 100, 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // Suns Song
    {
        { OCARINA_PITCH_A4, 12, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 13, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 29, 80, 2, 0, 0 },
        { OCARINA_PITCH_NONE, 9, 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, 12, 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, 13, 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, 120, 80, 3, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },
};

u8 sOoTOcarinaSongsNumNotes[] = {
    6, // Minuet
    8, // Bolero
    5, // Serenade
    6, // Requiem
    7, // Nocturne
    6, // Prelude
    6, // Sarias
    6, // Zeldas Lullaby
    6, // Suns Song
};

OcarinaNote* sPlaybackSong = sOcarinaSongNotes[0];
u8 sFrogsSongNotes[14] = {
    OCARINA_BTN_A,       OCARINA_BTN_C_LEFT,  OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_DOWN, OCARINA_BTN_C_LEFT,
    OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_DOWN,  OCARINA_BTN_A,       OCARINA_BTN_C_DOWN, OCARINA_BTN_A,
    OCARINA_BTN_C_DOWN,  OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_LEFT,  OCARINA_BTN_A,
};
u8* gFrogsSongPtr = sFrogsSongNotes;
u8 sRecordingState = OCARINA_RECORD_OFF;
u8 sRecordSongPos = 0;
u32 sOcarinaRecordTaskStart = 0;
u8 sRecordOcarinaPitch = 0;
u8 sRecordOcarinaVolume = 0;
u8 sRecordOcarinaVibrato = 0;
s8 sRecordOcarinaBendIndex = 0;
u8 sRecordOcarinaButtonIndex = 0;
u8 sPlayedOcarinaSongIndexPlusOne = 0;
u8 sMusicStaffNumNotesPerTest = 0;
u8 D_801D8530 = false;
u32 D_801D8534 = 0;
u8 sOcarinaDropInputTimer = 0;

OcarinaNote sScarecrowsLongSongNotes[108] = {
    { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
    { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
};

OcarinaNote* gScarecrowLongSongPtr = sScarecrowsLongSongNotes;
u8* gScarecrowSpawnSongPtr = (u8*)&sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN];
OcarinaNote* sTerminaWallSongPtr = sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL];

u8 sPitchToButtonMap[16] = {
    OCARINA_BTN_A,                 // OCARINA_PITCH_C4
    OCARINA_BTN_A,                 // OCARINA_PITCH_DFLAT4
    OCARINA_BTN_A,                 // OCARINA_PITCH_D4
    OCARINA_BTN_A,                 // OCARINA_PITCH_EFLAT4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_E4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_F4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_GFLAT4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_G4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_AFLAT4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_A4
    OCARINA_BTN_C_RIGHT_OR_C_LEFT, // OCARINA_PITCH_BFLAT4: Interface/Overlap between C_RIGHT and C_LEFT
    OCARINA_BTN_C_LEFT,            // OCARINA_PITCH_B4
    OCARINA_BTN_C_LEFT,            // OCARINA_PITCH_C5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_DFLAT5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_D5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_EFLAT5
};

// seqData written in the music macro language
// Only used in unused functions
// clang-format off
u8 sCustomSequenceScript[400] = {
    /* 0 */ 0xFE, // delay1
    /* 1 */ 0xFE, // delay1
    /* 2 */ 0xD3, 0x20, // mutebhv 0x20
    /* 4 */ 0xD7, 0x00, 0x01, // initchan 0x0001
    /* 7 */ 0xCC, 0x00, // ldi 0
    /* 9 */ 0x70, // stio 0
    /* 10 */ 0x90, 0x00, 22, // ldchan 0, chan0
    /* 13 */ 0xDB, 100, // vol 100
    /* 15 */ 0xDD, 120, // tempo 120

    // .sequence seq_loop
    /* 17 */ 0xFE, // delay1
    /* 18 */ 0x00, // testchan 0
    /* 19 */ 0xF3, -4, // rbeqz seq_loop
    /* 21 */ 0xFF, // end

    // .channel chan0
    /* 22 */ 0xC3, // short
    /* 23 */ 0x88, 0x00, 41, // ldlayer 0, layer0
    /* 26 */ 0x89, 0x00, 43, // ldlayer 1, layer1
    /* 29 */ 0xDF, 127, // vol 127
    /* 31 */ 0xE9, 0x0F, // notepri 0x0F
    /* 33 */ 0xDD, 55, // pan 55
    /* 35 */ 0xD4, 64, // reverb 64
    /* 37 */ 0xC1, 82, // instr 82

    // .channel chan_loop
    /* 39 */ 0xFE, // delay1
    /* 40 */ 0x80, // testlayer 0

    // (no end or loop; channel script seems to run into the note layers?)

    // .layer layer0
    /* 41 */ 0xF3, -4, // rbeqz chan_loop

    // .layer layer1
    /* 43 */ 0xFF, // end

    // .layer layer2?
    /* 44 */ 0xC2, -5, // transpose -5
    /* 46 */ 0xC0, 0, // ldelay 0
    /* 48 */ 0xC1, 87, // shortvel 87
    /* 50 */ 0xC9, 0, // shortgate 0
    /* 52 */ 0, // empty until the end
};
// clang-format on

OcarinaSongButtons gOcarinaSongButtons[OCARINA_SONG_MAX] = {
    // OCARINA_SONG_SONATA
    {
        7,
        {
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
        },
    },

    // OCARINA_SONG_GORON_LULLABY
    {
        8,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
        },
    },

    // OCARINA_SONG_NEW_WAVE
    {
        7,
        {
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_ELEGY
    {
        7,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
        },
    },

    // OCARINA_SONG_OATH
    {
        6,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_UP,
        },
    },

    // OCARINA_SONG_SARIAS
    {
        6,
        {
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
        },
    },

    // OCARINA_SONG_TIME
    {
        6,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
        },
    },

    // OCARINA_SONG_HEALING
    {
        6,
        {
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
        },
    },

    // OCARINA_SONG_EPONAS
    {
        6,
        {
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_SOARING
    {
        6,
        {
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
        },
    },

    // OCARINA_SONG_STORMS
    {
        6,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_UP,
        },
    },

    // OCARINA_SONG_SUNS
    {
        6,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_UP,
        },
    },

    // OCARINA_SONG_INVERTED_TIME
    {
        6,
        {
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_DOUBLE_TIME
    {
        6,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_DOWN,
        },
    },

    // OCARINA_SONG_GORON_LULLABY_INTRO
    {
        6,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
        },
    },

    // OCARINA_SONG_WIND_FISH_HUMAN
    {
        4,
        {
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_WIND_FISH_GORON
    {
        8,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_WIND_FISH_ZORA
    {
        8,
        {
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
        },
    },

    // OCARINA_SONG_WIND_FISH_DEKU
    {
        5,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_EVAN_PART1
    {
        8,
        {
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_A,
        },
    },

    // OCARINA_SONG_EVAN_PART2
    {
        8,
        {
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_DOWN,
        },
    },

    // OCARINA_SONG_ZELDAS_LULLABY
    {
        6,
        {
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_RIGHT,
        },
    },

    // OCARINA_SONG_SCARECROW_SPAWN
    {
        8,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
            OCARINA_BTN_A,
        },
    },

    // OCARINA_SONG_TERMINA_WALL
    {
        8,
        {
            OCARINA_BTN_A,
            OCARINA_BTN_C_DOWN,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_UP,
            OCARINA_BTN_C_LEFT,
            OCARINA_BTN_C_RIGHT,
            OCARINA_BTN_C_DOWN,
        },
    },
};

const u16 gAudioEnvironmentalSfx[] = {
    NA_SE_EV_RIVER_STREAM - SFX_FLAG,   NA_SE_EV_WAVE - SFX_FLAG,
    NA_SE_EV_WATER_WALL_BIG - SFX_FLAG, NA_SE_EV_WATER_WALL - SFX_FLAG,
    NA_SE_EV_MAGMA_LEVEL - SFX_FLAG,    NA_SE_EV_MAGMA_LEVEL_M - SFX_FLAG,
    NA_SE_EV_MAGMA_LEVEL_L - SFX_FLAG,  NA_SE_EV_TORCH - SFX_FLAG,
    NA_SE_EV_FOUNTAIN - SFX_FLAG,       NA_SE_EV_DRAIN - SFX_FLAG,
    NA_SE_EV_CROWD - SFX_FLAG,          NA_SE_EV_WATER_CONVECTION - SFX_FLAG,
    NA_SE_EV_GORON_CHEER - SFX_FLAG,    NA_SE_EV_WATER_WALL_BIG_SILENT - SFX_FLAG,
    NA_SE_EV_WATER_BUBBLE - SFX_FLAG,   NA_SE_EV_COW_CRY_LV - SFX_FLAG,
    NA_SE_EV_MAKE_TURRET - SFX_FLAG,    NA_SE_EV_BOILED_WATER_S - SFX_FLAG,
    NA_SE_EV_BOILED_WATER_L - SFX_FLAG, NA_SE_EV_WAVE_S - SFX_FLAG,
    NA_SE_EV_WAVE_S - SFX_FLAG,         NA_SE_EV_WAVE_S - SFX_FLAG,
};

const u8 sIsOcarinaSongReserved[OCARINA_SONG_MAX] = {
    true,  // OCARINA_SONG_SONATA
    true,  // OCARINA_SONG_GORON_LULLABY
    true,  // OCARINA_SONG_NEW_WAVE
    true,  // OCARINA_SONG_ELEGY
    true,  // OCARINA_SONG_OATH
    false, // OCARINA_SONG_SARIAS
    true,  // OCARINA_SONG_TIME
    true,  // OCARINA_SONG_HEALING
    true,  // OCARINA_SONG_EPONAS
    true,  // OCARINA_SONG_SOARING
    true,  // OCARINA_SONG_STORMS
    false, // OCARINA_SONG_SUNS
    true,  // OCARINA_SONG_INVERTED_TIME
    true,  // OCARINA_SONG_DOUBLE_TIME
    true,  // OCARINA_SONG_GORON_LULLABY_INTRO
    false, // OCARINA_SONG_WIND_FISH_HUMAN
    false, // OCARINA_SONG_WIND_FISH_GORON
    false, // OCARINA_SONG_WIND_FISH_ZORA
    false, // OCARINA_SONG_WIND_FISH_DEKU
    true,  // OCARINA_SONG_EVAN_PART1
    true,  // OCARINA_SONG_EVAN_PART2
    false, // OCARINA_SONG_ZELDAS_LULLABY
    true,  // OCARINA_SONG_SCARECROW_SPAWN
    false, // OCARINA_SONG_TERMINA_WALL
};

const char sAudioOcarinaUnusedText0[] = "key step is too long !!! %d:%d>%d\n";
const char sAudioOcarinaUnusedText1[] = "You played %d Melody !!! (last step:%d)\n";
const char sAudioOcarinaUnusedText2[] = "pass 0\n";
const char sAudioOcarinaUnusedText3[] = "pass 1\n";
const char sAudioOcarinaUnusedText4[] = "pass 2\n";
const char sAudioOcarinaUnusedText5[] = "last key is bad !!! %d %d %02X %02X\n";
const char sAudioOcarinaUnusedText6[] = "last key step is too short !!! %d:%d %d<%d\n";
const char sAudioOcarinaUnusedText7[] = "check is over!!! %d %d %d\n";

void AudioOcarina_ReadControllerInput(void) {
    Input inputs[MAXCONTROLLERS];
    Input* input = &inputs[0];
    u32 ocarinaInputButtonPrev = sOcarinaInputButtonCur;

    PadMgr_GetInput2(inputs, false);
    sOcarinaInputButtonCur = input->cur.button;
    sOcarinaInputButtonPrev = ocarinaInputButtonPrev;
    sOcarinaInputStickRel.x = input->rel.stick_x;
    sOcarinaInputStickRel.y = input->rel.stick_y;

    // 2S2H [Enhancement] When custom ocarina controls are enabled, block regular ocarina inputs
    if (CVarGetInteger("gEnhancements.Playback.CustomizeOcarinaControls", 0)) {
        sOcarinaInputButtonCur &= ~(BTN_A | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT | BTN_L | BTN_R | BTN_Z);
    }

    // 2S2H [Enhancement] Apply right stick ocarina input via GameInteractor
    sOcarinaInputButtonCur |= GameInteractor_RightStickOcarina(input);

    // 2S2H [Enhancement] Apply custom ocarina controls via GameInteractor
    sOcarinaInputButtonCur |= GameInteractor_CustomOcarinaControls(input);
}

/**
 * Looks up the frequency to bend the pitch by.
 * The pitch will bend up to a maximum of 2 semitones
 * in each direction giving a total range of 4 semitones
 */
f32 AudioOcarina_BendPitchTwoSemitones(s8 bendIndex) {
    s8 adjBendIndex;
    f32 bendFreq;

    if (bendIndex > 64) {
        adjBendIndex = 127;
    } else if (bendIndex < -64) {
        adjBendIndex = -128;
    } else if (bendIndex >= 0) {
        adjBendIndex = (bendIndex * 127) / 64;
    } else {
        adjBendIndex = (bendIndex * 128) / 64;
    }

    /**
     * index 128 is in the middle of the table and
     * contains the value 1.0f i.e. no bend
     * absolute indices above 128 will bend the pitch 2 semitones upwards
     * absolute indices below 128 will bend the pitch 2 semitones downwards
     */
    bendFreq = gBendPitchTwoSemitonesFrequencies[adjBendIndex + 128];
    return bendFreq;
}

/**
 * If an available song has been played, then return that song index
 * If the ocarina is on, but no song has been played then return 0xFE
 * If the ocarina is off, return 0xFF
 */
u8 AudioOcarina_GetPlayingState(void) {
    u8 playedOcarinaSongIndex;

    if (sPlayedOcarinaSongIndexPlusOne != 0) {
        playedOcarinaSongIndex = sPlayedOcarinaSongIndexPlusOne - 1;
        sPlayedOcarinaSongIndexPlusOne = 0;
    } else if (sOcarinaFlags != 0) {
        playedOcarinaSongIndex = 0xFE;
    } else {
        playedOcarinaSongIndex = 0xFF;
    }

    return playedOcarinaSongIndex;
}

u8 AudioOcarina_MapPitchToButton(u8 pitchAndBFlatFlag) {
    u8 buttonIndex = sPitchToButtonMap[pitchAndBFlatFlag & 0x3F];

    /**
     * Special case for bFlat4:
     * CRIGHT and CLEFT are the only two pitches that are 2 semitones apart
     * which are pitches A4 and B4 respectively
     * bFlat4 is in the middle of those two and is the only pitch that can not
     * be resolved between the two buttons without external information.
     * That information is stored as flags in pitch with the mask:
     * (pitchAndBFlatFlag & 0xC0)
     */
    if (buttonIndex == OCARINA_BTN_C_RIGHT_OR_C_LEFT) {
        if (pitchAndBFlatFlag & OCARINA_BUTTON_FLAG_BFLAT_RAISE) {
            return OCARINA_BTN_C_RIGHT;
        }
        // OCARINA_BUTTON_FLAG_BFLAT_LOWER
        return OCARINA_BTN_C_LEFT;
    }

    return buttonIndex;
}

void AudioOcarina_MapSongFromPitchToButton(u8 noteSongIndex, u8 buttonSongIndex, u8 numButtons) {
    u8 buttonSongPos = 0;
    u8 noteSongPos = 0;
    u8 pitch;

    while ((buttonSongPos < numButtons) && (noteSongPos < 16)) {
        pitch = sOcarinaSongNotes[noteSongIndex][noteSongPos++].pitch;

        if (pitch != OCARINA_PITCH_NONE) {
            gOcarinaSongButtons[buttonSongIndex].buttonIndex[buttonSongPos++] = sPitchToButtonMap[pitch];
        }
    }

    gOcarinaSongButtons[buttonSongIndex].numButtons = numButtons;
}

/**
 * Ocarina flags:
 * bitmask 0x3FFF:
 *      - Ocarina song id
 * bitmask 0xC000:
 *      - 0x0000: Limits the notes to 8 notes at a time. Not playing a correct song after 8 notes will cause an ocarina
 * error
 *      - 0x4000: (Identical to 0xC000)
 *      - 0x8000: Limits the notes to 1 note at a time. A single incorrect note will cause an ocarina error
 *      - 0xC000: Free-play, no limitations to the number of notes to play
 * bitmask 0x7FFF0000:
 *      - ocarina action (only used to make flags != 0)
 * bitmask 0x80000000:
 *      - unused (only used to make flags != 0)
 */

// The ocarina songs that can be performed at any time = 0x3FFF
#define OCARINA_SONGS_PLAYABLE_FLAGS                                                                                \
    ((1 << OCARINA_SONG_SONATA) | (1 << OCARINA_SONG_GORON_LULLABY) | (1 << OCARINA_SONG_NEW_WAVE) |                \
     (1 << OCARINA_SONG_ELEGY) | (1 << OCARINA_SONG_OATH) | (1 << OCARINA_SONG_SARIAS) | (1 << OCARINA_SONG_TIME) | \
     (1 << OCARINA_SONG_HEALING) | (1 << OCARINA_SONG_EPONAS) | (1 << OCARINA_SONG_SOARING) |                       \
     (1 << OCARINA_SONG_STORMS) | (1 << OCARINA_SONG_SUNS) | (1 << OCARINA_SONG_INVERTED_TIME) |                    \
     (1 << OCARINA_SONG_DOUBLE_TIME))

void AudioOcarina_Start(u32 ocarinaFlags) {
    u8 songIndex;

    if ((sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF) &&
        ((ocarinaFlags & OCARINA_SONGS_PLAYABLE_FLAGS) == OCARINA_SONGS_PLAYABLE_FLAGS)) {
        ocarinaFlags |= (1 << OCARINA_SONG_SCARECROW_SPAWN);
    }

    if ((ocarinaFlags == (0xC0000000 | OCARINA_SONGS_PLAYABLE_FLAGS)) &&
        (sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF)) {
        ocarinaFlags = (0xC0000000 | OCARINA_SONGS_PLAYABLE_FLAGS);
    }

    if ((ocarinaFlags == OCARINA_SONGS_PLAYABLE_FLAGS) &&
        (sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF)) {
        ocarinaFlags = OCARINA_SONGS_PLAYABLE_FLAGS;
    }

    if (ocarinaFlags != 0xFFFF) {
        sOcarinaFlags = 0x80000000 + ocarinaFlags;
        sFirstOcarinaSongIndex = 0;
        sLastOcarinaSongIndex = OCARINA_SONG_MAX;
        sOcarinaAvailableSongFlags = ocarinaFlags & 0xFFFFFF;
        sMusicStaffNumNotesPerTest = 8; // Ocarina Check
        sOcarinaHasStartedSong = false;
        sPlayedOcarinaSongIndexPlusOne = 0;
        sOcarinaStaffPlayingPos = 0;
        sPlayingStaff.state = AudioOcarina_GetPlayingState();
        sIsOcarinaInputEnabled = true;
        sPrevOcarinaWithMusicStaffFlags = 0;

        // Reset music staff song check
        for (songIndex = OCARINA_SONG_SONATA; songIndex <= OCARINA_SONG_EVAN_PART2; songIndex++) {
            for (sMusicStaffPos[songIndex] = 0;
                 sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]].pitch == OCARINA_PITCH_NONE;) {
                sMusicStaffPos[songIndex]++;
            }

            sMusicStaffCurHeldLength[songIndex] = 0;
            sMusicStaffExpectedLength[songIndex] = 0;
            sMusicStaffExpectedPitch[songIndex] = 0;

            if (D_801D8530) {
                if ((1 << songIndex) & ocarinaFlags) {
                    D_801FD518[songIndex] = 0;
                } else {
                    D_801FD518[songIndex] = 0xFF;
                }
            }
        }

        if (ocarinaFlags & 0x80000000) {
            sMusicStaffNumNotesPerTest = 0; // Ocarina Playback
        }

        if (ocarinaFlags & 0x40000000) {
            sOcarinaWithoutMusicStaffPos = 0;
        }

        if (ocarinaFlags & 0xC0400000) {
            AudioOcarina_MapSongFromPitchToButton(OCARINA_SONG_SCARECROW_SPAWN, OCARINA_SONG_SCARECROW_SPAWN, 8);
        }
    } else {
        sOcarinaFlags = 0;
        sIsOcarinaInputEnabled = false;
    }
}

#undef OCARINA_SONGS_PLAYABLE_FLAGS

void AudioOcarina_SetSongStartingPos(void) {
    sOcarinaSongStartingPos = sOcarinaWithoutMusicStaffPos;
}

void AudioOcarina_StartAtSongStartingPos(u32 ocarinaFlags) {
    D_801D8530 = false;
    AudioOcarina_Start(ocarinaFlags);
    if (sOcarinaSongStartingPos != 0) {
        sOcarinaWithoutMusicStaffPos = sOcarinaSongStartingPos;
        sOcarinaSongStartingPos = 0;
    }
}

void AudioOcarina_StartForSongCheck(u32 ocarinaFlags, u8 ocarinaStaffPlayingPosStart) {
    u8 songIndex;
    u8 j;

    AudioOcarina_Start(ocarinaFlags);
    sMusicStaffNumNotesPerTest = 8;
    sOcarinaStaffPlayingPos = ocarinaStaffPlayingPosStart;

    for (songIndex = 0; songIndex <= OCARINA_SONG_EVAN_PART2; songIndex++) {
        for (j = 0; j < ocarinaStaffPlayingPosStart;) {
            if (sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]].pitch != OCARINA_PITCH_NONE) {
                j++;
            }
            sMusicStaffPos[songIndex]++;
        }

        if (sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]].pitch == OCARINA_PITCH_NONE) {
            sMusicStaffPos[songIndex]++;
        }
    }
}

void AudioOcarina_StartWithSongNoteLengths(u32 ocarinaFlags) {
    u8 songIndex;

    for (songIndex = 0; songIndex < OCARINA_SONG_MAX; songIndex++) {
        if ((1 << songIndex) & ocarinaFlags) {
            D_801FD530[songIndex] = 0;
        } else {
            D_801FD530[songIndex] = 0xFF;
        }
    }

    D_801D8530 = true;
    D_801D8534 = ocarinaFlags;
    AudioOcarina_Start(ocarinaFlags);
}

void AudioOcarina_StartDefault(u32 ocarinaFlags) {
    D_801D8530 = false;
    AudioOcarina_Start(ocarinaFlags);
}

u8 func_8019B568(void) {
    u32 temp_v0 = D_801D8534;
    u8 songIndex;

    for (songIndex = OCARINA_SONG_SONATA; !((temp_v0 >> songIndex) & 1) && (songIndex < OCARINA_SONG_MAX);
         songIndex++) {}

    return songIndex;
}

u8 func_8019B5AC(void) {
    u8 songIndex = func_8019B568();

    if (songIndex < OCARINA_SONG_MAX) {
        return D_801FD518[songIndex];
    }

    return 0xFF;
}

u8 func_8019B5EC(void) {
    return D_801FD530[func_8019B568()];
}

void AudioOcarina_CheckIfStartedSong(void) {
    if ((sCurOcarinaPitch != OCARINA_PITCH_NONE) && !sOcarinaHasStartedSong) {
        sOcarinaHasStartedSong = true;
        sMusicStaffPrevPitch = OCARINA_PITCH_NONE;
    }
}

void AudioOcarina_UpdateCurOcarinaSong(void) {
    u8 i;

    if (sOcarinaWithoutMusicStaffPos == 8) {
        for (i = 0; i < ARRAY_COUNT(sCurOcarinaSongWithoutMusicStaff) - 1; i++) {
            sCurOcarinaSongWithoutMusicStaff[i] = sCurOcarinaSongWithoutMusicStaff[i + 1];
        }
    } else {
        sOcarinaWithoutMusicStaffPos++;
    }
}

/**
 * Checks for ocarina songs from user input with a music staff prompt
 * Type 1) Playback: tests note-by-note (ocarinaFlag & 0xC000 == 0x8000) eg:
 *      - learning a new song
 *      - playing the ocarina memory game
 * Type 2) Check: tests in 8-note chunks (ocarinaFlag & 0xC000 == 0x0000) eg:
 *      - validating scarecrow spawn song as adult
 *      - ocarina prompt for zelda's lullaby, saria's song, Storms, Song of Time, etc...
 */
void AudioOcarina_CheckSongsWithMusicStaff(void) {
    u32 curOcarinaSongFlag;
    s32 pad;
    u8 noNewValidInput = false;
    s16 pad2;
    s8 staffOcarinaPlayingPosOffset = 0;
    u8 songIndex;
    OcarinaNote* curNote;
    OcarinaNote* nextNote;

    AudioOcarina_CheckIfStartedSong();

    if (!sOcarinaHasStartedSong) {
        return;
    }

    if (ABS_ALT(sCurOcarinaBendIndex) > 20) {
        sOcarinaFlags = 0;
        for (songIndex = OCARINA_SONG_SONATA; songIndex < OCARINA_SONG_MAX; songIndex++) {
            D_801FD518[songIndex] = 4;
        }
        sIsOcarinaInputEnabled = false;
        return;
    }

    if ((sPrevOcarinaPitch == sCurOcarinaPitch) || (sCurOcarinaPitch == OCARINA_PITCH_NONE)) {
        noNewValidInput = true;
    } else {
        AudioOcarina_UpdateCurOcarinaSong();
        sCurOcarinaSongWithoutMusicStaff[sOcarinaWithoutMusicStaffPos - 1] = sCurOcarinaPitch;
    }

    for (songIndex = sFirstOcarinaSongIndex; songIndex < sLastOcarinaSongIndex; songIndex++) {
        curOcarinaSongFlag = 1 << songIndex;

        if (sOcarinaAvailableSongFlags & curOcarinaSongFlag) {
            if (D_801D8530) {
                sMusicStaffCurHeldLength[songIndex] += sOcarinaUpdateTaskStart - sOcarinaPlaybackTaskStart;
            } else {
                sMusicStaffCurHeldLength[songIndex] = sMusicStaffExpectedLength[songIndex] + 9;
            }

            if (noNewValidInput) {
                if ((sMusicStaffCurHeldLength[songIndex] > 8) &&
                    (sMusicStaffPrevPitch != sMusicStaffExpectedPitch[songIndex])) {
                    sOcarinaAvailableSongFlags ^= curOcarinaSongFlag;
                    if (D_801D8530) {
                        D_801FD518[songIndex] = 1;
                    }
                }

                if (D_801D8530 && ((sMusicStaffExpectedLength[songIndex] + 9) < sMusicStaffCurHeldLength[songIndex])) {
                    sOcarinaAvailableSongFlags ^= curOcarinaSongFlag;
                    D_801FD518[songIndex] = 3;
                } else if ((sMusicStaffCurHeldLength[songIndex] >= (sMusicStaffExpectedLength[songIndex] - 9)) &&
                           (sMusicStaffCurHeldLength[songIndex] >= sMusicStaffExpectedLength[songIndex]) &&
                           (sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]].length == 0) &&
                           (sMusicStaffPrevPitch == sMusicStaffExpectedPitch[songIndex])) {
                    // This case is taken if the song is finished and successfully played
                    // (i.e. .length == 0 indicates that the song is at the end)
                    sPlayedOcarinaSongIndexPlusOne = songIndex + 1;
                    sIsOcarinaInputEnabled = false;
                    sOcarinaFlags = 0;
                    if (D_801D8530) {
                        D_801FD518[songIndex] = 5;
                    }
                }
            } else if (sMusicStaffCurHeldLength[songIndex] >= (sMusicStaffExpectedLength[songIndex] - 9)) {
                // This else-if statement always holds true, taken if a new note is played
                if (sMusicStaffPrevPitch != OCARINA_PITCH_NONE) {
                    // New note is played
                    if (sMusicStaffPrevPitch == sMusicStaffExpectedPitch[songIndex]) {
                        // Note is part of expected song
                        if (D_801D8530) {
                            D_801FD530[songIndex] +=
                                ABS_ALT(sMusicStaffExpectedLength[songIndex] - sMusicStaffCurHeldLength[songIndex]);
                        }
                        sMusicStaffCurHeldLength[songIndex] = 0;
                    } else {
                        // Note is not part of expected song, so this song is no longer available as an option in this
                        // playback
                        sOcarinaAvailableSongFlags ^= curOcarinaSongFlag;
                        if (D_801D8530) {
                            D_801FD518[songIndex] = 1;
                        }
                    }
                }

                curNote = &sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]];
                nextNote = &sOcarinaSongNotes[songIndex][++sMusicStaffPos[songIndex]];
                sMusicStaffExpectedLength[songIndex] = curNote->length;
                sMusicStaffExpectedPitch[songIndex] = curNote->pitch;

                while ((curNote->pitch == nextNote->pitch) ||
                       ((nextNote->pitch == OCARINA_PITCH_NONE) && (nextNote->length != 0))) {
                    sMusicStaffExpectedLength[songIndex] += nextNote->length;
                    curNote = &sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]];
                    nextNote = &sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex] + 1];
                    sMusicStaffPos[songIndex]++;
                }
            } else if (sMusicStaffCurHeldLength[songIndex] < 9) {
                // case never taken
                staffOcarinaPlayingPosOffset = -1;
                sMusicStaffCurHeldLength[songIndex] = 0;
                sMusicStaffPrevPitch = sCurOcarinaPitch;
            } else {
                // case never taken
                if (sOcarinaSongNotes[songIndex][sMusicStaffPos[songIndex]].length == 0) {
                    D_801FD518[songIndex] = 1;
                } else if (D_801D8530) {
                    D_801FD518[songIndex] = 2;
                }
                sOcarinaAvailableSongFlags ^= curOcarinaSongFlag;
            }
        }

        // if a note is played that doesn't match a song, the song bit in sAvailOcarinaSongFlags is turned off
        // if there are no more songs remaining that it could be and the maximum position has been exceeded, then
        if ((sOcarinaAvailableSongFlags == 0) && (sOcarinaStaffPlayingPos >= sMusicStaffNumNotesPerTest)) {
            sIsOcarinaInputEnabled = false;
            if (!D_801D8530) {
                if ((sOcarinaFlags & 0x40000000) && (sCurOcarinaPitch == sOcarinaSongNotes[songIndex][0].pitch)) {
                    // case never taken, this function is not called if (sOcarinaFlags & 0x40000000) is set
                    sPrevOcarinaWithMusicStaffFlags = sOcarinaFlags;
                }
            }
            sOcarinaFlags = 0;
            return;
        }
    }

    if (!noNewValidInput) {
        sMusicStaffPrevPitch = sCurOcarinaPitch;
        sOcarinaStaffPlayingPos += staffOcarinaPlayingPosOffset + 1;
    }
}

/**
 * Checks for ocarina songs from user input with no music staff prompt.
 * Includes ocarina actions such as free play, no warp
 */
void AudioOcarina_CheckSongsWithoutMusicStaff(void) {
    u32 pitch;
    u8 ocarinaStaffPlayingPosStart;
    u8 songIndex;
    u8 j;
    u8 k;

    if (CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_L) &&
        CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP |
                                                  GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DPAD))) {
        AudioOcarina_StartDefault(sOcarinaFlags);
        return;
    }

    AudioOcarina_CheckIfStartedSong();

    if (!sOcarinaHasStartedSong) {
        return;
    }

    ocarinaStaffPlayingPosStart = sOcarinaStaffPlayingPos;
    if ((sPrevOcarinaPitch != sCurOcarinaPitch) && (sCurOcarinaPitch != OCARINA_PITCH_NONE)) {
        sOcarinaStaffPlayingPos++;
        if (sOcarinaStaffPlayingPos > ARRAY_COUNT(sCurOcarinaSongWithoutMusicStaff)) {
            sOcarinaStaffPlayingPos = 1;
        }

        AudioOcarina_UpdateCurOcarinaSong();

        if ((ABS_ALT(sCurOcarinaBendIndex) > 20) && (ocarinaStaffPlayingPosStart != sOcarinaStaffPlayingPos)) {
            sCurOcarinaSongWithoutMusicStaff[sOcarinaWithoutMusicStaffPos - 1] = OCARINA_PITCH_NONE;
        } else {
            sCurOcarinaSongWithoutMusicStaff[sOcarinaWithoutMusicStaffPos - 1] = sCurOcarinaPitch;
        }

        // This nested for-loop tests to see if the notes from the ocarina are identical
        // to any of the songIndex from sFirstOcarinaSongIndex to sLastOcarinaSongIndex

        // Loop through each of the songs
        for (songIndex = sFirstOcarinaSongIndex; songIndex < sLastOcarinaSongIndex; songIndex++) {
            // Checks to see if the song is available to be played
            bool vanillaCondition = (u32)sOcarinaAvailableSongFlags & (1 << songIndex);
            if (GameInteractor_Should(VB_SONG_AVAILABLE_TO_PLAY, vanillaCondition, &songIndex)) {
                // Loops through all possible starting indices?
                // Loops through the notes of the song?
                for (j = 0, k = 0; (j < gOcarinaSongButtons[songIndex].numButtons) && (k == 0) &&
                                   (sOcarinaWithoutMusicStaffPos >= gOcarinaSongButtons[songIndex].numButtons);) {

                    pitch = sCurOcarinaSongWithoutMusicStaff[(sOcarinaWithoutMusicStaffPos -
                                                              gOcarinaSongButtons[songIndex].numButtons) +
                                                             j];

                    if (pitch == sButtonToPitchMap[gOcarinaSongButtons[songIndex].buttonIndex[j]]) {
                        j++;
                    } else {
                        k++;
                    }
                }

                // This conditional is true if songIndex = i is detected
                if (j == gOcarinaSongButtons[songIndex].numButtons) {
                    sPlayedOcarinaSongIndexPlusOne = songIndex + 1;
                    sIsOcarinaInputEnabled = false;
                    sOcarinaFlags = 0;
                }
            }
        }
    }
}

void AudioOcarina_PlayControllerInput(u8 isOcarinaSfxSuppressedWhenCancelled) {
    u32 ocarinaBtnsHeld;

    // Prevents two different ocarina notes from being played on two consecutive frames
    if ((sOcarinaFlags != 0) && (sOcarinaDropInputTimer != 0)) {
        sOcarinaDropInputTimer--;
        if (!CVarGetInteger("gEnhancements.Playback.NoDropOcarinaInput", 0)) {
            return;
        }
    }

    // Ensures the button pressed to start the ocarina does not also play an ocarina note
    if ((sOcarinaInputButtonStart == 0) ||
        ((sOcarinaInputButtonStart &
          (BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DPAD))) !=
         (sOcarinaInputButtonCur &
          (BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DPAD))))) {
        sOcarinaInputButtonStart = 0;
        if (1) {}
        sCurOcarinaPitch = OCARINA_PITCH_NONE;
        sCurOcarinaButtonIndex = OCARINA_BTN_INVALID;
        ocarinaBtnsHeld = (sOcarinaInputButtonCur & (BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP |
                                                     GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DPAD))) &
                          (sOcarinaInputButtonPrev & (BTN_A | BTN_CRIGHT | BTN_CLEFT | BTN_CDOWN | BTN_CUP |
                                                      GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DPAD)));

        if (!(sOcarinaInputButtonPress & ocarinaBtnsHeld) && (sOcarinaInputButtonCur != 0)) {
            sOcarinaInputButtonPress = sOcarinaInputButtonCur;
        } else {
            sOcarinaInputButtonPress &= ocarinaBtnsHeld;
        }

        uint8_t dpadCvarActive = CVarGetInteger("gEnhancements.Playback.DpadOcarina", 0);

        // Interprets and transforms controller input into ocarina buttons and notes
        if (CHECK_BTN_ANY(sOcarinaInputButtonPress, BTN_A)) {
            sCurOcarinaPitch = OCARINA_PITCH_D4;
            sCurOcarinaButtonIndex = OCARINA_BTN_A;

        } else if (CHECK_BTN_ANY(sOcarinaInputButtonPress,
                                 BTN_CDOWN | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DDOWN))) {
            sCurOcarinaPitch = OCARINA_PITCH_F4;
            sCurOcarinaButtonIndex = OCARINA_BTN_C_DOWN;

        } else if (CHECK_BTN_ANY(sOcarinaInputButtonPress,
                                 BTN_CRIGHT | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DRIGHT))) {
            sCurOcarinaPitch = OCARINA_PITCH_A4;
            sCurOcarinaButtonIndex = OCARINA_BTN_C_RIGHT;

        } else if (CHECK_BTN_ANY(sOcarinaInputButtonPress,
                                 BTN_CLEFT | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DLEFT))) {
            sCurOcarinaPitch = OCARINA_PITCH_B4;
            sCurOcarinaButtonIndex = OCARINA_BTN_C_LEFT;

        } else if (CHECK_BTN_ANY(sOcarinaInputButtonPress, BTN_CUP | GameInteractor_Dpad(GI_DPAD_OCARINA, BTN_DUP))) {
            sCurOcarinaPitch = OCARINA_PITCH_D5;
            sCurOcarinaButtonIndex = OCARINA_BTN_C_UP;
        }

        if (GameInteractor_Should(VB_PLAY_OCARINA_NOTE, true, &sCurOcarinaButtonIndex, &sCurOcarinaPitch)) {}

        // Pressing the R Button will raise the pitch by 1 semitone
        if ((sCurOcarinaPitch != OCARINA_PITCH_NONE) && CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_R) &&
            (sRecordingState != OCARINA_RECORD_SCARECROW_SPAWN)) {
            sCurOcarinaButtonIndex += OCARINA_BUTTON_FLAG_BFLAT_RAISE; // Flag to resolve B Flat 4
            sCurOcarinaPitch++;                                        // Raise the pitch by 1 semitone
        }

        // Pressing the Z Button will lower the pitch by 1 semitone
        if ((sCurOcarinaPitch != OCARINA_PITCH_NONE) && CHECK_BTN_ANY(sOcarinaInputButtonCur, BTN_Z) &&
            (sRecordingState != OCARINA_RECORD_SCARECROW_SPAWN)) {
            sCurOcarinaButtonIndex += OCARINA_BUTTON_FLAG_BFLAT_LOWER; // Flag to resolve B Flat 4
            sCurOcarinaPitch--;                                        // Lower the pitch by 1 semitone
        }

        if (sRecordingState != OCARINA_RECORD_SCARECROW_SPAWN) {
            // Bend the pitch of the note based on y control stick
            sCurOcarinaBendIndex = sOcarinaInputStickRel.y;
            sCurOcarinaBendFreq = AudioOcarina_BendPitchTwoSemitones(sCurOcarinaBendIndex);

            // Add vibrato of the ocarina note based on the x control stick
            sCurOcarinaVibrato = ABS_ALT(sOcarinaInputStickRel.x) >> 2;
            // Sets vibrato to io port 6
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 6, sCurOcarinaVibrato);
        } else {
            // no bending or vibrato for recording state OCARINA_RECORD_SCARECROW_SPAWN
            sCurOcarinaBendIndex = 0;
            sCurOcarinaVibrato = 0;
            sCurOcarinaBendFreq = 1.0f; // No bend
        }

        // Processes new and valid notes
        if ((sCurOcarinaPitch != OCARINA_PITCH_NONE) && (sPrevOcarinaPitch != sCurOcarinaPitch)) {
            // Sets ocarina instrument Id to io port 7, which is used
            // as an index in seq 0 to get the true instrument Id
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 7, sOcarinaInstrumentId - 1);
            // Sets pitch to io port 5
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 5, sCurOcarinaPitch);
            AudioSfx_PlaySfx(NA_SE_OC_OCARINA, &gSfxDefaultPos, 4, &sCurOcarinaBendFreq, &sDefaultOcarinaVolume,
                             &gSfxDefaultReverb);
        } else if ((sPrevOcarinaPitch != OCARINA_PITCH_NONE) && (sCurOcarinaPitch == OCARINA_PITCH_NONE) &&
                   !isOcarinaSfxSuppressedWhenCancelled) {
            // Stops ocarina sound when transitioning from playing to not playing a note
            AudioSfx_StopById(NA_SE_OC_OCARINA);
        }
    }
}

/**
 * Directly enable the ocarina to receive input without
 * properly resetting it based on an ocarina instrument id
 * Unused.
 */
void AudioOcarina_EnableInput(u8 inputEnabled) {
    sIsOcarinaInputEnabled = inputEnabled;
}

/**
 * Resets ocarina properties
 */
void AudioOcarina_ResetAndMute(void) {
    sOcarinaInputButtonCur = 0;
    sOcarinaInputButtonPrev = 0;
    sOcarinaInputButtonPress = 0;

    sOcarinaInputButtonStart = 0xFFFF;

    AudioOcarina_PlayControllerInput(false);
    AudioSfx_StopById(NA_SE_OC_OCARINA);

    if (gAudioSpecId != 12) {
        AudioSfx_MuteBanks(0);
    }

    sPlaybackState = 0;
    sPlaybackStaffPos = 0;
    sIsOcarinaInputEnabled = false;

    sOcarinaFlags = 0;

    // return to full volume for both bgm players after ocarina is finished
    AudioSfx_RestoreBgmVolume(SFX_CHANNEL_OCARINA);
    // return to full volume for all sfx channels except system & ocarina banks
    Audio_SetSfxVolumeExceptSystemAndOcarinaBanks(0x7F);
}

void AudioOcarina_ResetAndReadInput(void) {
    sOcarinaInputButtonCur = 0;
    sOcarinaInputButtonPrev = 0;
    sOcarinaInputButtonPress = 0;

    sOcarinaInputButtonStart = 0xFFFF;

    AudioOcarina_PlayControllerInput(true);

    sPlaybackState = 0;
    sPlaybackStaffPos = 0;
    sIsOcarinaInputEnabled = false;

    sOcarinaFlags = 0;

    sOcarinaInputButtonCur = 0;

    AudioOcarina_ReadControllerInput();
    sOcarinaInputButtonStart = sOcarinaInputButtonCur;
}

/**
 * Set a timer that will disable the ocarina once the timer reaches 0
 */
void AudioOcarina_SetOcarinaDisableTimer(u8 unused, u8 timer) {
    sOcarinaDisableTimer = timer;
    sOcarinaUnused = unused;
}

void AudioOcarina_SetInstrument(u8 ocarinaInstrumentId) {
    if ((sOcarinaInstrumentId != ocarinaInstrumentId) || (ocarinaInstrumentId == OCARINA_INSTRUMENT_DEFAULT)) {
        // #region 2S2H [Port] Custom Sequnces
        u16 sfxEditorId = ocarinaInstrumentId + 0x81;
        u16 neSeq = AudioEditor_GetReplacementSeq(sfxEditorId);
        if (neSeq != sfxEditorId) {
            gAudioCtx.seqReplaced[SEQ_PLAYER_SFX] = 1;
            ocarinaInstrumentId = neSeq - 0x81;
        }
        // #end region
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 1, ocarinaInstrumentId);
        sOcarinaInstrumentId = ocarinaInstrumentId;

        if (ocarinaInstrumentId == OCARINA_INSTRUMENT_OFF) {
            AudioOcarina_ResetAndMute();
        } else {
            sOcarinaInputButtonCur = 0;
            AudioOcarina_ReadControllerInput();
            sOcarinaInputButtonStart = sOcarinaInputButtonCur;
            // lowers volume of all sfx channels except system & ocarina banks
            Audio_SetSfxVolumeExceptSystemAndOcarinaBanks(0x40);
            // lowers volume of bgm players while playing ocarina
            AudioSfx_LowerBgmVolume(SFX_CHANNEL_OCARINA);
        }
    }
}

void AudioOcarina_SetPlaybackSong(s8 songIndexPlusOne, u8 playbackState) {
    u8 i = 0;

    if (songIndexPlusOne == 0) {
        sPlaybackState = 0;
        AudioSfx_StopById(NA_SE_OC_OCARINA);
        return;
    }

    if (songIndexPlusOne < (OCARINA_SONG_MAX + 1)) {
        sPlaybackSong = sOcarinaSongNotes[songIndexPlusOne - 1];
    } else {
        sPlaybackSong = sScarecrowsLongSongNotes;
    }

    sPlaybackNotePos = 0;
    if (playbackState & 0x80) {
        sPlaybackState = 1;
        sPlaybackStaffStopPos = playbackState & 0xF;
        sPlaybackStaffStartPos = 0xFF;
    } else if (playbackState & 0x40) {
        sPlaybackState = 1;
        sPlaybackStaffStartPos = playbackState & 0xF;
        sPlaybackStaffStopPos = 0xFF;
        while (i < sPlaybackStaffStartPos) {
            if (sOcarinaSongNotes[songIndexPlusOne - 1][sPlaybackNotePos].pitch != OCARINA_PITCH_NONE) {
                i++;
            }
            sPlaybackNotePos++;
        }
    } else {
        sPlaybackState = playbackState;
        sPlaybackStaffStopPos = 0xFF;
        sPlaybackStaffStartPos = 0xFF;
    }

    sPlaybackNoteTimer = 0;
    sPlaybackPitch = OCARINA_PITCH_NONE;
    sPlaybackStaffPos = 0;

    if (sPlaybackStaffStartPos == 0xFF) {
        while ((sPlaybackSong[sPlaybackNotePos].pitch == OCARINA_PITCH_NONE) &&
               (songIndexPlusOne > (OCARINA_SONG_EVAN_PART2 + 1))) {
            sPlaybackNotePos++;
        }
    }
}

/**
 * Play a song with the ocarina to the user that is
 * based on OcarinaNote data and not user input
 */
void AudioOcarina_PlaybackSong(void) {
    s32 noteTimerStep;
    s32 nextNoteTimerStep;

    if (sPlaybackState == 0) {
        return;
    }

    if (sPlaybackStaffPos == 0) {
        noteTimerStep = 3;
    } else {
        noteTimerStep = sOcarinaUpdateTaskStart - sOcarinaPlaybackTaskStart;
    }

    if (noteTimerStep < sPlaybackNoteTimer) {
        sPlaybackNoteTimer -= noteTimerStep;
    } else {
        nextNoteTimerStep = noteTimerStep - sPlaybackNoteTimer;
        sPlaybackNoteTimer = 0;
    }

    if (sPlaybackNoteTimer == 0) {

        if (sPlaybackStaffPos == sPlaybackStaffStopPos) {
            sPlaybackState = 0;
            return;
        }

        sPlaybackNoteTimer = sPlaybackSong[sPlaybackNotePos].length;

        if (sPlaybackNotePos == 1) {
            sPlaybackNoteTimer++;
        }

        if (sPlaybackNoteTimer == 0) {
            sPlaybackState--;
            if (sPlaybackState != 0) {
                sPlaybackNotePos = 0;
                sPlaybackStaffPos = 0;
                sPlaybackPitch = OCARINA_PITCH_NONE;
            } else {
                AudioSfx_StopById(NA_SE_OC_OCARINA);
            }
            return;
        } else {
            sPlaybackNoteTimer -= nextNoteTimerStep;
        }

        // Update volume
        if (sNotePlaybackVolume != sPlaybackSong[sPlaybackNotePos].volume) {
            sNotePlaybackVolume = sPlaybackSong[sPlaybackNotePos].volume;
            sRelativeNotePlaybackVolume = sNotePlaybackVolume / 127.0f;
        }

        // Update vibrato
        sNotePlaybackVibrato = sPlaybackSong[sPlaybackNotePos].vibrato;
        // Sets vibrato to io port 6
        AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 6, sNotePlaybackVibrato);

        // Update bend
        if (sNotePlaybackBend != sPlaybackSong[sPlaybackNotePos].bend) {
            sNotePlaybackBend = sPlaybackSong[sPlaybackNotePos].bend;
            sRelativeNotePlaybackBend = AudioOcarina_BendPitchTwoSemitones(sNotePlaybackBend);
        }

        // No changes in volume, vibrato, or bend between notes
        if ((sPlaybackSong[sPlaybackNotePos].volume == sPlaybackSong[sPlaybackNotePos - 1].volume &&
             (sPlaybackSong[sPlaybackNotePos].vibrato == sPlaybackSong[sPlaybackNotePos - 1].vibrato) &&
             (sPlaybackSong[sPlaybackNotePos].bend == sPlaybackSong[sPlaybackNotePos - 1].bend))) {
            sPlaybackPitch = 0xFE;
        }

        if (sPlaybackPitch != sPlaybackSong[sPlaybackNotePos].pitch) {
            u8 pitch = sPlaybackSong[sPlaybackNotePos].pitch;

            // As bFlat4 is exactly in the middle of notes B & A, a flag is
            // added to the pitch to resolve which button to map Bflat4 to
            if (pitch == OCARINA_PITCH_BFLAT4) {
                sPlaybackPitch = pitch + sPlaybackSong[sPlaybackNotePos].bFlat4Flag;
            } else {
                sPlaybackPitch = pitch;
            }

            if (sPlaybackPitch != OCARINA_PITCH_NONE) {
                sPlaybackStaffPos++;
                // Sets ocarina instrument Id to io port 7, which is used
                // as an index in seq 0 to get the true instrument Id
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 7, sOcarinaInstrumentId - 1);
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 5,
                                        sPlaybackPitch & OCARINA_BUTTON_MASK_PITCH);
                AudioSfx_PlaySfx(NA_SE_OC_OCARINA, &gSfxDefaultPos, 4, &sRelativeNotePlaybackBend,
                                 &sRelativeNotePlaybackVolume, &gSfxDefaultReverb);
            } else {
                AudioSfx_StopById(NA_SE_OC_OCARINA);
            }
        }
        sPlaybackNotePos++;
    }
}

void AudioOcarina_SetRecordingSong(u8 isRecordingComplete) {
    u16 i;
    u16 i2;
    u16 pad;
    u8 pitch;
    OcarinaNote* note;
    u8 j;
    u8 k;
    s32 t;
    OcarinaNote* recordedSong;

    if (sRecordingState == OCARINA_RECORD_SCARECROW_LONG) {
        recordedSong = gScarecrowLongSongPtr;
    } else {
        /**
         * OCARINA_RECORD_SCARECROW_SPAWN
         *
         * The notes for scarecrows spawn song are first recorded into the ocarina termina
         * wall address to act as a buffer. That way, if a new scarecrow spawn song is
         * rejected, the previous scarecrow spawn song is not overwritten. If the scarecrow
         * spawn song is accepted, then the notes are copied over to the scarecrow spawn
         * song address
         */
        recordedSong = sTerminaWallSongPtr;
    }

    recordedSong[sRecordSongPos].pitch = sRecordOcarinaPitch;
    recordedSong[sRecordSongPos].length = sOcarinaUpdateTaskStart - sOcarinaRecordTaskStart;
    recordedSong[sRecordSongPos].volume = sRecordOcarinaVolume;
    recordedSong[sRecordSongPos].vibrato = sRecordOcarinaVibrato;
    recordedSong[sRecordSongPos].bend = sRecordOcarinaBendIndex;
    recordedSong[sRecordSongPos].bFlat4Flag = sRecordOcarinaButtonIndex & OCARINA_BUTTON_MASK_FLAG;

    sRecordOcarinaPitch = sCurOcarinaPitch;
    sRecordOcarinaVolume = sCurOcarinaVolume;
    sRecordOcarinaVibrato = sCurOcarinaVibrato;
    sRecordOcarinaBendIndex = sCurOcarinaBendIndex;
    sRecordOcarinaButtonIndex = sCurOcarinaButtonIndex;

    sRecordSongPos++;

    if ((sRecordSongPos != (ARRAY_COUNT(sScarecrowsLongSongNotes) - 1)) && !isRecordingComplete) {
        // Continue recording
        return;
    }

    // Recording is complete

    i = sRecordSongPos;
    pitch = OCARINA_PITCH_NONE;
    while ((i != 0) && (pitch == OCARINA_PITCH_NONE)) {
        i--;
        pitch = recordedSong[i].pitch;
    }

    if (sRecordSongPos != (i + 1)) {
        sRecordSongPos = i + 2;
        recordedSong[sRecordSongPos - 1].length = 0;
    }

    recordedSong[sRecordSongPos].length = 0;

    if (sRecordingState == OCARINA_RECORD_SCARECROW_SPAWN) {
        if (sOcarinaStaffPlayingPos >= 8) {
            for (i = 0; i < sRecordSongPos; i++) {
                recordedSong[i] = recordedSong[i + 1];
            }

            // Copies Notes from buffer into scarecrows spawn buttons to be tested for acceptance or rejection
            AudioOcarina_MapSongFromPitchToButton(OCARINA_SONG_TERMINA_WALL, OCARINA_SONG_SCARECROW_SPAWN, 8);

            // Tests to see if the notes from the scarecrow song contain identical
            // notes within its song to any of the reserved songIndex from 0 to 21

            // Loop through each of the songs
            for (i = 0; i < OCARINA_SONG_SCARECROW_SPAWN; i++) {
                // Checks to see if the song is reserved
                if (sIsOcarinaSongReserved[i]) {
                    // Loops through all possible starting indices
                    for (j = 0; (j < 9 - gOcarinaSongButtons[i].numButtons); j++) {
                        // Loops through the notes of the song
                        for (k = 0; (k < gOcarinaSongButtons[i].numButtons) && (k + j < 8) &&
                                    (gOcarinaSongButtons[i].buttonIndex[k] ==
                                     gOcarinaSongButtons[OCARINA_SONG_SCARECROW_SPAWN].buttonIndex[k + j]);
                             k++) {
                            ;
                        }

                        // This conditional is true if the recorded song contains a reserved song
                        // recorded song is cancelled
                        if (k == gOcarinaSongButtons[i].numButtons) {
                            sRecordingState = OCARINA_RECORD_REJECTED;
                            sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
                            return;
                        }
                    }
                }
            }

            // Counts how many times a note is repeated
            i = 1;
            while (i < 8) {
                if (gOcarinaSongButtons[OCARINA_SONG_SCARECROW_SPAWN].buttonIndex[0] !=
                    gOcarinaSongButtons[OCARINA_SONG_SCARECROW_SPAWN].buttonIndex[i]) {
                    i = 9; // break
                } else {
                    i++;
                }
            }

            // This condition is true if all 8 notes are the same pitch
            if (i == 8) {
                sRecordingState = OCARINA_RECORD_REJECTED;
                sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
                return;
            }

            // The scarecrow spawn song is accepted and copied from the buffer to the scarecrow spawn notes
            for (i = 0; i < sRecordSongPos; i++) {
                sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][i] = sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][i];
            }

            sIsOcarinaInputEnabled = false;
        } else {
            sOcarinaSongNotes[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
        }
    }

    sRecordingState = OCARINA_RECORD_OFF;
}

/**
 * recordingState = OCARINA_RECORD_OFF, end
 * recordingState = OCARINA_RECORD_SCARECROW_LONG, start long scarecrows song
 * recordingState = OCARINA_RECORD_SCARECROW_SPAWN, start spawn scarecrows song
 */
void AudioOcarina_SetRecordingState(u8 recordingState) {
    if ((u32)recordingState == sRecordingState) {
        return;
    }

    if (recordingState != OCARINA_RECORD_OFF) {
        sOcarinaRecordTaskStart = sOcarinaUpdateTaskStart;
        sRecordOcarinaPitch = OCARINA_PITCH_NONE;
        sRecordOcarinaVolume = 0x57;
        sRecordOcarinaVibrato = 0;
        sRecordOcarinaBendIndex = 0;
        sRecordOcarinaButtonIndex = 0;
        sRecordSongPos = 0;
        sIsOcarinaInputEnabled = true;
        sOcarinaStaffPlayingPos = 0;
        sScarecrowsLongSongSecondNote = sScarecrowsLongSongNotes[1];
    } else {
        if (sRecordSongPos == 0) {
            sScarecrowsLongSongNotes[1] = sScarecrowsLongSongSecondNote;
        } else {
            if (sRecordingState == OCARINA_RECORD_SCARECROW_SPAWN) {
                sOcarinaStaffPlayingPos = 1;
            }

            AudioOcarina_SetRecordingSong(true);
        }

        sIsOcarinaInputEnabled = false;
        sOcarinaStaffPlayingPos = 0;
    }

    sRecordingState = recordingState;
}

void AudioOcarina_UpdateRecordingStaff(void) {
    sRecordingStaff.state = sRecordingState;
    sRecordingStaff.pos = sOcarinaStaffPlayingPos;
    if (sRecordingState == OCARINA_RECORD_REJECTED) {
        sRecordingState = OCARINA_RECORD_OFF;
    }
}

void AudioOcarina_UpdatePlayingStaff(void) {
    if (sCurOcarinaButtonIndex != OCARINA_BTN_INVALID) {
        sPlayingStaff.buttonIndex = sCurOcarinaButtonIndex & OCARINA_BUTTON_MASK_PITCH;
    }
    sPlayingStaff.state = AudioOcarina_GetPlayingState();
    sPlayingStaff.pos = sOcarinaStaffPlayingPos;
}

void AudioOcarina_UpdatePlaybackStaff(void) {
    if ((sPlaybackPitch & OCARINA_BUTTON_MASK_PITCH) <= OCARINA_PITCH_EFLAT5) {
        sPlaybackStaff.buttonIndex = AudioOcarina_MapPitchToButton(sPlaybackPitch);
    }

    sPlaybackStaff.state = sPlaybackState;

    if (sPlaybackSong != sScarecrowsLongSongNotes) {
        sPlaybackStaff.pos = sPlaybackStaffPos;
    } else if (sPlaybackStaffPos == 0) {
        sPlaybackStaff.pos = 0;
    } else {
        sPlaybackStaff.pos = ((sPlaybackStaffPos - 1) % 8) + 1;
    }
}

OcarinaStaff* AudioOcarina_GetRecordingStaff(void) {
    return &sRecordingStaff;
}

OcarinaStaff* AudioOcarina_GetPlayingStaff(void) {
    if (sPlayingStaff.state < 0xFE) {
        sOcarinaFlags = 0;
    }

    return &sPlayingStaff;
}

OcarinaStaff* AudioOcarina_GetPlaybackStaff(void) {
    return &sPlaybackStaff;
}

void AudioOcarina_RecordSong(void) {
    s32 noteChanged;

    if ((sRecordingState != OCARINA_RECORD_OFF) && ((sOcarinaUpdateTaskStart - sOcarinaRecordTaskStart) >= 3)) {
        noteChanged = false;
        if (sRecordOcarinaPitch != sCurOcarinaPitch) {
            if (sCurOcarinaPitch != OCARINA_PITCH_NONE) {
                sRecordingStaff.buttonIndex = sCurOcarinaButtonIndex & OCARINA_BUTTON_MASK_PITCH;
                sOcarinaStaffPlayingPos++;
            } else if ((sRecordingState == OCARINA_RECORD_SCARECROW_SPAWN) && (sOcarinaStaffPlayingPos == 8)) {
                AudioOcarina_SetRecordingSong(true);
                return;
            }

            if (sOcarinaStaffPlayingPos > 8) {
                if (sRecordingState == OCARINA_RECORD_SCARECROW_SPAWN) {
                    // notes played are over 8 and in recording mode.
                    AudioOcarina_SetRecordingSong(true);
                    return;
                }
                sOcarinaStaffPlayingPos = 1;
            }
            noteChanged = true;
        } else if (sRecordOcarinaVolume != sCurOcarinaVolume) {
            noteChanged = true;
        } else if (sRecordOcarinaVibrato != sCurOcarinaVibrato) {
            if (sRecordingState != OCARINA_RECORD_SCARECROW_SPAWN) {
                noteChanged = true;
            }
        } else if ((sRecordOcarinaBendIndex != sCurOcarinaBendIndex) &&
                   (sRecordingState != OCARINA_RECORD_SCARECROW_SPAWN)) {
            noteChanged = true;
        }

        if (noteChanged) {
            AudioOcarina_SetRecordingSong(false);
            sOcarinaRecordTaskStart = sOcarinaUpdateTaskStart;
        }
    }
}

/**
 * Tests to see if the notes from songIndex contain identical notes
 * within its song to any of the reserved songIndex from 0 up to maxSongIndex
 */
s32 AudioOcarina_TerminaWallValidateNotes(u8 songIndex, u8 maxSongIndex) {
    u8 curSongIndex;
    u8 j;
    u8 k;

    // loop through all possible songs up to maxSongIndex
    for (curSongIndex = 0; curSongIndex < maxSongIndex; curSongIndex++) {
        // check to see if the song is reserved or not
        if (sIsOcarinaSongReserved[curSongIndex]) {
            // starting index to test the song
            for (j = 0; j < (9 - gOcarinaSongButtons[curSongIndex].numButtons); j++) {
                // loop through each note in the song
                for (k = 0; (k < gOcarinaSongButtons[curSongIndex].numButtons) && ((k + j) < 8) &&
                            (gOcarinaSongButtons[curSongIndex].buttonIndex[k] ==
                             gOcarinaSongButtons[songIndex].buttonIndex[(k + j)]);
                     k++) {
                    continue;
                }

                if (k == gOcarinaSongButtons[curSongIndex].numButtons) {
                    // failure: songIndex is identical to curSongIndex.
                    return -1;
                }
            }
        }
    }

    // success: notes are accepted and used
    return 0;
}

/**
 * Generates the notes displayed on the Termina Field wall of musical notes
 * Song generation loop alternates between 8 random notes and a random song from Ocarina of Time (OoT).
 * Will check to see that the notes are valid by ensuring no playable song is within the selected notes
 * All OoT songs are valid, so the outer loop will run a maxiumum of two times.
 * i.e. if random notes fails, then the next set of notes will be from a valid OoT song
 */
void AudioOcarina_TerminaWallGenerateNotes(void) {
    OcarinaNote* ocarinaNote;
    u8 randomButtonIndex;
    u8 i;
    u8 j;

    do {
        i = 0;
        if (sOcarinaWallCounter++ % 2) {
            j = 0;

            for (; i < 8; i++) {
                randomButtonIndex = AudioThread_NextRandom() % ARRAY_COUNT(sButtonToPitchMap);
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].pitch = sButtonToPitchMap[randomButtonIndex];
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].length = 19;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].volume = 80;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].vibrato = 0;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].bend = 0;
                j++;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].pitch = OCARINA_PITCH_NONE;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].length = 3;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].volume = 0;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].vibrato = 0;
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j].bend = 0;
                j++;
            }

            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j - 2].length = 90;
            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j - 1].length = 22;
            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j + 1].pitch = OCARINA_PITCH_NONE;
            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][j + 1].length = 0;
            AudioOcarina_MapSongFromPitchToButton(OCARINA_SONG_TERMINA_WALL, OCARINA_SONG_TERMINA_WALL, 8);
        } else {
            j = AudioThread_NextRandom() % ARRAY_COUNT(sOoTOcarinaSongNotes);
            ocarinaNote = sOoTOcarinaSongNotes[j];

            for (; ocarinaNote[i].length != 0; i++) {
                sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][i] = ocarinaNote[i];
            }

            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][i].pitch = OCARINA_PITCH_NONE;
            sOcarinaSongNotes[OCARINA_SONG_TERMINA_WALL][i].length = 0;
            AudioOcarina_MapSongFromPitchToButton(OCARINA_SONG_TERMINA_WALL, OCARINA_SONG_TERMINA_WALL,
                                                  sOoTOcarinaSongsNumNotes[j]);
        }
    } while (AudioOcarina_TerminaWallValidateNotes(OCARINA_SONG_TERMINA_WALL, OCARINA_SONG_TERMINA_WALL) != 0);
}

/**
 * Unused remnant of OoT
 */
void AudioOcarina_MemoryGameInit(u8 minigameRound) {
    u8 i;

    if (minigameRound > 2) {
        minigameRound = 2;
    }

    sOcarinaMemoryGameAppendPos = 0;
    sOcarinaMemoryGameEndPos = sOcarinaMemoryGameNumNotes[minigameRound];

    for (i = 0; i < 3; i++) {
        AudioOcarina_MemoryGameNextNote();
    }
}

/**
 * Unused remnant of OoT, Id 14 now represent Goron Lullaby Intro instead of the OoT ocarina memory game
 * //! @bug calling this function in MM will overwrite Goron Lullaby Intro
 */
#define OCARINA_SONG_MEMORY_GAME 14
s32 AudioOcarina_MemoryGameNextNote(void) {
    u32 randomButtonIndex;
    u8 randomPitch;

    if (sOcarinaMemoryGameAppendPos == sOcarinaMemoryGameEndPos) {
        return true;
    }

    randomButtonIndex = AudioThread_NextRandom();
    randomPitch = sButtonToPitchMap[randomButtonIndex % ARRAY_COUNT(sButtonToPitchMap)];

    if (sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos - 1].pitch == randomPitch) {
        randomPitch = sButtonToPitchMap[(randomButtonIndex + 1) % ARRAY_COUNT(sButtonToPitchMap)];
    }

    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].pitch = randomPitch;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].length = 45;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].volume = 0x50;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].vibrato = 0;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].bend = 0;

    sOcarinaMemoryGameAppendPos++;

    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].pitch = OCARINA_PITCH_NONE;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos].length = 0;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos + 1].pitch = OCARINA_PITCH_NONE;
    sOcarinaSongNotes[OCARINA_SONG_MEMORY_GAME][sOcarinaMemoryGameAppendPos + 1].length = 0;

    //! FAKE:
    if (1) {}

    return false;
}
#undef OCARINA_SONG_MEMORY_GAME

void AudioOcarina_Update(void) {
    sOcarinaUpdateTaskStart = gAudioCtx.totalTaskCount;
    if (sOcarinaInstrumentId != OCARINA_INSTRUMENT_OFF) {
        if (sIsOcarinaInputEnabled == true) {
            AudioOcarina_ReadControllerInput();
        }

        if ((sPlaybackState == 0) && (sIsOcarinaInputEnabled == true)) {
            AudioOcarina_PlayControllerInput(false);
        }

        if (sOcarinaFlags != 0) {
            if (sOcarinaFlags & 0x40000000) {
                AudioOcarina_CheckSongsWithoutMusicStaff();
            } else {
                AudioOcarina_CheckSongsWithMusicStaff();
            }
        }

        AudioOcarina_PlaybackSong();
        sOcarinaPlaybackTaskStart = sOcarinaUpdateTaskStart;

        if (sPlaybackState == 0) {
            AudioOcarina_RecordSong();
        }

        if ((sOcarinaFlags != 0) && (sPrevOcarinaPitch != sCurOcarinaPitch)) {
            sOcarinaDropInputTimer = 1; // Drops ocarina input for 1 frame
        }

        sPrevOcarinaPitch = sCurOcarinaPitch;

        if (sOcarinaDisableTimer != 0) {
            sOcarinaDisableTimer--;
            if (sOcarinaDisableTimer == 0) {
                AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
            }
        }
    }

    AudioOcarina_UpdatePlayingStaff();
    AudioOcarina_UpdatePlaybackStaff();
    AudioOcarina_UpdateRecordingStaff();
}

#define OCARINA_INSTRUMENT_OOT_MAX 7

void AudioOcarina_PlayLongScarecrowSong(void) {
    static u8 sScarecrowAfterCreditsState = 0;
    static u8 sScarecrowAfterCreditsIntrumentId = OCARINA_INSTRUMENT_DEFAULT;
    static u16 sScarecrowAfterCreditsTimer = 1200;

    switch (sScarecrowAfterCreditsState) {
        case 0:
            if (sScarecrowAfterCreditsTimer-- == 0) {
                // OoT originally had 7 Ocarina instruments
                // MM has more so this does not go all the way to the max
                if (sScarecrowAfterCreditsIntrumentId < OCARINA_INSTRUMENT_OOT_MAX) {
                    // set next ocarina instrument and restart
                    sScarecrowAfterCreditsState++;
                } else {
                    // finished
                    sScarecrowAfterCreditsState = 3;
                    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
                }
                sScarecrowAfterCreditsTimer = 1200;
            }
            break;

        case 1:
            AudioSfx_MuteBanks(0);
            AudioOcarina_SetInstrument(sScarecrowAfterCreditsIntrumentId);
            AudioOcarina_SetPlaybackSong(OCARINA_SONG_SCARECROW_LONG + 1, 1);
            sScarecrowAfterCreditsIntrumentId++;
            sScarecrowAfterCreditsState++;
            break;

        case 2:
            if (AudioOcarina_GetPlaybackStaff()->state == 0) {
                sScarecrowAfterCreditsState = 0;
            }
            break;
    }
}

u8 sRequestCustomSequence = false;

void AudioOcarina_SetCustomSequence(void) {
    // Never passes true as sRequestCustomSequence is never set true
    if (sRequestCustomSequence && gAudioCtx.seqPlayers[SEQ_PLAYER_FANFARE].enabled &&
        ((u8)gAudioCtx.seqPlayers[SEQ_PLAYER_FANFARE].seqScriptIO[0] == (u8)SEQ_IO_VAL_NONE)) {
        gAudioCtx.seqPlayers[SEQ_PLAYER_FANFARE].seqData = sCustomSequenceScript;
        sRequestCustomSequence = false;
    }
}

// Called by unused function
void AudioOcarina_PlayCustomSequence(void) {
    sRequestCustomSequence = true;
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 0, NA_BGM_DUNGEON_APPEAR);
}

// Unused
s32 AudioOcarina_CreateCustomSequence(void) {
    OcarinaNote* prevNote;
    u16 i;
    u16 delay = 0;
    u8 absPitch;
    s8 adjBendIndex;
    u8 prevVibrato = 0;
    s8 prevBend = 0;
    u8 isLegato = false;
    u16 phi_s4 = 0;
    u16 temp1; // prevNoteLength, reused temp
    u16 temp2;

    sCustomSequencePc = 39; // .channel chan_loop

    prevNote = &sScarecrowsLongSongNotes[0];
    for (i = 1; ((prevNote->pitch == OCARINA_PITCH_NONE) && (prevNote->length != 0)); i++) {
        prevNote = &sScarecrowsLongSongNotes[i];
    }

    for (; (prevNote->length != 0) && (sCustomSequencePc < 394); i++) {

        temp1 = ((prevNote->length * 48) / 30);

        if (prevVibrato != prevNote->vibrato) {
            sCustomSequenceScript[sCustomSequencePc++] = 0xFD; // delay cmd

            if (delay >= 0x80) {
                // Store as compressed u16
                sCustomSequenceScript[sCustomSequencePc++] = (((delay >> 8) & 0xFF) & 0x7F) + 0x80; // delay arg
                sCustomSequenceScript[sCustomSequencePc++] = delay & 0xFF;                          // delay arg
            } else {
                // Store as u8
                sCustomSequenceScript[sCustomSequencePc++] = delay; // delay arg
            }

            sCustomSequenceScript[sCustomSequencePc++] = 0xD8;              // vibdepth cmd
            sCustomSequenceScript[sCustomSequencePc++] = prevNote->vibrato; // vibdepth arg

            delay = temp1;
            prevVibrato = prevNote->vibrato;
        } else {
            delay += temp1;
        }

        prevNote = &sScarecrowsLongSongNotes[i];
    }

    if (delay != 0) {
        sCustomSequenceScript[sCustomSequencePc++] = 0xFD; // delay cmd
        if (delay >= 0x80) {
            // Store as compressed u16
            sCustomSequenceScript[sCustomSequencePc++] = (((delay >> 8) & 0xFF) & 0x7F) + 0x80; // delay arg
            sCustomSequenceScript[sCustomSequencePc++] = delay & 0xFF;                          // delay arg
        } else {
            // Store as u8
            sCustomSequenceScript[sCustomSequencePc++] = delay; // delay arg
        }
    }

    sCustomSequenceScript[sCustomSequencePc++] = 0xFF; // end cmd

    // ldlayer 0, layer0
    sCustomSequenceScript[24] = sCustomSequencePc >> 8;
    sCustomSequenceScript[25] = sCustomSequencePc & 0xFF;

    // ldlayer 1, layer1
    sCustomSequenceScript[27] = (sCustomSequencePc + 4) >> 8;
    sCustomSequenceScript[28] = (sCustomSequencePc + 4) & 0xFF;

    sCustomSequenceScript[sCustomSequencePc++] = 0xC2; // transpose cmd
    sCustomSequenceScript[sCustomSequencePc++] = -5;   // transpose arg

    sCustomSequenceScript[sCustomSequencePc++] = 0xC0; // ldelay cmd
    sCustomSequenceScript[sCustomSequencePc++] = 8;    // ldelay arg

    sCustomSequenceScript[sCustomSequencePc++] = 0xC1; // shortvel cmd
    sCustomSequenceScript[sCustomSequencePc++] = 87;   // shortvel arg

    sCustomSequenceScript[sCustomSequencePc++] = 0xC9; // shortgate cmd
    sCustomSequenceScript[sCustomSequencePc++] = 0;    // shortgate arg

    prevNote = &sScarecrowsLongSongNotes[0];

    for (i = 1; ((prevNote->pitch == OCARINA_PITCH_NONE) && (prevNote->length != 0)); i++) {
        prevNote = &sScarecrowsLongSongNotes[i];
    }

    for (; ((temp1 = prevNote->length) != 0) && (sCustomSequencePc < 394); i++) {
        if (prevNote->pitch == sScarecrowsLongSongNotes[i].pitch) {
            if ((sScarecrowsLongSongNotes[i].length != 0) && !isLegato) {
                sCustomSequenceScript[sCustomSequencePc++] = 0xC4; // legato cmd
                isLegato = true;
            }
        } else if ((isLegato == true) && (sScarecrowsLongSongNotes[i].pitch != OCARINA_PITCH_NONE) &&
                   (sScarecrowsLongSongNotes[i].pitch != OCARINA_PITCH_C4)) {
            sCustomSequenceScript[sCustomSequencePc++] = 0xC5; // nolegato cmd
            isLegato = false;
        }

        if (prevBend != prevNote->bend) {
            sCustomSequenceScript[sCustomSequencePc++] = 0xCE; // bendfine cmd

            if (ABS_ALT(prevNote->bend) > 64) {
                adjBendIndex = 127;
            } else if (prevNote->bend < 0) {
                adjBendIndex = -prevNote->bend;
            } else {
                adjBendIndex = ((prevNote->bend * 127) / 64);
            }

            if (prevNote->bend < 0) {
                adjBendIndex *= -1;
            }

            sCustomSequenceScript[sCustomSequencePc++] = adjBendIndex; // bendfine arg

            prevBend = prevNote->bend;
        }

        absPitch = prevNote->pitch + 39; // convert ocarina pitch to absolute pitch, + PITCH_C4 (Middle C)
        if (prevNote->pitch != OCARINA_PITCH_NONE) {
            sCustomSequenceScript[sCustomSequencePc++] = absPitch; // shortdvg cmd, shortdvg arg
        } else {
            sCustomSequenceScript[sCustomSequencePc++] = 0xC0; // ldelay cmd
        }

        // delay/duration
        temp2 = ((temp1 * 48) + phi_s4) / 30;

        if (temp2 < 0x80) {
            // Store as u8
            sCustomSequenceScript[sCustomSequencePc++] = temp2; // shortdvg/ldelay arg
        } else {
            // Store as compressed u16
            sCustomSequenceScript[sCustomSequencePc++] = (((temp2 >> 8) & 0xFF) & 0x7F) + 0x80; // shortdvg/ldelay arg
            sCustomSequenceScript[sCustomSequencePc++] = temp2 & 0xFF;                          // shortdvg/ldelay arg
        }

        phi_s4 = ((temp1 * 48) + phi_s4) - (temp2 * 30);

        prevNote = &sScarecrowsLongSongNotes[i];
    }

    sCustomSequenceScript[sCustomSequencePc++] = 0xFF; // end cmd

    AudioOcarina_PlayCustomSequence();

    if (i >= 394) {
        return -1;
    }

    return 0;
}

void AudioOcarina_ResetInstrument(void) {
    if (sOcarinaInstrumentId != OCARINA_INSTRUMENT_OFF) {
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 1, 0);
        sOcarinaInstrumentId = OCARINA_INSTRUMENT_OFF;
        AudioOcarina_ResetAndMute();
    }
}

void AudioOcarina_ResetStaffs(void) {
    u8 songIndex;

    sPlayingStaff.buttonIndex = OCARINA_BTN_INVALID;
    sPlayingStaff.state = 0xFF;
    sPlayingStaff.pos = 0;

    sPlaybackStaff.buttonIndex = OCARINA_BTN_INVALID;
    sPlaybackStaff.state = 0;
    sPlaybackStaff.pos = 0;

    sRecordingStaff.buttonIndex = OCARINA_BTN_INVALID;
    sRecordingStaff.state = 0xFF;
    sRecordingStaff.pos = 0;

    sOcarinaDisableTimer = 0;
    sOcarinaDropInputTimer = 0;

    for (songIndex = OCARINA_SONG_SONATA; songIndex < OCARINA_SONG_MAX; songIndex++) {
        D_801FD518[songIndex] = 0xFF;
        D_801FD530[songIndex] = 0;
    }

    D_801D8534 = 0;
}

void Audio_Noop1(s32 arg0) {
}

void Audio_Noop2(s32 arg0, s32 arg1) {
}

void Audio_Noop3(void) {
}

void Audio_Update(void) {
    if ((AudioSeq_UpdateAudioHeapReset() == 0) && !AudioSeq_ResetReverb()) {
        AudioOcarina_SetCustomSequence();
        AudioOcarina_Update();
        AudioVoice_Update();
        Audio_StepFreqLerp(&sRiverFreqScaleLerp);
        Audio_StepFreqLerp(&sWaterfallFreqScaleLerp);
        Audio_UpdateRiverSoundVolumes();
        Audio_UpdateSceneSequenceResumePoint();
        Audio_UpdateFanfare();
        Audio_UpdateSfxVolumeTransition();
        Audio_UpdateSubBgmAtPos();
        Audio_UpdateObjSoundFanfare();
        Audio_UpdateSequenceAtPos();
        Audio_UpdatePauseState();
        Audio_UpdateObjSoundProperties();
        Audio_ResetRequestedSceneSeqId();
        AudioSfx_ProcessRequests();
        AudioSeq_ProcessSeqCmds();
        AudioSfx_ProcessActiveSfx();
        AudioSeq_UpdateActiveSequences();
        AudioSfx_ProcessSfxSettings();
        AudioThread_ScheduleProcessCmds();
    }
}

void Audio_Noop4(UNK_TYPE arg0) {
}

void Audio_Noop5(UNK_TYPE arg0, UNK_TYPE arg1) {
}

/**
 * Queues a bgm sequence directly to the internal audio queue system.
 * Skips the external audio command process
 * Unused
 */
void Audio_PlayMainBgm(s16 seqId) {
    AUDIOCMD_GLOBAL_INIT_SEQPLAYER(SEQ_PLAYER_BGM_MAIN, seqId, 1);
}

f32 AudioSfx_ComputeVolume(u8 bankId, u8 entryIndex) {
    SfxBankEntry* bankEntry = &gSfxBanks[bankId][entryIndex];
    f32 minDist;
    f32 phi_f14;
    f32 baseDist;
    f32 phi_f12;
    f32 volume; // Also acts as intermediate volumeSquared
    s32 pad[3];

    if (bankEntry->sfxParams & SFX_FLAG_VOLUME_NO_DIST) {
        return 1.0f;
    }

    if (bankEntry->dist > 10000.0f) {
        volume = 0.0f;
    } else {
        if (bankEntry->sfxParams & SFX_PARAM_DIST_RANGE_MASK_UPPER) {
            phi_f12 = 0.35f;
            phi_f14 = 0.65f;
        } else {
            phi_f12 = 0.81f;
            phi_f14 = 0.19f;
        }

        switch (bankEntry->sfxParams & SFX_PARAM_DIST_RANGE_MASK) {
            case 0:
                baseDist = 500.0f;
                break;

            case 1:
                baseDist = 666.6667f;
                break;

            case 2:
                baseDist = 952.381f;
                break;

            case 3:
                baseDist = 3846.154f;
                break;

            case 4:
                baseDist = 1000.0f;
                break;

            case 5:
                baseDist = 666.6667f;
                break;

            case 6:
                baseDist = 500.0f;
                break;

            case 7:
                baseDist = 400.0f;
                break;
        }

        minDist = baseDist / 5.0f;

        if (bankEntry->dist < minDist) {
            volume = 1.0f;
        } else if (bankEntry->dist < baseDist) {
            volume =
                ((((baseDist - minDist) - (bankEntry->dist - minDist)) / (baseDist - minDist)) * phi_f14) + phi_f12;
        } else {
            volume = (1.0f - ((bankEntry->dist - baseDist) / (10000.0f - baseDist))) * phi_f12;
        }

        volume = SQ(volume);
    }

    return volume;
}

s8 AudioSfx_ComputeReverb(u8 bankId, u8 entryIndex, u8 channelIndex) {
    s8 distAdd = 0;
    s32 scriptAdd = 0;
    SfxBankEntry* entry = &gSfxBanks[bankId][entryIndex];
    s32 reverb;

    if (!(entry->sfxParams & SFX_FLAG_REVERB_NO_DIST)) {
        if (entry->dist < 2500.0f) {
            distAdd = *entry->posZ > 0.0f ? (entry->dist / 2500.0f) * 70.0f : (entry->dist / 2500.0f) * 91.0f;
        } else {
            distAdd = 70;
        }
    }

    if (IS_SEQUENCE_CHANNEL_VALID(gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].channels[channelIndex])) {
        scriptAdd = gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].channels[channelIndex]->seqScriptIO[1];
        if (gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].channels[channelIndex]->seqScriptIO[1] <= SEQ_IO_VAL_NONE) {
            scriptAdd = 0;
        }
    }

    reverb = (*entry->reverbAdd & 0x7F) + distAdd;

    if (entry->state != SFX_STATE_READY) {
        reverb += scriptAdd;
    }

    if ((bankId != BANK_OCARINA) || (SFX_INDEX(entry->sfxId) >= 2)) {
        reverb += sAudioEnvReverb + (sAudioCodeReverb & 0x3F) + sSpecReverb;
    }

    if (reverb > 0x7F) {
        reverb = 0x7F;
    }

    return reverb;
}

s8 AudioSfx_ComputePanSigned(f32 x, f32 z, u8 token) {
    f32 absX = ABS_ALT(x);
    f32 absZ = ABS_ALT(z);
    f32 pan;

    if (absX > 8000.0f) {
        absX = 8000.0f;
    }

    if (absZ > 8000.0f) {
        absZ = 8000.0f;
    }

    if ((x == 0.0f) && (z == 0.0f)) {
        pan = 0.5f;
    } else if (absZ <= absX) {
        pan = (16000.0f - absX) / (4.5f * (16000.0f - absZ));
        if (x >= 0.0f) {
            pan = 1.0f - pan;
        }
    } else {
        pan = (x / (3.6f * absZ)) + 0.5f;
    }

    if ((absZ < 50.0f) && (absX < 50.0f)) {
        pan = ((pan - 0.5f) * SQ(absX / 50.0f)) + 0.5f;
    }

    return (pan * 127.0f) + 0.5f;
}

f32 AudioSfx_ComputeFreqScale(u8 bankId, u8 entryIndex) {
    s32 applyRandScaling = false;
    SfxBankEntry* entry = &gSfxBanks[bankId][entryIndex];
    f32 dist;
    f32 freq = 1.0f;

    if (entry->sfxParams & SFX_PARAM_RAND_FREQ_LOWER) {
        freq = 1.0f - ((gAudioCtx.audioRandom & 0xF) / 192.0f);
    }

    switch (bankId) {
        case BANK_VOICE:
            if (((entry->sfxId & 0xFF) < 0x40) && (sAudioBaseFilter2 != 0)) {
                applyRandScaling = true;
            } else if (((entry->sfxId & 0xFF) >= 0x40) && (sAudioExtraFilter2 != 0)) {
                applyRandScaling = true;
            }
            break;

        case BANK_PLAYER:
        case BANK_ITEM:
            if (sAudioBaseFilter2 != 0) {
                applyRandScaling = true;
            }
            break;

        case BANK_ENV:
        case BANK_ENEMY:
            if (((*entry->reverbAdd & 0x80) != 0) | (sAudioExtraFilter2 != 0)) {
                applyRandScaling = true;
            }
            break;

        case BANK_SYSTEM:
        case BANK_OCARINA:
            break;

        default:
            break;
    }

    if ((applyRandScaling == true) && !(entry->sfxParams & SFX_PARAM_RAND_FREQ_SCALE)) {
        freq *= 1.0293 - ((gAudioCtx.audioRandom & 0xF) / 144.0f);
    }

    dist = entry->dist;
    if (!(entry->sfxParams & SFX_FLAG_VOLUME_NO_DIST) && !(entry->sfxParams & SFX_FLAG_FREQ_NO_DIST)) {
        if (dist >= 10000.0f) {
            freq += 0.2f;
        } else {
            freq += (0.2f * (dist / 10000.0f));
        }
    }

    if ((entry->sfxParams & SFX_PARAM_RAND_FREQ_RAISE_MASK) != (0 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT)) {
        freq += (entry->randFreq / 192.0f);
    }

    return freq;
}

u8 AudioSfx_ComputeSurroundSoundFilter(f32 behindScreenZ, SfxBankEntry* entry, s8 panSigned) {
    u8 lowPassCutoff;
    u8 highPassCutoff;
    u16* sfxParams;
    f32 distParam;
    f32 dist;

    // Remnant of OoT
    if (*entry->posZ < behindScreenZ) {
        lowPassCutoff = 0;
    } else {
        lowPassCutoff = 0;
    }

    sfxParams = &entry->sfxParams;

    if (*sfxParams & SFX_FLAG_SURROUND_LOWPASS_FILTER) {
        lowPassCutoff = 0xF;
    }

    switch ((*sfxParams & SFX_PARAM_DIST_RANGE_MASK)) {
        case 0:
            distParam = 15.0f;
            break;

        case 1:
            distParam = 12.0f;
            break;

        case 2:
            distParam = 9.0f;
            break;

        case 3:
            distParam = 6.0f;
            break;

        case 4:
            distParam = 18.0f;
            break;

        case 5:
            distParam = 21.0f;
            break;

        case 6:
            distParam = 24.0f;
            break;

        case 7:
            distParam = 27.0f;
            break;

        default:
            break;
    }

    if (!(entry->sfxFlags & SFX_FLAG2_SURROUND_NO_HIGHPASS_FILTER)) {
        if (entry->dist > 1923.077f) {
            dist = 1923.077f;
        } else {
            dist = entry->dist;
        }

        highPassCutoff = (distParam * dist) / (2 * 1923.077f);
    } else {
        highPassCutoff = 0;
    }

    return (lowPassCutoff << 4) + highPassCutoff;
}

s8 AudioSfx_ComputeCombFilter(f32 posY, u16 sfxParams) {
    s8 combFilterGain;

    if (posY < 0.0f) {
        if (posY < -625.0f) {
            combFilterGain = -32;
        } else {
            combFilterGain = (s8)(((625.0f + posY) / 625.0f) * 31.0f) + 0xE0;
        }
    } else if (posY > 1250.0f) {
        combFilterGain = 127;
    } else {
        combFilterGain = (posY / 1250.0f) * 126.0f;
    }

    return combFilterGain | 1;
}

void AudioSfx_SetProperties(u8 bankId, u8 entryIndex, u8 channelIndex) {
    static s8 sZVolumes[] = {
        0,  2,  4,   6,   8,   10,  12,  14,  16,  18,  20,  22,  24,  25,  26,  27,  28,  29,  30, 31, 32, 33,
        34, 35, 36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52, 53, 54, 55,
        56, 56, 57,  57,  58,  58,  59,  59,  60,  60,  60,  61,  61,  61,  62,  62,  62,  63,  63, 63, 64, 64,
        64, 65, 65,  65,  66,  66,  66,  67,  67,  67,  68,  68,  69,  69,  70,  70,  71,  71,  72, 73, 74, 75,
        76, 77, 78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94, 95, 96, 97,
        98, 99, 100, 101, 102, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127,
    };
    f32 volume = 1.0f;
    s8 volumeS8;
    s8 reverb = 0;
    f32 freqScale = 1.0f;
    s8 panSigned = 0x40;
    u8 pad;
    u8 filter = 0;
    s8 combFilterGain = 0;
    s8 zVolume = -1;
    f32 behindScreenZ;
    u8 baseFilter = 0;
    SfxBankEntry* entry = &gSfxBanks[bankId][entryIndex];
    s32 pad2;

    switch (bankId) {
        case BANK_PLAYER:
        case BANK_ITEM:
        case BANK_ENV:
        case BANK_ENEMY:
        case BANK_VOICE:
            if (sSoundMode == SOUNDMODE_SURROUND_EXTERNAL) {
                combFilterGain = AudioSfx_ComputeCombFilter(*entry->posY, entry->sfxParams);
            }
            // fallthrough
        case BANK_OCARINA:
            entry->dist = sqrtf(entry->dist * 10.0f);
            volume = AudioSfx_ComputeVolume(bankId, entryIndex) * *entry->volume;
            reverb = AudioSfx_ComputeReverb(bankId, entryIndex, channelIndex);
            panSigned = AudioSfx_ComputePanSigned(*entry->posX, *entry->posZ, entry->token);
            freqScale = AudioSfx_ComputeFreqScale(bankId, entryIndex) * *entry->freqScale;

            if (sSoundMode == SOUNDMODE_SURROUND_EXTERNAL) {
                if (*entry->posZ >= 0.0f) {
                    if (*entry->posZ < 200.0f) {
                        zVolume = 32.0f - ((*entry->posZ / 300.0f) * 32.0f);
                    } else {
                        zVolume = 0;
                    }
                } else if (*entry->posZ < 0.0f) {
                    if (*entry->posZ > -300.0f) {
                        zVolume = 32.0f - ((*entry->posZ / 300.0f) * 95.0f);
                    } else {
                        zVolume = 127;
                    }
                }
                zVolume = sZVolumes[zVolume];
            }

            if (sAudioBaseFilter != 0) {
                if ((bankId == BANK_PLAYER) || (bankId == BANK_ITEM) ||
                    ((bankId == BANK_VOICE) && (((entry->sfxId & 0xFF) < 0x40) || ((entry->sfxId & 0xFF) >= 0x80)))) {
                    baseFilter = sAudioBaseFilter;
                    if (sAudioExtraFilter) {
                        reverb >>= 2;
                    }
                }
            }

            if (((*entry->reverbAdd & 0x80) | baseFilter | sAudioExtraFilter |
                 (entry->sfxFlags & SFX_FLAG2_APPLY_LOWPASS_FILTER)) != 0) {
                filter = ((((*entry->reverbAdd & 0x80) >> 2) | baseFilter | sAudioExtraFilter |
                           ((entry->sfxFlags & SFX_FLAG2_APPLY_LOWPASS_FILTER) >> 2)) *
                          2);
                filter &= 0xFF;
            } else if ((sSoundMode == SOUNDMODE_SURROUND_EXTERNAL) && !(entry->sfxParams & SFX_FLAG_VOLUME_NO_DIST)) {
                // BENTODO look into this
                filter = AudioSfx_ComputeSurroundSoundFilter(behindScreenZ, entry, panSigned);
            }
            break;

        case BANK_SYSTEM:
            break;
    }

    if (sSfxChannelState[channelIndex].volume != volume) {
        volumeS8 = (u8)(volume * 127.0f);
        sSfxChannelState[channelIndex].volume = volume;
    } else {
        volumeS8 = -1;
    }

    //! FAKE:
    if (1) {}

    // ioPort 2, sets volume
    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex, 2, volumeS8);

    if (sSfxChannelState[channelIndex].reverb != reverb) {
        sSfxChannelState[channelIndex].reverb = reverb;
    }
    if (sSfxChannelState[channelIndex].freqScale != freqScale) {
        sSfxChannelState[channelIndex].freqScale = freqScale;
    }
    if (sSfxChannelState[channelIndex].stereoBits != 0) {
        sSfxChannelState[channelIndex].stereoBits = 0;
    }
    if (sSfxChannelState[channelIndex].filter != filter) {
        sSfxChannelState[channelIndex].filter = filter;
    }
    if (sSfxChannelState[channelIndex].combFilterGain != combFilterGain) {
        sSfxChannelState[channelIndex].combFilterGain = combFilterGain;
    }
    if (sSfxChannelState[channelIndex].zVolume != zVolume) {
        sSfxChannelState[channelIndex].zVolume = zVolume;
    }
    if (sSfxChannelState[channelIndex].panSigned != panSigned) {
        sSfxChannelState[channelIndex].panSigned = panSigned;
    }
}

u32 AudioSfx_SetFreqAndStereoBits(u8 seqScriptValIn, SequenceChannel* channel) {
    channel->stereoData.asByte = sSfxChannelState[seqScriptValIn].stereoBits;
    channel->freqScale = sSfxChannelState[seqScriptValIn].freqScale;
    channel->changes.s.freqScale = true;

    return seqScriptValIn;
}

void AudioSfx_ResetSfxChannelState(void) {
    SfxChannelState* state;
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sSfxChannelState); i++) {
        state = &sSfxChannelState[i];
        state->volume = 1.0f;
        state->freqScale = 1.0f;
        state->reverb = 0;
        state->panSigned = 0x40;
        state->stereoBits = 0;
        state->filter = 0xFF;
        state->combFilterGain = 0xFF;
        state->zVolume = 0xFF;
    }

    sSfxChannelState[SFX_CHANNEL_OCARINA].combFilterGain = 0;
    sPrevSeqMode = SEQ_MODE_DEFAULT;
    sAudioCodeReverb = 0;
}

void Audio_PlaySfx(u16 sfxId) {
    AudioSfx_PlaySfx(sfxId, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                     &gSfxDefaultReverb);
    if (sfxId == NA_SE_OC_TELOP_IMPACT) {
        Audio_SetSequenceMode(SEQ_MODE_DEFAULT);
    }
}

void Audio_PlaySfx_2(u16 sfxId) {
    AudioSfx_PlaySfx(sfxId, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                     &gSfxDefaultReverb);
}

/**
 * Bends the pitch of the sfx by a little under two semitones and adds reverb
 */
void Audio_PlaySfx_AtPosWithPresetLowFreqAndHighReverb(Vec3f* pos, u16 sfxId) {
    AudioSfx_PlaySfx(sfxId, pos, 4, &sTwoSemitonesLoweredFreq, &gSfxDefaultFreqAndVolScale, &sSfxIncreasedReverb);
}

void Audio_PlaySfx_AtPos(Vec3f* pos, u16 sfxId) {
    AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void Audio_PlaySfx_MessageDecide(void) {
    Audio_PlaySfx(NA_SE_SY_DECIDE);
    AudioSfx_StopById(NA_SE_SY_MESSAGE_PASS);
}

void Audio_PlaySfx_MessageCancel(void) {
    Audio_PlaySfx(NA_SE_SY_CANCEL);
    AudioSfx_StopById(NA_SE_SY_MESSAGE_PASS);
}

SfxSettings* AudioSfx_AddSfxSetting(Vec3f* pos) {
    SfxSettings* sfxSettings;
    u8 i = 0;
    u8 sfxSettingIndex = 0xFF;

    for (; i < ARRAY_COUNT(sSfxSettings); i++) {
        sfxSettings = &sSfxSettings[i];

        if ((sfxSettings->pos == NULL) && (sfxSettingIndex == 0xFF)) {
            sfxSettingIndex = i;
        }

        if (sfxSettings->pos == pos) {
            return sfxSettings;
        }
    }

    if (sfxSettingIndex != 0xFF) {
        sSfxSettingsFlags |= 1 << sfxSettingIndex;
        sfxSettings = &sSfxSettings[sfxSettingIndex];
        sfxSettings->pos = pos;
        return sfxSettings;
    }

    return NULL;
}

void AudioSfx_ProcessSfxSettings(void) {
    SfxBankEntry* entry;
    u8 sfxSettingsFlags;
    u8 sfxSettingIndex = 0;
    u8 bankId;
    u8 entryIndex;
    s32 found;

    if (sSfxSettingsFlags != 0) {
        sfxSettingsFlags = sSfxSettingsFlags;

        while (sfxSettingsFlags != 0) {
            bankId = BANK_ENV;

            if ((sfxSettingsFlags & (1 << sfxSettingIndex))) {
                found = false;
                while ((bankId <= BANK_ENEMY) && !found) {
                    entryIndex = gSfxBanks[bankId]->next;

                    while (entryIndex != 0xFF) {
                        entry = &gSfxBanks[bankId][entryIndex];
                        entryIndex = 0xFF;
                        if (entry->posX == &sSfxSettings[sfxSettingIndex].pos->x) {
                            found = true;
                        } else {
                            entryIndex = entry->next;
                        }
                    }
                    bankId++;
                }

                if (!found) {
                    sSfxSettingsFlags ^= (1 << sfxSettingIndex);
                    sSfxSettings[sfxSettingIndex].pos = NULL;
                }

                sfxSettingsFlags ^= (1 << sfxSettingIndex);
            }
            sfxSettingIndex++;
        }
    }
}

/**
 * Used for Gyorg and Bigslime
 */
void Audio_PlaySfx_Underwater(Vec3f* pos, u16 sfxId) {
    if ((sfxId == NA_SE_EN_KONB_JUMP_OLD) || (sfxId == NA_SE_EN_KONB_SINK_OLD)) {
        AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else {
        AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                         &gUnderwaterSfxReverbAdd);
    }
}

/**
 * Used only for eating the goron sirloin by the goron with Don Gero's Mask
 */
void Audio_PlaySfx_WithSfxSettingsReverb(Vec3f* pos, u16 sfxId) {
    SfxSettings* sfxSettings;

    if ((sfxId == NA_SE_EN_KONB_JUMP_OLD) || (sfxId == NA_SE_EN_KONB_SINK_OLD)) {
        AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else {
        sfxSettings = AudioSfx_AddSfxSetting(pos);

        if (sfxSettings != NULL) {
            AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                             &sfxSettings->reverbAdd);
        }
    }
}

void Audio_SetSfxUnderwaterReverb(s8 isUnderwaterReverbActivated) {
    if (isUnderwaterReverbActivated) {
        gUnderwaterSfxReverbAdd = -0x80;
    } else {
        gUnderwaterSfxReverbAdd = 0;
    }
}

void AudioSfx_LowerSfxSettingsReverb(Vec3f* pos, s8 isReverbLowered) {
    SfxSettings* sfxSettings = AudioSfx_AddSfxSetting(pos);

    if (sfxSettings != NULL) {
        if (isReverbLowered) {
            sfxSettings->reverbAdd = -0x80;
        } else {
            sfxSettings->reverbAdd = 0;
        }
    }
}

f32 Audio_SetSyncedSfxFreqAndVolume(f32 freqVolParam) {
    f32 ret = 1.0f;

    if (freqVolParam > 6.0f) {
        sSfxSyncedVolume = 1.0f;
        sSfxSyncedFreq = 1.1f;
    } else {
        ret = freqVolParam / 6.0f;
        sSfxSyncedVolume = ret * (1.0f - 0.775f) + 0.775f;
        sSfxSyncedFreq = (ret * 0.2f) + 0.9f;
    }

    return ret;
}

/**
 * Adjusts both frequency and volume based on a single parameter "freqVolParam"
 * When freqVolParam >= 6.0f, frequency is increased to 1.1f and volume remains fixed at 1.0f
 * For every -1.0f taken from freqVolParam (eg. 5.0f, 4.0f, 3.0f...):
 *     - volume will decrease by 0.0375f
 *     - frequency will decrease by 0.0333333f
 */
void Audio_PlaySfx_AtPosForMetalEffectsWithSyncedFreqAndVolume(Vec3f* pos, u16 sfxId, f32 freqVolParam) {
    f32 sp2C;
    f32 phi_f0;
    u8 phi_v0;
    u16 metalSfxId = NA_SE_NONE;

    sp2C = Audio_SetSyncedSfxFreqAndVolume(freqVolParam);
    AudioSfx_PlaySfx(sfxId, pos, 4, &sSfxSyncedFreq, &sSfxSyncedVolume, &gSfxDefaultReverb);

    if ((sfxId & 0xF0) == 0xB0) {
        // Crawlspaces (unused remnant of OoT)
        phi_f0 = 0.3f;
        phi_v0 = true;
        sp2C = 1.0f;
    } else {
        phi_f0 = 1.1f;
        phi_v0 = gAudioCtx.audioRandom & 1;
    }

    if (phi_f0 < freqVolParam) {
        if (phi_v0) {
            if ((sfxId & 0x1FF) < 0x80) {
                metalSfxId = NA_SE_PL_METALEFFECT_KID;
            } else if ((sfxId & 0x1FF) < 0xF0) {
                metalSfxId = NA_SE_PL_METALEFFECT_ADULT;
            }

            if (metalSfxId != NA_SE_NONE) {
                sSfxSyncedVolumeForMetalEffects = (sp2C * 0.7) + 0.3;
                AudioSfx_PlaySfx(metalSfxId, pos, 4, &sSfxSyncedFreq, &sSfxSyncedVolumeForMetalEffects,
                                 &gSfxDefaultReverb);
            }
        }
    }
}

/**
 * Adjusts both frequency and volume based on a single parameter "freqVolParam"
 * When freqVolParam >= 6.0f, frequency is increased to 1.1f and volume remains fixed at 1.0f
 * For every -1.0f taken from freqVolParam (eg. 5.0f, 4.0f, 3.0f...):
 *     - volume will decrease by 0.0375f
 *     - frequency will decrease by 0.0333333f
 */
void Audio_PlaySfx_AtPosWithSyncedFreqAndVolume(Vec3f* pos, u16 sfxId, f32 freqVolParam) {
    Audio_SetSyncedSfxFreqAndVolume(freqVolParam);
    AudioSfx_PlaySfx(sfxId, pos, 4, &sSfxSyncedFreq, &sSfxSyncedVolume, &gSfxDefaultReverb);
}

void Audio_PlaySfx_GiantsMaskUnused(Vec3f* pos, u16 sfxId) {
    AudioSfx_PlaySfx(sfxId | 0xE0, pos, 4, &sGiantsMaskFreq, &gSfxDefaultFreqAndVolScale, &sGiantsMaskReverbAdd);
}

void Audio_PlaySfx_GiantsMask(Vec3f* pos, u16 sfxId) {
    AudioSfx_PlaySfx((sfxId & 0x681F) + 0x20, pos, 4, &sGiantsMaskFreq, &gSfxDefaultFreqAndVolScale,
                     &sGiantsMaskReverbAdd);
}

void Audio_PlaySfx_Randomized(Vec3f* pos, u16 baseSfxId, u8 randLim) {
    u8 offset = AudioThread_NextRandom() % randLim;

    AudioSfx_PlaySfx(baseSfxId + offset, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                     &gSfxDefaultReverb);
}

/**
 * Plays increasingly high-pitched sword charging sfx as Player charges up the sword
 */
void Audio_PlaySfx_SwordCharge(Vec3f* pos, u8 chargeLevel) {
    chargeLevel %= 4U;
    if (chargeLevel != sPrevChargeLevel) {
        sCurChargeLevelSfxFreq = sChargeLevelsSfxFreq[chargeLevel];
        switch (chargeLevel) {
            case 1:
                AudioSfx_PlaySfx(NA_SE_PL_SWORD_CHARGE, pos, 4, &sCurChargeLevelSfxFreq, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultReverb);
                break;

            case 2:
                AudioSfx_PlaySfx(NA_SE_PL_SWORD_CHARGE, pos, 4, &sCurChargeLevelSfxFreq, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultReverb);
                break;

            default:
                break;
        }
        sPrevChargeLevel = chargeLevel;
    }

    if (chargeLevel != 0) {
        AudioSfx_PlaySfx(NA_SE_IT_SWORD_CHARGE - SFX_FLAG, pos, 4, &sCurChargeLevelSfxFreq, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultReverb);
    }
}

void Audio_PlaySfx_AtPosWithFreqAndVolume(Vec3f* pos, u16 sfxId, f32 freqScale, f32* volume) {
    SfxSettings* sfxSettings = AudioSfx_AddSfxSetting(pos);
    f32* freqScaleAdj;

    if (sfxSettings != NULL) {
        freqScaleAdj = &sfxSettings->freqScale;
        if (freqScale < 0.75f) {
            *freqScaleAdj = ((freqScale / 0.75f) * 0.25f) + 0.5f;
        } else {
            *freqScaleAdj = freqScale;
        }

        if (*freqScaleAdj > 0.5f) {
            AudioSfx_PlaySfx(sfxId, pos, 4, freqScaleAdj, volume, &gSfxDefaultReverb);
        }
    }
}

void Audio_PlaySfx_AtPosWithFreq(Vec3f* pos, u16 sfxId, f32 freqScale) {
    Audio_PlaySfx_AtPosWithFreqAndVolume(pos, sfxId, freqScale, &gSfxDefaultFreqAndVolScale);
}

void Audio_PlaySfx_AtPosWithFreqAndChannelIO(Vec3f* pos, u16 sfxId, f32 freqScale, u8 arg3) {
    if (freqScale > 1.0f) {
        freqScale = 1.0f;
    }

    AudioSfx_SetChannelIO(pos, sfxId, (arg3 - (u32)(freqScale * arg3)) & 0xFF);
    Audio_PlaySfx_AtPosWithFreq(pos, sfxId, freqScale);
}

void Audio_PlaySfx_WaterWheel(Vec3f* pos, u16 sfxId) {
    u8 isWaterWheelSfxNotPlaying = false;

    switch (sfxId) {
        case NA_SE_EV_DUMMY_WATER_WHEEL_LR - SFX_FLAG:
            if (!AudioSfx_IsPlaying(NA_SE_EV_BIG_WATER_WHEEL_LR - SFX_FLAG)) {
                isWaterWheelSfxNotPlaying = true;
            }
            break;

        case NA_SE_EV_DUMMY_WATER_WHEEL_RR - SFX_FLAG:
            if (!AudioSfx_IsPlaying(NA_SE_EV_BIG_WATER_WHEEL_RR - SFX_FLAG)) {
                isWaterWheelSfxNotPlaying = true;
            }
            break;

        default:
            break;
    }

    if (isWaterWheelSfxNotPlaying) {
        AudioSfx_SetChannelIO(pos, sfxId, 0);
        Audio_PlaySfx_AtPosWithFreqAndVolume(pos, sfxId, 1.0f, &sWaterWheelVolume);
    }
}

/**
 * at timerShiftedLerp == 1: use sSfxTimerLerpRange1
 * at timerShiftedLerp == 2: use sSfxTimerLerpRange2
 *
 * sSfxAdjustedFreq was modified in OoT, but remains 1.0f in MM
 *
 * Used for "NA_SE_IT_DEKUNUTS_FLOWER_ROLL" and "NA_SE_IT_FISHING_REEL_SLOW"
 */
void Audio_PlaySfx_AtPosWithTimer(Vec3f* pos, u16 sfxId, f32 timerShiftedLerp) {
    sSfxTimer--;
    if (sSfxTimer == 0) {
        AudioSfx_PlaySfx(sfxId, pos, 4, &sSfxAdjustedFreq, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

        if (timerShiftedLerp > 2.0f) {
            timerShiftedLerp = 2.0f;
        }

        // Linear interpolation between "sSfxTimerLerpRange1" and "sSfxTimerLerpRange2" from lerp factor
        // (timerShiftedLerp - 1.0f)
        sSfxTimer = (s8)((sSfxTimerLerpRange1 - sSfxTimerLerpRange2) * (1.0f - timerShiftedLerp)) + sSfxTimerLerpRange1;
    }
}

void Audio_SetSfxTimerLerpInterval(s8 timerLerpRange1, s8 timerLerpRange2) {
    sSfxTimer = 1;
    sSfxTimerLerpRange2 = timerLerpRange2;
    sSfxTimerLerpRange1 = timerLerpRange1;
}

void Audio_PlaySfx_AtPosWithReverb(Vec3f* pos, u16 sfxId, s8 reverbAdd) {
    sSfxCustomReverb = reverbAdd;
    AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &sSfxCustomReverb);
}

void Audio_PlaySfx_AtPosWithVolume(Vec3f* pos, u16 sfxId, f32 volume) {
    gSfxVolume = volume;
    AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxVolume, &gSfxDefaultReverb);
}

void Audio_SetSfxVolumeTransition(f32* volume, f32 volumeTarget, u16 duration) {
    sSfxVolumeCur = volume;
    sSfxVolumeTarget = volumeTarget;
    sSfxVolumeDuration = duration;
    sSfxVolumeRate = (*sSfxVolumeCur - sSfxVolumeTarget) / sSfxVolumeDuration;
}

// Part of audio update (runs every frame)
void Audio_UpdateSfxVolumeTransition(void) {
    if (sSfxVolumeDuration != 0) {
        sSfxVolumeDuration--;
        if (sSfxVolumeDuration == 0) {
            *sSfxVolumeCur = sSfxVolumeTarget;
        } else {
            *sSfxVolumeCur -= sSfxVolumeRate;
        }
    }
}

void Audio_PlaySfx_FishingReel(f32 timerShiftedLerp) {
    Audio_PlaySfx_AtPosWithTimer(&gSfxDefaultPos, NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG, timerShiftedLerp);
    Audio_PlaySfx_AtPosWithFreq(&gSfxDefaultPos, 0, (0.15f * timerShiftedLerp) + 1.4f);
}

/**
 * Used for EnRiverSound
 */
void Audio_PlaySfx_River(Vec3f* pos, f32 freqScale) {
    if (!AudioSfx_IsPlaying(NA_SE_EV_RIVER_STREAM - SFX_FLAG)) {
        sRiverFreqScaleLerp.value = freqScale;
    } else if (freqScale != sRiverFreqScaleLerp.value) {
        sRiverFreqScaleLerp.target = freqScale;
        sRiverFreqScaleLerp.remainingFrames = 40;
        sRiverFreqScaleLerp.step = (sRiverFreqScaleLerp.target - sRiverFreqScaleLerp.value) / 40;
    }
    AudioSfx_PlaySfx(NA_SE_EV_RIVER_STREAM - SFX_FLAG, pos, 4, &sRiverFreqScaleLerp.value, &gSfxDefaultFreqAndVolScale,
                     &gSfxDefaultReverb);
}

/**
 * Unused remnant of OoT's EnRiverSound
 * Used for Zora's River Waterfall
 */
void Audio_PlaySfx_Waterfall(Vec3f* pos, f32 freqScale) {
    if (!AudioSfx_IsPlaying(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG)) {
        sWaterfallFreqScaleLerp.value = freqScale;
    } else if (freqScale != sWaterfallFreqScaleLerp.value) {
        sWaterfallFreqScaleLerp.target = freqScale;
        sWaterfallFreqScaleLerp.remainingFrames = 40;
        sWaterfallFreqScaleLerp.step = (sWaterfallFreqScaleLerp.target - sWaterfallFreqScaleLerp.value) / 40;
    }
    AudioSfx_PlaySfx(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG, pos, 4, &sWaterfallFreqScaleLerp.value,
                     &sWaterfallFreqScaleLerp.value, &gSfxDefaultReverb);
}

/**
 * Used for EnRiverSound variables
 */
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

f32 sBigBellsVolume[] = {
    1.0f, 0.9f, 0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f,
};

void Audio_PlaySfx_BigBells(Vec3f* pos, u8 volumeIndex) {
    AudioSfx_PlaySfx(NA_SE_EV_SIGNAL_BIGBELL, pos, 4, &gSfxDefaultFreqAndVolScale, &sBigBellsVolume[volumeIndex & 7],
                     &gSfxDefaultReverb);
}

void Audio_SetBgmVolumeOff(void) {
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0, 10);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0, 10);
}

void Audio_SetBgmVolumeOn(void) {
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0x7F, 3);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0x7F, 3);
}

// 2S2H [Custom Audio] seqId was s8. Updated to allow more than 255 sequences
void func_801A0204(s16 seqId) {
    AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_BGM_MAIN, 2, seqId);
}

void Audio_SetMainBgmVolume(u8 targetVolume, u8 volumeFadeTimer) {
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, targetVolume, volumeFadeTimer);
}

/**
 * Unused remnant from OoT's EnRiverSound
 * Was designed to incrementally increase volume of NA_BGM_GANON_TOWER for each new room during the climb of Ganon's
 * Tower
 */
void Audio_SetGanonsTowerBgmVolumeLevel(u8 ganonsTowerLevel) {
    u8 channelIndex;
    s8 panChannelWeight = 0;

    // Ganondorfs's Lair
    if (ganonsTowerLevel == 0) {
        // Pan comes entirely from the SequenceChannel
        panChannelWeight = 0x7F;
    }

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        AUDIOCMD_CHANNEL_SET_PAN_WEIGHT(SEQ_PLAYER_BGM_MAIN, channelIndex, panChannelWeight);
    }

    // Lowest room in Ganon's Tower (Entrance Room)
    if (ganonsTowerLevel == 7) {
        // Adds a delay to setting the volume in the first room
        sEnterGanonsTowerTimer = 2;
    } else {
        Audio_SetGanonsTowerBgmVolume(sGanonsTowerLevelsVol[ganonsTowerLevel % ARRAY_COUNTU(sGanonsTowerLevelsVol)]);
    }
}

/**
 * Unused remnant from OoT's EnRiverSound
 * If a new volume is requested for ganon's tower, update the volume and
 * calculate a new low-pass filter cutoff and reverb based on the new volume
 */
s32 Audio_SetGanonsTowerBgmVolume(u8 targetVolume) {
    u8 lowPassFilterCutoff;
    u8 channelIndex;
    u16 reverb;

    if (sGanonsTowerVol != targetVolume) {
        // Sets the volume
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, targetVolume, 2);

        // Sets the filter cutoff of the form (lowPassFilterCutoff << 4) | (highPassFilter & 0xF). highPassFilter is
        // always set to 0
        if (targetVolume < 0x40) {
            // Only the first room
            lowPassFilterCutoff = 0x10;
        } else {
            // Higher volume leads to a higher cut-off frequency in the low-pass filtering
            lowPassFilterCutoff = (((targetVolume - 0x40) >> 2) + 1) << 4;
        }

        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, 15, 4, lowPassFilterCutoff);

        // Sets the reverb
        for (channelIndex = 0; channelIndex < ARRAY_COUNT(gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels);
             channelIndex++) {
            if (&gAudioCtx.sequenceChannelNone != gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex]) {
                // seqScriptIO[5] was originally set to 0x40 in channels 0, 1, and 4 by OoT's Ganon's Tower BGM,
                // which was removed in MM
                if ((u8)gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex]->seqScriptIO[5] !=
                    (u8)SEQ_IO_VAL_NONE) {
                    // Higher volume leads to lower reverb
                    reverb = (((u16)gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex]->seqScriptIO[5] -
                               targetVolume) +
                              0x7F);

                    if (reverb > 0x7F) {
                        reverb = 0x7F;
                    }

                    AUDIOCMD_CHANNEL_SET_REVERB_VOLUME(SEQ_PLAYER_BGM_MAIN, channelIndex, (u8)reverb);
                }
            }
        }

        sGanonsTowerVol = targetVolume;
    }
    return -1;
}

/**
 * Unused remnant from OoT's EnRiverSound
 * Responsible for lowering market bgm in Child Market Entrance and Child Market Back Alley
 * Only lowers volume for 1 frame, so must be called every frame to maintain lower volume
 */
void Audio_LowerMainBgmVolume(u8 volume) {
    sRiverSoundMainBgmVol = volume;
    sRiverSoundMainBgmLower = true;
}

/**
 * Unused remnant from OoT's EnRiverSound
 * Still called by Audio_Update every frame, but none of these processes get initialized
 */
void Audio_UpdateRiverSoundVolumes(void) {
    // Updates Main Bgm Volume (RiverSound of type RS_LOWER_MAIN_BGM_VOLUME)
    if (sRiverSoundMainBgmLower == true) {
        if (sRiverSoundMainBgmCurrentVol != sRiverSoundMainBgmVol) {
            // lowers the volume for 1 frame
            AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, sRiverSoundMainBgmVol, 10);
            sRiverSoundMainBgmCurrentVol = sRiverSoundMainBgmVol;
            sRiverSoundMainBgmRestore = true;
        }
        sRiverSoundMainBgmLower = false;
    } else if ((sRiverSoundMainBgmRestore == true) && (sAudioPauseMenuOpenOrClose == SFX_PAUSE_MENU_CLOSE)) {
        // restores the volume every frame
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, 0x7F, 10);
        sRiverSoundMainBgmCurrentVol = 0x7F;
        sRiverSoundMainBgmRestore = false;
    }

    // Update Ganon's Tower Volume (RiverSound of type RS_GANON_TOWER_7)
    if (sEnterGanonsTowerTimer != 0) {
        sEnterGanonsTowerTimer--;
        if (sEnterGanonsTowerTimer == 0) {
            Audio_SetGanonsTowerBgmVolume(sGanonsTowerLevelsVol[7]);
        }
    }
}

// Unused remnant of OoT
void Audio_PlaySfx_IncreasinglyTransposed(Vec3f* pos, s16 sfxId, u8* semitones) {
    AudioSfx_PlaySfx(sfxId, pos, 4, &gPitchFrequencies[semitones[sAudioIncreasingTranspose] + 39],
                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    if (sAudioIncreasingTranspose < 15) {
        sAudioIncreasingTranspose++;
    }
}

// Unused remnant of OoT
void Audio_ResetIncreasingTranspose(void) {
    sAudioIncreasingTranspose = 0;
}

// Unused remnant of OoT
void Audio_PlaySfx_Transposed(Vec3f* pos, u16 sfxId, s8 semitone) {
    AudioSfx_PlaySfx(sfxId, pos, 4, &gPitchFrequencies[semitone + 39], &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void AudioSfx_SetChannelIO(Vec3f* pos, u16 sfxId, u8 ioData) {
    u8 channelIndex = 0;
    u8 bankId = SFX_BANK_SHIFT(sfxId);
    u8 entryIndex;
    u8 i;

    for (i = 0; i < bankId; i++) {
        channelIndex += gChannelsPerBank[gSfxChannelLayout][i];
    }

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        entryIndex = gActiveSfx[bankId][i].entryIndex;

        if (entryIndex != 0xFF) {
            if ((sfxId == gSfxBanks[bankId][entryIndex].sfxId) && (&pos->x == gSfxBanks[bankId][entryIndex].posX)) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex, 6, ioData);
            }
        }
        channelIndex++;
    }
}

/**
 * Plays sfx and sets ioData to io port 6 if the sfx is active
 */
void Audio_PlaySfx_AtPosWithChannelIO(Vec3f* pos, u16 sfxId, u8 ioData) {
    AudioSfx_SetChannelIO(pos, sfxId, ioData);
    AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

/**
 * Plays sfx and sets ioData to io port 6
 */
void Audio_PlaySfx_AtPosWithAllChannelsIO(Vec3f* pos, u16 sfxId, u8 ioData) {
    u8 channelIndex = 0;
    u8 i;
    u8 bankId = SFX_BANK_SHIFT(sfxId);

    for (i = 0; i < bankId; i++) {
        channelIndex += gChannelsPerBank[gSfxChannelLayout][i];
    }

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex++, 6, ioData);
    }

    AudioSfx_PlaySfx(sfxId, pos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

/**
 * Unused remnant of OoT's EnRiverSound (func_800F4E30)
 */
void func_801A09D4(Vec3f* pos, f32 xzDistToPlayer) {
    f32 volumeRel;
    s8 pan;
    u8 channelIndex;

    if (sRiverSoundBgmPos == NULL) {
        sRiverSoundBgmPos = pos;
        sRiverSoundXZDistToPlayer = xzDistToPlayer;
    } else if (sRiverSoundBgmPos != pos) {
        if (sRiverSoundXZDistToPlayer > xzDistToPlayer) {
            sRiverSoundBgmPos = pos;
            sRiverSoundXZDistToPlayer = xzDistToPlayer;
        }
    } else {
        sRiverSoundXZDistToPlayer = xzDistToPlayer;
    }

    if (sRiverSoundBgmPos->x > 100.0f) {
        pan = 0x7F;
    } else if (sRiverSoundBgmPos->x < -100.0f) {
        pan = 0;
    } else {
        pan = ((sRiverSoundBgmPos->x / 100.0f) * 64.0f) + 64.0f;
    }

    if (sRiverSoundXZDistToPlayer > 400.0f) {
        volumeRel = 0.1f;
    } else if (sRiverSoundXZDistToPlayer < 120.0f) {
        volumeRel = 1.0f;
    } else {
        volumeRel = ((1.0f - ((sRiverSoundXZDistToPlayer - 120.0f) / 280.0f)) * 0.9f) + 0.1f;
    }

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        if (channelIndex != 9) {
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, channelIndex, 2, (u8)(127.0f * volumeRel));
            AUDIOCMD_CHANNEL_SET_PAN(SEQ_PLAYER_BGM_MAIN, channelIndex, pan);
        }
    }
}

/**
 * Unused remnant of OoT's EnRiverSound
 */
void Audio_ClearSariaBgm(void) {
    if (sRiverSoundBgmPos != NULL) {
        sRiverSoundBgmPos = NULL;
    }
}

/**
 * Unused remnant of OoT's EnRiverSound
 */
void Audio_ClearSariaBgmAtPos(Vec3f* pos) {
    if (sRiverSoundBgmPos == pos) {
        sRiverSoundBgmPos = NULL;
    }
}

/**
 * Turns on and off channels from both bgm players in a way that splits
 * equally between the two bgm channels. Split based on note priority
 */
void Audio_SplitBgmChannels(s8 volumeSplit) {
    u8 volume;
    u8 notePriority;
    u16 channelBits;
    u8 sBgmPlayers[2] = { SEQ_PLAYER_BGM_MAIN, SEQ_PLAYER_BGM_SUB };
    u8 i;
    u8 channelIndex;

    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE) == NA_BGM_DISABLED) &&
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) != NA_BGM_ROMANI_RANCH)) {
        for (i = 0; i < ARRAY_COUNT(sBgmPlayers); i++) {
            if (i == 0) {
                // Main Bgm SeqPlayer
                volume = volumeSplit;
            } else {
                // Sub Bgm SeqPlayer
                volume = 0x7F - volumeSplit;
            }

            if (volume > 100) {
                notePriority = 11;
            } else if (volume < 20) {
                notePriority = 2;
            } else {
                notePriority = ((volume - 20) / 10) + 2;
            }

            channelBits = 0;
            for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
                if (gAudioCtx.seqPlayers[sBgmPlayers[i]].channels[channelIndex]->notePriority < notePriority) {
                    // If the note currently playing in the channel is a high enough priority,
                    // then keep the channel on by setting a channelBit
                    // If this condition fails, then the channel will be shut off
                    channelBits += (1 << channelIndex);
                }
            }

            SEQCMD_SET_CHANNEL_DISABLE_MASK(sBgmPlayers[i], channelBits);
        }
    }
}

void Audio_SetSequenceProperties(u8 seqPlayerIndex, Vec3f* projectedPos, s16 flags, f32 minDist, f32 maxDist,
                                 f32 maxVolume, f32 minVolume) {
    f32 dist;
    f32 volume;
    s8 surroundEffectIndex;
    s8 pan;
    s32 pad;

    // calculating surround sound effect from screen depth
    if (projectedPos->z > 0.0f) {
        if (projectedPos->z > 100.0f) {
            surroundEffectIndex = 0;
        } else {
            surroundEffectIndex = ((100.0f - projectedPos->z) / 100.0f) * 64.0f;
        }
    } else {
        if (projectedPos->z < -100.0f) {
            surroundEffectIndex = 0x7F;
        } else {
            surroundEffectIndex = (s8)((-projectedPos->z / 100.0f) * 64.0f) + 0x3F;
        }
    }

    // calculating pan from left-right screen position
    if (projectedPos->x > 0.0f) {
        if (projectedPos->x > 200.0f) {
            pan = 0x6C;
        } else {
            pan = (s8)((projectedPos->x / 200.0f) * 45.0f) + 0x3F;
        }
    } else {
        if (projectedPos->x < -200.0f) {
            pan = 0x14;
        } else {
            pan = (s8)(((projectedPos->x + 200.0f) / 200.0f) * 44.0f) + 0x14;
        }
    }

    // Calculate weighted volume
    dist = SEQ_SCREEN_WEIGHTED_DIST(projectedPos);

    if (dist > maxDist) {
        volume = minVolume;
    } else if (dist < minDist) {
        volume = 1.0f;
    } else {
        volume = (((maxDist - dist) / (maxDist - minDist)) * (1.0f - minVolume)) + minVolume;
    }

    AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, 0xFFFF);

    if (flags & 1) {
        AUDIOCMD_CHANNEL_SET_SURROUND_EFFECT_INDEX(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, surroundEffectIndex);
    }

    if (flags & 2) {
        AUDIOCMD_CHANNEL_SET_PAN(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, pan);
    }

    if (flags & 4) {
        AUDIOCMD_CHANNEL_SET_PAN_WEIGHT(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0x7F);
    }

    // applies filter, stores result in sSequenceFilter
    if (flags & 8) {
        // Uses new filter gBandPassFilterData
        AUDIOCMD_CHANNEL_SET_FILTER(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0x54,
                                    ((uintptr_t)&sSequenceFilter[0] & ~0xF) + 0x10);
    } else {
        // Identity Filter
        AUDIOCMD_CHANNEL_SET_FILTER(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0,
                                    ((uintptr_t)&sSequenceFilter[0] & ~0xF) + 0x10);
    }

    if (flags & 0x10) {
        AUDIOCMD_CHANNEL_SET_GAIN(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0x7F);
    } else {
        AUDIOCMD_CHANNEL_SET_GAIN(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0);
    }

    if (flags & 0x20) {
        AUDIOCMD_CHANNEL_SET_VOL_SCALE(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, maxVolume * volume);
    }

    if (flags & 0x40) {
        AUDIOCMD_CHANNEL_SET_PAN_WEIGHT(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 0x40);
    }

    if (flags & 0x80) {
        AUDIOCMD_CHANNEL_SET_REVERB_INDEX(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 1);
    }

    if (flags & 0x100) {
        AUDIOCMD_CHANNEL_SET_REVERB_VOLUME(seqPlayerIndex, AUDIOCMD_ALL_CHANNELS, 55);
    }
}

// ======== BEGIN Z_OBJ_SOUND FUNCTIONS ========

/**
 * Used for Main Bgm and Fanfares
 */
void Audio_UpdateObjSoundProperties(void) {
    if (sObjSoundPlayerIndex != SEQ_PLAYER_INVALID) {
        if ((AudioSeq_GetActiveSeqId(sObjSoundPlayerIndex) != NA_BGM_FINAL_HOURS) &&
            AudioSeq_IsSeqCmdNotQueued((sObjSoundPlayerIndex << 24) + NA_BGM_FINAL_HOURS,
                                       SEQCMD_OP_MASK | SEQCMD_ASYNC_ACTIVE | SEQCMD_SEQPLAYER_MASK |
                                           SEQCMD_SEQID_MASK) &&
            !sIsFinalHoursOrSoaring) {
            Audio_SetSequenceProperties(sObjSoundPlayerIndex, &sObjSoundPos, sObjSoundFlags, sObjSoundMinDist,
                                        sObjSoundMaxDist, sObjSoundMaxVol, sObjSoundMinVol);
        }
        sObjSoundPlayerIndex = SEQ_PLAYER_INVALID;
    }
}

/**
 * Used for Main Bgm and Fanfares
 */
void Audio_SetObjSoundProperties(u8 seqPlayerIndex, Vec3f* pos, s16 flags, f32 minDist, f32 maxDist, f32 maxVolume,
                                 f32 minVolume) {
    sObjSoundPlayerIndex = seqPlayerIndex;
    sObjSoundPos.x = pos->x;
    sObjSoundPos.y = pos->y;
    sObjSoundPos.z = pos->z;
    sObjSoundFlags = flags;
    sObjSoundMinDist = minDist;
    sObjSoundMaxDist = maxDist;
    sObjSoundMaxVol = maxVolume;
    sObjSoundMinVol = minVolume;
}

// 2S2H [Custom Audio] Arg 3 was s8 seqId. Allow more that 255 sequences. Various casts removed in the function
void Audio_StartObjSoundFanfare(u8 seqPlayerIndex, Vec3f* pos, s16 seqId, u16 seqIdOffset) {
    s32 pad[3];
    u32 mask;

    if ((AudioSeq_GetActiveSeqId(seqPlayerIndex) == NA_BGM_FINAL_HOURS) ||
        !AudioSeq_IsSeqCmdNotQueued((seqPlayerIndex << 0x18) + NA_BGM_FINAL_HOURS,
                                    SEQCMD_OP_MASK | SEQCMD_ASYNC_ACTIVE | SEQCMD_SEQPLAYER_MASK | SEQCMD_SEQID_MASK) ||
        sIsFinalHoursOrSoaring) {
        sIsFinalHoursOrSoaring = true;
    } else if (pos != NULL) {
        if ((seqId != AudioSeq_GetActiveSeqId(seqPlayerIndex) & 0xFFFF) &&
            !gAudioCtx.seqPlayers[seqPlayerIndex].enabled && (sObjSoundMainBgmSeqId == NA_BGM_GENERAL_SFX)) {

            mask = 0xFFFF;

            SEQCMD_PLAY_SEQUENCE(seqPlayerIndex, ((((AudioThread_NextRandom() % 30) & 0xFF) + 1)),
                                 ((seqId & mask) + seqIdOffset));
            sObjSoundMainBgmSeqId = seqId;
        }

        Audio_SetObjSoundProperties(seqPlayerIndex, pos, 0x7F, 320.0f, 1280.0f, 1.0f, 0.0f);
    } else {
        SEQCMD_STOP_SEQUENCE(seqPlayerIndex, 5);
    }
}

// 2S2H [Custom Audio] Arg 2 was s8 seqId. Allow more that 255 sequences. Various casts removed in the function
void Audio_PlayObjSoundBgm(Vec3f* pos, s16 seqId) {
    s32 pad[2];
    u16 sp36;
    s32 sp2C;
    u16 seqId0 = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    u32 temp_a0;

    if ((seqId0 == NA_BGM_FINAL_HOURS) ||
        !AudioSeq_IsSeqCmdNotQueued(NA_BGM_FINAL_HOURS,
                                    SEQCMD_OP_MASK | SEQCMD_ASYNC_ACTIVE | SEQCMD_SEQPLAYER_MASK | SEQCMD_SEQID_MASK) ||
        sIsFinalHoursOrSoaring) {
        sIsFinalHoursOrSoaring = true;
        return;
    }

    if (seqId0 == NA_BGM_SONG_OF_SOARING) {
        sIsFinalHoursOrSoaring = true;
    }

    if (pos != NULL) {
        if (seqId == NA_BGM_ASTRAL_OBSERVATORY) {

            if ((seqId != seqId0) && !sAllPlayersMutedExceptSystemAndOcarina) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, seqId);
                sObjSoundMainBgmSeqId = seqId;
            } else if ((seqId == seqId0) && (sObjSoundMainBgmSeqId == NA_BGM_GENERAL_SFX)) {
                sObjSoundMainBgmSeqId = seqId;
            }

            Audio_SetObjSoundProperties(SEQ_PLAYER_BGM_MAIN, pos, 0x20, 100.0f, 1500.0f, 0.9f, 0.0f);
        } else {
            if (sObjSoundMainBgmSeqId == NA_BGM_GENERAL_SFX) {
                // 2S2H [Custom Audio] was originally `seqId + 7F00` This value did not work with the bit packing done
                // for 16 bit seqIds.
                /*
                 *  0x7F00 -> 0b0111 1111 0000 0000
                 *  0x7800 -> 0b0111 1000 0000 0000
                 *  0x07FF -> 0b0000 0111 1111 1111
                 *  The 7F00 mask will erase the top bits of the sequence id.
                 */
                temp_a0 = ((((AudioThread_NextRandom() % 30) & 0xFF) + 1) << 0x10) | (seqId + 0x7800);
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, temp_a0);
                sObjSoundMainBgmSeqId = seqId;
            }

            if (seqId == NA_BGM_MILK_BAR_DUPLICATE) {
                Audio_SetObjSoundProperties(SEQ_PLAYER_BGM_MAIN, pos, 0x1E3, 0.0f, 600.0f, 0.9f, 0.55f);
            } else if (seqId == NA_BGM_MILK_BAR) {
                Audio_SetObjSoundProperties(SEQ_PLAYER_BGM_MAIN, pos, 0x1FF, 0.0f, 600.0f, 0.9f, 0.55f);
            } else {
                Audio_SetObjSoundProperties(SEQ_PLAYER_BGM_MAIN, pos, 0x3F, 0.0f, 600.0f, 0.9f, 0.55f);
            }
        }
    } else {
        if (sObjSoundMainBgmSeqId == NA_BGM_ASTRAL_OBSERVATORY) {
            AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(SEQ_PLAYER_BGM_MAIN, 0xFFFF);
            AUDIOCMD_CHANNEL_SET_VOL_SCALE(SEQ_PLAYER_BGM_MAIN, AUDIOCMD_ALL_CHANNELS, 1.0f);
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10, NA_BGM_CAVERN);
        } else {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 5);
        }

        sObjSoundMainBgmSeqId = NA_BGM_GENERAL_SFX;
    }
}

// 2S2H [Custom Audio] Arg 2 was s8 seqId. Allow more that 255 sequences. Various casts removed in the function
void Audio_PlayObjSoundFanfare(Vec3f* pos, s16 seqId) {
    s32 requestFanfare = false;
    s32 pad;

    if (sObjSoundFanfareSeqId == NA_BGM_GENERAL_SFX) {
        // No spatial fanfare is currently playing
        requestFanfare = true;
    } else if (SEQ_SCREEN_WEIGHTED_DIST(pos) < SEQ_SCREEN_WEIGHTED_DIST(&sObjSoundFanfarePos)) {
        // The spatial fanfare requested is closer than the spatial fanfare currently playing
        requestFanfare = true;
    }

    if (requestFanfare) {
        sObjSoundFanfarePos.x = pos->x;
        sObjSoundFanfarePos.y = pos->y;
        sObjSoundFanfarePos.z = pos->z;
        sObjSoundFanfareSeqId = seqId;
        sObjSoundFanfareRequested = true;
    }
}

void Audio_UpdateObjSoundFanfare(void) {
    if (sObjSoundFanfareRequested && (sAudioPauseState == AUDIO_PAUSE_STATE_CLOSED)) {
        if (sObjSoundFanfareSeqId != NA_BGM_GENERAL_SFX) {
            Audio_StartObjSoundFanfare(SEQ_PLAYER_FANFARE, &sObjSoundFanfarePos, sObjSoundFanfareSeqId, 0);

            if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE) == NA_BGM_DISABLED) {
                Audio_MuteBgmPlayersForFanfare();
            }

            if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) &&
                (AudioSeq_GetActiveSeqId(SEQ_PLAYER_AMBIENCE) == NA_BGM_DISABLED)) {
                Audio_PlayAmbience(AMBIENCE_ID_09);
            }

            sAudioCutsceneFlag = true;
        } else {
            Audio_StartObjSoundFanfare(SEQ_PLAYER_FANFARE, NULL, sObjSoundFanfareSeqId, 0);
            if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 0);
            }
            sObjSoundFanfareRequested = false;
            sObjSoundMainBgmSeqId = NA_BGM_GENERAL_SFX;
            sAudioCutsceneFlag = false;
        }
        sObjSoundFanfareSeqId = NA_BGM_GENERAL_SFX;
    }
}

// ======== END Z_OBJ_SOUND FUNCTIONS ========

void Audio_StopSequenceAtPos(u8 seqPlayerIndex, u8 volumeFadeTimer) {
    SEQCMD_STOP_SEQUENCE((u32)seqPlayerIndex, 20);
    if ((seqPlayerIndex == SEQ_PLAYER_BGM_SUB) && (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED)) {
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, volumeFadeTimer);
        Audio_SplitBgmChannels(0);
    }
}

void Audio_StartSubBgmAtPos(u8 seqPlayerIndex, Vec3f* projectedPos, u8 seqId, u8 flags, f32 minDist, f32 maxDist,
                            f32 arg6) {
    f32 dist = SEQ_SCREEN_WEIGHTED_DIST(projectedPos);
    u8 targetVolume;
    u16 seqId0 = AudioSeq_GetActiveSeqId(seqPlayerIndex);
    f32 relVolume;
    s32 pad;

    if (dist > maxDist) {
        if ((u8)seqId0 == seqId) {
            Audio_StopSequenceAtPos(seqPlayerIndex, 10);
            sSpatialSeqIsActive[seqPlayerIndex] = false;
        }
        return;
    }

    if ((!gAudioCtx.seqPlayers[seqPlayerIndex].enabled && !sAllPlayersMutedExceptSystemAndOcarina) ||
        (seqId0 == (NA_BGM_ENEMY | 0x800))) {
        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            AudioSeq_SetVolumeScale(seqPlayerIndex, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 1);
        }

        SEQCMD_PLAY_SEQUENCE((u32)seqPlayerIndex, 1, seqId);

        sSpatialSeqIsActive[seqPlayerIndex] = true;
    }

    Audio_SetSequenceProperties(seqPlayerIndex, projectedPos, flags, minDist, maxDist, 1.0, 0.05f);

    if ((seqPlayerIndex == SEQ_PLAYER_BGM_SUB) && (gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].enabled == true)) {

        if (dist > maxDist) {
            relVolume = 1.0f;
        } else if (dist < minDist) {
            relVolume = 0.0f;
        } else {
            relVolume = 1.0f - ((maxDist - dist) / (maxDist - minDist));
        }

        targetVolume = (u8)(relVolume * 127.0f);

        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, targetVolume, 10);
        Audio_SplitBgmChannels(0x7F - targetVolume);
    }
}

/**
 * sSpatialSeqNoFilterPos takes priority over sSpatialSeqFilterPos
 * Used only by guru guru for song of storms
 */
void Audio_PlaySubBgmAtPos(Vec3f* pos, u16 seqId, f32 maxDist) {
    if (gAudioSpecId != 0xC) {
        sSpatialSeqNoFilterPos.x = pos->x;
        sSpatialSeqNoFilterPos.y = pos->y;
        sSpatialSeqNoFilterPos.z = pos->z;
        sSpatialSeqSeqId = seqId;
        sSpatialSeqMaxDist = maxDist;
        sSpatialSeqFlags |= 2; // Only update volume
        sSpatialSubBgmFadeTimer = 4;
    }
}

// 2S2H [Custom Audio] Arg 2 was s8 seqId. Allow more that 255 sequences. Various casts removed in the function
// Used only by guru guru for song of storms in stock pot from hallway or neighboring room
void Audio_PlaySubBgmAtPosWithFilter(Vec3f* pos, u16 seqId, f32 maxDist) {
    sSpatialSeqFilterPos.x = pos->x;
    sSpatialSeqFilterPos.y = pos->y;
    sSpatialSeqFilterPos.z = pos->z;
    sSpatialSeqSeqId = seqId;
    //! @bug Did not set sSpatialSeqMaxDist = maxDist; This will use the previously set value of sSpatialSeqMaxDist
    sSpatialSeqFlags |= 1; // Update with volume and filter
    sSpatialSubBgmFadeTimer = 4;
}

// Another bgm by pos, less customization
void Audio_UpdateSubBgmAtPos(void) {
    if (sSpatialSubBgmFadeTimer != 0) {
        if (sSpatialSeqFlags & 2) {
            // Affects only volume
            Audio_StartSubBgmAtPos(SEQ_PLAYER_BGM_SUB, &sSpatialSeqNoFilterPos, sSpatialSeqSeqId, 0x20, 100.0f,
                                   sSpatialSeqMaxDist, 1.0f);
        } else {
            // Set volume with band-pass filter
            Audio_StartSubBgmAtPos(SEQ_PLAYER_BGM_SUB, &sSpatialSeqFilterPos, sSpatialSeqSeqId, 0x28, 100.0f,
                                   sSpatialSeqMaxDist, 1.0f);
        }

        sSpatialSubBgmFadeTimer--;
        if (sSpatialSubBgmFadeTimer == 0) {
            Audio_StopSequenceAtPos(SEQ_PLAYER_BGM_SUB, 10);
        }

        sSpatialSeqFlags = 0;
    }
}

/**
 * Play sequence at the default center of the screen. Does not check for final hours or soaring bgms playing
 * Used only by minifrog for the frog song
 */
void Audio_PlaySequenceAtDefaultPos(u8 seqPlayerIndex, u16 seqId) {
    if (!sAudioCutsceneFlag && (gAudioSpecId != 0xC)) {
        sSpatialSeqFilterPos.x = gSfxDefaultPos.x;
        sSpatialSeqFilterPos.y = gSfxDefaultPos.y;
        sSpatialSeqFilterPos.z = gSfxDefaultPos.z;
        sSpatialSeqMaxDist = 10000.0f;
        sSpatialSeqFadeTimer = 128;
        sSpatialSeqSeqId = seqId;
        sSpatialSeqPlayerIndex = seqPlayerIndex;
    }
}

// Used only by minifrog
void Audio_StopSequenceAtDefaultPos(void) {
    if (gAudioSpecId != 0xC) {
        sSpatialSeqFadeTimer = 1;
        sSpatialSeqSeqId = NA_BGM_GENERAL_SFX;
    }
}

/**
 * Play the requested sequence at a position. Valid for sequences on players 0 - 3
 */
void Audio_PlaySequenceAtPos(u8 seqPlayerIndex, Vec3f* pos, u16 seqId, f32 maxDist) {
    if ((!sAudioCutsceneFlag) && ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) & 0xFF) != NA_BGM_SONG_OF_SOARING) &&
        (gAudioSpecId != 0xC) && (pos != NULL) &&
        ((sSpatialSeqPlayerIndex != SEQ_PLAYER_BGM_MAIN) ||
         (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_FINAL_HOURS))) {
        sSpatialSeqFilterPos.x = pos->x;
        sSpatialSeqFilterPos.y = pos->y;
        sSpatialSeqFilterPos.z = pos->z;
        sSpatialSeqMaxDist = maxDist;
        sSpatialSeqFadeTimer = 2;
        sSpatialSeqSeqId = seqId & 0xFF;
        sSpatialSeqPlayerIndex = seqPlayerIndex;
    }
}

void Audio_UpdateSequenceAtPos(void) {
    u16 mainBgmSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    u8 volumeFadeTimer;

    if ((sSpatialSeqFadeTimer != 0) && (sAudioPauseState == AUDIO_PAUSE_STATE_CLOSED)) {
        if ((sSpatialSeqSeqId == NA_BGM_GENERAL_SFX) || (mainBgmSeqId == NA_BGM_SONG_OF_SOARING)) {
            volumeFadeTimer = 10;

            if (mainBgmSeqId == NA_BGM_SONG_OF_SOARING) {
                sSpatialSeqFadeTimer = 0;
                volumeFadeTimer = 1;
            } else if (sSpatialSeqFadeTimer < 128) {
                sSpatialSeqFadeTimer--;
            }

            if (sSpatialSeqFadeTimer == 0) {
                Audio_StopSequenceAtPos(sSpatialSeqPlayerIndex, volumeFadeTimer);
                sSpatialSeqIsActive[sSpatialSeqPlayerIndex] = false;
            }
        } else {
            if ((sSpatialSeqPlayerIndex == SEQ_PLAYER_BGM_MAIN) && (mainBgmSeqId == NA_BGM_FINAL_HOURS)) {
                Audio_StopSequenceAtPos(sSpatialSeqPlayerIndex, 10);
                sSpatialSeqIsActive[sSpatialSeqPlayerIndex] = false;
                return;
            }

            Audio_StartSubBgmAtPos(sSpatialSeqPlayerIndex, &sSpatialSeqFilterPos, sSpatialSeqSeqId, 0x20, 200.0f,
                                   sSpatialSeqMaxDist, 1.0f);
            if (!sSpatialSeqIsActive[sSpatialSeqPlayerIndex]) {
                sSpatialSeqFadeTimer = 0;
            }
        }

        if (sSpatialSeqFadeTimer < 128) {
            sSpatialSeqSeqId = NA_BGM_GENERAL_SFX;
        }
    }
}

/**
 * Unused remnant of OoT's EnRiverSound
 */
void Audio_PlaySariaBgm(Vec3f* pos, u16 seqId, u16 distMax) {
    f32 absY;
    f32 dist;
    u8 targetVolume;
    f32 prevDist;

    if (sRiverSoundBgmTimer != 0) {
        sRiverSoundBgmTimer--;
        return;
    }

    dist = sqrtf(SQ(pos->z) + (SQ(pos->x) + SQ(pos->y)));

    if (sRiverSoundBgmPos == NULL) {
        sRiverSoundBgmPos = pos;
        Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_SUB, seqId, 0, 7, 2);
    } else {
        prevDist = sqrtf(SQ(sRiverSoundBgmPos->z) + SQ(sRiverSoundBgmPos->x));
        if (dist < prevDist) {
            sRiverSoundBgmPos = pos;
        } else {
            dist = prevDist;
        }
    }

    absY = ABS_ALT(pos->y);

    if ((distMax / 15.0f) < absY) {
        targetVolume = 0;
    } else if (dist < distMax) {
        targetVolume = (1.0f - (dist / distMax)) * 127.0f;
    } else {
        targetVolume = 0;
    }

    if (seqId != NA_BGM_FAIRY_FOUNTAIN) {
        Audio_SplitBgmChannels(targetVolume);
    }

    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, targetVolume, 0);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F - targetVolume, 0);
}

/**
 * Unused remnant of OoT's EnRiverSound
 */
void Audio_ClearSariaBgm2(void) {
    sRiverSoundBgmPos = NULL;
}

void func_801A246C(u8 seqPlayerIndex, u8 type) {
    u16 seqId;
    u8 targetVolume;
    s32 channelMask = 0;

    targetVolume = 0;

    switch (type) {
        case 0:
            targetVolume = 0x7F;
            break;

        case 1:
            channelMask = 0xFFFF;
            break;

        case 2:
            targetVolume = 0xFF;
            break;

        default:
            break;
    }

    if (targetVolume != 0xFF) {
        SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMask);
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, targetVolume, 1);
    } else {
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 0);
        seqId = AudioSeq_GetActiveSeqId(seqPlayerIndex);

        if (seqId != NA_BGM_DISABLED) {
            SEQCMD_PLAY_SEQUENCE(seqPlayerIndex, 0, seqId);
        }
    }
}

void Audio_PlayMorningSceneSequence(u16 seqId, u8 dayMinusOne) {
    Audio_StartMorningSceneSequence(seqId);
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 4, dayMinusOne);
}

void Audio_StartMorningSceneSequence(u16 seqId) {
    if (seqId != NA_BGM_AMBIENCE) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 0);
        Audio_StartSceneSequence(seqId);
        Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_MAIN, seqId, 0, 0, 1);
    } else {
        Audio_PlayAmbience(AMBIENCE_ID_08);
    }
}

void Audio_PlaySceneSequence(u16 seqId, u8 dayMinusOne) {
    if (GameInteractor_Should(VB_PLAY_SCENE_SEQUENCE, false, &sRequestedSceneSeqId, &sPrevMainBgmSeqId, &seqId)) {
        return;
    }

    if (sRequestedSceneSeqId != seqId) {
        if (seqId == NA_BGM_AMBIENCE) {
            Audio_PlayAmbience(AMBIENCE_ID_08);
        } else if ((seqId != NA_BGM_FINAL_HOURS) || (sPrevMainBgmSeqId == NA_BGM_DISABLED)) {
            Audio_StartSceneSequence(seqId);
            SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 4, dayMinusOne);
        }
        sRequestedSceneSeqId = seqId;
    }
}

void Audio_StartSceneSequence(u16 seqId) {
    u8 fadeInDuration = 0;
    u8 skipHarpIntro;
    seqId = AudioEditor_GetOriginalSeq(seqId);

    if ((sSeqFlags[sPrevSceneSeqId] & SEQ_FLAG_RESUME_PREV) && (sSeqFlags[seqId & 0xFF & 0xFF] & SEQ_FLAG_RESUME)) {
        // Resume the sequence from the point where it left off last time it was played in the scene
        if ((sSeqResumePoint & 0x3F) != 0) {
            fadeInDuration = 30;
        }

        // Write the sequence resumePoint to resume from into ioPort 7
        Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_MAIN, seqId, fadeInDuration, 7, sSeqResumePoint);

        sSeqResumePoint = 0;
    } else {
        // Start the sequence from the beginning

        // Writes to ioPort 7. See `SEQ_FLAG_SKIP_HARP_INTRO` for writing a value of 1 to ioPort 7.
        if (sSeqFlags[seqId & 0xFF & 0xFF] & SEQ_FLAG_SKIP_HARP_INTRO) {
            skipHarpIntro = 1;
        } else {
            skipHarpIntro = (u8)SEQ_IO_VAL_NONE;
        }
        Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_MAIN, seqId, 0, 7, skipHarpIntro);

        if (!(sSeqFlags[seqId] & SEQ_FLAG_RESUME_PREV)) {
            // Reset the sequence resumePoint
            sSeqResumePoint = SEQ_RESUME_POINT_NONE;
        }
    }
    sPrevSceneSeqId = seqId & 0xFF;
}

void Audio_UpdateSceneSequenceResumePoint(void) {
    u16 seqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    seqId = AudioEditor_GetOriginalSeq(seqId);

    if ((seqId != NA_BGM_DISABLED) && (sSeqFlags[seqId] & SEQ_FLAG_RESUME)) {
        if (sSeqResumePoint != SEQ_RESUME_POINT_NONE) {
            // Get the current point to resume from
            sSeqResumePoint = gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_MAIN].seqScriptIO[3];
        } else {
            // Initialize the point to resume from to the start of the sequence
            sSeqResumePoint = 0;
        }
    }
}

// Unused
void Audio_PlayBgmForSongOfStorms(void) {
    if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_SONG_OF_STORMS) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_SONG_OF_STORMS | SEQ_FLAG_ASYNC);
    }
}

/**
 * Specifically for SEQ_PLAYER_BGM_MAIN
 */
void Audio_ScaleTempoAndFreqForMainBgm(f32 freqTempoScale, u8 duration) {
    if (freqTempoScale == 1.0f) {
        SEQCMD_RESET_TEMPO(SEQ_PLAYER_BGM_MAIN, duration);
    } else {
        SEQCMD_SCALE_TEMPO(SEQ_PLAYER_BGM_MAIN, duration, freqTempoScale * 100.0f);
    }

    SEQCMD_SET_SEQPLAYER_FREQ(SEQ_PLAYER_BGM_MAIN, duration, freqTempoScale * 1000.0f);
}

void Audio_SetSeqTempoAndFreq(u8 seqPlayerIndex, f32 freqTempoScale, u8 duration) {
    if (freqTempoScale == 1.0f) {
        SEQCMD_RESET_TEMPO(seqPlayerIndex, duration);
    } else {
        SEQCMD_SCALE_TEMPO(seqPlayerIndex, duration, freqTempoScale * 100.0f);
    }

    SEQCMD_SET_SEQPLAYER_FREQ(seqPlayerIndex, duration, freqTempoScale * 1000.0f);
}

void Audio_PlaySubBgm(u16 seqId) {
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 0);
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 0, seqId);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0, 5);

    SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(SEQ_PLAYER_BGM_SUB, SEQ_PLAYER_BGM_MAIN, 3, 10);
    SEQCMD_SETUP_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_SUB, SEQ_PLAYER_BGM_MAIN, 0);
}

void Audio_StopSubBgm(void) {
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, 0);
}

// Unused remnant of OoT
void Audio_IncreaseTempoForTimedMinigame(void) {
    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_TIMED_MINI_GAME) &&
        AudioSeq_IsSeqCmdNotQueued(SEQCMD_OP_PLAY_SEQUENCE << 28, SEQCMD_OP_MASK)) {
        // Set tempo to be higher than the default tempo for th timed mini-game sequence
        SEQCMD_SET_TEMPO(SEQ_PLAYER_BGM_MAIN, 5, 210);
    }
}
// 2S2H [Custom audio] The &0xFF wasn't hurting anything because by default the original sequence IDs will never exceed
// 127
void Audio_PlaySequenceInCutscene(u16 seqId) {
    seqId = AudioEditor_GetOriginalSeq(seqId);
    if (sSeqFlags[seqId] & SEQ_FLAG_FANFARE) {
        Audio_PlayFanfare(seqId);
    } else if (sSeqFlags[seqId] & SEQ_FLAG_FANFARE_KAMARO) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 0, seqId);
    } else if (sSeqFlags[seqId] & SEQ_FLAG_NO_AMBIENCE) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 0, seqId);
    } else {
        Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_MAIN, seqId, 0, 7, SEQ_IO_VAL_NONE);
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 0);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    }
}

void Audio_StopSequenceInCutscene(u16 seqId) {
    seqId = AudioEditor_GetOriginalSeq(seqId);
    if (sSeqFlags[seqId] & SEQ_FLAG_FANFARE) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    } else if (sSeqFlags[seqId] & SEQ_FLAG_FANFARE_KAMARO) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    } else if (sSeqFlags[seqId] & SEQ_FLAG_NO_AMBIENCE) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, 0);
    } else {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
    }
}

s32 Audio_IsSequencePlaying(u16 seqId) {
    u16 origSeqId = AudioEditor_GetOriginalSeq(seqId);

    u8 seqPlayerIndex = SEQ_PLAYER_BGM_MAIN;

    if (sSeqFlags[origSeqId] & SEQ_FLAG_FANFARE) {
        seqPlayerIndex = SEQ_PLAYER_FANFARE;
    } else if (sSeqFlags[origSeqId] & SEQ_FLAG_FANFARE_KAMARO) {
        seqPlayerIndex = SEQ_PLAYER_FANFARE;
    }

    if (seqId == (AudioSeq_GetActiveSeqId(seqPlayerIndex) & 0xFF)) {
        return true;
    } else {
        return false;
    }
}

void Audio_PlayBgm_StorePrevBgm(u16 seqId) {
    u16 curSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    u64 origSeqId = AudioEditor_GetOriginalSeq(curSeqId);
    if (curSeqId == NA_BGM_DISABLED) {
        curSeqId = NA_BGM_GENERAL_SFX;
    }

    if (curSeqId != seqId) {
        Audio_SetSequenceMode(SEQ_MODE_IGNORE);

        // Ensure the sequence about to be stored isn't also storing a separate sequence
        if (!(sSeqFlags[origSeqId] & SEQ_FLAG_RESTORE)) {
            sPrevMainBgmSeqId = curSeqId;
        }

        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, seqId + SEQ_FLAG_ASYNC);
    }
}

/**
 * To be used in conjunction with Audio_PlayBgm_StorePrevBgm
 */
void Audio_RestorePrevBgm(void) {
    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) &&
        (sSeqFlags[AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) & 0xFF] & SEQ_FLAG_RESTORE)) {
        if ((sPrevMainBgmSeqId == NA_BGM_DISABLED) || (sPrevMainBgmSeqId == NA_BGM_GENERAL_SFX)) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
        } else {
            if (sPrevMainBgmSeqId == NA_BGM_AMBIENCE) {
                sPrevMainBgmSeqId = sPrevAmbienceSeqId;
            }
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, sPrevMainBgmSeqId + SEQ_FLAG_ASYNC);
        }
        sPrevMainBgmSeqId = NA_BGM_DISABLED;
    }
}

// Unused
void Audio_PlayAmbience_StorePrevBgm(u8 ambienceId) {
    u16 seqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);

    if (seqId != NA_BGM_AMBIENCE) {
        sPrevMainBgmSeqId = seqId;
    }

    Audio_PlayAmbience(ambienceId);
}

// Unused
void Audio_ForceRestorePreviousBgm(void) {
    if (sPrevMainBgmSeqId != NA_BGM_DISABLED) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, sPrevMainBgmSeqId + SEQ_FLAG_ASYNC);
    }

    sPrevMainBgmSeqId = NA_BGM_DISABLED;
}

void Audio_PlayFanfareWithPlayerIOPort7(u16 seqId, u8 ioData) {
    Audio_PlayFanfare(seqId);
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_FANFARE, 7, ioData);
}

void Audio_MuteBgmPlayersForFanfare(void) {
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0, 5);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0, 5);
    SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN, 1, 10);
    SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_SUB, 1, 10);
    SEQCMD_SETUP_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN, 0);
}

/**
 * Sets up seqId to play on seqPlayerIndex 1
 */
void Audio_PlayFanfare(u16 seqId) {
    u16 prevFontBuff[16];
    u16 fontBuff[16];
    u16 prevSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE);
    u32 outNumFonts;
    // 2S2H [Custom Audio] `prevSeqId` and `seqId` had a `& 0xFF`. This was likely OK because we always load soundFonts
    // but it was removed anyway just in case.
    u16* prevFontId = AudioThread_GetFontsForSequence(prevSeqId, &outNumFonts, prevFontBuff);
    u16* fontId = AudioThread_GetFontsForSequence(seqId, &outNumFonts, fontBuff);
    // BENTODO
    // #region 2S2H [Audio] TODO: Fixes fanfare crash, should/can be removed after audio is done
    // if ((prevSeqId == NA_BGM_DISABLED) || (*prevFontId == *fontId)) {
    if ((prevSeqId == NA_BGM_DISABLED) || (prevFontId != NULL && fontId != NULL && *prevFontId == *fontId)) {
        // #endregion
        sFanfareState = 1;
    } else {
        sFanfareState = 5;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    }

    sFanfareSeqId = seqId;
}

void Audio_UpdateFanfare(void) {
    if (sFanfareState != 0) {
        if ((sFanfareState != 5) &&
            !AudioSeq_IsSeqCmdNotQueued((SEQCMD_OP_STOP_SEQUENCE << 28) | (SEQ_PLAYER_FANFARE << 24),
                                        SEQCMD_OP_MASK | SEQCMD_ASYNC_ACTIVE | SEQCMD_SEQPLAYER_MASK)) {
            sFanfareState = 0;
        } else {
            sFanfareState--;
            if (sFanfareState == 0) {
                AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SEQUENCE_TABLE);
                AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(FONT_TABLE);
                AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
                if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE) == NA_BGM_DISABLED) {
                    Audio_MuteBgmPlayersForFanfare();
                }
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 0, sFanfareSeqId);
                SEQCMD_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_MAIN, 0xFFFF);
            }
        }
    }
}

void Audio_PlayFanfareWithPlayerIOCustomPort(u16 seqId, s8 ioPort, u8 ioData) {
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_FANFARE, ioPort, ioData);
    Audio_PlayFanfare(seqId);
}

void Audio_PlaySequenceWithSeqPlayerIO(s8 seqPlayerIndex, u16 seqId, u8 fadeInDuration, s8 ioPort, u8 ioData) {
    u16 flaggedSeqId;

    SEQCMD_SET_SEQPLAYER_IO(seqPlayerIndex, ioPort, ioData);
    if ((seqId & 0xFF) < 2) {
        flaggedSeqId = seqId;
    } else {
        flaggedSeqId = seqId | 0x8000;
    }
    SEQCMD_PLAY_SEQUENCE((u32)seqPlayerIndex, fadeInDuration, flaggedSeqId);
}

void Audio_SetSequenceMode(u8 seqMode) {
    s32 volumeFadeInTimer;
    u16 seqId;
    u8 volumeFadeOutTimer;

    if ((sPrevMainBgmSeqId == NA_BGM_DISABLED) && (sPrevMainBgmSeqId == NA_BGM_DISABLED)) {
        // clang-format off
        if (sAudioCutsceneFlag || sSpatialSeqIsActive[SEQ_PLAYER_BGM_SUB]) { \
            seqMode = SEQ_MODE_IGNORE;
        }
        // clang-format on

        seqId = gActiveSeqs[SEQ_PLAYER_BGM_MAIN].seqId;
        seqId = AudioEditor_GetOriginalSeq(seqId);
        if ((seqId == NA_BGM_DISABLED) || (sSeqFlags[seqId] & SEQ_FLAG_ENEMY) ||
            ((sPrevSeqMode & 0x7F) == SEQ_MODE_ENEMY)) {
            if (seqMode != (sPrevSeqMode & 0x7F)) {
                if (seqMode == SEQ_MODE_ENEMY) {
                    // If only seqMode = SEQ_MODE_ENEMY (Start)
                    volumeFadeInTimer = ABS_ALT(gActiveSeqs[SEQ_PLAYER_BGM_SUB].volScales[1] - sBgmEnemyVolume);

                    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, sBgmEnemyVolume,
                                            volumeFadeInTimer);
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10, NA_BGM_ENEMY | 0x800);

                    if (seqId >= NA_BGM_TERMINA_FIELD) {
                        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F - sBgmEnemyVolume,
                                                10);
                        Audio_SplitBgmChannels(sBgmEnemyVolume);
                    }
                } else if ((sPrevSeqMode & 0x7F) == SEQ_MODE_ENEMY) {
                    // If only sPrevSeqMode = SEQ_MODE_ENEMY (End)
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10);

                    if (seqMode == SEQ_MODE_IGNORE) {
                        volumeFadeOutTimer = 0;
                    } else {
                        volumeFadeOutTimer = 10;
                    }

                    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, volumeFadeOutTimer);
                    Audio_SplitBgmChannels(0);
                }

                sPrevSeqMode = seqMode + 0x80;
            } else {
                if (seqMode == SEQ_MODE_ENEMY) {
                    // If both seqMode = sPrevSeqMode = SEQ_MODE_ENEMY
                    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) == NA_BGM_DISABLED) &&
                        (seqId != NA_BGM_DISABLED) && (sSeqFlags[seqId] & SEQ_FLAG_ENEMY)) {
                        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10, NA_BGM_ENEMY | 0x800);
                        sPrevSeqMode = seqMode + 0x80;
                    }
                }
            }
        } else {
            // Remnant of OoT's Hyrule Field Sequence
            if (seqMode == SEQ_MODE_DEFAULT) {
                if (sPrevSeqMode == SEQ_MODE_STILL) {
                    sNumFramesMoving = 0;
                }
                sNumFramesStill = 0;
                sNumFramesMoving++;
            } else {
                sNumFramesStill++;
            }

            if ((seqMode == SEQ_MODE_STILL) && (sNumFramesStill < 30) && (sNumFramesMoving > 20)) {
                seqMode = SEQ_MODE_DEFAULT;
            }

            sPrevSeqMode = seqMode;
            SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 2, seqMode);
        }
    }
}

void Audio_UpdateEnemyBgmVolume(f32 dist) {
    f32 adjDist;
    u16 seqId = AudioEditor_GetOriginalSeq(gActiveSeqs[SEQ_PLAYER_BGM_MAIN].seqId);

    if (sPrevSeqMode == (SEQ_MODE_ENEMY | 0x80)) {
        if (dist != sBgmEnemyDist) {
            // clamp (dist - 150.0f) between 0 and 350
            if (dist < 150.0f) {
                adjDist = 0.0f;
            } else if (dist > 500.0f) {
                adjDist = 350.0f;
            } else {
                adjDist = dist - 150.0f;
            }

            sBgmEnemyVolume = ((350.0f - adjDist) * 127.0f) / 350.0f;
            AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, sBgmEnemyVolume, 10);

            if ((seqId >= NA_BGM_TERMINA_FIELD) && !(sSeqFlags[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE_KAMARO)) {
                AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, (0x7F - sBgmEnemyVolume), 10);
            }
        }

        Audio_SplitBgmChannels(sBgmEnemyVolume);
    }
    sBgmEnemyDist = dist;
}

void Audio_UpdateRomaniSinging(f32 dist, u16 seqId) {
    s8 pad;
    s8 melodyVolume;
    s16 curSeqId;

    if (sRomaniSingingDisabled) {
        return;
    }

    curSeqId = (s8)(AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) & 0xFF);

    if (curSeqId == (seqId & 0xFF)) {
        if ((seqId & 0xFF) == NA_BGM_ROMANI_RANCH) {
            // Romani is singing along with the Romani Ranch Sequence

            if (dist > 2000.0f) {
                melodyVolume = 127;
            } else if (dist < 200.0f) {
                melodyVolume = 0;
            } else {
                melodyVolume = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
            }

            // Update volume for channels 0 & 1, which contains Romani's singing
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 0, 3, 127 - melodyVolume);
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 1, 3, 127 - melodyVolume);

            // Update volume for channel 13, which contains the melody line for Romani's Ranch Sequence
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 13, 3, melodyVolume);
            if (sRomaniSingingTimer == 0) {
                sRomaniSingingTimer++;
            }
        }
    } else if ((curSeqId == NA_BGM_AMBIENCE) && ((seqId & 0xFF) == NA_BGM_ROMANI_RANCH)) {
        // Romani is singing along with ambience
        curSeqId = (s8)(AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) & 0xFF);

        if ((curSeqId != (seqId & 0xFF)) && (sRomaniSingingTimer < 10)) {
            // Disable all channels between 2-15.
            // Only allow the two channels with Romani's singing to play, and surpress the full Romani Ranch sequence.
            Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_SUB, NA_BGM_ROMANI_RANCH, 0, 0, 0);
            SEQCMD_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_SUB, 0xFFFC);
            sRomaniSingingTimer = 10;
        }

        if (dist > 2000.0f) {
            melodyVolume = 127;
        } else if (dist < 200.0f) {
            melodyVolume = 0;
        } else {
            melodyVolume = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
        }

        // Update volume for channels 0 & 1, which contains Romani's singing
        SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_SUB, 0, 3, 127 - melodyVolume);
        SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_SUB, 1, 3, 127 - melodyVolume);
    }

    if (sRomaniSingingTimer < 10) {
        sRomaniSingingTimer++;
    }
}

u8 func_801A3950(u8 seqPlayerIndex, u8 resetChannelIO) {
    u8 ret = SEQ_IO_VAL_NONE;
    SequenceChannel* channel = gAudioCtx.seqPlayers[seqPlayerIndex].channels[15];

    if (gAudioCtx.seqPlayers[seqPlayerIndex].enabled && channel->enabled) {
        if (channel->seqScriptIO[0] != SEQ_IO_VAL_NONE) {
            ret = channel->seqScriptIO[0];
            if (resetChannelIO == true) {
                SEQCMD_SET_CHANNEL_IO(seqPlayerIndex, 15, 0, (u8)SEQ_IO_VAL_NONE);
            }
        }
    }
    return ret;
}

// Used only by minifrog
u8 func_801A39F8(void) {
    u8 frogIndex = 0xFF;
    s32 pad;
    SequenceChannel* channel = gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_SUB].channels[15];

    if (gAudioCtx.seqPlayers[SEQ_PLAYER_BGM_SUB].enabled) {
        if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) == NA_BGM_FROG_SONG) && channel->enabled) {
            if (channel->seqScriptIO[0] != -1) {
                frogIndex = channel->seqScriptIO[0];
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_BGM_SUB, 15, 0, SEQ_IO_VAL_NONE);
            }
        }
    }
    return frogIndex;
}

void Audio_SetPauseState(u8 isPauseMenuOpen) {
    if ((isPauseMenuOpen == false) && (sAudioPauseState != AUDIO_PAUSE_STATE_CLOSED)) {
        sAudioPauseState = AUDIO_PAUSE_STATE_CLOSING;
    } else {
        sAudioPauseState = isPauseMenuOpen << 1; // Set to AUDIO_PAUSE_STATE_OPEN
    }
}

void Audio_UpdatePauseState(void) {
    if ((sAudioPauseState != AUDIO_PAUSE_STATE_CLOSED) && (sAudioPauseState != AUDIO_PAUSE_STATE_OPEN)) {
        sAudioPauseState--; // Set to AUDIO_PAUSE_STATE_CLOSED
    }
}

void Audio_PlaySfx_PauseMenuOpenOrClose(u8 pauseMenuOpenOrClose) {
    sAudioPauseMenuOpenOrClose = pauseMenuOpenOrClose;
    if (pauseMenuOpenOrClose != SFX_PAUSE_MENU_CLOSE) {
        Audio_PlaySfx(NA_SE_SY_WIN_OPEN);
        AUDIOCMD_GLOBAL_MUTE(AUDIOCMD_ALL_SEQPLAYERS);
    } else {
        Audio_PlaySfx(NA_SE_SY_WIN_CLOSE);
        AUDIOCMD_GLOBAL_UNMUTE(AUDIOCMD_ALL_SEQPLAYERS, false);
    }
}

// Only used by guru guru
void Audio_MuteSeqPlayerBgmSub(u8 isMuted) {
    if (isMuted == true) {
        AUDIOCMD_GLOBAL_MUTE(SEQ_PLAYER_BGM_SUB);
    } else {
        AUDIOCMD_GLOBAL_UNMUTE(SEQ_PLAYER_BGM_SUB, false);
    }
}

/**
 * Enable or disable Romani's singing
 *
 * @param romaniSingingDisabled true to disable, false to enable
 */
void Audio_ToggleRomaniSinging(u8 romaniSingingDisabled) {
    u8 seqPlayerIndex;
    u16 channelMaskDisable;

    sRomaniSingingDisabled = romaniSingingDisabled;

    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) & 0xFF) == NA_BGM_ROMANI_RANCH) {
        // Romani is singing along with the Romani Ranch Sequence
        seqPlayerIndex = SEQ_PLAYER_BGM_MAIN;
        // Do not disable any channel.
        // Allow the full Romani Ranch sequence to play in addition to Romani's singing.
        channelMaskDisable = 0;
    } else if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) & 0xFF) == NA_BGM_ROMANI_RANCH) {
        seqPlayerIndex = SEQ_PLAYER_BGM_SUB;
        // Disable all channels between 2-15.
        // Only allow the two channels with Romani's singing to play, and surpress the full Romani sequence.
        channelMaskDisable = 0xFFFC;
    } else {
        return;
    }

    if (romaniSingingDisabled) {
        // Turn volume off for channels 0 & 1, which contains Romani's singing
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 0, 1, 0);
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 1, 1, 0);
        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, disable all 16 channels
            SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMaskDisable | 3);
        }
    } else {
        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, start the sequence
            Audio_PlaySequenceWithSeqPlayerIO(SEQ_PLAYER_BGM_SUB, NA_BGM_ROMANI_RANCH, 0, 0, 0);
        }

        // Turn volume on for only channels 0 & 1, which contains Romani's singing
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 0, 1, 0x7F);
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 1, 1, 0x7F);

        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, disable channels 2-15
            SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMaskDisable);
        }
    }
}

void Audio_SetEnvReverb(s8 reverb) {
    sAudioEnvReverb = reverb & 0x7F;
}

void Audio_SetCodeReverb(s8 reverb) {
    u8 reverbIndex;

    if (reverb != 0) {
        if ((reverb & 0x40) != (sAudioCodeReverb & 0x40)) {
            reverbIndex = (reverb >> 6) + 1;
            Audio_SetSfxReverbIndexExceptOcarinaBank(reverbIndex);
        }
        sAudioCodeReverb = reverb & 0x7F;
    }
}

/**
 * Possibly a test for surround sound
 * Unused
 */
void Audio_PlaySfx_SurroundSoundTest(void) {
    s32 val = 0;

    if (sSoundMode == SOUNDMODE_SURROUND_EXTERNAL) {
        val = 2;
    }

    Audio_PlaySfx_AtPosWithAllChannelsIO(&gSfxDefaultPos, NA_SE_SY_SOUT_DEMO, val);
}

void Audio_SetFileSelectSettings(s8 audioSetting) {
    s8 soundMode;

    switch (audioSetting) {
        case SAVE_AUDIO_STEREO:
            soundMode = SOUNDMODE_STEREO;
            sSoundMode = SOUNDMODE_STEREO;
            // 2S2H [Port] Inform LUS of audio setting change
            SetAudioChannels(audioStereo);
            break;

        case SAVE_AUDIO_MONO:
            soundMode = SOUNDMODE_MONO;
            sSoundMode = SOUNDMODE_MONO;
            // 2S2H [Port] Inform LUS of audio setting change
            SetAudioChannels(audioStereo);
            break;

        case SAVE_AUDIO_HEADSET:
            soundMode = SOUNDMODE_HEADSET;
            sSoundMode = SOUNDMODE_HEADSET;
            // 2S2H [Port] Inform LUS of audio setting change
            SetAudioChannels(audioStereo);
            break;

        case SAVE_AUDIO_SURROUND:
            soundMode = SOUNDMODE_SURROUND;
            sSoundMode = SOUNDMODE_SURROUND_EXTERNAL;
            // 2S2H [Port] Inform LUS of audio setting change
            SetAudioChannels(audioMatrix51);
            break;

        default:
            break;
    }

    SEQCMD_SET_SOUND_MODE(soundMode);
}

void Audio_SetBaseFilter(u8 filter) {
    if (sAudioBaseFilter != filter) {
        if (filter == 0) {
            AudioSfx_StopById(NA_SE_PL_IN_BUBBLE);
        } else if (sAudioBaseFilter == 0) {
            AudioSfx_PlaySfx(NA_SE_PL_IN_BUBBLE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
    }
    sAudioBaseFilter = filter;
    sAudioBaseFilter2 = filter;
}

void Audio_SetExtraFilter(u8 filter) {
    u8 channelIndex;

    sAudioExtraFilter2 = filter;
    sAudioExtraFilter = filter;
    if (gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId == NA_BGM_AMBIENCE) {
        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            // seq player 4, all channels, slot 6
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_AMBIENCE, channelIndex, 6, filter);
        }
    }
}

void Audio_SetCutsceneFlag(s8 flag) {
    sAudioCutsceneFlag = flag;
}

void Audio_PlaySfx_IfNotInCutsceneImpl(u16 sfxId, Vec3f* pos, u8 token, f32* freqScale, f32* volume, s8* reverbAdd) {
    if (!sAudioCutsceneFlag) {
        AudioSfx_PlaySfx(sfxId, pos, token, freqScale, volume, reverbAdd);
    }
}

void Audio_PlaySfx_IfNotInCutscene(u16 sfxId) {
    Audio_PlaySfx_IfNotInCutsceneImpl(sfxId, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                      &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

// Unused
void Audio_MuteSfxAndAmbienceSeqExceptSystemAndOcarina(u8 muteOnlySfxAndAmbienceSeq) {
    sMuteOnlySfxAndAmbienceSeq = muteOnlySfxAndAmbienceSeq;
}

void Audio_SetSpec(u8 specId) {
    if (!sMuteOnlySfxAndAmbienceSeq) {
        SEQCMD_RESET_AUDIO_HEAP(0, 0, specId);
    } else {
        SEQCMD_RESET_AUDIO_HEAP(2, 0, specId);
    }
}

void Audio_MuteAllSeqExceptSystemAndOcarina(u16 duration) {
    s32 skip;
    u8 channelIndex;

    if (!sMuteOnlySfxAndAmbienceSeq) {
        sAllPlayersMutedExceptSystemAndOcarina = true;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, (duration * 3) / 2);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, (duration * 3) / 2);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, (duration * 3) / 2);
    }

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, (duration * 3) / 2);

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        skip = false;
        switch (channelIndex) {
            case SFX_CHANNEL_SYSTEM0:
            case SFX_CHANNEL_SYSTEM1:
                if (gAudioSpecId == 10) {} // Remnant of OoT
                if (gAudioSpecId == 11) {}
                skip = true;
                break;

            case SFX_CHANNEL_OCARINA:
                skip = true;
                break;
        }

        if (!skip) {
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_SFX, channelIndex, (u8)(duration >> 1), 0);
        }
    }
}

void Audio_MuteSfxAndAmbienceSeqExceptSysAndOca(u16 duration) {
    sMuteOnlySfxAndAmbienceSeq = true;
    Audio_MuteAllSeqExceptSystemAndOcarina(duration);
}

void Audio_StopFanfare(u16 duration) {
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, duration);
    if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE) != NA_BGM_DISABLED) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, duration);
    }
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, duration);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, duration);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 0);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
}

/**
 * The flag 0xFF makes the sequence process SkipTicks and SkipForwardSequence
 * Unused
 */
void Audio_StartSfxPlayer(void) {
    AudioSeq_StartSequence(SEQ_PLAYER_SFX, NA_BGM_GENERAL_SFX, 0xFF, 5);
}

void Audio_DisableAllSeq(void) {
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_BGM_MAIN, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_FANFARE, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_SFX, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_BGM_SUB, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_AMBIENCE, 0);
    AudioThread_ScheduleProcessCmds();
}

s8 Audio_GetEnabledNotesCount(void) {
    return AudioThread_GetEnabledNotesCount();
}

// Unused
void func_801A4348(void) {
    Audio_DisableAllSeq();
    AudioThread_ScheduleProcessCmds();

    // Wait for all notes to be finished
    while (true) {
        if (Audio_GetEnabledNotesCount() == 0) {
            break;
        }
    }
}

void Audio_SetSfxVolumeExceptSystemAndOcarinaBanks(u8 volume) {
    u8 channelIndex;

    if (!sAllPlayersMutedExceptSystemAndOcarina) {
        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            switch (channelIndex) {
                case SFX_CHANNEL_SYSTEM0:
                case SFX_CHANNEL_SYSTEM1:
                case SFX_CHANNEL_OCARINA:
                    break;

                default:
                    SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_SFX, channelIndex, 10, volume);
                    break;
            }
        }
    }
}

void Audio_SetSfxReverbIndexExceptOcarinaBank(u8 reverbIndex) {
    u8 channelIndex;

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        if (channelIndex != SFX_CHANNEL_OCARINA) {
            AUDIOCMD_CHANNEL_SET_REVERB_INDEX(SEQ_PLAYER_SFX, channelIndex, reverbIndex);
        }
    }
}

void Audio_PreNMI(void) {
    AudioThread_PreNMIInternal();
}

void Audio_ResetRequestedSceneSeqId(void) {
    sRequestedSceneSeqId = NA_BGM_DISABLED;
}

void Audio_ResetData(void) {
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sSfxSettings); i++) {
        sSfxSettings[i].pos = NULL;
        sSfxSettings[i].freqScale = 1.0;
        sSfxSettings[i].reverbAdd = 0;
    }
    sSfxSettingsFlags = 0;
    sTwoSemitonesLoweredFreq = 0.9f;
    sSfxIncreasedReverb = 20;
    sSfxVolumeDuration = 0;
    sPrevSeqMode = 0;
    sSfxSyncedVolume = 1.0f;
    sSfxSyncedFreq = 1.0f;
    sAudioBaseFilter = 0;
    sAudioExtraFilter = 0;
    sAudioBaseFilter2 = 0;
    sAudioExtraFilter2 = 0;
    AudioOcarina_ResetInstrument();
    sRiverFreqScaleLerp.remainingFrames = 0;
    sWaterfallFreqScaleLerp.remainingFrames = 0;
    sRiverFreqScaleLerp.value = 1;
    sWaterfallFreqScaleLerp.value = 1;
    sSfxAdjustedFreq = 1;
    sRiverSoundMainBgmVol = 0x7F;
    sRiverSoundMainBgmCurrentVol = 0x7F;
    sRiverSoundMainBgmLower = false;
    sRiverSoundMainBgmRestore = false;
    sGanonsTowerVol = 0xFF;
    sRomaniSingingTimer = 0;
    sObjSoundFanfareRequested = false;
    sSpecReverb = sSpecReverbs[gAudioSpecId];
    sAudioPauseMenuOpenOrClose = SFX_PAUSE_MENU_CLOSE;
    sPrevMainBgmSeqId = NA_BGM_DISABLED;
    AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_BGM_MAIN, 0, SEQ_IO_VAL_NONE);
    sRiverSoundBgmPos = NULL;
    sFanfareState = 0;
    sRiverSoundBgmTimer = 1;
    sRomaniSingingDisabled = false;
    sObjSoundMainBgmSeqId = NA_BGM_GENERAL_SFX;
    sPrevAmbienceSeqId = NA_BGM_DISABLED;
    sSpatialSeqFlags = 0;
    D_801FD432 = 0;
    sSpatialSeqSeqId = NA_BGM_GENERAL_SFX;
    sSpatialSubBgmFadeTimer = 0;
    D_801FD434 = 0;
    sSpatialSeqFadeTimer = 0;
    sAllPlayersMutedExceptSystemAndOcarina = false;
    sAudioPauseState = AUDIO_PAUSE_STATE_CLOSED;
    sObjSoundPlayerIndex = SEQ_PLAYER_INVALID;
    sIsFinalHoursOrSoaring = false;

    for (i = SEQ_PLAYER_BGM_MAIN; i <= SEQ_PLAYER_BGM_SUB; i++) {
        sSpatialSeqIsActive[i] = false;
    }
}

s32 func_801A46F8(void) {
    switch (func_801A3950(SEQ_PLAYER_FANFARE, true)) {
        case 0:
        case 0x8:
        case 0x10:
        case 0x18:
        case 0x20:
            return true;

        default:
            return false;
    }
}

// used for z_obj_sound and z_en_gk
void Audio_PlaySfx_AtFixedPos(Vec3f* pos, u16 sfxId) {
    static Vec3f sSfxOriginalPos[] = { 0.0f, 0.0f, 0.0f };
    s32 i;
    f32* dstPos = (f32*)&sSfxOriginalPos;
    f32* srcPos = (f32*)pos;

    // Vec3f copy
    for (i = 0; i < 3; i++) {
        dstPos[i] = srcPos[i];
    }

    Audio_PlaySfx_AtPos(sSfxOriginalPos, sfxId);
}

void Audio_PlaySfx_AtPosWithVolumeTransition(Vec3f* pos, u16 sfxId, u16 duration) {
    Audio_PlaySfx_AtPosWithVolume(pos, sfxId, 0.0f);
    Audio_SetSfxVolumeTransition(&gSfxVolume, 1.0f, duration);
}

void Audio_SetAmbienceChannelIO(u8 channelIndexRange, u8 ioPort, u8 ioData) {
    u8 firstChannelIndex;
    u8 lastChannelIndex;
    u8 channelIndex;

    if ((gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId != NA_BGM_AMBIENCE) &&
        AudioSeq_IsSeqCmdNotQueued((SEQCMD_OP_PLAY_SEQUENCE << 28) | NA_BGM_AMBIENCE,
                                   SEQCMD_OP_MASK | SEQCMD_SEQID_MASK)) {
        return;
    }

    // channelIndexRange = 01 on ioPort 1
    if ((((channelIndexRange << 8) + (u32)ioPort) == ((1 << 8) | (u32)1)) &&
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) != NA_BGM_ROMANI_RANCH)) {
        sRomaniSingingTimer = 0;
    }

    firstChannelIndex = channelIndexRange >> 4;
    lastChannelIndex = channelIndexRange & 0xF;

    if (firstChannelIndex == 0) {
        firstChannelIndex = channelIndexRange & 0xF;
    }

    for (channelIndex = firstChannelIndex; channelIndex <= lastChannelIndex; channelIndex++) {
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_AMBIENCE, channelIndex, ioPort, ioData);
    }
}

void Audio_StartAmbience(u16 initChannelMask, u16 initMuteChannelMask) {
    u8 channelIndex;

    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_AMBIENCE, SEQ_PLAYER_IO_PORT_0, 1);
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_AMBIENCE, SEQ_PLAYER_IO_PORT_4, (u8)(initChannelMask >> 8));
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_AMBIENCE, SEQ_PLAYER_IO_PORT_5, (u8)(initChannelMask & 0xFF));
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, 0x7F, 1);

    if ((AudioSeq_GetActiveSeqId(SEQ_PLAYER_AMBIENCE) != NA_BGM_DISABLED) &&
        (AudioSeq_GetActiveSeqId(SEQ_PLAYER_AMBIENCE) != NA_BGM_AMBIENCE)) {
        AudioSeq_StopSequence(SEQ_PLAYER_AMBIENCE, 0);
        AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
    }

    if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_SUB) == (NA_BGM_ENEMY | 0x800)) {
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 1);
    }

    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_AMBIENCE, 0, NA_BGM_AMBIENCE);

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        if (!(initMuteChannelMask & (1 << channelIndex)) && (initChannelMask & (1 << channelIndex))) {
            SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_AMBIENCE, channelIndex, CHANNEL_IO_PORT_1, 1);
        }
    }
}

void Audio_PlayAmbience(u8 ambienceId) {
    u8 i = 0;
    u8 channelIndex;
    u8 ioPort;
    u8 ioData;

    if (!((gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId != NA_BGM_DISABLED) &&
          (sSeqFlags[AudioEditor_GetOriginalSeq(gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId) & 0xFF & 0xFF] &
           SEQ_FLAG_NO_AMBIENCE))) {
        if (gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId != NA_BGM_AMBIENCE) {
            sPrevAmbienceSeqId = gActiveSeqs[SEQ_PLAYER_AMBIENCE].seqId;
        }

        Audio_StartAmbience(sAmbienceData[ambienceId].initChannelMask, sAmbienceData[ambienceId].initMuteChannelMask);

        while ((sAmbienceData[ambienceId].channelProperties[i] != 0xFF) &&
               (i < ARRAY_COUNT(sAmbienceData[ambienceId].channelProperties))) {
            channelIndex = sAmbienceData[ambienceId].channelProperties[i++];
            ioPort = sAmbienceData[ambienceId].channelProperties[i++];
            ioData = sAmbienceData[ambienceId].channelProperties[i++];
            SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_AMBIENCE, channelIndex, ioPort, ioData);
        }

        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_AMBIENCE, AMBIENCE_CHANNEL_SOUND_MODE, CHANNEL_IO_PORT_7, sSoundMode);
    }
}

u32 Audio_SetAmbienceRandomBend(u8 seqScriptValIn, SequenceChannel* channel) {
    u32 seqScriptValOut = seqScriptValIn;

    gAudioCtx.seqPlayers[SEQ_PLAYER_AMBIENCE].applyBend = true;

    if ((seqScriptValIn == 0) || (seqScriptValIn == 0xFF)) {
        gAudioCtx.seqPlayers[SEQ_PLAYER_AMBIENCE].bend = 1.0f;
    } else {
        seqScriptValOut = AudioThread_NextRandom();
        gAudioCtx.seqPlayers[SEQ_PLAYER_AMBIENCE].bend = 1.0293 - ((gAudioCtx.audioRandom & 0xF) / 144.0f);

        return seqScriptValOut;
    }

#ifdef AVOID_UB
    return seqScriptValOut;
#endif
}

void Audio_Init(void) {
    AudioLoad_Init(NULL, 0);
}

void AudioSfx_Init(u16 fadeTimer) {
    u8 channelIndex;

    AudioThread_ScheduleProcessCmds();
    AudioSeq_StartSequence(SEQ_PLAYER_SFX, NA_BGM_GENERAL_SFX, 0x70, fadeTimer);

    for (channelIndex = 0; channelIndex < ARRAY_COUNT(sSfxChannelState); channelIndex++) {
        AUDIOCMD_CHANNEL_SET_SFX_STATE(SEQ_PLAYER_SFX, channelIndex, &sSfxChannelState[channelIndex]);
    }

    AUDIOCMD_GLOBAL_SET_CUSTOM_FUNCTION(AUDIO_CUSTOM_FUNCTION_SEQ_0, AudioSfx_SetFreqAndStereoBits);
    AUDIOCMD_GLOBAL_SET_CUSTOM_FUNCTION(AUDIO_CUSTOM_FUNCTION_SEQ_1, Audio_SetAmbienceRandomBend);
}

void Audio_InitSound(void) {
    Audio_ResetData();
    AudioOcarina_ResetStaffs();
    AudioSfx_ResetSfxChannelState();
    AudioSeq_ResetActiveSequencesAndVolume();
    AudioSfx_Reset();
    AudioVoice_ResetData();
    AudioSfx_Init(10);
}

void Audio_ResetForAudioHeapStep3(void) {
    AudioSfx_Init(1);
    AUDIOCMD_GLOBAL_UNMUTE(AUDIOCMD_ALL_SEQPLAYERS, true);
    AudioThread_ScheduleProcessCmds();
    AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
    sIsFinalHoursOrSoaring = false;
    sObjSoundMainBgmSeqId = NA_BGM_GENERAL_SFX;
    sMuteOnlySfxAndAmbienceSeq = false;
}

void Audio_ResetForAudioHeapStep2(void) {
    Audio_ResetForAudioHeapStep3();
    if (gAudioSpecId < ARRAY_COUNT(gReverbSettingsTable)) {
        AUDIOCMD_GLOBAL_SET_REVERB_DATA(1, REVERB_DATA_TYPE_SETTINGS, &gReverbSettingsTable[gAudioSpecId][1]);
    }
}

void Audio_ResetForAudioHeapStep1(s32 specId) {
    gAudioHeapResetState = AUDIO_HEAP_RESET_STATE_RESETTING;
    Audio_ResetData();
    AudioOcarina_ResetStaffs();
    AudioSfx_ResetSfxChannelState();
    AudioSeq_ResetActiveSequences();
    AudioSfx_Reset();
    AudioVoice_ResetWord();
    if (gAudioSpecId == 0xB) {
        AudioSfx_MuteBanks((1 << BANK_PLAYER) | (1 << BANK_ITEM) | (1 << BANK_ENV) | (1 << BANK_ENEMY) |
                           (1 << BANK_OCARINA) | (1 << BANK_VOICE));
    }
}

void Audio_UnusedReset(void) {
    AudioSeq_ResetActiveSequences();
    AUDIOCMD_GLOBAL_UNMUTE(AUDIOCMD_ALL_SEQPLAYERS, true);
    Audio_ResetData();
    AudioSfx_ResetSfxChannelState();
    AudioSfx_Init(1);
    AudioVoice_ResetWord();
}
