#include "hint_list.hpp"
#include "custom_messages.hpp"

#include <array>

using namespace CustomMessages;

//Big thanks to Lioncache, Gabyelnuevo, Danius88, and Charade for their translations!

//Text is {english, french, spanish}

// there are special characters that are read for certain in game commands:
// ^ is a box break
// & is a new line
// @ will print the player name
// surrounding text with '#' will make it a different color
// - OoT Randomizer

// '%d' indicates a number will be placed there.

static std::array<HintText, KEY_ENUM_MAX> hintTable;

void HintTable_Init() {
    /*--------------------------
    |       GENERAL TEXT       |
    ---------------------------*/
    hintTable[NONE]            = HintText::Exclude({Text{"No Hint","",""}});
    hintTable[PREFIX]          = HintText::Exclude({Text{"They say that ",              /*french*/"Selon moi, ",                  /*spanish*/"Según dicen, "}});
    hintTable[WAY_OF_THE_HERO] = HintText::Exclude({Text{" is on the way of the hero.", /*french*/" est sur le chemin du héros.", /*spanish*/" conduce a la senda del héroe."}});
    hintTable[PLUNDERING]      = HintText::Exclude({Text{"plundering ",                 /*french*/"explorer ",                    /*spanish*/"inspeccionar "}});
    hintTable[FOOLISH]         = HintText::Exclude({Text{" is a foolish choice.",       /*french*/" est futile.",                 /*spanish*/" no es una sabia decisión."}});
    hintTable[CAN_BE_FOUND_AT] = HintText::Exclude({Text{"can be found at",             /*french*/"se trouve dans",               /*spanish*/"aguarda en"}});
    hintTable[HOARDS]          = HintText::Exclude({Text{"hoards",                      /*french*/"recèle",                       /*spanish*/"acapara"}});


    /*--------------------------
    |      ITEM HINT TEXT      |
    ---------------------------*/
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
                       Text{"the Kokiri Sword", /*french*/"l'épée Kokiri", /*spanish*/"la Espada Kokiri"}
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
                       Text{"the Master Sword", /*french*/"l'épée de légende", /*spanish*/"la Espada Maestra"}
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
                       Text{"the Giant's Knife", /*french*/"la lame des géants", /*spanish*/"la daga gigante"}
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
                       Text{"the Biggoron Sword", /*french*/"l'épée de Grogoron", /*spanish*/"la Espada de Biggoron"}
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
                       Text{"a Deku Shield", /*french*/"un bouclier Mojo", /*spanish*/"un escudo deku"}
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
                       Text{"a Hylian Shield", /*french*/"un bouclier hylien", /*spanish*/"un escudo hyliano"}
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
                       Text{"the Mirror Shield", /*french*/"le bouclier miroir", /*spanish*/"el escudo espejo"}
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
                       Text{"a Goron Tunic", /*french*/"une tunique Goron", /*spanish*/"un sayo goron"}
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
                       Text{"a Zora Tunic", /*french*/"une tunique Zora", /*spanish*/"un sayo zora"}
    );

    hintTable[IRON_BOOTS] = HintText::Item({
                       //obscure text
                       Text{"sink shoes",   /*french*/"un boulet de fer", /*spanish*/"un calzado de las profundidades"},
                       Text{"clank cleats", /*french*/"une paire de talons bruyants", /*spanish*/"unas suelas férreas"},
                     }, {
                       //ambiguous text
                       Text{"some boots", /*french*/"une paire de bottes", /*spanish*/"un par de botas"},
                       Text{"a feature of the Water Temple", /*french*/"une particularité du temple de l'eau", /*spanish*/"algo particular del Templo del Agua"},
                       Text{"something heavy", /*french*/"une chose pesante", /*spanish*/"algo de lo más pesado"},
                     },
                       //clear text
                       Text{"the Iron Boots", /*french*/"une paire de bottes de plomb", /*spanish*/"las botas de hierro"}
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
                       Text{"the Hover Boots", /*french*/"une paire de bottes des airs", /*spanish*/"las botas voladoras"}
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
                       Text{"the Weird Egg", /*french*/"l'œuf curieux", /*spanish*/"el huevo extraño"}
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
                       Text{"the Boomerang", /*french*/"le boomerang", /*spanish*/"el bumerán"}
    );

    hintTable[LENS_OF_TRUTH] = HintText::Item({
                       //obscure text
                       Text{"a lie detector",     /*french*/"un détecteur de mensonges", /*spanish*/"el detector de ilusiones"},
                       Text{"a ghost tracker",    /*french*/"un trouve-fantôme",         /*spanish*/"el rastreador paranormal"},
                       Text{"true sight",         /*french*/"le troisième œil",         /*spanish*/"el ojo que todo ve"},
                       Text{"a detective's tool", /*french*/"un trésor Sheikah",         /*spanish*/"la revelación verdadera"},
                     }, {
                       //ambiguous text
                       Text{"a secret-finding tool", /*french*/"un cherche-secrets", /*spanish*/"un instrumento para hallar objetos"},
                     },
                       //clear text
                       Text{"the Lens of Truth", /*french*/"le monocle de Vérité", /*spanish*/"la Lupa de la Verdad"}
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
                       Text{"the Megaton Hammer", /*french*/"la masse des titans", /*spanish*/"el martillo Megatón"}
    );

    hintTable[SHARD_OF_AGONY] = HintText::Item({
                       //obscure text
                       Text{"the shake shard", /*french*/"le fragment vibrant", /*spanish*/"el fragmento tintineante"},
                       Text{"a blue alarm",    /*french*/"une alerte bleue",    /*spanish*/"una azul alarma"},
                     }, {
                       //ambiguous text
                       Text{"a prize of the House of Skulltulas", /*french*/"un prix de la maison des Skulltulas", /*spanish*/"un obsequio de la Casa Skulltula"},
                       Text{"a secret-finding tool", /*french*/"un cherche-secrets", /*spanish*/"un instrumento para hallar objetos"},
                     },
                       //clear text
                       Text{"the Shard of Agony", /*french*/"le fragment de souffrance", /*spanish*/"la Piedra de la Agonía"}
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
                       Text{"Din's Fire", /*french*/"le feu de Din", /*spanish*/"el Fuego de Din"}
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
                       Text{"Farore's Wind", /*french*/"le vent de Farore", /*spanish*/"el Viento de Farore"}
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
                       Text{"Nayru's Love", /*french*/"l'amour de Nayru", /*spanish*/"el Amor de Nayru"}
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
                       Text{"the Fire Arrows", /*french*/"la flèche de feu", /*spanish*/"la flecha de fuego"}
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
                       Text{"the Ice Arrows", /*french*/"la flèche de glace", /*spanish*/"la flecha de hielo"}
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
                       Text{"the Light Arrows", /*french*/"la flèche de lumière", /*spanish*/"la flecha de luz"}
    );

    hintTable[GERUDO_TOKEN] = HintText::Item({
                       //obscure text
                       Text{"a girl club membership", /*french*/"une carte de membre", /*spanish*/"una fémina membresía"},
                       Text{"a desert tribe's pass",  /*french*/"un laissez-passer",   /*spanish*/"el vale del desierto"},
                     }, {
                       Text{"a token of recognition", /*french*/"une preuve de reconnaissance", /*spanish*/"una prueba de reconocimiento"},
                     },
                       //clear text
                       Text{"the Gerudo Token", /*french*/"la carte Gerudo", /*spanish*/"el pase de socio gerudo"}
    );

    hintTable[MAGIC_BEAN] = HintText::Item({
                       //obscure text
                       Text{"a wizardly legume", /*french*/"un légume ensorcelé", /*spanish*/"una legumbre hechizada"},
                     }, {
                       //ambiguous text
                       Text{"something sometimes buried", /*french*/"une chose parfois enterrée", /*spanish*/"algo a veces enterrado"},
                     },
                       //clear text
                       Text{"a Magic Bean", /*french*/"un haricot magique", /*spanish*/"una judía mágica"}
    );

    hintTable[MAGIC_BEAN_PACK] = HintText::Item({
                       //obscure text
                       Text{"wizardly legumes", /*french*/"un paquet de légumes ensorcelés", /*spanish*/"unas legumbres hechizadas"},
                     }, {
                       //ambiguous text
                       Text{"something sometimes buried", /*french*/"une chose parfois enterrée", /*spanish*/"algo a veces enterrado"},
                     },
                       //clear text
                       Text{"Magic Beans", /*french*/"un paquet de haricots magiques", /*spanish*/"unas judías mágicas"}
    );

    hintTable[DOUBLE_DEFENSE] = HintText::Item({
                       //obscure text
                       Text{"a white outline",   /*french*/"un rebord blanc",               /*spanish*/"un contorno blanco"},
                       Text{"damage decrease",   /*french*/"une protection supplémentaire", /*spanish*/"una reducción de daño"},
                       Text{"strengthened love", /*french*/"un amour coriace",              /*spanish*/"un amor fortalecido"},
                     }, {
                       //ambiguous text
                       Text{"a Great Fairy's power", /*french*/"le pouvoir d'une grande fée", /*spanish*/"el poder de una Gran Hada"},
                       Text{"something heart-shaped", /*french*/"une chose en forme de cœur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"Double Defense", /*french*/"la double défence", /*spanish*/"la doble defensa"}
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
                       Text{"a Gold Skulltula Token", /*french*/"un jeton de Skulltula d'or", /*spanish*/"un símbolo de skulltula dorada"}
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
                       Text{"the Pocket Egg", /*french*/"l'œuf de poche", /*spanish*/"el huevo de bolsillo"}
    );

    hintTable[POCKET_CUCCO] = HintText::Item({
                       //obscure text
                       Text{"a little clucker", /*french*/"un petit glousseur", /*spanish*/"un pollito chiquito"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Pocket Cucco", /*french*/"le Cocotte de poche", /*spanish*/"el cuco de bolsillo"}
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
                       Text{"an Odd Mushroom", /*french*/"un champignon suspect", /*spanish*/"un champiñón extraño"}
    );

    hintTable[ODD_POULTICE] = HintText::Item({
                       //obscure text
                       Text{"Granny's goodies", /*french*/"la confiserie de mamie", /*spanish*/"la especialidad de la abuela"},
                     }, {
                       //ambiguous text
                       Text{"something that contains medicine", /*french*/"une chose médicamenteuse", /*spanish*/"algo que contenga medicina"},
                       Text{"something with a strange smell", /*french*/"une chose qui sent bizarre", /*spanish*/"algo con un olor extraño"},
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"an Odd Poultice", /*french*/"une mixture suspecte", /*spanish*/"una medicina rara"}
    );

    hintTable[POACHERS_SAW] = HintText::Item({
                       //obscure text
                       Text{"a tree killer", /*french*/"un coupeur d'arbres", /*spanish*/"un destructor de árboles"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Poacher's Saw", /*french*/"la scie du chasseur", /*spanish*/"la sierra del furtivo"}
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
                       Text{"the Broken Goron's Sword", /*french*/"l'épée brisée de Goron", /*spanish*/"la espada goron rota"}
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
                       Text{"the Prescription", /*french*/"une ordonnance", /*spanish*/"la receta"}
    );

    hintTable[EYEBALL_FROG] = HintText::Item({
                       //obscure text
                       Text{"a perceiving polliwog", /*french*/"un amphibien", /*spanish*/"un variopinto batracio"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Eyeball Frog", /*french*/"le crapaud-qui-louche", /*spanish*/"la rana de ojos saltones"}
    );

    hintTable[EYEDROPS] = HintText::Item({
                       //obscure text
                       Text{"a vision vial", /*french*/"une solution oculaire", /*spanish*/"un remedio para la vista"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Eyedrops", /*french*/"une phiole de super gouttes", /*spanish*/"las supergotas oculares"}
    );

    hintTable[CLAIM_CHECK] = HintText::Item({
                       //obscure text
                       Text{"a three day wait", /*french*/"un rendez-vous dans trois jours", /*spanish*/"unos tres días de espera"},
                     }, {
                       //ambiguous text
                       Text{"a trade quest item", /*french*/"un objet de quête d'échanges", /*spanish*/"un objeto de una misión secundaria"},
                     },
                       //clear text
                       Text{"the Claim Check", /*french*/"un certificat", /*spanish*/"el recibo"}
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
                       Text{"a Hookshot", /*french*/"un grappin", /*spanish*/"un gancho"}
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
                       Text{"a Strength Upgrade", /*french*/"une amélioration de force", /*spanish*/"un aumento de fuerza"}
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
                       Text{"a Bomb Bag", /*french*/"un sac de bombes", /*spanish*/"un saco de bombas"}
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
                       Text{"a Bow", /*french*/"l'arc des fées", /*spanish*/"un arco de las hadas"}
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
                       Text{"a Slingshot", /*french*/"un lance-pierre", /*spanish*/"una resortera de las hadas"}
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
                       Text{"a Wallet", /*french*/"une bourse", /*spanish*/"una bolsa de rupias"}
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
                       Text{"a Zora Scale", /*french*/"une écaille Zora", /*spanish*/"una escama Zora"}
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
                       Text{"Deku Nut Capacity", /*french*/"une augmentation de noix Mojo", /*spanish*/"un aumento de nueces deku"}
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
                       Text{"a Magic Meter", /*french*/"une jauge de magie", /*spanish*/"un aumento de poder mágico"}
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
                       Text{"Bombchus", /*french*/"un paquet de Bombchus", /*spanish*/"unos bombchus"}
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
                       Text{"the ReDead's bane", /*french*/"le fléau des Remorts", /*spanish*/"la destructora de Redeads"},
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
                        Text{"the Deku Tree Map", /*french*/"la carte de l'arbre Mojo", /*spanish*/"el mapa del Gran Árbol Deku"}
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
                        Text{"the Dodongo's Cavern Map", /*french*/"la carte de la caverne Dodongo", /*spanish*/"el mapa de la Cueva de los Dodongos"}
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
                        Text{"the Forest Temple Map", /*french*/"la carte du temple de la forêt", /*spanish*/"el mapa del Templo del Bosque"}
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
                        Text{"the Fire Temple Map", /*french*/"la carte du temple du feu", /*spanish*/"el mapa del Templo del Fuego"}
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
                        Text{"the Water Temple Map", /*french*/"la carte du temple de l'eau", /*spanish*/"el mapa del Templo del Agua"}
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
                        Text{"the Spirit Temple Map", /*french*/"la carte du temple de l'esprit", /*spanish*/"el mapa del Templo del Espíritu"}
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
                        Text{"the Shadow Temple Map", /*french*/"la carte du temple de l'ombre", /*spanish*/"el mapa del Templo de las Sombras"}
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
                        Text{"the Bottom of the Well Map", /*french*/"la carte du fond du puits", /*spanish*/"el mapa del Fondo del pozo"}
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
                        Text{"the Ice Cavern Map", /*french*/"la carte de la caverne polaire", /*spanish*/"el mapa de la Caverna de hielo"}
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
                        Text{"the Deku Tree Compass", /*french*/"la boussole de l'arbre Mojo", /*spanish*/"la brújula del Gran Árbol Deku"}
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
                        Text{"the Dodongo's Cavern Compass", /*french*/"la boussole de la caverne Dodongo", /*spanish*/"la brújula de la Cueva de los Dodongos"}
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
                        Text{"the Forest Temple Compass", /*french*/"la boussole du temple de la forêt", /*spanish*/"la brújula del Templo del Bosque"}
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
                        Text{"the Fire Temple Compass", /*french*/"la boussole du temple du feu", /*spanish*/"la brújula del Templo del Fuego"}
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
                        Text{"the Water Temple Compass", /*french*/"la boussole du temple de l'eau", /*spanish*/"la brújula del Templo del Agua"}
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
                        Text{"the Spirit Temple Compass", /*french*/"la boussole du temple de l'esprit", /*spanish*/"la brújula del Templo del Espíritu"}
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
                        Text{"the Shadow Temple Compass", /*french*/"la boussole du temple de l'ombre", /*spanish*/"la brújula del Templo de las Sombras"}
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
                        Text{"the Bottom of the Well Compass", /*french*/"la boussole du fond du puits", /*spanish*/"la brújula del Fondo del pozo"}
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
                        Text{"the Ice Cavern Compass", /*french*/"la boussole de la caverne polaire", /*spanish*/"la brújula de la Caverna de hielo"}
     );
    hintTable[FOREST_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sylvan master of unlocking", /*french*/"un anti-grosse porte sylvestre",  /*spanish*/"la clave enselvada de un jefe"},
                        Text{"a sylvan dungeon's master pass",      /*french*/"une clé maléfique sylvestree",             /*spanish*/"el pase maestro enselvado"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Forest Temple Boss Key", /*french*/"la clé d'or du temple de la forêt", /*spanish*/"la gran llave del Templo del Bosque"}
     );
    hintTable[FIRE_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a molten master of unlocking", /*french*/"un anti-grosse porte fondu",  /*spanish*/"la clave fundido de un jefe"},
                        Text{"a molten dungeon's master pass",      /*french*/"une clé maléfique fondu",             /*spanish*/"el pase maestro fundido"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Fire Temple Boss Key", /*french*/"la clé d'or du temple du feu", /*spanish*/"la gran llave del Templo del Fuego"}
     );
    hintTable[WATER_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a wet master of unlocking", /*french*/"un anti-grosse porte humide",  /*spanish*/"la clave mojado de un jefe"},
                        Text{"a wet dungeon's master pass",      /*french*/"une clé maléfique humide",             /*spanish*/"el pase maestro mojado"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Water Temple Boss Key", /*french*/"la clé d'or du temple de l'eau", /*spanish*/"la gran llave del Templo del Agua"}
     );
    hintTable[SPIRIT_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a sandy master of unlocking", /*french*/"un anti-grosse porte sableux",  /*spanish*/"la clave arenoso de un jefe"},
                        Text{"a sandy dungeon's master pass",      /*french*/"une clé maléfique sableux",             /*spanish*/"el pase maestro arenoso"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Spirit Temple Boss Key", /*french*/"la clé d'or du temple de l'esprit", /*spanish*/"la gran llave del Templo del Espíritu"}
     );
    hintTable[SHADOW_TEMPLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a creepy master of unlocking", /*french*/"un anti-grosse porte sinistre",  /*spanish*/"la clave siniestra de un jefe"},
                        Text{"a creepy dungeon's master pass",      /*french*/"une clé maléfique sinistre",             /*spanish*/"el pase maestro siniestra"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Shadow Temple Boss Key", /*french*/"la clé d'or du temple de l'ombre", /*spanish*/"la gran llave del Templo de las Sombras"}
     );
    hintTable[GANONS_CASTLE_BOSS_KEY] = HintText::Item({
                        //obscure text
                        Text{"a final master of unlocking", /*french*/"un anti-grosse porte final",  /*spanish*/"la clave final de un jefe"},
                        Text{"a final dungeon's master pass",      /*french*/"une clé maléfique final",             /*spanish*/"el pase maestro final"},
                      }, {
                        //ambiguous text
                        Text{"a boss key", /*french*/"une clé d'or", /*spanish*/"una gran llave"},
                      },
                        //clear text
                        Text{"the Ganon's Castle Boss Key", /*french*/"la clé d'or du château de Ganon", /*spanish*/"la gran llave del Castillo de Ganon"}
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
                        Text{"a Forest Temple Small Key", /*french*/"une petite clé du temple de la forêt", /*spanish*/"una llave pequeña del Templo del Bosque"}
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
                        Text{"a Fire Temple Small Key", /*french*/"une petite clé du temple du feu", /*spanish*/"una llave pequeña del Templo del Fuego"}
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
                        Text{"a Water Temple Small Key", /*french*/"une petite clé du temple de l'eau", /*spanish*/"una llave pequeña del Templo del Agua"}
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
                        Text{"a Spirit Temple Small Key", /*french*/"une petite clé du temple de l'esprit", /*spanish*/"una llave pequeña del Templo del Espíritu"}
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
                        Text{"a Shadow Temple Small Key", /*french*/"une petite clé du temple de l'ombre", /*spanish*/"una llave pequeña del Templo de las Sombras"}
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
                        Text{"a Gerudo Training Ground Small Key", /*french*/"une petite clé du gymnase Gerudo", /*spanish*/"una llave pequeña del Centro de Instrucción Gerudo"}
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
                        Text{"a Gerudo Fortress Small Key", /*french*/"une petite clé de la forteresse Gerudo", /*spanish*/"una llave pequeña de la Fortaleza Gerudo"}
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
                        Text{"a Bottom of the Well Small Key", /*french*/"une petite clé du fond du puits", /*spanish*/"una llave pequeña del Fondo del pozo"}
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
                        Text{"a Ganon's Castle Small Key", /*french*/"une petite clé du château de Ganon", /*spanish*/"una llave pequeña del Castillo de Ganon"}
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
                        Text{"a Forest Temple Key Ring", /*french*/"un trousseau de clés du temple de la forêt", /*spanish*/"un llavero del Templo del Bosque"}
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
                        Text{"a Fire Temple Key Ring", /*french*/"un trousseau de clés du temple du feu", /*spanish*/"un llavero del Templo del Fuego"}
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
                        Text{"a Water Temple Key Ring", /*french*/"un trousseau de clés du temple de l'eau", /*spanish*/"un llavero del Templo del Agua"}
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
                        Text{"a Spirit Temple Key Ring", /*french*/"un trousseau de clés du temple de l'esprit", /*spanish*/"un llavero del Templo del Espíritu"}
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
                        Text{"a Shadow Temple Key Ring", /*french*/"un trousseau de clés du temple de l'ombre", /*spanish*/"un llavero del Templo de las Sombras"}
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
                        Text{"a Gerudo Training Ground Key Ring", /*french*/"un trousseau de clés du gymnase Gerudo", /*spanish*/"un llavero del Centro de Instrucción Gerudo"}
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
                        Text{"a Gerudo Fortress Key Ring", /*french*/"un trousseau de clés de la forteresse Gerudo", /*spanish*/"un llavero de la Fortaleza Gerudo"}
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
                        Text{"a Bottom of the Well Key Ring", /*french*/"un trousseau de clés du fond du puits", /*spanish*/"un llavero del Fondo del pozo"}
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
                        Text{"a Ganon's Castle Key Ring", /*french*/"un trousseau de clés du château de Ganon", /*spanish*/"un llavero del Castillo de Ganon"}
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
                       Text{"a spiritual stone", /*french*/"une pierre spirituelle", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Kokiri Emerald", /*french*/"l'émeraude Kokiri", /*spanish*/"la Esmeralda de los Kokiri"}
    );

    hintTable[GORON_RUBY] = HintText::Item({
                       //obscure text
                       Text{"a red stone",       /*french*/"une pierre rouge",         /*spanish*/"una piedra carmín"},
                       Text{"sworn brotherhood", /*french*/"un serment de fraternité", /*spanish*/"el juramento de hermanos de sangre"},
                     }, {
                       //ambiguous text
                       Text{"a spiritual stone", /*french*/"une pierre spirituelle", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Goron Ruby", /*french*/"le rubis Goron", /*spanish*/"el Rubí de los Goron"}
    );

    hintTable[ZORA_SAPPHIRE] = HintText::Item({
                       //obscure text
                       Text{"a blue stone",       /*french*/"une pierre bleue",     /*spanish*/"una piedra celeste"},
                       Text{"an engagement gift", /*french*/"un cadeau de mariage", /*spanish*/"un regalo de compromiso"},
                     }, {
                       //ambiguous text
                       Text{"a spiritual stone", /*french*/"une pierre spirituelle", /*spanish*/"una piedra espiritual"},
                     },
                       //clear text
                       Text{"the Zora Sapphire", /*french*/"le saphir Zora", /*spanish*/"el Zafiro de los Zora"}
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
                       Text{"the Forest Medallion", /*french*/"le médaillon de la forêt", /*spanish*/"el Medallón del Bosque"}
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
                       Text{"the Fire Medallion", /*french*/"le médaillon du feu", /*spanish*/"el Medallón del Fuego"}
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
                       Text{"the Water Medallion", /*french*/"le médaillon de l'eau", /*spanish*/"el Medallón del Agua"}
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
                       Text{"the Spirit Medallion", /*french*/"le médaillon de l'esprit", /*spanish*/"el Medallón del Espíritu"}
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
                       Text{"the Shadow Medallion", /*french*/"le médaillon de l'ombre", /*spanish*/"el Medallón de la Sombra"}
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
                       Text{"the Light Medallion", /*french*/"le médaillon de la lumière", /*spanish*/"el Medallón de la Luz"}
    );

    hintTable[RECOVERY_HEART] = HintText::Item({
                       //obscure text
                       Text{"a free heal",   /*french*/"un bec-au-bobo", /*spanish*/"una cura de regalo"},
                       Text{"a hearty meal", /*french*/"un petit amour",   /*spanish*/"una sanación romántica"},
                       Text{"a Band-Aid",    /*french*/"un diachylon",     /*spanish*/"un corazoncito sanador"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de cœur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Recovery Heart", /*french*/"un cœur de vie", /*spanish*/"un corazón"}
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
                       Text{"a broken heart", /*french*/"un cœur brisé",       /*spanish*/"un corazón roto"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de cœur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Piece of Heart", /*french*/"un quart de cœur", /*spanish*/"una pieza de corazón"}
    );

    hintTable[HEART_CONTAINER] = HintText::Item({
                       //obscure text
                       Text{"a lot of love",      /*french*/"le grand amour",             /*spanish*/"amor por doquier"},
                       Text{"a Valentine's gift", /*french*/"un cadeau de Saint-Valentin", /*spanish*/"un contenedor de afección"},
                       Text{"a boss's organ",     /*french*/"un organe de monstre",        /*spanish*/"los órganos de un jefe"},
                     }, {
                       //ambiguous text
                       Text{"something heart-shaped", /*french*/"une chose en forme de cœur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Heart Container", /*french*/"un cœur d'énergie", /*spanish*/"un contenedor de corazón"}
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
                       Text{"a spiritual stone", /*french*/"une pierre spirituelle", /*spanish*/"una piedra espiritual"},
                       Text{"something that can stun", /*french*/"une chose qui peut paralyser", /*spanish*/"algo que pueda paralizar"},
                     },
                       //clear text
                       Text{"an Ice Trap", /*french*/"un piège de glace", /*spanish*/"una trampa de hielo"}
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
                       Text{"Bombchus (5 pieces)", /*french*/"une demi-dizaine de Bombchus", /*spanish*/"unos (5) bombchus"}
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
                       Text{"Bombchus (10 pieces)", /*french*/"une dizaine de Bombchus", /*spanish*/"unos (10) bombchus"}
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
                       Text{"Bombchus (20 pieces)", /*french*/"une vingtaine de Bombchus", /*spanish*/"unos (20) bombchus"}
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
                       Text{"something heart-shaped", /*french*/"une chose en forme de cœur", /*spanish*/"algo con forma de corazón"},
                     },
                       //clear text
                       Text{"a Piece of Heart", /*french*/"un quart de cœur", /*spanish*/"el amor de la victoria"}
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

    /*--------------------------
    |     ALWAYS HINT TEXT     |
    ---------------------------*/

    hintTable[ZR_FROGS_OCARINA_GAME] = HintText::Always({
                       //obscure text
                       Text{"an #amphibian feast# yields",               /*french*/"un #festin d'amphibiens# donne", /*spanish*/"una #fiesta anfibia# brinda"},
                       Text{"the #croaking choir's magnum opus# awards", /*french*/"la #chorale coassante# donne",  /*spanish*/"un #coro maestro de ancas# premia"},
                       Text{"the #froggy finale# yields",                /*french*/"la #finale amphibienne# donne", /*spanish*/"el #gran final batracio# brinda"},
                     }, {},
                       //clear text
                       Text{"the final reward from the #Frogs of Zora's River# is", /*french*/"la dernière récompense des #grenouilles du fleuve zora# est", /*spanish*/"la recompensa final de las #ranas del Río Zora# premia"}
  );

    hintTable[KF_LINKS_HOUSE_COW] = HintText::Always({
                       //obscure text
                       Text{"the #bovine bounty of a horseback hustle# gifts", /*french*/"le cadeau #qui découle d'une réussite équestre# est", /*spanish*/"la #recompensa bovina de un paseo a caballo# brinda"},
                     }, {},
                       //clear text
                       Text{"#Malon's obstacle course# leads to", /*french*/"la #course à obstacle de Malon# amène à", /*spanish*/"la #carrera de obstáculos de Malon# brinda"}
  );

    /*--------------------------
    |    SOMETIMES HINT TEXT   |
    ---------------------------*/

    hintTable[SONG_FROM_OCARINA_OF_TIME] = HintText::Sometimes({
                       //obscure text
                       Text{"the #Ocarina of Time# teaches", /*french*/"l'#ocarina du temps# est accompagné par", /*spanish*/"la #Ocarina del Tiempo# enseña"},
  });

    hintTable[SONG_FROM_COMPOSERS_GRAVE] = HintText::Sometimes({
                       //obscure text
                       Text{"#ReDead in the Composers' Grave# guard", /*french*/"les #Remorts du tombeau des compositeurs# protègent", /*spanish*/"los #ReDeads del Panteón Real# guardan"},
                       Text{"the #Composer Brothers wrote#",          /*french*/"le #trésor des compositeurs# est",                    /*spanish*/"los #hermanos compositores escribieron#"},
  });

    hintTable[SHEIK_IN_FOREST] = HintText::Sometimes({
                       //obscure text
                       Text{"#in a meadow# Sheik teaches", /*french*/"Sheik confiera, #dans une clairière#,", /*spanish*/"#en la pradera sagrada# Sheik enseña"},
  });

    hintTable[SHEIK_AT_TEMPLE] = HintText::Sometimes({
                       //obscure text
                       Text{"Sheik waits at a #monument to time# to teach", /*french*/"Sheik confiera, #au pied de l'épée légendaire#,", /*spanish*/"Sheik espera en el #momumento del tiempo# para enseñar"},
  });

    hintTable[SHEIK_IN_CRATER] = HintText::Sometimes({
                       //obscure text
                       Text{"the #crater's melody# is", /*french*/"Sheik confiera, #entouré de lave#,", /*spanish*/"la #melodía del cráter# otorga"},
  });

    hintTable[SHEIK_IN_ICE_CAVERN] = HintText::Sometimes({
                       //obscure text
                       Text{"the #frozen cavern# echoes with", /*french*/"Sheik confiera, #dans une caverne étoilée#,", /*spanish*/"en la #caverna de hielo# retumban los ecos de"},
  });

    hintTable[SHEIK_IN_KAKARIKO] = HintText::Sometimes({
                       //obscure text
                       Text{"a #ravaged village# mourns with", /*french*/"Sheik confirera, #au cœur d'un village ravagé#,", /*spanish*/"un #arrasado pueblo# llora"},
  });

    hintTable[SHEIK_AT_COLOSSUS] = HintText::Sometimes({
                       //obscure text
                       Text{"a hero ventures #beyond the wasteland# to learn", /*french*/"Sheik confiera, #au bout d'un chemin sableux#,", /*spanish*/"el héroe que se adentre #más allá del desierto# aprenderá"},
  });


    hintTable[MARKET_10_BIG_POES] = HintText::Sometimes({
                       //obscure text
                       Text{"#ghost hunters# will be rewarded with", /*french*/"#les chasseurs de fantômes# sont récompensés avec", /*spanish*/"los #cazafantasmas# son premiados con"},
                     }, {},
                       //clear text
                       Text{"catching #Big Poes# leads to", /*french*/"#d'attraper des fantômes# donne", /*spanish*/"hacerte con #Grandes Poes# conduce a"}
  );

    hintTable[DEKU_THEATER_SKULL_MASK] = HintText::Sometimes({
                       //obscure text
                       Text{"the #Skull Mask# yields", /*french*/"le #masque de mort# donne", /*spanish*/"la #máscara de calavera# otorga"},
  });

    hintTable[DEKU_THEATER_MASK_OF_TRUTH] = HintText::Sometimes({
                       //obscure text
                       Text{"showing a #truthful eye to the crowd# rewards", /*french*/"montrer #l'oeil de vérité à la foule# donne", /*spanish*/"#mostrarle el ojo verdadero# a una multitud brinda"},
                     }, {},
                       //clear text
                       Text{"the #Mask of Truth# yields", /*french*/"le #masque de vérité# donne", /*spanish*/"la #máscara de la verdad# premia"}
  );

    hintTable[HF_OCARINA_OF_TIME_ITEM] = HintText::Sometimes({
                       //obscure text
                       Text{"the #treasure thrown by Princess Zelda# is", /*french*/"le trésor #laissé par la princesse# est", /*spanish*/"el #tesoro arrojado por la Princesa Zelda# se trata de"},
  });

    hintTable[DMT_TRADE_BROKEN_SWORD] = HintText::Sometimes({
                       //obscure text
                       Text{"a #blinded Biggoron# entrusts", /*french*/"un #Grogoron aveuglé# confie", /*spanish*/"un #miope Biggoron# otorga"},
  });

    hintTable[DMT_TRADE_EYEDROPS] = HintText::Sometimes({
                       //obscure text
                       Text{"while you wait, #Biggoron# gives", /*french*/"pendant que tu attends, #Grogoron# donne", /*spanish*/"#Biggoron# está a la espera de otorgar"},
  });

    hintTable[DMT_TRADE_CLAIM_CHECK] = HintText::Sometimes({
                       //obscure text
                       Text{"#Biggoron# crafts", /*french*/"#Grogoron# fabrique", /*spanish*/"#Biggoron# forja"},
  });

    hintTable[KAK_50_GOLD_SKULLTULA_REWARD] = HintText::Sometimes({
                       //obscure text
                       Text{"#50 bug badges# rewards",           /*french*/"#50 écussons# donnent",             /*spanish*/"#50 medallas de insectos# otorgan"},
                       Text{"#50 spider souls# yields",          /*french*/"#50 âmes# donnent",                 /*spanish*/"#50 almas de araña# otorgan"},
                       Text{"#50 auriferous arachnids# lead to", /*french*/"#50 arachnides aurifères# donnent", /*spanish*/"#50 arácnidos auríferos# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #50 Gold Skulltulas# reveals", /*french*/"détruire #50 Skulltulas d'or# donne", /*spanish*/"exterminar #50 skulltulas doradas# revela"}
  );

    hintTable[KAK_40_GOLD_SKULLTULA_REWARD] = HintText::Sometimes({
                       //obscure text
                       Text{"#40 bug badges# rewards",           /*french*/"#40 écussons# donnent",             /*spanish*/"#40 medallas de insectos# otorgan"},
                       Text{"#40 spider souls# yields",          /*french*/"#40 âmes# donnent",                 /*spanish*/"#40 almas de araña# otorgan"},
                       Text{"#40 auriferous arachnids# lead to", /*french*/"#40 arachnides aurifères# donnent", /*spanish*/"#40 arácnidos auríferos# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #40 Gold Skulltulas# reveals", /*french*/"détruire #40 Skulltulas d'or# donne", /*spanish*/"exterminar #40 skulltulas doradas# revela"}
  );

    hintTable[KAK_30_GOLD_SKULLTULA_REWARD] = HintText::Sometimes({
                       //obscure text
                       Text{"#30 bug badges# rewards",           /*french*/"#30 écussons# donnent",             /*spanish*/"#30 medallas de insectos# otorgan"},
                       Text{"#30 spider souls# yields",          /*french*/"#30 âmes# donnent",                 /*spanish*/"#30 almas de araña# otorgan"},
                       Text{"#30 auriferous arachnids# lead to", /*french*/"#30 arachnides aurifères# donnent", /*spanish*/"#30 arácnidos auríferos# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #30 Gold Skulltulas# reveals", /*french*/"détruire #30 Skulltulas d'or# donne", /*spanish*/"exterminar #30 skulltulas doradas# revela"}
  );

    hintTable[KAK_20_GOLD_SKULLTULA_REWARD] = HintText::Sometimes({
                       //obscure text
                       Text{"#20 bug badges# rewards",           /*french*/"#20 écussons# donnent",             /*spanish*/"#20 medallas de insectos# otorgan"},
                       Text{"#20 spider souls# yields",          /*french*/"#20 âmes# donnent",                 /*spanish*/"#20 almas de araña# otorgan"},
                       Text{"#20 auriferous arachnids# lead to", /*french*/"#20 arachnides aurifères# donnent", /*spanish*/"#20 arácnidos auríferos# otorgan"},
                     }, {},
                       //clear text
                       Text{"slaying #20 Gold Skulltulas# reveals", /*french*/"détruire #20 Skulltulas d'or# donne", /*spanish*/"exterminar #20 skulltulas doradas# revela"}
  );

    hintTable[KAK_ANJU_AS_CHILD] = HintText::Sometimes({
                       //obscure text
                       Text{"#wrangling roosters# rewards", /*french*/"#plumer des poulets# donne", /*spanish*/"#atrapar a las gallinas# premia"},
                       Text{"#chucking chickens# gifts",    /*french*/"#lancer des poulets# donne", /*spanish*/"#reunir a unos emplumados# premia"},
                     }, {},
                       //clear text
                       Text{"#collecting cuccos# rewards", /*french*/"#rapporter les Cocottes# donne", /*spanish*/"#hacerte con todos los cucos# premia"}
  );

    hintTable[KAK_TRADE_POCKET_CUCCO] = HintText::Sometimes({
                       //obscure text
                       Text{"an adult's #happy Cucco# awards", /*french*/"un adulte avec une #poulette joyeuse# obtient", /*spanish*/"un #alegre cuco# en la madurez otorga"},
  });

    hintTable[KAK_TRADE_ODD_MUSHROOM] = HintText::Sometimes({
                       //obscure text
                       Text{"the #potion shop lady# entrusts", /*french*/"la #gribiche du magasin de potion# confie", /*spanish*/"la #señora de la tienda de pociones# otorga"},
  });

    hintTable[GC_DARUNIAS_JOY] = HintText::Sometimes({
                       //obscure text
                       Text{"a #groovin' goron# gifts", /*french*/"#le Goron joyeux# donne", /*spanish*/"#un goron marchoso# otorga"},
                     }, {},
                       //clear text
                       Text{"#Darunia's dance# leads to", /*french*/"#la dance de Darunia# donne", /*spanish*/"#el baile de Darunia# conduce a"}
  );

    hintTable[LW_SKULL_KID] = HintText::Sometimes({
                       //obscure text
                       Text{"the #Skull Kid# grants", /*french*/"le #Skull Kid# donne", /*spanish*/"#Skull Kid# otorga"},
  });

    hintTable[LW_TRADE_COJIRO] = HintText::Sometimes({
                       //obscure text
                       Text{"returning a #special Cucco# awards", /*french*/"ramener une #poulette précieuse# donne", /*spanish*/"quien devuelva un #cuco especial# encontrará"},
  });

    hintTable[LW_TRADE_ODD_POULTICE] = HintText::Sometimes({
                       //obscure text
                       Text{"a #Kokiri girl in the woods# leaves", /*french*/"la #fillette Kokiri dans les bois# laisse", /*spanish*/"una #chica kokiri del bosque# otorga"},
  });

    hintTable[LH_SUN] = HintText::Sometimes({
                       //obscure text
                       Text{"staring into #the sun# grants", /*french*/"regarder #le soleil# donne", /*spanish*/"#mirar al sol# revela"},
                     }, {},
                       //clear text
                       Text{"shooting #the sun# grants", /*french*/"tirer une flèche dans #sur le soleil# donne", /*spanish*/"#disparar al sol# revela"}
  );

    hintTable[LH_TRADE_FROG] = HintText::Sometimes({
                       //obscure text
                       Text{"#Lake Hylia's scientist# hurriedly entrusts", /*french*/"le #scientifique du lac# confie rapidement", /*spanish*/"el #científico del Lago Hylia# otorga con prisa"},
  });

    hintTable[MARKET_TREASURE_CHEST_GAME_REWARD] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling# grants",               /*french*/"#parier# donne",                     /*spanish*/"#los juegos de azar# revelan"},
                       Text{"there is a #1/32 chance# to win", /*french*/"être #le gagnant parmi 32# donne", /*spanish*/"hay una #probabilidad de 1/32# de ganar"},
                     }, {},
                       //clear text
                       Text{"the #treasure chest game# grants", /*french*/"la #chasse aux trésors# donne", /*spanish*/"#el Cofre del Tesoro# premia"}
  );

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_1] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling once# grants",                    /*french*/"#parier une fois# donne",                        /*spanish*/"#apostar solo una vez# revelará"},
                       Text{"the #first or second game chest# contains", /*french*/"le #premier ou deuxième coffre à jeu# contient", /*spanish*/"#el primer o segundo cofre del azar# revela"},
                     }, {},
                       //clear text
                       Text{"the #first locked room# in the chest game contains", /*french*/"la #première salle# de la chasse aux trésors contient", /*spanish*/"#en la primera sala del Cofre del Tesoro# aguarda"}
  );

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_2] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling twice# grants",                   /*french*/"#parier deux fois# donne",                          /*spanish*/"#apostar dos veces# revelará"},
                       Text{"the #third or fourth game chest# contains", /*french*/"le #troisième ou quatrième coffre à jeu# contient", /*spanish*/"#el tercer o cuarto cofre del azar# revela"},
                     }, {},
                       //clear text
                       Text{"the #second locked room# in the chest game contains", /*french*/"la #deuxième salle# de la chasse aux trésors contient", /*spanish*/"#en la segunda sala del Cofre del Tesoro# aguarda"}
  );

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_3] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling 3 times# grants",                /*french*/"#parier trois fois# donne",                       /*spanish*/"#apostar tres veces# revelará"},
                       Text{"the #fifth or sixth game chest# contains", /*french*/"le #cinquième ou sixième coffre à jeu# contient", /*spanish*/"#el quinto o sexto cofre del azar# revela"},
                     }, {},
                       //clear text
                       Text{"the #third locked room# in the chest game contains", /*french*/"la #troisième salle# de la chasse aux trésors contient", /*spanish*/"#en la tercera sala del Cofre del Tesoro# aguarda"}
  );

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_4] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling 4 times# grants",                   /*french*/"#parier quatre fois# donne",                      /*spanish*/"#apostar cuatro veces# revelará"},
                       Text{"the #seventh or eighth game chest# contains", /*french*/"le #septième ou huitième coffre à jeu# contient", /*spanish*/"#el séptimo u octavo cofre del azar# revela"},
                     }, {},
                       //clear text
                       Text{"the #fourth locked room# in the chest game contains", /*french*/"la #quatrième salle# de la chasse aux trésors contient", /*spanish*/"#en la cuarta sala del Cofre del Tesoro# aguarda"}
  );

    hintTable[MARKET_TREASURE_CHEST_GAME_ITEM_5] = HintText::Sometimes({
                       //obscure text
                       Text{"#gambling 5 times# grants",                /*french*/"#parier cinq fois# donne",                       /*spanish*/"#apostar cinco veces# revelará"},
                       Text{"the #ninth or tenth game chest# contains", /*french*/"le #neuvième ou dixième coffre à jeu# contient", /*spanish*/"#el noveno o décimo cofre del azar# revela"},
                     }, {},
                       //clear text
                       Text{"the #fifth locked room# in the chest game contains", /*french*/"la #cinquième salle# de la chasse aux trésors contient", /*spanish*/"#en la quinta sala del Cofre del Tesoro# aguarda"}
  );

    hintTable[GF_HBA_1500_POINTS] = HintText::Sometimes({
                       //obscure text
                       Text{"mastery of #horseback archery# grants", /*french*/"maîtriser l'#archerie équestre# donne", /*spanish*/"dominar el #tiro con arco a caballo# premia con"},
                     }, {},
                       //clear text
                       Text{"scoring 1500 in #horseback archery# grants", /*french*/"obtenir 1500 points dans l'#archerie équestre# donne", /*spanish*/"conseguir 1500 puntos en el #tiro con arco a caballo# premia"}
  );

    hintTable[GRAVEYARD_HEART_PIECE_GRAVE_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"playing #Sun's Song# in a grave spawns", /*french*/"jouer le #chant du soleil# dans un tombeau donne", /*spanish*/"#tocar la Canción del Sol# en una cripta conduce a"},
  });

    hintTable[GC_MAZE_LEFT_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"in #Goron City# the hammer unlocks", /*french*/"dans le #village Goron#, le marteau donne accès à", /*spanish*/"en la #Ciudad Goron# el martillo desbloquea"},
  });

    hintTable[GV_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"in #Gerudo Valley# the hammer unlocks", /*french*/"dans la #vallée Gerudo#, le marteau donne accès à", /*spanish*/"en el #Valle Gerudo# el martillo desbloquea"},
  });

    hintTable[GV_TRADE_SAW] = HintText::Sometimes({
                       //obscure text
                       Text{"the #boss of the carpenters# leaves", /*french*/"le #patron des ouvriers# laisse", /*spanish*/"el #capataz de los carpinteros# otorga"},
  });

    hintTable[GV_COW] = HintText::Sometimes({
                       //obscure text
                       Text{"a #cow in Gerudo Valley# gifts", /*french*/"la #vache de la vallée Gerudo# donne", /*spanish*/"una #vaca del Valle Gerudo# brinda"},
  });

    hintTable[HC_GS_STORMS_GROTTO] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider behind a muddy wall# in a grotto holds", /*french*/"l'#araignée derrière un mur de boue# dans une grotte donne", /*spanish*/"una #Skulltula tras la agrietada pared# de una cueva otorga"},
  });

    hintTable[HF_GS_COW_GROTTO] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider behind webs# in a grotto holds", /*french*/"l'#araignée derrière une toile# dans une grotte donne", /*spanish*/"una #Skulltula tras la telaraña# de una cueva otorga"},
  });

    hintTable[HF_COW_GROTTO_COW] = HintText::Sometimes({
                       //obscure text
                       Text{"the #cobwebbed cow# gifts", /*french*/"la #vache prisonnière d'araignées# donne", /*spanish*/"una #vaca tras una telaraña# brinda"},
                     }, {},
                       //clear text
                       Text{"a #cow behind webs# in a grotto gifts", /*french*/"la #vache derrière les toiles# d'une grotte donne", /*spanish*/"una #vaca tras la telaraña# de una cueva brinda"}
  );

    hintTable[ZF_GS_HIDDEN_CAVE] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider high #above the icy waters# holds", /*french*/"l'araignée #en haut des eaux glacées# donne", /*spanish*/"una Skulltula en lo #alto de las congeladas aguas# otorga"},
  });

    hintTable[WASTELAND_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"#deep in the wasteland# is",         /*french*/"#loin dans le désert# gît",                     /*spanish*/"en lo #profundo del desierto encantado# yace"},
                       Text{"beneath #the sands#, flames reveal", /*french*/"#sous le désert#, les flammes font apparaître", /*spanish*/"tras las #arenas# unas llamas revelan"},
  });

    hintTable[WASTELAND_GS] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider in the wasteland# holds", /*french*/"#l'araignée dans le désert# donne", /*spanish*/"una #Skulltula del desierto encantado# otorga"},
  });

    hintTable[GRAVEYARD_COMPOSERS_GRAVE_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"#flames in the Composers' Grave# reveal", /*french*/"#les flammes dans le tombeau des compositeurs# cachent", /*spanish*/"#las llamas del Panteón Real# revelan"},
                       Text{"the #Composer Brothers hid#",             /*french*/"#les Frères compositeurs on caché#",                     /*spanish*/"los #hermanos compositores esconden#"},
  });

    hintTable[ZF_BOTTOM_FREESTANDING_POH] = HintText::Sometimes({
                       //obscure text
                       Text{"#under the icy waters# lies", /*french*/"#sous les eaux glacées# se cache", /*spanish*/"#bajo las congeladas aguas# yace"},
  });

    hintTable[GC_POT_FREESTANDING_POH] = HintText::Sometimes({
                       //obscure text
                       Text{"spinning #Goron pottery# contains", /*french*/"la #potterie Goron# contient", /*spanish*/"una #cerámica goron# contiene"},
  });

    hintTable[ZD_KING_ZORA_THAWED] = HintText::Sometimes({
                       //obscure text
                       Text{"a #defrosted dignitary# gifts", /*french*/"le #monarque libéré# donne", /*spanish*/"una #liberación monárquica# brinda"},
                     }, {},
                       //clear text
                       Text{"unfreezing #King Zora# grants", /*french*/"dégeler # le Roi Zora# donne", /*spanish*/"#descongelar al Rey Zora# conduce a"}
  );

    hintTable[ZD_TRADE_PRESCRIPTION] = HintText::Sometimes({
                       //obscure text
                       Text{"#King Zora# hurriedly entrusts", /*french*/"le #roi Zora# confie rapidement", /*spanish*/"el #Rey Zora# otorga con prisa"},
  });

    hintTable[DMC_DEKU_SCRUB] = HintText::Sometimes({
                       //obscure text
                       Text{"a single #scrub in the crater# sells", /*french*/"la #peste Mojo dans le cratère# vend", /*spanish*/"un solitario #deku del cráter# vende"},
  });

    hintTable[DMC_GS_CRATE] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider under a #crate in the crater# holds", /*french*/"la Skulltula dans une #boîte volcanique# a", /*spanish*/"una Skulltula bajo una #caja del cráter# otorga"},
  });


    hintTable[DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"a #temporal stone within a tree# contains", /*french*/"la #pierre bleue dans un arbre# mène à", /*spanish*/"un #bloque temporal de un árbol# contiene"},
                     }, {},
                       //clear text
                       Text{"a #temporal stone within the Deku Tree# contains", /*french*/"la #pierre temporelle dans l'arbre Mojo# cache", /*spanish*/"un #bloque temporal del Gran Árbol Deku# contiene"}
  );

    hintTable[DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider on a ceiling in a tree# holds", /*french*/"l'#araignée haut-perchée dans un arbre# a", /*spanish*/"una #Skulltula en el techo de un árbol# otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider on a ceiling in the Deku Tree# holds", /*french*/"la#Skulltula dans le cimetière de l'arbre Mojo# a", /*spanish*/"una #Skulltula en el techo del Gran Árbol Deku# otorga"}
  );

    hintTable[DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider under #temporal stones in a cavern# holds", /*french*/"l'araignée sous #une pierre bleue dans une caverne# a", /*spanish*/"una Skulltula bajo #bloques temporales de una cueva# otorga"},
                     }, {},
                       //clear text
                       Text{"a spider under #temporal stones in Dodongo's Cavern# holds", /*french*/"la Skulltula sous #la pierre temporelle dans la caverne Dodongo# a", /*spanish*/"una Skulltula bajo #bloques temporales de la Cueva de los Dodongos# otorga"}
  );

    hintTable[JABU_JABUS_BELLY_BOOMERANG_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"a school of #stingers swallowed by a deity# guard", /*french*/"les #raies dans un gardien# protègent", /*spanish*/"unos de #stingers engullidos por cierta deidad# guardan"},
                     }, {},
                       //clear text
                       Text{"a school of #stingers swallowed by Jabu-Jabu# guard", /*french*/"les #raies dans Jabu-Jabu# protègent", /*spanish*/"unos #stingers engullidos por Jabu-Jabu# guardan"}
  );

    hintTable[JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider surrounded by #shadows in the belly of a deity# holds", /*french*/"l'araignée entourée d'#ombres dans le ventre du gardien# possède", /*spanish*/"una Skulltula rodeada de #sombras en la tripa de cierta diedad# otorga"},
                     }, {},
                       //clear text
                       Text{"a spider surrounded by #shadows in Jabu-Jabu's Belly# holds", /*french*/"la Skulltula entourée d'#ombres dans Jabu-Jabu# possède", /*spanish*/"una Skulltula rodeada de #sombras en la Tripa de Jabu-Jabu# otorga"}
  );

    hintTable[JABU_JABUS_BELLY_MQ_COW] = HintText::Sometimes({
                       //obscure text
                       Text{"a #cow swallowed by a deity# gifts", /*french*/"la #vache dans le gardien# donne", /*spanish*/"una #vaca engullida por cierta deidad# brinda"},
                     }, {},
                       //clear text
                       Text{"a #cow swallowed by Jabu-Jabu# gifts", /*french*/"la #vache avallée par Jabu-Jabu# donne", /*spanish*/"una #vaca engullida por Jabu-Jabu# brinda"}
  );

    hintTable[FIRE_TEMPLE_SCARECROW_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"a #scarecrow atop the volcano# hides", /*french*/"l'#épouvantail au sommet d'un volcan# donne", /*spanish*/"un #espantapájaros en lo alto del volcán# esconde"},
                     }, {},
                       //clear text
                       Text{"#Pierre atop the Fire Temple# hides", /*french*/"#Pierre au sommet du temple du feu# donne", /*spanish*/"#Pierre en lo alto del Templo del Fuego# esconde"}
  );

    hintTable[FIRE_TEMPLE_MEGATON_HAMMER_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the #Flare Dancer atop the volcano# guards a chest containing", /*french*/"le #danseur au sommet du volcan# protège", /*spanish*/"el #Bailafuego en lo alto del volcán# otorga"},
                     }, {},
                       //clear text
                       Text{"the #Flare Dancer atop the Fire Temple# guards a chest containing", /*french*/"le #Danse-Flamme au sommet du temple du feu# protège", /*spanish*/"el #Bailaguego en lo alto del Templo del Fuego# otorga"}
  );

    hintTable[FIRE_TEMPLE_MQ_CHEST_ON_FIRE] = HintText::Sometimes({
                       //obscure text
                       Text{"the #Flare Dancer atop the volcano# guards a chest containing", /*french*/"le #danseur au sommet du volcan# protège", /*spanish*/"el #Bailafuego en lo alto del volcán# otorga"},
                     }, {},
                       //clear text
                       Text{"the #Flare Dancer atop the Fire Temple# guards a chest containing", /*french*/"le #Danse-Flamme au sommet du temple du feu# protège", /*spanish*/"el #Bailafuego en lo alto del Templo del Fuego# otorga"}
  );

    hintTable[FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider under a block in the volcano# holds", /*french*/"l'#araignée sous un bloc dans le volcan# a", /*spanish*/"una #Skulltula bajo el bloque de un volcán# otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider under a block in the Fire Temple# holds", /*french*/"une #Skulltula sous un bloc dans le temple du feu# a", /*spanish*/"una #Skulltula bajo un bloque del Templo del Fuego# otorga"}
  );

    hintTable[WATER_TEMPLE_RIVER_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"beyond the #river under the lake# waits", /*french*/"au delà de #la rivière sous le lac# se cache", /*spanish*/"tras el #río bajo el lago# yace"},
                     }, {},
                       //clear text
                       Text{"beyond the #river in the Water Temple# waits", /*french*/"au delà de #la rivière dans le temple de l'eau# se cache", /*spanish*/"tras el #río del Templo del Agua# yace"}
  );

    hintTable[WATER_TEMPLE_BOSS_KEY_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"dodging #rolling boulders under the lake# leads to", /*french*/"éviter des #rochers roulants sous le lac# mène à", /*spanish*/"esquivar #rocas rodantes bajo el lago# conduce a"},
                     }, {},
                       //clear text
                       Text{"dodging #rolling boulders in the Water Temple# leads to", /*french*/"éviter des #rochers roulants dans le temple de l'eau# mène à", /*spanish*/"esquivar #rocas rondantes del Templo del Agua# conduce a"}
  );

    hintTable[WATER_TEMPLE_GS_BEHIND_GATE] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider behind a #gate under the lake# holds", /*french*/"l'araignée derrière une #barrière sous le lac# a", /*spanish*/"una Skulltula tras #una valla bajo el lago# otorga"},
                     }, {},
                       //clear text
                       Text{"a spider behind a #gate in the Water Temple# holds", /*french*/"la Skulltula derrière une #barrière dans le temple de l'eau# a", /*spanish*/"una Skulltula tras #una valla del Templo del Agua# otorga"}
  );

    hintTable[WATER_TEMPLE_MQ_FREESTANDING_KEY] = HintText::Sometimes({
                       //obscure text
                       Text{"hidden in a #box under the lake# lies", /*french*/"dans une #boîte sous le lac# gît", /*spanish*/"en una #caja bajo el lago# yace"},
                     }, {},
                       //clear text
                       Text{"hidden in a #box in the Water Temple# lies", /*french*/"dans une #boîte dans le temple de l'eau# gît", /*spanish*/"en una #caja del Templo del Agua# yace"}
  );

    hintTable[WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA] = HintText::Sometimes({
                       //obscure text
                       Text{"the #locked spider under the lake# holds", /*french*/"l'#araignée emprisonnée sous le lac# a", /*spanish*/"la #Skulltula enjaulada bajo el lago# otorga"},
                     }, {},
                       //clear text
                       Text{"the #locked spider in the Water Temple# holds", /*french*/"une #Skulltula emprisonnée dans le temple de l'eau# a", /*spanish*/"la #Skulltula enjaulada del Templo del Agua# otorga"}
  );

    hintTable[WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH] = HintText::Sometimes({
                       //obscure text
                       Text{"a spider behind a #gate under the lake# holds", /*french*/"l'#araignée derrière une barrière sous le lac# a", /*spanish*/"una Skulltula tras una #valla bajo el lago# otorga"},
                     }, {},
                       //clear text
                       Text{"a spider behind a #gate in the Water Temple# holds", /*french*/"une #Skulltula derrière une barrière dans le temple de l'eau# a", /*spanish*/"una Skulltula tras una #valla del Templo del Agua#"}
  );

    hintTable[GERUDO_TRAINING_GROUNDS_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"those who seek #sunken silver rupees# will find", /*french*/"ceux qui pêchent les #joyaux argentés# trouveront", /*spanish*/"aquellos que busquen las #rupias plateadas sumergidas# encontrarán"},
                       Text{"the #thieves' underwater training# rewards",      /*french*/"l'#épreuve de plongée des voleurs# recèle",         /*spanish*/"la #instrucción submarina de las bandidas# premia"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_UNDERWATER_SILVER_RUPEE_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"those who seek #sunken silver rupees# will find", /*french*/"ceux qui pêchent les #joyaux argentés# trouveront", /*spanish*/"aquellos que busquen las #rupias plateadas sumergidas# encontrarán"},
                       Text{"the #thieves' underwater training# rewards",      /*french*/"l'#épreuve de plongée des voleurs# recèle",         /*spanish*/"la #instrucción submarina de las bandidas# premia"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_FINAL_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the final prize of #the thieves' training# is", /*french*/"la récompense ultime de #l'épreuve des voleurs# est", /*spanish*/"la recompensa final de la #instrucción de las bandida# brinda"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_ICE_ARROWS_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the final prize of #the thieves' training# is", /*french*/"la récompense ultime de #l'épreuve des voleurs# est", /*spanish*/"el premio final de la #instrucción de las bandidas# brinda"},
  });

    hintTable[BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the well's #grasping ghoul# hides",    /*french*/"la #terreur du puits# cache", /*spanish*/"en las #profundidades del pozo# se esconde"},
                       Text{"a #nether dweller in the well# holds", /*french*/"le #spectre qui réside dans le puits# a", /*spanish*/"el #temido morador del pozo# concede"},
                     }, {},
                       //clear text
                       Text{"#Dead Hand in the well# holds", /*french*/"le #Poigneur dans le puits# cache", /*spanish*/"la #Mano Muerta del pozo# concede"}
  );

    hintTable[BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the well's #grasping ghoul# hides",    /*french*/"la #terreur du puits# cache", /*spanish*/"en las #profundidades del pozo# se esconde"},
                       Text{"a #nether dweller in the well# holds", /*french*/"le #spectre qui réside dans le puits# a", /*spanish*/"el #temido morador del pozo# concede"},
                     }, {},
                       //clear text
                       Text{"#Dead Hand in the well# holds", /*french*/"le #Poigneur dans le puits# cache", /*spanish*/"la #Mano Muerta del pozo# concede"}
  );

    hintTable[SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"the treasure #sought by Nabooru# is", /*french*/"le trésor que #recherche Nabooru# est", /*spanish*/"el #ansiado tesoro de Nabooru# brinda"},
                     }, {},
                       //clear text
                       Text{"upon the #Colossus's right hand# is", /*french*/"sur la #main droite du colosse# repose", /*spanish*/"en la #mano derecha del Coloso# yace"}
  );

    hintTable[SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"upon the #Colossus's left hand# is", /*french*/"sur la #main gauche du colosse# repose", /*spanish*/"en la #mano izquierda del Coloso# yace"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"a #temporal paradox in the Colossus# yields", /*french*/"un #paradoxe temporel dans le colosse# révèle", /*spanish*/"una #paradoja temporal del Coloso# conduce a"},
                     }, {},
                       //clear text
                       Text{"a #temporal paradox in the Spirit Temple# yields", /*french*/"le #paradoxe temporel dans le temple de l'esprit# révèle", /*spanish*/"una #paradoja temporal del Coloso# conduce a"}
  );

    hintTable[SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"a #symphony in the Colossus# yields", /*french*/"la #symphonie du colosse# révèle", /*spanish*/"una #sinfonía del Coloso# conduce a"},
                     }, {},
                       //clear text
                       Text{"a #symphony in the Spirit Temple# yields", /*french*/"les #cinq chansons du temple de l'esprit# révèlent", /*spanish*/"una #sinfonía del Coloso# conduce a"}
  );

    hintTable[SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM] = HintText::Sometimes({
                       //obscure text
                       Text{"a #spider's symphony in the Colossus# yields", /*french*/"la #mélodie de l'araignée du colosse# révèle", /*spanish*/"la #Skulltula de la sinfonía del Coloso# otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider's symphony in the Spirit Temple# yields", /*french*/"la #mélodie de la Skulltula du temple de l'esprit# révèle", /*spanish*/"la #Skulltula de la sinfonía del Coloso# otorga"}
  );

    hintTable[SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"shadows in an #invisible maze# guard", /*french*/"les ombres dans le #labyrinthe invisible# protègent", /*spanish*/"las sombras del #laberinto misterioso# esconden"},
  });

    hintTable[SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST] = HintText::Sometimes({
                       //obscure text
                       Text{"shadows in an #invisible maze# guard", /*french*/"les ombres dans le #labyrinthe invisible# protègent", /*spanish*/"las sombras del #laberinto invisible# esconden"},
  });

    /*--------------------------
    |    EXCLUDE HINT TEXT     |
    ---------------------------*/

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
                       Text{"#playing an Ocarina in Lost Woods# is rewarded with", /*french*/"#jouer l'ocarina dans la forêt des méandres# donne", /*spanish*/"#tocar la ocarina en el Bosque Perdido# otorga"}
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

    hintTable[GF_GERUDO_TOKEN] = HintText::Exclude({
                       //obscure text
                       Text{"#rescuing captured carpenters# is rewarded with", /*french*/"#secourir les charpentiers capturés# assure", /*spanish*/"#rescatar los apresados carpinteros# se premia con"},
  });

    hintTable[WASTELAND_BOMBCHU_SALESMAN] = HintText::Exclude({
                       //obscure text
                       Text{"a #carpet guru# sells", /*french*/"#un marchand du désert# vend", /*spanish*/"el #genio de una alfombra# vende"},
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
                       Text{"a #crate in a graveyard# hides", /*french*/"#la boîte dans le cimetière# contient", /*spanish*/"bajo la #caja de un cementerio# yace"},
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
                       Text{"behind a #desert waterfall# is", /*french*/"#derrière la cascade du désert# se cache", /*spanish*/"tras una #desierta cascada# yace"},
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


    hintTable[DEKU_TREE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #center of the Deku Tree# lies", /*french*/"#le cœur de l'arbre Mojo# recèle", /*spanish*/"al #centro del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_SLINGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure guarded by a scrub# in the Deku Tree is", /*french*/"le #trésor protégé par une peste# dans l'arbre Mojo est", /*spanish*/"un #deku del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #treasure guarded by a scrub# in the Deku Tree is", /*french*/"le #trésor protégé par une peste# dans l'arbre Mojo est", /*spanish*/"un #deku del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_COMPASS_ROOM_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#webs in the Deku Tree# hide", /*french*/"les #toiles dans l'arbre Mojo# cachent", /*spanish*/"entre #telarañas del Gran Árbol Deku# yace"},
  });


    hintTable[DEKU_TREE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #center of the Deku Tree# lies", /*french*/"#le cœur de l'arbre Mojo# recèle", /*spanish*/"al #centro del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #treasure guarded by a large spider# in the Deku Tree is", /*french*/"le #trésor protégé par une grosse araignée# dans l'arbre Mojo est", /*spanish*/"una #gran araña del Gran Árbol Deku# esconde"},
  });

    hintTable[DEKU_TREE_MQ_SLINGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#pillars of wood# in the Deku Tree lead to", /*french*/"les #piliers de bois# dans l'arbre Mojo indiquent", /*spanish*/"los #salientes del Gran Árbol Deku# conducen a"},
  });

    hintTable[DEKU_TREE_MQ_BASEMENT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#webs in the Deku Tree# hide", /*french*/"les #toiles dans l'arbre Mojo# cachent", /*spanish*/"entre #telarañas del Gran Árbol Deku# yace"},
  });

    hintTable[DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#magical fire in the Deku Tree# leads to", /*french*/"le #feu magique dans l'arbre Mojo# éclaire", /*spanish*/"el #fuego mágico en el Gran Árbol Deku# conduce a"},
  });


    hintTable[DODONGOS_CAVERN_BOSS_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#above King Dodongo# lies", /*french*/"#par dessus le Roi Dodongo# gît", /*spanish*/"#sobre el Rey Dodongo# yace"},
  });


    hintTable[DODONGOS_CAVERN_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #muddy wall in Dodongo's Cavern# hides", /*french*/"le #mur fragile dans la caverne Dodongo# recèle", /*spanish*/"tras una #agrietada pared en la Cueva de los Dodongos# yace"},
  });

    hintTable[DODONGOS_CAVERN_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #statue in Dodongo's Cavern# guards", /*french*/"la #statue dans la caverne Dodongo# protège", /*spanish*/"una #estatua de la Cueva de los Dodongos# esconde"},
  });

    hintTable[DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"above a #maze of stone# in Dodongo's Cavern lies", /*french*/"sur #un labyrinthe de pierre# dans la caverne Dodongo gît", /*spanish*/"entre un #laberinto de piedra# en la Cueva de los Dodongos yace"},
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
                       Text{"a #muddy wall in Dodongo's Cavern# hides", /*french*/"le #mur fragile dans la caverne Dodongo# recèle", /*spanish*/"una #agrietada pared en la Cueva de los Dodongos# esconde"},
  });

    hintTable[DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #elevated alcove# in Dodongo's Cavern holds", /*french*/"l'#alcove haut perchée# dans la caverne Dodongo cache", /*spanish*/"una #elevada alcoba# en la Cueva de los Dodongos brinda"},
  });

    hintTable[DODONGOS_CAVERN_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire-breathing lizards# in Dodongo's Cavern guard", /*french*/"les #lézards cracheurs de feu# dans la caverne Dodongo protègent", /*spanish*/"unos #flamígeros reptiles# en la Cueva de los Dodongos esconden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#baby spiders# in Dodongo's Cavern guard", /*french*/"les #petites araignées dans la caverne Dodongo# protègent", /*spanish*/"unas #pequeñas larvas# en la Cueva de los Dodongos esconden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"above a #maze of stone# in Dodongo's Cavern lies", /*french*/"sur #un labyrinthe de pierre# dans la caverne Dodongo gît", /*spanish*/"sobre un #laberinto de piedra# en la Cueva de los Dodongos yace"},
  });

    hintTable[DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#beneath a headstone# in Dodongo's Cavern lies", /*french*/"#sous une pierre tombale# dans la caverne Dodongo gît", /*spanish*/"#bajo una lápida# en la Cueva de los Dodongos yace"},
  });


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
                       Text{"a #pair of digested cows# hold", /*french*/"#deux bœufs digérés# détiennent", /*spanish*/"un #par de digeridas vacas# otorgan"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #pair of digested cows# hold", /*french*/"#deux bœufs digérés# détiennent", /*spanish*/"un #par de digeridas vacas# otorgan"},
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


    hintTable[FOREST_TEMPLE_FIRST_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #tree in the Forest Temple# supports", /*french*/"sur l'#arbre dans le temple de la forêt# gît", /*spanish*/"sobre un #árbol del Templo del Bosque# yace"},
  });

    hintTable[FOREST_TEMPLE_FIRST_STALFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating enemies beneath a falling ceiling# in Forest Temple yields", /*french*/"#deux squelettes# dans le temple de la forêt protègent", /*spanish*/"#derrotar enemigos caídos de lo alto# del Templo del Bosque revela"},
  });

    hintTable[FOREST_TEMPLE_WELL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #sunken chest deep in the woods# contains", /*french*/"le #coffre submergé dans la forêt# contient", /*spanish*/"un #sumergido cofre en lo profundo del bosque# contiene"},
  });

    hintTable[FOREST_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #fiery skull# in Forest Temple guards", /*french*/"le #crâne enflammé# dans le temple de la forêt protège", /*spanish*/"una #ardiente calavera# del Templo del Bosque esconde"},
  });

    hintTable[FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest on a small island# in the Forest Temple holds", /*french*/"le #coffre sur l'îlot# du temple de la forêt contient", /*spanish*/"un #cofre sobre una isla# del Templo del Bosque contiene"},
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
                       Text{"#blocks of stone# in the Forest Temple surround", /*french*/"les #blocs dans le temple de la forêt# entourent", /*spanish*/"cerca de unos #bloques de piedra# del Templo del Bosque yace"}
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
                       Text{"#Stalfos deep in the Forest Temple# guard", /*french*/"#trois squelettes dans le temple de la forêt# protègent", /*spanish*/"los #Stalfos en lo profundo del Templo del Bosque# guardan"}
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
                       Text{"a #tree in the Forest Temple# supports", /*french*/"sur l'#arbre dans le temple de la forêt# gît", /*spanish*/"sobre un #árbol del Templo del Bosque# yace"},
  });

    hintTable[FOREST_TEMPLE_MQ_WOLFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#defeating enemies beneath a falling ceiling# in Forest Temple yields", /*french*/"#deux squelettes# dans le temple de la forêt protègent", /*spanish*/"#derrotar enemigos caídos de lo alto# del Templo del Bosque revela"},
  });

    hintTable[FOREST_TEMPLE_MQ_BOW_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #army of the dead# guards", /*french*/"des #squelettes sylvestres# protègent", /*spanish*/"un #ejército de soldados caídos# guarda"},
                     }, {},
                       //clear text
                       Text{"#Stalfos deep in the Forest Temple# guard", /*french*/"#trois squelettes dans le temple de la forêt# protègent", /*spanish*/"los #Stalfos en lo profundo del Templo del Bosque# guardan"}
  );

    hintTable[FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest on a small island# in the Forest Temple holds", /*french*/"le #coffre sur l'îlot# du temple de la forêt contient", /*spanish*/"un #cofre sobre una isla# del Templo del Bosque contiene"},
  });

    hintTable[FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#high in a courtyard# within the Forest Temple is", /*french*/"#haut perché dans le jardin# du temple de la forêt gît", /*spanish*/"un #cofre en lo alto de un patio# del Templo del Bosque contiene"},
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
                       Text{"deep in the forest #undead guard a chest# containing", /*french*/"des #revenants dans le temple de la forêt# protègent", /*spanish*/"en lo profundo del bosque #guardias del más allá# guardan"},
  });

    hintTable[FOREST_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #turned trunk# contains", /*french*/"le #coffre pivoté# contient", /*spanish*/"en una #sala con otro punto de vista# se esconde"},
  });


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
                       Text{"a #blocked path# in Fire Temple holds", /*french*/"un #sol fragile dans le temple du feu# contient", /*spanish*/"en un #camino bloqueado# del Templo del Fuego yace"},
  });

    hintTable[FIRE_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #caged chest# in the Fire Temple hoards", /*french*/"un #coffre emprisonné# dans le temple du feu contient", /*spanish*/"un #cofre entre rejas# del Templo del Fuego contiene"},
  });

    hintTable[FIRE_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #chest in a fiery maze# contains", /*french*/"un #coffre dans un labyrinthe enflammé# contient", /*spanish*/"un #cofre de un ardiente laberinto# contiene"},
  });

    hintTable[FIRE_TEMPLE_HIGHEST_GORON_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron atop the Fire Temple# holds", /*french*/"le #Goron au sommet du temple du feu# a", /*spanish*/"un #goron en lo alto del Templo del Fuego# guarda"},
  });


    hintTable[FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#near a dragon# is", /*french*/"#près d'un dragon# gît", /*spanish*/"#cerca de un dragón# yace"},
  });

    hintTable[FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Flare Dancer in the depths of a volcano# guards", /*french*/"le #Danse-Flamme au cœur du volcan# a", /*spanish*/"el #Bailafuego en lo profundo del volcán# esconde"},
                     }, {},
                       //clear text
                       Text{"the #Flare Dancer in the depths of the Fire Temple# guards", /*french*/"le #Danse-Flamme au cœur du volcan# a", /*spanish*/"el #Bailafuego en lo profundo del Templo del Fuego# esconde"}
  );

    hintTable[FIRE_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blocked path# in Fire Temple holds", /*french*/"le #chemin scellé# dans le temple du feu contient", /*spanish*/"en un #camino bloqueado# del Templo del Fuego yace"},
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
                       Text{"a #falling slug# in the Fire Temple guards", /*french*/"la #limace tombante# dans le temple du feu protège", /*spanish*/"una #babosa del techo# del Templo del Fuego guarda"},
  });

    hintTable[FIRE_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"using a #hammer in the depths of the Fire Temple# reveals", "frapper du #marteau au cœur du volcan# révèle", /*spanish*/"usar el #martillo en lo profundo del Templo del Fuego# revela"},
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


    hintTable[WATER_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#rolling spikes# in the Water Temple surround", /*french*/"des #Spikes# dans le temple de l'eau entourent", /*spanish*/"unas #rodantes púas# del Templo del Agua guardan"},
  });

    hintTable[WATER_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#roaming stingers in the Water Temple# guard", /*french*/"des #raies dans le temple de l'eau# protègent", /*spanish*/"unos #errantes stingers# del Templo del Agua guardan"},
  });

    hintTable[WATER_TEMPLE_TORCHES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire in the Water Temple# reveals", /*french*/"des #flammes dans le temple de l'eau# révèlent", /*spanish*/"el #fuego en el Templo del Agua# revela"},
  });

    hintTable[WATER_TEMPLE_DRAGON_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #serpent's prize# in the Water Temple is", /*french*/"la #récompense du dragon submergé# est", /*spanish*/"el #escamado premio# del Templo del Agua se trata de"},
  });

    hintTable[WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#blinding an eye# in the Water Temple leads to", /*french*/"#l'oeil# du temple de l'eau voit", /*spanish*/"#cegar un ojo# del Templo del Agua conduce a"},
  });

    hintTable[WATER_TEMPLE_CENTRAL_PILLAR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #depths of the Water Temple# lies", /*french*/"le #cœur du temple de l'eau# cache", /*spanish*/"en las #profundidades del Templo del Agua# yace"},
  });

    hintTable[WATER_TEMPLE_CRACKED_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#through a crack# in the Water Temple is", /*french*/"le #mur fragile# du temple de l'eau cache", /*spanish*/"tras una #agrietada pared# del Templo del Agua yace"},
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
                       Text{"in the #depths of the Water Temple# lies", /*french*/"le #cœur du temple de l'eau# cache", /*spanish*/"en las #profundidades del Templo del Agua# yace"},
  });

    hintTable[WATER_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"fire in the Water Temple unlocks a #vast gate# revealing a chest with", /*french*/"des #flammes au cœur du temple de l'eau# révèlent", /*spanish*/"el fuego en el Templo del Agua alza una #gran valla# con"},
  });

    hintTable[WATER_TEMPLE_MQ_LONGSHOT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#through a crack# in the Water Temple is", /*french*/"le #mur fragile# du temple de l'eau cache", /*spanish*/"tras una #agrietada pared# del Templo del Agua yace"},
  });

    hintTable[WATER_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fire in the Water Temple# reveals", /*french*/"des #flammes dans le temple de l'eau# révèlent", /*spanish*/"el #fuego en el Templo del Agua# revela"},
  });

    hintTable[WATER_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#sparring soldiers# in the Water Temple guard", /*french*/"les #soldats du temple de l'eau# protègent", /*spanish*/"#acabar con unos soldados# del Templo del Agua revela"},
  });


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

    hintTable[SHADOW_TEMPLE_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# pierces a hall of faces to reveal", /*french*/"l'#oeil de vérité# voit dans les couloirs du temple de l'ombre", /*spanish*/"el #Ojo de la Verdad# descubrirá un pasillo de facetas con"},
  });

    hintTable[SHADOW_TEMPLE_HOVER_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #nether dweller in the Shadow Temple# holds", /*french*/"le #spectre du temple de l'ombre# a", /*spanish*/"un #temido morador del Templo de las Sombras# guarda"},
                     }, {},
                       //clear text
                       Text{"#Dead Hand in the Shadow Temple# holds", /*french*/"le #Poigneur dans le temple de l'ombre# cache", /*spanish*/"la #Mano Muerta del Templo de las Sombras# guarda"}
  );

    hintTable[SHADOW_TEMPLE_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies revealed by the Eye of Truth# guard", /*french*/"les #Gibdos dans les couloirs# du temple de l'ombre protègent", /*spanish*/"las #momias reveladas por el Ojo de la Verdad# guardan"},
  });

    hintTable[SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#spinning scythes# protect", /*french*/"les #faucheurs danseurs# du temple de l'ombre protègent", /*spanish*/"las #giratorias guadañas# protegen"},
  });

    hintTable[SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du temple de l'ombre protègent", /*spanish*/"las #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du temple de l'ombre protègent", /*spanish*/"las #hojas invisibles# guardan"},
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
                       Text{"the #dead roam among invisible spikes# guarding", /*french*/"#parmi les clous invisibles# du temple de l'ombre se cache", /*spanish*/"los #muertos que vagan por pinchos invisibles# protegen"},
  });

    hintTable[SHADOW_TEMPLE_WIND_HINT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible chest guarded by the dead# holds", /*french*/"le #trésor invisible du cul-de-sac# du temple de l'ombre contient", /*spanish*/"un #cofre invisible custodiado por los del más allá# contiene"},
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
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du temple de l'ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du temple de l'ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#inside a burning skull# lies", /*french*/"#dans un crâne enflammé# gît", /*spanish*/"en el #interior de una calavera en llamas# aguarda"},
  });


    hintTable[SHADOW_TEMPLE_MQ_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# pierces a hall of faces to reveal", /*french*/"l'#oeil de vérité# voit dans les couloirs du temple de l'ombre", /*spanish*/"el #Ojo de la Verdad# descubre un pasillo de facetas con"},
  });

    hintTable[SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand in the Shadow Temple# holds", /*french*/"le #Poigneur dans le temple de l'ombre# cache", /*spanish*/"la #Mano Muerta del Templo de las Sombras# guarda"},
  });

    hintTable[SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#mummies revealed by the Eye of Truth# guard", /*french*/"les #Gibdos dans les couloirs# du temple de l'ombre protègent", /*spanish*/"las #momias reveladas por el Ojo de la Verdad# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#spinning scythes# protect", /*french*/"les #faucheurs danseurs# du temple de l'ombre protègent", /*spanish*/"las #giratorias guadañas# protegen"},
  });

    hintTable[SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#collecting rupees in a vast cavern# with the Shadow Temple unveils", /*french*/"les #pièces argentées dans le temple de l'ombre# révèlent", /*spanish*/"hacerte con las #rupias en una gran caverna# del Templo de las Sombras revela"},
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
                       Text{"the #dead roam among invisible spikes# guarding", /*french*/"#parmi les clous invisibles# du temple de l'ombre se cache", /*spanish*/"los #muertos que vagan por pinchos invisibles# protegen"},
  });

    hintTable[SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du temple de l'ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#walls consumed by a ball of fire# reveal", /*french*/"le #piège de bois# du temple de l'ombre cache", /*spanish*/"las #paredes consumidas por una esfera ígnea# revelan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"near an #empty pedestal# within the Shadow Temple lies", /*french*/"#près d'un pédestal vide du temple de l'ombre# gît", /*spanish*/"cerca de un #vacío pedestal# del Templo de las Sombras yace"},
  });

    hintTable[SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du temple de l'ombre protègent", /*spanish*/"unas #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#invisible blades# guard", /*french*/"les #faucheurs invisibles# du temple de l'ombre protègent", /*spanish*/"unas #hojas invisibles# guardan"},
  });

    hintTable[SHADOW_TEMPLE_MQ_WIND_HINT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible chest guarded by the dead# holds", /*french*/"le #trésor invisible du cul-de-sac# du temple de l'ombre contient", /*spanish*/"un #cofre invisible custodiado por los del más allá# contiene"},
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


    hintTable[BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the well# reveals", /*french*/"l'#oeil de vérité dans le puits# révèle", /*spanish*/"el #Ojo de la Verdad en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#gruesome debris# in the well hides", /*french*/"des #débris dans le puits# cachent", /*spanish*/"unos #horripilantes escombros# del pozo esconden"},
  });

    hintTable[BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth in the well# reveals", /*french*/"l'#oeil de vérité dans le puits# révèle", /*spanish*/"el #Ojo de la Verdad en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_COMPASS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #hidden entrance to a cage# in the well leads to", /*french*/"dans un #chemin caché dans le puits# gît", /*spanish*/"la #entrada oculta de una celda# del pozo conduce a"},
  });

    hintTable[BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarding a cage# in the well protects", /*french*/"l'#araignée dans la cage du puits# protège", /*spanish*/"una #araña protegiendo una celda# del pozo guarda"},
  });

    hintTable[BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#gruesome debris# in the well hides", /*french*/"des #débris dans le puits# cachent", /*spanish*/"unos #horripilantes escombros# del pozo esconden"},
  });

    hintTable[BOTTOM_OF_THE_WELL_INVISIBLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand's invisible secret# is", /*french*/"le #trésor invisible du Poigneur# est", /*spanish*/"el #secreto invisible de la Mano Muerta# esconde"},
  });

    hintTable[BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"in the #depths of the well# lies", /*french*/"#dans le cœur du puits# gît", /*spanish*/"en las #profundidades del pozo# yace"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#perilous pits# in the well guard the path to", /*french*/"#trois trous# dans le puits protègent", /*spanish*/"#peligrosos fosos# del pozo conducen a"},
  });

    hintTable[BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#locked in a cage# in the well lies", /*french*/"#dans une cage# du puits gît", /*spanish*/"#entre rejas# en el pozo yace"},
  });

    hintTable[BOTTOM_OF_THE_WELL_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#inside a coffin# hides", /*french*/"dans #un cercueil# gît", /*spanish*/"en el #interior de un ataúd# yace"},
  });


    hintTable[BOTTOM_OF_THE_WELL_MQ_MAP_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #royal melody in the well# uncovers", /*french*/"la #mélodie royale révèle dans le puits#", /*spanish*/"una #melodía real en el pozo# revela"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"an #army of the dead# in the well guards", /*french*/"l'#armée des morts# dans le puits protège", /*spanish*/"un #ejército del más allá# del pozo guarda"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"#Dead Hand's explosive secret# is", /*french*/"le #secret explosif du Poigneur# est", /*spanish*/"el #explosivo secreto de la Mano Muerta# esconde"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible path in the well# leads to", /*french*/"dans un #chemin caché dans le puits# gît", /*spanish*/"un #camino invisible del pozo# conduce a"},
  });


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


    hintTable[GERUDO_TRAINING_GROUNDS_LOBBY_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blinded eye in the Gerudo Training Grounds# drops", /*french*/"l'#Oeil dans le gymnase Gerudo# voit", /*spanish*/"#cegar un ojo en el Centro de Instrucción Gerudo# revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_LOBBY_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #blinded eye in the Gerudo Training Grounds# drops", /*french*/"l'#Oeil dans le gymnase Gerudo# voit", /*spanish*/"#cegar un ojo en el Centro de Instrucción Gerudo# revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_STALFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#soldiers walking on shifting sands# in the Gerudo Training Grounds guard", /*french*/"les #squelettes# du gymnase Gerudo protègent", /*spanish*/"#soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_BEAMOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reptilian warriors# in the Gerudo Training Grounds protect", /*french*/"les #lézards# dans le gymnase Gerudo protègent", /*spanish*/"#unos escamosos guerreros# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HIDDEN_CEILING_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# in the Gerudo Training Grounds reveals", /*french*/"#bien caché# dans le gymnase Gerudo gît", /*spanish*/"el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the first prize of #the thieves' training# is", /*french*/"le #premier trésor du gymnase Gerudo# est", /*spanish*/"el primer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the second prize of #the thieves' training# is", /*french*/"le #deuxième trésor du gymnase Gerudo# est", /*spanish*/"el segundo premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_PATH_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the third prize of #the thieves' training# is", /*french*/"le #troisième trésor du gymnase Gerudo# est", /*spanish*/"el tercer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_CENTRAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MAZE_RIGHT_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_CLEAR_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fiery foes# in the Gerudo Training Grounds guard", /*french*/"les #limaces de feu# du gymnase Gerudo protègent", /*spanish*/"unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HAMMER_ROOM_SWITCH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#engulfed in flame# where thieves train lies", /*french*/"le #trésor enflammé# du gymnase Gerudo est", /*spanish*/"donde entrenan las bandidas #entre llamas# yace"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_EYE_STATUE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_NEAR_SCARECROW_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_BEFORE_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#before a block of silver# thieves can find", /*french*/"#près d'un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"#ante un plateado bloque# las bandidas hallan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_HEAVY_BLOCK_FOURTH_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_FREESTANDING_KEY] = HintText::Exclude({
                       //obscure text
                       Text{"the #Song of Time# in the Gerudo Training Grounds leads to", /*french*/"le #chant du temps# révèle dans le gymnase Gerudo", /*spanish*/"la #Canción del Tiempo# en el Centro de Instrucción Gerudo conduce a"},
  });


    hintTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY_RIGHT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#thieves prepare for training# with", /*french*/"dans #l'entrée du gymnase Gerudo# gît", /*spanish*/"las #bandidas se instruyen# con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_LOBBY_LEFT_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#thieves prepare for training# with", /*french*/"dans #l'entrée du gymnase Gerudo# gît", /*spanish*/"las #bandidas se instruyen# con"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_FIRST_IRON_KNUCKLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#soldiers walking on shifting sands# in the Gerudo Training Grounds guard", /*french*/"les #squelettes# du gymnase Gerudo protègent", /*spanish*/"#soldados en resbaladizas arenas# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_BEFORE_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#before a block of silver# thieves can find", /*french*/"#près d'un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"#ante un plateado bloque# las bandidas hallan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_EYE_STATUE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"thieves #blind four faces# to find", /*french*/"l'#épreuve d'archerie# du gymnase Gerudo donne", /*spanish*/"las bandidas #ciegan cuatro bustos# para hallar"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_FLAME_CIRCLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#engulfed in flame# where thieves train lies", /*french*/"le #trésor enflammé# du gymnase Gerudo est", /*spanish*/"donde entrenan las bandidas #entre llamas# yace"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_SECOND_IRON_KNUCKLE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#fiery foes# in the Gerudo Training Grounds guard", /*french*/"les #ennemis de feu# du gymnase Gerudo protègent", /*spanish*/"unos #flamígeros enemigos# del Centro de Instrucción Gerudo guardan"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_DINOLFOS_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"#reptilian warriors# in the Gerudo Training Grounds protect", /*french*/"les #lézards# dans le gymnase Gerudo protègent", /*spanish*/"#unos escamosos guerreros# del Centro de Instrucción Gerudo protegen"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_CENTRAL_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #path of fire# leads thieves to", /*french*/"dans le #chemin enflammé# dans le gymnase Gerudo gît", /*spanish*/"un #camino de fuego# conduce a las bandidas a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_FIRST_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the first prize of #the thieves' training# is", /*french*/"le #premier trésor du gymnase Gerudo# est", /*spanish*/"el primer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_RIGHT_SIDE_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #path of fire# leads thieves to", /*french*/"dans le #chemin enflammé# dans le gymnase Gerudo gît", /*spanish*/"un #camino de fuego# conduce a las bandidas a"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_THIRD_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the third prize of #the thieves' training# is", /*french*/"le #troisième trésor du gymnase Gerudo# est", /*spanish*/"el tercer premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_MAZE_PATH_SECOND_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the second prize of #the thieves' training# is", /*french*/"le #deuxième trésor du gymnase Gerudo# est", /*spanish*/"el segundo premio de la #instrucción bandida# se trata de"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_HIDDEN_CEILING_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"the #Eye of Truth# in the Gerudo Training Grounds reveals", /*french*/"#bien caché# dans le gymnase Gerudo gît", /*spanish*/"el #Ojo de la Verdad# en el Centro de Instrucción Gerudo revela"},
  });

    hintTable[GERUDO_TRAINING_GROUNDS_MQ_HEAVY_BLOCK_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a #feat of strength# rewards thieves with", /*french*/"#derrière un bloc argent# dans le gymnase Gerudo gît", /*spanish*/"una #hazaña de fuerza# premia a las bandidas con"},
  });


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


    hintTable[DEKU_TREE_QUEEN_GOHMA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Parasitic Armored Arachnid# holds", /*french*/"le #monstre insectoïde géant# possède", /*spanish*/"el #arácnido parasitario acorazado# porta"},
                     }, {},
                       //clear text
                       Text{"#Queen Gohma# holds", /*french*/"la #Reine Gohma# possède", /*spanish*/"la #Reina Goma# porta"}
  );

    hintTable[DODONGOS_CAVERN_KING_DODONGO_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Infernal Dinosaur# holds", /*french*/"le #dinosaure infernal# possède", /*spanish*/"el #dinosaurio infernal# porta"},
                     }, {},
                       //clear text
                       Text{"#King Dodongo# holds", /*french*/"le #Roi Dodongo# possède", /*spanish*/"el #Rey Dodongo# porta"}
  );

    hintTable[JABU_JABUS_BELLY_BARINADE_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Bio-Electric Anemone# holds", /*french*/"l'#anémone bioélectrique# possède", /*spanish*/"la #anémona bioeléctrica# porta"},
                     }, {},
                       //clear text
                       Text{"#Barinade# holds", /*french*/"#Barinade# possède", /*spanish*/"#Barinade# porta"}
  );

    hintTable[FOREST_TEMPLE_PHANTOM_GANON_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Evil Spirit from Beyond# holds", /*french*/"l'#esprit maléfique de l'au-delà# possède", /*spanish*/"el #espíritu maligno de ultratumba# porta"},
                     }, {},
                       //clear text
                       Text{"#Phantom Ganon# holds", /*french*/"#Ganon Spectral# possède", /*spanish*/"#Ganon Fantasma# porta"}
  );

    hintTable[FIRE_TEMPLE_VOLVAGIA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Subterranean Lava Dragon# holds", /*french*/"le #dragon des profondeurs# possède", /*spanish*/"el #dragón de lava subterráneo# porta"},
                     }, {},
                       //clear text
                       Text{"#Volvagia# holds", /*french*/"#Volvagia# possède", /*spanish*/"#Volvagia# porta"}
  );

    hintTable[WATER_TEMPLE_MORPHA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Giant Aquatic Amoeba# holds", /*french*/"l'#amibe aquatique géante# possède", /*spanish*/"la #ameba acuática gigante# porta"},
                     }, {},
                       //clear text
                       Text{"#Morpha# holds", /*french*/"#Morpha# possède", /*spanish*/"#Morpha# porta"}
  );

    hintTable[SPIRIT_TEMPLE_TWINROVA_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Sorceress Sisters# hold", /*french*/"#les sorcières jumelles# possède", /*spanish*/"las #hermanas hechiceras# portan"},
                     }, {},
                       //clear text
                       Text{"#Twinrova# holds", /*french*/"#Twinrova# possède", /*spanish*/"#Birova# porta"}
    );

    hintTable[SHADOW_TEMPLE_BONGO_BONGO_HEART] = HintText::Exclude({
                       //obscure text
                       Text{"the #Phantom Shadow Beast# holds", /*french*/"le #monstre de l'ombre# possède", /*spanish*/"la #alimaña oscura espectral# porta"},
                     }, {},
                       //clear text
                       Text{"#Bongo Bongo# holds", /*french*/"#Bongo Bongo# possède", /*spanish*/"#Bongo Bongo# porta"}
    );


    hintTable[DEKU_TREE_GS_BASEMENT_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider deep within the Deku Tree# hides", /*french*/"une #Skulltula au cœur de l'arbre Mojo# a", /*spanish*/"una #Skulltula en las profundidades del Árbol Deku# otorga"},
  });

    hintTable[DEKU_TREE_GS_BASEMENT_GATE] = HintText::Exclude({
                       //obscure text
                       Text{"a #web protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière une toile dans l'arbre Mojo# a", /*spanish*/"una #Skulltula protegida por su tela# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_GS_BASEMENT_VINES] = HintText::Exclude({
                       //obscure text
                       Text{"a #web protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière une toile dans l'arbre Mojo# a", /*spanish*/"una #Skulltula protegida por su tela# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_GS_COMPASS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider atop the Deku Tree# holds", /*french*/"une #Skulltula au sommet de l'arbre Mojo# a", /*spanish*/"una #Skulltula en lo alto del Árbol Deku# otorga"},
  });


    hintTable[DEKU_TREE_MQ_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a crate# within the Deku Tree hides", /*french*/"une #Skulltula dans une boîte dans l'arbre Mojo# a", /*spanish*/"una #Skulltula bajo una caja# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_MQ_GS_COMPASS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #wall of rock protects a spider# within the Deku Tree holding", /*french*/"une #Skulltula derrière des rochers dans l'arbre Mojo# a", /*spanish*/"una #Skulltula protegida por una pared rocosa# del Árbol Deku otorga"},
  });

    hintTable[DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider deep within the Deku Tree# hides", /*french*/"une #Skulltula au cœur de l'arbre Mojo# a", /*spanish*/"una #Skulltula en las profundidades del Árbol Deku# otorga"},
  });


    hintTable[DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider entangled in vines# in Dodongo's Cavern guards", /*french*/"une #Skulltula sur les vignes dans la caverne Dodongo# a", /*spanish*/"una #Skulltula sobre unas cepas# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_SCARECROW] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among explosive slugs# hides", /*french*/"une #Skulltula dans l'alcove du couloir dans la caverne Dodongo# a", /*spanish*/"una #Skulltula rodeada de explosivos gusanos# otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider just out of reach# in Dodongo's Cavern holds", /*french*/"une #Skulltula au haut des escaliers de la caverne Dodongo# a", /*spanish*/"una #Skulltula fuera del alcance# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_BACK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider behind a statue# in Dodongo's Cavern holds", /*french*/"une #Skulltula au cœur de la caverne Dodongo# a", /*spanish*/"una #Skulltula tras una estatua# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among bats# in Dodongo's Cavern holds", /*french*/"une #Skulltula entourée de Saigneurs dans la caverne Dodongo# a", /*spanish*/"una #Skulltula rodeada de murciélagos# de la Cueva de los Dodongos otorga"},
  });


    hintTable[DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider high on a wall# in Dodongo's Cavern holds", /*french*/"une #Skulltula haut perchée dans la caverne Dodongo# a", /*spanish*/"una #Skulltula en lo alto de una pared# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on top of a pillar of rock# in Dodongo's Cavern holds", /*french*/"une #Skulltula sur l'énorme pilier de roc de la caverne Dodongo# a", /*spanish*/"una #Skulltula en lo alto de un pilar# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a crate# in Dodongo's Cavern holds", /*french*/"une #Skulltula dans une boîte de la caverne Dodongo# a", /*spanish*/"una #Skulltula bajo una caja# de la Cueva de los Dodongos otorga"},
  });

    hintTable[DODONGOS_CAVERN_MQ_GS_BACK_AREA] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among graves# in Dodongo's Cavern holds", /*french*/"une #Skulltula parmi les tombes de la caverne Dodongo# a", /*spanish*/"una #Skulltula entre lápidas# en la Cueva de los Dodongos otorga"},
  });


    hintTable[JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider resting near a princess# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula près de la princesse dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider resting near a princess# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula près de la princesse dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula junto a una princesa# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"#jellyfish surround a spider# holding", /*french*/"une #Skulltula entourée de méduses dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada de medusas# otorga"},
  });

    hintTable[JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula protégée par des raies dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada por unos stingers# en la Tripa de Jabu-Jabu otorga"},
  });


    hintTable[JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider surrounded by electricity# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula entourée d'électricité dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula rodeada de electricidad# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider guarded by a school of stingers# in Jabu-Jabu's Belly holds", /*french*/"une #Skulltula protégée par des raies dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula protegida por unos stingers# en la Tripa de Jabu-Jabu otorga"},
  });

    hintTable[JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a web within Jabu-Jabu's Belly# holds", /*french*/"une #Skulltula sur une toile dans le ventre de Jabu-Jabu# a", /*spanish*/"una #Skulltula sobre una red# en la Tripa de Jabu-Jabu otorga"},
  });


    hintTable[FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on a small island# in the Forest Temple holds", /*french*/"une #Skulltula sur l'îlot du temple de la forêt# a", /*spanish*/"una #Skulltula sobre una pequeña isla# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_FIRST_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider high on a wall of vines# in the Forest Temple holds", /*french*/"une #Skulltula sur un mur de vignes du temple de la forêt# a", /*spanish*/"una #Skulltula en lo alto de una pared de cepas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"#stone columns# lead to a spider in the Forest Temple hiding", /*french*/"une #Skulltula haut perchée dans le jardin du temple de la forêt# a", /*spanish*/"unas #columnas del Templo del Bosque# conducen a una Skulltula que otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider among ghosts# in the Forest Temple guards", /*french*/"une #Skulltula dans la grande salle du temple de la forêt# a", /*spanish*/"una #Skulltula rodeada de fantasmas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_GS_BASEMENT] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider within revolving walls# in the Forest Temple holds", /*french*/"une #Skulltula derrière les murs pivotants du temple de la forêt# a", /*spanish*/"una #Skulltula entre paredes giratorias# del Templo del Bosque otorga"},
  });


    hintTable[FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY] = HintText::Exclude({
                       //obscure text
                       Text{"an #ivy-hidden spider# in the Forest Temple hoards", /*french*/"une #Skulltula près de l'entrée du temple de la forêt# a", /*spanish*/"una #Skulltula escondida entre cepas# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in a hidden nook# within the Forest Temple holds", /*french*/"une #Skulltula dans un recoin caché du temple de la forêt# a", /*spanish*/"una #Skulltula en una esquina oculta# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on an arch# in the Forest Temple holds", /*french*/"une #Skulltula sur une arche du temple de la forêt# a", /*spanish*/"una #Skulltula sobre un arco# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider on a ledge# in the Forest Temple holds", /*french*/"une #Skulltula dans le jardin du temple de la forêt# a", /*spanish*/"una #Skulltula en un borde# del Templo del Bosque otorga"},
  });

    hintTable[FOREST_TEMPLE_MQ_GS_WELL] = HintText::Exclude({
                       //obscure text
                       Text{"#draining a well# in Forest Temple uncovers a spider with", /*french*/"une #Skulltula au fond du puits du temple de la forêt# a", /*spanish*/"#vaciar el pozo# del Templo del Bosque desvela una Skulltula que otorga"},
  });


    hintTable[FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#eight tiles of malice# guard a spider holding", /*french*/"une #Skulltula protégée par huit tuiles dans le temple du feu# a", /*spanish*/"#ocho baldosas de maldad# custodian una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_BOSS_KEY_LOOP] = HintText::Exclude({
                       //obscure text
                       Text{"#five tiles of malice# guard a spider holding", /*french*/"une #Skulltula protégée par cinq tuiles dans le temple du feu# a", /*spanish*/"#cinco baldosas de maldad# custodian una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_BOULDER_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"#explosives in a maze# unveil a spider hiding", /*french*/"une #Skulltula derrière un mur fragile du temple du feu# a", /*spanish*/"los #explosivos en un laberinto# desvelan una Skulltula que otorga"},
  });

    hintTable[FIRE_TEMPLE_GS_SCARECROW_TOP] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-friendly scarecrow# atop a volcano hides", /*french*/"une #Skulltula repérée par l'épouvantail du volcan# a", /*spanish*/"un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider-friendly scarecrow# atop the Fire Temple hides", /*french*/"une #Skulltula repérée par l'épouvantail du temple du feu# a", /*spanish*/"un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga"}
  );

    hintTable[FIRE_TEMPLE_GS_SCARECROW_CLIMB] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider-friendly scarecrow# atop a volcano hides", /*french*/"une #Skulltula repérée par l'épouvantail du volcan# a", /*spanish*/"un #espantapájaros en lo alto de un volcán# custodia una Skulltula que otorga"},
                     }, {},
                       //clear text
                       Text{"a #spider-friendly scarecrow# atop the Fire Temple hides", /*french*/"une #Skulltula repérée par l'épouvantail du temple du feu# a", /*spanish*/"un #espantapájaros del Templo del Fuego# custodia una Skulltula que otorga"}
  );


    hintTable[FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider above a fiery maze# holds", /*french*/"une #Skulltula au dessus du labyrinthe enflammé du temple du feu# a", /*spanish*/"una #Skulltula sobre un ardiente laberinto# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider within a fiery maze# holds", /*french*/"une #Skulltula dans le labyrinthe enflammé du temple du feu# a", /*spanish*/"una #Skulltula en el interior de un ardiente laberinto# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR] = HintText::Exclude({
                       //obscure text
                       Text{"a #Goron trapped near lava# befriended a spider with", /*french*/"une #Skulltula emprisonnée près du lac de lave du temple du feu# a", /*spanish*/"una #Skulltula amiga de un Goron atrapado junto a la lava# otorga"},
  });

    hintTable[FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beside a fiery maze# holds", /*french*/"une #Skulltula près du labyrinthe enflammé du temple du feu# a", /*spanish*/"una #Skulltula junto a un ardiente laberinto# otorga"},
  });


    hintTable[WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a waterfall# in the Water Temple holds", /*french*/"une #Skulltula au dessus d'une cascade du temple de l'eau# a", /*spanish*/"una #Skulltula tras una cascada# del Templo del Agua otorga"},
  });

    hintTable[WATER_TEMPLE_GS_CENTRAL_PILLAR] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider in the center of the Water Temple# holds", /*french*/"une #Skulltula au centre du temple de l'eau# a", /*spanish*/"una #Skulltula en el centro del Templo del Agua# otorga"},
  });

    hintTable[WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST] = HintText::Exclude({
                       //obscure text
                       Text{"a spider protected by #rolling boulders under the lake# hides", /*french*/"une #Skulltula derrière les rochers roulants sous le lac# a", /*spanish*/"una #Skulltula protegida por rocas rodantes# bajo el lago otorga"},
                     }, {},
                       //clear text
                       Text{"a spider protected by #rolling boulders in the Water Temple# hides", /*french*/"une #Skulltula derrière les rochers roulants du temple de l'eau# a", /*spanish*/"una #Skulltula protegida por rocas rodantes# del Templo del Agua otorga"}
  );

    hintTable[WATER_TEMPLE_GS_RIVER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a river# in the Water Temple holds", /*french*/"une #Skulltula au dessus de la rivière du temple de l'eau# a", /*spanish*/"una #Skulltula sobre un río# del Templo del Agua otorga"},
  });


    hintTable[WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond a pit of lizards# is a spider holding", /*french*/"une #Skulltula près des lézards du temple de l'eau# a", /*spanish*/"#más allá de un pozo de reptiles# una Skulltula otorga"},
  });

    hintTable[WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY] = HintText::Exclude({
                       //obscure text
                       Text{"#lizards guard a spider# in the Water Temple with", /*french*/"une #Skulltula dans les couloirs croisés du temple de l'eau# a", /*spanish*/"unos #reptiles custodian una Skulltula# del Templo del Agua que otorga"},
  });

    hintTable[WATER_TEMPLE_MQ_GS_RIVER] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider over a river# in the Water Temple holds", /*french*/"une #Skulltula au dessus de la rivière du temple de l'eau# a", /*spanish*/"una #Skulltula sobre un río# del Templo del Agua otorga"},
  });


    hintTable[SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula au dessus d'un escalier du temple de l'esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_BOULDER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider behind a temporal stone# in the Spirit Temple yields", /*french*/"une #Skulltula derrière une pierre temporelle du temple de l'esprit# a", /*spanish*/"una #Skulltula tras un bloque temporal# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beside a statue# holds", /*french*/"une #Skulltula dans la grande salle du temple de l'esprit# a", /*spanish*/"una #Skulltula junto a una estatua# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider at the top of a deep shaft# in the Spirit Temple holds", /*french*/"une #Skulltula près d'un mur d'escalade du temple de l'esprit# a", /*spanish*/"una #Skulltula en lo alto de un gran hueco# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_GS_METAL_FENCE] = HintText::Exclude({
                       //obscure text
                       Text{"a child defeats a #spider among bats# in the Spirit Temple to gain", /*french*/"une #Skulltula sur le grillage du temple de l'esprit# a", /*spanish*/"el joven que derrote la #Skulltula entre murciélagos# del Templo del Espíritu hallará"},
  });


    hintTable[SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#above a pit of sand# in the Spirit Temple hides", /*french*/"une #Skulltula au dessus du trou sableux du temple de l'esprit# a", /*spanish*/"una #Skulltula sobre un pozo de arena# del Templo del Espíritu otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula dans la salle aux neuf trônes du temple de l'esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH] = HintText::Exclude({
                       //obscure text
                       Text{"a spider in the #hall of a knight# guards", /*french*/"une #Skulltula dans la salle aux neuf trônes du temple de l'esprit# a", /*spanish*/"una #Skulltula en el salón de un guerrero# otorga"},
  });

    hintTable[SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"#upon a web of glass# in the Spirit Temple sits a spider holding", /*french*/"une #Skulltula sur une paroi de verre du temple de l'esprit# a", /*spanish*/"#sobre una plataforma de cristal# yace una Skulltula que otorga"},
  });


    hintTable[SHADOW_TEMPLE_GS_SINGLE_GIANT_POT] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond a burning skull# lies a spider with", /*french*/"une #Skulltula derrière un crâne enflammé du temple de l'ombre# a", /*spanish*/"#tras una ardiente calavera# yace una Skulltula que otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beyond falling spikes# holds", /*french*/"une #Skulltula au delà de la pluie de clous du temple de l'ombre# a", /*spanish*/"una #Skulltula tras los pinchos del techo# otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT] = HintText::Exclude({
                       //obscure text
                       Text{"#beyond three burning skulls# lies a spider with", /*french*/"une #Skulltula derrière trois crânes enflammés du temple de l'ombre# a", /*spanish*/"#tras tres ardientes calaveras# yace una Skulltula que otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a spider guarded by #invisible blades# holds", /*french*/"une #Skulltula protégée par les faucheurs invisibles du temple de l'ombre# a", /*spanish*/"una #Skulltula custodiada por hojas invisibles# otorga"},
  });

    hintTable[SHADOW_TEMPLE_GS_NEAR_SHIP] = HintText::Exclude({
                       //obscure text
                       Text{"a spider near a #docked ship# hoards", /*french*/"une #Skulltula près du traversier du temple de l'ombre# a", /*spanish*/"una #Skulltula cercana a un navío# otorga"},
  });


    hintTable[SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beyond falling spikes# holds", /*french*/"une #Skulltula au delà de la pluie de clous du temple de l'ombre# a", /*spanish*/"una #Skulltula tras los pinchos del techo# otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider amidst roaring winds# in the Shadow Temple holds", /*french*/"une #Skulltula près des vents du temple de l'ombre# a", /*spanish*/"una #Skulltula entre ventarrones# del Templo de las Sombras otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_AFTER_WIND] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider beneath gruesome debris# in the Shadow Temple hides", /*french*/"une #Skulltula sous des débris du temple de l'ombre# a", /*spanish*/"una #Skulltula bajo unos horripilantes escombros# del Templo de las Sombras otorga"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_AFTER_SHIP] = HintText::Exclude({
                       //obscure text
                       Text{"a #fallen statue# reveals a spider with", /*french*/"une #Skulltula près de la statue écroulée du temple de l'ombre# a", /*spanish*/"una #estatua caída# revelará una Skulltula que otorgue"},
  });

    hintTable[SHADOW_TEMPLE_MQ_GS_NEAR_BOSS] = HintText::Exclude({
                       //obscure text
                       Text{"a #suspended spider# guards", /*french*/"une #Skulltula près du repère du temple de l'ombre# a", /*spanish*/"una #Skulltula flotante# del Templo de las Sombras otorga"},
  });


    hintTable[BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a cage# in the well holds", /*french*/"une #Skulltula dans une cage au fonds du puits# a", /*spanish*/"una #Skulltula enjaulada# del pozo otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"an #invisible path in the well# leads to", /*french*/"une #Skulltula dans le chemin invisible au fonds du puits# a", /*spanish*/"un #camino invisible del pozo# conduce a una Skulltula que otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a crypt# within the well guards", /*french*/"une #Skulltula embarrée dans la crypte au fonds du puits# a", /*spanish*/"una #Skulltula encerrada en una cripta# del pozo otorga"},
  });


    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT] = HintText::Exclude({
                       //obscure text
                       Text{"a #gauntlet of invisible spiders# protects", /*french*/"une #Skulltula protégée par les araignées invisibles au fonds du puits# a", /*spanish*/"unas #arañas invisibles# custodian una Skulltula que otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider crawling near the dead# in the well holds", /*french*/"une #Skulltula près des cercueils au fonds du puits# a", /*spanish*/"una #Skulltula junto a los muertos# del pozo otorga"},
  });

    hintTable[BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider locked in a crypt# within the well guards", /*french*/"une #Skulltula embarrée dans la crypte au fonds du puits# a", /*spanish*/"una #Skulltula encerrada en una cripta# del pozo otorga"},
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
                       Text{"night reveals a #spider in a graveyard# holding", /*french*/"une #Skulltula sur une façade du cimetière# a", /*spanish*/"la noche revela una #Skulltula del cementerio# que otorga"},
  });

    hintTable[GRAVEYARD_GS_BEAN_PATCH] = HintText::Exclude({
                       //obscure text
                       Text{"a #spider buried in a graveyard# holds", /*french*/"une #Skulltula enterrée dans le cimetière# a", /*spanish*/"una #Skulltula enterrada en el cementerio# otorga"},
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
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );

    hintTable[MARKET_POTION_SHOP_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"a #potion seller# offers", /*french*/"l'#apothicaire# vend", /*spanish*/"un #vendedor de pociones# ofrece"},
                     }, {},
                       //clear text
                       Text{"the #Market Potion Shop# offers", /*french*/"l'#apothicaire dans la place du marché# vend", /*spanish*/"la #tienda de pociones del mercado# ofrece"}
  );


    hintTable[MARKET_BAZAAR_ITEM_1] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_2] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_3] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_4] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_5] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_6] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_7] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
  });

    hintTable[MARKET_BAZAAR_ITEM_8] = HintText::Exclude({
                       //obscure text
                       Text{"the #Market Bazaar# offers", /*french*/"le #bazar de la place du marché# vend", /*spanish*/"el #bazar del mercado# ofrece"},
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


    hintTable[DEKU_TREE_MQ_DEKU_SCRUB] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in the Deku Tree# sells", /*french*/"la #peste Mojo dans l'arbre Mojo# vend", /*spanish*/"un #deku del Gran Árbol Deku# vende"},
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
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au cœur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterráneos# de la pradera sagrada venden"},
  });

    hintTable[SFM_DEKU_SCRUB_GROTTO_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub underground duo# sells", /*french*/"le #duo de peste Mojo au cœur du sanctuaire sylvestre# vend", /*spanish*/"un #par de dekus subterráneos# de la pradera sagrada venden"},
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


    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans la caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub guarded by Lizalfos# sells", /*french*/"la #peste Mojo au cœur de la caverne Dodongo# vend", /*spanish*/"un #deku custodiado por Lizalfos# vende"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans la caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_DEKU_SCRUB_LOBBY] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in Dodongo's Cavern# sells", /*french*/"la #peste Mojo dans l'entrée de la caverne Dodongo# vend", /*spanish*/"un #deku en la Cueva de los Dodongos# vende"},
  });


    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans l'entrée de la caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT] = HintText::Exclude({
                       //obscure text
                       Text{"a pair of #scrubs in Dodongo's Cavern# sells", /*french*/"le #duo de peste Mojo dans l'entrée de la caverne Dodongo# vend", /*spanish*/"un #par de dekus en la Cueva de los Dodongos# venden"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in Dodongo's Cavern# sells", /*french*/"la #peste Mojo au sommet des escaliers dans la caverne Dodongo# vend", /*spanish*/"un #deku en la Cueva de los Dodongos# vende"},
  });

    hintTable[DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub guarded by Lizalfos# sells", /*french*/"la #peste Mojo au cœur de la caverne Dodongo# vend", /*spanish*/"un #deku custodiado por Lizalfos# vende"},
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


    hintTable[JABU_JABUS_BELLY_DEKU_SCRUB] = HintText::Exclude({
                       //obscure text
                       Text{"a #scrub in a deity# sells", /*french*/"la #peste Mojo dans le ventre du gardien# vend", /*spanish*/"un #deku dentro de cierta deidad# vende"},
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


    hintTable[GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_DEKU_SCRUB_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });


    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
  });

    hintTable[GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT] = HintText::Exclude({
                       //obscure text
                       Text{"#scrubs in Ganon's Castle# sell", /*french*/"les #pestes Mojo dans le château de Ganon# vendent", /*spanish*/"los #dekus del Castillo de Ganon# venden"},
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

    /*--------------------------
    |    ENTRANCE HINT TEXT    |
    ---------------------------*/

    hintTable[DESERT_COLOSSUS_TO_COLOSSUS_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"lifting a #rock in the desert# reveals", /*french*/"soulever une #roche dans le désert# révèle", /*spanish*/"levantar una #roca del desierto# revela"},
  });

    hintTable[GV_GROTTO_LEDGE_TO_GV_OCTOROK_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a rock on #a ledge in the valley# hides", /*french*/"soulever une #roche dans la vallée# révèle", /*spanish*/"levantar una #roca al borde del valle# esconde"},
  });

    hintTable[GC_GROTTO_PLATFORM_TO_GC_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a #pool of lava# in Goron City blocks the way to", /*french*/"l'#étang de lave# dans le village Goron renferme", /*spanish*/"un #estanque de lava# en la Ciudad Goron bloquea el paso a"},
  });

    hintTable[GERUDO_FORTRESS_TO_GF_STORMS_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a #storm within Gerudo's Fortress# reveals", /*french*/"la #tempête dans la forteresse# révèle", /*spanish*/"una #tormenta en la Fortaleza Gerudo# revela"},
  });

    hintTable[ZORAS_DOMAIN_TO_ZD_STORMS_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a #storm within Zora's Domain# reveals", /*french*/"la #tempête dans le domaine des Zoras# révèle", /*spanish*/"una #tormenta en la Región de los Zora# revela"},
  });

    hintTable[HYRULE_CASTLE_GROUNDS_TO_HC_STORMS_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a #storm near the castle# reveals", /*french*/"la #tempête près du château# révèle", /*spanish*/"una #tormenta junto al castillo# revela"},
  });

    hintTable[GV_FORTRESS_SIDE_TO_GV_STORMS_GROTTO] = HintText::Entrance({
                     //obscure text
                     Text{"a #storm in the valley# reveals", /*french*/"la #tempête dans la vallée# révèle", /*spanish*/"una #tormenta en el valle# revela"},
  });

    hintTable[DESERT_COLOSSUS_TO_COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
                     //obscure text
                     Text{"a #fractured desert wall# hides", /*french*/"le #mur fragile du désert# cache", /*spanish*/"una #agrietada pared del desierto# esconde"},
  });

    hintTable[GANONS_CASTLE_GROUNDS_TO_OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
                     //obscure text
                     Text{"a #heavy pillar# outside the castle obstructs", /*french*/"le #rocher fragile près du château# cache", /*spanish*/"una #pesada columna# fuera del castillo obstruye"},
  });

    hintTable[ZORAS_FOUNTAIN_TO_ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Entrance({
                     //obscure text
                     Text{"a #fountain wall# hides", /*french*/"le #mur fragile du réservoir# cache", /*spanish*/"una #pared de la fuente# esconde"},
  });

    hintTable[GV_FORTRESS_SIDE_TO_GV_CARPENTER_TENT] = HintText::Entrance({
                     //obscure text
                     Text{"a #tent in the valley# covers", /*french*/"la #tente dans la vallée# recouvre", /*spanish*/"una #tienda de campaña del valle# cubre"},
  });

    hintTable[GRAVEYARD_WARP_PAD_REGION_TO_SHADOW_TEMPLE_ENTRYWAY] = HintText::Entrance({
                     //obscure text
                     Text{"at the #back of the Graveyard#, there is", /*french*/"#derrière le cimetière# gît", /*spanish*/"en la #parte trasera del cementerio# se halla"},
  });

    hintTable[LAKE_HYLIA_TO_WATER_TEMPLE_LOBBY] = HintText::Entrance({
                     //obscure text
                     Text{"deep #under a vast lake#, one can find", /*french*/"#sous le lac# gît", /*spanish*/"en las #profundidades de un lago inmenso# se halla"},
  });

    hintTable[GERUDO_FORTRESS_TO_GERUDO_TRAINING_GROUNDS_LOBBY] = HintText::Entrance({
                     //obscure text
                     Text{"paying a #fee to the Gerudos# grants access to", /*french*/"l'#entrée payante des Gerudo# donne accès à", /*spanish*/"pagarle una #tasa a las gerudo# da acceso a"},
  });

    hintTable[ZORAS_FOUNTAIN_TO_JABU_JABUS_BELLY_BEGINNING] = HintText::Entrance({
                     //obscure text
                     Text{"inside #Jabu-Jabu#, one can find", /*french*/"#dans Jabu-Jabu# se trouve", /*spanish*/"dentro de #Jabu-Jabu# se halla"},
  });

    hintTable[KAKARIKO_VILLAGE_TO_BOTTOM_OF_THE_WELL] = HintText::Entrance({
                     //obscure text
                     Text{"a #village well# leads to", /*french*/"dans le fond du #puits du village# gît", /*spanish*/"el #pozo de un pueblo# conduce a"},
  });

    /*--------------------------
    |      EXIT HINT TEXT      |
    ---------------------------*/
    //maybe make a new type for this? I'm not sure if it really matters

    hintTable[LINKS_POCKET] = HintText::Exclude({
                     //obscure text
                     Text{"Link's Pocket", /*french*/"les poches de @", /*spanish*/"el bolsillo de @"},
  });

    hintTable[KOKIRI_FOREST] = HintText::Exclude({
                     //obscure text
                     Text{"Kokiri Forest", /*french*/"la forêt Kokiri", /*spanish*/"el Bosque Kokiri"},
  });

    hintTable[THE_LOST_WOODS] = HintText::Exclude({
                     //obscure text
                     Text{"the Lost Woods", /*french*/"la forêt des méandres", /*spanish*/"el Bosque Perdido"},
  });

    hintTable[SACRED_FOREST_MEADOW] = HintText::Exclude({
                     //obscure text
                     Text{"Sacred Forest Meadow", /*french*/"le bosquet sacré", /*spanish*/"la pradera sagrada del bosque"},
  });

    hintTable[HYRULE_FIELD] = HintText::Exclude({
                     //obscure text
                     Text{"Hyrule Field", /*french*/"la plaine d'Hyrule", /*spanish*/"la Llanura de Hyrule"},
  });

    hintTable[LAKE_HYLIA] = HintText::Exclude({
                     //obscure text
                     Text{"Lake Hylia", /*french*/"le lac Hylia", /*spanish*/"el Lago Hylia"},
  });

    hintTable[GERUDO_VALLEY] = HintText::Exclude({
                     //obscure text
                     Text{"Gerudo Valley", /*french*/"la vallée Gerudo", /*spanish*/"el Valle Gerudo"},
  });

    hintTable[GERUDO_FORTRESS] = HintText::Exclude({
                     //obscure text
                     Text{"Gerudo's Fortress", /*french*/"la forteresse Gerudo", /*spanish*/"la Fortaleza Gerudo"},
  });

    hintTable[HAUNTED_WASTELAND] = HintText::Exclude({
                     //obscure text
                     Text{"Haunted Wasteland", /*french*/"le désert hanté", /*spanish*/"el desierto encantado"},
  });

    hintTable[DESERT_COLOSSUS] = HintText::Exclude({
                     //obscure text
                     Text{"Desert Colossus", /*french*/"le colosse du désert", /*spanish*/"el Coloso del Desierto"},
  });

    hintTable[THE_MARKET] = HintText::Exclude({
                     //obscure text
                     Text{"the Market", /*french*/"la place du marché", /*spanish*/"la plaza del mercado"},
  });

    hintTable[TEMPLE_OF_TIME] = HintText::Exclude({
                     //obscure text
                     Text{"Temple of Time", /*french*/"le temple du temps", /*spanish*/"el Templo del Tiempo"},
  });

    hintTable[HYRULE_CASTLE] = HintText::Exclude({
                     //obscure text
                     Text{"Hyrule Castle", /*french*/"le château d'Hyrule", /*spanish*/"el Castillo de Hyrule"},
  });

    hintTable[OUTSIDE_GANONS_CASTLE] = HintText::Exclude({
                     //obscure text
                     Text{"outside Ganon's Castle", /*french*/"les alentours du château de Ganon", /*spanish*/"el exterior del Castillo de Ganon"},
  });

    hintTable[KAKARIKO_VILLAGE] = HintText::Exclude({
                     //obscure text
                     Text{"Kakariko Village", /*french*/"le village Cocorico", /*spanish*/"Kakariko"},
  });

    hintTable[THE_GRAVEYARD] = HintText::Exclude({
                     //obscure text
                     Text{"the Graveyard", /*french*/"le cimetière", /*spanish*/"el cementerio"},
  });

    hintTable[DEATH_MOUNTAIN_TRAIL] = HintText::Exclude({
                     //obscure text
                     Text{"Death Mountain Trail", /*french*/"le chemin du mont de la Mort", /*spanish*/"el sendero de la Montaña de la Muerte"},
  });

    hintTable[GORON_CITY] = HintText::Exclude({
                     //obscure text
                     Text{"Goron City", /*french*/"le village Goron", /*spanish*/"la Ciudad Goron"},
  });

    hintTable[DEATH_MOUNTAIN_CRATER] = HintText::Exclude({
                     //obscure text
                     Text{"Death Mountain Crater", /*french*/"le cratère du mont de la Mort", /*spanish*/"el cráter de la Montaña de la Muerte"},
  });

    hintTable[ZORAS_RIVER] = HintText::Exclude({
                     //obscure text
                     Text{"Zora's River", /*french*/"le fleuve Zora", /*spanish*/"el Río Zora"},
  });

    hintTable[ZORAS_DOMAIN] = HintText::Exclude({
                     //obscure text
                     Text{"Zora's Domain", /*french*/"le domaine des Zoras", /*spanish*/"la Región de los Zora"},
  });

    hintTable[ZORAS_FOUNTAIN] = HintText::Exclude({
                     //obscure text
                     Text{"Zora's Fountain", /*french*/"la fontaine Zora", /*spanish*/"la Fuente Zora"},
  });

    hintTable[LON_LON_RANCH] = HintText::Exclude({
                     //obscure text
                     Text{"Lon Lon Ranch", /*french*/"le ranch Lon Lon", /*spanish*/"el Rancho Lon Lon"},
  });


    /*--------------------------
    |     REGION HINT TEXT     |
    ---------------------------*/

    hintTable[KF_LINKS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Link's House", /*french*/"la #maison de @#", /*spanish*/"la casa de @"},
  });

  //   hintTable[TOT_MAIN] = HintText::Region({
  //                    //obscure text
  //                    Text{"the #Temple of Time#", /*french*/"le #temple du temps#", /*spanish*/"el Templo del Tiempo"},
  // });

    hintTable[KF_MIDOS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Mido's house", /*french*/"la #cabane du Grand Mido#", /*spanish*/"la casa de Mido"},
  });

    hintTable[KF_SARIAS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Saria's House", /*french*/"la #cabane de Saria#", /*spanish*/"la casa de Saria"},
  });

    hintTable[KF_HOUSE_OF_TWINS] = HintText::Region({
                     //obscure text
                     Text{"the #House of Twins#", /*french*/"la #cabane des jumelles#", /*spanish*/"la casa de las gemelas"},
  });

    hintTable[KF_KNOW_IT_ALL_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Know-It-All Brothers' House", /*french*/"la #cabane des frères Je-Sais-Tout#", /*spanish*/"la casa de los hermanos Sabelotodo"},
  });

    hintTable[KF_KOKIRI_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Kokiri Shop#", /*french*/"le #magasin Kokiri#", /*spanish*/"la tienda kokiri"},
  });

    hintTable[LH_LAB] = HintText::Region({
                     //obscure text
                     Text{"the #Lakeside Laboratory#", /*french*/"le #laboratoire du lac#", /*spanish*/"el laboratorio del lago"},
  });

    hintTable[LH_FISHING_HOLE] = HintText::Region({
                     //obscure text
                     Text{"the #Fishing Pond#", /*french*/"l'#étang#", /*spanish*/"el estanque"},
  });

    hintTable[GV_CARPENTER_TENT] = HintText::Region({
                     //obscure text
                     Text{"the #Carpenters' tent#", /*french*/"la #tente des charpentiers#", /*spanish*/"la #tienda de campaña de los carpinteros#"},
  });

    hintTable[MARKET_GUARD_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"the #Guard House#", /*french*/"le #poste de garde#", /*spanish*/"la caseta de guardia"},
  });

    hintTable[MARKET_MASK_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Happy Mask Shop#", /*french*/"la #foire aux masques#", /*spanish*/"la tienda de La Máscara Feliz"},
  });

    hintTable[MARKET_BOMBCHU_BOWLING] = HintText::Region({
                     //obscure text
                     Text{"the #Bombchu Bowling Alley#", /*french*/"le #bowling teigneux#", /*spanish*/"la Bolera Bombchu"},
  });

    hintTable[MARKET_POTION_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Market Potion Shop#", /*french*/"l'#apothicaire de la place du marché#", /*spanish*/"la tienda de pociones de la plaza del mercado"},
  });

    hintTable[MARKET_TREASURE_CHEST_GAME] = HintText::Region({
                     //obscure text
                     Text{"the #Treasure Chest Shop#", /*french*/"la #chasse aux trésors#", /*spanish*/"el Cofre del Tesoro"},
  });

    hintTable[MARKET_BOMBCHU_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Bombchu Shop#", /*french*/"le #magasin de Bombchus#", /*spanish*/"la Tienda Bombchu"},
  });

    hintTable[MARKET_MAN_IN_GREEN_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Man in Green's House", /*french*/"la #maison de l'homme en vert#", /*spanish*/"la casa del hombre de verde"},
  });

    hintTable[KAK_WINDMILL] = HintText::Region({
                     //obscure text
                     Text{"the #Windmill#", /*french*/"le #moulin#", /*spanish*/"el #molino#"},
  });

    hintTable[KAK_CARPENTER_BOSS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"the #Carpenters' Boss House#", /*french*/"la #maison du chef des ouvriers#", /*spanish*/"la casa del capataz de los carpinteros"},
  });

    hintTable[KAK_HOUSE_OF_SKULLTULA] = HintText::Region({
                     //obscure text
                     Text{"the #House of Skulltula#", /*french*/"la #maison des Skulltulas#", /*spanish*/"la casa de las Skulltulas"},
  });

    hintTable[KAK_IMPAS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Impa's House", /*french*/"la #maison d'Impa#", /*spanish*/"la casa de Impa"},
  });

    hintTable[KAK_IMPAS_HOUSE_BACK] = HintText::Region({
                     //obscure text
                     Text{"Impa's cow cage", /*french*/"la #cage à vache d'Impa#", /*spanish*/"la jaula de la vaca de Impa"},
  });

    hintTable[KAK_ODD_POULTICE_BUILDING] = HintText::Region({
                     //obscure text
                     Text{"Granny's Potion Shop", /*french*/"la #maison bleue de Cocorico#", /*spanish*/"la tienda de pociones de la abuela"},
  });

    hintTable[GRAVEYARD_DAMPES_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Dampé's Hut", /*french*/"la #cabane du fossoyeur#", /*spanish*/"la cabaña de Dampé"},
  });

    hintTable[GC_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Goron Shop#", /*french*/"la #boutique Goron#", /*spanish*/"la #tienda goron#"},
  });

    hintTable[ZD_SHOP] = HintText::Region({
                     //obscure text
                     Text{"the #Zora Shop#", /*french*/"la #boutique Zora#", /*spanish*/"la #tienda zora#"},
  });

    hintTable[LLR_TALONS_HOUSE] = HintText::Region({
                     //obscure text
                     Text{"Talon's House", /*french*/"la #maison de Talon#", /*spanish*/"la casa de Talon"},
  });

    hintTable[LLR_STABLES] = HintText::Region({
                     //obscure text
                     Text{"a #stable#", /*french*/"l'#étable#", /*spanish*/"el establo"},
  });

    hintTable[LLR_TOWER] = HintText::Region({
                     //obscure text
                     Text{"the #Lon Lon Tower#", /*french*/"le #silo du ranch Lon Lon#", /*spanish*/"la torre Lon Lon"},
  });

    hintTable[MARKET_BAZAAR] = HintText::Region({
                     //obscure text
                     Text{"the #Market Bazaar#", /*french*/"le #bazar de la place du marché#", /*spanish*/"el bazar de la plaza del mercado"},
  });

    hintTable[MARKET_SHOOTING_GALLERY] = HintText::Region({
                     //obscure text
                     Text{"a #Slingshot Shooting Gallery#", /*french*/"le #jeu d'adresse de la place du marché#", /*spanish*/"el Tiro al Blanco con tirachinas"},
  });

    hintTable[KAK_BAZAAR] = HintText::Region({
                     //obscure text
                     Text{"the #Kakariko Bazaar#", /*french*/"le #bazar de Cocorico#", /*spanish*/"el bazar de Kakariko"},
  });

    hintTable[KAK_POTION_SHOP_FRONT] = HintText::Region({
                     //obscure text
                     Text{"the #Kakariko Potion Shop#", /*french*/"l'#apothicaire de Cocorico#", /*spanish*/"la tienda de pociones de Kakariko"},
  });

    hintTable[KAK_POTION_SHOP_BACK] = HintText::Region({
                     //obscure text
                     Text{"the #Kakariko Potion Shop#", /*french*/"l'#apothicaire de Cocorico#", /*spanish*/"la tienda de pociones de Kakariko"},
  });

    hintTable[KAK_SHOOTING_GALLERY] = HintText::Region({
                     //obscure text
                     Text{"a #Bow Shooting Gallery#", /*french*/"le #jeu d'adresse de Cocorico#", /*spanish*/"el Tiro al Blanco con arco"},
  });

    hintTable[COLOSSUS_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée du colosse#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[HC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée du château#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[OGC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée des ruines#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[DMC_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée du volcan#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[DMT_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée de la montagne#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[ZF_GREAT_FAIRY_FOUNTAIN] = HintText::Region({
                     //obscure text
                     Text{"a #Great Fairy Fountain#", /*french*/"la #grande fée de la fontaine#", /*spanish*/"una fuente de la Gran Hada"},
  });

    hintTable[GRAVEYARD_SHIELD_GRAVE] = HintText::Region({
                     //obscure text
                     Text{"a #grave with a free chest#", /*french*/"le #tombeau avec un trésor#", /*spanish*/"la #tumba con un cofre#"},
  });

    hintTable[GRAVEYARD_HEART_PIECE_GRAVE] = HintText::Region({
                     //obscure text
                     Text{"a chest spawned by #Sun's Song#", /*french*/"le #tombeau avec un mort#", /*spanish*/"la #tumba de la Canción del Sol#"},
  });

    hintTable[GRAVEYARD_COMPOSERS_GRAVE] = HintText::Region({
                     //obscure text
                     Text{"the #Composers' Grave#", /*french*/"la #tombe royale#", /*spanish*/"el #Panteón Real#"},
  });

    hintTable[GRAVEYARD_DAMPES_GRAVE] = HintText::Region({
                     //obscure text
                     Text{"Dampé's Grave", /*french*/"la #tombe d'Igor#", /*spanish*/"la #tumba de Dampé#"},
  });

    hintTable[DMT_COW_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a solitary #Cow#", /*french*/"la #grotte avec une vache#", /*spanish*/"una #vaca# solitaria"},
  });

    hintTable[HC_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a sandy grotto with #fragile walls#", /*french*/"la #grotte avec des murs fragiles#", /*spanish*/"la arenosa gruta de #frágiles paredes#"},
  });

    hintTable[HF_TEKTITE_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a pool guarded by a #Tektite#", /*french*/"l'#étang sous-terrain avec un Araknon#", /*spanish*/"un charco custodiado por un #Tektite#"},
  });

    hintTable[HF_NEAR_KAK_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #Big Skulltula# guarding a Gold one", /*french*/"la #grotte d'araignées#", /*spanish*/"una #gran Skulltula# custodiando una dorada"},
  });

    hintTable[HF_COW_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a grotto full of #spider webs#", /*french*/"la #grotte couverte de toiles d'araignées#", /*spanish*/"una gruta llena de #telarañas#"},
  });

    hintTable[KAK_REDEAD_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"#ReDeads# guarding a chest", /*french*/"le tombeau de #deux morts#", /*spanish*/"los #ReDeads# que custodian un cofre"},
  });

    hintTable[SFM_WOLFOS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"#Wolfos# guarding a chest", /*french*/"la #grotte iridescente#", /*spanish*/"los #Wolfos# que custodian un cofre"},
  });

    hintTable[GV_OCTOROK_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"an #Octorok# guarding a rich pool", /*french*/"un #étang sous-terrain avec un Octorok#", /*spanish*/"un #Octorok# que custodia un lujoso charco"},
  });

    hintTable[DEKU_THEATER] = HintText::Region({
                     //obscure text
                     Text{"the #Lost Woods Stage#", /*french*/"le #théâtre sylvestre#", /*spanish*/"el #escenario del Bosque Perdido#"},
  });

    hintTable[ZR_OPEN_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[DMC_UPPER_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[DMT_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[KAK_OPEN_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[HF_NEAR_MARKET_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[HF_OPEN_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[HF_SOUTHEAST_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[KF_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[LW_NEAR_SHORTCUTS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #generic grotto#", /*french*/"une #grotte avec un trésor#", /*spanish*/"una #cueva genérica#"},
  });

    hintTable[HF_INSIDE_FENCE_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a #single Upgrade Deku Scrub#", /*french*/"une #grotte avec une peste Mojo#", /*spanish*/"una cueva con un #solitario mercader deku#"},
  });

    hintTable[LW_SCRUBS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"#2 Deku Scrubs# including an Upgrade one", /*french*/"une #grotte avec deux pestes Mojo#", /*spanish*/"una cueva con #dos mercaderes deku#"},
  });

    hintTable[COLOSSUS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"2 Deku Scrubs", /*french*/"une #grotte avec deux pestes Mojo#", /*spanish*/"una cueva con #dos mercaderes deku#"},
  });

    hintTable[ZR_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"2 Deku Scrubs", /*french*/"une #grotte avec deux pestes Mojo#", /*spanish*/"una cueva con #dos mercaderes deku#"},
  });

    hintTable[SFM_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"2 Deku Scrubs", /*french*/"une #grotte avec deux pestes Mojo#", /*spanish*/"una cueva con #dos mercaderes deku#"},
  });

    hintTable[GV_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"2 Deku Scrubs", /*french*/"une #grotte avec deux pestes Mojo#", /*spanish*/"una cueva con #dos mercaderes deku#"},
  });

    hintTable[LH_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"3 Deku Scrubs", /*french*/"une #grotte avec trois pestes Mojo#", /*spanish*/"una cueva con #tres mercaderes deku#"},
  });

    hintTable[DMC_HAMMER_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"3 Deku Scrubs", /*french*/"une #grotte avec trois pestes Mojo#", /*spanish*/"una cueva con #tres mercaderes deku#"},
  });

    hintTable[GC_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"3 Deku Scrubs", /*french*/"une #grotte avec trois pestes Mojo#", /*spanish*/"una cueva con #tres mercaderes deku#"},
  });

    hintTable[LLR_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"3 Deku Scrubs", /*french*/"une #grotte avec trois pestes Mojo#", /*spanish*/"una cueva con #tres mercaderes deku#"},
  });

    hintTable[ZR_FAIRY_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a small #Fairy Fountain#", /*french*/"une #fontaine de fées#", /*spanish*/"una pequeña #fuente de hadas#"},
  });

    hintTable[HF_FAIRY_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a small #Fairy Fountain#", /*french*/"une #fontaine de fées#", /*spanish*/"una pequeña #fuente de hadas#"},
  });

    hintTable[SFM_FAIRY_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a small #Fairy Fountain#", /*french*/"une #fontaine de fées#", /*spanish*/"una pequeña #fuente de hadas#"},
  });

    hintTable[ZD_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a small #Fairy Fountain#", /*french*/"une #fontaine de fées#", /*spanish*/"una pequeña #fuente de hadas#"},
  });

    hintTable[GF_STORMS_GROTTO] = HintText::Region({
                     //obscure text
                     Text{"a small #Fairy Fountain#", /*french*/"une #fontaine de fées#", /*spanish*/"una pequeña #fuente de hadas#"},
  });

    /*--------------------------
    |      JUNK HINT TEXT      |
    ---------------------------*/

    hintTable[JUNK01] = HintText::Junk({
                     //obscure text
                     Text{"Remember to check your 3DS battery level&and save often.", /*french*/"Surveillez votre batterie 3DS et sauvegardez souvent!", /*spanish*/"No te olvides de revisar la batería de la 3DS y guarda partida de vez en cuando."},
    });

    hintTable[JUNK02] = HintText::Junk({
                     //obscure text
                     Text{"They say you must read the names of \"Special Deal\" shop items carefully.", /*french*/"Selon moi, les « Offres spéciales » sont parfois trompeuses... Lisez attentivement!", /*spanish*/"Según dicen, se debería prestar atención a los nombres de las ofertas especiales."},
    });

    hintTable[JUNK03] = HintText::Junk({
                     //obscure text
                     Text{"They say that Zelda is a poor leader.", /*french*/"Selon moi, Zelda ne ferait pas un bon monarque.", /*spanish*/"Según dicen, Zelda es mala líder."},
    });

    hintTable[JUNK04] = HintText::Junk({
                     //obscure text
                     Text{"These hints can be quite useful. This is an exception.", /*french*/"Ces indices sont très utiles, à l'exception de celui-ci.", /*spanish*/"Las pistas suelen servir de ayuda. En cambio, esta no."},
    });

    hintTable[JUNK05] = HintText::Junk({
                     //obscure text
                     Text{"They say that the Lizalfos in Dodongo's Cavern like to play in lava.", /*french*/"Selon moi, les Lézalfos de la caverne Dodongo aiment patauger dans la lave.", /*spanish*/"Según dicen, a los Lizalfos de la Cueva de los Dodongos les gusta jugar en la lava."},
    });

    hintTable[JUNK06] = HintText::Junk({
                     //obscure text
                     Text{"They say that all the Zora drowned in Wind Waker.", /*french*/"Selon moi, les Zoras se sont noyés dans Wind Waker.", /*spanish*/"Según dicen, en Wind Waker todos los zora se ahogaron."},
    });

    hintTable[JUNK07] = HintText::Junk({
                     //obscure text
                     Text{"If Gorons eat rocks, does that mean I'm in danger?", /*french*/"Ne dis pas au Gorons que je suis ici. Ils mangent des roches, tu sais!", /*spanish*/"Si los Goron se tragan las piedras, ¿no me hace ser una especia vulnarable o algo así"},
    });

    hintTable[JUNK08] = HintText::Junk({
                     //obscure text
                     Text{"'Member when Ganon was a blue pig?^I 'member.", /*french*/"Dans mon temps, Ganon était un cochon bleu...^Pff! Les jeunes de nos jours, et leur Ganondorf!", /*spanish*/"¿T'acuerdas cuando Ganon era un cerdo azul?^Qué tiempos, chico."},
    });

    hintTable[JUNK09] = HintText::Junk({
                     //obscure text
                     Text{"One who does not have Triforce can't go in.", /*french*/"Ceux sans Triforce doivent rebrousser chemin.", /*spanish*/"Aquel que no porte la Trifuerza no podrá pasar."},
    });

    hintTable[JUNK10] = HintText::Junk({
                     //obscure text
                     Text{"Save your future, end the Happy Mask Salesman.", /*french*/"Selon moi, tu t'éviteras des jours de malheur si tu vaincs le vendeur de masques...", /*spanish*/"Salva tu futuro, acaba con el dueño de La Máscara Feliz."},
    });

    hintTable[JUNK11] = HintText::Junk({
                     //obscure text
                     Text{"Glitches are a pathway to many abilities some consider to be... Unnatural.", /*french*/"Les glitchs sont un moyen d'acquérir de nombreuses facultés considérées par certains comme... contraire à la nature.", /*spanish*/"Los glitches son el camino a muchas habilidades que varios consideran... nada natural."},
    });

    hintTable[JUNK12] = HintText::Junk({
                     //obscure text
                     Text{"I'm stoned. Get it?", /*french*/"Allez, roche, papier, ciseau...&Roche.", /*spanish*/"Me he quedado de piedra. ¿Lo pillas?"},
    });

    hintTable[JUNK13] = HintText::Junk({
                     //obscure text
                     Text{"Hoot! Hoot! Would you like me to repeat that?", /*french*/"Hou hou! Veux-tu que je répète tout ça?", /*spanish*/"¡Buuu, buuu! ¿Te lo vuelvo a repetir?"},
    });

    hintTable[JUNK14] = HintText::Junk({
                     //obscure text
                     Text{"Gorons are stupid. They eat rocks.", /*french*/"Les Gorons sont des vraies têtes dures.", /*spanish*/"Los Goron son tontos. Se comen las piedras."},
    });

    hintTable[JUNK15] = HintText::Junk({
                     //obscure text
                     Text{"They say that Lon Lon Ranch prospered under Ingo.", /*french*/"Selon moi, le ranch Lon Lon était plus prospère sous Ingo.", /*spanish*/"Según dicen, el Rancho Lon Lon prosperó gracias a Ingo."},
    });

    hintTable[JUNK16] = HintText::Junk({
                     //obscure text
                     Text{"The single rupee is a unique item.", /*french*/"Nul objet n'est plus unique que le rubis vert.", /*spanish*/"La rupia de uno es un objeto singular."},
    });

    hintTable[JUNK17] = HintText::Junk({
                     //obscure text
                     Text{"Without the Lens of Truth, the Treasure Chest Mini-Game is a 1 out of 32 chance.^Good luck!", /*french*/"Gagner la chasse aux trésors est 1 chance sur 32.^Bonne chance!", /*spanish*/"Sin la Lupa de la Verdad, ganarías 1/32 veces en el Cofre del Tesoro.^¡Buena suerte con ello!"},
    });

    hintTable[JUNK18] = HintText::Junk({
                     //obscure text
                     Text{"Use bombs wisely.", /*french*/"Utilise les bombes avec précaution.", /*spanish*/"No desperdicies las bombas."},
    });

    hintTable[JUNK19] = HintText::Junk({
                     //obscure text
                     Text{"They say that Volvagia hates splinters", /*french*/"Selon moi, Volvagia déteste les échardes.", /*spanish*/"Según dicen, Volvagia le teme a las astillas."},
    });

    hintTable[JUNK20] = HintText::Junk({
                     //obscure text
                     Text{"They say that funky monkeys can be spotted on Friday.", /*french*/"Selon moi, des capucins coquins sortent le vendredi.", /*spanish*/"Según dicen, en los viernes puedes hallar monos marchosos."},
    });

    hintTable[JUNK21] = HintText::Junk({
                     //obscure text
                     Text{"I found you, faker!", /*french*/"Ah-ha! Je t'ai trouvé!", /*spanish*/"¡Ahí estás, impostor!"},
    });

    hintTable[JUNK22] = HintText::Junk({
                     //obscure text
                     Text{"They say the Groose is loose.", /*french*/"Selon moi, Hergo est le vrai héros.", /*spanish*/"Según dicen, Malton es un espanto."},
    });

    hintTable[JUNK23] = HintText::Junk({
                     //obscure text
                     Text{"They say that players who select the \"ON\" option for \"MOTION CONTROL\" are the real \"Zelda players!\"", /*french*/"Selon moi, ceux qui utilisent les contrôles gyroscopiques sont les VRAIS joueurs.", /*spanish*/"Según dicen, aquellos que juegan usando el control por movimiento son los verdaderos jugadores de Zelda."},
    });

    hintTable[JUNK24] = HintText::Junk({
                     //obscure text
                     Text{"What happened to Sheik?", /*french*/"Donc... Qu'est-ce qui arrive avec Sheik?", /*spanish*/"¿Qué la habrá pasado a Sheik?"},
    });

    hintTable[JUNK25] = HintText::Junk({
                     //obscure text
                     Text{"L2P @.", /*french*/"Arrête de lire les indices et joue comme un grand, @.", /*spanish*/"Mira que eres novato, @."},
    });

    hintTable[JUNK26] = HintText::Junk({
                     //obscure text
                     Text{"I've heard you can cheat at Sploosh Kaboom.", /*french*/"Selon moi, il y a une carte aux trésors à Mercantîle... Duh!", /*spanish*/"He oído por ahí que puedes hacer trampa en el Sploosh Kaboom."},
    });

    hintTable[JUNK27] = HintText::Junk({
                     //obscure text
                     Text{"I'm Lonk from Pennsylvania.", /*french*/"Je suis Lonk, le héros de Pennsylvanie!", /*spanish*/"Soy Lonk, de Pensilvania."},
    });

    hintTable[JUNK28] = HintText::Junk({
                     //obscure text
                     Text{"I bet you'd like to have more bombs.", /*french*/"Je parie que tu veux plus de bombes.", /*spanish*/"Me apuesto a que quisieras tener más bombas."},
    });

    hintTable[JUNK29] = HintText::Junk({
                     //obscure text
                     Text{"When all else fails, use Fire.", /*french*/"Quand rien ne marche, utilise le feu.", /*spanish*/"Cuando nada funcione, usa el fuego."},
    });

    hintTable[JUNK30] = HintText::Junk({
                     //obscure text
                     Text{"Here's a hint, @. Don't be bad.", /*french*/"Selon moi, la #Triforce# n'est pas dans le jeu... Duh!", /*spanish*/"Aquí tienes una pista, @: deja de ser manco."},
    });

    hintTable[JUNK31] = HintText::Junk({
                     //obscure text
                     Text{"Game Over. Return of Ganon.", /*french*/"Partie terminée. RETOUR DE GANON.", /*spanish*/"Fin de la partida. El regreso de Ganon."},
    });

    hintTable[JUNK32] = HintText::Junk({
                     //obscure text
                     Text{"May the way of the Hero lead to the Triforce.", /*french*/"Que le chemin du héros te mène à la Triforce.", /*spanish*/"Puede que la senda del héroe te lleve hacia la Trifuerza."},
    });

    hintTable[JUNK33] = HintText::Junk({
                     //obscure text
                     Text{"Can't find an item? Scan an Amiibo.", /*french*/"Tu cherches de quoi? Utilise un Amiibo!", /*spanish*/"¿No encuentras algo? Escanea un amiibo."},
    });

    hintTable[JUNK34] = HintText::Junk({
                     //obscure text
                     Text{"They say this game has just a few glitches.", /*french*/"Selon moi, ce jeu est complètement exempt de glitchs.", /*spanish*/"Dicen que este juego apenas tiene glitches."},
    });

    hintTable[JUNK35] = HintText::Junk({
                     //obscure text
                     Text{"BRRING BRRING This is Ulrira. Wrong number?", /*french*/"DRING DRING!! Pépé le Ramollo à l'appareil... Quoi? Faux numéro?", /*spanish*/"¡Ring! ¡Ring! Al habla Ulrira. ¿Me he equivocado de número?"},
    });

    hintTable[JUNK36] = HintText::Junk({
                     //obscure text
                     Text{"Tingle Tingle Kooloo Limpah!", /*french*/"Tingle! Tingle! Kooloolin... Pah!", /*spanish*/"Tingle, Tingle, Kurulín... ¡PA!"},
    });

    hintTable[JUNK37] = HintText::Junk({
                     //obscure text
                     Text{"L is real 2401", /*french*/"L is real 2401", /*spanish*/"L es real 2401."},
    });

    hintTable[JUNK38] = HintText::Junk({
                     //obscure text
                     Text{"They say that Ganondorf will appear in the next Mario Tennis.", /*french*/"Selon moi, Ganondorf sera la nouvelle recrue dans Mario Tennis.", /*spanish*/"Según dicen, Ganondorf estará en el próximo Mario Tennis."},
    });

    hintTable[JUNK39] = HintText::Junk({
                     //obscure text
                     Text{"Medigoron sells the earliest Breath of the Wild demo.", /*french*/"Selon moi, Medigoron vend une démo de #Breath of the Wild#.", /*spanish*/"Medigoron vende la primera demo del Breath of the Wild."},
    });

    hintTable[JUNK40] = HintText::Junk({
                     //obscure text
                     Text{"Can you move me? I don't get great service here.", /*french*/"Peux-tu me déplacer? J'ai pas une bonne réception ici.", /*spanish*/"¿Puedes llevarme a otro lado? Aquí nadie me presta atención."},
    });

    hintTable[JUNK41] = HintText::Junk({
                     //obscure text
                     Text{"They say if you use Strength on the truck, you can find Mew.", /*french*/"Selon moi, #Mew# se trouve dessous le camion... Duh!", /*spanish*/"Según dicen, puedes hallar un Mew usando Fuerza contra el camión de Ciudad Carmín."},
    });

    hintTable[JUNK42] = HintText::Junk({
                     //obscure text
                     Text{"I'm a helpful hint Gossip Stone!^See, I'm helping.", /*french*/"Salut! Je suis une pierre de bons conseils!^Tiens, tu vois? J'aide bien, hein?", /*spanish*/"Soy una Piedra Sheikah muy útil.^¡Mira cómo te ayudo!"},
    });

    hintTable[JUNK43] = HintText::Junk({
                     //obscure text
                     Text{"Dear @, please come to the castle. I've baked a cake for you.&Yours truly, Princess Zelda.", /*french*/"Mon très cher @:&Viens vite au château, je t'ai préparé un délicieux gâteau...&À bientôt, Princesse Zelda", /*spanish*/"Querido @: Por favor, ven al castillo. He hecho una tarta para ti.&Sinceramente tuya: Princesa Zelda."},
    });

    hintTable[JUNK44] = HintText::Junk({
                     //obscure text
                     Text{"They say all toasters toast toast.", /*french*/"Selon moi, les grille-pains grillent du pain.", /*spanish*/"Según dicen, todas las tostadoras tostan tostadas tostadas."},
    });

    hintTable[JUNK45] = HintText::Junk({
                     //obscure text
                     Text{"You thought it would be a useful hint, but it was me, junk hint!", /*french*/"Tu t'attendais à un bon indice... Mais c'était moi, un mauvais indice!", /*spanish*/"Je... Creeías que iba a ser una piedra de utilidad, ¡pero no, era yo, la piedra de la agonía!"},
    });

    hintTable[JUNK46] = HintText::Junk({
                     //obscure text
                     Text{"They say that quest guidance can be found at a talking rock.", /*french*/"Selon moi, des #indices# se trouvent auprès d'une pierre parlante... Duh!", /*spanish*/"Según dicen, puedes consultarle ayuda a rocas parlanchinas."},
    });

    hintTable[JUNK47] = HintText::Junk({
                     //obscure text
                     Text{"They say that the final item you're looking for can be found somewhere in Hyrule.", /*french*/"Selon moi, le #dernier objet# se trouve quelque part dans Hyrule... Duh!", /*spanish*/"Según dicen, el último objeto que te falte puede estar en cualquier rincón de Hyrule."},
    });

    hintTable[JUNK48] = HintText::Junk({
                     //obscure text
                     Text{"Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.", /*french*/"Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.^Mwip.", /*spanish*/"Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep.^Mweep."},
    });

    hintTable[JUNK49] = HintText::Junk({
                     //obscure text
                     Text{"They say that Barinade fears Deku Nuts.", /*french*/"Selon moi, Barinade a la frousse des noix Mojo.", /*spanish*/"Según dicen, lo que más teme a Barinade son las nueces deku."},
    });

    hintTable[JUNK50] = HintText::Junk({
                     //obscure text
                     Text{"They say that Flare Dancers do not fear Goron-crafted blades.", /*french*/"Selon moi, le danse-flamme n'a pas peur des armes de Goron.", /*spanish*/"Según dicen, los Bailafuegos no le temen a las armas forjadas por Gorons."},
    });

    hintTable[JUNK51] = HintText::Junk({
                     //obscure text
                     Text{"They say that Morpha is easily trapped in a corner.", /*french*/"Selon moi, Morpha est facilement coincé.", /*spanish*/"Según dicen, puedes atrapar a Morpha con facilidad en una esquina."},
    });

    hintTable[JUNK52] = HintText::Junk({
                     //obscure text
                     Text{"They say that Bongo Bongo really hates the cold.", /*french*/"Selon moi, Bongo Bongo a facilement froid aux doigts.", /*spanish*/"Según dicen, Bongo Bongo odia a muerte el frío."},
    });

    hintTable[JUNK53] = HintText::Junk({
                     //obscure text
                     Text{"They say that your sword is most powerful when you put it away.", /*french*/"Selon moi, ton épée est à pleine puissance quand tu la rengaines.", /*spanish*/"Según dicen, tu espada se vuelve más poderosa si la guardas."},
    });

    hintTable[JUNK54] = HintText::Junk({
                     //obscure text
                     Text{"They say that bombing the hole Volvagia last flew into can be rewarding.", /*french*/"Selon moi, le trou où se creuse Volvagia est vulnérable aux bombes.", /*spanish*/"Según dicen, trae buena suerte colocar una bomba en el último agujero de donde salió Volvagia."},
    });

    hintTable[JUNK55] = HintText::Junk({
                     //obscure text
                     Text{"They say that invisible ghosts can be exposed with Deku Nuts.", /*french*/"Selon moi, des fantômes invisibles apparaissent avec des noix Mojo.", /*spanish*/"Según dicen, puedes exponer a los espectros invisibles con nueces deku."},
    });

    hintTable[JUNK56] = HintText::Junk({
                     //obscure text
                     Text{"They say that the real Phantom Ganon is bright and loud.", /*french*/"Selon moi, le vrai spectre de Ganon est clair et bruyant.", /*spanish*/"Según dicen, el verdadero Ganon Fantasma es brillante y ruidoso."},
    });

    hintTable[JUNK57] = HintText::Junk({
                     //obscure text
                     Text{"They say that walking backwards is very fast.", /*french*/"Selon moi, tu fais marche arrière très rapidement pour un héros.", /*spanish*/"Según dicen, es más rápido caminar hacia atrás."},
    });

    hintTable[JUNK58] = HintText::Junk({
                     //obscure text
                     Text{"They say that leaping above the Market entrance enriches most children.", /*french*/"Selon moi, les enfants riches se pavanent en haut du pont-levis.", /*spanish*/"Según dicen, saltar por las cadenas a la entrada de la plaza enriquece a muchos chiquillos."},
    });

    hintTable[JUNK59] = HintText::Junk({
                     //obscure text
                     Text{"They say Ingo is not very good at planning ahead.", /*french*/"Selon moi, Ingo ne fait pas un très bon geôlier.", /*spanish*/"Según dicen, a Ingo no se le da especialmente bien planificar con antelación."},
    });

    hintTable[JUNK60] = HintText::Junk({
                     //obscure text
                     Text{"You found a spiritual Stone! By which I mean, I worship Nayru.", /*french*/"Vous avez trouvé une pierre spirituelle! En effet, je vénère la déesse Hylia.", /*spanish*/"¡Has encontrado una piedra espiritual! Es que le rindo culto a Nayru..."},
    });

    hintTable[JUNK61] = HintText::Junk({
                     //obscure text
                     Text{"They say that a flying strike with a Deku Stick is no stronger than a grounded one.", /*french*/"Selon moi, un coup de bâton sauté n'est pas meilleur qu'au sol.", /*spanish*/"Según dicen, los golpes aéreos con palos deku son tan fuertes como los normales."},
    });

    hintTable[JUNK62] = HintText::Junk({
                     //obscure text
                     Text{"Open your eyes.^Open your eyes.^Wake up, @.", /*french*/"Réveille-toi...^Réveille-toi.^Ouvre les yeux, @.", /*spanish*/"Abre los ojos...^Abre los ojos...^Despierta, @..."},
    });

    hintTable[JUNK63] = HintText::Junk({
                     //obscure text
                     Text{"They say that the Nocturne of Shadow can bring you very close to Ganon.", /*french*/"Selon moi, le nocturne de l'ombre peut t'amener très près de Ganon.", /*spanish*/"Según dicen, el Nocturno de la sombra te puede acercar mucho a Ganon."},
    });

    hintTable[JUNK64] = HintText::Junk({
                     //obscure text
                     Text{"They say that Twinrova always casts the same spell the first three times.", /*french*/"Selon moi, Twinrova lance toujours les mêmes trois premiers sorts.", /*spanish*/"Según dicen, Birova siempre lanza el mismo hechizo las tres primeras veces."},
    });

    hintTable[JUNK65] = HintText::Junk({
                     //obscure text
                     Text{"They say that the nightly builds may be unstable.", /*french*/"Selon moi, les « nightly builds » peuvent être instables.", /*spanish*/"Según dicen, las últimas nightlies pueden llegar a ser algo inestables."},
    });

    hintTable[JUNK66] = HintText::Junk({
                     //obscure text
                     Text{"You're playing a Randomizer. I'm randomized!^Here's a random number:  #4#.&Enjoy your Randomizer!", /*french*/"Tu joues à un randomizer. Je suis aléatoire!^Voici un nombre aléatoire: #4#.&Bonne partie!", /*spanish*/"¡Estás jugando un Randomizer! ¡Yo también estoy aleatorizada!^Aquí tienes un número aleatorio:  #8#.&¡Diviértete!"},
    });

    hintTable[JUNK67] = HintText::Junk({
                     //obscure text
                     Text{"They say Ganondorf's bolts can be reflected with glass or steel.", /*french*/"Selon moi, les éclairs de Ganon se reflètent sur l'acier et le verre.", /*spanish*/"Según dicen, puedes reflejar las esferas de energía de Ganondorf con cristal y acero."},
    });

    hintTable[JUNK68] = HintText::Junk({
                     //obscure text
                     Text{"They say Ganon's tail is vulnerable to nuts, arrows, swords, explosives, hammers...^...sticks, seeds, boomerangs...^...rods, shovels, iron balls, angry bees...", /*french*/"Selon moi, la queue de Ganon est vulnérable aux noix, flèches, épées, bombes, marteaux...^...bâtons, graines, boomerangs...^...baguettes, pelles, boulets de fer, abeilles enragées...", /*spanish*/"Según dicen, la cola de Ganon es vulnerable a nueces, flechas, espadas, explosivos, martillos...^...palos, semillas, bumeráns...^...cetros, palas, bolas de hierro, abejas..."},
    });

    hintTable[JUNK69] = HintText::Junk({
                     //obscure text
                     Text{"They say that you're wasting time reading this hint, but I disagree. Talk to me again!", /*french*/"Selon moi... tu sais quoi? Parle-moi encore, et je te le dirai!", /*spanish*/"Según dicen, pierdes el tiempo en leer esta pista, pero no pienso igual. ¡Vuelve a hablarme, ya verás!"},
    });

    hintTable[JUNK70] = HintText::Junk({
                     //obscure text
                     Text{"They say Ganondorf knows where to find the instrument of his doom.", /*french*/"Selon moi, Ganondorf sait où il a caché son point faible.", /*spanish*/"Según dicen, Ganondorf sabe dónde hallar el instrumento de su perdición."},
    });

    hintTable[JUNK71] = HintText::Junk({
                     //obscure text
                     Text{"I heard @ is pretty good at Zelda.", /*french*/"Apparemment, @ est super bon à Zelda.", /*spanish*/"He oído que a @ se le dan muy bien los Zelda."},
    });

    hintTable[JUNK72] = HintText::Junk({
                     //obscure text
                     Text{"Hi @, we've been trying to reach you about your car's extended warranty. ", /*french*/"Bonjour, @. Vous avez une voiture? Vous savez, nous offrons des assurances abordables...", /*spanish*/"Buenas, @. Le llamamos para ofrecerle un nuevo seguro de hogar que puede pagar en cómodos plazos, sin intereses ni comisiones."},
    });

    hintTable[JUNK73] = HintText::Junk({
                     //obscure text
                     Text{"They say that the best weapon against Iron Knuckles is item 176.", /*french*/"Selon moi, les hache-viandes sont vulnérables contre l'objet 176.", /*spanish*/"Según dicen, la mejor arma para enfrentarse a los Nudillos de hierro es el objeto 176."},
    });

    hintTable[JUNK74] = HintText::Junk({
                     //obscure text
                     Text{"They say that it's actually possible to beat the running man.", /*french*/"Selon moi, il est possible de battre le coureur.&Donc, tu prends ton arc, et...", /*spanish*/"Según dicen, con mucha perseverancia puedes ganarle al corredor con la capucha de conejo."},
    });

    hintTable[JUNK75] = HintText::Junk({
                     //obscure text
                     Text{"They say that the stone-cold guardian of the Well is only present during work hours.", /*french*/"Selon moi, le gardien de pierre du puits quitte le soir pour aller se coucher.", /*spanish*/"Según dicen, la inmensa roca que bloquea el pozo solo trabaja en horas laborales."},
    });

    hintTable[JUNK76] = HintText::Junk({
                     //obscure text
                     Text{"They say this hint makes more sense in other languages.", /*french*/"Selon moi, ces indices auraient pu être mieux traduits... Duh!", /*spanish*/"Según dicen, esta pista revela algo de vital importancia si cambias el idioma del juego..."},
    });

    hintTable[JUNK77] = HintText::Junk({
                     //obscure text
                     Text{"BOK? No way.", /*french*/"BD'accord? Hors de question.", /*spanish*/"¿BVale? Ni hablar."},
    });

    /*--------------------------
    |     DUNGEON HINT TEXT    |
    ---------------------------*/

    hintTable[DEKU_TREE] = HintText::DungeonName({
                     //obscure text
                     Text{"an ancient tree", /*french*/"le vieil arbre", /*spanish*/"un ancestral árbol"},
                   }, {},
                     //clear text
                     Text{"Deku Tree", /*french*/"l'arbre Mojo", /*spanish*/"el Gran Árbol Deku"}
  );

    hintTable[DODONGOS_CAVERN] = HintText::DungeonName({
                     //obscure text
                     Text{"an immense cavern", /*french*/"l'immense caverne", /*spanish*/"una descomunal cueva"},
                   }, {},
                     //clear text
                     Text{ "Dodongo's Cavern", /*french*/"la caverne Dodongo", /*spanish*/"la Cueva de los Dodongos"}
  );

    hintTable[JABU_JABUS_BELLY] = HintText::DungeonName({
                     //obscure text
                     Text{"the belly of a deity", /*french*/"le ventre d'un gardien", /*spanish*/"la tripa de cierta deidad"},
                   }, {},
                     //clear text
                     Text{"Jabu-Jabu's Belly", /*french*/"le ventre de Jabu-Jabu", /*spanish*/"tripa de Jabu-Jabu"}
    );

    hintTable[FOREST_TEMPLE] = HintText::DungeonName({
                     //obscure text
                     Text{"a deep forest", /*french*/"la profonde forêt", /*spanish*/"las profundidades del bosque"},
                   }, {},
                     //clear text
                     Text{"Forest Temple", /*french*/"le temple de la forêt", /*spanish*/"el Templo del Bosque"}
    );

    hintTable[FIRE_TEMPLE] = HintText::DungeonName({
                     //obscure text
                     Text{"a high mountain", /*french*/"la grande montagne", /*spanish*/"una alta montaña"},
                   }, {},
                     //clear text
                     Text{"Fire Temple", /*french*/"le temple du feu", /*spanish*/"el Templo del Fuego"}
    );

    hintTable[WATER_TEMPLE] = HintText::DungeonName({
                     //obscure text
                     Text{"a vast lake", /*french*/"le vaste lac", /*spanish*/"un lago inmenso"},
                   }, {},
                     //clear text
                     Text{"Water Temple", /*french*/"le temple de l'eau", /*spanish*/"el Templo del Agua"}
    );

    hintTable[SPIRIT_TEMPLE] = HintText::DungeonName({
                     //obscure text
                     Text{"the goddess of the sand", /*french*/"la déesse des sables", /*spanish*/"la diosa de las arenas"},
                   }, {},
                     //clear text
                     Text{"Spirit Temple", /*french*/"le temple de l'esprit", /*spanish*/"el Templo del Espíritu"}

    );

    hintTable[SHADOW_TEMPLE] = HintText::DungeonName({
                     //obscure text
                     Text{"the house of the dead", /*french*/"la maison des morts", /*spanish*/"la casa de la muerte"},
                   }, {},
                     //clear text
                     Text{"Shadow Temple", /*french*/"le temple de l'ombre", /*spanish*/"el Templo de las Sombras"}
    );

    hintTable[ICE_CAVERN] = HintText::DungeonName({
                     //obscure text
                     Text{"a frozen maze", /*french*/"le dédale glacé", /*spanish*/"un gélido laberinto"},
                   }, {},
                     //clear text
                     Text{"Ice Cavern", /*french*/"la caverne de glace", /*spanish*/"la caverna de hielo"}
    );

    hintTable[BOTTOM_OF_THE_WELL] = HintText::DungeonName({
                     //obscure text
                     Text{"a shadow\'s prison", /*french*/"la prison d'une ombre", /*spanish*/"la prisión de las sombras"},
                   }, {},
                     //clear text
                     Text{"Bottom of the Well", /*french*/"le fonds du puits", /*spanish*/"el fondo del pozo"}
    );

    hintTable[GERUDO_TRAINING_GROUNDS] = HintText::DungeonName({
                     //obscure text
                     Text{"the test of thieves", /*french*/"l'épreuve des voleurs", /*spanish*/"la prueba de las bandidas"},
                   }, {},
                     //clear text
                     Text{"Gerudo Training Grounds", /*french*/"le gymnase Gerudo", /*spanish*/"el Centro de Instrucción Gerudo"}
    );

    hintTable[GANONS_CASTLE] = HintText::DungeonName({
                     //obscure text
                     Text{"a conquered citadel", /*french*/"la citadelle assiégée", /*spanish*/"una conquistada ciudadela"},
                   }, {},
                     //clear text
                     Text{"Inside Ganon's Castle", /*french*/"l'intérieur du château de Ganon", /*spanish*/"el interior del Castillo de Ganon"}
    );

    /*--------------------------
    |      BOSS HINT TEXT      |
    ---------------------------*/

    hintTable[QUEEN_GOHMA] = HintText::Boss({
                     //obscure text
                     Text{"An #ancient tree# rewards", /*french*/"le #vieil arbre# octroie", /*spanish*/"un #ancestral árbol# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Deku Tree# rewards", /*french*/"l'#arbre Mojo# octroie", /*spanish*/"el #Gran Árbol Deku# premia con"}
    );

    hintTable[KING_DODONGO] = HintText::Boss({
                     //obscure text
                     Text{"An #immense cavern# rewards", /*french*/"l'#immense caverne# octroie", /*spanish*/"una #descomunal cueva# premia con"},
                   }, {},
                     //clear text
                     Text{"#Dodongo's Cavern# rewards", /*french*/"la #caverne Dodongo# octroie", /*spanish*/"la #Cueva de los Dodongos# premia con"}
    );

    hintTable[BARINADE] = HintText::Boss({
                     //obscure text
                     Text{"the #belly of a deity# rewards", /*french*/"le #ventre du gardien# octroie", /*spanish*/"la #tripa de cierta deidad# premia con"},
                   }, {},
                     //clear text
                     Text{"#Jabu-Jabu's Belly# rewards", /*french*/"le #ventre de Jabu-Jabu# octroie", /*spanish*/"la #tripa de Jabu-Jabu# premia con"}
    );

    hintTable[PHANTOM_GANON] = HintText::Boss({
                     //obscure text
                     Text{"a #deep forest# rewards", /*french*/"la #profonde forêt# octroie", /*spanish*/"el #profundo bosque# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Forest Temple# rewards", /*french*/"le #temple de la forêt# octroie", /*spanish*/"el #Templo del Bosque# premia con"}
    );

    hintTable[VOLVAGIA] = HintText::Boss({
                     //obscure text
                     Text{"a #high mountain# rewards", /*french*/"la #grande montagne# octroie", /*spanish*/"una #alta montaña# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Fire Temple# rewards", /*french*/"le #temple du feu# octroie", /*spanish*/"el #Templo del Fuego# premia con"}
    );

    hintTable[MORPHA] = HintText::Boss({
                     //obscure text
                     Text{"a #vast lake# rewards", /*french*/"le #vaste lac# octroie", /*spanish*/"un #lago inmenso# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Water Temple# rewards", /*french*/"le #temple de l'eau# octroie", /*spanish*/"el #Templo del Agua# premia con"}
    );

    hintTable[BONGO_BONGO] = HintText::Boss({
                     //obscure text
                     Text{"the #house of the dead# rewards", /*french*/"la #maison des morts# octroie", /*spanish*/"la #casa de la muerte# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Shadow Temple# rewards", /*french*/"le #temple de l'ombre# octroie", /*spanish*/"el #Templo de las Sombras#"}
    );

    hintTable[TWINROVA] = HintText::Boss({
                     //obscure text
                     Text{"a #goddess of the sand# rewards", /*french*/"la #déesse des sables# octroie", /*spanish*/"la #diosa de la arena# premia con"},
                   }, {},
                     //clear text
                     Text{"the #Spirit Temple# rewards", /*french*/"le #temple de l'esprit# octroie", /*spanish*/"el #Templo del Espíritu# premia con"}
    );
    //
    // [LINKS_POCKET_BOSS] = HintText::Boss({
    //                    //obscure text
    //                    Text{"#@'s pocket# rewards", /*french*/"#@ débute avec#", /*spanish*/"el #bolsillo de @# premia con"},
    //                  },
    //                    //clear text
    //                           Text{"#@ already has#", /*french*/"#@ a déjà#", /*spanish*/"el #bolsillo de @ ya tiene#"}
    // );

    /*--------------------------
    |     BRIDGE HINT TEXT     |
    ---------------------------*/

    hintTable[BRIDGE_OPEN_HINT] = HintText::Bridge({
                     //obscure text
                     Text{"The awakened ones will have #already&created a bridge# to the castle where&the evil dwells.",
                /*french*/"Les êtres de sagesse ont#déjà créé&un pont# vers le repaire du mal.",
               /*spanish*/"Los sabios #ya habrán creado un puente#&al castillo, de donde emana el mal."},
    });

    hintTable[BRIDGE_VANILLA_HINT] = HintText::Bridge({
                     //obscure text
                     Text{"The awakened ones will await for the&Hero to collect the #Shadow and Spirit&Medallions# as well as the #Light Arrows#.",
                /*french*/"Les êtres de sagesse attendront le&héros muni des #médaillons de l'ombre et&l'esprit# et des #flèches de lumière#.",
               /*spanish*/"Los sabios aguardarán a que el héroe&obtenga tanto el #Medallón de las&Sombras y el del Espíritu# junto a la #flecha de luz#."},
    });

    hintTable[BRIDGE_STONES_HINT] = HintText::Bridge({
                     //obscure text                                                  singular        plural
                     Text{"The awakened ones will await for the&Hero to collect #%d |Spiritual Stone|Spiritual Stones|#.",
                /*french*/"Les êtres de sagesse attendront le&héros muni de #%d |pierre spirituelle|pierres spirituelles|#.",
               /*spanish*/"Los sabios aguardarán a que el héroe&obtenga #%d |piedra espiritual|piedras espirituales|#."},
    });

    hintTable[BRIDGE_MEDALLIONS_HINT] = HintText::Bridge({
                     //obscure text                                                  singular  plural
                     Text{"The awakened ones will await&for the Hero to collect&%d |Medallion|Medallions|.",
                /*french*/"Les êtres de sagesse attendront le&héros muni de #%d |médaillon|médaillons|#.",
               /*spanish*/"Los sabios aguardarán a que el héroe&obtenga #%d |medallón|medallones|#."},
    });

    hintTable[BRIDGE_REWARDS_HINT] = HintText::Bridge({
                     //obscure text                                                  singular                     plural
                     Text{"The awakened ones will await for the&Hero to collect #%d |Spiritual Stone&or Medallion|Spiritual Stones&and Medallions|#.",
                /*french*/"Les êtres de sagesse attendront le&héros muni de #%d |pierre spirituelle&ou médaillon|pierres spirituelles&et médaillons|#.",
               /*spanish*/"Los sabios aguardarán a que el héroe&obtenga #%d |piedra espiritual o medallón|piedras espirtuales y medallones|#."},
    });

    hintTable[BRIDGE_DUNGEONS_HINT] = HintText::Bridge({
                     //obscure text                                                  singular plural
                     Text{"The awakened ones will await for the&Hero to conquer #%d |Dungeon|Dungeons|#.",
                /*french*/"Les êtres de sagesse attendront la&conquête de #%d |donjon|donjons|#.",
               /*spanish*/"Los sabios aguardarán a que el héroe& complete #%d |mazmorra|mazmorras|#."},
    });

    hintTable[BRIDGE_TOKENS_HINT] = HintText::Bridge({
                     //obscure text
                     Text{"The awakened ones will await for the&Hero to collect #%d |Gold Skulltula&Token|Gold Skulltula&Tokens|#.",
                /*french*/"Les êtres de sagesse attendront le&héros muni de #%d |symbole|symboles| de&Skulltula d'or#.",
               /*spanish*/"Los sabios aguardarán a que el héroe&obtenga #%d |símbolo|símbolos| de&skulltula dorada#."},
    });

    /*--------------------------
    | GANON BOSS KEY HINT TEXT |
    ---------------------------*/

    hintTable[GANON_BK_START_WITH_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be #given from the start#.",
                /*french*/"Aussi, la clé du #Malin# sera #possession même du héros#.",
               /*spanish*/"Y obtendrás la llave del #señor del mal# desde el #inicio#."},
    });

    hintTable[GANON_BK_VANILLA_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be kept in a big chest #inside its tower#.",
                /*french*/"Aussi, la clé du #Malin# sera encoffrée #dans sa tour#.",
               /*spanish*/"Y la llave del #señor del mal# aguardará en un gran cofre de #su torre#."},
    });

    hintTable[GANON_BK_OWN_DUNGEON_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the evil one's key will&be hidden somewhere inside&its castle.",
                /*french*/"Aussi, la clé du #Malin# sera cachée #dans son vaste château#.",
               /*spanish*/"Y la llave del #señor del mal# aguardará en #algún lugar de su castillo#."},
    });

    hintTable[GANON_BK_OVERWORLD_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be hidden #outside of dungeons# in Hyrule.",
                /*french*/"Aussi, la clé du #Malin# se trouve #hors des donjons# d'Hyrule.",
               /*spanish*/"Y la llave del #señor del mal# aguardará #fuera de las mazmorras# de Hyrule."},
    });

    hintTable[GANON_BK_ANY_DUNGEON_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be hidden #inside a dungeon# in Hyrule.",
                /*french*/"Aussi, la clé du #Malin# se trouve #dans un donjon# d'Hyrule.",
               /*spanish*/"Y la llave del #señor del mal# aguardará #en una mazmorra# de Hyrule."},
    });

    hintTable[GANON_BK_ANYWHERE_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be hidden somewhere #in Hyrule#.",
                /*french*/"Aussi, la clé du #Malin# se trouve quelque part #dans Hyrule#.",
               /*spanish*/"Y la llave del #señor del mal# aguardará en #cualquier lugar de Hyrule#."},
    });

    hintTable[GANON_BK_TRIFORCE_HINT] = HintText::GanonsBossKey({
                     //obscure text
                     Text{"And the #evil one#'s key will be given to the Hero once the #Triforce# is completed.",
                /*french*/"Aussi, la clé du #Malin# se révèlera une fois la #Triforce# assemblée.",
               /*spanish*/"Y el héroe recibirá la llave del #señor del mal# cuando haya completado la #Trifuerza#."},
    });

    /*--------------------------
    |      LACS HINT TEXT      |
    ---------------------------*/

    hintTable[LACS_VANILLA_HINT] = HintText::LACS({
                     //obscure text
                     Text{"And the #evil one#'s key will be&provided by Zelda once the #Shadow&and Spirit Medallions# are retrieved.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# avec les #médaillons de&l'ombre et de l'esprit#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras obtener&#el medallón de las sombras y del espíritu#."},
    });

    hintTable[LACS_MEDALLIONS_HINT] = HintText::LACS({
                     //obscure text                                                     singular      plural
                     Text{"And the #evil one#'s key will be&provided by Zelda once #%d&|Medallion# is|Medallions# are| retrieved.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# avec #%d |médaillon|médaillons|#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |medallón|medallones|#."},
    });

    hintTable[LACS_STONES_HINT] = HintText::LACS({
                     //obscure text                                                     singular            plural
                     Text{"And the #evil one#'s key will be&provided by Zelda once #%d |Spiritual&Stone# is|Spiritual&Stones# are| retrieved.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# avec #%d des |pierre&spirituelle|pierres&spirituelles|#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |piedra espiritual|piedras espirituales|#."},
    });

    hintTable[LACS_REWARDS_HINT] = HintText::LACS({
                     //obscure text                                                     singular                         plural
                     Text{"And the #evil one#'s key will be&provided by Zelda once #%d |Spiritual&Stone or Medallion# is|Spiritual&Stones and Medallions# are| retrieved.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# avec #%d |pierre spirituelle&et des médaillon|pierres spirituelles&et des médaillons|#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |piedra espiritual o medallón|piedras espirituales o medallones|#."},
    });

    hintTable[LACS_DUNGEONS_HINT] = HintText::LACS({
                     //obscure text                                                     singular    plural
                     Text{"And the #evil one#'s key will be&provided by Zelda once #%d |Dungeon#&is|Dungeons#&are| conquered.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# une fois #%d |donjon conquéri|donjons conquéris|#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras completar&#%d |mazmorra|mazmorras|#."},
    });

    hintTable[LACS_TOKENS_HINT] = HintText::LACS({
                     //obscure text                                                     singular                 plural
                     Text{"And the #evil one#'s key will be&provided by Zelda once #%d |Gold&Skulltula Token# is|Gold&Skulltula Tokens# are| retrieved.",
                /*french*/"Aussi, Zelda crééra la clé du&#Malin# avec #%d |symbole|symboles| de&Skulltula d'or#.",
               /*spanish*/"Y Zelda entregará la llave&del #señor del mal# tras obtener&#%d |símbolo|símbolos| de&skulltula dorada#."},
    });
    /*--------------------------
    |     TRIAL HINT TEXT      |
    ---------------------------*/

    hintTable[SIX_TRIALS] = HintText::Exclude({
                     //obscure text
                     Text{"#Ganon's Tower# is protected by a powerful barrier.", /*french*/"#la tour de Ganon# est protégée par une puissante barrière", /*spanish*/"la #torre de Ganon# está protegida por una poderosa barrera"},
    });

    hintTable[ZERO_TRIALS] = HintText::Exclude({
                     //obscure text
                     Text{"Sheik dispelled the barrier around #Ganon's Tower#.", /*french*/"Sheik a dissipé la barrière autour de #la tour de Ganon#", /*spanish*/"Sheik disipó la barrera alrededor de la #torre de Ganon#."},
    });

    hintTable[FOUR_TO_FIVE_TRIALS] = HintText::Exclude({
                     //obscure text
                     Text{" was dispelled by Sheik.", /*french*/" a été dissipée par Sheik.", /*spanish*/" se disipó gracias a Sheik."},
    });

    hintTable[ONE_TO_THREE_TRIALS] = HintText::Exclude({
                     //obscure text
                     Text{" protects Ganons Tower.", /*french*/" protège la tour de Ganon.", /*spanish*/" protege la torre de Ganon"},
    });

    /*--------------------------
    |        ALTAR TEXT        |
    ---------------------------*/

    hintTable[SPIRITUAL_STONE_TEXT_START] = HintText::Altar({
                     //obscure text
                     Text{"3 Spiritual Stones found in Hyrule...",
                /*french*/"Les trois pierres spirituelles cachées dans Hyrule...",
               /*spanish*/"Tres piedras espirituales halladas por Hyrule..."},
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTOPEN] = HintText::Altar({
                     //obscure text
                     Text{"Ye who may become a Hero...&The path to the future is open...",
                /*french*/"À celui qui a quête de devenir héros...&Le futur vous accueille béant...",
               /*spanish*/"Para aquel que se convierta en el héroe...&La puerta al futuro está a su disposición..."},
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTCLOSED] = HintText::Altar({
                     //obscure text
                     Text{"Ye who may become a Hero...&Stand with the Ocarina and&play the Song of Time.",
                /*french*/"À celui qui a quête de devenir héros...&Portez l'Ocarina et&jouez le chant du temps.",
               /*spanish*/"Para aquel que se convierta en el héroe...&Tome la ocarina y&entone la Canción del Tiempo."},
    });

    hintTable[CHILD_ALTAR_TEXT_END_DOTINTENDED] = HintText::Altar({
                     //obscure text
                     Text{"Ye who may become a Hero...&Offer the spiritual stones and&play the Song of Time.",
                /*french*/"À celui qui a quête de devenir héros...&Présentez les pierres spirituelles et&jouez le chant du temps.",
               /*spanish*/"Para aquel que se convierta en el héroe...&Tome las piedras espirituales y&entone la Canción del Tiempo."},
    });

    hintTable[ADULT_ALTAR_TEXT_START] = HintText::Altar({
                     //obscure text
                     Text{"An awakening voice from the Sacred&Realm will call those destined to be&Sages, who dwell in the five temples.",
                /*french*/"Quand le mal aura triomphé, une voix du Saint Royaume appellera ceux cachés dans les #cinq temples#, destinés à être Sages.",
               /*spanish*/"Cuando el mal lo impregne todo, desde el Reino Sagrado surgirá una voz que hará despertar a los sabios que moran en los #cinco templos#."},
    });

    hintTable[ADULT_ALTAR_TEXT_END] = HintText::Altar({
                     //obscure text
                     Text{"Together with the Hero of Time,&the awakened ones will return the&light of peace to the world...",
                /*french*/"Ensemble avec le Héros du Temps,&ces Sages emprisonneront le mal et&réinstaureront la lumière de paix dans&le monde...",
               /*spanish*/"Con el Héroe del Tiempo, aquellos&que despierten detendrán el mal y&volverán al mundo de luz la paz..."},
    });

    /*--------------------------
    |   VALIDATION LINE TEXT   |
    ---------------------------*/

    hintTable[VALIDATION_LINE] = HintText::Validation({
                     //obscure text
                     Text{"Hmph... Since you made it this far, I'll let you know what glorious prize of Ganon's you likely missed out on in my tower.^Behold...^",
                /*french*/"Pah! Puisque tu t'es rendu ici, je te dirai quel trésor tu as manqué dans ma tour.^Et c'est...^",
               /*spanish*/"Mmm... Ya que has llegado hasta aquí, te diré qué preciado objeto de mi propiedad puedes haberte dejado en mi torre.^He aquí...^"},
    });

    /*--------------------------
    | LIGHT ARROW LOCATION TEXT|
    ---------------------------*/

    hintTable[LIGHT_ARROW_LOCATION_HINT] = HintText::LightArrow({
                     //obscure text
                     Text{"Ha ha ha... You'll never beat me by reflecting my lightning bolts and unleashing the arrows from ",
                /*french*/"Ha ha ha... Pauvre fou! Tu ne pourras jamais me vaincre sans les flèches que j'ai caché dans ",
               /*spanish*/"Ja, ja, ja... Nunca me derrotarás reflejando mis esferas de energía y desplegando la flecha de luz de "},
    });

    hintTable[YOUR_POCKET] = HintText::Exclude({
                     //obscure text
                     Text{"your pocket", /*french*/"tes poches", /*spanish*/"tu bolsillo"},
    });

    /*--------------------------
    |      GANON LINE TEXT     |
    ---------------------------*/

    hintTable[GANON_LINE01] = HintText::GanonLine({
                     //obscure text
                     Text{"Oh! It's @.&I was expecting someone called Sheik. Do you know what happened to them?",
                /*french*/"Ah, c'est @.&J'attendais un certain Sheik. Tu sais ce qui lui est arrivé?",
               /*spanish*/"¡Oh! Pero si es @.&Estaba esperando a alguien llamado Sheik. ¿Sabes qué puede haberle pasado?"},
    });

    hintTable[GANON_LINE02] = HintText::GanonLine({
                     //obscure text
                     Text{"I knew I shouldn't have put the key on the other side of my door.",
                /*french*/"J'aurais dû garder la clé ici. Hélas...",
               /*spanish*/"Sabía que no tendría que haber dejado la llave al otro lado de la puerta."},
    });

    hintTable[GANON_LINE03] = HintText::GanonLine({
                     //obscure text
                     Text{"Looks like it's time for a round of tennis.",
                /*french*/"C'est l'heure de jouer au tennis.",
               /*spanish*/"Parece que es hora de una pachanga de tenis."},
    });

    hintTable[GANON_LINE04] = HintText::GanonLine({
                     //obscure text
                     Text{"You'll never deflect my bolts of energy with your sword, then shoot me with those Light Arrows you happen to have.",
                /*french*/"Ne perds pas ton temps à frapper mes éclairs d'énergie avec ton épée et me tirer avec tes flèches de Lumière!",
               /*spanish*/"Nunca reflejarás mis esferas de energía con tu espada, para después dispararme con las flechas de luz que tendrás."},
    });

    hintTable[GANON_LINE05] = HintText::GanonLine({
                     //obscure text
                     Text{"Why did I leave my trident back in the desert?",
                /*french*/"Sale bêtise... Et j'ai oublié mon trident dans le désert!",
               /*spanish*/"Santa Hylia... ¿Por qué me habré dejado el tridente en el desierto?"},
    });

    hintTable[GANON_LINE06] = HintText::GanonLine({
                     //obscure text
                     Text{"Zelda is probably going to do something stupid, like send you back to your own timeline.^So this is quite meaningless. Do you really want to save this moron?",
                /*french*/"Même si je suis vaincu... Zelda te renverra dans ton ère, et je reviendrai conquérir!^Telle est la prophécie d'Hyrule Historia!",
               /*spanish*/"Seguro que Zelda trata de hacer alguna tontería, como enviarte de vuelta a tu línea temporal.^No tiene ningún sentido alguno. ¿De verdad quieres salvar a esa tonta?"},
    });

    hintTable[GANON_LINE07] = HintText::GanonLine({
                     //obscure text
                     Text{"What about Zelda makes you think she'd be a better ruler than I?^I saved Lon Lon Ranch,&fed the hungry,&and my castle floats.",
                /*french*/"Zelda ne sera jamais un meilleur monarque que moi!^J'ai un château volant, mes sujets sont des belles amazones... et mes Moblins sont clairement plus puissants que jamais!",
               /*spanish*/"¿Qué te hace pensar que Zelda gobierna mejor que yo?^Yo he salvado el Rancho Lon Lon,&he alimentado a los hambrientos&y hasta hago que mi castillo flote."},
    });

    hintTable[GANON_LINE08] = HintText::GanonLine({
                     //obscure text
                     Text{"I've learned this spell,&it's really neat,&I'll keep it later&for your treat!",
                /*french*/"Gamin, ton destin achève,&sous mon sort tu périras!&Cette partie ne fut pas brève,&et cette mort, tu subiras!",
               /*spanish*/"Veamos ahora que harás,&la batalla ha de comenzar,&te enviaré de una vez al más allá,&¿listo para afrontar la verdad?"},
    });

    hintTable[GANON_LINE09] = HintText::GanonLine({
                     //obscure text
                     Text{"Many tricks are up my sleeve,&to save yourself&you'd better leave!",
                /*french*/"Sale petit garnement,&tu fais erreur!&C'est maintenant que marque&ta dernière heure!",
               /*spanish*/"¿No osarás a mí enfrentarte?&Rimas aparte,&¡voy a matarte!"},
    });

    hintTable[GANON_LINE10] = HintText::GanonLine({
                     //obscure text
                     Text{"After what you did to Koholint Island, how can you call me the bad guy?",
                /*french*/"J'admire ce que tu as fait à l'Île Koholint... Toi et moi, nous devrions faire équipe!",
               /*spanish*/"Después de lo que le hiciste a la Isla Koholint, ¿cómo te atreves a llamarme malvado?"},
    });

    hintTable[GANON_LINE11] = HintText::GanonLine({
                     //obscure text
                     Text{"Today, let's begin down&'The Hero is Defeated' timeline.",
                /*french*/"Si tu me vaincs, Hyrule sera englouti... mais si tu meurs, on aura A Link to the Past, le meilleur opus de la série!",
               /*spanish*/"Hoy daremos lugar a la línea temporal del Héroe Derrotado.&¡Prepárate para el culmen de esta saga!"},
    });

    /*--------------------------
    |      MERCHANTS' ITEMS     |
    ---------------------------*/

    hintTable[MEDIGORON_DIALOG_FIRST] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{"How about buying ",
                /*french*/"Veux-tu acheter ",
               /*spanish*/"¿Me compras "},
    });

    hintTable[MEDIGORON_DIALOG_SECOND] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{" for #200 Rupees#?&"+TWO_WAY_CHOICE()+"#Buy&Don't buy#",
                /*french*/" pour #200 rubis#?&"+TWO_WAY_CHOICE()+"#Acheter&Ne pas acheter#",
               /*spanish*/" por #200 rupias#?&"+TWO_WAY_CHOICE()+"#Comprar&No comprar#"},
    });

    hintTable[CARPET_SALESMAN_DIALOG_FIRST] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{"Welcome!^I am selling stuff, strange and rare, from&all over the world to everybody. Today's&special is...^",
                /*french*/"Bienvenue!^Je vends des objets rares et merveilleux du&monde entier. En spécial aujourd'hui...^",
               /*spanish*/"¡Acércate!^Vendo productos extraños y difíciles de&encontrar... De todo el mundo a todo el&mundo. La oferta de hoy es...^¡"},
    });

    hintTable[CARPET_SALESMAN_DIALOG_SECOND] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{"! Terrifying!&I won't tell you what it is until I see the&money...^How about #200 Rupees#?&&"+TWO_WAY_CHOICE()+"#Buy&Don't buy#",
                /*french*/"! Un&concentré de puissance! Mais montre tes&rubis avant que je te dise ce que c'est...^Disons #200 rubis#?&&"+TWO_WAY_CHOICE()+"#Acheter&Ne pas acheter#",
               /*spanish*/"! ¡Terrorífico!&No te revelaré su nombre hasta que&vea el dinero...^#200 rupias#, ¿qué te parece?&&"+TWO_WAY_CHOICE()+"#Comprar&No comprar#"},
    });

    hintTable[CARPET_SALESMAN_DIALOG_THIRD] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{"Thank you very much!^What I'm selling is... #",
                /*french*/"Merci beaucoup!^Cet objet extraordinaire est... #",
               /*spanish*/"¡Muchas gracias!^Lo que vendo es... #¡"},
    });

    hintTable[CARPET_SALESMAN_DIALOG_FOURTH] = HintText::MerchantsDialogs({
                     //obscure text
                     Text{"!#^The mark that will lead you to the #Spirit&Temple# is the #flag on the "+IF_NOT_MQ()+"left"+MQ_ELSE()+"right"+MQ_END()+"# outside the shop. Be seeing you!",
                /*french*/"!#^La marque qui te mènera au #Temple de l'esprit# est le #drapeau "+IF_NOT_MQ()+"gauche"+MQ_ELSE()+"droite"+MQ_END()+"# en sortant d'ici. À la prochaine!",
               /*spanish*/"!#^La marca que te guiará al #Templo del&Espíritu# es la #bandera que está a la&"+IF_NOT_MQ()+"izquierda"+MQ_ELSE()+"derecha"+MQ_END()+"# al salir de aquí. ¡Nos vemos!"},
    });
}

int32_t StonesRequiredBySettings() {
    int32_t stones = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeStoneCount.Value<uint8_t>() });
    }
    if (Settings::Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeRewardCount.Value<uint8_t>() - 6 });
    }
    if ((Settings::Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) && (Settings::ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON))) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::BridgeDungeonCount.Value<uint8_t>() - 6 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSStoneCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSRewardCount.Value<uint8_t>() - 6 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
        stones = std::max<int32_t>({ stones, (int32_t)Settings::LACSDungeonCount.Value<uint8_t>() - 6 });
    }
    return stones;
}

int32_t MedallionsRequiredBySettings() {
    int32_t medallions = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeMedallionCount.Value<uint8_t>() });
    }
    if (Settings::Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeRewardCount.Value<uint8_t>() - 3 });
    }
    if ((Settings::Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) && (Settings::ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON))) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::BridgeDungeonCount.Value<uint8_t>() - 3 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSMedallionCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSRewardCount.Value<uint8_t>() - 3 });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
        medallions = std::max<int32_t>({ medallions, (int32_t)Settings::LACSDungeonCount.Value<uint8_t>() - 3 });
    }
    return medallions;
}

int32_t TokensRequiredBySettings() {
    int32_t tokens = 0;
    if (Settings::Bridge.Is(RAINBOWBRIDGE_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)Settings::BridgeTokenCount.Value<uint8_t>() });
    }
    if (Settings::GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
        tokens = std::max<int32_t>({ tokens, (int32_t)Settings::LACSTokenCount.Value<uint8_t>() });
    }
    return tokens;
}

std::array<ConditionalAlwaysHint, 9> conditionalAlwaysHints = {
    std::make_pair(MARKET_10_BIG_POES, [](){ return Settings::BigPoeTargetCount.Value<uint8_t>() >= 3; }), // Remember, the option's value being 3 means 4 are required
    std::make_pair(DEKU_THEATER_MASK_OF_TRUTH, [](){ return !Settings::CompleteMaskQuest; }),
    std::make_pair(SONG_FROM_OCARINA_OF_TIME, [](){ return StonesRequiredBySettings() < 2; }),
    std::make_pair(HF_OCARINA_OF_TIME_ITEM, [](){ return StonesRequiredBySettings() < 2; }),
    std::make_pair(SHEIK_IN_KAKARIKO, [](){ return MedallionsRequiredBySettings() < 5; }),
    std::make_pair(DMT_TRADE_CLAIM_CHECK, [](){ return false; }),
    std::make_pair(KAK_30_GOLD_SKULLTULA_REWARD, [](){ return TokensRequiredBySettings() < 30; }),
    std::make_pair(KAK_40_GOLD_SKULLTULA_REWARD, [](){ return TokensRequiredBySettings() < 40; }),
    std::make_pair(KAK_50_GOLD_SKULLTULA_REWARD, [](){ return TokensRequiredBySettings() < 50; })
};

const HintText& Hint(const uint32_t hintKey) {
    return hintTable[hintKey];
}

std::vector<HintText> GetHintCategory(HintCategory category) {

    std::vector<HintText> hintsInCategory = {};

    for (const auto& hint : hintTable) {
        if (hint.GetType() == category) {
            hintsInCategory.push_back(hint);
        }
    }
    return hintsInCategory;
}
