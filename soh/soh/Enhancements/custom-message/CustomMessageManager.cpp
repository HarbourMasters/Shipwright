#include "CustomMessageManager.h"
#include "CustomMessageInterfaceAddon.h"
#include <algorithm>
#include <stdint.h>
#include <cstring>
#include <string>

using namespace std::literals::string_literals;

static const std::unordered_map<std::string, char> textBoxSpecialCharacters = {
    { "À", 0x80 }, { "î", 0x81 }, { "Â", 0x82 }, { "Ä", 0x83 }, { "Ç", 0x84 }, { "È", 0x85 }, { "É", 0x86 },
    { "Ê", 0x87 }, { "Ë", 0x88 }, { "Ï", 0x89 }, { "Ô", 0x8A }, { "Ö", 0x8B }, { "Ù", 0x8C }, { "Û", 0x8D },
    { "Ü", 0x8E }, { "ß", 0x8F }, { "à", 0x90 }, { "á", 0x91 }, { "â", 0x92 }, { "ä", 0x93 }, { "ç", 0x94 },
    { "è", 0x95 }, { "é", 0x96 }, { "ê", 0x97 }, { "ë", 0x98 }, { "ï", 0x99 }, { "ô", 0x9A }, { "ö", 0x9B },
    { "ù", 0x9C }, { "û", 0x9D }, { "ü", 0x9E }
};
static const std::unordered_map<std::string, char> colors = { { "w", QM_WHITE },  { "r", QM_RED },   { "g", QM_GREEN },
                                                              { "b", QM_BLUE },   { "c", QM_LBLUE }, { "p", QM_PINK },
                                                              { "y", QM_YELLOW }, { "B", QM_BLACK } };
static const std::unordered_map<std::string, ItemID> altarIcons = {
    { "0", ITEM_KOKIRI_EMERALD },
    { "1", ITEM_GORON_RUBY },
    { "2", ITEM_ZORA_SAPPHIRE },
    { "8", ITEM_MEDALLION_LIGHT },
    { "3", ITEM_MEDALLION_FOREST },
    { "4", ITEM_MEDALLION_FIRE },
    { "5", ITEM_MEDALLION_WATER },
    { "6", ITEM_MEDALLION_SPIRIT },
    { "7", ITEM_MEDALLION_SHADOW },
    { "l", ITEM_ARROW_LIGHT },
    { "b", ITEM_KEY_BOSS },
    { "o", ITEM_SWORD_MASTER },
    { "c", ITEM_OCARINA_FAIRY },
    { "i", ITEM_OCARINA_TIME },
    { "L", ITEM_BOW_ARROW_LIGHT },
    { "k", ITEM_TUNIC_KOKIRI }
};

CustomMessage::CustomMessage(std::string english_, std::string german_, std::string french_, TextBoxType type_,
                             TextBoxPosition position_)
    : english(std::move(english_)), german(std::move(german_)), french(std::move(french_)), type(type_),
      position(position_) {
}

CustomMessage::CustomMessage(Text text, TextBoxType type_,TextBoxPosition position_)
    : english(text.GetEnglish()), german(text.GetGerman()), french(text.GetFrench()), type(type_),
      position(position_) {
}

const std::string& CustomMessage::GetEnglish() const {
    return english;
}

const std::string& CustomMessage::GetFrench() const {
    if (french.length() > 0) {
        return french;
    }
    return english;
}
const std::string& CustomMessage::GetGerman() const {
    if (german.length() > 0) {
        return german;
    }
    return english;
}

const TextBoxType& CustomMessage::GetTextBoxType() const {
    return type;
}

const TextBoxPosition& CustomMessage::GetTextBoxPosition() const {
    return position;
}

CustomMessage CustomMessage::operator+(const CustomMessage& right) const {
    return CustomMessage(english + right.GetEnglish(), german + right.GetGerman(), french + right.GetFrench());
}

CustomMessage CustomMessage::operator+(const std::string& right) const {
    return CustomMessage(english + right, german + right, french + right);
}

void CustomMessage::operator+=(const std::string& right) {
    english += right;
    french += right;
    german += right;
}

bool CustomMessage::operator==(const CustomMessage& operand) const {
    return english == operand.english;
}

bool CustomMessage::operator!=(const CustomMessage& operand) const {
    return !operator==(operand);
}

void CustomMessage::Replace(std::string&& oldStr, std::string&& newStr) {
    for (std::string* str : { &english, &french, &german }) {
        size_t position = str->find(oldStr);
        while (position != std::string::npos) {
            str->replace(position, oldStr.length(), newStr);
            position = str->find(oldStr);
        }
    }
    Format();
}

void CustomMessage::Replace(std::string&& oldStr, std::string&& newEnglish, std::string&& newGerman,
                            std::string&& newFrench) {
    size_t position = 0;
    position = english.find(oldStr);
    while (position != std::string::npos) {
        english.replace(position, oldStr.length(), newEnglish);
        position = english.find(oldStr);
    }
    position = french.find(oldStr);
    while (position != std::string::npos) {
        french.replace(position, oldStr.length(), newFrench);
        position = french.find(oldStr);
    }
    position = german.find(oldStr);
    while (position != std::string::npos) {
        german.replace(position, oldStr.length(), newGerman);
        position = german.find(oldStr);
    }
    Format();
}

void CustomMessage::Format(ItemID iid) {
    for (std::string* str : { &english, &french, &german }) {
        str->insert(0, ITEM_OBTAINED(iid));
        size_t start_pos = 0;
        std::replace(str->begin(), str->end(), '&', NEWLINE()[0]);
        while ((start_pos = str->find('^', start_pos)) != std::string::npos) {
            str->replace(start_pos, 1, WAIT_FOR_INPUT() + ITEM_OBTAINED(iid));
            start_pos += 3;
        }
        std::replace(str->begin(), str->end(), '@', PLAYER_NAME()[0]);
    }
    ReplaceSpecialCharacters();
    ReplaceColors();
    ReplaceAltarIcons();
    *this += MESSAGE_END();
}

void CustomMessage::Format() {
    for (std::string* str : { &english, &french, &german }) {
        std::replace(str->begin(), str->end(), '&', NEWLINE()[0]);
        std::replace(str->begin(), str->end(), '^', WAIT_FOR_INPUT()[0]);
        std::replace(str->begin(), str->end(), '@', PLAYER_NAME()[0]);
    }
    ReplaceSpecialCharacters();
    ReplaceColors();
    ReplaceAltarIcons();
    *this += MESSAGE_END();
}

void CustomMessage::Capitalize() {
    for (std::string* str : { &english, &french, &german }) {
        (*str)[0] = std::toupper((*str)[0]);
    }
}

void CustomMessage::ReplaceSpecialCharacters() {
    // add special characters
    for (std::string* str : { &english, &french, &german }) {
        for (auto specialCharacterPair : textBoxSpecialCharacters) {
            size_t start_pos = 0;
            std::string textBoxSpecialCharacterString = ""s;
            textBoxSpecialCharacterString += specialCharacterPair.second;
            while ((start_pos = str->find(specialCharacterPair.first, start_pos)) != std::string::npos) {
                str->replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
                start_pos += textBoxSpecialCharacterString.length();
            }
        }
    }
}

const char* Interface_ReplaceSpecialCharacters(char text[]) {
    std::string textString(text);

    for (auto specialCharacterPair : textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = ""s;
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = textString.find(specialCharacterPair.first, start_pos)) != std::string::npos) {
            textString.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
        }
    }

    char* textChar = new char[textString.length() + 1];
    strcpy(textChar, textString.c_str());
    return textChar;
}

void CustomMessage::ReplaceColors() {
    for (std::string* str : { &english, &french, &german }) {
        for (const auto& colorPair : colors) {
            std::string textToReplace = "%";
            textToReplace += colorPair.first;
            size_t start_pos = 0;
            while ((start_pos = str->find(textToReplace, start_pos)) != std::string::npos) {
                str->replace(start_pos, textToReplace.length(), COLOR(colorPair.second));
                start_pos += textToReplace.length();
            }
        }
    }
}

void CustomMessage::ReplaceAltarIcons() {
    for (std::string* str : { &english, &french, &german }) {
        for (const auto& iconPair : altarIcons) {
            std::string textToReplace = "$";
            textToReplace += iconPair.first;
            size_t start_pos = 0;
            while ((start_pos = str->find(textToReplace, start_pos)) != std::string::npos) {
                str->replace(start_pos, textToReplace.length(), ITEM_OBTAINED(iconPair.second));
                start_pos += textToReplace.length();
            }
        }
    }
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

bool CustomMessageManager::InsertCustomMessage(std::string tableID, uint16_t textID, CustomMessage messages) {
    auto foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return false;
    }
    auto& messageTable = foundMessageTable->second;
    auto messageInsertResult = messageTable.emplace(textID, messages);
    return messageInsertResult.second;
}

bool CustomMessageManager::CreateGetItemMessage(std::string tableID, uint16_t giid, ItemID iid,
                                                CustomMessage messageEntry) {
    messageEntry.Format(iid);
    const uint16_t textID = giid;
    return InsertCustomMessage(tableID, textID, messageEntry);
}

bool CustomMessageManager::CreateMessage(std::string tableID, uint16_t textID, CustomMessage messageEntry) {
    messageEntry.Format();
    return InsertCustomMessage(tableID, textID, messageEntry);
}

CustomMessage CustomMessageManager::RetrieveMessage(std::string tableID, uint16_t textID) {
    std::unordered_map<std::string, CustomMessageTable>::const_iterator foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        throw(MessageNotFoundException(tableID, textID));
    }
    CustomMessageTable messageTable = foundMessageTable->second;
    std::unordered_map<uint16_t, CustomMessage>::const_iterator foundMessage = messageTable.find(textID);
    if (foundMessage == messageTable.end()) {
        throw(MessageNotFoundException(tableID, textID));
    }
    CustomMessage message = foundMessage->second;
    return message;
}

bool CustomMessageManager::ClearMessageTable(std::string tableID) {
    auto foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return false;
    }
    auto& messageTable = foundMessageTable->second;
    messageTable.clear();
    return true;
}

bool CustomMessageManager::AddCustomMessageTable(std::string tableID) { 
    CustomMessageTable newMessageTable;
    return messageTables.emplace(tableID, newMessageTable).second;
}
