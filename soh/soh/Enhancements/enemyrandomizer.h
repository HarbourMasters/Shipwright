#pragma once

#include <global.h>

typedef struct EnemyEntry {
    s16 id;
    s16 params;
} EnemyEntry;

typedef struct RandomizedEnemy {
    s16 id;
    f32 posX;
    f32 posY;
    f32 posZ;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    s16 params;
} RandomizedEnemy;

#define RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE 49

uint8_t IsEnemyFoundToRandomize(s16 sceneNum, s16 actorId, s16 param, f32 posX);
uint8_t IsEnemyAllowedToSpawn(s16 sceneNum, EnemyEntry newEnemy);
EnemyEntry GetRandomizedEnemyEntry(f32 seed1, f32 seed2, f32 seed3);

#ifndef __cplusplus
RandomizedEnemy GetRandomizedEnemy(PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s16 params);
#endif
