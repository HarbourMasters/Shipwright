/*  gfx_gx2.cpp - Fast3D GX2 backend for libultraship

    Created in 2022 by GaryOderNichts
*/
#ifdef __WIIU__

#include "../../Window.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>

#include <map>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include "PR/ultra64/gbi.h"

#include "gfx_cc.h"
#include "gfx_rendering_api.h"
#include "gfx_pc.h"
#include "gfx_wiiu.h"

#include <gx2/texture.h>
#include <gx2/draw.h>
#include <gx2/clear.h>
#include <gx2/state.h>
#include <gx2/swap.h>
#include <gx2/event.h>
#include <gx2/utils.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/display.h>
#include "gx2_shader_gen.h"

#include <proc_ui/procui.h>
#include <coreinit/memory.h>

#include "../ImGui/backends/wiiu/imgui_impl_gx2.h"

#define ALIGN(x, align) (((x) + ((align) -1)) & ~((align) -1))

struct ShaderProgram {
    struct ShaderGroup group;
    uint8_t num_inputs;
    bool used_textures[2];
    bool used_noise;
    uint32_t window_params_offset;
    uint32_t samplers_location[2];
};

struct Texture {
    GX2Texture texture;
    bool texture_uploaded;

    GX2Sampler sampler;
    bool sampler_set;

    // For ImGui rendering
    ImGui_ImplGX2_Texture imtex;
};

struct Framebuffer {
    GX2ColorBuffer color_buffer;
    bool colorBufferMem1;
    GX2DepthBuffer depth_buffer;
    bool depthBufferMem1;

    GX2Texture texture;
    GX2Sampler sampler;

    // For ImGui rendering
    ImGui_ImplGX2_Texture imtex;
};

static struct Framebuffer main_framebuffer;
static GX2DepthBuffer depthReadBuffer;
static struct Framebuffer *current_framebuffer;

static std::map<std::pair<uint64_t, uint32_t>, struct ShaderProgram> shader_program_pool;
static struct ShaderProgram *current_shader_program;

static struct Texture *current_texture;
static int current_tile;

// 96 Mb (should be more than enough to draw everything without waiting for the GPU)
#define DRAW_BUFFER_SIZE 0x6000000
static uint8_t *draw_buffer = nullptr;
static uint8_t *draw_ptr = nullptr;

static uint32_t frame_count;
static float current_noise_scale;
static FilteringMode current_filter_mode = FILTER_LINEAR;

static BOOL current_depth_test = TRUE;
static BOOL current_depth_write = TRUE;
static GX2CompareFunction current_depth_compare_function = GX2_COMPARE_FUNC_LESS;

static float current_viewport_x = 0.0f;
static float current_viewport_y = 0.0f;
static float current_viewport_width = WIIU_DEFAULT_FB_WIDTH;
static float current_viewport_height = WIIU_DEFAULT_FB_HEIGHT;

static uint32_t current_scissor_x = 0;
static uint32_t current_scissor_y = 0;
static uint32_t current_scissor_width = WIIU_DEFAULT_FB_WIDTH;
static uint32_t current_scissor_height = WIIU_DEFAULT_FB_HEIGHT;

static bool current_zmode_decal = false;
static bool current_use_alpha = false;

static inline GX2SamplerVar *GX2GetPixelSamplerVar(const GX2PixelShader *shader, const char *name)
{
    for (uint32_t i = 0; i < shader->samplerVarCount; ++i) {
        if (strcmp(name, shader->samplerVars[i].name) == 0) {
            return &shader->samplerVars[i];
        }
    }

    return nullptr;
}

static inline int32_t GX2GetPixelSamplerVarLocation(const GX2PixelShader *shader, const char *name)
{
    GX2SamplerVar *sampler = GX2GetPixelSamplerVar(shader, name);
    return sampler ? sampler->location : -1;
}

static inline int32_t GX2GetPixelUniformVarOffset(const GX2PixelShader *shader, const char *name)
{
    GX2UniformVar *uniform = GX2GetPixelUniformVar(shader, name);
    return uniform ? uniform->offset : -1;
}

static void gfx_gx2_init_framebuffer(struct Framebuffer *buffer, uint32_t width, uint32_t height) {
    memset(&buffer->color_buffer, 0, sizeof(GX2ColorBuffer));
    buffer->color_buffer.surface.use = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    buffer->color_buffer.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer->color_buffer.surface.width = width;
    buffer->color_buffer.surface.height = height;
    buffer->color_buffer.surface.depth = 1;
    buffer->color_buffer.surface.mipLevels = 1;
    buffer->color_buffer.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
    buffer->color_buffer.surface.aa = GX2_AA_MODE1X;
    buffer->color_buffer.surface.tileMode = GX2_TILE_MODE_DEFAULT;
    buffer->color_buffer.viewNumSlices = 1;

    memset(&buffer->depth_buffer, 0, sizeof(GX2DepthBuffer));
    buffer->depth_buffer.surface.use = GX2_SURFACE_USE_DEPTH_BUFFER | GX2_SURFACE_USE_TEXTURE;
    buffer->depth_buffer.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer->depth_buffer.surface.width = width;
    buffer->depth_buffer.surface.height = height;
    buffer->depth_buffer.surface.depth = 1;
    buffer->depth_buffer.surface.mipLevels = 1;
    buffer->depth_buffer.surface.format = GX2_SURFACE_FORMAT_FLOAT_R32;
    buffer->depth_buffer.surface.aa = GX2_AA_MODE1X;
    buffer->depth_buffer.surface.tileMode = GX2_TILE_MODE_DEFAULT;
    buffer->depth_buffer.viewNumSlices = 1;
    buffer->depth_buffer.depthClear = 1.0f;
}

static struct GfxClipParameters gfx_gx2_get_clip_parameters(void) {
    return { false, false };
}

static void gfx_gx2_set_uniforms(struct ShaderProgram *prg) {
    if (prg->used_noise) {
        float window_params_array[2] = { current_noise_scale, (float) frame_count };

        GX2SetPixelUniformReg(prg->window_params_offset, 2, window_params_array);
    }
}

static void gfx_gx2_unload_shader(struct ShaderProgram *old_prg) {
    current_shader_program = nullptr;
}

static void gfx_gx2_load_shader(struct ShaderProgram *new_prg) {
    current_shader_program = new_prg;

    GX2SetFetchShader(&new_prg->group.fetchShader);
    GX2SetVertexShader(&new_prg->group.vertexShader);
    GX2SetPixelShader(&new_prg->group.pixelShader);

    gfx_gx2_set_uniforms(new_prg);
}

static struct ShaderProgram* gfx_gx2_create_and_load_new_shader(uint64_t shader_id0, uint32_t shader_id1) {
    struct CCFeatures cc_features;
    gfx_cc_get_features(shader_id0, shader_id1, &cc_features);

    struct ShaderProgram* prg = &shader_program_pool[std::make_pair(shader_id0, shader_id1)];

    printf("Generating shader: %016llx-%08x\n", shader_id0, shader_id1);
    if (gx2GenerateShaderGroup(&prg->group, &cc_features) != 0) {
        printf("Failed to generate shader\n");
        current_shader_program = nullptr;
        return nullptr;
    }

    prg->num_inputs = cc_features.num_inputs;
    prg->used_textures[0] = cc_features.used_textures[0];
    prg->used_textures[1] = cc_features.used_textures[1];

    gfx_gx2_load_shader(prg);

    prg->window_params_offset = GX2GetPixelUniformVarOffset(&prg->group.pixelShader, "window_params");
    prg->samplers_location[0] = GX2GetPixelSamplerVarLocation(&prg->group.pixelShader, "uTex0");
    prg->samplers_location[1] = GX2GetPixelSamplerVarLocation(&prg->group.pixelShader, "uTex1");

    prg->used_noise = cc_features.opt_alpha && cc_features.opt_noise;

    printf("Generated and loaded shader\n");

    return prg;
}

static struct ShaderProgram *gfx_gx2_lookup_shader(uint64_t shader_id0, uint32_t shader_id1) {
    auto it = shader_program_pool.find(std::make_pair(shader_id0, shader_id1));
    return it == shader_program_pool.end() ? nullptr : &it->second;
}

static void gfx_gx2_shader_get_info(struct ShaderProgram *prg, uint8_t *num_inputs, bool used_textures[2]) {
    *num_inputs = prg->num_inputs;
    used_textures[0] = prg->used_textures[0];
    used_textures[1] = prg->used_textures[1];
}

static uint32_t gfx_gx2_new_texture(void) {
    // some 32-bit trickery :P
    struct Texture *tex = (struct Texture *) calloc(1, sizeof(struct Texture));

    tex->imtex.Texture = &tex->texture;
    tex->imtex.Sampler = &tex->sampler;

    return (uint32_t) tex;
}

static void gfx_gx2_delete_texture(uint32_t texture_id) {
    struct Texture *tex = (struct Texture *) texture_id;

    if (tex->texture.surface.image) {
        free(tex->texture.surface.image);
    }

    free((void *) tex);
}

static void gfx_gx2_select_texture(int tile, uint32_t texture_id) {
    struct Texture *tex = (struct Texture *) texture_id;
    current_texture = tex;
    current_tile = tile;

    if (current_shader_program) {
        uint32_t sampler_location = current_shader_program->samplers_location[tile];

        if (tex->texture_uploaded) {
            GX2SetPixelTexture(&tex->texture, sampler_location);
        }

        if (tex->sampler_set) {
            GX2SetPixelSampler(&tex->sampler, sampler_location);
        }
    }
}

static void gfx_gx2_upload_texture(const uint8_t *rgba32_buf, uint32_t width, uint32_t height) {
    struct Texture *tex = current_texture;
    assert(tex);

    if ((tex->texture.surface.width != width) ||
        (tex->texture.surface.height != height) || 
        !tex->texture.surface.image) {

        if (tex->texture.surface.image) {
            free(tex->texture.surface.image);
            tex->texture.surface.image = nullptr;
        }

        memset(&tex->texture, 0, sizeof(GX2Texture));
        tex->texture.surface.use = GX2_SURFACE_USE_TEXTURE;
        tex->texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
        tex->texture.surface.width = width;
        tex->texture.surface.height = height;
        tex->texture.surface.depth = 1;
        tex->texture.surface.mipLevels = 1;
        tex->texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        tex->texture.surface.aa = GX2_AA_MODE1X;
        tex->texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
        tex->texture.viewFirstMip = 0;
        tex->texture.viewNumMips = 1;
        tex->texture.viewFirstSlice = 0;
        tex->texture.viewNumSlices = 1;
        tex->texture.compMap = GX2_COMP_MAP(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A);

        GX2CalcSurfaceSizeAndAlignment(&tex->texture.surface);
        GX2InitTextureRegs(&tex->texture);

        tex->texture.surface.image = memalign(tex->texture.surface.alignment, tex->texture.surface.imageSize);
    }

    uint8_t* buf = (uint8_t *) tex->texture.surface.image;
    assert(buf);

    for (uint32_t y = 0; y < height; ++y) {
        memcpy(buf + (y * tex->texture.surface.pitch * 4), rgba32_buf + (y * width * 4), width * 4);
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, tex->texture.surface.image, tex->texture.surface.imageSize);

    if (current_shader_program) {
        GX2SetPixelTexture(&tex->texture, current_shader_program->samplers_location[current_tile]);
    }

    tex->texture_uploaded = true;
}

static GX2TexClampMode gfx_cm_to_gx2(uint32_t val) {
    switch (val) {
        case G_TX_NOMIRROR | G_TX_CLAMP:
            return GX2_TEX_CLAMP_MODE_CLAMP;
        case G_TX_MIRROR | G_TX_WRAP:
            return GX2_TEX_CLAMP_MODE_MIRROR;
        case G_TX_MIRROR | G_TX_CLAMP:
            return GX2_TEX_CLAMP_MODE_MIRROR_ONCE;
        case G_TX_NOMIRROR | G_TX_WRAP:
            return GX2_TEX_CLAMP_MODE_WRAP;
    }

    return GX2_TEX_CLAMP_MODE_WRAP;
}

static void gfx_gx2_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    struct Texture *tex = current_texture;
    assert(tex);

    current_tile = tile;

    GX2InitSampler(&tex->sampler, GX2_TEX_CLAMP_MODE_CLAMP,
        (linear_filter && current_filter_mode == FILTER_LINEAR) ? 
        GX2_TEX_XY_FILTER_MODE_LINEAR : GX2_TEX_XY_FILTER_MODE_POINT);

    GX2InitSamplerClamping(&tex->sampler, gfx_cm_to_gx2(cms), gfx_cm_to_gx2(cmt), GX2_TEX_CLAMP_MODE_WRAP);

    if (current_shader_program) {
        GX2SetPixelSampler(&tex->sampler, current_shader_program->samplers_location[tile]);
    }

    tex->sampler_set = true;
}

static void gfx_gx2_set_depth_test_and_mask(bool depth_test, bool z_upd) {
    current_depth_test = depth_test || z_upd;
    current_depth_write = z_upd;
    current_depth_compare_function = depth_test ? GX2_COMPARE_FUNC_LEQUAL : GX2_COMPARE_FUNC_ALWAYS;

    GX2SetDepthOnlyControl(current_depth_test, current_depth_write, current_depth_compare_function);
}

static void gfx_gx2_set_zmode_decal(bool zmode_decal) {
    current_zmode_decal = zmode_decal;
    if (zmode_decal) {
        GX2SetPolygonControl(GX2_FRONT_FACE_CCW, FALSE, FALSE, TRUE,
                             GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE,
                             TRUE, TRUE, FALSE);
        GX2SetPolygonOffset(-2.0f, -2.0f, -2.0f, -2.0f, 0.0f);
    } else {
        GX2SetPolygonControl(GX2_FRONT_FACE_CCW, FALSE, FALSE, FALSE,
                             GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE,
                             FALSE, FALSE, FALSE);
        GX2SetPolygonOffset(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

static void gfx_gx2_set_viewport(int x, int y, int width, int height) {
    uint32_t buffer_height = current_framebuffer->color_buffer.surface.height;

    current_viewport_x = x;
    current_viewport_y = buffer_height - y - height;
    current_viewport_width = width;
    current_viewport_height = height;

    GX2SetViewport(current_viewport_x, current_viewport_y, current_viewport_width, current_viewport_height, 0.0f, 1.0f);
}

static void gfx_gx2_set_scissor(int x, int y, int width, int height) {
    uint32_t buffer_height = current_framebuffer->color_buffer.surface.height;
    uint32_t buffer_width = current_framebuffer->color_buffer.surface.width;

    current_scissor_x = std::min((uint32_t) width, (uint32_t) x);
    current_scissor_y = std::min((uint32_t) height, buffer_height - y - height);
    current_scissor_width = std::min((uint32_t) width, buffer_width);
    current_scissor_height = std::min((uint32_t) height, buffer_height);

    GX2SetScissor(current_scissor_x, current_scissor_y, current_scissor_width, current_scissor_height);
}

static void gfx_gx2_set_use_alpha(bool use_alpha) {
    current_use_alpha = use_alpha;
    GX2SetColorControl(GX2_LOGIC_OP_COPY, use_alpha ? 0xff : 0, FALSE, TRUE);
}

static void gfx_gx2_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {
    if (!current_shader_program) {
        return;
    }

    size_t vbo_len = sizeof(float) * buf_vbo_len;

    if (draw_ptr + vbo_len >= draw_buffer + DRAW_BUFFER_SIZE) {
        printf("Waiting on GPU!!!\n");
        GX2DrawDone();
        draw_ptr = draw_buffer;
    }

    float* new_vbo = (float *) draw_ptr;
    draw_ptr += ALIGN(vbo_len, GX2_VERTEX_BUFFER_ALIGNMENT);

    OSBlockMove(new_vbo, buf_vbo, vbo_len, FALSE);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, new_vbo, vbo_len);

    GX2SetAttribBuffer(0, vbo_len, current_shader_program->group.stride, new_vbo);
    GX2DrawEx(GX2_PRIMITIVE_MODE_TRIANGLES, 3 * buf_vbo_num_tris, 0, 1);
}

static void gfx_gx2_init(void) {
    // Init the default framebuffer
    gfx_gx2_init_framebuffer(&main_framebuffer, WIIU_DEFAULT_FB_WIDTH, WIIU_DEFAULT_FB_HEIGHT);

    GX2CalcSurfaceSizeAndAlignment(&main_framebuffer.color_buffer.surface);
    GX2InitColorBufferRegs(&main_framebuffer.color_buffer);

    main_framebuffer.color_buffer.surface.image = gfx_wiiu_alloc_mem1(main_framebuffer.color_buffer.surface.imageSize, main_framebuffer.color_buffer.surface.alignment);
    assert(main_framebuffer.color_buffer.surface.image);

    GX2CalcSurfaceSizeAndAlignment(&main_framebuffer.depth_buffer.surface);
    GX2InitDepthBufferRegs(&main_framebuffer.depth_buffer);

    main_framebuffer.depth_buffer.surface.image = gfx_wiiu_alloc_mem1(main_framebuffer.depth_buffer.surface.imageSize, main_framebuffer.depth_buffer.surface.alignment);
    assert(main_framebuffer.depth_buffer.surface.image);

    main_framebuffer.imtex.Texture = &main_framebuffer.texture;
    main_framebuffer.imtex.Sampler = &main_framebuffer.sampler;

    // create a linear aligned copy of the depth buffer to read pixels to
    memcpy(&depthReadBuffer, &main_framebuffer.depth_buffer, sizeof(GX2DepthBuffer));

    depthReadBuffer.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
    depthReadBuffer.surface.width = 32;
    depthReadBuffer.surface.height = 1;

    GX2CalcSurfaceSizeAndAlignment(&depthReadBuffer.surface);

    depthReadBuffer.surface.image = gfx_wiiu_alloc_mem1(depthReadBuffer.surface.alignment, depthReadBuffer.surface.imageSize);
    assert(depthReadBuffer.surface.image);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_DEPTH_BUFFER, depthReadBuffer.surface.image, depthReadBuffer.surface.imageSize);

    GX2SetColorBuffer(&main_framebuffer.color_buffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&main_framebuffer.depth_buffer);

    current_framebuffer = &main_framebuffer;

    // allocate draw buffer
    draw_buffer = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, DRAW_BUFFER_SIZE);
    assert(draw_buffer);
    draw_ptr = draw_buffer;

    GX2SetRasterizerClipControl(TRUE, FALSE);

    GX2SetBlendControl(GX2_RENDER_TARGET_0,
                       GX2_BLEND_MODE_SRC_ALPHA,
                       GX2_BLEND_MODE_INV_SRC_ALPHA,
                       GX2_BLEND_COMBINE_MODE_ADD,
                       FALSE,
                       GX2_BLEND_MODE_ZERO,
                       GX2_BLEND_MODE_ZERO,
                       GX2_BLEND_COMBINE_MODE_ADD);
}

void gfx_gx2_shutdown(void) {
    if (has_foreground) {
        GX2DrawDone();

        if (depthReadBuffer.surface.image) {
            gfx_wiiu_free_mem1(depthReadBuffer.surface.image);
            depthReadBuffer.surface.image = nullptr;
        }

        if (main_framebuffer.color_buffer.surface.image) {
            gfx_wiiu_free_mem1(main_framebuffer.color_buffer.surface.image);
            main_framebuffer.color_buffer.surface.image = nullptr;
        }

        if (main_framebuffer.depth_buffer.surface.image) {
            gfx_wiiu_free_mem1(main_framebuffer.depth_buffer.surface.image);
            main_framebuffer.depth_buffer.surface.image = nullptr;
        }
    }

    if (draw_buffer) {
        free(draw_buffer);
        draw_buffer = nullptr;
        draw_ptr = nullptr;
    }
}

static void gfx_gx2_on_resize(void) {
}

static void gfx_gx2_start_frame(void) {
    // Restore state since ImGui modified it when rendering
    GX2SetViewport(current_viewport_x, current_viewport_y, current_viewport_width, current_viewport_height, 0.0f, 1.0f);
    GX2SetScissor(current_scissor_x, current_scissor_y, current_scissor_width, current_scissor_height);

    GX2SetColorControl(GX2_LOGIC_OP_COPY, current_use_alpha ? 0xff : 0, FALSE, TRUE);

    GX2SetBlendControl(GX2_RENDER_TARGET_0,
                       GX2_BLEND_MODE_SRC_ALPHA,
                       GX2_BLEND_MODE_INV_SRC_ALPHA,
                       GX2_BLEND_COMBINE_MODE_ADD,
                       FALSE,
                       GX2_BLEND_MODE_ZERO,
                       GX2_BLEND_MODE_ZERO,
                       GX2_BLEND_COMBINE_MODE_ADD);

    GX2SetDepthOnlyControl(current_depth_test, current_depth_write, current_depth_compare_function);

    if (current_zmode_decal) {
        GX2SetPolygonControl(GX2_FRONT_FACE_CCW, FALSE, FALSE, TRUE,
                             GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE,
                             TRUE, TRUE, FALSE);
        GX2SetPolygonOffset(-2.0f, -2.0f, -2.0f, -2.0f, 0.0f);
    } else {
        GX2SetPolygonControl(GX2_FRONT_FACE_CCW, FALSE, FALSE, FALSE,
                             GX2_POLYGON_MODE_TRIANGLE, GX2_POLYGON_MODE_TRIANGLE,
                             FALSE, FALSE, FALSE);
        GX2SetPolygonOffset(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    frame_count++;
}

static void gfx_gx2_end_frame(void) {
    draw_ptr = draw_buffer;

    GX2CopyColorBufferToScanBuffer(&main_framebuffer.color_buffer, GX2_SCAN_TARGET_TV);
    GX2CopyColorBufferToScanBuffer(&main_framebuffer.color_buffer, GX2_SCAN_TARGET_DRC);
}

static void gfx_gx2_finish_render(void) {
}

static int gfx_gx2_create_framebuffer(void) {
    struct Framebuffer *buffer = (struct Framebuffer *) calloc(1, sizeof(struct Framebuffer));
    assert(buffer);

    GX2InitSampler(&buffer->sampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_LINEAR);

    buffer->imtex.Texture = &buffer->texture;
    buffer->imtex.Sampler = &buffer->sampler;

    // some more 32-bit shenanigans :D
    return (int) buffer;
}

static void gfx_gx2_update_framebuffer_parameters(int fb, uint32_t width, uint32_t height, uint32_t msaa_level, bool opengl_invert_y, bool render_target, bool has_depth_buffer, bool can_extract_depth) {
    struct Framebuffer *buffer = (struct Framebuffer *) fb;

    // we don't support updating the main buffer (fb 0)
    if (!buffer) {
        return;
    }

    if (buffer->texture.surface.width == width &&
        buffer->texture.surface.height == height) {
        return;
    }

    // make sure the GPU no longer writes to the buffer
    GX2DrawDone();

    if (buffer->texture.surface.image) {
        if (buffer->colorBufferMem1) {
            gfx_wiiu_free_mem1(buffer->texture.surface.image);
        } else {
            free(buffer->texture.surface.image);
        }
        buffer->texture.surface.image = nullptr;
    }

    if (buffer->depth_buffer.surface.image) {
        if (buffer->depthBufferMem1) {
            gfx_wiiu_free_mem1(buffer->depth_buffer.surface.image);
        } else {
            free(buffer->depth_buffer.surface.image);
        }
        buffer->depth_buffer.surface.image = nullptr;
    }

    gfx_gx2_init_framebuffer(buffer, width, height);

    GX2CalcSurfaceSizeAndAlignment(&buffer->depth_buffer.surface);
    GX2InitDepthBufferRegs(&buffer->depth_buffer);

    buffer->depth_buffer.surface.image = gfx_wiiu_alloc_mem1(buffer->depth_buffer.surface.imageSize, buffer->depth_buffer.surface.alignment);
    // fall back to mem2
    if (!buffer->depth_buffer.surface.image) {
        buffer->depth_buffer.surface.image = memalign(buffer->depth_buffer.surface.alignment, buffer->depth_buffer.surface.imageSize);
        buffer->depthBufferMem1 = false;
    } else {
        buffer->depthBufferMem1 = true;
    }
    assert(buffer->depth_buffer.surface.image);

    GX2CalcSurfaceSizeAndAlignment(&buffer->color_buffer.surface);
    GX2InitColorBufferRegs(&buffer->color_buffer);

    memset(&buffer->texture, 0, sizeof(GX2Texture));
    buffer->texture.surface.use = GX2_SURFACE_USE_TEXTURE;
    buffer->texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer->texture.surface.width = width;
    buffer->texture.surface.height = height;
    buffer->texture.surface.depth = 1;
    buffer->texture.surface.mipLevels = 1;
    buffer->texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
    buffer->texture.surface.aa = GX2_AA_MODE1X;
    buffer->texture.surface.tileMode = GX2_TILE_MODE_DEFAULT;
    buffer->texture.viewFirstMip = 0;
    buffer->texture.viewNumMips = 1;
    buffer->texture.viewFirstSlice = 0;
    buffer->texture.viewNumSlices = 1;
    buffer->texture.compMap = GX2_COMP_MAP(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A);

    GX2CalcSurfaceSizeAndAlignment(&buffer->texture.surface);
    GX2InitTextureRegs(&buffer->texture);

    // the texture and color buffer share a buffer
    assert(buffer->color_buffer.surface.imageSize == buffer->texture.surface.imageSize);

    buffer->texture.surface.image = gfx_wiiu_alloc_mem1(buffer->texture.surface.imageSize, buffer->texture.surface.alignment);
    // fall back to mem2
    if (!buffer->texture.surface.image) {
        buffer->texture.surface.image = memalign(buffer->texture.surface.alignment, buffer->texture.surface.imageSize);
        buffer->colorBufferMem1 = false;
    } else {
        buffer->colorBufferMem1 = true;
    }
    assert(buffer->texture.surface.image);

    buffer->color_buffer.surface.image = buffer->texture.surface.image;
}

void gfx_gx2_start_draw_to_framebuffer(int fb, float noise_scale) {
    struct Framebuffer *buffer = (struct Framebuffer *) fb;

    // fb 0 = main buffer
    if (!buffer) {
        buffer = &main_framebuffer;
    }

    if (noise_scale != 0.0f) {
        current_noise_scale = 1.0f / noise_scale;
    }

    GX2SetColorBuffer(&buffer->color_buffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&buffer->depth_buffer);

    current_framebuffer = buffer;
}

void gfx_gx2_clear_framebuffer(void) {
    struct Framebuffer *buffer = current_framebuffer;

    GX2ClearColor(&buffer->color_buffer, 0.0f, 0.0f, 0.0f, 1.0f);
    GX2ClearDepthStencilEx(&buffer->depth_buffer, 
        buffer->depth_buffer.depthClear,
        buffer->depth_buffer.stencilClear, GX2_CLEAR_FLAGS_BOTH);
    
    gfx_wiiu_set_context_state();
}

void gfx_gx2_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    struct Framebuffer *src_buffer = (struct Framebuffer *) fb_id_source;
    struct Framebuffer *target_buffer = (struct Framebuffer *) fb_id_target;

    // fb 0 = main buffer
    if (!src_buffer) {
        src_buffer = &main_framebuffer;
    }
    if (!target_buffer) {
        target_buffer = &main_framebuffer;
    }

    if (src_buffer->color_buffer.surface.aa == GX2_AA_MODE1X) {
        GX2CopySurface(&src_buffer->color_buffer.surface,
            src_buffer->color_buffer.viewMip,
            src_buffer->color_buffer.viewFirstSlice,
            &target_buffer->color_buffer.surface,
            target_buffer->color_buffer.viewMip,
            target_buffer->color_buffer.viewFirstSlice);
    } else {
        GX2ResolveAAColorBuffer(&src_buffer->color_buffer,
            &target_buffer->color_buffer.surface,
            target_buffer->color_buffer.viewMip,
            target_buffer->color_buffer.viewFirstSlice);
    }
}

void *gfx_gx2_get_framebuffer_texture_id(int fb_id) {
    struct Framebuffer *buffer = (struct Framebuffer *) fb_id;

    // fb 0 = main buffer
    if (!buffer) {
        buffer = &main_framebuffer;
    }

    return &buffer->imtex;
}

void gfx_gx2_select_texture_fb(int fb) {
    struct Framebuffer *buffer = (struct Framebuffer *) fb;
    assert(buffer);

    assert(current_shader_program);
    uint32_t location = current_shader_program->samplers_location[0];
    GX2SetPixelTexture(&buffer->texture, location);
    GX2SetPixelSampler(&buffer->sampler, location);
}

static std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> gfx_gx2_get_pixel_depth(int fb_id, const std::set<std::pair<float, float>>& coordinates) {
    struct Framebuffer *buffer = (struct Framebuffer *) fb_id;

    // fb 0 = main buffer
    if (!buffer) {
        buffer = &main_framebuffer;
    }

    std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> res;
    GX2Rect srcRects[25];
    GX2Point dstPoints[25];
    size_t num_coordinates = coordinates.size();
    while (num_coordinates > 0) {
        size_t numRects = 25;
        if (num_coordinates < numRects) {
            numRects = num_coordinates;
        }
        num_coordinates -= numRects;

        // initialize rects and points
        for (size_t i = 0; i < numRects; ++i) {
            const auto& c = *std::next(coordinates.begin(), num_coordinates + i);
            const int32_t x = (int32_t) std::clamp(c.first, 0.0f, (float) (buffer->depth_buffer.surface.width - 1));
            const int32_t y = (int32_t) std::clamp(c.second, 0.0f, (float) (buffer->depth_buffer.surface.height - 1));

            srcRects[i] = GX2Rect{
                x,
                (int32_t) buffer->depth_buffer.surface.height - y,
                x + 1,
                (int32_t) (buffer->depth_buffer.surface.height - y) + 1
            };

            // dst points will be spread over the x-axis of the buffer
            dstPoints[i] = GX2Point{ i, 0 };
        }

        // Invalidate the buffer first
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_DEPTH_BUFFER, depthReadBuffer.surface.image, depthReadBuffer.surface.imageSize);

        // Perform the copy
        GX2CopySurfaceEx(&buffer->depth_buffer.surface, 0, 0, &depthReadBuffer.surface, 0, 0, numRects, srcRects, dstPoints);

        // Wait for draws to be done and restore context, in case GPU was used
        GX2DrawDone();
        gfx_wiiu_set_context_state();

        // read the pixels from the depthReadBuffer
        for (size_t i = 0; i < numRects; ++i) {
            uint32_t tmp = __builtin_bswap32(*((uint32_t *)depthReadBuffer.surface.image + i));
            float val = *(float *)&tmp;

            const auto& c = *std::next(coordinates.begin(), num_coordinates + i);
            res.emplace(c, val * 65532.0f);
        }
    }

    return res;
}

void gfx_gx2_set_texture_filter(FilteringMode mode) {
    // three-point is not implemented in the shaders yet
    if (mode == FILTER_THREE_POINT) {
        mode = FILTER_LINEAR;
    }

    current_filter_mode = mode;
    gfx_texture_cache_clear();
}

FilteringMode gfx_gx2_get_texture_filter(void) {
    return current_filter_mode;
}

ImGui_ImplGX2_Texture* gfx_gx2_texture_for_imgui(uint32_t texture_id) {
    struct Texture *tex = (struct Texture *) texture_id;
    return &tex->imtex;
}

struct GfxRenderingAPI gfx_gx2_api = {
    gfx_gx2_get_clip_parameters,
    gfx_gx2_unload_shader,
    gfx_gx2_load_shader,
    gfx_gx2_create_and_load_new_shader,
    gfx_gx2_lookup_shader,
    gfx_gx2_shader_get_info,
    gfx_gx2_new_texture,
    gfx_gx2_select_texture,
    gfx_gx2_upload_texture,
    gfx_gx2_set_sampler_parameters,
    gfx_gx2_set_depth_test_and_mask,
    gfx_gx2_set_zmode_decal,
    gfx_gx2_set_viewport,
    gfx_gx2_set_scissor,
    gfx_gx2_set_use_alpha,
    gfx_gx2_draw_triangles,
    gfx_gx2_init,
    gfx_gx2_on_resize,
    gfx_gx2_start_frame,
    gfx_gx2_end_frame,
    gfx_gx2_finish_render,
    gfx_gx2_create_framebuffer,
    gfx_gx2_update_framebuffer_parameters,
    gfx_gx2_start_draw_to_framebuffer,
    gfx_gx2_clear_framebuffer,
    gfx_gx2_resolve_msaa_color_buffer,
    gfx_gx2_get_pixel_depth,
    gfx_gx2_get_framebuffer_texture_id,
    gfx_gx2_select_texture_fb,
    gfx_gx2_delete_texture,
    gfx_gx2_set_texture_filter,
    gfx_gx2_get_texture_filter
};

#endif
