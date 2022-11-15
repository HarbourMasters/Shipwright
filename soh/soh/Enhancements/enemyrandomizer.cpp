#include "enemyrandomizer.h"

extern "C" {
#include <z64.h>
}

#include "macros.h"

static enemyEntry randomizedEnemySpawnTable[RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE] = {
    { ACTOR_EN_FIREFLY, 2 },    // Regular Keese
    { ACTOR_EN_FIREFLY, 1 },    // Fire Keese
    { ACTOR_EN_FIREFLY, 4 },    // Ice Keese
    { ACTOR_EN_TEST, 2 },       // Stalfos
    { ACTOR_EN_TITE, -1 },      // Tektite (red)
    { ACTOR_EN_TITE, -2 },      // Tektite (blue)
    { ACTOR_EN_WALLMAS, 1 },    // Wallmaster
    { ACTOR_EN_DODONGO, -1 },   // Dodongo
    { ACTOR_EN_PEEHAT, -1 },    // Flying Peahat (big grounded, doesn't spawn larva)
    { ACTOR_EN_PEEHAT, 1 },     // Flying Peahat Larva
    { ACTOR_EN_ZF, -1 },        // Lizalfos
    { ACTOR_EN_ZF, -2 },        // Dinolfos
    { ACTOR_EN_GOMA, 7 },       // Gohma larva (non-gohma rooms)
    { ACTOR_EN_BUBBLE, 0 },     // Shabom (bubble)
    { ACTOR_EN_DODOJR, 0 },     // Baby Dodongo
    { ACTOR_EN_TORCH2, 0 },     // Dark Link
    { ACTOR_EN_BILI, 0 },       // Biri (jellyfish)
    { ACTOR_EN_TP, -1 },        // Electric Tailparasan
    { ACTOR_EN_ST, 0 },         // Skulltula (normal)
    { ACTOR_EN_ST, 1 },         // Skulltula (big)
    { ACTOR_EN_ST, 2 },         // Skulltula (invisible)
    { ACTOR_EN_BW, 0 },         // Torch Slug
    { ACTOR_EN_EIYER, 10 },     // Stinger (land) (One in formation, sink under floor and do not activate)
    { ACTOR_EN_MB, 0 },         // Moblins (Club)
    { ACTOR_EN_DEKUBABA, 0 },   // Deku Baba (small)
    { ACTOR_EN_DEKUBABA, 1 },   // Deku Baba (large)
    { ACTOR_EN_AM, -1 },        // Armos (enemy variant)
    { ACTOR_EN_DEKUNUTS, 768 }, // Mad Scrub (triple attack) (projectiles don't work)
    { ACTOR_EN_VALI, -1 },      // Bari (big jellyfish)
    { ACTOR_EN_BB, -1 },        // Bubble (flying skull enemy) (blue)
    { ACTOR_EN_YUKABYUN, 0 },   // Flying Floor Tile
    { ACTOR_EN_VM, 1280 },      // Beamos
    { ACTOR_EN_FLOORMAS, 0 },   // Floormaster
    { ACTOR_EN_RD, 1 },         // Redead (standing)
    { ACTOR_EN_RD, 32766 },     // Gibdo (standing)
    { ACTOR_EN_SB, 0 },         // Shell Blade
    { ACTOR_EN_KAREBABA, 0 },   // Withered Deku Baba
    { ACTOR_EN_RR, 0 },         // Like-Like
    { ACTOR_EN_NY, 0 },         // Spike (rolling enemy)
    { ACTOR_EN_IK, 2 },         // Iron Knuckle (black, standing)
    { ACTOR_EN_IK, 3 },         // Iron Knuckle (white, standing)
    { ACTOR_EN_TUBO_TRAP, 0 },  // Flying pot
    { ACTOR_EN_FZ, 0 },         // Freezard
    { ACTOR_EN_CLEAR_TAG, 1 },  // Arwing
    { ACTOR_EN_WF, 0 },         // Wolfos (normal)
    { ACTOR_EN_WF, 1 },         // Wolfos (white)
    { ACTOR_EN_SKB, 1 },        // Stalchild (small)
    { ACTOR_EN_SKB, 20 },       // Stalchild (big)
    { ACTOR_EN_CROW, 0 }        // Guay

    // Doesn't work {ACTOR_EN_POH, 0}, // Poe (Seems to rely on other objects?)
    // Doesn't work {ACTOR_EN_POH, 2}, // Poe (composer Sharp) (Seems to rely on other objects?)
    // Doesn't work {ACTOR_EN_POH, 3}, // Poe (composer Flat) (Seems to rely on other objects?)
    // Doesn't work {ACTOR_EN_OKUTA, 0}, // Octorok (actor directly uses water box collision to handle hiding/popping up)
    // Doesn't work {ACTOR_EN_REEBA, 0}, // Leever (reliant on surface and also normally used in tandem with a leever spawner, kills itself too quickly otherwise)
    // Kinda doesn't work { ACTOR_EN_FD, 0 }, // Flare Dancer (jumps out of bounds a lot, and possible cause of crashes because of spawning a ton of flame actors)
};

static int enemiesToRandomize[] = {
    ACTOR_EN_FIREFLY,   // Keese (including fire/ice)
    ACTOR_EN_TEST,      // Stalfos
    ACTOR_EN_TITE,      // Tektite
    ACTOR_EN_POH,       // Poe (normal, blue rupee, composers
    ACTOR_EN_OKUTA,     // Octorok
    ACTOR_EN_WALLMAS,   // Wallmaster
    ACTOR_EN_DODONGO,   // Dodongo
    // ACTOR_EN_REEBA,  // Leever (reliant on spawner (z_e_encount1.c)
    ACTOR_EN_PEEHAT,    // Flying Peahat, big one spawning larva, larva
    ACTOR_EN_ZF,        // Lizalfos, dinolfos
    ACTOR_EN_GOMA,      // Gohma larva (normal, eggs, gohma eggs)
    ACTOR_EN_BUBBLE,    // Shabom (bubble)
    ACTOR_EN_DODOJR,    // Baby Dodongo
    ACTOR_EN_TORCH2,    // Dark Link
    ACTOR_EN_BILI,      // Biri (small jellyfish)
    ACTOR_EN_TP,        // Electric Tailparasan
    ACTOR_EN_ST,        // Skulltula (normal, big, invisible)
    ACTOR_EN_BW,        // Torch Slug
    ACTOR_EN_EIYER,     // Stinger (land)
    ACTOR_EN_MB,        // Moblins (Club, spear)
    ACTOR_EN_DEKUBABA,  // Deku Baba (small, large)
    ACTOR_EN_AM,        // Armos (enemy variant)
    ACTOR_EN_DEKUNUTS,  // Mad Scrub (single attack, triple attack)
    ACTOR_EN_VALI,      // Bari (big jellyfish) (spawns very high up)
    ACTOR_EN_BB,        // Bubble (flying skull enemy) (all colors)
    ACTOR_EN_YUKABYUN,  // Flying Floor Tile
    ACTOR_EN_VM,        // Beamos
    ACTOR_EN_FLOORMAS,  // Floormaster
    ACTOR_EN_RD,        // Redead, Gibdo
    ACTOR_EN_SW,        // Skullwalltula
    // ACTOR_EN_FD,     // Flare Dancer (can be randomized, but not randomized to, so keeping it in vanilla locations means it atleast shows up in the game
    ACTOR_EN_SB,        // Shell Blade
    ACTOR_EN_KAREBABA,  // Withered Deku Baba
    ACTOR_EN_RR,        // Like-Like
    ACTOR_EN_NY,        // Spike (rolling enemy)
    ACTOR_EN_IK,        // Iron Knuckle
    ACTOR_EN_TUBO_TRAP, // Flying pot
    ACTOR_EN_FZ,        // Freezard
    ACTOR_EN_WEIYER,    // Stinger (Water)
    ACTOR_EN_HINTNUTS,  // Hint deku scrubs
    ACTOR_EN_WF,        // Wolfos
    ACTOR_EN_SKB,       // Stalchild
    ACTOR_EN_CROW       // Guay
};

extern "C" enemyEntry GetRandomizedEnemy(void) {
    return randomizedEnemySpawnTable[rand() % RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE];
}

extern "C" uint8_t IsEnemyFoundToRandomize(PlayState* play, int actorId = 0, int param = 0, f32 posX = 0) {

    for (int i = 0; i < ARRAY_COUNT(enemiesToRandomize); i++) {

        if (actorId == enemiesToRandomize[i]) {

            switch (actorId) {
                // Only randomize the main component of Electric Tailparasans, not the tail segments they spawn.
                case ACTOR_EN_TP:
                    return (param == -1);
                // Only randomize the initial deku scrub actor (single and triple attack), not the flower they spawn.
                case ACTOR_EN_DEKUNUTS:
                    return (param == -256 || param == 768);
                // Only randomize initial floormaster actor (it can split and does some spawning on init).
                case ACTOR_EN_FLOORMAS:
                    return (param == 0 || param == -32768);
                // Only randomize the initial eggs, not the enemies that spawn from them.
                case ACTOR_EN_GOMA:
                    return ((param >= 0 && param <= 6) || param == 8);
                // Only randomize Skullwalltulas, not Golden Skulltulas.
                case ACTOR_EN_SW:
                    return (param == 0);
                // Don't randomize Nabooru because it'll break the cutscene and the door.
                case ACTOR_EN_IK:
                    return (param != 1280);
                // Only randomize the intitial spawn of the huge jellyfish. It spawns another copy when hit with a sword.
                case ACTOR_EN_VALI:
                    return (param == -1);
                // Don't randomize lizalfos in Doodong's Cavern because the gates won't work correctly otherwise.
                case ACTOR_EN_ZF:
                    return (param != 1280 && param != 1281 && param != 1536 && param != 1537);
                // Don't randomize the Wolfos in SFM because it's needed to open the gate.
                case ACTOR_EN_WF:
                    return (param != 7936);
                // Don't randomize the Stalfos in Forest Temple because other enemies fall through the hole and don't trigger the platform.
                // Don't randomize the Stalfos spawning on the boat in Shadow Temple, as randomizing them places the new enemies
                // down in the river.
                case ACTOR_EN_TEST:
                    return (param != 1 && !(play->sceneNum == SCENE_HAKADAN && (posX == 2746 || posX == 1209)));
                // Only randomize the enemy variant of Armos Statue.
                // Leave one Armos unrandomized in the Spirit Temple room where an armos is needed to push down a button
                case ACTOR_EN_AM:
                    return (param == -1 && !(play->sceneNum == SCENE_JYASINZOU && posX == 2141));
                // Don't randomize Shell Blades and Spikes in the underwater portion in Water Temple as it's impossible to kill
                // most other enemies underwater with just hookshot and they're required to be killed for a grate to open.
                case ACTOR_EN_SB:
                    return (play->sceneNum == SCENE_MIZUSIN && (posX == 419 || posX == 435));
                case ACTOR_EN_NY:
                    return (play->sceneNum == SCENE_MIZUSIN && (posX == 380 || posX == 382 || posX == 416 || posX == 452 || posX == 454));
                // Don't randomize the 2 like-likes in Jabu Jabu's Belly in MQ because they spawn in a box on top of the ceilling.
                // Other enemies won't aggro on Link correctly to fall down.
                case ACTOR_EN_RR:
                    return (play->sceneNum != SCENE_BDAN);
                default:
                    return 1;
            }
        }
    }

    // If no enemy is found, don't randomize the actor.
    return 0;
}

extern "C" uint8_t IsEnemyAllowedToSpawn(PlayState* play, enemyEntry enemy) {

    switch (play->sceneNum) {
        // Don't allow Dark Link in areas with void out zones as it voids out the player as well.
        // Shadow Temple, Gerudo Training Ground and Death Mountain Crater. 
        case SCENE_HAKADAN:
        case SCENE_MEN:
        case SCENE_SPOT17:
            return (enemy.enemyId != ACTOR_EN_TORCH2);
        // Don't allow certain enemies in Ganon's Tower because they would spawn up on the ceilling,
        // becoming impossible to kill
        case SCENE_GANON:
        case SCENE_GANON_SONOGO:
            return (enemy.enemyId != ACTOR_EN_CLEAR_TAG && enemy.enemyId != ACTOR_EN_VALI && !(enemy.enemyId == ACTOR_EN_ZF && enemy.enemyParam == -1));
        default:
            return 1;
    }
}
