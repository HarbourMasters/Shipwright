#ifndef Z_EN_M_THUNDER_H
#define Z_EN_M_THUNDER_H

#include "global.h"

struct EnMThunder;

#define ENMTHUNDER_GET_TYPE(thisx) ((thisx)->params & 0xFF)
#define ENMTHUNDER_GET_MAGIC_COST(thisx) (((thisx)->params & 0xFF00) >> 8)

typedef enum {
    /* 0x00 */ ENMTHUNDER_TYPE_KOKIRI_SWORD,
    /* 0x01 */ ENMTHUNDER_TYPE_RAZOR_SWORD,
    /* 0x02 */ ENMTHUNDER_TYPE_GILDED_SWORD,
    /* 0x03 */ ENMTHUNDER_TYPE_GREAT_FAIRYS_SWORD,
    /* 0x80 */ ENMTHUNDER_TYPE_UNK = 0x80
} EnMThunderType;

typedef void (*EnMThunderActionFunc)(struct EnMThunder*, PlayState*);

typedef struct EnMThunder {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ LightNode* lightNode;
    /* 0x194 */ LightInfo lightInfo;
    /* 0x1A4 */ f32 lightColorFrac;
    /* 0x1A8 */ f32 alphaFrac;
    /* 0x1AC */ f32 scroll;
    /* 0x1B0 */ f32 unk1B0;
    /* 0x1B4 */ f32 adjustLightsArg1;
    /* 0x1B8 */ EnMThunderActionFunc actionFunc;
    /* 0x1BC */ u16 timer;
    /* 0x1BE */ u8 subtype;
    /* 0x1BF */ u8 type;
    /* 0x1C0 */ u8 chargingAlpha;
    /* 0x1C1 */ u8 scaleTarget;
    /* 0x1C2 */ u8 isCharging;
} EnMThunder; // size = 0x1C4

#endif // Z_EN_M_THUNDER_H
