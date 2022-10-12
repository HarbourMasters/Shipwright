#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    int16_t enemyId;
    int16_t enemyParam;
} enemyEntry;

enum { ENEMIES_TO_RANDOMIZE_TABLE_SIZE = 43 };
enum { ENEMIES_TO_SPAWN_TABLE_SIZE = 26 };

#ifndef __cplusplus
bool IsEnemyFoundToRandomize(int actorId, int param);
enemyEntry GetRandomizedEnemy(void);
#endif
