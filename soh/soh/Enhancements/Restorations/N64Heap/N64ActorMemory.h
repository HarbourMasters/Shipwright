#pragma once

// Rebuilds the NTSC 1.2 bytes of a live actor from SoH's copy of it

#include <cstdint>
#include <functional>
#include <vector>

struct Actor;

namespace n64heap {

class ActorMemory {
  public:
    // addressOf maps a host actor to its N64 address, 0 when the shadow heap does not hold it
    ActorMemory(const Actor* actor, uint32_t size, const std::function<uint32_t(const void*)>& addressOf);
    bool ReadWord(uint32_t offset, uint32_t* value) const;
    const std::vector<uint8_t>& Bytes() const {
        return mBytes;
    }
    const std::vector<bool>& Known() const {
        return mKnown;
    }

  private:
    void Put8(uint32_t offset, uint8_t value);
    void Put16(uint32_t offset, uint16_t value);
    void Put32(uint32_t offset, uint32_t value);
    void PutFloat(uint32_t offset, float value);
    void PutVec3f(uint32_t offset, const float* xyz);
    void PutVec3s(uint32_t offset, const int16_t* xyz);
    void PutActor(uint32_t offset, const void* actor);
    void PutUnknownPointer(uint32_t offset, const void* pointer);
    void WriteActorBase(const Actor* actor);

    std::function<uint32_t(const void*)> mAddressOf;
    std::vector<uint8_t> mBytes;
    std::vector<bool> mKnown;
};

} // namespace n64heap
