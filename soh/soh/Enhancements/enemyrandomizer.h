#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    int16_t enemyId;
    int16_t enemyParam;
} enemyEntry;

enum { RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE = 41 };

#ifndef __cplusplus
uint8_t IsEnemyFoundToRandomize(int actorId, int param);
enemyEntry GetRandomizedEnemy(void);
#endif
