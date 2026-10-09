#ifndef Z_EN_WOOD02_H
#define Z_EN_WOOD02_H

#include "global.h"

struct EnWood02;

#define ENWOOD02_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef struct EnWood02 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s16 unk_144;
    /* 0x146 */ s16 unk_146;
    /* 0x148 */ s16 unk_148;
    /* 0x14A */ u8 unk_14A[5];
    /* 0x14F */ u8 spawnType;
    /* 0x150 */ u8 drawType;
    /* 0x151 */ u8 unk_151;
    /* 0x154 */ ColliderCylinder collider;
} EnWood02; // size = 0x1A0

// Types with SPAWNED in the name are those that can be managed by a spawner, however the actor allows you to spawn them
// on their own without a spawner as well.
typedef enum {
    /* 0x00 */ WOOD_TREE_CONICAL_LARGE,
    /* 0x01 */ WOOD_TREE_CONICAL_MEDIUM,
    /* 0x02 */ WOOD_TREE_CONICAL_SMALL,
    /* 0x03 */ WOOD_TREE_CONICAL_SPAWNER,
    /* 0x04 */ WOOD_TREE_CONICAL_SPAWNED,
    /* 0x05 */ WOOD_TREE_OVAL_GREEN,
    /* 0x06 */ WOOD_TREE_OVAL_YELLOW_SPAWNER,
    /* 0x07 */ WOOD_TREE_OVAL_YELLOW_SPAWNED,
    /* 0x08 */ WOOD_TREE_OVAL_GREEN_SPAWNER,
    /* 0x09 */ WOOD_TREE_OVAL_GREEN_SPAWNED,
    /* 0x0A */ WOOD_TREE_KAKARIKO_ADULT,
    /* 0x0B */ WOOD_BUSH_GREEN_SMALL,
    /* 0x0C */ WOOD_BUSH_GREEN_LARGE,
    /* 0x0D */ WOOD_BUSH_GREEN_SMALL_SPAWNER,
    /* 0x0E */ WOOD_BUSH_GREEN_SMALL_SPAWNED,
    /* 0x0F */ WOOD_BUSH_GREEN_LARGE_SPAWNER,
    /* 0x10 */ WOOD_BUSH_GREEN_LARGE_SPAWNED,
    /* 0x11 */ WOOD_BUSH_BLACK_SMALL,
    /* 0x12 */ WOOD_BUSH_BLACK_LARGE,
    /* 0x13 */ WOOD_BUSH_BLACK_SMALL_SPAWNER,
    /* 0x14 */ WOOD_BUSH_BLACK_SMALL_SPAWNED,
    /* 0x15 */ WOOD_BUSH_BLACK_LARGE_SPAWNER,
    /* 0x16 */ WOOD_BUSH_BLACK_LARGE_SPAWNED,
    /* 0x17 */ WOOD_LEAF_GREEN,
    /* 0x18 */ WOOD_LEAF_YELLOW,
    /* 0x1A */ WOOD_TREE_SPECIAL = 0x1A
} WoodType;

#endif // Z_EN_WOOD02_H
