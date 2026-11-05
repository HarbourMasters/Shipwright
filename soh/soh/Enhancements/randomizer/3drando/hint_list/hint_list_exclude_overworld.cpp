#include "soh/Enhancements/custom-message/MessageCatalog.h"
#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {
void StaticData::HintTable_Init_Exclude_Overworld() {
    // clang-format off

    hintTextTable[RHT_KF_KOKIRI_SWORD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_KOKIRI_SWORD_CHEST.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #tesoro oculto de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_TOP_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_TOP_LEFT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                          {}, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_TOP_LEFT_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_TOP_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_TOP_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                           {}, {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_TOP_RIGHT_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                            // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_BOTTOM_LEFT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_BOTTOM_LEFT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                             {}, {
                                                             CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_BOTTOM_LEFT_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_BOTTOM_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_BOTTOM_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                              {}, {
                                                              CustomMessages::CreateMessage("randomizer.hints.RHT_KF_MIDOS_BOTTOM_RIGHT_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_SHIELD_GRAVE_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_SHIELD_GRAVE_CHEST.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el #tesoro de un soldado caído# esconde #[[1]]#.

    hintTextTable[RHT_DMT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_CHEST.clear", {QM_RED, QM_GREEN}));
                                            // /*spanish*/ Según dicen, tras una pared del #sendero de la montaña# yace #[[1]]#.

    hintTextTable[RHT_GC_MAZE_RIGHT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MAZE_RIGHT_CHEST.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, en la #Ciudad Goron# unos explosivos desbloquean #[[1]]#.

    hintTextTable[RHT_GC_MAZE_CENTER_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MAZE_CENTER_CHEST.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en la #Ciudad Goron# unos explosivos desbloquean #[[1]]#.

    hintTextTable[RHT_ZD_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_CHEST.clear", {QM_RED, QM_GREEN}));
                                           // /*spanish*/ Según dicen, las #llamas tras una una cascada# revelan #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_HOOKSHOT_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_HOOKSHOT_CHEST.clear", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, el primer premio de #la carrera de Dampé# se trata de #[[1]]#.
                                                           {}, {
                                                           CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_HOOKSHOT_CHEST.obscure.0", {QM_RED, QM_GREEN})});
                                                            // /*spanish*/ Según dicen, un cofre custodiado por un #espectro veloz# contiene #[[1]]#.

    hintTextTable[RHT_GF_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_CHEST.clear", {QM_RED, QM_GREEN}));
                                           // /*spanish*/ Según dicen, en una #azotea del desierto# yace #[[1]]#.

    hintTextTable[RHT_KAK_REDEAD_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_REDEAD_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, unos #zombis subterráneos# esconden #[[1]]#.

    hintTextTable[RHT_SFM_WOLFOS_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_WOLFOS_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, unos #lobos subterráneos# esconden #[[1]]#.

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_MARKET_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, bajo el #hoyo de una llanura cercano a un puente# yace #[[1]]#.

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_MARKET_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, bajo el #hoyo de una llanura cercano a un puente# yace #[[1]]#.

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SOUTHEAST_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, bajo el #hoyo de una llanura rodeado de árboles# yace #[[1]]#.

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SOUTHEAST_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, bajo el #hoyo de una llanura rodeado de árboles# yace #[[1]]#.

    hintTextTable[RHT_HF_OPEN_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_OPEN_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo el #hoyo descubierto de una llanura# yace #[[1]]#.

    hintTextTable[RHT_HF_OPEN_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_OPEN_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, bajo el #hoyo descubierto de una llanura# yace #[[1]]#.

    hintTextTable[RHT_KAK_OPEN_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_OPEN_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo descubierto de un pueblo# yace #[[1]]#.

    hintTextTable[RHT_KAK_OPEN_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_OPEN_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo el #hoyo descubierto de un pueblo# yace #[[1]]#.

    hintTextTable[RHT_ZR_OPEN_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_OPEN_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo un #hoyo junto a un río# yace #[[1]]#.

    hintTextTable[RHT_ZR_OPEN_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_OPEN_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, bajo un #hoyo junto a un río# yace #[[1]]#.

    hintTextTable[RHT_KF_STORMS_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_STORMS_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo el #hoyo de una tribu del bosque# yace #[[1]]#.

    hintTextTable[RHT_KF_STORMS_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_STORMS_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo de una tribu del bosque# yace #[[1]]#.

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_NEAR_SHORTCUTS_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, bajo un #hoyo de un laberinto forestal# yace #[[1]]#.

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_NEAR_SHORTCUTS_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, bajo un #hoyo de un laberinto forestal# yace #[[1]]#.

    hintTextTable[RHT_DMT_STORMS_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_STORMS_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, bajo un #hoyo de una montaña inundado de lluvia# yace #[[1]]#.

    hintTextTable[RHT_DMT_STORMS_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_STORMS_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo un #hoyo de una montaña inundado de lluvia# yace #[[1]]#.

    hintTextTable[RHT_DMC_UPPER_GROTTO_CHEST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_UPPER_GROTTO_CHEST.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo el #hoyo de un volcán# yace #[[1]]#.

    hintTextTable[RHT_DMC_UPPER_GROTTO_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_UPPER_GROTTO_FISH.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo de un volcán# yace #[[1]]#.

    hintTextTable[RHT_TOT_MASTER_SWORD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TOT_MASTER_SWORD.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, un #pedestal en un templo# sostiene #[[1]]#.

    hintTextTable[RHT_TOT_LIGHT_ARROWS_CUTSCENE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TOT_LIGHT_ARROWS_CUTSCENE.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el #obsequio final de la princesa# se trata de #[[1]]#.

    hintTextTable[RHT_LW_GIFT_FROM_SARIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GIFT_FROM_SARIA.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, el #regalo de Saria# se trata de #[[1]]#.
                                                     {}, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GIFT_FROM_SARIA.obscure.0", {QM_RED, QM_GREEN}),
                                                      // /*spanish*/ Según dicen, cierta #jovencita verde# concede #[[1]]#.
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GIFT_FROM_SARIA.obscure.1", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, una #gran amiga# concede #[[1]]#.

    hintTextTable[RHT_ZF_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #hada del viento# brinda #[[1]]#.

    hintTextTable[RHT_HC_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #hada del fuego# brinda #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, el #hada del amor# brinda #[[1]]#.

    hintTextTable[RHT_DMT_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, un #hada mágica# brinda #[[1]]#.

    hintTextTable[RHT_DMC_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, un #hada mágica# brinda #[[1]]#.

    hintTextTable[RHT_OGC_GREAT_FAIRY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OGC_GREAT_FAIRY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, el #hada de la fuerza# brinda #[[1]]#.

    hintTextTable[RHT_SONG_FROM_IMPA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_IMPA.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, en el #jardín del castillo Impa enseña# #[[1]]#.

    hintTextTable[RHT_SONG_FROM_MALON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_MALON.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #chica rupestre# canta #[[1]]#.

    hintTextTable[RHT_SONG_FROM_SARIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_SARIA.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, al #fondo del bosque# Saria enseña #[[1]]#.

    hintTextTable[RHT_SONG_FROM_WINDMILL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SONG_FROM_WINDMILL.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, el #hombre del molino# está obsesionado con #[[1]]#.

    hintTextTable[RHT_HC_MALON_EGG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_MALON_EGG.clear", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, una #chica en busca de su padre# otorga #[[1]]#.

    hintTextTable[RHT_HC_ZELDAS_LETTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_ZELDAS_LETTER.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la #princesa de un castillo# otorga #[[1]]#.

    hintTextTable[RHT_ZD_DIVING_MINIGAME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_DIVING_MINIGAME.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, aquellos que se #sumergan por las rupias zora# encontrarán #[[1]]#.
                                                     {}, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_DIVING_MINIGAME.obscure.0", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, un #mal modelo de negocio# premia con #[[1]]#.

    hintTextTable[RHT_LH_CHILD_FISHING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_CHILD_FISHING.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, #pescar en la juventud# conduce a #[[1]]#.

    hintTextTable[RHT_LH_POND_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_POND_FISH.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_HYRULE_LOACH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_HYRULE_LOACH.clear", {QM_RED, QM_GREEN}),
                                                  // /*spanish*/ Según dicen, si #pescas a la Locha de Hyrule# encontrarás #[[1]]#.
                                                  {}, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_LH_HYRULE_LOACH.obscure.0", {QM_RED, QM_GREEN})});
                                                   // /*spanish*/ Según dicen, #pescar a la leyenda# conduce a #[[1]]#.

    hintTextTable[RHT_LH_ADULT_FISHING] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_ADULT_FISHING.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, #pescar en la madurez# conduce a #[[1]]#.

    hintTextTable[RHT_LH_LAB_DIVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_LAB_DIVE.clear", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, #bucear para un experimento# se premia con #[[1]]#.

    hintTextTable[RHT_ZD_FISH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_FISH.clear", {QM_RED, QM_GREEN}));
                                          // /*spanish*/ Según dicen, un #pez junto a una cascada# otorga #[[1]]#.


    hintTextTable[RHT_GC_ROLLING_GORON_AS_ADULT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_ROLLING_GORON_AS_ADULT.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, #calmar a un joven Goron# otorga #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_GC_ROLLING_GORON_AS_ADULT.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, #confrontarte a ti mismo# otorga #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, el #primer premio explosivo# se trata de #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE.clear", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, el #segundo premio explosivo# se trata de #[[1]]#.

    hintTextTable[RHT_MARKET_LOST_DOG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_LOST_DOG.clear", {QM_RED, QM_GREEN}),
                                                  // /*spanish*/ Según dicen, #rescatar al perrito Ricardo# conduce a #[[1]]#.
                                                  {}, {
                                                  CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_LOST_DOG.obscure.0", {QM_RED, QM_GREEN})});
                                                   // /*spanish*/ Según dicen, los #amantes caninos# encontrarán #[[1]]#.

    hintTextTable[RHT_LW_OCARINA_MEMORY_GAME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_OCARINA_MEMORY_GAME.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, #tocar la ocarina en el Bosque Perdido# otorga #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_LW_OCARINA_MEMORY_GAME.obscure.0", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, #repetir ciertas melodías# otorga #[[1]]#.
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_LW_OCARINA_MEMORY_GAME.obscure.1", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, #tocar junto a otros# otorga #[[1]]#.

    hintTextTable[RHT_KAK_10_GOLD_SKULLTULA_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_10_GOLD_SKULLTULA_REWARD.clear", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #exterminar 10 skulltulas doradas# revela #[[1]]#.
                                                               {}, {
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_10_GOLD_SKULLTULA_REWARD.obscure.0", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, #10 medallas de insectos# otorgan #[[1]]#.
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_10_GOLD_SKULLTULA_REWARD.obscure.1", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, #10 almas de araña# otorgan #[[1]]#.
                                                               CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_10_GOLD_SKULLTULA_REWARD.obscure.2", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, #10 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_MAN_ON_ROOF] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_MAN_ON_ROOF.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, #alguien sobre un tejado# otorga #[[1]]#.

    hintTextTable[RHT_ZR_MAGIC_BEAN_SALESMAN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_MAGIC_BEAN_SALESMAN.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, el #vendedor de judías# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_MAGIC_BEAN_SALESMAN.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, el vendedor de un #colorido cultivo# ofrece #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_IN_THE_RAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_IN_THE_RAIN.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, las #ancas bajo la tormenta# otorgan #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_ZELDAS_LULLABY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_ZELDAS_LULLABY.clear", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, después de escuchar #la Nana de Zelda, las ranas# regalan #[[1]]#.
                                                          {}, {
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_ZELDAS_LULLABY.obscure.0", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, las #ranas somnolientas# regalan #[[1]]#.
                                                          CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_ZELDAS_LULLABY.obscure.1", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, el #Sapo Tenore al fondo, a la izquierda#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_EPONAS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_EPONAS_SONG.clear", {QM_RED, QM_GREEN}),
                                                       // /*spanish*/ Según dicen, después de escuchar #la Canción de Epona, las ranas# regalan #[[1]]#.
                                                       {}, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_EPONAS_SONG.obscure.0", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, las #ranas equinas# regalan #[[1]]#.
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_EPONAS_SONG.obscure.1", {QM_RED, QM_GREEN})});
                                                        // /*spanish*/ Según dicen, el #Sapo Tenore al fondo, a la derecha#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SARIAS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SARIAS_SONG.clear", {QM_RED, QM_GREEN}),
                                                       // /*spanish*/ Según dicen, después de escuchar #la Canción de Saria, las ranas# regalan #[[1]]#.
                                                       {}, {
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SARIAS_SONG.obscure.0", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, las #ranas silvestres# regalan #[[1]]#.
                                                       CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SARIAS_SONG.obscure.1", {QM_RED, QM_GREEN})});
                                                        // /*spanish*/ Según dicen, el #Sapo Tenore en el centro# regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SUNS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SUNS_SONG.clear", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, después de escuchar #la Canción del Sol, las ranas# regalan #[[1]]#.
                                                     {}, {
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SUNS_SONG.obscure.0", {QM_RED, QM_GREEN}),
                                                      // /*spanish*/ Según dicen, las #ranas alumbradas# regalan #[[1]]#.
                                                     CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SUNS_SONG.obscure.1", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, el #Sapo Tenore al frente, a la izquierda#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SONG_OF_TIME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SONG_OF_TIME.clear", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, después de escuchar #la Canción del tiempo, las ranas# regalan #[[1]]#.
                                                        {}, {
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SONG_OF_TIME.obscure.0", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, las #ranas viajeras del tiempo# regalan #[[1]]#.
                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FROGS_SONG_OF_TIME.obscure.1", {QM_RED, QM_GREEN})});
                                                         // /*spanish*/ Según dicen, el #Sapo Tenore al frente, a la derecha#, regala #[[1]]#.

    hintTextTable[RHT_GF_HBA_1000_POINTS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_HBA_1000_POINTS.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, conseguir 1000 puntos en el #tiro con arco a caballo# premia #[[1]]#.

    hintTextTable[RHT_MARKET_SHOOTING_GALLERY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_SHOOTING_GALLERY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #disparar en la juventud# otorga #[[1]]#.

    hintTextTable[RHT_KAK_SHOOTING_GALLERY_REWARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_SHOOTING_GALLERY_REWARD.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #disparar en la madurez# otorga #[[1]]#.

    hintTextTable[RHT_LW_TARGET_IN_WOODS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_TARGET_IN_WOODS.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, disparar a un #blanco forestal# brinda #[[1]]#.

    hintTextTable[RHT_KAK_ANJU_AS_ADULT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ANJU_AS_ADULT.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #cuidadora de emplumados# le ofrece a los mayores #[[1]]#.

    hintTextTable[RHT_LLR_TALONS_CHICKENS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_TALONS_CHICKENS.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, #hallar los supercucos# conduce a #[[1]]#.

    hintTextTable[RHT_GC_ROLLING_GORON_AS_CHILD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_ROLLING_GORON_AS_CHILD.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #gran Goron rodante# otorga #[[1]]#.

    hintTextTable[RHT_LH_UNDERWATER_ITEM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_UNDERWATER_ITEM.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, el #tesoro hundido del lago# se trata de #[[1]]#.

    hintTextTable[RHT_LH_SCARECROWS_SONG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SCARECROWS_SONG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TH_FREED_CARPENTERS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_FREED_CARPENTERS.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, #rescatar los apresados carpinteros# se premia con #[[1]]#.

    hintTextTable[RHT_WASTELAND_BOMBCHU_SALESMAN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WASTELAND_BOMBCHU_SALESMAN.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, el #genio de una alfombra# vende #[[1]]#.

    hintTextTable[RHT_GC_MEDIGORON] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MEDIGORON.clear", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, #Medigoron# vende #[[1]]#.

    hintTextTable[RHT_KAK_GRANNYS_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GRANNYS_SHOP.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la #señora de la tienda de pociones# vende #[[1]]#.

    hintTextTable[RHT_KAK_IMPAS_HOUSE_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_IMPAS_HOUSE_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, #en una casa entre rejas# yace #[[1]]#.

    hintTextTable[RHT_HF_TEKTITE_GROTTO_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_TEKTITE_GROTTO_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #en lo hondo bajo un hoyo# yace #[[1]]#.

    hintTextTable[RHT_KAK_WINDMILL_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_WINDMILL_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, al #borde de un molino# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}),
                                                                        // /*spanish*/ Según dicen, el segundo premio de #la carrera de Dampé# se trata de #[[1]]#.
                                                                        {}, {
                                                                        CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH.obscure.0", {QM_RED, QM_GREEN})});
                                                                         // /*spanish*/ Según dicen, #perseguir a un fantasma# conduce a #[[1]]#.

    hintTextTable[RHT_LLR_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en un #granero rupestre# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, bajo la #caja de un cementerio# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, cierto #sepultero desentierra# #[[1]]#.

    hintTextTable[RHT_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, en lo alto del #pilar de un río# yace #[[1]]#.

    hintTextTable[RHT_ZR_NEAR_DOMAIN_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_DOMAIN_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, al borde de #la entrada a una cascada# yace #[[1]]#.

    hintTextTable[RHT_LH_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, en lo #alto de un laboratorio# yace #[[1]]#.

    hintTextTable[RHT_ZF_ICEBERG_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_ICEBERG_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #flotando sobre hielo# yace #[[1]]#.

    hintTextTable[RHT_GV_WATERFALL_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_WATERFALL_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, tras una #desierta cascada# yace #[[1]]#.

    hintTextTable[RHT_GV_CRATE_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_CRATE_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, bajo la #caja de un valle# yace #[[1]]#.

    hintTextTable[RHT_COLOSSUS_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, en lo alto de un #arco de piedra# yace #[[1]]#.

    hintTextTable[RHT_DMT_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en lo alto de la #entrada de una cueva en la montaña# yace #[[1]]#.

    hintTextTable[RHT_DMC_WALL_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_WALL_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, entre unas #murallas volcánicas# yace #[[1]]#.

    hintTextTable[RHT_DMC_VOLCANO_FREESTANDING_POH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_VOLCANO_FREESTANDING_POH.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, bajo la #ceniza volcánica# yace #[[1]]#.

    hintTextTable[RHT_TH_1_TORCH_CARPENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_1_TORCH_CARPENTER.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                        // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_DEAD_END_CARPENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_DEAD_END_CARPENTER.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                        // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_DOUBLE_CELL_CARPENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_DOUBLE_CELL_CARPENTER.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                           // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_STEEP_SLOPE_CARPENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_STEEP_SLOPE_CARPENTER.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                           // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_HF_GS_NEAR_KAK_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_GS_NEAR_KAK_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula custodiada por otra# de un hoyo otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_BACK_WALL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_GS_BACK_WALL.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_RAIN_SHED] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_GS_RAIN_SHED.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_HOUSE_WINDOW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_GS_HOUSE_WINDOW.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de un rancho# otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada en un bosque# otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_KNOW_IT_ALL_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GS_KNOW_IT_ALL_HOUSE.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, la noche revela en el pasado una #Skulltula del bosque# que otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_HOUSE_OF_TWINS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GS_HOUSE_OF_TWINS.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, la noche revela en el futuro una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_BEAN_PATCH_NEAR_BRIDGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GS_BEAN_PATCH_NEAR_BRIDGE.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula enterrada en un laberinto forestal# otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_BEAN_PATCH_NEAR_THEATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GS_BEAN_PATCH_NEAR_THEATER.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula enterrada en un laberinto forestal# otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_ABOVE_THEATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GS_ABOVE_THEATER.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula del laberinto forestal# que otorga #[[1]]#.

    hintTextTable[RHT_SFM_GS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_GS.clear", {QM_RED, QM_GREEN}));
                                         // /*spanish*/ Según dicen, la noche revela una #Skulltula de la pradera del bosque# que otorga #[[1]]#.

    hintTextTable[RHT_OGC_GS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OGC_GS.clear", {QM_RED, QM_GREEN}));
                                         // /*spanish*/ Según dicen, una #Skulltula a las afueras de la torre de un tirano# otorga #[[1]]#.

    hintTextTable[RHT_HC_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de las afueras de un castillo# otorga #[[1]]#.

    hintTextTable[RHT_MARKET_GS_GUARD_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_GS_GUARD_HOUSE.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula bajo una custodiada caja# otorga #[[1]]#.

    hintTextTable[RHT_DMC_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #Skulltula enterrada en un volcán# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #Skulltula enterrada a la entrada de una cueva# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_NEAR_KAK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GS_NEAR_KAK.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #Skulltula oculta en el rincón de la montaña# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_ABOVE_DODONGOS_CAVERN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GS_ABOVE_DODONGOS_CAVERN.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el martillo revela #una Skulltula de la montaña# que otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_FALLING_ROCKS_PATH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GS_FALLING_ROCKS_PATH.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el martillo revela #una Skulltula de la montaña# que otorga #[[1]]#.

    hintTextTable[RHT_GC_GS_CENTER_PLATFORM] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_GS_CENTER_PLATFORM.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula suspendida# en la Ciudad Goron otorga #[[1]]#.

    hintTextTable[RHT_GC_GS_BOULDER_MAZE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_GS_BOULDER_MAZE.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #Skulltula bajo una caja# de la Ciudad Goron otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_HOUSE_UNDER_CONSTRUCTION] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_HOUSE_UNDER_CONSTRUCTION.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_SKULLTULA_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_SKULLTULA_HOUSE.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_GUARDS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_GUARDS_HOUSE.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_WATCHTOWER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_WATCHTOWER.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_ABOVE_IMPAS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GS_ABOVE_IMPAS_HOUSE.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_GS_WALL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_GS_WALL.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche revela una #Skulltula del cementerio# que otorga #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #Skulltula enterrada en el cementerio# otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_LADDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_GS_LADDER.clear", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de un río# otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_ABOVE_BRIDGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_GS_ABOVE_BRIDGE.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_NEAR_RAISED_GROTTOS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_GS_NEAR_RAISED_GROTTOS.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZD_GS_FROZEN_WATERFALL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_GS_FROZEN_WATERFALL.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a una congelada cascada# que otorga #[[1]]#.

    hintTextTable[RHT_ZF_GS_ABOVE_THE_LOG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_GS_ABOVE_THE_LOG.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a cierta deidad# que otorga #[[1]]#.

    hintTextTable[RHT_ZF_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol junto a cierta deidad# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada junto a un lago# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_SMALL_ISLAND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GS_SMALL_ISLAND.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a un lago# que otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_LAB_WALL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GS_LAB_WALL.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a un lago# que otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_LAB_CRATE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GS_LAB_CRATE.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #Skulltula bajo la sumergida caja de un laboratorio# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, la noche revela #una Skulltula del lago sobre un árbol# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada en un valle# otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_SMALL_BRIDGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GS_SMALL_BRIDGE.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_PILLAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GS_PILLAR.clear", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_BEHIND_TENT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GS_BEHIND_TENT.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GF_GS_ARCHERY_RANGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_GS_ARCHERY_RANGE.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula de una fortaleza# que otorga #[[1]]#.

    hintTextTable[RHT_GF_GS_TOP_FLOOR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_GS_TOP_FLOOR.clear", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, la noche revela una #Skulltula de una fortaleza# que otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_BEAN_PATCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GS_BEAN_PATCH.clear", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula enterrada en el desierto# otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_HILL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GS_HILL.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula en las profundidades del desierto# que otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_TREE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GS_TREE.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula en las profundidades del desierto# que otorga #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_1.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_2.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_3.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_4.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_5.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_6.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_7.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_POTION_SHOP_ITEM_8.obscure.0", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BOMBCHU_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_1.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_2.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_3.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_4.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_5.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_6.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_7.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_POTION_SHOP_ITEM_8.obscure.0", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_BAZAAR_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_BAZAAR_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_1.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_2.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_3] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_3.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_4] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_4.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_5] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_5.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_6] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_6.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_7] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_7.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_8] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SHOP_ITEM_8.clear", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_HF_DEKU_SCRUB_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_DEKU_SCRUB_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, un #singular deku bajo un hoyo# de la llanura vende #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_DEKU_SCRUB_GROTTO_LEFT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_DEKU_SCRUB_GROTTO_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_DEKU_SCRUB_GROTTO_CENTER.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un par de #dekus del bosque# venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT.clear", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un par de #dekus del bosque# venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_BRIDGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_NEAR_BRIDGE.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #deku bajo un puente# del bosque venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_GROTTO_REAR.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del bosque venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_GROTTO_FRONT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del bosque venden #[[1]]#.

    hintTextTable[RHT_SFM_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_DEKU_SCRUB_GROTTO_REAR.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# de la pradera sagrada venden #[[1]]#.

    hintTextTable[RHT_SFM_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_DEKU_SCRUB_GROTTO_FRONT.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #par de dekus subterráneos# de la pradera sagrada venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_DEKU_SCRUB_GROTTO_LEFT.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_DEKU_SCRUB_GROTTO_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_DEKU_SCRUB_GROTTO_CENTER.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_DEKU_SCRUB_GROTTO_LEFT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_DEKU_SCRUB_GROTTO_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_DEKU_SCRUB_GROTTO_CENTER.clear", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_ZR_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_DEKU_SCRUB_GROTTO_REAR.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del río venden #[[1]]#.

    hintTextTable[RHT_ZR_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_DEKU_SCRUB_GROTTO_FRONT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del río venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_DEKU_SCRUB_GROTTO_LEFT.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_DEKU_SCRUB_GROTTO_RIGHT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_DEKU_SCRUB_GROTTO_CENTER.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_GV_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_DEKU_SCRUB_GROTTO_REAR.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del valle venden #[[1]]#.

    hintTextTable[RHT_GV_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_DEKU_SCRUB_GROTTO_FRONT.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del valle venden #[[1]]#.

    hintTextTable[RHT_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT.clear", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #par de dekus subterráneos# del desierto venden #[[1]]#.

    hintTextTable[RHT_COLOSSUS_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_DEKU_SCRUB_GROTTO_REAR.clear", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #par de dekus subterráneos# del desierto venden #[[1]]#.

    hintTextTable[RHT_LLR_STABLES_LEFT_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_STABLES_LEFT_COW.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, una #vaca del establo# brinda #[[1]]#.

    hintTextTable[RHT_LLR_STABLES_RIGHT_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_STABLES_RIGHT_COW.clear", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #vaca del establo# brinda #[[1]]#.

    hintTextTable[RHT_LLR_TOWER_RIGHT_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_TOWER_RIGHT_COW.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, una #vaca del granero# brinda #[[1]]#.

    hintTextTable[RHT_LLR_TOWER_LEFT_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LLR_TOWER_LEFT_COW.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #vaca del granero# brinda #[[1]]#.

    hintTextTable[RHT_KAK_IMPAS_HOUSE_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_IMPAS_HOUSE_COW.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, una #vaca enjaulada de una casa# brinda #[[1]]#.

    hintTextTable[RHT_DMT_COW_GROTTO_COW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_COW_GROTTO_COW.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #vaca de un lujoso hoyo# brinda #[[1]]#.

    hintTextTable[RHT_BEEHIVE_CHEST_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_CHEST_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, una #colmena sobre un cofre# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_LONELY_SCRUB_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_LONELY_SCRUB_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #colmena sobre un deku solitario# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_SCRUB_PAIR_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_SCRUB_PAIR_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #colmena sobre un par de dekus# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_SCRUB_TRIO_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_SCRUB_TRIO_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #colmena sobre un trío de dekus# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_COW_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_COW_GROTTO.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #colmena sobre una vaca# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_IN_FRONT_OF_KING_ZORA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_IN_FRONT_OF_KING_ZORA.clear", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #colmena delante del rey de los zoras# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_BEHIND_KING_ZORA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEEHIVE_BEHIND_KING_ZORA.clear", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #colmena detrás del rey de los zoras# esconde #[[1]]#.

    hintTextTable[RHT_POT_LINKS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_LINKS_HOUSE.clear", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_TWINS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_TWINS_HOUSE.clear", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_KNOW_IT_ALL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_KNOW_IT_ALL.clear", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_TH_BREAK_ROOM_FRONT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_BREAK_ROOM_FRONT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_BREAK_ROOM_BACK_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_BREAK_ROOM_BACK_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_KITCHEN_POT_1] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_KITCHEN_POT_1.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_KITCHEN_POT_2] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_KITCHEN_POT_2.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_RIGHT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_1_TORCH_CELL_RIGHT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_MID_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_1_TORCH_CELL_MID_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_LEFT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_1_TORCH_CELL_LEFT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_STEEP_SLOPE_RIGHT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_STEEP_SLOPE_RIGHT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_STEEP_SLOPE_LEFT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_STEEP_SLOPE_LEFT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_RIGHT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_NEAR_DOUBLE_CELL_RIGHT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_MID_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_NEAR_DOUBLE_CELL_MID_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_LEFT_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_NEAR_DOUBLE_CELL_LEFT_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_RIGHTMOST_JAILED_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_RIGHTMOST_JAILED_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_RIGHT_MIDDLE_JAILED_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_RIGHT_MIDDLE_JAILED_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_LEFT_MIDDLE_JAILED_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_LEFT_MIDDLE_JAILED_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_LEFTMOST_JAILED_POT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_LEFTMOST_JAILED_POT.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_POT_WASTELAND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_WASTELAND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_GUARD_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_GUARD_HOUSE.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_POE_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_POE_SHOP.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_ALLEY_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_ALLEY_HOUSE.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_KAKARIKO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_KAKARIKO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_DAMPE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_DAMPE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_GORON_CITY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_GORON_CITY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_DEATH_MOUNTAIN_CRATER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_ZORAS_DOMAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_ZORAS_DOMAIN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_ZORAS_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_ZORAS_FOUNTAIN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_LON_LON_RANCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_LON_LON_RANCH.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_TALONS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_TALONS_HOUSE.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_WEB_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_WEB_GROTTO.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_MUD_WALL_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_POT_MUD_WALL_GROTTO.clear", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_KOKIRI_FOREST_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_FOREST_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/
    hintTextTable[RHT_KOKIRI_FOREST_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KOKIRI_FOREST_HEART.clear", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/
    hintTextTable[RHT_SARIAS_HOUSE_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SARIAS_HOUSE_HEART.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/
    hintTextTable[RHT_LOST_WOODS_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LOST_WOODS_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_LOST_WOODS_SHORTCUT_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LOST_WOODS_SHORTCUT_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/
    hintTextTable[RHT_LAKE_HYLIA_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LAKE_HYLIA_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_LABORATORY_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LABORATORY_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_DAMPES_GRAVE_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DAMPES_GRAVE_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/
    hintTextTable[RHT_GERUDO_VALLEY_GROTTO_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GERUDO_VALLEY_GROTTO_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_TRAIL_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_HEART] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_HEART.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_CRATER_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DEATH_MOUNTAIN_CRATER_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/
    hintTextTable[RHT_ZORAS_RIVER_WATERFALL_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_RIVER_WATERFALL_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/
    hintTextTable[RHT_ZORAS_FOUNTAIN_RUPEE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZORAS_FOUNTAIN_RUPEE.clear", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/

    hintTextTable[RHT_SFM_FAIRY_GROTTO_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_FAIRY_GROTTO_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_FAIRY_GROTTO_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_FAIRY_GROTTO_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_FAIRY_GROTTO_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_FAIRY_GROTTO_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_FAIRY_GROTTO_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_FAIRY_GROTTO_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GF_FAIRY_GROTTO_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GF_FAIRY_GROTTO_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_SHIELD_GRAVE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_SHIELD_GRAVE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_OASIS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_OASIS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_BEAN_SPROUT_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_BEAN_SPROUT_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TOT_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TOT_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TOT_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TOT_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MAZE_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MAZE_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MAZE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MAZE_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_MALON_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_MALON_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_MALON_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_MALON_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_LAB_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_LAB_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_LAB_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_LAB_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_MAZE_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_MAZE_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_MAZE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_MAZE_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_SARIA_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_SARIA_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_SARIA_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SFM_SARIA_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_JABU_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_JABU_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_JABU_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_JABU_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_ISLAND_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_ISLAND_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_POND_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_POND_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_FENCE_GROTTO_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_FENCE_GROTTO_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_FLAG_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_FLAG_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_COW_GROTTO_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_COW_GROTTO_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_SHORTCUT_STORMS_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_SHORTCUT_STORMS_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TH_KITCHEN_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TH_KITCHEN_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_DEKU_SCRUB_GROTTO_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_SUN_FAIRY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_SUN_FAIRY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_MARKET_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_MARKET_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GY_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GY_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GROTTO_GRASS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GROTTO_GRASS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GERUDO_VALLEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_GERUDO_VALLEY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GERUDOS_FORTRESS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_GERUDOS_FORTRESS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_WASTELAND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_WASTELAND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_MARKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_MARKET.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_KAKARIKO_VILLAGE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GRAVEYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_GRAVEYARD.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GORON_CITY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_GORON_CITY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_DEATH_MOUNTAIN_CRATER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_LON_LON_RANCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_LON_LON_RANCH.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_LAKESIDE_LABORATORY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_CRATE_LAKESIDE_LABORATORY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KF_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LW_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HC_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_OGC_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OGC_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_OGC_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_OGC_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMC_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GV_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_HF_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_KAK_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GY_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GY_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_LH_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZD_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZF_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_ZR_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_DMT_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_BRONZE_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_BRONZE_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_GC_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_SILVER_BOULDER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_SILVER_BOULDER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_ROCK] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_COLOSSUS_ROCK.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TREE_HYRULE_FIELD] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_HYRULE_FIELD.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_TREE_MARKET] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_MARKET.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_TREE_HYRULE_CASTLE] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_HYRULE_CASTLE.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_TREE_ZORAS_RIVER] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_ZORAS_RIVER.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_TREE_ZORAS_FOUNTAIN] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_ZORAS_FOUNTAIN.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_TREE_LON_LON_RANCH] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_TREE_LON_LON_RANCH.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_BUSH_HYRULE_FIELD] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUSH_HYRULE_FIELD.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_BUSH_ZORAS_FOUNTAIN] =
        HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUSH_ZORAS_FOUNTAIN.clear", { QM_RED, QM_GREEN }));

    hintTextTable[RHT_BUTTERFLY_FAIRY_HYRULE_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_HYRULE_CASTLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_LOST_WOODS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_LOST_WOODS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_KAKARIKO_VILLAGE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_GRAVEYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_GRAVEYARD.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_ZORAS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_ZORAS_RIVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_ZORAS_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_ZORAS_FOUNTAIN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_LAKE_HYLIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_LAKE_HYLIA.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_KF_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_KF_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_LW_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_LW_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_DMT_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_DMT_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_DMC_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_DMC_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_HF_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_HF_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_KAK_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_KAK_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BUTTERFLY_FAIRY_ZR_GROTTO] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BUTTERFLY_FAIRY_ZR_GROTTO.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_KOKIRI_FOREST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_KOKIRI_FOREST.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_LINKS_HOUSE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_LINKS_HOUSE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_DEKU_THEATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_DEKU_THEATER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_HYRULE_FIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_HYRULE_FIELD.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_MK_SHOOTING_GALLERY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_MK_SHOOTING_GALLERY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_KAK_SHOOTING_GALLERY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_KAK_SHOOTING_GALLERY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_HAPPY_MASK_SHOP] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_HAPPY_MASK_SHOP.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_TEMPLE_OF_TIME] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_TEMPLE_OF_TIME.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_HYRULE_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_HYRULE_CASTLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_KAKARIKO_VILLAGE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_GRAVEYARD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_GRAVEYARD.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_DEATH_MOUNTAIN_TRAIL] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_DEATH_MOUNTAIN_TRAIL.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_GORON_CITY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_GORON_CITY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_DEATH_MOUNTAIN_CRATER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_ZORAS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_ZORAS_RIVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_ZORAS_DOMAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_ZORAS_DOMAIN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_ZORAS_FOUNTAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_ZORAS_FOUNTAIN.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_LAKE_HYLIA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_LAKE_HYLIA.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_FISHING_POND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_FISHING_POND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_GERUDO_VALLEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_GERUDO_VALLEY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_GERUDO_FORTRESS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_GERUDO_FORTRESS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SIGN_HAUNTED_WASTELAND] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_SIGN_HAUNTED_WASTELAND.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_KOKIRI_FOREST] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_KOKIRI_FOREST.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_HYRULE_FIELD] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_HYRULE_FIELD.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_MARKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_MARKET.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_LON_LON_RANCH] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_LON_LON_RANCH.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_HYRULE_CASTLE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_HYRULE_CASTLE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_CASTLE_COURTYARD_ZELDA] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_CASTLE_COURTYARD_ZELDA.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_LOST_WOODS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_LOST_WOODS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_SACRED_FOREST_MEADOW] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_SACRED_FOREST_MEADOW.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_KAKARIKO_VILLAGE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_DAMPES_GRAVE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_DAMPES_GRAVE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_DEATH_MOUNTAIN_CRATER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_ZORAS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_ZORAS_RIVER.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_ZORAS_RIVER] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_ZORAS_RIVER.clear_2", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_GERUDO_VALLEY] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_GERUDO_VALLEY.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_GERUDOS_FORTRESS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_GERUDOS_FORTRESS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_THIEVES_HIDEOUT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_THIEVES_HIDEOUT.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_WONDER_ITEM_DESERT_COLOSSUS] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_WONDER_ITEM_DESERT_COLOSSUS.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BEGGAR_MARKET] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEGGAR_MARKET.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_BEGGAR_KAKARIKO_VILLAGE] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_BEGGAR_KAKARIKO_VILLAGE.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_FISHERMANS_HAT] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_FISHERMANS_HAT.clear", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_RED_ICE_ZORAS_DOMAIN] = HintText(CustomMessages::CreateMessage("randomizer.hints.RHT_RED_ICE_ZORAS_DOMAIN.clear", {QM_RED, QM_GREEN}));

    // clang-format on
}
} // namespace Rando
