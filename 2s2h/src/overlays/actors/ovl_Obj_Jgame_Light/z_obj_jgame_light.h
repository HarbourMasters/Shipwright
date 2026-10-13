#ifndef Z_OBJ_JGAME_LIGHT_H
#define Z_OBJ_JGAME_LIGHT_H

#include "global.h"

struct ObjJgameLight;

#define OBJJGAMELIGHT_GET_7F(thisx) ((thisx)->params & 0x7F)
#define OBJLUPYGAMELIFT_IGNITE_FIRE (1 << 0)
#define OBJLUPYGAMELIFT_DISPLAY_CORRECT (1 << 1)
#define OBJLUPYGAMELIFT_DISPLAY_INCORRECT (1 << 2)
#define OBJLUPYGAMELIFT_SNUFF_FIRE (1 << 3)

typedef struct ObjJgameLight {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ UNK_TYPE1 pad_190[4];
    /* 0x194 */ LightNode* lightNode;
    /* 0x198 */ LightInfo lightInfo;
    /* 0x1A8 */ f32 flameScaleProportion;
    /* 0x1AC */ s16 lightRadius;
    /* 0x1AE */ s16 flameScroll;
    /* 0x1B0 */ UNK_TYPE1 pad_1B0[2];
    /* 0x1B2 */ s16 alpha;
    /* 0x1B4 */ s16 signal;
    /* 0x1B6 */ s16 isOn;
    /* 0x1B8 */ u8 prevHealth;
} ObjJgameLight; // size = 0x1BC

#endif // Z_OBJ_JGAME_LIGHT_H
