#ifndef Z_EN_SNOWBALL_H
#define Z_EN_SNOWBALL_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnSnowball;

typedef void (*EnSnowballActionFunc)(struct EnSnowball*, PlayState*);

typedef struct EnSnowball {
    Actor actor;

    ColliderJntSph collider;
    ColliderJntSphElement colliderItems[1];
    s16 sRot;
    f32 prevY;
} EnSnowball;

#ifdef __cplusplus
extern "C" {
#endif
void EnSnowball_Init(Actor* thisx, PlayState* play);
void EnSnowball_Destroy(Actor* thisx, PlayState* play);
void EnSnowball_Update(Actor* thisx, PlayState* play);
void EnSnowball_Draw(Actor* thisx, PlayState* play);
#ifdef __cplusplus
}
#endif

#endif