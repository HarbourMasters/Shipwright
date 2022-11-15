#ifndef Z_EN_DOOR_H
#define Z_EN_DOOR_H

#include "ultra64.h"
#include "global.h"

/**
 * Actor Parameters
 *
 * |                  |       |             |
 * | Transition Index | Type  | Double Door | Switch Flag OR Text Id - 0x0200
 * |------------------|-------|-------------|---------------------------------
 * | 0 0 0 0 0 0      | 0 0 0 | 0           | 0 0 0 0 0 0
 * | 6                | 3     | 1           | 6
 * |
 *
 * Transition Index     1111110000000000    Set by the actor engine when the door is spawned
 * Type                 0000001110000000
 * Double Door          0000000001000000
 * Switch Flag          0000000000111111    For use with the `DOOR_LOCKED` type
 * Text id - 0x0200     0000000000111111    For use with the `DOOR_CHECKABLE` type
 *
 */

typedef struct {
    /* 0x00 */ s16 sceneNum;
    /* 0x02 */ u8 dListIndex;
    /* 0x04 */ s16 objectId;
} EnDoorInfo;

typedef enum {
    /* 0x00 */ DOOR_ROOMLOAD,  // loads rooms
    /* 0x01 */ DOOR_LOCKED,    // small key locked door
    /* 0x02 */ DOOR_ROOMLOAD2, // loads rooms
    /* 0x03 */ DOOR_SCENEEXIT, // doesn't load rooms, used for doors paired with scene transition polygons
    /* 0x04 */ DOOR_AJAR,      // open slightly but slams shut if Link gets too close
    /* 0x05 */ DOOR_CHECKABLE, // doors that display a textbox when interacting
    /* 0x06 */ DOOR_EVENING,   // unlocked between 18:00 and 21:00, Dampé's hut
    /* 0x07 */ DOOR_ROOMLOAD7  // loads rooms
} EnDoorType;

struct EnDoor;

typedef void (*EnDoorActionFunc)(struct EnDoor*, PlayState*);

typedef struct EnDoor {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ u8 animStyle; // Must be at same offset as animStyle in DoorKiller due to the cast in func_80839800
    /* 0x0191 */ u8 playerIsOpening; // Must be at same offset as playerIsOpening in DoorKiller due to the cast in func_80839800
    /* 0x0192 */ u8 unk_192;
    /* 0x0193 */ s8 requiredObjBankIndex;
    /* 0x0194 */ s8 dListIndex;
    /* 0x0196 */ s16 lockTimer;
    /* 0x0198 */ Vec3s jointTable[5];
    /* 0x01B6 */ Vec3s morphTable[5];
    /* 0x01D4 */ EnDoorActionFunc actionFunc;
} EnDoor; // size = 0x01D8

#endif
