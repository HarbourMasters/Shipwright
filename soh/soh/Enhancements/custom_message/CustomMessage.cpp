#include "CustomMessage.h"
#include <algorithm>

using namespace std::literals::string_literals;

CustomMessage::CustomMessage() {
    this->textBoxSpecialCharacters = { { "À", 0x80 }, { "î", 0x81 }, { "Â", 0x82 }, { "Ä", 0x83 }, { "Ç", 0x84 },
                                       { "È", 0x85 }, { "É", 0x86 }, { "Ê", 0x87 }, { "Ë", 0x88 }, { "Ï", 0x89 },
                                       { "Ô", 0x8A }, { "Ö", 0x8B }, { "Ù", 0x8C }, { "Û", 0x8D }, { "Ü", 0x8E },
                                       { "ß", 0x8F }, { "à", 0x90 }, { "á", 0x91 }, { "â", 0x92 }, { "ä", 0x93 },
                                       { "ç", 0x94 }, { "è", 0x95 }, { "é", 0x96 }, { "ê", 0x97 }, { "ë", 0x98 },
                                       { "ï", 0x99 }, { "ô", 0x9A }, { "ö", 0x9B }, { "ù", 0x9C }, { "û", 0x9D },
                                       { "ü", 0x9E } };
    this->colors = { { "w", QM_WHITE }, { "r", QM_RED },  { "g", QM_GREEN },  { "b", QM_BLUE },
                     { "c", QM_LBLUE }, { "p", QM_PINK }, { "y", QM_YELLOW }, { "B", QM_BLACK } };
}

CustomMessage::~CustomMessage() {
    this->textBoxSpecialCharacters.clear();
}

void CustomMessage::ReplaceSpecialCharacters(std::string& string) {
    // add special characters
    for (auto specialCharacterPair : textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = "";
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = string.find(specialCharacterPair.first, start_pos)) != std::string::npos) {
            string.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
        }
    }
}

void CustomMessage::ReplaceColors(std::string& string) {
    for (auto colorPair : colors) {
        std::string textToReplace = "%";
        textToReplace += colorPair.first;
        size_t start_pos = 0;
        while ((start_pos = string.find(textToReplace)) != std::string::npos) {
            string.replace(start_pos, textToReplace.length(), COLOR(colorPair.second));
            start_pos += textToReplace.length();
        }
    }
}

void CustomMessage::CreateGetItemMessage(GetItemID giid, ItemID iid, std::string messages[LANGUAGE_MAX]) {
    for (int i = 0; i < LANGUAGE_MAX; i++) {
        if (!(messages[i].empty())) {
            std::string message = messages[i];
            std::string formattedMessage = ITEM_OBTAINED(iid) + message;
            size_t start_pos = 0;
            std::replace(formattedMessage.begin(), formattedMessage.end(), '&', NEWLINE()[0]);
            while ((start_pos = formattedMessage.find('^', start_pos)) != std::string::npos) {
                formattedMessage.replace(start_pos, 1, WAIT_FOR_INPUT() + ITEM_OBTAINED(iid));
                start_pos += 3;
            }
            std::replace(formattedMessage.begin(), formattedMessage.end(), '@', PLAYER_NAME()[0]);
            ReplaceSpecialCharacters(formattedMessage);
            ReplaceColors(formattedMessage);
            formattedMessage += MESSAGE_END();
            this->getItemMessageTable[i].emplace(giid, formattedMessage);
        } else {
            this->getItemMessageTable[i].emplace(giid, MESSAGE_END());
        }
    }
}

std::string CustomMessage::RetrieveGetItemMessage(GetItemID giid) {
    std::unordered_map<GetItemID, std::string>::const_iterator result =
        getItemMessageTable[gSaveContext.language].find(giid);
    if (result == getItemMessageTable[gSaveContext.language].end()) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                return "Il n'y a pas de message personnalisé pour cet élément.";
            case LANGUAGE_GER:
                return "Für diesen Artikel gibt es keine benutzerdefinierte Nachricht.";
            case LANGUAGE_ENG:
            default:
                return "There is no custom message for this item.";
        }
    }
    return result->second;
}

std::string CustomMessage::MESSAGE_END() {
    return "\x02"s;
}

std::string CustomMessage::ITEM_OBTAINED(uint8_t x) {
    return "\x13"s + char(x);
}

std::string CustomMessage::NEWLINE() {
    return "\x01"s;
}

std::string CustomMessage::COLOR(uint8_t x) {
    return "\x05"s + char(x);
}

std::string CustomMessage::WAIT_FOR_INPUT() {
    return "\x04"s;
}

std::string CustomMessage::PLAYER_NAME() {
    return "\x0F"s;
}