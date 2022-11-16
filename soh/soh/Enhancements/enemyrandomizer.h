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

uint8_t IsEnemyFoundToRandomize(uint8_t sceneNum, uint8_t actorId, uint8_t param, float posX);
uint8_t IsEnemyAllowedToSpawn(uint8_t sceneNum, EnemyEntry newEnemy);
EnemyEntry GetRandomizedEnemyEntry(float seed1, float seed2, float seed3);

#ifndef __cplusplus
RandomizedEnemy GetRandomizedEnemy(PlayState* play, uint16_t actorId, f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s16 params);
#endif
