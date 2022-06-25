#include "../hint_list.hpp"

void HintTable_Init_Exclude_Overworld() {
    hintTable[KF_KOKIRI_SWORD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #hidden treasure of the Kokiri# is", /*french*/"le #tr�sor des Kokiri# est", /*spanish*/"el #tesoro oculto de los Kokiri# esconde"},
  });

    hintTable[KF_MIDOS_TOP_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# poss�de", /*spanish*/"el #l�der de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# g�t", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_TOP_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# poss�de", /*spanish*/"el #l�der de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# g�t", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_BOTTOM_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# poss�de", /*spanish*/"el #l�der de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# g�t", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_BOTTOM_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# poss�de", /*spanish*/"el #l�der de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# g�t", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[GRAVEYARD_SHIELD_GRAVE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure of a fallen soldier# is", /*french*/"le #tr�sor du soldat mort# est", /*spanish*/"el #tesoro de un soldado ca�do# esconde"},
  });

    hintTable[DMT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"hidden behind a wall on a #mountain trail# is", /*french*/"derri�re une fa�ade du #chemin montagneux# est", /*spanish*/"tras una pared del #sendero de la monta�a# yace"},
  });

    hintTable[GC_MAZE_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in #Goron City# explosives unlock", /*french*/"des explosions dans le #village Goron# r�v�lent", /*spanish*/"en la #Ciudad Goron# unos explosivos desbloquean"},
  });

    hintTable[GC_MAZE_CENTER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in #Goron City# explosives unlock", /*french*/"des explosions dans le #village Goron# r�v�lent", /*spanish*/"en la #Ciudad Goron# unos explosivos desbloquean"},
  });

    hintTable[ZD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"fire #beyond a waterfall# reveals", /*french*/"du feu #derri�re la cascade# �claire", /*spanish*/"las #llamas tras una una cascada# revelan"},
  });

    hintTable[GRAVEYARD_HOOKSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a chest hidden by a #speedy spectre# holds", /*french*/"le #coffre du rapide revenant# contient", /*spanish*/"un cofre custodiado por un #espectro veloz# contiene"},
                     }, {},
                       //clear text
                       Text{"#dead Damp�'s first prize# is", /*french*/"la #premi�re course d'Igor# donne", /*spanish*/"el primer premio de #la carrera de Damp�# se trata de"}
  );

    hintTable[GF_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"on a #rooftop in the desert# lies", /*french*/"sur un #toit du d�sert# g�t", /*spanish*/"en una #azotea del desierto# yace"},
  });

    hintTable[KAK_REDEAD_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#zombies beneath the earth# guard", /*french*/"les #revenants sous terre# prot�gent", /*spanish*/"unos #zombis subterr�neos# esconden"},
  });

    hintTable[SFM_WOLFOS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#wolves beneath the earth# guard", /*french*/"les #loups sous terre# prot�gent", /*spanish*/"unos #lobos subterr�neos# esconden"},
  });

    hintTable[HF_NEAR_MARKET_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a field near a drawbridge# holds", /*french*/"la #grotte pr�s d'un pont# contient", /*spanish*/"bajo el #hoyo de una llanura cercano a un puente# yace"},
  });

    hintTable[HF_SOUTHEAST_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole amongst trees in a field# holds", /*french*/"la #grotte pr�s des arbres# contient", /*spanish*/"bajo el #hoyo de una llanura rodeado de �rboles# yace"},
  });

    hintTable[HF_OPEN_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #open hole in a field# holds", /*french*/"la #grotte dans les plaines# contient", /*spanish*/"bajo el #hoyo descubierto de una llanura# yace"},
  });

    hintTable[KAK_OPEN_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #open hole in a town# holds", /*french*/"la #grotte dans le village# contient", /*spanish*/"bajo el #hoyo descubierto de un pueblo# yace"},
  });

    hintTable[ZR_OPEN_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole along a river# holds", /*french*/"la #grotte pr�s du fleuve# contient", /*spanish*/"bajo un #hoyo junto a un r�o# yace"},
  });

    hintTable[KF_STORMS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a forest village# holds", /*french*/"la #grotte inond�e de pluie dans le Village Kokiri# r�v�le", /*spanish*/"bajo el #hoyo de una tribu del bosque# yace"},
  });

    hintTable[LW_NEAR_SHORTCUTS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a wooded maze# holds", /*french*/"la #grotte dans le labyrinthe sylvestre# contient", /*spanish*/"bajo un #hoyo de un laberinto forestal# yace"},
  });

    hintTable[DMT_STORMS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#hole flooded with rain on a mountain# holds", /*french*/"la #grotte inond�e de pluie sur la montagne# contient", /*spanish*/"bajo un #hoyo de una monta�a inundado de lluvia# yace"},
  });

    hintTable[DMC_UPPER_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a volcano# holds", /*french*/"la #grotte dans le volcan# contient", /*spanish*/"bajo el #hoyo de un volc�n# yace"},
  });


    hintTable[TOT_LIGHT_ARROWS_CUTSCENE] = HintText::Exclude({
                       //obscure text
                       Text{"the #final gift of a princess# is", /*french*/"le #cadeau d'adieu de la princesse# est", /*spanish*/"el #obsequio final de la princesa# se trata de"},
  });

    hintTable[LW_GIFT_FROM_SARIA] = HintText::Exclude({
                       //obscure text
                       Text{"a #potato hoarder# holds",           /*french*/"le #panini m�lodieux# est en fait", /*spanish*/"cierta #jovencita verde# concede"},
                       Text{"a rooty tooty #flutey cutey# gifts", /*french*/"la #patate musicale# est en fait",  /*spanish*/"una #gran amiga# concede"},
                     }, {},
                       //clear text
                       Text{"#Saria's Gift# is", /*french*/"le #cadeau de Saria# est", /*spanish*/"el #regalo de Saria# se trata de"}
  );

    hintTable[ZF_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of winds# holds", /*french*/"la #f�e du vent# a", /*spanish*/"el #hada del viento# brinda"},
  });

    hintTable[HC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of fire# holds", /*french*/"la #f�e du feu# a", /*spanish*/"el #hada del fuego# brinda"},
  });

    hintTable[COLOSSUS_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of love# holds", /*french*/"la #f�e de l'amour# a", /*spanish*/"el #hada del amor# brinda"},
  });

    hintTable[DMT_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #magical fairy# gifts", /*french*/"la #f�e de la magie# a", /*spanish*/"un #hada m�gica# brinda"},
  });

    hintTable[DMC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #magical fairy# gifts", /*french*/"la #f�e de la magie# a", /*spanish*/"un #hada m�gica# brinda"},
  });

    hintTable[OGC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of strength# holds", /*french*/"la #f�e de la force# a", /*spanish*/"el #hada de la fuerza# brinda"},
  });


    hintTable[SONG_FROM_IMPA] = HintText::Exclude({
                       //obscure text
                       Text{"#deep in a castle#, Impa teaches", /*french*/"#la gardienne de la princesse# donne", /*spanish*/"en el #jard�n del castillo Impa ense�a#"},
  });

    hintTable[SONG_FROM_MALON] = HintText::Exclude({
                       //obscure text
                       Text{"#a farm girl# sings", /*french*/"la #fillette de la ferme# donne", /*spanish*/"una #chica rupestre# canta"},
  });

    hintTable[SONG_FROM_SARIA] = HintText::Exclude({
                       //obscure text
                       Text{"#deep in the forest#, Saria teaches", /*french*/"la #fille de la for�t# donne", /*spanish*/"al #fondo del bosque# Saria ense�a"},
  });

    hintTable[SONG_FROM_WINDMILL] = HintText::Exclude({
                       //obscure text
                       Text{"a man #in a windmill# is obsessed with", /*french*/"l'#homme du moulin# donne", /*spanish*/"el #hombre del molino# est� obsesionado con"},
  });


    hintTable[HC_MALON_EGG] = HintText::Exclude({
                       //obscure text
                       Text{"a #girl looking for her father# gives", /*french*/"la #fillette qui cherche son p�re# donne", /*spanish*/"una #chica en busca de su padre# otorga"},
  });

    hintTable[HC_ZELDAS_LETTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #princess in a castle# gifts", /*french*/"la #princesse dans le ch�teau# donne", /*spanish*/"la #princesa de un castillo# otorga"},
  });

    hintTable[ZD_DIVING_MINIGAME] = HintText::Exclude({
                       //obscure text
                       Text{"an #unsustainable business model# gifts", /*french*/"le #mauvais mod�le d'affaires# donne", /*spanish*/"un #mal modelo de negocio# premia con"},
                     }, {},
                       //clear text
                       Text{"those who #dive for Zora rupees# will find", /*french*/"ceux qui #plongent pour des rubis Zora# trouveront", /*spanish*/"aquellos que se #sumergan por las rupias zora# encontrar�n"}
  );

    hintTable[LH_CHILD_FISHING] = HintText::Exclude({
                       //obscure text
                       Text{"#fishing in youth# bestows", /*french*/"#p�cher dans sa jeunesse# promet", /*spanish*/"#pescar en la juventud# conduce a"},
  });

    hintTable[LH_ADULT_FISHING] = HintText::Exclude({
                       //obscure text
                       Text{"#fishing in maturity# bestows", /*french*/"#p�cher dans sa maturit�# promet", /*spanish*/"#pescar en la madurez# conduce a"},
  });

    hintTable[LH_LAB_DIVE] = HintText::Exclude({
                       //obscure text
                       Text{"a #diving experiment# is rewarded with", /*french*/"l'#exp�rience de plong�e# donne", /*spanish*/"#bucear para un experimento# se premia con"},
  });

    hintTable[GC_ROLLING_GORON_AS_ADULT] = HintText::Exclude({
                       //obscure text
                       Text{"#comforting yourself# provides", /*french*/"se #r�conforter soi-m�me# donne", /*spanish*/"#confrontarte a ti mismo# otorga"},
                     }, {},
                       //clear text
                       Text{"#reassuring a young Goron# is rewarded with", /*french*/"#rassurer un jeune Goron# donne", /*spanish*/"#calmar a un joven Goron# otorga"}
  );

    hintTable[MARKET_BOMBCHU_BOWLING_FIRST_PRIZE] = HintText::Exclude({
                       //obscure text
                       Text{"the #first explosive prize# is", /*french*/"le #premier prix explosif# est", /*spanish*/"el #primer premio explosivo# se trata de"},
  });

    hintTable[MARKET_BOMBCHU_BOWLING_SECOND_PRIZE] = HintText::Exclude({
                       //obscure text
                       Text{"the #second explosive prize# is", /*french*/"le #deuxi�me prix explosif# est", /*spanish*/"el #segundo premio explosivo# se trata de"},
  });

    hintTable[MARKET_LOST_DOG] = HintText::Exclude({
                       //obscure text
                       Text{"#puppy lovers# will find", /*french*/"les #amoureux canins# trouveront", /*spanish*/"los #amantes caninos# encontrar�n"},
                     }, {},
                       //clear text
                       Text{"#rescuing Richard the Dog# is rewarded with", /*french*/"#retrouver Kiki le chien# promet", /*spanish*/"#rescatar al perrito Ricardo# conduce a"}
  );

    hintTable[LW_OCARINA_MEMORY_GAME] = HintText::Exclude({
                       //obscure text
                       Text{"the prize for a #game of Simon Says# is", /*french*/"la #r�compense de Jean Dit# est", /*spanish*/"#repetir ciertas melod�as# otorga"},
                       Text{"a #child sing-a-long# holds",             /*french*/"les #jeunes fl�tistes# donnent",  /*spanish*/"#tocar junto a otros# otorga"},
                     }, {},
                       //clear text
                       Text{"#playing an Ocarina in Lost Woods# is rewarded with", /*french*/"#jouer l'ocarina dans la for�t des m�andres# donne", /*spanish*/"#tocar la ocarina en el Bosque Perdido# otorga"}
  );

    hintTable[KAK_10_GOLD_SKULLTULA_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#10 bug badges# rewards",           /*french*/"#10 �cussons# donnent",             /*spanish*/"#10 medallas de insectos# otorgan"},
                       Text{"#10 spider souls# yields",          /*french*/"#10 �mes# donnent",                 /*spanish*/"#10 almas de ara�a# otorgan"},
                       Text{"#10 auriferous arachnids# lead to", /*french*/"#10 arachnides aurif�res# donnent", /*spanish*/"#10 ar�cnidos aur�feros# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #10 Gold Skulltulas# reveals", /*french*/"d�truire #10 Skulltulas d'or# donne", /*spanish*/"#exterminar 10 skulltulas doradas# revela"}
  );

    hintTable[KAK_MAN_ON_ROOF] = HintText::Exclude({
                       //obscure text
                       Text{"a #rooftop wanderer# holds", /*french*/"une #rencontre sur un toit# donne", /*spanish*/"#alguien sobre un tejado# otorga"},
  });

    hintTable[ZR_MAGIC_BEAN_SALESMAN] = HintText::Exclude({
                       //obscure text
                       Text{"a seller of #colorful crops# has", /*french*/"le #marchand de l�gumes# vend", /*spanish*/"el vendedor de un #colorido cultivo# ofrece"},
                     }, {},
                       //clear text
                       Text{"a #bean seller# offers", /*french*/"le #marchand de haricots magiques# vend en fait", /*spanish*/"el #vendedor de jud�as# ofrece"}
  );

    hintTable[ZR_FROGS_IN_THE_RAIN] = HintText::Exclude({
                       //obscure text
                       Text{"#frogs in a storm# gift", /*french*/"#des grenouilles mouill�es# donnent", /*spanish*/"las #ancas bajo la tormenta# otorgan"},
  });

    hintTable[GF_HBA_1000_POINTS] = HintText::Exclude({
                       //obscure text
                       Text{"scoring 1000 in #horseback archery# grants", /*french*/"obtenir 1000 points dans l'#archerie �questre# donne", /*spanish*/"conseguir 1000 puntos en el #tiro con arco a caballo# premia"},
  });

    hintTable[MARKET_SHOOTING_GALLERY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#shooting in youth# grants", /*french*/"#faire du tir dans sa jeunesse# donne", /*spanish*/"#disparar en la juventud# otorga"},
  });

    hintTable[KAK_SHOOTING_GALLERY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#shooting in maturity# grants", /*french*/"#faire du tir dans sa maturit�# donne", /*spanish*/"#disparar en la madurez# otorga"},
  });

    hintTable[LW_TARGET_IN_WOODS] = HintText::Exclude({
                       //obscure text
                       Text{"shooting a #target in the woods# grants", /*french*/"#tirer une cible dans les bois# donne", /*spanish*/"disparar a un #blanco forestal# brinda"},
  });

    hintTable[KAK_ANJU_AS_ADULT] = HintText::Exclude({
                       //obscure text
                       Text{"a #chicken caretaker# offers adults", /*french*/"devenir un #�leveur de Cocottes# donne", /*spanish*/"una #cuidadora de emplumados# le ofrece a los mayores"},
  });

    hintTable[LLR_TALONS_CHICKENS] = HintText::Exclude({
                       //obscure text
                       Text{"#finding Super Cuccos# is rewarded with", /*french*/"#trouver des Super Cocottes# donne", /*spanish*/"#hallar los supercucos# conduce a"},
  });

    hintTable[GC_ROLLING_GORON_AS_CHILD] = HintText::Exclude({
                       //obscure text
                       Text{"the prize offered by a #large rolling Goron# is", /*french*/"la r�compense d'un #gros Goron roulant# est", /*spanish*/"un #gran Goron rodante# otorga"},
  });

    hintTable[LH_UNDERWATER_ITEM] = HintText::Exclude({
                       //obscure text
                       Text{"the #sunken treasure in a lake# is", /*french*/"le #tr�sor au fond du lac# est", /*spanish*/"el #tesoro hundido del lago# se trata de"},
  });

    hintTable[GF_GERUDO_TOKEN] = HintText::Exclude({
                       //obscure text
                       Text{"#rescuing captured carpenters# is rewarded with", /*french*/"#secourir les charpentiers captur�s# assure", /*spanish*/"#rescatar los apresados carpinteros# se premia con"},
  });

    hintTable[WASTELAND_BOMBCHU_SALESMAN] = HintText::Exclude({
                       //obscure text
                       Text{"a #carpet guru# sells", /*french*/"#un marchand du d�sert# vend", /*spanish*/"el #genio de una alfombra# vende"},
  });


    hintTable[KAK_IMPAS_HOUSE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#imprisoned in a house# lies", /*french*/"#encag� dans une maison# g�t", /*spanish*/"#en una casa entre rejas# yace"},
  });

    hintTable[HF_TEKTITE_GROTTO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#deep underwater in a hole# is", /*french*/"#dans les profondeurs d'une grotte# g�t", /*spanish*/"#en lo hondo bajo un hoyo# yace"},
  });

    hintTable[KAK_WINDMILL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on a #windmill ledge# lies", /*french*/"#haut perch� dans le moulin# g�t", /*spanish*/"al #borde de un molino# yace"},
  });

    hintTable[GRAVEYARD_DAMPE_RACE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#racing a ghost# leads to", /*french*/"le d�fi du #revenant rapide# donne", /*spanish*/"#perseguir a un fantasma# conduce a"},
                     }, {},
                       //clear text
                       Text{"#dead Dampe's second# prize is", /*french*/"la #deuxi�me course d'Igor# donne", /*spanish*/"el segundo premio de #la carrera de Damp�# se trata de"}
  );

    hintTable[LLR_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"in a #ranch silo# lies", /*french*/"#dans l'entrep�t de la ferme# g�t", /*spanish*/"en un #granero rupestre# yace"},
  });

    hintTable[GRAVEYARD_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"a #crate in a graveyard# hides", /*french*/"#la bo�te dans le cimeti�re# contient", /*spanish*/"bajo la #caja de un cementerio# yace"},
  });

    hintTable[GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR] = HintText::Exclude({
                       //obscure text
                       Text{"a #gravekeeper digs up#", /*french*/"#le jeu du fossoyeur# cache", /*spanish*/"cierto #sepultero desentierra#"},
  });

    hintTable[ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on top of a #pillar in a river# lies", /*french*/"#sur un pilier au dessus du fleuve# g�t", /*spanish*/"en lo alto del #pilar de un r�o# yace"},
  });

    hintTable[ZR_NEAR_DOMAIN_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on a #river ledge by a waterfall# lies", /*french*/"#sur la falaise au dessus du fleuve# g�t", /*spanish*/"al borde de #la entrada a una cascada# yace"},
  });

    hintTable[LH_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"high on a #lab rooftop# one can find", /*french*/"#la tour d'observation du lac# cache", /*spanish*/"en lo #alto de un laboratorio# yace"},
  });

    hintTable[ZF_ICEBERG_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#floating on ice# is", /*french*/"#gisant sur la glace# g�t", /*spanish*/"#flotando sobre hielo# yace"},
  });

    hintTable[GV_WATERFALL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"behind a #desert waterfall# is", /*french*/"#derri�re la cascade du d�sert# se cache", /*spanish*/"tras una #desierta cascada# yace"},
  });

    hintTable[GV_CRATE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"a #crate in a valley# hides", /*french*/"la #bo�te dans la vall�e# contient", /*spanish*/"bajo la #caja de un valle# yace"},
  });

    hintTable[COLOSSUS_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on top of an #arch of stone# lies", /*french*/"#gisant sur une arche de pierre# g�t", /*spanish*/"en lo alto de un #arco de piedra# yace"},
  });

    hintTable[DMT_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"above a #mountain cavern entrance# is", /*french*/"gisant #au dessus de la caverne montagneuse# g�t", /*spanish*/"en lo alto de la #entrada de una cueva en la monta�a# yace"},
  });

    hintTable[DMC_WALL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"nestled in a #volcanic wall# is", /*french*/"dans une #alcove volcanique# g�t", /*spanish*/"entre unas #murallas volc�nicas# yace"},
  });

    hintTable[DMC_VOLCANO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"obscured by #volcanic ash# is", /*french*/"#recouvert de cendres volcaniques# g�t", /*spanish*/"bajo la #ceniza volc�nica# yace"},
  });

    hintTable[GF_NORTH_F1_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #ge�liers Gerudo# d�tiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_NORTH_F2_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #ge�liers Gerudo# d�tiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_SOUTH_F1_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #ge�liers Gerudo# d�tiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_SOUTH_F2_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #ge�liers Gerudo# d�tiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });


    hintTable[HF_GS_NEAR_KAK_GROTTO] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-guarded spider in a hole# hoards", /*french*/"une #Skulltula dans un trou d'arachnides# a", /*spanish*/"una #Skulltula custodiada por otra# de un hoyo otorga"},
  });


    hintTable[LLR_GS_BACK_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a ranch# holding", /*french*/"une #Skulltula sur la fa�ade de la ferme# a", /*spanish*/"la noche revela una #Skulltula del rancho# que otorga"},
  });

    hintTable[LLR_GS_RAIN_SHED] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a ranch# holding", /*french*/"une #Skulltula sur le mur de l'enclos# a", /*spanish*/"la noche revela una #Skulltula del rancho# que otorga"},
  });

    hintTable[LLR_GS_HOUSE_WINDOW] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a ranch# holding", /*french*/"une #Skulltula sur la maison de ferme# a", /*spanish*/"la noche revela una #Skulltula del rancho# que otorga"},
  });

    hintTable[LLR_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #ranch tree# holds", /*french*/"une #Skulltula dans l'arbre de la ferme# a", /*spanish*/"una Skulltula escondida en el #�rbol de un rancho# otorga"},
  });


    hintTable[KF_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a forest# holds", /*french*/"une #Skulltula enterr�e dans la for�t# a", /*spanish*/"una #Skulltula enterrada en un bosque# otorga"},
  });

    hintTable[KF_GS_KNOW_IT_ALL_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a forest# holding", /*french*/"une #Skulltula derri�re une cabane de la for�t# a", /*spanish*/"la noche revela en el pasado una #Skulltula del bosque# que otorga"},
  });

    hintTable[KF_GS_HOUSE_OF_TWINS] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a forest# holding", /*french*/"une #Skulltula sur une cabane de la for�t# a", /*spanish*/"la noche revela en el futuro una #Skulltula del rancho# que otorga"},
  });


    hintTable[LW_GS_BEAN_PATCH_NEAR_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried deep in a forest maze# holds", /*french*/"une #Skulltula enterr�e dans les bois# a", /*spanish*/"una #Skulltula enterrada en un laberinto forestal# otorga"},
  });

    hintTable[LW_GS_BEAN_PATCH_NEAR_THEATER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried deep in a forest maze# holds", /*french*/"une #Skulltula enterr�e dans les bois# a", /*spanish*/"una #Skulltula enterrada en un laberinto forestal# otorga"},
  });

    hintTable[LW_GS_ABOVE_THEATER] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in a forest maze# holding", /*french*/"une #Skulltula haut perch�e dans les bois# a", /*spanish*/"la noche revela una #Skulltula del laberinto forestal# que otorga"},
  });

    hintTable[SFM_GS] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a forest meadow# holding", /*french*/"une #Skulltula dans le sanctuaire des bois# a", /*spanish*/"la noche revela una #Skulltula de la pradera del bosque# que otorga"},
  });


    hintTable[OGC_GS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider outside a tyrant's tower# holds", /*french*/"une #Skulltula parmi les ruines du ch�teau# a", /*spanish*/"una #Skulltula a las afueras de la torre de un tirano# otorga"},
  });

    hintTable[HC_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree outside of a castle# holds", /*french*/"une #Skulltula dans l'arbre pr�s du ch�teau# a", /*spanish*/"una Skulltula escondida en el #�rbol de las afueras de un castillo# otorga"},
  });

    hintTable[MARKET_GS_GUARD_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a guarded crate# holds", /*french*/"une #Skulltula dans une bo�te en ville# a", /*spanish*/"una #Skulltula bajo una custodiada caja# otorga"},
  });


    hintTable[DMC_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a volcano# holds", /*french*/"une #Skulltula enterr�e dans un volcan# a", /*spanish*/"una #Skulltula enterrada en un volc�n# otorga"},
  });


    hintTable[DMT_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried outside a cavern# holds", /*french*/"une #Skulltula enterr�e pr�s d'une caverne# a", /*spanish*/"una #Skulltula enterrada a la entrada de una cueva# otorga"},
  });

    hintTable[DMT_GS_NEAR_KAK] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider hidden in a mountain nook# holds", /*french*/"une #Skulltula cach�e dans le flanc d'une montagne# a", /*spanish*/"una #Skulltula oculta en el rinc�n de la monta�a# otorga"},
  });

    hintTable[DMT_GS_ABOVE_DODONGOS_CAVERN] = HintText::Exclude({
                       //obscure text
                       Text{"the hammer reveals a #spider on a mountain# holding", /*french*/"une #Skulltula derri�re un rocher massif pr�s d'une caverne# a", /*spanish*/"el martillo revela #una Skulltula de la monta�a# que otorga"},
  });

    hintTable[DMT_GS_FALLING_ROCKS_PATH] = HintText::Exclude({
                       //obscure text
                       Text{"the hammer reveals a #spider on a mountain# holding", /*french*/"une #Skulltula derri�re un rocher massif pr�s du sommet d'un volcan# a", /*spanish*/"el martillo revela #una Skulltula de la monta�a# que otorga"},
  });


    hintTable[GC_GS_CENTER_PLATFORM] = HintText::Exclude({
                       //obscure text
                       Text{"a #suspended spider# in Goron City holds", /*french*/"une #Skulltula perch�e dans le village Goron# a", /*spanish*/"una #Skulltula suspendida# en la Ciudad Goron otorga"},
  });

    hintTable[GC_GS_BOULDER_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in a #Goron City crate# holds", /*french*/"une #Skulltula dans une bo�te du village Goron# a", /*spanish*/"una #Skulltula bajo una caja# de la Ciudad Goron otorga"},
  });


    hintTable[KAK_GS_HOUSE_UNDER_CONSTRUCTION] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula dans le chantier de construction# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_SKULLTULA_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula sur une maison maudite# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_GUARDS_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula sur une maison de village# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula dans un arbre de village# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_WATCHTOWER] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula sur une �chelle dans un village# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_ABOVE_IMPAS_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a town# holding", /*french*/"une #Skulltula au dessus d'une grande maison# a", /*spanish*/"la noche del futuro revela una #Skulltula del pueblo# que otorga"},
  });


    hintTable[GRAVEYARD_GS_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a graveyard# holding", /*french*/"une #Skulltula sur une fa�ade du cimeti�re# a", /*spanish*/"la noche revela una #Skulltula del cementerio# que otorga"},
  });

    hintTable[GRAVEYARD_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a graveyard# holds", /*french*/"une #Skulltula enterr�e dans le cimeti�re# a", /*spanish*/"una #Skulltula enterrada en el cementerio# otorga"},
  });


    hintTable[ZR_GS_LADDER] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une �chelle pr�s d'une cascade# a", /*spanish*/"la noche del pasado revela una #Skulltula del r�o# que otorga"},
  });

    hintTable[ZR_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree by a river# holds", /*french*/"une #Skulltula dans un arbre pr�s du fleuve# a", /*spanish*/"una Skulltula escondida en el #�rbol de un r�o# otorga"},
  });

    hintTable[ZR_GS_ABOVE_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une fa�ade pr�s d'une cascade# a", /*spanish*/"la noche del futuro revela una #Skulltula del r�o# que otorga"},
  });

    hintTable[ZR_GS_NEAR_RAISED_GROTTOS] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une fa�ade pr�s d'une grotte du fleuve# a", /*spanish*/"la noche del futuro revela una #Skulltula del r�o# que otorga"},
  });


    hintTable[ZD_GS_FROZEN_WATERFALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a frozen waterfall# holding", /*french*/"une #Skulltula pr�s d'une cascade gel�e# a", /*spanish*/"la noche revela una #Skulltula junto a una congelada cascada# que otorga"},
  });

    hintTable[ZF_GS_ABOVE_THE_LOG] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider near a deity# holding", /*french*/"une #Skulltula pr�s du gardien aquatique# a", /*spanish*/"la noche revela una #Skulltula junto a cierta deidad# que otorga"},
  });

    hintTable[ZF_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree near a deity# holds", /*french*/"une #Skulltula dans un arbre dans un r�servoir# a", /*spanish*/"una Skulltula escondida en el #�rbol junto a cierta deidad# otorga"},
  });


    hintTable[LH_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried by a lake# holds", /*french*/"une #Skulltula enterr�e pr�s d'un lac# a", /*spanish*/"una #Skulltula enterrada junto a un lago# otorga"},
  });

    hintTable[LH_GS_SMALL_ISLAND] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake# holding", /*french*/"une #Skulltula sur un �lot du lac# a", /*spanish*/"la noche revela una #Skulltula junto a un lago# que otorga"},
  });

    hintTable[LH_GS_LAB_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake# holding", /*french*/"une #Skulltula sur le mur d'un centre de recherche# a", /*spanish*/"la noche revela una #Skulltula junto a un lago# que otorga"},
  });

    hintTable[LH_GS_LAB_CRATE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider deed underwater in a #lab crate# holds", /*french*/"une #Skulltula dans une bo�te au fond d'une cuve d'eau# a", /*spanish*/"una #Skulltula bajo la sumergida caja de un laboratorio# otorga"},
  });

    hintTable[LH_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake high in a tree# holding", /*french*/"une #Skulltula dans un grand arbre du lac# a", /*spanish*/"la noche revela #una Skulltula del lago sobre un �rbol# que otorga"},
  });


    hintTable[GV_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a valley# holds", /*french*/"une #Skulltula enterr� dans une vall�e# a", /*spanish*/"una #Skulltula enterrada en un valle# otorga"},
  });

    hintTable[GV_GS_SMALL_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a valley# holding", /*french*/"une #Skulltula au dessus d'une petite cascade# a", /*spanish*/"la noche del pasado revela una #Skulltula del valle# que otorga"},
  });

    hintTable[GV_GS_PILLAR] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a valley# holding", /*french*/"une #Skulltula sur une arche de pierre dans une vall�e# a", /*spanish*/"la noche del futuro revela una #Skulltula del valle# que otorga"},
  });

    hintTable[GV_GS_BEHIND_TENT] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a valley# holding", /*french*/"une #Skulltula derri�re une tente# a", /*spanish*/"la noche del futuro revela una #Skulltula del valle# que otorga"},
  });


    hintTable[GF_GS_ARCHERY_RANGE] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a fortress# holding", /*french*/"une #Skulltula sur une cible de tir# a", /*spanish*/"la noche revela una #Skulltula de una fortaleza# que otorga"},
  });

    hintTable[GF_GS_TOP_FLOOR] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a fortress# holding", /*french*/"une #Skulltula au sommet d'une forteresse# a", /*spanish*/"la noche revela una #Skulltula de una fortaleza# que otorga"},
  });


    hintTable[COLOSSUS_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in the desert# holds", /*french*/"une #Skulltula enterr�e au pied du colosse# a", /*spanish*/"una #Skulltula enterrada en el desierto# otorga"},
  });

    hintTable[COLOSSUS_GS_HILL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in the desert# holding", /*french*/"une #Skulltula sur une colline dans le d�sert# a", /*spanish*/"la noche revela una #Skulltula en las profundidades del desierto# que otorga"},
  });

    hintTable[COLOSSUS_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in the desert# holding", /*french*/"une #Skulltula dans un arbre du d�sert# a", /*spanish*/"la noche revela una #Skulltula en las profundidades del desierto# que otorga"},
  });


    hintTable[KF_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });

    hintTable[KF_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #child shopkeeper# sells", /*french*/"la #boutique Kokiri# vend", /*spanish*/"un #joven dependiente# vende"},
  });


    hintTable[KAK_POTION_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );

    hintTable[KAK_POTION_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Kakariko Potion Shop# offers", /*french*/"l'#apothicaire de Kakariko# vend", /*spanish*/"la #tienda de pociones de Kakariko# ofrece"}
  );


    hintTable[MARKET_BOMBCHU_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });


    hintTable[MARKET_POTION_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du march�# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );


    hintTable[MARKET_BAZAAR_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du march�# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });


    hintTable[KAK_BAZAAR_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });

    hintTable[KAK_BAZAAR_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"the #Kakariko Bazaar# offers", /*french*/"le #bazar de Kakariko# vend", /*spanish*/"el #bazar de Kakariko# ofrece"},
  });


    hintTable[ZD_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });

    hintTable[ZD_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #Zora shopkeeper# sells", /*french*/"la #boutique Zora# vend", /*spanish*/"el #dependiente Zora# vende"},
  });


    hintTable[GC_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });

    hintTable[GC_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron shopkeeper# sells", /*french*/"la #boutique Goron# vend", /*spanish*/"el #dependiente Goron# vende"},
  });


    hintTable[HF_DEKU_SCRUB_GROTTO] = HintText::Exclude({
                       //obscure text
                       Text{"a lonely #scrub in a hole# sells", /*french*/"la #peste Mojo dans une grotte de la plaine# vend", /*spanish*/"un #singular deku bajo un hoyo# de la llanura vende"},
  });

    hintTable[LLR_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo � la ferme# vend", /*spanish*/"un #tr�o de dekus# de una granja venden"},
  });

    hintTable[LLR_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo � la ferme# vend", /*spanish*/"un #tr�o de dekus# de una granja venden"},
  });

    hintTable[LLR_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo � la ferme# vend", /*spanish*/"un #tr�o de dekus# de una granja venden"},
  });


    hintTable[LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in the woods# sells", /*french*/"le #duo de peste Mojo pr�s du th��tre# vend", /*spanish*/"un par de #dekus del bosque# venden"},
  });

    hintTable[LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in the woods# sells", /*french*/"le #duo de peste Mojo pr�s du th��tre# vend", /*spanish*/"un par de #dekus del bosque# venden"},
  });

    hintTable[LW_DEKU_SCRUB_NEAR_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub by a bridge# sells", /*french*/"la #peste Mojo pr�s du pont dans les bois# vend", /*spanish*/"un #deku bajo un puente# del bosque venden"},
  });

    hintTable[LW_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans les sous-bois# vend", /*spanish*/"un #par de dekus subterr�neos# del bosque venden"},
  });

    hintTable[LW_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans les sous-bois# vend", /*spanish*/"un #par de dekus subterr�neos# del bosque venden"},
  });


    hintTable[SFM_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au c�ur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterr�neos# de la pradera sagrada venden"},
  });

    hintTable[SFM_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au c�ur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterr�neos# de la pradera sagrada venden"},
  });


    hintTable[GC_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #tr�o de dekus# de la Ciudad Goron venden"},
  });

    hintTable[GC_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #tr�o de dekus# de la Ciudad Goron venden"},
  });

    hintTable[GC_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #tr�o de dekus# de la Ciudad Goron venden"},
  });


    hintTable[DMC_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #tr�o de dekus# del volc�n venden"},
  });

    hintTable[DMC_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #tr�o de dekus# del volc�n venden"},
  });

    hintTable[DMC_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #tr�o de dekus# del volc�n venden"},
  });


    hintTable[ZR_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo pr�s du fleuve# vend", /*spanish*/"un #par de dekus subterr�neos# del r�o venden"},
  });

    hintTable[ZR_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo pr�s du fleuve# vend", /*spanish*/"un #par de dekus subterr�neos# del r�o venden"},
  });


    hintTable[LH_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo pr�s du lac# vend", /*spanish*/"un #tr�o de dekus# del lago venden"},
  });

    hintTable[LH_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo pr�s du lac# vend", /*spanish*/"un #tr�o de dekus# del lago venden"},
  });

    hintTable[LH_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo pr�s du lac# vend", /*spanish*/"un #tr�o de dekus# del lago venden"},
  });


    hintTable[GV_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo pr�s de la vall�e# vend", /*spanish*/"un #par de dekus subterr�neos# del valle venden"},
  });

    hintTable[GV_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo pr�s de la vall�e# vend", /*spanish*/"un #par de dekus subterr�neos# del valle venden"},
  });


    hintTable[COLOSSUS_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans le d�sert# vend", /*spanish*/"un #par de dekus subterr�neos# del desierto venden"},
  });

    hintTable[COLOSSUS_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans le d�sert# vend", /*spanish*/"un #par de dekus subterr�neos# del desierto venden"},
  });


    hintTable[LLR_STABLES_LEFT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a stable# gifts", /*french*/"la #vache dans l'�table# donne", /*spanish*/"una #vaca del establo# brinda"},
  });

    hintTable[LLR_STABLES_RIGHT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a stable# gifts", /*french*/"la #vache dans l'�table# donne", /*spanish*/"una #vaca del establo# brinda"},
  });

    hintTable[LLR_TOWER_RIGHT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a ranch silo# gifts", /*french*/"la #vache dans le silo de la ferme# donne", /*spanish*/"una #vaca del granero# brinda"},
  });

    hintTable[LLR_TOWER_LEFT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a ranch silo# gifts", /*french*/"la #vache dans le silo de la ferme# donne", /*spanish*/"una #vaca del granero# brinda"},
  });

    hintTable[KAK_IMPAS_HOUSE_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow imprisoned in a house# protects", /*french*/"la #vache en cage# donne", /*spanish*/"una #vaca enjaulada de una casa# brinda"},
  });

    hintTable[DMT_COW_GROTTO_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a luxurious hole# offers", /*french*/"la #vache dans une grotte luxueuse# donne", /*spanish*/"una #vaca de un lujoso hoyo# brinda"},
  });
}
