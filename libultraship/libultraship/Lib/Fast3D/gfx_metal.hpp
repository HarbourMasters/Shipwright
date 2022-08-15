#ifdef ENABLE_METAL

#ifndef gfx_metal_hpp
#define gfx_metal_hpp

#include "gfx_rendering_api.h"
#include "Lib/ImGui/backends/imgui_impl_sdl.h"

extern struct GfxRenderingAPI gfx_metal_api;

ImTextureID gfx_metal_get_texture_by_id(int id);

void Metal_SetRenderer(SDL_Renderer* renderer);

bool Metal_Init();
void Metal_NewFrame();
void Metal_RenderDrawData(ImDrawData* draw_data);

#endif
#endif
