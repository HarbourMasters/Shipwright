#pragma once

// X-macro list of every NPC soul.
//   NPC_SOUL(ID, "article", "Name", "TRACKER", CATEGORY, ADVANCEMENT, "German", "French")
// ARTICLE is "" or "the ". "the " marks a soul shared by a group of NPCs: the item is then
// "Soul of the Frogs" rather than "Frogs' Soul". Hint text reads "the soul of the Frogs".
// ID expands to RG_<ID>_SOUL, RAND_INF_<ID>_SOUL, RHT_<ID>_SOUL and RSK_STARTING_<ID>_SOUL.
// TRACKER is the short label drawn under the soul's icon in the item tracker (6 characters or fewer).
// CATEGORY is one of STORY, QUESTS, MINIGAMES, MERCHANTS, TOWNSFOLK and picks the
// Shuffle NPC Souls step that puts the soul in the pool (see IsNpcSoulCategoryShuffled).
// ADVANCEMENT is false for red-herring souls that gate no check: they still look and hint
// like real souls, but the fill treats them as junk.
// GERMAN is the item name with @ where the noun goes: "Talons @" is "Talons Seele", "@ der Zoras" is
// "Seele der Zoras". FRENCH follows the noun: "de Talon" is "Âme de Talon", "des Zoras" is "Âme des Zoras".
//
// New entries go at the END of this list: the generated enums are saved by position.

// clang-format off
#define NPC_SOUL_LIST(NPC_SOUL)                                                      \
    NPC_SOUL(KOKIRI_SHOPKEEPER,          "",     "Kokiri Shopkeeper",          "KF SHP", MERCHANTS,   true, "@ des Kokiri-Ladenbesitzers", "du marchand kokiri")  \
    NPC_SOUL(POTION_SHOPKEEPER,          "",     "Potion Shopkeeper",          "POTION", MERCHANTS,   true, "@ des Trankverkäufers", "du marchand de potions")  \
    NPC_SOUL(BAZAAR_SHOPKEEPER,          "",     "Bazaar Shopkeeper",          "BAZAAR", MERCHANTS,   true, "@ des Basarverkäufers", "du marchand du bazar")  \
    NPC_SOUL(GORON_SHOPKEEPER,           "",     "Goron Shopkeeper",           "GC SHP", MERCHANTS,   true, "@ des Goronen-Ladenbesitzers", "du marchand goron")  \
    NPC_SOUL(ZORA_SHOPKEEPER,            "",     "Zora Shopkeeper",            "ZD SHP", MERCHANTS,   true, "@ des Zora-Ladenbesitzers", "du marchand zora")  \
    NPC_SOUL(BOMBCHU_SHOPKEEPER,         "",     "Bombchu Shopkeeper",         "CHU",    MERCHANTS,   true, "@ des Krabbelminenhändlers", "du marchand de Missiles Teigneux")  \
    NPC_SOUL(MASK_SALESMAN,              "",     "Happy Mask Salesman",        "MASK",   QUESTS,      true, "@ des Maskenhändlers", "du vendeur de masques")  \
    NPC_SOUL(BEAN_SALESMAN,              "",     "Bean Salesman",              "BEAN",   MERCHANTS,   true, "@ des Bohnenverkäufers", "du marchand de haricots")  \
    NPC_SOUL(CARPET_SALESMAN,            "",     "Carpet Salesman",            "CARPET", MERCHANTS,   true, "@ des Teppichhändlers", "du marchand de tapis")  \
    NPC_SOUL(MEDIGORON,                  "",     "Medigoron",                  "MEDI",   MERCHANTS,   true, "Medigorons @", "de Medigoron")  \
    NPC_SOUL(GRANNY,                     "",     "Granny",                     "GRANNY", MERCHANTS,   true, "Omas @", "de mamie")  \
    NPC_SOUL(BEGGAR,                     "",     "Beggar",                     "BEGGAR", MERCHANTS,   true, "@ des Bettlers", "du mendiant")  \
    NPC_SOUL(POE_COLLECTOR,              "",     "Poe Collector",              "POES",   QUESTS,      true, "@ des Geistersammlers", "du collectionneur d'Esprits")  \
    NPC_SOUL(GROG,                       "",     "Grog",                       "GROG",   QUESTS,      true, "Grogs @", "de Grog")  \
    NPC_SOUL(FADO,                       "",     "Fado",                       "FADO",   QUESTS,      true, "Fados @", "de Fado")  \
    NPC_SOUL(MUTOH,                      "",     "Mutoh",                      "MUTOH",  QUESTS,      true, "@ des Chefs der Zimmerleute", "du patron des ouvriers")  \
    NPC_SOUL(BIGGORON,                   "",     "Biggoron",                   "BIGGO",  QUESTS,      true, "Biggorons @", "de Biggoron")  \
    NPC_SOUL(SHOOTING_GALLERY_MAN,       "",     "Shooting Gallery Man",       "SHOOT",  MINIGAMES,   true, "@ des Schießbudenbesitzers", "du gérant du stand de tir")  \
    NPC_SOUL(BOMBCHU_BOWLING_LADY,       "",     "Bombchu Bowling Lady",       "BOWL",   MINIGAMES,   true, "@ der Minenbowling-Dame", "de la dame du Bowling Teigneux")  \
    NPC_SOUL(TREASURE_CHEST_MAN,         "",     "Treasure Chest Game Man",    "TCG",    MINIGAMES,   true, "@ des Truhenlotterie-Besitzers", "du gérant de la chasse aux trésors")  \
    NPC_SOUL(DIVING_GAME_ZORA,           "",     "Diving Game Zora",           "DIVE",   MINIGAMES,   true, "@ des Tauchspiel-Zoras", "du Zora du jeu de plongée")  \
    NPC_SOUL(DAMPE,                      "",     "Dampe",                      "DAMPE",  MINIGAMES,   true, "Boris' @", "d'Igor")  \
    NPC_SOUL(HORSEBACK_ARCHER,           "",     "Horseback Archery Gerudo",   "HBA",    MINIGAMES,   true, "@ der Gerudo-Reitbogenschützin", "de la Gerudo du tir à cheval")  \
    NPC_SOUL(FISHING_POND_OWNER,         "",     "Fishing Pond Owner",         "FISH",   MINIGAMES,   true, "@ des Fischweiher-Besitzers", "du gérant de l'Étang")  \
    NPC_SOUL(SCIENTIST,                  "",     "Lakeside Scientist",         "LAB",    QUESTS,      true, "@ des Wissenschaftlers", "du scientifique du lac")  \
    NPC_SOUL(HOT_RODDER,                 "",     "Hot Rodder Goron",           "RODDER", QUESTS,      true, "@ des rollenden Goronen", "du Goron qui roule")  \
    NPC_SOUL(SKULL_KID,                  "",     "Skull Kid",                  "SKULL",  QUESTS,      true, "@ des Horror-Kids", "du Skull Kid")  \
    NPC_SOUL(FROGS,                      "the ", "Frogs",                      "FROGS",  QUESTS,      true, "@ der Frösche", "des grenouilles")  \
    NPC_SOUL(ANJU,                       "",     "Anju",                       "ANJU",   QUESTS,      true, "Anjus @", "d'Anju")  \
    NPC_SOUL(HYLIAN_GUARDS,              "the ", "Hylian Guards",              "GUARDS", TOWNSFOLK,   true, "@ der hylianischen Wachen", "des gardes hyliens")  \
    NPC_SOUL(MIDO,                       "",     "Mido",                       "MIDO",   STORY,       true, "Midos @", "de Mido")  \
    NPC_SOUL(KOKIRI,                     "the ", "Kokiri",                     "KOKIRI", TOWNSFOLK,   true, "@ der Kokiri", "des Kokiris")  \
    NPC_SOUL(KING_ZORA,                  "",     "King Zora",                  "KZ",     STORY,       true, "König Zoras @", "du Roi Zora")  \
    NPC_SOUL(GERUDO,                     "the ", "Gerudo",                     "GERUDO", TOWNSFOLK,   true, "@ der Gerudo", "des Gerudo")  \
    NPC_SOUL(ICHIRO,                     "",     "Ichiro",                     "ICHIRO", TOWNSFOLK,   true, "Ichiros @", "d'Ichiro")  \
    NPC_SOUL(SABOORO,                    "",     "Sabooro",                    "SABOO",  TOWNSFOLK,   true, "Saboros @", "de Sabooro")  \
    NPC_SOUL(JIRO,                       "",     "Jiro",                       "JIRO",   TOWNSFOLK,   true, "Jiros @", "de Jiro")  \
    NPC_SOUL(SHIRO,                      "",     "Shiro",                      "SHIRO",  TOWNSFOLK,   true, "Shiros @", "de Shiro")  \
    NPC_SOUL(TALON,                      "",     "Talon",                      "TALON",  STORY,       true, "Talons @", "de Talon")  \
    NPC_SOUL(MALON,                      "",     "Malon",                      "MALON",  STORY,       true, "Malons @", "de Malon")  \
    NPC_SOUL(INGO,                       "",     "Ingo",                       "INGO",   QUESTS,      true, "Basils @", "d'Ingo")  \
    NPC_SOUL(KAEPORA_GAEBORA,            "",     "Kaepora Gaebora",            "OWL",    TOWNSFOLK,   true, "Methusas @", "de Kaepora Gaebora")  \
    NPC_SOUL(SARIA,                      "",     "Saria",                      "SARIA",  STORY,       true, "Salias @", "de Saria")  \
    NPC_SOUL(DARUNIA,                    "",     "Darunia",                    "DARU",   STORY,       true, "Darunias @", "de Darunia")  \
    NPC_SOUL(RUTO,                       "",     "Ruto",                       "RUTO",   STORY,       true, "Rutos @", "de Ruto")  \
    NPC_SOUL(NABOORU,                    "",     "Nabooru",                    "NABOO",  STORY,       true, "Naborus @", "de Nabooru")  \
    NPC_SOUL(ZELDA,                      "",     "Zelda",                      "ZELDA",  STORY,       true, "Zeldas @", "de Zelda")  \
    NPC_SOUL(IMPA,                       "",     "Impa",                       "IMPA",   STORY,       true, "Impas @", "d'Impa")  \
    NPC_SOUL(RAURU,                      "",     "Rauru",                      "RAURU",  STORY,       true, "Raurus @", "de Rauru")  \
    NPC_SOUL(GREAT_FAIRIES,              "the ", "Great Fairies",              "FAIRY",  STORY,       true, "@ der großen Feen", "des grandes fées")  \
    NPC_SOUL(WINDMILL_MAN,               "",     "Guru-Guru",                  "GURU",   STORY,       true, "Guru-Gurus @", "de Guru-Guru")  \
    NPC_SOUL(MAN_ON_ROOF,                "",     "Man on the Roof",            "ROOF",   QUESTS,      true, "@ des Mannes auf dem Dach", "de l'homme sur le toit")  \
    NPC_SOUL(DOG_LADY,                   "",     "Dog Lady",                   "DOG",    QUESTS,      true, "@ der Hundedame", "de la dame au chien")  \
    NPC_SOUL(SKULLTULA_FAMILY,           "the ", "Cursed Family",              "CURSED", QUESTS,      true, "@ der verfluchten Familie", "de la famille maudite")  \
    NPC_SOUL(RUNNING_MAN,                "",     "Running Man",                "RUNNER", QUESTS,      true, "@ des Marathonläufers", "du marathonien")  \
    NPC_SOUL(GORON_LINK,                 "",     "Link the Goron",             "G LINK", QUESTS,      true, "@ des Goronen Link", "de Link le Goron")  \
    NPC_SOUL(KNOW_IT_ALL_BROTHERS,       "the ", "Know-It-All Brothers",       "KIA",    TOWNSFOLK,   false, "@ der allwissenden Brüder", "des frères Je-Sais-Tout")  \
    NPC_SOUL(HONEY_AND_DARLING,          "",     "Honey and Darling",          "H&D",    TOWNSFOLK,   false, "@ von Honey und Darling", "de Honey et Darling")  \
    NPC_SOUL(PIERRE,                     "",     "Pierre",                     "PIERRE", QUESTS,      true, "Pierres @", "de Pierre")  \
    NPC_SOUL(BONOORU,                    "",     "Bonooru",                    "BONOO",  QUESTS,      true, "Bonoorus @", "de Bonooru")  \
    NPC_SOUL(HYLIANS,                    "the ", "Hylians",                    "HYLIAN", TOWNSFOLK,   true, "@ der Hylianer", "des Hyliens")  \
    NPC_SOUL(GORONS,                     "the ", "Gorons",                     "GORONS", TOWNSFOLK,   true, "@ der Goronen", "des Gorons")  \
    NPC_SOUL(ZORAS,                      "the ", "Zoras",                      "ZORAS",  TOWNSFOLK,   false, "@ der Zoras", "des Zoras")  \
    NPC_SOUL(DEKU_SCRUBS,                "the ", "Deku Scrubs",                "SCRUBS", MERCHANTS,   true, "@ der Deku-Händler", "des pestes Mojo")  \
    NPC_SOUL(SHEIK,                      "",     "Sheik",                      "SHEIK",  STORY,       true, "Shieks @", "de Sheik")
// clang-format on
