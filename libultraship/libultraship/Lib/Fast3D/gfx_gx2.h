/*  gfx_gx2.h - Fast3D GX2 backend for libultraship

    Created in 2022 by GaryOderNichts
*/
#ifndef GFX_GX2_H
#define GFX_GX2_H

#include "gfx_rendering_api.h"

void gfx_gx2_shutdown(void);

struct ImGui_ImplGX2_Texture* gfx_gx2_texture_for_imgui(uint32_t texture_id);

extern struct GfxRenderingAPI gfx_gx2_api;

#endif
