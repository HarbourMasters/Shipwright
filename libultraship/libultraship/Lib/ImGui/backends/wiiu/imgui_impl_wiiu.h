// dear imgui: Platform Backend for the Wii U
#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

// GamePad Input
#include <vpad/input.h>
// Controller Input
#include <padscore/kpad.h>

struct ImGui_ImplWiiU_ControllerInput
{
   VPADStatus* vpad = nullptr;
   KPADStatus* kpad[4] = { nullptr };
};

enum ImGui_ImplWiiU_KeyboardOverlayType
{
   //! Draw for the DRC
   ImGui_KeyboardOverlay_DRC,
   //! Draw for the TV
   ImGui_KeyboardOverlay_TV,
   //! Draw for the controller which requested the keyboard
   ImGui_KeyboardOverlay_Auto
};

IMGUI_IMPL_API bool     ImGui_ImplWiiU_Init();
IMGUI_IMPL_API void     ImGui_ImplWiiU_Shutdown();
IMGUI_IMPL_API bool     ImGui_ImplWiiU_ProcessInput(ImGui_ImplWiiU_ControllerInput* input);
IMGUI_IMPL_API void     ImGui_ImplWiiU_DrawKeyboardOverlay(ImGui_ImplWiiU_KeyboardOverlayType type = ImGui_KeyboardOverlay_Auto);
