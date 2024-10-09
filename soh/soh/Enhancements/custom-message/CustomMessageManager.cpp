#include "CustomMessageManager.h"
#include "CustomMessageInterfaceAddon.h"
#include <algorithm>
#include <stdint.h>
#include <cstring>
#include <map>
#include <spdlog/spdlog.h>
#include <variables.h>

using namespace std::literals::string_literals;

static const std::unordered_map<std::string, char> textBoxSpecialCharacters = {
    { "À", 0x80 }, { "î", 0x81 }, { "Â", 0x82 }, { "Ä", 0x83 }, { "Ç", 0x84 }, { "È", 0x85 }, { "É", 0x86 },
    { "Ê", 0x87 }, { "Ë", 0x88 }, { "Ï", 0x89 }, { "Ô", 0x8A }, { "Ö", 0x8B }, { "Ù", 0x8C }, { "Û", 0x8D },
    { "Ü", 0x8E }, { "ß", 0x8F }, { "à", 0x90 }, { "á", 0x91 }, { "â", 0x92 }, { "ä", 0x93 }, { "ç", 0x94 },
    { "è", 0x95 }, { "é", 0x96 }, { "ê", 0x97 }, { "ë", 0x98 }, { "ï", 0x99 }, { "ô", 0x9A }, { "ö", 0x9B },
    { "ù", 0x9C }, { "û", 0x9D }, { "ü", 0x9E }
};
static const std::unordered_map<std::string, std::string> percentColors = { { "w", QM_WHITE },  { "r", QM_RED },   { "g", QM_GREEN },
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
    { "k", ITEM_TUNIC_KOKIRI },
    { "m", ITEM_DUNGEON_MAP },
    { "C", ITEM_COMPASS },
    { "s", ITEM_SKULL_TOKEN },
    { "g", ITEM_MASK_GORON }
};

static std::map<std::string, int> pixelWidthTable = {
    { " ", 6 },  { "!", 6 },  { "\"", 5 },     { "#", 7 },  { "$", 7 },  { "%", 11 }, { "&", 9 },  { "\'", 3 },
    { "(", 6 },  { ")", 6 },  { "*", 6 },      { "+", 7 },  { ",", 3 },  { "-", 5 },  { ".", 3 },  { "/", 7 },
    { "0", 8 },  { "1", 4 },  { "2", 7 },      { "3", 7 },  { "4", 8 },  { "5", 7 },  { "6", 7 },  { "7", 7 },
    { "8", 7 },  { "9", 7 },  { ":", 5 },      { ";", 5 },  { "<", 7 },  { "=", 9 },  { ">", 7 },  { "?", 9 },
    { "@", 10 }, { "A", 9 },  { "B", 7 },      { "C", 9 },  { "D", 9 },  { "E", 6 },  { "F", 6 },  { "G", 9 },
    { "H", 8 },  { "I", 3 },  { "J", 6 },      { "K", 8 },  { "L", 6 },  { "M", 10 }, { "N", 9 },  { "O", 10 },
    { "P", 7 },  { "Q", 10 }, { "R", 8 },      { "S", 8 },  { "T", 7 },  { "U", 8 },  { "V", 9 },  { "W", 12 },
    { "X", 9 },  { "Y", 8 },  { "Z", 8 },      { "[", 6 },  { "\\", 8 }, { "]", 6 },  { "^", 8 },  { "_", 7 },
    { "`", 4 },  { "a", 6 },  { "b", 7 },      { "c", 6 },  { "d", 7 },  { "e", 7 },  { "f", 5 },  { "g", 7 },
    { "h", 6 },  { "i", 3 },  { "j", 5 },      { "k", 6 },  { "l", 3 },  { "m", 9 },  { "n", 7 },  { "o", 7 },
    { "p", 7 },  { "q", 7 },  { "r", 6 },      { "s", 6 },  { "t", 6 },  { "u", 6 },  { "v", 7 },  { "w", 9 },
    { "x", 6 },  { "y", 7 },  { "z", 6 },      { "{", 6 },  { "¦", 4 },  { "}", 6 },  { "¡", 5 },  { "¢", 7 },
    { "£", 8 },  { "¤", 7 },  { "¥", 8 },      { "|", 4 },  { "§", 12 }, { "¨", 12 }, { "©", 10 }, { "ª", 5 },
    { "«", 8 },  { "¬", 7 },  { "\u00AD", 6 }, { "®", 10 }, { "¯", 8 },  { "°", 12 }, { "±", 12 }, { "²", 5 },
    { "³", 5 },  { "µ", 6 },  { "¶", 8 },      { "·", 4 },  { "¹", 4 },  { "º", 5 },  { "»", 9 },  { "¼", 9 },
    { "½", 9 },  { "¾", 10 }, { "¿", 7 },      { "À", 11 }, { "Á", 9 },  { "Â", 9 },  { "Ã", 9 },  { "Ä", 9 },
    { "Å", 9 },  { "Æ", 12 }, { "Ç", 9 },      { "È", 6 },  { "É", 6 },  { "Ê", 6 },  { "Ë", 6 },  { "Ì", 5 },
    { "Í", 5 },  { "Î", 5 },  { "Ï", 5 },      { "Ð", 10 }, { "Ñ", 9 },  { "Ò", 10 }, { "Ó", 10 }, { "Ô", 10 },
    { "Õ", 10 }, { "Ö", 10 }, { "×", 9 },      { "Ø", 10 }, { "Ù", 8 },  { "Ú", 8 },  { "Û", 8 },  { "Ü", 8 },
    { "Ý", 10 }, { "Þ", 8 },  { "ß", 7 },      { "à", 6 },  { "á", 6 },  { "â", 6 },  { "ã", 6 },  { "ä", 6 },
    { "å", 6 },  { "æ", 11 }, { "ç", 6 },      { "è", 7 },  { "é", 7 },  { "ê", 7 },  { "ë", 7 },  { "ì", 5 },
    { "í", 5 },  { "î", 5 },  { "ï", 5 },      { "ð", 7 },  { "ñ", 7 },  { "ò", 7 },  { "ó", 7 },  { "ô", 7 },
    { "õ", 7 },  { "ö", 7 },  { "÷", 11 },     { "ø", 9 },  { "ù", 7 },  { "ú", 7 },  { "û", 7 },  { "ü", 7 },
    { "ý", 8 },  { "þ", 8 },  { "ÿ", 8 },      { "Œ", 11 }, { "œ", 11 }, { "„", 5 },  { "”", 5 },  { "€", 10 },
    { "Ÿ", 10 }, { "~", 8 }
};

CustomMessage::CustomMessage(std::string english_, std::string german_, std::string french_, TextBoxType type_,
                             TextBoxPosition position_)
    : type(type_), position(position_){
    messages[LANGUAGE_ENG] = std::move(english_);
    messages[LANGUAGE_GER] = std::move(german_);
    messages[LANGUAGE_FRA] = std::move(french_);
}

CustomMessage::CustomMessage(std::string english_, std::string german_, std::string french_, std::vector<std::string> colors_,
                              std::vector<bool> capital_, TextBoxType type_, TextBoxPosition position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
    messages[LANGUAGE_GER] = std::move(german_);
    messages[LANGUAGE_FRA] = std::move(french_);
    colors = colors_;
    capital = capital_;
    type = type_;
    position = position_;
}

CustomMessage::CustomMessage(std::string english_, TextBoxType type_, TextBoxPosition position_)
    : type(type_), position(position_) {
    messages[LANGUAGE_ENG] = std::move(english_);
}

CustomMessage::CustomMessage(std::string english_, std::vector<std::string> colors_, std::vector<bool> capital_, TextBoxType type_, TextBoxPosition position_){
    messages[LANGUAGE_ENG] = std::move(english_);
    colors = colors_;
    capital = capital_;
    type = type_;
    position = position_;
}

CustomMessage::CustomMessage(Text text, TextBoxType type_,TextBoxPosition position_)
    : type(type_), position(position_) {
    messages[LANGUAGE_ENG] = text.GetEnglish();
    messages[LANGUAGE_GER] = text.GetGerman();
    messages[LANGUAGE_FRA] = text.GetFrench();
}

const std::string CustomMessage::GetEnglish(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_ENG, format);
}

const std::string CustomMessage::GetGerman(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_GER, format);
}

const std::string CustomMessage::GetFrench(MessageFormat format) const {
    return GetForLanguage(LANGUAGE_FRA, format);
}

const std::string CustomMessage::GetForCurrentLanguage(MessageFormat format) const {
    return GetForLanguage(gSaveContext.language, format);
}

const std::string CustomMessage::GetForLanguage(uint8_t language, MessageFormat format) const {
    std::string output = messages[language].length() > 0 ? messages[language] : messages[LANGUAGE_ENG];
    ProcessMessageFormat(output, format);
    return output;
}

const std::vector<std::string> CustomMessage::GetAllMessages(MessageFormat format) const{
    std::vector<std::string> output = messages;
    for (auto str : output){
        ProcessMessageFormat(str, format);
    }
    return output;
}

void CustomMessage::ProcessMessageFormat(std::string& str, MessageFormat format) const {
    if (format == MF_FORMATTED){
        FormatString(str);
    } else if (format == MF_CLEAN){
        CleanString(str);
    } else if (format == MF_AUTO_FORMAT){
        AutoFormatString(str);
    }
}

const std::vector<bool>& CustomMessage::GetCapital() const {
    return capital;
}

void CustomMessage::SetCapital(std::vector<bool> capital_){
    capital = capital_;
}
const std::vector<std::string>& CustomMessage::GetColors() const {
    return colors;
}

void CustomMessage::SetColors(std::vector<std::string> colors_){
    colors = colors_;
}

const TextBoxType& CustomMessage::GetTextBoxType() const {
    return type;
}

void CustomMessage::SetTextBoxType(TextBoxType boxType){
    type = boxType;
}

const TextBoxPosition& CustomMessage::GetTextBoxPosition() const {
    return position;
}

CustomMessage CustomMessage::operator+(const CustomMessage& right) const {
    std::vector<std::string> newColors = colors;
    std::vector<std::string> rColors = right.GetColors();
    for (auto color: rColors){
        newColors.push_back(color);
    }
    std::vector<bool> newCapital = capital;
    newCapital.insert(newCapital.end(), right.GetCapital().begin(), right.GetCapital().end());
    return CustomMessage(messages[LANGUAGE_ENG] + right.GetEnglish(MF_RAW),
                         messages[LANGUAGE_GER] + right.GetGerman(MF_RAW),
                         messages[LANGUAGE_FRA] + right.GetFrench(MF_RAW),
                         newColors, newCapital, type, position);
}

CustomMessage CustomMessage::operator+(const std::string& right) const {
    return CustomMessage(messages[LANGUAGE_ENG] + right, messages[LANGUAGE_GER] + right, messages[LANGUAGE_FRA] + right);
}

void CustomMessage::operator+=(const CustomMessage& right) {
    messages[LANGUAGE_ENG] += right.GetEnglish(MF_RAW);
    messages[LANGUAGE_GER] += right.GetGerman(MF_RAW);
    messages[LANGUAGE_FRA] += right.GetFrench(MF_RAW);
    colors.insert(colors.end(), right.GetColors().begin(), right.GetColors().end());
    capital.insert(capital.end(), right.GetCapital().begin(), right.GetCapital().end());
}

void CustomMessage::operator+=(const std::string& right) {
    messages[LANGUAGE_ENG] += right;
    messages[LANGUAGE_GER] += right;
    messages[LANGUAGE_FRA] += right;
}

bool CustomMessage::operator==(const CustomMessage& operand) const {
    return messages[LANGUAGE_ENG] == operand.messages[LANGUAGE_ENG];
}

bool CustomMessage::operator==(const std::string& operand) const {
    for (auto str: messages){
        if (str == operand){
            return true;
        }
    }
    return false;
}

bool CustomMessage::operator!=(const CustomMessage& operand) const {
    return !operator==(operand);
}

void CustomMessage::Replace(std::string&& oldStr, std::string&& newStr) {
    for (std::string& str : messages) {
        size_t position = str.find(oldStr);
        while (position != std::string::npos) {
            str.replace(position, oldStr.length(), newStr);
            position = str.find(oldStr);
        }
    }
}

void CustomMessage::Replace(std::string&& oldStr, CustomMessage newMessage) {
    for (uint8_t language = 0; language < LANGUAGE_MAX; language++) {
        size_t position = messages[language].find(oldStr);
        while (position != std::string::npos) {
            messages[language].replace(position, oldStr.length(), newMessage.messages[language]);
            position = messages[language].find(oldStr);
        }
    }
}

void CustomMessage::Format(ItemID iid) {
    for (std::string &str : messages) {
        str.insert(0, ITEM_OBTAINED(iid));
        size_t start_pos = 0;
        std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
        while ((start_pos = str.find('^', start_pos)) != std::string::npos) {
            str.replace(start_pos, 1, WAIT_FOR_INPUT() + ITEM_OBTAINED(iid));
            start_pos += 3;
        }
        std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
        ReplaceSpecialCharacters(str);
        ReplaceColors(str);
        ReplaceAltarIcons(str);
    }
    *this += MESSAGE_END();
}

void CustomMessage::Format() {
    for (std::string& str : messages) {
        FormatString(str);
    }
}

void CustomMessage::AutoFormat() {
    for (std::string& str : messages) {
        AutoFormatString(str);
    }
}

void CustomMessage::Clean() {
    for (std::string& str : messages) {
        CleanString(str);
    }
}

void CustomMessage::FormatString(std::string& str) const {
    std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
    std::replace(str.begin(), str.end(), '^', WAIT_FOR_INPUT()[0]);
    std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
    ReplaceSpecialCharacters(str);
    ReplaceColors(str);
    ReplaceAltarIcons(str);
    str += MESSAGE_END();
}

void DeleteControlCode(std::string& str, std::string code){
    size_t start_pos = 0;
    while ((start_pos = str.find(code, start_pos)) != std::string::npos) {
        str.replace(start_pos, code.length(), "");
        start_pos += code.length();
    }
}

void CustomMessage::CleanString(std::string& str) const {
    std::replace(str.begin(), str.end(), '&', "\n"[0]);
    std::replace(str.begin(), str.end(), '^', " "[0]);
    for (const auto& colorPair : percentColors) {
        DeleteControlCode(str, "%" + colorPair.first);
    }
    std::erase(str, '#');
    for (const auto& iconPair : altarIcons) {
        DeleteControlCode(str, "$" + iconPair.first);
    }
}

static size_t NextLineLength(const std::string* textStr, const size_t lastNewline, bool hasIcon = false) {
  const  size_t maxLinePixelWidth = hasIcon ? 200 : 216;

  size_t totalPixelWidth = 0;
  size_t currentPos = lastNewline;

  // Looping through the string from the lastNewline until the total
  // width of counted characters exceeds the maximum pixels in a line.
  size_t nextPosJump = 0;
  while (totalPixelWidth < maxLinePixelWidth && currentPos < textStr->length()) {
    // Skip over control codes
    if (textStr->at(currentPos) == '%') {
      nextPosJump = 2;
    } else if (textStr->at(currentPos) == '$') {
      nextPosJump = 2;
    } else if (textStr->at(currentPos) == '@') {
      nextPosJump = 1;
      // Assume worst case for player name 12 * 8 (widest character * longest name length)
      totalPixelWidth += 96;
    } else {
      // Some characters only one byte while others are two bytes
      // So check both possibilities when checking for a character
      if (pixelWidthTable.count(textStr->substr(currentPos, 1))) {
        totalPixelWidth += pixelWidthTable[textStr->substr(currentPos, 1)];
        nextPosJump = 1;
      } else if (pixelWidthTable.count(textStr->substr(currentPos, 2))) {
        totalPixelWidth += pixelWidthTable[textStr->substr(currentPos, 2)];
        nextPosJump = 2;
      } else {
        SPDLOG_DEBUG("Table does not contain " + textStr->substr(currentPos, 1) + "/" + textStr->substr(currentPos, 2));
        SPDLOG_DEBUG("Full string: " + *textStr);
        nextPosJump = 1;
      }
    }
    currentPos += nextPosJump;
  }
  // return the total number of characters we looped through
  if (totalPixelWidth > maxLinePixelWidth && textStr->at(currentPos - nextPosJump) != ' ') {
    return currentPos - lastNewline - nextPosJump;
  } else {
    return currentPos - lastNewline;
  }
}

void CustomMessage::AutoFormatString(std::string& str) const {
    ReplaceAltarIcons(str);
    ReplaceColors(str);
    // insert newlines either manually or when encountering a '&'
    size_t lastNewline = 0;
    const bool hasIcon = str.find('$', 0) != std::string::npos;
    size_t lineLength = NextLineLength(&str, lastNewline, hasIcon);
    size_t lineCount = 1;
    size_t yesNo = str.find("\x1B"s[0], lastNewline);
    while (lastNewline + lineLength < str.length() || yesNo != std::string::npos) {
        const size_t carrot = str.find('^', lastNewline);
        const size_t ampersand = str.find('&', lastNewline);
        const size_t lastSpace = str.rfind(' ', lastNewline + lineLength);
        size_t waitForInput = str.find(WAIT_FOR_INPUT()[0], lastNewline);
        size_t newLine = str.find(NEWLINE()[0], lastNewline);
        if (carrot < waitForInput){
            waitForInput = carrot;
        }
        if (ampersand < newLine){
            newLine = ampersand;
        }
        if (lineCount != 3 && yesNo < lastNewline + lineLength && yesNo < waitForInput && yesNo < newLine){
            if (lineCount >= 4){
                str.replace(yesNo, 1, "^&&\x1B");
                lineCount = 3;
                lastNewline = yesNo + 3;
            } else {
                while(lineCount < 3){
                    str.replace(yesNo, 1, "&\x1B");
                    yesNo++;
                    lineCount++;
                }
                lastNewline = yesNo;
            }
        } else {
            if (lineCount < 4){
                // replace '&' first if it's within the newline range
                if (newLine < lastNewline + lineLength) {
                    lastNewline = newLine + 1;
                    // or move the lastNewline cursor to the next line if a '^' is encountered
                } else if (waitForInput < lastNewline + lineLength) {
                    lastNewline = waitForInput + 1;
                    lineCount = 0;
                    // some lines need to be split but don't have spaces, look for periods instead
                } else if (lastSpace == std::string::npos) {
                    const size_t lastPeriod = str.rfind('.', lastNewline + lineLength);
                    str.replace(lastPeriod, 1, ".&");
                    lastNewline = lastPeriod + 2;
                } else {
                    str.replace(lastSpace, 1, "&");
                    lastNewline = lastSpace + 1;
                }
                lineCount += 1;
            } else { 
                const size_t lastColor = str.rfind("\x05"s, lastNewline + lineLength);
                std::string colorText = "";
                //check if we are on a non default colour, as ^ resets it, and readd if needed
                if (lastColor != std::string::npos && str[lastColor+1] != 0){
                    colorText = "\x05"s + str[lastColor+1];
                }
                // replace '&' first if it's within the newline range
                if (ampersand < lastNewline + lineLength) {
                    str.replace(ampersand, 1, "^" + colorText);
                    lastNewline = ampersand + 1;
                    // or move the lastNewline cursor to the next line if a '^' is encountered.
                } else if (carrot < lastNewline + lineLength) {
                    lastNewline = carrot + 1;
                    // some lines need to be split but don't have spaces, look for periods instead
                } else if (lastSpace == std::string::npos) {
                    const size_t lastPeriod = str.rfind('.', lastNewline + lineLength);
                    str.replace(lastPeriod, 1, ".^" + colorText);
                    lastNewline = lastPeriod + 2;
                } else {
                    str.replace(lastSpace, 1, "^" + colorText);
                    lastNewline = lastSpace + 1;
                }
                lineCount = 1;
            }
            lineLength = NextLineLength(&str, lastNewline, hasIcon);
        }
        yesNo = str.find("\x1B"s[0], lastNewline);
    }
    ReplaceSpecialCharacters(str);
    ReplaceAltarIcons(str);
    std::replace(str.begin(), str.end(), '&', NEWLINE()[0]);
    std::replace(str.begin(), str.end(), '^', WAIT_FOR_INPUT()[0]);
    std::replace(str.begin(), str.end(), '@', PLAYER_NAME()[0]);
    std::replace(str.begin(), str.end(), '_', " "[0]);
    str += MESSAGE_END();
}

void CustomMessage::InsertNumber(uint8_t num){
    for (std::string& str : messages) {
        size_t firstBar = str.find('|');
        if (firstBar != std::string::npos) {
            size_t secondBar = str.find('|', firstBar + 1);
            if (secondBar != std::string::npos) {
                size_t thirdBar = str.find('|', secondBar + 1);
                if (thirdBar != std::string::npos) {
                    if (num == 1) {
                        str.erase(secondBar, thirdBar - secondBar);
                    } else {
                        str.erase(firstBar, secondBar - firstBar);
                    }
                }
            }
        }
    }
    //remove the remaining bar
    this->Replace("|", "");
    Replace("[[d]]", std::to_string(num));
}


void CustomMessage::Capitalize() {
    for (std::string str : messages) {
        (str)[0] = std::toupper((str)[0]);
    }
}

void CustomMessage::ReplaceSpecialCharacters(std::string& str) const {
    // add special characters
    for (auto specialCharacterPair : textBoxSpecialCharacters) {
        size_t start_pos = 0;
        std::string textBoxSpecialCharacterString = ""s;
        textBoxSpecialCharacterString += specialCharacterPair.second;
        while ((start_pos = str.find(specialCharacterPair.first, start_pos)) != std::string::npos) {
            str.replace(start_pos, specialCharacterPair.first.length(), textBoxSpecialCharacterString);
            start_pos += textBoxSpecialCharacterString.length();
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

void CustomMessage::ReplaceColors(std::string& str) const {
    for (const auto& colorPair : percentColors) {
        std::string textToReplace = "%";
        textToReplace += colorPair.first;
        size_t start_pos = 0;
        while ((start_pos = str.find(textToReplace, start_pos)) != std::string::npos) {
            str.replace(start_pos, textToReplace.length(), COLOR(colorPair.second));
            start_pos += textToReplace.length();
        }
    }
    for (auto color: colors) {
        if (const size_t firstHashtag = str.find('#'); firstHashtag != std::string::npos) {
            str.replace(firstHashtag, 1, COLOR(color));
        if (const size_t secondHashtag = str.find('#', firstHashtag + 1); secondHashtag != std::string::npos) {
            str.replace(secondHashtag, 1, COLOR(QM_WHITE));
        } else {
            SPDLOG_DEBUG("non-matching hashtags in string: \"%s\"", str);
        }
        }
    }
    // Remove any remaining '#' characters.
    std::erase(str, '#');
}

void CustomMessage::ReplaceAltarIcons(std::string& str) const {
    for (const auto& iconPair : altarIcons) {
        std::string textToReplace = "$";
        textToReplace += iconPair.first;
        size_t start_pos = 0;
        while ((start_pos = str.find(textToReplace, start_pos)) != std::string::npos) {
            str.replace(start_pos, textToReplace.length(), ITEM_OBTAINED(iconPair.second));
            start_pos += textToReplace.length();
        }
    }
}

void CustomMessage::InsertNames(std::vector<CustomMessage> toInsert){
    for(uint8_t a = 0; a < toInsert.size(); a++){
        CustomMessage temp = toInsert[a];
        if ((capital.size() > a) && (capital[a] = true)){
            temp.Capitalize();
        } 
        Replace("[[" + std::to_string(a+1) + "]]", temp); 
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

std::string CustomMessage::COLOR(std::string x) {
    return "\x05"s + x;
}

std::string CustomMessage::POINTS(std::string x) {
    return "\x1E"s + x;
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
    return InsertCustomMessage(tableID, textID, messageEntry);
}

CustomMessage CustomMessageManager::RetrieveMessage(std::string tableID, uint16_t textID, MessageFormat format) {
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

    if (format == MF_FORMATTED){
        message.Format();
    } else if (format == MF_AUTO_FORMAT){
        message.AutoFormat();
    } else if (format == MF_CLEAN){
        message.Clean();
    }
    
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
