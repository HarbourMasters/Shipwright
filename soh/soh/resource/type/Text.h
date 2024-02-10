#pragma once

#include <cstdint>
#include <vector>
#include "Resource.h"
#include <libultraship/libultra/types.h>

namespace SOH {
// TODO: we've moved away from using classes for this stuff
class MessageEntry
{
public:
	uint16_t id;
	uint8_t textboxType;
	uint8_t textboxYPos;
	std::string msg;
};

class Text : public LUS::Resource<MessageEntry> {
public:
  using Resource::Resource;

    Text() : Resource(std::shared_ptr<LUS::ResourceInitData>()) {}

    MessageEntry* GetPointer();
    size_t GetPointerSize();

    std::vector<MessageEntry> messages;
};
}; // namespace LUS
