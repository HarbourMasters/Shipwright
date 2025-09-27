#pragma once

#include <string>
#include <cstdint>

#include "text.hpp"

namespace CustomMessages {
std::string MESSAGE_END();
std::string WAIT_FOR_INPUT();
std::string HORIZONTAL_SPACE(uint8_t x);
std::string GO_TO(uint16_t x);
std::string INSTANT_TEXT_ON();
std::string INSTANT_TEXT_OFF();
std::string SHOP_MESSAGE_BOX();
std::string EVENT_TRIGGER();
std::string DELAY_FRAMES(uint8_t x);
std::string CLOSE_AFTER(uint8_t x);
std::string PLAYER_NAME();
std::string PLAY_OCARINA();
std::string ITEM_OBTAINED(uint8_t x);
std::string SET_SPEED(uint8_t x);
std::string SKULLTULAS_DESTROYED();
std::string CURRENT_TIME();
std::string UNSKIPPABLE();
std::string TWO_WAY_CHOICE();
std::string NEWLINE();
std::string COLOR(std::string x);
std::string CENTER_TEXT();
std::string IF_NOT_MQ();
std::string MQ_ELSE();
std::string MQ_END();
} // namespace CustomMessages
