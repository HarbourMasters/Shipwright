#ifndef HORSE_H
#define HORSE_H

#include <libultraship/libultra.h>
#include "z64math.h"

#include "extern_c_helper.h"

struct PlayState;
struct Actor;
struct Player;

EXTERN_C void Horse_ResetHorseData(struct PlayState* play);
EXTERN_C void Horse_FixLakeHyliaPosition(struct PlayState* play);
EXTERN_C void Horse_InitPlayerHorse(struct PlayState* play, struct Player* player);
EXTERN_C void Horse_RotateToPoint(struct Actor* actor, Vec3f* arg1, s16 arg2);

#endif