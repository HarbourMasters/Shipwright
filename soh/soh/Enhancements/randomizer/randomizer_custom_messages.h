#pragma once

#include <string>
#include "../../../include/z64item.h"

#define QM_WHITE 0x00
#define QM_RED 0x41
#define QM_GREEN 0x42
#define QM_BLUE 0x43
#define QM_LBLUE 0x44
#define QM_PINK 0x45
#define QM_YELLOW 0x46
#define QM_BLACK 0x47

std::string MESSAGE_END();
std::string ITEM_OBTAINED(uint8_t x);
std::string NEWLINE();
std::string COLOR(uint8_t x);