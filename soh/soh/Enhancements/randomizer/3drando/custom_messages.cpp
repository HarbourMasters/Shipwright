#include "custom_messages.hpp"
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
    std::stringstream messageData;

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

        //If there's a two-way choice and only 1 newline before it in the same text box, add another one
        size_t choice = textStr->find(TWO_WAY_CHOICE());
        if (choice != std::string::npos) {
          size_t newLinesCount = 0;
          size_t lastBoxBreak = textStr->rfind(WAIT_FOR_INPUT(), choice);
          lastNewline = choice;

          if (lastBoxBreak == std::string::npos) {
            lastBoxBreak = 0;
          }

          while ((lastNewline != std::string::npos)) {
            lastNewline = textStr->rfind(NEWLINE(), lastNewline - 1);
            if (lastNewline != std::string::npos && lastNewline > lastBoxBreak) {
                newLinesCount++;
            } else {
                break;
            }
          }

          if (newLinesCount <= 1) {
              textStr->replace(choice, TWO_WAY_CHOICE().length(), NEWLINE()+TWO_WAY_CHOICE());
          }
        }

        //add colors
        for (auto color : colors) {
          size_t firstHashtag = textStr->find('#');
          if (firstHashtag != std::string::npos) {
              textStr->replace(firstHashtag, 1, COLOR(color));
              size_t secondHashtag = textStr->find('#');
              if (secondHashtag == std::string::npos) {
                //CitraPrint("ERROR: Couldn't find second '#' in " + (*textStr));
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
        messageData.str("");
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
    std::string COLOR(uint8_t x)       { return  "\x7F\x1D"s + char(x); }
    std::string CENTER_TEXT()          { return  "\x7F\x1E"s; }
    std::string IF_NOT_MQ()            { return  "\x7F\x29"s; }
    std::string MQ_ELSE()              { return  "\x7F\x2A"s; }
    std::string MQ_END()               { return  "\x7F\x2B"s; }
}
