#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {
void StaticData::HintTable_Init_Exclude_Dungeon() {
  /*--------------------------
  |        DEKU TREE         |
  ---------------------------*/
   hintTextTable[RHT_DEKU_TREE_MAP_CHEST] = HintText(CustomMessage("They say that in the #center of the Deku Tree# lies #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß im #Zentrum des Deku-Baums# #[[1]]# läge.",
                                                         /*french*/ "Selon moi, #le coeur de l'Arbre Mojo# recèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, al #centro del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_SLINGSHOT_CHEST] = HintText(CustomMessage("They say that the #treasure guarded by a scrub# in the Deku Tree is #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #von einem Deku bewachter Schatz# im Deku-Baum #[[1]]# sei.",
                                                               /*french*/ "Selon moi, le #trésor protégé par une peste# dans l'Arbre Mojo est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST] = HintText(CustomMessage("They say that the #treasure guarded by a scrub# in the Deku Tree is #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß ein #von einem Deku bewachter Schatz# im Deku-Baum #[[1]]# sei.",
                                                                         /*french*/ "Selon moi, le #trésor protégé par une peste# dans l'Arbre Mojo est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_COMPASS_CHEST] = HintText(CustomMessage("They say that #pillars of wood# in the Deku Tree lead to #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß die #Säulen aus Holz# im Deku-Baum zu #[[1]]# führen würden.",
                                                             /*french*/ "Selon moi, les #piliers de bois# dans l'Arbre Mojo indiquent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST] = HintText(CustomMessage("They say that #pillars of wood# in the Deku Tree lead to #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß die #Säulen aus Holz# im Deku-Baum zu #[[1]]# führen würden.",
                                                                       /*french*/ "Selon moi, les #piliers de bois# dans l'Arbre Mojo indiquent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_BASEMENT_CHEST] = HintText(CustomMessage("They say that #webs in the Deku Tree# hide #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß #Spinnweben im Deku-Baum# #[[1]]# verbergen würden.",
                                                              /*french*/ "Selon moi, les #toiles dans l'Arbre Mojo# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, entre #telarañas del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that in the #center of the Deku Tree# lies #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß im #Zentrum des Deku-Baums# #[[1]]# läge.",
                                                            /*french*/ "Selon moi, #le coeur de l'Arbre Mojo# recèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, al #centro del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that a #treasure guarded by a large spider# in the Deku Tree is #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #von einer großen Spinne bewachter Schatz# im Deku-Baum #[[1]]# sei.",
                                                                /*french*/ "Selon moi, le #trésor protégé par une grosse araignée# dans l'Arbre Mojo est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #gran araña del Gran Árbol Deku# esconde #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_SLINGSHOT_CHEST] = HintText(CustomMessage("They say that #pillars of wood# in the Deku Tree lead to #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß die #Säulen aus Holz# im Deku-Baum zu #[[1]]# führen würden.",
                                                                  /*french*/ "Selon moi, les #piliers de bois# dans l'Arbre Mojo indiquent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST] = HintText(CustomMessage("They say that #pillars of wood# in the Deku Tree lead to #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß die #Säulen aus Holz# im Deku-Baum zu #[[1]]# führen würden.",
                                                                            /*french*/ "Selon moi, les #piliers de bois# dans l'Arbre Mojo indiquent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, los #salientes del Gran Árbol Deku# conducen a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_BASEMENT_CHEST] = HintText(CustomMessage("They say that #webs in the Deku Tree# hide #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Spinnweben im Deku-Baum# #[[1]]# verbergen würden.",
                                                                 /*french*/ "Selon moi, les #toiles dans l'Arbre Mojo# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, entre #telarañas del Gran Árbol Deku# yace #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST] = HintText(CustomMessage("They say that #magical fire in the Deku Tree# leads to #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß #magisches Feuer im Deku-Baum# zu #[[1]]# führe.",
                                                                            /*french*/ "Selon moi, le #feu magique dans l'Arbre Mojo# éclaire #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, el #fuego mágico en el Gran Árbol Deku# conduce a #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_QUEEN_GOHMA_HEART] = HintText(CustomMessage("They say that #Queen Gohma# holds #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Königin Gohma# #[[1]]# besäße.",
                                                                 /*french*/ "Selon moi, la #Reine Gohma# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, la #Reina Goma# porta #[[1]]#.
                                                              {}, {
                                                              CustomMessage("They say that the #Parasitic Armored Arachnid# holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß das #gepanzerte parasitäre Spinne# #[[1]]# besäße.",
                                                                  /*french*/ "Selon moi, le #monstre insectoïde géant# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #arácnido parasitario acorazado# porta #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_BACK_ROOM] = HintText(CustomMessage("They say that a #spider deep within the Deku Tree# hides #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine #Spinne tief innerhalb des Deku-Baums# #[[1]]# verstecke.",
                                                                     /*french*/ "Selon moi, une #Skulltula au coeur de l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula en las profundidades del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_GATE] = HintText(CustomMessage("They say that a #web protects a spider# within the Deku Tree holding #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß eine #von einer Webe geschützte Spinne# innerhalb des Deku-Baums #[[1]]# hielte.",
                                                                /*french*/ "Selon moi, une #Skulltula derrière une toile dans l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula protegida por su tela# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_BASEMENT_VINES] = HintText(CustomMessage("They say that a #web protects a spider# within the Deku Tree holding #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß eine #von einer Webe geschützte Spinne# innerhalb des Deku-Baums #[[1]]# hielte.",
                                                                 /*french*/ "Selon moi, une #Skulltula derrière une toile dans l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #Skulltula protegida por su tela# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_GS_COMPASS_ROOM] = HintText(CustomMessage("They say that a #spider atop the Deku Tree# holds #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine #Spinne auf der Spitze des Deku-Baums# #[[1]]# hielte.",
                                                               /*french*/ "Selon moi, une #Skulltula au sommet de l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #Skulltula en lo alto del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_LOBBY] = HintText(CustomMessage("They say that a #spider in a crate# within the Deku Tree hides #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #Spinne in einer Kiste# innerhalb des Deku-Baums #[[1]]# verstecke.",
                                                           /*french*/ "Selon moi, une #Skulltula dans une boîte dans l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula bajo una caja# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_PAST_BOULDER_VINES] = HintText(CustomMessage("They say that a #wall of rock protects a spider# within the Deku Tree holding #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #von einer Steinwand geschützte Spinne# innerhalb des Deku-Baums #[[1]]# hielte.",
                                                                  /*french*/ "Selon moi, une #Skulltula derrière des rochers dans l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula protegida por una pared rocosa# del Árbol Deku otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM] = HintText(CustomMessage("They say that a #spider deep within the Deku Tree# hides #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß eine #Spinne tief innerhalb des Deku-Baums# #[[1]]# verstecke.",
                                                                        /*french*/ "Selon moi, une #Skulltula au coeur de l'Arbre Mojo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula en las profundidades del Árbol Deku# otorga #[[1]]#.

    hintTextTable[RHT_DEKU_TREE_MQ_DEKU_SCRUB] = HintText(CustomMessage("They say that a #scrub in the Deku Tree# sells #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Deku im Deku-Baum# #[[1]]# verkaufe.",
                                                             /*french*/ "Selon moi, la #peste Mojo dans l'Arbre Mojo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, un #deku del Gran Árbol Deku# vende #[[1]]#.

  /*--------------------------
  |     DODONGOS CAVERN      |
  ---------------------------*/
hintTextTable[RHT_DODONGOS_CAVERN_BOSS_ROOM_CHEST] = HintText(CustomMessage("They say that #above King Dodongo# lies #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #auf König Dodongo# #[[1]]# läge.",
                                                                     /*french*/ "Selon moi, #par dessus le Roi Dodongo# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, #sobre el Rey Dodongo# yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MAP_CHEST] = HintText(CustomMessage("They say that a #muddy wall in Dodongo's Cavern# hides #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine #schlammige Wand in Dodongos Höhle# #[[1]]# verstecke.",
                                                               /*french*/ "Selon moi, le #mur fragile dans la Caverne Dodongo# recèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, tras una #agrietada pared en la Cueva de los Dodongos# yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_COMPASS_CHEST] = HintText(CustomMessage("They say that a #statue in Dodongo's Cavern# guards #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #Statue in Dodongos Höhle# #[[1]]# bewache.",
                                                                   /*french*/ "Selon moi, la #statue dans la Caverne Dodongo# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #estatua de la Cueva de los Dodongos# esconde #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST] = HintText(CustomMessage("They say that above a #maze of stone# in Dodongo's Cavern lies #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß auf einem #Labyrinth aus Stein# in Dodongos Höhle #[[1]]# läge.",
                                                                                /*french*/ "Selon moi, sur #un labyrinthe de pierre# dans la Caverne Dodongo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, entre un #laberinto de piedra# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_BOMB_BAG_CHEST] = HintText(CustomMessage("They say that the #second lizard cavern battle# yields #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß der #zweite Reptilienkampf der Höhle# #[[1]]# brächte.",
                                                                    /*french*/ "Selon moi, le #deuxième duel de lézards de caverne# octroie #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #luchar dos veces contra reptiles en una cueva# conduce a #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST] = HintText(CustomMessage("They say that a #chest at the end of a bridge# yields #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß eine #Truhe am Ende der Brücke# #[[1]]# brächte.",
                                                                         /*french*/ "Selon moi, le #trésor à l'extrémité d'un pont# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #cofre al final de un quebrado puente# contiene #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_MAP_CHEST] = HintText(CustomMessage("They say that a #muddy wall in Dodongo's Cavern# hides #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #schlammige Wand in Dodongos Höhle# #[[1]]# verstecke.",
                                                                  /*french*/ "Selon moi, le #mur fragile dans la Caverne Dodongo# recèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #agrietada pared en la Cueva de los Dodongos# esconde #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST] = HintText(CustomMessage("They say that an #elevated alcove# in Dodongo's Cavern holds #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß ein #erhöhter Alkoven# in Dodongos Höhle #[[1]]# hielte.",
                                                                       /*french*/ "Selon moi, l'#alcove haut perchée# dans la Caverne Dodongo cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #elevada alcoba# en la Cueva de los Dodongos brinda #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that #fire-breathing lizards# in Dodongo's Cavern guard #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß #feuerschnaubende Reptilien# in Dodongos Höhle #[[1]]# bewachen würden.",
                                                                      /*french*/ "Selon moi, les #lézards cracheurs de feu# dans la Caverne Dodongo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, unos #flamígeros reptiles# en la Cueva de los Dodongos esconden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST] = HintText(CustomMessage("They say that #baby spiders# in Dodongo's Cavern guard #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #kleine Spinnen# in Dodongos Höhle #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, les #petites araignées dans la Caverne Dodongo# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, unas #pequeñas larvas# en la Cueva de los Dodongos esconden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST] = HintText(CustomMessage("They say that above a #maze of stone# in Dodongo's Cavern lies #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß auf einem #Labyrinth aus Stein# in Dodongos Höhle #[[1]]# läge.",
                                                                                /*french*/ "Selon moi, sur #un labyrinthe de pierre# dans la Caverne Dodongo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, sobre un #laberinto de piedra# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST] = HintText(CustomMessage("They say that #beneath a headstone# in Dodongo's Cavern lies #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #unterhalb eines Grabsteins# in Dodongos Höhle #[[1]]# läge.",
                                                                          /*french*/ "Selon moi, #sous une pierre tombale# dans la Caverne Dodongo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, #bajo una lápida# en la Cueva de los Dodongos yace #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_KING_DODONGO_HEART] = HintText(CustomMessage("They say that #King Dodongo# holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #König Dodongo# #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, le #Roi Dodongo# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                     // /*spanish*/ Según dicen, el #Rey Dodongo# porta #[[1]]#.
                                                                     {}, {
                                                                     CustomMessage("They say that the #Infernal Dinosaur# holds #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß der #infernalische Dinosaurier# #[[1]]# besäße.",
                                                                         /*french*/ "Selon moi, le #dinosaure infernal# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                      // /*spanish*/ Según dicen, el #dinosaurio infernal# porta #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS] = HintText(CustomMessage("They say that a #spider entangled in vines# in Dodongo's Cavern guards #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß eine in #Reben verwobene Spinne# in Dodongos Höhle #[[1]]# bewache.",
                                                                           /*french*/ "Selon moi, une #Skulltula sur les vignes dans la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula sobre unas cepas# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_SCARECROW] = HintText(CustomMessage("They say that a #spider among explosive slugs# hides #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #Spinne inmitten explosiver Schnecken# #[[1]]# verstecke.",
                                                                  /*french*/ "Selon moi, une #Skulltula dans l'alcove du couloir dans la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula rodeada de explosivos gusanos# otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS] = HintText(CustomMessage("They say that a #spider just out of reach# in Dodongo's Cavern holds #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß eine #Spinne außer Reichweite# in Dodongos Höhle #[[1]]# hielte.",
                                                                            /*french*/ "Selon moi, une #Skulltula au haut des escaliers de la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, una #Skulltula fuera del alcance# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_BACK_ROOM] = HintText(CustomMessage("They say that a #spider behind a statue# in Dodongo's Cavern holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #Spinne hinter einer Statue# in Dodongos Höhle #[[1]]# hielte.",
                                                                  /*french*/ "Selon moi, une #Skulltula au coeur de la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula tras una estatua# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText(CustomMessage("They say that a #spider among bats# in Dodongo's Cavern holds #[[1]]#.",
                                                                                      /*german*/ "Man erzählt sich, daß eine #Spinne inmitten von Fledermäusen# in Dodongos Höhle #[[1]]# hielte.",
                                                                                      /*french*/ "Selon moi, une #Skulltula entourée de Saigneurs dans la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, una #Skulltula rodeada de murciélagos# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM] = HintText(CustomMessage("They say that a #spider high on a wall# in Dodongo's Cavern holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß eine #hoch an einer Wand befindliche Spinne# in Dodongos Höhle #[[1]]# hielte.",
                                                                      /*french*/ "Selon moi, une #Skulltula haut perchée dans la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, una #Skulltula en lo alto de una pared# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM] = HintText(CustomMessage("They say that a #spider on top of a pillar of rock# in Dodongo's Cavern holds #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß eine #Spinne auf einer Steinsäule# in Dodongos Höhle #[[1]]# hielte.",
                                                                         /*french*/ "Selon moi, une #Skulltula sur l'énorme pilier de roc de la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, una #Skulltula en lo alto de un pilar# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM] = HintText(CustomMessage("They say that a #spider in a crate# in Dodongo's Cavern holds #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß eine #Spinne in einer Kiste# in Dodongos Höhle #[[1]]# hielte.",
                                                                       /*french*/ "Selon moi, une #Skulltula dans une boîte de la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #Skulltula bajo una caja# de la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_GS_BACK_AREA] = HintText(CustomMessage("They say that a #spider among graves# in Dodongo's Cavern holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine #Spinne inmitten von Gräbern# in Dodongos Höhle #[[1]]# hielte.",
                                                                     /*french*/ "Selon moi, une #Skulltula parmi les tombes de la Caverne Dodongo# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula entre lápidas# en la Cueva de los Dodongos otorga #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT] = HintText(CustomMessage("They say that a pair of #scrubs in Dodongo's Cavern# sells #[[1]]#.",
                                                                                   /*german*/ "Man erzählt sich, daß ein #Deku-Paar in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                                   /*french*/ "Selon moi, le #duo de peste Mojo dans la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS] = HintText(CustomMessage("They say that a #scrub guarded by Lizalfos# sells #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß ein #von Lizalfos bewachter Deku# #[[1]]# verkaufe.",
                                                                                        /*french*/ "Selon moi, la #peste Mojo au coeur de la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, un #deku custodiado por Lizalfos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT] = HintText(CustomMessage("They say that a pair of #scrubs in Dodongo's Cavern# sells #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß ein #Deku-Paar in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                                    /*french*/ "Selon moi, le #duo de peste Mojo dans la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY] = HintText(CustomMessage("They say that a #scrub in Dodongo's Cavern# sells #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß ein #Deku in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                      /*french*/ "Selon moi, la #peste Mojo dans l'entrée de la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #deku en la Cueva de los Dodongos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR] = HintText(CustomMessage("They say that a pair of #scrubs in Dodongo's Cavern# sells #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß ein #Deku-Paar in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                              /*french*/ "Selon moi, le #duo de peste Mojo dans l'entrée de la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT] = HintText(CustomMessage("They say that a pair of #scrubs in Dodongo's Cavern# sells #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Paar in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                               /*french*/ "Selon moi, le #duo de peste Mojo dans l'entrée de la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, un #par de dekus en la Cueva de los Dodongos# venden #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE] = HintText(CustomMessage("They say that a #scrub in Dodongo's Cavern# sells #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß ein #Deku-Paar in Dodongos Höhle# #[[1]]# verkaufe.",
                                                                             /*french*/ "Selon moi, la #peste Mojo au sommet des escaliers dans la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, un #deku en la Cueva de los Dodongos# vende #[[1]]#.

    hintTextTable[RHT_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText(CustomMessage("They say that a #scrub guarded by Lizalfos# sells #[[1]]#.",
                                                                                                 /*german*/ "Man erzählt sich, daß ein #von Lizalfos bewachter Deku# #[[1]]# verkaufe.",
                                                                                                 /*french*/ "Selon moi, la #peste Mojo au coeur de la Caverne Dodongo# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                              // /*spanish*/ Según dicen, un #deku custodiado por Lizalfos# vende #[[1]]#.

  /*--------------------------
  |     JABU JABUS BELLY     |
  ---------------------------*/
    hintTextTable[RHT_JABU_JABUS_BELLY_MAP_CHEST] = HintText(CustomMessage("They say that a #slimy thing# guards #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #schleimiges Ding# #[[1]]# bewache.",
                                                                /*french*/ "Selon moi, la #chose gluante# gardien protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, un #tentáculo parasitario# protege #[[1]]#.
                                                             {}, {
                                                             CustomMessage("They say that #tentacle trouble# in a deity's belly guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Tentakel# im Bauch einer Gottheit #[[1]]# bewachen würden.",
                                                                 /*french*/ "Selon moi, la #membrane# dans le ventre du gardien protège #[[1]]#.", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, un #problema tentaculoso# en la tripa de cierta deidad esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_COMPASS_CHEST] = HintText(CustomMessage("They say that #bubbles# guard #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #Blasen# #[[1]]# bewachen würden.",
                                                                    /*french*/ "Selon moi, des #bulles# entourent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, unas #burbujas# protegen #[[1]]#.
                                                                 {}, {
                                                                 CustomMessage("They say that #bubble trouble# in a deity's belly guards #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #Blasen# im Bauch einer Gottheit #[[1]]# bewachen würden.",
                                                                     /*french*/ "Selon moi, un #horde de bulles# dans le ventre du gardien protègent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #problema burbujesco# en la tripa de cierta deidad esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST] = HintText(CustomMessage("They say that shooting a #mouth cow# reveals #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß ein #Kuhmund# #[[1]]# enthüllen würde.",
                                                                               /*french*/ "Selon moi, tirer sur une #vache# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #dispararle a una vaca# revela #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_MAP_CHEST] = HintText(CustomMessage("They say that a #boulder before cows# hides #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß ein #Findling vor Kühen# #[[1]]# verstecke.",
                                                                   /*french*/ "Selon moi, des #rochers près des vaches# cachent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, cierta #roca rodeada de vacas# esconde #[[1]]#.
                                                                {}, {
                                                                CustomMessage("They say that #pop rocks# hide #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #Knallfelsen# #[[1]]# verbergen würden.",
                                                                    /*french*/ "Selon moi, des #pierres aux reins# cachent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, #cepillarse los dientes con explosivos# revela #[[1]]#.
                                                                CustomMessage("They say that an #explosive palate# holds #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß ein #explosiver Gaumen# #[[1]]# hielte.",
                                                                    /*french*/ "Selon moi, des #gargouillis explosifs# cachent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                 // /*spanish*/ Según dicen, un #paladar explosivo# esconde #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST] = HintText(CustomMessage("They say that near a #spiked elevator# lies #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß in der Nähe eines #stachligen Aufzugs# #[[1]]# läge.",
                                                                                 /*french*/ "Selon moi, près d'un #ascenseur visqueux# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, cerca de un #ascensor puntiagudo# yace #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that a #drowning cow# unveils #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß eine #ertrinkende Kuh# #[[1]]# enthüllen würde.",
                                                                       /*french*/ "Selon moi, une #vache à l'eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #vaca sumergida# revela #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST] = HintText(CustomMessage("They say that #moving anatomy# creates a path to #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß eine #sich bewegende Anatomie# einen Pfad zu #[[1]]# kreiere.",
                                                                                 /*french*/ "Selon moi, un #organe descendant# mène à #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, un #ser movedizo entre paredes# conduce a #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST] = HintText(CustomMessage("They say that a #pair of digested cows# hold #[[1]]#.",
                                                                                      /*german*/ "Man erzählt sich, daß ein #Paar verdauter Kühe# #[[1]]# hielte.",
                                                                                      /*french*/ "Selon moi, #deux boeufs digérés# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, un #par de digeridas vacas# otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST] = HintText(CustomMessage("They say that a #pair of digested cows# hold #[[1]]#.",
                                                                                   /*german*/ "Man erzählt sich, daß ein #Paar verdauter Kühe# #[[1]]# hielte.",
                                                                                   /*french*/ "Selon moi, #deux boeufs digérés# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, un #par de digeridas vacas# otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST] = HintText(CustomMessage("They say that the #final cows' reward# in a deity's belly is #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß die #Belohnung der letzten Kuh# im Bauch einer Gottheit #[[1]]# sei.",
                                                                         /*french*/ "Selon moi, le #cadeau des dernières vaches# estomacales est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, las #vacas al final# de la tripa de cierta deidad otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST] = HintText(CustomMessage("They say that #cows protected by falling monsters# in a deity's belly guard hide #[[1]]#.",
                                                                                      /*german*/ "Man erzählt sich, daß #von fallenden Monstern bewachte Kühe# im Bauch einer Gottheit #[[1]]# verstecken würden.",
                                                                                      /*french*/ "Selon moi, des #vaches protégées par des monstres tombants# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, unas #vacas custodiadas por monstruos del techo# de la tripa de cierta deidad otorgan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST] = HintText(CustomMessage("They say that a school of #stingers swallowed by Jabu-Jabu# guard #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß von #Jabu-Jabu verschluckte Rochen# #[[1]]# bewachen würden.",
                                                                                    /*french*/ "Selon moi, les #raies avallées par Jabu-Jabu# protègent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                                 // /*spanish*/ Según dicen, unos #stingers engullidos por Jabu-Jabu# guardan #[[1]]#.
                                                                                 {}, {
                                                                                 CustomMessage("They say that a school of #stingers swallowed by a deity# guard #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß von #einer Gottheit verschluckte Rochen# #[[1]]# bewachen würden.",
                                                                                     /*french*/ "Selon moi, les #raies avallées par le gardien# protègent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                                  // /*spanish*/ Según dicen, unos #stingers engullidos por cierta deidad# guardan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST] = HintText(CustomMessage("They say that a school of #stingers swallowed by Jabu-Jabu# guard #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß von #Jabu-Jabu verschluckte Rochen# #[[1]]# bewachen würden.",
                                                                         /*french*/ "Selon moi, les #raies avallées par Jabu-Jabu# protègent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, unos #stingers engullidos por Jabu-Jabu# guardan #[[1]]#.
                                                                      {}, {
                                                                      CustomMessage("They say that a school of #stingers swallowed by a deity# guard #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß von #einer Gottheit verschluckte Rochen# #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, les #raies avallées par le gardien# protègent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, unos #stingers engullidos por cierta deidad# guardan #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_BARINADE_HEART] = HintText(CustomMessage("They say that #Barinade# holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #Barinade# #[[1]]# besäße.",
                                                                     /*french*/ "Selon moi, #Barinade# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, #Barinade# porta #[[1]]#.
                                                                  {}, {
                                                                  CustomMessage("They say that the #Bio-Electric Anemone# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß die #bioelektrische Anemone# #[[1]]# besäße.",
                                                                      /*french*/ "Selon moi, l'#anémone bioélectrique# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, la #anémona bioeléctrica# porta #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER] = HintText(CustomMessage("They say that a #spider resting near a princess# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß eine #in der Nähe einer Prinzessin ruhende Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                              /*french*/ "Selon moi, une #Skulltula près de la princesse dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER] = HintText(CustomMessage("They say that a #spider resting near a princess# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß eine #in der Nähe einer Prinzessin ruhende Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                              /*french*/ "Selon moi, une #Skulltula près de la princesse dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_NEAR_BOSS] = HintText(CustomMessage("They say that #a spider surrounded by jellyfish# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #von Quallen umgebene Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                   /*french*/ "Selon moi, une #Skulltula entourée de méduses dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula rodeada de medusas# otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM] = HintText(CustomMessage("They say that a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß eine #von Rochen bewachte Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                           /*french*/ "Selon moi, une #Skulltula protégée par des raies dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula rodeada por unos stingers# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM] = HintText(CustomMessage("They say that a #spider surrounded by electricity# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß eine #von Elektrizität umgebene Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                             /*french*/ "Selon moi, une #Skulltula entourée d'électricité dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula rodeada de electricidad# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM] = HintText(CustomMessage("They say that a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß eine #von Rochen bewachte Spinne# in Jabu-Jabus Bauch #[[1]]# hielte.",
                                                                                 /*french*/ "Selon moi, une #Skulltula protégée par des raies dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula protegida por unos stingers# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS] = HintText(CustomMessage("They say that a #spider in a web within Jabu-Jabu's Belly# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß eine #Spinne innerhalb einer Webe in Jabu-Jabus Bauch# #[[1]]# hielte.",
                                                                      /*french*/ "Selon moi, une #Skulltula sur une toile dans le Ventre de Jabu-Jabu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, una #Skulltula sobre una red# en la Tripa de Jabu-Jabu otorga #[[1]]#.

    hintTextTable[RHT_JABU_JABUS_BELLY_DEKU_SCRUB] = HintText(CustomMessage("They say that a #scrub in a deity# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku in einer Gottheit #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, la #peste Mojo dans le ventre du gardien# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #deku dentro de cierta deidad# vende #[[1]]#.
  /*--------------------------
  |      FOREST TEMPLE       |
  ---------------------------*/
 hintTextTable[RHT_FOREST_TEMPLE_FIRST_ROOM_CHEST] = HintText(CustomMessage("They say that a #tree in the Forest Temple# supports #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß ein #Baum im Waldtempel# #[[1]]# unterstütze.",
                                                                    /*french*/ "Selon moi, sur l'#arbre dans le Temple de la Forêt# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, sobre un #árbol del Templo del Bosque# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FIRST_STALFOS_CHEST] = HintText(CustomMessage("They say that #defeating enemies beneath a falling ceiling# in Forest Temple yields #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß das #Besiegen von Gegnern unter einer fallenden Decke# im Waldtempel #[[1]]# brächte.",
                                                                       /*french*/ "Selon moi, #deux squelettes# dans le Temple de la Forêt protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #derrotar enemigos caídos de lo alto# del Templo del Bosque revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_WELL_CHEST] = HintText(CustomMessage("They say that a #sunken chest deep in the woods# contains #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß eine #gesunkene Truhe tief im Wald# #[[1]]# enthielte.",
                                                              /*french*/ "Selon moi, le #coffre submergé dans la forêt# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, un #sumergido cofre en lo profundo del bosque# contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MAP_CHEST] = HintText(CustomMessage("They say that a #fiery skull# in Forest Temple guards #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #feuriger Schädel# im Waldtempel #[[1]]# bewache.",
                                                             /*french*/ "Selon moi, le #crâne enflammé# dans le Temple de la Forêt protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #ardiente calavera# del Templo del Bosque esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST] = HintText(CustomMessage("They say that a #chest on a small island# in the Forest Temple holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß eine ##Truhe auf einer kleinen Insel# im Waldtempel #[[1]]# enthielte.",
                                                                                 /*french*/ "Selon moi, le #coffre sur l'îlot# du Temple de la Forêt contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, un #cofre sobre una isla# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST] = HintText(CustomMessage("They say that beneath a #checkerboard falling ceiling# lies #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß unter einer #fallenden Schachbrettdecke# #[[1]]# läge.",
                                                                              /*french*/ "Selon moi, sous #l'échiquier tombant# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, tras un #techo de ajedrez# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_EYE_SWITCH_CHEST] = HintText(CustomMessage("They say that #blocks of stone# in the Forest Temple surround #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #Steinblöcke# im Waldtempel #[[1]]# umgeben würden.",
                                                                    /*french*/ "Selon moi, les #blocs dans le Temple de la Forêt# entourent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, cerca de unos #bloques de piedra# del Templo del Bosque yace #[[1]]#.
                                                                 {}, {
                                                                 CustomMessage("They say that a #sharp eye# will spot #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß ein #scharfes Auge# #[[1]]# erkennen würde.",
                                                                     /*french*/ "Selon moi, l'#oeil perçant# dans la forêt verra #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #afilado ojo# revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a #turned trunk# contains #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein #gedrehter Baumstamm# #[[1]]# enthielte.",
                                                                  /*french*/ "Selon moi, le #coffre pivoté# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en una #sala con otro punto de vista# se esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_FLOORMASTER_CHEST] = HintText(CustomMessage("They say that deep in the forest #shadows guard a chest# containing #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß tief im Wald, #Schatten eine Truhe bewachen#, welche [[1]]# enthielte.",
                                                                     /*french*/ "Selon moi, l'#ombre de la forêt# protège un coffre contenant #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, en lo profundo del bosque #unas sombras# esconden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BOW_CHEST] = HintText(CustomMessage("They say that #Stalfos deep in the Forest Temple# guard #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #Stalfos tief im Waldtempel# #[[1]]# bewachen würden.",
                                                             /*french*/ "Selon moi, #trois squelettes dans le Temple de la Forêt# protègent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, los #Stalfos en lo profundo del Templo del Bosque# guardan #[[1]]#.
                                                          {}, {
                                                          CustomMessage("They say that an #army of the dead# guards #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß eine #Armee der Toten# #[[1]]# bewachen würden.",
                                                              /*french*/ "Selon moi, des #squelettes sylvestres# protègent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, un #ejército de soldados caídos# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_RED_POE_CHEST] = HintText(CustomMessage("They say that a #red ghost# guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #roter Geist# #[[1]]# bewachen würde.",
                                                                 /*french*/ "Selon moi, le #fantôme rouge# protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, un #espectro rojo# guarda #[[1]]#.
                                                              {}, {
                                                              CustomMessage("They say that #Joelle# guards #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #Joelle# #[[1]]# bewachen würde.",
                                                                  /*french*/ "Selon moi, #Joelle# protège #[[1]]#.", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, #Joelle# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BLUE_POE_CHEST] = HintText(CustomMessage("They say that a #blue ghost# guards #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein #blauer Geist# #[[1]]# bewachen würde.",
                                                                  /*french*/ "Selon moi, le #fantôme bleu# protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, un #espectro azul# guarda #[[1]]#.
                                                               {}, {
                                                               CustomMessage("They say that #Beth# guards #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß #Beth# #[[1]]# bewachen würde.",
                                                                   /*french*/ "Selon moi, #Beth# protège #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, #Beth# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_BASEMENT_CHEST] = HintText(CustomMessage("They say that #revolving walls# in the Forest Temple conceal #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #drehende Wände# im Waldtempel #[[1]]# verbergen würden.",
                                                                  /*french*/ "Selon moi, des #murs rotatifs dans la forêt# recèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, las #paredes giratorias# del Templo del Bosque conceden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST] = HintText(CustomMessage("They say that a #tree in the Forest Temple# supports #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß ein #Baum im Waldtempel# #[[1]]# unterstütze.",
                                                                       /*french*/ "Selon moi, sur l'#arbre dans le Temple de la Forêt# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, sobre un #árbol del Templo del Bosque# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_WOLFOS_CHEST] = HintText(CustomMessage("They say that #defeating enemies beneath a falling ceiling# in Forest Temple yields #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß das #Besiegen von Gegnern unter einer fallenden Decke# im Waldtempel #[[1]]# brächte.",
                                                                   /*french*/ "Selon moi, #deux squelettes# dans le Temple de la Forêt protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, #derrotar enemigos caídos de lo alto# del Templo del Bosque revela #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BOW_CHEST] = HintText(CustomMessage("They say that #Stalfos deep in the Forest Temple# guard #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #Stalfos tief im Waldtempel# #[[1]]# bewachen würden.",
                                                                /*french*/ "Selon moi, #trois squelettes dans le Temple de la Forêt# protègent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, los #Stalfos en lo profundo del Templo del Bosque# guardan #[[1]]#.
                                                             {}, {
                                                             CustomMessage("They say that an #army of the dead# guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß eine #Armee der Toten# #[[1]]# bewachen würden.",
                                                                 /*french*/ "Selon moi, des #squelettes sylvestres# protègent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, un #ejército de soldados caídos# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST] = HintText(CustomMessage("They say that a #chest on a small island# in the Forest Temple holds #[[1]]#.",
                                                                                          /*german*/ "Man erzählt sich, daß eine ##Truhe auf einer kleinen Insel# im Waldtempel #[[1]]# enthielte.",
                                                                                          /*french*/ "Selon moi, le #coffre sur l'îlot# du Temple de la Forêt contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, un #cofre sobre una isla# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST] = HintText(CustomMessage("They say that #high in a courtyard# within the Forest Temple is #[[1]]#.",
                                                                                          /*german*/ "Man erzählt sich, daß #hoch in einem Hof# innerhalb des Waldtempels #[[1]]# sei.",
                                                                                          /*french*/ "Selon moi, #haut perché dans le jardin# du Temple de la Forêt gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, un #cofre en lo alto de un patio# del Templo del Bosque contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_WELL_CHEST] = HintText(CustomMessage("They say that a #sunken chest deep in the woods# contains #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß eine #gesunkene Truhe tief im Wald# #[[1]]# enthielte.",
                                                                 /*french*/ "Selon moi, le #coffre submergé dans la forêt# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #sumergido cofre en lo profundo del bosque# contiene #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that a #red ghost# guards #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #roter Geist# #[[1]]# bewachen würde.",
                                                                /*french*/ "Selon moi, le #fantôme rouge# protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, un #fantasma rojo# guarda #[[1]]#.
                                                             {}, {
                                                             CustomMessage("They say that #Joelle# guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Joelle# #[[1]]# bewachen würde.",
                                                                 /*french*/ "Selon moi, #Joelle# protège #[[1]]#.", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, #Joelle# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that a #blue ghost# guards #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß ein #blauer Geist# #[[1]]# bewachen würde.",
                                                                    /*french*/ "Selon moi, le #fantôme bleu# protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, un #fantasma azul# guarda #[[1]]#.
                                                                 {}, {
                                                                 CustomMessage("They say that #Beth# guards #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #Beth# #[[1]]# bewachen würde.",
                                                                     /*french*/ "Selon moi, #Beth# protège #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, #Beth# guarda #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST] = HintText(CustomMessage("They say that beneath a #checkerboard falling ceiling# lies #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß unter einer #fallenden Schachbrettdecke# #[[1]]# läge.",
                                                                                 /*french*/ "Selon moi, sous #l'échiquier tombant# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, tras un #techo de ajedrez# yace #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BASEMENT_CHEST] = HintText(CustomMessage("They say that #revolving walls# in the Forest Temple conceal #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #drehende Wände# im Waldtempel #[[1]]# verbergen würden.",
                                                                     /*french*/ "Selon moi, des #murs rotatifs dans la forêt# recèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, las #paredes giratorias# del Templo del Bosque conceden #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_REDEAD_CHEST] = HintText(CustomMessage("They say that deep in the forest #undead guard a chest# containing #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß tief im Wald #Untote eine Truhe bewachen#, welche #[[1]]# enthielte.",
                                                                   /*french*/ "Selon moi, des #revenants dans le Temple de la Forêt# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, en lo profundo del bosque #guardias del más allá# guardan #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a #turned trunk# contains #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß ein #gedrehter Baumstamm# #[[1]]# enthielte.",
                                                                     /*french*/ "Selon moi, le #coffre pivoté# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, en una #sala con otro punto de vista# se esconde #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_PHANTOM_GANON_HEART] = HintText(CustomMessage("They say that #Phantom Ganon# holds #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß #Phantom-Ganon# #[[1]]# besäße.",
                                                                       /*french*/ "Selon moi, #Ganon Spectral# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                    // /*spanish*/ Según dicen, #Ganon Fantasma# porta #[[1]]#.
                                                                    {}, {
                                                                    CustomMessage("They say that the #Evil Spirit from Beyond# holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #böse Geist aus dem Jenseits# #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, l'#esprit maléfique de l'au-delà# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                     // /*spanish*/ Según dicen, el #espíritu maligno de ultratumba# porta #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD] = HintText(CustomMessage("They say that a #spider on a small island# in the Forest Temple holds #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß eine #Spinne auf einer kleinen Insel# im Waldtempel #[[1]]# besäße.",
                                                                              /*french*/ "Selon moi, une #Skulltula sur l'îlot du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula sobre una pequeña isla# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_FIRST_ROOM] = HintText(CustomMessage("They say that a #spider high on a wall of vines# in the Forest Temple holds #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß eine #Spinne hoch auf einer Wand aus Reben# im Waldtempel #[[1]]# besäße.",
                                                                 /*french*/ "Selon moi, une #Skulltula sur un mur de vignes du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #Skulltula en lo alto de una pared de cepas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD] = HintText(CustomMessage("They say that #stone columns# lead to a spider in the Forest Temple hiding #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #Säulen aus Stein# zu einer Spinne im Waldtempel führen, welche #[[1]]# verstecke.",
                                                                             /*french*/ "Selon moi, une #Skulltula haut perchée dans le jardin du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, unas #columnas del Templo del Bosque# conducen a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_LOBBY] = HintText(CustomMessage("They say that a #spider among ghosts# in the Forest Temple guards #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß eine #Spinne inmitten von Geistern# im Waldtempel #[[1]]# bewache.",
                                                            /*french*/ "Selon moi, une #Skulltula dans la grande salle du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula rodeada de fantasmas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_GS_BASEMENT] = HintText(CustomMessage("They say that a #spider within revolving walls# in the Forest Temple holds #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine #Spinne inmitten drehender Wände# im Waldtempel #[[1]]# besäße.",
                                                               /*french*/ "Selon moi, une #Skulltula derrière les murs pivotants du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #Skulltula entre paredes giratorias# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY] = HintText(CustomMessage("They say that an #ivy-hidden spider# in the Forest Temple hoards #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß eine #unter Efeu versteckte Spinne# im Waldtempel #[[1]]# horte.",
                                                                       /*french*/ "Selon moi, une #Skulltula près de l'entrée du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, una #Skulltula escondida entre cepas# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM] = HintText(CustomMessage("They say that a #spider in a hidden nook# within the Forest Temple holds #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß eine #Spinne in einem versteckten Winkel# im Waldtempel #[[1]]# besäße.",
                                                                         /*french*/ "Selon moi, une #Skulltula dans un recoin caché du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, una #Skulltula en una esquina oculta# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD] = HintText(CustomMessage("They say that a #spider on an arch# in the Forest Temple holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß eine #Spinne auf einem Bogen# im Waldtempel #[[1]]# besäße.",
                                                                                 /*french*/ "Selon moi, une #Skulltula sur une arche du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula sobre un arco# del Templo del Bosque otorga #[[1]]#.

    hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD] = HintText(CustomMessage("They say that a #spider on a ledge# in the Forest Temple holds #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß eine #Spinne auf einem Vorsprung# im Waldtempel #[[1]]# besäße.",
                                                                                /*french*/ "Selon moi, une #Skulltula dans le jardin du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en un borde# del Templo del Bosque otorga #[[1]]#.
  hintTextTable[RHT_FOREST_TEMPLE_MQ_GS_WELL] = HintText(CustomMessage("They say that #draining a well# in Forest Temple uncovers a spider with #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß das #Entleeren eines Brunnens# im Waldtempel eine Spinne mit #[[1]]# enthülle.",
                                                              /*french*/ "Selon moi, une #Skulltula au fond du Puits du Temple de la Forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, #vaciar el pozo# del Templo del Bosque desvela una Skulltula que otorga #[[1]]#.
  
  /*--------------------------
  |       FIRE TEMPLE        |
  ---------------------------*/
hintTextTable[RHT_FIRE_TEMPLE_NEAR_BOSS_CHEST] = HintText(CustomMessage("They say that #near a dragon# is #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #nahe eines Drachens# #[[1]]# sei.",
                                                                 /*french*/ "Selon moi, #près d'un dragon# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #cerca de un dragón# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_FLARE_DANCER_CHEST] = HintText(CustomMessage("They say that the #Flare Dancer behind a totem# guards #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß die #Flammenderwische hinter einem Totem# #[[1]]# bewachen würden.",
                                                                    /*french*/ "Selon moi, le #Danse-Flamme derrière un totem# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el #Bailafuego tras unos tótems# esconde #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a #prison beyond a totem# holds #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Gefängnis jenseits eines Totems# #[[1]]# enthielte.",
                                                                /*french*/ "Selon moi, la #prison derrière un totem# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, en una #prisión tras unos tótems# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText(CustomMessage("They say that #explosives over a lava pit# unveil #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß #Explosives über einem Lavastrom# #[[1]]# enthüllen würde.",
                                                                                  /*french*/ "Selon moi, des #explosifs dans un lac de lave# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, los #explosivos en un mar de llamas# revelan #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST] = HintText(CustomMessage("They say that a #Goron trapped near lava# holds #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß ein #nahe der Lava gefangene Gorone# #[[1]]# besäße.",
                                                                                     /*french*/ "Selon moi, le #Goron emprisonné près de la lave# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, un #goron atrapado cerca de un mar de llamas# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST] = HintText(CustomMessage("They say that a #Goron at the end of a maze# holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß ein #Gorone am Ende eines Labyrinths# #[[1]]# besäße.",
                                                                          /*french*/ "Selon moi, le #Goron dans le labyrinthe# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un #goron al final de un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST] = HintText(CustomMessage("They say that a #Goron above a maze# holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß ein #Gorone oberhalb eines Labyrinths# #[[1]]# besäße.",
                                                                          /*french*/ "Selon moi, le #Goron au dessus du labyrinthe# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un #goron sobre un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST] = HintText(CustomMessage("They say that a #Goron hidden near a maze# holds #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß ein #nahe eines Labyrinths versteckter Gorone# #[[1]]# besäße.",
                                                                              /*french*/ "Selon moi, le #Goron caché près du labyrinthe# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, un #goron escondido tras un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST] = HintText(CustomMessage("They say that a #blocked path# in Fire Temple holds #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß ein #blockierter Pfad# im Feuertempel #[[1]]# enthielte.",
                                                                             /*french*/ "Selon moi, un #sol fragile dans le Temple du Feu# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, en un #camino bloqueado# del Templo del Fuego yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MAP_CHEST] = HintText(CustomMessage("They say that a #caged chest# in the Fire Temple hoards #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #eingesperrte Truhe# im Feuertempel #[[1]]# enthielte.",
                                                           /*french*/ "Selon moi, un #coffre emprisonné# dans le Temple du Feu contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, un #cofre entre rejas# del Templo del Fuego contiene #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_COMPASS_CHEST] = HintText(CustomMessage("They say that a #chest in a fiery maze# contains #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine #Truhe in einem feurigen Labyrinth# #[[1]]# enthielte.",
                                                               /*french*/ "Selon moi, un #coffre dans un labyrinthe enflammé# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #cofre de un ardiente laberinto# contiene #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_HIGHEST_GORON_CHEST] = HintText(CustomMessage("They say that a #Goron atop the Fire Temple# holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß ein #Gorone auf der Spitze des Feuertempels# #[[1]]# besäße.",
                                                                     /*french*/ "Selon moi, le #Goron au sommet du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #goron en lo alto del Templo del Fuego# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST] = HintText(CustomMessage("They say that #near a dragon# is #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #nahe eines Drachens# #[[1]]# sei.",
                                                                    /*french*/ "Selon moi, #près d'un dragon# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #cerca de un dragón# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST] = HintText(CustomMessage("They say that the #Flare Dancer in the depths of the Fire Temple# guards #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß die #Flammenderwische in den Tiefen des Feuertempels# #[[1]]# bewachen würden.",
                                                                         /*french*/ "Selon moi, le #Danse-Flamme au coeur du volcan# a #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, el #Bailafuego en lo profundo del Templo del Fuego# esconde #[[1]]#.
                                                                      {}, {
                                                                      CustomMessage("They say that the #Flare Dancer in the depths of a volcano# guards #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß die #Flammenderwische in den Tiefen eines Vulkans# #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, le #Danse-Flamme au coeur du volcan# a #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, el #Bailafuego en lo profundo del volcán# esconde #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that a #blocked path# in Fire Temple holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein #blockierter Pfad# im Feuertempel #[[1]]# enthielte.",
                                                                  /*french*/ "Selon moi, le #chemin scellé# dans le Temple du Feu contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en un #camino bloqueado# del Templo del Fuego yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST] = HintText(CustomMessage("They say that #crates in a maze# contain #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #Kisten in einem Labyrinth# #[[1]]# enthielten.",
                                                                              /*french*/ "Selon moi, des #boîtes dans le labyrinthe# contiennent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #cajas de un laberinto# contienen #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST] = HintText(CustomMessage("They say that #crates in a maze# contain #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #Kisten in einem Labyrinth #[[1]]# enthielten.",
                                                                              /*french*/ "Selon moi, des #boîtes dans le labyrinthe# contiennent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #cajas de un laberinto# contienen #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST] = HintText(CustomMessage("They say that a #falling slug# in the Fire Temple guards #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß eine #fallende Schnecke# im Feuertempel #[[1]]# bewache.",
                                                                        /*french*/ "Selon moi, la #limace tombante# dans le Temple du Feu protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #babosa del techo# del Templo del Fuego guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that using a #hammer in the depths of the Fire Temple# reveals #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß die Benutzung eines #Hammers in den Tiefen des Feuertempels# #[[1]]# enthüllen würde.",
                                                              /*french*/ "Selon moi, frapper du #marteau au coeur du volcan# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, usar el #martillo en lo profundo del Templo del Fuego# revela #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that #illuminating a lava pit# reveals the path to #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß die #Illumination einer Lavagrube# den Pfad zu #[[1]]# enthülle.",
                                                                   /*french*/ "Selon moi, #éclairer le lac de lave# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, #iluminar un mar de llamas# revela #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText(CustomMessage("They say that #explosives over a lava pit# unveil #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß #Explosives oberhalb einer Lavagrube# #[[1]]# enthüllen würde.",
                                                                                     /*french*/ "Selon moi, des #explosifs dans un lac de lave# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, los #explosivos en un mar de llamas# revelan #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST] = HintText(CustomMessage("They say that a #Goron hidden near a maze# holds #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß ein #nahe eines Labyrinths versteckter Gorone# #[[1]]# besäße.",
                                                                                  /*french*/ "Selon moi, le #Goron caché près du labyrinthe# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, un #goron cerca de un laberinto# guarda #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_FREESTANDING_KEY] = HintText(CustomMessage("They say that hidden #beneath a block of stone# lies #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß versteckt #unter einem Steinblock# #[[1]]# läge.",
                                                                     /*french*/ "Selon moi, caché #derrière un bloc de pierre# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, #bajo unos bloques de piedra# yace #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_VOLVAGIA_HEART] = HintText(CustomMessage("They say that #Volvagia# holds #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #Volvagia# #[[1]]# besäße.",
                                                                /*french*/ "Selon moi, #Volcania# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, #Volvagia# porta #[[1]]#.
                                                             {}, {
                                                             CustomMessage("They say that the #Subterranean Lava Dragon# holds #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß der #subterrane Lavadrache# #[[1]]# besäße.",
                                                                 /*french*/ "Selon moi, le #dragon des profondeurs# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, el #dragón de lava subterráneo# porta #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM] = HintText(CustomMessage("They say that #eight tiles of malice# guard a spider holding #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß #acht Kacheln der Arglist# eine Spinne bewachen würden, welche #[[1]]# besäße.",
                                                                      /*french*/ "Selon moi, une #Skulltula protégée par huit tuiles dans le Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, #ocho baldosas de maldad# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_BOSS_KEY_LOOP] = HintText(CustomMessage("They say that #five tiles of malice# guard a spider holding #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #fünf Kacheln der Arglist# eine Spinne bewachen würden, welche #[[1]]# besäße.",
                                                                  /*french*/ "Selon moi, une #Skulltula protégée par cinq tuiles dans le Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, #cinco baldosas de maldad# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_BOULDER_MAZE] = HintText(CustomMessage("They say that #explosives in a maze# unveil a spider hiding #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Explosives in einem Labyrinth# eine Spinne enthüllen würde, welche #[[1]]# verstecke.",
                                                                 /*french*/ "Selon moi, une #Skulltula derrière un mur fragile du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, los #explosivos en un laberinto# desvelan una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SCARECROW_TOP] = HintText(CustomMessage("They say that a #spider-friendly scarecrow# atop the Fire Temple hides #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #spinnenfreundliche Vogelscheuche# auf der Spitze des Feuertempels #[[1]]# verstecke.",
                                                                  /*french*/ "Selon moi, une #Skulltula repérée par l'épouvantail du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga #[[1]]#.
                                                               {}, {
                                                               CustomMessage("They say that a #spider-friendly scarecrow# atop a volcano hides #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #spinnenfreundliche Vogelscheuche# auf der Spitze eines Vulkans #[[1]]# verstecke.",
                                                                   /*french*/ "Selon moi, une #Skulltula repérée par l'épouvantail du volcan# a #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_GS_SCARECROW_CLIMB] = HintText(CustomMessage("They say that a #spider-friendly scarecrow# atop the Fire Temple hides #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß eine #spinnenfreundliche Vogelscheuche# auf der der Spitze des Feuertempels #[[1]]# verstecke.",
                                                                    /*french*/ "Selon moi, une #Skulltula repérée par l'épouvantail du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                 // /*spanish*/ Según dicen, un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga #[[1]]#.
                                                                 {}, {
                                                                 CustomMessage("They say that a #spider-friendly scarecrow# atop a volcano hides #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine #spinnenfreundliche Vogelscheuche# auf der Spitze eines Vulkans #[[1]]# verstecke.",
                                                                     /*french*/ "Selon moi, une #Skulltula repérée par l'épouvantail du volcan# a #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                  // /*spanish*/ Según dicen, un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE] = HintText(CustomMessage("They say that a #spider above a fiery maze# holds #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß eine #Spinne oberhalb eines feurigen Labyrinths #[[1]]# besäße.",
                                                                            /*french*/ "Selon moi, une #Skulltula au dessus du labyrinthe enflammé du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, una #Skulltula sobre un ardiente laberinto# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER] = HintText(CustomMessage("They say that a #spider within a fiery maze# holds #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß eine #Spinne innerhalb eines feurigen Labyrinths# #[[1]]# besäße.",
                                                                             /*french*/ "Selon moi, une #Skulltula dans le labyrinthe enflammé du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula en el interior de un ardiente laberinto# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR] = HintText(CustomMessage("They say that a #Goron trapped near lava# befriended a spider with #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß ein #nahe der Lava gefangener Gorone# sich mit einer Spinne angefreundet hat, welche #[[1]]# besäße.",
                                                                               /*french*/ "Selon moi, une #Skulltula emprisonnée près du lac de lave du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #Skulltula amiga de un Goron atrapado junto a la lava# otorga #[[1]]#.

    hintTextTable[RHT_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM] = HintText(CustomMessage("They say that a #spider beside a fiery maze# holds #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß eine #Spinne neben einem feurigen Labyrinth# #[[1]]# besäße.",
                                                                                /*french*/ "Selon moi, une #Skulltula près du labyrinthe enflammé du Temple du Feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula junto a un ardiente laberinto# otorga #[[1]]#.

  /*--------------------------
  |       WATER TEMPLE       |
  ---------------------------*/
    hintTextTable[RHT_WATER_TEMPLE_MAP_CHEST] = HintText(CustomMessage("They say that #rolling spikes# in the Water Temple surround #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß #rollende Stacheln# im Wassertempel #[[1]]# umgeben würden.",
                                                            /*french*/ "Selon moi, des #Spikes# dans le Temple de l'Eau entourent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, unas #rodantes púas# del Templo del Agua guardan #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_COMPASS_CHEST] = HintText(CustomMessage("They say that #roaming stingers in the Water Temple# guard #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #umherstreifende Rochen im Wassertempel# #[[1]]# bewachen würden.",
                                                                /*french*/ "Selon moi, des #raies dans le Temple de l'Eau# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, unos #errantes stingers# del Templo del Agua guardan #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_TORCHES_CHEST] = HintText(CustomMessage("They say that #fire in the Water Temple# reveals #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #Feuer im Wassertempel# #[[1]]# enthüllen würde.",
                                                                /*french*/ "Selon moi, des #flammes dans le Temple de l'Eau# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, el #fuego en el Templo del Agua# revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_DRAGON_CHEST] = HintText(CustomMessage("They say that a #serpent's prize# in the Water Temple is #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Preis einer Schlange# im Wassertempel #[[1]]# sei.",
                                                               /*french*/ "Selon moi, la #récompense du dragon submergé# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el #escamado premio# del Templo del Agua se trata de #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST] = HintText(CustomMessage("They say that #blinding an eye# in the Water Temple leads to #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß das #Erblinden eines Auges# im Wassertempel zu #[[1]]# führe.",
                                                                           /*french*/ "Selon moi, #l'oeil# du Temple de l'Eau voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, #cegar un ojo# del Templo del Agua conduce a #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CENTRAL_PILLAR_CHEST] = HintText(CustomMessage("They say that in the #depths of the Water Temple# lies #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß in den #Tiefen des Wassertempels# #[[1]]# läge.",
                                                                       /*french*/ "Selon moi, le #coeur du Temple de l'Eau# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, en las #profundidades del Templo del Agua# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_CRACKED_WALL_CHEST] = HintText(CustomMessage("They say that #through a crack# in the Water Temple is #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #in einem Spalt# im Wassertempel #[[1]]# sei.",
                                                                     /*french*/ "Selon moi, le #mur fragile# du Temple de l'Eau cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, tras una #agrietada pared# del Templo del Agua yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_LONGSHOT_CHEST] = HintText(CustomMessage("They say that #Dark Link# guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß der #schwarze Link# #[[1]]# bewache.",
                                                                 /*french*/ "Selon moi, l'#Ombre de @# protège #[[1]]#.", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, #@ Oscuro# guarda #[[1]]#.
                                                              {}, {
                                                              CustomMessage("They say that #facing yourself# reveals #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß die #Konfrontation mit einem Selbst# #[[1]]# offenbare.",
                                                                  /*french*/ "Selon moi, se #vaincre soi-même# révèle #[[1]]#.", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #luchar contra ti mismo# revela #[[1]]#.
                                                              CustomMessage("They say that a #dark reflection# of yourself guards #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #dunkle Reflektion# von einem Selbst #[[1]]# bewache.",
                                                                  /*french*/ "Selon moi, son #propre reflet# cache #[[1]]#.", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #oscuro reflejo de ti mismo# guarda #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST] = HintText(CustomMessage("They say that in the #depths of the Water Temple# lies #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß in den #Tiefen des Wassertempels# #[[1]]# läge.",
                                                                          /*french*/ "Selon moi, le #coeur du Temple de l'Eau# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, en las #profundidades del Templo del Agua# yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that fire in the Water Temple unlocks a #vast gate# revealing a chest with #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß Feuer im Wassertempel ein #großes Tor# entschlüssele, welches eine Truhe mit #[[1]]# offenbare.",
                                                                    /*french*/ "Selon moi, des #flammes au coeur du Temple de l'Eau# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el fuego en el Templo del Agua alza una #gran valla# con #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_LONGSHOT_CHEST] = HintText(CustomMessage("They say that #through a crack# in the Water Temple is #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #in einem Spalt# im Wassertempel #[[1]]# sei.",
                                                                    /*french*/ "Selon moi, le #mur fragile# du Temple de l'Eau cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, tras una #agrietada pared# del Templo del Agua yace #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that #fire in the Water Temple# reveals #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß #Feuer im Wassertempel# #[[1]]# offenbare.",
                                                                   /*french*/ "Selon moi, des #flammes dans le Temple de l'Eau# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, el #fuego en el Templo del Agua# revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that #sparring soldiers# in the Water Temple guard #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß #sich duellierende Soldaten# im Wassertempel #[[1]]# bewachen würden.",
                                                               /*french*/ "Selon moi, les #soldats du Temple de l'Eau# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, #acabar con unos soldados# del Templo del Agua revela #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MORPHA_HEART] = HintText(CustomMessage("They say that #Morpha# holds #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß #Morpha# #[[1]]# besäße.",
                                                               /*french*/ "Selon moi, #Morpha# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, #Morpha# porta #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that the #Giant Aquatic Amoeba# holds #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß die #gigantische aquatische Amöbe# #[[1]]# besäße.",
                                                                /*french*/ "Selon moi, l'#amibe aquatique géante# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, la #ameba acuática gigante# porta #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM] = HintText(CustomMessage("They say that a #spider over a waterfall# in the Water Temple holds #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß eine #Spinne über einem Wasserfall# im Wassertempel #[[1]]# besäße.",
                                                                           /*french*/ "Selon moi, une #Skulltula au dessus d'une cascade du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula tras una cascada# del Templo del Agua otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_CENTRAL_PILLAR] = HintText(CustomMessage("They say that a #spider in the center of the Water Temple# holds #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß eine #Spinne im Zentrum des Wassertempels# #[[1]]# besäße.",
                                                                    /*french*/ "Selon moi, une #Skulltula au centre du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula en el centro del Templo del Agua# otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a spider protected by #rolling boulders in the Water Temple# hides #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß eine Spinne, welche von #rollenden Felsbrocken im Wassertempel# geschützt werde, #[[1]]# verstecke.",
                                                                         /*french*/ "Selon moi, une #Skulltula derrière les rochers roulants du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                      // /*spanish*/ Según dicen, una #Skulltula protegida por rocas rodantes# del Templo del Agua otorga #[[1]]#.
                                                                      {}, {
                                                                      CustomMessage("They say that a spider protected by #rolling boulders under the lake# hides #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß eine Spinne, welche von #rollenden Felsbrocken unterhalb eines Flusses# geschützt werde, #[[1]]# verstecke.",
                                                                          /*french*/ "Selon moi, une #Skulltula derrière les rochers roulants sous le lac# a #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                       // /*spanish*/ Según dicen, una #Skulltula protegida por rocas rodantes# bajo el lago otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_GS_RIVER] = HintText(CustomMessage("They say that a #spider over a river# in the Water Temple holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #Spinne über einem Fluß# im Wassertempel #[[1]]# besäße.",
                                                           /*french*/ "Selon moi, une #Skulltula au dessus de la rivière du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula sobre un río# del Templo del Agua otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH] = HintText(CustomMessage("They say that #beyond a pit of lizards# is a spider holding #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß #jenseits einer Reptiliengrube# eine Spinne sei, welche #[[1]]# besäße.",
                                                                                  /*french*/ "Selon moi, une #Skulltula près des lézards du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, #más allá de un pozo de reptiles# una Skulltula otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY] = HintText(CustomMessage("They say that #lizards guard a spider# in the Water Temple with #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß #eine von Reptilien bewachte Spinne# im Wassertempel #[[1]]# besäße.",
                                                                         /*french*/ "Selon moi, une #Skulltula dans les couloirs croisés du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, unos #reptiles custodian una Skulltula# del Templo del Agua que otorga #[[1]]#.

    hintTextTable[RHT_WATER_TEMPLE_MQ_GS_RIVER] = HintText(CustomMessage("They say that a #spider over a river# in the Water Temple holds #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß eine #Spinne oberhalb eines Flusses# im Wassertempel #[[1]]# besäße.",
                                                              /*french*/ "Selon moi, une #Skulltula au dessus de la rivière du Temple de l'Eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #Skulltula sobre un río# del Templo del Agua otorga #[[1]]#.

  /*--------------------------
  |      SPIRIT TEMPLE       |
  ---------------------------*/
 hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST] = HintText(CustomMessage("They say that a child conquers a #skull in green fire# in the Spirit Temple to reach #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß ein Kind einen #Schädel in grünem Feuer# im Geistertempel erobere, um #[[1]]# zu erreichen.",
                                                                      /*french*/ "Selon moi, le #crâne au halo vert dans le colosse# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, el joven que #baje el puente# del Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST] = HintText(CustomMessage("They say that a child can find a #caged chest# in the Spirit Temple with #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß ein Kind eine #gefangene Truhe# im Geistertempel finden könne, welche #[[1]]# enthielte.",
                                                                             /*french*/ "Selon moi, le #coffre embarré dans le colosse# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, un joven puede encontrar un #cofre entre rejas# del Templo del Espíritu con #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_COMPASS_CHEST] = HintText(CustomMessage("They say that #across a pit of sand# in the Spirit Temple lies #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #jenseits einer Sandgrube# im Geistertempel #[[1]]# läge.",
                                                                 /*french*/ "Selon moi, le #trou sableux dans le colosse# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, tras un #pozo de arena# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST] = HintText(CustomMessage("They say that #dodging boulders to collect silver rupees# in the Spirit Temple yields #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß das #Ausweichen von Felsbrocken um silberne Rubine zu sammeln# im Geistertempel #[[1]]# einbrächte.",
                                                                           /*french*/ "Selon moi, les #pièces argentées entourées de rochers dans le colosse# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, #esquivar rocas y conseguir plateadas rupias# en el Templo del Espíritu conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST] = HintText(CustomMessage("They say that a #shadow circling reflected light# in the Spirit Temple guards #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß ein #reflektierendes Licht umzirkelnder Schatten# im Geistertempel #[[1]]# bewachen würde.",
                                                                           /*french*/ "Selon moi, l'#ombre près d'un miroir# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un #círculo de reflectante luz# del Templo del Espíritu guarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST] = HintText(CustomMessage("They say that a #shadow circling reflected light# in the Spirit Temple guards #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß ein #reflektierendes Licht umzirkelnder Schatten# im Geistertempel #[[1]]# bewachen würde.",
                                                                            /*french*/ "Selon moi, l'#ombre près d'un miroir# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, un #círculo de reflectante luz# del Templo del Espíritu guarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MAP_CHEST] = HintText(CustomMessage("They say that #before a giant statue# in the Spirit Temple lies #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #vor einer riesigen Statue# im Geistertempel #[[1]]# läge.",
                                                             /*french*/ "Selon moi, #devant la statue# dans le colosse gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, #ante una gran estatua# del Templo del Espíritu aguarda #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST] = HintText(CustomMessage("They say that #lizards in the Spirit Temple# guard #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß #Reptilien im Geistertempel# #[[1]]# bewachen würden.",
                                                                           /*french*/ "Selon moi, les #lézards dans le colosse# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, los #reptiles del Templo del Espíritu# guardan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST] = HintText(CustomMessage("They say that #lizards in the Spirit Temple# guard #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #Reptilien im Geistertempel# #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, les #lézards dans le colosse# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #reptiles del Templo del Espíritu# guardan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST] = HintText(CustomMessage("They say that #torchlight among Beamos# in the Spirit Temple reveals #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #Fackellicht inmitten von Strahlzyklopen# #[[1]]# enthüllen würde.",
                                                                        /*french*/ "Selon moi, les #torches autour des Sentinelles# éclairent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, las #antorchas junto a Beamos# del Templo del Espíritu revelan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST] = HintText(CustomMessage("They say that a #statue in the Spirit Temple# holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß eine #Statue im Geistertempel# #[[1]]# hielte.",
                                                                          /*french*/ "Selon moi, la #statue dans le colosse# tient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #estatua del Templo del Espíritu# esconde #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST] = HintText(CustomMessage("They say that on a #ledge by a statue# in the Spirit Temple rests #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß auf einem #Vorsprung einer Statue# im Geistertempel #[[1]]# ruhe.",
                                                                               /*french*/ "Selon moi, #haut perché près de la statue# dans le colosse gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, al #borde de una estatua# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST] = HintText(CustomMessage("They say that those who #show the light among statues# in the Spirit Temple find #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß jene, welche #das Licht inmitten von Statuen# im Geistertempel zeigen würden, #[[1]]# fänden.",
                                                                         /*french*/ "Selon moi, le #soleil près des statues# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, aquellos que #iluminen ante las estatuas# del Templo del Espíritu encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth in the Spirit Temple# reveals #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit im Geistertempel# #[[1]]# enthüllen würde.",
                                                                                 /*french*/ "Selon moi, le #trésor invisible près du Hache-Viande# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth in the Spirit Temple# reveals #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit im Geistertempel# #[[1]]# enthüllen würde.",
                                                                                /*french*/ "Selon moi, le #trésor invisible près du Hache-Viande# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a #chest engulfed in flame# in the Spirit Temple holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #von Flammen eingehüllte Truhe# im Geistertempel #[[1]]# enthielte.",
                                                                  /*french*/ "Selon moi, le #coffre enflammé dans le colosse# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #cofre rodeado de llamas# del Templo del Espíritu contiene #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_TOPMOST_CHEST] = HintText(CustomMessage("They say that those who #show the light above the Colossus# find #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß jene, welche #das Licht auf dem Koloss# zeigen würden, #[[1]]# fänden.",
                                                                 /*french*/ "Selon moi, le #soleil au sommet du colosse# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, aquellos que #iluminen en lo alto del Coloso# encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST] = HintText(CustomMessage("They say that #lying unguarded# in the Spirit Temple is #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß #unbewacht liegend# im Geistertempel #[[1]]# sei.",
                                                                                /*french*/ "Selon moi, dans #l'entrée du colosse# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, en la #entrada del Templo del Espíritu# yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST] = HintText(CustomMessage("They say that a #switch in a pillar# within the Spirit Temple drops #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß ein #Schalter in einer Säule# innerhalb des Geistertempels #[[1]]# erbringe.",
                                                                                /*french*/ "Selon moi, l'#interrupteur dans un pilier# du colosse cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, el #interruptor de un pilar# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST] = HintText(CustomMessage("They say that #collecting rupees through a water jet# reveals #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß das #Sammeln von Rubin durch einen Wasserstrom# #[[1]]# enthüllen würde.",
                                                                                 /*french*/ "Selon moi, les #pièces argentées dans le jet d'eau# du colosse révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, #hacerte con rupias tras un géiser# revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST] = HintText(CustomMessage("They say that an #eye blinded by stone# within the Spirit Temple conceals #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß ein #durch einen Stein erblindetes Auge# im Geistertempel #[[1]]# verberge.",
                                                                               /*french*/ "Selon moi, #l'oeil derrière le rocher# dans le colosse voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #cegar a un ojo# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that surrounded by #fire and wrappings# lies #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß umgeben von #Feuer umhüllt# #[[1]]# läge.",
                                                                /*french*/ "Selon moi, près des #pierres tombales dans le colosse# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, rodeado de #fuego y vendas# yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST] = HintText(CustomMessage("They say that a child defeats a #gauntlet of monsters# within the Spirit Temple to find #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß ein Kind eine #Herausforderung von Monstern# innerhalb des Geistertempels bewältige und #[[1]]# fände.",
                                                                           /*french*/ "Selon moi, l'enfant qui vainc #plusieurs monstres# dans le colosse trouvera #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, el joven que derrote #unos monstruos# del Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST] = HintText(CustomMessage("They say that #explosive sunlight# within the Spirit Temple uncovers #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #explosives Sonnenlicht# innerhalb des Geistertempels #[[1]]# enthüllen würde.",
                                                                              /*french*/ "Selon moi, le #rayon de lumière explosif dans le colosse# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #explosiva luz solar# del Templo del Espíritu revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST] = HintText(CustomMessage("They say that #trapped by falling enemies# within the Spirit Temple is #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß sich #gefangen von fallenden Feinden# im Geistertempel #[[1]]# befände.",
                                                                              /*french*/ "Selon moi, des #ennemis tombants# dans le colosse protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #rodeado de enemigos del cielo# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that #blinding the colossus# unveils #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß das #Erblinden des Kolosses# #[[1]]# offenbare.",
                                                                    /*french*/ "Selon moi, #l'oeil dans le colosse# voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #cegar al coloso# revela #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST] = HintText(CustomMessage("They say that a #royal melody awakens the colossus# to reveal #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß eine #königliche Melodie den Koloss erwecke# und #[[1]]# enthüllen würde.",
                                                                                /*french*/ "Selon moi, la #mélodie royale éveille le colosse# et révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #melodía real que despierte al coloso# revelará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth# finds the colossus's hidden #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit# des Kolosses verborgene #[[1]]# fände.",
                                                                                  /*french*/ "Selon moi, #l'oeil de vérité# verra dans le colosse #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Templo del Espíritu encontrará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST] = HintText(CustomMessage("They say that #the old hide what the young find# to reveal #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß #der Alte verstecke, was der Junge finde# und #[[1]]# enthüllt würde.",
                                                                                 /*french*/ "Selon moi, l'#oeil dans le trou du bloc argent# dans le colosse voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #adulto esconde lo que el joven anhela#, revelando #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST] = HintText(CustomMessage("They say that #sunlight in a maze of fire# hides #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß #Sonnenlicht in einem Labyrinth aus Feuer# #[[1]]# verstecke.",
                                                                           /*french*/ "Selon moi, #la lumière dans le labyrinthe de feu# du colosse révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, la #luz solar de un ígneo laberinto# esconde #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST] = HintText(CustomMessage("They say that #across a pit of sand# in the Spirit Temple lies #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #jenseits einer Sandgrube# im Geistertempel #[[1]]# läge.",
                                                                        /*french*/ "Selon moi, le #trou sableux# dans le colosse a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #a través del pozo de arena# del Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST] = HintText(CustomMessage("They say that where #temporal stone blocks the path# within the Spirit Temple lies #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß wo #zeitlicher Stein den Pfad blockiere# im Geistertempel #[[1]]# läge.",
                                                                        /*french*/ "Selon moi, les #pierres temporelles# dans le colosse cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, donde los #bloques temporales bloquean# en el Templo del Espíritu yace #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST] = HintText(CustomMessage("They say that a #chest of double purpose# holds #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß eine #Truhe mit doppeltem Zweck# #[[1]]# enthielte.",
                                                                         /*french*/ "Selon moi, le #coffre à usage double# du colosse contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, un #cofre de doble uso# contiene #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that a #temporal stone blocks the light# leading to #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß ein #zeitlicher Stein das Licht blockiere#, was zu #[[1]]# führe.",
                                                                     /*french*/ "Selon moi, la #pierre temporelle# le colosse fait ombre sur #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #bloque temporal bloquea la luz# que conduce a #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST] = HintText(CustomMessage("They say that those who #show the light above the Colossus# find #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß jene, welche #das Licht auf dem Koloss# zeigen würden, #[[1]]# fänden.",
                                                                                    /*french*/ "Selon moi, le trésor invisible #au sommet du colosse# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, aquellos que #revelen la luz sobre el Coloso# encontrarán #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_TWINROVA_HEART] = HintText(CustomMessage("They say that #Twinrova# holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #Twinrova# #[[1]]# besäße.",
                                                                  /*french*/ "Selon moi, #Twinrova# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #Birova# porta #[[1]]#.
                                                               {}, {
                                                               CustomMessage("They say that the #Sorceress Sisters# hold #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß die #Hexenschwestern# #[[1]]# besäßen.",
                                                                   /*french*/ "Selon moi, #les sorcières jumelles# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, las #hermanas hechiceras# portan #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM] = HintText(CustomMessage("They say that a spider in the #hall of a knight# guards #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß eine Spinne in der #Halle eines Ritters# #[[1]]# bewache.",
                                                                                /*french*/ "Selon moi, une #Skulltula au dessus d'un escalier du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_BOULDER_ROOM] = HintText(CustomMessage("They say that a #spider behind a temporal stone# in the Spirit Temple yields #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #Spinne hinter einem zeitlichen Stein# im Geistertempel #[[1]]# einbrächte.",
                                                                   /*french*/ "Selon moi, une #Skulltula derrière une pierre temporelle du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula tras un bloque temporal# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_LOBBY] = HintText(CustomMessage("They say that a #spider beside a statue# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß eine #Spinne neben einer Statue# #[[1]]# besäße.",
                                                            /*french*/ "Selon moi, une #Skulltula dans la grande salle du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula junto a una estatua# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM] = HintText(CustomMessage("They say that a #spider at the top of a deep shaft# in the Spirit Temple holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß eine #Spinne auf der Spitze eines tiefen Stiels# im Geistertempel #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, une #Skulltula près d'un mur d'escalade du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula en lo alto de un gran hueco# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_GS_METAL_FENCE] = HintText(CustomMessage("They say that a child defeats a #spider among bats# in the Spirit Temple to gain #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein Kind #eine Spinne inmitten von Fledermäusen# im Geistertempel besiege und #[[1]]# erhielte.",
                                                                  /*french*/ "Selon moi, une #Skulltula sur le grillage du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el joven que derrote la #Skulltula entre murciélagos# del Templo del Espíritu hallará #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM] = HintText(CustomMessage("They say that #above a pit of sand# in the Spirit Temple hides #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß sich #oberhalb einer Sandgrube# im Geistertempel #[[1]]# verstecke.",
                                                                     /*french*/ "Selon moi, une #Skulltula au dessus du trou sableux du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula sobre un pozo de arena# del Templo del Espíritu otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST] = HintText(CustomMessage("They say that a spider in the #hall of a knight# guards #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß eine Spinne in der #Halle eines Ritters# #[[1]]# bewache.",
                                                                                /*french*/ "Selon moi, une #Skulltula dans la salle aux neuf trônes du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH] = HintText(CustomMessage("They say that a spider in the #hall of a knight# guards #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß eine Spinne in der #Halle eines Ritters# #[[1]]# bewache.",
                                                                                 /*french*/ "Selon moi, une #Skulltula dans la salle aux neuf trônes du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #Skulltula en el salón de un guerrero# otorga #[[1]]#.

    hintTextTable[RHT_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM] = HintText(CustomMessage("They say that #upon a web of glass# in the Spirit Temple sits a spider holding #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #auf einer Webe aus Glas# im Geistertempel eine Spinne säße, welche #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, une #Skulltula sur une paroi de verre du Temple de l'Esprit# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #sobre una plataforma de cristal# yace una Skulltula que otorga #[[1]]#.

  /*--------------------------
  |      SHADOW TEMPLE       |
  ---------------------------*/
hintTextTable[RHT_SHADOW_TEMPLE_MAP_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth# pierces a hall of faces to reveal #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit# eine Halle der Gesichter durchdränge und #[[1]]# offenbaren würde.",
                                                             /*french*/ "Selon moi, l'#oeil de vérité# voit dans les couloirs du Temple de l'Ombre #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, el #Ojo de la Verdad# descubrirá un pasillo de facetas con #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_HOVER_BOOTS_CHEST] = HintText(CustomMessage("They say that #Dead Hand in the Shadow Temple# holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine #tote Hand im Schattentempel# #[[1]]# hielte.",
                                                                     /*french*/ "Selon moi, le #Poigneur dans le Temple de l'Ombre# cache #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, la #Mano Muerta del Templo de las Sombras# guarda #[[1]]#.
                                                                  {}, {
                                                                  CustomMessage("They say that a #nether dweller in the Shadow Temple# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß ein #Bewohner der Unterwelt im Schattentempel# #[[1]]# besäße.",
                                                                      /*french*/ "Selon moi, le #spectre du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, un #temido morador del Templo de las Sombras# guarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_COMPASS_CHEST] = HintText(CustomMessage("They say that #mummies revealed by the Eye of Truth# guard #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #durch das Auge der Wahrheit offenbarte Mumien# #[[1]]# bewachen würden.",
                                                                 /*french*/ "Selon moi, les #Gibdos dans les couloirs# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, las #momias reveladas por el Ojo de la Verdad# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST] = HintText(CustomMessage("They say that #spinning scythes# protect #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß #rotierende Sensen# #[[1]]# schützen würden.",
                                                                            /*french*/ "Selon moi, les #faucheurs danseurs# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, las #giratorias guadañas# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText(CustomMessage("They say that #invisible blades# guard #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß #unsichtbare Klingen# #[[1]]# bewachen würden.",
                                                                                  /*french*/ "Selon moi, les #faucheurs invisibles# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, las #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText(CustomMessage("They say that #invisible blades# guard #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß #unsichtbare Klingen# #[[1]]# bewachen würden.",
                                                                                    /*french*/ "Selon moi, les #faucheurs invisibles# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, las #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #fallende Stacheln# den Pfad zu #[[1]]# blockieren würden.",
                                                                              /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #fallende Stacheln# den Pfad zu #[[1]]# blockieren würden.",
                                                                              /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß #fallende Stacheln# den Pfad zu #[[1]]# blockieren würden.",
                                                                               /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST] = HintText(CustomMessage("They say that the #dead roam among invisible spikes# guarding #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #herumschweifende Tote inmitten von unsichtbaren Stacheln# #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, #parmi les clous invisibles# du Temple de l'Ombre se cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #muertos que vagan por pinchos invisibles# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_WIND_HINT_CHEST] = HintText(CustomMessage("They say that an #invisible chest guarded by the dead# holds #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #von Toten bewachte unsichtbare Truhe# #[[1]]# enthielte.",
                                                                   /*french*/ "Selon moi, le #trésor invisible du cul-de-sac# du Temple de l'Ombre contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, un #cofre invisible custodiado por los del más allá# contiene #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST] = HintText(CustomMessage("They say that #mummies guarding a ferry# hide #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #eine Fähre bewachende Mumien# #[[1]]# verstecken würden.",
                                                                          /*french*/ "Selon moi, les #Gibdos qui bloquent le traversier# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST] = HintText(CustomMessage("They say that #mummies guarding a ferry# hide #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß #eine Fähre bewachende Mumien# #[[1]]# verstecken würden.",
                                                                           /*french*/ "Selon moi, les #Gibdos qui bloquent le traversier# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST] = HintText(CustomMessage("They say that #walls consumed by a ball of fire# reveal #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #von einem Feuerball verschlungende Wände# #[[1]]# offenbaren würden.",
                                                                          /*french*/ "Selon moi, le #piège de bois# du Temple de l'Ombre cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that #walls consumed by a ball of fire# reveal #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #von einem Feuerball verschlungende Wände# #[[1]]# offenbaren würden.",
                                                                  /*french*/ "Selon moi, le #piège de bois# du Temple de l'Ombre cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_FREESTANDING_KEY] = HintText(CustomMessage("They say that #inside a burning skull# lies #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #innerhalb eines brennenden Schädels# #[[1]]# läge.",
                                                                    /*french*/ "Selon moi, #dans un crâne enflammé# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, en el #interior de una calavera en llamas# aguarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth# pierces a hall of faces to reveal #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit# eine Halle der Gesichter durchdränge und #[[1]]# offenbaren würde.",
                                                                    /*french*/ "Selon moi, l'#oeil de vérité# voit dans les couloirs du Temple de l'Ombre #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, el #Ojo de la Verdad# descubre un pasillo de facetas con #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST] = HintText(CustomMessage("They say that #Dead Hand in the Shadow Temple# holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß eine #tote Hand im Schattentempel# #[[1]]# hielte.",
                                                                        /*french*/ "Selon moi, le #Poigneur dans le Temple de l'Ombre# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, la #Mano Muerta del Templo de las Sombras# guarda #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST] = HintText(CustomMessage("They say that #mummies revealed by the Eye of Truth# guard #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß #durch das Auge der Wahrheit offenbarte Mumien# #[[1]]# bewachen würden.",
                                                                         /*french*/ "Selon moi, les #Gibdos dans les couloirs# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, las #momias reveladas por el Ojo de la Verdad# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_MAP_CHEST] = HintText(CustomMessage("They say that #spinning scythes# protect #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #rotierende Sensen# #[[1]]# bewachen würden.",
                                                                /*french*/ "Selon moi, les #faucheurs danseurs# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, las #giratorias guadañas# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST] = HintText(CustomMessage("They say that #collecting rupees in a vast cavern# with the Shadow Temple unveils #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß das #Sammeln von Rubinen in einer riesigen Kaverne# im Schattentempel #[[1]]# offenbaren würde.",
                                                                                 /*french*/ "Selon moi, les #pièces argentées dans le Temple de l'Ombre# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, hacerte con las #rupias en una gran caverna# del Templo de las Sombras revela #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß #fallende Stachel# den Pfad zu #[[1]]# blockieren würden.",
                                                                                  /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß #fallende Stachel# den Pfad zu #[[1]]# blockieren würden.",
                                                                                 /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST] = HintText(CustomMessage("They say that #falling spikes# block the path to #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß #fallende Stachel# den Pfad zu #[[1]]# blockieren würden.",
                                                                                 /*french*/ "Selon moi, la #pluie de clous# surplombe #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, los #pinchos de un techo# conducen a #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST] = HintText(CustomMessage("They say that the #dead roam among invisible spikes# guarding #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #herumschweifende Tote inmitten von unsichtbaren Stacheln# #[[1]]# bewachen würden.",
                                                                             /*french*/ "Selon moi, #parmi les clous invisibles# du Temple de l'Ombre se cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, los #muertos que vagan por pinchos invisibles# protegen #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that #walls consumed by a ball of fire# reveal #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #von einem Feuerball verschlungende Wände# #[[1]]# offenbaren würden.",
                                                                     /*french*/ "Selon moi, le #piège de bois# du Temple de l'Ombre cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST] = HintText(CustomMessage("They say that #walls consumed by a ball of fire# reveal #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #von einem Feuerball verschlungende Wände# #[[1]]# offenbaren würden.",
                                                                             /*french*/ "Selon moi, le #piège de bois# du Temple de l'Ombre cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las #paredes consumidas por una esfera ígnea# revelan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST] = HintText(CustomMessage("They say that near an #empty pedestal# within the Shadow Temple lies #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß nahe einem #leeren Sockel# im Schattentempel #[[1]]# läge.",
                                                                         /*french*/ "Selon moi, #près d'un pédestal vide du Temple de l'Ombre# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, cerca de un #vacío pedestal# del Templo de las Sombras yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText(CustomMessage("They say that #invisible blades# guard #[[1]]#.",
                                                                                       /*german*/ "Man erzählt sich, daß #unsichtbare Klingen# #[[1]]# bewachen würden.",
                                                                                       /*french*/ "Selon moi, les #faucheurs invisibles# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, unas #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText(CustomMessage("They say that #invisible blades# guard #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß #unsichtbare Klingen# #[[1]]# bewachen würden.",
                                                                                     /*french*/ "Selon moi, les #faucheurs invisibles# du Temple de l'Ombre protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, unas #hojas invisibles# guardan #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST] = HintText(CustomMessage("They say that an #invisible chest guarded by the dead# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß eine #von Toten bewachte unsichtbare Truhe# #[[1]]# enthielte.",
                                                                      /*french*/ "Selon moi, le #trésor invisible du cul-de-sac# du Temple de l'Ombre contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #cofre invisible custodiado por los del más allá# contiene #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST] = HintText(CustomMessage("They say that #mummies guarding a ferry# hide #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #eine Fähre bewachende Mumien# #[[1]]# verstecken würden.",
                                                                              /*french*/ "Selon moi, les #Gibdos qui bloquent le traversier# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST] = HintText(CustomMessage("They say that #mummies guarding a ferry# hide #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #eine Fähre bewachende Mumien# #[[1]]# verstecken würden.",
                                                                             /*french*/ "Selon moi, les #Gibdos qui bloquent le traversier# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las #momias que protegen un navío# esconden #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST] = HintText(CustomMessage("They say that #caged near a ship# lies #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß #in der Nähe eines Schiffes eingesperrt# #[[1]]# läge.",
                                                                                /*french*/ "Selon moi, #dans une cage près du traversier# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, #entre rejas al lado de un navío# yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_FREESTANDING_KEY] = HintText(CustomMessage("They say that #behind three burning skulls# lies #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß #hinter drei brennenden Schädeln# #[[1]]# läge.",
                                                                       /*french*/ "Selon moi, #derrière trois crânes enflammés# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, tras #tres ardientes calaveras# yace #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_BONGO_BONGO_HEART] = HintText(CustomMessage("They say that #Bongo Bongo# holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß #Bongo Bongo# #[[1]]# besäße.",
                                                                     /*french*/ "Selon moi, #Bongo Bongo# possède #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                  // /*spanish*/ Según dicen, #Bongo Bongo# porta #[[1]]#.
                                                                  {}, {
                                                                  CustomMessage("They say that the #Phantom Shadow Beast# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß das #Phantomschattenbiest# #[[1]]# besäße.",
                                                                      /*french*/ "Selon moi, le #monstre de l'ombre# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                   // /*spanish*/ Según dicen, la #alimaña oscura espectral# porta #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT] = HintText(CustomMessage("They say that #beyond a burning skull# lies a spider with #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß #jenseits eines brennenden Schädels# eine Spinne läge, welche #[[1]]# besäße.",
                                                                       /*french*/ "Selon moi, une #Skulltula derrière un crâne enflammé du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #tras una ardiente calavera# yace una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM] = HintText(CustomMessage("They say that a #spider beyond falling spikes# holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß eine #Spinne jenseits fallender Stacheln# #[[1]]# besäße.",
                                                                          /*french*/ "Selon moi, une #Skulltula au delà de la pluie de clous du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula tras los pinchos del techo# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT] = HintText(CustomMessage("They say that #beyond three burning skulls# lies a spider with #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß #jenseits drei brennender Schädel# eine Spinne läge, welche #[[1]]# besäße.",
                                                                       /*french*/ "Selon moi, une #Skulltula derrière trois crânes enflammés du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, #tras tres ardientes calaveras# yace una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM] = HintText(CustomMessage("They say that a spider guarded by #invisible blades# holds #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine Spinne von #unsichtbaren Klingen# bewacht werde und #[[1]]# besäße.",
                                                                     /*french*/ "Selon moi, une #Skulltula protégée par les faucheurs invisibles du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #Skulltula custodiada por hojas invisibles# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_GS_NEAR_SHIP] = HintText(CustomMessage("They say that a spider near a #docked ship# hoards #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß eine Spinne nahe eines #geankerten Schiffs# #[[1]]# horte.",
                                                                /*french*/ "Selon moi, une #Skulltula près du traversier du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula cercana a un navío# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM] = HintText(CustomMessage("They say that a #spider beyond falling spikes# holds #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß eine #Spinne jenseits fallender Stacheln# #[[1]]# besäße.",
                                                                             /*french*/ "Selon moi, une #Skulltula au delà de la pluie de clous du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, una #Skulltula tras los pinchos del techo# otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM] = HintText(CustomMessage("They say that a #spider amidst roaring winds# in the Shadow Temple holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß eine #Spinne inmitten stürmischer Winde# im Schattentempel #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, une #Skulltula près des vents du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, una #Skulltula entre ventarrones# del Templo de las Sombras otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_AFTER_WIND] = HintText(CustomMessage("They say that a #spider beneath gruesome debris# in the Shadow Temple hides #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß eine #Spinne unterhalb grauenvoller Trümmer# im Schattentempel #[[1]]# verstecke.",
                                                                    /*french*/ "Selon moi, une #Skulltula sous des débris du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula bajo unos horripilantes escombros# del Templo de las Sombras otorga #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP] = HintText(CustomMessage("They say that a #fallen statue# reveals a spider with #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß eine #fallende Statue# eine Spinne enthülle, welche #[[1]]# besäße.",
                                                                    /*french*/ "Selon moi, une #Skulltula près de la statue écroulée du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #estatua caída# revelará una Skulltula que otorgue #[[1]]#.

    hintTextTable[RHT_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS] = HintText(CustomMessage("They say that a #suspended spider# guards #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #hängende Spinne# #[[1]]# bewache.",
                                                                   /*french*/ "Selon moi, une #Skulltula près du repère du Temple de l'Ombre# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula flotante# del Templo de las Sombras otorga #[[1]]#.

  /*--------------------------
  |    BOTTOM OF THE WELL    |
  ---------------------------*/
    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth in the well# reveals #[[1]]#.",
                                                                                   /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit im Brunnen# #[[1]]# offenbare.",
                                                                                   /*french*/ "Selon moi, l'#oeil de vérité dans le Puits# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, el #Ojo de la Verdad en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST] = HintText(CustomMessage("They say that #gruesome debris# in the well hides #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß #grauenvolle Trümmer# im Brunnen #[[1]]# verbergen würden.",
                                                                                    /*french*/ "Selon moi, des #débris dans le Puits# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, unos #horripilantes escombros# del pozo esconden #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth in the well# reveals #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit im Brunnen# #[[1]]# offenbare.",
                                                                                     /*french*/ "Selon moi, l'#oeil de vérité dans le Puits# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el #Ojo de la Verdad en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_COMPASS_CHEST] = HintText(CustomMessage("They say that a #hidden entrance to a cage# in the well leads to #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß ein #verborgener Eingang zu einem Käfig# im Brunnen zu #[[1]]# führe.",
                                                                      /*french*/ "Selon moi, dans un #chemin caché dans le Puits# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, la #entrada oculta de una celda# del pozo conduce a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST] = HintText(CustomMessage("They say that a #spider guarding a cage# in the well protects #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß eine #einen Käfig schützende Spinne# im Brunnen #[[1]]# schütze.",
                                                                               /*french*/ "Selon moi, l'#araignée dans la cage du Puits# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #araña protegiendo una celda# del pozo guarda #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST] = HintText(CustomMessage("They say that #gruesome debris# in the well hides #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß #grauenvolle Trümmer# im Brunnen #[[1]]# verbergen würde.",
                                                                                 /*french*/ "Selon moi, des #débris dans le Puits# cachent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, unos #horripilantes escombros# del pozo esconden #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST] = HintText(CustomMessage("They say that #Dead Hand's invisible secret# is #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß das #unsichtbare Geheimnis der toten Hand# #[[1]]# sei.",
                                                                        /*french*/ "Selon moi, le #trésor invisible du Poigneur# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, el #secreto invisible de la Mano Muerta# esconde #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST] = HintText(CustomMessage("They say that a #royal melody in the well# uncovers #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß eine #königliche Melodie im Brunnen# #[[1]]# enthüllen würde.",
                                                                               /*french*/ "Selon moi, la #mélodie royale révèle dans le Puits# #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST] = HintText(CustomMessage("They say that a #royal melody in the well# uncovers #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß eine #königliche Melodie im Brunnen# #[[1]]# enthüllen würde.",
                                                                              /*french*/ "Selon moi, la #mélodie royale révèle dans le Puits# #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MAP_CHEST] = HintText(CustomMessage("They say that in the #depths of the well# lies #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß in den #Tiefen des Brunnens# #[[1]]# läge.",
                                                                  /*french*/ "Selon moi, #dans le coeur du Puits# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, en las #profundidades del pozo# yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST] = HintText(CustomMessage("They say that #perilous pits# in the well guard the path to #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß #gefährliche Gruben# im Brunnen den Pfad zu #[[1]]# bewachen würden.",
                                                                         /*french*/ "Selon moi, #trois trous# dans le Puits protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, #peligrosos fosos# del pozo conducen a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST] = HintText(CustomMessage("They say that #locked in a cage# in the well lies #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #in einem Käfig eingeschlossen# im Brunnen #[[1]]# läge.",
                                                                        /*french*/ "Selon moi, #dans une cage# du Puits gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #entre rejas# en el pozo yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_FREESTANDING_KEY] = HintText(CustomMessage("They say that #inside a coffin# hides #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß #in einem Sarg# #[[1]]# verborgen läge.",
                                                                         /*french*/ "Selon moi, dans #un cercueil# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, en el #interior de un ataúd# yace #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST] = HintText(CustomMessage("They say that a #royal melody in the well# uncovers #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß eine #königliche Melodie im Brunnen# #[[1]]# enthüllen würde.",
                                                                     /*french*/ "Selon moi, la #mélodie royale révèle dans le Puits# #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, una #melodía real en el pozo# revela #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST] = HintText(CustomMessage("They say that an #army of the dead# in the well guards #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß eine #Armee der Toten# im Brunnen #[[1]]# bewachen würde.",
                                                                               /*french*/ "Selon moi, l'#armée des morts# dans le Puits protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, un #ejército del más allá# del pozo guarda #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY] = HintText(CustomMessage("They say that #Dead Hand's explosive secret# is #[[1]]#.",
                                                                                      /*german*/ "Man erzählt sich, daß das #explosive Geheimnis der toten Hand# #[[1]]# sei.",
                                                                                      /*french*/ "Selon moi, le #secret explosif du Poigneur# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, el #explosivo secreto de la Mano Muerta# esconde #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY] = HintText(CustomMessage("They say that an #invisible path in the well# leads to #[[1]]#.",
                                                                                            /*german*/ "Man erzählt sich, daß ein #unsichtbarer Pfad im Brunnen# zu #[[1]]# führe.",
                                                                                            /*french*/ "Selon moi, dans un #chemin caché dans le Puits# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                         // /*spanish*/ Según dicen, un #camino invisible del pozo# conduce a #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE] = HintText(CustomMessage("They say that a #spider locked in a cage# in the well holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß eine #in einem Käfig eingeschlossene Spinne# im Brunnen #[[1]]# besäße.",
                                                                          /*french*/ "Selon moi, une #Skulltula dans une cage au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula enjaulada# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM] = HintText(CustomMessage("They say that an #invisible path in the well# leads to #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß ein #unsichtbarer Pfad im Brunnen# zu #[[1]]# führe.",
                                                                           /*french*/ "Selon moi, une #Skulltula dans le chemin invisible au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un #camino invisible del pozo# conduce a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM] = HintText(CustomMessage("They say that a #spider locked in a crypt# within the well guards #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß eine #in einer Krypta eingeschlossene Spinne# im Brunnen #[[1]]# bewache.",
                                                                           /*french*/ "Selon moi, une #Skulltula embarrée dans la crypte au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, una #Skulltula encerrada en una cripta# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT] = HintText(CustomMessage("They say that a #gauntlet of invisible spiders# protects #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß eine #Herausforderung unsichtbarer Spinnen# #[[1]]# bewache.",
                                                                       /*french*/ "Selon moi, une #Skulltula protégée par les araignées invisibles au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, unas #arañas invisibles# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM] = HintText(CustomMessage("They say that a #spider crawling near the dead# in the well holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß eine #nahe der Toten kriechende Spinne# im Brunnen #[[1]]# besäße.",
                                                                          /*french*/ "Selon moi, une #Skulltula près des cercueils au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, una #Skulltula junto a los muertos# del pozo otorga #[[1]]#.

    hintTextTable[RHT_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM] = HintText(CustomMessage("They say that a #spider locked in a crypt# within the well guards #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß eine #in einer Krypta eingeschlossene Spinne# im Brunnen #[[1]]# bewache.",
                                                                              /*french*/ "Selon moi, une #Skulltula embarrée dans la crypte au fonds du Puits# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, una #Skulltula encerrada en una cripta# del pozo otorga #[[1]]#.

  /*--------------------------
  |        ICE CAVERN        |
  ---------------------------*/
    hintTextTable[RHT_ICE_CAVERN_MAP_CHEST] = HintText(CustomMessage("They say that #winds of ice# surround #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß #Eiswinde# #[[1]]# umgeben würden.",
                                                          /*french*/ "Selon moi, #figé dans la glace rouge# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_COMPASS_CHEST] = HintText(CustomMessage("They say that a #wall of ice# protects #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß eine #Eiswand# #[[1]]# schütze.",
                                                              /*french*/ "Selon moi, #un mur de glace rouge# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_IRON_BOOTS_CHEST] = HintText(CustomMessage("They say that a #monster in a frozen cavern# guards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Monster in einer gefrorenen Kaverne# #[[1]]# bewache.",
                                                                 /*french*/ "Selon moi, le #monstre de la caverne de glace# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #monstruo de una helada caverna# guarda #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_FREESTANDING_POH] = HintText(CustomMessage("They say that a #wall of ice# protects #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß eine #Eiswand# #[[1]]# schütze.",
                                                                 /*french*/ "Selon moi, un #mur de glace rouge# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_IRON_BOOTS_CHEST] = HintText(CustomMessage("They say that a #monster in a frozen cavern# guards #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß ein #Monster in einer gefrorenen Kaverne# #[[1]]# bewache.",
                                                                    /*french*/ "Selon moi, le #monstre de la caverne de glace# protège #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, un #monstruo de una helada caverna# guarda #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_COMPASS_CHEST] = HintText(CustomMessage("They say that #winds of ice# surround #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #Eiswinde# #[[1]]# umgeben würden.",
                                                                 /*french*/ "Selon moi, #entouré de vent glacial# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_MAP_CHEST] = HintText(CustomMessage("They say that a #wall of ice# protects #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß eine #Eiswand# #[[1]]# schütze.",
                                                             /*french*/ "Selon moi, #un mur de glace rouge# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #gélida pared# protege #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_FREESTANDING_POH] = HintText(CustomMessage("They say that #winds of ice# surround #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #Eiswinde# #[[1]]# umgeben würden.",
                                                                    /*french*/ "Selon moi, #entouré de vent glacial# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #heladas borrascas# rodean #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_PUSH_BLOCK_ROOM] = HintText(CustomMessage("They say that a #spider above icy pits# holds #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #Spinne oberhalb eisiger Gruben# #[[1]]# besäße.",
                                                                   /*french*/ "Selon moi, une #Skulltula au dessus d'un goufre glacial# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula sobre gélidos vacíos# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM] = HintText(CustomMessage("They say that #spinning ice# guards a spider holding #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #rotierendes Eis# eine Spinne beschütze, welche #[[1]]# besäße.",
                                                                        /*french*/ "Selon moi, une #Skulltula près de deux lames de glace# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, unos #témpanos giratorios# custodian una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_GS_HEART_PIECE_ROOM] = HintText(CustomMessage("They say that a #spider behind a wall of ice# hides #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß eine #Spinne hinter einer Eiswand# #[[1]]# verstecke.",
                                                                    /*french*/ "Selon moi, une #Skulltula derrière un mur de glace# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, una #Skulltula tras una gélida pared# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_SCARECROW] = HintText(CustomMessage("They say that a #spider above icy pits# holds #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß eine #Spinne oberhalb eisiger Gruben# #[[1]]# besäße.",
                                                                /*french*/ "Selon moi, une #Skulltula au dessus d'un goufre glacial# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #Skulltula sobre gélidos vacíos# otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_ICE_BLOCK] = HintText(CustomMessage("They say that a #web of ice# surrounds a spider with #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß eine #Webe aus Eis# eine Spinne umgebe, welche #[[1]]# besäße.",
                                                                /*french*/ "Selon moi, une #Skulltula protégée d'une toile glacée# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, una #gélida red# rodea a una Skulltula que otorga #[[1]]#.

    hintTextTable[RHT_ICE_CAVERN_MQ_GS_RED_ICE] = HintText(CustomMessage("They say that a #spider in fiery ice# hoards #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß eine #Spinne in feurigem Eis# #[[1]]# horte.",
                                                              /*french*/ "Selon moi, une #Skulltula figée dans la glace rouge# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #Skulltula tras un ardiente hielo# otorga #[[1]]#.

  /*--------------------------
  | GERUDO TRAINING GROUNDS  |
  ---------------------------*/
    hintTextTable[RHT_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST] = HintText(CustomMessage("They say that a #blinded eye in the Gerudo Training Grounds# drops #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß ein #erblindetes Auge in der Gerudo-Trainingsarena# #[[1]]# fallen ließe.",
                                                                             /*french*/ "Selon moi, l'#Oeil dans le Gymnase Gerudo# voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, #cegar un ojo en el Centro de Instrucción Gerudo# revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST] = HintText(CustomMessage("They say that a #blinded eye in the Gerudo Training Grounds# drops #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß ein #erblindetes Auge in der Gerudo-Trainingsarena# #[[1]]# fallen ließe.",
                                                                              /*french*/ "Selon moi, l'#Oeil dans le Gymnase Gerudo# voit #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #cegar un ojo en el Centro de Instrucción Gerudo# revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_STALFOS_CHEST] = HintText(CustomMessage("They say that #soldiers walking on shifting sands# in the Gerudo Training Grounds guard #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #auf veränderlichen Sanden laufende Soldaten# in der Gerudo-Trainingsarena #[[1]]# bewachen würden.",
                                                                          /*french*/ "Selon moi, les #squelettes# du Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, #soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_BEAMOS_CHEST] = HintText(CustomMessage("They say that #reptilian warriors# in the Gerudo Training Grounds protect #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß #reptilienartige Krieger# in der Gerudo-Trainingsarena #[[1]]# schützen würden.",
                                                                         /*french*/ "Selon moi, les #lézards# dans le Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, #unos escamosos guerreros# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth# in the Gerudo Training Grounds reveals #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit# in der Gerudo-Trainingsarena #[[1]]# enthülle.",
                                                                                 /*french*/ "Selon moi, #bien caché# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST] = HintText(CustomMessage("They say that the first prize of #the thieves' training# is #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß der erste Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                  /*french*/ "Selon moi, le #premier trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el primer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST] = HintText(CustomMessage("They say that the second prize of #the thieves' training# is #[[1]]#.",
                                                                                   /*german*/ "Man erzählt sich, daß der zweite Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                   /*french*/ "Selon moi, le #deuxième trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, el segundo premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST] = HintText(CustomMessage("They say that the third prize of #the thieves' training# is #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß der dritte Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                  /*french*/ "Selon moi, le #troisième trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, el tercer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST] = HintText(CustomMessage("They say that the #Song of Time# in the Gerudo Training Grounds leads to #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß die #Hymne der Zeit# in der Gerudo-Trainingsarena zu #[[1]]# führe.",
                                                                                     /*french*/ "Selon moi, le #chant du temps# révèle dans le Gymnase Gerudo #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST] = HintText(CustomMessage("They say that the #Song of Time# in the Gerudo Training Grounds leads to #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß die #Hymne der Zeit# in der Gerudo-Trainingsarena zu #[[1]]# führe.",
                                                                                  /*french*/ "Selon moi, le #chant du temps# révèle dans le Gymnase Gerudo #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST] = HintText(CustomMessage("They say that #fiery foes# in the Gerudo Training Grounds guard #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß #feurige Feinde# in der Gerudo-Trainingsarena #[[1]]# bewachen würden.",
                                                                                    /*french*/ "Selon moi, les #limaces de feu# du Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST] = HintText(CustomMessage("They say that #engulfed in flame# where thieves train lies #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß #von Flammen umschlungen# wo Diebe trainieren #[[1]]# läge.",
                                                                                     /*french*/ "Selon moi, le #trésor enflammé# du Gymnase Gerudo est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, donde entrenan las bandidas #entre llamas# yace #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST] = HintText(CustomMessage("They say that thieves #blind four faces# to find #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß Diebe #vier Gesichter erblinden# würden und #[[1]]# fänden.",
                                                                             /*french*/ "Selon moi, l'#épreuve d'archerie# du Gymnase Gerudo donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST] = HintText(CustomMessage("They say that thieves #blind four faces# to find #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß Diebe #vier Gesichter erblinden# würden und #[[1]]# fänden.",
                                                                                 /*french*/ "Selon moi, l'#épreuve d'archerie# du Gymnase Gerudo donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST] = HintText(CustomMessage("They say that #before a block of silver# thieves can find #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß #vor einem Block aus Silber# Diebe #[[1]]# finden könnten.",
                                                                                     /*french*/ "Selon moi, #près d'un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, #ante un plateado bloque# las bandidas hallan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST] = HintText(CustomMessage("They say that a #feat of strength# rewards thieves with #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß ein #Meisterstück der Stärke# Diebe mit #[[1]]# belohnen würde.",
                                                                                    /*french*/ "Selon moi, #derrière un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST] = HintText(CustomMessage("They say that a #feat of strength# rewards thieves with #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß ein #Meisterstück der Stärke# Diebe mit #[[1]]# belohnen würde.",
                                                                                     /*french*/ "Selon moi, #derrière un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST] = HintText(CustomMessage("They say that a #feat of strength# rewards thieves with #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß ein #Meisterstück der Stärke# Diebe mit #[[1]]# belohnen würde.",
                                                                                    /*french*/ "Selon moi, #derrière un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST] = HintText(CustomMessage("They say that a #feat of strength# rewards thieves with #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß ein #Meisterstück der Stärke# Diebe mit #[[1]]# belohnen würde.",
                                                                                     /*french*/ "Selon moi, #derrière un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_FREESTANDING_KEY] = HintText(CustomMessage("They say that the #Song of Time# in the Gerudo Training Grounds leads to #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß die #Hymne der Zeit# in der Gerudo-Trainingsarena zu #[[1]]# führe.",
                                                                             /*french*/ "Selon moi, le #chant du temps# révèle dans le Gymnase Gerudo #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST] = HintText(CustomMessage("They say that #thieves prepare for training# with #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß sich #Diebe auf das Training vorbereiteten# mit #[[1]]#.",
                                                                                 /*french*/ "Selon moi, dans #l'entrée du Gymnase Gerudo# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, las #bandidas se instruyen# con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST] = HintText(CustomMessage("They say that #thieves prepare for training# with #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß sich #Diebe auf das Training vorbereiteten# mit #[[1]]#.",
                                                                                /*french*/ "Selon moi, dans #l'entrée du Gymnase Gerudo# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, las #bandidas se instruyen# con #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST] = HintText(CustomMessage("They say that #soldiers walking on shifting sands# in the Gerudo Training Grounds guard #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß #auf veränderlichen Sanden laufende Soldaten# in der Gerudo-Trainingsarena #[[1]]# bewachen würden.",
                                                                                        /*french*/ "Selon moi, les #squelettes# du Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST] = HintText(CustomMessage("They say that #before a block of silver# thieves can find #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß #vor einem Block aus Silber# Diebe #[[1]]# finden könnten.",
                                                                                        /*french*/ "Selon moi, #près d'un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #ante un plateado bloque# las bandidas hallan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST] = HintText(CustomMessage("They say that thieves #blind four faces# to find #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß Diebe #vier Gesichter erblinden# würden und #[[1]]# fänden.",
                                                                                /*french*/ "Selon moi, l'#épreuve d'archerie# du Gymnase Gerudo donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, las bandidas #ciegan cuatro bustos# para hallar #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST] = HintText(CustomMessage("They say that #engulfed in flame# where thieves train lies #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß #von Flammen umschlungen# wo Diebe trainieren #[[1]]# läge.",
                                                                                  /*french*/ "Selon moi, le #trésor enflammé# du Gymnase Gerudo est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, donde entrenan las bandidas #entre llamas# yace #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST] = HintText(CustomMessage("They say that #fiery foes# in the Gerudo Training Grounds guard #[[1]]#.",
                                                                                         /*german*/ "Man erzählt sich, daß #feurige Feinde# in der Gerudo-Trainingsarena #[[1]]# bewachen würden.",
                                                                                         /*french*/ "Selon moi, les #ennemis de feu# du Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                      // /*spanish*/ Según dicen, unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST] = HintText(CustomMessage("They say that #reptilian warriors# in the Gerudo Training Grounds protect #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #reptilienartige Krieger# in der Gerudo-Trainingsarena #[[1]]# schützen würden.",
                                                                              /*french*/ "Selon moi, les #lézards# dans le Gymnase Gerudo protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, #unos escamosos guerreros# del Centro de Instrucción Gerudo protegen #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST] = HintText(CustomMessage("They say that a #path of fire# leads thieves to #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß der #Pfad des Feuers# Diebe zu #[[1]]# führe.",
                                                                                        /*french*/ "Selon moi, dans le #chemin enflammé# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, un #camino de fuego# conduce a las bandidas a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST] = HintText(CustomMessage("They say that the first prize of #the thieves' training# is #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß der erste Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                     /*french*/ "Selon moi, le #premier trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el primer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST] = HintText(CustomMessage("They say that a #path of fire# leads thieves to #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß der #Pfad des Feuers# Diebe zu #[[1]]# führe.",
                                                                                     /*french*/ "Selon moi, dans le #chemin enflammé# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, un #camino de fuego# conduce a las bandidas a #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST] = HintText(CustomMessage("They say that the third prize of #the thieves' training# is #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß der dritte Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                     /*french*/ "Selon moi, le #troisième trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, el tercer premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST] = HintText(CustomMessage("They say that the second prize of #the thieves' training# is #[[1]]#.",
                                                                                      /*german*/ "Man erzählt sich, daß der zweite Preis des #Diebestrainings# #[[1]]# sei.",
                                                                                      /*french*/ "Selon moi, le #deuxième trésor du Gymnase Gerudo# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                   // /*spanish*/ Según dicen, el segundo premio de la #instrucción bandida# se trata de #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST] = HintText(CustomMessage("They say that the #Eye of Truth# in the Gerudo Training Grounds reveals #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß das #Auge der Wahrheit# in der Gerudo-Trainingsarena #[[1]]# enthülle.",
                                                                                    /*french*/ "Selon moi, #bien caché# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela #[[1]]#.

    hintTextTable[RHT_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST] = HintText(CustomMessage("They say that a #feat of strength# rewards thieves with #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß ein #Meisterstück der Stärke# Diebe mit #[[1]]# belohnen würde.",
                                                                                 /*french*/ "Selon moi, #derrière un bloc argent# dans le Gymnase Gerudo gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, una #hazaña de fuerza# premia a las bandidas con #[[1]]#.

  /*--------------------------
  |      GANONS CASTLE       |
  ---------------------------*/
    hintTextTable[RHT_GANONS_TOWER_BOSS_KEY_CHEST] = HintText(CustomMessage("They say that the #Evil King# hoards #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß der #böse König# #[[1]]# horte.",
                                                                 /*french*/ "Selon moi, le #Roi du Mal# possède #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, el #Rey del Mal# acapara #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_FOREST_TRIAL_CHEST] = HintText(CustomMessage("They say that the #test of the wilds# holds #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß die #Prüfung der Wildnis# #[[1]]# enthielte.",
                                                                      /*french*/ "Selon moi, l'#épreuve des bois# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST] = HintText(CustomMessage("They say that the #test of the seas# holds #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß die #Prüfung der Meere# #[[1]]# enthielte.",
                                                                          /*french*/ "Selon moi, l'#épreuve des mers# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST] = HintText(CustomMessage("They say that the #test of the seas# holds #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß die #Prüfung der Meere# #[[1]]# enthielte.",
                                                                           /*french*/ "Selon moi, l'#épreuve des mers# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST] = HintText(CustomMessage("They say that #music in the test of darkness# unveils #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß #Musik in der Prüfung des Dunkelheit# #[[1]]# enthüllen würde.",
                                                                            /*french*/ "Selon moi, la #musique dans l'épreuve des ténèbres# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                         // /*spanish*/ Según dicen, la #música en la prueba de la oscuridad# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText(CustomMessage("They say that #light in the test of darkness# unveils #[[1]]#.",
                                                                                       /*german*/ "Man erzählt sich, daß #Licht in der Prüfung der Dunkelheit# #[[1]]# enthüllen würde.",
                                                                                       /*french*/ "Selon moi, la #lumière dans l'épreuve des ténèbres# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, la #luz en la prueba de la oscuridad# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST] = HintText(CustomMessage("They say that the #test of the sands# holds #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß die #Prüfung der Sande# #[[1]]# enthielte.",
                                                                                     /*french*/ "Selon moi, l'#épreuve des sables# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #prueba de las arenas# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText(CustomMessage("They say that the #test of the sands# holds #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß die #Prüfung der Sande# #[[1]]# enthielte.",
                                                                                /*french*/ "Selon moi, l'#épreuve des sables# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba de las arenas# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                 /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                 /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                  /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                  /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                               // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                 /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                 /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                              // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST] = HintText(CustomMessage("They say that the #test of radiance# holds #[[1]]#.",
                                                                                       /*german*/ "Man erzählt sich, daß die #Prüfung des Glanzes# #[[1]]# enthielte.",
                                                                                       /*french*/ "Selon moi, l'#épreuve du ciel# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, la #prueba del resplandor# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST] = HintText(CustomMessage("They say that #music in the test of radiance# reveals #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #Musik in der Prüfung des Glanzes# #[[1]]# enthüllen würde.",
                                                                             /*french*/ "Selon moi, la #musique dans l'épreuve du ciel# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, la #música en la prueba del resplandor# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST] = HintText(CustomMessage("They say that the #test of the seas# holds #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß die #Prüfung der Meere# #[[1]]# enthielte.",
                                                                        /*french*/ "Selon moi, l'#épreuve des mers# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, la #prueba del mar# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST] = HintText(CustomMessage("They say that the #test of the wilds# holds #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß die #Prüfung der Wildnis# #[[1]]# enthielte.",
                                                                                    /*french*/ "Selon moi, l'#épreuve des bois# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST] = HintText(CustomMessage("They say that the #test of the wilds# holds #[[1]]#.",
                                                                                           /*german*/ "Man erzählt sich, daß die #Prüfung der Wildnis# #[[1]]# enthielte.",
                                                                                           /*french*/ "Selon moi, l'#épreuve des bois# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                        // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST] = HintText(CustomMessage("They say that #music in the test of radiance# reveals #[[1]]#.",
                                                                                /*german*/ "Man erzählt sich, daß #Musik in der Prüfung des Glanzes# #[[1]]# enthüllen würde.",
                                                                                /*french*/ "Selon moi, la #musique dans l'épreuve du ciel# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                             // /*spanish*/ Según dicen, la #música en la prueba del resplandor# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST] = HintText(CustomMessage("They say that the #test of darkness# holds #[[1]]#.",
                                                                                     /*german*/ "Man erzählt sich, daß die #Prüfung der Dunkelheit# #[[1]]# enthielte.",
                                                                                     /*french*/ "Selon moi, l'#épreuve des ténèbres# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                  // /*spanish*/ Según dicen, la #prueba de la oscuridad# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST] = HintText(CustomMessage("They say that the #test of darkness# holds #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß die #Prüfung der Dunkelheit# #[[1]]# enthielte.",
                                                                                    /*french*/ "Selon moi, l'#épreuve des ténèbres# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la oscuridad# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                                          /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                                          /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                       // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                                        /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                                       /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                                       /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                    // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                                        /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                                        /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                     // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                               /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                               /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                            // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText(CustomMessage("They say that #reflected light in the test of the sands# reveals #[[1]]#.",
                                                                                   /*german*/ "Man erzählt sich, daß #reflektiertes Licht in der Prüfung der Sande# #[[1]]# enthüllen würde.",
                                                                                   /*french*/ "Selon moi, le #soleil dans l'épreuve des sables# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                // /*spanish*/ Según dicen, #reflejar la luz en la prueba de las arenas# revela #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY] = HintText(CustomMessage("They say that the #test of the wilds# holds #[[1]]#.",
                                                                                    /*german*/ "Man erzählt sich, daß die #Prüfung der Wildnis# #[[1]]# enthielte.",
                                                                                    /*french*/ "Selon moi, l'#épreuve des bois# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                                 // /*spanish*/ Según dicen, la #prueba de la naturaleza# brinda #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                          /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                           /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_RIGHT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                    /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_DEKU_SCRUB_LEFT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                   /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                       /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                             /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                             /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                          // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                        /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                              /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                              /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                           // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.

    hintTextTable[RHT_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT] = HintText(CustomMessage("They say that #scrubs in Ganon's Castle# sell #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß #Dekus in Ganons Schloß# #[[1]]# verkaufen würden.",
                                                                      /*french*/ "Selon moi, les #pestes Mojo dans le Château de Ganon# vendent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, los #dekus del Castillo de Ganon# venden #[[1]]#.
}
}
