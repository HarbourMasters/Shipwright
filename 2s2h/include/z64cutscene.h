#ifndef Z64CUTSCENE_H
#define Z64CUTSCENE_H

#include "ultra64.h"
#include "unk.h"


typedef union {
    struct {
        /* 0x00 */ u16 id; // "dousa"
        /* 0x02 */ u16 startFrame;
        /* 0x04 */ u16 endFrame;
        /* 0x06 */ Vec3us rot;
        /* 0x0C */ Vec3i startPos;
        /* 0x18 */ Vec3i endPos;
        /* 0x24 */ Vec3f normal;
    };
    s32 _words[12];
} CsCmdActorCue; // size = 0x30

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdMisc; // size = 0x30

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
        /* 0x6 */ u8  hour;
        /* 0x7 */ u8  minute;
    };
    s32 _words[3];
} CsCmdTime; // size = 0xC

typedef union {
    struct {
        /* 0x0 */ u16 settingPlusOne;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdLightSetting; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 seqIdPlusOne;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdStartSeq; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 seqIdPlusOne;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdStopSeq; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 seqPlayer;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[3];
} CsCmdFadeOutSeq; // size = 0xC

typedef enum {
    /* 1 */ CS_FADE_OUT_BGM_MAIN = 1,
    /* 2 */ CS_FADE_OUT_FANFARE
} CutsceneFadeOutSeqPlayer;

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdStartAmbience; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdFadeOutAmbience; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdModifySeq; // size = 0x8

typedef enum {
    /* 1 */ CS_MOD_SEQ_0 = 1,
    /* 2 */ CS_MOD_SEQ_1,
    /* 3 */ CS_MOD_SEQ_2,
    /* 4 */ CS_MOD_AMBIENCE_0,
    /* 5 */ CS_MOD_AMBIENCE_1,
    /* 6 */ CS_MOD_AMBIENCE_2,
    /* 7 */ CS_MOD_SEQ_STORE,
    /* 8 */ CS_MOD_SEQ_RESTORE
} CsModifySeqType;

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdDestination; // size = 0x8

typedef enum {
    /* 1 */ CS_DESTINATION_DEFAULT = 1,
    /* 2 */ CS_DESTINATION_BOSS_WARP
} CsDestinationType;

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdChooseCreditsScene; // size = 0x8

typedef enum {
    /*  1 */ CS_CREDITS_DESTINATION = CS_DESTINATION_DEFAULT,
    /*  2 */ CS_CREDITS_MASK_KAMARO,
    /*  3 */ CS_CREDITS_MASK_GREAT_FAIRY,
    /*  4 */ CS_CREDITS_MASK_ROMANI,
    /*  5 */ CS_CREDITS_MASK_BLAST,
    /*  6 */ CS_CREDITS_MASK_CIRCUS_LEADER,
    /*  7 */ CS_CREDITS_MASK_BREMEN,
    /*  8 */ CS_CREDITS_IKANA,
    /*  9 */ CS_CREDITS_MASK_COUPLE,
    /* 10 */ CS_CREDITS_MASK_BUNNY,
    /* 11 */ CS_CREDITS_MASK_POSTMAN
} CsChooseCreditsSceneType;

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdMotionBlur; // size = 0x8

typedef enum {
    /* 1 */ CS_MOTION_BLUR_ENABLE = 1, // enable instantly
    /* 2 */ CS_MOTION_BLUR_DISABLE // disable gradually
} CsMotionBlurType;

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdSfxReverbIndexTo2; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdSfxReverbIndexTo1; // size = 0x8

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
    };
    s32 _words[2];
} CsCmdTransition; // size = 0x8

typedef enum {
    /*  1 */ CS_TRANS_GRAY_FILL_IN = 1, // has hardcoded sounds for some scenes
    /*  2 */ CS_TRANS_BLUE_FILL_IN,
    /*  3 */ CS_TRANS_RED_FILL_OUT,
    /*  4 */ CS_TRANS_GREEN_FILL_OUT,
    /*  5 */ CS_TRANS_GRAY_FILL_OUT,
    /*  6 */ CS_TRANS_BLUE_FILL_OUT,
    /*  7 */ CS_TRANS_RED_FILL_IN,
    /*  8 */ CS_TRANS_GREEN_FILL_IN,
    /*  9 */ CS_TRANS_TRIGGER_INSTANCE, // used with `TRANS_MODE_INSTANCE_WAIT`
    /* 10 */ CS_TRANS_BLACK_FILL_OUT,
    /* 11 */ CS_TRANS_BLACK_FILL_IN,
    /* 12 */ CS_TRANS_GRAY_TO_BLACK,
    /* 13 */ CS_TRANS_BLACK_TO_GRAY
} CutsceneTransitionType;

typedef union {
    struct {
        /* 0x0 */ u16 textId; // can also be an ocarina action for `CS_TEXT_OCARINA_ACTION`
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
        /* 0x6 */ u16 type;
        /* 0x8 */ u16 altTextId1;
        /* 0xA */ u16 altTextId2;
    };
    s32 _words[3];
} CsCmdText; // size = 0xC

typedef enum {
    /* -1 */ CS_TEXT_TYPE_NONE = -1,
    /*  0 */ CS_TEXT_TYPE_DEFAULT,
    /*  1 */ CS_TEXT_TYPE_1,
    /*  2 */ CS_TEXT_OCARINA_ACTION,
    /*  3 */ CS_TEXT_TYPE_3,
    /*  4 */ CS_TEXT_TYPE_BOSSES_REMAINS, // use `altText1` in the Giant Cutscene if all remains are already obtained
    /*  5 */ CS_TEXT_TYPE_ALL_NORMAL_MASKS
} CutsceneTextType;

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
        /* 0x6 */ u8 intensity;
        /* 0x7 */ u8 decayTimer;
        /* 0x8 */ u8 decayStep;
    };
    s32 _words[3];
} CsCmdRumble; // size = 0xC

typedef enum {
    /* 1 */ CS_RUMBLE_ONCE = 1, // rumble once when startFrame is reached
    /* 2 */ CS_RUMBLE_PULSE // rumble every 64 frames between startFrame and endFrame
} CutsceneRumbleType;

typedef union {
    struct {
        /* 0x0 */ u16 type;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame;
        /* 0x6 */ Color_RGB8 color;
    };
    s32 _words[3];
} CsCmdTransitionGeneral; // size = 0xC

typedef enum {
    /* 1 */ CS_TRANS_GENERAL_FILL_IN = 1,
    /* 2 */ CS_TRANS_GENERAL_FILL_OUT
} CsTransitionGeneralType;

typedef union {
    struct {
        /* 0x0 */ u16 giveTatl;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdGiveTatl; // size = 0x8

typedef enum {
    /* 0 */ CS_STATE_IDLE,
    /* 1 */ CS_STATE_START,
    /* 2 */ CS_STATE_RUN,
    /* 3 */ CS_STATE_STOP,
    /* 4 */ CS_STATE_RUN_UNSTOPPABLE
} CutsceneState;

typedef union {
    struct {
        /* 0x0 */ u16 unused0;
        /* 0x2 */ u16 startFrame;
        /* 0x4 */ u16 endFrame; // unused
    };
    s32 _words[2];
} CsCmdUnimplemented; // size = 0x8

typedef enum {
    /* 0x00A */ CS_CMD_TEXT = 10,
    /* 0x05A */ CS_CMD_CAMERA_SPLINE = 90,
    /* 0x064 */ CS_CMD_ACTOR_CUE_100 = 100,
    /* 0x065 */ CS_CMD_ACTOR_CUE_101,
    /* 0x066 */ CS_CMD_ACTOR_CUE_102,
    /* 0x067 */ CS_CMD_ACTOR_CUE_103,
    /* 0x068 */ CS_CMD_ACTOR_CUE_104,
    /* 0x069 */ CS_CMD_ACTOR_CUE_105,
    /* 0x06A */ CS_CMD_ACTOR_CUE_106,
    /* 0x06B */ CS_CMD_ACTOR_CUE_107,
    /* 0x06C */ CS_CMD_ACTOR_CUE_108,
    /* 0x06D */ CS_CMD_ACTOR_CUE_109,
    /* 0x06E */ CS_CMD_ACTOR_CUE_110,
    /* 0x06F */ CS_CMD_ACTOR_CUE_111,
    /* 0x070 */ CS_CMD_ACTOR_CUE_112,
    /* 0x071 */ CS_CMD_ACTOR_CUE_113,
    /* 0x072 */ CS_CMD_ACTOR_CUE_114,
    /* 0x073 */ CS_CMD_ACTOR_CUE_115,
    /* 0x074 */ CS_CMD_ACTOR_CUE_116,
    /* 0x075 */ CS_CMD_ACTOR_CUE_117,
    /* 0x076 */ CS_CMD_ACTOR_CUE_118,
    /* 0x077 */ CS_CMD_ACTOR_CUE_119,
    /* 0x078 */ CS_CMD_ACTOR_CUE_120,
    /* 0x079 */ CS_CMD_ACTOR_CUE_121,
    /* 0x07A */ CS_CMD_ACTOR_CUE_122,
    /* 0x07B */ CS_CMD_ACTOR_CUE_123,
    /* 0x07C */ CS_CMD_ACTOR_CUE_124,
    /* 0x07D */ CS_CMD_ACTOR_CUE_125,
    /* 0x07E */ CS_CMD_ACTOR_CUE_126,
    /* 0x07F */ CS_CMD_ACTOR_CUE_127,
    /* 0x080 */ CS_CMD_ACTOR_CUE_128,
    /* 0x081 */ CS_CMD_ACTOR_CUE_129,
    /* 0x082 */ CS_CMD_ACTOR_CUE_130,
    /* 0x083 */ CS_CMD_ACTOR_CUE_131,
    /* 0x084 */ CS_CMD_ACTOR_CUE_132,
    /* 0x085 */ CS_CMD_ACTOR_CUE_133,
    /* 0x086 */ CS_CMD_ACTOR_CUE_134,
    /* 0x087 */ CS_CMD_ACTOR_CUE_135,
    /* 0x088 */ CS_CMD_ACTOR_CUE_136,
    /* 0x089 */ CS_CMD_ACTOR_CUE_137,
    /* 0x08A */ CS_CMD_ACTOR_CUE_138,
    /* 0x08B */ CS_CMD_ACTOR_CUE_139,
    /* 0x08C */ CS_CMD_ACTOR_CUE_140,
    /* 0x08D */ CS_CMD_ACTOR_CUE_141,
    /* 0x08E */ CS_CMD_ACTOR_CUE_142,
    /* 0x08F */ CS_CMD_ACTOR_CUE_143,
    /* 0x090 */ CS_CMD_ACTOR_CUE_144,
    /* 0x091 */ CS_CMD_ACTOR_CUE_145,
    /* 0x092 */ CS_CMD_ACTOR_CUE_146,
    /* 0x093 */ CS_CMD_ACTOR_CUE_147,
    /* 0x094 */ CS_CMD_ACTOR_CUE_148,
    /* 0x095 */ CS_CMD_ACTOR_CUE_149,
    /* 0x096 */ CS_CMD_MISC,
    /* 0x097 */ CS_CMD_LIGHT_SETTING,
    /* 0x098 */ CS_CMD_TRANSITION,
    /* 0x099 */ CS_CMD_MOTION_BLUR,
    /* 0x09A */ CS_CMD_GIVE_TATL,
    /* 0x09B */ CS_CMD_TRANSITION_GENERAL,
    /* 0x09C */ CS_CMD_FADE_OUT_SEQ,
    /* 0x09D */ CS_CMD_TIME,
    /* 0x0C8 */ CS_CMD_PLAYER_CUE = 200,
    /* 0x0C9 */ CS_CMD_ACTOR_CUE_201,
    /* 0x0FA */ CS_CMD_UNK_DATA_FA = 0xFA,
    /* 0x0FE */ CS_CMD_UNK_DATA_FE = 0xFE,
    /* 0x0FF */ CS_CMD_UNK_DATA_FF,
    /* 0x100 */ CS_CMD_UNK_DATA_100,
    /* 0x101 */ CS_CMD_UNK_DATA_101,
    /* 0x102 */ CS_CMD_UNK_DATA_102,
    /* 0x103 */ CS_CMD_UNK_DATA_103,
    /* 0x104 */ CS_CMD_UNK_DATA_104,
    /* 0x105 */ CS_CMD_UNK_DATA_105,
    /* 0x108 */ CS_CMD_UNK_DATA_108 = 0x108,
    /* 0x109 */ CS_CMD_UNK_DATA_109,
    /* 0x12C */ CS_CMD_START_SEQ = 300,
    /* 0x12D */ CS_CMD_STOP_SEQ,
    /* 0x12E */ CS_CMD_START_AMBIENCE,
    /* 0x12F */ CS_CMD_FADE_OUT_AMBIENCE,
    /* 0x130 */ CS_CMD_SFX_REVERB_INDEX_2,
    /* 0x131 */ CS_CMD_SFX_REVERB_INDEX_1,
    /* 0x132 */ CS_CMD_MODIFY_SEQ,
    /* 0x15E */ CS_CMD_DESTINATION = 350,
    /* 0x15F */ CS_CMD_CHOOSE_CREDITS_SCENES,
    /* 0x190 */ CS_CMD_RUMBLE = 400,
    /* 0x1C2 */ CS_CMD_ACTOR_CUE_450 = 450,
    /* 0x1C3 */ CS_CMD_ACTOR_CUE_451,
    /* 0x1C4 */ CS_CMD_ACTOR_CUE_452,
    /* 0x1C5 */ CS_CMD_ACTOR_CUE_453,
    /* 0x1C6 */ CS_CMD_ACTOR_CUE_454,
    /* 0x1C7 */ CS_CMD_ACTOR_CUE_455,
    /* 0x1C8 */ CS_CMD_ACTOR_CUE_456,
    /* 0x1C9 */ CS_CMD_ACTOR_CUE_457,
    /* 0x1CA */ CS_CMD_ACTOR_CUE_458,
    /* 0x1CB */ CS_CMD_ACTOR_CUE_459,
    /* 0x1CC */ CS_CMD_ACTOR_CUE_460,
    /* 0x1CD */ CS_CMD_ACTOR_CUE_461,
    /* 0x1CE */ CS_CMD_ACTOR_CUE_462,
    /* 0x1CF */ CS_CMD_ACTOR_CUE_463,
    /* 0x1D0 */ CS_CMD_ACTOR_CUE_464,
    /* 0x1D1 */ CS_CMD_ACTOR_CUE_465,
    /* 0x1D2 */ CS_CMD_ACTOR_CUE_466,
    /* 0x1D3 */ CS_CMD_ACTOR_CUE_467,
    /* 0x1D4 */ CS_CMD_ACTOR_CUE_468,
    /* 0x1D5 */ CS_CMD_ACTOR_CUE_469,
    /* 0x1D6 */ CS_CMD_ACTOR_CUE_470,
    /* 0x1D7 */ CS_CMD_ACTOR_CUE_471,
    /* 0x1D8 */ CS_CMD_ACTOR_CUE_472,
    /* 0x1D9 */ CS_CMD_ACTOR_CUE_473,
    /* 0x1DA */ CS_CMD_ACTOR_CUE_474,
    /* 0x1DB */ CS_CMD_ACTOR_CUE_475,
    /* 0x1DC */ CS_CMD_ACTOR_CUE_476,
    /* 0x1DD */ CS_CMD_ACTOR_CUE_477,
    /* 0x1DE */ CS_CMD_ACTOR_CUE_478,
    /* 0x1DF */ CS_CMD_ACTOR_CUE_479,
    /* 0x1E0 */ CS_CMD_ACTOR_CUE_480,
    /* 0x1E1 */ CS_CMD_ACTOR_CUE_481,
    /* 0x1E2 */ CS_CMD_ACTOR_CUE_482,
    /* 0x1E3 */ CS_CMD_ACTOR_CUE_483,
    /* 0x1E4 */ CS_CMD_ACTOR_CUE_484,
    /* 0x1E5 */ CS_CMD_ACTOR_CUE_485,
    /* 0x1E6 */ CS_CMD_ACTOR_CUE_486,
    /* 0x1E7 */ CS_CMD_ACTOR_CUE_487,
    /* 0x1E8 */ CS_CMD_ACTOR_CUE_488,
    /* 0x1E9 */ CS_CMD_ACTOR_CUE_489,
    /* 0x1EA */ CS_CMD_ACTOR_CUE_490,
    /* 0x1EB */ CS_CMD_ACTOR_CUE_491,
    /* 0x1EC */ CS_CMD_ACTOR_CUE_492,
    /* 0x1ED */ CS_CMD_ACTOR_CUE_493,
    /* 0x1EE */ CS_CMD_ACTOR_CUE_494,
    /* 0x1EF */ CS_CMD_ACTOR_CUE_495,
    /* 0x1F0 */ CS_CMD_ACTOR_CUE_496,
    /* 0x1F1 */ CS_CMD_ACTOR_CUE_497,
    /* 0x1F2 */ CS_CMD_ACTOR_CUE_498,
    /* 0x1F3 */ CS_CMD_ACTOR_CUE_499,
    /* 0x1F4 */ CS_CMD_ACTOR_CUE_500,
    /* 0x1F5 */ CS_CMD_ACTOR_CUE_501,
    /* 0x1F6 */ CS_CMD_ACTOR_CUE_502,
    /* 0x1F7 */ CS_CMD_ACTOR_CUE_503,
    /* 0x1F8 */ CS_CMD_ACTOR_CUE_504,
    /* 0x1F9 */ CS_CMD_ACTOR_CUE_SOTCS, // Song of Time Cutscenes (Double SoT, Three-Day Reset SoT)
    /* 0x1FA */ CS_CMD_ACTOR_CUE_506,
    /* 0x1FB */ CS_CMD_ACTOR_CUE_507,
    /* 0x1FC */ CS_CMD_ACTOR_CUE_508,
    /* 0x1FD */ CS_CMD_ACTOR_CUE_509,
    /* 0x1FE */ CS_CMD_ACTOR_CUE_510,
    /* 0x1FF */ CS_CMD_ACTOR_CUE_511,
    /* 0x200 */ CS_CMD_ACTOR_CUE_512,
    /* 0x201 */ CS_CMD_ACTOR_CUE_513,
    /* 0x202 */ CS_CMD_ACTOR_CUE_514,
    /* 0x203 */ CS_CMD_ACTOR_CUE_515,
    /* 0x204 */ CS_CMD_ACTOR_CUE_516,
    /* 0x205 */ CS_CMD_ACTOR_CUE_517,
    /* 0x206 */ CS_CMD_ACTOR_CUE_518,
    /* 0x207 */ CS_CMD_ACTOR_CUE_519,
    /* 0x208 */ CS_CMD_ACTOR_CUE_520,
    /* 0x209 */ CS_CMD_ACTOR_CUE_521,
    /* 0x20A */ CS_CMD_ACTOR_CUE_522,
    /* 0x20B */ CS_CMD_ACTOR_CUE_523,
    /* 0x20C */ CS_CMD_ACTOR_CUE_524,
    /* 0x20D */ CS_CMD_ACTOR_CUE_525,
    /* 0x20E */ CS_CMD_ACTOR_CUE_526,
    /* 0x20F */ CS_CMD_ACTOR_CUE_527,
    /* 0x210 */ CS_CMD_ACTOR_CUE_528,
    /* 0x211 */ CS_CMD_ACTOR_CUE_529,
    /* 0x212 */ CS_CMD_ACTOR_CUE_530,
    /* 0x213 */ CS_CMD_ACTOR_CUE_531,
    /* 0x214 */ CS_CMD_ACTOR_CUE_532,
    /* 0x215 */ CS_CMD_ACTOR_CUE_533,
    /* 0x216 */ CS_CMD_ACTOR_CUE_534,
    /* 0x217 */ CS_CMD_ACTOR_CUE_535,
    /* 0x218 */ CS_CMD_ACTOR_CUE_536,
    /* 0x219 */ CS_CMD_ACTOR_CUE_537,
    /* 0x21A */ CS_CMD_ACTOR_CUE_538,
    /* 0x21B */ CS_CMD_ACTOR_CUE_539,
    /* 0x21C */ CS_CMD_ACTOR_CUE_540,
    /* 0x21D */ CS_CMD_ACTOR_CUE_541,
    /* 0x21E */ CS_CMD_ACTOR_CUE_542,
    /* 0x21F */ CS_CMD_ACTOR_CUE_543,
    /* 0x220 */ CS_CMD_ACTOR_CUE_544,
    /* 0x221 */ CS_CMD_ACTOR_CUE_545,
    /* 0x222 */ CS_CMD_ACTOR_CUE_546,
    /* 0x223 */ CS_CMD_ACTOR_CUE_547,
    /* 0x224 */ CS_CMD_ACTOR_CUE_548,
    /* 0x225 */ CS_CMD_ACTOR_CUE_549,
    /* 0x226 */ CS_CMD_ACTOR_CUE_550,
    /* 0x227 */ CS_CMD_ACTOR_CUE_551,
    /* 0x228 */ CS_CMD_ACTOR_CUE_552,
    /* 0x229 */ CS_CMD_ACTOR_CUE_553,
    /* 0x22A */ CS_CMD_ACTOR_CUE_554,
    /* 0x22B */ CS_CMD_ACTOR_CUE_555,
    /* 0x22C */ CS_CMD_ACTOR_CUE_556,
    /* 0x22D */ CS_CMD_ACTOR_CUE_557, // Couple's Mask cs, Anju cues
    /* 0x22E */ CS_CMD_ACTOR_CUE_558,
    /* 0x22F */ CS_CMD_ACTOR_CUE_559,
    /* 0x230 */ CS_CMD_ACTOR_CUE_560,
    /* 0x231 */ CS_CMD_ACTOR_CUE_561,
    /* 0x232 */ CS_CMD_ACTOR_CUE_562,
    /* 0x233 */ CS_CMD_ACTOR_CUE_563,
    /* 0x234 */ CS_CMD_ACTOR_CUE_564,
    /* 0x235 */ CS_CMD_ACTOR_CUE_565,
    /* 0x236 */ CS_CMD_ACTOR_CUE_566,
    /* 0x237 */ CS_CMD_ACTOR_CUE_567,
    /* 0x238 */ CS_CMD_ACTOR_CUE_568,
    /* 0x239 */ CS_CMD_ACTOR_CUE_569,
    /* 0x23A */ CS_CMD_ACTOR_CUE_570,
    /* 0x23B */ CS_CMD_ACTOR_CUE_571,
    /* 0x23C */ CS_CMD_ACTOR_CUE_572,
    /* 0x23D */ CS_CMD_ACTOR_CUE_573,
    /* 0x23E */ CS_CMD_ACTOR_CUE_574,
    /* 0x23F */ CS_CMD_ACTOR_CUE_575,
    /* 0x240 */ CS_CMD_ACTOR_CUE_576,
    /* 0x241 */ CS_CMD_ACTOR_CUE_577,
    /* 0x242 */ CS_CMD_ACTOR_CUE_578,
    /* 0x243 */ CS_CMD_ACTOR_CUE_579,
    /* 0x244 */ CS_CMD_ACTOR_CUE_580,
    /* 0x245 */ CS_CMD_ACTOR_CUE_581,
    /* 0x246 */ CS_CMD_ACTOR_CUE_582,
    /* 0x247 */ CS_CMD_ACTOR_CUE_583,
    /* 0x248 */ CS_CMD_ACTOR_CUE_584,
    /* 0x249 */ CS_CMD_ACTOR_CUE_585,
    /* 0x24A */ CS_CMD_ACTOR_CUE_586,
    /* 0x24B */ CS_CMD_ACTOR_CUE_587,
    /* 0x24C */ CS_CMD_ACTOR_CUE_588,
    /* 0x24D */ CS_CMD_ACTOR_CUE_589,
    /* 0x24E */ CS_CMD_ACTOR_CUE_590,
    /* 0x24F */ CS_CMD_ACTOR_CUE_591,
    /* 0x250 */ CS_CMD_ACTOR_CUE_592,
    /* 0x251 */ CS_CMD_ACTOR_CUE_593,
    /* 0x252 */ CS_CMD_ACTOR_CUE_594,
    /* 0x253 */ CS_CMD_ACTOR_CUE_595,
    /* 0x254 */ CS_CMD_ACTOR_CUE_596,
    /* 0x255 */ CS_CMD_ACTOR_CUE_597,
    /* 0x256 */ CS_CMD_ACTOR_CUE_598,
    /* 0x257 */ CS_CMD_ACTOR_CUE_599,
    /*    -2 */ CS_CMD_ACTOR_CUE_POST_PROCESS = 0xFFFFFFFE,
    /*    -1 */ CS_CAM_STOP // OoT Remnant
} CutsceneCmd;

typedef enum {
    /* 0x00 */ CS_MISC_UNIMPLEMENTED_0,
    /* 0x01 */ CS_MISC_RAIN,
    /* 0x02 */ CS_MISC_LIGHTNING,
    /* 0x03 */ CS_MISC_LIFT_FOG,
    /* 0x04 */ CS_MISC_CLOUDY_SKY,
    /* 0x05 */ CS_MISC_STOP_CUTSCENE,
    /* 0x06 */ CS_MISC_UNIMPLEMENTED_6,
    /* 0x07 */ CS_MISC_SHOW_TITLE_CARD,
    /* 0x08 */ CS_MISC_EARTHQUAKE_MEDIUM,
    /* 0x09 */ CS_MISC_EARTHQUAKE_STOP,
    /* 0x0A */ CS_MISC_VISMONO_BLACK_AND_WHITE,
    /* 0x0B */ CS_MISC_VISMONO_SEPIA,
    /* 0x0C */ CS_MISC_HIDE_ROOM,
    /* 0x0D */ CS_MISC_RED_PULSATING_LIGHTS,
    /* 0x0E */ CS_MISC_HALT_ALL_ACTORS,
    /* 0x0F */ CS_MISC_RESUME_ALL_ACTORS,
    /* 0x10 */ CS_MISC_SANDSTORM_FILL,
    /* 0x11 */ CS_MISC_SUNSSONG_START,
    /* 0x12 */ CS_MISC_FREEZE_TIME,
    /* 0x13 */ CS_MISC_LONG_SCARECROW_SONG,
    /* 0x14 */ CS_MISC_SET_CSFLAG_3,
    /* 0x15 */ CS_MISC_SET_CSFLAG_4,
    /* 0x16 */ CS_MISC_PLAYER_FORM_DEKU,
    /* 0x17 */ CS_MISC_ENABLE_PLAYER_REFLECTION,
    /* 0x18 */ CS_MISC_DISABLE_PLAYER_REFLECTION,
    /* 0x19 */ CS_MISC_PLAYER_FORM_HUMAN,
    /* 0x1A */ CS_MISC_EARTHQUAKE_STRONG,
    /* 0x1B */ CS_MISC_DEST_MOON_CRASH_FIRE_WALL,
    /* 0x1C */ CS_MISC_MOON_CRASH_SKYBOX,
    /* 0x1D */ CS_MISC_PLAYER_FORM_RESTORED,
    /* 0x1E */ CS_MISC_DISABLE_PLAYER_CSACTION_START_POS,
    /* 0x1F */ CS_MISC_ENABLE_PLAYER_CSACTION_START_POS,
    /* 0x20 */ CS_MISC_UNIMPLEMENTED_20,
    /* 0x21 */ CS_MISC_SAVE_ENTER_CLOCK_TOWN,
    /* 0x22 */ CS_MISC_RESET_SAVE_FROM_MOON_CRASH,
    /* 0x23 */ CS_MISC_TIME_ADVANCE,
    /* 0x24 */ CS_MISC_EARTHQUAKE_WEAK,
    /* 0x25 */ CS_MISC_UNIMPLEMENTED_25,
    /* 0x26 */ CS_MISC_DAWN_OF_A_NEW_DAY,
    /* 0x27 */ CS_MISC_PLAYER_FORM_ZORA,
    /* 0x28 */ CS_MISC_FINALE
} CutsceneMiscType;

typedef union CutsceneData {
    s32 i;
    f32 f;
    s16 s[2];
    s8  b[4];
} CutsceneData;

// Do not trigger the scripted cutscene upon any spawn
#define CS_SPAWN_FLAG_NONE 0xFF
// Always trigger the scripted cutscene upon the specified spawn
#define CS_SPAWN_FLAG_ALWAYS 0xFE
// Trigger the scripted cutscene once upon the specified spawn if `weekEventFlags` is not set. Set `weekEventFlags` after the cutscene.
//! @note Despite there being more than `0x1F` indices for weekEventFlags, this u8 flag entry can only check weekEventFlags with indices [0 - 0x1F]
#define CS_SPAWN_FLAG_ONCE(weekEventFlag) ((((weekEventFlag) & 0x1F00) >> 5) | BIT_FLAG_TO_SHIFT((weekEventFlag) & 0xFF))

#define CHECK_CS_SPAWN_FLAG_WEEKEVENTREG(spawnFlags) (GET_WEEKEVENTREG((spawnFlags) / 8) & (1 << ((spawnFlags) % 8)))
#define SET_CS_SPAWN_FLAG_WEEKEVENTREG(spawnFlags) (WEEKEVENTREG((spawnFlags) / 8) = GET_WEEKEVENTREG((spawnFlags) / 8) | (1 << ((spawnFlags) % 8)))


typedef struct {
    /* 0x0 */ CutsceneData* script;
    /* 0x4 */ s16 nextEntrance;
    /* 0x6 */ u8 spawn;
    /* 0x7 */ u8 spawnFlags; // See `CS_SPAWN_FLAG_`
} CutsceneScriptEntry; // size = 0x8

// ZAPD compatibility typedefs
// TODO: Remove when ZAPD adds support for them
typedef CutsceneScriptEntry CutsceneEntry;

typedef struct {
    /* 0x00 */ u8 scriptListCount;
    /* 0x04 */ CutsceneData* script;
    /* 0x08 */ u8 state;
    /* 0x0C */ f32 timer;
    /* 0x10 */ u16 curFrame;
    /* 0x12 */ u16 scriptIndex;
    /* 0x14 */ s32 subCamId;
    /* 0x18 */ u16 camEyeSplinePointsAppliedFrame; // Remnant of OoT. Set but never used.
    /* 0x1C */ UNK_TYPE1 unk_1C[0xA]; // Remnant of cam data from OoT
    /* 0x24 */ CsCmdActorCue* playerCue;
    /* 0x28 */ CsCmdActorCue* actorCues[10]; // "npcdemopnt"
    /* 0x50 */ CutsceneScriptEntry* scriptList;
} CutsceneContext; // size = 0x54

/* Actor Cutscenes, which encompasses all cutscenes */

typedef struct {
    /* 0x0 */ s16 priority; // Lower means higher priority. -1 means it ignores priority
    /* 0x2 */ s16 length;
    /* 0x4 */ s16 csCamId; // Index of CsCameraEntry to use. Negative indices use sGlobalCamDataSettings. Indices 0 and above use CsCameraEntry from a sceneLayer
    /* 0x6 */ s16 scriptIndex;
    /* 0x8 */ s16 additionalCsId;
    /* 0xA */ u8 endSfx;
    /* 0xB */ u8 customValue; // 0 - 99: actor-specific custom value. 100+: spawn. 255: none
    /* 0xC */ s16 hudVisibility; 
    /* 0xE */ u8 endCam;
    /* 0xF */ u8 letterboxSize;
} ActorCutscene; // size = 0x10
// TODO: rename `ActorCutscene` to `CutsceneEntry` once ZAPD uses `CutsceneScriptEntry`
// typedef CutsceneEntry ActorCutscene;

typedef enum {
    /*   -1 */ CS_ID_NONE = -1,
    // CsId's 0 - 119 are sceneLayer-specific and index `ActorCutscene`
    /* 0x78 */ CS_ID_GLOBAL_78 = 120,
    /* 0x79 */ CS_ID_GLOBAL_79,
    /* 0x7A */ CS_ID_GLOBAL_7A,
    /* 0x7B */ CS_ID_GLOBAL_ELEGY,
    /* 0x7C */ CS_ID_GLOBAL_TALK,
    /* 0x7D */ CS_ID_GLOBAL_DOOR,
    /* 0x7E */ CS_ID_GLOBAL_RETURN_TO_CAM, // smoothly return to the previous camera
    /* 0x7F */ CS_ID_GLOBAL_END
} CutsceneId;

typedef enum {
    /*  0 */ PLAYER_CS_ID_ITEM_OCARINA,
    /*  1 */ PLAYER_CS_ID_ITEM_GET,
    /*  2 */ PLAYER_CS_ID_ITEM_BOTTLE,
    /*  3 */ PLAYER_CS_ID_ITEM_SHOW,
    /*  4 */ PLAYER_CS_ID_WARP_PAD_MOON,
    /*  5 */ PLAYER_CS_ID_MASK_TRANSFORMATION,
    /*  6 */ PLAYER_CS_ID_DEATH,
    /*  7 */ PLAYER_CS_ID_REVIVE,
    /*  8 */ PLAYER_CS_ID_SONG_WARP, // Song of Time and Song of Soaring
    /*  9 */ PLAYER_CS_ID_WARP_PAD_ENTRANCE,
    /* 10 */ PLAYER_CS_ID_MAX
} PlayerCsId;

#define CS_SCRIPT_ID_NONE -1

typedef enum {
    // global (see sGlobalCamDataSettings)
    /* -25 */ CS_CAM_ID_GLOBAL_ELEGY = -25,         // CAM_SET_ELEGY_SHELL
    /* -24 */ CS_CAM_ID_GLOBAL_SIDED,               // CAM_SET_SIDED
    /* -23 */ CS_CAM_ID_GLOBAL_BOAT_CRUISE,         // CAM_SET_BOAT_CRUISE
    /* -22 */ CS_CAM_ID_GLOBAL_N16,                 // CAM_SET_NONE
    /* -21 */ CS_CAM_ID_GLOBAL_SUBJECTD,            // CAM_SET_SUBJECTD
    /* -20 */ CS_CAM_ID_GLOBAL_NORMALD,             // CAM_SET_NORMALD
    /* -19 */ CS_CAM_ID_GLOBAL_N13,                 // CAM_SET_NONE
    /* -18 */ CS_CAM_ID_GLOBAL_N12,                 // CAM_SET_NONE
    /* -17 */ CS_CAM_ID_GLOBAL_N11,                 // CAM_SET_NONE
    /* -16 */ CS_CAM_ID_GLOBAL_WARP_PAD_ENTRANCE,   // CAM_SET_WARP_PAD_ENTRANCE
    /* -15 */ CS_CAM_ID_GLOBAL_ATTENTION,           // CAM_SET_ATTENTION
    /* -14 */ CS_CAM_ID_GLOBAL_CONNECT,             // CAM_SET_CONNECT0
    /* -13 */ CS_CAM_ID_GLOBAL_REMOTE_BOMB,         // CAM_SET_REMOTEBOMB
    /* -12 */ CS_CAM_ID_GLOBAL_N0C,                 // CAM_SET_NONE
    /* -11 */ CS_CAM_ID_GLOBAL_MASK_TRANSFORMATION, // CAM_SET_MASK_TRANSFORMATION
    /* -10 */ CS_CAM_ID_GLOBAL_LONG_CHEST_OPENING,  // CAM_SET_LONG_CHEST_OPENING
    /*  -9 */ CS_CAM_ID_GLOBAL_REVIVE,              // CAM_SET_REBIRTH
    /*  -8 */ CS_CAM_ID_GLOBAL_DEATH,               // CAM_SET_DEATH
    /*  -7 */ CS_CAM_ID_GLOBAL_WARP_PAD_MOON,       // CAM_SET_WARP_PAD_MOON
    /*  -6 */ CS_CAM_ID_GLOBAL_SONG_WARP,           // CAM_SET_NAVI
    /*  -5 */ CS_CAM_ID_GLOBAL_ITEM_SHOW,           // CAM_SET_ITEM3
    /*  -4 */ CS_CAM_ID_GLOBAL_ITEM_BOTTLE,         // CAM_SET_ITEM2
    /*  -3 */ CS_CAM_ID_GLOBAL_ITEM_OCARINA,        // CAM_SET_ITEM1
    /*  -2 */ CS_CAM_ID_GLOBAL_ITEM_GET,            // CAM_SET_ITEM0
    /*  -1 */ CS_CAM_ID_NONE
    // CamCsId's 0+ are sceneLayer-specific and index `ActorCsCamInfo`
} CutsceneCamId;

typedef enum {
    /* -1 */ CS_HUD_VISIBILITY_ALL_ALT = -1,
    /*  0 */ CS_HUD_VISIBILITY_NONE,
    /*  1 */ CS_HUD_VISIBILITY_ALL,
    /*  2 */ CS_HUD_VISIBILITY_A_HEARTS_MAGIC,
    /*  3 */ CS_HUD_VISIBILITY_C_HEARTS_MAGIC,
    /*  4 */ CS_HUD_VISIBILITY_ALL_NO_MINIMAP,
    /*  5 */ CS_HUD_VISIBILITY_A_B_C,
    /*  6 */ CS_HUD_VISIBILITY_B_MINIMAP,
    /*  7 */ CS_HUD_VISIBILITY_A
} CutsceneHudVisibility;

typedef enum {
    /*   0 */ CS_END_SFX_NONE,
    /*   1 */ CS_END_SFX_TRE_BOX_APPEAR,
    /*   2 */ CS_END_SFX_CORRECT_CHIME,
    /* 255 */ CS_END_SFX_NONE_ALT = 0xFF
} CutsceneEndSfx;

typedef enum {
    /* 0 */ CS_END_CAM_0,
    /* 1 */ CS_END_CAM_1,
    /* 2 */ CS_END_CAM_SMOOTH
} CutsceneEndCam;

/* Cutscene Camera Spline Scripts */

typedef struct {
    /* 0x0 */ s16 numEntries;
    /* 0x2 */ s16 unk_02; // unused
    /* 0x4 */ s16 unk_04; // unused
    /* 0x6 */ s16 duration; // total duration
} CsCmdCamSpline; // size = 0x8

// Both camAt and camEye
typedef struct {
    /* 0x0 */ u8 interpType; // see `CutsceneCamInterpType`
    /* 0x1 */ u8 weight; // for certain types of interpTypes, shifts the weight to certain points. Default is 100.
    /* 0x2 */ s16 duration; // duration of current point
    /* 0x4 */ Vec3s pos;
    /* 0xA */ s16 relativeTo; // see `CutsceneCamRelativeTo`
} CsCmdCamPoint; // size = 0xC

typedef enum CutsceneCamInterpType {
    /* 0 */ CS_CAM_INTERP_NONE, // values do not change.
    // values 1-3 only uses a single point from the cmd
    /* 1 */ CS_CAM_INTERP_SET, // values immediately set to cmd values.
    /* 2 */ CS_CAM_INTERP_LINEAR, // Lerp to the target position
    /* 3 */ CS_CAM_INTERP_SCALE, // Step to the target position in increments scaled by the remaining distance
    // values 4-5 uses multiple points from the cmd
    /* 4 */ CS_CAM_INTERP_MP_CUBIC, // cubic multi-point (identical to SM64/OoT)
    /* 5 */ CS_CAM_INTERP_MP_QUAD, // quadratic multi-point
    // value 6 only uses a single point from the cmd
    /* 6 */ CS_CAM_INTERP_GEO, // does VecGeo calculations using fov
    /* 7 */ CS_CAM_INTERP_OFF // interpolation is not processed.
} CutsceneCamInterpType;

typedef enum CutsceneCamRelativeTo {
    /* 0 */ CS_CAM_REL_0,
    /* 1 */ CS_CAM_REL_1,
    /* 2 */ CS_CAM_REL_2,
    /* 3 */ CS_CAM_REL_3,
    /* 4 */ CS_CAM_REL_4,
    /* 5 */ CS_CAM_REL_5
} CutsceneCamRelativeTo;


// Roll and Fov Data
typedef struct CsCmdCamMisc {
    /* 0x0 */ s16 unused0; // used only in the unused interp function
    /* 0x2 */ s16 roll;
    /* 0x4 */ s16 fov;
    /* 0x6 */ s16 unused1; // unused
} CsCmdCamMisc; // size = 0x8

typedef struct CutsceneCameraInterp {
    /* 0x00 */ Vec3f curPos;
    /* 0x0C */ Vec3f initPos;
    /* 0x18 */ f32 initFov;
    /* 0x1C */ f32 initRoll;
    /* 0x2A */ f32 unk_20; // position adjustment based on fov?
    /* 0x24 */ s16 curFrame;
    /* 0x26 */ s16 waypoint;
    /* 0x28 */ s16 duration;
    /* 0x2A */ s16 numEntries;
    /* 0x1E */ u8 curPoint;
    /* 0x2D */ u8 type; // See `CutsceneCamInterpType`
} CutsceneCameraInterp; // size = 0x30

typedef struct CutsceneCamera {
    /* 0x00 */ s16 splineIndex;
    /* 0x02 */ s16 cmdIndex;
    /* 0x04 */ s16 splineNeedsInit;
    /* 0x06 */ s16 state;
    /* 0x08 */ s16 nextSplineTimer;
    /* 0x0A */ s16 updateSplineTimer;
    /* 0x0C */ s16 duration; // Duration of the current spline
    /* 0x10 */ CutsceneCameraInterp eyeInterp;
    /* 0x40 */ CutsceneCameraInterp atInterp;
    /* 0x70 */ CsCmdCamPoint* atCmd;
    /* 0x74 */ CsCmdCamPoint* eyeCmd;
    /* 0x78 */ CsCmdCamMisc* miscCmd;
    /* 0x7C */ Camera* camera;
} CutsceneCamera; // size = 0x80

typedef enum {
    /*   0 */ CS_CAM_STATE_UPDATE_ALL, // Update spline and next spline timer
    /*   1 */ CS_CAM_STATE_UPDATE_SPLINE, // Update spline, do not advance next spline timer
    /*   2 */ CS_CAM_STATE_PAUSE, // No updates
    /*   3 */ CS_CAM_STATE_DONE_SPLINE, // Finished the current spline, ready for the next one
    /* 999 */ CS_CAM_STATE_DONE = 999 // Finished all the splines.
} CutsceneCameraState;

// OoT Remnant
#define CS_CAM_DATA_NOT_APPLIED 0xFFFF

void Cutscene_InitContext(struct PlayState* play, CutsceneContext* csCtx);
void Cutscene_StartManual(struct PlayState* play, CutsceneContext* csCtx);
void Cutscene_StopManual(struct PlayState* play, CutsceneContext* csCtx);
void Cutscene_UpdateManual(struct PlayState* play, CutsceneContext* csCtx);
void Cutscene_UpdateScripted(struct PlayState* play, CutsceneContext* csCtx);
void Cutscene_HandleEntranceTriggers(struct PlayState* play);
void func_800EDDB0(struct PlayState* play);
void Cutscene_StartScripted(struct PlayState* play, u8 scriptIndex);
void Cutscene_ActorTranslate(Actor* actor, struct PlayState* play, s32 cueChannel);
void Cutscene_ActorTranslateAndYaw(Actor* actor, struct PlayState* play, s32 cueChannel);
void Cutscene_ActorTranslateAndYawSmooth(Actor* actor, struct PlayState* play, s32 cueChannel);
void Cutscene_ActorTranslateXZAndYawSmooth(Actor* actor, struct PlayState* play, s32 cueChannel);
s32 Cutscene_GetSceneLayer(struct PlayState* play);
s32 Cutscene_GetCueChannel(struct PlayState* play, u16 cueType);
s32 Cutscene_IsCueInChannel(struct PlayState* play, u16 cueType);
u8 Cutscene_IsPlaying(struct PlayState* play);

void CutsceneManager_Init(struct PlayState* play, ActorCutscene* cutsceneList, s16 numEntries);
void CutsceneManager_StoreCamera(Camera* camera);
void CutsceneManager_ClearWaiting(void);
s16 CutsceneManager_Update(void);
void CutsceneManager_Queue(s16 csId);
s16 CutsceneManager_IsNext(s16 csId);
s16 CutsceneManager_StartWithPlayerCs(s16 csId, Actor* actor);
s16 CutsceneManager_StartWithPlayerCsAndSetFlag(s16 csId, Actor* actor);
s16 CutsceneManager_Start(s16 csId, Actor* actor);
s16 CutsceneManager_Stop(s16 csId);
s16 CutsceneManager_GetCurrentCsId(void);
ActorCutscene* CutsceneManager_GetCutsceneEntry(s16 csId);
s16 CutsceneManager_GetAdditionalCsId(s16 csId);
s16 CutsceneManager_GetLength(s16 csId);
s16 CutsceneManager_GetCutsceneScriptIndex(s16 csId);
s16 CutsceneManager_GetCutsceneCustomValue(s16 csId);
s16 CutsceneManager_GetCurrentSubCamId(s16 csId);
s16 CutsceneManager_FindEntranceCsId(void);
s32 func_800F22C4(s16 csId, Actor* actor);
void CutsceneManager_SetReturnCamera(s16 camId);

s32 CutsceneCamera_Init(Camera* camera, CutsceneCamera* csCamera);
s32 CutsceneCamera_UpdateSplines(u8* script, CutsceneCamera* csCamera);
void CutsceneCamera_SetState(s16 state);
void CutsceneCamera_Reset(void);

void CutsceneFlags_UnsetAll(struct PlayState* play);
void CutsceneFlags_Set(struct PlayState* play, s16 flag);
void CutsceneFlags_Unset(struct PlayState* play, s16 flag);
s32 CutsceneFlags_Get(struct PlayState* play, s16 flag);

extern u8 gOpeningEntranceIndex;

extern u8 D_801F4DDC;
extern u8 gDisablePlayerCsActionStartPos;
extern s16 gDungeonBossWarpSceneId;

#endif
