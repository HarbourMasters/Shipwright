#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "PR/ultratypes.h"
#include "z64math.h"

// For the defines below, the names in quotes come from the official soundtrack,
// and the names in parentheses are internal names that come from MM3D.
#define NA_BGM_GENERAL_SFX 0x00                 // General Sound Effects
#define NA_BGM_AMBIENCE 0x01                    // Ambient background noises
#define NA_BGM_TERMINA_FIELD 0x02               // "Termina Field" (NA_BGM_FIELD)
#define NA_BGM_CHASE 0x03                       // "Chase" (NA_BGM_CHASE)
#define NA_BGM_MAJORAS_THEME 0x04               // "Majora's Theme" (NA_BGM_STALKID)
#define NA_BGM_CLOCK_TOWER 0x05                 // "Clock Tower" (NA_BGM_CLOCK_TOWER)
#define NA_BGM_STONE_TOWER_TEMPLE 0x06          // "Stone Tower Temple" - used for both Stone Tower and the Temple (NA_BGM_RUIN_DUNGEON_FRONT)
#define NA_BGM_INV_STONE_TOWER_TEMPLE 0x07      // "Stone Tower Temple Upside-down" - used for both Inverted Stone Tower and the Temple (NA_BGM_RUIN_DUNGEON_REVERSE)
#define NA_BGM_FAILURE_0 0x08                   // "Missed Event 1" (NA_BGM_FAILURE_0)
#define NA_BGM_FAILURE_1 0x09                   // "Missed Event 2" - used for failing the Milk Run (NA_BGM_FAILURE_1)
#define NA_BGM_HAPPY_MASK_SALESMAN 0x0A         // "Happy Mask Saleman's Theme" (NA_BGM_OMENYA)
#define NA_BGM_SONG_OF_HEALING 0x0B             // "Song Of Healing" (NA_BGM_DEMO_CURE)
#define NA_BGM_SWAMP_REGION 0x0C                // "Southern Swamp" - used for both Southern Swamp and Woodfall (NA_BGM_DAMP_AREA)
#define NA_BGM_ALIEN_INVASION 0x0D              // "Ghost Attack" (NA_BGM_UFO)
#define NA_BGM_SWAMP_CRUISE 0x0E                // "Boat Cruise" (NA_BGM_JUNGLE_CREWS)
#define NA_BGM_SHARPS_CURSE 0x0F                // "Sharp's Curse" (NA_BGM_SHARP)
#define NA_BGM_GREAT_BAY_REGION 0x10            // "Great Bay Coast" - used for Great Bay Coast, Zora Cape, and Pinnacle Rock (NA_BGM_SEA_AREA)
#define NA_BGM_IKANA_REGION 0x11                // "Ikana Valley" (NA_BGM_IKANA_AREA)
#define NA_BGM_DEKU_PALACE 0x12                 // "Deku Palace" (NA_BGM_DEKU_KING)
#define NA_BGM_MOUNTAIN_REGION 0x13             // "Mountain Village" - used for Mountain Village, Snowhead, and Goron Village exterior (NA_BGM_SNOW_AREA)
#define NA_BGM_PIRATES_FORTRESS 0x14            // "Pirates' Fortress" (NA_BGM_PIRATE_AREA)
#define NA_BGM_CLOCK_TOWN_DAY_1 0x15            // "Clock Town, First Day" - Loaded from the main Clock Town seqeuence (sequence 0x1D) (NA_BGM_DUMMY_21)
#define NA_BGM_CLOCK_TOWN_DAY_2 0x16            // "Clock Town, Second Day" - Loaded from the main Clock Town seqeuence (sequence 0x1D) (NA_BGM_DUMMY_22)
#define NA_BGM_CLOCK_TOWN_DAY_3 0x17            // "Clock Town, Third Day" - Loaded from the main Clock Town seqeuence (sequence 0x1D) (NA_BGM_DUMMY_23)
#define NA_BGM_FILE_SELECT 0x18                 // File Select (NA_BGM_FILE_SELECT)
#define NA_BGM_CLEAR_EVENT 0x19                 // "Event Clear" (NA_BGM_EVENT_CLEAR)
#define NA_BGM_ENEMY 0x1A                       // "Battle" (NA_BGM_ENEMY)
#define NA_BGM_BOSS 0x1B                        // "Boss Battle" (NA_BGM_BOSS00)
#define NA_BGM_WOODFALL_TEMPLE 0x1C             // "Woodfall Temple" (NA_BGM_JUNGLE_DUNGEON)
#define NA_BGM_CLOCK_TOWN_MAIN_SEQUENCE 0x1D    // Loads one of sequences 0x15, 0x16, or 0x17. Will also play the "morning theme" if it is the start of a day (NA_BGM_MARKET)
#define NA_BGM_OPENING 0x1E                     // "Opening" - plays during the ambush in the opening forest area (NA_BGM_OPENING)
#define NA_BGM_INSIDE_A_HOUSE 0x1F              // "House" (NA_BGM_LINK_HOUSE)
#define NA_BGM_GAME_OVER 0x20                   // "Game Over" (NA_BGM_GAME_OVER)
#define NA_BGM_CLEAR_BOSS 0x21                  // "Boss Clear" (NA_BGM_BOSS_CLEAR)
#define NA_BGM_GET_ITEM 0x22                    // "Item Catch" (NA_BGM_ITEM_GET)
#define NA_BGM_CLOCK_TOWN_DAY_2_PTR 0x23        // Just points to the "Clock Town, Second Day" sequence (sequence 0x16) (NA_BGM_GATE_OPEN, same name as the Door of Time song from OoT)
#define NA_BGM_GET_HEART 0x24                   // "Get A Heart Container!" (NA_BGM_HEART_GET)
#define NA_BGM_TIMED_MINI_GAME 0x25             // "Mini Game" (NA_BGM_MINI_GAME_2)
#define NA_BGM_GORON_RACE 0x26                  // "Goron Race" (NA_BGM_GORON_RACE)
#define NA_BGM_MUSIC_BOX_HOUSE 0x27             // "Music Box House" (NA_BGM_ORGEL_HOUSE)
#define NA_BGM_FAIRY_FOUNTAIN 0x28              // "Fairy's Fountain" - just points to the File Select sequence (sequence 0x18) (NA_BGM_GODESS)
#define NA_BGM_ZELDAS_LULLABY 0x29              // "Zelda's Theme" (NA_BGM_HIME)
#define NA_BGM_ROSA_SISTERS 0x2A                // "Rosa Sisters" (NA_BGM_SISTER_DANCER)
#define NA_BGM_OPEN_CHEST 0x2B                  // "Open Treasure Box" (NA_BGM_OPEN_TRE_BOX)
#define NA_BGM_MARINE_RESEARCH_LAB 0x2C         // "Marine Research Laboratory" (NA_BGM_DRUGSTORE)
#define NA_BGM_GIANTS_THEME 0x2D                // "Giants' Theme" (NA_BGM_KYOJIN_ROOM)
#define NA_BGM_SONG_OF_STORMS 0x2E              // "Guru-Guru's Song" (NA_BGM_FUSHA)
#define NA_BGM_ROMANI_RANCH 0x2F                // "Romani Ranch" (NA_BGM_RONRON)
#define NA_BGM_GORON_VILLAGE 0x30               // "Goron Village" (NA_BGM_GORON)
#define NA_BGM_MAYORS_OFFICE 0x31               // "Mayor's Meeting" (NA_BGM_MEETING)
#define NA_BGM_OCARINA_EPONA 0x32               // "Ocarina “Epona's Song”" (NA_BGM_OCA_EPONA)
#define NA_BGM_OCARINA_SUNS 0x33                // "Ocarina “Sun's Song”" - leftover from OoT (NA_BGM_OCA_SUNS)
#define NA_BGM_OCARINA_TIME 0x34                // "Ocarina “Song Of Time”" (NA_BGM_TIME_NORMAL)
#define NA_BGM_OCARINA_STORM 0x35               // "Ocarina “Song Of Storms”" (NA_BGM_OCA_STORM)
#define NA_BGM_ZORA_HALL 0x36                   // "Zora Hall" (NA_BGM_ZORA)
#define NA_BGM_GET_NEW_MASK 0x37                // "Get A Mask!" (NA_BGM_MASK_GET)
#define NA_BGM_MINI_BOSS 0x38                   // "Middle Boss Battle" (NA_BGM_MIDDLE_BOSS)
#define NA_BGM_GET_SMALL_ITEM 0x39              // "Small Item Catch" (NA_BGM_S_ITEM_GET)
#define NA_BGM_ASTRAL_OBSERVATORY 0x3A          // "Astral Observatory" (NA_BGM_TENMONDAI)
#define NA_BGM_CAVERN 0x3B                      // "Cavern" - used for grottos (NA_BGM_NORMAL_DUNGEON)
#define NA_BGM_MILK_BAR 0x3C                    // "Milk Bar" (NA_BGM_MILK_BAR)
#define NA_BGM_ZELDA_APPEAR 0x3D                // "Enter Zelda" - leftover from OoT (NA_BGM_APPEAR)
#define NA_BGM_SARIAS_SONG 0x3E                 // "Woods Of Mystery" - Saria's Song from OoT, also used when dancing with the scarecrow (NA_BGM_MAYOIMORI)
#define NA_BGM_GORON_GOAL 0x3F                  // "Goron Race Goal" (NA_BGM_GORON_GOAL)
#define NA_BGM_HORSE 0x40                       // "Horse Race" (NA_BGM_HORSE)
#define NA_BGM_HORSE_GOAL 0x41                  // "Horse Race Goal" (NA_BGM_HORSE_GOAL)
#define NA_BGM_INGO 0x42                        // "Gorman Track" (NA_BGM_INGO)
#define NA_BGM_KOTAKE_POTION_SHOP 0x43          // "Magic Hags' Potion Shop" (NA_BGM_KOTAKE_KOUME)
#define NA_BGM_SHOP 0x44                        // "Shop" (NA_BGM_SHOP)
#define NA_BGM_OWL 0x45                         // "Owl" (NA_BGM_OWL)
#define NA_BGM_SHOOTING_GALLERY 0x46            // "Shooting Gallery" (NA_BGM_MINI_GAME)
#define NA_BGM_OCARINA_SOARING 0x47             // "Ocarina “Song Of Soaring”" (NA_BGM_OCA_BIGWING)
#define NA_BGM_OCARINA_HEALING 0x48             // "Ocarina “Song Of Healing”" (NA_BGM_OCA_CURE)
#define NA_BGM_INVERTED_SONG_OF_TIME 0x49       // "Ocarina “Inverted Song Of Time”" (NA_BGM_TIME_SPEED)
#define NA_BGM_SONG_OF_DOUBLE_TIME 0x4A         // "Ocarina “Song Of Double Time”" (NA_BGM_TIME_JUMP)
#define NA_BGM_SONATA_OF_AWAKENING 0x4B         // "Sonata of Awakening" (NA_BGM_DEMO_AWAKING)
#define NA_BGM_GORON_LULLABY 0x4C               // "Goron Lullaby" (NA_BGM_DEMO_LULLABY)
#define NA_BGM_NEW_WAVE_BOSSA_NOVA 0x4D         // "New Wave Bossa Nova" - both saxophone and vocals (NA_BGM_DEMO_TIDE)
#define NA_BGM_ELEGY_OF_EMPTINESS 0x4E          // "Elegy Of Emptiness" (NA_BGM_DEMO_SHELL)
#define NA_BGM_OATH_TO_ORDER 0x4F               // "Oath To Order" (NA_BGM_DEMO_PLEDGE)
#define NA_BGM_SWORD_TRAINING_HALL 0x50         // "Swordsman's School" (NA_BGM_EXERCISE_HALL)
#define NA_BGM_OCARINA_LULLABY_INTRO 0x51       // "Ocarina “Goron Lullaby Intro”" (NA_BGM_DEMO_LULLABY_HALF)
#define NA_BGM_LEARNED_NEW_SONG 0x52            // "Get The Ocarina!" (NA_BGM_OCA_YOUSEI)
#define NA_BGM_BREMEN_MARCH 0x53                // "Bremen March" (NA_BGM_FACEMASK)
#define NA_BGM_BALLAD_OF_THE_WIND_FISH 0x54     // "Ballad Of The Wind Fish" - quartet performed in on the Milk Bar stage (NA_BGM_QUARTET)
#define NA_BGM_SONG_OF_SOARING 0x55             // "Song Of Soaring" (NA_BGM_WING_WARP)
#define NA_BGM_MILK_BAR_DUPLICATE 0x56          // Just points to the "Milk Bar" sequence (sequence 0x3C) (NA_BGM_MILK_BAR_DUMMY)
#define NA_BGM_FINAL_HOURS 0x57                 // "Last Day" (NA_BGM_TIME_LIMIT)
#define NA_BGM_MIKAU_RIFF 0x58                  // "Mikau" - looping part of the song (NA_BGM_MIKAU_LIFF)
#define NA_BGM_MIKAU_FINALE 0x59                // "Mikau" - a single guitar chord to end the song (NA_BGM_MIKAU_FIN)
#define NA_BGM_FROG_SONG 0x5A                   // "Frog Song" - conducting the frogs with Don Gero's Mask (NA_BGM_FROG_SONG)
#define NA_BGM_OCARINA_SONATA 0x5B              // "Ocarina “Sonata Of Awakening”" (NA_BGM_OCA_AWAKING)
#define NA_BGM_OCARINA_LULLABY 0x5C             // "Ocarina “Goron Lullaby”" (NA_BGM_OCA_LULLABY)
#define NA_BGM_OCARINA_NEW_WAVE 0x5D            // "Ocarina “New Wave Bossa Nova”" (NA_BGM_OCA_TIDE)
#define NA_BGM_OCARINA_ELEGY 0x5E               // "Ocarina “Elegy of Emptiness”" (NA_BGM_OCA_SHELL)
#define NA_BGM_OCARINA_OATH 0x5F                // "Ocarina “Oath To Order”" (NA_BGM_OCA_PLEDGE)
#define NA_BGM_MAJORAS_LAIR 0x60                // Majora Boss Room. Just points to the "Last Day" sequence (sequence 0x57) (NA_BGM_LAST_DUNGEON)
#define NA_BGM_OCARINA_LULLABY_INTRO_PTR 0x61   // Just points to the "Ocarina “Goron Lullaby Intro”" sequence (sequence 0x51) (NA_BGM_OCA_LULLABY_HALF)
#define NA_BGM_OCARINA_GUITAR_BASS_SESSION 0x62 // "Bass & Guitar Session" (NA_BGM_OCA_SESSION)
#define NA_BGM_PIANO_SESSION 0x63               // "Piano Solo" (NA_BGM_DEMO_SESSION)
#define NA_BGM_INDIGO_GO_SESSION 0x64           // "The Indigo-Go's" (NA_BGM_ALLPART_SESSION)
#define NA_BGM_SNOWHEAD_TEMPLE 0x65             // "Snowhead Temple" (NA_BGM_SILVER_DUNGEON)
#define NA_BGM_GREAT_BAY_TEMPLE 0x66            // "Great Bay Temple" (NA_BGM_DEEPBLUE_DUNGEON)
#define NA_BGM_NEW_WAVE_SAXOPHONE 0x67          // "New Wave Bossa Nova" - saxophone only, no vocals (NA_BGM_DEMO_TIDE_SAX)
#define NA_BGM_NEW_WAVE_VOCAL 0x68              // "New Wave Bossa Nova" - vocals only, no saxophone (NA_BGM_DEMO_TIDE_VOCAL)
#define NA_BGM_MAJORAS_WRATH 0x69               // "Majora's Wrath Battle" (NA_BGM_MUJURA_1)
#define NA_BGM_MAJORAS_INCARNATION 0x6A         // "Majora's Incarnate Battle" (NA_BGM_MUJURA_2)
#define NA_BGM_MAJORAS_MASK 0x6B                // "Majora's Mask Battle" (NA_BGM_MUJURA_3)
#define NA_BGM_BASS_PLAY 0x6C                   // "Bass Practice" (NA_BGM_BASS_PLAY)
#define NA_BGM_DRUMS_PLAY 0x6D                  // "Drums Practice" (NA_BGM_DRUMS_PLAY)
#define NA_BGM_PIANO_PLAY 0x6E                  // "Piano Practice" (NA_BGM_PIANO_PLAY)
#define NA_BGM_IKANA_CASTLE 0x6F                // "Ikana Castle" (NA_BGM_IKANA_CASTLE)
#define NA_BGM_GATHERING_GIANTS 0x70            // "Calling The Four Giants" (NA_BGM_GATHERING)
#define NA_BGM_KAMARO_DANCE 0x71                // "Kamaro's Dance" (NA_BGM_CAMARO_DANCE)
#define NA_BGM_CREMIA_CARRIAGE 0x72             // "Cremia's Carriage" (NA_BGM_DONKEY_CART)
#define NA_BGM_KEATON_QUIZ 0x73                 // "Keaton's Quiz" (NA_BGM_QUIZ)
#define NA_BGM_END_CREDITS 0x74                 // "The End / Credits" - plays the first half, then loads sequence 0x7F for the second half (NA_BGM_END_DEMO)
#define NA_BGM_OPENING_LOOP 0x75                // Similar to "Opening" (sequence 0x1E), but the intro loops endlessly (NA_BGM_OPENING_LOOP)
#define NA_BGM_TITLE_THEME 0x76                 // "Title Theme" (NA_BGM_TITLE)
#define NA_BGM_DUNGEON_APPEAR 0x77              // "Woodfall Rises" (NA_BGM_DUNGEON_APPEAR)
#define NA_BGM_WOODFALL_CLEAR 0x78              // "Southern Swamp Clears" (NA_BGM_PURIFICATION_0)
#define NA_BGM_SNOWHEAD_CLEAR 0x79              // "Snowhead Clear" (NA_BGM_PURIFICATION_1)
#define NA_BGM_INTO_THE_MOON 0x7B               // "To The Moon" (NA_BGM_INTO_THE_MOON)
#define NA_BGM_GOODBYE_GIANT 0x7C               // "The Giants' Exit" (NA_BGM_GOODBYE_GIANT)
#define NA_BGM_TATL_AND_TAEL 0x7D               // "Tatl & Tael" (NA_BGM_CHAT)
#define NA_BGM_MOONS_DESTRUCTION 0x7E           // "Moon's Destruction" (NA_BGM_MOON_VANISH)
#define NA_BGM_END_CREDITS_SECOND_HALF 0x7F     // "The End / Credits" - second half. Sequence 0x74 loads this after playing the first half (NA_BGM_OCA_MUS)

// This is used as a placeholder in scene commands to indicate to use the nature ambience instead.
#define NA_BGM_NO_MUSIC 0x7F
#define NA_BGM_DISABLED 0xFFFF

#define SEQ_FLAG_ASYNC 0x8000

typedef enum SequencePlayerId {
    /*    0 */ SEQ_PLAYER_BGM_MAIN,
    /*    1 */ SEQ_PLAYER_FANFARE,
    /*    2 */ SEQ_PLAYER_SFX,
    /*    3 */ SEQ_PLAYER_BGM_SUB,
    /*    4 */ SEQ_PLAYER_AMBIENCE,
    /*    5 */ SEQ_PLAYER_MAX,
    /* 0xFF */ SEQ_PLAYER_INVALID = 0xFF
} SequencePlayerId;

typedef enum SequenceMode {
    /* 0 */ SEQ_MODE_DEFAULT,
    /* 1 */ SEQ_MODE_ENEMY,
    /* 2 */ SEQ_MODE_STILL, // Not moving or first-person view
    /* 3 */ SEQ_MODE_IGNORE
} SequenceMode;

typedef enum ChannelIOPort {
    /* 0 */ CHANNEL_IO_PORT_0,
    /* 1 */ CHANNEL_IO_PORT_1,
    /* 2 */ CHANNEL_IO_PORT_2,
    /* 3 */ CHANNEL_IO_PORT_3,
    /* 4 */ CHANNEL_IO_PORT_4,
    /* 5 */ CHANNEL_IO_PORT_5,
    /* 6 */ CHANNEL_IO_PORT_6,
    /* 7 */ CHANNEL_IO_PORT_7
} ChannelIOPort;

typedef enum VolumeScaleIndex {
    /* 0 */ VOL_SCALE_INDEX_BGM_MAIN,
    /* 1 */ VOL_SCALE_INDEX_FANFARE,
    /* 2 */ VOL_SCALE_INDEX_SFX,
    /* 3 */ VOL_SCALE_INDEX_BGM_SUB,
    /* 4 */ VOL_SCALE_INDEX_MAX
} VolumeScaleIndex; // May be worth using SequencePlayerId instead

typedef enum AmbienceChannelIndex {
    /* 0x0 */ AMBIENCE_CHANNEL_STREAM_0,
    /* 0x1 */ AMBIENCE_CHANNEL_CRITTER_0,
    /* 0x2 */ AMBIENCE_CHANNEL_CRITTER_1,
    /* 0x3 */ AMBIENCE_CHANNEL_CRITTER_2,
    /* 0x4 */ AMBIENCE_CHANNEL_CRITTER_3,
    /* 0x5 */ AMBIENCE_CHANNEL_CRITTER_4,
    /* 0x6 */ AMBIENCE_CHANNEL_CRITTER_5,
    /* 0x7 */ AMBIENCE_CHANNEL_CRITTER_6,
    /* 0x8 */ AMBIENCE_CHANNEL_CRITTER_7,
    /* 0xC */ AMBIENCE_CHANNEL_STREAM_1 = 12,
    /* 0xD */ AMBIENCE_CHANNEL_SOUND_MODE,
    /* 0xE */ AMBIENCE_CHANNEL_RAIN,
    /* 0xF */ AMBIENCE_CHANNEL_LIGHTNING
} AmbienceChannelIndex; // seqPlayerIndex = SEQ_PLAYER_AMBIENCE

typedef enum AmbienceId {
    /* 0x00 */ AMBIENCE_ID_00,
    /* 0x01 */ AMBIENCE_ID_01,
    /* 0x02 */ AMBIENCE_ID_02,
    /* 0x03 */ AMBIENCE_ID_03,
    /* 0x04 */ AMBIENCE_ID_04,
    /* 0x05 */ AMBIENCE_ID_05,
    /* 0x06 */ AMBIENCE_ID_06,
    /* 0x07 */ AMBIENCE_ID_07,
    /* 0x08 */ AMBIENCE_ID_08,
    /* 0x09 */ AMBIENCE_ID_09,
    /* 0x0A */ AMBIENCE_ID_0A,
    /* 0x0B */ AMBIENCE_ID_0B,
    /* 0x0C */ AMBIENCE_ID_0C,
    /* 0x0D */ AMBIENCE_ID_0D,
    /* 0x0E */ AMBIENCE_ID_0E,
    /* 0x0F */ AMBIENCE_ID_0F,
    /* 0x10 */ AMBIENCE_ID_10,
    /* 0x11 */ AMBIENCE_ID_11,
    /* 0x12 */ AMBIENCE_ID_12,
    /* 0x13 */ AMBIENCE_ID_13,
    /* 0xFF */ AMBIENCE_ID_DISABLED = 0xFF
} AmbienceId; // seqPlayerIndex = SEQ_PLAYER_AMBIENCE

typedef enum AmbienceStreamId {
    /* 0 */ AMBIENCE_STREAM_0,
    /* 1 */ AMBIENCE_STREAM_1,
    /* 2 */ AMBIENCE_STREAM_2,
    /* 3 */ AMBIENCE_STREAM_3
} AmbienceStreamId;

typedef enum AmbienceCritterId {
    /* 0x00 */ AMBIENCE_CRITTER_00,
    /* 0x01 */ AMBIENCE_CRITTER_01,
    /* 0x02 */ AMBIENCE_CRITTER_02,
    /* 0x03 */ AMBIENCE_CRITTER_03,
    /* 0x04 */ AMBIENCE_CRITTER_04,
    /* 0x05 */ AMBIENCE_CRITTER_05,
    /* 0x06 */ AMBIENCE_CRITTER_06,
    /* 0x07 */ AMBIENCE_CRITTER_07,
    /* 0x08 */ AMBIENCE_CRITTER_08,
    /* 0x09 */ AMBIENCE_CRITTER_09,
    /* 0x0A */ AMBIENCE_CRITTER_10,
    /* 0x0B */ AMBIENCE_CRITTER_11,
    /* 0x0C */ AMBIENCE_CRITTER_12,
    /* 0x0D */ AMBIENCE_CRITTER_13,
    /* 0x0E */ AMBIENCE_CRITTER_14,
    /* 0x0F */ AMBIENCE_CRITTER_15,
    /* 0x10 */ AMBIENCE_CRITTER_16,
    /* 0x11 */ AMBIENCE_CRITTER_17,
    /* 0x12 */ AMBIENCE_CRITTER_18,
    /* 0x13 */ AMBIENCE_CRITTER_19
} AmbienceCritterId;

#define AMBIENCE_IO_CRITTER_0_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_0_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_0_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_0_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_1_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_1_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_1_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_1_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_2_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_2_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_2_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_2_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_3_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_3_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_3_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_3_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_4_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_4_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_4_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_4_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_5_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_5_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_5_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_5_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_6_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_6_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_6_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_6_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_STREAM_0_TYPE(type)         AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_STREAM_0_PORT3(data)        AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_3, data
#define AMBIENCE_IO_STREAM_0_PORT4(data)        AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_4, data

#define AMBIENCE_IO_STREAM_1_TYPE(type)         AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_STREAM_1_PORT3(data)        AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_3, data
#define AMBIENCE_IO_STREAM_1_PORT4(data)        AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_4, data

#define AMBIENCE_IO_RAIN_PORT4(data)            AMBIENCE_CHANNEL_RAIN, CHANNEL_IO_PORT_4, data

#define AMBIENCE_IO_ENTRIES_END 0xFF

typedef struct {
    /* 0x00 */ f32 volCur;
    /* 0x04 */ f32 volTarget;
    /* 0x08 */ f32 volStep;
    /* 0x0C */ f32 freqScaleCur;
    /* 0x10 */ f32 freqScaleTarget;
    /* 0x14 */ f32 freqScaleStep;
    /* 0x18 */ u16 volTimer;
    /* 0x1A */ u16 freqScaleTimer;
} ActiveSequenceChannelData; // size = 0x1C

typedef struct {
    /* 0x000 */ ActiveSequenceChannelData channelData[16]; // SEQ_NUM_CHANNELS
    /* 0x1C0 */ f32 volCur;
    /* 0x1C4 */ f32 volTarget;
    /* 0x1C8 */ f32 volStep;
    /* 0x1CC */ u32 tempoCmd;
    /* 0x1D0 */ f32 tempoCur;
    /* 0x1D4 */ f32 tempoTarget;
    /* 0x1D8 */ f32 tempoStep;
    /* 0x1DC */ u32 setupCmd[8]; // setup commands
    /* 0x1FC */ u32 startAsyncSeqCmd; // temporarily stores the seqCmd used in SEQCMD_PLAY_SEQUENCE, to be called again once the font is reloaded in
    /* 0x200 */ u16 volTimer;
    /* 0x202 */ u16 tempoOriginal;
    /* 0x204 */ u16 tempoTimer;
    /* 0x206 */ u16 freqScaleChannelFlags;
    /* 0x208 */ u16 volChannelFlags;
    /* 0x20A */ u16 seqId;
    /* 0x20C */ u16 prevSeqId; // last seqId played on a player
    /* 0x20E */ u16 channelPortMask;
    /* 0x210 */ u8 isWaitingForFonts;
    /* 0x211 */ u8 fontId;
    /* 0x212 */ u8 volScales[VOL_SCALE_INDEX_MAX];
    /* 0x216 */ u8 volFadeTimer;
    /* 0x217 */ u8 fadeVolUpdate;
    /* 0x218 */ u8 setupCmdTimer;
    /* 0x219 */ u8 setupCmdNum; // number of setup commands
    /* 0x21A */ u8 setupFadeTimer;
    /* 0x21B */ u8 isSeqPlayerInit;
} ActiveSequence; // size = 0x21C

// 2S2H [Custom Audio]. Was originally u8 seqId. Made 16 bit to allow for more than 255 sequences.
typedef struct {
    /* 0x0 */ u16 seqId;
    /* 0x1 */ u8 priority;
} SeqRequest; // size = 0x2

// functions in sequence.c

void AudioSeq_StartSequence(u8 seqPlayerIndex, u16 seqId, u16 seqArgs, u16 fadeInDuration);
void AudioSeq_StopSequence(u8 seqPlayerIndex, u16 fadeOutDuration);
void AudioSeq_QueueSeqCmd(u32 cmd);
void AudioSeq_ProcessSeqCmds(void);
u16 AudioSeq_GetActiveSeqId(u8 seqPlayerIndex);
s32 AudioSeq_IsSeqCmdNotQueued(u32 cmdVal, u32 cmdMask);
void AudioSeq_SetVolumeScale(u8 seqPlayerIndex, u8 scaleIndex, u8 targetVol, u8 volFadeTimer);
void AudioSeq_UpdateActiveSequences(void);
u8 AudioSeq_UpdateAudioHeapReset(void);
u8 AudioSeq_ResetReverb(void);
void AudioSeq_ResetActiveSequences(void);
void AudioSeq_ResetActiveSequencesAndVolume(void);

// functions in code_8019AF00.c

void Audio_PlayObjSoundBgm(Vec3f* pos, s16 seqId);
void Audio_PlayObjSoundFanfare(Vec3f* pos, s16 seqId);
void Audio_PlaySubBgmAtPos(Vec3f* pos, u16 seqId, f32 maxDist);
void Audio_PlaySubBgmAtPosWithFilter(Vec3f* pos, u16 seqId, f32 maxDist);
void Audio_PlaySequenceAtDefaultPos(u8 seqPlayerIndex, u16 seqId);
void Audio_PlaySequenceAtPos(u8 seqPlayerIndex, Vec3f* pos, u16 seqId, f32 maxDist);
void Audio_PlayMorningSceneSequence(u16 seqId, u8 dayMinusOne);
void Audio_PlaySceneSequence(u16 seqId, u8 dayMinusOne);
void Audio_PlaySubBgm(u16 seqId);
void Audio_PlaySequenceInCutscene(u16 seqId);
void Audio_PlayBgm_StorePrevBgm(u16 seqId);
void Audio_PlayFanfareWithPlayerIOPort7(u16 seqId, u8 ioData);
void Audio_PlayFanfare(u16 seqId);
void Audio_PlayFanfareWithPlayerIOCustomPort(u16 seqId, s8 ioPort, u8 ioData);
void Audio_PlaySequenceWithSeqPlayerIO(s8 seqPlayerIndex, u16 seqId, u8 fadeInDuration, s8 ioPort, u8 ioData);
void Audio_PlayAmbience(u8 ambienceId);

void Audio_StopSequenceAtDefaultPos(void);
void Audio_StopSubBgm(void);
void Audio_StopSequenceInCutscene(u16 seqId);
void Audio_StopFanfare(u16 duration);

void Audio_SetBgmVolumeOff(void);
void Audio_SetBgmVolumeOn(void);
void Audio_SetMainBgmVolume(u8 targetVolume, u8 volumeFadeTimer);
void Audio_SetSequenceMode(u8 seqMode);

void Audio_SetPauseState(u8 isPauseMenuOpen);
void Audio_SetEnvReverb(s8 reverb);
void Audio_SetCodeReverb(s8 reverb);
void Audio_SetFileSelectSettings(s8 audioSetting);
void Audio_SetBaseFilter(u8 filter);
void Audio_SetExtraFilter(u8 filter);
void Audio_SetCutsceneFlag(s8 flag);
void Audio_SetSpec(u8 specId);
void Audio_SetAmbienceChannelIO(u8 channelIndexRange, u8 ioPort, u8 ioData);
void Audio_SetSeqTempoAndFreq(u8 seqPlayerIndex, f32 freqTempoScale, u8 duration);

void Audio_MuteSeqPlayerBgmSub(u8 isMuted);
void Audio_MuteAllSeqExceptSystemAndOcarina(u16 duration);
void Audio_MuteSfxAndAmbienceSeqExceptSysAndOca(u16 duration);

void func_801A0204(s16 seqId);
void func_801A246C(u8 seqPlayerIndex, u8 type);
s32 Audio_IsSequencePlaying(u16 seqId);
void Audio_RestorePrevBgm(void);
void Audio_UpdateEnemyBgmVolume(f32 dist);
u8 func_801A3950(u8 seqPlayerIndex, u8 resetChannelIO);
u8 func_801A39F8(void);
s32 func_801A46F8(void);

extern SeqRequest sSeqRequests[][5];
extern u8 sNumSeqRequests[5];
extern u32 sAudioSeqCmds[0x100];
extern ActiveSequence gActiveSeqs[];

#endif
