#pragma once

#include <cstdint>
#include <vector>
#include <ship/resource/Resource.h>
#include <libultraship/libultra/types.h>

namespace SOH {
// TODO: we've moved away from using classes for this stuff

class MessageEntryMM {
  public:
    uint16_t id;
    uint8_t textboxType;
    uint8_t textboxYPos;
    uint8_t icon;
    uint16_t nextMessageID;
    uint16_t firstItemCost;
    uint16_t secondItemCost;
    uint32_t segmentId;
    uint32_t msgOffset;
    std::string msg;
};

class TextMM : public Ship::Resource<MessageEntryMM> {
  public:
    using Resource::Resource;

    TextMM() : Resource(std::shared_ptr<Ship::ResourceInitData>()) {
    }

    MessageEntryMM* GetPointer();
    size_t GetPointerSize();

    std::vector<MessageEntryMM> messages;
};
}; // namespace SOH
