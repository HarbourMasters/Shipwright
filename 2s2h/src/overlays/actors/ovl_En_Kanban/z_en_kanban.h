#ifndef Z_EN_KANBAN_H
#define Z_EN_KANBAN_H

#include "global.h"

struct EnKanban;

#define PART_UPPER_SIDE_LEFT (1 << 0)
#define PART_LEFT_SIDE_UPPER (1 << 1)
#define PART_LEFT_SIDE_LOWER (1 << 2)
#define PART_RIGHT_SIDE_UPPER (1 << 3)
#define PART_RIGHT_SIDE_LOWER (1 << 4)
#define PART_LOWER_SIDE_LEFT (1 << 5)
#define PART_UPPER_SIDE_RIGHT (1 << 6)
#define PART_LOWER_SIDE_RIGHT (1 << 7)
#define PART_POST_UPPER (1 << 8)
#define PART_POST_LOWER (1 << 9)
#define PART_POST_STAND (1 << 10)
#define LEFT_HALF (PART_UPPER_SIDE_LEFT | PART_LEFT_SIDE_UPPER | PART_LEFT_SIDE_LOWER | PART_LOWER_SIDE_LEFT)
#define RIGHT_HALF (PART_UPPER_SIDE_RIGHT | PART_RIGHT_SIDE_UPPER | PART_RIGHT_SIDE_LOWER | PART_LOWER_SIDE_RIGHT)
#define UPPER_HALF (PART_POST_UPPER | PART_UPPER_SIDE_RIGHT | PART_RIGHT_SIDE_UPPER | PART_UPPER_SIDE_LEFT | PART_LEFT_SIDE_UPPER)
#define UPPERLEFT_HALF (PART_POST_UPPER | PART_UPPER_SIDE_RIGHT | PART_LEFT_SIDE_LOWER | PART_UPPER_SIDE_LEFT | PART_LEFT_SIDE_UPPER)
#define UPPERRIGHT_HALF (PART_POST_UPPER | PART_UPPER_SIDE_RIGHT | PART_RIGHT_SIDE_UPPER | PART_UPPER_SIDE_LEFT | PART_RIGHT_SIDE_LOWER)
#define ALL_PARTS (LEFT_HALF | RIGHT_HALF | PART_POST_UPPER | PART_POST_LOWER)

typedef enum {
    /* 0 */ ENKANBAN_SIGN,
    /* 1 */ ENKANBAN_AIR,
    /* 2 */ ENKANBAN_UNUSED,
    /* 3 */ ENKANBAN_GROUND,
    /* 4 */ ENKANBAN_WATER,
    /* 5 */ ENKANBAN_REPAIR
} EnKanbanActionState;

typedef enum {
    /*   0 */ PIECE_WHOLE_SIGN,
    /*   1 */ PIECE_UPPER_HALF,
    /*   2 */ PIECE_LOWER_HALF,
    /*   3 */ PIECE_RIGHT_HALF,
    /*   4 */ PIECE_LEFT_HALF,
    /*   5 */ PIECE_2ND_QUAD,
    /*   6 */ PIECE_1ST_QUAD,
    /*   7 */ PIECE_3RD_QUAD,
    /*   8 */ PIECE_4TH_QUAD,
    /*   9 */ PIECE_UPPER_SIDE_LEFT,
    /*  10 */ PIECE_LEFT_SIDE_UPPER,
    /*  11 */ PIECE_LEFT_SIDE_LOWER,
    /*  12 */ PIECE_LOWER_SIDE_LEFT,
    /*  13 */ PIECE_UPPER_SIDE_RIGHT,
    /*  14 */ PIECE_RIGHT_SIDE_UPPER,
    /*  15 */ PIECE_RIGHT_SIDE_LOWER,
    /*  16 */ PIECE_LOWER_SIDE_RIGHT,
    /*  17 */ PIECE_POST_UPPER,
    /*  18 */ PIECE_POST_LOWER,
    /* 100 */ PIECE_OTHER = 100
} EnKanbanPiece;

typedef enum {
    /* 0 */ CUT_POST,
    /* 1 */ CUT_VERT_L,
    /* 2 */ CUT_HORIZ,
    /* 3 */ CUT_DIAG_L, // lower left to upper right
    /* 4 */ CUT_DIAG_R, // upper left to lower right
    /* 5 */ CUT_VERT_R
} EnKanbanCutType;

typedef struct EnKanban {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[4]; // actionFunc?
    /* 0x148 */ u8 frameCount;
    /* 0x14A */ s16 airTimer;
    /* 0x14C */ u8 actionState;
    /* 0x14E */ u16 partFlags;
    /* 0x150 */ u8 partCount;
    /* 0x152 */ s16 invincibilityTimer;
    /* 0x154 */ Vec3f offset;
    /* 0x160 */ Vec3s spinRot;
    /* 0x166 */ Vec3s spinVel;
    /* 0x16C */ s8 spinXFlag;
    /* 0x16D */ s8 spinZFlag;
    /* 0x16E */ s16 bounceX;
    /* 0x170 */ s16 bounceZ;
    /* 0x172 */ u8 bounceCount;
    /* 0x174 */ f32 pieceWidth;
    /* 0x178 */ f32 pieceHeight;
    /* 0x17C */ s16 direction;
    /* 0x180 */ Vec3f floorRot;
    /* 0x18C */ u8 cutType;
    /* 0x18D */ u8 pieceType;
    /* 0x18E */ s16 cutMarkTimer;
    /* 0x190 */ s16 cutMarkAlpha;
    /* 0x192 */ s16 zTargetTimer;
    /* 0x194 */ u8 msgFlag;
    /* 0x195 */ u8 msgTimer;
    /* 0x196 */ u8 ocarinaFlag;
    /* 0x197 */ s8 unk_197;
    /* 0x198 */ s8 unk_198;
    /* 0x199 */ u8 unk_199;
    /* 0x19A */ u8 unk_19A;
    /* 0x19C */ Actor* unk_19C;
    /* 0x1A0 */ s16 unk_1A0;
    /* 0x1A4 */ ColliderCylinder collider;
} EnKanban; // size = 0x1F0

#define ENKANBAN_PIECE ((s16)0xFFDD)
#define ENKANBAN_FISHING 0x300

#endif // Z_EN_KANBAN_H
