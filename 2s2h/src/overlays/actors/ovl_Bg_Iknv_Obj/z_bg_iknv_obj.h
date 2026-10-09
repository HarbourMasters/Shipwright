#ifndef Z_BG_IKNV_OBJ_H
#define Z_BG_IKNV_OBJ_H

#include "global.h"

struct BgIknvObj;

typedef void (*BgIknvObjActionFunc)(struct BgIknvObj*, PlayState*);

#define IKNV_OBJ_TYPE(x) (x->dyna.actor.params & 0xF)

typedef struct BgIknvObj {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ColliderCylinder collider;
    /* 0x1A8 */ Gfx* displayListPtr;
    /* 0x1AC */ BgIknvObjActionFunc actionFunc;
} BgIknvObj; // size = 0x1B0

typedef enum {
    /* 0 */ IKNV_OBJ_WATERWHEEL,
    /* 1 */ IKNV_OBJ_RAISED_DOOR, // defunct door covering entrance to Stone Tower
    /* 2 */ IKNV_OBJ_SAKON_DOOR   // door to Sakon's Hideout
} BgIknvObjType;

#endif // Z_BG_IKNV_OBJ_H
