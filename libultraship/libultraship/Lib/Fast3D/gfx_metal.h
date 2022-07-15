#ifdef ENABLE_METAL

#ifndef GFX_METAL_H
#define GFX_METAL_H

#include "gfx_rendering_api.h"
#include "Lib/ImGui/backends/imgui_impl_sdl.h"

extern struct GfxRenderingAPI gfx_metal_api;

void Metal_SetRenderer(SDL_Renderer* renderer);

bool Metal_Init();
void Metal_NewFrame();
void Metal_RenderDrawData(ImDrawData* draw_data);

#endif
#endif