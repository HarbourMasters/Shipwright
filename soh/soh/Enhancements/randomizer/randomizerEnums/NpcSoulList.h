#pragma once

// X-macro list of every NPC soul.
//   NPC_SOUL(ID, "article", "Name", "TRACKER", CATEGORY, ADVANCEMENT)
// ARTICLE is "" or "the ", used in hint text ("the soul of the Frogs").
// ID expands to RG_<ID>_SOUL, RAND_INF_<ID>_SOUL, RHT_<ID>_SOUL and RSK_STARTING_<ID>_SOUL.
// TRACKER is the short label drawn under the soul's icon in the item tracker (6 characters or fewer).
// CATEGORY is one of MERCHANTS, MINIGAMES, GATEKEEPERS, STORY, TOWNSFOLK and picks the
// RSK_SHUFFLE_NPC_SOULS_<CATEGORY> setting that puts the soul in the pool.
// ADVANCEMENT is false for red-herring souls that gate no check: they still look and hint
// like real souls, but the fill treats them as junk.
//
// New entries go at the END of this list: the generated enums are saved by position.

// clang-format off
#define NPC_SOUL_LIST(NPC_SOUL)                                                      \
    /* Merchants */                                                                  \
    NPC_SOUL(KOKIRI_SHOPKEEPER,          "",     "Kokiri Shopkeeper",          "KF SHP", MERCHANTS,   true)  \
    NPC_SOUL(POTION_SHOPKEEPER,          "",     "Potion Shopkeeper",          "POTION", MERCHANTS,   true)  \
    NPC_SOUL(BAZAAR_SHOPKEEPER,          "",     "Bazaar Shopkeeper",          "BAZAAR", MERCHANTS,   true)  \
    NPC_SOUL(GORON_SHOPKEEPER,           "",     "Goron Shopkeeper",           "GC SHP", MERCHANTS,   true)  \
    NPC_SOUL(ZORA_SHOPKEEPER,            "",     "Zora Shopkeeper",            "ZD SHP", MERCHANTS,   true)  \
    NPC_SOUL(BOMBCHU_SHOPKEEPER,         "",     "Bombchu Shopkeeper",         "CHU",    MERCHANTS,   true)  \
    NPC_SOUL(MASK_SALESMAN,              "",     "Happy Mask Salesman",        "MASK",   MERCHANTS,   true)  \
    NPC_SOUL(BEAN_SALESMAN,              "",     "Bean Salesman",              "BEAN",   MERCHANTS,   true)  \
    NPC_SOUL(CARPET_SALESMAN,            "",     "Carpet Salesman",            "CARPET", MERCHANTS,   true)  \
    NPC_SOUL(MEDIGORON,                  "",     "Medigoron",                  "MEDI",   MERCHANTS,   true)  \
    NPC_SOUL(GRANNY,                     "",     "Granny",                     "GRANNY", MERCHANTS,   true)  \
    NPC_SOUL(BEGGAR,                     "",     "Beggar",                     "BEGGAR", MERCHANTS,   true)  \
    NPC_SOUL(POE_COLLECTOR,              "",     "Poe Collector",              "POES",   MERCHANTS,   true)  \
    NPC_SOUL(GROG,                       "",     "Grog",                       "GROG",   MERCHANTS,   true)  \
    NPC_SOUL(FADO,                       "",     "Fado",                       "FADO",   MERCHANTS,   true)  \
    NPC_SOUL(MUTOH,                      "",     "Mutoh",                      "MUTOH",  MERCHANTS,   true)  \
    NPC_SOUL(BIGGORON,                   "",     "Biggoron",                   "BIGGO",  MERCHANTS,   true)  \
    /* Minigames */                                                                  \
    NPC_SOUL(SHOOTING_GALLERY_MAN,       "",     "Shooting Gallery Man",       "SHOOT",  MINIGAMES,   true)  \
    NPC_SOUL(BOMBCHU_BOWLING_LADY,       "",     "Bombchu Bowling Lady",       "BOWL",   MINIGAMES,   true)  \
    NPC_SOUL(TREASURE_CHEST_MAN,         "",     "Treasure Chest Game Man",    "TCG",    MINIGAMES,   true)  \
    NPC_SOUL(DIVING_GAME_ZORA,           "",     "Diving Game Zora",           "DIVE",   MINIGAMES,   true)  \
    NPC_SOUL(DAMPE,                      "",     "Dampe",                      "DAMPE",  MINIGAMES,   true)  \
    NPC_SOUL(HORSEBACK_ARCHER,           "",     "Horseback Archery Gerudo",   "HBA",    MINIGAMES,   true)  \
    NPC_SOUL(FISHING_POND_OWNER,         "",     "Fishing Pond Owner",         "FISH",   MINIGAMES,   true)  \
    NPC_SOUL(SCIENTIST,                  "",     "Lakeside Scientist",         "LAB",    MINIGAMES,   true)  \
    NPC_SOUL(HOT_RODDER,                 "",     "Hot Rodder Goron",           "RODDER", MINIGAMES,   true)  \
    NPC_SOUL(SKULL_KID,                  "",     "Skull Kid",                  "SKULL",  MINIGAMES,   true)  \
    NPC_SOUL(FROGS,                      "the ", "Frogs",                      "FROGS",  MINIGAMES,   true)  \
    NPC_SOUL(ANJU,                       "",     "Anju",                       "ANJU",   MINIGAMES,   true)  \
    /* Gatekeepers */                                                                \
    NPC_SOUL(KAKARIKO_GATEKEEPER,        "",     "Kakariko Gate Guard",        "KAK GT", GATEKEEPERS, true)  \
    NPC_SOUL(MIDO,                       "",     "Mido",                       "MIDO",   GATEKEEPERS, true)  \
    NPC_SOUL(KOKIRI_EXIT_GUARD,          "",     "Forest Exit Kokiri",         "EXIT",   GATEKEEPERS, true)  \
    NPC_SOUL(KING_ZORA,                  "",     "King Zora",                  "KZ",     GATEKEEPERS, true)  \
    NPC_SOUL(WASTELAND_GATEKEEPER,       "",     "Wasteland Gatekeeper",       "HW GT",  GATEKEEPERS, true)  \
    NPC_SOUL(TRAINING_GROUND_GATEKEEPER, "",     "Training Ground Gatekeeper", "GTG GT", GATEKEEPERS, true)  \
    NPC_SOUL(GERUDO_CARD_GIVER,          "",     "Gerudo Guard",               "CARD",   GATEKEEPERS, true)  \
    NPC_SOUL(ICHIRO,                     "",     "Ichiro",                     "ICHIRO", GATEKEEPERS, true)  \
    NPC_SOUL(SABOORO,                    "",     "Sabooro",                    "SABOO",  GATEKEEPERS, true)  \
    NPC_SOUL(JIRO,                       "",     "Jiro",                       "JIRO",   GATEKEEPERS, true)  \
    NPC_SOUL(SHIRO,                      "",     "Shiro",                      "SHIRO",  GATEKEEPERS, true)  \
    NPC_SOUL(TALON,                      "",     "Talon",                      "TALON",  GATEKEEPERS, true)  \
    NPC_SOUL(MALON,                      "",     "Malon",                      "MALON",  GATEKEEPERS, true)  \
    NPC_SOUL(INGO,                       "",     "Ingo",                       "INGO",   GATEKEEPERS, true)  \
    NPC_SOUL(KAEPORA_GAEBORA,            "",     "Kaepora Gaebora",            "OWL",    GATEKEEPERS, true)  \
    /* Story */                                                                      \
    NPC_SOUL(SARIA,                      "",     "Saria",                      "SARIA",  STORY,       true)  \
    NPC_SOUL(DARUNIA,                    "",     "Darunia",                    "DARU",   STORY,       true)  \
    NPC_SOUL(RUTO,                       "",     "Ruto",                       "RUTO",   STORY,       true)  \
    NPC_SOUL(NABOORU,                    "",     "Nabooru",                    "NABOO",  STORY,       true)  \
    NPC_SOUL(ZELDA,                      "",     "Zelda",                      "ZELDA",  STORY,       true)  \
    NPC_SOUL(IMPA,                       "",     "Impa",                       "IMPA",   STORY,       true)  \
    NPC_SOUL(RAURU,                      "",     "Rauru",                      "RAURU",  STORY,       true)  \
    NPC_SOUL(GREAT_FAIRIES,              "the ", "Great Fairies",              "FAIRY",  STORY,       true)  \
    /* Townsfolk */                                                                  \
    NPC_SOUL(WINDMILL_MAN,               "",     "Guru-Guru",                  "GURU",   TOWNSFOLK,   true)  \
    NPC_SOUL(MAN_ON_ROOF,                "",     "Man on the Roof",            "ROOF",   TOWNSFOLK,   true)  \
    NPC_SOUL(DOG_LADY,                   "",     "Dog Lady",                   "DOG",    TOWNSFOLK,   true)  \
    NPC_SOUL(SKULLTULA_FAMILY,           "the ", "Cursed Family",              "CURSED", TOWNSFOLK,   true)  \
    NPC_SOUL(HYLIAN_GUARDS,              "the ", "Hylian Guards",              "GUARDS", TOWNSFOLK,   true)  \
    NPC_SOUL(RUNNING_MAN,                "",     "Running Man",                "RUNNER", TOWNSFOLK,   true)  \
    NPC_SOUL(GORON_LINK,                 "",     "Link the Goron",             "G LINK", TOWNSFOLK,   true)
// clang-format on
