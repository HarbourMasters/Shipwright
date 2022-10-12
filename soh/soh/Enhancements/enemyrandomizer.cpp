#include "enemyrandomizer.h"

extern "C" {
#include <z64.h>
}

uint32_t enemyFound = 0;

enemyEntry randomizedEnemyEntryTable[ENEMY_TABLE_SIZE] = {
	{ACTOR_EN_FIREFLY, 2},	// Regular Keese
	{ACTOR_EN_FIREFLY, 1},	// Fire Keese
	{ACTOR_EN_FIREFLY, 4},	// Ice Keese
    {ACTOR_EN_TEST, 2},		// Stalfos
	{ACTOR_EN_TITE, 0},		// Tektite
	{ACTOR_EN_RR, 0},		// Like-Like
	{ACTOR_EN_FZ, 0},		// Freezard
	{ACTOR_EN_POH, 0},		// Poe
	{ACTOR_EN_POH, 2},		// Poe (composer Sharp)
	{ACTOR_EN_POH, 3},		// Poe (composer Flat)
	{ACTOR_EN_OKUTA, 0},	// Octorok
	{ACTOR_EN_WALLMAS, 0},	// Wallmaster
	{ACTOR_EN_REEBA, 0},	// Leever
	{ACTOR_EN_PEEHAT, 0},	// Flying Peahat, spawns larva
	{ACTOR_EN_PEEHAT, 1},	// Flying Peahat Larva
	{ACTOR_EN_ZF, 0},		// Lizalfos mini-boss, drops from ceiling. Partner to 01, in order to be killed
	{ACTOR_EN_ZF, 1},		// Lizalfos mini-boss, no mini-boss music Partner to 00, in order to be killed
	{ACTOR_EN_GOMA, 0},		// Gohma larva
	{ACTOR_EN_GOMA, 6},		// Gohma larva egg
	{ACTOR_EN_BUBBLE, 0},	// Shabom (bubble)
	{ACTOR_EN_DODOJR, 0},	// Baby Dodongo
	{ACTOR_EN_TORCH2, 0},	// Dark Link
	{ACTOR_EN_BILI, 0},		// Biri (jellyfish)
	{ACTOR_EN_TP, 0},		// Electric Tailparasan
	{ACTOR_EN_BW, 0},		// Torch Slug
	{ACTOR_EN_DEKUBABA, 0}	// Deku baba (small)
};

extern "C" enemyEntry GetRandomizedEnemy(void) {
    return randomizedEnemyEntryTable[rand() % ENEMY_TABLE_SIZE];
}

extern "C" bool IsEnemyFoundToRandomize(int actorId = 0, int param = 0) {
	for (int i = 0; i < ENEMY_TABLE_SIZE; i++) {
        if (actorId == randomizedEnemyEntryTable[i].enemyId && param == randomizedEnemyEntryTable[i].enemyParam) {
            return 1;
		}
	}

	return 0;
}
