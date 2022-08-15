#ifdef __WIIU__

#include <stdio.h>
#include <time.h>
#include <malloc.h>

#include <coreinit/time.h>
#include <coreinit/thread.h>
#include <coreinit/foreground.h>
#include <coreinit/memory.h>
#include <coreinit/memheap.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/memexpheap.h>
#include <coreinit/memfrmheap.h>

#include <gx2/state.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/swap.h>
#include <gx2/mem.h>
#include <gx2r/mem.h>

#include <whb/proc.h>
#include <proc_ui/procui.h>
#include <proc_ui/memory.h>

#include <vpad/input.h>
#include <padscore/kpad.h>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include "PR/ultra64/gbi.h"

#include "gfx_window_manager_api.h"
#include "gfx_pc.h"
#include "gfx_gx2.h"
#include "gfx_wiiu.h"

#include "Lib/ImGui/backends/wiiu/imgui_impl_wiiu.h"
#include "../../WiiUImpl.h"
#include "../../ImGuiImpl.h"
#include "../../Hooks.h"

static MEMHeapHandle heap_MEM1 = nullptr;
static MEMHeapHandle heap_foreground = nullptr;

bool has_foreground = false;
static void *mem1_storage = nullptr;
static void *command_buffer_pool = nullptr;
GX2ContextState *context_state = nullptr;

static GX2TVRenderMode tv_render_mode;
static void *tv_scan_buffer = nullptr;
static uint32_t tv_scan_buffer_size = 0;
static uint32_t tv_width;
static uint32_t tv_height;

static GX2DrcRenderMode drc_render_mode;
static void *drc_scan_buffer = nullptr;
static uint32_t drc_scan_buffer_size = 0;

static int frame_divisor = 1;

// for ImGui DeltaTime
// (initialized to 1 to not trigger imguis assert on initial draw)
uint32_t frametime = 1;

bool gfx_wiiu_init_mem1(void) {
    MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
    uint32_t size;
    void *base;

    size = MEMGetAllocatableSizeForFrmHeapEx(heap, 4);
    if (!size) {
        printf("%s: MEMGetAllocatableSizeForFrmHeapEx == 0", __FUNCTION__);
        return false;
    }

    base = MEMAllocFromFrmHeapEx(heap, size, 4);
    if (!base) {
        printf("%s: MEMAllocFromFrmHeapEx(heap, 0x%X, 4) failed", __FUNCTION__, size);
        return false;
    }

    heap_MEM1 = MEMCreateExpHeapEx(base, size, 0);
    if (!heap_MEM1) {
        printf("%s: MEMCreateExpHeapEx(%p, 0x%X, 0) failed", __FUNCTION__, base, size);
        return false;
    }

    return true;
}

void gfx_wiiu_destroy_mem1(void) {
    MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);

    if (heap_MEM1) {
        MEMDestroyExpHeap(heap_MEM1);
        heap_MEM1 = NULL;
    }
}

bool gfx_wiiu_init_foreground(void) {
    MEMHeapHandle heap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    uint32_t size;
    void *base;

    size = MEMGetAllocatableSizeForFrmHeapEx(heap, 4);
    if (!size) {
        printf("%s: MEMAllocFromFrmHeapEx(heap, 0x%X, 4)", __FUNCTION__, size);
        return false;
    }

    base = MEMAllocFromFrmHeapEx(heap, size, 4);
    if (!base) {
        printf("%s: MEMGetAllocatableSizeForFrmHeapEx == 0", __FUNCTION__);
        return false;
    }

    heap_foreground = MEMCreateExpHeapEx(base, size, 0);
    if (!heap_foreground) {
        printf("%s: MEMCreateExpHeapEx(%p, 0x%X, 0)", __FUNCTION__, base, size);
        return false;
    }

    return true;
}

void gfx_wiiu_destroy_foreground(void) {
    MEMHeapHandle foreground = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);

    if (heap_foreground) {
        MEMDestroyExpHeap(heap_foreground);
        heap_foreground = NULL;
    }

    MEMFreeToFrmHeap(foreground, MEM_FRM_HEAP_FREE_ALL);
}

void *gfx_wiiu_alloc_mem1(uint32_t size, uint32_t alignment) {
    void *block;

    if (!heap_MEM1) {
        return NULL;
    }

    if (alignment < 4) {
        alignment = 4;
    }

    block = MEMAllocFromExpHeapEx(heap_MEM1, size, alignment);
    return block;
}

void gfx_wiiu_free_mem1(void *block) {
    if (!heap_MEM1) {
        return;
    }

    MEMFreeToExpHeap(heap_MEM1, block);
}

void *gfx_wiiu_alloc_foreground(uint32_t size, uint32_t alignment) {
    void *block;

    if (!heap_foreground) {
        return NULL;
    }

    if (alignment < 4) {
        alignment = 4;
    }

    block = MEMAllocFromExpHeapEx(heap_foreground, size, alignment);
    return block;
}

void gfx_wiiu_free_foreground(void *block) {
    if (!heap_foreground) {
        return;
    }

    MEMFreeToExpHeap(heap_foreground, block);
}

static uint32_t gfx_wiiu_proc_callback_acquired(void *context) {
    has_foreground = true;

    bool result = gfx_wiiu_init_foreground();
    assert(result);

    tv_scan_buffer = gfx_wiiu_alloc_foreground(tv_scan_buffer_size, GX2_SCAN_BUFFER_ALIGNMENT);
    assert(tv_scan_buffer);

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tv_scan_buffer, tv_scan_buffer_size);
    GX2SetTVBuffer(tv_scan_buffer, tv_scan_buffer_size, tv_render_mode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    drc_scan_buffer = gfx_wiiu_alloc_foreground(drc_scan_buffer_size, GX2_SCAN_BUFFER_ALIGNMENT);
    assert(drc_scan_buffer);

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drc_scan_buffer, drc_scan_buffer_size);
    GX2SetDRCBuffer(drc_scan_buffer, drc_scan_buffer_size, drc_render_mode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    return 0;
}

static uint32_t gfx_wiiu_proc_callback_released(void* context) {
    if (tv_scan_buffer) {
        gfx_wiiu_free_foreground(tv_scan_buffer);
        tv_scan_buffer = nullptr;
    }

    if (drc_scan_buffer) {
        gfx_wiiu_free_foreground(drc_scan_buffer);
        drc_scan_buffer = nullptr;
    }

    gfx_wiiu_destroy_foreground();

    has_foreground = false;

    return 0;
}

static void gfx_wiiu_init(const char *game_name, bool start_in_fullscreen, uint32_t width, uint32_t height) {
    WHBProcInit();

    uint32_t mem1_addr, mem1_size;
    OSGetMemBound(OS_MEM1, &mem1_addr, &mem1_size);
    mem1_storage = memalign(0x40, mem1_size);
    assert(mem1_storage);

    ProcUISetMEM1Storage(mem1_storage, mem1_size);

    bool result = gfx_wiiu_init_mem1();
    assert(result);

    command_buffer_pool = memalign(GX2_COMMAND_BUFFER_ALIGNMENT, 0x400000);
    assert(command_buffer_pool);

    uint32_t initAttribs[] = {
        GX2_INIT_CMD_BUF_BASE, (uintptr_t) command_buffer_pool,
        GX2_INIT_CMD_BUF_POOL_SIZE, 0x400000,
        GX2_INIT_ARGC, 0,
        GX2_INIT_ARGV, 0,
        GX2_INIT_END
    };
    GX2Init(initAttribs);

    switch(GX2GetSystemTVScanMode()) {
    case GX2_TV_SCAN_MODE_480I:
    case GX2_TV_SCAN_MODE_480P:
        tv_render_mode = GX2_TV_RENDER_MODE_WIDE_480P;
        tv_width = 854;
        tv_height = 480;
        break;
    case GX2_TV_SCAN_MODE_1080I:
    case GX2_TV_SCAN_MODE_1080P:
        tv_render_mode = GX2_TV_RENDER_MODE_WIDE_1080P;
        tv_width = 1920;
        tv_height = 1080;
        break;
    case GX2_TV_SCAN_MODE_720P:
    default:
        tv_render_mode = GX2_TV_RENDER_MODE_WIDE_720P;
        tv_width = 1280;
        tv_height = 720;
        break;
    }

    drc_render_mode = GX2GetSystemDRCScanMode();

    uint32_t unk;
    GX2CalcTVSize(tv_render_mode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE, &tv_scan_buffer_size, &unk);
    GX2CalcDRCSize(drc_render_mode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE, &drc_scan_buffer_size, &unk);

    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, gfx_wiiu_proc_callback_acquired, nullptr, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, gfx_wiiu_proc_callback_released, nullptr, 100);

    gfx_wiiu_proc_callback_acquired(nullptr);

    context_state = (GX2ContextState *) memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));
    assert(context_state);

    GX2SetupContextStateEx(context_state, TRUE);
    GX2SetContextState(context_state);

    GX2SetTVScale(WIIU_DEFAULT_FB_WIDTH, WIIU_DEFAULT_FB_HEIGHT);
    GX2SetDRCScale(WIIU_DEFAULT_FB_WIDTH, WIIU_DEFAULT_FB_HEIGHT);

    GX2SetSwapInterval(frame_divisor);

    gfx_current_dimensions.width = gfx_current_game_window_viewport.width = WIIU_DEFAULT_FB_WIDTH;
    gfx_current_dimensions.height = gfx_current_game_window_viewport.height = WIIU_DEFAULT_FB_HEIGHT;

    SohImGui::WindowImpl window_impl;
    window_impl.backend = SohImGui::Backend::GX2;
    window_impl.gx2.width = WIIU_DEFAULT_FB_WIDTH;
    window_impl.gx2.height = WIIU_DEFAULT_FB_HEIGHT;
    SohImGui::Init(window_impl);
}

static void gfx_wiiu_shutdown(void) {
    if (has_foreground) {
        gfx_wiiu_proc_callback_released(nullptr);
        gfx_wiiu_destroy_mem1();
    }

    GX2Shutdown();

    if (context_state) {
        free(context_state);
        context_state = nullptr;
    }

    if (command_buffer_pool) {
        free(command_buffer_pool);
        command_buffer_pool = nullptr;
    }

    ProcUISetMEM1Storage(nullptr, 0);
    free(mem1_storage);
}

void gfx_wiiu_set_context_state(void) {
    GX2SetContextState(context_state);
}

static void gfx_wiiu_set_fullscreen_changed_callback(void (*on_fullscreen_changed)(bool is_now_fullscreen)) {
}

static void gfx_wiiu_set_fullscreen(bool enable) {
}

static void gfx_wiiu_show_cursor(bool hide) {
}

static void gfx_wiiu_set_keyboard_callbacks(bool (*on_key_down)(int scancode), bool (*on_key_up)(int scancode), void (*on_all_keys_up)(void)) {
}

static void gfx_wiiu_main_loop(void (*run_one_game_iter)(void)) {
    while (WHBProcIsRunning()) {
        run_one_game_iter();
    }

    Ship::ExecuteHooks<Ship::ExitGame>();
    Ship::WiiU::Exit();

    gfx_gx2_shutdown();
    gfx_wiiu_shutdown();
    WHBProcShutdown();
}

static void gfx_wiiu_get_dimensions(uint32_t *width, uint32_t *height) {
    *width = WIIU_DEFAULT_FB_WIDTH;
    *height = WIIU_DEFAULT_FB_HEIGHT;
}

static void gfx_wiiu_handle_events(void) {
    Ship::WiiU::Update();

    ImGui_ImplWiiU_ControllerInput input{};

    VPADReadError vpad_error;
    input.vpad = Ship::WiiU::GetVPADStatus(&vpad_error);
    if (vpad_error != VPAD_READ_SUCCESS) {
        input.vpad = nullptr;
    }

    KPADError kpad_error;
    for (int i = 0; i < 4; i++) {
        input.kpad[i] = Ship::WiiU::GetKPADStatus((WPADChan) i, &kpad_error);
        if (kpad_error != KPAD_ERROR_OK) {
            input.kpad[i] = nullptr;
        }
    }

    SohImGui::EventImpl event_impl;
    event_impl.gx2.input = &input;
    SohImGui::Update(event_impl);
}

static bool gfx_wiiu_start_frame(void) {
    uint32_t swap_count, flip_count;
    OSTime last_flip, last_vsync;
    uint32_t wait_count = 0;

    while (true) {
        GX2GetSwapStatus(&swap_count, &flip_count, &last_flip, &last_vsync);

        if (flip_count >= swap_count) {
            break;
        }

        if (wait_count >= 10) {
            // GPU timed out, drop frame
            return false;
        }

        wait_count++;
        GX2WaitForVsync();
    }

    return true;
}

static void gfx_wiiu_swap_buffers_begin(void) {
    GX2SwapScanBuffers();
    GX2Flush();

    gfx_wiiu_set_context_state();

    GX2SetTVEnable(TRUE);
    GX2SetDRCEnable(TRUE);
}

static void gfx_wiiu_swap_buffers_end(void) {
    static OSTick tick = 0;
    frametime = OSTicksToMicroseconds(OSGetSystemTick() - tick);
    tick = OSGetSystemTick();
}

static double gfx_wiiu_get_time(void) {
    return 0.0;
}

static void gfx_wiiu_set_target_fps(int fps) {
    // use the nearest divisor
    int divisor = 60 / fps;
    if (divisor < 1) {
        divisor = 1;
    }

    if (frame_divisor != divisor) {
        GX2SetSwapInterval(divisor);
        frame_divisor = divisor;
    }
}

static void gfx_wiiu_set_maximum_frame_latency(int latency) {
}

static float gfx_wiiu_get_detected_hz(void) {
    return 0;
}

struct GfxWindowManagerAPI gfx_wiiu = {
    gfx_wiiu_init,
    gfx_wiiu_set_keyboard_callbacks,
    gfx_wiiu_set_fullscreen_changed_callback,
    gfx_wiiu_set_fullscreen,
    gfx_wiiu_show_cursor,
    gfx_wiiu_main_loop,
    gfx_wiiu_get_dimensions,
    gfx_wiiu_handle_events,
    gfx_wiiu_start_frame,
    gfx_wiiu_swap_buffers_begin,
    gfx_wiiu_swap_buffers_end,
    gfx_wiiu_get_time,
    gfx_wiiu_set_target_fps,
    gfx_wiiu_set_maximum_frame_latency,
    gfx_wiiu_get_detected_hz,
};

#endif
