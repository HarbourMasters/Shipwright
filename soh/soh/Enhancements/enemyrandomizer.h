#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    int16_t enemyId;
    int16_t enemyParam;
} enemyEntry;

#define RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE 50

#ifndef __cplusplus
uint8_t IsEnemyFoundToRandomize(PlayState* play, int actorId, int param, f32 posX);
enemyEntry GetRandomizedEnemy(void);
uint8_t IsEnemyAllowedToSpawn(PlayState* play, enemyEntry newEnemy);
#endif
