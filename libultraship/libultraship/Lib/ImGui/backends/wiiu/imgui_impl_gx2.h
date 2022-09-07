// dear imgui: Renderer Backend for the Nintendo Wii U using GX2
#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

// GX2 Texture / contains a texture and sampler
// Can be used as a ImTextureID with the GX2 backend
struct ImGui_ImplGX2_Texture
{
    struct GX2Texture* Texture;
    struct GX2Sampler* Sampler;

    ImGui_ImplGX2_Texture() { memset(this, 0, sizeof(*this)); }
};

// Backend API
IMGUI_IMPL_API bool     ImGui_ImplGX2_Init();
IMGUI_IMPL_API void     ImGui_ImplGX2_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplGX2_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplGX2_RenderDrawData(ImDrawData* draw_data);

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplGX2_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplGX2_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplGX2_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplGX2_DestroyDeviceObjects();
