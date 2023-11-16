#ifndef Z_EN_CHRISTMASTREE_H
#define Z_EN_CHRISTMASTREE_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnChristmasTree;

typedef void (*EnChristmasTreeActionFunc)(struct EnChristmasTree*, PlayState*);

typedef struct EnChristmasTree {
    Actor actor;
    ColliderCylinder collider;
    EnChristmasTreeActionFunc actionFunc;
    u8 spawnedRupee;
} EnChristmasTree;

#ifdef __cplusplus
extern "C" {
#endif
void EnChristmasTree_Init(Actor* thisx, PlayState* play);
void EnChristmasTree_Destroy(Actor* thisx, PlayState* play);
void EnChristmasTree_Update(Actor* thisx, PlayState* play);
void EnChristmasTree_Draw(Actor* thisx, PlayState* play);
#ifdef __cplusplus
}
#endif

#endif
