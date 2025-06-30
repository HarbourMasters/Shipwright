#include "WeirdAnimation.h"

#include "resource/ResourceManager.h"
#include "Context.h"

#include <cassert>
#include <cstring>
#include <spdlog/spdlog.h>

extern "C" {
#include "z64math.h"
}

const void* WeirdAnimation::GetFrame(s32 frame, s32 targetAnimationFrameCount) {
    if (!animationData.has_value()) {
        Build();
    }

    if (direction == IndexDirection::BACKWARD) {
        const auto frameData = std::end(*animationData) + (((sizeof(Vec3s) * limbCount + sizeof(s16)) * frame));

        return frameData >= std::begin(*animationData) ? &*frameData : nullptr;
    } else if (direction == IndexDirection::FORWARD) {
        const auto frameData = std::begin(*animationData) +
                               (((sizeof(Vec3s) * limbCount + sizeof(s16)) * (frame - targetAnimationFrameCount)));

        return frameData < std::end(*animationData) ? &*frameData : nullptr;
    } else {
        SPDLOG_ERROR("Expected valid IndexDirection, got: {}", static_cast<int>(direction));
        assert(false);
        return nullptr;
    }
}

void WeirdAnimation::Build() {
    auto& animation = animationData.emplace();

    for (const auto& neighborName : neighborAnimations) {
        const auto neighbor = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(neighborName);

        const auto prevSize = animation.size();
        animation.resize(prevSize + neighbor->GetPointerSize());
        std::memcpy(animation.data() + prevSize, neighbor->GetRawPointer(), neighbor->GetPointerSize());

        // Animation data in ROM is padded to 0x10 bytes. Align the buffer the same way.
        animation.resize((animation.size() + 0xF) & ~0xF);
    }

    const auto frameCount = animation.size() / (sizeof(Vec3s) * limbCount + sizeof(s16));
    const auto directionStr = [this] {
        switch (direction) {
            case IndexDirection::BACKWARD:
                return "backward";
            case IndexDirection::FORWARD:
                return "forward";
            default:
                SPDLOG_ERROR("Expected valid IndexDirection, got: {}", static_cast<int>(direction));
                assert(false);
                return "???";
        }
    }();

    SPDLOG_DEBUG("Weird animation built for \"{}\": frameCount = {}, direction = {}", targetAnimation, frameCount,
                 directionStr);
}
