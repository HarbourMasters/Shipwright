#include "PR/ultra64/gbi.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "Lib/Fast3D/gfx_sdl.h"
#include "Lib/Fast3D/gfx_dxgi.h"
#include "Lib/Fast3D/gfx_glx.h"
#include "Lib/Fast3D/gfx_opengl.h"
#include "Lib/Fast3D/gfx_metal.hpp"
#include "Lib/Fast3D/gfx_direct3d11.h"
#include "Lib/Fast3D/gfx_direct3d12.h"
#include "Lib/Fast3D/gfx_wiiu.h"
#include "Lib/Fast3D/gfx_gx2.h"
#include "Lib/Fast3D/gfx_window_manager_api.h"

#include <string>

/*
 * Begin shims for gfx_pc.cpp. Eventually, a file from SOH repo should be moved in here.
 */

/*
 * End empty shims
 */

void SetWindowManager(struct GfxWindowManagerAPI** WmApi, struct GfxRenderingAPI** RenderingApi, const std::string& gfx_backend, const std::string& gfx_api) {
    // First set default
#if defined(ENABLE_OPENGL) || defined(ENABLE_METAL)
    #ifdef ENABLE_METAL
    *RenderingApi = &gfx_metal_api;
    #else
    *RenderingApi = &gfx_opengl_api;
    #endif
    #if defined(__linux__) && defined(X11_SUPPORTED)
        // LINUX_TODO:
        // *WmApi = &gfx_glx;
        *WmApi = &gfx_sdl;
    #else
        *WmApi = &gfx_sdl;
    #endif
#endif
#ifdef ENABLE_DX12
    *RenderingApi = &gfx_direct3d12_api;
    *WmApi = &gfx_dxgi_api;
#endif
#ifdef ENABLE_DX11
    *RenderingApi = &gfx_direct3d11_api;
    *WmApi = &gfx_dxgi_api;
#endif
#ifdef __WIIU__
    *RenderingApi = &gfx_gx2_api;
    *WmApi = &gfx_wiiu;
#endif

    // Config can override
#ifdef ENABLE_DX11
    if (gfx_backend == "dx11") {
        *RenderingApi = &gfx_direct3d11_api;
        *WmApi = &gfx_dxgi_api;
    }
#endif
#if defined(ENABLE_OPENGL) || defined(ENABLE_METAL)
    if (gfx_backend == "sdl") {
        if (gfx_api == "OpenGL") {
            *RenderingApi = &gfx_opengl_api;
        }
#ifdef ENABLE_METAL
        else {
            *RenderingApi = &gfx_metal_api;
        }
#endif
        *WmApi = &gfx_sdl;
    }
#if defined(__linux__) && defined(X11_SUPPORTED)
    if (gfx_backend == "glx") {
        *RenderingApi = &gfx_opengl_api;
        *WmApi = &gfx_glx;
    }
#endif
#endif
}
