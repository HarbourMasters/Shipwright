#ifndef Z_BG_SPOT18_BASKET_H
#define Z_BG_SPOT18_BASKET_H

#include <libultraship/libultra.h>
#include "global.h"

struct BgSpot18Basket;

typedef void (*BgSpot18BasketActionFunc)(struct BgSpot18Basket*, PlayState*);

typedef struct BgSpot18Basket {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ColliderJntSph colliderJntSph;
    /* 0x0184 */ ColliderJntSphElement ColliderJntSphElements[2];
    /* 0x0204 */ BgSpot18BasketActionFunc actionFunc;
    /* 0x0208 */ f32 circleRadius;
    /* 0x020C */ s16 circleRate;
    /* 0x020E */ s16 circleMoveAngle;
    /* 0x0210 */ s16 spinRate;
    /* 0x0212 */ s16 pivotAzimuth;
    /* 0x0214 */ s16 pivotAltitude;
    /* 0x0216 */ s16 timer;
    /* 0x0218 */ s16 prize;
    /* 0x021A */ u8 isHeartPieceGiven;
    /* 0x021B */ u8 pad;
} BgSpot18Basket; // size = 0x021C

#endif

