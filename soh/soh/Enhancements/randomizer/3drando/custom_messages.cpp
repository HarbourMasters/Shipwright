#include "custom_messages.hpp"
#include "debug.hpp"
#include "shops.hpp"
#include "z64item.h"

#include <array>
#include <set>
#include <sstream>

namespace CustomMessages {

using namespace std::literals::string_literals;

class MessageEntryComp {
public:
    bool operator()(const MessageEntry& lhs, const MessageEntry& rhs) const {
        return lhs.id < rhs.id;
    }
};

constexpr std::array EnglishDungeonNames = {
    "Deku Tree",
    "Dodongo's Cavern",
    "Jabu Jabu's Belly",
    "Forest Temple",
    "Fire Temple",
    "Water Temple",
    "Spirit Temple",
    "Shadow Temple",
    "Bottom of the Well",
    "Ice Cavern",
    "Ganon's Tower",
    "Gerudo Training Grounds",
    "Gerudo Fortress",
    "Ganon's Castle",
};

constexpr std::array FrenchDungeonNames = {
    "Vénérable Arbre Mojo",
    "Caverne Dodongo",
    "Ventre de Jabu-Jabu",
    "Temple de la Forêt",
    "Temple du Feu",
    "Temple de l'Eau",
    "Temple de l'Esprit",
    "Temple de l'Ombre",
    "Puits",
    "Caverne Polaire",
    "Tour de Ganon",
    "Gymnase Gerudo",
    "Repaire des Voleurs",
    "Château de Ganon",
};

constexpr std::array FrenchDungeonArticles = {
    "du ",
    "de la ",
    "du ",
    "du ",
    "du ",
    "du ",
    "du ",
    "du ",
    "du ",
    "de la ",
    "",
    "du ",
    "de la ",
    "du ",
};

constexpr std::array SpanishDungeonNames = {
    "Gran Árbol Deku",
    "Cueva de los Dodongos",
    "Tripa de Jabu-Jabu",
    "Templo del Bosque",
    "Templo del Fuego",
    "Templo del Agua",
    "Templo del Espíritu",
    "Templo de las Sombras",
    "Fondo del pozo",
    "Caverna de hielo",
    "Torre de Ganon",
    "Centro de Instrucción Gerudo",
    "Fortaleza Gerudo",
    "Castillo de Ganon",
};

constexpr std::array SpanishDungeonArticles = {
    "del",
    "de la",
    "de la",
    "del",
    "del",
    "del",
    "del",
    "del",
    "del",
    "de la",
    "de la",
    "del",
    "de la",
    "del",
};

constexpr std::array DungeonColors = {
    QM_GREEN,
    QM_RED,
    QM_BLUE,
    QM_GREEN,
    QM_RED,
    QM_BLUE,
    QM_YELLOW,
    QM_PINK,
    QM_PINK,
    QM_LBLUE,
    QM_BLACK,
    QM_YELLOW,
    QM_YELLOW,
    QM_RED,
};

    std::set<MessageEntry, MessageEntryComp> messageEntries;
    std::vector<MessageEntry> arrangedMessageEntries;
    std::stringstream messageData;
    std::string arrangedMessageData;

    //textBoxType and textBoxPosition are defined here: https://wiki.cloudmodding.com/oot/Text_Format#Message_Id
    void CreateMessage(uint32_t textId, uint32_t unk_04, uint32_t textBoxType, uint32_t textBoxPosition,
                       std::string englishText, std::string frenchText, std::string spanishText) {
            MessageEntry newEntry = { textId, unk_04, textBoxType, textBoxPosition, { 0 } };

            while ((englishText.size() % 4) != 0) {
              englishText += "\0"s;
            }
            messageData.seekg(0, messageData.end);
            newEntry.info[ENGLISH_U].offset = (char*)((int)messageData.tellg());
            newEntry.info[ENGLISH_U].length = englishText.size();
            messageData << englishText;

            while ((frenchText.size() % 4) != 0) {
              frenchText += "\0"s;
            }
            messageData.seekg(0, messageData.end);
            newEntry.info[FRENCH_U].offset = (char*)((int)messageData.tellg());
            newEntry.info[FRENCH_U].length = frenchText.size();
            messageData << frenchText;

            while ((spanishText.size() % 4) != 0) {
              spanishText += "\0"s;
            }
            messageData.seekg(0, messageData.end);
            newEntry.info[SPANISH_U].offset = (char*)((int)messageData.tellg());
            newEntry.info[SPANISH_U].length = spanishText.size();
            messageData << spanishText;

            messageEntries.insert(newEntry);
    }

    void CreateMessageFromTextObject(uint32_t textId, uint32_t unk_04, uint32_t textBoxType, uint32_t textBoxPosition, const Text& text) {
        CreateMessage(textId, unk_04, textBoxType, textBoxPosition, text.GetEnglish(), text.GetFrench(), text.GetSpanish());
    }

    uint32_t NumMessages() {
        return messageEntries.size();
    }

    std::pair<const char*, uint32_t> RawMessageEntryData() {
        arrangedMessageEntries.assign(messageEntries.begin(), messageEntries.end());
        const char* data = (const char*)arrangedMessageEntries.data();
        uint32_t size = arrangedMessageEntries.size() * sizeof(MessageEntry);
        return { data, size };
    }

    std::pair<const char*, uint32_t> RawMessageData() {
        arrangedMessageData = messageData.str();
        const char* data = arrangedMessageData.data();
        uint32_t size = arrangedMessageData.size();
        return { data, size };
    }

    void CreateAlwaysIncludedMessages() {
        // Bombchu (10) Purchase Prompt
        CreateMessage(0x8C, 0, 2, 3,
            INSTANT_TEXT_ON()+"Bombchu (10): 99 Rupees"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Buy"+NEWLINE()+"Don't buy"+COLOR(QM_WHITE)+MESSAGE_END(),
            INSTANT_TEXT_ON()+"Bombchus (10): 99 rubis"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Acheter"+NEWLINE()+"Ne pas acheter"+COLOR(QM_WHITE)+MESSAGE_END(),
            INSTANT_TEXT_ON()+"Bombchus (10): 99 rupias"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Comprar"+NEWLINE()+"No comprar"+COLOR(QM_WHITE)+MESSAGE_END());
        //Gold Skulltula Tokens (there are two text IDs the game uses)
        for (const uint32_t textId : {0xB4, 0xB5}) {
            CreateMessage(textId, 0, 2, 3,
                INSTANT_TEXT_ON()+"You destroyed a "+COLOR(QM_RED)+"Gold Skulltula"+COLOR(QM_WHITE)+". You got a"+NEWLINE()+"token proving you destroyed it!"+NEWLINE()+NEWLINE()+"You have "+COLOR(QM_RED)+SKULLTULAS_DESTROYED()+COLOR(QM_WHITE)+" tokens!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+"Vous venez de détruire une "+COLOR(QM_RED)+"Skulltula d'or"+COLOR(QM_WHITE)+"!"+NEWLINE()+"Ce symbole prouve votre prouesse!"+NEWLINE()+NEWLINE()+"Vous avez "+COLOR(QM_RED)+SKULLTULAS_DESTROYED()+COLOR(QM_WHITE)+" jetons!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+"¡Has eliminado una "+COLOR(QM_RED)+"skulltula dorada"+COLOR(QM_WHITE)+" y has"+NEWLINE()+"conseguido un símbolo para probarlo!"+NEWLINE()+NEWLINE()+"¡Tienes "+COLOR(QM_RED)+SKULLTULAS_DESTROYED()+COLOR(QM_WHITE)+" símbolos!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }

        //Bombchu (10) Description
        CreateMessage(0xBC, 0, 2, 3,
            INSTANT_TEXT_ON()+COLOR(QM_RED)+"Bombchu (10): 99 Rupees"+NEWLINE()+COLOR(QM_WHITE)+"These look like toy mice, but they're"+NEWLINE()+"actually self-propelled time bombs!"+INSTANT_TEXT_OFF()+SHOP_MESSAGE_BOX()+MESSAGE_END(),
            INSTANT_TEXT_ON()+COLOR(QM_RED)+"Bombchus (10): 99 rubis"+NEWLINE()+COLOR(QM_WHITE)+"Profilée comme une souris mécanique, il"+NEWLINE()+"s'agit en fait d'une bombe à retardement"+NEWLINE()+"autopropulsée!"+INSTANT_TEXT_OFF()+SHOP_MESSAGE_BOX()+MESSAGE_END(),
            INSTANT_TEXT_ON()+COLOR(QM_RED)+"Bombchus (10): 99 rupias"+NEWLINE()+COLOR(QM_WHITE)+"Aunque parezcan ratoncitos de juguete,"+NEWLINE()+"¡son bombas de relojería autopropulsadas!"+INSTANT_TEXT_OFF()+SHOP_MESSAGE_BOX()+MESSAGE_END());
        //Boss Keys
        for (uint32_t bossKey = 0; bossKey <= (DUNGEON_SHADOW_TEMPLE - DUNGEON_FOREST_TEMPLE); bossKey++) {
            uint32_t dungeon = DUNGEON_FOREST_TEMPLE + bossKey;
            CreateMessage(0x9D4 + bossKey, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"You got the "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Boss Key"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"Vous trouvez la "+COLOR(DungeonColors[dungeon])+"Clé d'Or "+NEWLINE()+FrenchDungeonArticles[dungeon]+" "+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"¡Tienes la "+COLOR(DungeonColors[dungeon])+"gran llave "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        CreateMessage(0x9D9, 0, 2, 3,
            UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"You got the "+COLOR(DungeonColors[DUNGEON_GANONS_CASTLE_FIRST_PART])+EnglishDungeonNames[DUNGEON_GANONS_CASTLE_FIRST_PART]+NEWLINE()+"Boss Key"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"Vous trouvez la "+COLOR(DungeonColors[DUNGEON_GANONS_CASTLE_FIRST_PART])+"Clé d'Or "+NEWLINE()+FrenchDungeonArticles[DUNGEON_GANONS_CASTLE_FIRST_PART]+" "+FrenchDungeonNames[DUNGEON_GANONS_CASTLE_FIRST_PART]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_BOSS)+INSTANT_TEXT_ON()+"¡Tienes la "+COLOR(DungeonColors[DUNGEON_GANONS_CASTLE_FIRST_PART])+"gran llave "+SpanishDungeonArticles[DUNGEON_GANONS_CASTLE_FIRST_PART]+NEWLINE()+SpanishDungeonNames[DUNGEON_GANONS_CASTLE_FIRST_PART]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        //Compasses
        for (uint32_t dungeon = DUNGEON_DEKU_TREE; dungeon <= DUNGEON_ICE_CAVERN; dungeon++) {
            CreateMessage(0x9DA + dungeon, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_COMPASS)+INSTANT_TEXT_ON()+"You got the "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Compass"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_COMPASS)+INSTANT_TEXT_ON()+"Vous trouvez la "+COLOR(DungeonColors[dungeon])+"boussole "+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_COMPASS)+INSTANT_TEXT_ON()+"¡Tienes la "+COLOR(DungeonColors[dungeon])+"brújula "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //Maps
        for (uint32_t dungeon = DUNGEON_DEKU_TREE; dungeon <= DUNGEON_ICE_CAVERN; dungeon++) {
            CreateMessage(0x9E4 + dungeon, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_DUNGEON_MAP)+INSTANT_TEXT_ON()+"You got the "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Map"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_DUNGEON_MAP)+INSTANT_TEXT_ON()+"Vous trouvez la "+COLOR(DungeonColors[dungeon])+"carte "+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_DUNGEON_MAP)+INSTANT_TEXT_ON()+"¡Has obtenido el "+COLOR(DungeonColors[dungeon])+"mapa "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //Small Keys
        for (uint32_t smallKey = 0; smallKey <= (DUNGEON_BOTTOM_OF_THE_WELL - DUNGEON_FOREST_TEMPLE); smallKey++) {
            uint32_t dungeon = DUNGEON_FOREST_TEMPLE + smallKey;
            CreateMessage(0x9EE + smallKey, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got a "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Small Key"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez une "+COLOR(DungeonColors[dungeon])+"Petite Clé"+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido una "+COLOR(DungeonColors[dungeon])+"llave pequeña "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        for (uint32_t smallKey = 0; smallKey <= (DUNGEON_GANONS_CASTLE_FIRST_PART - DUNGEON_GERUDO_TRAINING_GROUNDS); smallKey++) {
            uint32_t dungeon = DUNGEON_GERUDO_TRAINING_GROUNDS + smallKey;
            CreateMessage(0x9F4 + smallKey, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got a "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Small Key"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez une "+COLOR(DungeonColors[dungeon])+"Petite Clé"+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido una "+COLOR(DungeonColors[dungeon])+"llave pequeña "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //Key Rings
        for (uint32_t smallKey = 0; smallKey <= (DUNGEON_BOTTOM_OF_THE_WELL - DUNGEON_FOREST_TEMPLE); smallKey++) {
            uint32_t dungeon = DUNGEON_FOREST_TEMPLE + smallKey;
            CreateMessage(0x9300 + smallKey, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got a "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Key Ring"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez un "+COLOR(DungeonColors[dungeon])+"trousseau"+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido un "+COLOR(DungeonColors[dungeon])+"llavero "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        for (uint32_t smallKey = 0; smallKey <= (DUNGEON_GANONS_CASTLE_FIRST_PART - DUNGEON_GERUDO_TRAINING_GROUNDS); smallKey++) {
            uint32_t dungeon = DUNGEON_GERUDO_TRAINING_GROUNDS + smallKey;
            CreateMessage(0x9306 + smallKey, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got a "+COLOR(DungeonColors[dungeon])+EnglishDungeonNames[dungeon]+NEWLINE()+"Key Ring"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez un "+COLOR(DungeonColors[dungeon])+"trousseau"+NEWLINE()+FrenchDungeonArticles[dungeon]+FrenchDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido un "+COLOR(DungeonColors[dungeon])+"llavero "+SpanishDungeonArticles[dungeon]+NEWLINE()+SpanishDungeonNames[dungeon]+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //Tycoon's Wallet
        CreateMessage(0x09F7, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_WALLET_GIANT)+INSTANT_TEXT_ON()+"You got a "+COLOR(QM_RED)+"Tycoon's Wallet"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+NEWLINE()+"It's gigantic! Now you can carry"+NEWLINE()+"up to "+COLOR(QM_YELLOW)+"999 "+COLOR(QM_WHITE)+COLOR(QM_YELLOW)+"Rupees"+COLOR(QM_WHITE)+"!"+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_WALLET_GIANT)+INSTANT_TEXT_ON()+"Vous obtenez la "+COLOR(QM_RED)+"Bourse de star"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+NEWLINE()+"Elle peut contenir jusqu'à "+COLOR(QM_YELLOW)+"999 "+COLOR(QM_WHITE)+COLOR(QM_YELLOW)+"rubis"+COLOR(QM_WHITE)+"!"+NEWLINE()+"C'est gigantesque!"+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_WALLET_GIANT)+INSTANT_TEXT_ON()+"¡Has conseguido una "+COLOR(QM_RED)+"bolsa para ricachones"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+NEWLINE()+"¡Qué descomunal! Ya puedes llevar"+NEWLINE()+"hasta "+COLOR(QM_YELLOW)+"999 "+COLOR(QM_WHITE)+COLOR(QM_YELLOW)+"rupias"+COLOR(QM_WHITE)+"!"+MESSAGE_END());
        //Saria's Song Default Hint
        CreateMessage(0x0A00, 0, 2, 3,
            UNSKIPPABLE()+"Have you tried talking to the gossip"+NEWLINE()+ "stones around Hyrule? They might have"+NEWLINE()+"some good advice... Hee hee!"+WAIT_FOR_INPUT()+"If you learn something from the gossip stones,"+NEWLINE()+"I will remember it!"+EVENT_TRIGGER()+MESSAGE_END(),
            UNSKIPPABLE()+"As-tu parlé aux pierres à potins"+NEWLINE()+ "dans Hyrule? Elles sont de bons conseils..."+NEWLINE()+"Hi hi!"+WAIT_FOR_INPUT()+"Si elles te révèlent quelque chose,"+NEWLINE()+"je m'en souviendrai!"+EVENT_TRIGGER()+MESSAGE_END(),
            UNSKIPPABLE()+"¿Has probado a consultarle a las"+NEWLINE()+ "piedras chismosas esparcidas por Hyrule? Puede"+NEWLINE()+"que sean de ayuda a tu empresa... ¡Ji, ji!"+WAIT_FOR_INPUT()+"¡Puedo recordarte todo lo que aprendas de ellas,"+NEWLINE()+"si así lo deseas!"+EVENT_TRIGGER()+MESSAGE_END());
        //Poe Collector (when enough has been sold)
        CreateMessage(0x70F8, 0, 0, 0,
            UNSKIPPABLE()+"Wait a minute! WOW!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"You have earned enough points!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Young man, you are a genuine "+COLOR(QM_RED)+"ghost hunter"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Is that what you expected me to say?"+NEWLINE()+"Heh heh heh!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Because of you, I have extra inventory of"+NEWLINE()+COLOR(QM_RED)+"Big Poes"+COLOR(QM_WHITE)+", so this will be the last time I can"+NEWLINE()
                +"buy one of these ghosts."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"You're thinking about what I promised would"+NEWLINE()+"happen when you earned enough points."+NEWLINE()+"Heh heh."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Don't worry. I didn't forget. Just take this."+MESSAGE_END(),
            UNSKIPPABLE()+"Ooooh! WHOA!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Tu as obtenu suffisamment de points!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Tu es un véritable "+COLOR(QM_RED)+"chasseur de fantômes"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Il est content, hein?"+NEWLINE()+"Il est content le monsieur?"+NEWLINE()+"Hé hé hé!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Grâce à toi, mon stock d'"+COLOR(QM_RED)+"Âmes"+COLOR(QM_WHITE)+" est plein!"+NEWLINE()+"C'est donc la dernière fois que nous"+NEWLINE()+"faisons affaire."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Je sais, je sais..."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Nous avions passé un pacte..."+NEWLINE()+"Tu as eu tes points et je t'en félicite..."+NEWLINE()+"Hé hé hé!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Alors prends donc ceci, mon bon ami!"+MESSAGE_END(),
            UNSKIPPABLE()+"¡Un momento! ¡OYE!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¡Has conseguido los puntos suficientes!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¡Jovencito, eres un auténtico "+COLOR(QM_RED)+"cazador de"+NEWLINE()+"fantasmas"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"¿Era eso lo que esperabas que dijera?"+NEWLINE()+"¡Je, je je!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Gracias a ti, ya tengo la cantidad necesaria"+NEWLINE()+"de "+COLOR(QM_RED)+"grandes poes"+COLOR(QM_WHITE)+", así que esta será la"+NEWLINE()
                +"última vez que te compre unos de ese tipo."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¿Recuerdas lo que te dije que ocurriría"+NEWLINE()+"cuando tuvieses suficientes puntos?"+NEWLINE()+"Je, je, je."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                +"Tranquilo, que no se me ha olvidado."+NEWLINE()+"Toma esto."+MESSAGE_END());
        //Ice Trap
        CreateMessage(0x9002, 0, 2, 3,
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+COLOR(QM_RED)+"FOOL!"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+COLOR(QM_RED)+"IDIOT!"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+COLOR(QM_RED)+"¡TONTO!"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END());
        //Business Scrubs
        //The less significant byte represents the price of the item
        for (uint32_t price = 0; price <= 95; price += 5) {
            CreateMessage(0x9000 + price, 0, 0, 0,
                INSTANT_TEXT_ON()+"I'll sell you something good for "+COLOR(QM_RED)+std::to_string(price)+" Rupees"+COLOR(QM_WHITE)+"!"+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"OK"+NEWLINE()+"No way"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+"Je te vends un truc super pour "+COLOR(QM_RED)+std::to_string(price)+" Rubis"+COLOR(QM_WHITE)+"!"+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"D'accord"+NEWLINE()+"Hors de question"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+"¡Te puedo vender algo bueno por "+COLOR(QM_RED)+std::to_string(price)+" rupias"+COLOR(QM_WHITE)+"!"+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Vale"+NEWLINE()+"Ni hablar"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //Poe Collector
        //The last digit represent the number of poes needed to collect
        for (uint32_t poes = 1; poes <= 10; poes++) {
            CreateMessage(0x9080 + poes, 0, 0, 0,
                UNSKIPPABLE()+"Oh, you brought a Poe today!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Hmmmm!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Very interesting! This is a "+COLOR(QM_RED)+"Big Poe"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"I'll buy it for "+COLOR(QM_RED)+"50 Rupees"+COLOR(QM_WHITE)+"."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"On top of that, I'll put "+COLOR(QM_RED)+"100 points "+COLOR(QM_WHITE)+"on"+NEWLINE()+"your card."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"If you earn "+COLOR(QM_RED)+std::to_string(poes * 100)+" points"+COLOR(QM_WHITE)+", you'll be a"+NEWLINE()+"happy man! Heh heh."+MESSAGE_END(),

                UNSKIPPABLE()+"Oh! Tu as apporté un fantôme!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Hmmmm!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Magnifique!"+NEWLINE()+"C'est une "+COLOR(QM_RED)+"Âme"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"Je t'en donne "+COLOR(QM_RED)+"50 Rubis"+COLOR(QM_WHITE)+"."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Et en plus, j'inscris "+COLOR(QM_RED)+"100 points "+COLOR(QM_WHITE)+NEWLINE()+"sur ta carte."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"Obtiens "+COLOR(QM_RED)+std::to_string(poes * 100)+" points"+COLOR(QM_WHITE)+" et tu ne"+NEWLINE()+"seras pas déçu..."+NEWLINE()+"Hé hé hé."+MESSAGE_END(),

                UNSKIPPABLE()+"¡Vaya! ¡Traes un poe!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¡Mmm! ¿A ver?"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¡Qué interesante! ¡Es un "+COLOR(QM_RED)+"gran poe"+COLOR(QM_WHITE)+"!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"Te daré "+COLOR(QM_RED)+"50 rupias "+COLOR(QM_WHITE)+"por él."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Y además agregaré "+COLOR(QM_RED)+"100 puntos "+COLOR(QM_WHITE)+"a tu"+NEWLINE()+"tarjeta."+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()
                    +"¡Si llegas a "+COLOR(QM_RED)+std::to_string(poes * 100)+" puntos"+COLOR(QM_WHITE)+", serás muy feliz!"+NEWLINE()+"Je, je, je..."+MESSAGE_END());
        }

        //Talon (this is to prevent accidentally skipping Malon in HC)
        CreateMessage(0x9100, 0, 2, 0,
            UNSKIPPABLE()+"You should go talk to my daughter Malon,"+NEWLINE()+"she has an item for you."+NEWLINE()+SET_SPEED(3)+"........."+SET_SPEED(0)+WAIT_FOR_INPUT()+"I have to think about some stuff now,"+NEWLINE()+"please don't distract me."+MESSAGE_END(),
            UNSKIPPABLE()+"Zzzz... Muh... Malon..."+NEWLINE()+"Parler avec... Malon..."+NEWLINE()+SET_SPEED(3)+"........."+SET_SPEED(0)+WAIT_FOR_INPUT()+"Si fatigué..."+NEWLINE()+"Quelle vie..."+MESSAGE_END(),
            UNSKIPPABLE()+"Habla con Malon, tiene algo que darte..."+SET_SPEED(3)+"........."+SET_SPEED(0)+MESSAGE_END());

        //Bow Shooting Gallery reminder
        CreateMessage(0x9140, 0, 0, 0,
            UNSKIPPABLE()+"Wonderful! Bravo! Perfect!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Here's a fantastic present! But I have"+COLOR(QM_RED)+NEWLINE()+"something else "+COLOR(QM_WHITE)+"for you once you have a bow."+SET_SPEED(30)+" "+EVENT_TRIGGER()+MESSAGE_END(),
            UNSKIPPABLE()+"Merveilleux! Bravo! C'est parfait!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"Voici un prix fantastique! J'aurai "+COLOR(QM_RED)+"autre chose"+COLOR(QM_WHITE)+NEWLINE()+"pour toi quand tu auras un arc."+SET_SPEED(30)+" "+EVENT_TRIGGER()+MESSAGE_END(),
            UNSKIPPABLE()+"¡Espectacular! ¡Bravo! ¡Perfecto!"+WAIT_FOR_INPUT()+NEWLINE()+UNSKIPPABLE()+"¡Toma este sensacional regalo! Pero te tengo"+NEWLINE()+"guardado "+COLOR(QM_RED)+"algo más "+COLOR(QM_WHITE)+"para cuando traigas tu"+NEWLINE()+"propio arco."+SET_SPEED(30)+" "+EVENT_TRIGGER()+MESSAGE_END());

        //Shopsanity items
        //64 textboxes, 2 for each of 32 potential shopsanity items
        for(uint32_t shopitems = 0; shopitems < NonShopItems.size(); shopitems++) {
            Text name = NonShopItems[shopitems].Name;
            std::string price = std::to_string(NonShopItems[shopitems].Price);
            //Prevent names from being too long and overflowing textbox
            if (name.GetEnglish() == "Piece of Heart (Treasure Chest Minigame)") {
                name = Text{"Piece of Heart", "Quart de Coeur", "Pieza de corazón"};
            } else if (name.GetEnglish() == "Green Rupee (Treasure Chest Minigame)") {
                name = Text{"Green Rupee", "Rubis Vert", "Rupia verde"};
            }
            //Message to display when hovering over the item
            if (NonShopItems[shopitems].Repurchaseable) { //Different checkbox for repurchaseable items
                CreateMessage(0x9200+shopitems*2, 0, 0, 0,
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetEnglish()+": "+price+" Rupees"+NEWLINE()+COLOR(QM_WHITE)+"Special deal!"+NEWLINE()+"Buy as many as you want!"+SHOP_MESSAGE_BOX()+MESSAGE_END(),
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetFrench()+": "+price+" rubis"+NEWLINE()+COLOR(QM_WHITE)+"Offre spéciale!"+NEWLINE()+"Achetez-en à volonté!"+SHOP_MESSAGE_BOX()+MESSAGE_END(),
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetSpanish()+": "+price+" rupias"+NEWLINE()+COLOR(QM_WHITE)+"¡Oferta especial!"+NEWLINE()+"¡Compra todo lo que quieras!"+SHOP_MESSAGE_BOX()+MESSAGE_END());
            }
            else { //Normal textbox
                CreateMessage(0x9200+shopitems*2, 0, 0, 0,
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetEnglish()+": "+price+" Rupees"+NEWLINE()+COLOR(QM_WHITE)+"Special deal! ONE LEFT!"+NEWLINE()+"Get it while it lasts!"+SHOP_MESSAGE_BOX()+MESSAGE_END(),
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetFrench()+": "+price+" rubis"+NEWLINE()+COLOR(QM_WHITE)+"Offre spéciale! DERNIER EN STOCK!"+NEWLINE()+"Faites vite!"+SHOP_MESSAGE_BOX()+MESSAGE_END(),
                    INSTANT_TEXT_ON()+COLOR(QM_RED)+name.GetSpanish()+": "+price+" rupias"+NEWLINE()+COLOR(QM_WHITE)+"¡Oferta especial! ¡SOLO QUEDA UNA UNIDAD!"+NEWLINE()+"¡Hazte con ella antes de que se agote!"+SHOP_MESSAGE_BOX()+MESSAGE_END());
            }
            //Message to display when going to buy the item
            CreateMessage(0x9200+shopitems*2+1, 0, 0, 0,
                INSTANT_TEXT_ON()+name.GetEnglish()+": "+price+" Rupees"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Buy"+NEWLINE()+"Don't buy"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+name.GetFrench()+": "+price+" rubis"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Acheter"+NEWLINE()+"Ne pas acheter"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END(),
                INSTANT_TEXT_ON()+name.GetSpanish()+": "+price+" rupias"+INSTANT_TEXT_OFF()+NEWLINE()+NEWLINE()+TWO_WAY_CHOICE()+COLOR(QM_GREEN)+"Comprar"+NEWLINE()+"No comprar"+COLOR(QM_WHITE)+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
        //easter egg
        CreateMessage(0x96F, 0, 2, 2,
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Oh hey, you watched all the credits!"+NEWLINE()+CENTER_TEXT()+"Here's a prize for your patience."+NEWLINE()+CENTER_TEXT()+"Unlocking MQ and saving..."+NEWLINE()+NEWLINE()+CENTER_TEXT()+COLOR(QM_RED)+"Do not remove the Game Card"+NEWLINE()+CENTER_TEXT()+"or turn the power off."+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"The Legend of Zelda Ocarina of Time 3D"+NEWLINE()+CENTER_TEXT()+"Master Quest va être déverrouillé."+NEWLINE()+CENTER_TEXT()+"Sauvegarde... Veuillez patienter."+NEWLINE()+NEWLINE()+CENTER_TEXT()+COLOR(QM_RED)+"N'éteignez pas la console et"+NEWLINE()+CENTER_TEXT()+"ne retirez pas la carte de jeu"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Desbloqueando The Legend of Zelda"+NEWLINE()+CENTER_TEXT()+"Ocarina of Time 3D Master Quest."+NEWLINE()+CENTER_TEXT()+"Guardando. Espera un momento..."+NEWLINE()+NEWLINE()+CENTER_TEXT()+COLOR(QM_RED)+"No saques la tarjeta de juego"+NEWLINE()+CENTER_TEXT()+"ni apagues la consola."+INSTANT_TEXT_OFF()+MESSAGE_END());
        CreateMessage(0x970, 0, 2, 3,
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Master Quest doesn't affect the Randomizer,"+NEWLINE()+CENTER_TEXT()+"so you can use 3 more save slots now."+NEWLINE()+NEWLINE()+CENTER_TEXT()+"Thanks for playing!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Vous pouvez désormais jouer à"+NEWLINE()+CENTER_TEXT()+"The Legend of Zelda Ocarina of Time 3D"+NEWLINE()+CENTER_TEXT()+"Master Quest!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"¡Ya puedes jugar The Legend of Zelda"+NEWLINE()+CENTER_TEXT()+"Ocarina of Time 3D Master Quest!"+INSTANT_TEXT_OFF()+MESSAGE_END());

        //Messages for the new Lake Hylia switch
        CreateMessage(0x346, 0, 1, 3,
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Water level control system."+NEWLINE()+CENTER_TEXT()+"Keep away!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Contrôle du niveau d'eau."+NEWLINE()+CENTER_TEXT()+"Ne pas toucher!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
            UNSKIPPABLE()+INSTANT_TEXT_ON()+CENTER_TEXT()+"Control del nivel del agua."+NEWLINE()+CENTER_TEXT()+"¡No te acerques!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        CreateMessage(0x1B3, 0, 0, 3,
            UNSKIPPABLE()+"This switch is rustier than you think."+WAIT_FOR_INPUT()+"Something must be wrong with the pipe"+NEWLINE()+"system in the Water Temple."+MESSAGE_END(),
            UNSKIPPABLE()+"Cet interrupteur est très rouillé."+WAIT_FOR_INPUT()+"Quelque chose ne va pas avec"+NEWLINE()+"la tuyauterie du Temple de l'Eau."+MESSAGE_END(),
            UNSKIPPABLE()+"El interruptor está más oxidado de lo que"+NEWLINE()+"aparenta."+WAIT_FOR_INPUT()+"Algo debe andar mal en el sistema de"+NEWLINE()+"cañerías del Templo del Agua."+MESSAGE_END());

        //Treasure chest shop keys. If they're not randomized leave the base game text
        if (Settings::ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_SINGLE_KEYS)) {
            CreateMessage(0x0F3, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got a "+COLOR(QM_RED)+"Treasure Chest Shop"+NEWLINE()+"Small Key"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez une "+COLOR(QM_RED)+"Petite Clé"+NEWLINE()+"de la Chasse-aux-Trésors"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido una "+COLOR(QM_RED)+"llave pequeña del"+NEWLINE()+"Cofre del Tesoro"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        } else if (Settings::ShuffleChestMinigame.Is(SHUFFLECHESTMINIGAME_PACK)) {
            CreateMessage(0x0F3, 0, 2, 3,
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"You got all 6 "+COLOR(QM_RED)+"Treasure Chest Shop"+NEWLINE()+"Small Keys"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"Vous trouvez les "+COLOR(QM_RED)+"petites clés"+NEWLINE()+"de la Chasse-aux-Trésors"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END(),
                UNSKIPPABLE()+ITEM_OBTAINED(ITEM_KEY_SMALL)+INSTANT_TEXT_ON()+"¡Has obtenido todas las 6 "+COLOR(QM_RED)+"llaves"+NEWLINE()+"pequeñas del Cofre del Tesoro"+COLOR(QM_WHITE)+"!"+INSTANT_TEXT_OFF()+MESSAGE_END());
        }
    }

    Text AddColorsAndFormat(Text text, const std::vector<uint8_t>& colors /*= {}*/) {

      //for each language
      for (std::string* textStr : {&text.english, &text.french, &text.spanish}) {

        //insert playername
        size_t atSymbol = textStr->find('@');
        while (atSymbol != std::string::npos) {
          textStr->replace(atSymbol, 1, PLAYER_NAME());
          atSymbol = textStr->find('@');
        }
        //insert newlines either manually or when encountering a '&'
        constexpr size_t lineLength = 44;
        size_t lastNewline = 0;
        while (lastNewline + lineLength < textStr->length()) {
          size_t carrot     = textStr->find('^', lastNewline);
          size_t ampersand  = textStr->find('&', lastNewline);
          size_t lastSpace  = textStr->rfind(' ', lastNewline + lineLength);
          size_t lastPeriod = textStr->rfind('.', lastNewline + lineLength);
          //replace '&' first if it's within the newline range
          if (ampersand < lastNewline + lineLength) {
            textStr->replace(ampersand, 1, NEWLINE());
            lastNewline = ampersand + NEWLINE().length();
          //or move the lastNewline cursor to the next line if a '^' is encountered
          } else if (carrot < lastNewline + lineLength) {
            lastNewline = carrot + 1;
          //some lines need to be split but don't have spaces, look for periods instead
          } else if (lastSpace == std::string::npos) {
            textStr->replace(lastPeriod, 1, "."+NEWLINE());
            lastNewline = lastPeriod + NEWLINE().length() + 1;
          } else {
            textStr->replace(lastSpace, 1, NEWLINE());
            lastNewline = lastSpace + NEWLINE().length();
          }
        }
        //clean up any remaining '&' characters
        size_t ampersand = textStr->find('&');
        while (ampersand != std::string::npos) {
          textStr->replace(ampersand, 1, NEWLINE());
          ampersand = textStr->find('&');
        }

        //insert box break
        size_t carrotSymbol = textStr->find('^');
        while (carrotSymbol != std::string::npos) {
          textStr->replace(carrotSymbol, 1, INSTANT_TEXT_OFF()+WAIT_FOR_INPUT()+INSTANT_TEXT_ON());
          carrotSymbol = textStr->find('^');
        }
        //add colors
        for (auto color : colors) {
          size_t firstHashtag = textStr->find('#');
          if (firstHashtag != std::string::npos) {
              textStr->replace(firstHashtag, 1, COLOR(color));
              size_t secondHashtag = textStr->find('#');
              if (secondHashtag == std::string::npos) {
                CitraPrint("ERROR: Couldn't find second '#' in " + (*textStr));
              } else {
                textStr->replace(secondHashtag, 1, COLOR(QM_WHITE));
              }
          }
        }
      }
      return Text{"","",""}+UNSKIPPABLE()+INSTANT_TEXT_ON()+text+INSTANT_TEXT_OFF()+MESSAGE_END();
    }

    void ClearMessages() {
        messageEntries.clear();
        arrangedMessageEntries.clear();
        messageData.str("");
        arrangedMessageData = "";
    }

    std::string MESSAGE_END()          { return  "\x7F\x00"s; }
    std::string WAIT_FOR_INPUT()       { return  "\x7F\x01"s; }
    std::string HORIZONTAL_SPACE(uint8_t x) {
        return "\x7F\x02"s + char(x);
    }
    std::string GO_TO(uint16_t x) {
        return "\x7F\x03"s + char(x >> 8) + char(x & 0x00FF);
    }
    std::string INSTANT_TEXT_ON()      { return  "\x7F\x04"s; }
    std::string INSTANT_TEXT_OFF()     { return  "\x7F\x05"s; }
    std::string SHOP_MESSAGE_BOX()     { return  "\x7F\x06\x00"s; }
    std::string EVENT_TRIGGER()        { return  "\x7F\x07"s; }
    std::string DELAY_FRAMES(uint8_t x) {
        return "\x7F\x08"s + char(x);
    }
    std::string CLOSE_AFTER(uint8_t x) {
        return "\x7F\x0A"s + char(x);
    }
    std::string PLAYER_NAME()          { return  "\x7F\x0B"s; }
    std::string PLAY_OCARINA()         { return  "\x7F\x0C"s; }
    std::string ITEM_OBTAINED(uint8_t x) {
        return "\x7F\x0F"s + char(x);
    }
    std::string SET_SPEED(uint8_t x) {
        return "\x7F\x10"s + char(x);
    }
    std::string SKULLTULAS_DESTROYED() { return  "\x7F\x15"s; }
    std::string CURRENT_TIME()         { return  "\x7F\x17"s; }
    std::string UNSKIPPABLE()          { return  "\x7F\x19"s; }
    std::string TWO_WAY_CHOICE()       { return  "\x7F\x1A\xFF\xFF\xFF\xFF"s; }
    std::string NEWLINE()              { return  "\x7F\x1C"s; }
    std::string COLOR(uint8_t x)            { return  "\x7F\x1D"s + char(x); }
    std::string CENTER_TEXT()          { return  "\x7F\x1E"s; }
    std::string IF_NOT_MQ()            { return  "\x7F\x29"s; }
    std::string MQ_ELSE()              { return  "\x7F\x2A"s; }
    std::string MQ_END()               { return  "\x7F\x2B"s; }
}
