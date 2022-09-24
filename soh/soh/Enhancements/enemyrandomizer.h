#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    uint16_t enemyId;
    uint16_t enemyParam;
} enemyEntry;

enum { ENEMY_TABLE_SIZE = 8 };

#ifndef __cplusplus
bool IsEnemyFoundToRandomize(int actorId);
enemyEntry GetRandomizedEnemy(void);
#endif
