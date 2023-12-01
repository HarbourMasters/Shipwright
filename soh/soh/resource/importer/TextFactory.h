#pragma once

#include "Resource.h"
#include "ResourceFactory.h"
#include "include/message_data_fmt.h"
#include "include/message_data_textbox_types.h"
#include <unordered_map>

namespace LUS {
class TextFactory : public ResourceFactory
{
  public:
    std::shared_ptr<IResource>
    ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) override;
    std::shared_ptr<IResource>
    ReadResourceXML(std::shared_ptr<ResourceInitData> initData, tinyxml2::XMLElement *reader) override;
};

class TextFactoryV0 : public ResourceVersionFactory
{
  public:
    void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource) override;
    void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) override;
};
}; // namespace LUS

const std::unordered_map<std::string, int> XMLToTextBoxType = {
    { "Black",        TEXTBOX_TYPE_BLACK },
    { "Wooden",       TEXTBOX_TYPE_WOODEN },
    { "Blue",         TEXTBOX_TYPE_BLUE },
    { "Ocarina",      TEXTBOX_TYPE_OCARINA },
    { "NoneBottom",   TEXTBOX_TYPE_NONE_BOTTOM },
    { "NoneNoShadow", TEXTBOX_TYPE_NONE_NO_SHADOW },
    { "Credits",      TEXTBOX_TYPE_CREDITS }
};

const std::unordered_map<int, std::string> TextBoxTypeToXML = {
    { TEXTBOX_TYPE_BLACK,          "Black" },
    { TEXTBOX_TYPE_WOODEN,         "Wooden" },
    { TEXTBOX_TYPE_BLUE,           "Blue" },
    { TEXTBOX_TYPE_OCARINA,        "Ocarina" },
    { TEXTBOX_TYPE_NONE_BOTTOM,    "NoneBottom" },
    { TEXTBOX_TYPE_NONE_NO_SHADOW, "NoneNoShadow" },
    { TEXTBOX_TYPE_CREDITS,        "Credits" }
};

const std::unordered_map<std::string, int> XMLToTextBoxPosition = {
    { "Variable", TEXTBOX_POS_VARIABLE },
    { "Top",      TEXTBOX_POS_TOP },
    { "Middle",   TEXTBOX_POS_MIDDLE },
    { "Bottom",   TEXTBOX_POS_BOTTOM }
};

const std::unordered_map<int, std::string> TextBoxPositionToXML = {
    { TEXTBOX_POS_VARIABLE, "Variable" },
    { TEXTBOX_POS_TOP,      "Top" },
    { TEXTBOX_POS_MIDDLE,   "Middle" },
    { TEXTBOX_POS_BOTTOM,   "Bottom" }
};

const std::unordered_map<std::string, int> XMLToControlCode = {
    { "Newline",          MESSAGE_NEWLINE },
    { "End",              MESSAGE_END },
    { "BoxBreak",         MESSAGE_BOX_BREAK },
    { "Color",            MESSAGE_COLOR },
    { "Shift",            MESSAGE_SHIFT },
    { "TextID",           MESSAGE_TEXTID },
    { "QuicktextEnable",  MESSAGE_QUICKTEXT_ENABLE },
    { "QuicktextDisable", MESSAGE_QUICKTEXT_DISABLE },
    { "Persistent",       MESSAGE_PERSISTENT },
    { "Event",            MESSAGE_EVENT },
    { "BoxBreakDelayed",  MESSAGE_BOX_BREAK_DELAYED },
    { "AwaitButtonPress", MESSAGE_AWAIT_BUTTON_PRESS },
    { "Fade",             MESSAGE_FADE },
    { "Name",             MESSAGE_NAME },
    { "Ocarina",          MESSAGE_OCARINA },
    { "Fade2",            MESSAGE_FADE2 },
    { "SFX",              MESSAGE_SFX },
    { "ItemIcon",         MESSAGE_ITEM_ICON },
    { "TextSpeed",        MESSAGE_TEXT_SPEED },
    { "Background",       MESSAGE_BACKGROUND },
    { "MarathonTime",     MESSAGE_MARATHON_TIME },
    { "RaceTime",         MESSAGE_RACE_TIME },
    { "Points",           MESSAGE_POINTS },
    { "Tokens",           MESSAGE_TOKENS },
    { "Unskippable",      MESSAGE_UNSKIPPABLE },
    { "TwoChoice",        MESSAGE_TWO_CHOICE },
    { "ThreeChoice",      MESSAGE_THREE_CHOICE },
    { "FishInfo",         MESSAGE_FISH_INFO },
    { "Highscore",        MESSAGE_HIGHSCORE },
    { "Time",             MESSAGE_TIME },
};

const std::unordered_map<int, std::string> ControlCodeToXML = {
    { MESSAGE_NEWLINE,            "Newline", },
    { MESSAGE_END,                "End", },
    { MESSAGE_BOX_BREAK,          "BoxBreak", },
    { MESSAGE_COLOR,              "Color", },
    { MESSAGE_SHIFT,              "Shift", },
    { MESSAGE_TEXTID,             "TextID", },
    { MESSAGE_QUICKTEXT_ENABLE,   "QuicktextEnable", },
    { MESSAGE_QUICKTEXT_DISABLE,  "QuicktextDisable",},
    { MESSAGE_PERSISTENT,         "Persistent", },
    { MESSAGE_EVENT,              "Event", },
    { MESSAGE_BOX_BREAK_DELAYED,  "BoxBreakDelayed", },
    { MESSAGE_AWAIT_BUTTON_PRESS, "AwaitButtonPress",},
    { MESSAGE_FADE,               "Fade", },
    { MESSAGE_NAME,               "Name", },
    { MESSAGE_OCARINA,            "Ocarina", },
    { MESSAGE_FADE2,              "Fade2", },
    { MESSAGE_SFX,                "SFX", },
    { MESSAGE_ITEM_ICON,          "ItemIcon", },
    { MESSAGE_TEXT_SPEED,         "TextSpeed", },
    { MESSAGE_BACKGROUND,         "Background", },
    { MESSAGE_MARATHON_TIME,      "MarathonTime", },
    { MESSAGE_RACE_TIME,          "RaceTime", },
    { MESSAGE_POINTS,             "Points", },
    { MESSAGE_TOKENS,             "Tokens", },
    { MESSAGE_UNSKIPPABLE,        "Unskippable", },
    { MESSAGE_TWO_CHOICE,         "TwoChoice", },
    { MESSAGE_THREE_CHOICE,       "ThreeChoice", },
    { MESSAGE_FISH_INFO,          "FishInfo", },
    { MESSAGE_HIGHSCORE,          "Highscore", },
    { MESSAGE_TIME,               "Time", },
};

const std::unordered_map<std::string, int> XMLToColorCode = {
    { "Default",    MSGCOL_DEFAULT },
    { "Red",        MSGCOL_RED },
    { "Adjustable", MSGCOL_ADJUSTABLE },
    { "Blue",       MSGCOL_BLUE },
    { "LightBlue",  MSGCOL_LIGHTBLUE },
    { "Purple",     MSGCOL_PURPLE },
    { "Yellow",     MSGCOL_YELLOW },
    { "Black",      MSGCOL_BLACK },
};

const std::unordered_map<int, std::string> ColorCodeToXML = {
    { MSGCOL_DEFAULT,    "Default" },
    { MSGCOL_RED,        "Red" },
    { MSGCOL_ADJUSTABLE, "Adjustable" },
    { MSGCOL_BLUE,       "Blue" },
    { MSGCOL_LIGHTBLUE,  "LightBlue" },
    { MSGCOL_PURPLE,     "Purple" },
    { MSGCOL_YELLOW,     "Yellow" },
    { MSGCOL_BLACK,      "Black" },
};

const std::unordered_map<std::string, std::string> XMLToSpecialChar = {
    { "‾",             "\x7F" },
    { "À",             "\x80" },
    { "î",             "\x81" },
    { "Â",             "\x82" },
    { "Ä",             "\x83" },
    { "Ç",             "\x84" },
    { "È",             "\x85" },
    { "É",             "\x86" },
    { "Ê",             "\x87" },
    { "Ë",             "\x88" },
    { "Ï",             "\x89" },
    { "Ô",             "\x8A" },
    { "Ö",             "\x8B" },
    { "Ù",             "\x8C" },
    { "Û",             "\x8D" },
    { "Ü",             "\x8E" },
    { "ß",             "\x8F" },
    { "à",             "\x90" },
    { "á",             "\x91" },
    { "â",             "\x92" },
    { "ä",             "\x93" },
    { "ç",             "\x94" },
    { "è",             "\x95" },
    { "é",             "\x96" },
    { "ê",             "\x97" },
    { "ë",             "\x98" },
    { "ï",             "\x99" },
    { "ô",             "\x9A" },
    { "ö",             "\x9B" },
    { "ù",             "\x9C" },
    { "û",             "\x9D" },
    { "ü",             "\x9E" },
    { "[A]",           "\x9F" },
    { "[B]",           "\xA0" },
    { "[C]",           "\xA1" },
    { "[L]",           "\xA2" },
    { "[R]",           "\xA3" },
    { "[Z]",           "\xA4" },
    { "[C-Up]",        "\xA5" },
    { "[C-Down]",      "\xA6" },
    { "[C-Left]",      "\xA7" },
    { "[C-Right]",     "\xA8" },
    { "▼",             "\xA9" },
    { "[Control-Pad]", "\xAA" },
    { "[D-Pad]",       "\xAB" },
};

const std::unordered_map<int, std::string> SpecialCharToXML = {
    { '\x7F', "‾" },
    { '\x80', "À" },
    { '\x81', "î" },
    { '\x82', "Â" },
    { '\x83', "Ä" },
    { '\x84', "Ç" },
    { '\x85', "È" },
    { '\x86', "É" },
    { '\x87', "Ê" },
    { '\x88', "Ë" },
    { '\x89', "Ï" },
    { '\x8A', "Ô" },
    { '\x8B', "Ö" },
    { '\x8C', "Ù" },
    { '\x8D', "Û" },
    { '\x8E', "Ü" },
    { '\x8F', "ß" },
    { '\x90', "à" },
    { '\x91', "á" },
    { '\x92', "â" },
    { '\x93', "ä" },
    { '\x94', "ç" },
    { '\x95', "è" },
    { '\x96', "é" },
    { '\x97', "ê" },
    { '\x98', "ë" },
    { '\x99', "ï" },
    { '\x9A', "ô" },
    { '\x9B', "ö" },
    { '\x9C', "ù" },
    { '\x9D', "û" },
    { '\x9E', "ü" },
    { '\x9F', "[A]" },
    { '\xA0', "[B]" },
    { '\xA1', "[C]" },
    { '\xA2', "[L]" },
    { '\xA3', "[R]" },
    { '\xA4', "[Z]" },
    { '\xA5', "[C-Up]" },
    { '\xA6', "[C-Down]" },
    { '\xA7', "[C-Left]" },
    { '\xA8', "[C-Right]" },
    { '\xA9', "▼" },
    { '\xAA', "[Control-Pad]" },
    { '\xAB', "[D-Pad]" },
};