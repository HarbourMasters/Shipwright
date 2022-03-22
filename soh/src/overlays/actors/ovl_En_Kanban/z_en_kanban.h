#ifndef Z_EN_KANBAN_H
#define Z_EN_KANBAN_H

#include "ultra64.h"
#include "global.h"

struct EnKanban;

typedef struct EnKanban {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ char unk_14C[4];
    /* 0x0150 */ u8 frameCount;
    /* 0x0152 */ s16 airTimer;
    /* 0x0154 */ u8 actionState;
    /* 0x0156 */ u16 partFlags;
    /* 0x0158 */ u8 partCount;
    /* 0x015A */ s16 invincibilityTimer;
    /* 0x015C */ Vec3f offset;
    /* 0x0168 */ Vec3s spinRot;
    /* 0x016E */ Vec3s spinVel;
    /* 0x0174 */ s8 spinXFlag;
    /* 0x0175 */ s8 spinZFlag;
    /* 0x0176 */ s16 bounceX;
    /* 0x0178 */ s16 bounceZ;
    /* 0x017A */ u8 bounceCount;
    /* 0x017C */ f32 pieceWidth;
    /* 0x0180 */ f32 pieceHeight;
    /* 0x0184 */ s16 direction;
    /* 0x0188 */ Vec3f floorRot;
    /* 0x0194 */ u8 cutType;
    /* 0x0195 */ u8 pieceType;
    /* 0x0196 */ s16 cutMarkTimer;
    /* 0x0198 */ s16 cutMarkAlpha;
    /* 0x019A */ s16 zTargetTimer;
    /* 0x019C */ u8 msgFlag;
    /* 0x019D */ u8 msgTimer;
    /* 0x019E */ u8 ocarinaFlag;
    /* 0x01A0 */ ColliderCylinder collider;
} EnKanban; // size = 0x01EC

#define ENKANBAN_PIECE ((s16)0xFFDD)
#define ENKANBAN_FISHING 0x300

#endif
