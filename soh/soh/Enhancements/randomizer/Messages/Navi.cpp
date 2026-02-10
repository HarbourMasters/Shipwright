/**
 * This file is responsible for the messages
 * for the Rando-Relevant Navi Hints enhancement.
 */
#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

static CustomMessage NaviMessages[] = {

    { "%cMissing a small key in a dungeon?&Maybe the %rboss %chas it!",
      "%cFehlt Dir ein kleiner Schlüssel in &einem Labyrinth? Vielleicht hat ihn&ja der %rEndgegner%c!",
      "%cIl te manque une %wPetite Clé %cdans&un donjon? C'est peut-être le %rboss&%cqui l'a!" },

    { "%cSometimes you can use the %rMegaton&Hammer %cinstead of bombs!",
      "%cManchmal kannst Du den %rStahlhammer&%cstatt Bomben verwenden!",
      "%cParfois, tu peux utiliser la %rMasse&des Titans %cau lieu de tes bombes!" },

    { "%cStuck on this seed? You could &throw in the towel and check the&%wspoiler log%c...",
      "%cHängst Du bei diesem Seed fest?&Du könntest die Flinte ins Korn&werfen und ins %wSpoiler Log %cschauen...",
      "%cSi tu es coincé sur cette seed,&tu peux toujours jeter l'éponge&et regader le %wSpoiler log%c..." },

    { "%cDid you know that the %yHover&Boots %ccan be used to cross&%wquicksand%c?",
      "%cWußtest Du, daß Du mit den&%yGleitstiefeln %cTreibsand %wüberqueren&kannst%c?",
      "%cEst-ce que tu savais que les %rBottes&des airs %cpouvaient être utilisées&pour traverser les %wsables "
      "mouvants%c?" },

    { "%cYou can reach the back of %wImpa's&House %cby jumping from the&unfinished house with a %rcucco%c!",
      "%cDu kannst den Balkon von %wImpas&Haus %cerreichen indem Du von&der Baustelle aus mit einem "
      "%rHuhn&%cspringst!",
      "%cTu peux atteindre l'arrière de la&%wMaison d'Impa %cen sautant depuis la&maison en construction avec "
      "une&%rcocotte%c!" },

    { "%cThe %ySpirit Trial %cin %pGanon's Castle&%chas a %whidden chest %csomewhere.&Did you already know that?",
      "%cDie %yGeister-Prüfung %cin %pGanons&Schloß %chat irgendwo eine&%wversteckte Kiste%c. Weißt Du schon&wo?",
      "%cL'%yÉpreuve de l'Esprit%c dans le %pChâteau&de Ganon %ca un coffre caché quelque&part. Je suppose que tu "
      "le savais&déjà?" },

    { "%cYou know the %wBombchu Bowling&Alley%c? I heard %wonly two of the &prizes %care worthwhile. The rest &is "
      "junk!",
      "%cKennst Du die %wMinenbowlingbahn%c?&Ich habe gehört, daß sich nur &%wzwei der Preise%c lohnen. Der "
      "Rest&ist Krimskrams!",
      "%cEst-ce que tu savais qu'au %wBowling&Teigneux%c, il n'y a que les %wdeux&premiers prix %cqui sont "
      "intéréssant?" },

    { "%cHave you been using %wDeku Nuts&%cenough? I've seen them blow up&a %rBombchu%c!",
      "%cBenutzt Du auch genügend %wDeku-Nüsse%c?&Ich habe mal gesehen, daß&man damit %rKrabbelminen "
      "%cdetonieren&lassen kann!",
      "%cTu es sûr d'utiliser tes %wNoix Mojo &%ccorrectement? J'en ai déjà vu&exploser des %rChoux-Péteurs%c!" },

    { "%cYou might end up with an %wextra&key %cfor the %bWater Temple %cor the&%rFire Temple%c. It's for your "
      "safety!",
      "%cVielleicht verbleibt Dir im&%bWassertempel %coder %rFeuertempel %cein&%wzusätzlicher Schlüssel%c. "
      "Dies&ist zu Deiner Sicherheit!",
      "%cIl se peut que tu aies une %wPetite&Clé %cen trop dans le %bTemple de l'Eau&%cou le %rTemple du Feu%c. "
      "C'est pour ta&propre sécurité!" },

    { "%cIf you can't pick up a %rbomb&flower %cwith your hands, you can&still detonate it with %rfire %cor&with "
      "%warrows%c!",
      "%cNur weil Du eine %rDonnerblume&%cnicht hochheben kannst, so kannst&Du sie noch immer mit %rFeuer "
      "%coder&%wPfeilen %cdetonieren lassen!",
      "%cSi tu ne peux pas ramasser&un %rChoux-Péteur %cavec tes mains, tu&peux toujours le faire exploser&avec du "
      "%rFeu %cou avec des %wflèches%c!" },

    { "%cEven an adult can't push large&blocks without some %wextra&strength%c!",
      "%cSelbst ein Erwachsener kann ohne&etwas %wzusätzliche Kraft %ckeine&großen Blöcke verschieben!",
      "%cMême un adulte ne peut pas pousser&de grands blocs sans un petit %wgain&de force%c!" },

    { "%cI've heard that %rFlare Dancer&%cis weak to the %wMaster Sword%c!&Have you tried it?",
      "%cIch habe gehört, daß der&%rFlammenderwisch %ceine Schwäche für&das %wMaster-Schwert %caufweist. Hast Du&es "
      "schon einmal versucht einzusetzen?",
      "%cJ'ai entendu dire que les %rDanse-&Flammes %csont faîbles face à l'%wÉpée de&Légende%c! Est-ce que tu as "
      "essayé?" },

    { "%cDon't have a weapon to kill a&%rspider%c? Try throwing a %wpot&%cat it!",
      "%cFehlt Dir die Waffe um gegen&eine %rSkulltula %czu kämpfen? Versuche&sie mit einem %wKrug %cabzuwerfen!",
      "%cSi tu n'as pas d'arme pour tuer&une %raraignée%c, pourquoi n'essayerais&-tu pas de lui jetter une "
      "%wjarre&%cà la figure?" },

    { "%cI hear the patch of %wsoft soil&%cin %bZora's River %cis the only one&that isn't home to a %rspider%c!",
      "%cIch habe gehört, daß die Stelle&%wfeuchten Bodens %cim %bZora-Fluß %cals&einzige keine %rSkulltula "
      "%cbeherbergt.",
      "%cJ'ai entendu dire que la %wterre meuble&%cqui se trouve à la %bRivière Zora %cest&la seule qui ne "
      "contienne pas&d'%raraignée%c." },

    { "%cThe people of Hyrule sometimes&have %witems %cfor you, but they won't&like it if you're %wwearing a "
      "mask%c!",
      "%cDie Bewohner Hyrules haben manchmal&%wGegenstände %cfür Dich, aber sie mögen&es nicht, wenn Du %wMasken "
      "trägst%c!",
      "%cIl se peut que les habitants d'Hyrule&aient des %wobjets %cpour toi. Par contre,&ils risquent de ne pas "
      "trop apprécier&le fait que tu %wportes un masque%c!" },

    { "%cIf you get trapped somewhere, you&might have to %wsave your game %cand&%wreset%c!",
      "%cSolltest Du irgendwo eingeschlossen&sein, mußt Du vielleicht Dein %wSpiel&speichern %cund %wneu starten%c!",
      "%cSi tu es coincé quelque part, tu&devrais %wsauvegarder ta partie %cet&faire un %wreset%c!" },

    { "%cSheik will meet you in a %rburning&village %conce you have %gForest%c,&%rFire%c, and %bWater "
      "%cMedallions!",
      "%cShiek wird Dich in einem %rbrennenden&Dorf %ctreffen, sobald Du das Amulett&des %gWaldes%c, %rFeuers %cund "
      "%bWassers&%cbesitzt.",
      "%cSheik t'attendra dans un %rvillage&en feu %clorsque tu auras récupéré&les médaillons de la %gForêt%c, du "
      "%rFeu&%cet de l'%bEau%c!" },

    { "%cIf you don't have a %wsword %cas a&child, try buying %wDeku Sticks%c!&They're effective against your "
      "foes!",
      "%cSolltest Du als Kind kein %wSchwert&%cbesitzen, empfehle ich %wDeku-Stäbe&%czu kaufen! Diese sind "
      "effektiv gegen&Widersacher!",
      "%cSi tu n'as pas d'%wépée %cen tant&qu'enfant, pourquoi n'irais-tu pas&acheter quelques %wBâtons Mojo%c? "
      "Ils&sont efficaces contre tes ennemis!" }
};

void BuildNaviMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = ShipUtils::RandomElement(NaviMessages);
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterNaviMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_DEKU_TREE_SUMMONS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CMON_BE_BRAVE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_VISIT_THE_PRINCESS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FIND_MALONS_FATHER,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FIND_THE_PRINCESS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHAT_WOULD_SARIA_SAY,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IMPA_SAID_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_USE_BOMB_FLOWER,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_FAIRY_LIVES_ON_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SARIA_KNOWS_ABOUT_STONES,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_RUTO_INSIDE_JABUS_BELLY,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_COLLECTED_THREE_STONES,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_THREW_SOMETHING_IN_MOAT,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CHECK_TEMPLE_OF_TIME,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SHOULD_WE_BELIEVE_SHEIK,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHATS_GOING_ON_IN_FOREST,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_CLOUD_OVER_DEATH_MOUNTAIN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_ARTIC_WIND_IS_BLOWING,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IRON_BOOTS_WEIGH_A_TON,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_LOOK_FOR_SOMONE_WHO_KNOWS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_IT_CAME_OUT_OF_THE_WELL,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHO_BUILT_THE_SPIRIT_TEMPLE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_HAVE_YOU_EVER_PLAYED_NOCTURNE,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHERE_GANONDORF_WAS_BORN,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_EQUIP_THE_SILVER_GAUNTLETS,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_WHO_IS_WAITING_FOR_US,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_SAVE_PRINCESS_ZELDA,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
    COND_ID_HOOK(OnOpenText, TEXT_NAVI_TRY_TO_KEEP_MOVING,
                 IS_RANDO && CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), 1), BuildNaviMessage);
}

static RegisterShipInitFunc initFunc(RegisterNaviMessages,
                                     { CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), "IS_RANDO" });