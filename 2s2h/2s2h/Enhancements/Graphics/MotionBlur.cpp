#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
}

const char* motionBlurOptions[] = { "Dynamic (default)", "Always Off", "Always On" };

void MotionBlur_RenderMenuOptions() {
    ImGui::SeparatorText("Motion Blur");
    UIWidgets::CVarCombobox("Motion Blur Mode", "gEnhancements.Graphics.MotionBlur.Mode", motionBlurOptions,
                            UIWidgets::ComboboxOptions().LabelPosition(UIWidgets::LabelPosition::None));

    UIWidgets::CVarCheckbox(
        "Interpolate", "gEnhancements.Graphics.MotionBlur.Interpolate",
        UIWidgets::CheckboxOptions().Tooltip(
            "Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
            "This notably reduces the overall motion blur strength but smooths out the trails."));

    if (CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0) == 0) {
        UIWidgets::Checkbox("On/Off", (bool*)&R_MOTION_BLUR_ENABLED);
        if (R_MOTION_BLUR_ENABLED) {
            int32_t motionBlurStrength = R_MOTION_BLUR_ALPHA;
            if (UIWidgets::SliderInt("Strength", &motionBlurStrength, { .min = 0, .max = 255 })) {
                R_MOTION_BLUR_ALPHA = motionBlurStrength;
            }
        }
    }
    if (CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0) == 2) {
        UIWidgets::CVarSliderInt("Strength", "gEnhancements.Graphics.MotionBlur.Strength",
                                 UIWidgets::IntSliderOptions().Min(0).Max(255).DefaultValue(180));
    }
}

extern "C" void MotionBlur_Override(u8* status, s32* alpha) {
    if (CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0) == 1) {
        *status = 0;
        *alpha = 0;
    } else if (CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0) == 2) {
        if (*status == 0)
            *status = 2;
        *alpha = CVarGetInteger("gEnhancements.Graphics.MotionBlur.Strength", 180);
    }
}
