#include "../hint_list.hpp"

void HintTable_Init_Item() {
    hintTable[KOKIRI_SWORD] = HintText::Item({
                       //obscure text
                       Text{"a butter knife",    /*french*/"un couteau à beurre",  /*spanish*/"un ágil puñal"},
                       Text{"a starter slasher", /*french*/"une arme de débutant", /*spanish*/"una hoja de principiantes"},
                       Text{"a switchblade",     /*french*/"un canif",             /*spanish*/"una navaja"},
                     }, {
                       //ambiguous text
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"the Kokiri Sword", /*french*/"l'Épée Kokiri", /*spanish*/"la Espada Kokiri"}
    );

    hintTable[MASTER_SWORD] = HintText::Item({
                       //obscure text
                       Text{"evil's bane",        /*french*/"le fléau du mal",       /*spanish*/"la destructora del mal"},
                       Text{"a seven year limbo", /*french*/"une stase de sept ans",  /*spanish*/"unos siete años de espera"},
                     }, {
                       //ambiguous text
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"the Master Sword", /*french*/"l'Épée de Légende", /*spanish*/"la Espada Maestra"}
    );

    hintTable[GIANTS_KNIFE] = HintText::Item({
                       //obscure text
                       Text{"a fragile blade",     /*french*/"une lame fragile",    /*spanish*/"una frágil hoja"},
                       Text{"a breakable cleaver", /*french*/"un espadon de verre", /*spanish*/"un rompible acero"},
                     }, {
                       //ambiguous text
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"the Giant's Knife", /*french*/"la Lame des Géants", /*spanish*/"la daga gigante"}
    );

    hintTable[BIGGORON_SWORD] = HintText::Item({
                       //obscure text
                       Text{"the biggest blade",  /*french*/"une lame gigantesque", /*spanish*/"el mayor mandoble"},
                       Text{"a colossal cleaver", /*french*/"un espadon colossal",  /*spanish*/"un estoque colosal"},
                     }, {
                       //ambiguous text
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"the Biggoron Sword", /*french*/"l'Épée de Biggoron", /*spanish*/"la Espada de Biggoron"}
    );

    hintTable[DEKU_SHIELD] = HintText::Item({
                       //obscure text
                       Text{"a wooden ward",      /*french*/"un écu d'écorce",            /*spanish*/"una protección del bosque"},
                       Text{"a burnable barrier", /*french*/"une protection inflammable", /*spanish*/"una barrera quemable"},
                     }, {
                       //ambiguous text
                       Text{"a shield", /*french*/"un bouclier", /*spanish*/"un escudo"},
                     },
                       //clear text
                       Text{"a Deku Shield", /*french*/"un Bouclier Mojo", /*spanish*/"un escudo deku"}
    );

    hintTable[HYLIAN_SHIELD] = HintText::Item({
                       //obscure text
                       Text{"a steel safeguard",      /*french*/"une carapace d'acier",       /*spanish*/"una protección de acero"},
                       Text{"Like Like's metal meal", /*french*/"un amuse-gueule de Pudding", /*spanish*/"un alimento de Like Like"},
                     }, {
                       //ambiguous text
                       Text{"a shield", /*french*/"un bouclier", /*spanish*/"un escudo"},
                     },
                       //clear text
                       Text{"a Hylian Shield", /*french*/"un Bouclier Hylien", /*spanish*/"un escudo hyliano"}
    );

    hintTable[MIRROR_SHIELD] = HintText::Item({
                       //obscure text
                       Text{"a reflective rampart", /*french*/"un capteur de lumière",   /*spanish*/"una muralla reflectora"},
                       Text{"Medusa's weakness",    /*french*/"la faiblesse de Méduse", /*spanish*/"la debilidad de Medusa"},
                       Text{"a silvered surface",   /*french*/"une surface argentée",    /*spanish*/"una superficie plateada"},
                     }, {
                       //ambiguous text
                       Text{"a shield", /*french*/"un bouclier", /*spanish*/"un escudo"},
                     },
                       //clear text
                       Text{"the Mirror Shield", /*french*/"le Bouclier Miroir", /*spanish*/"el escudo espejo"}
    );

    hintTable[GORON_TUNIC] = HintText::Item({
                       //obscure text
                       Text{"ruby robes",       /*french*/"un pigment rouge",      /*spanish*/"una vestimenta rubí"},
                       Text{"fireproof fabric", /*french*/"un trésor anti-flamme", /*spanish*/"una ignífuga prenda"},
                       Text{"cooking clothes",  /*french*/"une tenue de cuisine",  /*spanish*/"unos abrasantes ropajes"},
                     }, {
                       //ambiguous text
                       Text{"a tunic", /*french*/"une tunique", /*spanish*/"un sayo"},
                     },
                       //clear text
                       Text{"a Goron Tunic", /*french*/"une Tunique Goron", /*spanish*/"un sayo goron"}
    );

    hintTable[ZORA_TUNIC] = HintText::Item({
                       //obscure text
                       Text{"a sapphire suit", /*french*/"un pigment bleuté",      /*spanish*/"una vestidura zafiro"},
                       Text{"scuba gear",      /*french*/"un habit de plongée",    /*spanish*/"un traje impermeable"},
                       Text{"a swimsuit",      /*french*/"un costume de baignade", /*spanish*/"unos ropajes sumergibles"},
                     }, {
                       //ambiguous text
                       Text{"a tunic", /*french*/"une tunique", /*spanish*/"un sayo"},
                       Text{"something expensive", /*french*/"une chose dispendieuse", /*spanish*/"algo caro"},
                     },
                       //clear text
                       Text{"a Zora Tunic", /*french*/"une Tunique Zora", /*spanish*/"un sayo zora"}
    );

    hintTable[IRON_BOOTS] = HintText::Item({
                       //obscure text
                       Text{"sink shoes",   /*french*/"un boulet de fer", /*spanish*/"un calzado de las profundidades"},
                       Text{"clank cleats", /*french*/"une paire de talons bruyants", /*spanish*/"unas suelas férreas"},
                     }, {
                       //ambiguous text
                       Text{"some boots", /*french*/"une paire de bottes", /*spanish*/"un par de botas"},
                       Text{"a feature of the Water Temple", /*french*/"une particularité du Temple de l'Eau", /*spanish*/"algo particular del Templo del Agua"},
                       Text{"something heavy", /*french*/"une chose pesante", /*spanish*/"algo de lo más pesado"},
                     },
                       //clear text
                       Text{"the Iron Boots", /*french*/"une paire de Bottes de plomb", /*spanish*/"las botas de hierro"}
    );

    hintTable[HOVER_BOOTS] = HintText::Item({
                       //obscure text
                       Text{"butter boots",    /*french*/"une paire de patins de beurre",   /*spanish*/"unas suelas resvaladizas"},
                       Text{"sacred slippers", /*french*/"une paire de pantoufles sacrées", /*spanish*/"unos escurridizos botines"},
                       Text{"spacewalkers",    /*french*/"une paire de bottes spatiales",   /*spanish*/"un calzado antigravitatorio"},
                     }, {
                       //ambiguous text
                       Text{"some boots", /*french*/"une paire de bottes", /*spanish*/"un par de botas"},
                     },
                       //clear text
                       Text{"the Hover Boots", /*french*/"une paire de Bottes des airs", /*spanish*/"las botas voladoras"}
    );


    hintTable[ZELDAS_LETTER] = HintText::Item({
                       //obscure text
                       Text{"an autograph",     /*french*/"un autographe",        /*spanish*/"un autógrafo"},
                       Text{"royal stationery", /*french*/"du papier royal",      /*spanish*/"un escrito real"},
                       Text{"royal snail mail", /*french*/"une enveloppe royale", /*spanish*/"correo de la realeza"},
                     }, {},
                       //clear text
                       Text{"Zelda's Letter", /*french*/"la Lettre de Zelda", /*spanish*/"la carta de Zelda"}
    );

    hintTable[WEIRD_EGG] = HintText::Item({
                       //obscure text
                       Text{"a chicken dilemma", /*french*/"un drôle d'ovale", /*spanish*/"el dilema de la gallina"},
                     }, {
                       //ambiguous text
                       Text{"an egg", /*french*/"un oeuf", /*spanish*/"un huevo"},
                     },
                       //clear text
                       Text{"the Weird Egg", /*french*/"l'Oeuf Curieux", /*spanish*/"el huevo extraño"}
    );

    hintTable[BOOMERANG] = HintText::Item({
                       //obscure text
                       Text{"a banana",       /*french*/"une banane",            /*spanish*/"un plátano"},
                       Text{"a stun stick",   /*french*/"un bâton étourdissant", /*spanish*/"un palo aturdidor"},
                       Text{"a yellow angle", /*french*/"un angle jaune",        /*spanish*/"un ángulo amarillo"},
                     }, {
                       //ambiguous text
                       Text{"something that can grab things", /*french*/"une chose qui peut attraper", /*spanish*/"algo que pueda agarrar cosas"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"the Boomerang", /*french*/"le Boomerang", /*spanish*/"el bumerán"}
    );

    hintTable[LENS_OF_TRUTH] = HintText::Item({
                       //obscure text
                       Text{"a lie detector",     /*french*/"un détecteur de mensonges", /*spanish*/"el detector de ilusiones"},
                       Text{"a ghost tracker",    /*french*/"un trouve-fantôme",         /*spanish*/"el rastreador paranormal"},
                       Text{"true sight",         /*french*/"le troisième il",         /*spanish*/"el ojo que todo ve"},
                       Text{"a detective's tool", /*french*/"un trésor Sheikah",         /*spanish*/"la revelación verdadera"},
                     }, {
                       //ambiguous text
                       Text{"a secret-finding tool", /*french*/"un cherche-secrets", /*spanish*/"un instrumento para hallar objetos"},
                     },
                       //clear text
                       Text{"the Lens of Truth", /*french*/"le Monocle de Vérité", /*spanish*/"la Lupa de la Verdad"}
    );

    hintTable[MEGATON_HAMMER] = HintText::Item({
                       //obscure text
                       Text{"the dragon smasher", /*french*/"le tueur de dragons",     /*spanish*/"un destructor de dragones"},
                       Text{"the metal mallet",   /*french*/"un outil de construction", /*spanish*/"un mazo de metal"},
                       Text{"the heavy hitter",   /*french*/"un poids lourd",           /*spanish*/"un machacador"},
                     }, {
                       //ambiguous text
                       Text{"something that can remove boulders", /*french*/"une chose qui enlève les rochers", /*spanish*/"algo que pueda quitar rocas"},
                     },
                       //clear text
                       Text{"the Megaton Hammer", /*french*/"la Masse des Titans", /*spanish*/"el martillo Megatón"}
    );

    hintTable[STONE_OF_AGONY] = HintText::Item({
                       //obscure text
                       Text{"the shake stone", /*french*/"le fragment vibrant", /*spanish*/"el fragmento tintineante"},
                       Text{"a gray alarm",    /*french*/"une alerte bleue",    /*spanish*/"una azul alarma"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"a secret-finding tool", /*french*/"un cherche-secrets", /*spanish*/"un instrumento para hallar objetos"},
                     },
                       //clear text
                       Text{"the Stone of Agony", /*french*/"la Pierre de Souffrance", /*spanish*/"la Piedra de la Agonía"}
    );

    hintTable[DINS_FIRE] = HintText::Item({
                       //obscure text
                       Text{"an inferno",  /*french*/"un brasier",               /*spanish*/"un incendio"},
                       Text{"a heat wave", /*french*/"une vague de chaleur",     /*spanish*/"una onda de calor"},
                       Text{"a red ball",  /*french*/"une explosion de flammes", /*spanish*/"una roja esfera"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                     },
                       //clear text
                       Text{"Din's Fire", /*french*/"le Feu de Din", /*spanish*/"el Fuego de Din"}
    );

    hintTable[FARORES_WIND] = HintText::Item({
                       //obscure text
                       Text{"teleportation",     /*french*/"la téléportation",      /*spanish*/"un teletransportador"},
                       Text{"a relocation rune", /*french*/"une rune de relocation", /*spanish*/"una runa de transporte"},
                       Text{"a green ball",      /*french*/"une boule verte",        /*spanish*/"una verde esfera"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                     },
                       //clear text
                       Text{"Farore's Wind", /*french*/"le Vent de Farore", /*spanish*/"el Viento de Farore"}
    );

    hintTable[NAYRUS_LOVE] = HintText::Item({
                       //obscure text
                       Text{"a safe space",        /*french*/"une bulle de cristal", /*spanish*/"una seguridad temporal"},
                       Text{"an impregnable aura", /*french*/"un aura impénétrable", /*spanish*/"un aura impenetrable"},
                       Text{"a blue barrier",      /*french*/"une toison bleu",      /*spanish*/"una barrera azul"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                     },
                       //clear text
                       Text{"Nayru's Love", /*french*/"l'Amour de Nayru", /*spanish*/"el Amor de Nayru"}
    );

    hintTable[FIRE_ARROWS] = HintText::Item({
                       //obscure text
                       Text{"the furnace firearm" , /*french*/"une fusée solaire",     /*spanish*/"el ardiente aguijón"},
                       Text{"the burning bolts",    /*french*/"un obus enflammé",      /*spanish*/"las puntas ígneas"},
                       Text{"a magma missile",      /*french*/"un missile volcanique", /*spanish*/"el misil abrasador"},
                     }, {
                       //ambiguous text
                       Text{"a magic arrow", /*french*/"une flèche magique", /*spanish*/"una flecha mágica"},
                     },
                       //clear text
                       Text{"the Fire Arrows", /*french*/"les Flèches de Feu", /*spanish*/"la flecha de fuego"}
    );

    hintTable[ICE_ARROWS] = HintText::Item({
                       //obscure text
                       Text{"the refrigerator rocket", /*french*/"un missile pétrifiant", /*spanish*/"el misil congelador"},
                       Text{"the frostbite bolts",     /*french*/"un froid mordant",      /*spanish*/"las puntas gélidas"},
                       Text{"an iceberg maker",        /*french*/"une aiguille glaciale", /*spanish*/"el control de escarcha"},
                     }, {
                       //ambiguous text
                       Text{"a magic arrow", /*french*/"une flèche magique", /*spanish*/"una flecha mágica"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"the Ice Arrows", /*french*/"les Flèches de Glace", /*spanish*/"la flecha de hielo"}
    );

    hintTable[LIGHT_ARROWS] = HintText::Item({
                       //obscure text
                       Text{"the shining shot",      /*french*/"l'arme brillante",      /*spanish*/"el haz de luz"},
                       Text{"the luminous launcher", /*french*/"un jet de lumière",      /*spanish*/"el disparo luminoso"},
                       Text{"Ganondorf's bane",      /*french*/"le fléau de Ganondorf", /*spanish*/"la perdición de Ganondorf"},
                       Text{"the lighting bolts",    /*french*/"l'éclair sacré",        /*spanish*/"las puntas resplandecientes"},
                     }, {
                       //ambiguous text
                       Text{"a magic arrow", /*french*/"une flèche magique", /*spanish*/"una flecha mágica"},
                     },
                       //clear text
                       Text{"the Light Arrows", /*french*/"les Flèches de Lumière", /*spanish*/"la flecha de luz"}
    );

    hintTable[GERUDO_MEMBERSHIP_CARD] = HintText::Item({
                       //obscure text
                       Text{"a girl club membership", /*french*/"une carte de membre", /*spanish*/"una fémina membresía"},
                       Text{"a desert tribe's pass",  /*french*/"un laissez-passer",   /*spanish*/"el vale del desierto"},
                     }, {
                       Text{"a token of recognition", /*french*/"une preuve de reconnaissance", /*spanish*/"una prueba de reconocimiento"},
                     },
                       //clear text
                       Text{"the Gerudo Membership Card", /*french*/"la Carte Gerudo", /*spanish*/"el pase de socio gerudo"}
    );

    hintTable[MAGIC_BEAN] = HintText::Item({
                       //obscure text
                       Text{"a wizardly legume", /*french*/"un légume ensorcelé", /*spanish*/"una legumbre hechizada"},
                     }, {
                       //ambiguous text
                       Text{"something sometimes buried", /*french*/"une chose parfois enterrée", /*spanish*/"algo a veces enterrado"},
                     },
                       //clear text
                       Text{"a Magic Bean", /*french*/"un Haricot Magique", /*spanish*/"una judía mágica"}
    );

    hintTable[MAGIC_BEAN_PACK] = HintText::Item({
                       //obscure text
                       Text{"wizardly legumes", /*french*/"un paquet de légumes ensorcelés", /*spanish*/"unas legumbres hechizadas"},
                     }, {
                       //ambiguous text
                       Text{"something sometimes buried", /*french*/"une chose parfois enterrée", /*spanish*/"algo a veces enterrado"},
                     },
                       //clear text
                       Text{"Magic Beans", /*french*/"un Paquet de Haricots Magiques", /*spanish*/"unas judías mágicas"}
    );

    hintTable[DOUBLE_DEFENSE] = HintText::Item({
                       //obscure text
                       Text{"a white outline",   /*french*/"un rebord blanc",               /*spanish*/"un contorno blanco"},
                       Text{"damage decrease",   /*french*/"une protection supplémentaire", /*spanish*/"una reducción de daño"},
                       Text{"strengthened love", /*french*/"un amour coriace",              /*spanish*/"un amor fortalecido"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                       Text{"something heart-shaped", /*french*/"une chose en forme de coeur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"Double Defense", /*french*/"la Double Défence", /*spanish*/"la doble defensa"}
    );

    hintTable[GOLD_SKULLTULA_TOKEN] = HintText::Item({
                       //obscure text
                       Text{"proof of destruction",   /*french*/"un certificat d'élimination", /*spanish*/"una prueba de la destrucción"},
                       Text{"an arachnid chip",       /*french*/"un symbole cranien",            /*spanish*/"una figura arácnida"},
                       Text{"spider remains",         /*french*/"une dépouille dorée",         /*spanish*/"unos restos dorados"},
                       Text{"one percent of a curse", /*french*/"un centième de malédiction",  /*spanish*/"una centésima de una maldición"},
                     }, {
                       //ambiguous text
                       Text{"a token of recognition", /*french*/"une preuve de reconnaissance", /*spanish*/"una prueba de reconocimiento"},
                       Text{"something sometimes buried", /*french*/"une chose parfois enterrée", /*spanish*/"algo a veces enterrado"},
                     },
                       //clear text
                       Text{"a Gold Skulltula Token", /*french*/"un Symbole de Skulltula d'or", /*spanish*/"un símbolo de skulltula dorada"}
    );

    hintTable[POCKET_EGG] = HintText::Item({
                       //obscure text
                       Text{"a Cucco container",   /*french*/"un réservoir à Cocotte", /*spanish*/"cuco contenido"},
                       Text{"a Cucco, eventually", /*french*/"un poussin éventuel",    /*spanish*/"un futuro cuco"},
                       Text{"a fowl youth",        /*french*/"une omelette crue",      /*spanish*/"una dulce juventud"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                       Text{"an egg", /*french*/"un oeuf", /*spanish*/"un huevo"},
                     },
                       //clear text
                       Text{"the Pocket Egg", /*french*/"l'Oeuf de Poche", /*spanish*/"el huevo de bolsillo"}
    );

    hintTable[POCKET_CUCCO] = HintText::Item({
                       //obscure text
                       Text{"a little clucker", /*french*/"un petit glousseur", /*spanish*/"un pollito chiquito"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Pocket Cucco", /*french*/"la Cocotte de Poche", /*spanish*/"el cuco de bolsillo"}
    );

    hintTable[COJIRO] = HintText::Item({
                       //obscure text
                       Text{"a cerulean capon", /*french*/"un paon azur", /*spanish*/"un cerúleo capón"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"Cojiro", /*french*/"le p'tit poulet", /*spanish*/"a Cojiro"}
    );

    hintTable[ODD_MUSHROOM] = HintText::Item({
                       //obscure text
                       Text{"a powder ingredient", /*french*/"un ingrédient à poudre", /*spanish*/"un oloroso ingrediente"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"an Odd Mushroom", /*french*/"un Champignon Suspect", /*spanish*/"un champiñón extraño"}
    );

    hintTable[ODD_POTION] = HintText::Item({
                       //obscure text
                       Text{"Granny's goodies", /*french*/"la confiserie de mamie", /*spanish*/"la especialidad de la abuela"},
                     }, {
                       //ambiguous text
                       Text{"something that contains medicine", /*french*/"une chose médicamenteuse", /*spanish*/"algo que contenga medicina"},
                       Text{"something with a strange smell", /*french*/"une chose qui sent bizarre", /*spanish*/"algo con un olor extraño"},
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"an Odd Potion", /*french*/"une Mixture Suspecte", /*spanish*/"una medicina rara"}
    );

    hintTable[POACHERS_SAW] = HintText::Item({
                       //obscure text
                       Text{"a tree killer", /*french*/"un coupeur d'arbres", /*spanish*/"un destructor de árboles"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Poacher's Saw", /*french*/"la Scie du Chasseur", /*spanish*/"la sierra del furtivo"}
    );

    hintTable[BROKEN_SWORD] = HintText::Item({
                       //obscure text
                       Text{"a shattered slicer", /*french*/"une arme cassée", /*spanish*/"una rebanadora rota"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"the Broken Goron's Sword", /*french*/"l'Épée Brisée de Goron", /*spanish*/"la espada goron rota"}
    );

    hintTable[PRESCRIPTION] = HintText::Item({
                       //obscure text
                       Text{"a pill pamphlet", /*french*/"un document urgent", /*spanish*/"un instructivo medicinal"},
                       Text{"a doctor's note", /*french*/"un papier médical",  /*spanish*/"unas notas del doctor"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Prescription", /*french*/"une Ordonnance", /*spanish*/"la receta"}
    );

    hintTable[EYEBALL_FROG] = HintText::Item({
                       //obscure text
                       Text{"a perceiving polliwog", /*french*/"un amphibien", /*spanish*/"un variopinto batracio"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Eyeball Frog", /*french*/"le Crapaud-qui-louche", /*spanish*/"la rana de ojos saltones"}
    );

    hintTable[EYEDROPS] = HintText::Item({
                       //obscure text
                       Text{"a vision vial", /*french*/"une solution oculaire", /*spanish*/"un remedio para la vista"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Eyedrops", /*french*/"une phiole de Super-Gouttes", /*spanish*/"las supergotas oculares"}
    );

    hintTable[CLAIM_CHECK] = HintText::Item({
                       //obscure text
                       Text{"a three day wait", /*french*/"un rendez-vous dans trois jours", /*spanish*/"unos tres días de espera"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Claim Check", /*french*/"un Certificat", /*spanish*/"el recibo"}
    );

    hintTable[PROGRESSIVE_HOOKSHOT] = HintText::Item({
                       //obscure text
                       Text{"Dampé's keepsake", /*french*/"l'héritage d'Igor",   /*spanish*/"un recuerdo de Dampé"},
                       Text{"the Grapple Beam", /*french*/"le rayon grippeur",   /*spanish*/"una garra metálica"},
                       Text{"the BOING! chain", /*french*/"la chaîne de BOING!", /*spanish*/"una cadena retráctil"},
                     }, {
                       //ambiguous text
                       Text{"something that can grab things", /*french*/"une chose qui peut attraper", /*spanish*/"algo que pueda agarrar cosas"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"a Hookshot", /*french*/"un Grappin", /*spanish*/"un gancho"}
    );

    hintTable[PROGRESSIVE_STRENGTH] = HintText::Item({
                       //obscure text
                       Text{"power gloves",    /*french*/"une paire de gants de travail",       /*spanish*/"unos poderosos guanteletes"},
                       Text{"metal mittens",   /*french*/"une paire de mitaines",               /*spanish*/"unas manoplas metálicas"},
                       Text{"the heavy lifty", /*french*/"la puissance de dix hommes", /*spanish*/"un levantamiento pesado"},
                     }, {
                       //ambiguous text
                       Text{"something that can remove boulders", /*french*/"une chose qui enlève les rochers", /*spanish*/"algo que pueda quitar rocas"},
                     },
                       //clear text
                       Text{"a Strength Upgrade", /*french*/"une Amélioration de Force", /*spanish*/"un aumento de fuerza"}
    );

    hintTable[PROGRESSIVE_BOMB_BAG] = HintText::Item({
                       //obscure text
                       Text{"an explosive container", /*french*/"un porte-grenade",      /*spanish*/"un recipiente explosivo"},
                       Text{"a blast bag",            /*french*/"un estomac de Dodongo", /*spanish*/"un zurrón de estallidos"},
                     }, {
                       //ambiguous text
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                       Text{"something that can remove boulders", /*french*/"une chose qui enlève les rochers", /*spanish*/"algo que pueda quitar rocas"},
                     },
                       //clear text
                       Text{"a Bomb Bag", /*french*/"un Sac de Bombes", /*spanish*/"un saco de bombas"}
    );

    hintTable[PROGRESSIVE_BOW] = HintText::Item({
                       //obscure text
                       Text{"an archery enabler",     /*french*/"un facilitateur de tir", /*spanish*/"un tiro al blanco"},
                       Text{"a danger dart launcher", /*french*/"un tire-fléchette",      /*spanish*/"un peligroso lanzadardos"},
                     }, {
                       //ambiguous text
                       Text{"a projectile shooter", /*french*/"un tire-projectile", /*spanish*/"un arma de proyectil"},
                     },
                       //clear text
                       Text{"a Bow", /*french*/"l'Arc des Fées", /*spanish*/"un arco de las hadas"}
    );

    hintTable[PROGRESSIVE_SLINGSHOT] = HintText::Item({
                       //obscure text
                       Text{"a seed shooter",     /*french*/"un lance-noix",     /*spanish*/"un lanzasemillas"},
                       Text{"a rubberband",       /*french*/"un élastique",      /*spanish*/"un tirachinas"},
                       Text{"a child's catapult", /*french*/"un jouet d'enfant", /*spanish*/"una catapulta infantil"},
                     }, {
                       //ambiguous text
                       Text{"a projectile shooter", /*french*/"un tire-projectile", /*spanish*/"un arma de proyectil"},
                     },
                       //clear text
                       Text{"a Slingshot", /*french*/"un Lance-Pierre", /*spanish*/"una resortera de las hadas"}
    );

    hintTable[PROGRESSIVE_WALLET] = HintText::Item({
                       //obscure text
                       Text{"a mo' money holder", /*french*/"un sac à sous",     /*spanish*/"una cartera de dinero"},
                       Text{"a gem purse",        /*french*/"une sacoche",       /*spanish*/"un zurrón de gemas"},
                       Text{"a portable bank",    /*french*/"une petite banque", /*spanish*/"un banco portable"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                     },
                       //clear text
                       Text{"a Wallet", /*french*/"une Bourse", /*spanish*/"una bolsa de rupias"}
    );

    hintTable[PROGRESSIVE_SCALE] = HintText::Item({
                       //obscure text
                       Text{"a deeper dive",   /*french*/"une bulle de plongée", /*spanish*/"un profundo buceo"},
                       Text{"a piece of Zora", /*french*/"un morceau de Zora",   /*spanish*/"un fragmento de Zora"},
                     }, {
                       //ambiguous text
                       Text{"a diving tool", /*french*/"un outil de plongée", /*spanish*/"un instrumento de buceo"},
                     },
                       //clear text
                       Text{"a Zora Scale", /*french*/"une Écaille Zora", /*spanish*/"una escama Zora"}
    );

    hintTable[PROGRESSIVE_NUT_UPGRADE] = HintText::Item({
                       //obscure text
                       Text{"more nuts",         /*french*/"ecnore plus de noix", /*spanish*/"más semillas de nogal"},
                       Text{"flashbang storage", /*french*/"un sac à noix",       /*spanish*/"más frutos aturdidores"},
                     }, {
                       //ambiguous text
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"Deku Nut Capacity", /*french*/"une Augmentation de Noix Mojo", /*spanish*/"un aumento de nueces deku"}
    );

    hintTable[PROGRESSIVE_STICK_UPGRADE] = HintText::Item({
                       //obscure text
                       Text{"a lumber rack",        /*french*/"un paquet de bois",    /*spanish*/"más bastones"},
                       Text{"more flammable twigs", /*french*/"beaucoup de branches", /*spanish*/"más varas"},
                     }, {
                       //ambiguous text
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                     },
                       //clear text
                       Text{"Deku Stick Capacity", /*french*/"une augmentation de bâtons Mojo", /*spanish*/"un aumento de palos deku"}
    );

    hintTable[PROGRESSIVE_MAGIC_METER] = HintText::Item({
                       //obscure text
                       Text{"mystic training",    /*french*/"un potentiel magique", /*spanish*/"una maestría mística"},
                       Text{"pixie dust",         /*french*/"de la poudre de fée", /*spanish*/"un polvo de hada"},
                       Text{"a green rectangle",  /*french*/"un rectangle vert",    /*spanish*/"una verduzca barra"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                     },
                       //clear text
                       Text{"a Magic Meter", /*french*/"une Jauge de Magie", /*spanish*/"un aumento de poder mágico"}
    );

    hintTable[PROGRESSIVE_OCARINA] = HintText::Item({
                       //obscure text
                       Text{"a flute",       /*french*/"un bec musical",         /*spanish*/"un utensilio musical"},
                       Text{"a music maker", /*french*/"un porteur de chansons", /*spanish*/"un instrumento"},
                     }, {
                       //ambiguous text
                       Text{"something given by Saria", /*french*/"un cadeau de Saria", /*spanish*/"un obsequio de Saria"},
                       Text{"something kept by the royal family", /*french*/"une chose qui paralyse", /*spanish*/"algo guardado por la familia real"},
                     },
                       //clear text
                       Text{"an Ocarina", /*french*/"un ocarina", /*spanish*/"una ocarina"}
    );

    hintTable[PROGRESSIVE_BOMBCHUS] = HintText::Item({
                       //obscure text
                       Text{"mice bombs",     /*french*/"un adorable explosif",  /*spanish*/"unas bombas roedoras"},
                       Text{"proximity mice", /*french*/"une mine anti-rongeur",    /*spanish*/"unos explosivos ratoncitos"},
                       Text{"wall crawlers",  /*french*/"un rapide grimpeur",  /*spanish*/"unos trepaparedes"},
                       Text{"trail blazers",  /*french*/"un zigzag éclatant", /*spanish*/"unas ratas propulsadas"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombchus", /*french*/"un paquet de Missiles", /*spanish*/"unos bombchus"}
    );

    hintTable[PROGRESSIVE_GORONSWORD] = HintText::Item({
                       //obscure text
                       Text{"a long blade",   /*french*/"une longue lame", /*spanish*/"una gran hoja"},
                       Text{"a Goron weapon", /*french*/"une arme Goron",  /*spanish*/"un arma goron"},
                     }, {
                       //ambiguous text
                       Text{"a sword", /*french*/"une épée", /*spanish*/"una espada"},
                     },
                       //clear text
                       Text{"a Goron Sword", /*french*/"une épée Goron", /*spanish*/"una espada goron"}
    );

    hintTable[EMPTY_BOTTLE] = HintText::Item({
                       //obscure text
                       Text{"a glass container", /*french*/"un cylindre de cristal", /*spanish*/"un recipiente de cristal"},
                       Text{"an empty jar",      /*french*/"une jarre incassable",   /*spanish*/"un frasco vacío"},
                       Text{"encased air",       /*french*/"un bocal d'air",         /*spanish*/"aire a presión"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Bottle", /*french*/"un flacon vide", /*spanish*/"una botella"}
    );

    hintTable[BOTTLE_WITH_MILK] = HintText::Item({
                       //obscure text
                       Text{"cow juice",          /*french*/"une source de calcium", /*spanish*/"una fuente de calcio"},
                       Text{"a white liquid",     /*french*/"un liquide blanc",      /*spanish*/"una bebida nutritiva"},
                       Text{"a baby's breakfast", /*french*/"du jus pour bébé",     /*spanish*/"un trago para bebés"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Milk Bottle", /*french*/"un flacon de lait", /*spanish*/"una botella de leche"}
    );

    hintTable[BOTTLE_WITH_RED_POTION] = HintText::Item({
                       //obscure text
                       Text{"a vitality vial", /*french*/"un mélange de vitalité", /*spanish*/"una pócima vitalicia"},
                       Text{"a red liquid",    /*french*/"un liquide rouge",       /*spanish*/"un remedio rojizo"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Red Potion Bottle", /*french*/"un flacon de potion rouge", /*spanish*/"una botella de poción roja"}
    );

    hintTable[BOTTLE_WITH_GREEN_POTION] = HintText::Item({
                       //obscure text
                       Text{"a magic mixture", /*french*/"une réserve magique", /*spanish*/"un potingue mágico"},
                       Text{"a green liquid",  /*french*/"un liquide vert",     /*spanish*/"un remedio verduzco"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Green Potion Bottle", /*french*/"un flacon de potion verte", /*spanish*/"una botella de poción verde"}
    );

    hintTable[BOTTLE_WITH_BLUE_POTION] = HintText::Item({
                       //obscure text
                       Text{"an ailment antidote", /*french*/"l'élixir ultime", /*spanish*/"un antídoto para el dolor"},
                       Text{"a blue liquid",       /*french*/"un liquide bleu", /*spanish*/"un remedio índigo"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Blue Potion Bottle", /*french*/"un flacon de potion bleue", /*spanish*/"una botella de poción azul"}
    );

    hintTable[BOTTLE_WITH_FAIRY] = HintText::Item({
                       //obscure text
                       Text{"an imprisoned fairy", /*french*/"une fée emprisonnée", /*spanish*/"un hada atrapada"},
                       Text{"an extra life",       /*french*/"une vie de rechange", /*spanish*/"una oportunidad más"},
                       Text{"Navi's cousin",       /*french*/"le cousin de Navi",  /*spanish*/"una prima de Navi"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Fairy Bottle", /*french*/"une fée en flacon", /*spanish*/"un hada en una botella"}
    );

    hintTable[BOTTLE_WITH_FISH] = HintText::Item({
                       //obscure text
                       Text{"an aquarium",     /*french*/"un aquarium",               /*spanish*/"un escamado ser"},
                       Text{"a deity's snack", /*french*/"le repas d'un dieu marin", /*spanish*/"un tentempié de cierta deidad"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Fish Bottle", /*french*/"un poisson en flacon", /*spanish*/"un pez en una botella"}
    );

    hintTable[BOTTLE_WITH_BLUE_FIRE] = HintText::Item({
                       //obscure text
                       Text{"a conflagration canteen", /*french*/"une mystérieuse flamme", /*spanish*/"un incendio retenido"},
                       Text{"an icemelt jar",          /*french*/"un brasier glacial",     /*spanish*/"unas brasas enfrascadas"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Blue Fire Bottle", /*french*/"une flamme bleue en flacon", /*spanish*/"una botella de fuego azul"}
    );

    hintTable[BOTTLE_WITH_BUGS] = HintText::Item({
                       //obscure text
                       Text{"an insectarium",    /*french*/"un insectarium",       /*spanish*/"unos invertebrados seres"},
                       Text{"Skulltula finders", /*french*/"une poignée de trouve-Skulltula", /*spanish*/"unos rastreadores de skulltulas"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Bug Bottle", /*french*/"un insecte en flacon", /*spanish*/"unos insectos en una botella"}
    );

    hintTable[BOTTLE_WITH_POE] = HintText::Item({
                       //obscure text
                       Text{"a spooky ghost",    /*french*/"un effroyable fantôme",   /*spanish*/"un espantoso espectro"},
                       Text{"a face in the jar", /*french*/"un visage dans un bocal", /*spanish*/"una expresión enfrascada"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Poe Bottle", /*french*/"un Esprit en flacon", /*spanish*/"un Poe en una botella"}
    );

    hintTable[BOTTLE_WITH_BIG_POE] = HintText::Item({
                       //obscure text
                       Text{"the spookiest ghost", /*french*/"un épouvantable spectre", /*spanish*/"el espectro más espeluznante"},
                       Text{"a sidequest spirit",  /*french*/"un précieux esprit",      /*spanish*/"un buen valorado espíritu"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"a Big Poe Bottle", /*french*/"une Ame en flacon", /*spanish*/"un Gran Poe en una botella"}
    );

    hintTable[RUTOS_LETTER] = HintText::Item({
                       //obscure text
                       Text{"a call for help",      /*french*/"un appel au secours", /*spanish*/"una llamada de auxilio"},
                       Text{"the note that Mweeps", /*french*/"un message qui fait mwip",          /*spanish*/"un escrito mweep"},
                       Text{"an SOS call",          /*french*/"un signal SOS",       /*spanish*/"una nota de socorro"},
                       Text{"a fishy stationery",   /*french*/"un papier mouillé",  /*spanish*/"un mensaje de ayuda"},
                     }, {
                       //ambiguous text
                       Text{"a bottle", /*french*/"un flacon", /*spanish*/"una botella"},
                     },
                       //clear text
                       Text{"Ruto's Letter", /*french*/"la lettre de Ruto", /*spanish*/"la carta de Ruto"}
    );

    hintTable[ZELDAS_LULLABY] = HintText::Item({
                       //obscure text
                       Text{"a song of royal slumber", /*french*/"une chanson royale", /*spanish*/"la canción real"},
                       Text{"a triforce tune",         /*french*/"la musique sacrée", /*spanish*/"la melodía de la trifuerza"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                       Text{"something kept by the royal family", /*french*/"une chose qui paralyse", /*spanish*/"algo guardado por la familia real"},
                     },
                       //clear text
                       Text{"Zelda's Lullaby", /*french*/"la berceuse de Zelda", /*spanish*/"la Nana de Zelda"}
    );

    hintTable[EPONAS_SONG] = HintText::Item({
                       //obscure text
                       Text{"an equestrian etude", /*french*/"une hymne équestre",     /*spanish*/"una copla ecuestre"},
                       Text{"Malon's melody",      /*french*/"la mélodie des vaches", /*spanish*/"la sonata de Malon"},
                       Text{"a ranch song",        /*french*/"le chant des champs",   /*spanish*/"un canto rupestre"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                     },
                       //clear text
                       Text{"Epona's Song", /*french*/"le chant d'Epona", /*spanish*/"la Canción de Epona"}
    );

    hintTable[SARIAS_SONG] = HintText::Item({
                       //obscure text
                       Text{"a song of dancing Gorons", /*french*/"une chanson danceuse",     /*spanish*/"un pegadizo tono goron"},
                       Text{"Saria's phone number",     /*french*/"le téléphone d'une amie", /*spanish*/"una consulta de asistencia"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                       Text{"something given by Saria", /*french*/"un cadeau de Saria", /*spanish*/"un obsequio de Saria"},
                     },
                       //clear text
                       Text{"Saria's Song", /*french*/"le chant de Saria", /*spanish*/"la Canción de Saria"}
    );

    hintTable[SUNS_SONG] = HintText::Item({
                       //obscure text
                       Text{"Sunny Day",         /*french*/"Zénith",               /*spanish*/"un día soleado"},
                       Text{"the ReDead's bane", /*french*/"le fléau des Éffrois", /*spanish*/"la destructora de Redeads"},
                       Text{"the Gibdo's bane",  /*french*/"le fléau des Gibdo",   /*spanish*/"la destructora de Gibdos"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"the Sun's Song", /*french*/"le chant du soleil", /*spanish*/"la Canción del Sol"}
    );

    hintTable[SONG_OF_TIME] = HintText::Item({
                       //obscure text
                       Text{"a song 7 years long", /*french*/"le flot du temps", /*spanish*/"la setenada canción"},
                       Text{"the tune of ages",    /*french*/"le Chant des Âges",  /*spanish*/"la melodía eónica"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                     },
                       //clear text
                       Text{"the Song of Time", /*french*/"le chant du temps", /*spanish*/"la Canción del tiempo"}
    );

    hintTable[SONG_OF_STORMS] = HintText::Item({
                       //obscure text
                       Text{"Rain Dance",            /*french*/"Danse Pluie",               /*spanish*/"la danza de la lluvia"},
                       Text{"a thunderstorm tune",   /*french*/"une hymne foudroyante",      /*spanish*/"una sonata tormentosa"},
                       Text{"windmill acceleration", /*french*/"l'accélérateur de moulins", /*spanish*/"el arranque de molinos"},
                     }, {
                       //ambiguous text
                       Text{"a regular song", /*french*/"une chanson normale", /*spanish*/"una cancion normal"},
                     },
                       //clear text
                       Text{"the Song of Storms", /*french*/"le chant des tempêtes", /*spanish*/"la Canción de la Tormenta"}
    );

    hintTable[MINUET_OF_FOREST] = HintText::Item({
                       //obscure text
                       Text{"the song of tall trees", /*french*/"le bruit des arbres", /*spanish*/"la canción de las copas"},
                       Text{"an arboreal anthem",     /*french*/"l'hymne sylvestre",   /*spanish*/"el himno forestal"},
                       Text{"a green spark trail",    /*french*/"une comète verte",     /*spanish*/"el sendero esmeralda"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Minuet of Forest", /*french*/"le menuet de la forêt", /*spanish*/"el Minueto del bosque"}
    );

    hintTable[BOLERO_OF_FIRE] = HintText::Item({
                       //obscure text
                       Text{"a song of lethal lava", /*french*/"une musique enflammée",          /*spanish*/"la canción de la lava"},
                       Text{"a red spark trail",     /*french*/"une comète rouge",      /*spanish*/"el sendero rubí"},
                       Text{"a volcanic verse",      /*french*/"le souffle du volcan", /*spanish*/"el verso volcánico"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Bolero of Fire", /*french*/"le boléro du feu", /*spanish*/"el Bolero del fuego"}
    );

    hintTable[SERENADE_OF_WATER] = HintText::Item({
                       //obscure text
                       Text{"a song of a damp ditch", /*french*/"le calme de l'eau", /*spanish*/"la canción del estanque"},
                       Text{"a blue spark trail",     /*french*/"une comète bleue",   /*spanish*/"el sendero zafiro"},
                       Text{"the lake's lyric",       /*french*/"la voix du lac",    /*spanish*/"la letra del lago"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Serenade of Water", /*french*/"la sérénade de l'eau", /*spanish*/"la Serenata del agua"}
    );

    hintTable[REQUIEM_OF_SPIRIT] = HintText::Item({
                       //obscure text
                       Text{"a song of sandy statues", /*french*/"la mélodie d'une grande statue", /*spanish*/"la canción de la gran estatua"},
                       Text{"an orange spark trail",   /*french*/"une comète orange",               /*spanish*/"el sendero ámbar"},
                       Text{"the desert ditty",        /*french*/"le vent du désert",              /*spanish*/"la estrofa del desierto"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Requiem of Spirit", /*french*/"le requiem des esprits", /*spanish*/"el Réquiem del espíritu"}
    );

    hintTable[NOCTURNE_OF_SHADOW] = HintText::Item({
                       //obscure text
                       Text{"a song of spooky spirits", /*french*/"une hymne de chair de poule", /*spanish*/"la canción de los espectros"},
                       Text{"a graveyard boogie",       /*french*/"un boogie de fantômes",       /*spanish*/"una honra fúnebre"},
                       Text{"a haunted hymn",           /*french*/"une chanson lugubre",         /*spanish*/"una estrofa encantada"},
                       Text{"a purple spark trail",     /*french*/"une comète mauve",            /*spanish*/"el sendero malva"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Nocturne of Shadow", /*french*/"le nocturne de l'ombre", /*spanish*/"el Nocturno de la sombra"}
    );

    hintTable[PRELUDE_OF_LIGHT] = HintText::Item({
                       //obscure text
                       Text{"a luminous prologue melody", /*french*/"une matine illuminée", /*spanish*/"la melodía refulgente"},
                       Text{"a yellow spark trail",       /*french*/"une comète jaune",        /*spanish*/"el sendero resplandeciente"},
                       Text{"the temple traveler",        /*french*/"un chant de sanctuaire",  /*spanish*/"la ruta del templo"},
                     }, {
                       //ambiguous text
                       Text{"a warp song", /*french*/"une chanson de téléportation", /*spanish*/"una canción de teletransportación"},
                     },
                       //clear text
                       Text{"the Prelude of Light", /*french*/"le prélude de la lumière", /*spanish*/"el Preludio de la luz"}
    );
    hintTable[DEKU_TREE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a mossy atlas", /*french*/"un atlas boisé",  /*spanish*/"un atlas musgoso"},
                        Text{"some mossy blueprints",      /*french*/"un plan boisé",             /*spanish*/"unos planos musgosos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Deku Tree Map", /*french*/"la carte de l'Arbre Mojo", /*spanish*/"el mapa del Gran Árbol Deku"}
     );
    hintTable[DODONGOS_CAVERN_MAP] = HintText::Item({
                        //obscure text
                        Text{"a rocky atlas", /*french*/"un atlas rocheux",  /*spanish*/"un atlas rocoso"},
                        Text{"some rocky blueprints",      /*french*/"un plan rocheux",             /*spanish*/"unos planos rocosos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Dodongo's Cavern Map", /*french*/"la carte de la Caverne Dodongo", /*spanish*/"el mapa de la Cueva de los Dodongos"}
     );
    hintTable[JABU_JABUS_BELLY_MAP] = HintText::Item({
                        //obscure text
                        Text{"a fishy atlas", /*french*/"un atlas digéré",  /*spanish*/"un atlas digesto"},
                        Text{"some fishy blueprints",      /*french*/"un plan digéré",             /*spanish*/"unos planos digestos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Jabu-Jabu's Belly Map", /*french*/"la carte de Jabu-Jabu", /*spanish*/"el mapa de la Tripa de Jabu-Jabu"}
     );
    hintTable[FOREST_TEMPLE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a sylvan atlas", /*french*/"un atlas sylvestre",  /*spanish*/"un atlas enselvado"},
                        Text{"some sylvan blueprints",      /*french*/"un plan sylvestre",             /*spanish*/"unos planos enselvados"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Forest Temple Map", /*french*/"la carte du Temple de la Forêt", /*spanish*/"el mapa del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a molten atlas", /*french*/"un atlas fondu",  /*spanish*/"un atlas fundido"},
                        Text{"some molten blueprints",      /*french*/"un plan fondu",             /*spanish*/"unos planos fundidos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Fire Temple Map", /*french*/"la carte du Temple du Feu", /*spanish*/"el mapa del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a wet atlas", /*french*/"un atlas humide",  /*spanish*/"un atlas mojado"},
                        Text{"some wet blueprints",      /*french*/"un plan humide",             /*spanish*/"unos planos mojados"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Water Temple Map", /*french*/"la carte du Temple de l'Eau", /*spanish*/"el mapa del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a sandy atlas", /*french*/"un atlas sableux",  /*spanish*/"un atlas arenoso"},
                        Text{"some sandy blueprints",      /*french*/"un plan sableux",             /*spanish*/"unos planos arenosos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Spirit Temple Map", /*french*/"la carte du Temple de l'Esprit", /*spanish*/"el mapa del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_MAP] = HintText::Item({
                        //obscure text
                        Text{"a creepy atlas", /*french*/"un atlas sinistre",  /*spanish*/"un atlas siniestra"},
                        Text{"some creepy blueprints",      /*french*/"un plan sinistre",             /*spanish*/"unos planos siniestras"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Shadow Temple Map", /*french*/"la carte du Temple de l'Ombre", /*spanish*/"el mapa del Templo de las Sombras"}
     );
    hintTable[BOTTOM_OF_THE_WELL_MAP] = HintText::Item({
                        //obscure text
                        Text{"a moldy atlas", /*french*/"un atlas moisi",  /*spanish*/"un atlas mohoso"},
                        Text{"some moldy blueprints",      /*french*/"un plan moisi",             /*spanish*/"unos planos mohosos"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Bottom of the Well Map", /*french*/"la carte du fond du Puits", /*spanish*/"el mapa del Fondo del pozo"}
     );
    hintTable[ICE_CAVERN_MAP] = HintText::Item({
                        //obscure text
                        Text{"a polar atlas", /*french*/"un atlas polaire",  /*spanish*/"un atlas polar"},
                        Text{"some polar blueprints",      /*french*/"un plan polaire",             /*spanish*/"unos planos polars"},
                      }, {
                        //ambiguous text
                        Text{"a dungeon map", /*french*/"une carte", /*spanish*/"un mapa"},
                      },
                        //clear text
                        Text{"the Ice Cavern Map", /*french*/"la carte de la Caverne Polaire", /*spanish*/"el mapa de la Caverna de hielo"}
     );
    hintTable[DEKU_TREE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a mossy treasure tracker", /*french*/"un cherche-trésor boisé",  /*spanish*/"un zahorí musgoso"},
                        Text{"a mossy magnetic needle",      /*french*/"une aimant boisée",             /*spanish*/"un imán musgoso"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Deku Tree Compass", /*french*/"la boussole de l'Arbre Mojo", /*spanish*/"la brújula del Gran Árbol Deku"}
     );
    hintTable[DODONGOS_CAVERN_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a rocky treasure tracker", /*french*/"un cherche-trésor rocheux",  /*spanish*/"un zahorí rocoso"},
                        Text{"a rocky magnetic needle",      /*french*/"une aimant rocheux",             /*spanish*/"un imán rocoso"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Dodongo's Cavern Compass", /*french*/"la boussole de la Caverne Dodongo", /*spanish*/"la brújula de la Cueva de los Dodongos"}
     );
    hintTable[JABU_JABUS_BELLY_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a fishy treasure tracker", /*french*/"un cherche-trésor digéré",  /*spanish*/"un zahorí digesto"},
                        Text{"a fishy magnetic needle",      /*french*/"une aimant digéré",             /*spanish*/"un imán digesto"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Jabu-Jabu's Belly Compass", /*french*/"la boussole de Jabu-Jabu", /*spanish*/"la brújula de la Tripa de Jabu-Jabu"}
     );
    hintTable[FOREST_TEMPLE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a sylvan treasure tracker", /*french*/"un cherche-trésor sylvestre",  /*spanish*/"un zahorí enselvado"},
                        Text{"a sylvan magnetic needle",      /*french*/"une aimant sylvestre",             /*spanish*/"un imán enselvado"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Forest Temple Compass", /*french*/"la boussole du Temple de la Forêt", /*spanish*/"la brújula del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a molten treasure tracker", /*french*/"un cherche-trésor fondu",  /*spanish*/"un zahorí fundido"},
                        Text{"a molten magnetic needle",      /*french*/"une aimant fondu",             /*spanish*/"un imán fundido"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Fire Temple Compass", /*french*/"la boussole du Temple du Feu", /*spanish*/"la brújula del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a wet treasure tracker", /*french*/"un cherche-trésor humide",  /*spanish*/"un zahorí mojado"},
                        Text{"a wet magnetic needle",      /*french*/"une aimant humide",             /*spanish*/"un imán mojado"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Water Temple Compass", /*french*/"la boussole du Temple de l'Eau", /*spanish*/"la brújula del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a sandy treasure tracker", /*french*/"un cherche-trésor sableux",  /*spanish*/"un zahorí arenoso"},
                        Text{"a sandy magnetic needle",      /*french*/"une aimant sableux",             /*spanish*/"un imán arenoso"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Spirit Temple Compass", /*french*/"la boussole du Temple de l'Esprit", /*spanish*/"la brújula del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a creepy treasure tracker", /*french*/"un cherche-trésor sinistre",  /*spanish*/"un zahorí siniestra"},
                        Text{"a creepy magnetic needle",      /*french*/"une aimant sinistre",             /*spanish*/"un imán siniestra"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Shadow Temple Compass", /*french*/"la boussole du Temple de l'Ombre", /*spanish*/"la brújula del Templo de las Sombras"}
     );
    hintTable[BOTTOM_OF_THE_WELL_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a dank treasure tracker", /*french*/"un cherche-trésor moisi",  /*spanish*/"un zahorí mohoso"},
                        Text{"a dank magnetic needle",      /*french*/"une aimant moisi",             /*spanish*/"un imán mohoso"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Bottom of the Well Compass", /*french*/"la boussole du fond du Puits", /*spanish*/"la brújula del Fondo del pozo"}
     );
    hintTable[ICE_CAVERN_COMPASS] = HintText::Item({
                        //obscure text
                        Text{"a polar treasure tracker", /*french*/"un cherche-trésor polaire",  /*spanish*/"un zahorí polar"},
                        Text{"a polar magnetic needle",      /*french*/"une aimant polaire",             /*spanish*/"un imán polar"},
                      }, {
                        //ambiguous text
                        Text{"a compass", /*french*/"une boussole", /*spanish*/"una brújula"},
                      },
                        //clear text
                        Text{"the Ice Cavern Compass", /*french*/"la Boussole de la Caverne Polaire", /*spanish*/"la brújula de la Caverna de hielo"}
     );
    hintTable[FOREST_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sylvan master of unlocking", /*french*/"un anti-grosse porte sylvestre",  /*spanish*/"la clave enselvada de un jefe"},
                        Text{"a sylvan dungeon's master pass",      /*french*/"une clé maléfique sylvestree",             /*spanish*/"el pase maestro enselvado"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Forest Temple Boss Key", /*french*/"la Clé d'Or du Temple de la Forêt", /*spanish*/"la gran llave del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a molten master of unlocking", /*french*/"un anti-grosse porte fondu",  /*spanish*/"la clave fundido de un jefe"},
                        Text{"a molten dungeon's master pass",      /*french*/"une clé maléfique fondu",             /*spanish*/"el pase maestro fundido"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Fire Temple Boss Key", /*french*/"la Clé d'Or du Temple du Feu", /*spanish*/"la gran llave del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a wet master of unlocking", /*french*/"un anti-grosse porte humide",  /*spanish*/"la clave mojado de un jefe"},
                        Text{"a wet dungeon's master pass",      /*french*/"une clé maléfique humide",             /*spanish*/"el pase maestro mojado"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Water Temple Boss Key", /*french*/"la Clé d'Or du Temple de l'Eau", /*spanish*/"la gran llave del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sandy master of unlocking", /*french*/"un anti-grosse porte sableux",  /*spanish*/"la clave arenoso de un jefe"},
                        Text{"a sandy dungeon's master pass",      /*french*/"une clé maléfique sableux",             /*spanish*/"el pase maestro arenoso"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Spirit Temple Boss Key", /*french*/"la Clé d'Or du Temple de l'Esprit", /*spanish*/"la gran llave del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a creepy master of unlocking", /*french*/"un anti-grosse porte sinistre",  /*spanish*/"la clave siniestra de un jefe"},
                        Text{"a creepy dungeon's master pass",      /*french*/"une clé maléfique sinistre",             /*spanish*/"el pase maestro siniestra"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Shadow Temple Boss Key", /*french*/"la Clé d'Or du Temple de l'Ombre", /*spanish*/"la gran llave del Templo de las Sombras"}
     );
    hintTable[GANONS_CASTLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a final master of unlocking", /*french*/"un anti-grosse porte final",  /*spanish*/"la clave final de un jefe"},
                        Text{"a final dungeon's master pass",      /*french*/"une clé maléfique final",             /*spanish*/"el pase maestro final"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une Clé d'Or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Ganon's Castle Boss Key", /*french*/"la Clé d'Or du Château de Ganon", /*spanish*/"la gran llave del Castillo de Ganon"}
     );
    hintTable[FOREST_TEMPLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sylvan tool for unlocking", /*french*/"un anti-porte sylvestre",            /*spanish*/"una clave de una entrada enselvada"},
                        Text{"a sylvan dungeon pass",       /*french*/"le rêve sylvestre d'un prisonnier",  /*spanish*/"un pase de una mazmorra enselvada"},
                        Text{"a sylvan lock remover",       /*french*/"un efface-serrure sylvestre",        /*spanish*/"un destructor de cerraduras enselvada"},
                        Text{"a sylvan lockpick",           /*french*/"un crochet à porte sylvestre",       /*spanish*/"una apertura portentosa enselvada"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Forest Temple Small Key", /*french*/"une petite clé du Temple de la Forêt", /*spanish*/"una llave pequeña del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a molten tool for unlocking", /*french*/"un anti-porte fondu",            /*spanish*/"una clave de una entrada fundida"},
                        Text{"a molten dungeon pass",       /*french*/"le rêve fondu d'un prisonnier",  /*spanish*/"un pase de una mazmorra fundida"},
                        Text{"a molten lock remover",       /*french*/"un efface-serrure fondu",        /*spanish*/"un destructor de cerraduras fundida"},
                        Text{"a molten lockpick",           /*french*/"un crochet à porte fondu",       /*spanish*/"una apertura portentosa fundida"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Fire Temple Small Key", /*french*/"une petite clé du Temple du Feu", /*spanish*/"una llave pequeña del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a wet tool for unlocking", /*french*/"un anti-porte humide",            /*spanish*/"una clave de una entrada mojada"},
                        Text{"a wet dungeon pass",       /*french*/"le rêve humide d'un prisonnier",  /*spanish*/"un pase de una mazmorra mojada"},
                        Text{"a wet lock remover",       /*french*/"un efface-serrure humide",        /*spanish*/"un destructor de cerraduras mojada"},
                        Text{"a wet lockpick",           /*french*/"un crochet à porte humide",       /*spanish*/"una apertura portentosa mojada"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Water Temple Small Key", /*french*/"une petite clé du Temple de l'Eau", /*spanish*/"una llave pequeña del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sandy tool for unlocking", /*french*/"un anti-porte sableux",            /*spanish*/"una clave de una entrada arenosa"},
                        Text{"a sandy dungeon pass",       /*french*/"le rêve sableux d'un prisonnier",  /*spanish*/"un pase de una mazmorra arenosa"},
                        Text{"a sandy lock remover",       /*french*/"un efface-serrure sableux",        /*spanish*/"un destructor de cerraduras arenosa"},
                        Text{"a sandy lockpick",           /*french*/"un crochet à porte sableux",       /*spanish*/"una apertura portentosa arenosa"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Spirit Temple Small Key", /*french*/"une petite clé du Temple de l'Esprit", /*spanish*/"una llave pequeña del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a creepy tool for unlocking", /*french*/"un anti-porte sinistre",            /*spanish*/"una clave de una entrada siniestra:a"},
                        Text{"a creepy dungeon pass",       /*french*/"le rêve sinistre d'un prisonnier",  /*spanish*/"un pase de una mazmorra siniestra:a"},
                        Text{"a creepy lock remover",       /*french*/"un efface-serrure sinistre",        /*spanish*/"un destructor de cerraduras siniestra:a"},
                        Text{"a creepy lockpick",           /*french*/"un crochet à porte sinistre",       /*spanish*/"una apertura portentosa siniestra:a"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Shadow Temple Small Key", /*french*/"une petite clé du Temple de l'Ombre", /*spanish*/"una llave pequeña del Templo de las Sombras"}
     );
    hintTable[GERUDO_TRAINING_GROUNDS_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a labyrinthian tool for unlocking", /*french*/"un anti-porte labyrinthique",            /*spanish*/"una clave de una entrada laberíntica"},
                        Text{"a labyrinthian dungeon pass",       /*french*/"le rêve labyrinthique d'un prisonnier",  /*spanish*/"un pase de una mazmorra laberíntica"},
                        Text{"a labyrinthian lock remover",       /*french*/"un efface-serrure labyrinthique",        /*spanish*/"un destructor de cerraduras laberíntica"},
                        Text{"a labyrinthian lockpick",           /*french*/"un crochet à porte labyrinthique",       /*spanish*/"una apertura portentosa laberíntica"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Gerudo Training Ground Small Key", /*french*/"une petite clé du Gymnase Gerudo", /*spanish*/"una llave pequeña del Centro de Instrucción Gerudo"}
     );
    hintTable[GERUDO_FORTRESS_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"an imprisoned tool for unlocking", /*french*/"un anti-porte emprisonné",            /*spanish*/"una clave de una entrada encarcelada"},
                        Text{"an imprisoned dungeon pass",       /*french*/"le rêve emprisonné d'un prisonnier",  /*spanish*/"un pase de una mazmorra encarcelada"},
                        Text{"an imprisoned lock remover",       /*french*/"un efface-serrure emprisonné",        /*spanish*/"un destructor de cerraduras encarcelada"},
                        Text{"an imprisoned lockpick",           /*french*/"un crochet à porte emprisonné",       /*spanish*/"una apertura portentosa encarcelada"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Gerudo Fortress Small Key", /*french*/"une petite clé de la Repaire des Voleurs", /*spanish*/"una llave pequeña de la Fortaleza Gerudo"}
     );
    hintTable[BOTTOM_OF_THE_WELL_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a moldy tool for unlocking", /*french*/"un anti-porte moisi",            /*spanish*/"una clave de una entrada mohosa"},
                        Text{"a moldy dungeon pass",       /*french*/"le rêve moisi d'un prisonnier",  /*spanish*/"un pase de una mazmorra mohosa"},
                        Text{"a moldy lock remover",       /*french*/"un efface-serrure moisi",        /*spanish*/"un destructor de cerraduras mohosa"},
                        Text{"a moldy lockpick",           /*french*/"un crochet à porte moisi",       /*spanish*/"una apertura portentosa mohosa"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Bottom of the Well Small Key", /*french*/"une petite clé du fond du Puits", /*spanish*/"una llave pequeña del Fondo del pozo"}
     );
    hintTable[GANONS_CASTLE_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a final tool for unlocking", /*french*/"un anti-porte final",            /*spanish*/"una clave de una entrada final"},
                        Text{"a final dungeon pass",       /*french*/"le rêve final d'un prisonnier",  /*spanish*/"un pase de una mazmorra final"},
                        Text{"a final lock remover",       /*french*/"un efface-serrure final",        /*spanish*/"un destructor de cerraduras final"},
                        Text{"a final lockpick",           /*french*/"un crochet à porte final",       /*spanish*/"una apertura portentosa final"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Ganon's Castle Small Key", /*french*/"une petite clé du Château de Ganon", /*spanish*/"una llave pequeña del Castillo de Ganon"}
     );
     hintTable[FOREST_TEMPLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a sylvan toolbox for unlocking", /*french*/"des anti-portes sylvestres",            /*spanish*/"un conjunto silvestre de cerrajero"},
                        Text{"a sylvan dungeon season pass",   /*french*/"les rêves sylvestres d'un prisonnier",  /*spanish*/"un pase vip de mazmorras silvestre"},
                        Text{"a sylvan jingling ring",         /*french*/"des efface-serrures sylvestres",        /*spanish*/"una cadena multiusos silvestre"},
                        Text{"a sylvan skeleton key",          /*french*/"des crochets à porte sylvestres",       /*spanish*/"un anillo silvestre contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Forest Temple Key Ring", /*french*/"un trousseau de clés du Temple de la Forêt", /*spanish*/"un llavero del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a molten toolbox for unlocking", /*french*/"des anti-portes fondus",            /*spanish*/"un conjunto fundido de cerrajero"},
                        Text{"a molten dungeon season pass",   /*french*/"les rêves fondus d'un prisonnier",  /*spanish*/"un pase vip de mazmorras fundido"},
                        Text{"a molten jingling ring",         /*french*/"des efface-serrures fondus",        /*spanish*/"una cadena multiusos fundida"},
                        Text{"a molten skeleton key",          /*french*/"des crochets à porte fondus",       /*spanish*/"un anillo fundido contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Fire Temple Key Ring", /*french*/"un trousseau de clés du Temple du Feu", /*spanish*/"un llavero del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a wet toolbox for unlocking", /*french*/"des anti-portes humides",            /*spanish*/"un conjunto abisal de cerrajero"},
                        Text{"a wet dungeon season pass",   /*french*/"les rêves humides d'un prisonnier",  /*spanish*/"un pase vip de mazmorras abisal"},
                        Text{"a wet jingling ring",         /*french*/"des efface-serrures humides",        /*spanish*/"una cadena multiusos abisal"},
                        Text{"a wet skeleton key",          /*french*/"des crochets à porte humides",       /*spanish*/"un anillo abisal contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Water Temple Key Ring", /*french*/"un trousseau de clés du Temple de l'Eau", /*spanish*/"un llavero del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a sandy toolbox for unlocking", /*french*/"des anti-portes sableux",            /*spanish*/"un conjunto arenoso de cerrajero"},
                        Text{"a sandy dungeon season pass",   /*french*/"les rêves sableux d'un prisonnier",  /*spanish*/"un pase vip de mazmorras arenoso"},
                        Text{"a sandy jingling ring",         /*french*/"des efface-serrures sableux",        /*spanish*/"una cadena multiusos arenosa"},
                        Text{"a sandy skeleton key",          /*french*/"des crochets à porte sableux",       /*spanish*/"un anillo arenoso contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Spirit Temple Key Ring", /*french*/"un trousseau de clés du Temple de l'Esprit", /*spanish*/"un llavero del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a creepy toolbox for unlocking", /*french*/"des anti-portes sinistres",            /*spanish*/"un conjunto tenebroso de cerrajero"},
                        Text{"a creepy dungeon season pass",   /*french*/"les rêves sinistres d'un prisonnier",  /*spanish*/"un pase vip de mazmorras tenebroso"},
                        Text{"a creepy jingling ring",         /*french*/"des efface-serrures sinistres",        /*spanish*/"una cadena multiusos tenebrosa"},
                        Text{"a creepy skeleton key",          /*french*/"des crochets à porte sinistres",       /*spanish*/"un anillo tenebroso contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Shadow Temple Key Ring", /*french*/"un trousseau de clés du Temple de l'Ombre", /*spanish*/"un llavero del Templo de las Sombras"}
     );
    hintTable[GERUDO_TRAINING_GROUNDS_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a labyrinthian toolbox for unlocking", /*french*/"des anti-portes labyrinthiques",            /*spanish*/"un conjunto laberíntico de cerrajero"},
                        Text{"a labyrinthian dungeon season pass",   /*french*/"les rêves labyrinthiques d'un prisonnier",  /*spanish*/"un pase vip de mazmorras laberíntico"},
                        Text{"a labyrinthian jingling ring",         /*french*/"des efface-serrures labyrinthiques",        /*spanish*/"una cadena multiusos laberíntica"},
                        Text{"a labyrinthian skeleton key",          /*french*/"des crochets à porte labyrinthiques",       /*spanish*/"un anillo laberíntico contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Gerudo Training Ground Key Ring", /*french*/"un trousseau de clés du Gymnase Gerudo", /*spanish*/"un llavero del Centro de Instrucción Gerudo"}
     );
    hintTable[GERUDO_FORTRESS_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"an imprisoned toolbox for unlocking", /*french*/"des anti-portes emprisonnés",            /*spanish*/"un conjunto enjaulado de cerrajero"},
                        Text{"an imprisoned dungeon season pass",   /*french*/"les rêves emprisonnés d'un prisonnier",  /*spanish*/"un pase vip de una mazmorra enjaulado"},
                        Text{"an imprisoned jingling ring",         /*french*/"des efface-serrures emprisonnés",        /*spanish*/"una cadena multiusos enjaulada"},
                        Text{"an imprisoned skeleton key",          /*french*/"des crochets à porte emprisonnés",       /*spanish*/"un anillo enjaulado contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Gerudo Fortress Key Ring", /*french*/"un trousseau de clés de la Repaire des Voleurs", /*spanish*/"un llavero de la Fortaleza Gerudo"}
     );
    hintTable[BOTTOM_OF_THE_WELL_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a moldy toolbox for unlocking", /*french*/"des anti-portes moisis",            /*spanish*/"un conjunto subterráneo de cerrajero"},
                        Text{"a moldy dungeon season pass",   /*french*/"les rêves moisis d'un prisonnier",  /*spanish*/"un pase vip de una mazmorra subterráneo"},
                        Text{"a moldy jingling ring",         /*french*/"des efface-serrures moisis",        /*spanish*/"una cadena multiusos subterránea"},
                        Text{"a moldy skeleton key",          /*french*/"des crochets à porte moisis",       /*spanish*/"un anillo subterráneo contra cerrojos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Bottom of the Well Key Ring", /*french*/"un trousseau de clés du fond du Puits", /*spanish*/"un llavero del Fondo del pozo"}
     );
    hintTable[GANONS_CASTLE_KEY_RING] = HintText::Item({
                        //obscure text
                        Text{"a final toolbox for unlocking", /*french*/"des anti-portes finaux",            /*spanish*/"un conjunto decisivo de cerrajero"},
                        Text{"a final dungeon season pass",   /*french*/"les rêves finaux d'un prisonnier",  /*spanish*/"un pase vip de una mazmorra decisivo"},
                        Text{"a final jingling ring",         /*french*/"des efface-serrures finaux",        /*spanish*/"una cadena multiusos decisiva"},
                        Text{"a final skeleton key",          /*french*/"des crochets à porte finaux",       /*spanish*/"un anillo decisivo multiusos"},
                      }, {
                        //ambiguous text
                        Text{"a key ring", /*french*/"un trousseau de clés", /*spanish*/"un llavero"},
                      },
                        //clear text
                        Text{"a Ganon's Castle Key Ring", /*french*/"un trousseau de clés du Château de Ganon", /*spanish*/"un llavero del Castillo de Ganon"}
     );

    hintTable[TREASURE_GAME_SMALL_KEY] = HintText::Item({
                        //obscure text
                        Text{"a gambler's tool for unlocking", /*french*/"un anti-porte de parieur",        /*spanish*/"una clave de un juego de azar"},
                        Text{"a gambler's dungeon pass",       /*french*/"le rêve d'un prisonnier parieur", /*spanish*/"un pase de un juego de azar"},
                        Text{"a gambler's lock remover",       /*french*/"un efface-serrure de parieur",    /*spanish*/"un destructor de cerraduras del juego de azar"},
                        Text{"a gambler's lockpick",           /*french*/"un crochet à serrure de parieur", /*spanish*/"una apertura portentosa del juego de azar"},
                      }, {
                        //ambiguous text
                        Text{"a small key", /*french*/"une petite clé", /*spanish*/"una llave pequeña"},
                      },
                        //clear text
                        Text{"a Treasure Chest Shop Small Key", /*french*/"une petite clé de la chasse aux trésors", /*spanish*/"una llave pequeña del Cofre del Tesoro"}
    );

    hintTable[KOKIRI_EMERALD] = HintText::Item({
                       //obscure text
                       Text{"a green stone",       /*french*/"une pierre verte",               /*spanish*/"una piedra verde"},
                       Text{"a gift before death", /*french*/"le dernier souffle d'un arbre", /*spanish*/"un obsequio testamentario"},
                     }, {
                       //ambiguous text
                       Text{"a spiritual stone", /*french*/"une Pierre Ancestrale", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Kokiri Emerald", /*french*/"l'Émeraude Kokiri", /*spanish*/"la Esmeralda de los Kokiri"}
    );

    hintTable[GORON_RUBY] = HintText::Item({
                       //obscure text
                       Text{"a red stone",       /*french*/"une pierre rouge",         /*spanish*/"una piedra carmín"},
                       Text{"sworn brotherhood", /*french*/"un serment de fraternité", /*spanish*/"el juramento de hermanos de sangre"},
                     }, {
                       //ambiguous text
                       Text{"a spiritual stone", /*french*/"une Pierre Ancestrale", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Goron Ruby", /*french*/"le Rubis Goron", /*spanish*/"el Rubí de los Goron"}
    );

    hintTable[ZORA_SAPPHIRE] = HintText::Item({
                       //obscure text
                       Text{"a blue stone",       /*french*/"une pierre bleue",     /*spanish*/"una piedra celeste"},
                       Text{"an engagement gift", /*french*/"un cadeau de mariage", /*spanish*/"un regalo de compromiso"},
                     }, {
                       //ambiguous text
                       Text{"a spiritual stone", /*french*/"une Pierre Ancestrale", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Zora Sapphire", /*french*/"le Saphir Zora", /*spanish*/"el Zafiro de los Zora"}
    );

    hintTable[FOREST_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"a green coin",       /*french*/"une pièce verte",    /*spanish*/"una moneda esmeralda"},
                       Text{"Saria's friendship", /*french*/"l'amitié de Saria", /*spanish*/"la amistad de Saria"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Forest Medallion", /*french*/"le Médaillon de la Forêt", /*spanish*/"el Medallón del Bosque"}
    );

    hintTable[FIRE_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"a red coin",      /*french*/"une pièce rouge",           /*spanish*/"una moneda rubí"},
                       Text{"Darunia's power", /*french*/"la fraternité de Darunia", /*spanish*/"la fraternidad de Darunia"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Fire Medallion", /*french*/"le Médaillon du Feu", /*spanish*/"el Medallón del Fuego"}
    );

    hintTable[WATER_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"a blue coin",  /*french*/"une pièce bleue",  /*spanish*/"una moneda zafiro"},
                       Text{"Ruto's power", /*french*/"l'amour de Ruto", /*spanish*/"el amor de Ruto"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Water Medallion", /*french*/"le Médaillon de l'Eau", /*spanish*/"el Medallón del Agua"}
    );

    hintTable[SPIRIT_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"an orange coin",  /*french*/"une pièce orange",       /*spanish*/"una moneda ámbar"},
                       Text{"Nabooru's power", /*french*/"le respect de Nabooru", /*spanish*/"el respeto de Nabooru"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Spirit Medallion", /*french*/"le Médaillon de l'Esprit", /*spanish*/"el Medallón del Espíritu"}
    );

    hintTable[SHADOW_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"a purple coin", /*french*/"une pièce pourpre",    /*spanish*/"una moneda malva"},
                       Text{"Impa's power",  /*french*/"la confiance d'Impa", /*spanish*/"la confianza de Impa"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Shadow Medallion", /*french*/"le Médaillon de l'Ombre", /*spanish*/"el Medallón de la Sombra"}
    );

    hintTable[LIGHT_MEDALLION] = HintText::Item({
                       //obscure text
                       Text{"a yellow coin", /*french*/"une pièce jaune",  /*spanish*/"una moneda resplandeciente"},
                       Text{"Rauru's power", /*french*/"la foi de Rauru", /*spanish*/"la fe de Rauru"},
                     }, {
                       //ambiguous text
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                     },
                       //clear text
                       Text{"the Light Medallion", /*french*/"le Médaillon de la Lumière", /*spanish*/"el Medallón de la Luz"}
    );

    hintTable[RECOVERY_HEART] = HintText::Item({
                       //obscure text
                       Text{"a free heal",   /*french*/"un bec-au-bobo", /*spanish*/"una cura de regalo"},
                       Text{"a hearty meal", /*french*/"un petit amour",   /*spanish*/"una sanación romántica"},
                       Text{"a Band-Aid",    /*french*/"un diachylon",     /*spanish*/"un corazoncito sanador"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de coeur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Recovery Heart", /*french*/"un coeur de vie", /*spanish*/"un corazón"}
    );

    hintTable[GREEN_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"a unique coin", /*french*/"un rubis bien mérité", /*spanish*/"una singular moneda"},
                       Text{"a penny",       /*french*/"un sou",               /*spanish*/"un peso hyliano"},
                       Text{"a green gem",   /*french*/"un joyau vert",        /*spanish*/"una gema verde"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Green Rupee", /*french*/"un rubis vert", /*spanish*/"una rupia verde"}
    );

    hintTable[GREG_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"an old friend",  /*french*/"Greg", /*spanish*/"Greg"},
                       Text{"a glorious gem", /*french*/"Greg", /*spanish*/"Greg"},
                     }, {
                       //ambiguous text
                       Text{"a Green Rupee", /*french*/"un rubis vert", /*spanish*/"una rupia verde"}
                     },
                       //clear text
                       Text{"Greg", /*french*/"Greg", /*spanish*/"Greg"}
    );

    hintTable[BLUE_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"a common coin", /*french*/"quelques sous", /*spanish*/"una moneda usual"},
                       Text{"a blue gem",    /*french*/"un joyau bleu",  /*spanish*/"una gema azul"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Blue Rupee", /*french*/"un rubis bleu", /*spanish*/"una rupia azul"}
    );

    hintTable[RED_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"couch cash", /*french*/"un peu de fric", /*spanish*/"una buena moneda"},
                       Text{"a red gem",  /*french*/"un joyau rouge", /*spanish*/"una gema roja"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Red Rupee", /*french*/"un rubis rouge", /*spanish*/"una rupia roja"}
    );

    hintTable[PURPLE_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"big bucks",    /*french*/"plein de fric",  /*spanish*/"plata de calidad"},
                       Text{"a purple gem", /*french*/"un joyau mauve", /*spanish*/"una gema morada"},
                       Text{"wealth",       /*french*/"la richesse",    /*spanish*/"una buena riqueza"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Purple Rupee", /*french*/"un rubis pourpre", /*spanish*/"una rupia morada"}
    );

    hintTable[HUGE_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"a juicy jackpot", /*french*/"le jackpot",           /*spanish*/"el premio gordo"},
                       Text{"a yellow gem",    /*french*/"un joyau doré",         /*spanish*/"una gema amarilla"},
                       Text{"a giant gem",     /*french*/"un gros joyau",         /*spanish*/"una gema descomunal"},
                       Text{"great wealth",    /*french*/"l'aisance financière", /*spanish*/"dinero a caudales"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Huge Rupee", /*french*/"un énorme rubis", /*spanish*/"una rupia gigante"}
    );

    hintTable[PIECE_OF_HEART] = HintText::Item({
                       //obscure text
                       Text{"a little love",  /*french*/"un peu plus d'amour", /*spanish*/"un cuarto de amor"},
                       Text{"a broken heart", /*french*/"un coeur brisé",       /*spanish*/"un corazón roto"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de coeur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Piece of Heart", /*french*/"un Quart de Coeur", /*spanish*/"una pieza de corazón"}
    );

    hintTable[HEART_CONTAINER] = HintText::Item({
                       //obscure text
                       Text{"a lot of love",      /*french*/"le grand amour",             /*spanish*/"amor por doquier"},
                       Text{"a Valentine's gift", /*french*/"un cadeau de Saint-Valentin", /*spanish*/"un contenedor de afección"},
                       Text{"a boss's organ",     /*french*/"un organe de monstre",        /*spanish*/"los órganos de un jefe"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de coeur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Heart Container", /*french*/"un Réceptacle de Coeur", /*spanish*/"un contenedor de corazón"}
    );

    hintTable[ICE_TRAP] = HintText::Item({
                       //obscure text
                       Text{"a gift from Ganon",    /*french*/"un cadeau de Ganon",         /*spanish*/"un regalo de Ganon"},
                       Text{"a chilling discovery", /*french*/"une frissonante découverte", /*spanish*/"un escalofriante hallazgo"},
                       Text{"frosty fun",           /*french*/"une engelure",               /*spanish*/"una gélida diversión"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                       Text{"a magic arrow", /*french*/"une flèche magique", /*spanish*/"una flecha mágica"},
                       Text{"a medallion", /*french*/"un médaillon", /*spanish*/"un medallón"},
                       Text{"a spiritual stone", /*french*/"une Pierre Ancestrale", /*spanish*/"una piedra espiritual"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"an Ice Trap", /*french*/"un Piège de Glace", /*spanish*/"una trampa de hielo"}
    );

  //MILK

    hintTable[BOMBS_5] = HintText::Item({
                       //obscure text
                       Text{"a few explosives",  /*french*/"une poignée de pétards",       /*spanish*/"un par de explosivos"},
                       Text{"a few blast balls", /*french*/"une poignée de boules bleues", /*spanish*/"un par de estallidos"},
                     }, {
                       //ambiguous text
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombs (5 pieces)", /*french*/"une demi-dizaine de bombes", /*spanish*/"unas (5) bombas"}
    );

    hintTable[BOMBS_10] = HintText::Item({
                       //obscure text
                       Text{"some explosives",  /*french*/"un paquet de pétards",       /*spanish*/"unos cuantos explosivos"},
                       Text{"some blast balls", /*french*/"un paquet de boules bleues", /*spanish*/"unos cuantos estallidos"},
                     }, {
                       //ambiguous text
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombs (10 pieces)", /*french*/"une dizaine de bombes", /*spanish*/"unas (10) bombas"}
    );

    hintTable[BOMBS_20] = HintText::Item({
                       //obscure text
                       Text{"lots-o-explosives",     /*french*/"une abondance de pétards",       /*spanish*/"un puñado de explosivos"},
                       Text{"plenty of blast balls", /*french*/"une abondance de boules bleues", /*spanish*/"bastantes estallidos"},
                     }, {
                       //ambiguous text
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombs (20 pieces)", /*french*/"une vingtaine de bombes", /*spanish*/"unas (20) bombas"}
    );

    hintTable[BOMBCHU_5] = HintText::Item({
                       //obscure text
                       Text{"a few mice bombs",     /*french*/"une poignée de mignons explosifs", /*spanish*/"un par de bombas roedoras"},
                       Text{"a few proximity mice", /*french*/"une poignée de jouets à remonter", /*spanish*/"un par de explosivos ratoncitos"},
                       Text{"a few wall crawlers",  /*french*/"une poignée de rapides grimpeurs", /*spanish*/"un par de trepaparedes"},
                       Text{"a few trail blazers",  /*french*/"une poignée de zigzags éclatants", /*spanish*/"un par de ratas propulsadas"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombchus (5 pieces)", /*french*/"une demi-dizaine de Missiles", /*spanish*/"unos (5) bombchus"}
    );

    hintTable[BOMBCHU_10] = HintText::Item({
                       //obscure text
                       Text{"some mice bombs",     /*french*/"un paquet de mignons explosifs", /*spanish*/"unas cuantas bombas roedoras"},
                       Text{"some proximity mice", /*french*/"un paquet de jouets à remonter", /*spanish*/"unos cuantos explosivos ratoncitos"},
                       Text{"some wall crawlers",  /*french*/"un paquet de rapides grimpeurs", /*spanish*/"unos cuantos trepaparedes"},
                       Text{"some trail blazers",  /*french*/"un paquet de zigzags éclatants", /*spanish*/"unas cuantas ratas propulsadas"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombchus (10 pieces)", /*french*/"une dizaine de Missiles", /*spanish*/"unos (10) bombchus"}
    );

    hintTable[BOMBCHU_20] = HintText::Item({
                       //obscure text
                       Text{"plenty of mice bombs",     /*french*/"une abondance de mignons explosifs", /*spanish*/"bastantes bombas roedoras"},
                       Text{"plenty of proximity mice", /*french*/"une abondance de jouets à remonter", /*spanish*/"bastantes explosivos ratoncitos"},
                       Text{"plenty of wall crawlers",  /*french*/"une abondance de rapides grimpeurs", /*spanish*/"bastantes trepaparedes"},
                       Text{"plenty of trail blazers",  /*french*/"une abondance de zigzags éclatants", /*spanish*/"bastantes ratas propulsadas"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"explosives", /*french*/"un paquet d'explosifs", /*spanish*/"un montón de explosivos"},
                     },
                       //clear text
                       Text{"Bombchus (20 pieces)", /*french*/"une vingtaine de Missiles", /*spanish*/"unos (20) bombchus"}
    );

  //BOMBCHU_DROP

    hintTable[ARROWS_5] = HintText::Item({
                       //obscure text
                       Text{"a few danger darts", /*french*/"une poignée d'obus",    /*spanish*/"un par de peligrosos dardos"},
                       Text{"a few sharp shafts", /*french*/"une poignée de piquets", /*spanish*/"un par de puntas afiladas"},
                     }, {
                       //ambiguous text
                       Text{"a projectile", /*french*/"un projectile", /*spanish*/"un proyectil"},
                     },
                       //clear text
                       Text{"Arrows (5 pieces)", /*french*/"une demi-dizaine de flèches", /*spanish*/"unas (5) flechas"}
    );

    hintTable[ARROWS_10] = HintText::Item({
                       //obscure text
                       Text{"some danger darts", /*french*/"un paquet d'obus",     /*spanish*/"unos cuantos peligrosos dardos"},
                       Text{"some sharp shafts", /*french*/"un paquet de piquets", /*spanish*/"unas cuantas puntas afiladas"},
                     }, {
                       //ambiguous text
                       Text{"a projectile", /*french*/"un projectile", /*spanish*/"un proyectil"},
                     },
                       //clear text
                       Text{"Arrows (10 pieces)", /*french*/"une dizaine de flèches", /*spanish*/"unas (10) flechas"}
    );

    hintTable[ARROWS_30] = HintText::Item({
                       //obscure text
                       Text{"plenty of danger darts", /*french*/"une abondance d'obus",     /*spanish*/"bastantes peligrosos dardos"},
                       Text{"plenty of sharp shafts", /*french*/"une abondance de piquets", /*spanish*/"bastantes puntas afiladas"},
                     }, {
                       //ambiguous text
                       Text{"a projectile", /*french*/"un projectile", /*spanish*/"un proyectil"},
                     },
                       //clear text
                       Text{"Arrows (30 pieces)", /*french*/"une trentaine de flèches", /*spanish*/"unas (30) flechas"}
    );

    hintTable[DEKU_NUTS_5] = HintText::Item({
                       //obscure text
                       Text{"some nuts",       /*french*/"une poignée de noisettes",     /*spanish*/"un par de nueces"},
                       Text{"some flashbangs", /*french*/"une poignée d'éclats",        /*spanish*/"un par de semillas aturdidoras"},
                       Text{"some scrub spit", /*french*/"une poignée de crachats Mojo", /*spanish*/"un par de escupitajos deku"},
                     }, {
                       //ambiguous text
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"Deku Nuts (5 pieces)", /*french*/"une demi-dizaine de noix Mojo", /*spanish*/"unas (5) nueces deku"}
    );

    hintTable[DEKU_NUTS_10] = HintText::Item({
                       //obscure text
                       Text{"lots-o-nuts",          /*french*/"un paquet de noisettes",     /*spanish*/"un puñado de nueces"},
                       Text{"plenty of flashbangs", /*french*/"un paquet d'éclats",         /*spanish*/"unas cuantas semillas aturdidoras"},
                       Text{"plenty of scrub spit", /*french*/"un paquet de crachats Mojo", /*spanish*/"unos cuantos escupitajos deku"},
                     }, {
                       //ambiguous text
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"Deku Nuts (10 pieces)", /*french*/"une dizaine de noix Mojo", /*spanish*/"unas (10) nueces deku"}
    );

    hintTable[DEKU_SEEDS_30] = HintText::Item({
                       //obscure text
                       Text{"catapult ammo", /*french*/"un paquet de délicieuses munitions", /*spanish*/"un par de munición infantil"},
                       Text{"lots-o-seeds",  /*french*/"un paquet de germes séchés",   /*spanish*/"un puñado de semillas"},
                     }, {
                       //ambiguous text
                       Text{"a projectile", /*french*/"un projectile", /*spanish*/"un proyectil"},
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                     },
                       //clear text
                       Text{"Deku Seeds (30 pieces)", /*french*/"une trentaine de graines Mojo", /*spanish*/"unas (30) semillas deku"}
    );

    hintTable[DEKU_STICK_1] = HintText::Item({
                       //obscure text
                       Text{"a breakable branch", /*french*/"un bout de bois", /*spanish*/"un pequeño báculo"},
                     }, {
                       //ambiguous text
                       Text{"some Deku munitions", /*french*/"un paquet de munitions Mojo", /*spanish*/"un montón de municiones Deku"},
                     },
                       //clear text
                       Text{"a Deku Stick", /*french*/"un bâton Mojo", /*spanish*/"un palo deku"}
    );

    hintTable[TREASURE_GAME_HEART] = HintText::Item({
                       //obscure text
                       Text{"a victory valentine", /*french*/"un amour gagnant", /*spanish*/"el amor victorioso"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de coeur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Piece of Heart", /*french*/"un Quart de Coeur", /*spanish*/"el amor de la victoria"}
    );

    hintTable[TREASURE_GAME_GREEN_RUPEE] = HintText::Item({
                       //obscure text
                       Text{"the dollar of defeat", /*french*/"le rubis de la défaite", /*spanish*/"el peso de la derrota"},
                     }, {
                       //ambiguous text
                       Text{"some rupees", /*french*/"une quantité de rubis", /*spanish*/"una cantidad de rupias"},
                     },
                       //clear text
                       Text{"a Green Rupee", /*french*/"un rubis vert", /*spanish*/"una rupia verde"}
    );

    hintTable[TRIFORCE_PIECE] = HintText::Item({
                       //obscure text
                       Text{"a triumph fork",  /*french*/"la Tribosse",     /*spanish*/"un trígono del triunfo"},
                       Text{"cheese",          /*french*/"du fromage",      /*spanish*/"un porción de queso"},
                       Text{"a gold fragment", /*french*/"un fragment d'or", /*spanish*/"un fragmento dorado"},
                     }, {},
                       //clear text
                       Text{"a Piece of the Triforce", /*french*/"un fragment de la Triforce", /*spanish*/"un fragmento de la Trifuerza"}
    );

    hintTable[EPONA] = HintText::Item({
                       //obscure text
                       Text{"a horse",              /*french*/"un fidèle destrier", /*spanish*/"una yegua"},
                       Text{"a four legged friend", /*french*/"un puissant animal", /*spanish*/"una amiga cuadrúpeda"},
                     }, {
                       //ambiguous text
                       Text{"something from Malon", /*french*/"un cadeau de Malon", /*spanish*/"un obsequio de Malon"},
                       Text{"a song sung by frogs", /*french*/"une chanson aimée des grenouilles", /*spanish*/"una melodía de ranas"},
                       Text{"something to cross a broken bridge", /*french*/"une chose pour traverser un pont brisé", /*spanish*/"algo para cruzar un puente roto"},
                     },
                       //clear text
                       Text{"Epona", /*french*/"Epona", /*spanish*/"a Epona"}
    );

    // [HINT_ERROR] = HintText::Item({
    //                      //obscure text
    //                      Text{"something mysterious", /*french*/"un sacré mystère", /*spanish*/"algo misterioso"},
    //                      Text{"an unknown treasure",  /*french*/"un trésor inconnu", /*spanish*/"un desconocido tesoro"},
    //                    },
    //                      //clear text
    //                      Text{"An Error (Please Report This)", /*french*/"une erreur (signaler S.V.P.)", /*spanish*/"un error (repórtelo si es posible)"}
    // );
}
