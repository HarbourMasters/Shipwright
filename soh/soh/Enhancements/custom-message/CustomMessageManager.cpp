#include "CustomMessageManager.h"
#include <algorithm>
#include <stdint.h>

using namespace std::literals::string_literals;

CustomMessageManager::CustomMessageManager() {
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

CustomMessageManager::~CustomMessageManager() {
    this->textBoxSpecialCharacters.clear();
    this->colors.clear();
    this->messageTables.clear();
}

void CustomMessageManager::ReplaceSpecialCharacters(std::string& string) {
    // add special characters
    for (auto specialCharacterPair : this->textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = ""s;
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = string.find(specialCharacterPair.first, 0)) != std::string::npos) {
            string.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
        }
    }
}

void CustomMessageManager::ReplaceColors(std::string& string) {
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

void ReplaceString(std::string& source, std::string textToReplace, std::string value) {
    size_t pos = source.find(textToReplace);
    if (pos != std::string::npos) {
        source.replace(pos, textToReplace.length(), value);
    }
    CustomMessageManager::Instance->FormatCustomMessage(source);
}

void CustomMessageManager::ReplaceStringInMessage(CustomMessageEntry& messageEntry, std::string textToReplace, std::string value) {
    ReplaceString(messageEntry.english, textToReplace, value);
    ReplaceString(messageEntry.german, textToReplace, value);
    ReplaceString(messageEntry.french, textToReplace, value);
}

void CustomMessageManager::ReplaceStringInMessage(CustomMessageEntry& messageEntry, std::string textToReplace, std::string englishValue, std::string germanValue, std::string frenchValue) {
    ReplaceString(messageEntry.english, textToReplace, englishValue);
    ReplaceString(messageEntry.german, textToReplace, germanValue);
    ReplaceString(messageEntry.french, textToReplace, frenchValue);
}

void CustomMessageManager::FormatCustomMessage(std::string& message, ItemID iid) {
    message.insert(0, ITEM_OBTAINED(iid));
    size_t start_pos = 0;
    std::replace(message.begin(), message.end(), '&', NEWLINE()[0]);
    while ((start_pos = message.find('^', start_pos)) != std::string::npos) {
        message.replace(start_pos, 1, WAIT_FOR_INPUT() + ITEM_OBTAINED(iid));
        start_pos += 3;
    }
    std::replace(message.begin(), message.end(), '@', PLAYER_NAME()[0]);
    ReplaceSpecialCharacters(message);
    ReplaceColors(message);
    message += MESSAGE_END();
}

void CustomMessageManager::FormatCustomMessage(std::string& message) {
    size_t start_pos = 0;
    std::replace(message.begin(), message.end(), '&', NEWLINE()[0]);
    std::replace(message.begin(), message.end(), '^', WAIT_FOR_INPUT()[0]);
    std::replace(message.begin(), message.end(), '@', PLAYER_NAME()[0]);
    ReplaceSpecialCharacters(message);
    ReplaceColors(message);
    message += MESSAGE_END();
}

bool CustomMessageManager::InsertCustomMessage(std::string tableID, uint16_t textID, CustomMessageEntry messages) {
    auto foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return false;
    }
    auto& messageTable = foundMessageTable->second;
    auto messageInsertResult = messageTable.emplace(textID, messages);
    return messageInsertResult.second;
}

bool CustomMessageManager::CreateGetItemMessage(std::string tableID, uint16_t giid, ItemID iid,
                                                CustomMessageEntry messageEntry) {
    FormatCustomMessage(messageEntry.english, iid);
    FormatCustomMessage(messageEntry.german, iid);
    FormatCustomMessage(messageEntry.french, iid);
    const uint16_t textID = giid;
    return InsertCustomMessage(tableID, textID, messageEntry);
}

bool CustomMessageManager::CreateMessage(std::string tableID, uint16_t textID, CustomMessageEntry messageEntry) {
    FormatCustomMessage(messageEntry.english);
    FormatCustomMessage(messageEntry.german);
    FormatCustomMessage(messageEntry.french);
    return InsertCustomMessage(tableID, textID, messageEntry);
}

CustomMessageEntry CustomMessageManager::RetrieveMessage(std::string tableID, uint16_t textID) {
    std::unordered_map<std::string, CustomMessageTable>::const_iterator foundMessageTable = messageTables.find(tableID);
    if (foundMessageTable == messageTables.end()) {
        return NULL_CUSTOM_MESSAGE;
    }
    CustomMessageTable messageTable = foundMessageTable->second;
    std::unordered_map<uint16_t, CustomMessageEntry>::const_iterator foundMessage = messageTable.find(textID);
    if (foundMessage == messageTable.end()) {
        return NULL_CUSTOM_MESSAGE;
    }
    CustomMessageEntry message = foundMessage->second;
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

std::string CustomMessageManager::MESSAGE_END() {
    return "\x02"s;
}

std::string CustomMessageManager::ITEM_OBTAINED(uint8_t x) {
    return "\x13"s + char(x);
}

std::string CustomMessageManager::NEWLINE() {
    return "\x01"s;
}

std::string CustomMessageManager::COLOR(uint8_t x) {
    return "\x05"s + char(x);
}

std::string CustomMessageManager::WAIT_FOR_INPUT() {
    return "\x04"s;
}

std::string CustomMessageManager::PLAYER_NAME() {
    return "\x0F"s;
}
