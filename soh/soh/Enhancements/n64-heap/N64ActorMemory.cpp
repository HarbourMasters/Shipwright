#include "N64ActorMemory.h"
#include "N64HeapTables.h"

#include <cstring>

extern "C" {
#include "z64.h"
}

namespace n64heap {

ActorMemory::ActorMemory(const Actor* actor, uint32_t size, const std::function<uint32_t(const void*)>& addressOf)
    : mAddressOf(addressOf), mBytes(size, 0), mKnown(size, true) {
    // Actor_Spawn clears the instance, so bytes no field writes to are zero; actor-specific fields are not modelled
    WriteActorBase(actor);
    std::fill(mKnown.begin() + kActorBaseSize, mKnown.end(), false);
}

bool ActorMemory::ReadWord(uint32_t offset, uint32_t* value) const {
    if (offset + 4 > mBytes.size()) {
        return false;
    }
    for (uint32_t i = 0; i < 4; i++) {
        if (!mKnown[offset + i]) {
            return false;
        }
    }
    *value = (uint32_t(mBytes[offset]) << 24) | (uint32_t(mBytes[offset + 1]) << 16) |
             (uint32_t(mBytes[offset + 2]) << 8) | mBytes[offset + 3];
    return true;
}

void ActorMemory::Put8(uint32_t offset, uint8_t value) {
    mBytes[offset] = value;
    mKnown[offset] = true;
}

void ActorMemory::Put16(uint32_t offset, uint16_t value) {
    Put8(offset, value >> 8);
    Put8(offset + 1, value & 0xFF);
}

void ActorMemory::Put32(uint32_t offset, uint32_t value) {
    Put16(offset, value >> 16);
    Put16(offset + 2, value & 0xFFFF);
}

void ActorMemory::PutFloat(uint32_t offset, float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    Put32(offset, bits);
}

void ActorMemory::PutVec3f(uint32_t offset, const float* xyz) {
    for (uint32_t i = 0; i < 3; i++) {
        PutFloat(offset + i * 4, xyz[i]);
    }
}

void ActorMemory::PutVec3s(uint32_t offset, const int16_t* xyz) {
    for (uint32_t i = 0; i < 3; i++) {
        Put16(offset + i * 2, static_cast<uint16_t>(xyz[i]));
    }
}

void ActorMemory::PutActor(uint32_t offset, const void* actor) {
    uint32_t address = actor != nullptr ? mAddressOf(actor) : 0;
    Put32(offset, address);
    if (actor != nullptr && address == 0) {
        std::fill(mKnown.begin() + offset, mKnown.begin() + offset + 4, false);
    }
}

// Pointers into code, overlays or the collision system have no known N64 value unless they are NULL
void ActorMemory::PutUnknownPointer(uint32_t offset, const void* pointer) {
    Put32(offset, 0);
    if (pointer != nullptr) {
        std::fill(mKnown.begin() + offset, mKnown.begin() + offset + 4, false);
    }
}

void ActorMemory::WriteActorBase(const Actor* actor) {
    Put16(0x000, actor->id);
    Put8(0x002, actor->category);
    Put8(0x003, actor->room);
    Put32(0x004, actor->flags);
    PutVec3f(0x008, &actor->home.pos.x);
    PutVec3s(0x014, &actor->home.rot.x);
    Put16(0x01C, actor->params);
    Put8(0x01E, actor->objBankIndex);
    Put8(0x01F, actor->targetMode);
    Put16(0x020, actor->sfx);
    PutVec3f(0x024, &actor->world.pos.x);
    PutVec3s(0x030, &actor->world.rot.x);
    PutVec3f(0x038, &actor->focus.pos.x);
    PutVec3s(0x044, &actor->focus.rot.x);
    PutFloat(0x04C, actor->targetArrowOffset);
    PutVec3f(0x050, &actor->scale.x);
    PutVec3f(0x05C, &actor->velocity.x);
    PutFloat(0x068, actor->speedXZ);
    PutFloat(0x06C, actor->gravity);
    PutFloat(0x070, actor->minVelocityY);
    PutUnknownPointer(0x074, actor->wallPoly);
    PutUnknownPointer(0x078, actor->floorPoly);
    Put8(0x07C, actor->wallBgId);
    Put8(0x07D, actor->floorBgId);
    Put16(0x07E, actor->wallYaw);
    PutFloat(0x080, actor->floorHeight);
    PutFloat(0x084, actor->yDistToWater);
    Put16(0x088, actor->bgCheckFlags);
    Put16(0x08A, actor->yawTowardsPlayer);
    PutFloat(0x08C, actor->xyzDistToPlayerSq);
    PutFloat(0x090, actor->xzDistToPlayer);
    PutFloat(0x094, actor->yDistToPlayer);

    const CollisionCheckInfo& colChk = actor->colChkInfo;
    PutUnknownPointer(0x098, colChk.damageTable);
    PutVec3f(0x09C, &colChk.displacement.x);
    Put16(0x0A8, colChk.cylRadius);
    Put16(0x0AA, colChk.cylHeight);
    Put16(0x0AC, colChk.cylYShift);
    Put8(0x0AE, colChk.mass);
    Put8(0x0AF, colChk.health);
    Put8(0x0B0, colChk.damage);
    Put8(0x0B1, colChk.damageEffect);
    Put8(0x0B2, colChk.atHitEffect);
    Put8(0x0B3, colChk.acHitEffect);

    const ActorShape& shape = actor->shape;
    PutVec3s(0x0B4, &shape.rot.x);
    Put16(0x0BA, shape.face);
    PutFloat(0x0BC, shape.yOffset);
    PutUnknownPointer(0x0C0, reinterpret_cast<const void*>(shape.shadowDraw));
    PutFloat(0x0C4, shape.shadowScale);
    Put8(0x0C8, shape.shadowAlpha);
    Put8(0x0C9, shape.feetFloorFlags);
    PutVec3f(0x0CC, &shape.feetPos[0].x);
    PutVec3f(0x0D8, &shape.feetPos[1].x);

    PutVec3f(0x0E4, &actor->projectedPos.x);
    PutFloat(0x0F0, actor->projectedW);
    PutFloat(0x0F4, actor->uncullZoneForward);
    PutFloat(0x0F8, actor->uncullZoneScale);
    PutFloat(0x0FC, actor->uncullZoneDownward);
    PutVec3f(0x100, &actor->prevPos.x);
    Put8(0x10C, actor->isTargeted);
    Put8(0x10D, actor->targetPriority);
    Put16(0x10E, actor->textId);
    Put16(0x110, actor->freezeTimer);
    Put16(0x112, actor->colorFilterParams);
    Put8(0x114, actor->colorFilterTimer);
    Put8(0x115, actor->isDrawn);
    Put8(0x116, actor->dropFlag);
    Put8(0x117, actor->naviEnemyId);
    PutActor(0x118, actor->parent);
    PutActor(0x11C, actor->child);
    PutActor(0x120, actor->prev);
    PutActor(0x124, actor->next);
    PutUnknownPointer(0x128, reinterpret_cast<const void*>(actor->init));
    PutUnknownPointer(0x12C, reinterpret_cast<const void*>(actor->destroy));
    PutUnknownPointer(0x130, reinterpret_cast<const void*>(actor->update));
    PutUnknownPointer(0x134, reinterpret_cast<const void*>(actor->draw));
    Put32(0x138, kActorOverlayTable + actor->id * kActorOverlayEntrySize);
}

} // namespace n64heap
