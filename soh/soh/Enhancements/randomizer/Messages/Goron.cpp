/**
 * Handles the randomized silly messages that the Fire Temple
 * trapped Gorons have when you free them.
 */
#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
}

static CustomMessage FireTempleGoronMessages[] = {
    {
        "Are you the one they call %g@%w?^You look really weird for %rDarunia's kid.%w&Are you adopted?",
        "Du bist also der, den sie @ nennen?^Du siehst nicht aus als wärst Du&%rDarunias Kind.%w Bist Du "
        "adoptiert?",
        "C'est toi qu'on appelle %g@%w?^Tu es vraiment bizarre pour être&le %rfils du Chef%w. Tu as été adopté?",
    },
    {
        "Thank Hylia! I was so worried about&when my teacher would let me get&out of detention.^I gotta go home "
        "and see my parents.",
        "Ich wollte nur dieses Ding hier wieder&in seine Truhe zurücklegen, weil...^...gehört mir ja eigentlich "
        "nicht,&weißt Du?^Doch dann ging plötzlich dieses&Tor hinter mir zu.&Danke für die Rettung.",
        "Par les déesses!&Mon Frère?!&C'est bien toi?&Comment ça on ne se connaît pas?^Tu trouves vraiment que "
        "je&ressemble à n'importe quel Goron?",
    },
    {
        "How long has it been, do you know?^%r[[days]]%w days!?^Oh no, and it's %r\x1F%w?&I have to check on my "
        "cake!!",
        "Weißt Du zufällig, wie viele Tage&vergangen sind?^%r[[days]]%w Tage!?^Oh je, und es ist %r\x1F%w Uhr? "
        "Ich&muss dringend nach meinem Kuchen&sehen!!!",
        "Cela fait combien de temps que&je suis enfermé ici?&Non mais je ne vais pas crier.^COMBIEN?! %r[[days]]%w "
        "JOURS!?^En plus il est %r\x1F%w...&Il faut vraiment que je rentre...",
    },
    {
        // 0x39C7 - ganon laugh
        "\x12\x39\xC7You fell into my %rtrap!%w&Foolish boy, it was me, Ganondorf!!!^...whoa, where am I?&What "
        "happened?^Weird.",
        "\x12\x39\xC7"
        "Du bist mir in die %rFalle%w gegangen!&Du Narr, ich bin es, %rGanondorf%w!!!^...Huch? Wo bin ich? Was ist "
        "passiert?^Seltsam...",
        "\x12\x39\xC7Tu es tombé dans mon %rpiège%w!&Tu croyais que j'étais un Goron mais,&c'était moi! "
        "%rGanondorf%w!^...Hein? Où suis-je?&Que s'est-il passé?",
    },
    {
        "Thanks, but I don't know if I wanna go&just yet...^Hmm...^...^...^...^...^...maybe I can come back "
        "later.&Bye bye.",
        "Danke für die Rettung, aber&eigentlich finde ich es hier ganz&nett...^Hmm...^...^...^...^...^...Naja, ich "
        "kann ja jederzeit&wiederkommen. Man sieht sich.",
        "Merci, mais je me sens plus en&sécurité ici...^...^...^...^...^Hmm...^...Tout compte fait, je vais y "
        "aller.&A plus tard.",
    },
    { "Do you know about %b\x9f%w?&It's this weird symbol that's been&in my dreams lately...^Apparently, you "
      "pressed it %b[[a_btn]]%w times.^Wow.",
      "Weißt Du über %b\x9f%w Bescheid?&Es sind Symbole, die mir&in letzter Zeit öfter in&meinen Träumen "
      "erschienen sind...^Es scheint, dass Du sie schon&%b[[a_btn]]%w mal betätigt hast.^Faszinierend...",
      "Tu as déjà entendu parler du&symbole %b\x9f%w?&C'est un symbole bizarre qui est&apparu dans mes rêves "
      "dernièrement...^Apparemment, tu as appuyé dessus&%b[[a_btn]]%w fois.^Wow..." },
    {
        "\x13\x1A"
        "Boy, you must be hot!&Get yourself a bottle of&%rLon Lon Milk%w right away and cool&down, for only %g30%w "
        "rupees!",
        "\x13\x1A"
        "Hey, ist Dir nicht zu warm?&Besorge Dir doch eine Flasche&%rLon Lon-Milch%w, um Dich&abzukühlen.^Kostet "
        "Dich auch nur %g30%w Rubine!",
        "\x13\x1A"
        "Woah! Tu dois avoir chaud!&Tu savais que tu pouvais acheter&du %rLait de Lon Lon%w pour&seulement %g30 "
        "rubis%w?^Il n'y a rien de mieux pour s'hydrater!",
    },
    {
        "In that case, I'll help you out!^They say that %rthe thing you're&looking for%w can only be found%g "
        "when&you're not looking for it.%w^Hope that helps!",
        "Pass auf, ich gebe Dir einen Tipp!^Man sagt, man findet %rdas was&man sucht%w nur, und wirklich nur&dann, "
        "%gwenn man gerade nicht danach&sucht%w.^Du kannst mich jederzeit wieder für&mehr hilfreiche Tipps "
        "aufsuchen!",
        "Dans ce cas, je vais t'aider!&On dit que l'objet que tu cherches&ne peut être trouvé que lorsque&tu ne le "
        "cherches pas.",
    },
    {
        "I dunno why I was thrown in here,&truth be told.&I'm just a %g\"PR\"%w person.",
        "Wat weiß'n ich, wieso ich hier&eingepfercht wurd. Ich mach&doch nur %g\"Pull&Requests\"%w.",
        "Je ne sais pas comment on m'a jeté&ici. Il faut croire que je dors comme&une pierre.",
    },
};

void BuildGoronMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = ShipUtils::RandomElement(FireTempleGoronMessages);
    msg.Replace("[[days]]", std::to_string(gSaveContext.totalDays));
    msg.Replace("[[a_btn]]", std::to_string(gSaveContext.ship.stats.count[COUNT_BUTTON_PRESSES_A]));
    msg.Format();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterGoronMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_OWE_YOU_BIG_TIME, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FALLING_DOORS_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FIRE_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FLAME_DANCER_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SWITCH_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_FLAME_DANCER_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SWITCH_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_OCARINA_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_PILLAR_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_HIDDEN_DOOR_SECRET, IS_RANDO, BuildGoronMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FIRE_TEMPLE_GORON_SOUNDS_DIFFERENT_SECRET, IS_RANDO, BuildGoronMessage);
}

static RegisterShipInitFunc initFunc(RegisterGoronMessages, { "IS_RANDO" });