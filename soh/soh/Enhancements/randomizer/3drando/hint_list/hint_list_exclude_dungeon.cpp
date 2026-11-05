#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {
void StaticData::HintTable_Init_Exclude_Dungeon() {
    // clang-format off

    // TODO move these to region specific sections when hint text stable
    hintTextTable[RHT_DODONGOS_CAVERN_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SHADOW_TEMPLE_SILVER]  = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTW_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTW_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ICE_CAVERN_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GTG_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GTG_SILVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_SILVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SILVER.clear", {QM_RED, QM_GREEN}));

    /*--------------------------
    |        DEKU TREE         |
    ---------------------------*/
    hintTextTable[RHT_DEKU_TREE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, al #centro del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_SLINGSHOT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_SLINGSHOT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_BASEMENT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_BASEMENT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, entre #telarañas del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, al #centro del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #gran araña del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_SLINGSHOT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_SLINGSHOT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_BASEMENT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_BASEMENT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, entre #telarañas del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, el #fuego mágico en el Gran Árbol Deku# conduce a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_QUEEN_GOHMA_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_QUEEN_GOHMA_HEART.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, la #Reina Goma# porta #[[1]]#.
                                                              {}, {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_QUEEN_GOHMA_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #arácnido parasitario acorazado# porta #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_BACK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_GS_BASEMENT_BACK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula en las profundidades del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_GATE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_GS_BASEMENT_GATE.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula protegida por su tela# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_VINES] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_GS_BASEMENT_VINES.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #Skulltula protegida por su tela# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_COMPASS_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_GS_COMPASS_ROOM.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #Skulltula en lo alto del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_GS_LOBBY.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula bajo una caja# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula protegida por una pared rocosa# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula en las profundidades del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_DEKU_SCRUB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_MQ_DEKU_SCRUB.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# vende #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DEKU_TREE_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_TREE_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_DEKU_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_DEKU_TREE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DEKU_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEKU_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_DEKU_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_DEKU_TREE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |     DODONGOS CAVERN      |
  ---------------------------*/
    hintTextTable[RHT_DODONGOS_CAVERN_BOSS_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_BOSS_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, #sobre el Rey Dodongo# yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, tras una #agrietada pared en la Cueva de los Dodongos# yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #estatua de la Cueva de los Dodongos# esconde #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, entre un #laberinto de piedra# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_BOMB_BAG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_BOMB_BAG_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #luchar dos veces contra reptiles en una cueva# conduce a #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #cofre al final de un quebrado puente# contiene #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #agrietada pared en la Cueva de los Dodongos# esconde #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #elevada alcoba# en la Cueva de los Dodongos brinda #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, unos #flamígeros reptiles# en la Cueva de los Dodongos esconden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, unas #pequeñas larvas# en la Cueva de los Dodongos esconden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, sobre un #laberinto de piedra# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, #bajo una lápida# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_KING_DODONGO_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_KING_DODONGO_HEART.clear", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/ Según dicen, el #Rey Dodongo# porta #[[1]]#.
                                                                     {}, {
                                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_KING_DODONGO_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                                      // /*spanish*/ Según dicen, el #dinosaurio infernal# porta #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula sobre unas cepas# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_SCARECROW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GS_SCARECROW.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula rodeada de explosivos gusanos# otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, una #Skulltula fuera del alcance# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_BACK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GS_BACK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula tras una estatua# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS.clear", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, una #Skulltula rodeada de murciélagos# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, una #Skulltula en lo alto de una pared# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, una #Skulltula en lo alto de un pilar# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #Skulltula bajo una caja# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_BACK_AREA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_GS_BACK_AREA.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula entre lápidas# en la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, un #deku custodiado por Lizalfos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #deku en la Cueva de los Dodongos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, un #deku en la Cueva de los Dodongos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS.clear", {QM_RED, QM_GREEN}));
                                                                                              // /*spanish*/ Según dicen, un #deku custodiado por Lizalfos# vende #[[1]]#.

    hintTextTable[RHT_POT_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_DODONGOS_CAVERN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DODONGOS_CAVERN_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DODONGOS_CAVERN_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_CAVERN_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_DODONGOS_CAVERN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DODONGOS_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DODONGOS_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_DODONGOS_CAVERN.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |     JABU JABUS BELLY     |
  ---------------------------*/
    hintTextTable[RHT_JABU_JABUS_BELLY_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP_CHEST.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, un #tentáculo parasitario# protege #[[1]]#.
                                                             {}, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MAP_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, un #problema tentaculoso# en la tripa de cierta deidad esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, unas #burbujas# protegen #[[1]]#.
                                                                 {}, {
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_COMPASS_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #problema burbujesco# en la tripa de cierta deidad esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #dispararle a una vaca# revela #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, cierta #roca rodeada de vacas# esconde #[[1]]#.
                                                                {}, {
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_MAP_CHEST.obscure.0", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, #cepillarse los dientes con explosivos# revela #[[1]]#.
                                                                CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_MAP_CHEST.obscure.1", {QM_RED, QM_GREEN})});
                                                                 // /*spanish*/ Según dicen, un #paladar explosivo# esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, cerca de un #ascensor puntiagudo# yace #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #vaca sumergida# revela #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, un #ser movedizo entre paredes# conduce a #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, un #par de digeridas vacas# otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, un #par de digeridas vacas# otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, las #vacas al final# de la tripa de cierta deidad otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, unas #vacas custodiadas por monstruos del techo# de la tripa de cierta deidad otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                                 // /*spanish*/ Según dicen, unos #stingers engullidos por Jabu-Jabu# guardan #[[1]]#.
                                                                                 {}, {
                                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                                  // /*spanish*/ Según dicen, unos #stingers engullidos por cierta deidad# guardan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, unos #stingers engullidos por Jabu-Jabu# guardan #[[1]]#.
                                                                      {}, {
                                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, unos #stingers engullidos por cierta deidad# guardan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_BARINADE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_BARINADE_HEART.clear", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, #Barinade# porta #[[1]]#.
                                                                  {}, {
                                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_BARINADE_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, la #anémona bioeléctrica# porta #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_NEAR_BOSS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_GS_NEAR_BOSS.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula rodeada de medusas# otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula rodeada por unos stingers# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula rodeada de electricidad# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula protegida por unos stingers# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, una #Skulltula sobre una red# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_DEKU_SCRUB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_DEKU_SCRUB.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #deku dentro de cierta deidad# vende #[[1]]#.

    hintTextTable[RHT_POT_JABU_JABUS_BELLY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_JABU_JABUS_BELLY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_JABU_JABU_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABU_RUPEE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_JABU_JABU_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABU_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_JABU_JABUS_BELLY_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_JABUS_BELLY_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_JABU_JABU] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_JABU_JABU.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_JABU_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_JABU_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_JABU_JABU] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_JABU_JABU.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |      FOREST TEMPLE       |
  ---------------------------*/
    hintTextTable[RHT_FOREST_TEMPLE_FIRST_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_FIRST_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, sobre un #árbol del Templo del Bosque# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FIRST_STALFOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_FIRST_STALFOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #derrotar enemigos caídos de lo alto# del Templo del Bosque revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_WELL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_WELL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, un #sumergido cofre en lo profundo del bosque# contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #ardiente calavera# del Templo del Bosque esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, un #cofre sobre una isla# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, tras un #techo de ajedrez# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_EYE_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_EYE_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, cerca de unos #bloques de piedra# del Templo del Bosque yace #[[1]]#.
                                                                 {}, {
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_EYE_SWITCH_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #afilado ojo# revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en una #sala con otro punto de vista# se esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FLOORMASTER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_FLOORMASTER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, en lo profundo del bosque #unas sombras# esconden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BOW_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOW_CHEST.clear", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, los #Stalfos en lo profundo del Templo del Bosque# guardan #[[1]]#.
                                                          {}, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BOW_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, un #ejército de soldados caídos# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_RED_POE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_RED_POE_CHEST.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, un #espectro rojo# guarda #[[1]]#.
                                                              {}, {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_RED_POE_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, #Joelle# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BLUE_POE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BLUE_POE_CHEST.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, un #espectro azul# guarda #[[1]]#.
                                                               {}, {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BLUE_POE_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, #Beth# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BASEMENT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_BASEMENT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, las #paredes giratorias# del Templo del Bosque conceden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, sobre un #árbol del Templo del Bosque# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_WOLFOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_WOLFOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, #derrotar enemigos caídos de lo alto# del Templo del Bosque revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BOW_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_BOW_CHEST.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, los #Stalfos en lo profundo del Templo del Bosque# guardan #[[1]]#.
                                                             {}, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_BOW_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, un #ejército de soldados caídos# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, un #cofre sobre una isla# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, un #cofre en lo alto de un patio# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_WELL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_WELL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #sumergido cofre en lo profundo del bosque# contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, un #fantasma rojo# guarda #[[1]]#.
                                                             {}, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_MAP_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, #Joelle# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, un #fantasma azul# guarda #[[1]]#.
                                                                 {}, {
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_COMPASS_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, #Beth# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, tras un #techo de ajedrez# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BASEMENT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_BASEMENT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, las #paredes giratorias# del Templo del Bosque conceden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_REDEAD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_REDEAD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, en lo profundo del bosque #guardias del más allá# guardan #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, en una #sala con otro punto de vista# se esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_PHANTOM_GANON_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_PHANTOM_GANON_HEART.clear", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/ Según dicen, #Ganon Fantasma# porta #[[1]]#.
                                                                    {}, {
                                                                    CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_PHANTOM_GANON_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/ Según dicen, el #espíritu maligno de ultratumba# porta #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula sobre una pequeña isla# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_FIRST_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_GS_FIRST_ROOM.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #Skulltula en lo alto de una pared de cepas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, unas #columnas del Templo del Bosque# conducen a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_GS_LOBBY.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula rodeada de fantasmas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_BASEMENT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_GS_BASEMENT.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #Skulltula entre paredes giratorias# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #Skulltula escondida entre cepas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, una #Skulltula en una esquina oculta# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula sobre un arco# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en un borde# del Templo del Bosque otorga #[[1]]#.
    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_WELL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_MQ_GS_WELL.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, #vaciar el pozo# del Templo del Bosque desvela una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_POT_FOREST_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_FOREST_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FOREST_TEMPLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FOREST_TEMPLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_FOREST_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_FOREST_TEMPLE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |       FIRE TEMPLE        |
  ---------------------------*/
    hintTextTable[RHT_FIRE_TEMPLE_NEAR_BOSS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_NEAR_BOSS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #cerca de un dragón# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_FLARE_DANCER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_FLARE_DANCER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el #Bailafuego tras unos tótems# esconde #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, en una #prisión tras unos tótems# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, los #explosivos en un mar de llamas# revelan #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, un #goron atrapado cerca de un mar de llamas# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un #goron al final de un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un #goron sobre un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, un #goron escondido tras un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, en un #camino bloqueado# del Templo del Fuego yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, un #cofre entre rejas# del Templo del Fuego contiene #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #cofre de un ardiente laberinto# contiene #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_HIGHEST_GORON_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_HIGHEST_GORON_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #goron en lo alto del Templo del Fuego# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #cerca de un dragón# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, el #Bailafuego en lo profundo del Templo del Fuego# esconde #[[1]]#.
                                                                      {}, {
                                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, el #Bailafuego en lo profundo del volcán# esconde #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en un #camino bloqueado# del Templo del Fuego yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #cajas de un laberinto# contienen #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #cajas de un laberinto# contienen #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #babosa del techo# del Templo del Fuego guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, usar el #martillo en lo profundo del Templo del Fuego# revela #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, #iluminar un mar de llamas# revela #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, los #explosivos en un mar de llamas# revelan #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, un #goron cerca de un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, #bajo unos bloques de piedra# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_VOLVAGIA_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_VOLVAGIA_HEART.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, #Volvagia# porta #[[1]]#.
                                                             {}, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_VOLVAGIA_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, el #dragón de lava subterráneo# porta #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, #ocho baldosas de maldad# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_BOSS_KEY_LOOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_BOSS_KEY_LOOP.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, #cinco baldosas de maldad# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_BOULDER_MAZE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_BOULDER_MAZE.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, los #explosivos en un laberinto# desvelan una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SCARECROW_TOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_SCARECROW_TOP.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga #[[1]]#.
                                                               {}, {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_SCARECROW_TOP.obscure.0", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SCARECROW_CLIMB] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_SCARECROW_CLIMB.clear", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga #[[1]]#.
                                                                 {}, {
                                                                 CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_GS_SCARECROW_CLIMB.obscure.0", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, una #Skulltula sobre un ardiente laberinto# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula en el interior de un ardiente laberinto# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #Skulltula amiga de un Goron atrapado junto a la lava# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula junto a un ardiente laberinto# otorga #[[1]]#.

    hintTextTable[RHT_POT_FIRE_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_FIRE_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LOOP_STALFOS_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LOOP_STALFOS_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LOOP_KNUCKLE_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LOOP_KNUCKLE_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_NEAR_BOSS_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_NEAR_BOSS_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BOSS_KEY_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOSS_KEY_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_HIGHEST_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_HIGHEST_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MAP_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_MAP_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_OPEN_DOOR_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_GORON.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_FIRE_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_FIRE_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_FIRE_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_FIRE_TEMPLE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |       WATER TEMPLE       |
  ---------------------------*/
    hintTextTable[RHT_WATER_TEMPLE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, unas #rodantes púas# del Templo del Agua guardan #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, unos #errantes stingers# del Templo del Agua guardan #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_TORCHES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_TORCHES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, el #fuego en el Templo del Agua# revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_DRAGON_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_DRAGON_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el #escamado premio# del Templo del Agua se trata de #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, #cegar un ojo# del Templo del Agua conduce a #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CENTRAL_PILLAR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_CENTRAL_PILLAR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, en las #profundidades del Templo del Agua# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CRACKED_WALL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_CRACKED_WALL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, tras una #agrietada pared# del Templo del Agua yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_LONGSHOT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_LONGSHOT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, #@ Oscuro# guarda #[[1]]#.
                                                              {}, {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_LONGSHOT_CHEST.obscure.0", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #luchar contra ti mismo# revela #[[1]]#.
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_LONGSHOT_CHEST.obscure.1", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #oscuro reflejo de ti mismo# guarda #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, en las #profundidades del Templo del Agua# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el fuego en el Templo del Agua alza una #gran valla# con #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_LONGSHOT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_LONGSHOT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, tras una #agrietada pared# del Templo del Agua yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, el #fuego en el Templo del Agua# revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, #acabar con unos soldados# del Templo del Agua revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MORPHA_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MORPHA_HEART.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, #Morpha# porta #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MORPHA_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, la #ameba acuática gigante# porta #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula tras una cascada# del Templo del Agua otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_CENTRAL_PILLAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_CENTRAL_PILLAR.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula en el centro del Templo del Agua# otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, una #Skulltula protegida por rocas rodantes# del Templo del Agua otorga #[[1]]#.
                                                                      {}, {
                                                                      CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, una #Skulltula protegida por rocas rodantes# bajo el lago otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_GS_RIVER.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula sobre un río# del Templo del Agua otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, #más allá de un pozo de reptiles# una Skulltula otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, unos #reptiles custodian una Skulltula# del Templo del Agua que otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_GS_RIVER.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #Skulltula sobre un río# del Templo del Agua otorga #[[1]]#.

    hintTextTable[RHT_POT_WATER_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_WATER_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WATER_TEMPLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WATER_TEMPLE_MQ_DARK_LINK_PILAR_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_DARK_LINK_PILAR_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WATER_TEMPLE_MQ_DARK_LINK_LEFT_STORM_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_DARK_LINK_LEFT_STORM_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WATER_TEMPLE_MQ_DARK_LINK_RIGHT_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WATER_TEMPLE_MQ_DARK_LINK_RIGHT_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_WATER_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_WATER_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_WATER_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_WATER_TEMPLE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |      SPIRIT TEMPLE       |
  ---------------------------*/
    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, el joven que #baje el puente# del Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, un joven puede encontrar un #cofre entre rejas# del Templo del Espíritu con #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, tras un #pozo de arena# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, #esquivar rocas y conseguir plateadas rupias# en el Templo del Espíritu conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un #círculo de reflectante luz# del Templo del Espíritu guarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, un #círculo de reflectante luz# del Templo del Espíritu guarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, #ante una gran estatua# del Templo del Espíritu aguarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, los #reptiles del Templo del Espíritu# guardan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #reptiles del Templo del Espíritu# guardan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, las #antorchas junto a Beamos# del Templo del Espíritu revelan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #estatua del Templo del Espíritu# esconde #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, al #borde de una estatua# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, aquellos que #iluminen ante las estatuas# del Templo del Espíritu encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #cofre rodeado de llamas# del Templo del Espíritu contiene #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_TOPMOST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_TOPMOST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, aquellos que #iluminen en lo alto del Coloso# encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, en la #entrada del Templo del Espíritu# yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, el #interruptor de un pilar# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, #hacerte con rupias tras un géiser# revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #cegar a un ojo# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, rodeado de #fuego y vendas# yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, el joven que derrote #unos monstruos# del Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #explosiva luz solar# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #rodeado de enemigos del cielo# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #cegar al coloso# revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #melodía real que despierte al coloso# revelará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #adulto esconde lo que el joven anhela#, revelando #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, la #luz solar de un ígneo laberinto# esconde #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #a través del pozo de arena# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, donde los #bloques temporales bloquean# en el Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #cofre de doble uso# contiene #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #bloque temporal bloquea la luz# que conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, aquellos que #revelen la luz sobre el Coloso# encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_TWINROVA_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_TWINROVA_HEART.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #Birova# porta #[[1]]#.
                                                               {}, {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_TWINROVA_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, las #hermanas hechiceras# portan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_BOULDER_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_GS_BOULDER_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula tras un bloque temporal# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_LOBBY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_GS_LOBBY.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula junto a una estatua# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula en lo alto de un gran hueco# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_METAL_FENCE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_GS_METAL_FENCE.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el joven que derrote la #Skulltula entre murciélagos# del Templo del Espíritu hallará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula sobre un pozo de arena# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #sobre una plataforma de cristal# yace una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_POT_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_SPIRIT_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_MQ_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_BOULDER_ROOM_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOULDER_ROOM_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_ARMOS_ROOM_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_ARMOS_ROOM_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_SPIRIT_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SPIRIT_TEMPLE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SPIRIT_TEMPLE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_SPIRIT_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_SPIRIT_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_SPIRIT_TEMPLE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |      SHADOW TEMPLE       |
  ---------------------------*/
    hintTextTable[RHT_SHADOW_TEMPLE_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, el #Ojo de la Verdad# descubrirá un pasillo de facetas con #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_HOVER_BOOTS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_HOVER_BOOTS_CHEST.clear", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, la #Mano Muerta del Templo de las Sombras# guarda #[[1]]#.
                                                                  {}, {
                                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_HOVER_BOOTS_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, un #temido morador del Templo de las Sombras# guarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, las #momias reveladas por el Ojo de la Verdad# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, las #giratorias guadañas# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, las #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, las #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #muertos que vagan por pinchos invisibles# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_WIND_HINT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_WIND_HINT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, un #cofre invisible custodiado por los del más allá# contiene #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, en el #interior de una calavera en llamas# aguarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el #Ojo de la Verdad# descubre un pasillo de facetas con #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, la #Mano Muerta del Templo de las Sombras# guarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, las #momias reveladas por el Ojo de la Verdad# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, las #giratorias guadañas# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, hacerte con las #rupias en una gran caverna# del Templo de las Sombras revela #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, los #muertos que vagan por pinchos invisibles# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, cerca de un #vacío pedestal# del Templo de las Sombras yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, unas #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, unas #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #cofre invisible custodiado por los del más allá# contiene #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, #entre rejas al lado de un navío# yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, tras #tres ardientes calaveras# yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_BONGO_BONGO_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BONGO_BONGO_HEART.clear", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, #Bongo Bongo# porta #[[1]]#.
                                                                  {}, {
                                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BONGO_BONGO_HEART.obscure.0", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, la #alimaña oscura espectral# porta #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #tras una ardiente calavera# yace una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula tras los pinchos del techo# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #tras tres ardientes calaveras# yace una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula custodiada por hojas invisibles# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_NEAR_SHIP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_GS_NEAR_SHIP.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula cercana a un navío# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula tras los pinchos del techo# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula entre ventarrones# del Templo de las Sombras otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_AFTER_WIND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_GS_AFTER_WIND.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula bajo unos horripilantes escombros# del Templo de las Sombras otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #estatua caída# revelará una Skulltula que otorgue #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula flotante# del Templo de las Sombras otorga #[[1]]#.

    hintTextTable[RHT_POT_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_SHADOW_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SHADOW_TEMPLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SHADOW_TEMPLE_BEAMOS_STORM_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_BEAMOS_STORM_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SHADOW_TEMPLE_PIT_STORM_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_PIT_STORM_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SHADOW_TEMPLE_WIND_HINT_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SHADOW_TEMPLE_WIND_HINT_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_SHADOW_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_SHADOW_TEMPLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_SHADOW_TEMPLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_SHADOW_TEMPLE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |    BOTTOM OF THE WELL    |
  ---------------------------*/
    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, el #Ojo de la Verdad en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, unos #horripilantes escombros# del pozo esconden #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el #Ojo de la Verdad en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, la #entrada oculta de una celda# del pozo conduce a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #araña protegiendo una celda# del pozo guarda #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, unos #horripilantes escombros# del pozo esconden #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, el #secreto invisible de la Mano Muerta# esconde #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en las #profundidades del pozo# yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, #peligrosos fosos# del pozo conducen a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #entre rejas# en el pozo yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, en el #interior de un ataúd# yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, un #ejército del más allá# del pozo guarda #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, el #explosivo secreto de la Mano Muerta# esconde #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                                         // /*spanish*/ Según dicen, un #camino invisible del pozo# conduce a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula enjaulada# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un #camino invisible del pozo# conduce a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula encerrada en una cripta# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, unas #arañas invisibles# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula junto a los muertos# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula encerrada en una cripta# del pozo otorga #[[1]]#.

    hintTextTable[RHT_POT_BOTTOM_OF_THE_WELL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_BOTTOM_OF_THE_WELL.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_RUPEE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_BASEMENT_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_BASEMENT_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_CELL_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_CELL_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTW_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTW_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_WONDER_ITEM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BOTTOM_OF_THE_WELL_WONDER_ITEM.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |        ICE CAVERN        |
  ---------------------------*/
    hintTextTable[RHT_ICE_CAVERN_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_IRON_BOOTS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_IRON_BOOTS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #monstruo de una helada caverna# guarda #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_IRON_BOOTS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_IRON_BOOTS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, un #monstruo de una helada caverna# guarda #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_COMPASS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_COMPASS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_MAP_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_MAP_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_PUSH_BLOCK_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_GS_PUSH_BLOCK_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula sobre gélidos vacíos# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, unos #témpanos giratorios# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_HEART_PIECE_ROOM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_GS_HEART_PIECE_ROOM.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula tras una gélida pared# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_SCARECROW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_GS_SCARECROW.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula sobre gélidos vacíos# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_ICE_BLOCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_GS_ICE_BLOCK.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #gélida red# rodea a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_RED_ICE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_MQ_GS_RED_ICE.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #Skulltula tras un ardiente hielo# otorga #[[1]]#.

    hintTextTable[RHT_POT_ICE_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_ICE_CAVERN.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_ICE_CAVERN_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ICE_CAVERN_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_RUPEE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ICE_CAVERN_ENTRANCE_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_ENTRANCE_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #Skulltula tras un ardiente hielo# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_ICICLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_ICICLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ICE_CAVERN_RED_ICE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ICE_CAVERN_RED_ICE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  | Gerudo Training Ground  |
  ---------------------------*/
    hintTextTable[RHT_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, #cegar un ojo en el Centro de Instrucción Gerudo# revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #cegar un ojo en el Centro de Instrucción Gerudo# revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_STALFOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_STALFOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, #soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_BEAMOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_BEAMOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, #unos escamosos guerreros# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el primer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, el segundo premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el tercer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, donde entrenan las bandidas #entre llamas# yace #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, #ante un plateado bloque# las bandidas hallan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_ENTRANCE_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_ENTRANCE_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, las #bandidas se instruyen# con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, las #bandidas se instruyen# con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #ante un plateado bloque# las bandidas hallan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, donde entrenan las bandidas #entre llamas# yace #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                      // /*spanish*/ Según dicen, unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #unos escamosos guerreros# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, un #camino de fuego# conduce a las bandidas a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el primer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, un #camino de fuego# conduce a las bandidas a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el tercer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, el segundo premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUNDS_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUNDS_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GERUDO_TRAINING_GROUND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_GERUDO_TRAINING_GROUND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_GERUDO_TRAINING_GROUND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_GERUDO_TRAINING_GROUND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_ICICLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_ICICLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_RED_ICE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_TRAINING_GROUND_RED_ICE.clear", {QM_RED, QM_GREEN}));

  /*--------------------------
  |      GANONS CASTLE       |
  ---------------------------*/
    hintTextTable[RHT_GANONS_TOWER_BOSS_KEY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_TOWER_BOSS_KEY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, el #Rey del Mal# acapara #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_FOREST_TRIAL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_FOREST_TRIAL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, la #música en la prueba de la oscuridad# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, la #luz en la prueba de la oscuridad# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #prueba de las arenas# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba de las arenas# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, la #música en la prueba del resplandor# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SPIRIT_TRIAL_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SPIRIT_TRIAL_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_SCRUBS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_SCRUBS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                        // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #música en la prueba del resplandor# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #prueba de la oscuridad# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la oscuridad# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY.clear", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_DEKU_SCRUB_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_DEKU_SCRUB_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_POT_GANONS_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_GANONS_CASTLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_HEART.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_WONDER_ITEM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_WONDER_ITEM.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_ICICLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_ICICLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GANONS_CASTLE_RED_ICE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GANONS_CASTLE_RED_ICE.clear", {QM_RED, QM_GREEN}));
    // clang-format on
}
} // namespace Rando
