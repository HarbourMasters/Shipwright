#pragma once

// When using RTA timing
    // get the diff since the save was created,
    // unless the game is complete in which we use the defeated ganon timestamp
// When not using RTA timing
    // Total gameplay time is tracked in tenths of seconds
    // I.E. game time counts frames at 20fps/2, pause time counts frames at 30fps/3
    // Frame counts in z_play.c and z_kaleido_scope_call.c
#define GAMEPLAYSTAT_TOTAL_TIME (gSaveContext.sohStats.rtaTiming ?\
    (!gSaveContext.sohStats.gameComplete ?\
        (!gSaveContext.sohStats.fileCreatedAt ? 0 : ((GetUnixTimestamp() - gSaveContext.sohStats.fileCreatedAt) / 100)) :\
        (gSaveContext.sohStats.itemTimestamp[TIMESTAMP_DEFEAT_GANON])) :\
    (gSaveContext.sohStats.playTimer / 2 + gSaveContext.sohStats.pauseTimer / 3))
#define CURRENT_MODE_TIMER (CVarGetInteger("gGameplayStats.RoomBreakdown", 0) ?\
    gSaveContext.sohStats.roomTimer :\
    gSaveContext.sohStats.sceneTimer)

void InitStatTracker();
char* GameplayStats_GetCurrentTime();

typedef enum {
    // 0x00 to 0x9B (0 to 155) used for getting items,
    // piggybacked off enum "ItemID" in z64item.h

    /* 0xA0 */ TIMESTAMP_DEFEAT_GOHMA = 0xA0,   // z_boss_goma.c
    /* 0xA1 */ TIMESTAMP_DEFEAT_KING_DODONGO,   // z_boss_dodongo.c
    /* 0xA2 */ TIMESTAMP_DEFEAT_BARINADE,       // z_boss_va.c
    /* 0xA3 */ TIMESTAMP_DEFEAT_PHANTOM_GANON,  // z_boss_ganondrof.c
    /* 0xA4 */ TIMESTAMP_DEFEAT_VOLVAGIA,       // z_boss_fd2.c
    /* 0xA5 */ TIMESTAMP_DEFEAT_MORPHA,         // z_boss_mo.c
    /* 0xA6 */ TIMESTAMP_DEFEAT_BONGO_BONGO,    // z_boss_sst.c
    /* 0xA7 */ TIMESTAMP_DEFEAT_TWINROVA,       // z_boss_tw.c
    /* 0xA8 */ TIMESTAMP_DEFEAT_GANONDORF,      // z_boss_ganon.c
    /* 0xA9 */ TIMESTAMP_DEFEAT_GANON,          // z_boss_ganon2.c
    /* 0xA9 */ TIMESTAMP_BOSSRUSH_FINISH,       // z_boss_ganon2.c
    /* 0xAA */ TIMESTAMP_FOUND_GREG,            // z_parameter.c
    /* 0xAA */ TIMESTAMP_TRIFORCE_COMPLETED,    // z_parameter.c
    /* 0xAB */ TIMESTAMP_MAX

}GameplayStatTimestamp;

typedef enum {
    // Enemies defeated
    COUNT_ENEMIES_DEFEATED_ANUBIS,               // EN_ANUBICE
    COUNT_ENEMIES_DEFEATED_ARMOS,                // EN_AM
    COUNT_ENEMIES_DEFEATED_ARWING,               // EN_CLEAR_TAG
    COUNT_ENEMIES_DEFEATED_BARI,                 // EN_VALI
    COUNT_ENEMIES_DEFEATED_BEAMOS,               // EN_VM
    COUNT_ENEMIES_DEFEATED_BIG_OCTO,             // EN_BIGOKUTA
    COUNT_ENEMIES_DEFEATED_BIRI,                 // EN_BILI
    COUNT_ENEMIES_DEFEATED_BUBBLE_GREEN,         // EN_BB
    COUNT_ENEMIES_DEFEATED_BUBBLE_BLUE,          // EN_BB
    COUNT_ENEMIES_DEFEATED_BUBBLE_WHITE,         // EN_BB
    COUNT_ENEMIES_DEFEATED_BUBBLE_RED,           // EN_BB
    COUNT_ENEMIES_DEFEATED_BUSINESS_SCRUB,       // EN_DNS
    COUNT_ENEMIES_DEFEATED_DARK_LINK,            // EN_TORCH2
    COUNT_ENEMIES_DEFEATED_DEAD_HAND,            // EN_DH
    COUNT_ENEMIES_DEFEATED_DEKU_BABA,            // EN_DEKUBABA
    COUNT_ENEMIES_DEFEATED_DEKU_BABA_BIG,        // EN_DEKUBABA
    COUNT_ENEMIES_DEFEATED_DEKU_SCRUB,           // EN_HINTNUTS
    COUNT_ENEMIES_DEFEATED_DINOLFOS,             // EN_ZF
    COUNT_ENEMIES_DEFEATED_DODONGO,              // EN_DODONGO
    COUNT_ENEMIES_DEFEATED_DODONGO_BABY,         // EN_DODOJR
    COUNT_ENEMIES_DEFEATED_DOOR_TRAP,            // DOOR_KILLER
    COUNT_ENEMIES_DEFEATED_FLARE_DANCER,         // EN_FD
    COUNT_ENEMIES_DEFEATED_FLOORMASTER,          // EN_FLOORMAS
    COUNT_ENEMIES_DEFEATED_FLYING_POT,           // EN_TUBO_TRAP
    COUNT_ENEMIES_DEFEATED_FLOOR_TILE,           // EN_YUKABYUN
    COUNT_ENEMIES_DEFEATED_FREEZARD,             // EN_FZ
    COUNT_ENEMIES_DEFEATED_GERUDO_THIEF,         // EN_GELDB
    COUNT_ENEMIES_DEFEATED_GIBDO,                // EN_RD
    COUNT_ENEMIES_DEFEATED_GOHMA_LARVA,          // EN_GOMA
    COUNT_ENEMIES_DEFEATED_GUAY,                 // EN_CROW
    COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE,         // EN_IK
    COUNT_ENEMIES_DEFEATED_IRON_KNUCKLE_NABOORU, // EN_IK
    COUNT_ENEMIES_DEFEATED_KEESE,                // EN_FIREFLY
    COUNT_ENEMIES_DEFEATED_KEESE_FIRE,           // EN_FIREFLY
    COUNT_ENEMIES_DEFEATED_KEESE_ICE,            // EN_FIREFLY
    COUNT_ENEMIES_DEFEATED_LEEVER,               // EN_REEBA
    COUNT_ENEMIES_DEFEATED_LEEVER_BIG,           // EN_REEBA
    COUNT_ENEMIES_DEFEATED_LIKE_LIKE,            // EN_RR
    COUNT_ENEMIES_DEFEATED_LIZALFOS,             // EN_ZF
    COUNT_ENEMIES_DEFEATED_MAD_SCRUB,            // EN_DEKUNUTS
    COUNT_ENEMIES_DEFEATED_MOBLIN,               // EN_MB
    COUNT_ENEMIES_DEFEATED_MOBLIN_CLUB,          // EN_MB
    COUNT_ENEMIES_DEFEATED_OCTOROK,              // EN_OKUTA
    COUNT_ENEMIES_DEFEATED_PARASITIC_TENTACLE,   // EN_BA
    COUNT_ENEMIES_DEFEATED_PEAHAT,               // EN_PEEHAT
    COUNT_ENEMIES_DEFEATED_PEAHAT_LARVA,         // EN_PEEHAT
    COUNT_ENEMIES_DEFEATED_POE,                  // EN_POH & EN_PO_FIELD
    COUNT_ENEMIES_DEFEATED_POE_BIG,              // EN_PO_FIELD
    COUNT_ENEMIES_DEFEATED_POE_COMPOSER,         // EN_POH
    COUNT_ENEMIES_DEFEATED_POE_SISTERS,          // EN_PO_SISTERS
    COUNT_ENEMIES_DEFEATED_REDEAD,               // EN_RD
    COUNT_ENEMIES_DEFEATED_SHABOM,               // EN_BUBBLE
    COUNT_ENEMIES_DEFEATED_SHELLBLADE,           // EN_SB
    COUNT_ENEMIES_DEFEATED_SKULLTULA,            // EN_ST
    COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG,        // EN_ST
    COUNT_ENEMIES_DEFEATED_SKULLTULA_GOLD,       // EN_SW
    COUNT_ENEMIES_DEFEATED_SKULLWALLTULA,        // EN_SW
    COUNT_ENEMIES_DEFEATED_SKULL_KID,            // EN_SKJ
    COUNT_ENEMIES_DEFEATED_SPIKE,                // EN_NY
    COUNT_ENEMIES_DEFEATED_STALCHILD,            // EN_SKB
    COUNT_ENEMIES_DEFEATED_STALFOS,              // EN_TEST
    COUNT_ENEMIES_DEFEATED_STINGER,              // EN_WEIYER
    COUNT_ENEMIES_DEFEATED_TAILPASARAN,          // EN_TP
    COUNT_ENEMIES_DEFEATED_TEKTITE_BLUE,         // EN_TITE
    COUNT_ENEMIES_DEFEATED_TEKTITE_RED,          // EN_TITE
    COUNT_ENEMIES_DEFEATED_TORCH_SLUG,           // EN_BW
    COUNT_ENEMIES_DEFEATED_WALLMASTER,           // EN_WALLMAS
    COUNT_ENEMIES_DEFEATED_WITHERED_DEKU_BABA,   // EN_KAREBABA
    COUNT_ENEMIES_DEFEATED_WOLFOS,               // EN_WF
    COUNT_ENEMIES_DEFEATED_WOLFOS_WHITE,         // EN_WF
    // Ammo used (z_parameter.c)
    COUNT_AMMO_USED_STICK,
    COUNT_AMMO_USED_NUT,
    COUNT_AMMO_USED_BOMB,
    COUNT_AMMO_USED_ARROW,
    COUNT_AMMO_USED_SEED,
    COUNT_AMMO_USED_BOMBCHU,
    COUNT_AMMO_USED_BEAN,
    // Buttons pressed (z_play.c)
    COUNT_BUTTON_PRESSES_A,
    COUNT_BUTTON_PRESSES_B,
    COUNT_BUTTON_PRESSES_L,
    COUNT_BUTTON_PRESSES_R,
    COUNT_BUTTON_PRESSES_Z,
    COUNT_BUTTON_PRESSES_CUP,
    COUNT_BUTTON_PRESSES_CRIGHT,
    COUNT_BUTTON_PRESSES_CDOWN,
    COUNT_BUTTON_PRESSES_CLEFT,
    COUNT_BUTTON_PRESSES_DUP,
    COUNT_BUTTON_PRESSES_DRIGHT,
    COUNT_BUTTON_PRESSES_DDOWN,
    COUNT_BUTTON_PRESSES_DLEFT,
    COUNT_BUTTON_PRESSES_START,
    // Other counts
    COUNT_RUPEES_COLLECTED, // z_parameter.c
    COUNT_RUPEES_SPENT,     // z_parameter.c
    COUNT_CHESTS_OPENED,    // z_en_box.c
    COUNT_DAMAGE_TAKEN,     // z_parameter.c
    COUNT_ICE_TRAPS,        // z_player.c
    COUNT_ROLLS,            // z_player.c
    COUNT_BONKS,            // z_player.c
    COUNT_PAUSES,           // z_kaleido_scope_call.c
    COUNT_STEPS,            // z_player.c
    COUNT_POTS_BROKEN,      // z_obj_tsubo.c
    COUNT_BUSHES_CUT,       // z_en_kusa.c
    COUNT_SWORD_SWINGS,     // z_player.c
    COUNT_SIDEHOPS,         // z_player.c
    COUNT_BACKFLIPS,        // z_player.c
    COUNT_TIME_BUNNY_HOOD,  // z_play.c

    COUNT_MAX

} GameplayStatCount;