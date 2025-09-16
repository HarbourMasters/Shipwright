#include "../../static_data.h"
#include "../hints.hpp"
#include "../../../custom-message/CustomMessageManager.h"

namespace Rando {
void StaticData::HintTable_Init_Exclude_Overworld() {
    // clang-format off

    hintTextTable[RHT_KF_KOKIRI_SWORD_CHEST] = HintText(CustomMessage("They say that the #hidden treasure of the Kokiri# is #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß der #versteckte Schatz der Kokiri# #[[1]]# |sei|seien|.",
                                                           /*french*/ "Selon moi, le #trésor des Kokiri# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #tesoro oculto de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_TOP_LEFT_CHEST] = HintText(CustomMessage("They say that #inside Mido's house# is #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #in Midos Haus# #[[1]]# |sei|seien|.",
                                                             /*french*/ "Selon moi, #dans la maison de Mido# gît #[[1]]#.", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                          {}, {
                                                          CustomMessage("They say that the #leader of the Kokiri# hides #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß der #Anführer der Kokiri# #[[1]]# verstecke.",
                                                              /*french*/ "Selon moi, le #chef des Kokiri# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_TOP_RIGHT_CHEST] = HintText(CustomMessage("They say that #inside Mido's house# is #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß #in Midos Haus# #[[1]]# |sei|seien|.",
                                                              /*french*/ "Selon moi, #dans la maison de Mido# gît #[[1]]#.", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                           {}, {
                                                           CustomMessage("They say that the #leader of the Kokiri# hides #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Anführer der Kokiri# #[[1]]# verstecke.",
                                                               /*french*/ "Selon moi, le #chef des Kokiri# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                            // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_BOTTOM_LEFT_CHEST] = HintText(CustomMessage("They say that #inside Mido's house# is #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #in Midos Haus# #[[1]]# |sei|seien|.",
                                                                /*french*/ "Selon moi, #dans la maison de Mido# gît #[[1]]#.", {QM_RED, QM_GREEN}),
                                                             // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                             {}, {
                                                             CustomMessage("They say that the #leader of the Kokiri# hides #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß der #Anführer der Kokiri# #[[1]]# verstecke.",
                                                                 /*french*/ "Selon moi, le #chef des Kokiri# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                              // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_KF_MIDOS_BOTTOM_RIGHT_CHEST] = HintText(CustomMessage("They say that #inside Mido's house# is #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß #in Midos Haus# #[[1]]# |sei|seien|.",
                                                                 /*french*/ "Selon moi, #dans la maison de Mido# gît #[[1]]#.", {QM_RED, QM_GREEN}),
                                                              // /*spanish*/ Según dicen, en la #casa de Mido# yace #[[1]]#.
                                                              {}, {
                                                              CustomMessage("They say that the #leader of the Kokiri# hides #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß der #Anführer der Kokiri# #[[1]]# verstecke.",
                                                                  /*french*/ "Selon moi, le #chef des Kokiri# possède #[[1]]#.", {QM_RED, QM_GREEN})});
                                                               // /*spanish*/ Según dicen, el #líder de los Kokiri# esconde #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_SHIELD_GRAVE_CHEST] = HintText(CustomMessage("They say that the #treasure of a fallen soldier# is #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß der #Schatz eines gefallenen Soldaten# #[[1]]# |sei|seien|.",
                                                                  /*french*/ "Selon moi, le #trésor du soldat mort# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el #tesoro de un soldado caído# esconde #[[1]]#.

    hintTextTable[RHT_DMT_CHEST] = HintText(CustomMessage("They say that hidden behind a wall on a #mountain trail# is #[[1]]#.",
                                               /*german*/ "Man erzählt sich, daß hinter einer Wand eines #Gebirgspfades# #[[1]]# |sei|seien|.",
                                               /*french*/ "Selon moi, derrière une façade du #chemin montagneux# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                            // /*spanish*/ Según dicen, tras una pared del #sendero de la montaña# yace #[[1]]#.

    hintTextTable[RHT_GC_MAZE_RIGHT_CHEST] = HintText(CustomMessage("They say that in #Goron City# explosives unlock #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß Explosionen in #Goronia# #[[1]]# freilegen würden.",
                                                         /*french*/ "Selon moi, des explosions dans le #village Goron# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, en la #Ciudad Goron# unos explosivos desbloquean #[[1]]#.

    hintTextTable[RHT_GC_MAZE_CENTER_CHEST] = HintText(CustomMessage("They say that in #Goron City# explosives unlock #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß Explosionen in #Goronia# #[[1]]# freilegen würden.",
                                                          /*french*/ "Selon moi, des explosions dans le #village Goron# révèlent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en la #Ciudad Goron# unos explosivos desbloquean #[[1]]#.

    hintTextTable[RHT_ZD_CHEST] = HintText(CustomMessage("They say that fire #beyond a waterfall# reveals #[[1]]#.",
                                              /*german*/ "Man erzählt sich, daß Feuer #hinter einem Wasserfall# #[[1]]# enthüllen würde.",
                                              /*french*/ "Selon moi, du feu #derrière la cascade# éclaire #[[1]]#.", {QM_RED, QM_GREEN}));
                                           // /*spanish*/ Según dicen, las #llamas tras una una cascada# revelan #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_HOOKSHOT_CHEST] = HintText(CustomMessage("They say that #dead Dampé's first prize# is #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß #Boris' erster Preis# #[[1]]# |sei|seien|.",
                                                              /*french*/ "Selon moi, la #première course d'Igor# donne #[[1]]#.", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, el primer premio de #la carrera de Dampé# se trata de #[[1]]#.
                                                           {}, {
                                                           CustomMessage("They say that a chest hidden by a #speedy spectre# holds #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine von einem #schnellen Gespenst# versteckte Truhe #[[1]]# enthielte.",
                                                               /*french*/ "Selon moi, le #coffre du rapide revenant# contient #[[1]]#.", {QM_RED, QM_GREEN})});
                                                            // /*spanish*/ Según dicen, un cofre custodiado por un #espectro veloz# contiene #[[1]]#.

    hintTextTable[RHT_GF_CHEST] = HintText(CustomMessage("They say that on a #rooftop in the desert# lies #[[1]]#.",
                                              /*german*/ "Man erzählt sich, daß auf einem #Dach in der Wüste# #[[1]]# |läge|lägen|.",
                                              /*french*/ "Selon moi, sur un #toit du désert# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                           // /*spanish*/ Según dicen, en una #azotea del desierto# yace #[[1]]#.

    hintTextTable[RHT_KAK_REDEAD_GROTTO_CHEST] = HintText(CustomMessage("They say that #zombies beneath the earth# guard #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #Untote unter der Erde# #[[1]]# bewachen würden.",
                                                             /*french*/ "Selon moi, les #revenants sous terre# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, unos #zombis subterráneos# esconden #[[1]]#.

    hintTextTable[RHT_SFM_WOLFOS_GROTTO_CHEST] = HintText(CustomMessage("They say that #wolves beneath the earth# guard #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #Wölfe unter der Erde# #[[1]]# bewachen würden.",
                                                             /*french*/ "Selon moi, les #loups sous terre# protègent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, unos #lobos subterráneos# esconden #[[1]]#.

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole in a field near a drawbridge# holds #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Erdloch nahe der Zugbrücke in einer Steppe# #[[1]]# enthielte.",
                                                                 /*french*/ "Selon moi, la #grotte près d'un pont# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, bajo el #hoyo de una llanura cercano a un puente# yace #[[1]]#.

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole in a field near a drawbridge# holds #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch nahe der Zugbrücke einer Steppe# #[[1]]# enthielte.",
                                                                /*french*/ "Selon moi, la #grotte près d'un pont# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, bajo el #hoyo de una llanura cercano a un puente# yace #[[1]]#.

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole amongst trees in a field# holds #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Erdloch inmitten von Bäumen einer Steppe# #[[1]]# enthielte.",
                                                               /*french*/ "Selon moi, la #grotte près des arbres# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, bajo el #hoyo de una llanura rodeado de árboles# yace #[[1]]#.

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole amongst trees in a field# holds #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch inmitten von Bäumen einer Steppe# #[[1]]# enthielte.",
                                                              /*french*/ "Selon moi, la #grotte près des arbres# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, bajo el #hoyo de una llanura rodeado de árboles# yace #[[1]]#.

    hintTextTable[RHT_HF_OPEN_GROTTO_CHEST] = HintText(CustomMessage("They say that an #open hole in a field# holds #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß ein #offenes Erdloch einer Steppe# #[[1]]# enthielte.",
                                                          /*french*/ "Selon moi, la #grotte dans les plaines# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo el #hoyo descubierto de una llanura# yace #[[1]]#.

    hintTextTable[RHT_HF_OPEN_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in an open hole in a field# holds #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein #Fisch in einem offenen Erdloch einer Steppe# #[[1]]# enthielte.",
                                                         /*french*/ "Selon moi, la #grotte dans les plaines# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, bajo el #hoyo descubierto de una llanura# yace #[[1]]#.

    hintTextTable[RHT_KAK_OPEN_GROTTO_CHEST] = HintText(CustomMessage("They say that an #open hole in a town# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß ein #offenes Erdloch eines Dorfes# #[[1]]# enthielte.",
                                                           /*french*/ "Selon moi, la #grotte dans le village# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo descubierto de un pueblo# yace #[[1]]#.

    hintTextTable[RHT_KAK_OPEN_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in an open hole in a town# holds #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß ein #Fisch in einem offenen Erdloch eines Dorfes# #[[1]]# enthielte.",
                                                          /*french*/ "Selon moi, la #grotte dans le village# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo el #hoyo descubierto de un pueblo# yace #[[1]]#.

    hintTextTable[RHT_ZR_OPEN_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole along a river# holds #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß ein #Erdloch nahe eines Flußes# #[[1]]# enthielte.",
                                                          /*french*/ "Selon moi, la #grotte près du fleuve# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, bajo un #hoyo junto a un río# yace #[[1]]#.

    hintTextTable[RHT_ZR_OPEN_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole along a river# holds #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch nahe eines Flußes# #[[1]]# enthielte.",
                                                         /*french*/ "Selon moi, la #grotte près du fleuve# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, bajo un #hoyo junto a un río# yace #[[1]]#.

    hintTextTable[RHT_KF_STORMS_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole in a forest village# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß ein #Erdloch eines Dorfes im Wald# #[[1]]# enthielte.",
                                                            /*french*/ "Selon moi, la #grotte inondée de pluie dans le Village Kokiri# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo el #hoyo de una tribu del bosque# yace #[[1]]#.

    hintTextTable[RHT_KF_STORMS_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole in a forest village# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch eines Dorfes im Wald# #[[1]]# enthielte.",
                                                           /*french*/ "Selon moi, la #grotte inondée de pluie dans le Village Kokiri# révèle #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo de una tribu del bosque# yace #[[1]]#.

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole in a wooded maze# holds #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß ein #Erdloch eines hölzernen Labyrinths# #[[1]]# enthielte.",
                                                                    /*french*/ "Selon moi, la #grotte dans le labyrinthe sylvestre# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, bajo un #hoyo de un laberinto forestal# yace #[[1]]#.

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole in a wooded maze# holds #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch eines hölzernen Labyrinths# #[[1]]# enthielte.",
                                                                   /*french*/ "Selon moi, la #grotte dans le labyrinthe sylvestre# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, bajo un #hoyo de un laberinto forestal# yace #[[1]]#.

    hintTextTable[RHT_DMT_STORMS_GROTTO_CHEST] = HintText(CustomMessage("They say that #hole flooded with rain on a mountain# holds #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #durch Regen geflutetes Erdloch eines Berges# #[[1]]# enthielte.",
                                                             /*french*/ "Selon moi, la #grotte inondée de pluie sur la montagne# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, bajo un #hoyo de una montaña inundado de lluvia# yace #[[1]]#.

    hintTextTable[RHT_DMT_STORMS_GROTTO_FISH] = HintText(CustomMessage("They say that #fish in a hole flooded with rain on a mountain# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß ein #Fisch in einem durch Regen geflutetem Erdloch eines Berges# #[[1]]# enthielte.",
                                                            /*french*/ "Selon moi, la #grotte inondée de pluie sur la montagne# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo un #hoyo de una montaña inundado de lluvia# yace #[[1]]#.

    hintTextTable[RHT_DMC_UPPER_GROTTO_CHEST] = HintText(CustomMessage("They say that a #hole in a volcano# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß ein #Erdloch eines Vulkans# #[[1]]# enthielte.",
                                                            /*french*/ "Selon moi, la #grotte dans le volcan# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, bajo el #hoyo de un volcán# yace #[[1]]#.

    hintTextTable[RHT_DMC_UPPER_GROTTO_FISH] = HintText(CustomMessage("They say that a #fish in a hole in a volcano# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß ein #Fisch in einem Erdloch eines Vulkans# #[[1]]# enthielte.",
                                                           /*french*/ "Selon moi, la #grotte dans le volcan# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, bajo el #hoyo de un volcán# yace #[[1]]#.

    hintTextTable[RHT_TOT_MASTER_SWORD] = HintText(CustomMessage("They say that a #pedestal in a temple# holds #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß sich auf einem #Podest eines Tempels# #[[1]]# |befände|befänden|.",
                                                      /*french*/ "Selon moi, un #piédestal dans un temple# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, un #pedestal en un templo# sostiene #[[1]]#.

    hintTextTable[RHT_TOT_LIGHT_ARROWS_CUTSCENE] = HintText(CustomMessage("They say that the #final gift of a princess# is #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß das #letzte Geschenk einer Prinzessin# #[[1]]# |sei|seien|.",
                                                               /*french*/ "Selon moi, le #cadeau d'adieu de la princesse# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el #obsequio final de la princesa# se trata de #[[1]]#.

    hintTextTable[RHT_LW_GIFT_FROM_SARIA] = HintText(CustomMessage("They say that #Saria's Gift# is #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß #Salias Geschenk# #[[1]]# |sei|seien|.",
                                                        /*french*/ "Selon moi, le #cadeau de Saria# est #[[1]]#.", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, el #regalo de Saria# se trata de #[[1]]#.
                                                     {}, {
                                                     CustomMessage("They say that a #potato hoarder# holds #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein #Kartoffelhorter# #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, le #panini mélodieux# est en fait #[[1]]#.", {QM_RED, QM_GREEN}),
                                                      // /*spanish*/ Según dicen, cierta #jovencita verde# concede #[[1]]#.
                                                     CustomMessage("They say that a rooty tooty #flutey cutey# gifts #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die #musikalische Kartoffel# #[[1]]# |sei|seien|.",
                                                         /*french*/ "Selon moi, la #patate musicale# est en fait #[[1]]#.", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, una #gran amiga# concede #[[1]]#.

    hintTextTable[RHT_ZF_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that the #fairy of winds# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß die #Fee des Sturmes# #[[1]]# besäße.",
                                                           /*french*/ "Selon moi, la #fée du vent# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #hada del viento# brinda #[[1]]#.

    hintTextTable[RHT_HC_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that the #fairy of fire# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß die #Fee des Feuers# #[[1]]# besäße.",
                                                           /*french*/ "Selon moi, la #fée du feu# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, el #hada del fuego# brinda #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that the #fairy of love# holds #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß die #Fee der Liebe# #[[1]]# besäße.",
                                                                 /*french*/ "Selon moi, la #fée de l'amour# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, el #hada del amor# brinda #[[1]]#.

    hintTextTable[RHT_DMT_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that a #magical fairy# gifts #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß die #Fee der Fähigkeiten# #[[1]]# schenke.",
                                                            /*french*/ "Selon moi, la #fée de la magie# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, un #hada mágica# brinda #[[1]]#.

    hintTextTable[RHT_DMC_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that a #magical fairy# gifts #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß die #Fee der Weisheit# #[[1]]# schenke.",
                                                            /*french*/ "Selon moi, la #fée de la magie# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, un #hada mágica# brinda #[[1]]#.

    hintTextTable[RHT_OGC_GREAT_FAIRY_REWARD] = HintText(CustomMessage("They say that the #fairy of strength# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß die #Fee des Mutes# #[[1]]# besäße.",
                                                            /*french*/ "Selon moi, la #fée de la force# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, el #hada de la fuerza# brinda #[[1]]#.

    hintTextTable[RHT_SONG_FROM_IMPA] = HintText(CustomMessage("They say that #deep in a castle#, Impa teaches #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß #tief in einem Schloß#, Impa #[[1]]# lehre.",
                                                    /*french*/ "Selon moi, #la nourrice de la princesse# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, en el #jardín del castillo Impa enseña# #[[1]]#.

    hintTextTable[RHT_SONG_FROM_MALON] = HintText(CustomMessage("They say that #a farm girl# sings #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß ein #Mädchen vom Lande# #[[1]]# singe.",
                                                     /*french*/ "Selon moi, la #fillette de la ferme# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #chica rupestre# canta #[[1]]#.

    hintTextTable[RHT_SONG_FROM_SARIA] = HintText(CustomMessage("They say that #deep in the forest#, Saria teaches #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß #tief im Wald#, Salia #[[1]]# lehre.",
                                                     /*french*/ "Selon moi, la #fille de la forêt# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, al #fondo del bosque# Saria enseña #[[1]]#.

    hintTextTable[RHT_SONG_FROM_WINDMILL] = HintText(CustomMessage("They say that a man #in a windmill# is obsessed with #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß ein besessener Mann #in einer Windmühle# #[[1]]# lehre.",
                                                        /*french*/ "Selon moi, l'#homme du moulin# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, el #hombre del molino# está obsesionado con #[[1]]#.

    hintTextTable[RHT_HC_MALON_EGG] = HintText(CustomMessage("They say that a #girl looking for her father# gives #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß ein #nach ihrem Vater suchendes Mädchen# #[[1]]# gäbe.",
                                                  /*french*/ "Selon moi, la #fillette qui cherche son père# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, una #chica en busca de su padre# otorga #[[1]]#.

    hintTextTable[RHT_HC_ZELDAS_LETTER] = HintText(CustomMessage("They say that a #princess in a castle# gifts #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß eine #Prinzessin eines Schloßes# #[[1]]# schenke.",
                                                      /*french*/ "Selon moi, la #princesse dans le château# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la #princesa de un castillo# otorga #[[1]]#.

    hintTextTable[RHT_ZD_DIVING_MINIGAME] = HintText(CustomMessage("They say that those who #dive for Zora rupees# will find #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß jene, welche nach den #Rubinen der Zora tauchen# #[[1]]# fänden.",
                                                        /*french*/ "Selon moi, ceux qui #plongent pour des rubis Zora# trouveront #[[1]]#.", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, aquellos que se #sumergan por las rupias zora# encontrarán #[[1]]#.
                                                     {}, {
                                                     CustomMessage("They say that an #unsustainable business model# gifts #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein #unwirtschaftliches Geschäftsmodell# #[[1]]# schenke.",
                                                         /*french*/ "Selon moi, le #mauvais modèle d'affaires# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, un #mal modelo de negocio# premia con #[[1]]#.

    hintTextTable[RHT_LH_CHILD_FISHING] = HintText(CustomMessage("They say that #fishing in youth# bestows #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß das #Fischen der Jugend# #[[1]]# verleihe.",
                                                      /*french*/ "Selon moi, #pêcher dans sa jeunesse# promet #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, #pescar en la juventud# conduce a #[[1]]#.

    hintTextTable[RHT_LH_POND_FISH] = HintText(CustomMessage("They say that #hitting the pond# reveals #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß das #Fischen im Teich# #[[1]]# enthülle.",
                                                  /*french*/ "Selon moi, #aller à l'étang# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_HYRULE_LOACH] = HintText(CustomMessage("They say that #fishing the hyrule loach# will give you #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß das #Fischen der hylianischen Forelle# #[[1]]# einbrächte.",
                                                     /*french*/ "Selon moi, #pêcher le brochet Hylien# vous donnera #[[1]]#.", {QM_RED, QM_GREEN}),
                                                  // /*spanish*/ Según dicen, si #pescas a la Locha de Hyrule# encontrarás #[[1]]#.
                                                  {}, {
                                                  CustomMessage("They say that #fishing the legend# bestows #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß das #Fischen der Legende# #[[1]]# verleihe.",
                                                      /*french*/ "Selon moi, #pêcher la légende# confère #[[1]]#.", {QM_RED, QM_GREEN})});
                                                   // /*spanish*/ Según dicen, #pescar a la leyenda# conduce a #[[1]]#.

    hintTextTable[RHT_LH_ADULT_FISHING] = HintText(CustomMessage("They say that #fishing in maturity# bestows #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß das #Fischen des Alters# #[[1]]# verleihe.",
                                                      /*french*/ "Selon moi, #pêcher dans sa maturité# promet #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, #pescar en la madurez# conduce a #[[1]]#.

    hintTextTable[RHT_LH_LAB_DIVE] = HintText(CustomMessage("They say that a #diving experiment# is rewarded with #[[1]]#.",
                                                 /*german*/ "Man erzählt sich, daß ein #Tauchexperiment# #[[1]]# einbrächte.",
                                                 /*french*/ "Selon moi, l'#expérience de plongée# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, #bucear para un experimento# se premia con #[[1]]#.

    hintTextTable[RHT_ZD_FISH] = HintText(CustomMessage("They say that a #fish by a waterfall# hoards #[[1]]#.",
                                             /*german*/ "Man erzählt sich, daß ein #Fisch nahe eines Wasserfalls# #[[1]]# horte.",
                                             /*french*/ "Selon moi, un #poisson près d'une cascade# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                          // /*spanish*/ Según dicen, un #pez junto a una cascada# otorga #[[1]]#.


    hintTextTable[RHT_GC_ROLLING_GORON_AS_ADULT] = HintText(CustomMessage("They say that #reassuring a young Goron# is rewarded with #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß das #Beruhigen eines jungen Goronen# #[[1]]# einbrächte.",
                                                               /*french*/ "Selon moi, #rassurer un jeune Goron# donne #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, #calmar a un joven Goron# otorga #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that #comforting yourself# provides #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß #sich selbst zu trösten# #[[1]]# einbrächte.",
                                                                /*french*/ "Selon moi, se #réconforter soi-même# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, #confrontarte a ti mismo# otorga #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE] = HintText(CustomMessage("They say that the #first explosive prize# is #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß der #erste explosive Preis# #[[1]]# |sei|seien|.",
                                                                        /*french*/ "Selon moi, le #premier prix explosif# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, el #primer premio explosivo# se trata de #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE] = HintText(CustomMessage("They say that the #second explosive prize# is #[[1]]#.",
                                                                         /*german*/ "Man erzählt sich, daß der #zweite explosive Preis# #[[1]]# |sei|seien|.",
                                                                         /*french*/ "Selon moi, le #deuxième prix explosif# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                      // /*spanish*/ Según dicen, el #segundo premio explosivo# se trata de #[[1]]#.

    hintTextTable[RHT_MARKET_LOST_DOG] = HintText(CustomMessage("They say that #rescuing Richard the Dog# is rewarded with #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß die #Rettung des Hundes Richard# #[[1]]# einbrächte.",
                                                     /*french*/ "Selon moi, #retrouver Kiki le chien# promet #[[1]]#.", {QM_RED, QM_GREEN}),
                                                  // /*spanish*/ Según dicen, #rescatar al perrito Ricardo# conduce a #[[1]]#.
                                                  {}, {
                                                  CustomMessage("They say that #puppy lovers# will find #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß #Welpenliebhaber# #[[1]]# fänden.",
                                                      /*french*/ "Selon moi, les #amoureux canins# trouveront #[[1]]#.", {QM_RED, QM_GREEN})});
                                                   // /*spanish*/ Según dicen, los #amantes caninos# encontrarán #[[1]]#.

    hintTextTable[RHT_LW_OCARINA_MEMORY_GAME] = HintText(CustomMessage("They say that #playing an Ocarina in Lost Woods# is rewarded with #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß das #Spielen der Okarina in den verlorenen Wäldern# #[[1]]# gewähre.",
                                                            /*french*/ "Selon moi, #jouer l'ocarina dans les Bois Perdus# donne #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, #tocar la ocarina en el Bosque Perdido# otorga #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that the prize for a #game of Simon Says# is #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß der Preis für eine Partie #Simon sagt# #[[1]]# |sei|seien|.",
                                                             /*french*/ "Selon moi, la #récompense de Jean Dit# est #[[1]]#.", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, #repetir ciertas melodías# otorga #[[1]]#.
                                                         CustomMessage("They say that a #child sing-a-long# holds #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß die #jungen Flötisten# #[[1]]# besäßen.",
                                                             /*french*/ "Selon moi, les #jeunes flûtistes# donnent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, #tocar junto a otros# otorga #[[1]]#.

    hintTextTable[RHT_KAK_10_GOLD_SKULLTULA_REWARD] = HintText(CustomMessage("They say that slaying #10 Gold Skulltulas# reveals #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß das Vernichten von #10 Goldenen Skulltulas# #[[1]]# enthülle.",
                                                                  /*french*/ "Selon moi, détruire #10 Skulltulas d'or# donne #[[1]]#.", {QM_RED, QM_GREEN}),
                                                               // /*spanish*/ Según dicen, #exterminar 10 skulltulas doradas# revela #[[1]]#.
                                                               {}, {
                                                               CustomMessage("They say that #10 bug badges# rewards #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß die Belohnung für #10 Insektenplaketten# #[[1]]# |sei|seien|.",
                                                                   /*french*/ "Selon moi, #10 écussons# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, #10 medallas de insectos# otorgan #[[1]]#.
                                                               CustomMessage("They say that #10 spider souls# yields #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß #10 Spinnenseelen# #[[1]]# einbrächten.",
                                                                   /*french*/ "Selon moi, #10 âmes# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                // /*spanish*/ Según dicen, #10 almas de araña# otorgan #[[1]]#.
                                                               CustomMessage("They say that #10 auriferous arachnids# lead to #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß #10 goldhaltige Arachniden# #[[1]]# gewähren.",
                                                                   /*french*/ "Selon moi, #10 arachnides aurifères# donnent #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                // /*spanish*/ Según dicen, #10 arácnidos auríferos# otorgan #[[1]]#.

    hintTextTable[RHT_KAK_MAN_ON_ROOF] = HintText(CustomMessage("They say that a #rooftop wanderer# holds #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß ein #Dachwanderer# #[[1]]# besäße.",
                                                     /*french*/ "Selon moi, une #rencontre sur un toit# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, #alguien sobre un tejado# otorga #[[1]]#.

    hintTextTable[RHT_ZR_MAGIC_BEAN_SALESMAN] = HintText(CustomMessage("They say that a #bean seller# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß ein #Erbsenverkäufer# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, le #marchand de haricots magiques# vend en fait #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, el #vendedor de judías# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a seller of #colorful crops# has #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein Verkäufer #bunter Ernte# #[[1]]# besäße.",
                                                             /*french*/ "Selon moi, le #marchand de légumes# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, el vendedor de un #colorido cultivo# ofrece #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_IN_THE_RAIN] = HintText(CustomMessage("They say that #frogs in a storm# gift #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß #Frösche im Sturm# #[[1]]# schenken würden.",
                                                          /*french*/ "Selon moi, #des grenouilles mouillées# donnent #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, las #ancas bajo la tormenta# otorgan #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_ZELDAS_LULLABY] = HintText(CustomMessage("They say that after hearing #Zelda's Lullaby, the frogs# gift #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß #Frösche die Zeldas Wiegenlied# hören #[[1]]# schenken würden.",
                                                             /*french*/ "Selon moi, à l'écoute de #la berceuse de Zelda, les grenouilles# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                          // /*spanish*/ Según dicen, después de escuchar #la Nana de Zelda, las ranas# regalan #[[1]]#.
                                                          {}, {
                                                          CustomMessage("They say that #sleepy frogs# gift #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß #schläfrige Frösche# #[[1]]# schenken würden.",
                                                              /*french*/ "Selon moi, #les grenouilles somnolentes# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                           // /*spanish*/ Según dicen, las #ranas somnolientas# regalan #[[1]]#.
                                                          CustomMessage("They say that #the Froggish Tenor in the back-left# gifts #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß #der froschige Tenor hinten links# #[[1]]# schenke.",
                                                              /*french*/ "Selon moi, #le ténor grenouillesque au fond à gauche# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                           // /*spanish*/ Según dicen, el #Sapo Tenore al fondo, a la izquierda#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_EPONAS_SONG] = HintText(CustomMessage("They say that after hearing #Epona's Song, the frogs# gift #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß #Frösche die Eponas Lied# hören #[[1]]# schenken würden.",
                                                          /*french*/ "Selon moi, à l'écoute du #chant d'Epona, les grenouilles# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                       // /*spanish*/ Según dicen, después de escuchar #la Canción de Epona, las ranas# regalan #[[1]]#.
                                                       {}, {
                                                       CustomMessage("They say that #equine frogs# gift #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß #pferdeartige Frösche# #[[1]]# schenken würden.",
                                                           /*french*/ "Selon moi, #les grenouilles équestres# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, las #ranas equinas# regalan #[[1]]#.
                                                       CustomMessage("They say that #the Froggish Tenor in the back-right# gifts #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß #der froschige Tenor hinten rechts# #[[1]]# schenke.",
                                                           /*french*/ "Selon moi, #le ténor grenouillesque au fond à droite# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                        // /*spanish*/ Según dicen, el #Sapo Tenore al fondo, a la derecha#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SARIAS_SONG] = HintText(CustomMessage("They say that after hearing #Saria's Song, the frogs# gift #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß #Frösche die Salias Lied# hören #[[1]]# schenken würden.",
                                                          /*french*/ "Selon moi, à l'écoute du #chant de Saria, les grenouilles# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                       // /*spanish*/ Según dicen, después de escuchar #la Canción de Saria, las ranas# regalan #[[1]]#.
                                                       {}, {
                                                       CustomMessage("They say that #sylvan frogs# gift #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß #waldige Frösche# #[[1]]# schenken würden.",
                                                           /*french*/ "Selon moi, #les grenouilles sylvestres# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, las #ranas silvestres# regalan #[[1]]#.
                                                       CustomMessage("They say that #the Froggish Tenor in the center# gifts #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß #der froschige Tenor im Zentrum# #[[1]]# schenke.",
                                                           /*french*/ "Selon moi, #le ténor grenouillesque dans le centre# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                        // /*spanish*/ Según dicen, el #Sapo Tenore en el centro# regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SUNS_SONG] = HintText(CustomMessage("They say that after hearing #the Sun's Song, the frogs# gift #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß #Frösche die die Hymne der Sonne# hören #[[1]]# schenken würden.",
                                                        /*french*/ "Selon moi, à l'écoute du #chant du soleil, les grenouilles# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                     // /*spanish*/ Según dicen, después de escuchar #la Canción del Sol, las ranas# regalan #[[1]]#.
                                                     {}, {
                                                     CustomMessage("They say that #enlightened frogs# gift #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß #erleuchtete Frösche# #[[1]]# schenken würden.",
                                                         /*french*/ "Selon moi, #les grenouilles éclairées# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                      // /*spanish*/ Según dicen, las #ranas alumbradas# regalan #[[1]]#.
                                                     CustomMessage("They say that #the Froggish Tenor in the front-left# gifts #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß #der froschige Tenor vorne links# #[[1]]# schenke.",
                                                         /*french*/ "Selon moi, #le ténor grenouillesque à l'avant gauche# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                      // /*spanish*/ Según dicen, el #Sapo Tenore al frente, a la izquierda#, regala #[[1]]#.

    hintTextTable[RHT_ZR_FROGS_SONG_OF_TIME] = HintText(CustomMessage("They say that after hearing #the Song of Time, the frogs# gift #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß #Frösche die die Hymne der Zeit# hören #[[1]]# schenken würden.",
                                                           /*french*/ "Selon moi, à l'écoute du #chant du temps, les grenouilles# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                        // /*spanish*/ Según dicen, después de escuchar #la Canción del tiempo, las ranas# regalan #[[1]]#.
                                                        {}, {
                                                        CustomMessage("They say that #time-traveling frogs# gift #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß #zeitreisende Frösche# #[[1]]# schenken würden.",
                                                            /*french*/ "Selon moi, #les grenouilles voyageuses dans le temps# donnent #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, las #ranas viajeras del tiempo# regalan #[[1]]#.
                                                        CustomMessage("They say that #the Froggish Tenor in the front-right# gifts #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß #der froschige Tenor vorne rechts# #[[1]]# schenke.",
                                                            /*french*/ "Selon moi, #le ténor grenouillesque à l'avant droite# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                         // /*spanish*/ Según dicen, el #Sapo Tenore al frente, a la derecha#, regala #[[1]]#.

    hintTextTable[RHT_GF_HBA_1000_POINTS] = HintText(CustomMessage("They say that scoring 1000 in #horseback archery# grants #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß das Erzielen von 1.000 Punkten beim #Bogenschießen zu Pferde# #[[1]]# gewähre.",
                                                        /*french*/ "Selon moi, obtenir 1000 points dans l'#archerie équestre# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, conseguir 1000 puntos en el #tiro con arco a caballo# premia #[[1]]#.

    hintTextTable[RHT_MARKET_SHOOTING_GALLERY_REWARD] = HintText(CustomMessage("They say that #shooting in youth# grants #[[1]]#.",
                                                                    /*german*/ "Man erzählt sich, daß das #Schießen der Jugend# #[[1]]# einbrächte.",
                                                                    /*french*/ "Selon moi, #faire du tir dans sa jeunesse# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                 // /*spanish*/ Según dicen, #disparar en la juventud# otorga #[[1]]#.

    hintTextTable[RHT_KAK_SHOOTING_GALLERY_REWARD] = HintText(CustomMessage("They say that #shooting in maturity# grants #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß das #Schießen des Alters# #[[1]]# einbrächte.",
                                                                 /*french*/ "Selon moi, #faire du tir dans sa maturité# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #disparar en la madurez# otorga #[[1]]#.

    hintTextTable[RHT_LW_TARGET_IN_WOODS] = HintText(CustomMessage("They say that shooting a #target in the woods# grants #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß das Abschießen eines #Zieles in den Wäldern# #[[1]]# einbrächte.",
                                                        /*french*/ "Selon moi, #tirer une cible dans les bois# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, disparar a un #blanco forestal# brinda #[[1]]#.

    hintTextTable[RHT_KAK_ANJU_AS_ADULT] = HintText(CustomMessage("They say that a #chicken caretaker# offers adults #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß ein #Hühnchenpfleger# Erwachsenen #[[1]]# anböte.",
                                                       /*french*/ "Selon moi, devenir un #éleveur de Cocottes# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #cuidadora de emplumados# le ofrece a los mayores #[[1]]#.

    hintTextTable[RHT_LLR_TALONS_CHICKENS] = HintText(CustomMessage("They say that #finding Super Cuccos# is rewarded with #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß das #Finden von Super-Hühnern# #[[1]]# einbrächte.",
                                                         /*french*/ "Selon moi, #trouver des Super Cocottes# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, #hallar los supercucos# conduce a #[[1]]#.

    hintTextTable[RHT_GC_ROLLING_GORON_AS_CHILD] = HintText(CustomMessage("They say that the prize offered by a #large rolling Goron# is #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der angebotene Preis eines #großen rollenden Goronen# #[[1]]# |sei|seien|.",
                                                               /*french*/ "Selon moi, la récompense d'un #gros Goron roulant# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #gran Goron rodante# otorga #[[1]]#.

    hintTextTable[RHT_LH_UNDERWATER_ITEM] = HintText(CustomMessage("They say that the #sunken treasure in a lake# is #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß der #versunkene Schatz eines Sees# #[[1]]# |sei|seien|.",
                                                        /*french*/ "Selon moi, le #trésor au fond du lac# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, el #tesoro hundido del lago# se trata de #[[1]]#.

    hintTextTable[RHT_TH_FREED_CARPENTERS] = HintText(CustomMessage("They say that #rescuing captured carpenters# is rewarded with #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß das #Retten gefangener Zimmerleute# #[[1]]# einbrächte.",
                                                         /*french*/ "Selon moi, #secourir les charpentiers capturés# assure #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, #rescatar los apresados carpinteros# se premia con #[[1]]#.

    hintTextTable[RHT_WASTELAND_BOMBCHU_SALESMAN] = HintText(CustomMessage("They say that a #carpet guru# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #fliegender Einsiedler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, #un marchand du désert# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, el #genio de una alfombra# vende #[[1]]#.

    hintTextTable[RHT_GC_MEDIGORON] = HintText(CustomMessage("They say that #Medigoron# sells #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß #Medigoron# #[[1]]# verkaufe.",
                                                  /*french*/ "Selon moi, #Medigoron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, #Medigoron# vende #[[1]]#.

    hintTextTable[RHT_KAK_GRANNYS_SHOP] = HintText(CustomMessage("They say that the #potion shop lady# sells #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß die #Dame des Hexenladens# #[[1]]# verkaufe.",
                                                      /*french*/ "Selon moi, la #dame du magasin de potion# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la #señora de la tienda de pociones# vende #[[1]]#.

    hintTextTable[RHT_KAK_IMPAS_HOUSE_FREESTANDING_POH] = HintText(CustomMessage("They say that #imprisoned in a house# lies #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß #eingesperrt in einem Haus# #[[1]]# |läge|lägen|.",
                                                                      /*french*/ "Selon moi, #encagé dans une maison# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, #en una casa entre rejas# yace #[[1]]#.

    hintTextTable[RHT_HF_TEKTITE_GROTTO_FREESTANDING_POH] = HintText(CustomMessage("They say that #deep underwater in a hole# is #[[1]]#.",
                                                                        /*german*/ "Man erzählt sich, daß #tief unter Wasser in einem Erdloch# #[[1]]# |sei|seien|.",
                                                                        /*french*/ "Selon moi, #dans les profondeurs d'une grotte# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                     // /*spanish*/ Según dicen, #en lo hondo bajo un hoyo# yace #[[1]]#.

    hintTextTable[RHT_KAK_WINDMILL_FREESTANDING_POH] = HintText(CustomMessage("They say that on a #windmill ledge# lies #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß auf einem #Vorsprung einer Windmühle# #[[1]]# |läge|lägen|.",
                                                                   /*french*/ "Selon moi, #haut perché dans le moulin# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, al #borde de un molino# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH] = HintText(CustomMessage("They say that #dead Dampe's second# prize is #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß der #zweite Preis des toten Boris# #[[1]]# |sei|seien|.",
                                                                           /*french*/ "Selon moi, la #deuxième course d'Igor# donne #[[1]]#.", {QM_RED, QM_GREEN}),
                                                                        // /*spanish*/ Según dicen, el segundo premio de #la carrera de Dampé# se trata de #[[1]]#.
                                                                        {}, {
                                                                        CustomMessage("They say that #racing a ghost# leads to #[[1]]#.",
                                                                            /*german*/ "Man erzählt sich, daß der #Wettlauf gegen einen Geist# #[[1]]# gewähre.",
                                                                            /*french*/ "Selon moi, le défi du #revenant rapide# donne #[[1]]#.", {QM_RED, QM_GREEN})});
                                                                         // /*spanish*/ Según dicen, #perseguir a un fantasma# conduce a #[[1]]#.

    hintTextTable[RHT_LLR_FREESTANDING_POH] = HintText(CustomMessage("They say that in a #ranch silo# lies #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß in einem #Farmsilo# #[[1]]# |läge|lägen|.",
                                                          /*french*/ "Selon moi, #dans l'entrepôt de la ferme# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en un #granero rupestre# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_FREESTANDING_POH] = HintText(CustomMessage("They say that a #crate in a graveyard# hides #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß eine #Kiste auf einem Friedhof# #[[1]]# verberge.",
                                                                /*french*/ "Selon moi, #la boîte dans le Cimetière# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, bajo la #caja de un cementerio# yace #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR] = HintText(CustomMessage("They say that a #gravekeeper digs up# #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß ein #Grabpfleger# #[[1]]# ausgrabe.",
                                                                       /*french*/ "Selon moi, #le jeu du fossoyeur# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/ Según dicen, cierto #sepultero desentierra# #[[1]]#.

    hintTextTable[RHT_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH] = HintText(CustomMessage("They say that on top of a #pillar in a river# lies #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß auf einer #Säule in einem Fluß# #[[1]]# |läge|lägen|.",
                                                                          /*french*/ "Selon moi, #sur un pilier au dessus du fleuve# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, en lo alto del #pilar de un río# yace #[[1]]#.

    hintTextTable[RHT_ZR_NEAR_DOMAIN_FREESTANDING_POH] = HintText(CustomMessage("They say that on a #river ledge by a waterfall# lies #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß auf einem #Vorsprung eines Flußes nahe eines Wasserfalls# #[[1]]# |läge|lägen|.",
                                                                     /*french*/ "Selon moi, #sur la falaise au dessus du fleuve# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, al borde de #la entrada a una cascada# yace #[[1]]#.

    hintTextTable[RHT_LH_FREESTANDING_POH] = HintText(CustomMessage("They say that high on a #lab rooftop# one can find #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß man auf dem #Dach eines Labors# #[[1]]# finden könne.",
                                                         /*french*/ "Selon moi, #la tour d'observation du lac# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, en lo #alto de un laboratorio# yace #[[1]]#.

    hintTextTable[RHT_ZF_ICEBERG_FREESTANDING_POH] = HintText(CustomMessage("They say that #floating on ice# is #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß sich auf einer Eisscholle #[[1]]# |befände|befänden|.",
                                                                 /*french*/ "Selon moi, #gisant sur la glace# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, #flotando sobre hielo# yace #[[1]]#.

    hintTextTable[RHT_GV_WATERFALL_FREESTANDING_POH] = HintText(CustomMessage("They say that behind a #valley waterfall# is #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß hinter einem #Wasserfall eines Tals# #[[1]]# |sei|seien|.",
                                                                   /*french*/ "Selon moi, #derrière la cascade du désert# se cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, tras una #desierta cascada# yace #[[1]]#.

    hintTextTable[RHT_GV_CRATE_FREESTANDING_POH] = HintText(CustomMessage("They say that a #crate in a valley# hides #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß eine #Kiste in einem Tal# #[[1]]# verberge.",
                                                               /*french*/ "Selon moi, la #boîte dans la vallée# contient #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, bajo la #caja de un valle# yace #[[1]]#.

    hintTextTable[RHT_COLOSSUS_FREESTANDING_POH] = HintText(CustomMessage("They say that on top of an #arch of stone# lies #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß auf einem #Steinbogen# #[[1]]# |läge|lägen|.",
                                                               /*french*/ "Selon moi, #gisant sur une arche de pierre# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, en lo alto de un #arco de piedra# yace #[[1]]#.

    hintTextTable[RHT_DMT_FREESTANDING_POH] = HintText(CustomMessage("They say that above a #mountain cavern entrance# is #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß oberhalb eines #Berghöhleneingangs# #[[1]]# |sei|seien|.",
                                                          /*french*/ "Selon moi, gisant #au dessus de la caverne montagneuse# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, en lo alto de la #entrada de una cueva en la montaña# yace #[[1]]#.

    hintTextTable[RHT_DMC_WALL_FREESTANDING_POH] = HintText(CustomMessage("They say that nestled in a #volcanic wall# is #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß in einer #vulkanischen Wand# #[[1]]# |sei|seien|.",
                                                               /*french*/ "Selon moi, dans une #alcove volcanique# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, entre unas #murallas volcánicas# yace #[[1]]#.

    hintTextTable[RHT_DMC_VOLCANO_FREESTANDING_POH] = HintText(CustomMessage("They say that obscured by #volcanic ash# is #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß #Vulkanasche# #[[1]]# verdecke.",
                                                                  /*french*/ "Selon moi, #recouvert de cendres volcaniques# gît #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, bajo la #ceniza volcánica# yace #[[1]]#.

    hintTextTable[RHT_TH_1_TORCH_CARPENTER] = HintText(CustomMessage("They say that #defeating the Gerudo in a room with 1 torch# reveals #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß das #Besiegen der Gerudo-Wachen# #[[1]]# enthüllen würde.",//TODO_TRANSLATE update to match
                                                           /*french*/ "Selon moi, les #geôliers Gerudo# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                        // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_DEAD_END_CARPENTER] = HintText(CustomMessage("They say that #defeating the Gerudo in a Dead End# reveals #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß das #Besiegen der Gerudo-Wachen# #[[1]]# enthüllen würde.",//TODO_TRANSLATE update to match
                                                           /*french*/ "Selon moi, les #geôliers Gerudo# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                        // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_DOUBLE_CELL_CARPENTER] = HintText(CustomMessage("They say that #defeating the Gerudo in a large jail# reveals #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß das #Besiegen der Gerudo-Wachen# #[[1]]# enthüllen würde.",//TODO_TRANSLATE update to match
                                                              /*french*/ "Selon moi, les #geôliers Gerudo# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                           // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_TH_STEEP_SLOPE_CARPENTER] = HintText(CustomMessage("They say that #defeating the Gerudo between steep slopes# reveals #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß das #Besiegen der Gerudo-Wachen# #[[1]]# enthüllen würde.",//TODO_TRANSLATE update to match
                                                              /*french*/ "Selon moi, les #geôliers Gerudo# détiennent #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match
                                                           // /*spanish*/ Según dicen, #derrotar a las guardas Gerudo# revela #[[1]]#.

    hintTextTable[RHT_HF_GS_NEAR_KAK_GROTTO] = HintText(CustomMessage("They say that a #spider-guarded spider in a hole# hoards #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #spinnenbewachte Spinne in einem Erdloch #[[1]]# horte.",
                                                           /*french*/ "Selon moi, une #Skulltula dans un trou d'arachnides# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula custodiada por otra# de un hoyo otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_BACK_WALL] = HintText(CustomMessage("They say that night reveals a #spider in a ranch# holding #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne auf einer Farm# enthülle, welche #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula sur la façade de la ferme# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_RAIN_SHED] = HintText(CustomMessage("They say that night reveals a #spider in a ranch# holding #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne auf einer Farm# enthülle, welche #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula sur le mur de l'enclos# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_HOUSE_WINDOW] = HintText(CustomMessage("They say that night reveals a #spider in a ranch# holding #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne auf einer Farm# enthülle, welche #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, une #Skulltula sur la maison de ferme# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LLR_GS_TREE] = HintText(CustomMessage("They say that a spider hiding in a #ranch tree# holds #[[1]]#.",
                                                 /*german*/ "Man erzählt sich, daß eine in einem #Baum auf einer Farm# versteckte Spinne #[[1]]# besäße.",
                                                 /*french*/ "Selon moi, une #Skulltula dans l'arbre de la ferme# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de un rancho# otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried in a forest# holds #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß eine #in einem Wald vergrabene Spinne# #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula enterrée dans la forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada en un bosque# otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_KNOW_IT_ALL_HOUSE] = HintText(CustomMessage("They say that night in the past reveals a #spider in a forest# holding #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Wald# enthülle, welche #[[1]]# besäße.",
                                                             /*french*/ "Selon moi, une #Skulltula derrière une cabane de la forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, la noche revela en el pasado una #Skulltula del bosque# que otorga #[[1]]#.

    hintTextTable[RHT_KF_GS_HOUSE_OF_TWINS] = HintText(CustomMessage("They say that night in the future reveals a #spider in a forest# holding #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne in einem Wald# enthülle, welche #[[1]]# besäße.",
                                                          /*french*/ "Selon moi, une #Skulltula sur une cabane de la forêt# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, la noche revela en el futuro una #Skulltula del rancho# que otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_BEAN_PATCH_NEAR_BRIDGE] = HintText(CustomMessage("They say that a #spider buried deep in a forest maze# holds #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß eine #tief in einem Waldlabyrinth vergrabene Spinne# #[[1]]# besäße.",
                                                                  /*french*/ "Selon moi, une #Skulltula enterrée dans les bois# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, una #Skulltula enterrada en un laberinto forestal# otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_BEAN_PATCH_NEAR_THEATER] = HintText(CustomMessage("They say that a #spider buried deep in a forest maze# holds #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß eine #tief in einem Waldlabyrinth vergrabene Spinne# #[[1]]# besäße.",
                                                                   /*french*/ "Selon moi, une #Skulltula enterrée dans les bois# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #Skulltula enterrada en un laberinto forestal# otorga #[[1]]#.

    hintTextTable[RHT_LW_GS_ABOVE_THEATER] = HintText(CustomMessage("They say that night reveals a #spider deep in a forest maze# holding #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne tief in einem Waldlabyrinth# enthülle, welche #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, une #Skulltula haut perchée dans les bois# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula del laberinto forestal# que otorga #[[1]]#.

    hintTextTable[RHT_SFM_GS] = HintText(CustomMessage("They say that night reveals a #spider in a forest meadow# holding #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne auf einer Waldlichtung# enthülle, welche #[[1]]# besäße.",
                                            /*french*/ "Selon moi, une #Skulltula dans le sanctuaire des bois# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                         // /*spanish*/ Según dicen, la noche revela una #Skulltula de la pradera del bosque# que otorga #[[1]]#.

    hintTextTable[RHT_OGC_GS] = HintText(CustomMessage("They say that a #spider outside a tyrant's tower# holds #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Spinne außerhalb eines Turmes eines Tyrannen# #[[1]]# besäße.",
                                            /*french*/ "Selon moi, une #Skulltula parmi les ruines du château# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                         // /*spanish*/ Según dicen, una #Skulltula a las afueras de la torre de un tirano# otorga #[[1]]#.

    hintTextTable[RHT_HC_GS_TREE] = HintText(CustomMessage("They say that a spider hiding in a #tree outside of a castle# holds #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine in einem #Baum außerhalb eines Schloßes befindliche Spinne# #[[1]]# besäße.",
                                                /*french*/ "Selon moi, une #Skulltula dans l'arbre près du château# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de las afueras de un castillo# otorga #[[1]]#.

    hintTextTable[RHT_MARKET_GS_GUARD_HOUSE] = HintText(CustomMessage("They say that a #spider in a guarded crate# holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #Spinne in einer bewachten Kiste# #[[1]]# besäße.",
                                                           /*french*/ "Selon moi, une #Skulltula dans une boîte en ville# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula bajo una custodiada caja# otorga #[[1]]#.

    hintTextTable[RHT_DMC_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried in a volcano# holds #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #in einem Vulkan vergrabene Spinne# #[[1]]# besäße.",
                                                       /*french*/ "Selon moi, une #Skulltula enterrée dans un volcan# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #Skulltula enterrada en un volcán# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried outside a cavern# holds #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #außerhalb einer Höhle vergrabene Spinne# #[[1]]# besäße.",
                                                       /*french*/ "Selon moi, une #Skulltula enterrée près d'une caverne# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, una #Skulltula enterrada a la entrada de una cueva# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_NEAR_KAK] = HintText(CustomMessage("They say that a #spider hidden in a mountain nook# holds #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß eine #in einer Bergnische versteckte Spinne# #[[1]]# besäße.",
                                                     /*french*/ "Selon moi, une #Skulltula cachée dans le flanc d'une montagne# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #Skulltula oculta en el rincón de la montaña# otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_ABOVE_DODONGOS_CAVERN] = HintText(CustomMessage("They say that the hammer reveals a #spider on a mountain# holding #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß der Hammer eine #Spinne auf einem Berg# enthülle, welche #[[1]]# besäße.",
                                                                  /*french*/ "Selon moi, une #Skulltula derrière un rocher massif près d'une caverne# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, el martillo revela #una Skulltula de la montaña# que otorga #[[1]]#.

    hintTextTable[RHT_DMT_GS_FALLING_ROCKS_PATH] = HintText(CustomMessage("They say that the hammer reveals a #spider on a mountain# holding #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der Hammer eine #Spinne auf einem Berg# enthülle, welche #[[1]]# besäße.",
                                                               /*french*/ "Selon moi, une #Skulltula derrière un rocher massif près du sommet d'un volcan# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, el martillo revela #una Skulltula de la montaña# que otorga #[[1]]#.

    hintTextTable[RHT_GC_GS_CENTER_PLATFORM] = HintText(CustomMessage("They say that a #suspended spider# in Goron City holds #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #hängende Spinne# in Goronia #[[1]]# besäße.",
                                                           /*french*/ "Selon moi, une #Skulltula perchée dans le village Goron# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #Skulltula suspendida# en la Ciudad Goron otorga #[[1]]#.

    hintTextTable[RHT_GC_GS_BOULDER_MAZE] = HintText(CustomMessage("They say that a spider in a #Goron City crate# holds #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß eine Spinne in einer #Kiste in Goronia# #[[1]]# besäße.",
                                                        /*french*/ "Selon moi, une #Skulltula dans une boîte du village Goron# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #Skulltula bajo una caja# de la Ciudad Goron otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_HOUSE_UNDER_CONSTRUCTION] = HintText(CustomMessage("They say that night in the past reveals a #spider in a town# holding #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                                     /*french*/ "Selon moi, une #Skulltula dans le chantier de construction# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_SKULLTULA_HOUSE] = HintText(CustomMessage("They say that night in the past reveals a #spider in a town# holding #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                            /*french*/ "Selon moi, une #Skulltula sur une maison maudite# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_GUARDS_HOUSE] = HintText(CustomMessage("They say that night in the past reveals a #spider in a town# holding #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, une #Skulltula sur une maison de village# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_TREE] = HintText(CustomMessage("They say that night in the past reveals a #spider in a town# holding #[[1]]#.",
                                                 /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                 /*french*/ "Selon moi, une #Skulltula dans un arbre de village# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                              // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_WATCHTOWER] = HintText(CustomMessage("They say that night in the past reveals a #spider in a town# holding #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                       /*french*/ "Selon moi, une #Skulltula sur une échelle dans un village# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_KAK_GS_ABOVE_IMPAS_HOUSE] = HintText(CustomMessage("They say that night in the future reveals a #spider in a town# holding #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne in einem Dorf# enthülle, welche #[[1]]# besäße.",
                                                              /*french*/ "Selon moi, une #Skulltula au dessus d'une grande maison# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del pueblo# que otorga #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_GS_WALL] = HintText(CustomMessage("They say that night reveals a #spider in a graveyard# holding #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne auf einem Friedhof# enthülle, welche #[[1]]# besäße.",
                                                       /*french*/ "Selon moi, une #Skulltula sur une façade du Cimetière# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche revela una #Skulltula del cementerio# que otorga #[[1]]#.

    hintTextTable[RHT_GRAVEYARD_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried in a graveyard# holds #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß eine #auf einem Friedhof vergrabene Spinne# #[[1]]# besäße.",
                                                             /*french*/ "Selon moi, une #Skulltula enterrée dans le Cimetière# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                          // /*spanish*/ Según dicen, una #Skulltula enterrada en el cementerio# otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_LADDER] = HintText(CustomMessage("They say that night in the past reveals a #spider in a river# holding #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                  /*french*/ "Selon moi, une #Skulltula sur une échelle près d'une cascade# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_TREE] = HintText(CustomMessage("They say that a spider hiding in a #tree by a river# holds #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine in einem #Baum an einem Fluß# versteckte Spinne #[[1]]# besäße.",
                                                /*french*/ "Selon moi, une #Skulltula dans un arbre près du fleuve# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol de un río# otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_ABOVE_BRIDGE] = HintText(CustomMessage("They say that night in the future reveals a #spider in a river# holding #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                        /*french*/ "Selon moi, une #Skulltula sur une façade près d'une cascade# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZR_GS_NEAR_RAISED_GROTTOS] = HintText(CustomMessage("They say that night in the future reveals a #spider in a river# holding #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                               /*french*/ "Selon moi, une #Skulltula sur une façade près d'une grotte du fleuve# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del río# que otorga #[[1]]#.

    hintTextTable[RHT_ZD_GS_FROZEN_WATERFALL] = HintText(CustomMessage("They say that night reveals a #spider by a frozen waterfall# holding #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne an einem gefrorenen Wasserfall# enthülle, welche #[[1]]# besäße.",
                                                            /*french*/ "Selon moi, une #Skulltula près d'une cascade gelée# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a una congelada cascada# que otorga #[[1]]#.

    hintTextTable[RHT_ZF_GS_ABOVE_THE_LOG] = HintText(CustomMessage("They say that night reveals a #spider near a deity# holding #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne in der Nähe einer Gottheit# enthülle, welche #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, une #Skulltula près du gardien aquatique# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a cierta deidad# que otorga #[[1]]#.

    hintTextTable[RHT_ZF_GS_TREE] = HintText(CustomMessage("They say that a spider hiding in a #tree near a deity# holds #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine in einem #Baum in der Nähe einer Gottheit# versteckte Spinne #[[1]]# besäße.",
                                                /*french*/ "Selon moi, une #Skulltula dans un arbre dans un réservoir# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, una Skulltula escondida en el #árbol junto a cierta deidad# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried by a lake# holds #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß eine #an einem Fluß vergrabene Spinne# #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula enterrée près d'un lac# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada junto a un lago# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_SMALL_ISLAND] = HintText(CustomMessage("They say that night reveals a #spider by a lake# holding #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                        /*french*/ "Selon moi, une #Skulltula sur un îlot du lac# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a un lago# que otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_LAB_WALL] = HintText(CustomMessage("They say that night reveals a #spider by a lake# holding #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                    /*french*/ "Selon moi, une #Skulltula sur le mur d'un centre de recherche# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, la noche revela una #Skulltula junto a un lago# que otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_LAB_CRATE] = HintText(CustomMessage("They say that a spider deed underwater in a #lab crate# holds #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß eine Spinne in einer #Laborkiste# unter Wasser #[[1]]# besäße.",
                                                     /*french*/ "Selon moi, une #Skulltula dans une boîte au fond d'une cuve d'eau# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, una #Skulltula bajo la sumergida caja de un laboratorio# otorga #[[1]]#.

    hintTextTable[RHT_LH_GS_TREE] = HintText(CustomMessage("They say that night reveals a #spider by a lake high in a tree# holding #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne in einem Baum an einem Fluß# enthülle, welche #[[1]]# besäße.",
                                                /*french*/ "Selon moi, une #Skulltula dans un grand arbre du lac# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                             // /*spanish*/ Según dicen, la noche revela #una Skulltula del lago sobre un árbol# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried in a valley# holds #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß eine #in einem Tal vergrabene Spinne# #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula enterré dans une vallée# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, una #Skulltula enterrada en un valle# otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_SMALL_BRIDGE] = HintText(CustomMessage("They say that night in the past reveals a #spider in a valley# holding #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß die Nacht in der Vergangenheit eine #Spinne in einem Tal# enthülle, welche #[[1]]# besäße.",
                                                        /*french*/ "Selon moi, une #Skulltula au dessus d'une petite cascade# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, la noche del pasado revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_PILLAR] = HintText(CustomMessage("They say that night in the future reveals a #spider in a valley# holding #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne in einem Tal# enthülle, welche #[[1]]# besäße.",
                                                  /*french*/ "Selon moi, une #Skulltula sur une arche de pierre dans une vallée# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                               // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GV_GS_BEHIND_TENT] = HintText(CustomMessage("They say that night in the future reveals a #spider in a valley# holding #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß die Nacht in der Zukunft eine #Spinne in einem Tal# enthülle, welche #[[1]]# besäße.",
                                                       /*french*/ "Selon moi, une #Skulltula derrière une tente# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, la noche del futuro revela una #Skulltula del valle# que otorga #[[1]]#.

    hintTextTable[RHT_GF_GS_ARCHERY_RANGE] = HintText(CustomMessage("They say that night reveals a #spider in a fortress# holding #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne in einer Festung# enthülle, welche #[[1]]# besäße.",
                                                         /*french*/ "Selon moi, une #Skulltula sur une cible de tir# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, la noche revela una #Skulltula de una fortaleza# que otorga #[[1]]#.

    hintTextTable[RHT_GF_GS_TOP_FLOOR] = HintText(CustomMessage("They say that night reveals a #spider in a fortress# holding #[[1]]#.",
                                                     /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne in einer Festung# enthülle, welche #[[1]]# besäße.",
                                                     /*french*/ "Selon moi, une #Skulltula au sommet d'une forteresse# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                  // /*spanish*/ Según dicen, la noche revela una #Skulltula de una fortaleza# que otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_BEAN_PATCH] = HintText(CustomMessage("They say that a #spider buried in the desert# holds #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß eine #in der Wüste vergrabene Spinne# #[[1]]# besäße.",
                                                            /*french*/ "Selon moi, une #Skulltula enterrée au pied du colosse# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                         // /*spanish*/ Según dicen, una #Skulltula enterrada en el desierto# otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_HILL] = HintText(CustomMessage("They say that night reveals a #spider deep in the desert# holding #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne tief in der Wüste# enthülle, welche #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula sur une colline dans le désert# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula en las profundidades del desierto# que otorga #[[1]]#.

    hintTextTable[RHT_COLOSSUS_GS_TREE] = HintText(CustomMessage("They say that night reveals a #spider deep in the desert# holding #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß die Nacht eine #Spinne tief in der Wüste# enthülle, welche #[[1]]# besäße.",
                                                      /*french*/ "Selon moi, une #Skulltula dans un arbre du désert# a #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/ Según dicen, la noche revela una #Skulltula en las profundidades del desierto# que otorga #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_1] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_2] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_3] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_4] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_5] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_6] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_7] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KF_SHOP_ITEM_8] = HintText(CustomMessage("They say that a #child shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #kindlicher Ladenbesitzer# #[[1]]# verkaufe.",
                                                    /*french*/ "Selon moi, la #boutique Kokiri# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, un #joven dependiente# vende #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_1] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_2] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_3] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_4] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_5] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_6] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_7] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_POTION_SHOP_ITEM_8] = HintText(CustomMessage("They say that the #Kakariko Potion Shop# offers #[[1]]#.",
                                                            /*german*/ "Man erzählt sich, daß der #Magie-Laden von Kakariko# #[[1]]# offeriere.",
                                                            /*french*/ "Selon moi, l'#apothicaire de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                         // /*spanish*/ Según dicen, la #tienda de pociones de Kakariko# ofrece #[[1]]#.
                                                         {}, {
                                                         CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                             /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                             /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                          // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_1] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_2] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_3] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_4] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_5] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_6] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_7] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_BOMBCHU_SHOP_ITEM_8] = HintText(CustomMessage("They say that a #Bombchu merchant# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Krabbelminenhändler# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #marchand de Missiles# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #mercader de bombchus# vende #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_1] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_2] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_3] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_4] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_5] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_6] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_7] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_POTION_SHOP_ITEM_8] = HintText(CustomMessage("They say that the #Market Potion Shop# offers #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß der #Magie-Laden des Marktes# #[[1]]# offeriere.",
                                                               /*french*/ "Selon moi, l'#apothicaire dans la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}),
                                                            // /*spanish*/ Según dicen, la #tienda de pociones del mercado# ofrece #[[1]]#.
                                                            {}, {
                                                            CustomMessage("They say that a #potion seller# offers #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Trankhändler# #[[1]]# anböte.",
                                                                /*french*/ "Selon moi, l'#apothicaire# vend #[[1]]#.", {QM_RED, QM_GREEN})});
                                                             // /*spanish*/ Según dicen, un #vendedor de pociones# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_1] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_2] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_3] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_4] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_5] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_6] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_7] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_MARKET_BAZAAR_ITEM_8] = HintText(CustomMessage("They say that the #Market Bazaar# offers #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem #Marktbasar# #[[1]]# angeboten |würde|würden|.",
                                                          /*french*/ "Selon moi, le #bazar de la Place du Marché# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, el #bazar del mercado# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_1] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_2] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_3] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_4] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_5] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_6] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_7] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_KAK_BAZAAR_ITEM_8] = HintText(CustomMessage("They say that the #Kakariko Bazaar# offers #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß auf dem #Basar von Kakariko# #[[1]]# angeboten |würde|würden|.",
                                                       /*french*/ "Selon moi, le #bazar de Kakariko# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                    // /*spanish*/ Según dicen, el #bazar de Kakariko# ofrece #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_1] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_2] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_3] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_4] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_5] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_6] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_7] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_ZD_SHOP_ITEM_8] = HintText(CustomMessage("They say that a #Zora shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Zora# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Zora# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Zora# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_1] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_2] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_3] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_4] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_5] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_6] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_7] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_GC_SHOP_ITEM_8] = HintText(CustomMessage("They say that a #Goron shopkeeper# sells #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Händler der Goronen# #[[1]]# verkaufen würde.",
                                                    /*french*/ "Selon moi, la #boutique Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                 // /*spanish*/ Según dicen, el #dependiente Goron# vende #[[1]]#.

    hintTextTable[RHT_HF_DEKU_SCRUB_GROTTO] = HintText(CustomMessage("They say that a lonely #scrub in a hole# sells #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß ein #einsamer Deku-Händler in einem Erdloch# #[[1]]# verkaufe.",
                                                          /*french*/ "Selon moi, la #peste Mojo dans une grotte de la plaine# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, un #singular deku bajo un hoyo# de la llanura vende #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #trio de peste Mojo à la ferme# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, le #trio de peste Mojo à la ferme# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LLR_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                  /*french*/ "Selon moi, le #trio de peste Mojo à la ferme# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #trío de dekus# de una granja venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT] = HintText(CustomMessage("They say that a pair of #scrubs in the woods# sells #[[1]]#.",
                                                                           /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar in den Wäldern# #[[1]]# verkaufe.",
                                                                           /*french*/ "Selon moi, le #duo de peste Mojo près du théâtre# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                        // /*spanish*/ Según dicen, un par de #dekus del bosque# venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT] = HintText(CustomMessage("They say that a pair of #scrubs in the woods# sells #[[1]]#.",
                                                                          /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar in den Wäldern# #[[1]]# verkaufe.",
                                                                          /*french*/ "Selon moi, le #duo de peste Mojo près du théâtre# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                       // /*spanish*/ Según dicen, un par de #dekus del bosque# venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_NEAR_BRIDGE] = HintText(CustomMessage("They say that a #scrub by a bridge# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler an einer Brücke# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, la #peste Mojo près du pont dans les bois# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #deku bajo un puente# del bosque venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, le #duo de peste Mojo dans les sous-bois# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del bosque venden #[[1]]#.

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #duo de peste Mojo dans les sous-bois# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del bosque venden #[[1]]#.

    hintTextTable[RHT_SFM_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #duo de peste Mojo au coeur du sanctuaire sylvestre# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# de la pradera sagrada venden #[[1]]#.

    hintTextTable[RHT_SFM_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, le #duo de peste Mojo au coeur du sanctuaire sylvestre# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #par de dekus subterráneos# de la pradera sagrada venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, le #trio de peste Mojo dans le village Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #trio de peste Mojo dans le village Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_GC_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, le #trio de peste Mojo dans le village Goron# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# de la Ciudad Goron venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #trio de peste Mojo dans le volcan# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, le #trio de peste Mojo dans le volcan# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_DMC_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                  /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                  /*french*/ "Selon moi, le #trio de peste Mojo dans le volcan# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                               // /*spanish*/ Según dicen, un #trío de dekus# del volcán venden #[[1]]#.

    hintTextTable[RHT_ZR_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, le #duo de peste Mojo près du fleuve# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del río venden #[[1]]#.

    hintTextTable[RHT_ZR_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #duo de peste Mojo près du fleuve# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del río venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_LEFT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, le #trio de peste Mojo près du lac# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_RIGHT] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #trio de peste Mojo près du lac# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_LH_DEKU_SCRUB_GROTTO_CENTER] = HintText(CustomMessage("They say that a #trio of scrubs# sells #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Trio# #[[1]]# verkaufe.",
                                                                 /*french*/ "Selon moi, le #trio de peste Mojo près du lac# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, un #trío de dekus# del lago venden #[[1]]#.

    hintTextTable[RHT_GV_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                               /*french*/ "Selon moi, le #duo de peste Mojo près de la vallée# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, un #par de dekus subterráneos# del valle venden #[[1]]#.

    hintTextTable[RHT_GV_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                /*french*/ "Selon moi, le #duo de peste Mojo près de la vallée# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/ Según dicen, un #par de dekus subterráneos# del valle venden #[[1]]#.

    hintTextTable[RHT_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                      /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                      /*french*/ "Selon moi, le #duo de peste Mojo dans le désert# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                   // /*spanish*/ Según dicen, un #par de dekus subterráneos# del desierto venden #[[1]]#.

    hintTextTable[RHT_COLOSSUS_DEKU_SCRUB_GROTTO_REAR] = HintText(CustomMessage("They say that a #scrub underground duo# sells #[[1]]#.",
                                                                     /*german*/ "Man erzählt sich, daß ein #Deku-Händler-Paar im Untergrund# #[[1]]# verkaufe.",
                                                                     /*french*/ "Selon moi, le #duo de peste Mojo dans le désert# vend #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                  // /*spanish*/ Según dicen, un #par de dekus subterráneos# del desierto venden #[[1]]#.

    hintTextTable[RHT_LLR_STABLES_LEFT_COW] = HintText(CustomMessage("They say that a #cow in a stable# gifts #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß eine #Kuh in einem Stall# #[[1]]# schenke.",
                                                          /*french*/ "Selon moi, la #vache dans l'étable# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, una #vaca del establo# brinda #[[1]]#.

    hintTextTable[RHT_LLR_STABLES_RIGHT_COW] = HintText(CustomMessage("They say that a #cow in a stable# gifts #[[1]]#.",
                                                           /*german*/ "Man erzählt sich, daß eine #Kuh in einem Stall# #[[1]]# schenke.",
                                                           /*french*/ "Selon moi, la #vache dans l'étable# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                        // /*spanish*/ Según dicen, una #vaca del establo# brinda #[[1]]#.

    hintTextTable[RHT_LLR_TOWER_RIGHT_COW] = HintText(CustomMessage("They say that a #cow in a ranch silo# gifts #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß eine #Kuh in einem Farmsilo# #[[1]]# schenke.",
                                                         /*french*/ "Selon moi, la #vache dans le silo de la ferme# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, una #vaca del granero# brinda #[[1]]#.

    hintTextTable[RHT_LLR_TOWER_LEFT_COW] = HintText(CustomMessage("They say that a #cow in a ranch silo# gifts #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß eine #Kuh in einem Farmsilo# #[[1]]# schenke.",
                                                        /*french*/ "Selon moi, la #vache dans le silo de la ferme# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #vaca del granero# brinda #[[1]]#.

    hintTextTable[RHT_KAK_IMPAS_HOUSE_COW] = HintText(CustomMessage("They say that a #cow imprisoned in a house# protects #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß eine #in einem Haus gefangene Kuh# #[[1]]# schütze.",
                                                         /*french*/ "Selon moi, la #vache en cage# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/ Según dicen, una #vaca enjaulada de una casa# brinda #[[1]]#.

    hintTextTable[RHT_DMT_COW_GROTTO_COW] = HintText(CustomMessage("They say that a #cow in a luxurious hole# offers #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß eine #Kuh in einem luxuriösen Erdloch# #[[1]]# offeriere.",
                                                        /*french*/ "Selon moi, la #vache dans une grotte luxueuse# donne #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #vaca de un lujoso hoyo# brinda #[[1]]#.

    hintTextTable[RHT_BEEHIVE_CHEST_GROTTO] = HintText(CustomMessage("They say that a #beehive above a chest# hides #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß ein #Bienenstock oberhalb einer Truhe# #[[1]]# verberge.",
                                                          /*french*/ "Selon moi, une #ruche au dessus d'un coffre# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/ Según dicen, una #colmena sobre un cofre# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_LONELY_SCRUB_GROTTO] = HintText(CustomMessage("They say that a #beehive above a lonely scrub# hides #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß ein #Bienenstock oberhalb eines einsamen Deku-Händlers# #[[1]]# verberge.",
                                                                 /*french*/ "Selon moi, une #ruche au dessus d'une Peste Mojo solitaire# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/ Según dicen, una #colmena sobre un deku solitario# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_SCRUB_PAIR_GROTTO] = HintText(CustomMessage("They say that a #beehive above a pair of scrubs# hides #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Bienenstock oberhalb eines Deku-Händler-Paars# #[[1]]# verberge.",
                                                               /*french*/ "Selon moi, une #ruche au dessus d'un duo de Pestes Mojo# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #colmena sobre un par de dekus# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_SCRUB_TRIO_GROTTO] = HintText(CustomMessage("They say that a #beehive above a trio of scrubs# hides #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Bienenstock oberhalb eines Deku-Händler-Trios# #[[1]]# verberge.",
                                                               /*french*/ "Selon moi, une #ruche au dessus d'un trio de Pestes Mojo# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/ Según dicen, una #colmena sobre un trío de dekus# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_COW_GROTTO] = HintText(CustomMessage("They say that a #beehive above a cow# hides #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß ein #Bienenstock oberhalb einer Kuh# #[[1]]# verberge.",
                                                        /*french*/ "Selon moi, une #ruche au dessus d'une vache# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/ Según dicen, una #colmena sobre una vaca# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_IN_FRONT_OF_KING_ZORA] = HintText(CustomMessage("They say that a #beehive in front of the king of the zoras# hides #[[1]]#.",
                                                                   /*german*/ "Man erzählt sich, daß ein #Bienenstock vor dem König der Zoras# #[[1]]# verberge.",
                                                                   /*french*/ "Selon moi, une #ruche devant le Roi des Zoras# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                // /*spanish*/ Según dicen, una #colmena delante del rey de los zoras# esconde #[[1]]#.

    hintTextTable[RHT_BEEHIVE_BEHIND_KING_ZORA] = HintText(CustomMessage("They say that a #beehive behind the king of the zoras# hides #[[1]]#.",
                                                              /*german*/ "Man erzählt sich, daß ein #Bienenstock hinter dem König der Zoras# #[[1]]# verberge.",
                                                              /*french*/ "Selon moi, une #rucheau derrière le Roi des Zoras# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                           // /*spanish*/ Según dicen, una #colmena detrás del rey de los zoras# esconde #[[1]]#.

    hintTextTable[RHT_POT_LINKS_HOUSE] = HintText(CustomMessage("They say that the #pot in the hero's house# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #Vase im Kokiri-Wald# #[[1]]# enthielte.", //TODO_TRANSLATE update to match
                                                       /*french*/ "Selon moi, une #jarre dans la Fôret Kokiri# contient #[[1]]#.", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_TWINS_HOUSE] = HintText(CustomMessage("They say that a #pot shared by twins# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #Vase im Kokiri-Wald# #[[1]]# enthielte.",  //TODO_TRANSLATE update to match
                                                       /*french*/ "Selon moi, une #jarre dans la Fôret Kokiri# contient #[[1]]#.", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_KNOW_IT_ALL] = HintText(CustomMessage("They say that a #know-it-all bother's pot# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #Vase im Kokiri-Wald# #[[1]]# enthielte.",  //TODO_TRANSLATE update to match
                                                       /*french*/ "Selon moi, une #jarre dans la Fôret Kokiri# contient #[[1]]#.", {QM_RED, QM_GREEN}));  //TODO_TRANSLATE update to match

    hintTextTable[RHT_TH_BREAK_ROOM_FRONT_POT] = HintText(CustomMessage("They say that the #front pot watched by resting thieves# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_BREAK_ROOM_BACK_POT] = HintText(CustomMessage("They say that the #back pot watched by resting thieves# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_KITCHEN_POT_1] = HintText(CustomMessage("They say that a #Hideout's Kitchen pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_KITCHEN_POT_2] = HintText(CustomMessage("They say that a #Hideout's Kitchen pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_RIGHT_POT] = HintText(CustomMessage("They say that the #thieve's right pot, watched by a single torch# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_MID_POT] = HintText(CustomMessage("They say that the #thieve's middle pot, watched by a single torch# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_1_TORCH_CELL_LEFT_POT] = HintText(CustomMessage("They say that the #thieve's left pot, watched by a single torch# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_STEEP_SLOPE_RIGHT_POT] = HintText(CustomMessage("They say that a #Gerudo pot, on the right of a sloped corner# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_STEEP_SLOPE_LEFT_POT] = HintText(CustomMessage("They say that a #Gerudo pot, on the left of a sloped corner# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_RIGHT_POT] = HintText(CustomMessage("They say that the #right pot near 2 jail cells# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_MID_POT] = HintText(CustomMessage("They say that the #mid pot near 2 jail cells# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_NEAR_DOUBLE_CELL_LEFT_POT] = HintText(CustomMessage("They say that the #left pot near 2 jail cells# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_RIGHTMOST_JAILED_POT] = HintText(CustomMessage("They say that the #rightmost jailed pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_RIGHT_MIDDLE_JAILED_POT] = HintText(CustomMessage("They say that the #right middle jailed pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_LEFT_MIDDLE_JAILED_POT] = HintText(CustomMessage("They say that the #left middle jailed pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_TH_LEFTMOST_JAILED_POT] = HintText(CustomMessage("They say that the #leftmost jailed pot# contains #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE

    hintTextTable[RHT_POT_WASTELAND] = HintText(CustomMessage("They say that a #pot in Haunted Wasteland# contains #[[1]]#.",
                                                   /*german*/ "Man erzählt sich, daß ein #Krug in der Gespensterwüste# #[[1]]# enthielte.",
                                                   /*french*/ "Selon moi, une #jarre dans le Désert Hanté# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_GUARD_HOUSE] = HintText(CustomMessage("They say that a #pot in the Guard House# contains #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine #Krug auf dem Markt# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                /*french*/ "Selon moi, une #jarre dans la Place du Marché# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_POE_SHOP] = HintText(CustomMessage("They say that a #pot in the Poe Shop# contains #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine #Krug auf dem Markt# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                /*french*/ "Selon moi, une #jarre dans la Place du Marché# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_ALLEY_HOUSE] = HintText(CustomMessage("They say that a #bearded man's pot# contains #[[1]]#.",
                                                /*german*/ "Man erzählt sich, daß eine #Krug auf dem Markt# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                /*french*/ "Selon moi, une #jarre dans la Place du Marché# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_KAKARIKO] = HintText(CustomMessage("They say that a #pot in Kakariko Village# contains #[[1]]#.",
                                                  /*german*/ "Man erzählt sich, daß ein #Krug in Kakariko# #[[1]]# enthielte.",
                                                  /*french*/ "Selon moi, une #jarre dans le Village de Cocorico# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_DAMPE] = HintText(CustomMessage("They say that a #pot in gravekeeper's tomb# contains #[[1]]#.",
                                                         //TODO_TRANSLATE check these to make sure they refernce dampe's tomb not the graveyard area
                                                   /*german*/ "Man erzählt sich, daß ein #Krug auf dem Friedhof# #[[1]]# enthielte.",
                                                   /*french*/ "Selon moi, une #jarre dans le Cimetière# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_GORON_CITY] = HintText(CustomMessage("They say that a #pot in Goron City# contains #[[1]]#.",
                                                    /*german*/ "Man erzählt sich, daß ein #Krug in Goronia# #[[1]]# enthielte.",
                                                    /*french*/ "Selon moi, une #jarre dans le Village Goron# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessage("They say that a #pot in Death Mountain Crater# contains #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß ein #Krug im Todeskrater# #[[1]]# enthielte.",
                                                               /*french*/ "Selon moi, une #jarre sur le Mont du Péril# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_ZORAS_DOMAIN] = HintText(CustomMessage("They say that a #pot in Zora's Domain# contains #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß ein #Krug in Zoras Reich# #[[1]]# enthielte.",
                                                      /*french*/ "Selon moi, une #jarre dans le Domaine Zora# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_ZORAS_FOUNTAIN] = HintText(CustomMessage("They say that a #pot in Zora's Fountain# contains #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß ein #Krug in Zoras Quelle# #[[1]]# enthielte.",
                                                        /*french*/ "Selon moi, une #jarre dans la Fontaine Zora# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_LON_LON_RANCH] = HintText(CustomMessage("They say that a #pot in Lon Lon Ranch# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß ein #Krug auf der Lon Lon-Farm# #[[1]]# enthielte.",
                                                       /*french*/ "Selon moi, une #jarre dans le Ranch Lon Lon# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_POT_TALONS_HOUSE] = HintText(CustomMessage("They say that a #pot in Talon's Bedroom# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #Krug auf der Lon Lon-Farm# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                       /*french*/ "Selon moi, une #jarre dans le Ranch Lon Lon# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_WEB_GROTTO] = HintText(CustomMessage("They say that a #pot behind a grotto's webbing# contains #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß eine #Krug in der Hylianischen Steppe# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                      /*french*/ "Selon moi, une #jarre dans la Plaine d'Hyrule# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_POT_MUD_WALL_GROTTO] = HintText(CustomMessage("They say that a #pot walled off in a grotto# contains #[[1]]#.",
                                                       /*german*/ "Man erzählt sich, daß eine #Krug in Schloß Hyrule# #[[1]]# enthielte.",//TODO_TRANSLATE update to match
                                                       /*french*/ "Selon moi, une #jarre dans le Château d'Hyrule# contient #[[1]]#.", {QM_RED, QM_GREEN}));//TODO_TRANSLATE update to match

    hintTextTable[RHT_KOKIRI_FOREST_RUPEE] = HintText(CustomMessage("They say that a rupee in a #tranquil forest# hides #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein Rubin in einem #ruhigen Wald# #[[1]]# verstecke.",
                                                         /*french*/ "Selon moi, un rubis dans une #forêt tranquille# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/
    hintTextTable[RHT_KOKIRI_FOREST_HEART] = HintText(CustomMessage("They say that a heart in a #tranquil forest# hides #[[1]]#.",
                                                         /*german*/ "Man erzählt sich, daß ein Herz in einem #ruhigen Wald# #[[1]]# verstecke.",
                                                         /*french*/ "Selon moi, un cœur dans une #forêt tranquille# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                      // /*spanish*/
    hintTextTable[RHT_SARIAS_HOUSE_HEART] = HintText(CustomMessage("They say that a heart in a #dear friend's home# hides #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß ein Herz #im Heim eines lieben Freundes# #[[1]]# verstecke.",
                                                        /*french*/ "Selon moi, un cœur dans la #maison d'une amie chère# cache #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/
    hintTextTable[RHT_LOST_WOODS_RUPEE] = HintText(CustomMessage("They say that under a #boulder in the woods# lies #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß unter einem #Felsen in den Wäldern# #[[1]]# |läge|lägen|.",
                                                      /*french*/ "Selon moi, sous un #rocher dans les bois# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_LOST_WOODS_SHORTCUT_RUPEE] = HintText(CustomMessage("They say that in a #pool of water in the woods# lies #[[1]]#.",
                                                               /*german*/ "Man erzählt sich, daß in einem #Wasserbecken in den Wäldern# #[[1]]# |läge|lägen|.",
                                                               /*french*/ "Selon moi, dans une #mare d'eau dans les bois# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                            // /*spanish*/
    hintTextTable[RHT_LAKE_HYLIA_RUPEE] = HintText(CustomMessage("They say that just off the #coast of a lake# lies #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß vor dem #Ufer eines Sees# #[[1]]# |läge|lägen|.",
                                                      /*french*/ "Selon moi, juste au large de la #côte d'un lac# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_LABORATORY_RUPEE] = HintText(CustomMessage("They say that at the #bottom of a tank# lies #[[1]]#.",
                                                      /*german*/ "Man erzählt sich, daß auf dem #Grund eines Reservoirs# #[[1]]# |läge|lägen|.",
                                                      /*french*/ "Selon moi, au #fond d'un réservoir# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                   // /*spanish*/
    hintTextTable[RHT_DAMPES_GRAVE_RUPEE] = HintText(CustomMessage("They say that within a #quick-footed spirit's grave# lies #[[1]]#.",
                                                        /*german*/ "Man erzählt sich, daß innerhalb eines #Grabes eines leichtfüßigen Geistes# #[[1]]# |läge|lägen|.",
                                                        /*french*/ "Selon moi, dans la #tombe d'un esprit à pieds rapides# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                     // /*spanish*/
    hintTextTable[RHT_GERUDO_VALLEY_GROTTO_RUPEE] = HintText(CustomMessage("They say that an Octarok in an #underground spring# guards #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß ein Oktorok in einer #untergründigen Quelle# #[[1]]# bewache.",
                                                                /*french*/ "Selon moi, un Octrocher dans une #source souterraine# garde #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_RUPEE] = HintText(CustomMessage("They say that beneath a boulder on a #mountain's cliffside# lies #[[1]]#.",
                                                                /*german*/ "Man erzählt sich, daß unterhalb eines Felsens an der #Klippe eines Berges# #[[1]]# |läge|lägen|.",
                                                                /*french*/ "Selon moi, sous un rocher sur la #falaise d'une montagne# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                             // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_HEART] = HintText(CustomMessage("They say that accompanying a #cow in a small cave# is #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß neben einer #Kuh in einer kleinen Höhle# #[[1]]# |sei|seien|.",
                                                                       /*french*/ "Selon moi, accompagner une #vache dans une petite grotte# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_TRAIL_GROTTO_RUPEE] = HintText(CustomMessage("They say that accompanying a #cow in a small cave# is #[[1]]#.",
                                                                       /*german*/ "Man erzählt sich, daß neben einer #Kuh in einer kleinen Höhle# #[[1]]# |sei|seien|.",
                                                                       /*french*/ "Selon moi, accompagner une #vache dans une petite grotte# est #[[1]]#.", {QM_RED, QM_GREEN}));
                                                                    // /*spanish*/
    hintTextTable[RHT_DEATH_MOUNTAIN_CRATER_RUPEE] = HintText(CustomMessage("They say that on a #small platform suspended above lava# lies #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß auf einer #auf Lava schwimmenden kleinen Plattform# #[[1]]# |läge|lägen|.",
                                                                 /*french*/ "Selon moi, sur une #petite plateforme suspendue au-dessus de la lave# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/
    hintTextTable[RHT_ZORAS_RIVER_WATERFALL_RUPEE] = HintText(CustomMessage("They say that beneath a #waterfall feeding a narrow river# lies #[[1]]#.",
                                                                 /*german*/ "Man erzählt sich, daß unterhalb eines #Wasserfalls der einen schmalen Fluß nähre# #[[1]]# |läge|lägen|.",
                                                                 /*french*/ "Selon moi, sous une #cascade alimentant une rivière étroite# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                              // /*spanish*/
    hintTextTable[RHT_ZORAS_FOUNTAIN_RUPEE] = HintText(CustomMessage("They say that at the bottom of a #partially-frozen spring# lies #[[1]]#.",
                                                          /*german*/ "Man erzählt sich, daß auf dem Grund einer #teilweise gefrorenen Quelle# #[[1]]# |läge|lägen|.",
                                                          /*french*/ "Selon moi, au fond d'une #source partiellement gelée# se trouve #[[1]]#.", {QM_RED, QM_GREEN}));
                                                       // /*spanish*/

    hintTextTable[RHT_SFM_FAIRY_GROTTO_FAIRY] = HintText(CustomMessage("They say that within #a fountain beneath a forest meadow# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens unter einer Waldlichtung# #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine sous un bosquet forestier# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_FAIRY_GROTTO_FAIRY] = HintText(CustomMessage("They say that within #a fountain beneath a river# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens unter einem Fluß# #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine sous une rivière# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_FAIRY_GROTTO_FAIRY] = HintText(CustomMessage("They say that within #a fountain beneath a few trees bordering a wide field# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens, unter Bäumen auf einer weiten Steppe#, #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine sous quelques arbres bordant un vaste champ# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_FAIRY_GROTTO_FAIRY] = HintText(CustomMessage("They say that within #a fountain beneath the home of the Zoras# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens unter der Heimat der Zoras# #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine sous la maison des Zora# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GF_FAIRY_GROTTO_FAIRY] = HintText(CustomMessage("They say that within #a fountain beneath the home of thieves# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens unter der Heimat der Diebe# #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine sous la maison des voleurs# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_SHIELD_GRAVE_FAIRY] = HintText(CustomMessage("They say that within #a fountain behind a wall within a grave# rests #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß innerhalb #eines Brunnens, hinter einer Mauer eines Grabes#, #[[1]]# |ruhe|ruhen|.",
                                            /*french*/ "Selon moi, dans #une fontaine derrière un mur dans une tombe# repose #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_OASIS_FAIRY] = HintText(CustomMessage("They say that #restoring water to a dried oasis# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #ausgetrockneten Oase wieder aufzufüllen# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #restaurer l'eau dans un oasis asséché# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout on the riverside# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes am Flußufer# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse au bord de la rivière# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout near a forest shop# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes nahe des Kokiri-Ladens# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse près d'un magasin forestier# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout near the entrance to the forest# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes nahe des Waldeingangs# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse près de l'entrée de la forêt# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout above a sylvan theatre# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes oberhalb eines waldigen Theaters# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse au-dessus d'un théâtre sylvestre# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout on the lakeside# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes am Seeufer# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse au bord du lac# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout on the side of a canyon# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes an der Seite einer Schlucht# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse sur le côté d'un canyon# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout near a temple of the sand# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes nahe eines Tempels des Sandes# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse près d'un temple du sable# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout within a graveyard# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes auf einem Friedhof# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse dans un cimetière# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout within a volcano# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes innerhalb eines Vulkans# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse dans un volcan# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_BEAN_SPROUT_FAIRY] = HintText(CustomMessage("They say that #watering a young sprout on the mountainside# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Wässern eines jungen Sproßes an einem Berghang# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #arroser une jeune pousse sur le flanc d'une montagne# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TOT_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone in the outskirts of the market# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein am Rand des Marktes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange en périphérie du marché# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TOT_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone in the outskirts of the market# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein am Rand des Marktes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange en périphérie du marché# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone within a volcano# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein innerhalb eines Vulkans#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange dans un volcan# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone within a volcano# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein innerhalb eines Vulkans#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange dans un volcan# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone on a mountain cliff face# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein an einer Bergklippe#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sur une falaise de montagne# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone on a mountain cliff face# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein an einer Bergklippe#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sur une falaise de montagne# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone near the temple of the sand# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein nahe des Tempels des Sandes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange près du temple du sable# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_COLOSSUS_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone near the temple of the sand# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein nahe des Tempels des Sandes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange près du temple du sable# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone near a canyon waterfall# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein nahe eines Wasserfalls in einer Schlucht#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange près d'une cascade de canyon# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GV_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone near a canyon waterfall# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein nahe eines Wasserfalls in einer Schlucht#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange près d'une cascade de canyon# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MAZE_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone behind a maze of rock# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein hinter einem Felslabyrinth#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange derrière un labyrinthe de roches# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MAZE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone behind a maze of rock# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein hinter einem Felslabyrinth#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange derrière un labyrinthe de roches# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone near a blacksmith# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein nahe eines Schmiedes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange près d'un forgeron# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone near a blacksmith# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein nahe eines Schmiedes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange près d'un forgeron# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overlooking the graveyard# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher den Friedhof überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant le cimetière# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overlooking the graveyard# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher den Friedhof überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant le cimetière# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_MALON_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overlooking the path to the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher den Pfad zum Schloß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant le chemin du château# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_MALON_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overlooking the path to the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher den Pfad zum Schloß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant le chemin du château# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone near a secret path to the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein nahe eines geheimen Pfades zum Schloß#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange près d'un chemin secret vers le château# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_ROCK_WALL_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone near a secret path to the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein nahe eines geheimen Pfades zum Schloß#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange près d'un chemin secret vers le château# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb des Schloßes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous le château# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath the castle# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb des Schloßes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous le château# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone near an ancient tree# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein nahe eines antiken Baumes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange près d'un arbre ancien# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_DEKU_TREE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone near an ancient tree# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein nahe eines antiken Baumes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange près d'un arbre ancien# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overlooking a forest village# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher ein Dorf im Wald überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant un village forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overlooking a forest village# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher ein Dorf im Wald überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant un village forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath a forest village# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb eines Dorfes im Wald#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous un village forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath a forest village# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb eines Dorfes im Wald#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous un village forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_LAB_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overlooking the river feeding a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher einen See nährenden Fluß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant la rivière qui alimente un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_LAB_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overlooking the river feeding a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher einen See nährenden Fluß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant la rivière qui alimente un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone at the back of a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein am Rücken eines Sees#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange au fond d'un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHEAST_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone at the back of a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein am Rücken eines Sees#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange au fond d'un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone at the back of a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein am Rücken eines Sees#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange au fond d'un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_SOUTHWEST_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone at the back of a lake# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein am Rücken eines Sees#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange au fond d'un lac# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone within a perplexing wood# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein innerhalb eines verwirrenden Waldes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange dans un bois déroutant# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone within a perplexing wood# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein innerhalb eines verwirrenden Waldes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange dans un bois déroutant# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_MAZE_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overlooking a forest maze# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher ein Waldlabyrinth überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant un labyrinthe forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_MAZE_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overlooking a forest maze# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher ein Waldlabyrinth überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant un labyrinthe forestier# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_SARIA_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone watching a hiding place in the woods# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher ein Versteck im Wald überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surveillant une cachette dans les bois# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_SFM_SARIA_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone watching a hiding place in the woods# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher ein Versteck im Wald überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surveillant une cachette dans les bois# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone listening to an aquatic king# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher einem aquatischen König zuhört#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange écoutant un roi aquatique# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZD_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone listening to an aquatic king# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher einem aquatischen König zuhört#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange écoutant un roi aquatique# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone in the outskirts of a deep fountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein am Rand eines tiefen Brunnens#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange en périphérie d'une profonde fontaine# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone in the outskirts of a deep fountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein am Rand eines tiefen Brunnens#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange en périphérie d'une profonde fontaine# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_JABU_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone watching a guardian of the sea# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher einen Wächter des Meeres beobachtet#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surveillant un gardien de la mer# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZF_JABU_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone watching a guardian of the sea# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher einen Wächter des Meeres beobachtet#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surveillant un gardien de la mer# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone overwatching a river# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher einen Fluß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange surplombant une rivière# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_GROTTOS_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone overwatching a river# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher einen Fluß überblickt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange surplombant une rivière# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath a waterfall# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb eines Wasserfalls#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous une cascade# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_NEAR_DOMAIN_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath a waterfall# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb eines Wasserfalls#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous une cascade# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone hiding near a cow# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein, welcher sich nahe einer Kuh versteckt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange cachée près d'une vache# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_COW_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone hiding near a cow# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein, welcher sich nahe einer Kuh versteckt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange cachée près d'une vache# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath the entrance to the market# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb des Eingangs zum Markt# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous l'entrée du marché# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath the entrance to the market# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb des Eingangs zum Markt#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous l'entrée du marché# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath trees guarded by a Peahat# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb von Killeranas bewachten Bäumen#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous des arbres gardés par un Peahat# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_SOUTHEAST_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath trees guarded by a Peahat# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb von Killeranas bewachten Bäumen#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous des arbres gardés par un Peahat# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath a village at the base of a mountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb eines Dorfes am Fuße eines Berges#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous un village au pied d'une montagne# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath a village at the base of a mountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb eines Dorfes am Fuße eines Berges#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous un village au pied d'une montagne# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone within a plateau by a river# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein auf einem Plateau an einem Fluß#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange dans un plateau près d'une rivière# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_OPEN_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone within a plateau by a river# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein auf einem Plateau an einem Fluß#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange dans un plateau près d'une rivière# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath an escape from the forest# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb eines Auswegs des Waldes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous une issue de la forêt# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath an escape from the forest# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb eines Auswegs des Waldes#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous une issue de la forêt# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone beneath the entrance to a village within a mountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein unterhalb eines Eingangs zu einem Dorf innerhalb eines Berges#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange sous l'entrée d'un village dans une montagne# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_STORMS_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone beneath the entrance to a village within a mountain# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein unterhalb eines Eingangs zu einem Dorf innerhalb eines Berges#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange sous l'entrée d'un village dans une montagne# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY] = HintText(CustomMessage("They say that #playing a tune for an odd stone within the side of a crater# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer Melodie, für einen seltsamen Stein innerhalb der Seite eines Kraters#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie pour une pierre étrange dans le côté d'un cratère# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG] = HintText(CustomMessage("They say that #playing a stormy tune for an odd stone within the side of a crater# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Spielen einer stürmischen Melodie, für einen seltsamen Stein innerhalb der Seite eines Kraters#, #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une pierre étrange dans le côté d'un cratère# révèle [[1]].", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_ISLAND_SUN_FAIRY] = HintText(CustomMessage("They say that #summoning the sun on the lake's island# calls #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Beschwören der Sonne auf der Insel eines Sees# #[[1]]# anrufe.",
                                            /*french*/ "Selon moi, #appeler le soleil sur l'île du lac # révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_POND_STORMS_FAIRY] = HintText(CustomMessage("They say that #calling rain to the field's pond# summons #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Anrufen des Regens für den Tümpel einer Steppe# #[[1]]# beschwöre.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse dans l'étang d'une plaine# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_FENCE_GROTTO_STORMS_FAIRY] = HintText(CustomMessage("They say that #making it rain in a scrub's cave# wakes #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Machen von Regen in der Höhle eines Deku-Händlers# #[[1]]# erwecke.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse dans la grotte d'une Peste Mojo# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_FLAG_SUN_FAIRY] = HintText(CustomMessage("They say that #changing the time in front of the trail's flag# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Verändern der Zeit im Angesicht der Flagge eines Pfades# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #appeler le soleil devant un drapeau du Mont du Péril # révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_DMT_COW_GROTTO_STORMS_FAIRY] = HintText(CustomMessage("They say that #calling a storm for a lonely cow# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Anrufen eines Sturms für eine einsame Kuh# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse pour une vache solitaire# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_SHORTCUT_STORMS_FAIRY] = HintText(CustomMessage("They say that #making it rain in the Lost Woods# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Machen von Regen in den verlorenen Wäldern# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #jouer une mélodie orageuse dans les Bois Perdus# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_TH_KITCHEN_SUN_FAIRY] = HintText(CustomMessage("They say that #calling the sun in a guarded kitchen# exposes #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Anrufen der Sonne in einer bewachten Küche# #[[1]]# freilege.",
                                            /*french*/ "Selon moi, #appeler le soleil dans une cuisine gardée# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_DEKU_SCRUB_GROTTO_SUN_FAIRY] = HintText(CustomMessage("They say that #calling the sun for scrubs in the Lost Woods# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Anrufen der Sonne für Deku-Händler in den verlorenen Wäldern# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #appeler le soleil pour les Pestes dans les Bois Perdus # révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GRAVEYARD_ROYAL_FAMILYS_TOMB_SUN_FAIRY] = HintText(CustomMessage("They say that #calling the sun in a royal tomb# reveals #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß das #Anrufen der Sonne in einer königlichen Gruft# #[[1]]# enthülle.",
                                            /*french*/ "Selon moi, #appeler le soleil dans une tombe royale# révèle #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KF_GRASS] = HintText(CustomMessage("They say that #cutting some grass in a forest# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in einem Wald# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans un fôret# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LW_GRASS] = HintText(CustomMessage("They say that #cutting some grass in the woods# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in den Wäldern# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans des bois# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_MARKET_GRASS] = HintText(CustomMessage("They say that #cutting some grass in the market# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras auf dem Markt# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans un marché# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HC_GRASS] = HintText(CustomMessage("They say that #cutting some grass near the castle# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in der Nähe des Schloßes# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe près du château# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_KAK_GRASS] = HintText(CustomMessage("They say that #cutting some grass in a village# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in einem Dorf# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans un village# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GY_GRASS] = HintText(CustomMessage("They say that #cutting some grass in a graveyard# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras auf einem Friedhof# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans un cimetère# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_LH_GRASS] = HintText(CustomMessage("They say that #cutting some grass near a lake# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras nahe eines Sees# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe près d'un lac# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_HF_GRASS] = HintText(CustomMessage("They say that #cutting some grass on a field# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras auf einer Steppe# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe dans une plaine# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_ZR_GRASS] = HintText(CustomMessage("They say that #cutting some grass near a river# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in der Nähe eines Flußes# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe près d'une rivière# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_GROTTO_GRASS] = HintText(CustomMessage("They say that #cutting some grass in a grotto# reveals #[[1]]#.",
                            /*german*/ "Man erzählt sich, daß das #Mähen von Gras in einer Grotte# #[[1]]# enthülle.",
                            /*french*/ "Selon moi, de l'#herbe# cache #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GERUDO_VALLEY] = HintText(CustomMessage("They say that a #crate in Gerudo Valley# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste im Gerudotal# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans une vallée# a #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GERUDOS_FORTRESS] = HintText(CustomMessage("They say that a #crate in Gerudo Fortress# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste in der Gerudo-Festung# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans la Forteresse Gerudo# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_WASTELAND] = HintText(CustomMessage("They say that a #crate in Haunted Wasteland# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste in der Gespensterwüste# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans le Désert Hanté# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_MARKET] = HintText(CustomMessage("They say that a #crate in the Market# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste auf dem Markt# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans la Place du Marché# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_KAKARIKO_VILLAGE] = HintText(CustomMessage("They say that a #crate in Kakariko Village# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste in Kakariko# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans le Village de Cocorico# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GRAVEYARD] = HintText(CustomMessage("They say that a #crate in the Graveyard# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste auf dem Friedhof# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans le Cimetière# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_GORON_CITY] = HintText(CustomMessage("They say that a #crate in Goron City# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste in Goronia# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans le Village Goron# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_DEATH_MOUNTAIN_CRATER] = HintText(CustomMessage("They say that a #crate in Death Mountain Crater# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste im Todeskrater# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse sur le Mont du Péril# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_LON_LON_RANCH] = HintText(CustomMessage("They say that a #crate in Lon Lon Ranch# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste auf der Lon Lon-Farm# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans le Ranch Lon Lon# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    hintTextTable[RHT_CRATE_LAKESIDE_LABORATORY] = HintText(CustomMessage("They say that a #crate in the Laboratory# contains #[[1]]#.",
                                            /*german*/ "Man erzählt sich, daß eine #Kiste im Labor am See# #[[1]]# enthielte.",
                                            /*french*/ "Selon moi, une #caisse dans un laboratoire# contient #[[1]]#.", {QM_RED, QM_GREEN}));

    // clang-format on
}
} // namespace Rando
