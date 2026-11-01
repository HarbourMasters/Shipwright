#include <tinyxml2.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <optional>
#include <unordered_map>

#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/type/Text.h"
#include "message_data_fmt.h"
#include "message_data_textbox_types.h"
#include "z64save.h"

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextV0::ReadResource(std::shared_ptr<Ship::File> file,
                                          std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto text = std::make_shared<Text>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    uint32_t msgCount = reader->ReadUInt32();
    text->messages.reserve(msgCount);

    for (uint32_t i = 0; i < msgCount; i++) {
        MessageEntry entry;
        entry.id = reader->ReadUInt16();
        entry.textboxType = reader->ReadUByte();
        entry.textboxYPos = reader->ReadUByte();
        entry.msg = reader->ReadString();

        text->messages.push_back(entry);
    }

    return text;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLTextV0::ReadResource(std::shared_ptr<Ship::File> file,
                                       std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto txt = std::make_shared<Text>(initData);
    auto child =
        std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            MessageEntry entry;
            entry.id = child->IntAttribute("ID");
            entry.textboxType = child->IntAttribute("TextboxType");
            entry.textboxYPos = child->IntAttribute("TextboxYPos");
            entry.msg = child->Attribute("Message");
            entry.msg += "\x2";

            txt->messages.push_back(entry);
            int bp = 0;
        }

        child = child->NextSiblingElement();
    }

    return txt;
}

struct TextControl {
    uint8_t code;
    uint8_t valueSize;
};

static const std::unordered_map<std::string, TextControl> sTextControls = {
    { "Space", { ' ', 0 } },
    { "NewLine", { MESSAGE_NEWLINE, 0 } },
    { "End", { MESSAGE_END, 0 } },
    { "BoxBreak", { MESSAGE_BOX_BREAK, 0 } },
    { "Shift", { MESSAGE_SHIFT, 1 } },
    { "QuickTextEnable", { MESSAGE_QUICKTEXT_ENABLE, 0 } },
    { "QuickTextDisable", { MESSAGE_QUICKTEXT_DISABLE, 0 } },
    { "Persistent", { MESSAGE_PERSISTENT, 0 } },
    { "Event", { MESSAGE_EVENT, 0 } },
    { "BoxBreakDelayed", { MESSAGE_BOX_BREAK_DELAYED, 1 } },
    { "AwaitButtonPress", { MESSAGE_AWAIT_BUTTON_PRESS, 0 } },
    { "Fade", { MESSAGE_FADE, 1 } },
    { "PlayerName", { MESSAGE_NAME, 0 } },
    { "Ocarina", { MESSAGE_OCARINA, 0 } },
    { "Fade2", { MESSAGE_FADE2, 2 } },
    { "Sfx", { MESSAGE_SFX, 2 } },
    { "TextSpeed", { MESSAGE_TEXT_SPEED, 1 } },
    { "Background", { MESSAGE_BACKGROUND, 3 } },
    { "MarathonTime", { MESSAGE_MARATHON_TIME, 0 } },
    { "RaceTime", { MESSAGE_RACE_TIME, 0 } },
    { "Points", { MESSAGE_POINTS, 0 } },
    { "Tokens", { MESSAGE_TOKENS, 0 } },
    { "TwoChoice", { MESSAGE_TWO_CHOICE, 0 } },
    { "ThreeChoice", { MESSAGE_THREE_CHOICE, 0 } },
    { "FishInfo", { MESSAGE_FISH_INFO, 0 } },
    { "Time", { MESSAGE_TIME, 0 } },
};

static const std::unordered_map<std::string, uint8_t> sHighScores = {
    { "HighScoreHorseArchery", HS_HBA },    { "HighScorePoePoints", HS_POE_POINTS },
    { "HighScoreLargestFish", HS_FISHING }, { "HighScoreHorseRace", HS_HORSE_RACE },
    { "HighScoreMarathon", HS_MARATHON },   { "HighScoreDampeRace", HS_DAMPE_RACE },
};

static const std::unordered_map<std::string, uint8_t> sTextColors = {
    { "Default", MSGCOL_DEFAULT },     { "Red", MSGCOL_RED },
    { "Green", MSGCOL_ADJUSTABLE },    { "Blue", MSGCOL_BLUE },
    { "LightBlue", MSGCOL_LIGHTBLUE }, { "Purple", MSGCOL_PURPLE },
    { "Pink", MSGCOL_PURPLE },         { "Yellow", MSGCOL_YELLOW },
    { "Black", MSGCOL_BLACK },
};

static const std::unordered_map<std::string, uint8_t> sTextButtons = {
    { "A", 0x9F },     { "B", 0xA0 },      { "C", 0xA1 },       { "L", 0xA2 },
    { "R", 0xA3 },     { "Z", 0xA4 },      { "CUp", 0xA5 },     { "CDown", 0xA6 },
    { "CLeft", 0xA7 }, { "CRight", 0xA8 }, { "ZTarget", 0xA9 }, { "ControlStick", 0xAA },
};

static const std::unordered_map<std::string, uint32_t> sBoxTypes = {
    { "Black", TEXTBOX_TYPE_BLACK },
    { "Sign", TEXTBOX_TYPE_WOODEN },
    { "Blue", TEXTBOX_TYPE_BLUE },
    { "Ocarina", TEXTBOX_TYPE_OCARINA },
    { "NoneBottom", TEXTBOX_TYPE_NONE_BOTTOM },
    { "None", TEXTBOX_TYPE_NONE_NO_SHADOW },
    { "Credits", TEXTBOX_TYPE_CREDITS },
};

static const std::unordered_map<std::string, uint32_t> sBoxPositions = {
    { "Auto", TEXTBOX_POS_VARIABLE },
    { "Top", TEXTBOX_POS_TOP },
    { "Middle", TEXTBOX_POS_MIDDLE },
    { "Bottom", TEXTBOX_POS_BOTTOM },
};

// Font characters from 0x80 on
static const std::vector<std::string> sExtendedChars = {
    "À", "î", "Â", "Ä", "Ç", "È", "É", "Ê", "Ë", "Ï", "Ô", "Ö", "Ù", "Û", "Ü", "ß",
    "à", "á", "â", "ä", "ç", "è", "é", "ê", "ë", "ï", "ô", "ö", "ù", "û", "ü",
};

static const char* ValueOrEmpty(const char* value) {
    return value != nullptr ? value : "";
}

static std::optional<uint32_t> ParseNumber(const char* text, uint32_t max) {
    unsigned value;
    if (text == nullptr || !tinyxml2::XMLUtil::ToUnsigned(text, &value) || value > max) {
        return std::nullopt;
    }

    return value;
}

static uint8_t ReadBoxField(const tinyxml2::XMLElement* element, const char* attribute,
                            const std::unordered_map<std::string, uint32_t>& names, const std::string& logContext) {
    const char* text = element->Attribute(attribute);
    if (text == nullptr) {
        return 0;
    }

    auto name = names.find(text);
    if (name != names.end()) {
        return name->second;
    }

    auto value = ParseNumber(text, 0xF);
    if (value.has_value()) {
        return *value;
    }

    SPDLOG_WARN("{}: {} \"{}\" is not a name or a number up to 15", logContext, attribute, text);
    return 0;
}

static bool ReadUnskippable(const tinyxml2::XMLElement* element, const std::string& logContext) {
    std::string skip = ValueOrEmpty(element->Attribute("Skip"));
    if (skip == "Unskippable") {
        return true;
    }

    if (!skip.empty() && skip != "Normal") {
        SPDLOG_WARN("{}: Skip \"{}\" is not Normal or Unskippable", logContext, skip);
    }
    return false;
}

static std::vector<std::string> SplitLines(const std::string& text) {
    std::vector<std::string> lines;
    size_t start = 0;
    size_t end;

    while ((end = text.find('\n', start)) != std::string::npos) {
        lines.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    lines.push_back(text.substr(start));

    return lines;
}

// Like JSX, spaces next to a line break are removed and the remaining lines are joined with one space
static std::string CollapseWhitespace(const std::string& text) {
    std::vector<std::string> lines = SplitLines(text);
    std::string out;

    for (size_t i = 0; i < lines.size(); i++) {
        std::string line = lines[i];
        std::erase(line, '\t');
        if (i > 0) {
            line.erase(0, line.find_first_not_of(' '));
        }
        if (i < lines.size() - 1) {
            line.erase(line.find_last_not_of(' ') + 1);
        }

        if (line.empty()) {
            continue;
        }
        if (!out.empty()) {
            out += ' ';
        }
        out += line;
    }

    return out;
}

static size_t Utf8CharLength(uint8_t firstByte) {
    if (firstByte >= 0xF0) {
        return 4;
    }
    if (firstByte >= 0xE0) {
        return 3;
    }
    if (firstByte >= 0xC0) {
        return 2;
    }
    return 1;
}

static std::optional<uint8_t> FontByte(const std::string& c) {
    // The font draws 0x5C as a yen sign
    if (c == "¥") {
        return 0x5C;
    }
    if (c.size() == 1 && c[0] >= ' ' && c[0] <= '~' && c[0] != '\\') {
        return c[0];
    }

    auto extended = std::find(sExtendedChars.begin(), sExtendedChars.end(), c);
    if (extended != sExtendedChars.end()) {
        return static_cast<uint8_t>(0x80 + (extended - sExtendedChars.begin()));
    }

    return std::nullopt;
}

static bool HasContent(const tinyxml2::XMLNode* node) {
    for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
        if (child->ToText() != nullptr || child->ToElement() != nullptr) {
            return true;
        }
    }
    return false;
}

class MessageWriter {
  public:
    MessageWriter(const std::string& logContext) : mLogContext(logContext) {
    }

    std::string Write(const tinyxml2::XMLElement* element) {
        WriteHeader(element);
        WriteContent(element);
        if (element->Attribute("Next") != nullptr) {
            WriteNext(element);
        }
        if (!mEnded) {
            Push(MESSAGE_END);
        }
        return mMsg;
    }

  private:
    std::string mLogContext;
    std::string mMsg;
    uint8_t mColor = MSGCOL_DEFAULT;
    bool mUnskippable = false;
    // Track the End command separately; control arguments can also contain 0x02.
    bool mEnded = false;

    void Push(uint8_t byte) {
        mMsg += (char)byte;
        mEnded = false;
    }

    void PushColor(uint8_t color) {
        Push(MESSAGE_COLOR);
        Push(0x40 | color);
    }

    void WriteHeader(const tinyxml2::XMLElement* element) {
        mUnskippable = ReadUnskippable(element, mLogContext);
        if (mUnskippable) {
            Push(MESSAGE_UNSKIPPABLE);
        }
        if (element->Attribute("Icon") != nullptr) {
            WriteControl(element, { MESSAGE_ITEM_ICON, 1 }, "Icon");
        }
    }

    void WriteNext(const tinyxml2::XMLElement* element) {
        if (mEnded) {
            SPDLOG_WARN("{}: Next can't be reached after <End/>", mLogContext);
            return;
        }
        WriteControl(element, { MESSAGE_TEXTID, 2 }, "Next");
    }

    void WriteContent(const tinyxml2::XMLNode* node) {
        for (auto child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
            if (child->ToText() != nullptr) {
                WriteText(child->Value());
            } else if (child->ToElement() != nullptr) {
                WriteTag(child->ToElement());
            }
        }
    }

    void WriteText(const std::string& text) {
        std::string collapsed = CollapseWhitespace(text);
        size_t pos = 0;

        while (pos < collapsed.size()) {
            std::string c = collapsed.substr(pos, Utf8CharLength(collapsed[pos]));
            pos += c.size();

            auto byte = FontByte(c);
            if (byte.has_value()) {
                Push(*byte);
            } else {
                SPDLOG_WARN("{}: \"{}\" is not in the font", mLogContext, c);
            }
        }
    }

    void WriteTag(const tinyxml2::XMLElement* element) {
        std::string name = element->Name();
        auto control = sTextControls.find(name);
        auto highScore = sHighScores.find(name);

        if (name == "Color") {
            WriteColor(element);
            return;
        }

        if (name == "QuickText") {
            Push(MESSAGE_QUICKTEXT_ENABLE);
            WriteContent(element);
            Push(MESSAGE_QUICKTEXT_DISABLE);
            return;
        }

        if (name == "Button") {
            WriteButton(element);
        } else if (highScore != sHighScores.end()) {
            Push(MESSAGE_HIGHSCORE);
            Push(highScore->second);
        } else if (control != sTextControls.end()) {
            WriteControl(element, control->second, "Value");
        } else {
            SPDLOG_WARN("{}: unknown tag <{}>", mLogContext, name);
            WriteContent(element);
            return;
        }

        if (HasContent(element)) {
            SPDLOG_WARN("{}: <{}> can't wrap text, so its text is written after it", mLogContext, name);
            WriteContent(element);
        }
    }

    void WriteColor(const tinyxml2::XMLElement* element) {
        const char* value = ValueOrEmpty(element->Attribute("Value"));
        auto color = sTextColors.find(value);

        if (color == sTextColors.end()) {
            SPDLOG_WARN("{}: <Color> Value \"{}\" is not a color", mLogContext, value);
            WriteContent(element);
            return;
        }

        uint8_t previousColor = mColor;
        mColor = color->second;
        PushColor(mColor);
        if (!HasContent(element)) {
            return;
        }

        // Closing a wrap goes back to the color around it
        WriteContent(element);
        mColor = previousColor;
        PushColor(mColor);
    }

    void WriteButton(const tinyxml2::XMLElement* element) {
        const char* value = ValueOrEmpty(element->Attribute("Value"));
        auto button = sTextButtons.find(value);

        if (button == sTextButtons.end()) {
            SPDLOG_WARN("{}: <Button> Value \"{}\" is not a button", mLogContext, value);
            return;
        }
        Push(button->second);
    }

    void WriteControl(const tinyxml2::XMLElement* element, TextControl control, const char* attribute) {
        if (control.valueSize == 0) {
            Push(control.code);
            mEnded = control.code == MESSAGE_END;
            RepeatUnskippable(control.code);
            return;
        }

        const char* value = element->Attribute(attribute);
        uint32_t max = (1u << (control.valueSize * 8)) - 1;
        auto number = ParseNumber(value, max);
        if (!number.has_value()) {
            SPDLOG_WARN("{}: <{}> {} \"{}\" is not a number up to {}", mLogContext, element->Name(), attribute,
                        ValueOrEmpty(value), max);
            return;
        }

        Push(control.code);
        for (int shift = (control.valueSize - 1) * 8; shift >= 0; shift -= 8) {
            Push((*number >> shift) & 0xFF);
        }
        RepeatUnskippable(control.code);
    }

    // Each new box starts skippable again
    void RepeatUnskippable(uint8_t code) {
        if (mUnskippable && (code == MESSAGE_BOX_BREAK || code == MESSAGE_BOX_BREAK_DELAYED)) {
            Push(MESSAGE_UNSKIPPABLE);
        }
    }
};

static std::optional<MessageEntry> EncodeTextEntry(const tinyxml2::XMLElement* element, const std::string& path) {
    auto id = ParseNumber(element->Attribute("ID"), 0xFFFF);
    if (!id.has_value()) {
        SPDLOG_WARN("Text XML {}: a <TextEntry> has no valid ID", path);
        return std::nullopt;
    }

    std::string logContext = fmt::format("Text XML {} 0x{:04X}", path, *id);
    MessageEntry entry;
    entry.id = *id;
    entry.textboxType = ReadBoxField(element, "Type", sBoxTypes, logContext);
    entry.textboxYPos = ReadBoxField(element, "Position", sBoxPositions, logContext);
    entry.msg = MessageWriter(logContext).Write(element);

    if (entry.msg.size() > Text::MaxMessageSize) {
        SPDLOG_WARN("{}: message is {} bytes, over the {} byte limit", logContext, entry.msg.size(),
                    Text::MaxMessageSize);
        return std::nullopt;
    }

    return entry;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLTextV1::ReadResource(std::shared_ptr<Ship::File> file,
                                       std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto txt = std::make_shared<Text>(initData);
    auto child =
        std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            auto entry = EncodeTextEntry(child, initData->Path);
            if (entry.has_value()) {
                txt->messages.push_back(*entry);
            }
        }

        child = child->NextSiblingElement();
    }

    return txt;
}
} // namespace SOH
