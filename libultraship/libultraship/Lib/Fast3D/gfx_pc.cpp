#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <list>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include <PR/ultra64/types.h>
#include <PR/ultra64/gbi.h>
#include <PR/ultra64/gs2dex.h>
#include <string>
#include <iostream>

#include "../../Cvar.h"

#include "gfx_pc.h"
#include "gfx_cc.h"
#include "gfx_window_manager_api.h"
#include "gfx_rendering_api.h"
#include "gfx_screen_config.h"
#include "../../Hooks.h"

#include "../../luslog.h"
#include "../StrHash64.h"
#include "../../ImGuiImpl.h"
#include "../../GameVersions.h"
#include "../../ResourceMgr.h"
#include "../../Utils.h"


// OTRTODO: fix header files for these
extern "C" {
    const char* ResourceMgr_GetNameByCRC(uint64_t crc);
    int32_t* ResourceMgr_LoadMtxByCRC(uint64_t crc);
    Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
    Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
    char* ResourceMgr_LoadTexByCRC(uint64_t crc);
    void ResourceMgr_RegisterResourcePatch(uint64_t hash, uint32_t instrIndex, uintptr_t origData);
    char* ResourceMgr_LoadTexByName(char* texPath);
    int ResourceMgr_OTRSigCheck(char* imgData);
}

uintptr_t gfxFramebuffer;

using namespace std;

#define SEG_ADDR(seg, addr) (addr | (seg << 24) | 1)

#define SUPPORT_CHECK(x) assert(x)

// SCALE_M_N: upscale/downscale M-bit integer to N-bit
#define SCALE_5_8(VAL_) (((VAL_) * 0xFF) / 0x1F)
#define SCALE_8_5(VAL_) ((((VAL_) + 4) * 0x1F) / 0xFF)
#define SCALE_4_8(VAL_) ((VAL_) * 0x11)
#define SCALE_8_4(VAL_) ((VAL_) / 0x11)
#define SCALE_3_8(VAL_) ((VAL_) * 0x24)
#define SCALE_8_3(VAL_) ((VAL_) / 0x24)

// SCREEN_WIDTH and SCREEN_HEIGHT are defined in the headerfile
#define HALF_SCREEN_WIDTH (SCREEN_WIDTH / 2)
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT / 2)

#define RATIO_X (gfx_current_dimensions.width / (2.0f * HALF_SCREEN_WIDTH))
#define RATIO_Y (gfx_current_dimensions.height / (2.0f * HALF_SCREEN_HEIGHT))

#define MAX_BUFFERED 256
//#define MAX_LIGHTS 2
#define MAX_LIGHTS 32
#define MAX_VERTICES 64

#define TEXTURE_CACHE_MAX_SIZE 500

struct RGBA {
    uint8_t r, g, b, a;
};

struct LoadedVertex {
    float x, y, z, w;
    float u, v;
    struct RGBA color;
    uint8_t clip_rej;
};

static struct {
    TextureCacheMap map;
    list<TextureCacheMapIter> lru;
    vector<uint32_t> free_texture_ids;
} gfx_texture_cache;

struct ColorCombiner {
    uint64_t shader_id0;
    uint32_t shader_id1;
    bool used_textures[2];
    struct ShaderProgram *prg[16];
    uint8_t shader_input_mapping[2][7];
};

static map<uint64_t, struct ColorCombiner> color_combiner_pool;
static map<uint64_t, struct ColorCombiner>::iterator prev_combiner = color_combiner_pool.end();

static struct RSP {
    float modelview_matrix_stack[11][4][4];
    uint8_t modelview_matrix_stack_size;

    float MP_matrix[4][4];
    float P_matrix[4][4];

    Light_t lookat[2];
    Light_t current_lights[MAX_LIGHTS + 1];
    float current_lights_coeffs[MAX_LIGHTS][3];
    float current_lookat_coeffs[2][3]; // lookat_x, lookat_y
    uint8_t current_num_lights; // includes ambient light
    bool lights_changed;

    uint32_t geometry_mode;
    int16_t fog_mul, fog_offset;

    struct {
        // U0.16
        uint16_t s, t;
    } texture_scaling_factor;

    struct LoadedVertex loaded_vertices[MAX_VERTICES + 4];
} rsp;

static struct RDP {
    const uint8_t *palettes[2];
    struct {
        const uint8_t *addr;
        uint8_t siz;
        uint32_t width;
        const char* otr_path;
    } texture_to_load;
    struct {
        const uint8_t *addr;
        uint32_t size_bytes;
        uint32_t full_image_line_size_bytes;
        uint32_t line_size_bytes;
        const char* otr_path;
    } loaded_texture[2];
    struct {
        uint8_t fmt;
        uint8_t siz;
        uint8_t cms, cmt;
        uint8_t shifts, shiftt;
        uint16_t uls, ult, lrs, lrt; // U10.2
        uint16_t tmem; // 0-511, in 64-bit word units
        uint32_t line_size_bytes;
        uint8_t palette;
        uint8_t tmem_index; // 0 or 1 for offset 0 kB or offset 2 kB, respectively
    } texture_tile[8];
    bool textures_changed[2];

    uint8_t first_tile_index;

    uint32_t other_mode_l, other_mode_h;
    uint64_t combine_mode;
    bool     grayscale;

    uint8_t prim_lod_fraction;
    struct RGBA env_color, prim_color, fog_color, fill_color, grayscale_color;
    struct XYWidthHeight viewport, scissor;
    bool viewport_or_scissor_changed;
    void *z_buf_address;
    void *color_image_address;
} rdp;

static struct RenderingState {
    uint8_t depth_test_and_mask; // 1: depth test, 2: depth mask
    bool decal_mode;
    bool alpha_blend;
    struct XYWidthHeight viewport, scissor;
    struct ShaderProgram *shader_program;
    TextureCacheNode *textures[2];
} rendering_state;

struct GfxDimensions gfx_current_window_dimensions;
struct GfxDimensions gfx_current_dimensions;
static struct GfxDimensions gfx_prev_dimensions;
struct XYWidthHeight gfx_current_game_window_viewport;

static bool game_renders_to_framebuffer;
static int game_framebuffer;
static int game_framebuffer_msaa_resolved;

uint32_t gfx_msaa_level = 1;

static bool has_drawn_imgui_menu;

static bool dropped_frame;

static const std::unordered_map<Mtx *, MtxF> *current_mtx_replacements;

static float buf_vbo[MAX_BUFFERED * (32 * 3)]; // 3 vertices in a triangle and 32 floats per vtx
static size_t buf_vbo_len;
static size_t buf_vbo_num_tris;

static struct GfxWindowManagerAPI *gfx_wapi;
static struct GfxRenderingAPI *gfx_rapi;

static int markerOn;
static uintptr_t segmentPointers[16];

struct FBInfo {
    uint32_t orig_width, orig_height;
    uint32_t applied_width, applied_height;
};

static bool fbActive = 0;
static map<int, FBInfo>::iterator active_fb;
static map<int, FBInfo> framebuffers;

static set<pair<float, float>> get_pixel_depth_pending;
static unordered_map<pair<float, float>, uint16_t, hash_pair_ff> get_pixel_depth_cached;

#ifdef _WIN32
// TODO: Properly implement for MSVC
static unsigned long get_time(void)
{
    return 0;
}
#else
#include <time.h>
#include <string>
static unsigned long get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}
#endif

static void gfx_flush(void) {
    if (buf_vbo_len > 0) {
        int num = buf_vbo_num_tris;
        unsigned long t0 = get_time();

        if (markerOn)
        {
            int bp = 0;
        }

        gfx_rapi->draw_triangles(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
        buf_vbo_len = 0;
        buf_vbo_num_tris = 0;
        unsigned long t1 = get_time();
        /*if (t1 - t0 > 1000) {
            printf("f: %d %d\n", num, (int)(t1 - t0));
        }*/
    }
}

static struct ShaderProgram *gfx_lookup_or_create_shader_program(uint64_t shader_id0, uint32_t shader_id1) {
    struct ShaderProgram *prg = gfx_rapi->lookup_shader(shader_id0, shader_id1);
    if (prg == NULL) {
        gfx_rapi->unload_shader(rendering_state.shader_program);
        prg = gfx_rapi->create_and_load_new_shader(shader_id0, shader_id1);
        rendering_state.shader_program = prg;
    }
    return prg;
}

static const char* ccmux_to_string(uint32_t ccmux) {
        static const char* const tbl[] = {
            "G_CCMUX_COMBINED",
            "G_CCMUX_TEXEL0",
            "G_CCMUX_TEXEL1",
            "G_CCMUX_PRIMITIVE",
            "G_CCMUX_SHADE",
            "G_CCMUX_ENVIRONMENT",
            "G_CCMUX_1",
            "G_CCMUX_COMBINED_ALPHA",
            "G_CCMUX_TEXEL0_ALPHA",
            "G_CCMUX_TEXEL1_ALPHA",
            "G_CCMUX_PRIMITIVE_ALPHA",
            "G_CCMUX_SHADE_ALPHA",
            "G_CCMUX_ENV_ALPHA",
            "G_CCMUX_LOD_FRACTION",
            "G_CCMUX_PRIM_LOD_FRAC",
            "G_CCMUX_K5",
     };
        if (ccmux > 15) {
                return "G_CCMUX_0";

    }
    else {
                return tbl[ccmux];

    }

}

static const char* acmux_to_string(uint32_t acmux) {
        static const char* const tbl[] = {
            "G_ACMUX_COMBINED or G_ACMUX_LOD_FRACTION",
            "G_ACMUX_TEXEL0",
            "G_ACMUX_TEXEL1",
            "G_ACMUX_PRIMITIVE",
            "G_ACMUX_SHADE",
            "G_ACMUX_ENVIRONMENT",
            "G_ACMUX_1 or G_ACMUX_PRIM_LOD_FRAC",
            "G_ACMUX_0",
     };
        return tbl[acmux];

}


static void gfx_generate_cc(struct ColorCombiner *comb, uint64_t cc_id) {
    if (markerOn)
    {
        int bp = 0;
    }

    bool is_2cyc = (cc_id & (uint64_t)SHADER_OPT_2CYC << CC_SHADER_OPT_POS) != 0;

    uint8_t c[2][2][4];
    uint64_t shader_id0 = 0;
    uint32_t shader_id1 = (cc_id >> CC_SHADER_OPT_POS);
    uint8_t shader_input_mapping[2][7] = {{0}};
    bool used_textures[2] = {false, false};
    for (int i = 0; i < 2 && (i == 0 || is_2cyc); i++) {
        uint32_t rgb_a = (cc_id >> (i * 28)) & 0xf;
        uint32_t rgb_b = (cc_id >> (i * 28 + 4)) & 0xf;
        uint32_t rgb_c = (cc_id >> (i * 28 + 8)) & 0x1f;
        uint32_t rgb_d = (cc_id >> (i * 28 + 13)) & 7;
        uint32_t alpha_a = (cc_id >> (i * 28 + 16)) & 7;
        uint32_t alpha_b = (cc_id >> (i * 28 + 16 + 3)) & 7;
        uint32_t alpha_c = (cc_id >> (i * 28 + 16 + 6)) & 7;
        uint32_t alpha_d = (cc_id >> (i * 28 + 16 + 9)) & 7;

        if (rgb_a >= 8) rgb_a = G_CCMUX_0;
        if (rgb_b >= 8) rgb_b = G_CCMUX_0;
        if (rgb_c >= 16) rgb_c = G_CCMUX_0;
        if (rgb_d == 7) rgb_d = G_CCMUX_0;

        if (rgb_a == rgb_b || rgb_c == G_CCMUX_0) {
            // Normalize
            rgb_a = G_CCMUX_0;
            rgb_b = G_CCMUX_0;
            rgb_c = G_CCMUX_0;
        }
        if (alpha_a == alpha_b || alpha_c == G_ACMUX_0) {
            // Normalize
            alpha_a = G_ACMUX_0;
            alpha_b = G_ACMUX_0;
            alpha_c = G_ACMUX_0;

        }
        if (i == 1) {
            if (rgb_a != G_CCMUX_COMBINED && rgb_b != G_CCMUX_COMBINED && rgb_c != G_CCMUX_COMBINED && rgb_d != G_CCMUX_COMBINED) {
                // First cycle RGB not used, so clear it away
                c[0][0][0] = c[0][0][1] = c[0][0][2] = c[0][0][3] = G_CCMUX_0;
            }
            if (rgb_c != G_CCMUX_COMBINED_ALPHA && alpha_a != G_ACMUX_COMBINED && alpha_b != G_ACMUX_COMBINED && alpha_d != G_ACMUX_COMBINED)
            {
                // First cycle ALPHA not used, so clear it away
                c[0][1][0] = c[0][1][1] = c[0][1][2] = c[0][1][3] = G_ACMUX_0;
            }
        }

        c[i][0][0] = rgb_a;
        c[i][0][1] = rgb_b;
        c[i][0][2] = rgb_c;
        c[i][0][3] = rgb_d;
        c[i][1][0] = alpha_a;
        c[i][1][1] = alpha_b;
        c[i][1][2] = alpha_c;
        c[i][1][3] = alpha_d;
    }
    if (!is_2cyc) {
        for (int i = 0; i < 2; i++) {
            for (int k = 0; k < 4; k++) {
                c[1][i][k] = i == 0 ? G_CCMUX_0 : G_ACMUX_0;
            }
        }
    }
    {
        uint8_t input_number[32] = { 0 };
        int next_input_number = SHADER_INPUT_1;
        for (int i = 0; i < 2 && (i == 0 || is_2cyc); i++) {
            for (int j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][0][j]) {
                case G_CCMUX_0:
                    val = SHADER_0;
                    break;
                case G_CCMUX_1:
                    val = SHADER_1;
                    break;
                case G_CCMUX_TEXEL0:
                    val = SHADER_TEXEL0;
                    used_textures[0] = true;
                    break;
                case G_CCMUX_TEXEL1:
                    val = SHADER_TEXEL1;
                    used_textures[1] = true;
                    break;
                case G_CCMUX_TEXEL0_ALPHA:
                    val = SHADER_TEXEL0A;
                    used_textures[0] = true;
                    break;
                case G_CCMUX_TEXEL1_ALPHA:
                    val = SHADER_TEXEL1A;
                    used_textures[1] = true;
                    break;
                case G_CCMUX_NOISE:
                    val = SHADER_NOISE;
                    break;
                case G_CCMUX_PRIMITIVE:
                case G_CCMUX_PRIMITIVE_ALPHA:
                case G_CCMUX_PRIM_LOD_FRAC:
                case G_CCMUX_SHADE:
                case G_CCMUX_ENVIRONMENT:
                case G_CCMUX_ENV_ALPHA:
                case G_CCMUX_LOD_FRACTION:
                    if (input_number[c[i][0][j]] == 0) {
                        shader_input_mapping[0][next_input_number - 1] = c[i][0][j];
                        input_number[c[i][0][j]] = next_input_number++;
                    }
                    val = input_number[c[i][0][j]];
                    break;
                case G_CCMUX_COMBINED:
                    val = SHADER_COMBINED;
                    break;
                default:
                    fprintf(stderr, "Unsupported ccmux: %d\n", c[i][0][j]);
                    break;
                }
                shader_id0 |= (uint64_t)val << (i * 32 + j * 4);
            }
        }
    }
    {
        uint8_t input_number[16] = { 0 };
        int next_input_number = SHADER_INPUT_1;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][1][j]) {
                case G_ACMUX_0:
                    val = SHADER_0;
                    break;
                case G_ACMUX_TEXEL0:
                    val = SHADER_TEXEL0;
                    used_textures[0] = true;
                    break;
                case G_ACMUX_TEXEL1:
                    val = SHADER_TEXEL1;
                    used_textures[1] = true;
                    break;
                case G_ACMUX_LOD_FRACTION:
                //case G_ACMUX_COMBINED: same numerical value
                    if (j != 2) {
                        val = SHADER_COMBINED;
                        break;
                    }
                    c[i][1][j] = G_CCMUX_LOD_FRACTION;
                    [[fallthrough]]; // for G_ACMUX_LOD_FRACTION
                case G_ACMUX_1:
                    //case G_ACMUX_PRIM_LOD_FRAC: same numerical value
                    if (j != 2) {
                        val = SHADER_1;
                        break;
                    }
                    [[fallthrough]]; // for G_ACMUX_PRIM_LOD_FRAC
                case G_ACMUX_PRIMITIVE:
                case G_ACMUX_SHADE:
                case G_ACMUX_ENVIRONMENT:
                    if (input_number[c[i][1][j]] == 0) {
                        shader_input_mapping[1][next_input_number - 1] = c[i][1][j];
                        input_number[c[i][1][j]] = next_input_number++;
                    }
                    val = input_number[c[i][1][j]];
                    break;
                }
                shader_id0 |= (uint64_t)val << (i * 32 + 16 + j * 4);
            }
        }
    }
    comb->shader_id0 = shader_id0;
    comb->shader_id1 = shader_id1;
    comb->used_textures[0] = used_textures[0];
    comb->used_textures[1] = used_textures[1];
    //comb->prg = gfx_lookup_or_create_shader_program(shader_id0, shader_id1);
    memcpy(comb->shader_input_mapping, shader_input_mapping, sizeof(shader_input_mapping));
}

static struct ColorCombiner *gfx_lookup_or_create_color_combiner(uint64_t cc_id) {
    if (prev_combiner != color_combiner_pool.end() && prev_combiner->first == cc_id) {
        return &prev_combiner->second;
    }

    prev_combiner = color_combiner_pool.find(cc_id);
    if (prev_combiner != color_combiner_pool.end()) {
        return &prev_combiner->second;
    }
    gfx_flush();
    prev_combiner = color_combiner_pool.insert(make_pair(cc_id, ColorCombiner())).first;
    gfx_generate_cc(&prev_combiner->second, cc_id);
    return &prev_combiner->second;
}

void gfx_texture_cache_clear()
{
    for (const auto& entry : gfx_texture_cache.map) {
        gfx_texture_cache.free_texture_ids.push_back(entry.second.texture_id);
    }
    gfx_texture_cache.map.clear();
    gfx_texture_cache.lru.clear();
}

static bool gfx_texture_cache_lookup(int i, int tile) {
    uint8_t fmt = rdp.texture_tile[tile].fmt;
    uint8_t siz = rdp.texture_tile[tile].siz;
    uint32_t tmem_index = rdp.texture_tile[tile].tmem_index;

    TextureCacheNode** n = &rendering_state.textures[i];
    const uint8_t* orig_addr = rdp.loaded_texture[tmem_index].addr;
    uint8_t palette_index = rdp.texture_tile[tile].palette;

    TextureCacheKey key;
    if (fmt == G_IM_FMT_CI) {
        key = { orig_addr, { rdp.palettes[0], rdp.palettes[1] }, fmt, siz, palette_index };
    } else {
        key = { orig_addr, { }, fmt, siz, palette_index };
    }

    TextureCacheMap::iterator it = gfx_texture_cache.map.find(key);

    if (it != gfx_texture_cache.map.end()) {
        gfx_rapi->select_texture(i, it->second.texture_id);
        *n = &*it;
        gfx_texture_cache.lru.splice(gfx_texture_cache.lru.end(), gfx_texture_cache.lru, it->second.lru_location); // move to back
        return true;
    }

    if (gfx_texture_cache.map.size() >= TEXTURE_CACHE_MAX_SIZE) {
        // Remove the texture that was least recently used
        it = gfx_texture_cache.lru.front().it;
        gfx_texture_cache.free_texture_ids.push_back(it->second.texture_id);
        gfx_texture_cache.map.erase(it);
        gfx_texture_cache.lru.pop_front();
    }

    uint32_t texture_id;
    if (!gfx_texture_cache.free_texture_ids.empty()) {
        texture_id = gfx_texture_cache.free_texture_ids.back();
        gfx_texture_cache.free_texture_ids.pop_back();
    } else {
        texture_id = gfx_rapi->new_texture();
    }

    it = gfx_texture_cache.map.insert(make_pair(key, TextureCacheValue())).first;
    TextureCacheNode* node = &*it;
    node->second.texture_id = texture_id;
    node->second.lru_location = gfx_texture_cache.lru.insert(gfx_texture_cache.lru.end(), { it });

    gfx_rapi->select_texture(i, texture_id);
    gfx_rapi->set_sampler_parameters(i, false, 0, 0);
    *n = node;
    return false;
}

static void gfx_texture_cache_delete(const uint8_t* orig_addr)
{
    while (gfx_texture_cache.map.bucket_count() > 0) {
        TextureCacheKey key = { orig_addr, {0}, 0, 0 }; // bucket index only depends on the address
        size_t bucket = gfx_texture_cache.map.bucket(key);
        bool again = false;
        for (auto it = gfx_texture_cache.map.begin(bucket); it != gfx_texture_cache.map.end(bucket); ++it) {
            if (it->first.texture_addr == orig_addr) {
                gfx_texture_cache.lru.erase(it->second.lru_location);
                gfx_texture_cache.free_texture_ids.push_back(it->second.texture_id);
                gfx_texture_cache.map.erase(it->first);
                again = true;
                break;
            }
        }
        if (!again) {
            break;
        }
    }
}

static void import_texture_rgba16(int tile) {
    uint8_t rgba32_buf[480 * 240 * 4];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    //SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes / 2; i++) {
        uint16_t col16 = (addr[2 * i] << 8) | addr[2 * i + 1];
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;
        rgba32_buf[4*i + 0] = SCALE_5_8(r);
        rgba32_buf[4*i + 1] = SCALE_5_8(g);
        rgba32_buf[4*i + 2] = SCALE_5_8(b);
        rgba32_buf[4*i + 3] = a ? 255 : 0;
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes / 2;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_rgba32(int tile) {
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    uint32_t width = rdp.texture_tile[tile].line_size_bytes / 2;
    uint32_t height = (size_bytes / 2) / rdp.texture_tile[tile].line_size_bytes;
    gfx_rapi->upload_texture(addr, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, addr, width, height);
}

static void import_texture_ia4(int tile) {
    uint8_t rgba32_buf[32768];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes * 2; i++) {
        uint8_t byte = addr[i / 2];
        uint8_t part = (byte >> (4 - (i % 2) * 4)) & 0xf;
        uint8_t intensity = part >> 1;
        uint8_t alpha = part & 1;
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        rgba32_buf[4*i + 0] = SCALE_3_8(r);
        rgba32_buf[4*i + 1] = SCALE_3_8(g);
        rgba32_buf[4*i + 2] = SCALE_3_8(b);
        rgba32_buf[4*i + 3] = alpha ? 255 : 0;
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes * 2;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_ia8(int tile) {
    uint8_t rgba32_buf[16384];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes; i++) {
        uint8_t intensity = addr[i] >> 4;
        uint8_t alpha = addr[i] & 0xf;
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        rgba32_buf[4*i + 0] = SCALE_4_8(r);
        rgba32_buf[4*i + 1] = SCALE_4_8(g);
        rgba32_buf[4*i + 2] = SCALE_4_8(b);
        rgba32_buf[4*i + 3] = SCALE_4_8(alpha);
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_ia16(int tile) {
    uint8_t rgba32_buf[8192];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes / 2; i++) {
        uint8_t intensity = addr[2 * i];
        uint8_t alpha = addr[2 * i + 1];
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        rgba32_buf[4*i + 0] = r;
        rgba32_buf[4*i + 1] = g;
        rgba32_buf[4*i + 2] = b;
        rgba32_buf[4*i + 3] = alpha;
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes / 2;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_i4(int tile) {
    uint8_t rgba32_buf[32768];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    //SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes * 2; i++) {
        uint8_t byte = addr[i / 2];
        uint8_t part = (byte >> (4 - (i % 2) * 4)) & 0xf;
        uint8_t intensity = part;
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        uint8_t a = intensity;
        rgba32_buf[4*i + 0] = SCALE_4_8(r);
        rgba32_buf[4*i + 1] = SCALE_4_8(g);
        rgba32_buf[4*i + 2] = SCALE_4_8(b);
        rgba32_buf[4 * i + 3] = SCALE_4_8(a);
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes * 2;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_i8(int tile) {
    uint8_t rgba32_buf[16384];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    //SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes; i++) {
        uint8_t intensity = addr[i];
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        uint8_t a = intensity;
        rgba32_buf[4*i + 0] = r;
        rgba32_buf[4*i + 1] = g;
        rgba32_buf[4*i + 2] = b;
        rgba32_buf[4 * i + 3] = a;
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}


static void import_texture_ci4(int tile) {
    uint8_t rgba32_buf[32768];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;
    uint32_t pal_idx = rdp.texture_tile[tile].palette; // 0-15
    const uint8_t *palette = rdp.palettes[pal_idx / 8] + (pal_idx % 8) * 16 * 2; // 16 pixel entries, 16 bits each
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    for (uint32_t i = 0; i < size_bytes * 2; i++) {
        uint8_t byte = addr[i / 2];
        uint8_t idx = (byte >> (4 - (i % 2) * 4)) & 0xf;
        uint16_t col16 = (palette[idx * 2] << 8) | palette[idx * 2 + 1]; // Big endian load
        uint8_t a = col16 & 1;
        uint8_t r = col16 >> 11;
        uint8_t g = (col16 >> 6) & 0x1f;
        uint8_t b = (col16 >> 1) & 0x1f;
        rgba32_buf[4*i + 0] = SCALE_5_8(r);
        rgba32_buf[4*i + 1] = SCALE_5_8(g);
        rgba32_buf[4*i + 2] = SCALE_5_8(b);
        rgba32_buf[4*i + 3] = a ? 255 : 0;
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes * 2;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture_ci8(int tile) {
    uint8_t rgba32_buf[16384];
    const uint8_t* addr = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes;

    for (uint32_t i = 0, j = 0; i < size_bytes; j += full_image_line_size_bytes - line_size_bytes)
    {
        for (uint32_t k = 0; k < line_size_bytes; i++, k++, j++) {
            uint8_t idx = addr[j];
            uint16_t col16 = (rdp.palettes[idx / 128][(idx % 128) * 2] << 8) | rdp.palettes[idx / 128][(idx % 128) * 2 + 1]; // Big endian load
            uint8_t a = col16 & 1;
            uint8_t r = col16 >> 11;
            uint8_t g = (col16 >> 6) & 0x1f;
            uint8_t b = (col16 >> 1) & 0x1f;
            rgba32_buf[4 * i + 0] = SCALE_5_8(r);
            rgba32_buf[4 * i + 1] = SCALE_5_8(g);
            rgba32_buf[4 * i + 2] = SCALE_5_8(b);
            rgba32_buf[4 * i + 3] = a ? 255 : 0;
        }
    }

    uint32_t width = rdp.texture_tile[tile].line_size_bytes;
    uint32_t height = size_bytes / rdp.texture_tile[tile].line_size_bytes;

    if (size_bytes > 15000)
    {
        int bp = 0;
    }

    gfx_rapi->upload_texture(rgba32_buf, width, height);
    // DumpTexture(rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path, rgba32_buf, width, height);
}

static void import_texture(int i, int tile) {
    uint8_t fmt = rdp.texture_tile[tile].fmt;
    uint8_t siz = rdp.texture_tile[tile].siz;
    uint32_t tmem_index = rdp.texture_tile[tile].tmem_index;

    if (gfx_texture_cache_lookup(i, tile))
    {
        return;
    }

    int t0 = get_time();
    if (fmt == G_IM_FMT_RGBA) {
        if (siz == G_IM_SIZ_16b) {
            import_texture_rgba16(tile);
        } else if (siz == G_IM_SIZ_32b) {
            import_texture_rgba32(tile);
        } else {
            //abort(); // OTRTODO: Sometimes, seemingly randomly, we end up here. Could be a bad dlist, could be something F3D does not have supported. Further investigation is needed.
        }
    } else if (fmt == G_IM_FMT_IA) {
        if (siz == G_IM_SIZ_4b) {
            import_texture_ia4(tile);
        } else if (siz == G_IM_SIZ_8b) {
            import_texture_ia8(tile);
        } else if (siz == G_IM_SIZ_16b) {
            import_texture_ia16(tile);
        } else {
            abort();
        }
    } else if (fmt == G_IM_FMT_CI) {
        if (siz == G_IM_SIZ_4b) {
            import_texture_ci4(tile);
        } else if (siz == G_IM_SIZ_8b) {
            import_texture_ci8(tile);
        } else {
            abort();
        }
    } else if (fmt == G_IM_FMT_I) {
        if (siz == G_IM_SIZ_4b) {
            import_texture_i4(tile);
        } else if (siz == G_IM_SIZ_8b) {
            import_texture_i8(tile);
        } else {
            abort();
        }
    } else {
        abort();
    }
    int t1 = get_time();
    //printf("Time diff: %d\n", t1 - t0);
}

static void gfx_normalize_vector(float v[3]) {
    float s = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] /= s;
    v[1] /= s;
    v[2] /= s;
}

static void gfx_transposed_matrix_mul(float res[3], const float a[3], const float b[4][4]) {
    res[0] = a[0] * b[0][0] + a[1] * b[0][1] + a[2] * b[0][2];
    res[1] = a[0] * b[1][0] + a[1] * b[1][1] + a[2] * b[1][2];
    res[2] = a[0] * b[2][0] + a[1] * b[2][1] + a[2] * b[2][2];
}

static void calculate_normal_dir(const Light_t *light, float coeffs[3]) {
    float light_dir[3] = {
        light->dir[0] / 127.0f,
        light->dir[1] / 127.0f,
        light->dir[2] / 127.0f
    };

    gfx_transposed_matrix_mul(coeffs, light_dir, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1]);
    gfx_normalize_vector(coeffs);
}

static void gfx_matrix_mul(float res[4][4], const float a[4][4], const float b[4][4]) {
    float tmp[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            tmp[i][j] = a[i][0] * b[0][j] +
                        a[i][1] * b[1][j] +
                        a[i][2] * b[2][j] +
                        a[i][3] * b[3][j];
        }
    }
    memcpy(res, tmp, sizeof(tmp));
}

static void gfx_sp_matrix(uint8_t parameters, const int32_t *addr) {
    float matrix[4][4];

    if (auto it = current_mtx_replacements->find((Mtx *)addr); it != current_mtx_replacements->end()) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                float v = it->second.mf[i][j];
                int as_int = (int)(v * 65536.0f);
                matrix[i][j] = as_int * (1.0f / 65536.0f);
            }
        }
    } else {
#ifndef GBI_FLOATS
        // Original GBI where fixed point matrices are used
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j += 2) {
                int32_t int_part = addr[i * 2 + j / 2];
                uint32_t frac_part = addr[8 + i * 2 + j / 2];
                matrix[i][j] = (int32_t)((int_part & 0xffff0000) | (frac_part >> 16)) / 65536.0f;
                matrix[i][j + 1] = (int32_t)((int_part << 16) | (frac_part & 0xffff)) / 65536.0f;
            }
        }
#else
        // For a modified GBI where fixed point values are replaced with floats
        memcpy(matrix, addr, sizeof(matrix));
#endif
    }

    if (parameters & G_MTX_PROJECTION) {
        if (parameters & G_MTX_LOAD) {
            memcpy(rsp.P_matrix, matrix, sizeof(matrix));
        } else {
            gfx_matrix_mul(rsp.P_matrix, matrix, rsp.P_matrix);
        }
    } else { // G_MTX_MODELVIEW
        if ((parameters & G_MTX_PUSH) && rsp.modelview_matrix_stack_size < 11) {
            ++rsp.modelview_matrix_stack_size;
            memcpy(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 2], sizeof(matrix));
        }
        if (parameters & G_MTX_LOAD) {
            memcpy(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], matrix, sizeof(matrix));
        } else {
            gfx_matrix_mul(rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], matrix, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1]);
        }
        rsp.lights_changed = 1;
    }
    gfx_matrix_mul(rsp.MP_matrix, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], rsp.P_matrix);
}

static void gfx_sp_pop_matrix(uint32_t count) {
    while (count--) {
        if (rsp.modelview_matrix_stack_size > 0) {
            --rsp.modelview_matrix_stack_size;
            if (rsp.modelview_matrix_stack_size > 0) {
                gfx_matrix_mul(rsp.MP_matrix, rsp.modelview_matrix_stack[rsp.modelview_matrix_stack_size - 1], rsp.P_matrix);
            }
        }
    }
}

static float gfx_adjust_x_for_aspect_ratio(float x)
{
    if (fbActive)
        return x;
    else
        return x * (4.0f / 3.0f) / ((float)gfx_current_dimensions.width / (float)gfx_current_dimensions.height);
}

static void gfx_adjust_width_height_for_scale(uint32_t& width, uint32_t& height) {
    width = round(width * RATIO_Y);
    height = round(height * RATIO_Y);
    if (width == 0) {
        width = 1;
    }
    if (height == 0) {
        height = 1;
    }
}

static void gfx_sp_vertex(size_t n_vertices, size_t dest_index, const Vtx *vertices) {
    for (size_t i = 0; i < n_vertices; i++, dest_index++) {
        const Vtx_t *v = &vertices[i].v;
        const Vtx_tn *vn = &vertices[i].n;
        struct LoadedVertex *d = &rsp.loaded_vertices[dest_index];

        if (v == NULL)
            return;

        float x = v->ob[0] * rsp.MP_matrix[0][0] + v->ob[1] * rsp.MP_matrix[1][0] + v->ob[2] * rsp.MP_matrix[2][0] + rsp.MP_matrix[3][0];
        float y = v->ob[0] * rsp.MP_matrix[0][1] + v->ob[1] * rsp.MP_matrix[1][1] + v->ob[2] * rsp.MP_matrix[2][1] + rsp.MP_matrix[3][1];
        float z = v->ob[0] * rsp.MP_matrix[0][2] + v->ob[1] * rsp.MP_matrix[1][2] + v->ob[2] * rsp.MP_matrix[2][2] + rsp.MP_matrix[3][2];
        float w = v->ob[0] * rsp.MP_matrix[0][3] + v->ob[1] * rsp.MP_matrix[1][3] + v->ob[2] * rsp.MP_matrix[2][3] + rsp.MP_matrix[3][3];

        x = gfx_adjust_x_for_aspect_ratio(x);

        short U = v->tc[0] * rsp.texture_scaling_factor.s >> 16;
        short V = v->tc[1] * rsp.texture_scaling_factor.t >> 16;

        if (rsp.geometry_mode & G_LIGHTING) {
            if (rsp.lights_changed) {
                for (int i = 0; i < rsp.current_num_lights - 1; i++) {
                    calculate_normal_dir(&rsp.current_lights[i], rsp.current_lights_coeffs[i]);
                }
                /*static const Light_t lookat_x = {{0, 0, 0}, 0, {0, 0, 0}, 0, {127, 0, 0}, 0};
                static const Light_t lookat_y = {{0, 0, 0}, 0, {0, 0, 0}, 0, {0, 127, 0}, 0};*/
                calculate_normal_dir(&rsp.lookat[0], rsp.current_lookat_coeffs[0]);
                calculate_normal_dir(&rsp.lookat[1], rsp.current_lookat_coeffs[1]);
                rsp.lights_changed = false;
            }

            int r = rsp.current_lights[rsp.current_num_lights - 1].col[0];
            int g = rsp.current_lights[rsp.current_num_lights - 1].col[1];
            int b = rsp.current_lights[rsp.current_num_lights - 1].col[2];

            for (int i = 0; i < rsp.current_num_lights - 1; i++) {
                float intensity = 0;
                intensity += vn->n[0] * rsp.current_lights_coeffs[i][0];
                intensity += vn->n[1] * rsp.current_lights_coeffs[i][1];
                intensity += vn->n[2] * rsp.current_lights_coeffs[i][2];
                intensity /= 127.0f;
                if (intensity > 0.0f) {
                    r += intensity * rsp.current_lights[i].col[0];
                    g += intensity * rsp.current_lights[i].col[1];
                    b += intensity * rsp.current_lights[i].col[2];
                }
            }

            d->color.r = r > 255 ? 255 : r;
            d->color.g = g > 255 ? 255 : g;
            d->color.b = b > 255 ? 255 : b;

            if (rsp.geometry_mode & G_TEXTURE_GEN) {
                float dotx = 0, doty = 0;
                dotx += vn->n[0] * rsp.current_lookat_coeffs[0][0];
                dotx += vn->n[1] * rsp.current_lookat_coeffs[0][1];
                dotx += vn->n[2] * rsp.current_lookat_coeffs[0][2];
                doty += vn->n[0] * rsp.current_lookat_coeffs[1][0];
                doty += vn->n[1] * rsp.current_lookat_coeffs[1][1];
                doty += vn->n[2] * rsp.current_lookat_coeffs[1][2];


                dotx /= 127.0f;
                doty /= 127.0f;

                dotx = Ship::Math::clamp(dotx, -1.0f, 1.0f);
                doty = Ship::Math::clamp(doty, -1.0f, 1.0f);

                if (rsp.geometry_mode & G_TEXTURE_GEN_LINEAR) {
                                    // Not sure exactly what formula we should use to get accurate values
                                    /*dotx = (2.906921f * dotx * dotx + 1.36114f) * dotx;
                                    doty = (2.906921f * doty * doty + 1.36114f) * doty;
                                    dotx = (dotx + 1.0f) / 4.0f;
                                    doty = (doty + 1.0f) / 4.0f;*/
                    dotx = acosf(-dotx) /* M_PI */ / 4.0f;
                    doty = acosf(-doty) /* M_PI */ / 4.0f;
                }
                else {
                    dotx = (dotx + 1.0f) / 4.0f;
                    doty = (doty + 1.0f) / 4.0f;
                }

                U = (int32_t)(dotx * rsp.texture_scaling_factor.s);
                V = (int32_t)(doty * rsp.texture_scaling_factor.t);
            }
        } else {
            d->color.r = v->cn[0];
            d->color.g = v->cn[1];
            d->color.b = v->cn[2];
        }

        d->u = U;
        d->v = V;

        // trivial clip rejection
        d->clip_rej = 0;
        if (x < -w) d->clip_rej |= 1; // CLIP_LEFT
        if (x > w) d->clip_rej |= 2; // CLIP_RIGHT
        if (y < -w) d->clip_rej |= 4; // CLIP_BOTTOM
        if (y > w) d->clip_rej |= 8; // CLIP_TOP
        // if (z < -w) d->clip_rej |= 16; // CLIP_NEAR
        if (z > w) d->clip_rej |= 32; // CLIP_FAR

        d->x = x;
        d->y = y;
        d->z = z;
        d->w = w;

        if (rsp.geometry_mode & G_FOG) {
            if (fabsf(w) < 0.001f) {
                // To avoid division by zero
                w = 0.001f;
            }

            float winv = 1.0f / w;
            if (winv < 0.0f) winv = std::numeric_limits<int16_t>::max();

            float fog_z = z * winv * rsp.fog_mul + rsp.fog_offset;
            fog_z = Ship::Math::clamp(fog_z, 0.0f, 255.0f);
            d->color.a = fog_z; // Use alpha variable to store fog factor
        } else {
            d->color.a = v->cn[3];
        }
    }
}

static void gfx_sp_modify_vertex(uint16_t vtx_idx, uint8_t where, uint32_t val) {
    SUPPORT_CHECK(where == G_MWO_POINT_ST);

    int16_t s = (int16_t)(val >> 16);
    int16_t t = (int16_t)val;

    struct LoadedVertex* v = &rsp.loaded_vertices[vtx_idx];
    v->u = s;
    v->v = t;
}

static void gfx_sp_tri1(uint8_t vtx1_idx, uint8_t vtx2_idx, uint8_t vtx3_idx, bool is_rect) {
    struct LoadedVertex* v1 = &rsp.loaded_vertices[vtx1_idx];
    struct LoadedVertex* v2 = &rsp.loaded_vertices[vtx2_idx];
    struct LoadedVertex* v3 = &rsp.loaded_vertices[vtx3_idx];
    struct LoadedVertex* v_arr[3] = { v1, v2, v3 };

    //if (rand()%2) return;

    if (v1->clip_rej & v2->clip_rej & v3->clip_rej) {
        // The whole triangle lies outside the visible area
        return;
    }

    if ((rsp.geometry_mode & G_CULL_BOTH) != 0) {
        float dx1 = v1->x / (v1->w) - v2->x / (v2->w);
        float dy1 = v1->y / (v1->w) - v2->y / (v2->w);
        float dx2 = v3->x / (v3->w) - v2->x / (v2->w);
        float dy2 = v3->y / (v3->w) - v2->y / (v2->w);
        float cross = dx1 * dy2 - dy1 * dx2;

        if ((v1->w < 0) ^ (v2->w < 0) ^ (v3->w < 0)) {
            // If one vertex lies behind the eye, negating cross will give the correct result.
            // If all vertices lie behind the eye, the triangle will be rejected anyway.
            cross = -cross;
        }

        switch (rsp.geometry_mode & G_CULL_BOTH) {
        case G_CULL_FRONT:
            if (cross <= 0) return;
            break;
        case G_CULL_BACK:
            if (cross >= 0) return;
            break;
        case G_CULL_BOTH:
            // Why is this even an option?
            return;
        }
    }

    bool depth_test = (rsp.geometry_mode & G_ZBUFFER) == G_ZBUFFER;
    bool depth_mask = (rdp.other_mode_l & Z_UPD) == Z_UPD;
    uint8_t depth_test_and_mask = (depth_test ? 1 : 0) | (depth_mask ? 2 : 0);
    if (depth_test_and_mask != rendering_state.depth_test_and_mask) {
        gfx_flush();
        gfx_rapi->set_depth_test_and_mask(depth_test, depth_mask);
        rendering_state.depth_test_and_mask = depth_test_and_mask;
    }

    bool zmode_decal = (rdp.other_mode_l & ZMODE_DEC) == ZMODE_DEC;
    if (zmode_decal != rendering_state.decal_mode) {
        gfx_flush();
        gfx_rapi->set_zmode_decal(zmode_decal);
        rendering_state.decal_mode = zmode_decal;
    }

    if (rdp.viewport_or_scissor_changed) {
        if (memcmp(&rdp.viewport, &rendering_state.viewport, sizeof(rdp.viewport)) != 0) {
            gfx_flush();
            gfx_rapi->set_viewport(rdp.viewport.x, rdp.viewport.y, rdp.viewport.width, rdp.viewport.height);
            rendering_state.viewport = rdp.viewport;
        }
        if (memcmp(&rdp.scissor, &rendering_state.scissor, sizeof(rdp.scissor)) != 0) {
            gfx_flush();
            gfx_rapi->set_scissor(rdp.scissor.x, rdp.scissor.y, rdp.scissor.width, rdp.scissor.height);
            rendering_state.scissor = rdp.scissor;
        }
        rdp.viewport_or_scissor_changed = false;
    }


    uint64_t cc_id = rdp.combine_mode;
    bool use_alpha = (rdp.other_mode_l & (3 << 20)) == (G_BL_CLR_MEM << 20) && (rdp.other_mode_l & (3 << 16)) == (G_BL_1MA << 16);
    bool use_fog = (rdp.other_mode_l >> 30) == G_BL_CLR_FOG;
    bool texture_edge = (rdp.other_mode_l & CVG_X_ALPHA) == CVG_X_ALPHA;
    bool use_noise = (rdp.other_mode_l & (3U << G_MDSFT_ALPHACOMPARE)) == G_AC_DITHER;
    bool use_2cyc = (rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE)) == G_CYC_2CYCLE;
    bool alpha_threshold = (rdp.other_mode_l & (3U << G_MDSFT_ALPHACOMPARE)) == G_AC_THRESHOLD;
    bool invisible = (rdp.other_mode_l & (3 << 24)) == (G_BL_0 << 24) && (rdp.other_mode_l & (3 << 20)) == (G_BL_CLR_MEM << 20);
    bool use_grayscale = rdp.grayscale;

    if (texture_edge) {
        use_alpha = true;
    }

    if (use_alpha) cc_id |= (uint64_t)SHADER_OPT_ALPHA << CC_SHADER_OPT_POS;
    if (use_fog) cc_id |= (uint64_t)SHADER_OPT_FOG << CC_SHADER_OPT_POS;
    if (texture_edge) cc_id |= (uint64_t)SHADER_OPT_TEXTURE_EDGE << CC_SHADER_OPT_POS;
    if (use_noise) cc_id |= (uint64_t)SHADER_OPT_NOISE << CC_SHADER_OPT_POS;
    if (use_2cyc) cc_id |= (uint64_t)SHADER_OPT_2CYC << CC_SHADER_OPT_POS;
    if (alpha_threshold) cc_id |= (uint64_t)SHADER_OPT_ALPHA_THRESHOLD << CC_SHADER_OPT_POS;
    if (invisible) cc_id |= (uint64_t)SHADER_OPT_INVISIBLE << CC_SHADER_OPT_POS;
    if (use_grayscale) cc_id |= (uint64_t)SHADER_OPT_GRAYSCALE << CC_SHADER_OPT_POS;

    if (!use_alpha) {
        cc_id &= ~((0xfff << 16) | ((uint64_t)0xfff << 44));
    }

    ColorCombiner* comb = gfx_lookup_or_create_color_combiner(cc_id);

    uint32_t tm = 0;
    uint32_t tex_width[2], tex_height[2], tex_width2[2], tex_height2[2];

    for (int i = 0; i < 2; i++) {
        uint32_t tile = rdp.first_tile_index + i;
        if (comb->used_textures[i]) {
            if (rdp.textures_changed[i]) {
                gfx_flush();
                import_texture(i, tile);
                rdp.textures_changed[i] = false;
            }

            uint8_t cms = rdp.texture_tile[tile].cms;
            uint8_t cmt = rdp.texture_tile[tile].cmt;

            uint32_t tex_size_bytes = rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes;
            uint32_t line_size = rdp.texture_tile[tile].line_size_bytes;

            if (line_size == 0)
                line_size = 1;

            tex_height[i] = tex_size_bytes / line_size;
            switch (rdp.texture_tile[tile].siz) {
            case G_IM_SIZ_4b:
                line_size <<= 1;
                break;
            case G_IM_SIZ_8b:
                break;
            case G_IM_SIZ_16b:
                line_size /= G_IM_SIZ_16b_LINE_BYTES;
                break;
            case G_IM_SIZ_32b:
                line_size /= G_IM_SIZ_32b_LINE_BYTES; // this is 2!
                tex_height[i] /= 2;
                break;
            }
            tex_width[i] = line_size;

            tex_width2[i] = (rdp.texture_tile[tile].lrs - rdp.texture_tile[tile].uls + 4) / 4;
            tex_height2[i] = (rdp.texture_tile[tile].lrt - rdp.texture_tile[tile].ult + 4) / 4;

            uint32_t tex_width1 = tex_width[i] << (cms & G_TX_MIRROR);
            uint32_t tex_height1 = tex_height[i] << (cmt & G_TX_MIRROR);

            if ((cms & G_TX_CLAMP) && ((cms & G_TX_MIRROR) || tex_width1 != tex_width2[i])) {
                tm |= 1 << 2 * i;
                cms &= ~G_TX_CLAMP;
            }
            if ((cmt & G_TX_CLAMP) && ((cmt & G_TX_MIRROR) || tex_height1 != tex_height2[i])) {
                tm |= 1 << 2 * i + 1;
                cmt &= ~G_TX_CLAMP;
            }

            bool linear_filter = (rdp.other_mode_h & (3U << G_MDSFT_TEXTFILT)) != G_TF_POINT;
            if (linear_filter != rendering_state.textures[i]->second.linear_filter || cms != rendering_state.textures[i]->second.cms || cmt != rendering_state.textures[i]->second.cmt) {
                gfx_flush();
                gfx_rapi->set_sampler_parameters(i, linear_filter, cms, cmt);
                rendering_state.textures[i]->second.linear_filter = linear_filter;
                rendering_state.textures[i]->second.cms = cms;
                rendering_state.textures[i]->second.cmt = cmt;
            }
        }
    }

    struct ShaderProgram* prg = comb->prg[tm];
    if (prg == NULL) {
        comb->prg[tm] = prg = gfx_lookup_or_create_shader_program(comb->shader_id0, comb->shader_id1 | (tm * SHADER_OPT_TEXEL0_CLAMP_S));
    }
    if (prg != rendering_state.shader_program) {
        gfx_flush();
        gfx_rapi->unload_shader(rendering_state.shader_program);
        gfx_rapi->load_shader(prg);
        rendering_state.shader_program = prg;
    }
    if (use_alpha != rendering_state.alpha_blend) {
        gfx_flush();
        gfx_rapi->set_use_alpha(use_alpha);
        rendering_state.alpha_blend = use_alpha;
    }
    uint8_t num_inputs;
    bool used_textures[2];

    if (markerOn)
    {
        int bp = 0;
    }

    gfx_rapi->shader_get_info(prg, &num_inputs, used_textures);

    struct GfxClipParameters clip_parameters = gfx_rapi->get_clip_parameters();

    for (int i = 0; i < 3; i++) {
        float z = v_arr[i]->z, w = v_arr[i]->w;
        if (clip_parameters.z_is_from_0_to_1) {
            z = (z + w) / 2.0f;
        }

        if (markerOn)
        {
            //z = 10;
        }

        buf_vbo[buf_vbo_len++] = v_arr[i]->x;
        buf_vbo[buf_vbo_len++] = clip_parameters.invert_y ? -v_arr[i]->y : v_arr[i]->y;
        buf_vbo[buf_vbo_len++] = z;
        buf_vbo[buf_vbo_len++] = w;

        for (int t = 0; t < 2; t++) {
            if (!used_textures[t]) {
                continue;
            }
            float u = v_arr[i]->u / 32.0f;
            float v = v_arr[i]->v / 32.0f;
            int shifts = rdp.texture_tile[rdp.first_tile_index + t].shifts;
            int shiftt = rdp.texture_tile[rdp.first_tile_index + t].shiftt;
            if (shifts != 0) {
                if (shifts <= 10) {
                    u /= 1 << shifts;
                } else {
                    u *= 1 << (16 - shifts);
                }
            }
            if (shiftt != 0) {
                if (shiftt <= 10) {
                    v /= 1 << shiftt;
                } else {
                    v *= 1 << (16 - shiftt);
                }

            }

            u -= rdp.texture_tile[rdp.first_tile_index + t].uls / 4.0f;
            v -= rdp.texture_tile[rdp.first_tile_index + t].ult / 4.0f;

            if ((rdp.other_mode_h & (3U << G_MDSFT_TEXTFILT)) != G_TF_POINT) {
                // Linear filter adds 0.5f to the coordinates
                if (!is_rect) {
                    u += 0.5f;
                    v += 0.5f;
                }
            }

            buf_vbo[buf_vbo_len++] = u / tex_width[t];
            buf_vbo[buf_vbo_len++] = v / tex_height[t];

            bool clampS = tm & (1 << 2 * t);
            bool clampT = tm & (1 << 2 * t + 1);

            if (clampS) {
                buf_vbo[buf_vbo_len++] = (tex_width2[t] - 0.5f) / tex_width[t];
            }
#ifdef __WIIU__
            else {
                buf_vbo[buf_vbo_len++] = 0.0f;
            }
#endif
            if (clampT) {
                buf_vbo[buf_vbo_len++] = (tex_height2[t] - 0.5f) / tex_height[t];
            }
#ifdef __WIIU__
            else {
                buf_vbo[buf_vbo_len++] = 0.0f;
            }
#endif
        }

        if (use_fog) {
            buf_vbo[buf_vbo_len++] = rdp.fog_color.r / 255.0f;
            buf_vbo[buf_vbo_len++] = rdp.fog_color.g / 255.0f;
            buf_vbo[buf_vbo_len++] = rdp.fog_color.b / 255.0f;
            buf_vbo[buf_vbo_len++] = v_arr[i]->color.a / 255.0f; // fog factor (not alpha)
        }

        if (use_grayscale) {
            buf_vbo[buf_vbo_len++] = rdp.grayscale_color.r / 255.0f;
            buf_vbo[buf_vbo_len++] = rdp.grayscale_color.g / 255.0f;
            buf_vbo[buf_vbo_len++] = rdp.grayscale_color.b / 255.0f;
            buf_vbo[buf_vbo_len++] = rdp.grayscale_color.a / 255.0f; // lerp interpolation factor (not alpha)
        }

        for (int j = 0; j < num_inputs; j++) {
            struct RGBA* color = 0;
            struct RGBA tmp;
            for (int k = 0; k < 1 + (use_alpha ? 1 : 0); k++) {
                switch (comb->shader_input_mapping[k][j]) {
                    // Note: CCMUX constants and ACMUX constants used here have same value, which is why this works (except LOD fraction).
                case G_CCMUX_PRIMITIVE:
                    color = &rdp.prim_color;
                    break;
                case G_CCMUX_SHADE:
                    color = &v_arr[i]->color;
                    break;
                case G_CCMUX_ENVIRONMENT:
                    color = &rdp.env_color;
                    break;
                case G_CCMUX_PRIMITIVE_ALPHA:
                {
                    tmp.r = tmp.g = tmp.b = rdp.prim_color.a;
                    color = &tmp;
                    break;
                }
                case G_CCMUX_ENV_ALPHA:
                {
                    tmp.r = tmp.g = tmp.b = rdp.env_color.a;
                    color = &tmp;
                    break;
                }
                case G_CCMUX_PRIM_LOD_FRAC:
                {
                    tmp.r = tmp.g = tmp.b = rdp.prim_lod_fraction;
                    color = &tmp;
                    break;
                }
                case G_CCMUX_LOD_FRACTION:
                {
                    if (rdp.other_mode_l & G_TL_LOD) {
                        // "Hack" that works for Bowser - Peach painting
                        float distance_frac = (v1->w - 3000.0f) / 3000.0f;
                        if (distance_frac < 0.0f) distance_frac = 0.0f;
                        if (distance_frac > 1.0f) distance_frac = 1.0f;
                        tmp.r = tmp.g = tmp.b = tmp.a = distance_frac * 255.0f;
                    } else {
                        tmp.r = tmp.g = tmp.b = tmp.a = 255.0f;
                    }
                    color = &tmp;
                    break;
                }
                case G_ACMUX_PRIM_LOD_FRAC:
                    tmp.a = rdp.prim_lod_fraction;
                    color = &tmp;
                    break;
                default:
                    memset(&tmp, 0, sizeof(tmp));
                    color = &tmp;
                    break;
                }
                if (k == 0) {
                    buf_vbo[buf_vbo_len++] = color->r / 255.0f;
                    buf_vbo[buf_vbo_len++] = color->g / 255.0f;
                    buf_vbo[buf_vbo_len++] = color->b / 255.0f;
#ifdef __WIIU__
                    // padding
                    if (!use_alpha) {
                        buf_vbo[buf_vbo_len++] = 1.0f;
                    }
#endif
                }
                else {
                    if (use_fog && color == &v_arr[i]->color) {
                        // Shade alpha is 100% for fog
                        buf_vbo[buf_vbo_len++] = 1.0f;
                    }
                    else {
                        buf_vbo[buf_vbo_len++] = color->a / 255.0f;
                    }
                }
            }
        }
        //struct RGBA *color = &v_arr[i]->color;
        //buf_vbo[buf_vbo_len++] = color->r / 255.0f;
        //buf_vbo[buf_vbo_len++] = color->g / 255.0f;
        //buf_vbo[buf_vbo_len++] = color->b / 255.0f;
        //buf_vbo[buf_vbo_len++] = color->a / 255.0f;
    }

    if (++buf_vbo_num_tris == MAX_BUFFERED) {
        //if (++buf_vbo_num_tris == 1) {
        if (markerOn)
        {
            int bp = 0;
        }
        gfx_flush();
    }
}

static void gfx_sp_geometry_mode(uint32_t clear, uint32_t set) {
    rsp.geometry_mode &= ~clear;
    rsp.geometry_mode |= set;
}

static void gfx_adjust_viewport_or_scissor(XYWidthHeight *area) {
    if (!fbActive) {
        area->width *= RATIO_X;
        area->height *= RATIO_Y;
        area->x *= RATIO_X;
        area->y = SCREEN_HEIGHT - area->y;
        area->y *= RATIO_Y;

        if (!game_renders_to_framebuffer || (gfx_msaa_level > 1 && gfx_current_dimensions.width == gfx_current_game_window_viewport.width && gfx_current_dimensions.height == gfx_current_game_window_viewport.height)) {
            area->x += gfx_current_game_window_viewport.x;
            area->y += gfx_current_window_dimensions.height - (gfx_current_game_window_viewport.y + gfx_current_game_window_viewport.height);
        }
    } else {
        area->width *= RATIO_Y;
        area->height *= RATIO_Y;
        area->x *= RATIO_Y;
        area->y = active_fb->second.orig_height - area->y;
        area->y *= RATIO_Y;
    }
}

static void gfx_calc_and_set_viewport(const Vp_t *viewport) {
    // 2 bits fraction
    float width = 2.0f * viewport->vscale[0] / 4.0f;
    float height = 2.0f * viewport->vscale[1] / 4.0f;
    float x = (viewport->vtrans[0] / 4.0f) - width / 2.0f;
    float y = ((viewport->vtrans[1] / 4.0f) + height / 2.0f);

    rdp.viewport.x = x;
    rdp.viewport.y = y;
    rdp.viewport.width = width;
    rdp.viewport.height = height;

    gfx_adjust_viewport_or_scissor(&rdp.viewport);

    rdp.viewport_or_scissor_changed = true;
}

static void gfx_sp_movemem(uint8_t index, uint8_t offset, const void* data) {
    switch (index) {
        case G_MV_VIEWPORT:
            gfx_calc_and_set_viewport((const Vp_t *) data);
            break;
#if 0
        case G_MV_LOOKATY:
        case G_MV_LOOKATX:
            memcpy(rsp.current_lookat + (index - G_MV_LOOKATY) / 2, data, sizeof(Light_t));
            //rsp.lights_changed = 1;
            break;
#endif
#ifdef F3DEX_GBI_2
        case G_MV_LIGHT: {
            int lightidx = offset / 24 - 2;
            if (lightidx >= 0 && lightidx <= MAX_LIGHTS) { // skip lookat
                // NOTE: reads out of bounds if it is an ambient light
                memcpy(rsp.current_lights + lightidx, data, sizeof(Light_t));
            } else if (lightidx < 0) {
                memcpy(rsp.lookat + offset / 24, data, sizeof(Light_t));
            }
            break;
        }
#else
        case G_MV_L0:
        case G_MV_L1:
        case G_MV_L2:
            // NOTE: reads out of bounds if it is an ambient light
            memcpy(rsp.current_lights + (index - G_MV_L0) / 2, data, sizeof(Light_t));
            break;
#endif
    }
}

static void gfx_sp_moveword(uint8_t index, uint16_t offset, uintptr_t data) {
    switch (index) {
        case G_MW_NUMLIGHT:
#ifdef F3DEX_GBI_2
            rsp.current_num_lights = data / 24 + 1; // add ambient light
#else
            // Ambient light is included
            // The 31th bit is a flag that lights should be recalculated
            rsp.current_num_lights = (data - 0x80000000U) / 32;
#endif
            rsp.lights_changed = 1;
            break;
        case G_MW_FOG:
            rsp.fog_mul = (int16_t)(data >> 16);
            rsp.fog_offset = (int16_t)data;
            break;
        case G_MW_SEGMENT:
            int segNumber = offset / 4;
            segmentPointers[segNumber] = data;
            break;
    }
}

static void gfx_sp_texture(uint16_t sc, uint16_t tc, uint8_t level, uint8_t tile, uint8_t on) {
    rsp.texture_scaling_factor.s = sc;
    rsp.texture_scaling_factor.t = tc;
    if (rdp.first_tile_index != tile) {
        rdp.textures_changed[0] = true;
        rdp.textures_changed[1] = true;
    }

    rdp.first_tile_index = tile;
}

static void gfx_dp_set_scissor(uint32_t mode, uint32_t ulx, uint32_t uly, uint32_t lrx, uint32_t lry) {
    float x = ulx / 4.0f;
    float y = lry / 4.0f;
    float width = (lrx - ulx) / 4.0f;
    float height = (lry - uly) / 4.0f;

    rdp.scissor.x = x;
    rdp.scissor.y = y;
    rdp.scissor.width = width;
    rdp.scissor.height = height;

    gfx_adjust_viewport_or_scissor(&rdp.scissor);

    rdp.viewport_or_scissor_changed = true;
}

static void gfx_dp_set_texture_image(uint32_t format, uint32_t size, uint32_t width, const void* addr, const char* otr_path) {
    rdp.texture_to_load.addr = (const uint8_t*)addr;
    rdp.texture_to_load.siz = size;
    rdp.texture_to_load.width = width;
    if (otr_path != nullptr && !strncmp(otr_path, "__OTR__", 7)) otr_path = otr_path + 7;
    rdp.texture_to_load.otr_path = otr_path;
}

static void gfx_dp_set_tile(uint8_t fmt, uint32_t siz, uint32_t line, uint32_t tmem, uint8_t tile, uint32_t palette, uint32_t cmt, uint32_t maskt, uint32_t shiftt, uint32_t cms, uint32_t masks, uint32_t shifts) {
    // OTRTODO:
    //SUPPORT_CHECK(tmem == 0 || tmem == 256);

    if (cms == G_TX_WRAP && masks == G_TX_NOMASK) {
        cms = G_TX_CLAMP;
    }
    if (cmt == G_TX_WRAP && maskt == G_TX_NOMASK) {
        cmt = G_TX_CLAMP;
    }

    rdp.texture_tile[tile].palette = palette; // palette should set upper 4 bits of color index in 4b mode
    rdp.texture_tile[tile].fmt = fmt;
    rdp.texture_tile[tile].siz = siz;
    rdp.texture_tile[tile].cms = cms;
    rdp.texture_tile[tile].cmt = cmt;
    rdp.texture_tile[tile].shifts = shifts;
    rdp.texture_tile[tile].shiftt = shiftt;
    rdp.texture_tile[tile].line_size_bytes = line * 8;

    if (rdp.texture_tile[tile].line_size_bytes > 15000)
    {
        int bp = 0;
    }

    rdp.texture_tile[tile].tmem = tmem;
    //rdp.texture_tile[tile].tmem_index = tmem / 256; // tmem is the 64-bit word offset, so 256 words means 2 kB
    rdp.texture_tile[tile].tmem_index = tmem != 0; // assume one texture is loaded at address 0 and another texture at any other address
    rdp.textures_changed[0] = true;
    rdp.textures_changed[1] = true;
}

static void gfx_dp_set_tile_size(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt) {
    rdp.texture_tile[tile].uls = uls;
    rdp.texture_tile[tile].ult = ult;
    rdp.texture_tile[tile].lrs = lrs;
    rdp.texture_tile[tile].lrt = lrt;
    rdp.textures_changed[0] = true;
    rdp.textures_changed[1] = true;
}

static void gfx_dp_load_tlut(uint8_t tile, uint32_t high_index) {
    SUPPORT_CHECK(tile == G_TX_LOADTILE);
    SUPPORT_CHECK(rdp.texture_to_load.siz == G_IM_SIZ_16b);

    SUPPORT_CHECK((rdp.texture_tile[tile].tmem == 256 && (high_index <= 127 || high_index == 255)) || (rdp.texture_tile[tile].tmem == 384 && high_index == 127));

    if (rdp.texture_tile[tile].tmem == 256) {
        rdp.palettes[0] = rdp.texture_to_load.addr;
        if (high_index == 255) {
            rdp.palettes[1] = rdp.texture_to_load.addr + 2 * 128;
        }
    } else {
        rdp.palettes[1] = rdp.texture_to_load.addr;
    }
}

static void gfx_dp_load_block(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t dxt) {
    if (markerOn)
    {
        int bp = 0;
    }

    SUPPORT_CHECK(tile == G_TX_LOADTILE);
    SUPPORT_CHECK(uls == 0);
    SUPPORT_CHECK(ult == 0);

    // The lrs field rather seems to be number of pixels to load
    uint32_t word_size_shift = 0;
    switch (rdp.texture_to_load.siz) {
        case G_IM_SIZ_4b:
            word_size_shift = 0; // Or -1? It's unused in SM64 anyway.
            break;
        case G_IM_SIZ_8b:
            word_size_shift = 0;
            break;
        case G_IM_SIZ_16b:
            word_size_shift = 1;
            break;
        case G_IM_SIZ_32b:
            word_size_shift = 2;
            break;
    }
    uint32_t size_bytes = (lrs + 1) << word_size_shift;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes = size_bytes;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes = size_bytes;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes = size_bytes;
    //assert(size_bytes <= 4096 && "bug: too big texture");
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr = rdp.texture_to_load.addr;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path = rdp.texture_to_load.otr_path;
    rdp.textures_changed[rdp.texture_tile[tile].tmem_index] = true;
}

static void gfx_dp_load_tile(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t lrt) {
    SUPPORT_CHECK(tile == G_TX_LOADTILE);

    uint32_t word_size_shift = 0;
    switch (rdp.texture_to_load.siz) {
        case G_IM_SIZ_4b:
            word_size_shift = 0;
            break;
        case G_IM_SIZ_8b:
            word_size_shift = 0;
            break;
        case G_IM_SIZ_16b:
            word_size_shift = 1;
            break;
        case G_IM_SIZ_32b:
            word_size_shift = 2;
            break;
    }

    uint32_t size_bytes = ((((lrs - uls) >> G_TEXTURE_IMAGE_FRAC) + 1) * (((lrt - ult) >> G_TEXTURE_IMAGE_FRAC) + 1)) << word_size_shift;
    uint32_t full_image_line_size_bytes = (rdp.texture_to_load.width + 1) << word_size_shift;
    uint32_t line_size_bytes = (((lrs - uls) >> G_TEXTURE_IMAGE_FRAC) + 1) << word_size_shift;
    uint32_t start_offset = full_image_line_size_bytes * (ult >> G_TEXTURE_IMAGE_FRAC) + ((uls >> G_TEXTURE_IMAGE_FRAC) << word_size_shift);
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].size_bytes = size_bytes;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].full_image_line_size_bytes = full_image_line_size_bytes;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].line_size_bytes = line_size_bytes;

    assert(size_bytes <= 4096 && "bug: too big texture");
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].addr = rdp.texture_to_load.addr + start_offset;
    rdp.loaded_texture[rdp.texture_tile[tile].tmem_index].otr_path = rdp.texture_to_load.otr_path;
    rdp.texture_tile[tile].uls = uls;
    rdp.texture_tile[tile].ult = ult;
    rdp.texture_tile[tile].lrs = lrs;
    rdp.texture_tile[tile].lrt = lrt;

    rdp.textures_changed[rdp.texture_tile[tile].tmem_index] = true;
}


/*static uint8_t color_comb_component(uint32_t v) {
    switch (v) {
        case G_CCMUX_TEXEL0:
            return CC_TEXEL0;
        case G_CCMUX_TEXEL1:
            return CC_TEXEL1;
        case G_CCMUX_PRIMITIVE:
            return CC_PRIM;
        case G_CCMUX_SHADE:
            return CC_SHADE;
        case G_CCMUX_ENVIRONMENT:
            return CC_ENV;
        case G_CCMUX_TEXEL0_ALPHA:
            return CC_TEXEL0A;
        case G_CCMUX_LOD_FRACTION:
            return CC_LOD;
        default:
            return CC_0;
    }
}

static inline uint32_t color_comb(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    return color_comb_component(a) |
           (color_comb_component(b) << 3) |
           (color_comb_component(c) << 6) |
           (color_comb_component(d) << 9);
}

static void gfx_dp_set_combine_mode(uint32_t rgb, uint32_t alpha) {
    rdp.combine_mode = rgb | (alpha << 12);
}*/

static void gfx_dp_set_combine_mode(uint32_t rgb, uint32_t alpha, uint32_t rgb_cyc2, uint32_t alpha_cyc2) {
    rdp.combine_mode = rgb | (alpha << 16) | ((uint64_t)rgb_cyc2 << 28) | ((uint64_t)alpha_cyc2 << 44);
}

static inline uint32_t color_comb(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    return (a & 0xf) | ((b & 0xf) << 4) | ((c & 0x1f) << 8) | ((d & 7) << 13);
}

static inline uint32_t alpha_comb(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
    return (a & 7) | ((b & 7) << 3) | ((c & 7) << 6) | ((d & 7) << 9);
}

static void gfx_dp_set_grayscale_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rdp.grayscale_color.r = r;
    rdp.grayscale_color.g = g;
    rdp.grayscale_color.b = b;
    rdp.grayscale_color.a = a;
}

static void gfx_dp_set_env_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rdp.env_color.r = r;
    rdp.env_color.g = g;
    rdp.env_color.b = b;
    rdp.env_color.a = a;
}

static void gfx_dp_set_prim_color(uint8_t m, uint8_t l, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rdp.prim_lod_fraction = l;
    rdp.prim_color.r = r;
    rdp.prim_color.g = g;
    rdp.prim_color.b = b;
    rdp.prim_color.a = a;
}

static void gfx_dp_set_fog_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    rdp.fog_color.r = r;
    rdp.fog_color.g = g;
    rdp.fog_color.b = b;
    rdp.fog_color.a = a;
}

static void gfx_dp_set_fill_color(uint32_t packed_color) {
    uint16_t col16 = (uint16_t)packed_color;
    uint32_t r = col16 >> 11;
    uint32_t g = (col16 >> 6) & 0x1f;
    uint32_t b = (col16 >> 1) & 0x1f;
    uint32_t a = col16 & 1;
    rdp.fill_color.r = SCALE_5_8(r);
    rdp.fill_color.g = SCALE_5_8(g);
    rdp.fill_color.b = SCALE_5_8(b);
    rdp.fill_color.a = a * 255;
}

static void gfx_draw_rectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry) {
    uint32_t saved_other_mode_h = rdp.other_mode_h;
    uint32_t cycle_type = (rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE));

    if (cycle_type == G_CYC_COPY) {
        rdp.other_mode_h = (rdp.other_mode_h & ~(3U << G_MDSFT_TEXTFILT)) | G_TF_POINT;
    }

    // U10.2 coordinates
    float ulxf = ulx;
    float ulyf = uly;
    float lrxf = lrx;
    float lryf = lry;

    ulxf = ulxf / (4.0f * HALF_SCREEN_WIDTH) - 1.0f;
    ulyf = -(ulyf / (4.0f * HALF_SCREEN_HEIGHT)) + 1.0f;
    lrxf = lrxf / (4.0f * HALF_SCREEN_WIDTH) - 1.0f;
    lryf = -(lryf / (4.0f * HALF_SCREEN_HEIGHT)) + 1.0f;

    ulxf = gfx_adjust_x_for_aspect_ratio(ulxf);
    lrxf = gfx_adjust_x_for_aspect_ratio(lrxf);

    struct LoadedVertex* ul = &rsp.loaded_vertices[MAX_VERTICES + 0];
    struct LoadedVertex* ll = &rsp.loaded_vertices[MAX_VERTICES + 1];
    struct LoadedVertex* lr = &rsp.loaded_vertices[MAX_VERTICES + 2];
    struct LoadedVertex* ur = &rsp.loaded_vertices[MAX_VERTICES + 3];

    ul->x = ulxf;
    ul->y = ulyf;
    ul->z = -1.0f;
    ul->w = 1.0f;

    ll->x = ulxf;
    ll->y = lryf;
    ll->z = -1.0f;
    ll->w = 1.0f;

    lr->x = lrxf;
    lr->y = lryf;
    lr->z = -1.0f;
    lr->w = 1.0f;

    ur->x = lrxf;
    ur->y = ulyf;
    ur->z = -1.0f;
    ur->w = 1.0f;

    // The coordinates for texture rectangle shall bypass the viewport setting
    struct XYWidthHeight default_viewport = { 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT };
    struct XYWidthHeight viewport_saved = rdp.viewport;
    uint32_t geometry_mode_saved = rsp.geometry_mode;

    gfx_adjust_viewport_or_scissor(&default_viewport);

    rdp.viewport = default_viewport;
    rdp.viewport_or_scissor_changed = true;
    rsp.geometry_mode = 0;

    gfx_sp_tri1(MAX_VERTICES + 0, MAX_VERTICES + 1, MAX_VERTICES + 3, true);
    gfx_sp_tri1(MAX_VERTICES + 1, MAX_VERTICES + 2, MAX_VERTICES + 3, true);

    rsp.geometry_mode = geometry_mode_saved;
    rdp.viewport = viewport_saved;
    rdp.viewport_or_scissor_changed = true;

    if (cycle_type == G_CYC_COPY) {
        rdp.other_mode_h = saved_other_mode_h;
    }
}

static void gfx_dp_texture_rectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, uint8_t tile, int16_t uls, int16_t ult, int16_t dsdx, int16_t dtdy, bool flip) {
    //printf("render %d at %d\n", tile, lrx);
    uint64_t saved_combine_mode = rdp.combine_mode;
    if ((rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE)) == G_CYC_COPY) {
        // Per RDP Command Summary Set Tile's shift s and this dsdx should be set to 4 texels
        // Divide by 4 to get 1 instead
        dsdx >>= 2;

        // Color combiner is turned off in copy mode
        gfx_dp_set_combine_mode(color_comb(0, 0, 0, G_CCMUX_TEXEL0), alpha_comb(0, 0, 0, G_ACMUX_TEXEL0), 0, 0);

        // Per documentation one extra pixel is added in this modes to each edge
        lrx += 1 << 2;
        lry += 1 << 2;
    }

    // uls and ult are S10.5
    // dsdx and dtdy are S5.10
    // lrx, lry, ulx, uly are U10.2
    // lrs, lrt are S10.5
    if (flip) {
        dsdx = -dsdx;
        dtdy = -dtdy;
    }
    int16_t width = !flip ? lrx - ulx : lry - uly;
    int16_t height = !flip ? lry - uly : lrx - ulx;
    float lrs = ((uls << 7) + dsdx * width) >> 7;
    float lrt = ((ult << 7) + dtdy * height) >> 7;

    struct LoadedVertex* ul = &rsp.loaded_vertices[MAX_VERTICES + 0];
    struct LoadedVertex* ll = &rsp.loaded_vertices[MAX_VERTICES + 1];
    struct LoadedVertex* lr = &rsp.loaded_vertices[MAX_VERTICES + 2];
    struct LoadedVertex* ur = &rsp.loaded_vertices[MAX_VERTICES + 3];
    ul->u = uls;
    ul->v = ult;
    lr->u = lrs;
    lr->v = lrt;
    if (!flip) {
        ll->u = uls;
        ll->v = lrt;
        ur->u = lrs;
        ur->v = ult;
    } else {
        ll->u = lrs;
        ll->v = ult;
        ur->u = uls;
        ur->v = lrt;
    }

    uint8_t saved_tile = rdp.first_tile_index;
    if (saved_tile != tile) {
        rdp.textures_changed[0] = true;
        rdp.textures_changed[1] = true;
    }
    rdp.first_tile_index = tile;

    gfx_draw_rectangle(ulx, uly, lrx, lry);
    if (saved_tile != tile) {
        rdp.textures_changed[0] = true;
        rdp.textures_changed[1] = true;
    }
    rdp.first_tile_index = saved_tile;
    rdp.combine_mode = saved_combine_mode;
}

static void gfx_dp_fill_rectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry) {
    if (rdp.color_image_address == rdp.z_buf_address) {
        // Don't clear Z buffer here since we already did it with glClear
        return;
    }
    uint32_t mode = (rdp.other_mode_h & (3U << G_MDSFT_CYCLETYPE));

    // OTRTODO: This is a bit of a hack for widescreen screen fades, but it'll work for now...
    if (ulx == 0 && uly == 0 && lrx == (319 * 4) && lry == (239 * 4))
    {
        ulx = -1024;
        uly = -1024;
        lrx = 2048;
        lry = 2048;
    }

    if (mode == G_CYC_COPY || mode == G_CYC_FILL) {
        // Per documentation one extra pixel is added in this modes to each edge
        lrx += 1 << 2;
        lry += 1 << 2;
    }

    for (int i = MAX_VERTICES; i < MAX_VERTICES + 4; i++) {
        struct LoadedVertex* v = &rsp.loaded_vertices[i];
        v->color = rdp.fill_color;
    }

    uint64_t saved_combine_mode = rdp.combine_mode;

    if (mode == G_CYC_FILL)
        gfx_dp_set_combine_mode(color_comb(0, 0, 0, G_CCMUX_SHADE), alpha_comb(0, 0, 0, G_ACMUX_SHADE), 0, 0);

    gfx_draw_rectangle(ulx, uly, lrx, lry);
    rdp.combine_mode = saved_combine_mode;
}

static void gfx_dp_set_z_image(void *z_buf_address) {
    rdp.z_buf_address = z_buf_address;
}

static void gfx_dp_set_color_image(uint32_t format, uint32_t size, uint32_t width, void* address) {
    rdp.color_image_address = address;
}

static void gfx_sp_set_other_mode(uint32_t shift, uint32_t num_bits, uint64_t mode) {
    uint64_t mask = (((uint64_t)1 << num_bits) - 1) << shift;
    uint64_t om = rdp.other_mode_l | ((uint64_t)rdp.other_mode_h << 32);
    om = (om & ~mask) | mode;
    rdp.other_mode_l = (uint32_t)om;
    rdp.other_mode_h = (uint32_t)(om >> 32);
}

static void gfx_dp_set_other_mode(uint32_t h, uint32_t l) {
    rdp.other_mode_h = h;
    rdp.other_mode_l = l;
}

static void gfx_s2dex_bg_copy(const uObjBg* bg) {
        /*
        bg->b.imageX = 0;
        bg->b.imageW = width * 4;
        bg->b.frameX = frameX * 4;
        bg->b.imageY = 0;
        bg->b.imageH = height * 4;
        bg->b.frameY = frameY * 4;
        bg->b.imagePtr = source;
        bg->b.imageLoad = G_BGLT_LOADTILE;
        bg->b.imageFmt = fmt;
        bg->b.imageSiz = siz;
        bg->b.imagePal = 0;
        bg->b.imageFlip = 0;
        */
    SUPPORT_CHECK(bg->b.imageSiz == G_IM_SIZ_16b);
    gfx_dp_set_texture_image(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, bg->b.imagePtr, nullptr);
    gfx_dp_set_tile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gfx_dp_load_block(G_TX_LOADTILE, 0, 0, (bg->b.imageW * bg->b.imageH >> 4) - 1, 0);
    gfx_dp_set_tile(bg->b.imageFmt, G_IM_SIZ_16b, bg->b.imageW >> 4, 0, G_TX_RENDERTILE, bg->b.imagePal, 0, 0, 0, 0, 0, 0);
    gfx_dp_set_tile_size(G_TX_RENDERTILE, 0, 0, bg->b.imageW, bg->b.imageH);
    gfx_dp_texture_rectangle(bg->b.frameX, bg->b.frameY, bg->b.frameX + bg->b.imageW - 4, bg->b.frameY + bg->b.imageH - 4, G_TX_RENDERTILE, bg->b.imageX << 3, bg->b.imageY << 3, 4 << 10, 1 << 10, false);
}

static inline void* seg_addr(uintptr_t w1)
{
    // Segmented?
    if (w1 & 1)
    {
        uint32_t segNum = (w1 >> 24);

        uint32_t offset = w1 & 0x00FFFFFE;
        //offset = 0; // Cursed Malon bug

        if (segmentPointers[segNum] != 0)
            return (void*)(segmentPointers[segNum] + offset);
        else
            return (void*)w1;
    }
    else
    {
        return (void*)w1;
    }
}

#define C0(pos, width) ((cmd->words.w0 >> (pos)) & ((1U << width) - 1))
#define C1(pos, width) ((cmd->words.w1 >> (pos)) & ((1U << width) - 1))

unsigned int dListBP;
int matrixBP;
uintptr_t clearMtx;

static void gfx_run_dl(Gfx* cmd) {
    //puts("dl");
    int dummy = 0;
    char dlName[128];
    const char* fileName;

    Gfx* dListStart = cmd;
    uint64_t ourHash = -1;

    for (;;) {
        uint32_t opcode = cmd->words.w0 >> 24;
        //uint32_t opcode = cmd->words.w0 & 0xFF;

        //if (markerOn)
            //printf("OP: %02X\n", opcode);


        switch (opcode) {
            // RSP commands:
        case G_LOAD_UCODE:
            rsp.fog_mul = 0;
            rsp.fog_offset = 0;
            break;
        case G_MARKER:
        {
            cmd++;

            ourHash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;

#if _DEBUG
            //uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;
            //ResourceMgr_GetNameByCRC(hash, dlName);
            //lusprintf(__FILE__, __LINE__, 6, "G_MARKER: %s\n", dlName);
#endif

            markerOn = true;
        }
            break;
        case G_INVALTEXCACHE:
        {
            uintptr_t texAddr = cmd->words.w1;

            if (texAddr == 0)
                gfx_texture_cache_clear();
            else
                gfx_texture_cache_delete((const uint8_t*)texAddr);
        }
            break;
        case G_NOOP:
        {
            uint32_t index = C0(0, 16);
            uint32_t type = C0(16, 8);
            if (type == 2) {
                const char* str = (const char *)cmd->words.w1;
                //printf("%s, %u\n", str, index);
            }
        }
            break;
            case G_MTX: {
                uintptr_t mtxAddr = cmd->words.w1;

                // OTRTODO: Temp way of dealing with gMtxClear. Need something more elegant in the future...
                uint32_t gameVersion = Ship::Window::GetInstance()->GetResourceManager()->GetGameVersion();
                if (gameVersion == OOT_PAL_GC) {
                    if (mtxAddr == SEG_ADDR(0, 0x0FBC20)) {
                        mtxAddr = clearMtx;
                    }
                } else {
                    if (mtxAddr == SEG_ADDR(0, 0x12DB20) || mtxAddr == SEG_ADDR(0, 0x12DB40)) {
                        mtxAddr = clearMtx;
                    }
                }

#ifdef F3DEX_GBI_2
                gfx_sp_matrix(C0(0, 8) ^ G_MTX_PUSH, (const int32_t *) seg_addr(mtxAddr));
#else
                gfx_sp_matrix(C0(16, 8), (const int32_t *) seg_addr(cmd->words.w1));
#endif
                break;
            }
            case G_MTX_OTR: {
                cmd++;

                uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;

#if _DEBUG
                //char fileName[4096];
                //ResourceMgr_GetNameByCRC(hash, fileName);

                //printf("G_MTX_OTR: %s\n", fileName);
#endif

                int32_t* mtx = ResourceMgr_LoadMtxByCRC(hash);

#ifdef F3DEX_GBI_2
                if (mtx != NULL)
                {
                    cmd--;
                    gfx_sp_matrix(C0(0, 8) ^ G_MTX_PUSH, mtx);
                    cmd++;
                }
#else
                gfx_sp_matrix(C0(16, 8), (const int32_t*)seg_addr(cmd->words.w1));
#endif
                break;
            }
            case (uint8_t)G_POPMTX:
#ifdef F3DEX_GBI_2
                gfx_sp_pop_matrix(cmd->words.w1 / 64);
#else
                gfx_sp_pop_matrix(1);
#endif
                break;
            case G_MOVEMEM:
#ifdef F3DEX_GBI_2
                gfx_sp_movemem(C0(0, 8), C0(8, 8) * 8, seg_addr(cmd->words.w1));
#else
                gfx_sp_movemem(C0(16, 8), 0, seg_addr(cmd->words.w1));
#endif
                break;
            case (uint8_t)G_MOVEWORD:
#ifdef F3DEX_GBI_2
                gfx_sp_moveword(C0(16, 8), C0(0, 16), cmd->words.w1);
#else
                gfx_sp_moveword(C0(0, 8), C0(8, 16), cmd->words.w1);
#endif
                break;
            case (uint8_t)G_TEXTURE:
#ifdef F3DEX_GBI_2
                gfx_sp_texture(C1(16, 16), C1(0, 16), C0(11, 3), C0(8, 3), C0(1, 7));
#else
                gfx_sp_texture(C1(16, 16), C1(0, 16), C0(11, 3), C0(8, 3), C0(0, 8));
#endif
                break;
            case G_VTX:
#ifdef F3DEX_GBI_2
                gfx_sp_vertex(C0(12, 8), C0(1, 7) - C0(12, 8), (const Vtx*)seg_addr(cmd->words.w1));
#elif defined(F3DEX_GBI) || defined(F3DLP_GBI)
                gfx_sp_vertex(C0(10, 6), C0(16, 8) / 2, seg_addr(cmd->words.w1));
#else
                gfx_sp_vertex((C0(0, 16)) / sizeof(Vtx), C0(16, 4), seg_addr(cmd->words.w1));
#endif
                break;
            case G_VTX_OTR:
            {
                uintptr_t offset = cmd->words.w1;
                cmd++;
                uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;

#if _DEBUG
                //char fileName[4096];
                //ResourceMgr_GetNameByCRC(hash, fileName);

                //printf("G_VTX_OTR: %s, 0x%08X\n", fileName, hash);
#endif
                if (offset > 0xFFFFF)
                {
                    cmd--;
                    gfx_sp_vertex(C0(12, 8), C0(1, 7) - C0(12, 8), (Vtx*)offset);
                    cmd++;
                }
                else
                {
                    Vtx* vtx = ResourceMgr_LoadVtxByCRC(hash);

                    if (vtx != NULL)
                    {
                        vtx = (Vtx*)((char*)vtx + offset);

                        cmd--;

                        if (ourHash != (uint64_t)-1)
                            ResourceMgr_RegisterResourcePatch(ourHash, cmd - dListStart, cmd->words.w1);

                        cmd->words.w1 = (uintptr_t)vtx;

                        gfx_sp_vertex(C0(12, 8), C0(1, 7) - C0(12, 8), vtx);
                        cmd++;
                    }
                }
            }
                break;
            case G_MODIFYVTX:
                gfx_sp_modify_vertex(C0(1, 15), C0(16, 8), cmd->words.w1);
                break;
            case G_DL:
                if (cmd->words.w1 == dListBP)
                {
                    int bp = 0;
                }

                if (C0(16, 1) == 0) {
                    // Push return address
                    gfx_run_dl((Gfx *)seg_addr(cmd->words.w1));
                } else {
                    cmd = (Gfx *)seg_addr(cmd->words.w1);
                    --cmd; // increase after break
                }
                break;
            case G_DL_OTR:
                if (C0(16, 1) == 0)
                {
                    // Push return address

                    cmd++;

                    uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;

#if _DEBUG
                    //char fileName[4096];
                    //ResourceMgr_GetNameByCRC(hash, fileName);

                    //printf("G_DL_OTR: %s\n", fileName);
#endif

                    Gfx* gfx = ResourceMgr_LoadGfxByCRC(hash);

                    if (gfx != 0)
                        gfx_run_dl(gfx);
                }
                else {
                    cmd = (Gfx*)seg_addr(cmd->words.w1);
                    cmd++;
                    --cmd; // increase after break
                }
                break;
            case G_BRANCH_Z_OTR:
            {
                // Push return address

                uint8_t vbidx = cmd->words.w0 & 0x00000FFF;
                uint32_t zval = cmd->words.w1;

                cmd++;

                if (rsp.loaded_vertices[vbidx].z <= zval)
                {

                    uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + cmd->words.w1;

#if _DEBUG
                    //char fileName[4096];
                    //ResourceMgr_GetNameByCRC(hash, fileName);

                    //printf("G_BRANCH_Z_OTR: %s\n", fileName);
#endif

                    Gfx* gfx = ResourceMgr_LoadGfxByCRC(hash);

                    if (gfx != 0)
                    {
                        cmd = gfx;
                        --cmd; // increase after break
                    }
                }
            }
                break;
            case (uint8_t)G_ENDDL:

                //if (markerOn)
                    //printf("END DL ON MARKER\n");

                markerOn = false;
                return;
#ifdef F3DEX_GBI_2
            case G_GEOMETRYMODE:
                gfx_sp_geometry_mode(~C0(0, 24), cmd->words.w1);
                break;
#else
            case (uint8_t)G_SETGEOMETRYMODE:
                gfx_sp_geometry_mode(0, cmd->words.w1);
                break;
            case (uint8_t)G_CLEARGEOMETRYMODE:
                gfx_sp_geometry_mode(cmd->words.w1, 0);
                break;
#endif
            case (uint8_t)G_TRI1:
#ifdef F3DEX_GBI_2
                gfx_sp_tri1(C0(16, 8) / 2, C0(8, 8) / 2, C0(0, 8) / 2, false);
#elif defined(F3DEX_GBI) || defined(F3DLP_GBI)
                gfx_sp_tri1(C1(16, 8) / 2, C1(8, 8) / 2, C1(0, 8) / 2, false);
#else
                gfx_sp_tri1(C1(16, 8) / 10, C1(8, 8) / 10, C1(0, 8) / 10, false);
#endif
                break;
#ifdef F3DEX_GBI_2
            case G_QUAD:
            {
                int bp = 0;
                [[fallthrough]];
            }
#endif
#if defined(F3DEX_GBI) || defined(F3DLP_GBI)
            case (uint8_t)G_TRI2:
                gfx_sp_tri1(C0(16, 8) / 2, C0(8, 8) / 2, C0(0, 8) / 2, false);
                gfx_sp_tri1(C1(16, 8) / 2, C1(8, 8) / 2, C1(0, 8) / 2, false);
                break;
#endif
            case (uint8_t)G_SETOTHERMODE_L:
#ifdef F3DEX_GBI_2
                gfx_sp_set_other_mode(31 - C0(8, 8) - C0(0, 8), C0(0, 8) + 1, cmd->words.w1);
#else
                gfx_sp_set_other_mode(C0(8, 8), C0(0, 8), cmd->words.w1);
#endif
                break;
            case (uint8_t)G_SETOTHERMODE_H:
#ifdef F3DEX_GBI_2
                gfx_sp_set_other_mode(63 - C0(8, 8) - C0(0, 8), C0(0, 8) + 1, (uint64_t) cmd->words.w1 << 32);
#else
                gfx_sp_set_other_mode(C0(8, 8) + 32, C0(0, 8), (uint64_t) cmd->words.w1 << 32);
#endif
                break;

            // RDP Commands:
            case G_SETTIMG: {
                uintptr_t i = (uintptr_t) seg_addr(cmd->words.w1);

                char* imgData = (char*)i;

                if ((i & 1) != 1)
                    if (ResourceMgr_OTRSigCheck(imgData) == 1)
                        i = (uintptr_t)ResourceMgr_LoadTexByName(imgData);

                gfx_dp_set_texture_image(C0(21, 3), C0(19, 2), C0(0, 10), (void*) i, imgData);
                break;
            }
            case G_SETTIMG_OTR:
            {
                uintptr_t addr = cmd->words.w1;
                cmd++;
                uint64_t hash = ((uint64_t)cmd->words.w0 << 32) + (uint64_t)cmd->words.w1;
                fileName = ResourceMgr_GetNameByCRC(hash);
#if _DEBUG && 0
                char* tex = ResourceMgr_LoadTexByCRC(hash);
                ResourceMgr_GetNameByCRC(hash, fileName);
                printf("G_SETTIMG_OTR: %s, %08X\n", fileName, hash);
#else
                char* tex = NULL;
#endif

                if (addr != 0)
                {
                    tex = (char*)addr;
                }
                else
                {
                    tex = ResourceMgr_LoadTexByCRC(hash);

                    if (tex != nullptr)
                    {
                        cmd--;
                        uintptr_t oldData = cmd->words.w1;
                        cmd->words.w1 = (uintptr_t)tex;

                        if (ourHash != (uint64_t)-1)
                            ResourceMgr_RegisterResourcePatch(ourHash, cmd - dListStart, oldData);

                        cmd++;
                    }
                }

                cmd--;


                uint32_t fmt = C0(21, 3);
                uint32_t size = C0(19, 2);
                uint32_t width = C0(0, 10);

                if (tex != NULL)
                    gfx_dp_set_texture_image(fmt, size, width, tex, fileName);

                cmd++;
                break;
            }
            case G_SETFB:
            {
                gfx_flush();
                fbActive = 1;
                active_fb = framebuffers.find(cmd->words.w1);
                gfx_rapi->start_draw_to_framebuffer(active_fb->first, (float)active_fb->second.applied_height / active_fb->second.orig_height);
                gfx_rapi->clear_framebuffer();
                break;
            }
            case G_RESETFB:
            {
                gfx_flush();
                fbActive = 0;
                gfx_rapi->start_draw_to_framebuffer(game_renders_to_framebuffer ? game_framebuffer : 0, (float)gfx_current_dimensions.height / SCREEN_HEIGHT);
                break;
            }
            case G_SETTIMG_FB:
            {
                gfx_flush();
                gfx_rapi->select_texture_fb(cmd->words.w1);
                rdp.textures_changed[0] = false;
                rdp.textures_changed[1] = false;

                //if (texPtr != NULL)
                    //gfx_dp_set_texture_image(C0(21, 3), C0(19, 2), C0(0, 10), texPtr);
                break;
            }
            case G_SETGRAYSCALE:
            {
                rdp.grayscale = cmd->words.w1;
                break;
            }
            case G_LOADBLOCK:
                gfx_dp_load_block(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
                break;
            case G_LOADTILE:
                gfx_dp_load_tile(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
                break;
            case G_SETTILE:
                gfx_dp_set_tile(C0(21, 3), C0(19, 2), C0(9, 9), C0(0, 9), C1(24, 3), C1(20, 4), C1(18, 2), C1(14, 4), C1(10, 4), C1(8, 2), C1(4, 4), C1(0, 4));
                break;
            case G_SETTILESIZE:
                gfx_dp_set_tile_size(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
                break;
            case G_LOADTLUT:
                gfx_dp_load_tlut(C1(24, 3), C1(14, 10));
                break;
            case G_SETENVCOLOR:
                gfx_dp_set_env_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
                break;
            case G_SETPRIMCOLOR:
                gfx_dp_set_prim_color(C0(8, 8), C0(0, 8), C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
                break;
            case G_SETFOGCOLOR:
                gfx_dp_set_fog_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
                break;
            case G_SETFILLCOLOR:
                gfx_dp_set_fill_color(cmd->words.w1);
                break;
            case G_SETINTENSITY:
                gfx_dp_set_grayscale_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
                break;
            case G_SETCOMBINE:
                gfx_dp_set_combine_mode(
                    color_comb(C0(20, 4), C1(28, 4), C0(15, 5), C1(15, 3)),
                    alpha_comb(C0(12, 3), C1(12, 3), C0(9, 3), C1(9, 3)),
                    color_comb(C0(5, 4), C1(24, 4), C0(0, 5), C1(6, 3)),
                    alpha_comb(C1(21, 3), C1(3, 3), C1(18, 3), C1(0, 3)));
                break;
            // G_SETPRIMCOLOR, G_CCMUX_PRIMITIVE, G_ACMUX_PRIMITIVE, is used by Goddard
            // G_CCMUX_TEXEL1, LOD_FRACTION is used in Bowser room 1
            case G_TEXRECT:
            case G_TEXRECTFLIP:
            {
                int32_t lrx, lry, tile, ulx, uly;
                uint32_t uls, ult, dsdx, dtdy;
#ifdef F3DEX_GBI_2E
                lrx = (int32_t)(C0(0, 24) << 8) >> 8;
                lry = (int32_t)(C1(0, 24) << 8) >> 8;
                tile = C1(24, 3);
                ++cmd;
                ulx = (int32_t)(C0(0, 24) << 8) >> 8;
                uly = (int32_t)(C1(0, 24) << 8) >> 8;
                ++cmd;
                uls = C0(16, 16);
                ult = C0(0, 16);
                dsdx = C1(16, 16);
                dtdy = C1(0, 16);
#else
                lrx = C0(12, 12);
                lry = C0(0, 12);
                tile = C1(24, 3);
                ulx = C1(12, 12);
                uly = C1(0, 12);
                ++cmd;
                uls = C1(16, 16);
                ult = C1(0, 16);
                ++cmd;
                dsdx = C1(16, 16);
                dtdy = C1(0, 16);
#endif
                gfx_dp_texture_rectangle(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, opcode == G_TEXRECTFLIP);
                break;
            }
            case G_TEXRECT_WIDE:
            {
                int32_t lrx, lry, tile, ulx, uly;
                uint32_t uls, ult, dsdx, dtdy;
                lrx = static_cast<int32_t>((C0(0, 24) << 8)) >> 8;
                lry = static_cast<int32_t>((C1(0, 24) << 8)) >> 8;
                tile = C1(24, 3);
                ++cmd;
                ulx = static_cast<int32_t>((C0(0, 24) << 8)) >> 8;
                uly = static_cast<int32_t>((C1(0, 24) << 8)) >> 8;
                ++cmd;
                uls = C0(16, 16);
                ult = C0(0, 16);
                dsdx = C1(16, 16);
                dtdy = C1(0, 16);
                gfx_dp_texture_rectangle(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, opcode == G_TEXRECTFLIP);
                break;
            }
            case G_FILLRECT:
#ifdef F3DEX_GBI_2E
            {
                int32_t lrx, lry, ulx, uly;
                lrx = (int32_t)(C0(0, 24) << 8) >> 8;
                lry = (int32_t)(C1(0, 24) << 8) >> 8;
                ++cmd;
                ulx = (int32_t)(C0(0, 24) << 8) >> 8;
                uly = (int32_t)(C1(0, 24) << 8) >> 8;
                gfx_dp_fill_rectangle(ulx, uly, lrx, lry);
                break;
            }
#else
                gfx_dp_fill_rectangle(C1(12, 12), C1(0, 12), C0(12, 12), C0(0, 12));
                break;
            case G_FILLWIDERECT:
            {
                int32_t lrx, lry, ulx, uly;
                lrx = (int32_t)(C0(0, 24) << 8) >> 8;
                lry = (int32_t)(C1(0, 24) << 8) >> 8;
                ++cmd;
                ulx = (int32_t)(C0(0, 24) << 8) >> 8;
                uly = (int32_t)(C1(0, 24) << 8) >> 8;
                gfx_dp_fill_rectangle(ulx, uly, lrx, lry);
                break;
            }
#endif
            case G_SETSCISSOR:
                gfx_dp_set_scissor(C1(24, 2), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
                break;
            case G_SETZIMG:
                gfx_dp_set_z_image(seg_addr(cmd->words.w1));
                break;
            case G_SETCIMG:
                gfx_dp_set_color_image(C0(21, 3), C0(19, 2), C0(0, 11), seg_addr(cmd->words.w1));
                break;
            case G_RDPSETOTHERMODE:
                gfx_dp_set_other_mode(C0(0, 24), cmd->words.w1);
                break;
                // S2DEX
            case G_BG_COPY:
                if (!markerOn)
                    gfx_s2dex_bg_copy((const uObjBg*)cmd->words.w1); // not seg_addr here it seems

                break;
        }
        ++cmd;
    }
}

static void gfx_sp_reset() {
    rsp.modelview_matrix_stack_size = 1;
    rsp.current_num_lights = 2;
    rsp.lights_changed = true;
}

void gfx_get_dimensions(uint32_t *width, uint32_t *height) {
    gfx_wapi->get_dimensions(width, height);
}

void gfx_init(struct GfxWindowManagerAPI *wapi, struct GfxRenderingAPI *rapi, const char *game_name, bool start_in_fullscreen, uint32_t width, uint32_t height) {
    gfx_wapi = wapi;
    gfx_rapi = rapi;
    gfx_wapi->init(game_name, start_in_fullscreen, width, height);
    gfx_rapi->init();
    gfx_rapi->update_framebuffer_parameters(0, width, height, 1, false, true, true, true);
#ifdef __APPLE__
    gfx_current_dimensions.internal_mul = 1;
#else
    gfx_current_dimensions.internal_mul = CVar_GetFloat("gInternalResolution", 1);
#endif
    gfx_msaa_level = CVar_GetS32("gMSAAValue", 1);
#ifndef __WIIU__ // Wii U overrides dimentions in gfx_wapi->init to match framebuffer size
    gfx_current_dimensions.width = width;
    gfx_current_dimensions.height = height;
#endif
    game_framebuffer = gfx_rapi->create_framebuffer();
    game_framebuffer_msaa_resolved = gfx_rapi->create_framebuffer();

    for (int i = 0; i < 16; i++)
        segmentPointers[i] = 0;

    // Used in the 120 star TAS
    static uint32_t precomp_shaders[] = {
        0x01200200,
        0x00000045,
        0x00000200,
        0x01200a00,
        0x00000a00,
        0x01a00045,
        0x00000551,
        0x01045045,
        0x05a00a00,
        0x01200045,
        0x05045045,
        0x01045a00,
        0x01a00a00,
        0x0000038d,
        0x01081081,
        0x0120038d,
        0x03200045,
        0x03200a00,
        0x01a00a6f,
        0x01141045,
        0x07a00a00,
        0x05200200,
        0x03200200,
        0x09200200,
        0x0920038d,
        0x09200045
    };
    for (size_t i = 0; i < sizeof(precomp_shaders) / sizeof(uint32_t); i++) {
        //gfx_lookup_or_create_shader_program(precomp_shaders[i]);
    }

    Ship::ExecuteHooks<Ship::GfxInit>();
}

struct GfxRenderingAPI *gfx_get_current_rendering_api(void) {
    return gfx_rapi;
}

void gfx_start_frame(void) {
    gfx_wapi->handle_events();
    gfx_wapi->get_dimensions(&gfx_current_window_dimensions.width, &gfx_current_window_dimensions.height);
    SohImGui::DrawMainMenuAndCalculateGameSize();
    has_drawn_imgui_menu = true;
    if (gfx_current_dimensions.height == 0) {
        // Avoid division by zero
        gfx_current_dimensions.height = 1;
    }
    gfx_current_dimensions.aspect_ratio = (float)gfx_current_dimensions.width / (float)gfx_current_dimensions.height;

    if (gfx_current_dimensions.height != gfx_prev_dimensions.height) {
        for (auto& fb : framebuffers) {
            uint32_t width = fb.second.orig_width, height = fb.second.orig_height;
            gfx_adjust_width_height_for_scale(width, height);
            if (width != fb.second.applied_width || height != fb.second.applied_height) {
                gfx_rapi->update_framebuffer_parameters(fb.first, width, height, 1, true, true, true, true);
                fb.second.applied_width = width;
                fb.second.applied_height = height;
            }
        }
    }
    gfx_prev_dimensions = gfx_current_dimensions;

    bool different_size = gfx_current_dimensions.width != gfx_current_game_window_viewport.width || gfx_current_dimensions.height != gfx_current_game_window_viewport.height;
    if (different_size || gfx_msaa_level > 1) {
        game_renders_to_framebuffer = true;
        if (different_size) {
            gfx_rapi->update_framebuffer_parameters(game_framebuffer, gfx_current_dimensions.width, gfx_current_dimensions.height, gfx_msaa_level, true, true, true, true);
        } else {
            // MSAA framebuffer needs to be resolved to an equally sized target when complete, which must therefore match the window size
            gfx_rapi->update_framebuffer_parameters(game_framebuffer, gfx_current_window_dimensions.width, gfx_current_window_dimensions.height, gfx_msaa_level, false, true, true, true);
        }
        if (gfx_msaa_level > 1 && different_size) {
            gfx_rapi->update_framebuffer_parameters(game_framebuffer_msaa_resolved, gfx_current_dimensions.width, gfx_current_dimensions.height, 1, false, false, false, false);
        }
    } else {
        game_renders_to_framebuffer = false;
    }

    fbActive = 0;
}

void gfx_run(Gfx *commands, const std::unordered_map<Mtx *, MtxF>& mtx_replacements) {
    gfx_sp_reset();

    //puts("New frame");
    get_pixel_depth_pending.clear();
    get_pixel_depth_cached.clear();

    if (!gfx_wapi->start_frame()) {
        dropped_frame = true;
        if (has_drawn_imgui_menu) {
            SohImGui::DrawFramebufferAndGameInput();
            SohImGui::CancelFrame();
            has_drawn_imgui_menu = false;
        }
        return;
    }
    dropped_frame = false;

    if (!has_drawn_imgui_menu) {
        SohImGui::DrawMainMenuAndCalculateGameSize();
    }

    current_mtx_replacements = &mtx_replacements;

    double t0 = gfx_wapi->get_time();
    gfx_rapi->update_framebuffer_parameters(0, gfx_current_window_dimensions.width, gfx_current_window_dimensions.height, 1, false, true, true, !game_renders_to_framebuffer);
    gfx_rapi->start_frame();
    gfx_rapi->start_draw_to_framebuffer(game_renders_to_framebuffer ? game_framebuffer : 0, (float)gfx_current_dimensions.height / SCREEN_HEIGHT);
    gfx_rapi->clear_framebuffer();
    rdp.viewport_or_scissor_changed = true;
    rendering_state.viewport = {};
    rendering_state.scissor = {};
    gfx_run_dl(commands);
    gfx_flush();
    gfxFramebuffer = 0;
    if (game_renders_to_framebuffer) {
        gfx_rapi->start_draw_to_framebuffer(0, 1);
        gfx_rapi->clear_framebuffer();

        if (gfx_msaa_level > 1) {
            bool different_size = gfx_current_dimensions.width != gfx_current_game_window_viewport.width || gfx_current_dimensions.height != gfx_current_game_window_viewport.height;

            if (different_size) {
                gfx_rapi->resolve_msaa_color_buffer(game_framebuffer_msaa_resolved, game_framebuffer);
                gfxFramebuffer = (uintptr_t)gfx_rapi->get_framebuffer_texture_id(game_framebuffer_msaa_resolved);
            } else {
                gfx_rapi->resolve_msaa_color_buffer(0, game_framebuffer);
            }
        } else {
            gfxFramebuffer = (uintptr_t)gfx_rapi->get_framebuffer_texture_id(game_framebuffer);
        }
    }
    SohImGui::DrawFramebufferAndGameInput();
    SohImGui::Render();
    double t1 = gfx_wapi->get_time();
    //printf("Process %f %f\n", t1, t1 - t0);
    gfx_rapi->end_frame();
    gfx_wapi->swap_buffers_begin();
    has_drawn_imgui_menu = false;
}

void gfx_end_frame(void) {
    if (!dropped_frame) {
        gfx_rapi->finish_render();
        gfx_wapi->swap_buffers_end();
    }
}

void gfx_set_target_fps(int fps) {
    gfx_wapi->set_target_fps(fps);
}

void gfx_set_maximum_frame_latency(int latency) {
    gfx_wapi->set_maximum_frame_latency(latency);
}

float gfx_get_detected_hz(void) {
    return gfx_wapi->get_detected_hz();
}

int gfx_create_framebuffer(uint32_t width, uint32_t height) {
    uint32_t orig_width = width, orig_height = height;
    gfx_adjust_width_height_for_scale(width, height);
    int fb = gfx_rapi->create_framebuffer();
    gfx_rapi->update_framebuffer_parameters(fb, width, height, 1, true, true, true, true);
    framebuffers[fb] = { orig_width, orig_height, width, height };
    return fb;
}

void gfx_set_framebuffer(int fb, float noise_scale) {
    gfx_rapi->start_draw_to_framebuffer(fb, noise_scale);
    gfx_rapi->clear_framebuffer();
}

void gfx_reset_framebuffer() {
    gfx_rapi->start_draw_to_framebuffer(0, (float)gfx_current_dimensions.height / SCREEN_HEIGHT);
}

static void adjust_pixel_depth_coordinates(float& x, float& y) {
    x = x * RATIO_Y - (SCREEN_WIDTH * RATIO_Y - gfx_current_dimensions.width) / 2;
    y *= RATIO_Y;
    if (!game_renders_to_framebuffer || (gfx_msaa_level > 1 && gfx_current_dimensions.width == gfx_current_game_window_viewport.width && gfx_current_dimensions.height == gfx_current_game_window_viewport.height)) {
        x += gfx_current_game_window_viewport.x;
        y += gfx_current_window_dimensions.height - (gfx_current_game_window_viewport.y + gfx_current_game_window_viewport.height);
    }
}

void gfx_get_pixel_depth_prepare(float x, float y) {
    adjust_pixel_depth_coordinates(x, y);
    get_pixel_depth_pending.emplace(x, y);
}

uint16_t gfx_get_pixel_depth(float x, float y) {
    adjust_pixel_depth_coordinates(x, y);

    if (auto it = get_pixel_depth_cached.find(make_pair(x, y)); it != get_pixel_depth_cached.end()) {
        return it->second;
    }

    get_pixel_depth_pending.emplace(x, y);

    unordered_map<pair<float, float>, uint16_t, hash_pair_ff> res = gfx_rapi->get_pixel_depth(game_renders_to_framebuffer ? game_framebuffer : 0, get_pixel_depth_pending);
    get_pixel_depth_cached.merge(res);
    get_pixel_depth_pending.clear();

    return get_pixel_depth_cached.find(make_pair(x, y))->second;
}
