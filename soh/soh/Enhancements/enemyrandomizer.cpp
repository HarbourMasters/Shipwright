#include "enemyrandomizer.h"

extern "C" {
#include <z64.h>
}

uint32_t enemyFound = 0;

static enemyEntry enemiesToSpawnEntryTable[ENEMIES_TO_SPAWN_TABLE_SIZE] = {
	{ACTOR_EN_FIREFLY, 2},		// Regular Keese
	{ACTOR_EN_FIREFLY, 1},		// Fire Keese
	{ACTOR_EN_FIREFLY, 4},		// Ice Keese
    {ACTOR_EN_TEST, 2},			// Stalfos
	{ACTOR_EN_TITE, 0},			// Tektite
	{ACTOR_EN_RR, 0},			// Like-Like
	{ACTOR_EN_FZ, 0},			// Freezard
	// Broken {ACTOR_EN_POH, 0},		// Poe (requires actor changes)
	// Broken {ACTOR_EN_POH, 2},		// Poe (composer Sharp) (requires actor changes)
	// Broken {ACTOR_EN_POH, 3},		// Poe (composer Flat) (requires actor changes)
    // Broken {ACTOR_EN_OKUTA, 0},		// Octorok (requires actor changes)
	{ACTOR_EN_WALLMAS, 1},		// Wallmaster
    // Broken {ACTOR_EN_REEBA, 0},		// Leever (requires actor changes)
    // Broken {ACTOR_EN_PEEHAT, 0},		// Flying Peahat, spawns larva (requires more testing)
    // Broken {ACTOR_EN_PEEHAT, 1},		// Flying Peahat Larva (requires actor changes)
	{ACTOR_EN_ZF, -1},			// Lizalfos
	{ACTOR_EN_ZF, -2},			// Dinolfos
	{ACTOR_EN_GOMA, 7},			// Gohma larva (non-gohma rooms)
	{ACTOR_EN_BUBBLE, 0},		// Shabom (bubble)
	{ACTOR_EN_DODOJR, 0},		// Baby Dodongo
	{ACTOR_EN_TORCH2, 0},		// Dark Link
	{ACTOR_EN_BILI, 0},			// Biri (jellyfish)
	{ACTOR_EN_TP, -1},			// Electric Tailparasan
	{ACTOR_EN_ST, 0},			// Skulltula (normal)
	{ACTOR_EN_ST, 1},			// Skulltula (big)
	{ACTOR_EN_ST, 2},			// Skulltula (invisible)
	{ACTOR_EN_BW, 0},			// Torch Slug
	{ACTOR_EN_EIYER, 10},		// Stinger (land) (One in formation, sink under floor and do not activate)
	{ACTOR_EN_MB, 0},			// Moblins (Club)
	{ACTOR_EN_DEKUBABA, 0},		// Deku baba (small)
	{ACTOR_EN_DEKUBABA, 1},		// Deku baba (large)
    /* Partly broken */ { ACTOR_EN_DEKUNUTS, 768 },	// Mad Scrub (triple attack) (projectiles don't work)
	// Broken {ACTOR_EN_VALI, -1}			// Bari (big jellyfish) (problems with splitting and positioning - spawning in the air)
	{ACTOR_EN_BB, -1}		// Bubble (flying skull enemy) (blue)
};

static enemyEntry enemiesToRandomizeEntryTable[ENEMIES_TO_RANDOMIZE_TABLE_SIZE] = {
	{ACTOR_EN_FIREFLY, 2},		// Regular Keese
	{ACTOR_EN_FIREFLY, 1},		// Fire Keese
	{ACTOR_EN_FIREFLY, 4},		// Ice Keese
    {ACTOR_EN_TEST, 2},			// Stalfos
	{ACTOR_EN_TITE, 0},			// Tektite
	{ACTOR_EN_RR, 0},			// Like-Like
	{ACTOR_EN_FZ, 0},			// Freezard
	{ACTOR_EN_FZ, -1},			// Freezard
	{ACTOR_EN_POH, 0},			// Poe
	{ACTOR_EN_POH, 1},			// Poe (blue rupee drop)
	{ACTOR_EN_POH, 2},			// Poe (composer Sharp)
	{ACTOR_EN_POH, 3},			// Poe (composer Flat)
    {ACTOR_EN_OKUTA, 0},		// Octorok
	{ACTOR_EN_WALLMAS, 1},		// Wallmaster
    {ACTOR_EN_REEBA, 0},		// Leever
    {ACTOR_EN_PEEHAT, 0},		// Flying Peahat, spawns larva
	{ACTOR_EN_PEEHAT, 1},		// Flying Peahat Larva
	{ACTOR_EN_ZF, -1},			// Lizalfos
	{ACTOR_EN_ZF, 0},			// Lizalfos mini-boss, drops from ceiling. Partner to 01, in order to be killed
	{ACTOR_EN_ZF, 1},			// Lizalfos mini-boss, no mini-boss music Partner to 00, in order to be killed
	{ACTOR_EN_ZF, -2},			// Dinolfos
	{ACTOR_EN_GOMA, 0},			// Gohma larva
	{ACTOR_EN_GOMA, 6},			// Gohma larva egg
	{ACTOR_EN_GOMA, 8},			// Gohma larva egg (non-gohma rooms)
	{ACTOR_EN_BUBBLE, -1},		// Shabom (bubble)
	{ACTOR_EN_BUBBLE, 0},		// Shabom (bubble)
	{ACTOR_EN_DODOJR, 0},		// Baby Dodongo
	{ACTOR_EN_TORCH2, 0},		// Dark Link
	{ACTOR_EN_BILI, 0},			// Biri (jellyfish)
	{ACTOR_EN_TP, -1},			// Electric Tailparasan
	{ACTOR_EN_ST, 0},			// Skulltula (normal)
	{ACTOR_EN_ST, 1},			// Skulltula (big)
	{ACTOR_EN_ST, 2},			// Skulltula (invisible)
	{ACTOR_EN_BW, 0},			// Torch Slug
	{ACTOR_EN_EIYER, 10},		// Stinger (land)
	{ACTOR_EN_MB, 0},			// Moblins (Club)
	{ACTOR_EN_MB, 1},			// Moblins (Spear)
	{ACTOR_EN_DEKUBABA, 0},		// Deku baba (small)
	{ACTOR_EN_DEKUBABA, 1},		// Deku baba (large)
	{ACTOR_EN_DEKUNUTS, -256},	// Mad Scrub (single attack)
	{ACTOR_EN_DEKUNUTS, 768},	// Mad Scrub (triple attack)
	{ACTOR_EN_VALI, -1},		// Bari (big jellyfish)
	{ACTOR_EN_BB, -1}			// Bubble (flying skull enemy) (blue)
};

extern "C" enemyEntry GetRandomizedEnemy(void) {
    //return enemiesToSpawnEntryTable[rand() % ENEMIES_TO_SPAWN_TABLE_SIZE];
    return enemiesToSpawnEntryTable[25];
}

extern "C" bool IsEnemyFoundToRandomize(int actorId = 0, int param = 0) {
    for (int i = 0; i < ENEMIES_TO_RANDOMIZE_TABLE_SIZE; i++) {
        if (actorId == enemiesToRandomizeEntryTable[i].enemyId && param == enemiesToRandomizeEntryTable[i].enemyParam) {
            return 1;
		}
	}

	return 0;
}
