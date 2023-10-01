#ifndef Z_EN_XC_H
#define Z_EN_XC_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnXc;

typedef void (*EnXcActionFunc)(struct EnXc*, PlayState*);
typedef void (*EnXcDrawFunc)(struct Actor*, PlayState*);

typedef enum {
    /*-1 */ SHEIK_TYPE_RANDO = -1,
    /* 0 */ SHEIK_TYPE_0,
    /* 1 */ SHEIK_TYPE_1,
    /* 2 */ SHEIK_TYPE_2,
    /* 3 */ SHEIK_TYPE_3,
    /* 4 */ SHEIK_TYPE_4,
    /* 5 */ SHEIK_TYPE_5,
    /* 6 */ SHEIK_TYPE_MINUET,
    /* 7 */ SHEIK_TYPE_BOLERO,
    /* 8 */ SHEIK_TYPE_SERENADE,
    /* 9 */ SHEIK_TYPE_9
} EnXcType;

typedef enum {
    /* 0 */ SHEIK_DRAW_NOTHING,
    /* 1 */ SHEIK_DRAW_DEFAULT,
    /* 2 */ SHEIK_DRAW_PULLING_OUT_HARP,
    /* 3 */ SHEIK_DRAW_HARP,
    /* 4 */ SHEIK_DRAW_TRIFORCE,
    /* 5 */ SHEIK_DRAW_SQUINT
} EnXcDrawMode;

typedef enum {
    /* 00 */ SHEIK_ACTION_INIT,
    /* 01 */ SHEIK_ACTION_WAIT,
    /* 02 */ SHEIK_ACTION_GRACEFUL_FALL,
    /* 03 */ SHEIK_ACTION_ACCEL,
    /* 04 */ SHEIK_ACTION_WALK,
    /* 05 */ SHEIK_ACTION_HALT,
    /* 06 */ SHEIK_ACTION_STOPPED,
    /* 07 */ SHEIK_ACTION_7,
    /* 08 */ SHEIK_ACTION_HARP_READY,
    /* 09 */ SHEIK_PLAYING_HARP,
    /* 10 */ SHEIK_ACTION_10,
    /* 11 */ SHEIK_ACTION_PUT_HARP_AWAY,
    /* 12 */ SHEIK_ACTION_12,
    /* 13 */ SHEIK_ACTION_13,
    /* 14 */ SHEIK_ACTION_REVERSE_ACCEL,
    /* 15 */ SHEIK_ACTION_REVERSE_WALK,
    /* 16 */ SHEIK_ACTION_REVERSE_HALT,
    /* 17 */ SHEIK_ACTION_THROW_NUT,
    /* 18 */ SHEIK_ACTION_DELETE,
    /* 19 */ SHEIK_ACTION_FADE,
    /* 20 */ SHEIK_ACTION_20,
    /* 21 */ SHEIK_ACTION_21,
    /* 22 */ SHEIK_ACTION_22,
    /* 23 */ SHEIK_ACTION_23,
    /* 24 */ SHEIK_ACTION_24,
    /* 25 */ SHEIK_ACTION_25,
    /* 26 */ SHEIK_ACTION_26,
    /* 27 */ SHEIK_ACTION_27,
    /* 28 */ SHEIK_ACTION_28,
    /* 29 */ SHEIK_ACTION_SERENADE,
    /* 30 */ SHEIK_ACTION_30,
    /* 31 */ SHEIK_ACTION_31,
    /* 32 */ SHEIK_ACTION_32,
    /* 33 */ SHEIK_ACTION_33,
    /* 34 */ SHEIK_ACTION_34,
    /* 35 */ SHEIK_ACTION_35,
    /* 36 */ SHEIK_ACTION_36,
    /* 37 */ SHEIK_ACTION_37,
    /* 38 */ SHEIK_ACTION_38,
    /* 39 */ SHEIK_ACTION_39,
    /* 40 */ SHEIK_ACTION_40,
    /* 41 */ SHEIK_ACTION_41,
    /* 42 */ SHEIK_ACTION_42,
    /* 43 */ SHEIK_ACTION_43,
    /* 44 */ SHEIK_ACTION_44,
    /* 45 */ SHEIK_ACTION_45,
    /* 46 */ SHEIK_ACTION_46,
    /* 47 */ SHEIK_ACTION_47,
    /* 48 */ SHEIK_ACTION_48,
    /* 49 */ SHEIK_ACTION_49,
    /* 50 */ SHEIK_ACTION_KNEEL,
    /* 51 */ SHEIK_ACTION_51,
    /* 52 */ SHEIK_ACTION_52,
    /* 53 */ SHEIK_ACTION_53,
    /* 54 */ SHEIK_ACTION_54,
    /* 55 */ SHEIK_ACTION_SHOW_TRIFORCE,
    /* 56 */ SHEIK_ACTION_SHOW_TRIFORCE_IDLE,
    /* 57 */ SHEIK_ACTION_NOCTURNE_INIT,
    /* 58 */ SHEIK_ACTION_NOCTURNE_IDLE,
    /* 59 */ SHEIK_ACTION_DEFENSE_STANCE,
    /* 60 */ SHEIK_ACTION_CONTORT,
    /* 61 */ SHEIK_ACTION_NOCTURNE_FALL,
    /* 62 */ SHEIK_ACTION_NOCTURNE_HIT_GROUND,
    /* 63 */ SHEIK_ACTION_63,
    /* 64 */ SHEIK_ACTION_NOCTURNE_KNEEL,
    /* 65 */ SHEIK_ACTION_65,
    /* 66 */ SHEIK_ACTION_66,
    /* 67 */ SHEIK_ACTION_67,
    /* 68 */ SHEIK_ACTION_68,
    /* 69 */ SHEIK_ACTION_69,
    /* 70 */ SHEIK_ACTION_70,
    /* 71 */ SHEIK_ACTION_71,
    /* 72 */ SHEIK_ACTION_72,
    /* 73 */ SHEIK_ACTION_NOCTURNE_REVERSE_ACCEL,
    /* 74 */ SHEIK_ACTION_NOCTURNE_REVERSE_WALK,
    /* 75 */ SHEIK_ACTION_NOCTURNE_REVERSE_HALT,
    /* 76 */ SHEIK_ACTION_NOCTURNE_THROW_NUT,
    /* 77 */ SHEIK_ACTION_77,
    /* 78 */ SHEIK_ACTION_78,
    /* 79 */ SHEIK_ACTION_BLOCK_PEDESTAL,
    /* 80 */ SHEIK_ACTION_IN_DIALOGUE
} EnXcAction;

typedef struct EnXc {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ s16 eyeIdx;
    /* 0x025E */ s16 blinkTimer;
    /* 0x0260 */ s32 action;
    /* 0x0264 */ s32 drawMode;
    /* 0x0268 */ f32 timer;
    /* 0x026C */ s32 unk_26C;
    /* 0x0270 */ s32 unk_270; // some sort of flag
    /* 0x0274 */ s32 unk_274;
    /* 0x0278 */ s32 triforcePrimColor[4];
    /* 0x0288 */ s32 triforceEnvColor[4];
    /* 0x0298 */ f32 triforceScale[3];
    /* 0x02A4 */ s16 triforceAngle;
    /* 0x02A8 */ s32 unk_2A8; // sound related
    /* 0x02AC */ s32 unk_2AC; // sound related
    /* 0x02B0 */ Vec3f handPos;
    /* 0x02BC */ s32 unk_2BC; // hand pos related
    /* 0x02C0 */ ColliderCylinder collider;
    /* 0x030C */ s32 unk_30C;
    /* 0x0310 */ Actor* flameActor;
    /* 0x0314 */ NpcInteractInfo interactInfo;
} EnXc; // size = 0x033C

#endif
