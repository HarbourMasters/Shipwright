#include "../hint_list.hpp"

void HintTable_Init_Exclude_Dungeon() {
  /*--------------------------
  |        DEKU TREE         |
  ---------------------------*/
    hintTable[DEKU_TREE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #center of the Deku Tree# lies", /*french*/"#le coeur de l'Arbre Mojo# recèle", /*spanish*/"al #centro del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_SLINGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure guarded by a scrub# in the Deku Tree is", /*french*/"le #trésor protégé par une peste# dans l'Arbre Mojo est", /*spanish*/"un #deku del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure guarded by a scrub# in the Deku Tree is", /*french*/"le #trésor protégé par une peste# dans l'Arbre Mojo est", /*spanish*/"un #deku del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'Arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_COMPASS_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'Arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#webs in the Deku Tree# hide", /*french*/"les #toiles dans l'Arbre Mojo# cachent", /*spanish*/"entre #telarañas del Gran Árbol Deku# yace"},
  });


    hintTable[DEKU_TREE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #center of the Deku Tree# lies", /*french*/"#le coeur de l'Arbre Mojo# recèle", /*spanish*/"al #centro del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #treasure guarded by a large spider# in the Deku Tree is", /*french*/"le #trésor protégé par une grosse araignée# dans l'Arbre Mojo est", /*spanish*/"una #gran araña del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_MQ_SLINGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'Arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'Arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_MQ_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#webs in the Deku Tree# hide", /*french*/"les #toiles dans l'Arbre Mojo# cachent", /*spanish*/"entre #telarañas del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#magical fire in the Deku Tree# leads to", /*french*/"le #feu magique dans l'Arbre Mojo# éclaire", /*spanish*/"el #fuego mágico en el Gran Árbol Deku# conduce a"},
  });

    hintTable[DEKU_TREE_QUEEN_GOHMA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Parasitic Armored Arachnid# holds", /*french*/"le #monstre insectoïde géant# possède", /*spanish*/"el #arácnido parasitario acorazado# porta"},
                     }, {},
                       //clear text
                       Text{"#Queen Gohma# holds", /*french*/"la #Reine Gohma# possède", /*spanish*/"la #Reina Goma# porta"}
  );

    hintTable[DEKU_TREE_GS_BASEMENT_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider deep within the Deku Tree# hides", /*french*/"une #Skulltula au coeur de l'Arbre Mojo# a", /*spanish*/"una #Skulltula en las profundidades del Árbol Deku# otorga"},
  });

    hintTable[DEKU_TREE_GS_BASEMENT_GATE] = HintText::Exclude({
                       //obscure text
                       Text{"a #web protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière une toile dans l'Arbre Mojo# a", /*spanish*/"una #Skulltula protegida por su tela# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_GS_BASEMENT_VINES] = HintText::Exclude({
                       //obscure text
                       Text{"a #web protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière une toile dans l'Arbre Mojo# a", /*spanish*/"una #Skulltula protegida por su tela# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_GS_COMPASS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider atop the Deku Tree# holds", /*french*/"une #Skulltula au sommet de l'Arbre Mojo# a", /*spanish*/"una #Skulltula en lo alto del Árbol Deku# otorga"},
  });


    hintTable[DEKU_TREE_MQ_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a crate# within the Deku Tree hides", /*french*/"une #Skulltula dans une boîte dans l'Arbre Mojo# a", /*spanish*/"una #Skulltula bajo una caja# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_MQ_GS_COMPASS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #wall of rock protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière des rochers dans l'Arbre Mojo# a", /*spanish*/"una #Skulltula protegida por una pared rocosa# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider deep within the Deku Tree# hides", /*french*/"une #Skulltula au coeur de l'Arbre Mojo# a", /*spanish*/"una #Skulltula en las profundidades del Árbol Deku# otorga"},
  });

    hintTable[DEKU_TREE_MQ_DEKU_SCRUB] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in the Deku Tree# sells", /*french*/"la #peste Mojo dans l'Arbre Mojo# vend", /*spanish*/"un #deku del Gran Árbol Deku# vende"},
  });

  /*--------------------------
  |     DODONGOS CAVERN      |
  ---------------------------*/
    hintTable[DODONGOS_CAVERN_BOSS_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#above King Dodongo# lies", /*french*/"#par dessus le Roi Dodongo# gît", /*spanish*/"#sobre el Rey Dodongo# yace"},
  });

    hintTable[DODONGOS_CAVERN_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #muddy wall in Dodongo's Cavern# hides", /*french*/"le #mur fragile dans la Caverne Dodongo# recèle", /*spanish*/"tras una #agrietada pared en la Cueva de los Dodongos# yace"},
  });

    hintTable[DODONGOS_CAVERN_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #statue in Dodongo's Cavern# guards", /*french*/"la #statue dans la Caverne Dodongo# protège", /*spanish*/"una #estatua de la Cueva de los Dodongos# esconde"},
  });

    hintTable[DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"above a #maze of stone# in Dodongo's Cavern lies", /*french*/"sur #un labyrinthe de pierre# dans la Caverne Dodongo gît", /*spanish*/"entre un #laberinto de piedra# en la Cueva de los Dodongos yace"},
  });

    hintTable[DODONGOS_CAVERN_BOMB_BAG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #second lizard cavern battle# yields", /*french*/"le #deuxième duel de lézards de caverne# octroie", /*spanish*/"#luchar dos veces contra reptiles en una cueva# conduce a"},
  });

    hintTable[DODONGOS_CAVERN_END_OF_BRIDGE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest at the end of a bridge# yields", /*french*/"le #trésor à l'extrémité d'un pont# contient", /*spanish*/"un #cofre al final de un quebrado puente# contiene"},
  });


    hintTable[DODONGOS_CAVERN_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #muddy wall in Dodongo's Cavern# hides", /*french*/"le #mur fragile dans la Caverne Dodongo# recèle", /*spanish*/"una #agrietada pared en la Cueva de los Dodongos# esconde"},
  });

    hintTable[DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #elevated alcove# in Dodongo's Cavern holds", /*french*/"l'#alcove haut perchée# dans la Caverne Dodongo cache", /*spanish*/"una #elevada alcoba# en la Cueva de los Dodongos brinda"},
  });

    hintTable[DODONGOS_CAVERN_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire-breathing lizards# in Dodongo's Cavern guard", /*french*/"les #lézards cracheurs de feu# dans la Caverne Dodongo protègent", /*spanish*/"unos #flamígeros reptiles# en la Cueva de los Dodongos esconden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#baby spiders# in Dodongo's Cavern guard", /*french*/"les #petites araignées dans la Caverne Dodongo# protègent", /*spanish*/"unas #pequeñas larvas# en la Cueva de los Dodongos esconden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"above a #maze of stone# in Dodongo's Cavern lies", /*french*/"sur #un labyrinthe de pierre# dans la Caverne Dodongo gît", /*spanish*/"sobre un #laberinto de piedra# en la Cueva de los Dodongos yace"},
  });

    hintTable[DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#beneath a headstone# in Dodongo's Cavern lies", /*french*/"#sous une pierre tombale# dans la Caverne Dodongo gît", /*spanish*/"#bajo una lápida# en la Cueva de los Dodongos yace"},
  });

    hintTable[DODONGOS_CAVERN_KING_DODONGO_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Infernal Dinosaur# holds", /*french*/"le #dinosaure infernal# possède", /*spanish*/"el #dinosaurio infernal# porta"},
                     }, {},
                       //clear text
                       Text{"#King Dodongo# holds", /*french*/"le #Roi Dodongo# possède", /*spanish*/"el #Rey Dodongo# porta"}
  );

    hintTable[DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider entangled in vines# in Dodongo's Cavern guards", /*french*/"une #Skulltula sur les vignes dans la Caverne Dodongo# a", /*spanish*/"una #Skulltula sobre unas cepas# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_SCARECROW] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among explosive slugs# hides", /*french*/"une #Skulltula dans l'alcove du couloir dans la Caverne Dodongo# a", /*spanish*/"una #Skulltula rodeada de explosivos gusanos# otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider just out of reach# in Dodongo's Cavern holds", /*french*/"une #Skulltula au haut des escaliers de la Caverne Dodongo# a", /*spanish*/"una #Skulltula fuera del alcance# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider behind a statue# in Dodongo's Cavern holds", /*french*/"une #Skulltula au coeur de la Caverne Dodongo# a", /*spanish*/"una #Skulltula tras una estatua# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among bats# in Dodongo's Cavern holds", /*french*/"une #Skulltula entourée de Saigneurs dans la Caverne Dodongo# a", /*spanish*/"una #Skulltula rodeada de murciélagos# de la Cueva de los Dodongos otorga"},
  });


    hintTable[DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider high on a wall# in Dodongo's Cavern holds", /*french*/"une #Skulltula haut perchée dans la Caverne Dodongo# a", /*spanish*/"una #Skulltula en lo alto de una pared# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on top of a pillar of rock# in Dodongo's Cavern holds", /*french*/"une #Skulltula sur l'énorme pilier de roc de la Caverne Dodongo# a", /*spanish*/"una #Skulltula en lo alto de un pilar# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a crate# in Dodongo's Cavern holds", /*french*/"une #Skulltula dans une boîte de la Caverne Dodongo# a", /*spanish*/"una #Skulltula bajo una caja# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_BACK_AREA] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among graves# in Dodongo's Cavern holds", /*french*/"une #Skulltula parmi les tombes de la Caverne Dodongo# a", /*spanish*/"una #Skulltula entre lápidas# en la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans la Caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub guarded by Lizalfos# sells", /*french*/"la #peste Mojo au coeur de la Caverne Dodongo# vend", /*spanish*/"un #deku custodiado por Lizalfos# vende"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans la Caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in Dodongo's Cavern# sells", /*french*/"la #peste Mojo dans l'entrée de la Caverne Dodongo# vend", /*spanish*/"un #deku en la Cueva de los Dodongos# vende"},
  });


    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans l'entrée de la Caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans l'entrée de la Caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in Dodongo's Cavern# sells", /*french*/"la #peste Mojo au sommet des escaliers dans la Caverne Dodongo# vend", /*spanish*/"un #deku en la Cueva de los Dodongos# vende"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub guarded by Lizalfos# sells", /*french*/"la #peste Mojo au coeur de la Caverne Dodongo# vend", /*spanish*/"un #deku custodiado por Lizalfos# vende"},
  });

  /*--------------------------
  |     JABU JABUS BELLY     |
  ---------------------------*/
    hintTable[JABU_JABUS_BELLY_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#tentacle trouble# in a deity's belly guards", /*french*/"la #membrane# dans le ventre du gardien protège", /*spanish*/"un #problema tentaculoso# en la tripa de cierta deidad esconde"},
                     }, {},
                       //clear text
                       Text{"a #slimy thing# guards", /*french*/"la #chose gluante# gardien protège", /*spanish*/"un #tentáculo parasitario# protege"}
  );

    hintTable[JABU_JABUS_BELLY_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#bubble trouble# in a deity's belly guards", /*french*/"un #horde de bulles# dans le ventre du gardien protègent", /*spanish*/"un #problema burbujesco# en la tripa de cierta deidad esconde"},
                     }, {},
                       //clear text
                       Text{"#bubbles# guard", /*french*/"des #bulles# entourent", /*spanish*/"unas #burbujas# protegen"}
  );


    hintTable[JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"shooting a #mouth cow# reveals", /*french*/"tirer sur une #vache# révèle", /*spanish*/"#dispararle a una vaca# revela"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pop rocks# hide",            /*french*/"des #pierres aux reins# cachent",     /*spanish*/"#cepillarse los dientes con explosivos# revela"},
                       Text{"an #explosive palate# holds", /*french*/"des #gargouillis explosifs# cachent", /*spanish*/"un #paladar explosivo# esconde"},
                     }, {},
                       //clear text
                       Text{"a #boulder before cows# hides", /*french*/"des #rochers près des vaches# cachent", /*spanish*/"cierta #roca rodeada de vacas# esconde"}
  );

    hintTable[JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"near a #spiked elevator# lies", /*french*/"près d'un #ascenseur visqueux# gît", /*spanish*/"cerca de un #ascensor puntiagudo# yace"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #drowning cow# unveils", /*french*/"une #vache à l'eau# a", /*spanish*/"una #vaca sumergida# revela"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#moving anatomy# creates a path to", /*french*/"un #organe descendant# mène à", /*spanish*/"un #ser movedizo entre paredes# conduce a"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #pair of digested cows# hold", /*french*/"#deux boeufs digérés# détiennent", /*spanish*/"un #par de digeridas vacas# otorgan"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #pair of digested cows# hold", /*french*/"#deux boeufs digérés# détiennent", /*spanish*/"un #par de digeridas vacas# otorgan"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #final cows' reward# in a deity's belly is", /*french*/"le #cadeau des dernières vaches# estomacales est", /*spanish*/"las #vacas al final# de la tripa de cierta deidad otorgan"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#cows protected by falling monsters# in a deity's belly guard", /*french*/"des #vaches protégées par des monstres tombants# cachent", /*spanish*/"unas #vacas custodiadas por monstruos del techo# de la tripa de cierta deidad otorgan"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a school of #stingers swallowed by a deity# guard", /*french*/"les #raies avallées par le gardien# protègent", /*spanish*/"unos #stingers engullidos por cierta deidad# guardan"},
                     }, {},
                       //clear text
                       Text{"a school of #stingers swallowed by Jabu-Jabu# guard", /*french*/"les #raies avallées par Jabu-Jabu# protègent", /*spanish*/"unos #stingers engullidos por Jabu-Jabu# guardan"}
  );

    hintTable[JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a school of #stingers swallowed by a deity# guard", /*french*/"les #raies avallées par le gardien# protègent", /*spanish*/"unos #stingers engullidos por cierta deidad# guardan"},
                     }, {},
                       //clear text
                       Text{"a school of #stingers swallowed by Jabu-Jabu# guard", /*french*/"les #raies avallées par Jabu-Jabu# protègent", /*spanish*/"unos #stingers engullidos por Jabu-Jabu# guardan"}
  );

    hintTable[JABU_JABUS_BELLY_BARINADE_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Bio-Electric Anemone# holds", /*french*/"l'#anémone bioélectrique# possède", /*spanish*/"la #anémona bioeléctrica# porta"},
                     }, {},
                       //clear text
                       Text{"#Barinade# holds", /*french*/"#Barinade# possède", /*spanish*/"#Barinade# porta"}
  );

    hintTable[JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider resting near a princess# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula près de la princesse dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider resting near a princess# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula près de la princesse dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"#jellyfish surround a spider# holding", /*french*/"une #Skulltula entourée de méduses dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada de medusas# otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula protégée par des raies dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada por unos stingers# en la Tripa de Jabu-Jabu otorga"},
  });


    hintTable[JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider surrounded by electricity# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula entourée d'électricité dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada de electricidad# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula protégée par des raies dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula protegida por unos stingers# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a web within Jabu-Jabu's Belly# holds", /*french*/"une #Skulltula sur une toile dans le Ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula sobre una red# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_DEKU_SCRUB] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in a deity# sells", /*french*/"la #peste Mojo dans le ventre du gardien# vend", /*spanish*/"un #deku dentro de cierta deidad# vende"},
  });

  /*--------------------------
  |      FOREST TEMPLE       |
  ---------------------------*/
    hintTable[FOREST_TEMPLE_FIRST_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #tree in the Forest Temple# supports", /*french*/"sur l'#arbre dans le Temple de la Forêt# gît", /*spanish*/"sobre un #árbol del Templo del Bosque# yace"},
  });

    hintTable[FOREST_TEMPLE_FIRST_STALFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating enemies beneath a falling ceiling# in Forest Temple yields", /*french*/"#deux squelettes# dans le Temple de la Forêt protègent", /*spanish*/"#derrotar enemigos caídos de lo alto# del Templo del Bosque revela"},
  });

    hintTable[FOREST_TEMPLE_WELL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #sunken chest deep in the woods# contains", /*french*/"le #coffre submergé dans la forêt# contient", /*spanish*/"un #sumergido cofre en lo profundo del bosque# contiene"},
  });

    hintTable[FOREST_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #fiery skull# in Forest Temple guards", /*french*/"le #crâne enflammé# dans le Temple de la Forêt protège", /*spanish*/"una #ardiente calavera# del Templo del Bosque esconde"},
  });

    hintTable[FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest on a small island# in the Forest Temple holds", /*french*/"le #coffre sur l'îlot# du Temple de la Forêt contient", /*spanish*/"un #cofre sobre una isla# del Templo del Bosque contiene"},
  });

    hintTable[FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"beneath a #checkerboard falling ceiling# lies", /*french*/"sous #l'échiquier tombant# gît", /*spanish*/"tras un #techo de ajedrez# yace"},
  });

    hintTable[FOREST_TEMPLE_EYE_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #sharp eye# will spot", /*french*/"l'#oeil perçant# dans la forêt verra", /*spanish*/"un #afilado ojo# revela"},
                     }, {},
                       //clear text
                       Text{"#blocks of stone# in the Forest Temple surround", /*french*/"les #blocs dans le Temple de la Forêt# entourent", /*spanish*/"cerca de unos #bloques de piedra# del Templo del Bosque yace"}
  );

    hintTable[FOREST_TEMPLE_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #turned trunk# contains", /*french*/"le #coffre pivoté# contient", /*spanish*/"en una #sala con otro punto de vista# se esconde"},
  });

    hintTable[FOREST_TEMPLE_FLOORMASTER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"deep in the forest #shadows guard a chest# containing", /*french*/"l'#ombre de la forêt# protège un coffre contenant", /*spanish*/"en lo profundo del bosque #unas sombras# esconden"},
  });

    hintTable[FOREST_TEMPLE_BOW_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #army of the dead# guards", /*french*/"des #squelettes sylvestres# protègent", /*spanish*/"un #ejército de soldados caídos# guarda"},
                     }, {},
                       //clear text
                       Text{"#Stalfos deep in the Forest Temple# guard", /*french*/"#trois squelettes dans le Temple de la Forêt# protègent", /*spanish*/"los #Stalfos en lo profundo del Templo del Bosque# guardan"}
  );

    hintTable[FOREST_TEMPLE_RED_POE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Joelle# guards", /*french*/"#Joelle# protège", /*spanish*/"#Joelle# guarda"},
                     }, {},
                       //clear text
                       Text{"a #red ghost# guards", /*french*/"le #fantôme rouge# protège", /*spanish*/"un #espectro rojo# guarda"}
  );

    hintTable[FOREST_TEMPLE_BLUE_POE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Beth# guards", /*french*/"#Beth# protège", /*spanish*/"#Beth# guarda"},
                     }, {},
                       //clear text
                       Text{"a #blue ghost# guards", /*french*/"le #fantôme bleu# protège", /*spanish*/"un #espectro azul# guarda"}
  );

    hintTable[FOREST_TEMPLE_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#revolving walls# in the Forest Temple conceal", /*french*/"des #murs rotatifs dans la forêt# recèlent", /*spanish*/"las #paredes giratorias# del Templo del Bosque conceden"},
  });


    hintTable[FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #tree in the Forest Temple# supports", /*french*/"sur l'#arbre dans le Temple de la Forêt# gît", /*spanish*/"sobre un #árbol del Templo del Bosque# yace"},
  });

    hintTable[FOREST_TEMPLE_MQ_WOLFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating enemies beneath a falling ceiling# in Forest Temple yields", /*french*/"#deux squelettes# dans le Temple de la Forêt protègent", /*spanish*/"#derrotar enemigos caídos de lo alto# del Templo del Bosque revela"},
  });

    hintTable[FOREST_TEMPLE_MQ_BOW_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #army of the dead# guards", /*french*/"des #squelettes sylvestres# protègent", /*spanish*/"un #ejército de soldados caídos# guarda"},
                     }, {},
                       //clear text
                       Text{"#Stalfos deep in the Forest Temple# guard", /*french*/"#trois squelettes dans le Temple de la Forêt# protègent", /*spanish*/"los #Stalfos en lo profundo del Templo del Bosque# guardan"}
  );

    hintTable[FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest on a small island# in the Forest Temple holds", /*french*/"le #coffre sur l'îlot# du Temple de la Forêt contient", /*spanish*/"un #cofre sobre una isla# del Templo del Bosque contiene"},
  });

    hintTable[FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#high in a courtyard# within the Forest Temple is", /*french*/"#haut perché dans le jardin# du Temple de la Forêt gît", /*spanish*/"un #cofre en lo alto de un patio# del Templo del Bosque contiene"},
  });

    hintTable[FOREST_TEMPLE_MQ_WELL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #sunken chest deep in the woods# contains", /*french*/"le #coffre submergé dans la forêt# contient", /*spanish*/"un #sumergido cofre en lo profundo del bosque# contiene"},
  });

    hintTable[FOREST_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Joelle# guards", /*french*/"#Joelle# protège", /*spanish*/"#Joelle# guarda"},
                     }, {},
                       //clear text
                       Text{"a #red ghost# guards", /*french*/"le #fantôme rouge# protège", /*spanish*/"un #fantasma rojo# guarda"}
  );

    hintTable[FOREST_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Beth# guards", /*french*/"#Beth# protège", /*spanish*/"#Beth# guarda"},
                     }, {},
                       //clear text
                       Text{"a #blue ghost# guards", /*french*/"le #fantôme bleu# protège", /*spanish*/"un #fantasma azul# guarda"}
  );

    hintTable[FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"beneath a #checkerboard falling ceiling# lies", /*french*/"sous #l'échiquier tombant# gît", /*spanish*/"tras un #techo de ajedrez# yace"},
  });

    hintTable[FOREST_TEMPLE_MQ_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#revolving walls# in the Forest Temple conceal", /*french*/"des #murs rotatifs dans la forêt# recèlent", /*spanish*/"las #paredes giratorias# del Templo del Bosque conceden"},
  });

    hintTable[FOREST_TEMPLE_MQ_REDEAD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"deep in the forest #undead guard a chest# containing", /*french*/"des #revenants dans le Temple de la Forêt# protègent", /*spanish*/"en lo profundo del bosque #guardias del más allá# guardan"},
  });

    hintTable[FOREST_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #turned trunk# contains", /*french*/"le #coffre pivoté# contient", /*spanish*/"en una #sala con otro punto de vista# se esconde"},
  });

    hintTable[FOREST_TEMPLE_PHANTOM_GANON_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Evil Spirit from Beyond# holds", /*french*/"l'#esprit maléfique de l'au-delà# possède", /*spanish*/"el #espíritu maligno de ultratumba# porta"},
                     }, {},
                       //clear text
                       Text{"#Phantom Ganon# holds", /*french*/"#Ganon Spectral# possède", /*spanish*/"#Ganon Fantasma# porta"}
  );

    hintTable[FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on a small island# in the Forest Temple holds", /*french*/"une #Skulltula sur l'îlot du Temple de la Forêt# a", /*spanish*/"una #Skulltula sobre una pequeña isla# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_FIRST_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider high on a wall of vines# in the Forest Temple holds", /*french*/"une #Skulltula sur un mur de vignes du Temple de la Forêt# a", /*spanish*/"una #Skulltula en lo alto de una pared de cepas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"#stone columns# lead to a spider in the Forest Temple hiding", /*french*/"une #Skulltula haut perchée dans le jardin du Temple de la Forêt# a", /*spanish*/"unas #columnas del Templo del Bosque# conducen a una Skulltula que otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among ghosts# in the Forest Temple guards", /*french*/"une #Skulltula dans la grande salle du Temple de la Forêt# a", /*spanish*/"una #Skulltula rodeada de fantasmas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_BASEMENT] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider within revolving walls# in the Forest Temple holds", /*french*/"une #Skulltula derrière les murs pivotants du Temple de la Forêt# a", /*spanish*/"una #Skulltula entre paredes giratorias# del Templo del Bosque otorga"},
  });


    hintTable[FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY] = HintText::Exclude({
                       //obscure text
                       Text{"an #ivy-hidden spider# in the Forest Temple hoards", /*french*/"une #Skulltula près de l'entrée du Temple de la Forêt# a", /*spanish*/"una #Skulltula escondida entre cepas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a hidden nook# within the Forest Temple holds", /*french*/"une #Skulltula dans un recoin caché du Temple de la Forêt# a", /*spanish*/"una #Skulltula en una esquina oculta# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on an arch# in the Forest Temple holds", /*french*/"une #Skulltula sur une arche du Temple de la Forêt# a", /*spanish*/"una #Skulltula sobre un arco# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on a ledge# in the Forest Temple holds", /*french*/"une #Skulltula dans le jardin du Temple de la Forêt# a", /*spanish*/"una #Skulltula en un borde# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_WELL] = HintText::Exclude({
                       //obscure text
                       Text{"#draining a well# in Forest Temple uncovers a spider with", /*french*/"une #Skulltula au fond du Puits du Temple de la Forêt# a", /*spanish*/"#vaciar el pozo# del Templo del Bosque desvela una Skulltula que otorga"},
  });

  /*--------------------------
  |       FIRE TEMPLE        |
  ---------------------------*/
    hintTable[FIRE_TEMPLE_NEAR_BOSS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#near a dragon# is", /*french*/"#près d'un dragon# gît", /*spanish*/"#cerca de un dragón# yace"},
  });

    hintTable[FIRE_TEMPLE_FLARE_DANCER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Flare Dancer behind a totem# guards", /*french*/"le #Danse-Flamme derrière un totem# protège", /*spanish*/"el #Bailafuego tras unos tótems# esconde"},
  });

    hintTable[FIRE_TEMPLE_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #prison beyond a totem# holds", /*french*/"la #prison derrière un totem# contient", /*spanish*/"en una #prisión tras unos tótems# yace"},
  });

    hintTable[FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#explosives over a lava pit# unveil", /*french*/"des #explosifs dans un lac de lave# révèlent", /*spanish*/"los #explosivos en un mar de llamas# revelan"},
  });

    hintTable[FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron trapped near lava# holds", /*french*/"le #Goron emprisonné près de la lave# a", /*spanish*/"un #goron atrapado cerca de un mar de llamas# guarda"},
  });

    hintTable[FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron at the end of a maze# holds", /*french*/"le #Goron dans le labyrinthe# a", /*spanish*/"un #goron al final de un laberinto# guarda"},
  });

    hintTable[FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron above a maze# holds", /*french*/"le #Goron au dessus du labyrinthe# a", /*spanish*/"un #goron sobre un laberinto# guarda"},
  });

    hintTable[FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron hidden near a maze# holds", /*french*/"le #Goron caché près du labyrinthe# a", /*spanish*/"un #goron escondido tras un laberinto# guarda"},
  });

    hintTable[FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blocked path# in Fire Temple holds", /*french*/"un #sol fragile dans le Temple du Feu# contient", /*spanish*/"en un #camino bloqueado# del Templo del Fuego yace"},
  });

    hintTable[FIRE_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #caged chest# in the Fire Temple hoards", /*french*/"un #coffre emprisonné# dans le Temple du Feu contient", /*spanish*/"un #cofre entre rejas# del Templo del Fuego contiene"},
  });

    hintTable[FIRE_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest in a fiery maze# contains", /*french*/"un #coffre dans un labyrinthe enflammé# contient", /*spanish*/"un #cofre de un ardiente laberinto# contiene"},
  });

    hintTable[FIRE_TEMPLE_HIGHEST_GORON_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron atop the Fire Temple# holds", /*french*/"le #Goron au sommet du Temple du Feu# a", /*spanish*/"un #goron en lo alto del Templo del Fuego# guarda"},
  });


    hintTable[FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#near a dragon# is", /*french*/"#près d'un dragon# gît", /*spanish*/"#cerca de un dragón# yace"},
  });

    hintTable[FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Flare Dancer in the depths of a volcano# guards", /*french*/"le #Danse-Flamme au coeur du volcan# a", /*spanish*/"el #Bailafuego en lo profundo del volcán# esconde"},
                     }, {},
                       //clear text
                       Text{"the #Flare Dancer in the depths of the Fire Temple# guards", /*french*/"le #Danse-Flamme au coeur du volcan# a", /*spanish*/"el #Bailafuego en lo profundo del Templo del Fuego# esconde"}
  );

    hintTable[FIRE_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blocked path# in Fire Temple holds", /*french*/"le #chemin scellé# dans le Temple du Feu contient", /*spanish*/"en un #camino bloqueado# del Templo del Fuego yace"},
  });

    hintTable[FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#crates in a maze# contain", /*french*/"des #boîtes dans le labyrinthe# contiennent", /*spanish*/"las #cajas de un laberinto# contienen"},
  });

    hintTable[FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#crates in a maze# contain", /*french*/"des #boîtes dans le labyrinthe# contiennent", /*spanish*/"las #cajas de un laberinto# contienen"},
  });

    hintTable[FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #falling slug# in the Fire Temple guards", /*french*/"la #limace tombante# dans le Temple du Feu protège", /*spanish*/"una #babosa del techo# del Templo del Fuego guarda"},
  });

    hintTable[FIRE_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"using a #hammer in the depths of the Fire Temple# reveals", "frapper du #marteau au coeur du volcan# révèle", /*spanish*/"usar el #martillo en lo profundo del Templo del Fuego# revela"},
  });

    hintTable[FIRE_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#illuminating a lava pit# reveals the path to", /*french*/"#éclairer le lac de lave# révèle", /*spanish*/"#iluminar un mar de llamas# revela"},
  });

    hintTable[FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#explosives over a lava pit# unveil", /*french*/"des #explosifs dans un lac de lave# révèlent", /*spanish*/"los #explosivos en un mar de llamas# revelan"},
  });

    hintTable[FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron hidden near a maze# holds", /*french*/"le #Goron caché près du labyrinthe# a", /*spanish*/"un #goron cerca de un laberinto# guarda"},
  });

    hintTable[FIRE_TEMPLE_MQ_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"hidden #beneath a block of stone# lies", /*french*/"caché #derrière un bloc de pierre# gît", /*spanish*/"#bajo unos bloques de piedra# yace"},
  });

    hintTable[FIRE_TEMPLE_VOLVAGIA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Subterranean Lava Dragon# holds", /*french*/"le #dragon des profondeurs# possède", /*spanish*/"el #dragón de lava subterráneo# porta"},
                     }, {},
                       //clear text
                       Text{"#Volvagia# holds", /*french*/"#Volvagia# possède", /*spanish*/"#Volvagia# porta"}
  );

    hintTable[FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#eight tiles of malice# guard a spider holding", /*french*/"une #Skulltula protégée par huit tuiles dans le Temple du Feu# a", /*spanish*/"#ocho baldosas de maldad# custodian una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_BOSS_KEY_LOOP] = HintText::Exclude({
                       //obscure text
                       Text{"#five tiles of malice# guard a spider holding", /*french*/"une #Skulltula protégée par cinq tuiles dans le Temple du Feu# a", /*spanish*/"#cinco baldosas de maldad# custodian una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_BOULDER_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"#explosives in a maze# unveil a spider hiding", /*french*/"une #Skulltula derrière un mur fragile du Temple du Feu# a", /*spanish*/"los #explosivos en un laberinto# desvelan una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_SCARECROW_TOP] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-friendly scarecrow# atop a volcano hides", /*french*/"une #Skulltula repérée par l'épouvantail du volcan# a", /*spanish*/"un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider-friendly scarecrow# atop the Fire Temple hides", /*french*/"une #Skulltula repérée par l'épouvantail du Temple du Feu# a", /*spanish*/"un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga"}
  );

    hintTable[FIRE_TEMPLE_GS_SCARECROW_CLIMB] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-friendly scarecrow# atop a volcano hides", /*french*/"une #Skulltula repérée par l'épouvantail du volcan# a", /*spanish*/"un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider-friendly scarecrow# atop the Fire Temple hides", /*french*/"une #Skulltula repérée par l'épouvantail du Temple du Feu# a", /*spanish*/"un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga"}
  );


    hintTable[FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider above a fiery maze# holds", /*french*/"une #Skulltula au dessus du labyrinthe enflammé du Temple du Feu# a", /*spanish*/"una #Skulltula sobre un ardiente laberinto# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider within a fiery maze# holds", /*french*/"une #Skulltula dans le labyrinthe enflammé du Temple du Feu# a", /*spanish*/"una #Skulltula en el interior de un ardiente laberinto# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron trapped near lava# befriended a spider with", /*french*/"une #Skulltula emprisonnée près du lac de lave du Temple du Feu# a", /*spanish*/"una #Skulltula amiga de un Goron atrapado junto a la lava# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beside a fiery maze# holds", /*french*/"une #Skulltula près du labyrinthe enflammé du Temple du Feu# a", /*spanish*/"una #Skulltula junto a un ardiente laberinto# otorga"},
  });

  /*--------------------------
  |       WATER TEMPLE       |
  ---------------------------*/
    hintTable[WATER_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#rolling spikes# in the Water Temple surround", /*french*/"des #Spikes# dans le Temple de l'Eau entourent", /*spanish*/"unas #rodantes púas# del Templo del Agua guardan"},
  });

    hintTable[WATER_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#roaming stingers in the Water Temple# guard", /*french*/"des #raies dans le Temple de l'Eau# protègent", /*spanish*/"unos #errantes stingers# del Templo del Agua guardan"},
  });

    hintTable[WATER_TEMPLE_TORCHES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire in the Water Temple# reveals", /*french*/"des #flammes dans le Temple de l'Eau# révèlent", /*spanish*/"el #fuego en el Templo del Agua# revela"},
  });

    hintTable[WATER_TEMPLE_DRAGON_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #serpent's prize# in the Water Temple is", /*french*/"la #récompense du dragon submergé# est", /*spanish*/"el #escamado premio# del Templo del Agua se trata de"},
  });

    hintTable[WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#blinding an eye# in the Water Temple leads to", /*french*/"#l'oeil# du Temple de l'Eau voit", /*spanish*/"#cegar un ojo# del Templo del Agua conduce a"},
  });

    hintTable[WATER_TEMPLE_CENTRAL_PILLAR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #depths of the Water Temple# lies", /*french*/"le #coeur du Temple de l'Eau# cache", /*spanish*/"en las #profundidades del Templo del Agua# yace"},
  });

    hintTable[WATER_TEMPLE_CRACKED_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#through a crack# in the Water Temple is", /*french*/"le #mur fragile# du Temple de l'Eau cache", /*spanish*/"tras una #agrietada pared# del Templo del Agua yace"},
  });

    hintTable[WATER_TEMPLE_LONGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#facing yourself# reveals",              /*french*/"se #vaincre soi-même# révèle", /*spanish*/"#luchar contra ti mismo# revela"},
                       Text{"a #dark reflection# of yourself guards", /*french*/"son #propre reflet# cache",    /*spanish*/"el #oscuro reflejo de ti mismo# guarda"},
                     }, {},
                       //clear text
                       Text{"#Dark Link# guards", /*french*/"l'#Ombre de @# protège", /*spanish*/"#@ Oscuro# guarda"}
  );


    hintTable[WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #depths of the Water Temple# lies", /*french*/"le #coeur du Temple de l'Eau# cache", /*spanish*/"en las #profundidades del Templo del Agua# yace"},
  });

    hintTable[WATER_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"fire in the Water Temple unlocks a #vast gate# revealing a chest with", /*french*/"des #flammes au coeur du Temple de l'Eau# révèlent", /*spanish*/"el fuego en el Templo del Agua alza una #gran valla# con"},
  });

    hintTable[WATER_TEMPLE_MQ_LONGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#through a crack# in the Water Temple is", /*french*/"le #mur fragile# du Temple de l'Eau cache", /*spanish*/"tras una #agrietada pared# del Templo del Agua yace"},
  });

    hintTable[WATER_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire in the Water Temple# reveals", /*french*/"des #flammes dans le Temple de l'Eau# révèlent", /*spanish*/"el #fuego en el Templo del Agua# revela"},
  });

    hintTable[WATER_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#sparring soldiers# in the Water Temple guard", /*french*/"les #soldats du Temple de l'Eau# protègent", /*spanish*/"#acabar con unos soldados# del Templo del Agua revela"},
  });

    hintTable[WATER_TEMPLE_MORPHA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Giant Aquatic Amoeba# holds", /*french*/"l'#amibe aquatique géante# possède", /*spanish*/"la #ameba acuática gigante# porta"},
                     }, {},
                       //clear text
                       Text{"#Morpha# holds", /*french*/"#Morpha# possède", /*spanish*/"#Morpha# porta"}
  );

    hintTable[WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a waterfall# in the Water Temple holds", /*french*/"une #Skulltula au dessus d'une cascade du Temple de l'Eau# a", /*spanish*/"una #Skulltula tras una cascada# del Templo del Agua otorga"},
  });

    hintTable[WATER_TEMPLE_GS_CENTRAL_PILLAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in the center of the Water Temple# holds", /*french*/"une #Skulltula au centre du Temple de l'Eau# a", /*spanish*/"una #Skulltula en el centro del Templo del Agua# otorga"},
  });

    hintTable[WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a spider protected by #rolling boulders under the lake# hides", /*french*/"une #Skulltula derrière les rochers roulants sous le lac# a", /*spanish*/"una #Skulltula protegida por rocas rodantes# bajo el lago otorga"},
                     }, {},
                       //clear text
                       Text{"a spider protected by #rolling boulders in the Water Temple# hides", /*french*/"une #Skulltula derrière les rochers roulants du Temple de l'Eau# a", /*spanish*/"una #Skulltula protegida por rocas rodantes# del Templo del Agua otorga"}
  );

    hintTable[WATER_TEMPLE_GS_RIVER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a river# in the Water Temple holds", /*french*/"une #Skulltula au dessus de la rivière du Temple de l'Eau# a", /*spanish*/"una #Skulltula sobre un río# del Templo del Agua otorga"},
  });


    hintTable[WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond a pit of lizards# is a spider holding", /*french*/"une #Skulltula près des lézards du Temple de l'Eau# a", /*spanish*/"#más allá de un pozo de reptiles# una Skulltula otorga"},
  });

    hintTable[WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY] = HintText::Exclude({
                       //obscure text
                       Text{"#lizards guard a spider# in the Water Temple with", /*french*/"une #Skulltula dans les couloirs croisés du Temple de l'Eau# a", /*spanish*/"unos #reptiles custodian una Skulltula# del Templo del Agua que otorga"},
  });

    hintTable[WATER_TEMPLE_MQ_GS_RIVER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a river# in the Water Temple holds", /*french*/"une #Skulltula au dessus de la rivière du Temple de l'Eau# a", /*spanish*/"una #Skulltula sobre un río# del Templo del Agua otorga"},
  });

  /*--------------------------
  |      SPIRIT TEMPLE       |
  ---------------------------*/
    hintTable[SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a child conquers a #skull in green fire# in the Spirit Temple to reach", /*french*/"le #crâne au halo vert dans le colosse# cache", /*spanish*/"el joven que #baje el puente# del Templo del Espíritu encontrará"},
  });

    hintTable[SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a child can find a #caged chest# in the Spirit Temple with", /*french*/"le #coffre embarré dans le colosse# contient", /*spanish*/"un joven puede encontrar un #cofre entre rejas# del Templo del Espíritu con"},
  });

    hintTable[SPIRIT_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#across a pit of sand# in the Spirit Temple lies", /*french*/"le #trou sableux dans le colosse# a", /*spanish*/"tras un #pozo de arena# del Templo del Espíritu yace"},
  });

    hintTable[SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#dodging boulders to collect silver rupees# in the Spirit Temple yields", /*french*/"les #pièces argentées entourées de rochers dans le colosse# révèlent", /*spanish*/"#esquivar rocas y conseguir plateadas rupias# en el Templo del Espíritu conduce a"},
  });

    hintTable[SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #shadow circling reflected light# in the Spirit Temple guards", /*french*/"l'#ombre près d'un miroir# protège", /*spanish*/"un #círculo de reflectante luz# del Templo del Espíritu guarda"},
  });

    hintTable[SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #shadow circling reflected light# in the Spirit Temple guards", /*french*/"l'#ombre près d'un miroir# protège", /*spanish*/"un #círculo de reflectante luz# del Templo del Espíritu guarda"},
  });

    hintTable[SPIRIT_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#before a giant statue# in the Spirit Temple lies", /*french*/"#devant la statue# dans le colosse gît", /*spanish*/"#ante una gran estatua# del Templo del Espíritu aguarda"},
  });

    hintTable[SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#lizards in the Spirit Temple# guard", /*french*/"les #lézards dans le colosse# protègent", /*spanish*/"los #reptiles del Templo del Espíritu# guardan"},
  });

    hintTable[SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#lizards in the Spirit Temple# guard", /*french*/"les #lézards dans le colosse# protègent", /*spanish*/"los #reptiles del Templo del Espíritu# guardan"},
  });

    hintTable[SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#torchlight among Beamos# in the Spirit Temple reveals", /*french*/"les #torches autour des Sentinelles# éclairent", /*spanish*/"las #antorchas junto a Beamos# del Templo del Espíritu revelan"},
  });

    hintTable[SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #statue in the Spirit Temple# holds", /*french*/"la #statue dans le colosse# tient", /*spanish*/"una #estatua del Templo del Espíritu# esconde"},
  });

    hintTable[SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"on a #ledge by a statue# in the Spirit Temple rests", /*french*/"#haut perché près de la statue# dans le colosse gît", /*spanish*/"al #borde de una estatua# del Templo del Espíritu yace"},
  });

    hintTable[SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"those who #show the light among statues# in the Spirit Temple find", /*french*/"le #soleil près des statues# cache", /*spanish*/"aquellos que #iluminen ante las estatuas# del Templo del Espíritu encontrarán"},
  });

    hintTable[SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the Spirit Temple# reveals", /*french*/"le #trésor invisible près du Hache-Viande# contient", /*spanish*/"el #Ojo de la Verdad# en el Templo del Espíritu revela"},
  });

    hintTable[SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the Spirit Temple# reveals", /*french*/"le #trésor invisible près du Hache-Viande# contient", /*spanish*/"el #Ojo de la Verdad# en el Templo del Espíritu revela"},
  });

    hintTable[SPIRIT_TEMPLE_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest engulfed in flame# in the Spirit Temple holds", /*french*/"le #coffre enflammé dans le colosse# contient", /*spanish*/"un #cofre rodeado de llamas# del Templo del Espíritu contiene"},
  });

    hintTable[SPIRIT_TEMPLE_TOPMOST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"those who #show the light above the Colossus# find", /*french*/"le #soleil au sommet du colosse# révèle", /*spanish*/"aquellos que #iluminen en lo alto del Coloso# encontrarán"},
  });


    hintTable[SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#lying unguarded# in the Spirit Temple is", /*french*/"dans #l'entrée du colosse# se trouve", /*spanish*/"en la #entrada del Templo del Espíritu# yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #switch in a pillar# within the Spirit Temple drops", /*french*/"l'#interrupteur dans un pilier# du colosse cache", /*spanish*/"el #interruptor de un pilar# del Templo del Espíritu revela"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#collecting rupees through a water jet# reveals", /*french*/"les #pièces argentées dans le jet d'eau# du colosse révèlent", /*spanish*/"#hacerte con rupias tras un géiser# revela"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #eye blinded by stone# within the Spirit Temple conceals", /*french*/"#l'oeil derrière le rocher# dans le colosse voit", /*spanish*/"#cegar a un ojo# del Templo del Espíritu revela"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"surrounded by #fire and wrappings# lies", /*french*/"près des #pierres tombales dans le colosse# gît", /*spanish*/"rodeado de #fuego y vendas# yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a child defeats a #gauntlet of monsters# within the Spirit Temple to find", /*french*/"l'enfant qui vainc #plusieurs monstres# dans le colosse trouvera", /*spanish*/"el joven que derrote #unos monstruos# del Templo del Espíritu encontrará"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#explosive sunlight# within the Spirit Temple uncovers", /*french*/"le #rayon de lumière explosif dans le colosse# révèle", /*spanish*/"una #explosiva luz solar# del Templo del Espíritu revela"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#trapped by falling enemies# within the Spirit Temple is", /*french*/"des #ennemis tombants# dans le colosse protègent", /*spanish*/"#rodeado de enemigos del cielo# del Templo del Espíritu yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#blinding the colossus# unveils", /*french*/"#l'oeil dans le colosse# voit", /*spanish*/"#cegar al coloso# revela"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody awakens the colossus# to reveal", /*french*/"la #mélodie royale éveille le colosse# et révèle", /*spanish*/"la #melodía real que despierte al coloso# revelará"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# finds the colossus's hidden", /*french*/"#l'oeil de vérité# verra dans le colosse", /*spanish*/"el #Ojo de la Verdad# en el Templo del Espíritu encontrará"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#the old hide what the young find# to reveal", /*french*/"l'#oeil dans le trou du bloc argent# dans le colosse voit", /*spanish*/"el #adulto esconde lo que el joven anhela#, revelando"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#sunlight in a maze of fire# hides", /*french*/"#la lumière dans le labyrinthe de feu# du colosse révèle", /*spanish*/"la #luz solar de un ígneo laberinto# esconde"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#across a pit of sand# in the Spirit Temple lies", /*french*/"le #trou sableux# dans le colosse a", /*spanish*/"#a través del pozo de arena# del Templo del Espíritu yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"where #temporal stone blocks the path# within the Spirit Temple lies", /*french*/"les #pierres temporelles# dans le colosse cachent", /*spanish*/"donde los #bloques temporales bloquean# en el Templo del Espíritu yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest of double purpose# holds", /*french*/"le #coffre à usage double# du colosse contient", /*spanish*/"un #cofre de doble uso# contiene"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #temporal stone blocks the light# leading to", /*french*/"la #pierre temporelle# le colosse fait ombre sur", /*spanish*/"un #bloque temporal bloquea la luz# que conduce a"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"those who #show the light above the Colossus# find", /*french*/"le trésor invisible #au sommet du colosse# contient", /*spanish*/"aquellos que #revelen la luz sobre el Coloso# encontrarán"},
  });

    hintTable[SPIRIT_TEMPLE_TWINROVA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Sorceress Sisters# hold", /*french*/"#les sorcières jumelles# possède", /*spanish*/"las #hermanas hechiceras# portan"},
                     }, {},
                       //clear text
                       Text{"#Twinrova# holds", /*french*/"#Twinrova# possède", /*spanish*/"#Birova# porta"}
    );

    hintTable[SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula au dessus d'un escalier du Temple de l'Esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_BOULDER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider behind a temporal stone# in the Spirit Temple yields", /*french*/"une #Skulltula derrière une pierre temporelle du Temple de l'Esprit# a", /*spanish*/"una #Skulltula tras un bloque temporal# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beside a statue# holds", /*french*/"une #Skulltula dans la grande salle du Temple de l'Esprit# a", /*spanish*/"una #Skulltula junto a una estatua# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider at the top of a deep shaft# in the Spirit Temple holds", /*french*/"une #Skulltula près d'un mur d'escalade du Temple de l'Esprit# a", /*spanish*/"una #Skulltula en lo alto de un gran hueco# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_METAL_FENCE] = HintText::Exclude({
                       //obscure text
                       Text{"a child defeats a #spider among bats# in the Spirit Temple to gain", /*french*/"une #Skulltula sur le grillage du Temple de l'Esprit# a", /*spanish*/"el joven que derrote la #Skulltula entre murciélagos# del Templo del Espíritu hallará"},
  });


    hintTable[SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#above a pit of sand# in the Spirit Temple hides", /*french*/"une #Skulltula au dessus du trou sableux du Temple de l'Esprit# a", /*spanish*/"una #Skulltula sobre un pozo de arena# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula dans la salle aux neuf trônes du Temple de l'Esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula dans la salle aux neuf trônes du Temple de l'Esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#upon a web of glass# in the Spirit Temple sits a spider holding", /*french*/"une #Skulltula sur une paroi de verre du Temple de l'Esprit# a", /*spanish*/"#sobre una plataforma de cristal# yace una Skulltula que otorga"},
  });

  /*--------------------------
  |      SHADOW TEMPLE       |
  ---------------------------*/
    hintTable[SHADOW_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# pierces a hall of faces to reveal", /*french*/"l'#oeil de vérité# voit dans les couloirs du Temple de l'Ombre", /*spanish*/"el #Ojo de la Verdad# descubrirá un pasillo de facetas con"},
  });

    hintTable[SHADOW_TEMPLE_HOVER_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #nether dweller in the Shadow Temple# holds", /*french*/"le #spectre du Temple de l'Ombre# a", /*spanish*/"un #temido morador del Templo de las Sombras# guarda"},
                     }, {},
                       //clear text
                       Text{"#Dead Hand in the Shadow Temple# holds", /*french*/"le #Poigneur dans le Temple de l'Ombre# cache", /*spanish*/"la #Mano Muerta del Templo de las Sombras# guarda"}
  );

    hintTable[SHADOW_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies revealed by the Eye of Truth# guard", /*french*/"les #Gibdos dans les couloirs# du Temple de l'Ombre protègent", /*spanish*/"las #momias reveladas por el Ojo de la Verdad# guardan"},
  });

    hintTable[SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#spinning scythes# protect", /*french*/"les #faucheurs danseurs# du Temple de l'Ombre protègent", /*spanish*/"las #giratorias guadañas# protegen"},
  });

    hintTable[SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du Temple de l'Ombre protègent", /*spanish*/"las #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du Temple de l'Ombre protègent", /*spanish*/"las #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #dead roam among invisible spikes# guarding", /*french*/"#parmi les clous invisibles# du Temple de l'Ombre se cache", /*spanish*/"los #muertos que vagan por pinchos invisibles# protegen"},
  });

    hintTable[SHADOW_TEMPLE_WIND_HINT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible chest guarded by the dead# holds", /*french*/"le #trésor invisible du cul-de-sac# du Temple de l'Ombre contient", /*spanish*/"un #cofre invisible custodiado por los del más allá# contiene"},
  });

    hintTable[SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies guarding a ferry# hide", /*french*/"les #Gibdos qui bloquent le traversier# cachent", /*spanish*/"las #momias que protegen un navío# esconden"},
  });

    hintTable[SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies guarding a ferry# hide", /*french*/"les #Gibdos qui bloquent le traversier# cachent", /*spanish*/"las #momias que protegen un navío# esconden"},
  });

    hintTable[SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du Temple de l'Ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du Temple de l'Ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#inside a burning skull# lies", /*french*/"#dans un crâne enflammé# gît", /*spanish*/"en el #interior de una calavera en llamas# aguarda"},
  });


    hintTable[SHADOW_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# pierces a hall of faces to reveal", /*french*/"l'#oeil de vérité# voit dans les couloirs du Temple de l'Ombre", /*spanish*/"el #Ojo de la Verdad# descubre un pasillo de facetas con"},
  });

    hintTable[SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand in the Shadow Temple# holds", /*french*/"le #Poigneur dans le Temple de l'Ombre# cache", /*spanish*/"la #Mano Muerta del Templo de las Sombras# guarda"},
  });

    hintTable[SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies revealed by the Eye of Truth# guard", /*french*/"les #Gibdos dans les couloirs# du Temple de l'Ombre protègent", /*spanish*/"las #momias reveladas por el Ojo de la Verdad# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#spinning scythes# protect", /*french*/"les #faucheurs danseurs# du Temple de l'Ombre protègent", /*spanish*/"las #giratorias guadañas# protegen"},
  });

    hintTable[SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#collecting rupees in a vast cavern# with the Shadow Temple unveils", /*french*/"les #pièces argentées dans le Temple de l'Ombre# révèlent", /*spanish*/"hacerte con las #rupias en una gran caverna# del Templo de las Sombras revela"},
  });

    hintTable[SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#falling spikes# block the path to", /*french*/"la #pluie de clous# surplombe", /*spanish*/"los #pinchos de un techo# conducen a"},
  });

    hintTable[SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #dead roam among invisible spikes# guarding", /*french*/"#parmi les clous invisibles# du Temple de l'Ombre se cache", /*spanish*/"los #muertos que vagan por pinchos invisibles# protegen"},
  });

    hintTable[SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du Temple de l'Ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du Temple de l'Ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"near an #empty pedestal# within the Shadow Temple lies", /*french*/"#près d'un pédestal vide du Temple de l'Ombre# gît", /*spanish*/"cerca de un #vacío pedestal# del Templo de las Sombras yace"},
  });

    hintTable[SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du Temple de l'Ombre protègent", /*spanish*/"unas #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du Temple de l'Ombre protègent", /*spanish*/"unas #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_WIND_HINT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible chest guarded by the dead# holds", /*french*/"le #trésor invisible du cul-de-sac# du Temple de l'Ombre contient", /*spanish*/"un #cofre invisible custodiado por los del más allá# contiene"},
  });

    hintTable[SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies guarding a ferry# hide", /*french*/"les #Gibdos qui bloquent le traversier# cachent", /*spanish*/"las #momias que protegen un navío# esconden"},
  });

    hintTable[SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies guarding a ferry# hide", /*french*/"les #Gibdos qui bloquent le traversier# cachent", /*spanish*/"las #momias que protegen un navío# esconden"},
  });

    hintTable[SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#caged near a ship# lies", /*french*/"#dans une cage près du traversier# gît", /*spanish*/"#entre rejas al lado de un navío# yace"},
  });

    hintTable[SHADOW_TEMPLE_MQ_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#behind three burning skulls# lies", /*french*/"#derrière trois crânes enflammés# gît", /*spanish*/"tras #tres ardientes calaveras# yace"},
  });

    hintTable[SHADOW_TEMPLE_BONGO_BONGO_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Phantom Shadow Beast# holds", /*french*/"le #monstre de l'ombre# possède", /*spanish*/"la #alimaña oscura espectral# porta"},
                     }, {},
                       //clear text
                       Text{"#Bongo Bongo# holds", /*french*/"#Bongo Bongo# possède", /*spanish*/"#Bongo Bongo# porta"}
    );

    hintTable[SHADOW_TEMPLE_GS_SINGLE_GIANT_POT] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond a burning skull# lies a spider with", /*french*/"une #Skulltula derrière un crâne enflammé du Temple de l'Ombre# a", /*spanish*/"#tras una ardiente calavera# yace una Skulltula que otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beyond falling spikes# holds", /*french*/"une #Skulltula au delà de la pluie de clous du Temple de l'Ombre# a", /*spanish*/"una #Skulltula tras los pinchos del techo# otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond three burning skulls# lies a spider with", /*french*/"une #Skulltula derrière trois crânes enflammés du Temple de l'Ombre# a", /*spanish*/"#tras tres ardientes calaveras# yace una Skulltula que otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a spider guarded by #invisible blades# holds", /*french*/"une #Skulltula protégée par les faucheurs invisibles du Temple de l'Ombre# a", /*spanish*/"una #Skulltula custodiada por hojas invisibles# otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_NEAR_SHIP] = HintText::Exclude({
                       //obscure text
                       Text{"a spider near a #docked ship# hoards", /*french*/"une #Skulltula près du traversier du Temple de l'Ombre# a", /*spanish*/"una #Skulltula cercana a un navío# otorga"},
  });


    hintTable[SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beyond falling spikes# holds", /*french*/"une #Skulltula au delà de la pluie de clous du Temple de l'Ombre# a", /*spanish*/"una #Skulltula tras los pinchos del techo# otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider amidst roaring winds# in the Shadow Temple holds", /*french*/"une #Skulltula près des vents du Temple de l'Ombre# a", /*spanish*/"una #Skulltula entre ventarrones# del Templo de las Sombras otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_AFTER_WIND] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beneath gruesome debris# in the Shadow Temple hides", /*french*/"une #Skulltula sous des débris du Temple de l'Ombre# a", /*spanish*/"una #Skulltula bajo unos horripilantes escombros# del Templo de las Sombras otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_AFTER_SHIP] = HintText::Exclude({
                       //obscure text
                       Text{"a #fallen statue# reveals a spider with", /*french*/"une #Skulltula près de la statue écroulée du Temple de l'Ombre# a", /*spanish*/"una #estatua caída# revelará una Skulltula que otorgue"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"a #suspended spider# guards", /*french*/"une #Skulltula près du repère du Temple de l'Ombre# a", /*spanish*/"una #Skulltula flotante# del Templo de las Sombras otorga"},
  });

  /*--------------------------
  |    BOTTOM OF THE WELL    |
  ---------------------------*/
    hintTable[BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the well# reveals", /*french*/"l'#oeil de vérité dans le Puits# révèle", /*spanish*/"el #Ojo de la Verdad en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#gruesome debris# in the well hides", /*french*/"des #débris dans le Puits# cachent", /*spanish*/"unos #horripilantes escombros# del pozo esconden"},
  });

    hintTable[BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the well# reveals", /*french*/"l'#oeil de vérité dans le Puits# révèle", /*spanish*/"el #Ojo de la Verdad en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hidden entrance to a cage# in the well leads to", /*french*/"dans un #chemin caché dans le Puits# gît", /*spanish*/"la #entrada oculta de una celda# del pozo conduce a"},
  });

    hintTable[BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarding a cage# in the well protects", /*french*/"l'#araignée dans la cage du Puits# protège", /*spanish*/"una #araña protegiendo una celda# del pozo guarda"},
  });

    hintTable[BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#gruesome debris# in the well hides", /*french*/"des #débris dans le Puits# cachent", /*spanish*/"unos #horripilantes escombros# del pozo esconden"},
  });

    hintTable[BOTTOM_OF_THE_WELL_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand's invisible secret# is", /*french*/"le #trésor invisible du Poigneur# est", /*spanish*/"el #secreto invisible de la Mano Muerta# esconde"},
  });

    hintTable[BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le Puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le Puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #depths of the well# lies", /*french*/"#dans le coeur du Puits# gît", /*spanish*/"en las #profundidades del pozo# yace"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#perilous pits# in the well guard the path to", /*french*/"#trois trous# dans le Puits protègent", /*spanish*/"#peligrosos fosos# del pozo conducen a"},
  });

    hintTable[BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#locked in a cage# in the well lies", /*french*/"#dans une cage# du Puits gît", /*spanish*/"#entre rejas# en el pozo yace"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#inside a coffin# hides", /*french*/"dans #un cercueil# gît", /*spanish*/"en el #interior de un ataúd# yace"},
  });


    hintTable[BOTTOM_OF_THE_WELL_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le Puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #army of the dead# in the well guards", /*french*/"l'#armée des morts# dans le Puits protège", /*spanish*/"un #ejército del más allá# del pozo guarda"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand's explosive secret# is", /*french*/"le #secret explosif du Poigneur# est", /*spanish*/"el #explosivo secreto de la Mano Muerta# esconde"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible path in the well# leads to", /*french*/"dans un #chemin caché dans le Puits# gît", /*spanish*/"un #camino invisible del pozo# conduce a"},
  });

    hintTable[BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a cage# in the well holds", /*french*/"une #Skulltula dans une cage au fonds du Puits# a", /*spanish*/"una #Skulltula enjaulada# del pozo otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible path in the well# leads to", /*french*/"une #Skulltula dans le chemin invisible au fonds du Puits# a", /*spanish*/"un #camino invisible del pozo# conduce a una Skulltula que otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a crypt# within the well guards", /*french*/"une #Skulltula embarrée dans la crypte au fonds du Puits# a", /*spanish*/"una #Skulltula encerrada en una cripta# del pozo otorga"},
  });


    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT] = HintText::Exclude({
                       //obscure text
                       Text{"a #gauntlet of invisible spiders# protects", /*french*/"une #Skulltula protégée par les araignées invisibles au fonds du Puits# a", /*spanish*/"unas #arañas invisibles# custodian una Skulltula que otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider crawling near the dead# in the well holds", /*french*/"une #Skulltula près des cercueils au fonds du Puits# a", /*spanish*/"una #Skulltula junto a los muertos# del pozo otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a crypt# within the well guards", /*french*/"une #Skulltula embarrée dans la crypte au fonds du Puits# a", /*spanish*/"una #Skulltula encerrada en una cripta# del pozo otorga"},
  });

  /*--------------------------
  |        ICE CAVERN        |
  ---------------------------*/
    hintTable[ICE_CAVERN_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#winds of ice# surround", /*french*/"#figé dans la glace rouge# gît", /*spanish*/"#heladas borrascas# rodean"},
  });

    hintTable[ICE_CAVERN_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #wall of ice# protects", /*french*/"#un mur de glace rouge# cache", /*spanish*/"una #gélida pared# protege"},
  });

    hintTable[ICE_CAVERN_IRON_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #monster in a frozen cavern# guards", /*french*/"le #monstre de la caverne de glace# protège", /*spanish*/"un #monstruo de una helada caverna# guarda"},
  });

    hintTable[ICE_CAVERN_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"a #wall of ice# protects", /*french*/"un #mur de glace rouge# cache", /*spanish*/"una #gélida pared# protege"},
  });


    hintTable[ICE_CAVERN_MQ_IRON_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #monster in a frozen cavern# guards", /*french*/"le #monstre de la caverne de glace# protège", /*spanish*/"un #monstruo de una helada caverna# guarda"},
  });

    hintTable[ICE_CAVERN_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#winds of ice# surround", /*french*/"#entouré de vent glacial# gît", /*spanish*/"#heladas borrascas# rodean"},
  });

    hintTable[ICE_CAVERN_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #wall of ice# protects", /*french*/"#un mur de glace rouge# cache", /*spanish*/"una #gélida pared# protege"},
  });

    hintTable[ICE_CAVERN_MQ_FREESTANDING_POH] = HintText::Exclude({
                       //obscure text
                       Text{"#winds of ice# surround", /*french*/"#entouré de vent glacial# gît", /*spanish*/"#heladas borrascas# rodean"},
  });

    hintTable[ICE_CAVERN_GS_PUSH_BLOCK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider above icy pits# holds", /*french*/"une #Skulltula au dessus d'un goufre glacial# a", /*spanish*/"una #Skulltula sobre gélidos vacíos# otorga"},
  });

    hintTable[ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#spinning ice# guards a spider holding", /*french*/"une #Skulltula près de deux lames de glace# a", /*spanish*/"unos #témpanos giratorios# custodian una Skulltula que otorga"},
  });

    hintTable[ICE_CAVERN_GS_HEART_PIECE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider behind a wall of ice# hides", /*french*/"une #Skulltula derrière un mur de glace# a", /*spanish*/"una #Skulltula tras una gélida pared# otorga"},
  });


    hintTable[ICE_CAVERN_MQ_GS_SCARECROW] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider above icy pits# holds", /*french*/"une #Skulltula au dessus d'un goufre glacial# a", /*spanish*/"una #Skulltula sobre gélidos vacíos# otorga"},
  });

    hintTable[ICE_CAVERN_MQ_GS_ICE_BLOCK] = HintText::Exclude({
                       //obscure text
                       Text{"a #web of ice# surrounds a spider with", /*french*/"une #Skulltula protégée d'une toile glacée# a", /*spanish*/"una #gélida red# rodea a una Skulltula que otorga"},
  });

    hintTable[ICE_CAVERN_MQ_GS_RED_ICE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in fiery ice# hoards", /*french*/"une #Skulltula figée dans la glace rouge# a", /*spanish*/"una #Skulltula tras un ardiente hielo# otorga"},
  });

  /*--------------------------
  | GERUDO TRAINING GROUNDS  |
  ---------------------------*/
    hintTable[GERUDO_TRAINING_GROUNDS_LOBBY_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blinded eye in the Gerudo Training Grounds# drops", /*french*/"l'#Oeil dans le Gymnase Gerudo# voit", /*spanish*/"#cegar un ojo en el Centro de Instrucción Gerudo# revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_LOBBY_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blinded eye in the Gerudo Training Grounds# drops", /*french*/"l'#Oeil dans le Gymnase Gerudo# voit", /*spanish*/"#cegar un ojo en el Centro de Instrucción Gerudo# revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_STALFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#soldiers walking on shifting sands# in the Gerudo Training Grounds guard", /*french*/"les #squelettes# du Gymnase Gerudo protègent", /*spanish*/"#soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_BEAMOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reptilian warriors# in the Gerudo Training Grounds protect", /*french*/"les #lézards# dans le Gymnase Gerudo protègent", /*spanish*/"#unos escamosos guerreros# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HIDDEN_CEILING_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# in the Gerudo Training Grounds reveals", /*french*/"#bien caché# dans le Gymnase Gerudo gît", /*spanish*/"el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the first prize of #the thieves' training# is", /*french*/"le #premier trésor du Gymnase Gerudo# est", /*spanish*/"el primer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the second prize of #the thieves' training# is", /*french*/"le #deuxième trésor du Gymnase Gerudo# est", /*spanish*/"el segundo premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the third prize of #the thieves' training# is", /*french*/"le #troisième trésor du Gymnase Gerudo# est", /*spanish*/"el tercer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_CENTRAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le Gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le Gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_CLEAR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fiery foes# in the Gerudo Training Grounds guard", /*french*/"les #limaces de feu# du Gymnase Gerudo protègent", /*spanish*/"unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#engulfed in flame# where thieves train lies", /*french*/"le #trésor enflammé# du Gymnase Gerudo est", /*spanish*/"donde entrenan las bandidas #entre llamas# yace"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du Gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_NEAR_SCARECROW_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du Gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_BEFORE_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#before a block of silver# thieves can find", /*french*/"#près d'un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"#ante un plateado bloque# las bandidas hallan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FOURTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le Gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });


    hintTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#thieves prepare for training# with", /*french*/"dans #l'entrée du Gymnase Gerudo# gît", /*spanish*/"las #bandidas se instruyen# con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#thieves prepare for training# with", /*french*/"dans #l'entrée du Gymnase Gerudo# gît", /*spanish*/"las #bandidas se instruyen# con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_FIRST_IRON_KNUCKLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#soldiers walking on shifting sands# in the Gerudo Training Grounds guard", /*french*/"les #squelettes# du Gymnase Gerudo protègent", /*spanish*/"#soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_BEFORE_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#before a block of silver# thieves can find", /*french*/"#près d'un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"#ante un plateado bloque# las bandidas hallan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_EYE_STATUE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du Gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_FLAME_CIRCLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#engulfed in flame# where thieves train lies", /*french*/"le #trésor enflammé# du Gymnase Gerudo est", /*spanish*/"donde entrenan las bandidas #entre llamas# yace"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_SECOND_IRON_KNUCKLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fiery foes# in the Gerudo Training Grounds guard", /*french*/"les #ennemis de feu# du Gymnase Gerudo protègent", /*spanish*/"unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_DINOLFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reptilian warriors# in the Gerudo Training Grounds protect", /*french*/"les #lézards# dans le Gymnase Gerudo protègent", /*spanish*/"#unos escamosos guerreros# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_CENTRAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #path of fire# leads thieves to", /*french*/"dans le #chemin enflammé# dans le Gymnase Gerudo gît", /*spanish*/"un #camino de fuego# conduce a las bandidas a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the first prize of #the thieves' training# is", /*french*/"le #premier trésor du Gymnase Gerudo# est", /*spanish*/"el primer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #path of fire# leads thieves to", /*french*/"dans le #chemin enflammé# dans le Gymnase Gerudo gît", /*spanish*/"un #camino de fuego# conduce a las bandidas a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the third prize of #the thieves' training# is", /*french*/"le #troisième trésor du Gymnase Gerudo# est", /*spanish*/"el tercer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the second prize of #the thieves' training# is", /*french*/"le #deuxième trésor du Gymnase Gerudo# est", /*spanish*/"el segundo premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_HIDDEN_CEILING_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# in the Gerudo Training Grounds reveals", /*french*/"#bien caché# dans le Gymnase Gerudo gît", /*spanish*/"el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le Gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

  /*--------------------------
  |      GANONS CASTLE       |
  ---------------------------*/
    hintTable[GANONS_TOWER_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Evil King# hoards", /*french*/"le #Roi du Mal# possède", /*spanish*/"el #Rey del Mal# acapara"},
  });


    hintTable[GANONS_CASTLE_FOREST_TRIAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the wilds# holds", /*french*/"l'#épreuve des bois# contient", /*spanish*/"la #prueba de la naturaleza# brinda"},
  });

    hintTable[GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the seas# holds", /*french*/"l'#épreuve des mers# contient", /*spanish*/"la #prueba del mar# brinda"},
  });

    hintTable[GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the seas# holds", /*french*/"l'#épreuve des mers# contient", /*spanish*/"la #prueba del mar# brinda"},
  });

    hintTable[GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#music in the test of darkness# unveils", /*french*/"la #musique dans l'épreuve des ténèbres# révèle", /*spanish*/"la #música en la prueba de la oscuridad# revela"},
  });

    hintTable[GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#light in the test of darkness# unveils", /*french*/"la #lumière dans l'épreuve des ténèbres# révèle", /*spanish*/"la #luz en la prueba de la oscuridad# revela"},
  });

    hintTable[GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the sands# holds", /*french*/"l'#épreuve des sables# contient", /*spanish*/"la #prueba de las arenas# brinda"},
  });

    hintTable[GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the sands# holds", /*french*/"l'#épreuve des sables# contient", /*spanish*/"la #prueba de las arenas# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of radiance# holds", /*french*/"l'#épreuve du ciel# contient", /*spanish*/"la #prueba del resplandor# brinda"},
  });

    hintTable[GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#music in the test of radiance# reveals", /*french*/"la #musique dans l'épreuve du ciel# révèle", /*spanish*/"la #música en la prueba del resplandor# revela"},
  });


    hintTable[GANONS_CASTLE_MQ_WATER_TRIAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the seas# holds", /*french*/"l'#épreuve des mers# contient", /*spanish*/"la #prueba del mar# brinda"},
  });

    hintTable[GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the wilds# holds", /*french*/"l'#épreuve des bois# contient", /*spanish*/"la #prueba de la naturaleza# brinda"},
  });

    hintTable[GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the wilds# holds", /*french*/"l'#épreuve des bois# contient", /*spanish*/"la #prueba de la naturaleza# brinda"},
  });

    hintTable[GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#music in the test of radiance# reveals", /*french*/"la #musique dans l'épreuve du ciel# révèle", /*spanish*/"la #música en la prueba del resplandor# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of darkness# holds", /*french*/"l'#épreuve des ténèbres# contient", /*spanish*/"la #prueba de la oscuridad# brinda"},
  });

    hintTable[GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of darkness# holds", /*french*/"l'#épreuve des ténèbres# contient", /*spanish*/"la #prueba de la oscuridad# brinda"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reflected light in the test of the sands# reveals", /*french*/"le #soleil dans l'épreuve des sables# révèle", /*spanish*/"#reflejar la luz en la prueba de las arenas# revela"},
  });

    hintTable[GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"the #test of the wilds# holds", /*french*/"l'#épreuve des bois# révèle", /*spanish*/"la #prueba de la naturaleza# brinda"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });


    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le Château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });
}
