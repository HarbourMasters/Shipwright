#include "ResolutionEditor.h"
#include <ImGui/imgui.h>
#include <libultraship/libultraship.h>

#include <soh/UIWidgets.hpp>
#include <graphic/Fast3D/gfx_pc.h>

/*  Console Variables are grouped under gAdvancedResolution. (e.g. "gAdvancedResolution.Enabled")

    The following CVars are used in Libultraship and can be edited here:
        - Enabled                                       - Turns Advanced Resolution Mode on.
        - AspectRatioX, AspectRatioY                    - Aspect ratio controls. To toggle off, set either to zero.
        - VerticalPixelCount, VerticalResolutionToggle  - Resolution controls.
        - PixelPerfectMode, IntegerScale.Factor         - Pixel Perfect Mode a.k.a. integer scaling controls.
        - IntegerScale.FitAutomatically                 - Automatic resizing for Pixel Perfect Mode.
        - IntegerScale.NeverExceedBounds                - Prevents manual resizing from exceeding screen bounds.

    The following CVars are also implemented in LUS for niche use cases:
        - IgnoreAspectCorrection                        - Stretch framebuffer to fill screen.
            This is something of a power-user setting for niche setups that most people won't need or care about,
            but may be useful if playing the Switch/Wii U ports on a 4:3 television.
        - IntegerScale.ExceedBoundsBy                   - Offset the max screen bounds, usually by +1.
                                                          This isn't that useful at the moment, so it's unused here.
*/

namespace AdvancedResolutionSettings {
enum setting { UPDATE_aspectRatioX, UPDATE_aspectRatioY, UPDATE_verticalPixelCount };

const char* aspectRatioPresetLabels[] = {
    "Off", "Custom", "Original (4:3)", "Widescreen (16:9)", "Nintendo 3DS (5:3)", "16:10 (8:5)", "Ultrawide (21:9)"
};
const float aspectRatioPresetsX[] = { 0.0f, 16.0f, 4.0f, 16.0f, 5.0f, 16.0f, 21.0f };
const float aspectRatioPresetsY[] = { 0.0f,  9.0f, 3.0f,  9.0f, 3.0f, 10.0f,  9.0f };
const int default_aspectRatio = 1; // Default combo list option

const char* pixelCountPresetLabels[] = { "Custom",     "Native N64 (240p)", "2x (480p)",       "3x (720p)", "4x (960p)",
                                         "5x (1200p)", "6x (1440p)",        "Full HD (1080p)", "4K (2160p)" };
const int pixelCountPresets[] = { 480, 240, 480, 720, 960, 1200, 1440, 1080, 2160 };
const int default_pixelCount = 0; // Default combo list option

// Resolution clamp values as defined in LUS::Gui::ApplyResolutionChanges()
const uint32_t minVerticalPixelCount = SCREEN_HEIGHT;
const uint32_t maxVerticalPixelCount = 4320; // 18x native, or 8K TV resolution

const unsigned short default_maxIntegerScaleFactor = 6; // Default size of Integer scale factor slider.

enum messageType { MESSAGE_ERROR, MESSAGE_WARNING, MESSAGE_QUESTION, MESSAGE_INFO, MESSAGE_GRAY_75 };
const ImVec4 messageColor[]{
    { 0.85f, 0.0f, 0.0f, 1.0f },  // MESSAGE_ERROR
    { 0.85f, 0.85f, 0.0f, 1.0f }, // MESSAGE_WARNING
    { 0.0f, 0.85f, 0.85f, 1.0f }, // MESSAGE_QUESTION
    { 0.0f, 0.85f, 0.55f, 1.0f }, // MESSAGE_INFO
    { 0.75f, 0.75f, 0.75f, 1.0f } // MESSAGE_GRAY_75
};
const float enhancementSpacerHeight = 19.0f;

void AdvancedResolutionSettingsWindow::InitElement() {
}

void AdvancedResolutionSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(497, 599), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Advanced Resolution Settings", &mIsVisible)) {
        // Initialise update flags.
        bool update[sizeof(setting)];
        for (unsigned short i = 0; i < sizeof(setting); i++)
            update[i] = false;
        static short updateCountdown = 0;
        short countdownStartingValue = CVarGetInteger("gInterpolationFPS", 20) / 2; // half of a second, in frames.

        // Initialise integer scale bounds.
        short max_integerScaleFactor = default_maxIntegerScaleFactor; // default value, which may or may not get
                                                                      // overridden depending on viewport res

        short integerScale_maximumBounds = 1; // can change when window is resized
        // This is mostly just for UX purposes, as Fit Automatically logic is part of LUS.
        if (((float)gfx_current_game_window_viewport.width / gfx_current_game_window_viewport.height) >
            ((float)gfx_current_dimensions.width / gfx_current_dimensions.height)) {
            // Scale to window height
            integerScale_maximumBounds = gfx_current_game_window_viewport.height / gfx_current_dimensions.height;
        } else {
            // Scale to window width
            integerScale_maximumBounds = gfx_current_game_window_viewport.width / gfx_current_dimensions.width;
        }
        // Lower-clamping maximum bounds value to 1 is no-longer necessary as that's accounted for in LUS.
        // Letting it go below 1 in this Editor will even allow for checking if screen bounds are being exceeded.
        if (default_maxIntegerScaleFactor < integerScale_maximumBounds) {
            max_integerScaleFactor =
                integerScale_maximumBounds + CVarGetInteger("gAdvancedResolution.IntegerScale.ExceedBoundsBy", 0);
        }

        // Combo List defaults
        static int item_aspectRatio = CVarGetInteger("gAdvancedResolution.UIComboItem.AspectRatio", 3);
        static int item_pixelCount = CVarGetInteger("gAdvancedResolution.UIComboItem.PixelCount", default_pixelCount);
        // Stored Values for non-UIWidgets elements
        static float aspectRatioX =
            CVarGetFloat("gAdvancedResolution.AspectRatioX", aspectRatioPresetsX[item_aspectRatio]);
        static float aspectRatioY =
            CVarGetFloat("gAdvancedResolution.AspectRatioY", aspectRatioPresetsY[item_aspectRatio]);
        static int verticalPixelCount =
            CVarGetInteger("gAdvancedResolution.VerticalPixelCount", pixelCountPresets[item_pixelCount]);
        // Additional settings
        static bool showHorizontalResField = false;
        static int horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;

#ifdef __APPLE__
        ImGui::Text("Note: these settings may behave incorrectly on Apple Retina Displays.");
        UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
#endif

        if (ImGui::CollapsingHeader("Original Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            // The original resolution slider (for convenience)
            const bool cantUseResolutionSlider = (CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0) &&
                                                  CVarGetInteger("gAdvancedResolution.Enabled", 0)) ||
                                                 CVarGetInteger("gLowResMode", 0);
            if (UIWidgets::EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f,
                                                  2.0f, "", 1.0f, true, true, cantUseResolutionSlider)) {
                LUS::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                    CVarGetFloat("gInternalResolution", 1));
            }
            UIWidgets::Tooltip("Multiplies your output resolution by the value entered.");

            // The original MSAA slider (also for convenience)
#ifndef __WIIU__
            if (UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, true, true,
                                                      false)) {
                LUS::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger("gMSAAValue", 1));
            };
            UIWidgets::Tooltip(
                "Activates multi-sample anti-aliasing when above 1x, up to 8x for 8 samples for every pixel.\n\n"
                " " ICON_FA_INFO_CIRCLE
                " Higher MSAA with low resolution can resemble an authentic \"real N64\" look!");
#endif

            // N64 Mode toggle (again for convenience)
            // UIWidgets::PaddedEnhancementCheckbox("(Enhancements>Graphics) N64 Mode", "gLowResMode", false, false, false, "", UIWidgets::CheckboxGraphics::Cross, false);
        }

        UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
        // Activator
        UIWidgets::PaddedEnhancementCheckbox("Enable advanced settings.", "gAdvancedResolution.Enabled", false, false,
                                             false, "", UIWidgets::CheckboxGraphics::Cross, false);
        // Error/Warning display
        if (!CVarGetInteger("gLowResMode", 0)) {
            if (IsDroppingFrames()) { // Significant frame drop warning
                ImGui::TextColored(messageColor[MESSAGE_WARNING],
                                   ICON_FA_EXCLAMATION_TRIANGLE " Significant frame rate (FPS) drops may be occuring.");
                UIWidgets::Spacer(2);
            } else { // No warnings
                UIWidgets::Spacer(enhancementSpacerHeight);
            }
        } else { // N64 Mode warning
            ImGui::TextColored(messageColor[MESSAGE_QUESTION],
                               ICON_FA_QUESTION_CIRCLE " \"N64 Mode\" is overriding these settings.");
            ImGui::SameLine();
            if (ImGui::Button("Click to disable")) {
                CVarSetInteger("gLowResMode", (int)false);
                CVarSave();
            }
        }
        // Resolution visualiser
        ImGui::Text("Viewport dimensions: %d x %d", gfx_current_game_window_viewport.width,
                    gfx_current_game_window_viewport.height);
        ImGui::Text("Internal resolution: %d x %d", gfx_current_dimensions.width, gfx_current_dimensions.height);

        UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

        // Aspect Ratio
        ImGui::Text("Force aspect ratio:");
        ImGui::SameLine();
        ImGui::TextColored(messageColor[MESSAGE_GRAY_75], "(Select \"Off\" to disable.)");
        // Presets
        if (ImGui::Combo(" ", &item_aspectRatio, aspectRatioPresetLabels,
                         IM_ARRAYSIZE(aspectRatioPresetLabels)) &&
            item_aspectRatio != default_aspectRatio) { // don't change anything if "Custom" is selected.
            aspectRatioX = aspectRatioPresetsX[item_aspectRatio];
            aspectRatioY = aspectRatioPresetsY[item_aspectRatio];
            update[UPDATE_aspectRatioX] = true;
            update[UPDATE_aspectRatioY] = true;

            if (showHorizontalResField) {
                horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
            }

            CVarSetInteger("gAdvancedResolution.UIComboItem.AspectRatio", item_aspectRatio);
            CVarSave();
        }
        // Hide aspect ratio input fields if using one of the presets.
        if (item_aspectRatio == default_aspectRatio && !showHorizontalResField) {
            // Declaring the Y input interaction in particular as a variable beforehand
            // will prevent a bug where the Y field would disappear when modifying X.
            bool inputX = ImGui::InputFloat("X", &aspectRatioX, 0.1f, 1.0f, "%.3f");
            bool inputY = ImGui::InputFloat("Y", &aspectRatioY, 0.1f, 1.0f, "%.3f");
            if (inputX || inputY) {
                item_aspectRatio = default_aspectRatio;
                update[UPDATE_aspectRatioX] = true;
                update[UPDATE_aspectRatioY] = true;
            }
        } else if (showHorizontalResField) { // Show calculated aspect ratio
            if (item_aspectRatio) {
                UIWidgets::Spacer(2);
                float resolvedAspectRatio = (float)gfx_current_dimensions.height / gfx_current_dimensions.width;
                ImGui::Text("Aspect ratio: %.4f", resolvedAspectRatio);
            } else {
                UIWidgets::Spacer(enhancementSpacerHeight);
            }
        }

        UIWidgets::Spacer(0);
        // Vertical Resolution
        UIWidgets::PaddedEnhancementCheckbox("Set fixed vertical resolution (disables Resolution slider)",
                                             "gAdvancedResolution.VerticalResolutionToggle", true, false, false, "",
                                             UIWidgets::CheckboxGraphics::Cross, false);
        UIWidgets::Tooltip(
            "Override the resolution scale slider and use the settings below, irrespective of window size.");
        if (ImGui::Combo("Pixel Count Presets", &item_pixelCount, pixelCountPresetLabels,
                         IM_ARRAYSIZE(pixelCountPresetLabels)) &&
            item_pixelCount != default_pixelCount) { // don't change anything if "Custom" is selected.
            verticalPixelCount = pixelCountPresets[item_pixelCount];
            update[UPDATE_verticalPixelCount] = true;

            if (showHorizontalResField) {
                horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
            }

            CVarSetInteger("gAdvancedResolution.UIComboItem.PixelCount", item_pixelCount);
            CVarSave();
        }
        // Horizontal Resolution, if visibility is enabled for it.
        if (showHorizontalResField) {
            // Only show the field if Aspect Ratio is being enforced.
            if ((aspectRatioX > 0.0f) && (aspectRatioY > 0.0f)) {
                // So basically we're "faking" this one by setting aspectRatioX instead.
                if (ImGui::InputInt("Horiz. Pixel Count", &horizontalPixelCount, 8, 320)) {
                    item_aspectRatio = default_aspectRatio;
                    if (horizontalPixelCount < SCREEN_WIDTH) {
                        horizontalPixelCount = (minVerticalPixelCount / 3.0f) * 4.0f;
                    }
                    aspectRatioX = aspectRatioY * horizontalPixelCount / verticalPixelCount;
                    update[UPDATE_aspectRatioX] = true;
                }
            } else { // Display a notice instead.
                ImGui::TextColored(messageColor[MESSAGE_QUESTION],
                                   ICON_FA_QUESTION_CIRCLE " \"Force aspect ratio\" required.");
                // ImGui::Text(" ");
                ImGui::SameLine();
                if (ImGui::Button("Click to resolve")) {
                    item_aspectRatio = default_aspectRatio; // Set it to Custom
                    aspectRatioX = aspectRatioPresetsX[2];  // but use the 4:3 defaults
                    aspectRatioY = aspectRatioPresetsY[2];
                    update[UPDATE_aspectRatioX] = true;
                    update[UPDATE_aspectRatioY] = true;
                    horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
                }
            }
        }
        // Vertical Resolution part 2
        if (ImGui::InputInt("Vertical Pixel Count", &verticalPixelCount, 8, 240)) {
            item_pixelCount = default_pixelCount;
            update[UPDATE_verticalPixelCount] = true;

            // Account for the natural instinct to enter horizontal first.
            // Ignore vertical resolutions that are below the lower clamp constant.
            if (showHorizontalResField && !(verticalPixelCount < minVerticalPixelCount)) {
                aspectRatioX = aspectRatioY * horizontalPixelCount / verticalPixelCount;
                update[UPDATE_aspectRatioX] = true;
            }
        }

        UIWidgets::Spacer(0);

        // Integer scaling settings group
        static const ImGuiTreeNodeFlags IntegerScalingResolvedImGuiFlag =
            CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0) ? ImGuiTreeNodeFlags_DefaultOpen
                                                                      : ImGuiTreeNodeFlags_None;
        if (ImGui::CollapsingHeader("Integer Scaling Settings", IntegerScalingResolvedImGuiFlag)) {
            // Pixel-perfect Mode
            UIWidgets::PaddedEnhancementCheckbox("Pixel-perfect Mode", "gAdvancedResolution.PixelPerfectMode", true,
                                                 true,
                                                 !CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0), "",
                                                 UIWidgets::CheckboxGraphics::Cross, false);
            UIWidgets::Tooltip("Don't scale image to fill window.");
            if (!CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0)) {
                CVarSetInteger("gAdvancedResolution.PixelPerfectMode", (int)false);
                CVarSave();
            }

            // Integer Scaling
            UIWidgets::EnhancementSliderInt("Integer scale factor: %d", "##ARSIntScale",
                                            "gAdvancedResolution.IntegerScale.Factor", 1, max_integerScaleFactor, "%d",
                                            1, true,
                                            !CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0) ||
                                                CVarGetInteger("gAdvancedResolution.IntegerScale.FitAutomatically", 0));
            UIWidgets::Tooltip("Integer scales the image. Only available in pixel-perfect mode.");
            // Display warning if size is being clamped or if framebuffer is larger than viewport.
            if (CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0) &&
                (CVarGetInteger("gAdvancedResolution.IntegerScale.NeverExceedBounds", 1) &&
                 CVarGetInteger("gAdvancedResolution.IntegerScale.Factor", 1) > integerScale_maximumBounds)) {
                ImGui::SameLine();
                ImGui::TextColored(messageColor[MESSAGE_WARNING], ICON_FA_EXCLAMATION_TRIANGLE " Window exceeded.");
            }

            UIWidgets::PaddedEnhancementCheckbox("Automatically scale image to fit viewport",
                                                 "gAdvancedResolution.IntegerScale.FitAutomatically", true, true,
                                                 !CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0), "",
                                                 UIWidgets::CheckboxGraphics::Cross, false);
            UIWidgets::Tooltip("Automatically sets scale factor to fit window. Only available in pixel-perfect mode.");
            if (CVarGetInteger("gAdvancedResolution.IntegerScale.FitAutomatically", 0)) {
                // This is just here to update the value shown on the slider.
                // The function in LUS to handle this setting will ignore IntegerScaleFactor while active.
                CVarSetInteger("gAdvancedResolution.IntegerScale.Factor", integerScale_maximumBounds);
                // CVarSave();
            }
        } // End of integer scaling settings

        UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

        // Collapsible panel for additional settings
        if (ImGui::CollapsingHeader("Additional Settings")) {
            UIWidgets::Spacer(0);
#if defined(__SWITCH__) || defined(__WIIU__)
            // Disable aspect correction, stretching the framebuffer to fill the viewport.
            // This option is only really needed on systems limited to 16:9 TV resolutions, such as consoles.
            // The associated CVar is still functional on PC platforms if you want to use it though.
            UIWidgets::PaddedEnhancementCheckbox("Disable aspect correction and stretch the output image.\n"
                                                 "(Might be useful for 4:3 televisions!)\n"
                                                 "Not available in Pixel Perfect Mode.",
                                                 "gAdvancedResolution.IgnoreAspectCorrection", false, true,
                                                 CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0), "",
                                                 UIWidgets::CheckboxGraphics::Cross, false);
#else
            if (CVarGetInteger("gAdvancedResolution.IgnoreAspectCorrection", 0)) {
                // This setting is intentionally not exposed on PC platforms,
                // but may be accidentally activated for varying reasons.
                // Having this button should hopefully prevent support headaches.
                ImGui::TextColored(messageColor[MESSAGE_QUESTION], ICON_FA_QUESTION_CIRCLE
                                   " If the image is stretched and you don't know why, click this.");
                if (ImGui::Button("Click to reenable aspect correction.")) {
                    CVarSetInteger("gAdvancedResolution.IgnoreAspectCorrection", (int)false);
                    CVarSave();
                }
                UIWidgets::Spacer(2);
            }
#endif

            // A requested addiition; an alternative way of displaying the resolution field.
            if (ImGui::Checkbox("Show a horizontal resolution field.", &showHorizontalResField)) {
                if (!showHorizontalResField && (aspectRatioX > 0.0f)) { // when turning this setting off
                    // Refresh relevant values
                    aspectRatioX = aspectRatioY * horizontalPixelCount / verticalPixelCount;
                    horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
                } else { // when turning this setting on
                    item_aspectRatio = default_aspectRatio;
                    if (aspectRatioX > 0.0f) {
                        // Refresh relevant values in the opposite order
                        horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
                        aspectRatioX = aspectRatioY * horizontalPixelCount / verticalPixelCount;
                    }
                }
                update[UPDATE_aspectRatioX] = true;
            }

            UIWidgets::PaddedEnhancementCheckbox(
                "Prevent integer scaling from exceeding screen bounds.\n"
                "(Makes screen bounds take priority over specified factor.)",
                "gAdvancedResolution.IntegerScale.NeverExceedBounds", true, false,
                !CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0) ||
                    CVarGetInteger("gAdvancedResolution.IntegerScale.FitAutomatically", 0),
                "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Prevents integer scaling factor from exceeding screen bounds.\n\n"
                "Enabled: Will clamp the scaling factor and display a gentle warning in the resolution editor.\n"
                "Disabled: Will allow scaling factor to exceed screen bounds, for users who want to crop overscan.\n\n"
                " " ICON_FA_INFO_CIRCLE " Please note that exceeding screen bounds may show a scroll bar on-screen.");

            if (!CVarGetInteger("gAdvancedResolution.IntegerScale.NeverExceedBounds", 1) ||
                CVarGetInteger("gAdvancedResolution.IntegerScale.ExceedBoundsBy", 0)) {
                ImGui::TextColored(messageColor[MESSAGE_INFO],
                                   " " ICON_FA_INFO_CIRCLE
                                   " A scroll bar may become visible if screen bounds are exceeded.");
                // Another helpful button, to disable the unused "Exceed Bounds" cvar.
                if (CVarGetInteger("gAdvancedResolution.IntegerScale.ExceedBoundsBy", 0)) {
                    if (ImGui::Button("Click to reset a console variable that may be causing this.")) {
                        CVarSetInteger("gAdvancedResolution.IntegerScale.ExceedBoundsBy", 0);
                        CVarSave();
                    }
                }
            } else {
                UIWidgets::Spacer(enhancementSpacerHeight);
            }

            // A popular feature in some emulators/frontends/upscalers, sometimes called "crop overscan" or "1080p 5x".
            // I've ended up dummying this one out because it doesn't function in a satisfactory way.
            // Consider this idea on the table, but I don't deem it an important enough feature to push for.
            /*
            UIWidgets::PaddedEnhancementCheckbox("Allow integer scale factor to go +1 above maximum screen bounds.",
                                                 "gAdvancedResolution.IntegerScale.ExceedBoundsBy", false, false,
                                                 !CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0), "",
                                                 UIWidgets::CheckboxGraphics::Cross, false);
            if (CVarGetInteger("gAdvancedResolution.IntegerScale.ExceedBoundsBy", 0)) {
                ImGui::TextColored(messageColor[MESSAGE_INFO],
                                   " " ICON_FA_INFO_CIRCLE
                                   " A scroll bar may become visible if screen bounds are exceeded.");
            }*/

        } // End of additional settings

        // Clamp and update the CVars that don't use UIWidgets
        if (IsBoolArrayTrue(update)) {
            if (update[UPDATE_aspectRatioX]) {
                if (aspectRatioX < 0.0f) {
                    aspectRatioX = 0.0f;
                }
                CVarSetFloat("gAdvancedResolution.AspectRatioX", aspectRatioX);
            }
            if (update[UPDATE_aspectRatioY]) {
                if (aspectRatioY < 0.0f) {
                    aspectRatioY = 0.0f;
                }
                CVarSetFloat("gAdvancedResolution.AspectRatioY", aspectRatioY);
            }
            if (update[UPDATE_verticalPixelCount]) {
                // There's a upper and lower clamp on the Libultraship side too,
                // so clamping it here is purely visual, so the vertical resolution field reflects it.
                if (verticalPixelCount < minVerticalPixelCount) {
                    verticalPixelCount = minVerticalPixelCount;
                }
                if (verticalPixelCount > maxVerticalPixelCount) {
                    verticalPixelCount = maxVerticalPixelCount;
                }
                CVarSetInteger("gAdvancedResolution.VerticalPixelCount", verticalPixelCount);
            }
            // Delay saving this set of CVars by a predetermined length of time, in frames.
            updateCountdown = countdownStartingValue;
        }
        if (updateCountdown > 0) {
            updateCountdown--;
        } else {
            CVarSetInteger("gAdvancedResolution.UIComboItem.AspectRatio", item_aspectRatio);
            CVarSetInteger("gAdvancedResolution.UIComboItem.PixelCount", item_pixelCount);
            CVarSave();
        }
    }
    ImGui::End();
}

void AdvancedResolutionSettingsWindow::UpdateElement() {
}

bool AdvancedResolutionSettingsWindow::IsDroppingFrames() {
    // a rather imprecise way of checking for frame drops.
    // but it's mostly there to inform the player of large drops.
    const short targetFPS = CVarGetInteger("gInterpolationFPS", 20);
    const float threshold = targetFPS / 20.0f + 4.1f;
    return ImGui::GetIO().Framerate < targetFPS - threshold;
}

bool AdvancedResolutionSettingsWindow::IsBoolArrayTrue(bool* foo) {
    for (unsigned short i = 0; i < sizeof(&foo); i++)
        if (&foo[i])
            return true;
    return false;
}
} // namespace AdvancedResolutionSettings