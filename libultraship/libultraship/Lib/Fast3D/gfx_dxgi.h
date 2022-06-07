#ifndef GFX_DXGI_H
#define GFX_DXGI_H

#include "gfx_rendering_api.h"

#ifdef DECLARE_GFX_DXGI_FUNCTIONS

#include <functional>

#include <dxgi1_2.h>

void gfx_dxgi_create_factory_and_device(bool debug, int d3d_version, bool (*create_device_fn)(IDXGIAdapter1 *adapter, bool test_only));
void gfx_dxgi_create_swap_chain(IUnknown *device, std::function<void()>&& before_destroy_fn);
HWND gfx_dxgi_get_h_wnd(void);
IDXGISwapChain1* gfx_dxgi_get_swap_chain();
void ThrowIfFailed(HRESULT res);
void ThrowIfFailed(HRESULT res, HWND h_wnd, const char *message);
#endif

extern struct GfxWindowManagerAPI gfx_dxgi_api;

#endif
