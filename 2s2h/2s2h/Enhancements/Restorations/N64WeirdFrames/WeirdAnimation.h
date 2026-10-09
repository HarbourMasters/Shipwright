#pragma once

#include <libultraship/libultra/types.h>

#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

enum class IndexDirection {
    BACKWARD,
    FORWARD,
};

class WeirdAnimation {
  public:
    WeirdAnimation(std::string targetAnimation, s32 limbCount, IndexDirection direction,
                   std::vector<std::string> neighborAnimations)
        : targetAnimation(std::move(targetAnimation)), limbCount(limbCount), direction(direction),
          neighborAnimations(std::move(neighborAnimations)) {
    }

    const std::string& GetTargetAnimation() {
        return targetAnimation;
    }

    IndexDirection GetDirection() {
        return direction;
    }

    const void* GetFrame(s32 frame, s32 targetAnimationFrameCount);

  private:
    std::string targetAnimation;
    s32 limbCount;
    IndexDirection direction;
    std::vector<std::string> neighborAnimations;

    std::optional<std::vector<std::byte>> animationData;

    void Build();
};
