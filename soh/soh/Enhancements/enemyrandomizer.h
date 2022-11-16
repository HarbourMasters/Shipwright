#pragma once

#include <stdint.h>

typedef struct EnemyEntry {
    int16_t id;
    int16_t params;
} EnemyEntry;

typedef struct RandomizedEnemy {
    int16_t id;
    float posX;
    float posY;
    float posZ;
    int16_t rotX;
    int16_t rotY;
    int16_t rotZ;
    int16_t params;
} RandomizedEnemy;

#define RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE 49

uint8_t IsEnemyFoundToRandomize(int16_t sceneNum, int16_t actorId, int16_t params, float posX);
uint8_t IsEnemyAllowedToSpawn(int16_t sceneNum, EnemyEntry enemy);
EnemyEntry GetRandomizedEnemyEntry(float seed1, float seed2, float seed3);

#ifndef __cplusplus
RandomizedEnemy GetRandomizedEnemy(PlayState* play, int16_t actorId, f32 posX, f32 posY, f32 posZ, int16_t rotX, int16_t rotY, int16_t rotZ, int16_t params);
#endif
