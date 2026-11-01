#pragma once

// X-macro list of every NPC soul.
//   NPC_SOUL(ID, "article", "Name", CATEGORY, ADVANCEMENT)
// ARTICLE is "" or "the ", used in hint text ("the soul of the Frogs").
// ID expands to RG_<ID>_SOUL, RAND_INF_<ID>_SOUL, RHT_<ID>_SOUL and RSK_STARTING_<ID>_SOUL.
// CATEGORY is one of MERCHANTS, MINIGAMES, GATEKEEPERS, STORY, TOWNSFOLK and picks the
// RSK_SHUFFLE_NPC_SOULS_<CATEGORY> setting that puts the soul in the pool.
// ADVANCEMENT is false for red-herring souls that gate no check: they still look and hint
// like real souls, but the fill treats them as junk.
//
// New entries go at the END of this list: the generated enums are saved by position.

// clang-format off
#define NPC_SOUL_LIST(NPC_SOUL)                                                      \
    /* Merchants */                                                                  \
    NPC_SOUL(KOKIRI_SHOPKEEPER,          "",     "Kokiri Shopkeeper",          MERCHANTS,   true)  \
    NPC_SOUL(POTION_SHOPKEEPER,          "",     "Potion Shopkeeper",          MERCHANTS,   true)  \
    NPC_SOUL(BAZAAR_SHOPKEEPER,          "",     "Bazaar Shopkeeper",          MERCHANTS,   true)  \
    NPC_SOUL(GORON_SHOPKEEPER,           "",     "Goron Shopkeeper",           MERCHANTS,   true)  \
    NPC_SOUL(ZORA_SHOPKEEPER,            "",     "Zora Shopkeeper",            MERCHANTS,   true)  \
    NPC_SOUL(BOMBCHU_SHOPKEEPER,         "",     "Bombchu Shopkeeper",         MERCHANTS,   true)  \
    NPC_SOUL(MASK_SALESMAN,              "",     "Happy Mask Salesman",        MERCHANTS,   true)  \
    NPC_SOUL(BEAN_SALESMAN,              "",     "Bean Salesman",              MERCHANTS,   true)  \
    NPC_SOUL(CARPET_SALESMAN,            "",     "Carpet Salesman",            MERCHANTS,   true)  \
    NPC_SOUL(MEDIGORON,                  "",     "Medigoron",                  MERCHANTS,   true)  \
    NPC_SOUL(GRANNY,                     "",     "Granny",                     MERCHANTS,   true)  \
    NPC_SOUL(BEGGAR,                     "",     "Beggar",                     MERCHANTS,   true)  \
    NPC_SOUL(POE_COLLECTOR,              "",     "Poe Collector",              MERCHANTS,   true)  \
    NPC_SOUL(GROG,                       "",     "Grog",                       MERCHANTS,   true)  \
    NPC_SOUL(FADO,                       "",     "Fado",                       MERCHANTS,   true)  \
    NPC_SOUL(MUTOH,                      "",     "Mutoh",                      MERCHANTS,   true)  \
    NPC_SOUL(BIGGORON,                   "",     "Biggoron",                   MERCHANTS,   true)  \
    /* Minigames */                                                                  \
    NPC_SOUL(SHOOTING_GALLERY_MAN,       "",     "Shooting Gallery Man",       MINIGAMES,   true)  \
    NPC_SOUL(BOMBCHU_BOWLING_LADY,       "",     "Bombchu Bowling Lady",       MINIGAMES,   true)  \
    NPC_SOUL(TREASURE_CHEST_MAN,         "",     "Treasure Chest Game Man",    MINIGAMES,   true)  \
    NPC_SOUL(DIVING_GAME_ZORA,           "",     "Diving Game Zora",           MINIGAMES,   true)  \
    NPC_SOUL(DAMPE,                      "",     "Dampe",                      MINIGAMES,   true)  \
    NPC_SOUL(HORSEBACK_ARCHER,           "",     "Horseback Archery Gerudo",   MINIGAMES,   true)  \
    NPC_SOUL(FISHING_POND_OWNER,         "",     "Fishing Pond Owner",         MINIGAMES,   true)  \
    NPC_SOUL(SCIENTIST,                  "",     "Lakeside Scientist",         MINIGAMES,   true)  \
    NPC_SOUL(HOT_RODDER,                 "",     "Hot Rodder Goron",           MINIGAMES,   true)  \
    NPC_SOUL(SKULL_KID,                  "",     "Skull Kid",                  MINIGAMES,   true)  \
    NPC_SOUL(FROGS,                      "the ", "Frogs",                      MINIGAMES,   true)  \
    NPC_SOUL(ANJU,                       "",     "Anju",                       MINIGAMES,   true)  \
    /* Gatekeepers */                                                                \
    NPC_SOUL(KAKARIKO_GATEKEEPER,        "",     "Kakariko Gate Guard",        GATEKEEPERS, true)  \
    NPC_SOUL(MIDO,                       "",     "Mido",                       GATEKEEPERS, true)  \
    NPC_SOUL(KOKIRI_EXIT_GUARD,          "",     "Forest Exit Kokiri",         GATEKEEPERS, true)  \
    NPC_SOUL(KING_ZORA,                  "",     "King Zora",                  GATEKEEPERS, true)  \
    NPC_SOUL(WASTELAND_GATEKEEPER,       "",     "Wasteland Gatekeeper",       GATEKEEPERS, true)  \
    NPC_SOUL(TRAINING_GROUND_GATEKEEPER, "",     "Training Ground Gatekeeper", GATEKEEPERS, true)  \
    NPC_SOUL(GERUDO_CARD_GIVER,          "",     "Gerudo Guard",               GATEKEEPERS, true)  \
    NPC_SOUL(ICHIRO,                     "",     "Ichiro",                     GATEKEEPERS, true)  \
    NPC_SOUL(SABOORO,                    "",     "Sabooro",                    GATEKEEPERS, true)  \
    NPC_SOUL(JIRO,                       "",     "Jiro",                       GATEKEEPERS, true)  \
    NPC_SOUL(SHIRO,                      "",     "Shiro",                      GATEKEEPERS, true)  \
    NPC_SOUL(TALON,                      "",     "Talon",                      GATEKEEPERS, true)  \
    NPC_SOUL(MALON,                      "",     "Malon",                      GATEKEEPERS, true)  \
    NPC_SOUL(INGO,                       "",     "Ingo",                       GATEKEEPERS, true)  \
    NPC_SOUL(KAEPORA_GAEBORA,            "",     "Kaepora Gaebora",            GATEKEEPERS, true)  \
    /* Story */                                                                      \
    NPC_SOUL(SARIA,                      "",     "Saria",                      STORY,       true)  \
    NPC_SOUL(DARUNIA,                    "",     "Darunia",                    STORY,       true)  \
    NPC_SOUL(RUTO,                       "",     "Ruto",                       STORY,       true)  \
    NPC_SOUL(NABOORU,                    "",     "Nabooru",                    STORY,       true)  \
    NPC_SOUL(ZELDA,                      "",     "Zelda",                      STORY,       true)  \
    NPC_SOUL(IMPA,                       "",     "Impa",                       STORY,       true)  \
    NPC_SOUL(RAURU,                      "",     "Rauru",                      STORY,       true)  \
    NPC_SOUL(GREAT_FAIRIES,              "the ", "Great Fairies",              STORY,       true)  \
    /* Townsfolk */                                                                  \
    NPC_SOUL(WINDMILL_MAN,               "",     "Guru-Guru",                  TOWNSFOLK,   true)  \
    NPC_SOUL(MAN_ON_ROOF,                "",     "Man on the Roof",            TOWNSFOLK,   true)  \
    NPC_SOUL(DOG_LADY,                   "",     "Dog Lady",                   TOWNSFOLK,   true)  \
    NPC_SOUL(SKULLTULA_FAMILY,           "the ", "Cursed Family",              TOWNSFOLK,   true)  \
    NPC_SOUL(HYLIAN_GUARDS,              "the ", "Hylian Guards",              TOWNSFOLK,   true)  \
    NPC_SOUL(RUNNING_MAN,                "",     "Running Man",                TOWNSFOLK,   true)  \
    NPC_SOUL(GORON_LINK,                 "",     "Link the Goron",             TOWNSFOLK,   true)
// clang-format on
