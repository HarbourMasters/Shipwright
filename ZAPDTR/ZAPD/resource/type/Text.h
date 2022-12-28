#pragma once

#include <cstdint>
#include <vector>
#include "Resource.h"
#include <libultraship/libultra/types.h>

namespace Ship {
class MessageEntry
{
public:
	uint16_t id;
	uint8_t textboxType;
	uint8_t textboxYPos;
	std::string msg;
};

class Text : public Resource {
public:
  void* GetPointer();
  size_t GetPointerSize();

  std::vector<MessageEntry> messages;
};
}; // namespace Ship
