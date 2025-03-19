#include "custom_messages.hpp"
#include "../../custom-message/CustomMessageManager.h"
#include "z64item.h"

namespace CustomMessages {
using namespace std::literals::string_literals;

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
    std::string TWO_WAY_CHOICE()       { return  "\x1B"s; }
    std::string NEWLINE()              { return  "\x7F\x1C"s; }
    std::string COLOR(std::string x)   { return  "\x7F\x1D"s + x; }
    std::string CENTER_TEXT()          { return  "\x7F\x1E"s; }
    std::string IF_NOT_MQ()            { return  "\x7F\x29"s; }
    std::string MQ_ELSE()              { return  "\x7F\x2A"s; }
    std::string MQ_END()               { return  "\x7F\x2B"s; }
}
