#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#include <vector>
#include <spdlog/spdlog.h>

#include "WeirdAnimation.h"

extern "C" {
#include "functions.h"
#include "z64player.h"

#include "objects/gameplay_keep/gameplay_keep.h"

int ResourceMgr_OTRSigCheck(char* imgData);
AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path);
}

#define CVAR_NAME "gEnhancements.Restorations.N64WeirdFrames"
// This CVAR is defaulted to on, because it is consistent with hardware, but able to be disabled if desired.
#define CVAR CVarGetInteger(CVAR_NAME, 1)

// A list of weird animations to contruct. These can either be an index out of bounds before
// the start of the animation or past the end of it. In either case you add a list of animations'
// data that are neighboring before or after the target animation. If more weird frame data is
// required then add more of the neighboring animations in ROM.
static std::vector<WeirdAnimation> weirdAnimations{
    // For weirdshots.
    { gPlayerAnim_link_bow_side_walk,
      PLAYER_LIMB_MAX,
      IndexDirection::FORWARD,
      {
          "__OTR__misc/link_animetion/gPlayerAnim_alink_ozigi_Data",
          "__OTR__misc/link_animetion/gPlayerAnim_pz_attackBendR_Data",
          "__OTR__misc/link_animetion/gPlayerAnim_pz_attackCendR_Data",
          "__OTR__misc/link_animetion/gPlayerAnim_pz_attackAendR_Data",
          "__OTR__misc/link_animetion/gPlayerAnim_pz_cutterattack_Data",
          "__OTR__misc/link_animetion/gPlayerAnim_kf_hanare_loop_Data",
          "__OTR__misc/link_animetion/gPlayerAnimData_2C6350",
      } },
};

void RegisterN64WeirdFrames() {
    COND_VB_SHOULD(VB_LOAD_PLAYER_ANIMATION_FRAME, CVAR, {
        const auto entry = va_arg(args, AnimTask*);
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
            static_cast<LinkAnimationHeader*>(Lib_SegmentedToVirtual(static_cast<void*>(animation)));

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
