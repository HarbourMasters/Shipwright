#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    uint32_t enemyId;
    uint32_t enemyParam;
} enemyEntry;

enum { ENEMY_TABLE_SIZE = 26 };

#ifndef __cplusplus
bool IsEnemyFoundToRandomize(int actorId, int param);
enemyEntry GetRandomizedEnemy(void);
#endif
