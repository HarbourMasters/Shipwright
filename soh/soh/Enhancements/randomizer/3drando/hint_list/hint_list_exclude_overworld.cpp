#include "../hint_list.hpp"

void HintTable_Init_Exclude_Overworld() {
    hintTable[KF_KOKIRI_SWORD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #hidden treasure of the Kokiri# is", /*french*/"le #trésor des Kokiri# est", /*spanish*/"el #tesoro oculto de los Kokiri# esconde"},
  });

    hintTable[KF_MIDOS_TOP_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# possède", /*spanish*/"el #líder de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# gît", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_TOP_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# possède", /*spanish*/"el #líder de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# gît", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_BOTTOM_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# possède", /*spanish*/"el #líder de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# gît", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[KF_MIDOS_BOTTOM_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #leader of the Kokiri# hides", /*french*/"le #chef des Kokiri# possède", /*spanish*/"el #líder de los Kokiri# esconde"},
                     }, {},
                       //clear text
                       Text{"#inside Mido's house# is", /*french*/"#dans la maison de Mido# gît", /*spanish*/"en la #casa de Mido# yace"}
  );

    hintTable[GRAVEYARD_SHIELD_GRAVE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure of a fallen soldier# is", /*french*/"le #trésor du soldat mort# est", /*spanish*/"el #tesoro de un soldado caído# esconde"},
  });

    hintTable[DMT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"hidden behind a wall on a #mountain trail# is", /*french*/"derrière une façade du #chemin montagneux# est", /*spanish*/"tras una pared del #sendero de la montaña# yace"},
  });

    hintTable[GC_MAZE_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in #Goron City# explosives unlock", /*french*/"des explosions dans le #village Goron# révèlent", /*spanish*/"en la #Ciudad Goron# unos explosivos desbloquean"},
  });

    hintTable[GC_MAZE_CENTER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in #Goron City# explosives unlock", /*french*/"des explosions dans le #village Goron# révèlent", /*spanish*/"en la #Ciudad Goron# unos explosivos desbloquean"},
  });

    hintTable[ZD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"fire #beyond a waterfall# reveals", /*french*/"du feu #derrière la cascade# éclaire", /*spanish*/"las #llamas tras una una cascada# revelan"},
  });

    hintTable[GRAVEYARD_HOOKSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a chest hidden by a #speedy spectre# holds", /*french*/"le #coffre du rapide revenant# contient", /*spanish*/"un cofre custodiado por un #espectro veloz# contiene"},
                     }, {},
                       //clear text
                       Text{"#dead Dampé's first prize# is", /*french*/"la #première course d'Igor# donne", /*spanish*/"el primer premio de #la carrera de Dampé# se trata de"}
  );

    hintTable[GF_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"on a #rooftop in the desert# lies", /*french*/"sur un #toit du désert# gît", /*spanish*/"en una #azotea del desierto# yace"},
  });

    hintTable[KAK_REDEAD_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#zombies beneath the earth# guard", /*french*/"les #revenants sous terre# protègent", /*spanish*/"unos #zombis subterráneos# esconden"},
  });

    hintTable[SFM_WOLFOS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#wolves beneath the earth# guard", /*french*/"les #loups sous terre# protègent", /*spanish*/"unos #lobos subterráneos# esconden"},
  });

    hintTable[HF_NEAR_MARKET_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a field near a drawbridge# holds", /*french*/"la #grotte près d'un pont# contient", /*spanish*/"bajo el #hoyo de una llanura cercano a un puente# yace"},
  });

    hintTable[HF_SOUTHEAST_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole amongst trees in a field# holds", /*french*/"la #grotte près des arbres# contient", /*spanish*/"bajo el #hoyo de una llanura rodeado de árboles# yace"},
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
                       Text{"a #hole along a river# holds", /*french*/"la #grotte près du fleuve# contient", /*spanish*/"bajo un #hoyo junto a un río# yace"},
  });

    hintTable[KF_STORMS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a forest village# holds", /*french*/"la #grotte inondée de pluie dans le Village Kokiri# révèle", /*spanish*/"bajo el #hoyo de una tribu del bosque# yace"},
  });

    hintTable[LW_NEAR_SHORTCUTS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a wooded maze# holds", /*french*/"la #grotte dans le labyrinthe sylvestre# contient", /*spanish*/"bajo un #hoyo de un laberinto forestal# yace"},
  });

    hintTable[DMT_STORMS_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#hole flooded with rain on a mountain# holds", /*french*/"la #grotte inondée de pluie sur la montagne# contient", /*spanish*/"bajo un #hoyo de una montaña inundado de lluvia# yace"},
  });

    hintTable[DMC_UPPER_GROTTO_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hole in a volcano# holds", /*french*/"la #grotte dans le volcan# contient", /*spanish*/"bajo el #hoyo de un volcán# yace"},
  });


    hintTable[TOT_LIGHT_ARROWS_CUTSCENE] = HintText::Exclude({
                       //obscure text
                       Text{"the #final gift of a princess# is", /*french*/"le #cadeau d'adieu de la princesse# est", /*spanish*/"el #obsequio final de la princesa# se trata de"},
  });

    hintTable[LW_GIFT_FROM_SARIA] = HintText::Exclude({
                       //obscure text
                       Text{"a #potato hoarder# holds",           /*french*/"le #panini mélodieux# est en fait", /*spanish*/"cierta #jovencita verde# concede"},
                       Text{"a rooty tooty #flutey cutey# gifts", /*french*/"la #patate musicale# est en fait",  /*spanish*/"una #gran amiga# concede"},
                     }, {},
                       //clear text
                       Text{"#Saria's Gift# is", /*french*/"le #cadeau de Saria# est", /*spanish*/"el #regalo de Saria# se trata de"}
  );

    hintTable[ZF_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of winds# holds", /*french*/"la #fée du vent# a", /*spanish*/"el #hada del viento# brinda"},
  });

    hintTable[HC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of fire# holds", /*french*/"la #fée du feu# a", /*spanish*/"el #hada del fuego# brinda"},
  });

    hintTable[COLOSSUS_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of love# holds", /*french*/"la #fée de l'amour# a", /*spanish*/"el #hada del amor# brinda"},
  });

    hintTable[DMT_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #magical fairy# gifts", /*french*/"la #fée de la magie# a", /*spanish*/"un #hada mágica# brinda"},
  });

    hintTable[DMC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #magical fairy# gifts", /*french*/"la #fée de la magie# a", /*spanish*/"un #hada mágica# brinda"},
  });

    hintTable[OGC_GREAT_FAIRY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"the #fairy of strength# holds", /*french*/"la #fée de la force# a", /*spanish*/"el #hada de la fuerza# brinda"},
  });


    hintTable[SONG_FROM_IMPA] = HintText::Exclude({
                       //obscure text
                       Text{"#deep in a castle#, Impa teaches", /*french*/"#la gardienne de la princesse# donne", /*spanish*/"en el #jardín del castillo Impa enseña#"},
  });

    hintTable[SONG_FROM_MALON] = HintText::Exclude({
                       //obscure text
                       Text{"#a farm girl# sings", /*french*/"la #fillette de la ferme# donne", /*spanish*/"una #chica rupestre# canta"},
  });

    hintTable[SONG_FROM_SARIA] = HintText::Exclude({
                       //obscure text
                       Text{"#deep in the forest#, Saria teaches", /*french*/"la #fille de la forêt# donne", /*spanish*/"al #fondo del bosque# Saria enseña"},
  });

    hintTable[SONG_FROM_WINDMILL] = HintText::Exclude({
                       //obscure text
                       Text{"a man #in a windmill# is obsessed with", /*french*/"l'#homme du moulin# donne", /*spanish*/"el #hombre del molino# está obsesionado con"},
  });


    hintTable[HC_MALON_EGG] = HintText::Exclude({
                       //obscure text
                       Text{"a #girl looking for her father# gives", /*french*/"la #fillette qui cherche son père# donne", /*spanish*/"una #chica en busca de su padre# otorga"},
  });

    hintTable[HC_ZELDAS_LETTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #princess in a castle# gifts", /*french*/"la #princesse dans le château# donne", /*spanish*/"la #princesa de un castillo# otorga"},
  });

    hintTable[ZD_DIVING_MINIGAME] = HintText::Exclude({
                       //obscure text
                       Text{"an #unsustainable business model# gifts", /*french*/"le #mauvais modèle d'affaires# donne", /*spanish*/"un #mal modelo de negocio# premia con"},
                     }, {},
                       //clear text
                       Text{"those who #dive for Zora rupees# will find", /*french*/"ceux qui #plongent pour des rubis Zora# trouveront", /*spanish*/"aquellos que se #sumergan por las rupias zora# encontrarán"}
  );

    hintTable[LH_CHILD_FISHING] = HintText::Exclude({
                       //obscure text
                       Text{"#fishing in youth# bestows", /*french*/"#pêcher dans sa jeunesse# promet", /*spanish*/"#pescar en la juventud# conduce a"},
  });

    hintTable[LH_ADULT_FISHING] = HintText::Exclude({
                       //obscure text
                       Text{"#fishing in maturity# bestows", /*french*/"#pêcher dans sa maturité# promet", /*spanish*/"#pescar en la madurez# conduce a"},
  });

    hintTable[LH_LAB_DIVE] = HintText::Exclude({
                       //obscure text
                       Text{"a #diving experiment# is rewarded with", /*french*/"l'#expérience de plongée# donne", /*spanish*/"#bucear para un experimento# se premia con"},
  });

    hintTable[GC_ROLLING_GORON_AS_ADULT] = HintText::Exclude({
                       //obscure text
                       Text{"#comforting yourself# provides", /*french*/"se #réconforter soi-même# donne", /*spanish*/"#confrontarte a ti mismo# otorga"},
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
                       Text{"the #second explosive prize# is", /*french*/"le #deuxième prix explosif# est", /*spanish*/"el #segundo premio explosivo# se trata de"},
  });

    hintTable[MARKET_LOST_DOG] = HintText::Exclude({
                       //obscure text
                       Text{"#puppy lovers# will find", /*french*/"les #amoureux canins# trouveront", /*spanish*/"los #amantes caninos# encontrarán"},
                     }, {},
                       //clear text
                       Text{"#rescuing Richard the Dog# is rewarded with", /*french*/"#retrouver Kiki le chien# promet", /*spanish*/"#rescatar al perrito Ricardo# conduce a"}
  );

    hintTable[LW_OCARINA_MEMORY_GAME] = HintText::Exclude({
                       //obscure text
                       Text{"the prize for a #game of Simon Says# is", /*french*/"la #récompense de Jean Dit# est", /*spanish*/"#repetir ciertas melodías# otorga"},
                       Text{"a #child sing-a-long# holds",             /*french*/"les #jeunes flûtistes# donnent",  /*spanish*/"#tocar junto a otros# otorga"},
                     }, {},
                       //clear text
                       Text{"#playing an Ocarina in Lost Woods# is rewarded with", /*french*/"#jouer l'ocarina dans les Bois Perdus# donne", /*spanish*/"#tocar la ocarina en el Bosque Perdido# otorga"}
  );

    hintTable[KAK_10_GOLD_SKULLTULA_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#10 bug badges# rewards",           /*french*/"#10 écussons# donnent",             /*spanish*/"#10 medallas de insectos# otorgan"},
                       Text{"#10 spider souls# yields",          /*french*/"#10 âmes# donnent",                 /*spanish*/"#10 almas de araña# otorgan"},
                       Text{"#10 auriferous arachnids# lead to", /*french*/"#10 arachnides aurifères# donnent", /*spanish*/"#10 arácnidos auríferos# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #10 Gold Skulltulas# reveals", /*french*/"détruire #10 Skulltulas d'or# donne", /*spanish*/"#exterminar 10 skulltulas doradas# revela"}
  );

    hintTable[KAK_MAN_ON_ROOF] = HintText::Exclude({
                       //obscure text
                       Text{"a #rooftop wanderer# holds", /*french*/"une #rencontre sur un toit# donne", /*spanish*/"#alguien sobre un tejado# otorga"},
  });

    hintTable[ZR_MAGIC_BEAN_SALESMAN] = HintText::Exclude({
                       //obscure text
                       Text{"a seller of #colorful crops# has", /*french*/"le #marchand de légumes# vend", /*spanish*/"el vendedor de un #colorido cultivo# ofrece"},
                     }, {},
                       //clear text
                       Text{"a #bean seller# offers", /*french*/"le #marchand de haricots magiques# vend en fait", /*spanish*/"el #vendedor de judías# ofrece"}
  );

    hintTable[ZR_FROGS_IN_THE_RAIN] = HintText::Exclude({
                       //obscure text
                       Text{"#frogs in a storm# gift", /*french*/"#des grenouilles mouillées# donnent", /*spanish*/"las #ancas bajo la tormenta# otorgan"},
  });
    hintTable[ZR_FROGS_ZELDAS_LULLABY] = HintText::Exclude(
        {
            // obscure text
            Text{ "#sleepy frogs# gift", /*french*/ "#les grenouilles somnolentes# donnent",
                  /*spanish*/ "las #ranas somnolientas# regalan" },
            Text{ "#the Froggish Tenor in the back-left# gifts",
                  /*french*/ "#le ténor grenouillesque au fond à gauche# donne",
                  /*spanish*/ "el #Sapo Tenore al fondo, a la izquierda#, regala" },
        },
        {},
        // clear text
        Text{ "after hearing #Zelda's Lullaby, the frogs# gift",
              /*french*/ "à l'écoute de #la berceuse de Zelda, les grenouilles# donnent",
              /*spanish*/ "después de escuchar #la Nana de Zelda, las ranas# regalan" });

    hintTable[ZR_FROGS_EPONAS_SONG] = HintText::Exclude(
        {
            // obscure text
            Text{ "#equine frogs# gift", /*french*/ "#les grenouilles équestres# donnent",
                  /*spanish*/ "las #ranas equinas# regalan" },
            Text{ "#the Froggish Tenor in the back-right# gifts",
                  /*french*/ "#le ténor grenouillesque au fond à droite# donne",
                  /*spanish*/ "el #Sapo Tenore al fondo, a la derecha#, regala" },
        },
        {},
        // clear text
        Text{ "after hearing #Epona's Song, the frogs# gift",
              /*french*/ "à l'écoute du #chant d'Epona, les grenouilles# donnent",
              /*spanish*/ "después de escuchar #la Canción de Epona, las ranas# regalan" });

    hintTable[ZR_FROGS_SARIAS_SONG] = HintText::Exclude(
        {
            // obscure text
            Text{ "#sylvan frogs# gift", /*french*/ "#les grenouilles sylvestres# donnent",
                  /*spanish*/ "las #ranas silvestres# regalan" },
            Text{ "#the Froggish Tenor in the center# gifts",
                  /*french*/ "#le ténor grenouillesque dans le centre# donne",
                  /*spanish*/ "el #Sapo Tenore en el centro# regala" },
        },
        {},
        // clear text
        Text{ "after hearing #Saria's Song, the frogs# gift",
              /*french*/ "à l'écoute du #chant de Saria, les grenouilles# donnent",
              /*spanish*/ "después de escuchar #la Canción de Saria, las ranas# regalan" });

    hintTable[ZR_FROGS_SUNS_SONG] = HintText::Exclude(
        {
            // obscure text
            Text{ "#enlightened frogs# gift", /*french*/ "#les grenouilles éclairées# donnent",
                  /*spanish*/ "las #ranas alumbradas# regalan" },
            Text{ "#the Froggish Tenor in the front-left# gifts",
                  /*french*/ "#le ténor grenouillesque à l'avant gauche# donne",
                  /*spanish*/ "el #Sapo Tenore al frente, a la izquierda#, regala" },
        },
        {},
        // clear text
        Text{ "after hearing #the Sun's Song, the frogs# gift",
              /*french*/ "à l'écoute du #chant du soleil, les grenouilles# donnent",
              /*spanish*/ "después de escuchar #la Canción del Sol, las ranas# regalan" });

    hintTable[ZR_FROGS_SONG_OF_TIME] = HintText::Exclude(
        {
            // obscure text
            Text{ "#time-traveling frogs# gift", /*french*/ "#les grenouilles voyageuses dans le temps# donnent",
                  /*spanish*/ "las #ranas viajeras del tiempo# regalan" },
            Text{ "#the Froggish Tenor in the front-right# gifts",
                  /*french*/ "#le ténor grenouillesque à l'avant droite# donne",
                  /*spanish*/ "el #Sapo Tenore al frente, a la derecha#, regala" },
        },
        {},
        // clear text
        Text{ "after hearing #the Song of Time, the frogs# gift",
              /*french*/ "à l'écoute du #chant du temps, les grenouilles# donnent",
              /*spanish*/ "después de escuchar #la Canción del tiempo, las ranas# regalan" });

    hintTable[GF_HBA_1000_POINTS] = HintText::Exclude({
                       //obscure text
                       Text{"scoring 1000 in #horseback archery# grants", /*french*/"obtenir 1000 points dans l'#archerie équestre# donne", /*spanish*/"conseguir 1000 puntos en el #tiro con arco a caballo# premia"},
  });

    hintTable[MARKET_SHOOTING_GALLERY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#shooting in youth# grants", /*french*/"#faire du tir dans sa jeunesse# donne", /*spanish*/"#disparar en la juventud# otorga"},
  });

    hintTable[KAK_SHOOTING_GALLERY_REWARD] = HintText::Exclude({
                       //obscure text
                       Text{"#shooting in maturity# grants", /*french*/"#faire du tir dans sa maturité# donne", /*spanish*/"#disparar en la madurez# otorga"},
  });

    hintTable[LW_TARGET_IN_WOODS] = HintText::Exclude({
                       //obscure text
                       Text{"shooting a #target in the woods# grants", /*french*/"#tirer une cible dans les bois# donne", /*spanish*/"disparar a un #blanco forestal# brinda"},
  });

    hintTable[KAK_ANJU_AS_ADULT] = HintText::Exclude({
                       //obscure text
                       Text{"a #chicken caretaker# offers adults", /*french*/"devenir un #éleveur de Cocottes# donne", /*spanish*/"una #cuidadora de emplumados# le ofrece a los mayores"},
  });

    hintTable[LLR_TALONS_CHICKENS] = HintText::Exclude({
                       //obscure text
                       Text{"#finding Super Cuccos# is rewarded with", /*french*/"#trouver des Super Cocottes# donne", /*spanish*/"#hallar los supercucos# conduce a"},
  });

    hintTable[GC_ROLLING_GORON_AS_CHILD] = HintText::Exclude({
                       //obscure text
                       Text{"the prize offered by a #large rolling Goron# is", /*french*/"la récompense d'un #gros Goron roulant# est", /*spanish*/"un #gran Goron rodante# otorga"},
  });

    hintTable[LH_UNDERWATER_ITEM] = HintText::Exclude({
                       //obscure text
                       Text{"the #sunken treasure in a lake# is", /*french*/"le #trésor au fond du lac# est", /*spanish*/"el #tesoro hundido del lago# se trata de"},
  });

    hintTable[GF_GERUDO_MEMBERSHIP_CARD] = HintText::Exclude({
                       //obscure text
                       Text{"#rescuing captured carpenters# is rewarded with", /*french*/"#secourir les charpentiers capturés# assure", /*spanish*/"#rescatar los apresados carpinteros# se premia con"},
  });

    hintTable[WASTELAND_BOMBCHU_SALESMAN] = HintText::Exclude({
                       //obscure text
                       Text{"a #carpet guru# sells", /*french*/"#un marchand du désert# vend", /*spanish*/"el #genio de una alfombra# vende"},
  });

    hintTable[GC_MEDIGORON] = HintText::Exclude({
                       //obscure text
                       Text{"#Medigoron# sells", /*french*/"#Medigoron# vend", /*spanish*/"#Medigoron# vende"},
  });

  hintTable[KAK_GRANNYS_SHOP] = HintText::Exclude({
                       // obscure text
                       Text{"the #potion shop lady# sells", /*french*/"la #dame du magasin de potion# vend", /*spanish*/"la #señora de la tienda de pociones# vende" },
  });

    hintTable[KAK_IMPAS_HOUSE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#imprisoned in a house# lies", /*french*/"#encagé dans une maison# gît", /*spanish*/"#en una casa entre rejas# yace"},
  });

    hintTable[HF_TEKTITE_GROTTO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#deep underwater in a hole# is", /*french*/"#dans les profondeurs d'une grotte# gît", /*spanish*/"#en lo hondo bajo un hoyo# yace"},
  });

    hintTable[KAK_WINDMILL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on a #windmill ledge# lies", /*french*/"#haut perché dans le moulin# gît", /*spanish*/"al #borde de un molino# yace"},
  });

    hintTable[GRAVEYARD_DAMPE_RACE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#racing a ghost# leads to", /*french*/"le défi du #revenant rapide# donne", /*spanish*/"#perseguir a un fantasma# conduce a"},
                     }, {},
                       //clear text
                       Text{"#dead Dampe's second# prize is", /*french*/"la #deuxième course d'Igor# donne", /*spanish*/"el segundo premio de #la carrera de Dampé# se trata de"}
  );

    hintTable[LLR_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"in a #ranch silo# lies", /*french*/"#dans l'entrepôt de la ferme# gît", /*spanish*/"en un #granero rupestre# yace"},
  });

    hintTable[GRAVEYARD_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"a #crate in a graveyard# hides", /*french*/"#la boîte dans le Cimetière# contient", /*spanish*/"bajo la #caja de un cementerio# yace"},
  });

    hintTable[GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR] = HintText::Exclude({
                       //obscure text
                       Text{"a #gravekeeper digs up#", /*french*/"#le jeu du fossoyeur# cache", /*spanish*/"cierto #sepultero desentierra#"},
  });

    hintTable[ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on top of a #pillar in a river# lies", /*french*/"#sur un pilier au dessus du fleuve# gît", /*spanish*/"en lo alto del #pilar de un río# yace"},
  });

    hintTable[ZR_NEAR_DOMAIN_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on a #river ledge by a waterfall# lies", /*french*/"#sur la falaise au dessus du fleuve# gît", /*spanish*/"al borde de #la entrada a una cascada# yace"},
  });

    hintTable[LH_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"high on a #lab rooftop# one can find", /*french*/"#la tour d'observation du lac# cache", /*spanish*/"en lo #alto de un laboratorio# yace"},
  });

    hintTable[ZF_ICEBERG_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#floating on ice# is", /*french*/"#gisant sur la glace# gît", /*spanish*/"#flotando sobre hielo# yace"},
  });

    hintTable[GV_WATERFALL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"behind a #valley waterfall# is", /*french*/"#derrière la cascade du désert# se cache", /*spanish*/"tras una #desierta cascada# yace"},
  });

    hintTable[GV_CRATE_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"a #crate in a valley# hides", /*french*/"la #boîte dans la vallée# contient", /*spanish*/"bajo la #caja de un valle# yace"},
  });

    hintTable[COLOSSUS_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"on top of an #arch of stone# lies", /*french*/"#gisant sur une arche de pierre# gît", /*spanish*/"en lo alto de un #arco de piedra# yace"},
  });

    hintTable[DMT_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"above a #mountain cavern entrance# is", /*french*/"gisant #au dessus de la caverne montagneuse# gît", /*spanish*/"en lo alto de la #entrada de una cueva en la montaña# yace"},
  });

    hintTable[DMC_WALL_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"nestled in a #volcanic wall# is", /*french*/"dans une #alcove volcanique# gît", /*spanish*/"entre unas #murallas volcánicas# yace"},
  });

    hintTable[DMC_VOLCANO_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"obscured by #volcanic ash# is", /*french*/"#recouvert de cendres volcaniques# gît", /*spanish*/"bajo la #ceniza volcánica# yace"},
  });

    hintTable[GF_NORTH_F1_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #geôliers Gerudo# détiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_NORTH_F2_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #geôliers Gerudo# détiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_SOUTH_F1_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #geôliers Gerudo# détiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });

    hintTable[GF_SOUTH_F2_CARPENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating Gerudo guards# reveals", /*french*/"les #geôliers Gerudo# détiennent", /*spanish*/"#derrotar a las guardas Gerudo# revela"},
  });


    hintTable[HF_GS_NEAR_KAK_GROTTO] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-guarded spider in a hole# hoards", /*french*/"une #Skulltula dans un trou d'arachnides# a", /*spanish*/"una #Skulltula custodiada por otra# de un hoyo otorga"},
  });


    hintTable[LLR_GS_BACK_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a ranch# holding", /*french*/"une #Skulltula sur la façade de la ferme# a", /*spanish*/"la noche revela una #Skulltula del rancho# que otorga"},
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
                       Text{"a spider hiding in a #ranch tree# holds", /*french*/"une #Skulltula dans l'arbre de la ferme# a", /*spanish*/"una Skulltula escondida en el #árbol de un rancho# otorga"},
  });


    hintTable[KF_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a forest# holds", /*french*/"une #Skulltula enterrée dans la forêt# a", /*spanish*/"una #Skulltula enterrada en un bosque# otorga"},
  });

    hintTable[KF_GS_KNOW_IT_ALL_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a forest# holding", /*french*/"une #Skulltula derrière une cabane de la forêt# a", /*spanish*/"la noche revela en el pasado una #Skulltula del bosque# que otorga"},
  });

    hintTable[KF_GS_HOUSE_OF_TWINS] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a forest# holding", /*french*/"une #Skulltula sur une cabane de la forêt# a", /*spanish*/"la noche revela en el futuro una #Skulltula del rancho# que otorga"},
  });


    hintTable[LW_GS_BEAN_PATCH_NEAR_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried deep in a forest maze# holds", /*french*/"une #Skulltula enterrée dans les bois# a", /*spanish*/"una #Skulltula enterrada en un laberinto forestal# otorga"},
  });

    hintTable[LW_GS_BEAN_PATCH_NEAR_THEATER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried deep in a forest maze# holds", /*french*/"une #Skulltula enterrée dans les bois# a", /*spanish*/"una #Skulltula enterrada en un laberinto forestal# otorga"},
  });

    hintTable[LW_GS_ABOVE_THEATER] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in a forest maze# holding", /*french*/"une #Skulltula haut perchée dans les bois# a", /*spanish*/"la noche revela una #Skulltula del laberinto forestal# que otorga"},
  });

    hintTable[SFM_GS] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a forest meadow# holding", /*french*/"une #Skulltula dans le sanctuaire des bois# a", /*spanish*/"la noche revela una #Skulltula de la pradera del bosque# que otorga"},
  });


    hintTable[OGC_GS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider outside a tyrant's tower# holds", /*french*/"une #Skulltula parmi les ruines du château# a", /*spanish*/"una #Skulltula a las afueras de la torre de un tirano# otorga"},
  });

    hintTable[HC_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree outside of a castle# holds", /*french*/"une #Skulltula dans l'arbre près du château# a", /*spanish*/"una Skulltula escondida en el #árbol de las afueras de un castillo# otorga"},
  });

    hintTable[MARKET_GS_GUARD_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a guarded crate# holds", /*french*/"une #Skulltula dans une boîte en ville# a", /*spanish*/"una #Skulltula bajo una custodiada caja# otorga"},
  });


    hintTable[DMC_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a volcano# holds", /*french*/"une #Skulltula enterrée dans un volcan# a", /*spanish*/"una #Skulltula enterrada en un volcán# otorga"},
  });


    hintTable[DMT_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried outside a cavern# holds", /*french*/"une #Skulltula enterrée près d'une caverne# a", /*spanish*/"una #Skulltula enterrada a la entrada de una cueva# otorga"},
  });

    hintTable[DMT_GS_NEAR_KAK] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider hidden in a mountain nook# holds", /*french*/"une #Skulltula cachée dans le flanc d'une montagne# a", /*spanish*/"una #Skulltula oculta en el rincón de la montaña# otorga"},
  });

    hintTable[DMT_GS_ABOVE_DODONGOS_CAVERN] = HintText::Exclude({
                       //obscure text
                       Text{"the hammer reveals a #spider on a mountain# holding", /*french*/"une #Skulltula derrière un rocher massif près d'une caverne# a", /*spanish*/"el martillo revela #una Skulltula de la montaña# que otorga"},
  });

    hintTable[DMT_GS_FALLING_ROCKS_PATH] = HintText::Exclude({
                       //obscure text
                       Text{"the hammer reveals a #spider on a mountain# holding", /*french*/"une #Skulltula derrière un rocher massif près du sommet d'un volcan# a", /*spanish*/"el martillo revela #una Skulltula de la montaña# que otorga"},
  });


    hintTable[GC_GS_CENTER_PLATFORM] = HintText::Exclude({
                       //obscure text
                       Text{"a #suspended spider# in Goron City holds", /*french*/"une #Skulltula perchée dans le village Goron# a", /*spanish*/"una #Skulltula suspendida# en la Ciudad Goron otorga"},
  });

    hintTable[GC_GS_BOULDER_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in a #Goron City crate# holds", /*french*/"une #Skulltula dans une boîte du village Goron# a", /*spanish*/"una #Skulltula bajo una caja# de la Ciudad Goron otorga"},
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
                       Text{"night in the past reveals a #spider in a town# holding", /*french*/"une #Skulltula sur une échelle dans un village# a", /*spanish*/"la noche del pasado revela una #Skulltula del pueblo# que otorga"},
  });

    hintTable[KAK_GS_ABOVE_IMPAS_HOUSE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a town# holding", /*french*/"une #Skulltula au dessus d'une grande maison# a", /*spanish*/"la noche del futuro revela una #Skulltula del pueblo# que otorga"},
  });


    hintTable[GRAVEYARD_GS_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider in a graveyard# holding", /*french*/"une #Skulltula sur une façade du Cimetière# a", /*spanish*/"la noche revela una #Skulltula del cementerio# que otorga"},
  });

    hintTable[GRAVEYARD_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a graveyard# holds", /*french*/"une #Skulltula enterrée dans le Cimetière# a", /*spanish*/"una #Skulltula enterrada en el cementerio# otorga"},
  });


    hintTable[ZR_GS_LADDER] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une échelle près d'une cascade# a", /*spanish*/"la noche del pasado revela una #Skulltula del río# que otorga"},
  });

    hintTable[ZR_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree by a river# holds", /*french*/"une #Skulltula dans un arbre près du fleuve# a", /*spanish*/"una Skulltula escondida en el #árbol de un río# otorga"},
  });

    hintTable[ZR_GS_ABOVE_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une façade près d'une cascade# a", /*spanish*/"la noche del futuro revela una #Skulltula del río# que otorga"},
  });

    hintTable[ZR_GS_NEAR_RAISED_GROTTOS] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a river# holding", /*french*/"une #Skulltula sur une façade près d'une grotte du fleuve# a", /*spanish*/"la noche del futuro revela una #Skulltula del río# que otorga"},
  });


    hintTable[ZD_GS_FROZEN_WATERFALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a frozen waterfall# holding", /*french*/"une #Skulltula près d'une cascade gelée# a", /*spanish*/"la noche revela una #Skulltula junto a una congelada cascada# que otorga"},
  });

    hintTable[ZF_GS_ABOVE_THE_LOG] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider near a deity# holding", /*french*/"une #Skulltula près du gardien aquatique# a", /*spanish*/"la noche revela una #Skulltula junto a cierta deidad# que otorga"},
  });

    hintTable[ZF_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider hiding in a #tree near a deity# holds", /*french*/"une #Skulltula dans un arbre dans un réservoir# a", /*spanish*/"una Skulltula escondida en el #árbol junto a cierta deidad# otorga"},
  });


    hintTable[LH_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried by a lake# holds", /*french*/"une #Skulltula enterrée près d'un lac# a", /*spanish*/"una #Skulltula enterrada junto a un lago# otorga"},
  });

    hintTable[LH_GS_SMALL_ISLAND] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake# holding", /*french*/"une #Skulltula sur un îlot du lac# a", /*spanish*/"la noche revela una #Skulltula junto a un lago# que otorga"},
  });

    hintTable[LH_GS_LAB_WALL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake# holding", /*french*/"une #Skulltula sur le mur d'un centre de recherche# a", /*spanish*/"la noche revela una #Skulltula junto a un lago# que otorga"},
  });

    hintTable[LH_GS_LAB_CRATE] = HintText::Exclude({
                       //obscure text
                       Text{"a spider deed underwater in a #lab crate# holds", /*french*/"une #Skulltula dans une boîte au fond d'une cuve d'eau# a", /*spanish*/"una #Skulltula bajo la sumergida caja de un laboratorio# otorga"},
  });

    hintTable[LH_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider by a lake high in a tree# holding", /*french*/"une #Skulltula dans un grand arbre du lac# a", /*spanish*/"la noche revela #una Skulltula del lago sobre un árbol# que otorga"},
  });


    hintTable[GV_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a valley# holds", /*french*/"une #Skulltula enterré dans une vallée# a", /*spanish*/"una #Skulltula enterrada en un valle# otorga"},
  });

    hintTable[GV_GS_SMALL_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"night in the past reveals a #spider in a valley# holding", /*french*/"une #Skulltula au dessus d'une petite cascade# a", /*spanish*/"la noche del pasado revela una #Skulltula del valle# que otorga"},
  });

    hintTable[GV_GS_PILLAR] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a valley# holding", /*french*/"une #Skulltula sur une arche de pierre dans une vallée# a", /*spanish*/"la noche del futuro revela una #Skulltula del valle# que otorga"},
  });

    hintTable[GV_GS_BEHIND_TENT] = HintText::Exclude({
                       //obscure text
                       Text{"night in the future reveals a #spider in a valley# holding", /*french*/"une #Skulltula derrière une tente# a", /*spanish*/"la noche del futuro revela una #Skulltula del valle# que otorga"},
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
                       Text{"a #spider buried in the desert# holds", /*french*/"une #Skulltula enterrée au pied du colosse# a", /*spanish*/"una #Skulltula enterrada en el desierto# otorga"},
  });

    hintTable[COLOSSUS_GS_HILL] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in the desert# holding", /*french*/"une #Skulltula sur une colline dans le désert# a", /*spanish*/"la noche revela una #Skulltula en las profundidades del desierto# que otorga"},
  });

    hintTable[COLOSSUS_GS_TREE] = HintText::Exclude({
                       //obscure text
                       Text{"night reveals a #spider deep in the desert# holding", /*french*/"une #Skulltula dans un arbre du désert# a", /*spanish*/"la noche revela una #Skulltula en las profundidades del desierto# que otorga"},
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
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });

    hintTable[MARKET_BOMBCHU_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #Bombchu merchant# sells", /*french*/"le #marchand de Missiles# vend", /*spanish*/"un #mercader de bombchus# vende"},
  });


    hintTable[MARKET_POTION_SHOP_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la Place du Marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );


    hintTable[MARKET_BAZAAR_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la Place du Marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
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
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo à la ferme# vend", /*spanish*/"un #trío de dekus# de una granja venden"},
  });

    hintTable[LLR_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo à la ferme# vend", /*spanish*/"un #trío de dekus# de una granja venden"},
  });

    hintTable[LLR_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo à la ferme# vend", /*spanish*/"un #trío de dekus# de una granja venden"},
  });


    hintTable[LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in the woods# sells", /*french*/"le #duo de peste Mojo près du théâtre# vend", /*spanish*/"un par de #dekus del bosque# venden"},
  });

    hintTable[LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in the woods# sells", /*french*/"le #duo de peste Mojo près du théâtre# vend", /*spanish*/"un par de #dekus del bosque# venden"},
  });

    hintTable[LW_DEKU_SCRUB_NEAR_BRIDGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub by a bridge# sells", /*french*/"la #peste Mojo près du pont dans les bois# vend", /*spanish*/"un #deku bajo un puente# del bosque venden"},
  });

    hintTable[LW_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans les sous-bois# vend", /*spanish*/"un #par de dekus subterráneos# del bosque venden"},
  });

    hintTable[LW_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans les sous-bois# vend", /*spanish*/"un #par de dekus subterráneos# del bosque venden"},
  });


    hintTable[SFM_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au cur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterráneos# de la pradera sagrada venden"},
  });

    hintTable[SFM_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au cur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterráneos# de la pradera sagrada venden"},
  });


    hintTable[GC_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #trío de dekus# de la Ciudad Goron venden"},
  });

    hintTable[GC_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #trío de dekus# de la Ciudad Goron venden"},
  });

    hintTable[GC_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le village Goron# vend", /*spanish*/"un #trío de dekus# de la Ciudad Goron venden"},
  });


    hintTable[DMC_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #trío de dekus# del volcán venden"},
  });

    hintTable[DMC_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #trío de dekus# del volcán venden"},
  });

    hintTable[DMC_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo dans le volcan# vend", /*spanish*/"un #trío de dekus# del volcán venden"},
  });


    hintTable[ZR_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo près du fleuve# vend", /*spanish*/"un #par de dekus subterráneos# del río venden"},
  });

    hintTable[ZR_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo près du fleuve# vend", /*spanish*/"un #par de dekus subterráneos# del río venden"},
  });


    hintTable[LH_DEKU_SCRUB_GROTTO_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo près du lac# vend", /*spanish*/"un #trío de dekus# del lago venden"},
  });

    hintTable[LH_DEKU_SCRUB_GROTTO_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo près du lac# vend", /*spanish*/"un #trío de dekus# del lago venden"},
  });

    hintTable[LH_DEKU_SCRUB_GROTTO_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #trio of scrubs# sells", /*french*/"le #trio de peste Mojo près du lac# vend", /*spanish*/"un #trío de dekus# del lago venden"},
  });


    hintTable[GV_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo près de la vallée# vend", /*spanish*/"un #par de dekus subterráneos# del valle venden"},
  });

    hintTable[GV_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo près de la vallée# vend", /*spanish*/"un #par de dekus subterráneos# del valle venden"},
  });


    hintTable[COLOSSUS_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans le désert# vend", /*spanish*/"un #par de dekus subterráneos# del desierto venden"},
  });

    hintTable[COLOSSUS_DEKU_SCRUB_GROTTO_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo dans le désert# vend", /*spanish*/"un #par de dekus subterráneos# del desierto venden"},
  });


    hintTable[LLR_STABLES_LEFT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a stable# gifts", /*french*/"la #vache dans l'étable# donne", /*spanish*/"una #vaca del establo# brinda"},
  });

    hintTable[LLR_STABLES_RIGHT_COW] = HintText::Exclude({
                       //obscure text
                       Text{"a #cow in a stable# gifts", /*french*/"la #vache dans l'étable# donne", /*spanish*/"una #vaca del establo# brinda"},
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
