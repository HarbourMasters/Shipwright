#include "ResolutionEditor.h"
#include <imgui.h>
#include <libultraship/libultraship.h>

#include <soh/UIWidgets.hpp>
#include <graphic/Fast3D/gfx_pc.h>
#include "soh/OTRGlobals.h"

/*  Console Variables are grouped under gAdvancedResolution. (e.g. CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled")

    The following cvars are used in Libultraship and can be edited here:
        - Enabled                                       - Turns Advanced Resolution Mode on.
        - AspectRatioX, AspectRatioY                    - Aspect ratio controls. To toggle off, set either to zero.
        - VerticalPixelCount, VerticalResolutionToggle  - Resolution controls.
        - PixelPerfectMode, IntegerScale.Factor         - Pixel Perfect Mode a.k.a. integer scaling controls.
        - IntegerScale.FitAutomatically                 - Automatic resizing for Pixel Perfect Mode.
        - IntegerScale.NeverExceedBounds                - Prevents manual resizing from exceeding screen bounds.

    The following cvars are also implemented in LUS for niche use cases:
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

// Resolution clamp values as hardcoded in LUS::Gui::ApplyResolutionChanges()
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
    // Initialise update flags.
    bool update[3];
    for (uint8_t i = 0; i < sizeof(update); i++)
        update[i] = false;

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
            integerScale_maximumBounds + CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0);
    }

    // Combo List defaults
    static int item_aspectRatio = CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.AspectRatio", 3);
    static int item_pixelCount = CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.PixelCount", default_pixelCount);
    // Stored Values for non-UIWidgets elements
    static float aspectRatioX =
        CVarGetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioX", aspectRatioPresetsX[item_aspectRatio]);
    static float aspectRatioY =
        CVarGetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioY", aspectRatioPresetsY[item_aspectRatio]);
    static int verticalPixelCount =
        CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalPixelCount", pixelCountPresets[item_pixelCount]);
    // Additional settings
    static bool showHorizontalResField = false;
    static int horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
    // Disabling flags
    const bool disabled_everything = !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", 0);
    const bool disabled_pixelCount = !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", 0);

#ifdef __APPLE__
    // Display HiDPI warning. (Remove this once we can definitively say it's fixed.)
    ImGui::TextColored(messageColor[MESSAGE_INFO],
                        ICON_FA_INFO_CIRCLE " These settings may behave incorrectly on Retina displays.");
    UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
#endif

    if (ImGui::CollapsingHeader("Original Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        // The original resolution slider (for convenience)
        const bool disabled_resolutionSlider = (CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", 0) &&
                                                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", 0)) ||
                                                CVarGetInteger(CVAR_LOW_RES_MODE, 0);
        if (UIWidgets::EnhancementSliderFloat("Internal Resolution: %.1f%%", "##IMul", CVAR_INTERNAL_RESOLUTION, 0.5f,
                                                2.0f, "", 1.0f, true, true, disabled_resolutionSlider)) {
            Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        }
        UIWidgets::Tooltip("Multiplies your output resolution by the value entered.");

        // The original MSAA slider (also for convenience)
#ifndef __WIIU__
        if (UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", CVAR_MSAA_VALUE, 1, 8, "", 1, true, true,
                                                    false)) {
            Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
        };
        UIWidgets::Tooltip(
            "Activates multi-sample anti-aliasing when above 1x, up to 8x for 8 samples for every pixel.\n\n"
            " " ICON_FA_INFO_CIRCLE
            " (Higher MSAA with low resolution can approximate an authentic \"real N64\" look!)");
#endif

        // N64 Mode toggle (again for convenience)
        // UIWidgets::PaddedEnhancementCheckbox("(Enhancements>Graphics) N64 Mode", CVAR_LOW_RES_MODE, false, false, false, "", UIWidgets::CheckboxGraphics::Cross, false);
    }

    UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
    // Activator
    UIWidgets::PaddedEnhancementCheckbox("Enable advanced settings.", CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", false, false,
                                            false, "", UIWidgets::CheckboxGraphics::Cross, false);
    // Error/Warning display
    if (!CVarGetInteger(CVAR_LOW_RES_MODE, 0)) {
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
            CVarSetInteger(CVAR_LOW_RES_MODE, 0);
            CVarSave();
        }
    }
    // Resolution visualiser
    ImGui::Text("Viewport dimensions: %d x %d", gfx_current_game_window_viewport.width,
                gfx_current_game_window_viewport.height);
    ImGui::Text("Internal resolution: %d x %d", gfx_current_dimensions.width, gfx_current_dimensions.height);

    UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
    if (disabled_everything) { // Hide aspect ratio controls.
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }

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

        if (showHorizontalResField) {
            horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
        }

        CVarSetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioX", aspectRatioX);
        CVarSetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioY", aspectRatioY);
        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.AspectRatio", item_aspectRatio);
        CVarSave();
    }
    // Hide aspect ratio input fields if using one of the presets.
    if (item_aspectRatio == default_aspectRatio && !showHorizontalResField) {
        // Declare input interaction bools outside of IF statement to prevent Y field from disappearing.
        const bool input_X = ImGui::InputFloat("X", &aspectRatioX, 0.1f, 1.0f, "%.3f");
        const bool input_Y = ImGui::InputFloat("Y", &aspectRatioY, 0.1f, 1.0f, "%.3f");
        if (input_X || input_Y) {
            item_aspectRatio = default_aspectRatio;
            update[UPDATE_aspectRatioX] = true;
            update[UPDATE_aspectRatioY] = true;
        }
    } else if (showHorizontalResField) { // Show calculated aspect ratio
        if (item_aspectRatio) {
            UIWidgets::Spacer(2);
            const float resolvedAspectRatio = (float)gfx_current_dimensions.width / gfx_current_dimensions.height;
            ImGui::Text("Aspect ratio: %.2f:1", resolvedAspectRatio);
        } else {
            UIWidgets::Spacer(enhancementSpacerHeight);
        }
    }

    if (disabled_everything) { // Hide aspect ratio controls.
        UIWidgets::ReEnableComponent("disabledTooltipText");
    }
    UIWidgets::Spacer(0);

    // Vertical Resolution
    UIWidgets::PaddedEnhancementCheckbox("Set fixed vertical resolution (disables Resolution slider)",
                                            CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", true, false,
                                            disabled_everything, "", UIWidgets::CheckboxGraphics::Cross, false);
    UIWidgets::Tooltip(
        "Override the resolution scale slider and use the settings below, irrespective of window size.");
    if (disabled_pixelCount || disabled_everything) { // Hide pixel count controls.
        UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
    }
    if (ImGui::Combo("Pixel Count Presets", &item_pixelCount, pixelCountPresetLabels,
                        IM_ARRAYSIZE(pixelCountPresetLabels)) &&
        item_pixelCount != default_pixelCount) { // don't change anything if "Custom" is selected.
        verticalPixelCount = pixelCountPresets[item_pixelCount];

        if (showHorizontalResField) {
            horizontalPixelCount = (verticalPixelCount / aspectRatioY) * aspectRatioX;
        }

        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalPixelCount", verticalPixelCount);
        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.PixelCount", item_pixelCount);
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
                    horizontalPixelCount = SCREEN_WIDTH;
                }
                aspectRatioX = horizontalPixelCount;
                aspectRatioY = verticalPixelCount;
                update[UPDATE_aspectRatioX] = true;
                update[UPDATE_aspectRatioY] = true;
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
            item_aspectRatio = default_aspectRatio;
            aspectRatioX = horizontalPixelCount;
            aspectRatioY = verticalPixelCount;
            update[UPDATE_aspectRatioX] = true;
            update[UPDATE_aspectRatioY] = true;
        }
    }
    if (disabled_pixelCount || disabled_everything) { // Hide pixel count controls.
        UIWidgets::ReEnableComponent("disabledTooltipText");
    }

    UIWidgets::Spacer(0);

    // Integer scaling settings group (Pixel-perfect Mode)
    static const ImGuiTreeNodeFlags IntegerScalingResolvedImGuiFlag =
        CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0) ? ImGuiTreeNodeFlags_DefaultOpen
                                                                    : ImGuiTreeNodeFlags_None;
    if (ImGui::CollapsingHeader("Integer Scaling Settings", IntegerScalingResolvedImGuiFlag)) {
        const bool disabled_pixelPerfectMode =
            !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0) || disabled_everything;
        // Pixel-perfect Mode
        UIWidgets::PaddedEnhancementCheckbox("Pixel-perfect Mode", CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", true,
                                                true, disabled_pixelCount || disabled_everything, "",
                                                UIWidgets::CheckboxGraphics::Cross, false);
        UIWidgets::Tooltip("Don't scale image to fill window.");
        if (disabled_pixelCount && CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0)) {
            CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0);
            CVarSave();
        }

        // Integer Scaling
        UIWidgets::EnhancementSliderInt(
            "Integer scale factor: %d", "##ARSIntScale", CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.Factor", 1,
            max_integerScaleFactor, "%d", 1, true,
            disabled_pixelPerfectMode || CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.FitAutomatically", 0));
        UIWidgets::Tooltip("Integer scales the image. Only available in pixel-perfect mode.");
        // Display warning if size is being clamped or if framebuffer is larger than viewport.
        if (!disabled_pixelPerfectMode &&
            (CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.NeverExceedBounds", 1) &&
                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.Factor", 1) > integerScale_maximumBounds)) {
            ImGui::SameLine();
            ImGui::TextColored(messageColor[MESSAGE_WARNING], ICON_FA_EXCLAMATION_TRIANGLE " Window exceeded.");
        }

        UIWidgets::PaddedEnhancementCheckbox(
            "Automatically scale image to fit viewport", CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.FitAutomatically", true,
            true, disabled_pixelPerfectMode, "", UIWidgets::CheckboxGraphics::Cross, false);
        UIWidgets::Tooltip("Automatically sets scale factor to fit window. Only available in pixel-perfect mode.");
        if (CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.FitAutomatically", 0)) {
            // This is just here to update the value shown on the slider.
            // The function in LUS to handle this setting will ignore IntegerScaleFactor while active.
            CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.Factor", integerScale_maximumBounds);
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
        // The associated cvar is still functional on PC platforms if you want to use it though.
        UIWidgets::PaddedEnhancementCheckbox("Disable aspect correction and stretch the output image.\n"
                                                "(Might be useful for 4:3 televisions!)\n"
                                                "Not available in Pixel Perfect Mode.",
                                                CVAR_PREFIX_ADVANCED_RESOLUTION ".IgnoreAspectCorrection", false, true,
                                                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0) ||
                                                    disabled_everything,
                                                "", UIWidgets::CheckboxGraphics::Cross, false);
#else
        if (CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IgnoreAspectCorrection", 0)) {
            // This setting is intentionally not exposed on PC platforms,
            // but may be accidentally activated for varying reasons.
            // Having this button should hopefully prevent support headaches.
            ImGui::TextColored(messageColor[MESSAGE_QUESTION], ICON_FA_QUESTION_CIRCLE
                                " If the image is stretched and you don't know why, click this.");
            if (ImGui::Button("Click to reenable aspect correction.")) {
                CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IgnoreAspectCorrection", 0);
                CVarSave();
            }
            UIWidgets::Spacer(2);
        }
#endif

        // A requested addition; an alternative way of displaying the resolution field.
        if (ImGui::Checkbox("Show a horizontal resolution field, instead of aspect ratio.", &showHorizontalResField)) {
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
                        
        // Beginning of Integer Scaling additional settings.
        {
            // UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
                
            // Integer Scaling - Never Exceed Bounds.
            const bool disabled_neverExceedBounds =
                !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0) ||
                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.FitAutomatically", 0) || disabled_everything;
            const bool checkbox_neverExceedBounds = 
                UIWidgets::PaddedEnhancementCheckbox("Prevent integer scaling from exceeding screen bounds.\n"
                                                        "(Makes screen bounds take priority over specified factor.)",
                                                        CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.NeverExceedBounds", 
                                                        true, false, disabled_neverExceedBounds, "",
                                                        UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Prevents integer scaling factor from exceeding screen bounds.\n\n"
                "Enabled: Will clamp the scaling factor and display a gentle warning in the resolution editor.\n"
                "Disabled: Will allow scaling to exceed screen bounds, for users who want to crop overscan.\n\n"
                " " ICON_FA_INFO_CIRCLE
                " Please note that exceeding screen bounds may show a scroll bar on-screen.");

            // Initialise the (currently unused) "Exceed Bounds By" cvar if it's been changed.
            if (checkbox_neverExceedBounds &&
                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0)) {
                CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0);
                CVarSave();
            }

            // Integer Scaling - Exceed Bounds By 1x/Offset.
            // A popular feature in some retro frontends/upscalers, sometimes called "crop overscan" or "1080p 5x". 
            /*
            UIWidgets::PaddedEnhancementCheckbox("Allow integer scale factor to go +1 above maximum screen bounds.", CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", false, false, !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".PixelPerfectMode", 0) || disabled_everything, "", UIWidgets::CheckboxGraphics::Cross, false);
            */
            // It does actually function as expected, but exceeding the bottom of the screen shows a scroll bar.
            // I've ended up commenting this one out because of the scroll bar, and for simplicity.

            // Display an info message about the scroll bar.
            if (!CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.NeverExceedBounds", 1) ||
                CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0)) {
                if (disabled_neverExceedBounds) { // Dim this help text accordingly
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }
                ImGui::TextColored(messageColor[MESSAGE_INFO],
                                    " " ICON_FA_INFO_CIRCLE
                                    " A scroll bar may become visible if screen bounds are exceeded.");
                if (disabled_neverExceedBounds) { // Dim this help text accordingly
                    UIWidgets::ReEnableComponent("disabledTooltipText");
                }
                    
                // Another support helper button, to disable the unused "Exceed Bounds By" cvar.
                // (Remove this button if uncommenting the checkbox.)
                if (CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0)) {
                    if (ImGui::Button("Click to reset a console variable that may be causing this.")) {
                        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".IntegerScale.ExceedBoundsBy", 0);
                        CVarSave();
                    }
                }
            } else {
                ImGui::Text(" ");
            }
            // UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
        } // End of Integer Scaling additional settings.

    } // End of additional settings

    // Clamp and update the cvars that don't use UIWidgets
    if (update[UPDATE_aspectRatioX] || update[UPDATE_aspectRatioY] || update[UPDATE_verticalPixelCount]) {
        if (update[UPDATE_aspectRatioX]) {
            if (aspectRatioX < 0.0f) {
                aspectRatioX = 0.0f;
            }
            CVarSetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioX", aspectRatioX);
        }
        if (update[UPDATE_aspectRatioY]) {
            if (aspectRatioY < 0.0f) {
                aspectRatioY = 0.0f;
            }
            CVarSetFloat(CVAR_PREFIX_ADVANCED_RESOLUTION ".AspectRatioY", aspectRatioY);
        }
        if (update[UPDATE_verticalPixelCount]) {
            // There's a upper and lower clamp on the Libultraship side too,
            // so clamping it here is entirely visual, so the vertical resolution field reflects it.
            if (verticalPixelCount < minVerticalPixelCount) {
                verticalPixelCount = minVerticalPixelCount;
            }
            if (verticalPixelCount > maxVerticalPixelCount) {
                verticalPixelCount = maxVerticalPixelCount;
            }
            CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalPixelCount", verticalPixelCount);
        }
        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.AspectRatio", item_aspectRatio);
        CVarSetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".UIComboItem.PixelCount", item_pixelCount);
        CVarSave();
    }
}

void AdvancedResolutionSettingsWindow::UpdateElement() {
}

bool AdvancedResolutionSettingsWindow::IsDroppingFrames() {
    // a rather imprecise way of checking for frame drops.
    // but it's mostly there to inform the player of large drops.
    const short targetFPS = CVarGetInteger(CVAR_SETTING("InterpolationFPS"), 20);
    const float threshold = targetFPS / 20.0f + 4.1f;
    return ImGui::GetIO().Framerate < targetFPS - threshold;
}
} // namespace AdvancedResolutionSettings
