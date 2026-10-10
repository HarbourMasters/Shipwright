#ifndef SOULS_H
#define SOULS_H

#include "Rando/Rando.h"

#define SOUL_RI_TO_RANDO_INF(randoItemId) ((randoItemId - RI_SOUL_BOSS_GOHT) + RANDO_INF_OBTAINED_SOUL_OF_BOSS_GOHT)

extern bool HaveEnemySoul(ActorId enemyId);

#endif // SOULS_H