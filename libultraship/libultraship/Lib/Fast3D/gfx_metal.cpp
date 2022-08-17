//
//  gfx_metal.cpp
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef ENABLE_METAL

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <time.h>
#include <math.h>
#include <cmath>
#include <stddef.h>
#include <simd/simd.h>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <SDL_render.h>

#include "gfx_metal_bridge.hpp"
#include "gfx_metal_shader.hpp"

#include "Lib/ImGui/backends/imgui_impl_metal.h"

#include "PR/ultra64/gbi.h"
#include "PR/ultra64/abi.h"

#include "gfx_cc.h"
#include "gfx_rendering_api.h"

#include "gfx_pc.h"
#include "spdlog/spdlog.h"
#include "Cvar.h"

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

static constexpr size_t kMaxBufferPoolSize = 5;
static constexpr NS::UInteger METAL_MAX_MULTISAMPLE_SAMPLE_COUNT = 8;
static constexpr NS::UInteger MAX_PIXEL_DEPTH_COORDS = 1024;

// MARK: - Hashing Helpers

int cantor(uint64_t a, uint64_t b) {
    return (a + b + 1.0) * (a + b) / 2 + b;
}

struct hash_pair_shader_ids {
    size_t operator()(const std::pair<uint64_t, uint32_t> &p ) const {
        auto value1 = p.first;
        auto value2 = p.second;
        return cantor(value1, value2);
    }
};

// MARK: - Structs

struct ShaderProgramMetal {
    uint64_t shader_id0;
    uint32_t shader_id1;

    uint8_t num_inputs;
    uint8_t num_floats;
    bool used_textures[2];

    // hashed by msaa_level
    MTL::RenderPipelineState* pipeline_state_variants[9];
};

struct FrameUniforms {
    simd::int1 frameCount;
    simd::float1 noiseScale;
};

struct CoordUniforms {
    simd::uint2 coords[MAX_PIXEL_DEPTH_COORDS];
};

struct TextureDataMetal {
    MTL::Texture* texture;
    MTL::Texture* msaaTexture;
    MTL::SamplerState* sampler;
    uint32_t width;
    uint32_t height;
    bool linear_filtering;
};

struct FramebufferMetal {
    MTL::CommandBuffer* command_buffer;
    MTL::RenderPassDescriptor* render_pass_descriptor;
    MTL::RenderCommandEncoder* command_encoder;

    MTL::Texture* depth_texture;
    MTL::Texture* msaa_depth_texture;
    uint32_t texture_id;
    bool has_depth_buffer;
    uint32_t msaa_level;
    bool render_target;

    // State
    bool has_ended_encoding;
    bool has_bounded_vertex_buffer;
    bool has_bounded_fragment_buffer;

    struct ShaderProgramMetal* last_shader_program;
    MTL::Texture* last_bound_textures[2];
    MTL::SamplerState* last_bound_samplers[2];

    int8_t last_depth_test = -1;
    int8_t last_depth_mask = -1;
    int8_t last_zmode_decal = -1;
};

static struct {
    // Elements that only need to be setup once
    SDL_Renderer* renderer;
    void* layer; // CA::MetalLayer*
    MTL::Device* device;
    MTL::CommandQueue* command_queue;

    std::queue<MTL::Buffer*> buffer_pool;
    std::unordered_map<std::pair<uint64_t, uint32_t>, struct ShaderProgramMetal, hash_pair_shader_ids> shader_program_pool;

    std::vector<struct TextureDataMetal> textures;
    std::vector<FramebufferMetal> framebuffers;
    FrameUniforms frame_uniforms;
    CoordUniforms coord_uniforms;
    MTL::Buffer* frame_uniform_buffer;

    uint32_t msaa_num_quality_levels[METAL_MAX_MULTISAMPLE_SAMPLE_COUNT];

    // Depth querying
    MTL::Buffer* coord_uniform_buffer;
    MTL::Buffer* depth_value_output_buffer;
    size_t coord_buffer_size;
    MTL::Function* depth_compute_function;

    // Current state
    struct ShaderProgramMetal* shader_program;
    CA::MetalDrawable* current_drawable;
    std::set<int> drawn_framebuffers;
    NS::AutoreleasePool* frame_autorelease_pool;

    int current_tile;
    uint32_t current_texture_ids[2];
    int current_framebuffer;
    size_t current_vertex_buffer_offset;
    FilteringMode current_filter_mode = FILTER_THREE_POINT;

    int8_t depth_test;
    int8_t depth_mask;
    int8_t zmode_decal;
} mctx;

// MARK: - Helpers

static MTL::SamplerAddressMode gfx_cm_to_metal(uint32_t val) {
    switch (val) {
        case G_TX_NOMIRROR | G_TX_CLAMP:
            return MTL::SamplerAddressModeClampToEdge;
        case G_TX_MIRROR | G_TX_WRAP:
            return MTL::SamplerAddressModeMirrorRepeat;
        case G_TX_MIRROR | G_TX_CLAMP:
            return MTL::SamplerAddressModeMirrorClampToEdge;
        case G_TX_NOMIRROR | G_TX_WRAP:
            return MTL::SamplerAddressModeRepeat;
    }

    return MTL::SamplerAddressModeClampToEdge;
}

static MTL::Buffer* next_available_buffer() {
    if (mctx.buffer_pool.size() == 0) {
        // Create a new buffer that can hold all draw triangle buffers. Size buf_vbo size in gfx_pc * 50.
        MTL::Buffer* new_buffer = mctx.device->newBuffer(256 * 32 * 3 * sizeof(float) * 50, MTL::ResourceStorageModeShared);
        mctx.buffer_pool.push(new_buffer);
        SPDLOG_DEBUG("Metal: new buffer for pool created");

        return new_buffer;
    }

    return mctx.buffer_pool.front();
}

static void pop_buffer_and_wait_to_requeue(MTL::CommandBuffer* command_buffer) {
    MTL::Buffer* buffer = mctx.buffer_pool.front();
    mctx.buffer_pool.pop();

    command_buffer->addCompletedHandler(^void( MTL::CommandBuffer* cmd_buf ) {
        if (mctx.buffer_pool.size() <= kMaxBufferPoolSize) {
            mctx.buffer_pool.push(buffer);
        }
    });
}

// MARK: - ImGui & SDL Wrappers

void Metal_SetRenderer(SDL_Renderer* renderer) {
    mctx.renderer = renderer;
}

bool Metal_Init() {
    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    mctx.layer = SDL_RenderGetMetalLayer(mctx.renderer);
    set_layer_pixel_format(mctx.layer);

    mctx.device = get_layer_device(mctx.layer);
    mctx.command_queue = mctx.device->newCommandQueue();

    autorelease_pool->release();

    return ImGui_ImplMetal_Init(mctx.device);
}

static void gfx_metal_setup_screen_framebuffer(uint32_t width, uint32_t height);

void Metal_NewFrame() {
    int width, height;
    SDL_GetRendererOutputSize(mctx.renderer, &width, &height);
    set_layer_drawable_size(mctx.layer, width, height);
    gfx_metal_setup_screen_framebuffer(width, height);

    MTL::RenderPassDescriptor* current_render_pass = mctx.framebuffers[0].render_pass_descriptor;
    ImGui_ImplMetal_NewFrame(current_render_pass);
}

void Metal_RenderDrawData(ImDrawData* draw_data) {
    auto framebuffer = mctx.framebuffers[0];
    ImGui_ImplMetal_RenderDrawData(draw_data, framebuffer.command_buffer, framebuffer.command_encoder);
}

// MARK: - Metal Graphics Rendering API

static const char* gfx_metal_get_name() {
    return "Metal";
}

// Forward declare this method
int gfx_metal_create_framebuffer(void);

static void gfx_metal_init(void) {
    // Create the default framebuffer which represents the window
    FramebufferMetal& fb = mctx.framebuffers[gfx_metal_create_framebuffer()];
    fb.msaa_level = 1;

    // Check device for supported msaa levels
    for (uint32_t sample_count = 1; sample_count <= METAL_MAX_MULTISAMPLE_SAMPLE_COUNT; sample_count++) {
        if (mctx.device->supportsTextureSampleCount(sample_count)) {
            mctx.msaa_num_quality_levels[sample_count - 1] = 1;
        } else {
            mctx.msaa_num_quality_levels[sample_count - 1] = 0;
        }
    }

    // Compute shader for retrieving depth values
    const char* depth_shader = R"(
        #include <metal_stdlib>
        using namespace metal;

        struct CoordUniforms {
            uint2 coords[1024];
        };
        
        kernel void depthKernel(depth2d<float, access::read> depth_texture [[ texture(0) ]],
                                     constant CoordUniforms& query_coords [[ buffer(0) ]],
                                     device float* output_values [[ buffer(1) ]],
                                     ushort2 thread_position [[ thread_position_in_grid ]]) {
            uint2 coord = query_coords.coords[thread_position.x];
            output_values[thread_position.x] = depth_texture.read(coord);
        }
    )";

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    NS::Error* error = nullptr;
    MTL::Library* library = mctx.device->newLibrary(NS::String::string(depth_shader, NS::UTF8StringEncoding), nullptr, &error);

    if (error != nullptr)
        SPDLOG_ERROR("Failed to compile shader library, error {}", error->localizedDescription()->cString(NS::UTF8StringEncoding));

    mctx.depth_compute_function = library->newFunction(NS::String::string("depthKernel", NS::UTF8StringEncoding));
    autorelease_pool->release();
}

static struct GfxClipParameters gfx_metal_get_clip_parameters() {
    return { true, false };
}

static void gfx_metal_unload_shader(struct ShaderProgram *old_prg) {}

static void gfx_metal_load_shader(struct ShaderProgram *new_prg) {
    mctx.shader_program = (struct ShaderProgramMetal *)new_prg;
}

static struct ShaderProgram* gfx_metal_create_and_load_new_shader(uint64_t shader_id0, uint32_t shader_id1) {
    CCFeatures cc_features;
    gfx_cc_get_features(shader_id0, shader_id1, &cc_features);

    size_t num_floats = 0;
    char buf[4096];

    memset(buf, 0, sizeof(buf));
    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    MTL::VertexDescriptor* vertex_descriptor = gfx_metal_build_shader(buf, num_floats, cc_features, mctx.current_filter_mode == FILTER_THREE_POINT);

    NS::Error* error = nullptr;
    MTL::Library* library = mctx.device->newLibrary(NS::String::string(buf, NS::UTF8StringEncoding), nullptr, &error);

    if (error != nullptr)
        SPDLOG_ERROR("Failed to compile shader library, error {}", error->localizedDescription()->cString(NS::UTF8StringEncoding));

    MTL::RenderPipelineDescriptor* pipeline_descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    pipeline_descriptor->setVertexFunction(library->newFunction(NS::String::string("vertexShader", NS::UTF8StringEncoding)));
    pipeline_descriptor->setFragmentFunction(library->newFunction(NS::String::string("fragmentShader", NS::UTF8StringEncoding)));
    pipeline_descriptor->setVertexDescriptor(vertex_descriptor);

    pipeline_descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    pipeline_descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
    if (cc_features.opt_alpha) {
        pipeline_descriptor->colorAttachments()->object(0)->setBlendingEnabled(true);
        pipeline_descriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        pipeline_descriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
        pipeline_descriptor->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperationAdd);
        pipeline_descriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorZero);
        pipeline_descriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorOne);
        pipeline_descriptor->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperationAdd);
        pipeline_descriptor->colorAttachments()->object(0)->setWriteMask(MTL::ColorWriteMaskAll);
    } else {
        pipeline_descriptor->colorAttachments()->object(0)->setBlendingEnabled(false);
        pipeline_descriptor->colorAttachments()->object(0)->setWriteMask(MTL::ColorWriteMaskAll);
    }

    struct ShaderProgramMetal* prg = &mctx.shader_program_pool[std::make_pair(shader_id0, shader_id1)];
    prg->shader_id0 = shader_id0;
    prg->shader_id1 = shader_id1;
    prg->used_textures[0] = cc_features.used_textures[0];
    prg->used_textures[1] = cc_features.used_textures[1];
    prg->num_inputs = cc_features.num_inputs;
    prg->num_floats = num_floats;

    // Prepoluate pipeline state cache with program and available msaa levels
    for (int i = 0; i < ARRAY_COUNT(mctx.msaa_num_quality_levels); i++) {
        if (mctx.msaa_num_quality_levels[i] == 1) {
            int msaa_level = i + 1;
            pipeline_descriptor->setSampleCount(msaa_level);
            MTL::RenderPipelineState* pipeline_state = mctx.device->newRenderPipelineState(pipeline_descriptor, &error);

            if (!pipeline_state || error != nullptr) {
                // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
                // If the Metal API validation is enabled, we can find out more information about what
                // went wrong.  (Metal API validation is enabled by default when a debug build is run
                // from Xcode)
                SPDLOG_ERROR("Failed to create pipeline state, error {}", error->localizedDescription()->cString(NS::UTF8StringEncoding));
            }

            prg->pipeline_state_variants[msaa_level] = pipeline_state;
        }
    }

    gfx_metal_load_shader((struct ShaderProgram *)prg);

    autorelease_pool->release();

    return (struct ShaderProgram *)prg;
}

static struct ShaderProgram* gfx_metal_lookup_shader(uint64_t shader_id0, uint32_t shader_id1) {
    auto it = mctx.shader_program_pool.find(std::make_pair(shader_id0, shader_id1));
    return it == mctx.shader_program_pool.end() ? nullptr : (struct ShaderProgram *)&it->second;
}

static void gfx_metal_shader_get_info(struct ShaderProgram *prg, uint8_t *num_inputs, bool used_textures[2]) {
    struct ShaderProgramMetal *p = (struct ShaderProgramMetal *)prg;

    *num_inputs = p->num_inputs;
    used_textures[0] = p->used_textures[0];
    used_textures[1] = p->used_textures[1];
}

static uint32_t gfx_metal_new_texture(void) {
    mctx.textures.resize(mctx.textures.size() + 1);
    return (uint32_t)(mctx.textures.size() - 1);
}

static void gfx_metal_delete_texture(uint32_t texID) {}

static void gfx_metal_select_texture(int tile, uint32_t texture_id) {
    mctx.current_tile = tile;
    mctx.current_texture_ids[tile] = texture_id;
}

static void gfx_metal_upload_texture(const uint8_t *rgba32_buf, uint32_t width, uint32_t height) {
    TextureDataMetal* texture_data = &mctx.textures[mctx.current_texture_ids[mctx.current_tile]];

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    MTL::TextureDescriptor* texture_descriptor = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatRGBA8Unorm, width, height, true);
    texture_descriptor->setArrayLength(1);
    texture_descriptor->setMipmapLevelCount(1);
    texture_descriptor->setSampleCount(1);
    texture_descriptor->setStorageMode(MTL::StorageModeShared);

    MTL::Region region = MTL::Region::Make2D(0, 0, width, height);

    MTL::Texture* texture = texture_data->texture;
    if (texture_data->texture == nullptr || texture_data->texture->width() != width || texture_data->texture->height() != height) {
        if (texture_data->texture != nullptr)
            texture_data->texture->release();

        texture = mctx.device->newTexture(texture_descriptor);
    }

    NS::UInteger bytes_per_pixel = 4;
    NS::UInteger bytes_per_row = bytes_per_pixel * width;
    texture->replaceRegion(region, 0, rgba32_buf, bytes_per_row);
    texture_data->texture = texture;

    autorelease_pool->release();
}

static void gfx_metal_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    TextureDataMetal *texture_data = &mctx.textures[mctx.current_texture_ids[tile]];
    texture_data->linear_filtering = linear_filter;

    // This function is called twice per texture, the first one only to set default values.
    // Maybe that could be skipped? Anyway, make sure to release the first default sampler
    // state before setting the actual one.
    texture_data->sampler->release();

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    MTL::SamplerDescriptor* sampler_descriptor = MTL::SamplerDescriptor::alloc()->init();
    MTL::SamplerMinMagFilter filter = linear_filter && mctx.current_filter_mode == FILTER_LINEAR ? MTL::SamplerMinMagFilterLinear : MTL::SamplerMinMagFilterNearest;
    sampler_descriptor->setMinFilter(filter);
    sampler_descriptor->setMagFilter(filter);
    sampler_descriptor->setSAddressMode(gfx_cm_to_metal(cms));
    sampler_descriptor->setTAddressMode(gfx_cm_to_metal(cmt));
    sampler_descriptor->setRAddressMode(MTL::SamplerAddressModeRepeat);

    texture_data->sampler = mctx.device->newSamplerState(sampler_descriptor);

    autorelease_pool->release();
}

static void gfx_metal_set_depth_test_and_mask(bool depth_test, bool depth_mask) {
    mctx.depth_test = depth_test;
    mctx.depth_mask = depth_mask;
}

static void gfx_metal_set_zmode_decal(bool zmode_decal) {
    mctx.zmode_decal = zmode_decal;
}

static void gfx_metal_set_viewport(int x, int y, int width, int height) {
    auto current_framebuffer = mctx.framebuffers[mctx.current_framebuffer];
    current_framebuffer.command_encoder->setViewport({ x, y, width, height, 0.0, 1.0 });
}

static void gfx_metal_set_scissor(int x, int y, int width, int height) {
    FramebufferMetal fb = mctx.framebuffers[mctx.current_framebuffer];
    TextureDataMetal tex = mctx.textures[fb.texture_id];

    // clamp to viewport size as metal does not support larger values than viewport size
    x = std::max(0, std::min<int>(x, tex.width));
    y = std::max(0, std::min<int>(y, tex.height));
    width = std::max(0, std::min<int>(width,  tex.width));
    height = std::max(0, std::min<int>(height, tex.height));
    fb.command_encoder->setScissorRect({ x, y, width, height });
}

static void gfx_metal_set_use_alpha(bool use_alpha) {
    // Already part of the pipeline state from shader info
}

static void gfx_metal_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {
    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    auto& current_framebuffer = mctx.framebuffers[mctx.current_framebuffer];

    if (current_framebuffer.last_depth_test != mctx.depth_test || current_framebuffer.last_depth_mask != mctx.depth_mask) {
        current_framebuffer.last_depth_test = mctx.depth_test;
        current_framebuffer.last_depth_mask = mctx.depth_mask;

        MTL::DepthStencilDescriptor* depth_descriptor = MTL::DepthStencilDescriptor::alloc()->init();
        depth_descriptor->setDepthWriteEnabled(mctx.depth_mask);
        depth_descriptor->setDepthCompareFunction(mctx.depth_test ? MTL::CompareFunctionLessEqual : MTL::CompareFunctionAlways);

        MTL::DepthStencilState* depth_stencil_state = mctx.device->newDepthStencilState(depth_descriptor);
        current_framebuffer.command_encoder->setDepthStencilState(depth_stencil_state);
    }

    if (current_framebuffer.last_zmode_decal != mctx.zmode_decal) {
        current_framebuffer.last_zmode_decal = mctx.zmode_decal;

        current_framebuffer.command_encoder->setTriangleFillMode(MTL::TriangleFillModeFill);
        current_framebuffer.command_encoder->setCullMode(MTL::CullModeNone);
        current_framebuffer.command_encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
        current_framebuffer.command_encoder->setDepthBias(0, mctx.zmode_decal ? -2 : 0, 0);
    }

    MTL::Buffer* vertex_buffer = next_available_buffer();
    memcpy((char *)vertex_buffer->contents() + mctx.current_vertex_buffer_offset, buf_vbo, sizeof(float) * buf_vbo_len);


    if (!current_framebuffer.has_bounded_vertex_buffer) {
        current_framebuffer.command_encoder->setVertexBuffer(vertex_buffer, 0, 0);
        current_framebuffer.has_bounded_vertex_buffer = true;
    }

    current_framebuffer.command_encoder->setVertexBufferOffset(mctx.current_vertex_buffer_offset, 0);

    if (!current_framebuffer.has_bounded_fragment_buffer) {
        current_framebuffer.command_encoder->setFragmentBuffer(mctx.frame_uniform_buffer, 0, 0);
        current_framebuffer.has_bounded_fragment_buffer = true;
    }

    for (int i = 0; i < 2; i++) {
        if (mctx.shader_program->used_textures[i]) {
            if (current_framebuffer.last_bound_textures[i] != mctx.textures[mctx.current_texture_ids[i]].texture) {
                current_framebuffer.last_bound_textures[i] = mctx.textures[mctx.current_texture_ids[i]].texture;
                current_framebuffer.command_encoder->setFragmentTexture(mctx.textures[mctx.current_texture_ids[i]].texture, i);

                if (current_framebuffer.last_bound_samplers[i] != mctx.textures[mctx.current_texture_ids[i]].sampler) {
                    current_framebuffer.last_bound_samplers[i] = mctx.textures[mctx.current_texture_ids[i]].sampler;
                    current_framebuffer.command_encoder->setFragmentSamplerState(mctx.textures[mctx.current_texture_ids[i]].sampler, i);
                }
            }
        }
    }


    if (current_framebuffer.last_shader_program != mctx.shader_program) {
        current_framebuffer.last_shader_program = mctx.shader_program;

        MTL::RenderPipelineState* pipeline_state = mctx.shader_program->pipeline_state_variants[current_framebuffer.msaa_level];
        current_framebuffer.command_encoder->setRenderPipelineState(pipeline_state);
    }

    current_framebuffer.command_encoder->drawPrimitives(MTL::PrimitiveTypeTriangle, 0.f, buf_vbo_num_tris * 3);
    mctx.current_vertex_buffer_offset += sizeof(float) * buf_vbo_len;

    autorelease_pool->release();
}

static void gfx_metal_on_resize(void) {}

static void gfx_metal_start_frame(void) {
    mctx.frame_uniforms.frameCount++;
    if (mctx.frame_uniforms.frameCount > 150) {
        // No high values, as noise starts to look ugly
        mctx.frame_uniforms.frameCount = 0;
    }

    if (!mctx.frame_uniform_buffer) {
        mctx.frame_uniform_buffer = mctx.device->newBuffer(sizeof(FrameUniforms), MTL::ResourceCPUCacheModeDefaultCache);
    }
    if (!mctx.coord_uniform_buffer) {
        mctx.coord_uniform_buffer = mctx.device->newBuffer(sizeof(CoordUniforms), MTL::ResourceCPUCacheModeDefaultCache);
    }

    mctx.current_vertex_buffer_offset = 0;

    mctx.frame_autorelease_pool = NS::AutoreleasePool::alloc()->init();
}

void gfx_metal_end_frame(void) {
    std::set<int>::iterator it = mctx.drawn_framebuffers.begin();
    it++;

    while (it != mctx.drawn_framebuffers.end()) {
        auto framebuffer = mctx.framebuffers[*it];

        if (!framebuffer.has_ended_encoding)
            framebuffer.command_encoder->endEncoding();

        pop_buffer_and_wait_to_requeue(framebuffer.command_buffer);
        framebuffer.command_buffer->commit();
        framebuffer.command_buffer->waitUntilCompleted();

        it++;
    }

    auto screen_framebuffer = mctx.framebuffers[0];
    screen_framebuffer.command_encoder->endEncoding();

    screen_framebuffer.command_buffer->presentDrawable(mctx.current_drawable);

    pop_buffer_and_wait_to_requeue(screen_framebuffer.command_buffer);
    screen_framebuffer.command_buffer->commit();

    mctx.drawn_framebuffers.clear();

    // Cleanup states
    for (int fb_id = 0; fb_id < (int)mctx.framebuffers.size(); fb_id++) {
        FramebufferMetal& fb = mctx.framebuffers[fb_id];

        fb.last_shader_program = nullptr;
        fb.command_buffer = nullptr;
        fb.command_encoder = nullptr;
        fb.has_ended_encoding = false;
        fb.has_bounded_vertex_buffer = false;
        fb.has_bounded_fragment_buffer = false;
        fb.last_bound_textures[0] = nullptr;
        fb.last_bound_textures[1] = nullptr;
        fb.last_bound_samplers[0] = nullptr;
        fb.last_bound_samplers[1] = nullptr;
        fb.last_depth_test = -1;
        fb.last_depth_mask = -1;
        fb.last_zmode_decal = -1;
    }

    mctx.frame_autorelease_pool->release();
}

static void gfx_metal_finish_render(void) {}

int gfx_metal_create_framebuffer(void) {
    uint32_t texture_id = gfx_metal_new_texture();
    TextureDataMetal& t = mctx.textures[texture_id];

    size_t index = mctx.framebuffers.size();
    mctx.framebuffers.resize(mctx.framebuffers.size() + 1);
    FramebufferMetal& data = mctx.framebuffers.back();
    data.texture_id = texture_id;

    uint32_t tile = 0;
    uint32_t saved = mctx.current_texture_ids[tile];
    mctx.current_texture_ids[tile] = texture_id;
    gfx_metal_set_sampler_parameters(0, true, G_TX_WRAP, G_TX_WRAP);
    mctx.current_texture_ids[tile] = saved;

    return (int)index;
}

static void gfx_metal_setup_screen_framebuffer(uint32_t width, uint32_t height) {
    mctx.current_drawable = nullptr;
    mctx.current_drawable = get_layer_next_drawable(mctx.layer);

    bool msaa_enabled = CVar_GetS32("gMSAAValue", 1) > 1;

    FramebufferMetal& fb = mctx.framebuffers[0];
    TextureDataMetal& tex = mctx.textures[fb.texture_id];

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    if (tex.texture != nullptr)
        tex.texture->release();

    tex.texture = mctx.current_drawable->texture();

    MTL::RenderPassDescriptor* render_pass_descriptor = MTL::RenderPassDescriptor::renderPassDescriptor();
    MTL::ClearColor clear_color = MTL::ClearColor::Make(0, 0, 0, 1);
    render_pass_descriptor->colorAttachments()->object(0)->setTexture(tex.texture);
    render_pass_descriptor->colorAttachments()->object(0)->setLoadAction(msaa_enabled ? MTL::LoadActionLoad : MTL::LoadActionClear);
    render_pass_descriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
    render_pass_descriptor->colorAttachments()->object(0)->setClearColor(clear_color);

    if (fb.render_pass_descriptor != nullptr)
        fb.render_pass_descriptor->release();

    fb.render_pass_descriptor = render_pass_descriptor;
    fb.render_pass_descriptor->retain();

    tex.width = width;
    tex.height = height;

    if (fb.depth_texture != nullptr)
        fb.depth_texture->release();

    // If possible, we eventually we want to disable this when msaa is enabled since we don't need this depth texture
    // However, problem is if the user switches to msaa during game, we need a way to then generate it before drawing.
    MTL::TextureDescriptor* depth_tex_desc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, width, height, true);

    depth_tex_desc->setTextureType(MTL::TextureType2D);
    depth_tex_desc->setStorageMode(MTL::StorageModePrivate);
    depth_tex_desc->setSampleCount(1);
    depth_tex_desc->setArrayLength(1);
    depth_tex_desc->setMipmapLevelCount(1);
    depth_tex_desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

    fb.depth_texture = mctx.device->newTexture(depth_tex_desc);

    fb.render_pass_descriptor->depthAttachment()->setTexture(fb.depth_texture);
    fb.render_pass_descriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    fb.render_pass_descriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
    fb.render_pass_descriptor->depthAttachment()->setClearDepth(1);

    fb.render_target = true;
    fb.has_depth_buffer = true;

    autorelease_pool->release();
}

static void gfx_metal_update_framebuffer_parameters(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level, bool opengl_invert_y, bool render_target, bool has_depth_buffer, bool can_extract_depth) {

    // Screen framebuffer is handled separately on a frame by frame basis
    // see `gfx_metal_setup_screen_framebuffer`.
    if (fb_id == 0)
        return;

    FramebufferMetal& fb = mctx.framebuffers[fb_id];
    TextureDataMetal& tex = mctx.textures[fb.texture_id];

    width = std::max(width, 1U);
    height = std::max(height, 1U);
    while (msaa_level > 1 && mctx.msaa_num_quality_levels[msaa_level - 1] == 0) {
        --msaa_level;
    }

    bool diff = tex.width != width || tex.height != height || fb.msaa_level != msaa_level;

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    if (diff || (fb.render_pass_descriptor != nullptr) != render_target) {
        MTL::TextureDescriptor* tex_descriptor = MTL::TextureDescriptor::alloc()->init();
        tex_descriptor->setTextureType(MTL::TextureType2D);
        tex_descriptor->setWidth(width);
        tex_descriptor->setHeight(height);
        tex_descriptor->setSampleCount(1);
        tex_descriptor->setMipmapLevelCount(1);
        tex_descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
        tex_descriptor->setUsage((render_target ? MTL::TextureUsageRenderTarget : 0) | MTL::TextureUsageShaderRead);

        if (tex.texture != nullptr)
            tex.texture->release();

        tex.texture = mctx.device->newTexture(tex_descriptor);

        if (msaa_level > 1) {
            MTL::TextureDescriptor* msaa_tex_descriptor = MTL::TextureDescriptor::alloc()->init();
            msaa_tex_descriptor->setTextureType(MTL::TextureType2DMultisample);
            msaa_tex_descriptor->setWidth(width);
            msaa_tex_descriptor->setHeight(height);
            msaa_tex_descriptor->setSampleCount(msaa_level);
            msaa_tex_descriptor->setMipmapLevelCount(1);
            msaa_tex_descriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
            msaa_tex_descriptor->setStorageMode(MTL::StorageModePrivate);
            msaa_tex_descriptor->setUsage(render_target ? MTL::TextureUsageRenderTarget : 0);

            if (tex.msaaTexture != nullptr)
                tex.msaaTexture->release();
            tex.msaaTexture = mctx.device->newTexture(msaa_tex_descriptor);
        }

        if (render_target) {
            MTL::RenderPassDescriptor* render_pass_descriptor = MTL::RenderPassDescriptor::renderPassDescriptor();

            bool msaa_enabled = (msaa_level > 1);
            MTL::ClearColor clear_color = MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0);

            if (msaa_enabled) {
                render_pass_descriptor->colorAttachments()->object(0)->setTexture(tex.msaaTexture);
                render_pass_descriptor->colorAttachments()->object(0)->setResolveTexture(tex.texture);
                render_pass_descriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
                render_pass_descriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionMultisampleResolve);
                render_pass_descriptor->colorAttachments()->object(0)->setClearColor(clear_color);
            } else {
                render_pass_descriptor->colorAttachments()->object(0)->setTexture(tex.texture);
                render_pass_descriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
                render_pass_descriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
                render_pass_descriptor->colorAttachments()->object(0)->setClearColor(clear_color);
            }

            if (fb.render_pass_descriptor != nullptr)
                fb.render_pass_descriptor->release();

            fb.render_pass_descriptor = render_pass_descriptor;
            fb.render_pass_descriptor->retain();
        }

        tex.width = width;
        tex.height = height;
    }

    if (has_depth_buffer && (diff || !fb.has_depth_buffer || (fb.depth_texture != nullptr) != can_extract_depth)) {
        MTL::TextureDescriptor *depth_tex_desc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, width, height, true);

        depth_tex_desc->setTextureType(MTL::TextureType2D);
        depth_tex_desc->setStorageMode(MTL::StorageModePrivate);
        depth_tex_desc->setSampleCount(1);
        depth_tex_desc->setArrayLength(1);
        depth_tex_desc->setMipmapLevelCount(1);
        depth_tex_desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

        if (fb.depth_texture != nullptr)
            fb.depth_texture->release();

        fb.depth_texture = mctx.device->newTexture(depth_tex_desc);

        if (msaa_level > 1) {
            MTL::TextureDescriptor *msaa_depth_tex_desc = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, width, height, true);

            msaa_depth_tex_desc->setTextureType(MTL::TextureType2DMultisample);
            msaa_depth_tex_desc->setStorageMode(MTL::StorageModePrivate);
            msaa_depth_tex_desc->setSampleCount(msaa_level);
            msaa_depth_tex_desc->setArrayLength(1);
            msaa_depth_tex_desc->setMipmapLevelCount(1);
            msaa_depth_tex_desc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

            if (fb.msaa_depth_texture != nullptr)
                fb.msaa_depth_texture->release();

            fb.msaa_depth_texture = mctx.device->newTexture(msaa_depth_tex_desc);
        }
    }

    if (has_depth_buffer) {
        if (msaa_level > 1) {
            fb.render_pass_descriptor->depthAttachment()->setTexture(fb.msaa_depth_texture);
            fb.render_pass_descriptor->depthAttachment()->setResolveTexture(fb.depth_texture);
            fb.render_pass_descriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
            fb.render_pass_descriptor->depthAttachment()->setStoreAction(MTL::StoreActionMultisampleResolve);
            fb.render_pass_descriptor->depthAttachment()->setClearDepth(1);
        } else {
            fb.render_pass_descriptor->depthAttachment()->setTexture(fb.depth_texture);
            fb.render_pass_descriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
            fb.render_pass_descriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
            fb.render_pass_descriptor->depthAttachment()->setClearDepth(1);
        }
    } else {
        fb.render_pass_descriptor->setDepthAttachment(nullptr);
    }

    fb.render_target = render_target;
    fb.has_depth_buffer = has_depth_buffer;
    fb.msaa_level = msaa_level;

    autorelease_pool->release();
}

void gfx_metal_start_draw_to_framebuffer(int fb_id, float noise_scale) {
    FramebufferMetal& fb = mctx.framebuffers[fb_id];
    mctx.current_framebuffer = fb_id;
    mctx.drawn_framebuffers.insert(fb_id);

    if (fb.render_target && fb.command_buffer == nullptr && fb.command_encoder == nullptr) {
        fb.command_buffer = mctx.command_queue->commandBuffer();
        fb.command_buffer->setLabel(string_with_format("Framebuffer (%d) Command Buffer", fb_id));

        fb.command_encoder = fb.command_buffer->renderCommandEncoder(fb.render_pass_descriptor);
        fb.command_encoder->setLabel(string_with_format("Framebuffer (%d) Render Pass", fb_id));
        fb.command_encoder->setDepthClipMode(MTL::DepthClipModeClamp);
    }

    if (noise_scale != 0.0f) {
        mctx.frame_uniforms.noiseScale = 1.0f / noise_scale;
    }

    memcpy(mctx.frame_uniform_buffer->contents(), &mctx.frame_uniforms, sizeof(FrameUniforms));
}

void gfx_metal_clear_framebuffer(void) {}

void gfx_metal_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    auto& source_framebuffer = mctx.framebuffers[fb_id_source];
    source_framebuffer.command_encoder->endEncoding();
    source_framebuffer.has_ended_encoding = true;
    MTL::BlitCommandEncoder* blit_encoder = source_framebuffer.command_buffer->blitCommandEncoder();
    blit_encoder->setLabel(NS::String::string("MSAA Copy Encoder", NS::UTF8StringEncoding));

    // Copy over the source framebuffer's texture to the target!
    int source_texture_id = mctx.framebuffers[fb_id_source].texture_id;
    MTL::Texture* source_texture = mctx.textures[source_texture_id].texture;

    int target_texture_id = mctx.framebuffers[fb_id_target].texture_id;
    MTL::Texture* target_texture = target_texture_id == 0 ? mctx.current_drawable->texture() : mctx.textures[target_texture_id].texture;

    blit_encoder->copyFromTexture(source_texture, target_texture);
    blit_encoder->endEncoding();
}

std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> gfx_metal_get_pixel_depth(int fb_id, const std::set<std::pair<float, float>>& coordinates) {
    auto framebuffer = mctx.framebuffers[fb_id];

    if (coordinates.size() > mctx.coord_buffer_size) {
        if (mctx.depth_value_output_buffer != nullptr)
            mctx.depth_value_output_buffer->release();

        mctx.depth_value_output_buffer = mctx.device->newBuffer(sizeof(float) * coordinates.size(), MTL::ResourceOptionCPUCacheModeDefault);
        mctx.depth_value_output_buffer->setLabel(NS::String::string("Depth output buffer", NS::UTF8StringEncoding));

        mctx.coord_buffer_size = coordinates.size();
    }

    // zero out the buffer
    memset(mctx.coord_uniform_buffer->contents(), 0, sizeof(CoordUniforms));
    memset(mctx.depth_value_output_buffer->contents(), 0, sizeof(float) * coordinates.size());

    // map coordinates to right y axis
    size_t i = 0;
    for (const auto& coord : coordinates) {
        mctx.coord_uniforms.coords[i].x = coord.first;
        mctx.coord_uniforms.coords[i].y = framebuffer.depth_texture->height() - 1 - coord.second;
        ++i;
    }

    // set uniform values
    memcpy(mctx.coord_uniform_buffer->contents(), &mctx.coord_uniforms, sizeof(CoordUniforms));

    NS::AutoreleasePool* autorelease_pool = NS::AutoreleasePool::alloc()->init();

    auto command_buffer = mctx.command_queue->commandBuffer();
    command_buffer->setLabel(NS::String::string("Depth Shader Command Buffer", NS::UTF8StringEncoding));

    NS::Error* error = nullptr;
    MTL::ComputePipelineState* compute_pipeline_state = mctx.device->newComputePipelineState(mctx.depth_compute_function, &error);

    MTL::ComputeCommandEncoder* compute_encoder = command_buffer->computeCommandEncoder();
    compute_encoder->setComputePipelineState(compute_pipeline_state);
    compute_encoder->setTexture(framebuffer.depth_texture, 0);
    compute_encoder->setBuffer(mctx.coord_uniform_buffer, 0, 0);
    compute_encoder->setBuffer(mctx.depth_value_output_buffer, 0, 1);

    MTL::Size thread_group_size = MTL::Size::Make(1, 1, 1);
    MTL::Size thread_group_count = MTL::Size::Make(coordinates.size(), 1, 1);

    compute_encoder->dispatchThreads(thread_group_count, thread_group_size);
    compute_encoder->endEncoding();

    command_buffer->commit();
    command_buffer->waitUntilCompleted();

    // Now the depth values can be accessed in the buffer.
    float* depth_values = (float*)mctx.depth_value_output_buffer->contents();

    std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> res;
    {
        size_t i = 0;
        for (const auto& coord : coordinates) {
            res.emplace(coord, depth_values[i++] * 65532.0f);
        }
    }

    autorelease_pool->release();

    return res;
}

void *gfx_metal_get_framebuffer_texture_id(int fb_id) {
    return (void*)mctx.textures[mctx.framebuffers[fb_id].texture_id].texture;
}

void gfx_metal_select_texture_fb(int fb_id) {
    int tile = 0;
    gfx_metal_select_texture(tile, mctx.framebuffers[fb_id].texture_id);
}

void gfx_metal_set_texture_filter(FilteringMode mode) {
    mctx.current_filter_mode = mode;
    gfx_texture_cache_clear();
}

FilteringMode gfx_metal_get_texture_filter(void) {
    return mctx.current_filter_mode;
}

ImTextureID gfx_metal_get_texture_by_id(int fb_id) {
    return (void*)mctx.textures[fb_id].texture;
}

struct GfxRenderingAPI gfx_metal_api = {
    gfx_metal_get_name,
    gfx_metal_get_clip_parameters,
    gfx_metal_unload_shader,
    gfx_metal_load_shader,
    gfx_metal_create_and_load_new_shader,
    gfx_metal_lookup_shader,
    gfx_metal_shader_get_info,
    gfx_metal_new_texture,
    gfx_metal_select_texture,
    gfx_metal_upload_texture,
    gfx_metal_set_sampler_parameters,
    gfx_metal_set_depth_test_and_mask,
    gfx_metal_set_zmode_decal,
    gfx_metal_set_viewport,
    gfx_metal_set_scissor,
    gfx_metal_set_use_alpha,
    gfx_metal_draw_triangles,
    gfx_metal_init,
    gfx_metal_on_resize,
    gfx_metal_start_frame,
    gfx_metal_end_frame,
    gfx_metal_finish_render,
    gfx_metal_create_framebuffer,
    gfx_metal_update_framebuffer_parameters,
    gfx_metal_start_draw_to_framebuffer,
    gfx_metal_clear_framebuffer,
    gfx_metal_resolve_msaa_color_buffer,
    gfx_metal_get_pixel_depth,
    gfx_metal_get_framebuffer_texture_id,
    gfx_metal_select_texture_fb,
    gfx_metal_delete_texture,
    gfx_metal_set_texture_filter,
    gfx_metal_get_texture_filter
};
#endif
