#pragma once

#include <stdint.h>

typedef struct enemyEntry {
    int16_t enemyId;
    int16_t enemyParam;
} enemyEntry;

#define RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE 49

#ifndef __cplusplus
uint8_t IsEnemyFoundToRandomize(PlayState* play, int actorId, int param, f32 posX);
enemyEntry GetRandomizedEnemy(f32 seed1, f32 seed2, f32 seed3);
uint8_t IsEnemyAllowedToSpawn(PlayState* play, enemyEntry newEnemy);
#endif
