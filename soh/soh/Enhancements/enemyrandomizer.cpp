#include "enemyrandomizer.h"

static EnemyEntry randomizedEnemySpawnTable[RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE] = {
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

extern "C" RandomizedEnemy GetRandomizedEnemy(PlayState* play, s16 actorId, f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, s16 params) {

    if (IsEnemyFoundToRandomize(play->sceneNum, actorId, params, posX)) {

        // When replacing Iron Knuckles in Spirit Temple, move them away from the throne because
        // some enemies can get stuck on the throne.
        if (actorId == ACTOR_EN_IK && play->sceneNum == SCENE_JYASINZOU) {
            if (params == 6657) {
                posX = posX + 150;
            } else if (params == 6401) {
                posX = posX - 150;
            }
        }

        // Do a raycast from the original position of the actor to find the ground below it, then try to place
        // the new actor on the ground. This way enemies don't spawn very high in the sky, and gives us control
        // over height offsets per enemy from a proven grounded position.
        CollisionPoly poly;
        Vec3f pos;
        f32 raycastResult;

        pos.x = posX;
        pos.y = posY + 50;
        pos.z = posZ;
        raycastResult = BgCheck_AnyRaycastFloor1(&play->colCtx, &poly, &pos);

        // If ground is found below actor, move actor to that height.
        if (raycastResult > BGCHECK_Y_MIN) {
            posY = raycastResult;
        }

        // Get randomized enemy ID and parameter.
        EnemyEntry randomEnemy = GetRandomizedEnemyEntry(posX, posY, posZ);

        // While randomized enemy isn't allowed in certain situations, randomize again.
        while (!IsEnemyAllowedToSpawn(play->sceneNum, randomEnemy)) {
            randomEnemy = GetRandomizedEnemyEntry(posX, posY, posZ);
        }

        actorId = randomEnemy.id;
        params = randomEnemy.params;

        // Straighten out enemies so they aren't flipped on their sides when the original spawn is.
        rotX = 0;

        switch (actorId) {
            // When spawning big jellyfish, spawn it up high.
            case ACTOR_EN_VALI:
                posY = posY + 300;
                break;
            // Spawn peahat off the ground, otherwise it kills itself by colliding with the ground.
            case ACTOR_EN_PEEHAT:
                if (params == 1) {
                    posY = posY + 100;
                }
                break;
            // Spawn skulltulas off the ground.
            case ACTOR_EN_ST:
                posY = posY + 200;
                break;
            // Spawn flying enemies off the ground.
            case ACTOR_EN_FIREFLY:
            case ACTOR_EN_BILI:
            case ACTOR_EN_BB:
            case ACTOR_EN_CLEAR_TAG:
            case ACTOR_EN_CROW:
                posY = posY + 75;
                break;
            default:
                break;
        }
    }

    RandomizedEnemy newEnemy;
    newEnemy.id = actorId;
    newEnemy.posX = posX;
    newEnemy.posY = posY;
    newEnemy.posZ = posZ;
    newEnemy.rotX = rotX;
    newEnemy.rotY = rotX;
    newEnemy.rotZ = rotX;
    newEnemy.params = params;

    return newEnemy;
}

EnemyEntry GetRandomizedEnemyEntry(f32 seed1, f32 seed2, f32 seed3) {
    uint32_t randomNumber = rand() + (int)seed1 + (int)seed2 + (int)seed3;
    return randomizedEnemySpawnTable[randomNumber % RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE];
}

uint8_t IsEnemyFoundToRandomize(s16 sceneNum, s16 actorId, s16 params, f32 posX) {

    for (int i = 0; i < ARRAY_COUNT(enemiesToRandomize); i++) {

        if (actorId == enemiesToRandomize[i]) {

            switch (actorId) {
                // Only randomize the main component of Electric Tailparasans, not the tail segments they spawn.
                case ACTOR_EN_TP:
                    return (params == -1);
                // Only randomize the initial deku scrub actor (single and triple attack), not the flower they spawn.
                case ACTOR_EN_DEKUNUTS:
                    return (params == -256 || params == 768);
                // Only randomize initial floormaster actor (it can split and does some spawning on init).
                case ACTOR_EN_FLOORMAS:
                    return (params == 0 || params == -32768);
                // Only randomize the initial eggs, not the enemies that spawn from them.
                case ACTOR_EN_GOMA:
                    return ((params >= 0 && params <= 6) || params == 8);
                // Only randomize Skullwalltulas, not Golden Skulltulas.
                case ACTOR_EN_SW:
                    return (params == 0);
                // Don't randomize Nabooru because it'll break the cutscene and the door.
                case ACTOR_EN_IK:
                    return (params != 1280);
                // Only randomize the intitial spawn of the huge jellyfish. It spawns another copy when hit with a sword.
                case ACTOR_EN_VALI:
                    return (params == -1);
                // Don't randomize lizalfos in Doodong's Cavern because the gates won't work correctly otherwise.
                case ACTOR_EN_ZF:
                    return (params != 1280 && params != 1281 && params != 1536 && params != 1537);
                // Don't randomize the Wolfos in SFM because it's needed to open the gate.
                case ACTOR_EN_WF:
                    return (params != 7936);
                // Don't randomize the Stalfos in Forest Temple because other enemies fall through the hole and don't trigger the platform.
                // Don't randomize the Stalfos spawning on the boat in Shadow Temple, as randomizing them places the new enemies
                // down in the river.
                case ACTOR_EN_TEST:
                    return (params != 1 && !(sceneNum == SCENE_HAKADAN && (posX == 2746 || posX == 1209)));
                // Only randomize the enemy variant of Armos Statue.
                // Leave one Armos unrandomized in the Spirit Temple room where an armos is needed to push down a button
                case ACTOR_EN_AM:
                    return ((params == -1 || params == 255) && !(sceneNum == SCENE_JYASINZOU && posX == 2141));
                // Don't randomize Shell Blades and Spikes in the underwater portion in Water Temple as it's impossible to kill
                // most other enemies underwater with just hookshot and they're required to be killed for a grate to open.
                case ACTOR_EN_SB:
                    return (sceneNum == SCENE_MIZUSIN && (posX == 419 || posX == 435));
                case ACTOR_EN_NY:
                    return (sceneNum == SCENE_MIZUSIN && (posX == 380 || posX == 382 || posX == 416 || posX == 452 || posX == 454));
                // Don't randomize the 2 like-likes in Jabu Jabu's Belly in MQ because they spawn in a box on top of the ceilling.
                // Other enemies won't aggro on Link correctly to fall down.
                case ACTOR_EN_RR:
                    return (sceneNum != SCENE_BDAN);
                default:
                    return 1;
            }
        }
    }

    // If no enemy is found, don't randomize the actor.
    return 0;
}

uint8_t IsEnemyAllowedToSpawn(s16 sceneNum, EnemyEntry enemy) {

    switch (sceneNum) {
        // Don't allow Dark Link in areas with void out zones as it voids out the player as well.
        // Shadow Temple, Gerudo Training Ground and Death Mountain Crater. 
        case SCENE_HAKADAN:
        case SCENE_MEN:
        case SCENE_SPOT17:
            return (enemy.id != ACTOR_EN_TORCH2);
        // Don't allow certain enemies in Ganon's Tower because they would spawn up on the ceilling,
        // becoming impossible to kill
        case SCENE_GANON:
        case SCENE_GANON_SONOGO:
            return (enemy.id != ACTOR_EN_CLEAR_TAG && enemy.id != ACTOR_EN_VALI && !(enemy.id == ACTOR_EN_ZF && enemy.params == -1));
        default:
            return 1;
    }
}
