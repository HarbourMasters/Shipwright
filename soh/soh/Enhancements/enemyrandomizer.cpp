#include "enemyrandomizer.h"

extern "C" {
#include <z64.h>
}

uint32_t enemyFound = 0;

static enemyEntry randomizedEnemySpawnTable[RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE] = {
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
	{ACTOR_EN_DEKUBABA, 0},		// Deku Baba (small)
	{ACTOR_EN_DEKUBABA, 1},		// Deku Baba (large)
    /* Partly broken */ { ACTOR_EN_DEKUNUTS, 768 },	// Mad Scrub (triple attack) (projectiles don't work)
	// Broken {ACTOR_EN_VALI, -1}			// Bari (big jellyfish) (problems with splitting and positioning - spawning in the air)
	{ACTOR_EN_BB, -1},		// Bubble (flying skull enemy) (blue)
	{ACTOR_EN_YUKABYUN, 0},		// Flying Floor Tile
	{ACTOR_EN_VM, 1280},		// Beamos
	{ACTOR_EN_FLOORMAS, 0},		// Floormaster
	{ACTOR_EN_RD, 1},			// Redead (standing)
	{ACTOR_EN_RD, -2}			// Gibdo (standing)
};

static int enemiesToRandomize[] = {
	ACTOR_EN_FIREFLY,		// Keese (including fire/ice)
    ACTOR_EN_TEST,			// Stalfos
	ACTOR_EN_TITE,			// Tektite
	ACTOR_EN_RR,			// Like-Like
	ACTOR_EN_FZ,			// Freezard
	ACTOR_EN_POH,			// Poe (normal, blue rupee, composers
    ACTOR_EN_OKUTA,			// Octorok
	ACTOR_EN_WALLMAS,		// Wallmaster
    ACTOR_EN_REEBA,			// Leever
    ACTOR_EN_PEEHAT,		// Flying Peahat, big one spawning larva, larva
	ACTOR_EN_ZF,			// Lizalfos, dinolfos
	ACTOR_EN_GOMA,			// Gohma larva (normal, eggs, gohma eggs)
	ACTOR_EN_BUBBLE,		// Shabom (bubble)
	ACTOR_EN_DODOJR,		// Baby Dodongo
	ACTOR_EN_TORCH2,		// Dark Link
	ACTOR_EN_BILI,			// Biri (small jellyfish)
	ACTOR_EN_TP,			// Electric Tailparasan
	ACTOR_EN_ST,			// Skulltula (normal, big, invisible)
	ACTOR_EN_BW,			// Torch Slug
	ACTOR_EN_EIYER,			// Stinger (land)
	ACTOR_EN_MB,			// Moblins (Club, spear)
	ACTOR_EN_DEKUBABA,		// Deku Baba (small, large)
	ACTOR_EN_DEKUNUTS,		// Mad Scrub (single attack, triple attack)
    // Broken ACTOR_EN_VALI,		// Bari (big jellyfish) (enemies spawn way too high)
	ACTOR_EN_BB,			// Bubble (flying skull enemy) (all colors)
	ACTOR_EN_YUKABYUN,		// Flying Floor Tile
	ACTOR_EN_VM,			// Beamos
	ACTOR_EN_FLOORMAS,		// Floormaster
	ACTOR_EN_RD				// Redead, Gibdo
};

extern "C" enemyEntry GetRandomizedEnemy(void) {
    //return randomizedEnemySpawnTable[rand() % RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE];
    return randomizedEnemySpawnTable[28];
}

extern "C" uint8_t IsEnemyFoundToRandomize(int actorId = 0, int param = 0) {

	uint8_t isEnemyFound = 0;

	for (int i = 0; i < sizeof(enemiesToRandomize) / sizeof(enemiesToRandomize[0]); i++) {
        if (actorId == enemiesToRandomize[i]) {
            isEnemyFound = 1;
        }
    }

	if (isEnemyFound) {
		switch (actorId) {
			// Only randomize the main component of Electric Tailparasans, not the tail segments they spawn
			case ACTOR_EN_TP:
				if (param != -1) {
                    return 0;
                }
                break;
			// Only randomize the initial deku scrub actor (single and triple attack), not the flower they spawn.
			case ACTOR_EN_DEKUNUTS:
				if (param != -256 && param != 768) {
                    return 0;
				}
                break;
            default:
                break;
		}

		// Enemy found.
        return 1;
	}

	// If no enemy is found, don't randomize the actor.
	return 0;
}
