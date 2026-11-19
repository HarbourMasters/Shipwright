#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "../static_data.h"

using namespace std::literals::string_literals;

// Big thanks to Lioncache, Gabyelnuevo, Danius88, and Charade for their translations!

// Text is {english, german, french}

// there are special characters that are read for certain in game commands:
// ^ is a box break
// & is a new line
// @ will print the player name
// surrounding text with '#' will make it a different color
// - OoT Randomizer

// '[[d]]' indicates a number will be placed there, numbers in [[]] will be replaced by other text in hints
// '€' at the end of an item name indicates that it's plural.
// '|' separates singular and plural form |singular|plural|.

namespace Rando {

void StaticData::HintTable_Init() {
    // clang-format off
    /*--------------------------
    |       GENERAL TEXT        |
    ---------------------------*/
    hintTextTable[RHT_NONE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_NONE.clear" ));
    hintTextTable[RHT_WAY_OF_THE_HERO] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WAY_OF_THE_HERO.clear", {QM_RED, QM_LBLUE}));
                 // /*spanish*/ "Según dicen, #[[1]]# conduce a la senda del héroe."
    hintTextTable[RHT_FOOLISH] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOOLISH.clear", {QM_RED, QM_PINK}));
                 // /*spanish*/ "Según dicen, inspeccionar #[[1]]# #no es una sabia decisión#."
    hintTextTable[RHT_CAN_BE_FOUND_AT] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CAN_BE_FOUND_AT.clear", {QM_GREEN, QM_RED}));
                 // /*spanish*/ "Según dicen, #[[1]]# aguarda en #[[2]]#."
    hintTextTable[RHT_HOARDS] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HOARDS.clear", {QM_RED, QM_GREEN}));
                 // /*spanish*/ "Según dicen, #[[2]]# acapara #[[1]]#."
    // clang-format on

    HintTable_Init_Item();
    HintTable_Init_Exclude_Overworld();
    HintTable_Init_Exclude_Dungeon();

    // clang-format off
    /*--------------------------
    |    SOMETIMES HINT TEXT   |
    ---------------------------*/

    hintTextTable[RHT_KF_LINKS_HOUSE_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_LINKS_HOUSE_COW.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/la #carrera de obstáculos de Malon# brinda #[[1]]#.
                                                     {},
                                                     {CustomMessages::CreateMessage("randomizer.hints.RHT_KF_LINKS_HOUSE_COW.obscure.0", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/la #recompensa bovina de un paseo a caballo# brinda #[[1]]#.

    hintTextTable[RHT_KAK_100_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_100_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/exterminar #100 skulltulas doradas# revela #[[1]]#.
                                                                {},
                                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_100_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/#100 medallas de insectos# otorgan #[[1]]#.
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_100_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/#100 almas de araña# otorgan #[[1]]#.
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_100_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                 // /*spanish*/#100 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_SONG_FROM_OCARINA_OF_TIME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_OCARINA_OF_TIME.clear", {QM_RED, QM_GREEN})),
                                                            // /*spanish*/la #Ocarina del Tiempo# enseña #[[1]]#.

    hintTextTable[RHT_SONG_FROM_ROYAL_FAMILYS_TOMB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_ROYAL_FAMILYS_TOMB.clear", {QM_RED, QM_GREEN})),
                                                               // /*spanish*/los #ReDeads del Panteón Real# guardan #[[1]]#.

    hintTextTable[RHT_SHEIK_IN_FOREST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_IN_FOREST.clear", {QM_RED, QM_GREEN})),
                                                  // /*spanish*/#en la pradera sagrada# Sheik enseña #[[1]]#.

    hintTextTable[RHT_SHEIK_AT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_AT_TEMPLE.clear", {QM_RED, QM_GREEN})),
                                                  // /*spanish*/Sheik espera en el #momumento del tiempo# para enseñar #[[1]]#.

    hintTextTable[RHT_SHEIK_IN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_IN_CRATER.clear", {QM_RED, QM_GREEN})),
                                                  // /*spanish*/la #melodía del cráter# otorga #[[1]]#.

    hintTextTable[RHT_SHEIK_IN_ICE_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_IN_ICE_CAVERN.clear", {QM_RED, QM_GREEN})),
                                                      // /*spanish*/en la #caverna de hielo# retumban los ecos de #[[1]]#.

    hintTextTable[RHT_SHEIK_IN_KAKARIKO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_IN_KAKARIKO.clear", {QM_RED, QM_GREEN})),
                                                    // /*spanish*/un #arrasado pueblo# llora #[[1]]#.

    hintTextTable[RHT_SHEIK_AT_COLOSSUS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_AT_COLOSSUS.clear", {QM_RED, QM_GREEN})),
                                                    // /*spanish*/el héroe que se adentre #más allá del desierto# aprenderá #[[1]]#.

    hintTextTable[RHT_MARKET_10_BIG_POES] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_10_BIG_POES.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/hacerte con #Grandes Poes# conduce a #[[1]]#.
                                                     {},
                                                     {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_10_BIG_POES.obscure.0", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/los #cazafantasmas# son premiados con #[[1]]#.

    hintTextTable[RHT_DEKU_THEATER_SKULL_MASK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_THEATER_SKULL_MASK.clear", {QM_RED, QM_GREEN})),
                                                          // /*spanish*/la #máscara de calavera# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_THEATER_MASK_OF_TRUTH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_THEATER_MASK_OF_TRUTH.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/la #máscara de la verdad# premia #[[1]]#.
                                                             {},
                                                             {CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_THEATER_MASK_OF_TRUTH.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/#mostrarle el ojo verdadero# a una multitud brinda #[[1]]#.

    hintTextTable[RHT_HF_OCARINA_OF_TIME_ITEM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_OCARINA_OF_TIME_ITEM.clear", {QM_RED, QM_GREEN})),
                                                          // /*spanish*/el #tesoro arrojado por la Princesa Zelda# se trata de #[[1]]#.

    hintTextTable[RHT_DMT_TRADE_BROKEN_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_TRADE_BROKEN_SWORD.clear", {QM_RED, QM_GREEN})),
                                                         // /*spanish*/un #miope Biggoron# otorga #[[1]]#.

    hintTextTable[RHT_DMT_TRADE_EYEDROPS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_TRADE_EYEDROPS.clear", {QM_RED, QM_GREEN})),
                                                     // /*spanish*/#Biggoron# está a la espera de otorgar #[[1]]#.

    hintTextTable[RHT_DMT_TRADE_CLAIM_CHECK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_TRADE_CLAIM_CHECK.clear", {QM_RED, QM_GREEN})),
                                                        // /*spanish*/#Biggoron# forja #[[1]]#.

    hintTextTable[RHT_KAK_50_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_50_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/exterminar #50 skulltulas doradas# revela #[[1]]#.
                                                               {},
                                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_50_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#50 medallas de insectos# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_50_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#50 almas de araña# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_50_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/#50 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_40_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_40_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/exterminar #40 skulltulas doradas# revela #[[1]]#.
                                                               {},
                                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_40_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#40 medallas de insectos# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_40_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#40 almas de araña# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_40_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/#40 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_30_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_30_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/exterminar #30 skulltulas doradas# revela #[[1]]#.
                                                               {},
                                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_30_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#30 medallas de insectos# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_30_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#30 almas de araña# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_30_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/#30 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_20_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_20_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/exterminar #20 skulltulas doradas# revela #[[1]]#.
                                                               {},
                                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_20_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#20 medallas de insectos# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_20_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/#20 almas de araña# otorgan #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_20_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/#20 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_ANJU_AS_CHILD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ANJU_AS_CHILD.clear", {QM_RED, QM_GREEN}),
                                                    // /*spanish*/#hacerte con todos los cucos# premia #[[1]]#.
                                                    {},
                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ANJU_AS_CHILD.obscure.0", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/#atrapar a las gallinas# premia #[[1]]#.
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ANJU_AS_CHILD.obscure.1", {QM_RED, QM_GREEN})});
                                                     // /*spanish*/#reunir a unos emplumados# premia #[[1]]#.

    hintTextTable[RHT_KAK_TRADE_POCKET_CUCCO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_TRADE_POCKET_CUCCO.clear", {QM_RED, QM_GREEN})),
                                                         // /*spanish*/un #alegre cuco# en la madurez otorga #[[1]]#.

    hintTextTable[RHT_KAK_TRADE_ODD_MUSHROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_TRADE_ODD_MUSHROOM.clear", {QM_RED, QM_GREEN})),
                                                         // /*spanish*/la #señora de la tienda de pociones# otorga #[[1]]#.

    hintTextTable[RHT_GC_DARUNIAS_JOY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_DARUNIAS_JOY.clear", {QM_RED, QM_GREEN}),
                                                  // /*spanish*/#el baile de Darunia# conduce a #[[1]]#.
                                                  {},
                                                  {CustomMessages::CreateMessage("randomizer.hints.RHT_GC_DARUNIAS_JOY.obscure.0", {QM_RED, QM_GREEN})});
                                                   // /*spanish*/#un goron marchoso# otorga #[[1]]#.

    hintTextTable[RHT_LW_SKULL_KID] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_SKULL_KID.clear", {QM_RED, QM_GREEN})),
                                               // /*spanish*/#Skull Kid# otorga #[[1]]#.

    hintTextTable[RHT_LW_TRADE_COJIRO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_TRADE_COJIRO.clear", {QM_RED, QM_GREEN})),
                                                  // /*spanish*/quien devuelva un #cuco especial# encontrará #[[1]]#.

    hintTextTable[RHT_LW_TRADE_ODD_POTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_TRADE_ODD_POTION.clear", {QM_RED, QM_GREEN})),
                                                      // /*spanish*/una #chica kokiri del bosque# otorga #[[1]]#.

    hintTextTable[RHT_LH_SUN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SUN.clear", {QM_RED, QM_GREEN}),
                                         // /*spanish*/#disparar al sol# revela #[[1]]#.
                                         {},
                                         {CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SUN.obscure.0", {QM_RED, QM_GREEN})});
                                          // /*spanish*/#mirar al sol# revela #[[1]]#.

    hintTextTable[RHT_LH_TRADE_FROG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_TRADE_FROG.clear", {QM_RED, QM_GREEN})),
                                                // /*spanish*/el #científico del Lago Hylia# otorga con prisa #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_REWARD.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#el Cofre del Tesoro# premia #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#los juegos de azar# revelan #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_REWARD.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/hay una #probabilidad de 1/32# de ganar #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_1.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la primera sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_1.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar solo una vez# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_1.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el primer o segundo cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_2.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la segunda sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_2.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar dos veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_2.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el tercer o cuarto cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_3.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la tercera sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_3.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar tres veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_3.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el quinto o sexto cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_4.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la cuarta sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_4.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar cuatro veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_4.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el séptimo u octavo cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_5.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la quinta sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_5.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar cinco veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_ITEM_5.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el noveno o décimo cofre del azar# revela #[[1]]#.

   hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_KEY_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_1.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la primera sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_1.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar solo una vez# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_1.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el primer o segundo cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_KEY_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_2.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la segunda sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_2.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar dos veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_2.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el tercer o cuarto cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_KEY_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_3.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la tercera sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_3.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar tres veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_3.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el quinto o sexto cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_KEY_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_4.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la cuarta sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_4.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar cuatro veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_4.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el séptimo u octavo cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_KEY_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_5.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/#en la quinta sala del Cofre del Tesoro# aguarda #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_5.obscure.0", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/#apostar cinco veces# revelará #[[1]]#.
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_KEY_5.obscure.1", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/#el noveno o décimo cofre del azar# revela #[[1]]#.

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME_SHOPKEEPER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_SHOPKEEPER.clear", {QM_RED, QM_GREEN}),
                                                                        {},
                                                                        {CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME_SHOPKEEPER.obscure.0", {QM_RED, QM_GREEN})});

    hintTextTable[RHT_GF_HBA_1500_POINTS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_HBA_1500_POINTS.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/conseguir 1500 puntos en el #tiro con arco a caballo# premia #[[1]]#.
                                                     {},
                                                     {CustomMessages::CreateMessage("randomizer.hints.RHT_GF_HBA_1500_POINTS.obscure.0", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/dominar el #tiro con arco a caballo# premia con #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_HEART_PIECE_GRAVE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_HEART_PIECE_GRAVE_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                    // /*spanish*/#tocar la Canción del Sol# en una cripta conduce a #[[1]]#.

    hintTextTable[RHT_GC_MAZE_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MAZE_LEFT_CHEST.clear", {QM_RED, QM_GREEN})),
                                                     // /*spanish*/en la #Ciudad Goron# el martillo desbloquea #[[1]]#.

    hintTextTable[RHT_GV_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_CHEST.clear", {QM_RED, QM_GREEN})),
                                           // /*spanish*/en el #Valle Gerudo# el martillo desbloquea #[[1]]#.

    hintTextTable[RHT_GV_TRADE_SAW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_TRADE_SAW.clear", {QM_RED, QM_GREEN})),
                                               // /*spanish*/el #capataz de los carpinteros# otorga #[[1]]#.

    hintTextTable[RHT_GV_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_COW.clear", {QM_RED, QM_GREEN})),
                                         // /*spanish*/una #vaca del Valle Gerudo# brinda #[[1]]#.

    hintTextTable[RHT_HC_GS_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_GS_STORMS_GROTTO.clear", {QM_RED, QM_GREEN})),
                                                      // /*spanish*/una #Skulltula tras la agrietada pared# de una cueva otorga #[[1]]#.

    hintTextTable[RHT_HF_GS_COW_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_GS_COW_GROTTO.clear", {QM_RED, QM_GREEN})),
                                                   // /*spanish*/una #Skulltula tras la telaraña# de una cueva otorga #[[1]]#.

    hintTextTable[RHT_HF_COW_GROTTO_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_COW_GROTTO_COW.clear", {QM_RED, QM_GREEN}),
                                                    // /*spanish*/una #vaca tras la telaraña# de una cueva brinda #[[1]]#.
                                                    {},
                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_HF_COW_GROTTO_COW.obscure.0", {QM_RED, QM_GREEN})});
                                                     // /*spanish*/una #vaca tras una telaraña# brinda #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_OCARINA_GAME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_OCARINA_GAME.clear", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/la recompensa final de las #ranas del Río Zora# premia #[[1]]#.
                                                        {},
                                                        {CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_OCARINA_GAME.obscure.0", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/una #fiesta anfibia# brinda #[[1]]#.
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_OCARINA_GAME.obscure.1", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/un #coro maestro de ancas# premia #[[1]]#.
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_OCARINA_GAME.obscure.2", {QM_RED, QM_GREEN})});
                                                         // /*spanish*/el #gran final batracio# brinda #[[1]]#.

    hintTextTable[RHT_ZF_GS_HIDDEN_CAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_GS_HIDDEN_CAVE.clear", {QM_RED, QM_GREEN})),
                                                    // /*spanish*/una Skulltula en lo #alto de las congeladas aguas# otorga #[[1]]#.

    hintTextTable[RHT_WASTELAND_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WASTELAND_CHEST.clear", {QM_RED, QM_GREEN})),
                                                  // /*spanish*/en lo #profundo del desierto encantado# yace #[[1]]#.

    hintTextTable[RHT_WASTELAND_GS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WASTELAND_GS.clear", {QM_RED, QM_GREEN})),
                                               // /*spanish*/una #Skulltula del desierto encantado# otorga #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                     // /*spanish*/#las llamas del Panteón Real# revelan #[[1]]#.

    hintTextTable[RHT_ZF_BOTTOM_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_BOTTOM_FREESTANDING_POH.clear", {QM_RED, QM_GREEN})),
                                                             // /*spanish*/#bajo las congeladas aguas# yace #[[1]]#.

    hintTextTable[RHT_GC_POT_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_POT_FREESTANDING_POH.clear", {QM_RED, QM_GREEN})),
                                                          // /*spanish*/una #cerámica goron# contiene #[[1]]#.

    hintTextTable[RHT_ZD_KING_ZORA_THAWED] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_KING_ZORA_THAWED.clear", {QM_RED, QM_GREEN}),
                                                      // /*spanish*/#descongelar al Rey Zora# conduce a #[[1]]#.
                                                      {},
                                                      {CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_KING_ZORA_THAWED.obscure.0", {QM_RED, QM_GREEN})});
                                                       // /*spanish*/una #liberación monárquica# brinda #[[1]]#.

    hintTextTable[RHT_ZD_TRADE_PRESCRIPTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_TRADE_PRESCRIPTION.clear", {QM_RED, QM_GREEN})),
                                                        // /*spanish*/el #Rey Zora# otorga con prisa #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_DEKU_SCRUB.clear", {QM_RED, QM_GREEN})),
                                                 // /*spanish*/un solitario #deku del cráter# vende #[[1]]#.
    hintTextTable[RHT_DMC_GS_CRATE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GS_CRATE.clear", {QM_RED, QM_GREEN})),
                                               // /*spanish*/una Skulltula bajo una #caja del cráter# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                        // /*spanish*/un #bloque temporal del Gran Árbol Deku# contiene #[[1]]#.
                                                                        {},
                                                                        {CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                         // /*spanish*/un #bloque temporal de un árbol# contiene #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM.clear", {QM_RED, QM_GREEN}),
                                                                       // /*spanish*/una #Skulltula en el techo del Gran Árbol Deku# otorga #[[1]]#.
                                                                       {},
                                                                       {CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM.obscure.0", {QM_RED, QM_GREEN})});
                                                                        // /*spanish*/una #Skulltula en el techo de un árbol# otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM.clear", {QM_RED, QM_GREEN}),
                                                                                // /*spanish*/una Skulltula bajo #bloques temporales de la Cueva de los Dodongos# otorga #[[1]]#.
                                                                                {},
                                                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM.obscure.0", {QM_RED, QM_GREEN})});
                                                                                 // /*spanish*/una Skulltula bajo #bloques temporales de una cueva# otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_BOOMERANG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_BOOMERANG_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                   // /*spanish*/unos #stingers engullidos por Jabu-Jabu# guardan #[[1]]#.
                                                                   {},
                                                                   {CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_BOOMERANG_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                    // /*spanish*/unos de #stingers engullidos por cierta deidad# guardan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM.clear", {QM_RED, QM_GREEN}),
                                                                                // /*spanish*/una Skulltula rodeada de #sombras en la Tripa de Jabu-Jabu# otorga #[[1]]#.
                                                                                {},
                                                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM.obscure.0", {QM_RED, QM_GREEN})});
                                                                                 // /*spanish*/una Skulltula rodeada de #sombras en la tripa de cierta diedad# otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_COW.clear", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/una #vaca engullida por Jabu-Jabu# brinda #[[1]]#.
                                                          {},
                                                          {CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_COW.obscure.0", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/una #vaca engullida por cierta deidad# brinda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_SCARECROW_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SCARECROW_CHEST.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/#Pierre en lo alto del Templo del Fuego# esconde #[[1]]#.
                                                              {},
                                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_SCARECROW_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/un #espantapájaros en lo alto del volcán# esconde #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MEGATON_HAMMER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MEGATON_HAMMER_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                   // /*spanish*/el #Bailaguego en lo alto del Templo del Fuego# otorga #[[1]]#.
                                                                   {},
                                                                   {CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MEGATON_HAMMER_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                    // /*spanish*/el #Bailafuego en lo alto del volcán# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_CHEST_ON_FIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_CHEST_ON_FIRE.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/el #Bailafuego en lo alto del Templo del Fuego# otorga #[[1]]#.
                                                               {},
                                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_CHEST_ON_FIRE.obscure.0", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/el #Bailafuego en lo alto del volcán# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE.clear", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/una #Skulltula bajo un bloque del Templo del Fuego# otorga #[[1]]#.
                                                                  {},
                                                                  {CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE.obscure.0", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/una #Skulltula bajo el bloque de un volcán# otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_RIVER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_RIVER_CHEST.clear", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/tras el #río del Templo del Agua# yace #[[1]]#.
                                                           {},
                                                           {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_RIVER_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                            // /*spanish*/tras el #río bajo el lago# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/esquivar #rocas rondantes del Templo del Agua# conduce a #[[1]]#.
                                                              {},
                                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_BOSS_KEY_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/esquivar #rocas rodantes bajo el lago# conduce a #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_BEHIND_GATE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_BEHIND_GATE.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/una Skulltula tras #una valla del Templo del Agua# otorga #[[1]]#.
                                                              {},
                                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_BEHIND_GATE.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/una Skulltula tras #una valla bajo el lago# otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}),
                                                                   // /*spanish*/en una #caja del Templo del Agua# yace #[[1]]#.
                                                                   {},
                                                                   {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_FREESTANDING_KEY.obscure.0", {QM_RED, QM_GREEN})});
                                                                    // /*spanish*/en una #caja bajo el lago# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA.clear", {QM_RED, QM_GREEN}),
                                                                           // /*spanish*/la #Skulltula enjaulada del Templo del Agua# otorga #[[1]]#.
                                                                           {},
                                                                           {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA.obscure.0", {QM_RED, QM_GREEN})});
                                                                            // /*spanish*/la #Skulltula enjaulada bajo el lago# otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH.clear", {QM_RED, QM_GREEN}),
                                                                       // /*spanish*/una Skulltula tras una #valla del Templo del Agua# #[[1]]#.
                                                                       {},
                                                                       {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH.obscure.0", {QM_RED, QM_GREEN})});
                                                                        // /*spanish*/una Skulltula tras una #valla bajo el lago# otorga #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                                       // /*spanish*/aquellos que busquen las #rupias plateadas sumergidas# encontrarán

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                                          // /*spanish*/aquellos que busquen las #rupias plateadas sumergidas# encontrarán #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                               // /*spanish*/la recompensa final de la #instrucción de las bandida# brinda #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                             // /*spanish*/el premio final de la #instrucción de las bandidas# brinda #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                         // /*spanish*/la #Mano Muerta del pozo# concede #[[1]]#.
                                                                         {},
                                                                         {CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST.obscure.0", {QM_RED, QM_GREEN}),
                                                                          // /*spanish*/en las #profundidades del pozo# se esconde #[[1]]#.
                                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST.obscure.1", {QM_RED, QM_GREEN})});
                                                                          // /*spanish*/el #temido morador del pozo# concede #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/la #Mano Muerta del pozo# concede #[[1]]#.
                                                                      {},
                                                                      {CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST.obscure.0", {QM_RED, QM_GREEN}),
                                                                       // /*spanish*/en las #profundidades del pozo# se esconde #[[1]]#.
                                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST.obscure.1", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/el #temido morador del pozo# concede #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                       // /*spanish*/en la #mano derecha del Coloso# yace #[[1]]#.
                                                                       {},
                                                                       {CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                        // /*spanish*/el #ansiado tesoro de Nabooru# brinda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                    // /*spanish*/en la #mano izquierda del Coloso# yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                             // /*spanish*/una #paradoja temporal del Coloso# conduce a #[[1]]#.
                                                                             {},
                                                                             {CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                              // /*spanish*/una #paradoja temporal del Coloso# conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                       // /*spanish*/una #sinfonía del Coloso# conduce a #[[1]]#.
                                                                       {},
                                                                       {CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                        // /*spanish*/una #sinfonía del Coloso# conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/la #Skulltula de la sinfonía del Coloso# otorga #[[1]]#.
                                                                    {},
                                                                    {CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM.obscure.0", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/la #Skulltula de la sinfonía del Coloso# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                            // /*spanish*/las sombras del #laberinto misterioso# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST.clear", {QM_RED, QM_GREEN})),
                                                                     // /*spanish*/las sombras del #laberinto invisible# esconden #[[1]]#.

    /*--------------------------
    |    ENTRANCE HINT TEXT    |
    ---------------------------*/

    hintTextTable[RHT_DESERT_COLOSSUS_TO_COLOSSUS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DESERT_COLOSSUS_TO_COLOSSUS_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                     // /*spanish*/levantar una #roca del desierto# revela #[[1]]#.

    hintTextTable[RHT_GV_GROTTO_LEDGE_TO_GV_OCTOROK_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GROTTO_LEDGE_TO_GV_OCTOROK_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                       // /*spanish*/levantar una #roca al borde del valle# esconde #[[1]]#.

    hintTextTable[RHT_GC_GROTTO_PLATFORM_TO_GC_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_GROTTO_PLATFORM_TO_GC_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                  // /*spanish*/un #estanque de lava# en la Ciudad Goron bloquea el paso a #[[1]]#.

    hintTextTable[RHT_GERUDO_FORTRESS_TO_GF_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_TO_GF_STORMS_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                      // /*spanish*/una #tormenta en la Fortaleza Gerudo# revela #[[1]]#.

    hintTextTable[RHT_ZORAS_DOMAIN_TO_ZD_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_DOMAIN_TO_ZD_STORMS_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                   // /*spanish*/una #tormenta en la Región de los Zora# revela #[[1]]#.

    hintTextTable[RHT_HYRULE_CASTLE_GROUNDS_TO_HC_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HYRULE_CASTLE_GROUNDS_TO_HC_STORMS_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                            // /*spanish*/una #tormenta junto al castillo# revela #[[1]]#.

    hintTextTable[RHT_GV_FORTRESS_SIDE_TO_GV_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_FORTRESS_SIDE_TO_GV_STORMS_GROTTO.clear", {QM_RED, QM_BLUE})),
                                                                       // /*spanish*/una #tormenta en el valle# revela #[[1]]#.

    hintTextTable[RHT_DESERT_COLOSSUS_TO_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DESERT_COLOSSUS_TO_COLOSSUS_GREAT_FAIRY_FOUNTAIN.clear", {QM_RED, QM_BLUE})),
                                                                                   // /*spanish*/una #agrietada pared del desierto# esconde #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_GROUNDS_TO_OGC_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_GROUNDS_TO_OGC_GREAT_FAIRY_FOUNTAIN.clear", {QM_RED, QM_BLUE})),
                                                                                    // /*spanish*/una #pesada columna# fuera del castillo obstruye #[[1]]#.

    hintTextTable[RHT_ZORAS_FOUNTAIN_TO_ZF_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_FOUNTAIN_TO_ZF_GREAT_FAIRY_FOUNTAIN.clear", {QM_RED, QM_BLUE})),
                                                                            // /*spanish*/una #pared de la fuente# esconde #[[1]]#.

    hintTextTable[RHT_GV_FORTRESS_SIDE_TO_GV_CARPENTER_TENT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_FORTRESS_SIDE_TO_GV_CARPENTER_TENT.clear", {QM_RED, QM_BLUE})),
                                                                        // /*spanish*/una #tienda de campaña del valle# cubre #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_WARP_PAD_REGION_TO_SHADOW_TEMPLE_ENTRYWAY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_WARP_PAD_REGION_TO_SHADOW_TEMPLE_ENTRYWAY.clear", {QM_RED, QM_BLUE})),
                                                                                      // /*spanish*/en la #parte trasera del cementerio# se halla #[[1]]#.

    hintTextTable[RHT_LAKE_HYLIA_TO_WATER_TEMPLE_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LAKE_HYLIA_TO_WATER_TEMPLE_LOBBY.clear", {QM_RED, QM_BLUE})),
                                                                   // /*spanish*/en las #profundidades de un lago inmenso# se halla #[[1]]#.

    hintTextTable[RHT_GERUDO_FORTRESS_TO_GERUDO_TRAINING_GROUND_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS_TO_GERUDO_TRAINING_GROUND_LOBBY.clear", {QM_RED, QM_BLUE})),
                                                                                   // /*spanish*/pagarle una #tasa a las gerudo# da acceso a #[[1]]#.

    hintTextTable[RHT_ZORAS_FOUNTAIN_TO_JABU_JABUS_BELLY_BEGINNING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_FOUNTAIN_TO_JABU_JABUS_BELLY_BEGINNING.clear", {QM_RED, QM_BLUE})),
                                                                               // /*spanish*/dentro de #Jabu-Jabu# se halla #[[1]]#.

    hintTextTable[RHT_KAKARIKO_VILLAGE_TO_BOTTOM_OF_THE_WELL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAKARIKO_VILLAGE_TO_BOTTOM_OF_THE_WELL.clear", {QM_RED, QM_BLUE})),
                                                                         // /*spanish*/el #pozo de un pueblo# conduce a #[[1]]#.

    /*--------------------------
    |      EXIT HINT TEXT      |
    ---------------------------*/

    hintTextTable[RHT_LINKS_POCKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LINKS_POCKET.clear"));
                                               // /*spanish*/el bolsillo de @

    hintTextTable[RHT_KOKIRI_FOREST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_FOREST.clear"));
                                                // /*spanish*/el Bosque Kokiri

    hintTextTable[RHT_THE_LOST_WOODS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_THE_LOST_WOODS.clear"));
                                                 // /*spanish*/el Bosque Perdido

    hintTextTable[RHT_SACRED_FOREST_MEADOW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SACRED_FOREST_MEADOW.clear"));
                                                       // /*spanish*/la pradera sagrada del bosque

    hintTextTable[RHT_HYRULE_FIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HYRULE_FIELD.clear"));
                                               // /*spanish*/la Llanura de Hyrule

    hintTextTable[RHT_LAKE_HYLIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LAKE_HYLIA.clear"));
                                             // /*spanish*/el Lago Hylia

    hintTextTable[RHT_GERUDO_VALLEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_VALLEY.clear"));
                                                // /*spanish*/el Valle Gerudo

    hintTextTable[RHT_GERUDO_FORTRESS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_FORTRESS.clear"));
                                                  // /*spanish*/la Fortaleza Gerudo

    hintTextTable[RHT_THIEVES_HIDEOUT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_THIEVES_HIDEOUT.clear"));
                                                  // /*spanish*/la Guarida de los Ladrones

    hintTextTable[RHT_HAUNTED_WASTELAND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HAUNTED_WASTELAND.clear"));
                                                    // /*spanish*/el desierto encantado

    hintTextTable[RHT_DESERT_COLOSSUS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DESERT_COLOSSUS.clear"));
                                                  // /*spanish*/el Coloso del Desierto

    hintTextTable[RHT_THE_MARKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_THE_MARKET.clear"));
                                             // /*spanish*/la plaza del mercado

    hintTextTable[RHT_TEMPLE_OF_TIME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TEMPLE_OF_TIME.clear"));
                                                 // /*spanish*/el Templo del Tiempo

    hintTextTable[RHT_CASTLE_GROUNDS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CASTLE_GROUNDS.clear"));
                                                 // /*spanish*/

    hintTextTable[RHT_HYRULE_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HYRULE_CASTLE.clear"));
                                                // /*spanish*/el Castillo de Hyrule

    hintTextTable[RHT_OUTSIDE_GANONS_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OUTSIDE_GANONS_CASTLE.clear"));
                                                        // /*spanish*/el exterior del Castillo de Ganon

    hintTextTable[RHT_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAKARIKO_VILLAGE.clear"));
                                                   // /*spanish*/Kakariko

    hintTextTable[RHT_THE_GRAVEYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_THE_GRAVEYARD.clear"));
                                                // /*spanish*/el cementerio

    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_TRAIL.clear"));
                                                       // /*spanish*/el sendero de la Montaña de la Muerte

    hintTextTable[RHT_GORON_CITY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GORON_CITY.clear"));
                                             // /*spanish*/la Ciudad Goron

    hintTextTable[RHT_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_CRATER.clear"));
                                                        // /*spanish*/el cráter de la Montaña de la Muerte

    hintTextTable[RHT_ZORAS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_RIVER.clear"));
                                              // /*spanish*/el Río Zora

    hintTextTable[RHT_ZORAS_DOMAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_DOMAIN.clear"));
                                               // /*spanish*/la Región de los Zora

    hintTextTable[RHT_ZORAS_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_FOUNTAIN.clear"));
                                                 // /*spanish*/la Fuente Zora

    hintTextTable[RHT_LON_LON_RANCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LON_LON_RANCH.clear"));
                                                // /*spanish*/el Rancho Lon Lon

    /*--------------------------
    |     REGION HINT TEXT     |
    ---------------------------*/

    hintTextTable[RHT_KF_LINKS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_LINKS_HOUSE.clear"));
                                                 // /*spanish*/la casa de @

    hintTextTable[RHT_KF_MIDOS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_HOUSE.clear"));
                                                 // /*spanish*/la casa de Mido

    hintTextTable[RHT_KF_SARIAS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SARIAS_HOUSE.clear"));
                                                  // /*spanish*/la casa de Saria

    hintTextTable[RHT_KF_HOUSE_OF_TWINS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_HOUSE_OF_TWINS.clear"));
                                                    // /*spanish*/la casa de las gemelas

    hintTextTable[RHT_KF_KNOW_IT_ALL_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_KNOW_IT_ALL_HOUSE.clear"));
                                                       // /*spanish*/la casa de los hermanos Sabelotodo

    hintTextTable[RHT_KF_KOKIRI_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_KOKIRI_SHOP.clear"));
                                                 // /*spanish*/la tienda kokiri

    hintTextTable[RHT_LH_LAB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_LAB.clear"));
                                         // /*spanish*/el laboratorio del lago

    hintTextTable[RHT_LH_FISHING_POND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_FISHING_POND.clear"));
                                                  // /*spanish*/el estanque

    hintTextTable[RHT_GV_CARPENTER_TENT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_CARPENTER_TENT.clear"));
                                                    // /*spanish*/la #tienda de campaña de los carpinteros#

    hintTextTable[RHT_MARKET_GUARD_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_GUARD_HOUSE.clear"));
                                                     // /*spanish*/la caseta de guardia

    hintTextTable[RHT_MARKET_MASK_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_MASK_SHOP.clear"));
                                                   // /*spanish*/la tienda de La Máscara Feliz

    hintTextTable[RHT_MARKET_BOMBCHU_BOWLING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_BOWLING.clear"));
                                                         // /*spanish*/la Bolera Bombchu

    hintTextTable[RHT_MARKET_POTION_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP.clear"));
                                                     // /*spanish*/la tienda de pociones de la plaza del mercado

    hintTextTable[RHT_MARKET_TREASURE_CHEST_GAME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_TREASURE_CHEST_GAME.clear"));
                                                             // /*spanish*/el Cofre del Tesoro

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP.clear"));
                                                      // /*spanish*/la Tienda Bombchu

    hintTextTable[RHT_MARKET_MAN_IN_GREEN_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_MAN_IN_GREEN_HOUSE.clear"));
                                                            // /*spanish*/la casa del hombre de verde

    hintTextTable[RHT_KAK_WINDMILL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_WINDMILL.clear"));
                                               // /*spanish*/el #molino#

    hintTextTable[RHT_KAK_CARPENTER_BOSS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_CARPENTER_BOSS_HOUSE.clear"));
                                                           // /*spanish*/la casa del capataz de los carpinteros

    hintTextTable[RHT_KAK_HOUSE_OF_SKULLTULA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_HOUSE_OF_SKULLTULA.clear"));
                                                         // /*spanish*/la casa de las Skulltulas

    hintTextTable[RHT_KAK_IMPAS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_IMPAS_HOUSE.clear"));
                                                  // /*spanish*/la casa de Impa

    hintTextTable[RHT_KAK_IMPAS_HOUSE_BACK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_IMPAS_HOUSE_BACK.clear"));
                                                       // /*spanish*/la jaula de la vaca de Impa

    hintTextTable[RHT_KAK_ODD_POTION_BUILDING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ODD_POTION_BUILDING.clear"));
                                                          // /*spanish*/la tienda de pociones de la abuela

    hintTextTable[RHT_GRAVEYARD_DAMPES_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_DAMPES_HOUSE.clear"));
                                                         // /*spanish*/la cabaña de Dampé

    hintTextTable[RHT_GC_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP.clear"));
                                          // /*spanish*/la #tienda goron#

    hintTextTable[RHT_ZD_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP.clear"));
                                          // /*spanish*/la #tienda zora#

    hintTextTable[RHT_LLR_TALONS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_TALONS_HOUSE.clear"));
                                                   // /*spanish*/la casa de Talon

    hintTextTable[RHT_LLR_STABLES] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_STABLES.clear"));
                                              // /*spanish*/el establo

    hintTextTable[RHT_LLR_TOWER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_TOWER.clear"));
                                            // /*spanish*/la torre Lon Lon

    hintTextTable[RHT_MARKET_BAZAAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR.clear"));
                                                // /*spanish*/el bazar de la plaza del mercado

    hintTextTable[RHT_MARKET_SHOOTING_GALLERY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_SHOOTING_GALLERY.clear"));
                                                          // /*spanish*/el Tiro al Blanco con tirachinas

    hintTextTable[RHT_KAK_BAZAAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR.clear"));
                                             // /*spanish*/el bazar de Kakariko

    hintTextTable[RHT_KAK_POTION_SHOP_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_FRONT.clear"));
                                                        // /*spanish*/la tienda de pociones de Kakariko

    hintTextTable[RHT_KAK_POTION_SHOP_BACK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_BACK.clear"));
                                                       // /*spanish*/la tienda de pociones de Kakariko

    hintTextTable[RHT_KAK_SHOOTING_GALLERY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_SHOOTING_GALLERY.clear"));
                                                       // /*spanish*/el Tiro al Blanco con arco

    hintTextTable[RHT_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GREAT_FAIRY_FOUNTAIN.clear"));
                                                                // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_HC_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_GREAT_FAIRY_FOUNTAIN.clear"));
                                                          // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_OGC_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OGC_GREAT_FAIRY_FOUNTAIN.clear"));
                                                           // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_DMC_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GREAT_FAIRY_FOUNTAIN.clear"));
                                                           // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_DMT_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GREAT_FAIRY_FOUNTAIN.clear"));
                                                           // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_ZF_GREAT_FAIRY_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_GREAT_FAIRY_FOUNTAIN.clear"));
                                                          // /*spanish*/una fuente de la Gran Hada

    hintTextTable[RHT_GRAVEYARD_SHIELD_GRAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_SHIELD_GRAVE.clear"));
                                                         // /*spanish*/la #tumba con un cofre#

    hintTextTable[RHT_GRAVEYARD_HEART_PIECE_GRAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_HEART_PIECE_GRAVE.clear"));
                                                              // /*spanish*/la #tumba de la Canción del Sol#

    hintTextTable[RHT_GRAVEYARD_COMPOSERS_GRAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_COMPOSERS_GRAVE.clear"));
                                                            // /*spanish*/el #Panteón Real#

    hintTextTable[RHT_GRAVEYARD_DAMPES_GRAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_DAMPES_GRAVE.clear"));
                                                         // /*spanish*/la #tumba de Dampé#

    hintTextTable[RHT_DMT_COW_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_COW_GROTTO.clear"));
                                                 // /*spanish*/una #vaca# solitaria

    hintTextTable[RHT_HC_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_STORMS_GROTTO.clear"));
                                                   // /*spanish*/la arenosa gruta de #frágiles paredes#

    hintTextTable[RHT_HF_TEKTITE_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_TEKTITE_GROTTO.clear"));
                                                    // /*spanish*/un charco custodiado por un #Tektite#

    hintTextTable[RHT_HF_NEAR_KAK_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_KAK_GROTTO.clear"));
                                                     // /*spanish*/una #gran Skulltula# custodiando una dorada

    hintTextTable[RHT_HF_COW_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_COW_GROTTO.clear"));
                                                // /*spanish*/una gruta llena de #telarañas#

    hintTextTable[RHT_KAK_REDEAD_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_REDEAD_GROTTO.clear"));
                                                    // /*spanish*/los #ReDeads# que custodian un cofre

    hintTextTable[RHT_SFM_WOLFOS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_WOLFOS_GROTTO.clear"));
                                                    // /*spanish*/los #Wolfos# que custodian un cofre

    hintTextTable[RHT_GV_OCTOROK_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_OCTOROK_GROTTO.clear"));
                                                    // /*spanish*/un #Octorok# que custodia un lujoso charco

    hintTextTable[RHT_DEKU_THEATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_THEATER.clear"));
                                               // /*spanish*/el #escenario del Bosque Perdido#

    hintTextTable[RHT_ZR_OPEN_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_OPEN_GROTTO.clear"));
                                                 // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_DMC_UPPER_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_UPPER_GROTTO.clear"));
                                                   // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_DMT_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_STORMS_GROTTO.clear"));
                                                    // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_KAK_OPEN_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_OPEN_GROTTO.clear"));
                                                  // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_MARKET_GROTTO.clear"));
                                                        // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_HF_OPEN_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_OPEN_GROTTO.clear"));
                                                 // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SOUTHEAST_GROTTO.clear"));
                                                      // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_KF_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_STORMS_GROTTO.clear"));
                                                   // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_NEAR_SHORTCUTS_GROTTO.clear"));
                                                           // /*spanish*/una #cueva genérica#

    hintTextTable[RHT_HF_INSIDE_FENCE_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_INSIDE_FENCE_GROTTO.clear"));
                                                         // /*spanish*/una cueva con un #solitario mercader deku#

    hintTextTable[RHT_LW_SCRUBS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_SCRUBS_GROTTO.clear"));
                                                   // /*spanish*/una cueva con #dos mercaderes deku#

    hintTextTable[RHT_COLOSSUS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GROTTO.clear"));
                                                  // /*spanish*/una cueva con #dos mercaderes deku#

    hintTextTable[RHT_ZR_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_STORMS_GROTTO.clear"));
                                                   // /*spanish*/una cueva con #dos mercaderes deku#

    hintTextTable[RHT_SFM_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_STORMS_GROTTO.clear"));
                                                    // /*spanish*/una cueva con #dos mercaderes deku#

    hintTextTable[RHT_GV_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_STORMS_GROTTO.clear"));
                                                   // /*spanish*/una cueva con #dos mercaderes deku#

    hintTextTable[RHT_LH_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GROTTO.clear"));
                                            // /*spanish*/una cueva con #tres mercaderes deku#

    hintTextTable[RHT_DMC_HAMMER_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_HAMMER_GROTTO.clear"));
                                                    // /*spanish*/una cueva con #tres mercaderes deku#

    hintTextTable[RHT_GC_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_GROTTO.clear"));
                                            // /*spanish*/una cueva con #tres mercaderes deku#

    hintTextTable[RHT_LLR_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_GROTTO.clear"));
                                             // /*spanish*/una cueva con #tres mercaderes deku#

    hintTextTable[RHT_ZR_FAIRY_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FAIRY_GROTTO.clear"));
                                                  // /*spanish*/una pequeña #fuente de hadas#

    hintTextTable[RHT_HF_FAIRY_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_FAIRY_GROTTO.clear"));
                                                  // /*spanish*/una pequeña #fuente de hadas#

    hintTextTable[RHT_SFM_FAIRY_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_FAIRY_GROTTO.clear"));
                                                   // /*spanish*/una pequeña #fuente de hadas#

    hintTextTable[RHT_ZD_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_STORMS_GROTTO.clear"));
                                                   // /*spanish*/una pequeña #fuente de hadas#

    hintTextTable[RHT_GF_STORMS_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_STORMS_GROTTO.clear"));
                                                   // /*spanish*/una pequeña #fuente de hadas#

    /*--------------------------
    |      JUNK HINT TEXT      |
    ---------------------------*/

    hintTextTable[RHT_JUNK01] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK01.clear"));

    hintTextTable[RHT_JUNK02] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK02.clear"));

    hintTextTable[RHT_JUNK03] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK03.clear"));

    hintTextTable[RHT_JUNK04] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK04.clear"));

    hintTextTable[RHT_JUNK05] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK05.clear"));

    hintTextTable[RHT_JUNK06] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK06.clear"));

    hintTextTable[RHT_JUNK07] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK07.clear"));

    hintTextTable[RHT_JUNK08] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK08.clear"));

    hintTextTable[RHT_JUNK09] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK09.clear"));

    hintTextTable[RHT_JUNK10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK10.clear"));

    hintTextTable[RHT_JUNK11] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK11.clear"));

    hintTextTable[RHT_JUNK12] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK12.clear"));

    hintTextTable[RHT_JUNK13] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK13.clear"));

    hintTextTable[RHT_JUNK14] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK14.clear"));

    hintTextTable[RHT_JUNK15] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK15.clear"));

    hintTextTable[RHT_JUNK16] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK16.clear"));

    hintTextTable[RHT_JUNK17] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK17.clear"));

    hintTextTable[RHT_JUNK18] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK18.clear"));

    hintTextTable[RHT_JUNK19] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK19.clear"));

    hintTextTable[RHT_JUNK20] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK20.clear"));

    hintTextTable[RHT_JUNK21] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK21.clear"));

    hintTextTable[RHT_JUNK22] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK22.clear"));

    hintTextTable[RHT_JUNK23] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK23.clear"));

    hintTextTable[RHT_JUNK24] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK24.clear"));

    hintTextTable[RHT_JUNK25] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK25.clear"));

    hintTextTable[RHT_JUNK26] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK26.clear"));

    hintTextTable[RHT_JUNK27] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK27.clear"));

    hintTextTable[RHT_JUNK28] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK28.clear"));

    hintTextTable[RHT_JUNK29] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK29.clear"));

    hintTextTable[RHT_JUNK30] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK30.clear"));

    hintTextTable[RHT_JUNK31] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK31.clear"));

    hintTextTable[RHT_JUNK32] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK32.clear"));

    hintTextTable[RHT_JUNK33] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK33.clear"));

    hintTextTable[RHT_JUNK34] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK34.clear"));

    hintTextTable[RHT_JUNK35] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK35.clear"));

    hintTextTable[RHT_JUNK36] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK36.clear"));

    hintTextTable[RHT_JUNK37] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK37.clear"));

    hintTextTable[RHT_JUNK38] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK38.clear"));

    hintTextTable[RHT_JUNK39] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK39.clear"));

    hintTextTable[RHT_JUNK40] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK40.clear"));

    hintTextTable[RHT_JUNK41] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK41.clear"));

    hintTextTable[RHT_JUNK42] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK42.clear"));

    hintTextTable[RHT_JUNK43] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK43.clear"));

    hintTextTable[RHT_JUNK44] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK44.clear"));

    hintTextTable[RHT_JUNK45] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK45.clear"));

    hintTextTable[RHT_JUNK46] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK46.clear"));

    hintTextTable[RHT_JUNK47] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK47.clear"));

    hintTextTable[RHT_JUNK48] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK48.clear"));

    hintTextTable[RHT_JUNK49] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK49.clear"));

    hintTextTable[RHT_JUNK50] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK50.clear"));

    hintTextTable[RHT_JUNK51] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK51.clear"));

    hintTextTable[RHT_JUNK52] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK52.clear"));

    hintTextTable[RHT_JUNK53] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK53.clear"));

    hintTextTable[RHT_JUNK54] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK54.clear"));

    hintTextTable[RHT_JUNK55] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK55.clear"));

    hintTextTable[RHT_JUNK56] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK56.clear"));

    hintTextTable[RHT_JUNK57] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK57.clear"));

    hintTextTable[RHT_JUNK58] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK58.clear"));

    hintTextTable[RHT_JUNK59] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK59.clear"));

    hintTextTable[RHT_JUNK60] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK60.clear"));

    hintTextTable[RHT_JUNK61] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK61.clear"));

    hintTextTable[RHT_JUNK62] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK62.clear"));

    hintTextTable[RHT_JUNK63] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK63.clear"));

    // ^ Junk hints above are from 3drando
    // v Junk hints below are new to soh rando

    // Please keep hints to stuff related to ship directly, or to Nintendo/Zelda related stuff.
    // And nothing that's super obscure that no one's going to understand.

    hintTextTable[RHT_JUNK64] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK64.clear"));

    hintTextTable[RHT_JUNK65] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK65.clear" ));

    hintTextTable[RHT_JUNK66] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK66.clear"));

    hintTextTable[RHT_JUNK67] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK67.clear"));

    hintTextTable[RHT_JUNK68] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK68.clear"));

    hintTextTable[RHT_JUNK69] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK69.clear"));

    hintTextTable[RHT_JUNK70] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK70.clear"));

    hintTextTable[RHT_JUNK71] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JUNK71.clear"));

    /*--------------------------
    |     DUNGEON HINT TEXT    |
    ---------------------------*/


    hintTextTable[RHT_DEKU_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE.clear"),
                                            // /*spanish*/el Gran Árbol Deku
                                            {},
                                            {CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE.obscure.0")});
                                             // /*spanish*/un ancestral árbol

    hintTextTable[RHT_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN.clear"),
                                                  // /*spanish*/la Cueva de los Dodongos
                                                  {},
                                                  {CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN.obscure.0")});
                                                   // /*spanish*/una descomunal cueva

    hintTextTable[RHT_JABU_JABUS_BELLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY.clear"),
                                                   // /*spanish*/tripa de Jabu-Jabu
                                                   {},
                                                   {CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY.obscure.0")});
                                                    // /*spanish*/la tripa de cierta deidad

    hintTextTable[RHT_FOREST_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE.clear"),
                                                // /*spanish*/el Templo del Bosque
                                                {},
                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE.obscure.0")});
                                                 // /*spanish*/las profundidades del bosque

    hintTextTable[RHT_FIRE_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE.clear"),
                                              // /*spanish*/el Templo del Fuego
                                              {},
                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE.obscure.0")});
                                               // /*spanish*/una alta montaña

    hintTextTable[RHT_WATER_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE.clear"),
                                               // /*spanish*/el Templo del Agua
                                               {},
                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE.obscure.0")});
                                                // /*spanish*/un lago inmenso

    hintTextTable[RHT_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE.clear"),
                                                // /*spanish*/el Templo del Espíritu
                                                {},
                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE.obscure.0")});
                                                 // /*spanish*/la diosa de las arenas

    hintTextTable[RHT_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE.clear"),
                                                // /*spanish*/el Templo de las Sombras
                                                {},
                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE.obscure.0")});
                                                 // /*spanish*/la casa de la muerte

    hintTextTable[RHT_ICE_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN.clear"),
                                             // /*spanish*/la caverna de hielo
                                             {},
                                             {CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN.obscure.0")});
                                              // /*spanish*/un gélido laberinto

    hintTextTable[RHT_BOTTOM_OF_THE_WELL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL.clear"),
                                                     // /*spanish*/el fondo del pozo
                                                     {},
                                                     {CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL.obscure.0")});
                                                      // /*spanish*/la prisión de las sombras

    hintTextTable[RHT_GERUDO_TRAINING_GROUND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND.clear"),
                                                         // /*spanish*/el Centro de Instrucción Gerudo
                                                         {},
                                                         {CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND.obscure.0")});
                                                          // /*spanish*/la prueba de las bandidas

    hintTextTable[RHT_GANONS_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE.clear"),
                                                // /*spanish*/el interior del Castillo de Ganon
                                                {},
                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE.obscure.0")});
                                                 // /*spanish*/una conquistada ciudadela

    /*--------------------------
    |      BOSS HINT TEXT      |
    ---------------------------*/
//RANDOTODO check the beginning and end on the french translations
    hintTextTable[RHT_QUEEN_GOHMA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_QUEEN_GOHMA.clear", {QM_RED, QM_GREEN}),
                                              // /*spanish*/la #Reina Goma# porta #[[1]]#.
                                              {},
                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_QUEEN_GOHMA.obscure.0", {QM_RED, QM_GREEN})});
                                               // /*spanish*/el #arácnido parasitario acorazado# porta #[[1]]#.

    hintTextTable[RHT_KING_DODONGO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO.clear", {QM_RED, QM_GREEN}),
                                               // /*spanish*/el #Rey Dodongo# porta #[[1]]#.
                                               {},
                                               {CustomMessages::CreateMessage("randomizer.hints.RHT_KING_DODONGO.obscure.0", {QM_RED, QM_GREEN})});
                                                // /*spanish*/el #dinosaurio infernal# porta #[[1]]#.

    hintTextTable[RHT_BARINADE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE.clear", {QM_RED, QM_GREEN}),
                                           // /*spanish*/#Barinade# porta #[[1]]#.
                                           {},
                                           {CustomMessages::CreateMessage("randomizer.hints.RHT_BARINADE.obscure.0", {QM_RED, QM_GREEN})});
                                            // /*spanish*/la #anémona bioeléctrica# porta #[[1]]#.

    hintTextTable[RHT_PHANTOM_GANON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON.clear", {QM_RED, QM_GREEN}),
                                                // /*spanish*/#Ganon Fantasma# porta #[[1]]#.
                                                {},
                                                {CustomMessages::CreateMessage("randomizer.hints.RHT_PHANTOM_GANON.obscure.0", {QM_RED, QM_GREEN})});
                                                 // /*spanish*/el #espíritu maligno de ultratumba# porta #[[1]]#.

    hintTextTable[RHT_VOLVAGIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA.clear", {QM_RED, QM_GREEN}),
                                           // /*spanish*/#Volvagia# porta #[[1]]#.
                                           {},
                                           {CustomMessages::CreateMessage("randomizer.hints.RHT_VOLVAGIA.obscure.0", {QM_RED, QM_GREEN})});
                                            // /*spanish*/el #dragón de lava subterráneo# porta #[[1]]#.

    hintTextTable[RHT_MORPHA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA.clear", {QM_RED, QM_GREEN}),
                                         // /*spanish*/#Morpha# porta #[[1]]#.
                                         {},
                                         {CustomMessages::CreateMessage("randomizer.hints.RHT_MORPHA.obscure.0", {QM_RED, QM_GREEN})});
                                          // /*spanish*/la #ameba acuática gigante# porta #[[1]]#.

    hintTextTable[RHT_BONGO_BONGO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO.clear", {QM_RED, QM_GREEN}),
                                              // /*spanish*/#Bongo Bongo# porta #[[1]]#.
                                              {},
                                              {CustomMessages::CreateMessage("randomizer.hints.RHT_BONGO_BONGO.obscure.0", {QM_RED, QM_GREEN})});
                                               // /*spanish*/la #alimaña oscura espectral# porta #[[1]]#.

    hintTextTable[RHT_TWINROVA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA.clear", {QM_RED, QM_GREEN}),
                                           // /*spanish*/#Birova# porta #[[1]]#.
                                           {},
                                           {CustomMessages::CreateMessage("randomizer.hints.RHT_TWINROVA.obscure.0", {QM_RED, QM_GREEN})});
                                            // /*spanish*/las #hermanas hechiceras# portan #[[1]]#.

    hintTextTable[RHT_GIFT_FROM_RAURU] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GIFT_FROM_RAURU.clear", {QM_RED, QM_GREEN}),
                                                 {},
                                                 {CustomMessages::CreateMessage("randomizer.hints.RHT_GIFT_FROM_RAURU.obscure.0", {QM_RED, QM_GREEN})});

    /*--------------------------
    |     BRIDGE HINT TEXT     |
    ---------------------------*/

    hintTextTable[RHT_BRIDGE_OPEN_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_OPEN_HINT.clear",
                                                                 {QM_LBLUE}, {}, TEXTBOX_TYPE_BLUE));
                                                   // /*spanish*/$lLos sabios #ya habrán creado un puente#&al castillo, de donde emana el mal.^


    hintTextTable[RHT_BRIDGE_VANILLA_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_VANILLA_HINT.clear",
                                                                   {QM_RED, QM_YELLOW}, {}, TEXTBOX_TYPE_BLUE));
                                                     // /*spanish*/$6Los sabios aguardarán a que el héroe obtenga tanto el #Medallón de las Sombras y el del Espíritu# junto
                                                                // a la #flecha de luz#.^

    hintTextTable[RHT_BRIDGE_STONES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_STONES_HINT.clear",
                                                                   {QM_BLUE}, {}, TEXTBOX_TYPE_BLUE));
                                                     // /*spanish*/$0Los sabios aguardarán a que el héroe&obtenga #[[d]] |piedra espiritual|piedras espirituales|#.^

    hintTextTable[RHT_BRIDGE_MEDALLIONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_MEDALLIONS_HINT.clear",
                                                                       {QM_RED}, {}, TEXTBOX_TYPE_BLUE));
                                                         // /*spanish*/$8Los sabios aguardarán a que el héroe&obtenga #[[d]] |medallón|medallones|#.^

    hintTextTable[RHT_BRIDGE_REWARDS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_REWARDS_HINT.clear",
                                                                     {QM_YELLOW, QM_BLUE, QM_RED}, {}, TEXTBOX_TYPE_BLUE));
                                                      // /*spanish*/$CLos sabios aguardarán a que el héroe obtenga #[[d]]# |#piedra espiritual# o #medallón#|
                                                                    //#piedras espirtuales# y #medallones#|.^

    hintTextTable[RHT_BRIDGE_DUNGEONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_DUNGEONS_HINT.clear",
                                                                     {QM_PINK}, {}, TEXTBOX_TYPE_BLUE));
                                                       // /*spanish*/$mLos sabios aguardarán a que el héroe complete #[[d]] mazmorra||s|#.^

    hintTextTable[RHT_BRIDGE_TOKENS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_TOKENS_HINT.clear",
                                                                   {QM_YELLOW}, {}, TEXTBOX_TYPE_BLUE));
                                                     // /*spanish*/$sLos sabios aguardarán a que el héroe obtenga #[[d]] símbolo||s| de skulltula dorada#.^

    hintTextTable[RHT_BRIDGE_TRIFORCE_PIECES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_TRIFORCE_PIECES_HINT.clear",
                                                                            {QM_YELLOW}, {}, TEXTBOX_TYPE_BLUE));

    hintTextTable[RHT_BRIDGE_GREG_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BRIDGE_GREG_HINT.clear",
                                                                 {QM_GREEN}, {}, TEXTBOX_TYPE_BLUE));


    /*--------------------------
    | GANON BOSS KEY HINT TEXT |
    ---------------------------*/

    hintTextTable[RHT_GANON_BK_START_WITH_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_START_WITH_HINT.clear",
                                                                         {QM_PINK, QM_LBLUE}));
                                                           // /*spanish*/$bY obtendrás la llave del #señor del mal# desde el #inicio#.^

    hintTextTable[RHT_GANON_BK_VANILLA_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_VANILLA_HINT.clear",
                                                                      {QM_PINK, QM_LBLUE}));
                                                        // /*spanish*/$bY la llave del #señor del mal# aguardará en un gran cofre de #su torre#.^

    hintTextTable[RHT_GANON_BK_OWN_DUNGEON_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_OWN_DUNGEON_HINT.clear",
                                                                          {QM_PINK, QM_PINK}));
                                                            // /*spanish*/$bY la llave del #señor del mal# aguardará en #algún lugar de su castillo#.^

    hintTextTable[RHT_GANON_BK_OVERWORLD_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_OVERWORLD_HINT.clear",
                                                                        {QM_PINK, QM_GREEN}));
                                                          // /*spanish*/$bY la llave del #señor del mal# aguardará #fuera de las mazmorras# de Hyrule.^

    hintTextTable[RHT_GANON_BK_ANY_DUNGEON_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_ANY_DUNGEON_HINT.clear",
                                                                          {QM_PINK, QM_PINK}));
                                                            // /*spanish*/$bY la llave del #señor del mal# aguardará #en una mazmorra# de Hyrule.^

    hintTextTable[RHT_GANON_BK_ANYWHERE_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_BK_ANYWHERE_HINT.clear",
                                                                       {QM_PINK, QM_BLUE}));
                                                         // /*spanish*/$bY la llave del #señor del mal# aguardará en #cualquier lugar de Hyrule#.^

    hintTextTable[RHT_GBK_MEDALLIONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_MEDALLIONS_HINT.clear",
                                                                     {QM_PINK, QM_YELLOW, QM_RED}));
                                                       // /*spanish*/$bY #Zelda# entregará la llave&del #señor del mal# tras obtener #[[d]] |medallón|medallones|#.^

    hintTextTable[RHT_GBK_STONES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_STONES_HINT.clear",
                                                                 {QM_PINK, QM_YELLOW, QM_BLUE}));
                                                   // /*spanish*/$bY #Zelda# entregará la llave del #señor del mal# tras obtener #[[d]] piedra| espiritual|s espirituales|#.^

    hintTextTable[RHT_GBK_REWARDS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_REWARDS_HINT.clear",
                                                                  {QM_PINK, QM_YELLOW, QM_YELLOW, QM_BLUE, QM_RED}));
                                                    // /*spanish*/$bY #Zelda# entregará la llave del #señor del mal# tras obtener #[[d]]# piedra| espiritual o medallón|s espirituales o medallones|#.^

    hintTextTable[RHT_GBK_DUNGEONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_DUNGEONS_HINT.clear",
                                                                  {QM_PINK, QM_YELLOW, QM_PINK}));
                                                     // /*spanish*/$bY #Zelda# entregará la llave del #señor del mal# tras completar #[[d]] mazmorra||s|#.^

    hintTextTable[RHT_GBK_TOKENS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_TOKENS_HINT.clear",
                                                                  {QM_PINK, QM_YELLOW, QM_YELLOW}));
                                                    // /*spanish*/$bY #Zelda# entregará la llave del #señor del mal# tras obtener #[[d]] símbolo
                                                              // ||s| de skulltula dorada#.^

    hintTextTable[RHT_GBK_TRIFORCE_PIECES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GBK_TRIFORCE_PIECES_HINT.clear",
                                                                         {QM_PINK, QM_YELLOW, QM_YELLOW}));

    /*--------------------------
    |  GANON'S SOUL HINT TEXT  |
    ---------------------------*/

    hintTextTable[RHT_GANONS_SOUL_MEDALLIONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_MEDALLIONS_HINT.clear",
                                                                            {QM_PINK, QM_YELLOW, QM_RED}));

    hintTextTable[RHT_GANONS_SOUL_STONES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_STONES_HINT.clear",
                                                                        {QM_PINK, QM_YELLOW, QM_BLUE}));

    hintTextTable[RHT_GANONS_SOUL_REWARDS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_REWARDS_HINT.clear",
                                                                         {QM_PINK, QM_YELLOW, QM_YELLOW, QM_BLUE, QM_RED}));

    hintTextTable[RHT_GANONS_SOUL_DUNGEONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_DUNGEONS_HINT.clear",
                                                                          {QM_PINK, QM_YELLOW, QM_PINK}));

    hintTextTable[RHT_GANONS_SOUL_TOKENS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_TOKENS_HINT.clear",
                                                                        {QM_PINK, QM_YELLOW, QM_YELLOW}));

    hintTextTable[RHT_GANONS_SOUL_TRIFORCE_PIECES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_SOUL_TRIFORCE_PIECES_HINT.clear",
                                                                                {QM_PINK, QM_YELLOW, QM_YELLOW}));

    /*--------------------------
    |      WINCON HINT TEXT     |
    ---------------------------*/

    hintTextTable[RHT_WINCON_DEFEAT_GANON_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_DEFEAT_GANON_HINT.clear",
                                                                           {QM_YELLOW, QM_PINK}));

    hintTextTable[RHT_WINCON_ANYWHERE_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_ANYWHERE_HINT.clear",
                                                                     {QM_YELLOW, QM_BLUE}));

    hintTextTable[RHT_WINCON_MEDALLIONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_MEDALLIONS_HINT.clear",
                                                                       {QM_YELLOW, QM_YELLOW, QM_RED}));

    hintTextTable[RHT_WINCON_STONES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_STONES_HINT.clear",
                                                                   {QM_YELLOW, QM_YELLOW, QM_BLUE}));

    hintTextTable[RHT_WINCON_REWARDS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_REWARDS_HINT.clear",
                                                                    {QM_YELLOW, QM_YELLOW, QM_YELLOW, QM_BLUE, QM_RED}));

    hintTextTable[RHT_WINCON_DUNGEONS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_DUNGEONS_HINT.clear",
                                                                     {QM_YELLOW, QM_YELLOW, QM_PINK}));

    hintTextTable[RHT_WINCON_TOKENS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_TOKENS_HINT.clear",
                                                                   {QM_YELLOW, QM_YELLOW, QM_YELLOW}));

    hintTextTable[RHT_WINCON_TRIFORCE_PIECES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WINCON_TRIFORCE_PIECES_HINT.clear",
                                                                           {QM_YELLOW, QM_YELLOW, QM_YELLOW}));

    /*--------------------------
    |     TRIAL HINT TEXT      |
    ---------------------------*/

    hintTextTable[RHT_SIX_TRIALS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIX_TRIALS.clear", {QM_PINK}));
                                             // /*spanish*/Según dicen, la #torre de Ganon# está protegida por una poderosa barrera

    hintTextTable[RHT_ZERO_TRIALS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZERO_TRIALS.clear", {QM_YELLOW}));
                                              // /*spanish*/Según dicen, Sheik disipó la barrera alrededor de la #torre de Ganon#.

    hintTextTable[RHT_TRIAL_OFF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TRIAL_OFF.clear", {QM_YELLOW}));
                                            // /*spanish*/Según dicen, #[[1]]# se disipó gracias a Sheik.

    hintTextTable[RHT_TRIAL_ON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TRIAL_ON.clear", {QM_PINK}));
                                           // /*spanish*/Según dicen, #[[1]]# protege la torre de Ganon

    hintTextTable[RHT_LIGHT_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LIGHT_TRIAL.clear"));
                                              // /*spanish*/la prueba de la luz

    hintTextTable[RHT_FOREST_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TRIAL.clear"));
                                               // /*spanish*/la prueba del bosque

    hintTextTable[RHT_FIRE_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TRIAL.clear"));
                                             // /*spanish*/la prueba del fuego

    hintTextTable[RHT_WATER_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TRIAL.clear"));
                                              // /*spanish*/la prueba del agua

    hintTextTable[RHT_SPIRIT_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TRIAL.clear"));
                                               // /*spanish*/la prueba del espíritu

    hintTextTable[RHT_SHADOW_TRIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TRIAL.clear"));
                                               // /*spanish*/la prueba de las sombras

    /*--------------------------
    |        ALTAR TEXT        |
    ---------------------------*/

    hintTextTable[RHT_CHILD_ALTAR_STONES] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CHILD_ALTAR_STONES.clear",
                                                        {QM_GREEN, QM_RED, QM_BLUE}, {true, true, true}, TEXTBOX_TYPE_BLUE));
                                                    // /*spanish*/ Tres piedras espirituales halladas por Hyrule...$0#[[1]]#...^$1#[[2]]#...^$2#[[3]]#...^

    hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTOPEN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CHILD_ALTAR_TEXT_END_DOTOPEN.clear", TEXTBOX_TYPE_BLUE));
                                                               // /*spanish*/$oPara aquel que se convierta en el héroe...&La puerta al futuro está a su disposición...

    hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CHILD_ALTAR_TEXT_END_DOTSONGONLY.clear", TEXTBOX_TYPE_BLUE));
                                                                   // /*spanish*/$cPara aquel que se convierta en el héroe...&Tome la ocarina y&entone la Canción del Tiempo.

    hintTextTable[RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CHILD_ALTAR_TEXT_END_DOTCLOSED.clear", TEXTBOX_TYPE_BLUE));
                                                                 // /*spanish*/$iPara aquel que se convierta en el héroe...&Tome las piedras espirituales y&entone la Canción del Tiempo.

    hintTextTable[RHT_ADULT_ALTAR_MEDALLIONS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ADULT_ALTAR_MEDALLIONS.clear" ,
                                                                       { QM_RED, QM_YELLOW, QM_GREEN, QM_RED, QM_BLUE, QM_YELLOW, QM_PINK}, {true, true, true, true, true, true}, TEXTBOX_TYPE_BLUE));
                                                         // /*spanish*/Cuando el mal lo impregne todo, desde el Reino Sagrado surgirá una voz que hará despertar a los sabios que moran en los #cinco templos#.^
                                                                    // $8#[[1]]#...^$3#[[2]]#...^$4#[[3]]#...^$5#[[4]]#...^$6#[[5]]#...^$7#[[6]]#...^

    hintTextTable[RHT_ADULT_ALTAR_TEXT_END] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ADULT_ALTAR_TEXT_END.clear"));
                                                        // /*spanish*/Con el Héroe del Tiempo, aquellos&que despierten detendrán el mal y&volverán al mundo de luz la paz...

    /*--------------------------
    |     Static Item Hints     |
    ---------------------------*/

    hintTextTable[RHT_GANONDORF_HINT_LA_ONLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONDORF_HINT_LA_ONLY.clear",
                                                                       {QM_RED}));
                                                         // /*spanish*/Ja, ja, ja... Nunca me derrotarás reflejando mis esferas de energía y desplegando la flecha de luz de #[[1]]#!

    hintTextTable[RHT_GANONDORF_HINT_MS_ONLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONDORF_HINT_MS_ONLY.clear",
                                                                       {QM_RED}));

    hintTextTable[RHT_GANONDORF_HINT_LA_AND_MS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONDORF_HINT_LA_AND_MS.clear",
                                                                         {QM_RED, QM_RED}));
                                                           // /*spanish*/Ja, ja, ja... Nunca me derrotarás reflejando mis esferas de energía y desplegando la flecha de luz de #[[1]]#!
                                                                      // ^E incluso si lo haces, nunca encontrarás la espada legendaria escondida en #[[2]]#!

    hintTextTable[RHT_SHEIK_HINT_LA_ONLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHEIK_HINT_LA_ONLY.clear",
                                                                   {QM_YELLOW, QM_RED}));

    hintTextTable[RHT_BOSS_KEY_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOSS_KEY_HINT.clear"));

    hintTextTable[RHT_DAMPE_DIARY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DAMPE_DIARY.clear",
                                                            {QM_RED, QM_RED}));

    hintTextTable[RHT_GREG_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GREG_HINT.clear",
                                                          {QM_GREEN, QM_RED, QM_RED}));

    hintTextTable[RHT_SARIA_TALK_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SARIA_TALK_HINT.clear",
                                                                {QM_GREEN, QM_RED}));

    hintTextTable[RHT_SARIA_SONG_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SARIA_SONG_HINT.clear",
                                                                {QM_GREEN, QM_RED}, {}, TEXTBOX_TYPE_BLUE));

    hintTextTable[RHT_MIDO_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MIDO_HINT.clear",
                                                          {QM_GREEN, QM_RED}));

    hintTextTable[RHT_LOACH_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LOACH_HINT.clear",
                                                           {QM_RED}));

    hintTextTable[RHT_FISHING_POLE_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FISHING_POLE_HINT.clear",
                                                                   {QM_RED}));

    /*--------------------------
    |    Static Entrance Hint   |
    ---------------------------*/

    hintTextTable[RHT_WARP_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WARP_SONG.clear",
                                                          {QM_RED, QM_GREEN}));

   /*--------------------------
    |   STATIC LOCATION HINTS   |
    ---------------------------*/

    hintTextTable[RHT_HBA_HINT_SIGN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HBA_HINT_SIGN.clear",
                                                              {QM_RED, QM_GREEN, QM_GREEN, QM_GREEN}, {}, TEXTBOX_TYPE_WOODEN));

    hintTextTable[RHT_HBA_HINT_NOT_ON_HORSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HBA_HINT_NOT_ON_HORSE.clear",
                                                                      {QM_RED, QM_RED, QM_GREEN, QM_GREEN}));

    hintTextTable[RHT_HBA_HINT_INITIAL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HBA_HINT_INITIAL.clear",
                                                                 {QM_RED, QM_GREEN, QM_GREEN}));

    hintTextTable[RHT_HBA_HINT_HAVE_1000] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HBA_HINT_HAVE_1000.clear",
                                                                   {QM_RED, QM_GREEN}));

    hintTextTable[RHT_MALON_HINT_HOW_IS_EPONA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MALON_HINT_HOW_IS_EPONA.clear",
                                                                        {QM_RED, QM_BLUE, QM_GREEN}));

    hintTextTable[RHT_MALON_HINT_OBSTICLE_COURSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MALON_HINT_OBSTICLE_COURSE.clear",
                                                                           {QM_RED, QM_BLUE, QM_GREEN, QM_RED, QM_GREEN}));

    hintTextTable[RHT_MALON_HINT_TURNING_EVIL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MALON_HINT_TURNING_EVIL.clear",
                                                                        {QM_RED, QM_BLUE, QM_GREEN}));

    hintTextTable[RHT_MALON_HINT_INGO_TEMPTED] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MALON_HINT_INGO_TEMPTED.clear",
                                                                        {QM_RED, QM_BLUE, QM_GREEN}));

    hintTextTable[RHT_CHICKENS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CHICKENS_HINT.clear",
                                                              {QM_GREEN}));

    hintTextTable[RHT_BIG_POES_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BIG_POES_HINT.clear",
                                                              {QM_YELLOW, QM_GREEN}));

    hintTextTable[RHT_BIGGORON_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BIGGORON_HINT.clear",
                                                              {QM_GREEN, QM_RED}));

    hintTextTable[RHT_FROGS_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FROGS_HINT.clear",
                                                           {QM_GREEN}));

    hintTextTable[RHT_OOT_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OOT_HINT.clear",
                                                         {QM_BLUE, QM_GREEN, QM_GREEN}));

    hintTextTable[RHT_MASK_SHOP_HINT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MASK_SHOP_HINT.clear",
                                                               {QM_GREEN, QM_GREEN, QM_RED, QM_GREEN, QM_RED}));

    /*--------------------------
    |      GANON LINE TEXT     |
    ---------------------------*/

    hintTextTable[RHT_GANON_JOKE01] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE01.clear"));
                                               // /*spanish*/¡Oh! Pero si es @.&Estaba esperando a alguien llamado Sheik. ¿Sabes qué puede haberle pasado?

    hintTextTable[RHT_GANON_JOKE02] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE02.clear"));
                                               // /*spanish*/Sabía que no tendría que haber dejado la llave al otro lado de la puerta.

    hintTextTable[RHT_GANON_JOKE03] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE03.clear"));
                                               // /*spanish*/Parece que es hora de una pachanga de tenis.

    hintTextTable[RHT_GANON_JOKE04] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE04.clear"));
                                               // /*spanish*/Nunca reflejarás mis esferas de energía con tu espada, para después dispararme con las flechas de luz que tendrás.

    hintTextTable[RHT_GANON_JOKE05] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE05.clear"));
                                               // /*spanish*/Santa Hylia... ¿Por qué me habré dejado el tridente en el desierto?

    hintTextTable[RHT_GANON_JOKE06] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE06.clear"));
                                               // /*spanish*/Seguro que Zelda trata de hacer alguna tontería, como enviarte de vuelta a tu línea temporal.^No tiene
                                                         // ningún sentido alguno. ¿De verdad quieres salvar a esa tonta?

    hintTextTable[RHT_GANON_JOKE07] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE07.clear"));
                                               // /*spanish*/¿Qué te hace pensar que Zelda gobierna mejor que yo?^Yo he salvado el Rancho Lon Lon,&he alimentado a
                                                          // los hambrientos&y hasta hago que mi castillo flote.

    hintTextTable[RHT_GANON_JOKE08] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE08.clear"));
                                               // /*spanish*/Veamos ahora que harás,&la batalla ha de comenzar,&te enviaré de una vez al más allá,&¿listo para
                                                          // afrontar la verdad?

    hintTextTable[RHT_GANON_JOKE09] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE09.clear"));
                                               // /*spanish*/¿No osarás a mí enfrentarte?&Rimas aparte,&¡voy a matarte!

    hintTextTable[RHT_GANON_JOKE10] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE10.clear"));
                                               // /*spanish*/Después de lo que le hiciste a la Isla Koholint, ¿cómo te atreves a llamarme malvado?

    hintTextTable[RHT_GANON_JOKE11] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANON_JOKE11.clear"));
                                               // /*spanish*/Hoy daremos lugar a la línea temporal del Héroe Derrotado.&¡Prepárate para el culmen de esta saga!

   /*--------------------------
   |     Misc utilities       |
   ---------------------------*/

    hintTextTable[RHT_YOUR_POCKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_YOUR_POCKET.clear"));
                                              // /*spanish*/tu bolsillo

    hintTextTable[RHT_ISOLATED_PLACE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ISOLATED_PLACE.clear"));

    hintTextTable[RHT_DUNGEON_ORDINARY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DUNGEON_ORDINARY.clear"));

    hintTextTable[RHT_DUNGEON_MASTERFUL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DUNGEON_MASTERFUL.clear"));

    // clang-format on
}
} // namespace Rando
