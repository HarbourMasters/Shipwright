#include "PR/ultra64/gbi.h"
#include "Lib/Fast3D/gfx_pc.h"
#include "Lib/Fast3D/gfx_sdl.h"
#include "Lib/Fast3D/gfx_dxgi.h"
#include "Lib/Fast3D/gfx_glx.h"
#include "Lib/Fast3D/gfx_opengl.h"
#include "Lib/Fast3D/gfx_direct3d11.h"
#include "Lib/Fast3D/gfx_direct3d12.h"
#include "Lib/Fast3D/gfx_window_manager_api.h"

#include <string>

/*
 * Begin shims for gfx_pc.cpp. Eventually, a file from SOH repo should be moved in here.
 */

/*
 * End empty shims
 */

void SetWindowManager(struct GfxWindowManagerAPI** WmApi, struct GfxRenderingAPI** RenderingApi, const std::string& gfx_backend) {
    // First set default
#ifdef ENABLE_OPENGL
    *RenderingApi = &gfx_opengl_api;
    #if defined(__linux__)
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

    // Config can override
#ifdef ENABLE_DX11
    if (gfx_backend == "dx11") {
        *RenderingApi = &gfx_direct3d11_api;
        *WmApi = &gfx_dxgi_api;
    }
#endif
#ifdef ENABLE_OPENGL
    if (gfx_backend == "sdl") {
        *RenderingApi = &gfx_opengl_api;
        *WmApi = &gfx_sdl;
    }
#ifdef __linux__
    if (gfx_backend == "glx") {
        *RenderingApi = &gfx_opengl_api;
        *WmApi = &gfx_glx;
    }
#endif
#endif
}