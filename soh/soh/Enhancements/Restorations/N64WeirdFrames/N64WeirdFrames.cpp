#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#include <vector>
#include <spdlog/spdlog.h>

#include "WeirdAnimation.h"

extern "C" {
#include "macros.h"
#include "z64player.h"

#include "objects/gameplay_keep/gameplay_keep.h"

int ResourceMgr_OTRSigCheck(char* imgData);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
}

#define CVAR_NAME CVAR_ENHANCEMENT("N64WeirdFrames")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

// A list of weird animations to contruct. These can either be an index out of bounds before
// the start of the animation or past the end of it. In either case you add a list of animations'
// data that are neighboring before or after the target animation. If more weird frame data is
// required then add more of the neighboring animations in ROM.
static std::vector<WeirdAnimation> weirdAnimations{
    // For weirdshots.
    { gPlayerAnim_link_bow_side_walk,
      PLAYER_LIMB_MAX,
      IndexDirection::BACKWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnimData_0940E0",
          "__OTR__misc/link_animetion/gPlayerAnimData_096370",
          "__OTR__misc/link_animetion/gPlayerAnimData_096610",
          "__OTR__misc/link_animetion/gPlayerAnimData_097650",
          "__OTR__misc/link_animetion/gPlayerAnimData_097FC0",
      } },
    // For weirdslides.
    { gPlayerAnim_link_normal_side_walk,
      PLAYER_LIMB_MAX,
      IndexDirection::BACKWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnimData_1AFDD0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B0E10",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B1910",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B20F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B25B0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B2B80",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B3600",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B4080",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B4B00",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B51D0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B5BD0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B6C10",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B72E0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B7B40",
      } },
    // For weirdslides.
    { gPlayerAnim_link_normal_side_walk,
      PLAYER_LIMB_MAX,
      IndexDirection::FORWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnimData_1B92D0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B9B30",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BA390",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BB2C0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BC6B0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BCA60",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BDC30",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BE410",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BF6F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C01F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C0CF0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C1230",
      } },
    // For weirdslides.
    { gPlayerAnim_link_normal_side_walk_free,
      PLAYER_LIMB_MAX,
      IndexDirection::BACKWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnimData_1B1910",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B20F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B25B0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B2B80",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B3600",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B4080",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B4B00",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B51D0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B5BD0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B6C10",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B72E0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B7B40",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B83A0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B92D0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1B9B30",
      } },
    // For weirdslides.
    { gPlayerAnim_link_normal_side_walk_free,
      PLAYER_LIMB_MAX,
      IndexDirection::FORWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnimData_1BB2C0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BC6B0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BCA60",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BDC30",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BE410",
          "__OTR__misc/link_animetion/gPlayerAnimData_1BF6F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C01F0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C0CF0",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C1230",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C1C30",
          "__OTR__misc/link_animetion/gPlayerAnimData_1C2630",
      } },
};

void RegisterN64WeirdFrames() {
    COND_VB_SHOULD(VB_LOAD_PLAYER_ANIMATION_FRAME, CVAR_VALUE, {
        const auto entry = va_arg(args, AnimationEntry*);
        if (entry == nullptr) {
            return;
        }

        auto animation = va_arg(args, LinkAnimationHeader*);
        auto frame = va_arg(args, s32);
        const auto limbCount = va_arg(args, s32);
        const auto frameTable = va_arg(args, Vec3s*);

        std::optional<const char*> animationName;

        if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(animation)) != 0) {
            animationName = reinterpret_cast<const char*>(animation);
            animation = reinterpret_cast<LinkAnimationHeader*>(ResourceMgr_LoadAnimByName(*animationName));
        }

        const auto playerAnimHeader =
            static_cast<LinkAnimationHeader*>(SEGMENTED_TO_VIRTUAL(static_cast<void*>(animation)));

        if (frame < 0 || frame >= playerAnimHeader->common.frameCount) {
            const auto direction = frame < 0 ? IndexDirection::BACKWARD : IndexDirection::FORWARD;

            if (animationName.has_value()) {
                for (auto& weirdAnimation : weirdAnimations) {
                    if (weirdAnimation.GetDirection() == direction &&
                        weirdAnimation.GetTargetAnimation() == *animationName) {
                        if (const auto frameData = weirdAnimation.GetFrame(frame, playerAnimHeader->common.frameCount);
                            frameData != nullptr) {
                            *should = false;

                            SPDLOG_DEBUG("Weird animation for \"{}\": frame {}", weirdAnimation.GetTargetAnimation(),
                                         frame);
                            std::memcpy(frameTable, frameData, sizeof(Vec3s) * limbCount + sizeof(s16));
                        } else {
                            SPDLOG_WARN("Weird Frame {} not included in weird animation for \"{}\"", frame,
                                        weirdAnimation.GetTargetAnimation());
                        }

                        return;
                    }
                }
            }

            SPDLOG_WARN("Weird Animation not present for \"{}\" but frame {} is out of bounds",
                        animationName.has_value() ? *animationName : "<null>", frame);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterN64WeirdFrames, { CVAR_NAME });
