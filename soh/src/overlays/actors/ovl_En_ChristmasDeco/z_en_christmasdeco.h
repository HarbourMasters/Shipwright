#ifndef Z_EN_CHRISTMASDECO_H
#define Z_EN_CHRISTMASDECO_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnChristmasDeco;

typedef void (*EnChristmasDecoActionFunc)(struct EnChristmasDeco*, PlayState*);

typedef struct EnChristmasDeco {
    Actor actor;
    EnChristmasDecoActionFunc actionFunc;
} EnChristmasDeco;

#ifdef __cplusplus
extern "C" {
#endif
void EnChristmasDeco_Init(Actor* thisx, PlayState* play);
void EnChristmasDeco_Destroy(Actor* thisx, PlayState* play);
void EnChristmasDeco_Update(Actor* thisx, PlayState* play);
void EnChristmasDeco_Draw(Actor* thisx, PlayState* play);
#ifdef __cplusplus
}
#endif

#endif
