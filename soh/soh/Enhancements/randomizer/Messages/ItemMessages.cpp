// RANDOTODO: Consider putting Ice Traps in their own file.
/**
 * This file handles custom messages relating to Items,
 * such as Get Item messages for non-vanilla items,
 * Vanilla/MQ hints when collecting Maps, Ice Trap messages,
 * etc.
 */
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/ShipInit.hpp"
#include "z64item.h"
#include <soh/ResourceManagerHelpers.h>

extern "C" {
#include <variables.h>
#include <macros.h>
extern PlayState* gPlayState;
}

static const char* const englishIceTrapMessages[] = {
    "You are a #FOOL#!",
    "You are a #FOWL#!",
    "#FOOL#!",
    "You just got #PUNKED#!",
    "Stay #frosty#, @.",
    "Take a #chill pill#, @.",
    "#Winter# is coming.",
    "#ICE# to see you, @.",
    "Feeling a little %rhot%w under the collar? #Let's fix that#.",
    "It's a #cold day# in the Evil Realm.",
    "Getting #cold feet#?",
    "Say hello to the #Zoras# for me!",
    "Can you keep a #cool head#?",
    "Ganondorf used #Ice Trap#!&It's super effective!",
    "Allow me to break the #ice#!",
    "#Cold pun#.",
    "The #Titanic# would be scared of you, @.",
    "Oh no!",
    "Uh oh!",
    "What killed the dinosaurs?&The #ICE# age!",
    "Knock knock. Who's there? Ice. Ice who? Ice see that you're a #FOOL#.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Thank you #@#! But your item is in another castle!",
    "#FREEZE#! Don't move!",
    "Wouldn't it be #ice# if we were colder?",
    "Greetings from #Snowhead#! Wish you were here.",
    "Too #cool# for you?",
    "#Ice#, #ice#, baby...",
    "Time to break the #ice#.",
    "We wish that you would read this... We wish that you would read this... But we set our bar low.",
    "#Freeze# and put your hands in the air!",
    "#Ice# to meet you!",
    "Do you want to #freeze# a snowman?",
    "Isn't there a #mansion# around here?",
    "Now you know how #King Zora# feels.",
    "May the #Frost# be with you.",
    "Carpe diem. #Freeze# the day.",
    "There #snow# place like home.",
    "That'll do, #ice#. That'll do.",
    "All that is #cold# does not glitter. Not all those who wander are #frost#.",
    "I Used To Be An Adventurer Like You. Then I Took An #Icetrap# To The Knee.",
    "Would you like #ice# with that?",
    "You have obtained the #Ice# Medallion!",
    "Quick, do a #Zora# impression!",
    "One item #on the rocks#!", // would be better if it could display the name of the item
    "How much does a polar bear weigh?&Enough to break the #ice#.",
    "You got Din's #Ice#!",
    "You got Nayru's #Cold#!",
    "You got Farore's #Freeze#!",
    "KEKW",
    "You just got #ICE TRAPPED#! Tag your friends to totally #ICE TRAP# them!",
    "Are you okay, @? You're being #cold# today.",
    "In a moment, your game might experience some #freezing#.",
    "Breeze? Trees? Squeeze? No, it's a #freeze#!",
    "After collecting this item, @ was assaulted in #cold# blood.",
    "Only #chill# vibes around here!",
    "Here's a #cool# gift for you!",
    "Aha! You THOUGHT.",
    "Stay hydrated and brush your teeth!",
    "Isn't it too hot here? Let's turn the #AC# on.",
    "One serving of #cold# @, coming right up!",
    "Is it #cold# in here or is that just me?",
    "Yahaha! You found me!",
    "You'd make a great #ice#-tronaut!",
    "That's just the tip of the #iceberg#!",
    "It's the triforce!&No, just kidding, it's an #ice trap#.",
    "WINNER!",
    "LOSER!",
    "Greetings from #Cold Miser#!",
    "Pardon me while I turn up the #AC#.",
    "If you can't stand the #cold#, get out of the #freezer#.",
    "Oh, goodie! #Frozen @# for the main course!",
    "You have #freeze# power!",
    "You obtained the #Ice Beam#! No wait, wrong game.",
    "Here's to another lousy millenium!",
    "You've activated my #trap card#!",
    "I love #refrigerators#!",
    "You expected an item,&BUT IT WAS I, AN #ICE TRAP#!",
    "It's dangerous to go alone! Take #this#!",
    "soh.exe has #stopped responding#.",
    "Enough! My #Ice Trap# thaws in the morning!",
    "Nobody expects the span-#ice# inquisition!",
    "This is one #cool# item!",
    "Say hello to my #little friend#!",
    "We made you an offer you #can't refuse#.",
    "Hyrule? More like #Hycool#!",
    "Ice puns are #snow# problem!",
    "This #ice# is #snow# joke!",
    "There's no business like #snow# business!",
    "No, dude.",
    "N#ice# trap ya got here!",
    "Quick do your best impression of #Zoras Domain#!",
    "Ganon used #ice beam#, it's super effective!",
    "I was #frozen# today.",
    "You're not in a #hurry#, right?",
    "It's a #trap#!",
    "At least it's not a VC crash and only Link is #frozen#!",
    "Oh no! #BRAIN FREEZE#!",
    "Looks like your game #froze#! Nope just you!",
    "PK #FREEZE#!",
    "May I interest you in some #iced# Tea?",
    "Time for some Netflix and #chill#.",
    "I know, I know... #FREEZE#!",
    "#Ice# of you to drop by!",
    "STOP!&You violated the #Thaw#!",
    "I wanted to give you a treasure, but it looks like you got #cold feet#.",
    "You told me you wanted to deliver #just ice# to Ganondorf!",
    "You got the triforce!&This ancient artifact of divine power can grant any- wait, no, sorry, it's just an ice "
    "trap. My bad.",
    "Time to #cool off#!",
    "The #Ice Cavern# sends its regards.",
    "Loading item, please #wait#...",
    "Mash A+B to not #die#.",
    "Sorry, your item is in another location.", // would be better if it could have the name of the item
    "You only wish this was %gGreg%w.",
    "Do you want to drink a hot chocolate?",
    "The #cold# never bothered me anyway.",
    "Hope you're too school for #cool#!",
    "Be thankful this isn't #absolute zero#.",
    "Did you know the F in ZFG stands for #Freeze#?",
    "You got #Ice Age (2002)#!",
    "Now you can cast a #spell# you don't know.",
    "How's about a hero #on the rocks#?",
    "Ain't no tunic for #this#!",
    "I knew you were #part metroid#!",
    "That's just the #icing on the cake#!",
    "You're so #cool#, @!",
    "You found #disappointment#!",
    "You got #FOOLED#!",
    "Start Mashing.",
    "This item will #self-destruct# in 5 seconds...",
    "Remember, there may be some momentary #discomfort#.",
    "In a perfect world #ice traps# like me would not exist, but this is not a perfect world.",
    "Gee, it sure is #cold# around here.",
    "You tested the item with your #ice detector#, it beeped.", // would be better if it could have the name of the item
    "You have found the way of the zero. The #sub-zero#.",
    "Mweep... mweep... mweep...",
    "Scum, #freezebag#! I mean #freeze#, scumbag!",
    "Is it #chilly# in here or is it just #you#?",
    "#Proceed#.",
    "WHAT'S SHE GONNA DO, MAKE ME AN #[Ice Cream]#!?",
    "You've met with a #terrible fate#, haven't you?",
    "So I heard you like the Shining, here's how it #ends#.",
    "Minor routing mistake. #I win#.",
    "Hold this #L#, @.",
    "#SKILL ISSUE#.",
    "All your heat are belong to us.",
    "Wait a second, don't you already have #this item#?",
    "#Freeze#! We have you surrounded!",
    "Error 404 - Item not #found#.",
    "Hydration break! Hey, who #froze# my water?",
    "Oops, wrong #item model#.",
    "Whoops! You have to put the item #in your inventory#.",
    "You dropped the item, shattering it into #shards of ice#!", // would be better if it could have the name of the
                                                                 // item
    "Is this... golden age Simpsons?&BECAUSE I'M ABOUT TO #CHOKE A CHILD#.",
    "You are the weakest @, #goodbye#!",
    "Ugh... Why did we even randomize #this item#?",
    "The #Frost Moon# is rising...",
    "According to all known laws of physics and biology, there is no way that @ should be able to survive #getting "
    "fully encased in ice#. The cells in @'s body would all die by the time they #unthaw#. Of course, this is a video "
    "game, so @ survives anyway... #Probably#.",
    "Okay, so stop me if you've heard this one - a gamer and a bottle of #liquid nitrogen# walk into a milk bar...",
    "Lástima, es una #trampa de hielo#...&&Nobody expects the Spanish #ice trap#!",
    "Gee, it sure is #BURR#ing around here.",
    "Navi? Oh! I thought she was called #Névé#!",
    "It's fine, @ knew this was a #trap#, they're just using it to take damage intentionally to manipulate RNG.",
    "Unfortunately, the item has #stopped#.",        // would be better if it could have the name of the item
    "This item is #not available# in your country.", // would be better if it could have the name of the item
    "#Ice# try. #;)#",
    "D'oh, I #missed#!",
    "Where is my #super suit#?",
    "#Titanic's revenge#.",
};

static const char* const germanIceTrapMessages[] = {
    "Du bist ein #DUMMKOPF#!",
    "Du bist eine #Frostbeule#!",
    "#DUMMKOPF#!",
    "Du wurdest #eiskalt# erwischt!",
    "Es läuft Dir #eiskalt# den Rücken hinunter, @.",
    "Bleib #cool#, @.",
    "Der #Winter# naht.",
    "Alles #cool# im Pool?",
    "#Abkühlung gefällig#?",
    "Es ist ein %kalter%w Tag im Herzen von Hyrule.",
    "Bekommst Du etwa #kalte# Füße?",
    "Sag den #Zoras# viele Grüße von mir!",
    "Bewahre einen #kühlen#! Kopf.",
    "Ganondorf setzt #Eisstrahl# ein. Das ist sehr effektiv!",
    "Ein Lächeln ist der beste Weg, um das #Eis# zu brechen!",
    "#Eiskalt# lässt du meine Seele erfrier'n.",
    "Die #Titanic# hätte Angst vor Dir, @.",
    "Oh nein!",
    "Was die Dinosaurier getötet hat?&Die #Eiszeit#!",
    "Nachts ist es #kälter# als draußen.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Danke #@#! Aber der Gegenstand ist in einem anderem Schloß!",
    "Kalt. Kalt. Kälter. #EISKALT#!",
};

static const char* const frenchIceTrapMessages[] = {
    "#Pauvre fou#...",
    "Tu es un #glaçon#, Harry!",
    "#Sot# que tu es.",
    "Ça me #glace# le sang!",
    "#Reste au frais#, @.",
    "Et c'est la douche #froide#!",
    "L'#hiver# vient.",
    "#Glacier#!",
    "Ça en jette un #froid#.",
    "Est-ce que tu as déjà eu des sueurs #froides#?",
    "La vengeance est un plat qui se mange #froid#!",
    "Dit bonjour aux #Zoras# pour moi!",
    "Il faut parfois savoir garder la tête #froide#!",
    "Ganondorf utilise #Piège de Glace#! C'est super efficace!",
    "Laisse moi briser la #glace#!",
    "Balance man..., Cadence man..., Trace la #glace#..., c'est le Cooooolllll Rasta!",
    "Le #Titanic# aurait peur de toi, @.",
    "Oh non!",
    "Qu'est-ce qui a tué les dinosaures?&L'ère #glacière#!",
    "L'imbécile réfléchit uniquement quand il s'observe dans la #glace#.",
    "Never gonna #give you up#. Never gonna #let you down#. Never gonna run around and #desert you#.",
    "Merci #@#! Mais ton objet est dans un autre château!",
    "J'espère que ça ne te fait ni chaud, ni #froid#.",
    "Je voulais t'offrir un trésor, mais il semble que tu aies eu #froid aux pieds#",
    "Tu m'as dit que tu voulais livrer #de la glace# à Ganondorf!",
    "Tu as obtenu la Triforce!&Cet ancien artefact divin peut exaucer n'importe quel... ah non, désolé, c'est juste un "
    "piège de glace.",
    "Il est temps de #te rafraîchir#!",
    "La #Caverne Polaire# te passe le bonjour.",
    "Chargement de l'objet, veuillez #patienter#...",
    "Martèle A+B pour ne pas #mourir#.",
    "Désolé, ton objet est à un autre endroit.", // would be better if it could have the name of the item
    "Tu espérais que ce soit %gGreg%w.",
    "Tu veux boire un chocolat chaud?",
    "Le #froid# ne m'a jamais dérangé, de toute façon.",
    "J'espère que tu es trop cool pour être #cool#!",
    "Sois reconnaissant que ce n'est pas le #zéro absolu#.",
    "Tu savais que le G de ZFG signifie #Glace#?",
    "Tu as obtenu #L'Âge de Glace (2002)#!",
    "Maintenant, tu peux lancer un #sort# que tu ne connais pas.",
    "Que dirais-tu d'un héros #sur glace# ?",
    "Pas de tunique pour #ça#!",
    "Je savais que tu étais #partiellement Metroid#!",
    "Voilà juste la #cerise sur le gâteau#!",
    "Tu es tellement #cool#, @!",
    "Tu as trouvé de la #déception#!",
    "Tu t'es fait #BERNER#!",
    "Commence à marteler.",
    "Cet objet va #s'autodétruire# dans 5 secondes...",
    "Souviens-toi, il pourrait y avoir un léger #inconfort#.",
    "Dans un monde parfait, les #pièges de glace# comme moi n'existeraient pas, mais ce n'est pas un monde parfait.",
    "Mon dieu qu'il fait #froid# ici.",
    "Tu as testé l'objet avec ton #détecteur de glace#, il a bipé.", // would be better if it could have the name of the
                                                                     // item
    "Tu as découvert le chemin du zéro. Le #sub-zéro#.",
    "Mweep... mweep... mweep...",
    "Gelé, #sac à glace#! Je veux dire #gèle-toi#, racaille!",
    "Est-ce qu'il fait #frais# ici ou est-ce juste #toi#?",
    "#Continue#",
    "QU'EST-CE QU'ELLE VA FAIRE, ME FAIRE UNE #[Glace]#!?",
    "Tu as rencontré un #terrible destin#, n'est-ce pas?",
    "Alors comme ça, tu aimes Shining ? Voici comment ça #finit#.",
    "Petite erreur de trajectoire. #Je gagne#.",
    "Prends ce #L#, @.",
    "#Problème de compétence#",
    "Tout ton chauffage nous appartient.",
    "Attends une seconde, tu as déjà #cet objet#, non?",
    "#Gèle#! Tu es entouré !",
    "Erreur 404 - Objet non #trouvé#.",
    "Pause hydratation ! Hé, qui a #gelé# mon eau?",
    "Oups, mauvais #modèle d'objet#.",
    "Oups! Tu dois mettre l'objet #dans ton inventaire#.",
    "Tu as fait tomber l'objet, le brisant en #éclats de glace#!", // would be better if it could have the name of the
                                                                   // item
    "Tu es le maillon faible @, #au revoir#!",
    "Ugh... Pourquoi avons-nous même randomisé #cet objet#?",
    "La #Lune de Givre# se lève...",
    "Selon toutes les lois connues de la physique et de la biologie, @ ne devrait pas survivre à #être complètement "
    "enfermé dans la glace#. Les cellules de @ mourraient avant qu'elles ne #dégèlent#. Mais c'est un jeu vidéo, alors "
    "@ survit... #Probablement#.",
    "OK, arrête-moi si tu l'as déjà entendue - un joueur et une bouteille de #nitrogène liquide# entrent dans un bar à "
    "lait...",
    "Lástima, c'est un #piège de glace#...&&Personne ne s'attend à un #piège de glace espagnol#!",
    "Mon dieu qu'il fait #GLAGLA# ici.",
    "C'est bon, @ savait que c'était un #piège#, il l'utilise juste pour prendre des dégâts intentionnellement et "
    "manipuler la RNG.",
    "Cet objet n'est #pas disponible# dans votre pays.", // would be better if it could have the name of the item
    "#Bonne# tentative. #;)#",
    "Où est mon #Super Costume#?",
    "#La revanche du Titanic#.",
};

void BuildIceTrapMessage(CustomMessage& msg) {
    if (CVarGetInteger(CVAR_GENERAL("LetItSnow"), 0)) {
        msg = CustomMessage(
            /*english*/ "This year for Christmas, all you get is #COAL#!",
            /*german*/ "This year for Christmas, all you get is #COAL#!",
            /*french*/ "Pour Noël, cette année, tu n'auras que du #CHARBON#! %rJoyeux Noël%w!", { QM_BLUE });
    } else {
        msg = CustomMessage(ShipUtils::RandomElement(englishIceTrapMessages),
                            ShipUtils::RandomElement(germanIceTrapMessages),
                            ShipUtils::RandomElement(frenchIceTrapMessages), { QM_BLUE, QM_BLUE, QM_BLUE });
    }

    msg.AutoFormat();
}

void BuildTriforcePieceMessage(CustomMessage& msg) {
    uint8_t current = gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected + 1;
    uint8_t required = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED) + 1;
    uint8_t remaining = required - current;
    float percentageCollected = (float)current / (float)required;

    if (percentageCollected <= 0.25) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. It's a start!",
                "Ein %yTriforce-Splitter%w! Du hast&%g[[current]]%w von %c[[required]]%w gefunden. Es ist ein&Anfang!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. C'est un début!" };
    } else if (percentageCollected <= 0.5) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Progress!",
                "Ein %yTriforce-Splitter%w! Du hast&%g[[current]]%w von %c[[required]]%w gefunden. Es geht voran!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. Ça avance!" };
    } else if (percentageCollected <= 0.75) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Over half-way&there!",
                "Ein %yTriforce-Splitter%w! Du hast&schon %g[[current]]%w von %c[[required]]%w gefunden. Schon&über "
                "die Hälfte!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. Il en reste un&peu moins que la moitié!" };
    } else if (percentageCollected < 1.0) {
        msg = {
            "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Almost done!",
            "Ein %yTriforce-Splitter%w! Du hast&schon %g[[current]]%w von %c[[required]]%w gefunden. Fast&geschafft!",
            "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste %c[[remaining]]%w "
            "à trouver. C'est presque&terminé!"
        };
    } else if (current == required) {
        msg = { "You completed the %yTriforce of&Courage%w! %gGG%w!",
                "Das %yTriforce des Mutes%w! Du hast&alle Splitter gefunden. %gGut gemacht%w!",
                "Vous avez complété la %yTriforce&du Courage%w! %gFélicitations%w!" };
    } else {
        msg = { "You found a spare %yTriforce Piece%w!&You only needed %c[[required]]%w, but you have %g[[current]]%w!",
                "Ein übriger %yTriforce-Splitter%w! Du&hast nun %g[[current]]%w von %c[[required]]%w nötigen gefunden.",
                "Vous avez trouvé un %yFragment de&Triforce%w en plus! Vous n'aviez besoin&que de %c[[required]]%w, "
                "mais vous en avez %g[[current]]%w en&tout!" };
    }
    msg.Replace("[[current]]", std::to_string(current));
    msg.Replace("[[remaining]]", std::to_string(remaining));
    msg.Replace("[[required]]", std::to_string(required));
    msg.Format();
}

void BuildCustomItemMessage(Player* player, CustomMessage& msg) {
    int16_t rgid;
    msg = CustomMessage("You found [[article]][[color]][[name]]%w!",
                        "Du erhältst [[article]][[color]][[name]]%w gefunden!",
                        "Vous avez trouvé [[article]][[color]][[name]]%w!", TEXTBOX_TYPE_BLUE);
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        rgid = player->getItemEntry.getItemId;
    } else {
        rgid = player->getItemId;
    }
    CustomMessage name =
        CustomMessage(Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetName(), TEXTBOX_TYPE_BLUE);
    CustomMessage article = CustomMessage(
        Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetArticle(), TEXTBOX_TYPE_BLUE);
    msg.Replace("[[article]]", article);
    msg.Replace("[[color]]", Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetColor());
    msg.Replace("[[name]]", name);
    msg.AutoFormat();
}

void BuildItemMessage(u16* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    CustomMessage msg;

    if (player->getItemEntry.getItemId == RG_ICE_TRAP) {
        BuildIceTrapMessage(msg);
    } else if (player->getItemEntry.getItemId == RG_TRIFORCE_PIECE) {
        BuildTriforcePieceMessage(msg);
    } else {
        BuildCustomItemMessage(player, msg);
    }
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void BuildMapMessage(uint16_t* textId, bool* loadFromMessageTable) {
    GetItemEntry itemEntry = GET_PLAYER(gPlayState)->getItemEntry;
    auto ctx = OTRGlobals::Instance->gRandoContext;
    CustomMessage msg =
        CustomMessage("You found the %g[[name]]%w! [[typeHint]]", "Du erhältst das %g[[name]]%w! [[typeHint]]",
                      "Vous ebtenez %g[[name]]%w! [[typeHint]]", TEXTBOX_TYPE_BLUE);
    int sceneNum;
    switch (itemEntry.getItemId) {
        case RG_DEKU_TREE_MAP:
            sceneNum = SCENE_DEKU_TREE;
            break;
        case RG_DODONGOS_CAVERN_MAP:
            sceneNum = SCENE_DODONGOS_CAVERN;
            break;
        case RG_JABU_JABUS_BELLY_MAP:
            sceneNum = SCENE_JABU_JABU;
            break;
        case RG_FOREST_TEMPLE_MAP:
            sceneNum = SCENE_FOREST_TEMPLE;
            break;
        case RG_FIRE_TEMPLE_MAP:
            sceneNum = SCENE_FIRE_TEMPLE;
            break;
        case RG_WATER_TEMPLE_MAP:
            sceneNum = SCENE_WATER_TEMPLE;
            break;
        case RG_SPIRIT_TEMPLE_MAP:
            sceneNum = SCENE_SPIRIT_TEMPLE;
            break;
        case RG_SHADOW_TEMPLE_MAP:
            sceneNum = SCENE_SHADOW_TEMPLE;
            break;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            sceneNum = SCENE_BOTTOM_OF_THE_WELL;
            break;
        case RG_ICE_CAVERN_MAP:
            sceneNum = SCENE_ICE_CAVERN;
            break;
    }
    CustomMessage name =
        CustomMessage(Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(itemEntry.getItemId)).GetName());
    msg.Replace("[[name]]", name);
    if (ctx->GetOption(RSK_MQ_DUNGEON_RANDOM).Is(RO_MQ_DUNGEONS_NONE) ||
        (ctx->GetOption(RSK_MQ_DUNGEON_RANDOM).Is(RO_MQ_DUNGEONS_SET_NUMBER) &&
         ctx->GetOption(RSK_MQ_DUNGEON_COUNT).Is(12))) {
        msg.Replace("[[typeHint]]", "");
    } else if (ResourceMgr_IsSceneMasterQuest(sceneNum)) {
        msg.Replace("[[typeHint]]", Rando::StaticData::hintTextTable[RHT_DUNGEON_MASTERFUL].GetHintMessage());
    } else {
        msg.Replace("[[typeHint]]", Rando::StaticData::hintTextTable[RHT_DUNGEON_ORDINARY].GetHintMessage());
    }
    *loadFromMessageTable = false;
    msg.AutoFormat(ITEM_DUNGEON_MAP);
    msg.LoadIntoFont();
}

void BuildBossKeyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->getItemEntry.getItemId == RG_GANONS_CASTLE_BOSS_KEY &&
        !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_GANONS_BOSS_KEY)) {
        return;
    }
    if (player->getItemEntry.getItemId != RG_GANONS_CASTLE_BOSS_KEY &&
        !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_BOSS_KEYSANITY)) {
        return;
    }
    CustomMessage msg;
    BuildCustomItemMessage(player, msg);
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void BuildSmallKeyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->getItemEntry.getItemId == RG_GERUDO_FORTRESS_SMALL_KEY &&
        OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_KEYS).Is(RO_GERUDO_KEYS_VANILLA)) {
        return;
    }
    if (player->getItemEntry.getItemId != RG_GERUDO_FORTRESS_SMALL_KEY &&
        DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_KEYSANITY)) {
        return;
    }
    CustomMessage msg;
    BuildCustomItemMessage(player, msg);
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void RegisterItemMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_RANDOMIZER_CUSTOM_ITEM, IS_RANDO, BuildItemMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_DUNGEON_MAP, DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS),
                 BuildMapMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_COMPASS, DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS),
                 BuildItemMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_KEY_BOSS,
                 (DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_BOSS_KEYSANITY) ||
                  DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_GANONS_BOSS_KEY)),
                 BuildBossKeyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_KEY_SMALL,
                 (OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_KEYS).IsNot(RO_GERUDO_KEYS_VANILLA) ||
                  DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_KEYSANITY)),
                 BuildSmallKeyMessage);
}

static RegisterShipInitFunc initFunc(RegisterItemMessages, { "IS_RANDO" });