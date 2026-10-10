/**
 * Scene Table
 *
 * DEFINE_SCENE should be used for non-empty scenes
 *    - Argument 0: Name of the scene segment in spec
 *    - Argument 1: Enum value for this scene
 *    - Argument 2: Title card's text id, or zero for no title card
 *    - Argument 3: Scene draw config index
 *    - Argument 4: Restriction flags. See `RESTRICTIONS_SET` on z_parameter.c
 *    - Argument 5: Persistent cycle flag set: bitwise flags which won't be turned off on a cycle reset (will persist between cycles)
 * 2S2H The following columns were added to the scene table:
 *    - Argument 6: EntranceSceneId used for better map select
 *    - Argument 7: Better map select index
 *    - Argument 8: Human readable name
 *
 * DEFINE_SCENE_UNSET is needed to define empty entries from the original game
 *    - Argument 0: Enum value for this scene
 */
// Southern Swamp (Clear)
/* 0x00 */ DEFINE_SCENE(Z2_20SICHITAI2,       SCENE_20SICHITAI2,       0x116, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10) | (1 << 11), 0, 0, (1 << 30)),                                         SOUTHERN_SWAMP_CLEARED,        31, "Southern Swamp (Clear)")
/* 0x01 */ DEFINE_SCENE_UNSET(SCENE_UNSET_01)
/* 0x02 */ DEFINE_SCENE_UNSET(SCENE_UNSET_02)
/* 0x03 */ DEFINE_SCENE_UNSET(SCENE_UNSET_03)
/* 0x04 */ DEFINE_SCENE_UNSET(SCENE_UNSET_04)
/* 0x05 */ DEFINE_SCENE_UNSET(SCENE_UNSET_05)
/* 0x06 */ DEFINE_SCENE_UNSET(SCENE_UNSET_06)
// Lone Peak Shrine & Grottos
/* 0x07 */ DEFINE_SCENE(KAKUSIANA,            SCENE_KAKUSIANA,         0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 2) | (1 << 10) | (1 << 11)),                                      GROTTOS,                      101, "Lone Peak Shrine & Grottos")
// Cutscene Scene
/* 0x08 */ DEFINE_SCENE(SPOT00,               SCENE_SPOT00,            0,     SCENE_DRAW_CFG_NOTHING,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                CUTSCENE,                      99, "Cutscene Scene")
/* 0x09 */ DEFINE_SCENE_UNSET(SCENE_UNSET_09)
// Magic Hags' Potion Shop
/* 0x0A */ DEFINE_SCENE(Z2_WITCH_SHOP,        SCENE_WITCH_SHOP,        0x11A, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MAGIC_HAGS_POTION_SHOP,        33, "Magic Hags' Potion Shop")
// Majora's Lair
/* 0x0B */ DEFINE_SCENE(Z2_LAST_BS,           SCENE_LAST_BS,           0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             MAJORAS_LAIR,                  96, "Majora's Lair")
// Beneath the Graveyard
/* 0x0C */ DEFINE_SCENE(Z2_HAKASHITA,         SCENE_HAKASHITA,         0x113, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             BENEATH_THE_GRAVERYARD,        75, "Beneath the Graveyard")
// Curiosity Shop
/* 0x0D */ DEFINE_SCENE(Z2_AYASHIISHOP,       SCENE_AYASHIISHOP,       0x10E, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                CURIOSITY_SHOP,                11, "Curiosity Shop")
/* 0x0E */ DEFINE_SCENE_UNSET(SCENE_UNSET_0E)
/* 0x0F */ DEFINE_SCENE_UNSET(SCENE_UNSET_0F)
// Mama's House (Ranch House in PAL) & Barn
/* 0x10 */ DEFINE_SCENE(Z2_OMOYA,             SCENE_OMOYA,             0x132, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                RANCH_HOUSE,                   25, "Mama's House & Barn")
// Honey & Darling's Shop
/* 0x11 */ DEFINE_SCENE(Z2_BOWLING,           SCENE_BOWLING,           0x108, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                HONEY_AND_DARLINGS_SHOP,       15, "Honey & Darling's Shop")
// The Mayor's Residence
/* 0x12 */ DEFINE_SCENE(Z2_SONCHONOIE,        SCENE_SONCHONOIE,        0x10B, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MAYORS_RESIDENCE,               5, "The Mayor's Residence")
// Ikana Canyon
/* 0x13 */ DEFINE_SCENE(Z2_IKANA,             SCENE_IKANA,             0x141, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 20), 0, 0, (1 << 1) | (1 << 30)),                                          IKANA_CANYON,                  77, "Ikana Canyon")
// Pirates' Fortress
/* 0x14 */ DEFINE_SCENE(Z2_KAIZOKU,           SCENE_KAIZOKU,           0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                PIRATES_FORTRESS,              69, "Pirates' Fortress")
// Milk Bar
/* 0x15 */ DEFINE_SCENE(Z2_MILK_BAR,          SCENE_MILK_BAR,          0x10C, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MILK_BAR,                      10, "Milk Bar")
// Stone Tower Temple
/* 0x16 */ DEFINE_SCENE(Z2_INISIE_N,          SCENE_INISIE_N,          0x144, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 26), 0, 0, 0),                                                             STONE_TOWER_TEMPLE,            87, "Stone Tower Temple")
// Treasure Chest Shop
/* 0x17 */ DEFINE_SCENE(Z2_TAKARAYA,          SCENE_TAKARAYA,          0x109, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_SET(2, 0, 0, 0),                                                                     TREASURE_CHEST_SHOP,           14, "Treasure Chest Shop")
// Inverted Stone Tower Temple
/* 0x18 */ DEFINE_SCENE(Z2_INISIE_R,          SCENE_INISIE_R,          0x144, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 26), 0, 0, 0),                                                             STONE_TOWER_TEMPLE_INVERTED,   88, "Inverted Stone Tower Temple")
// Clock Tower Rooftop
/* 0x19 */ DEFINE_SCENE(Z2_OKUJOU,            SCENE_OKUJOU,            0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                CLOCK_TOWER_ROOFTOP,           20, "Clock Tower Rooftop")
// Before Clock Town
/* 0x1A */ DEFINE_SCENE(Z2_OPENINGDAN,        SCENE_OPENINGDAN,        0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3), 0, 0, 0),                             OPENING_DUNGEON,               18, "Before Clock Town")
// Woodfall Temple
/* 0x1B */ DEFINE_SCENE(Z2_MITURIN,           SCENE_MITURIN,           0x11F, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 12) | (1 << 13) | (1 << 15) | (1 << 20) | (1 << 21) | (1 << 22), 0, 0, 0), WOODFALL_TEMPLE,               40, "Woodfall Temple")
// Path to Mountain Village
/* 0x1C */ DEFINE_SCENE(Z2_13HUBUKINOMITI,    SCENE_13HUBUKINOMITI,    0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                PATH_TO_MOUNTAIN_VILLAGE,      42, "Path to Mountain Village")
// Ancient Castle of Ikana
/* 0x1D */ DEFINE_SCENE(Z2_CASTLE,            SCENE_CASTLE,            0x142, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, (1 << 31), 0, (1 << 10)),                                                     IKANA_CASTLE,                  83, "Ancient Castle of Ikana")
// Deku Scrub Playground
/* 0x1E */ DEFINE_SCENE(Z2_DEKUTES,           SCENE_DEKUTES,           0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                DEKU_SCRUB_PLAYGROUND,         98, "Deku Scrub Playground")
// Odolwa's Lair
/* 0x1F */ DEFINE_SCENE(Z2_MITURIN_BS,        SCENE_MITURIN_BS,        0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             ODOLWAS_LAIR,                  41, "Odolwa's Lair")
// Town Shooting Gallery
/* 0x20 */ DEFINE_SCENE(Z2_SYATEKI_MIZU,      SCENE_SYATEKI_MIZU,      0x107, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                TOWN_SHOOTING_GALLERY,         16, "Town Shooting Gallery")
// Snowhead Temple
/* 0x21 */ DEFINE_SCENE(Z2_HAKUGIN,           SCENE_HAKUGIN,           0x12B, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 5) | (1 << 21) | (1 << 23) | (1 << 24), 0, 0, 0),                          SNOWHEAD_TEMPLE,               56, "Snowhead Temple")
// Milk Road
/* 0x22 */ DEFINE_SCENE(Z2_ROMANYMAE,         SCENE_ROMANYMAE,         0x149, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             MILK_ROAD,                     22, "Milk Road")
// Pirates' Fortress Interior
/* 0x23 */ DEFINE_SCENE(Z2_PIRATE,            SCENE_PIRATE,            0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 12)),                                                             PIRATES_FORTRESS_INTERIOR,     70, "Pirates' Fortress Interior")
// Swamp Shooting Gallery
/* 0x24 */ DEFINE_SCENE(Z2_SYATEKI_MORI,      SCENE_SYATEKI_MORI,      0x11B, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                SWAMP_SHOOTING_GALLERY,        29, "Swamp Shooting Gallery")
// Pinnacle Rock
/* 0x25 */ DEFINE_SCENE(Z2_SINKAI,            SCENE_SINKAI,            0x135, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 2),                                                                     PINNACLE_ROCK,                 66, "Pinnacle Rock")
// Fairy's Fountain
/* 0x26 */ DEFINE_SCENE(Z2_YOUSEI_IZUMI,      SCENE_YOUSEI_IZUMI,      0x13E, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             FAIRY_FOUNTAIN,                97, "Fairy's Fountain")
// Swamp Spider House
/* 0x27 */ DEFINE_SCENE(Z2_KINSTA1,           SCENE_KINSTA1,           0x11E, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                SWAMP_SPIDER_HOUSE,            34, "Swamp Spider House")
// Oceanside Spider House
/* 0x28 */ DEFINE_SCENE(Z2_KINDAN2,           SCENE_KINDAN2,           0x13F, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             OCEANSIDE_SPIDER_HOUSE,        62, "Oceanside Spider House")
// Astral Observatory
/* 0x29 */ DEFINE_SCENE(Z2_TENMON_DAI,        SCENE_TENMON_DAI,        0x114, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                ASTRAL_OBSERVATORY,             6, "Astral Observatory")
// Moon Deku Trial
/* 0x2A */ DEFINE_SCENE(Z2_LAST_DEKU,         SCENE_LAST_DEKU,         0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 2),                                                                     MOON_DEKU_TRIAL,               92, "Moon Deku Trial")
// Deku Palace
/* 0x2B */ DEFINE_SCENE(Z2_22DEKUCITY,        SCENE_22DEKUCITY,        0x118, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 30)),                                                             DEKU_PALACE,                   35, "Deku Palace")
// Mountain Smithy
/* 0x2C */ DEFINE_SCENE(Z2_KAJIYA,            SCENE_KAJIYA,            0x127, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MOUNTAIN_SMITHY,               45, "Mountain Smithy")
// Termina Field
/* 0x2D */ DEFINE_SCENE(Z2_00KEIKOKU,         SCENE_00KEIKOKU,         0x100, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 4), 0, 0, 0),                                                              TERMINA_FIELD,                 21, "Termina Field")
// Post Office
/* 0x2E */ DEFINE_SCENE(Z2_POSTHOUSE,         SCENE_POSTHOUSE,         0x111, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_SET(3, 0, 0, 0),                                                                     POST_OFFICE,                    8, "Post Office")
// Marine Research Lab
/* 0x2F */ DEFINE_SCENE(Z2_LABO,              SCENE_LABO,              0x13A, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MARINE_RESEARCH_LAB,           60, "Marine Research Lab")
// Beneath the Graveyard (Day 3) and Dampe's House
/* 0x30 */ DEFINE_SCENE(Z2_DANPEI2TEST,       SCENE_DANPEI2TEST,       0x113, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             DAMPES_HOUSE,                  76, "Beneath Graveyard and Dampe's House")
/* 0x31 */ DEFINE_SCENE_UNSET(SCENE_UNSET_31)
// Goron Shrine
/* 0x32 */ DEFINE_SCENE(Z2_16GORON_HOUSE,     SCENE_16GORON_HOUSE,     0x124, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GORON_SHRINE,                  52, "Goron Shrine")
// Zora Hall
/* 0x33 */ DEFINE_SCENE(Z2_33ZORACITY,        SCENE_33ZORACITY,        0x136, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                ZORA_HALL,                     64, "Zora Hall")
// Trading Post
/* 0x34 */ DEFINE_SCENE(Z2_8ITEMSHOP,         SCENE_8ITEMSHOP,         0x10F, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                TRADING_POST,                  13, "Trading Post")
// Romani Ranch
/* 0x35 */ DEFINE_SCENE(Z2_F01,               SCENE_F01,               0x12E, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                ROMANI_RANCH,                  24, "Romani Ranch")
// Twinmold's Lair
/* 0x36 */ DEFINE_SCENE(Z2_INISIE_BS,         SCENE_INISIE_BS,         0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             TWINMOLDS_LAIR,                89, "Twinmold's Lair")
// Great Bay Coast
/* 0x37 */ DEFINE_SCENE(Z2_30GYOSON,          SCENE_30GYOSON,          0x134, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10) | (1 << 20), 0, 0, (1 << 1) | (1 << 5)),                               GREAT_BAY_COAST,               58, "Great Bay Coast")
// Zora Cape
/* 0x38 */ DEFINE_SCENE(Z2_31MISAKI,          SCENE_31MISAKI,          0x134, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, (1 << 7)),                                                      ZORA_CAPE,                     63, "Zora Cape")
// Lottery Shop
/* 0x39 */ DEFINE_SCENE(Z2_TAKARAKUJI,        SCENE_TAKARAKUJI,        0x112, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                LOTTERY_SHOP,                  17, "Lottery Shop")
/* 0x3A */ DEFINE_SCENE_UNSET(SCENE_UNSET_3A)
// Pirates' Fortress Moat
/* 0x3B */ DEFINE_SCENE(Z2_TORIDE,            SCENE_TORIDE,            0x138, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             PIRATES_FORTRESS_EXTERIOR,     68, "Pirates' Fortress Moat")
// Fisherman's Hut
/* 0x3C */ DEFINE_SCENE(Z2_FISHERMAN,         SCENE_FISHERMAN,         0x13B, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                FISHERMANS_HUT,                61, "Fisherman's Hut")
// Goron Shop
/* 0x3D */ DEFINE_SCENE(Z2_GORONSHOP,         SCENE_GORONSHOP,         0x129, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GORON_SHOP,                    53, "Goron Shop")
// Deku King's Chamber
/* 0x3E */ DEFINE_SCENE(Z2_DEKU_KING,         SCENE_DEKU_KING,         0x11C, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                DEKU_KINGS_CHAMBER,            37, "Deku King's Chamber")
// Moon Goron Trial
/* 0x3F */ DEFINE_SCENE(Z2_LAST_GORON,        SCENE_LAST_GORON,        0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 2),                                                                     MOON_GORON_TRIAL,              93, "Moon Goron Trial")
// Road to Southern Swamp
/* 0x40 */ DEFINE_SCENE(Z2_24KEMONOMITI,      SCENE_24KEMONOMITI,      0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 1) | (1 << 31)),                                                  ROAD_TO_SOUTHERN_SWAMP,        28, "Road to Southern Swamp")
// Doggy Racetrack
/* 0x41 */ DEFINE_SCENE(Z2_F01_B,             SCENE_F01_B,             0x130, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                DOGGY_RACETRACK,               27, "Doggy Racetrack")
// Cucco Shack
/* 0x42 */ DEFINE_SCENE(Z2_F01C,              SCENE_F01C,              0x12F, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                CUCCO_SHACK,                   26, "Cucco Shack")
// Ikana Graveyard
/* 0x43 */ DEFINE_SCENE(Z2_BOTI,              SCENE_BOTI,              0x106, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                IKANA_GRAVEYARD,               74, "Ikana Graveyard")
// Goht's Lair
/* 0x44 */ DEFINE_SCENE(Z2_HAKUGIN_BS,        SCENE_HAKUGIN_BS,        0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             GOHTS_LAIR,                    57, "Goht's Lair")
// Southern Swamp (poison)
/* 0x45 */ DEFINE_SCENE(Z2_20SICHITAI,        SCENE_20SICHITAI,        0x116, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10) | (1 << 11), 0, 0, (1 << 30)),                                         SOUTHERN_SWAMP_POISONED,       30, "Southern Swamp (poison)")
// Woodfall
/* 0x46 */ DEFINE_SCENE(Z2_21MITURINMAE,      SCENE_21MITURINMAE,      0x117, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0x102, 0, 0, (1 << 10)),                                                         WOODFALL,                      39, "Woodfall")
// Moon Zora Trial
/* 0x47 */ DEFINE_SCENE(Z2_LAST_ZORA,         SCENE_LAST_ZORA,         0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 2),                                                                     MOON_ZORA_TRIAL,               94, "Moon Zora Trial")
// Goron Village (spring)
/* 0x48 */ DEFINE_SCENE(Z2_11GORONNOSATO2,    SCENE_11GORONNOSATO2,    0x123, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 30)),                                                             GORON_VILLAGE_SPRING,          51, "Goron Village (spring)")
// Great Bay Temple
/* 0x49 */ DEFINE_SCENE(Z2_SEA,               SCENE_SEA,               0x13D, SCENE_DRAW_CFG_GREAT_BAY_TEMPLE,     RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 4) | (1 << 5) | (1 << 6), 0, 0, 0),                                        GREAT_BAY_TEMPLE,              71, "Great Bay Temple")
// Waterfall Rapids
/* 0x4A */ DEFINE_SCENE(Z2_35TAKI,            SCENE_35TAKI,            0x137, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                WATERFALL_RAPIDS,              67, "Waterfall Rapids")
// Beneath the Well
/* 0x4B */ DEFINE_SCENE(Z2_REDEAD,            SCENE_REDEAD,            0x145, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                BENEATH_THE_WELL,              80, "Beneath the Well")
// Zora Hall Rooms
/* 0x4C */ DEFINE_SCENE(Z2_BANDROOM,          SCENE_BANDROOM,          0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 30)),                                                             ZORA_HALL_ROOMS,               65, "Zora Hall Rooms")
// Goron Village (winter)
/* 0x4D */ DEFINE_SCENE(Z2_11GORONNOSATO,     SCENE_11GORONNOSATO,     0x123, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 30)),                                                             GORON_VILLAGE_WINTER,          50, "Goron Village (winter)")
// Goron Graveyard
/* 0x4E */ DEFINE_SCENE(Z2_GORON_HAKA,        SCENE_GORON_HAKA,        0x12A, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GORON_GRAVERYARD,              46, "Goron Graveyard")
// Sakon's Hideout
/* 0x4F */ DEFINE_SCENE(Z2_SECOM,             SCENE_SECOM,             0x143, SCENE_DRAW_CFG_MAT_ANIM_MANUAL_STEP, RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 3, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                SAKONS_HIDEOUT,                78, "Sakon's Hideout")
// Mountain Village (winter)
/* 0x50 */ DEFINE_SCENE(Z2_10YUKIYAMANOMURA,  SCENE_10YUKIYAMANOMURA,  0x122, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 20), 0, 0, (1 << 31)),                                                     MOUNTAIN_VILLAGE_WINTER,       43, "Mountain Village (winter)")
// Ghost Hut
/* 0x51 */ DEFINE_SCENE(Z2_TOUGITES,          SCENE_TOUGITES,          0x146, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GHOST_HUT,                     81, "Ghost Hut")
// Deku Shrine
/* 0x52 */ DEFINE_SCENE(Z2_DANPEI,            SCENE_DANPEI,            0x120, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                DEKU_SHRINE,                   36, "Deku Shrine")
// Road to Ikana
/* 0x53 */ DEFINE_SCENE(Z2_IKANAMAE,          SCENE_IKANAMAE,          0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                ROAD_TO_IKANA,                 73, "Road to Ikana")
// Swordsman's School
/* 0x54 */ DEFINE_SCENE(Z2_DOUJOU,            SCENE_DOUJOU,            0x110, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 1), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                SWORDMANS_SCHOOL,               7, "Swordsman's School")
// Music Box House
/* 0x55 */ DEFINE_SCENE(Z2_MUSICHOUSE,        SCENE_MUSICHOUSE,        0x147, SCENE_DRAW_CFG_MAT_ANIM_MANUAL_STEP, RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                MUSIC_BOX_HOUSE,               79, "Music Box House")
// Igos du Ikana's Lair
/* 0x56 */ DEFINE_SCENE(Z2_IKNINSIDE,         SCENE_IKNINSIDE,         0x142, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_NONE,                                                                                IGOS_DU_IKANAS_LAIR,           84, "Igos du Ikana's Lair")
// Tourist Information
/* 0x57 */ DEFINE_SCENE(Z2_MAP_SHOP,          SCENE_MAP_SHOP,          0x119, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                TOURIST_INFORMATION,           32, "Tourist Information")
// Stone Tower
/* 0x58 */ DEFINE_SCENE(Z2_F40,               SCENE_F40,               0x140, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             STONE_TOWER,                   85, "Stone Tower")
// Inverted Stone Tower
/* 0x59 */ DEFINE_SCENE(Z2_F41,               SCENE_F41,               0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             STONE_TOWER_INVERTED,          86, "Inverted Stone Tower")
// Mountain Village (spring)
/* 0x5A */ DEFINE_SCENE(Z2_10YUKIYAMANOMURA2, SCENE_10YUKIYAMANOMURA2, 0x122, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 20), 0, 0, (1 << 31)),                                                     MOUNTAIN_VILLAGE_SPRING,       44, "Mountain Village (spring)")
// Path to Snowhead
/* 0x5B */ DEFINE_SCENE(Z2_14YUKIDAMANOMITI,  SCENE_14YUKIDAMANOMITI,  0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 8)),                                                              PATH_TO_SNOWHEAD,              54, "Path to Snowhead")
// Snowhead
/* 0x5C */ DEFINE_SCENE(Z2_12HAKUGINMAE,      SCENE_12HAKUGINMAE,      0x125, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 10), 0, 0, 0),                                                             SNOWHEAD,                      55, "Snowhead")
// Path to Goron Village (winter)
/* 0x5D */ DEFINE_SCENE(Z2_17SETUGEN,         SCENE_17SETUGEN,         0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 7)),                                                              PATH_TO_GORON_VILLAGE_WINTER,  47, "Path to Goron Village (winter)")
// Path to Goron Village (spring)
/* 0x5E */ DEFINE_SCENE(Z2_17SETUGEN2,        SCENE_17SETUGEN2,        0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 7)),                                                              PATH_TO_GORON_VILLAGE_SPRING,  48, "Path to Goron Village (spring)")
// Gyorg's Lair
/* 0x5F */ DEFINE_SCENE(Z2_SEA_BS,            SCENE_SEA_BS,            0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 31)),                                                             GYORGS_LAIR,                   72, "Gyorg's Lair")
// Secret Shrine
/* 0x60 */ DEFINE_SCENE(Z2_RANDOM,            SCENE_RANDOM,            0x12C, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 10)),                                                             SECRET_SHRINE,                 82, "Secret Shrine")
// Stock Pot Inn
/* 0x61 */ DEFINE_SCENE(Z2_YADOYA,            SCENE_YADOYA,            0x10A, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                STOCK_POT_INN,                  9, "Stock Pot Inn")
// Great Bay Cutscene
/* 0x62 */ DEFINE_SCENE(Z2_KONPEKI_ENT,       SCENE_KONPEKI_ENT,       0x139, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GREAT_BAY_CUTSCENE,            59, "Great Bay Cutscene")
// Clock Tower Interior
/* 0x63 */ DEFINE_SCENE(Z2_INSIDETOWER,       SCENE_INSIDETOWER,       0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_SET(0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0), PERSISTENT_CYCLE_FLAGS_SET(1 << 0, 0, 0, 0),                                                                CLOCK_TOWER_INTERIOR,          19, "Clock Tower Interior")
// Woods of Mystery
/* 0x64 */ DEFINE_SCENE(Z2_26SARUNOMORI,      SCENE_26SARUNOMORI,      0x11D, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                WOODS_OF_MYSTERY,              38, "Woods of Mystery")
// Lost Woods (Intro)
/* 0x65 */ DEFINE_SCENE(Z2_LOST_WOODS,        SCENE_LOST_WOODS,        0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                LOST_WOODS,                   100, "Lost Woods (Intro)")
// Moon Link Trial
/* 0x66 */ DEFINE_SCENE(Z2_LAST_LINK,         SCENE_LAST_LINK,         0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 2),                                                                     MOON_LINK_TRIAL,               95, "Moon Link Trial")
// The Moon
/* 0x67 */ DEFINE_SCENE(Z2_SOUGEN,            SCENE_SOUGEN,            0,     SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_MOON,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                THE_MOON,                      91, "The Moon")
// Bomb Shop
/* 0x68 */ DEFINE_SCENE(Z2_BOMYA,             SCENE_BOMYA,             0x10D, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_INDOORS,                                 PERSISTENT_CYCLE_FLAGS_NONE,                                                                                BOMB_SHOP,                     12, "Bomb Shop")
// Giants' Chamber
/* 0x69 */ DEFINE_SCENE(Z2_KYOJINNOMA,        SCENE_KYOJINNOMA,        0,     SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GIANTS_CHAMBER,                90, "Giants' Chamber")
// Gorman Track
/* 0x6A */ DEFINE_SCENE(Z2_KOEPONARACE,       SCENE_KOEPONARACE,       0x131, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NO_DOUBLE_TIME,                          PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GORMAN_TRACK,                  23, "Gorman Track")
// Goron Racetrack
/* 0x6B */ DEFINE_SCENE(Z2_GORONRACE,         SCENE_GORONRACE,         0x126, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                GORON_RACETRACK,               49, "Goron Racetrack")
// East Clock Town
/* 0x6C */ DEFINE_SCENE(Z2_TOWN,              SCENE_TOWN,              0x101, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(1, 0, 0, 0),                                                                     EAST_CLOCK_TOWN,                1, "East Clock Town")
// West Clock Town
/* 0x6D */ DEFINE_SCENE(Z2_ICHIBA,            SCENE_ICHIBA,            0x102, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_NONE,                                                                                WEST_CLOCK_TOWN,                2, "West Clock Town")
// North Clock Town
/* 0x6E */ DEFINE_SCENE(Z2_BACKTOWN,          SCENE_BACKTOWN,          0x103, SCENE_DRAW_CFG_DEFAULT,              RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, (1 << 10)),                                                             NORTH_CLOCK_TOWN,               3, "North Clock Town")
// South Clock Town
/* 0x6F */ DEFINE_SCENE(Z2_CLOCKTOWER,        SCENE_CLOCKTOWER,        0x104, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET((1 << 20), 0, 0, (1 << 10)),                                                     SOUTH_CLOCK_TOWN,               0, "South Clock Town")
// Laundry Pool
/* 0x70 */ DEFINE_SCENE(Z2_ALLEY,             SCENE_ALLEY,             0x105, SCENE_DRAW_CFG_MAT_ANIM,             RESTRICTIONS_NONE,                                    PERSISTENT_CYCLE_FLAGS_SET(0, 0, 1, 0),                                                                     LAUNDRY_POOL,                   4, "Laundry Pool")
