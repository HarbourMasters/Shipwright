#include "enemyrandomizer.h"
#include "functions.h"
#include "macros.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "variables.h"

extern "C" {
#include <z64.h>
}

extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

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

extern "C" uint8_t GetRandomizedEnemy(PlayState* play, int16_t *actorId, f32 *posX, f32 *posY, f32 *posZ, int16_t *rotX,
                                      int16_t *rotY, int16_t *rotZ, int16_t *params) {

    uint32_t isMQ = ResourceMgr_IsSceneMasterQuest(play->sceneNum);

    // Hack to remove enemies that wrongfully spawn because of bypassing object dependency with enemy randomizer on.
    // This should probably be handled on OTR generation in the future when object dependency is fully removed.
    // Remove bats and skulltulas from graveyard.
    // Remove octorok in lost woods.
    if (((*actorId == ACTOR_EN_FIREFLY || (*actorId == ACTOR_EN_SW && *params == 0)) && play->sceneNum == SCENE_SPOT02) ||
        (*actorId == ACTOR_EN_OKUTA && play->sceneNum == SCENE_SPOT10)) {
        return 0;
    }

    // Hack to change a pot in Spirit Temple that holds a Deku Shield to not hold anything.
    // This should probably be handled on OTR generation in the future when object dependency is fully removed.
    // This Deku Shield doesn't normally spawn in authentic gameplay because of object dependency.
    if (*actorId == ACTOR_OBJ_TSUBO && *params == 24597) {
        *params = 24067;
    }

    // Lengthen timer in non-MQ Jabu Jabu bubble room.
    if (!isMQ && *actorId == ACTOR_OBJ_ROOMTIMER && *params == 30760 && play->sceneNum == SCENE_BDAN &&
        play->roomCtx.curRoom.num == 12) {
        *params = 92280;
    }

    if (IsEnemyFoundToRandomize(play->sceneNum, play->roomCtx.curRoom.num, *actorId, *params, *posX)) {

        // When replacing Iron Knuckles in Spirit Temple, move them away from the throne because
        // some enemies can get stuck on the throne.
        if (*actorId == ACTOR_EN_IK && play->sceneNum == SCENE_JYASINZOU) {
            if (*params == 6657) {
                *posX = *posX + 150;
            } else if (*params == 6401) {
                *posX = *posX - 150;
            }
        }

        // Move like-likes in MQ Jabu Jabu down into the room as they otherwise get stuck on Song of Time blocks.
        if (*actorId == ACTOR_EN_RR && play->sceneNum == SCENE_BDAN && play->roomCtx.curRoom.num == 11) {
            if (*posX == 1003) {
                *posX = *posX - 75;
            } else {
                *posX = *posX + 75;
            }
            *posY = *posY - 200;
        }

        // Do a raycast from the original position of the actor to find the ground below it, then try to place
        // the new actor on the ground. This way enemies don't spawn very high in the sky, and gives us control
        // over height offsets per enemy from a proven grounded position.
        CollisionPoly poly;
        Vec3f pos;
        f32 raycastResult;

        pos.x = *posX;
        pos.y = *posY + 50;
        pos.z = *posZ;
        raycastResult = BgCheck_AnyRaycastFloor1(&play->colCtx, &poly, &pos);

        // If ground is found below actor, move actor to that height.
        if (raycastResult > BGCHECK_Y_MIN) {
            *posY = raycastResult;
        }

        // Get randomized enemy ID and parameter.
        uint32_t seed = play->sceneNum + *actorId + (int)*posX + (int)*posY + (int)*posZ + *rotX + *rotY + *rotZ + *params;
        EnemyEntry randomEnemy = GetRandomizedEnemyEntry(seed);

        int8_t timesRandomized = 1;

        // While randomized enemy isn't allowed in certain situations, randomize again.
        while (!IsEnemyAllowedToSpawn(play->sceneNum, play->roomCtx.curRoom.num, randomEnemy)) {
            randomEnemy = GetRandomizedEnemyEntry(seed + timesRandomized);
            timesRandomized++;
        }

        *actorId = randomEnemy.id;
        *params = randomEnemy.params;

        // Straighten out enemies so they aren't flipped on their sides when the original spawn is.
        *rotX = 0;

        switch (*actorId) {
            // When spawning big jellyfish, spawn it up high.
            case ACTOR_EN_VALI:
                *posY = *posY + 300;
                break;
            // Spawn peahat off the ground, otherwise it kills itself by colliding with the ground.
            case ACTOR_EN_PEEHAT:
                if (*params == 1) {
                    *posY = *posY + 100;
                }
                break;
            // Spawn skulltulas off the ground.
            case ACTOR_EN_ST:
                *posY = *posY + 200;
                break;
            // Spawn flying enemies off the ground.
            case ACTOR_EN_FIREFLY:
            case ACTOR_EN_BILI:
            case ACTOR_EN_BB:
            case ACTOR_EN_CLEAR_TAG:
            case ACTOR_EN_CROW:
                *posY = *posY + 75;
                break;
            default:
                break;
        }
    }

    // Enemy finished randomization process.
    return 1;
}

EnemyEntry GetRandomizedEnemyEntry(uint32_t seed) {
    if (CVar_GetS32("gSeededRandomizedEnemies", 0) && gSaveContext.n64ddFlag) {
        uint32_t finalSeed = seed + gSaveContext.seedIcons[0] + gSaveContext.seedIcons[1] + gSaveContext.seedIcons[2] +
                        gSaveContext.seedIcons[3] + gSaveContext.seedIcons[4];
        Random_Init(finalSeed);
        uint32_t randomNumber = Random(0, RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE);
        return randomizedEnemySpawnTable[randomNumber];
    } else {
        uint32_t randomNumber = rand() + seed;
        return randomizedEnemySpawnTable[randomNumber % RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE];
    }
}

bool IsEnemyFoundToRandomize(int16_t sceneNum, int8_t roomNum, int16_t actorId, int16_t params, float posX) {

    uint32_t isMQ = ResourceMgr_IsSceneMasterQuest(sceneNum);

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
                    return (params >= 0 && params <= 9);
                // Only randomize Skullwalltulas, not Golden Skulltulas.
                case ACTOR_EN_SW:
                    return (params == 0);
                // Don't randomize Nabooru because it'll break the cutscene and the door.
                // Don't randomize Iron Knuckle in MQ Spirit Trial because it's needed to
                // break the thrones in the room to access a button.
                case ACTOR_EN_IK:
                    return (params != 1280 && !(isMQ && sceneNum == SCENE_GANONTIKA && roomNum == 17));
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
                    return (params != 1 && !(sceneNum == SCENE_HAKADAN && roomNum == 21));
                // Only randomize the enemy variant of Armos Statue.
                // Leave one Armos unrandomized in the Spirit Temple room where an armos is needed to push down a button
                case ACTOR_EN_AM:
                    return ((params == -1 || params == 255) && !(sceneNum == SCENE_JYASINZOU && posX == 2141));
                // Don't randomize Shell Blades and Spikes in the underwater portion in Water Temple as it's impossible to kill
                // most other enemies underwater with just hookshot and they're required to be killed for a grate to open.
                case ACTOR_EN_SB:
                case ACTOR_EN_NY:
                    return (!(!isMQ && sceneNum == SCENE_MIZUSIN && roomNum == 2));
                default:
                    return 1;
            }
        }
    }

    // If no enemy is found, don't randomize the actor.
    return 0;
}

bool IsEnemyAllowedToSpawn(int16_t sceneNum, int8_t roomNum, EnemyEntry enemy) {

    uint32_t isMQ = ResourceMgr_IsSceneMasterQuest(sceneNum);

    // Freezard - Child Link can only kill this with jump slash deku sticks or other equipment like bombs.
    // Beamos - Needs bombs.
    // Shell Blade & Spike - Child link can't kill these with sword or deku stick.
    // Arwing & Dark Link - Both go out of bounds way too easily, softlocking the player.
    // Wallmaster - Not easily visible, often makes players think they're softlocked and that there's no enemies left.
    bool enemiesToExcludeClearRooms = enemy.id == ACTOR_EN_FZ || enemy.id == ACTOR_EN_VM || enemy.id == ACTOR_EN_SB ||
                                      enemy.id == ACTOR_EN_NY || enemy.id == ACTOR_EN_CLEAR_TAG ||
                                      enemy.id == ACTOR_EN_WALLMAS || enemy.id == ACTOR_EN_TORCH2;

    // Bari - Spawns 3 more enemies, potentially extremely difficult in timed rooms.
    bool enemiesToExcludeTimedRooms = enemiesToExcludeClearRooms || enemy.id == ACTOR_EN_VALI;

    switch (sceneNum) {
        // Deku Tree
        case SCENE_YDAN:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 1 || roomNum == 9)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 4 || roomNum == 6 || roomNum == 9 || roomNum == 10)));
        // Dodongo's Cavern
        case SCENE_DDAN:
            return (!(!isMQ && enemiesToExcludeClearRooms && roomNum == 15) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 5 || roomNum == 13 || roomNum == 14)));
        // Jabu Jabu
        case SCENE_BDAN:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 8 || roomNum == 9)) &&
                    !(!isMQ && enemiesToExcludeTimedRooms && roomNum == 12) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 11 || roomNum == 14)));
        // Forest Temple
        case SCENE_BMORI1:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 6 || roomNum == 10 || roomNum == 18 || roomNum == 21)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 5 || roomNum == 6 || roomNum == 18 || roomNum == 21)));
        // Fire Temple
        case SCENE_HIDAN:
            return (!(!isMQ && enemiesToExcludeClearRooms && roomNum == 15) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 15 || roomNum == 17 || roomNum == 18)));
        // Water Temple
        case SCENE_MIZUSIN:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 13 || roomNum == 18 || roomNum == 19)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 13 || roomNum == 18)));
        // Spirit Temple
        case SCENE_JYASINZOU:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 1 || roomNum == 10 || roomNum == 17 || roomNum == 20)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 1 || roomNum == 2 || roomNum == 4 || roomNum == 10 || roomNum == 15 || roomNum == 19 || roomNum == 20)));
        // Shadow Temple
        case SCENE_HAKADAN:
            return (!(!isMQ && enemiesToExcludeClearRooms && 
                        (roomNum == 1 || roomNum == 7 || roomNum == 11 || roomNum == 14 || roomNum == 16 || roomNum == 17 || roomNum == 19 || roomNum == 20)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 1 || roomNum == 6 || roomNum == 7 || roomNum == 11 || roomNum == 14 || roomNum == 20)));
        // Ganon's Castle Trials
        case SCENE_GANONTIKA:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 2 || roomNum == 5 || roomNum == 9)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 0 || roomNum == 2 || roomNum == 5 || roomNum == 9)));
        // Ice Caverns
        case SCENE_ICE_DOUKUTO:
            return (!(!isMQ && enemiesToExcludeClearRooms && (roomNum == 1 || roomNum == 7)) &&
                    !(isMQ && enemiesToExcludeClearRooms && (roomNum == 3 || roomNum == 7)));
        // Bottom of the Well
        // Exclude Dark Link from room with holes in the floor because it can pull you in a like-like making the player fall down.
        case SCENE_HAKADANCH:
            return (!(!isMQ && enemy.id == ACTOR_EN_TORCH2 && roomNum == 3));
        // Don't allow Dark Link in areas with lava void out zones as it voids out the player as well.
        // Gerudo Training Ground.
        case SCENE_MEN:
            return (!(enemy.id == ACTOR_EN_TORCH2 && roomNum == 6) &&
                    !(!isMQ && enemiesToExcludeTimedRooms && (roomNum == 1 || roomNum == 7)) &&
                    !(!isMQ && enemiesToExcludeClearRooms && (roomNum == 3 || roomNum == 5 || roomNum == 10)) &&
                    !(isMQ && enemiesToExcludeTimedRooms && (roomNum == 1 || roomNum == 3 || roomNum == 5 || roomNum == 7)) &&
                    !(isMQ && enemiesToExcludeClearRooms && roomNum == 10));
        // Don't allow certain enemies in Ganon's Tower because they would spawn up on the ceilling,
        // becoming impossible to kill.
        // Ganon's Tower.
        case SCENE_GANON:
            return (!(enemiesToExcludeClearRooms || enemy.id == ACTOR_EN_VALI || (enemy.id == ACTOR_EN_ZF && enemy.params == -1)));
        // Ganon's Tower Escape.
        case SCENE_GANON_SONOGO:
            return (!((enemiesToExcludeTimedRooms || (enemy.id == ACTOR_EN_ZF && enemy.params == -1)) && roomNum == 1));
        // Don't allow big stalchildren, big peahats and the large Bari (jellyfish) during the Gohma fight because they can clip into Gohma
        // and it crashes the game. Likely because Gohma on the ceilling can't handle collision with other enemies.
        case SCENE_YDAN_BOSS:
            return (!enemiesToExcludeTimedRooms && !(enemy.id == ACTOR_EN_SKB && enemy.params == 20) &&
                    !(enemy.id == ACTOR_EN_PEEHAT && enemy.params == -1));
        // Grottos.
        case SCENE_KAKUSIANA:
            return (!(enemiesToExcludeClearRooms && (roomNum == 2 || roomNum == 7)));
        // Royal Grave.
        case SCENE_HAKAANA_OUKE:
            return (!(enemiesToExcludeClearRooms && roomNum == 0));
        // Don't allow Dark Link in areas with lava void out zones as it voids out the player as well.
        // Death Mountain Crater.
        case SCENE_SPOT17:
            return (enemy.id != ACTOR_EN_TORCH2);
        default:
            return 1;
    }
}
