#ifndef Z_OBJ_ETCETERA_H
#define Z_OBJ_ETCETERA_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define DEKU_FLOWER_TYPE(thisx) (((thisx)->params & 0xFF80) >> 7)
#define DEKU_FLOWER_PARAMS(type) (((type) << 7) & 0xFF80)

typedef enum {
    /* 0 */ DEKU_FLOWER_TYPE_PINK,
    /* 1 */ DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE,
    /* 2 */ DEKU_FLOWER_TYPE_GOLD,
    /* 3 */ DEKU_FLOWER_TYPE_GOLD_WITH_INITIAL_BOUNCE,
    /* 4 */ DEKU_FLOWER_TYPE_MAX
} DekuFlowerType;

struct ObjEtcetera;

typedef void (*ObjEtceteraActionFunc)(struct ObjEtcetera*, PlayState*);

typedef struct ObjEtcetera {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ ColliderCylinder collider;
    /* 0x1EC */ Vec3s jointTable[PINK_DEKU_FLOWER_LIMB_MAX];
    /* 0x22E */ Vec3s morphTable[PINK_DEKU_FLOWER_LIMB_MAX];
    /* 0x270 */ f32 bounceOscillationScale;
    /* 0x274 */ s16 oscillationTimer;
    /* 0x276 */ u16 burrowFlag;
    /* 0x278 */ s8 objectSlot;
    /* 0x27C */ Gfx* dList;
    /* 0x280 */ ObjEtceteraActionFunc actionFunc;
} ObjEtcetera; // size = 0x284

#endif // Z_OBJ_ETCETERA_H
