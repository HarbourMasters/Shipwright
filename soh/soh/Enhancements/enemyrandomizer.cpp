#include "enemyrandomizer.h"

extern "C" {
#include <z64.h>
}

uint32_t enemyFound = 0;

enemyEntry randomizedEnemyEntryTable[ENEMY_TABLE_SIZE] = {
	{ACTOR_EN_FIREFLY, 2},	// Regular Keese
	{ACTOR_EN_FIREFLY, 1},	// Fire Keese
	{ACTOR_EN_FIREFLY, 4},	// Ice Keese
    {ACTOR_EN_TEST, 1},		// Stalfos
	{ACTOR_EN_TORCH2, 0},	// Dark Link
	{ACTOR_EN_TITE, 0},		// Tektite
	{ACTOR_EN_RR, 0},		// Like-Like
	{ACTOR_EN_FZ, 0}		// Freezard
};

extern "C" enemyEntry GetRandomizedEnemy(void) {
    return randomizedEnemyEntryTable[rand() % ENEMY_TABLE_SIZE];
}

extern "C" bool IsEnemyFoundToRandomize(int actorId = 0) {
	for (int i = 0; i < ENEMY_TABLE_SIZE; i++) {
		if (actorId == randomizedEnemyEntryTable[i].enemyId) {
            return 1;
		}
	}

	return 0;
}
