#ifdef ENABLE_METAL

#include <vector>
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

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#include "Lib/ImGui/backends/imgui_impl_metal.h"
#include "Lib/SDL/SDL2/SDL_render.h"

#include "PR/ultra64/gbi.h"
#include "PR/ultra64/abi.h"

#include "gfx_cc.h"
#include "gfx_rendering_api.h"

#include "gfx_pc.h"
#include "spdlog/spdlog.h"

static constexpr size_t kMaxBufferPoolSize = 15;

// Hashing Helpers

// A hash function used to hash a: pair<float, float>
struct hash_pair_shader_ids {
    size_t operator()(const std::pair<uint64_t, uint32_t> &p ) const {
        auto hash1 = std::hash<uint64_t>{}(p.first);
        auto hash2 = std::hash<uint32_t>{}(p.second);

        // If hash1 == hash2, their XOR is zero.
        return (hash1 != hash2) ? hash1 ^ hash2 : hash1;
    }
};

// MARK: - Structs

struct ShaderProgramMetal {
    uint64_t shader_id0;
    uint32_t shader_id1;

    uint8_t num_inputs;
    uint8_t num_floats;
    bool used_textures[2];
};

struct FrameUniforms {
    simd::int1 frameCount;
    simd::float1 noiseScale;
};

struct TextureDataMetal {
    id<MTLTexture> texture;
    id<MTLSamplerState> sampler;
    uint32_t width;
    uint32_t height;
    bool linear_filtering;
};

struct FramebufferMetal {
    id<MTLCommandBuffer> command_buffer;
    MTLRenderPassDescriptor* render_pass_descriptor;
    id<MTLRenderCommandEncoder> command_encoder;

    id<MTLTexture> depth_texture;
    uint32_t texture_id;
    bool has_depth_buffer;
    uint32_t msaa_level;
    bool render_target;
};

static struct {
    // Elements that only need to be setup once
    SDL_Renderer* renderer = nullptr;
    CAMetalLayer* layer = nullptr;
    id<MTLDevice> device = nullptr;
    id<MTLCommandQueue> command_queue = nullptr;

    std::queue<id<MTLBuffer>> buffer_pool;
    std::unordered_map<std::pair<uint64_t, uint32_t>, struct ShaderProgramMetal, hash_pair_shader_ids> shader_program_pool;
    std::unordered_map<std::pair<uint64_t, uint32_t>, id<MTLRenderPipelineState>, hash_pair_shader_ids> pipeline_cache;

    std::vector<struct TextureDataMetal> textures;
    std::vector<FramebufferMetal> framebuffers;
    FrameUniforms frame_uniforms;
    id<MTLBuffer> frame_uniform_buffer = nullptr;

    // Current state
    struct ShaderProgramMetal* shader_program;
    id<MTLRenderCommandEncoder> current_command_encoder = nullptr;
    std::set<int> drawn_framebuffers;
    NSAutoreleasePool* frame_autorelease_pool = nullptr;

    int current_tile;
    uint32_t current_texture_ids[2];
    int current_framebuffer;
    size_t current_vertex_buffer_offset;
    FilteringMode current_filter_mode = FILTER_THREE_POINT;

//    SDL_Renderer* renderer = nullptr;
//    CA::MetalLayer* layer = nullptr;
//    MTL::Device* device = nullptr;
//    MTL::CommandQueue* command_queue = nullptr;
//
//    std::queue<MTL::Buffer *> buffer_pool;
//    std::unordered_map<std::pair<uint64_t, uint32_t>, struct ShaderProgramMetal, hash_pair_shader_ids> shader_program_pool;
//    std::unordered_map<std::pair<uint64_t, uint32_t>, MTL::RenderPipelineState*, hash_pair_shader_ids> pipeline_cache;
//
//    std::vector<struct TextureDataMetal> textures;
//    std::vector<FramebufferMetal> framebuffers;
//    FrameUniforms frame_uniforms;
//    MTL::Buffer* frame_uniform_buffer = nullptr;
//
//    // Current state
//    struct ShaderProgramMetal* shader_program = nullptr;
//    MTL::CommandBuffer* current_command_buffer = nullptr;
//    MTL::RenderCommandEncoder* current_command_encoder = nullptr;
//
//    int current_tile;
//    uint32_t current_texture_ids[2];
//    int current_framebuffer;
//    size_t current_vertex_buffer_offset;
//    FilteringMode current_filter_mode = NONE;
//
    int8_t depth_test;
    int8_t depth_mask;
    int8_t zmode_decal;
//    simd::int2 viewport_size;

    // Previous states (to prevent setting states needlessly)
    struct ShaderProgramMetal* last_shader_program = nullptr;
//    MTL::Texture* last_bound_textures[2] = { nullptr, nullptr };
//    MTL::SamplerState* last_bound_samplers[2] = { nullptr, nullptr };
    int8_t last_depth_test = -1;
    int8_t last_depth_mask = -1;
    int8_t last_zmode_decal = -1;
} mctx;

// Shader, Sampler & String Helpers

static MTLSamplerAddressMode gfx_cm_to_metal(uint32_t val) {
    switch (val) {
        case G_TX_NOMIRROR | G_TX_CLAMP:
            return MTLSamplerAddressModeClampToEdge;
        case G_TX_MIRROR | G_TX_WRAP:
            return MTLSamplerAddressModeMirrorRepeat;
        case G_TX_MIRROR | G_TX_CLAMP:
            return MTLSamplerAddressModeMirrorClampToEdge;
        case G_TX_NOMIRROR | G_TX_WRAP:
            return MTLSamplerAddressModeRepeat;
    }
    
    return MTLSamplerAddressModeClampToEdge;
}

static id<MTLBuffer> next_available_buffer() {
    if (mctx.buffer_pool.size() == 0) {
        // Create a new buffer that can hold all draw triangle buffers. Size buf_vbo size in gfx_pc * 50.
        id<MTLBuffer> newBuffer = [mctx.device newBufferWithLength:256 * 32 * 3 * sizeof(float) * 50 options:MTLResourceStorageModeShared];
        mctx.buffer_pool.push(newBuffer);
        SPDLOG_DEBUG("Metal: new buffer for pool created");

        return newBuffer;
    }

    return mctx.buffer_pool.front();
}

static void pop_buffer_and_wait_to_requeue(id<MTLCommandBuffer> command_buffer) {
    id<MTLBuffer> buffer = mctx.buffer_pool.front();
    mctx.buffer_pool.pop();

    [command_buffer addCompletedHandler:^(id<MTLCommandBuffer>) {
        SPDLOG_DEBUG("Metal: buffer pool size: {}", mctx.buffer_pool.size());
        if (mctx.buffer_pool.size() <= kMaxBufferPoolSize) {
            mctx.buffer_pool.push(buffer);
        }
    }];
}

static void append_str(char *buf, size_t *len, const char *str) {
    while (*str != '\0') buf[(*len)++] = *str++;
}

static void append_line(char *buf, size_t *len, const char *str) {
    while (*str != '\0') buf[(*len)++] = *str++;
    buf[(*len)++] = '\n';
}

static const char *shader_item_to_str(uint32_t item, bool with_alpha, bool only_alpha, bool inputs_have_alpha, bool hint_single_element) {
    if (!only_alpha) {
        switch (item) {
            case SHADER_0:
                return with_alpha ? "float4(0.0, 0.0, 0.0, 0.0)" : "float3(0.0, 0.0, 0.0)";
            case SHADER_1:
                return with_alpha ? "float4(1.0, 1.0, 1.0, 1.0)" : "float3(1.0, 1.0, 1.0)";
            case SHADER_INPUT_1:
                return with_alpha || !inputs_have_alpha ? "in.input1" : "in.input1.xyz";
            case SHADER_INPUT_2:
                return with_alpha || !inputs_have_alpha ? "in.input2" : "in.input2.xyz";
            case SHADER_INPUT_3:
                return with_alpha || !inputs_have_alpha ? "in.input3" : "in.input3.xyz";
            case SHADER_INPUT_4:
                return with_alpha || !inputs_have_alpha ? "in.input4" : "in.input4.xyz";
            case SHADER_TEXEL0:
                return with_alpha ? "texVal0" : "texVal0.xyz";
            case SHADER_TEXEL0A:
                return hint_single_element ? "texVal0.w" :
                (with_alpha ? "float4(texVal0.w, texVal0.w, texVal0.w, texVal0.w)" : "float3(texVal0.w, texVal0.w, texVal0.w)");
            case SHADER_TEXEL1A:
                return hint_single_element ? "texVal1.w" :
                (with_alpha ? "float4(texVal1.w, texVal1.w, texVal1.w, texVal1.w)" : "float3(texVal1.w, texVal1.w, texVal1.w)");
            case SHADER_TEXEL1:
                return with_alpha ? "texVal1" : "texVal1.xyz";
            case SHADER_COMBINED:
                return with_alpha ? "texel" : "texel.xyz";
        }
    } else {
        switch (item) {
            case SHADER_0:
                return "0.0";
            case SHADER_1:
                return "1.0";
            case SHADER_INPUT_1:
                return "in.input1.w";
            case SHADER_INPUT_2:
                return "in.input2.w";
            case SHADER_INPUT_3:
                return "in.input3.w";
            case SHADER_INPUT_4:
                return "in.input4.w";
            case SHADER_TEXEL0:
                return "texVal0.w";
            case SHADER_TEXEL0A:
                return "texVal0.w";
            case SHADER_TEXEL1A:
                return "texVal1.w";
            case SHADER_TEXEL1:
                return "texVal1.w";
            case SHADER_COMBINED:
                return "texel.w";
        }
    }
    return "";
}

static void append_formula(char *buf, size_t *len, uint8_t c[2][4], bool do_single, bool do_multiply, bool do_mix, bool with_alpha, bool only_alpha, bool opt_alpha) {
    if (do_single) {
        append_str(buf, len, shader_item_to_str(c[only_alpha][3], with_alpha, only_alpha, opt_alpha, false));
    } else if (do_multiply) {
        append_str(buf, len, shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, " * ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
    } else if (do_mix) {
        append_str(buf, len, "mix(");
        append_str(buf, len, shader_item_to_str(c[only_alpha][1], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, ", ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, ", ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
        append_str(buf, len, ")");
    } else {
        append_str(buf, len, "(");
        append_str(buf, len, shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, " - ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][1], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, ") * ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
        append_str(buf, len, " + ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][3], with_alpha, only_alpha, opt_alpha, false));
    }
}

// MARK: - ImGui & SDL Wrappers

void Metal_SetRenderer(SDL_Renderer* renderer) {
    mctx.renderer = renderer;
}

bool Metal_Init() {
    mctx.layer = (__bridge CAMetalLayer*)SDL_RenderGetMetalLayer(mctx.renderer);
    mctx.layer.pixelFormat = MTLPixelFormatRGBA8Unorm;

    mctx.device = mctx.layer.device;
    mctx.command_queue = [mctx.device newCommandQueue];

    return ImGui_ImplMetal_Init(mctx.device);
}

void Metal_NewFrame() {
    int width, height;
    SDL_GetRendererOutputSize(mctx.renderer, &width, &height);
    mctx.layer.drawableSize = CGSizeMake(width, height);

    // this will be more appropriately be called later, but we do it here to setup the font textures
    MTLRenderPassDescriptor *current_render_pass = mctx.framebuffers[mctx.current_framebuffer].render_pass_descriptor;
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
    
    size_t len = 0, num_floats = 4;
    int vertexIndex = 0;
    char buf[4096];

    @autoreleasepool {
        MTLVertexDescriptor *vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];

        memset(buf, 0, sizeof(buf));
        append_line(buf, &len, "#include <metal_stdlib>");
        append_line(buf, &len, "using namespace metal;");

        // Uniforms struct
        append_line(buf, &len, "struct FrameUniforms {");
        append_line(buf, &len, "    int frameCount;");
        append_line(buf, &len, "    float noiseScale;");
        append_line(buf, &len, "};");
        // end uniforms struct

        // Vertex struct
        append_line(buf, &len, "struct Vertex {");
        len += sprintf(buf + len, "    float4 position [[attribute(%d)]];\n", vertexIndex);
        vertexDescriptor.attributes[vertexIndex].format = MTLVertexFormatFloat4;
        vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
        vertexDescriptor.attributes[vertexIndex++].offset = 0;

        for (int i = 0; i < 2; i++) {
            if (cc_features.used_textures[i]) {
                len += sprintf(buf + len, "    float2 texCoord%d [[attribute(%d)]];\n", i, vertexIndex);
                vertexDescriptor.attributes[vertexIndex].format = MTLVertexFormatFloat2;
                vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
                vertexDescriptor.attributes[vertexIndex++].offset = num_floats * sizeof(float);
                num_floats += 2;
                for (int j = 0; j < 2; j++) {
                    if (cc_features.clamp[i][j]) {
                        len += sprintf(buf + len, "    float texClamp%s%d [[attribute(%d)]];\n", j == 0 ? "S" : "T", i, vertexIndex);
                        vertexDescriptor.attributes[vertexIndex].format = MTLVertexFormatFloat;
                        vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
                        vertexDescriptor.attributes[vertexIndex++].offset = num_floats * sizeof(float);
                        num_floats += 1;
                    }
                }
            }
        }
        if (cc_features.opt_fog) {
            len += sprintf(buf + len, "    float4 fog [[attribute(%d)]];\n", vertexIndex);
            vertexDescriptor.attributes[vertexIndex].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
            vertexDescriptor.attributes[vertexIndex++].offset = num_floats * sizeof(float);
            num_floats += 4;
        }
        if (cc_features.opt_grayscale) {
            len += sprintf(buf + len, "    float4 grayscale [[attribute(%d)]];\n", vertexIndex);
            vertexDescriptor.attributes[vertexIndex].format = MTLVertexFormatFloat4;
            vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
            vertexDescriptor.attributes[vertexIndex++].offset = num_floats * sizeof(float);
            num_floats += 4;
        }
        for (int i = 0; i < cc_features.num_inputs; i++) {
            len += sprintf(buf + len, "    float%d input%d [[attribute(%d)]];\n",  cc_features.opt_alpha ? 4 : 3, i + 1, vertexIndex);
            vertexDescriptor.attributes[vertexIndex].format = cc_features.opt_alpha ? MTLVertexFormatFloat4 : MTLVertexFormatFloat3;
            vertexDescriptor.attributes[vertexIndex].bufferIndex = 0;
            vertexDescriptor.attributes[vertexIndex++].offset = num_floats * sizeof(float);
            num_floats += cc_features.opt_alpha ? 4 : 3;
        }
        append_line(buf, &len, "};");
        // end vertex struct

        // fragment output struct
        append_line(buf, &len, "struct ProjectedVertex {");
        for (int i = 0; i < 2; i++) {
            if (cc_features.used_textures[i]) {
                len += sprintf(buf + len, "    float2 texCoord%d;\n", i);
                for (int j = 0; j < 2; j++) {
                    if (cc_features.clamp[i][j]) {
                        len += sprintf(buf + len, "    float texClamp%s%d;\n", j == 0 ? "S" : "T", i);
                    }
                }
            }
        }

        if (cc_features.opt_fog) {
            append_line(buf, &len, "    float4 fog;");
        }
        if (cc_features.opt_grayscale) {
            append_line(buf, &len, "    float4 grayscale;");
        }
        for (int i = 0; i < cc_features.num_inputs; i++) {
            len += sprintf(buf + len, "    float%d input%d;\n",  cc_features.opt_alpha ? 4 : 3, i + 1);
        }
        append_line(buf, &len, "    float4 position [[position]];");
        append_line(buf, &len, "};");
        // end fragment output struct

        // vertex shader
        append_line(buf, &len, "vertex ProjectedVertex vertexShader(Vertex in [[stage_in]]) {");
        append_line(buf, &len, "    ProjectedVertex out;");
        for (int i = 0; i < 2; i++) {
            if (cc_features.used_textures[i]) {
                len += sprintf(buf + len, "    out.texCoord%d = in.texCoord%d;\n", i, i);
                for (int j = 0; j < 2; j++) {
                    if (cc_features.clamp[i][j]) {
                        len += sprintf(buf + len, "    out.texClamp%s%d = in.texClamp%s%d;\n", j == 0 ? "S" : "T", i, j == 0 ? "S" : "T", i);
                    }
                }
            }
        }

        if (cc_features.opt_fog) {
            append_line(buf, &len, "    out.fog = in.fog;");
        }
        if (cc_features.opt_grayscale) {
            append_line(buf, &len, "    out.grayscale = in.grayscale;");
        }
        for (int i = 0; i < cc_features.num_inputs; i++) {
            len += sprintf(buf + len, "    out.input%d = in.input%d;\n", i + 1, i + 1);
        }

        append_line(buf, &len, "    out.position = in.position;");
        append_line(buf, &len, "    return out;");
        append_line(buf, &len, "}");
        // end vertex shader

        // fragment shader

        if (mctx.current_filter_mode == FILTER_THREE_POINT) {
            append_line(buf, &len, "#define TEX_OFFSET(tex, texSmplr, texCoord, off, texSize) tex.sample(texSmplr, texCoord - off / texSize)");
            append_line(buf, &len, "float4 filter3point(thread const texture2d<float> tex, thread const sampler texSmplr, thread const float2& texCoord, thread const float2& texSize) {");
            append_line(buf, &len, "    float2 offset = fract((texCoord * texSize) - float2(0.5));");
            append_line(buf, &len, "    offset -= float2(step(1.0, offset.x + offset.y));");
            append_line(buf, &len, "    float4 c0 = TEX_OFFSET(tex, texSmplr, texCoord, offset, texSize);");
            append_line(buf, &len, "    float4 c1 = TEX_OFFSET(tex, texSmplr, texCoord, float2(offset.x - sign(offset.x), offset.y), texSize);");
            append_line(buf, &len, "    float4 c2 = TEX_OFFSET(tex, texSmplr, texCoord, float2(offset.x, offset.y - sign(offset.y)), texSize);");
            append_line(buf, &len, "    return c0 + abs(offset.x) * (c1 - c0) + abs(offset.y) * (c2 - c0);");
            append_line(buf, &len, "}");


            append_line(buf, &len, "float4 hookTexture2D(thread const texture2d<float> tex, thread const sampler texSmplr, thread const float2& uv, thread const float2& texSize) {");
            append_line(buf, &len, "    return filter3point(tex, texSmplr, uv, texSize);");
            append_line(buf, &len, "}");
        } else {
            append_line(buf, &len, "float4 hookTexture2D(thread const texture2d<float> tex, thread const sampler texSmplr, thread const float2& uv, thread const float2& texSize) {");
            append_line(buf, &len, "   return tex.sample(texSmplr, uv);");
            append_line(buf, &len, "}");
        }

        append_str(buf, &len, "fragment float4 fragmentShader(ProjectedVertex in [[stage_in]], constant FrameUniforms &frameUniforms [[buffer(0)]]");

        if (cc_features.used_textures[0]) {
            append_str(buf, &len, ", texture2d<float> uTex0 [[texture(0)]], sampler uTex0Smplr [[sampler(0)]]");
        }
        if (cc_features.used_textures[1]) {
            append_str(buf, &len, ", texture2d<float> uTex1 [[texture(1)]], sampler uTex1Smplr [[sampler(1)]]");
        }
        append_line(buf, &len, ") {");

        for (int i = 0; i < 2; i++) {
            if (cc_features.used_textures[i]) {
                bool s = cc_features.clamp[i][0], t = cc_features.clamp[i][1];

                len += sprintf(buf + len, "    float2 texSize%d = float2(int2(uTex%d.get_width(), uTex%d.get_height()));\n", i, i, i);

                if (!s && !t) {
                    len += sprintf(buf + len, "    float4 texVal%d = hookTexture2D(uTex%d, uTex%dSmplr, in.texCoord%d, texSize%d);\n", i, i, i, i, i);
                } else {
                    if (s && t) {
                        len += sprintf(buf + len, "    float2 uv%d = fast::clamp(in.texCoord%d, float2(0.5) / texSize%d, float2(in.texClampS%d, in.texClampT%d));\n", i, i, i, i, i);
                        len += sprintf(buf + len, "    float4 texVal%d = hookTexture2D(uTex%d, uTex%dSmplr, uv%d, texSize%d);\n", i, i, i, i, i);
                    } else if (s) {
                        len += sprintf(buf + len, "    float2 uv%d = float2(fast::clamp(in.texCoord%d.x, 0.5 / texSize%d.x, in.texClampS%d), in.texCoord%d.y);\n", i, i, i, i, i);
                        len += sprintf(buf + len, "    float4 texVal%d = hookTexture2D(uTex%d, uTex%dSmplr, uv%d, texSize%d);\n", i, i, i, i, i);
                    } else {
                        len += sprintf(buf + len, "    float2 uv%d = float2(in.texCoord%d.x, fast::clamp(in.texCoord%d.y, 0.5 / texSize%d.y, in.texClampT%d));\n", i, i, i, i, i);
                        len += sprintf(buf + len, "    float4 texVal%d = hookTexture2D(uTex%d, uTex%dSmplr, uv%d, texSize%d);\n", i, i, i, i, i);
                    }
                }
            }
        }

        append_line(buf, &len, cc_features.opt_alpha ? "    float4 texel;" : "    float3 texel;");
        for (int c = 0; c < (cc_features.opt_2cyc ? 2 : 1); c++) {
            append_str(buf, &len, "    texel = ");

            if (!cc_features.color_alpha_same[c] && cc_features.opt_alpha) {
                append_str(buf, &len, "float4(");
                append_formula(buf, &len, cc_features.c[c], cc_features.do_single[c][0], cc_features.do_multiply[c][0], cc_features.do_mix[c][0], false, false, true);
                append_str(buf, &len, ", ");
                append_formula(buf, &len, cc_features.c[c], cc_features.do_single[c][1], cc_features.do_multiply[c][1], cc_features.do_mix[c][1], true, true, true);
                append_str(buf, &len, ")");
            } else {
                append_formula(buf, &len, cc_features.c[c], cc_features.do_single[c][0], cc_features.do_multiply[c][0], cc_features.do_mix[c][0], cc_features.opt_alpha, false, cc_features.opt_alpha);
            }
            append_line(buf, &len, ";");
        }

        if (cc_features.opt_fog) {
            if (cc_features.opt_alpha) {
                append_line(buf, &len, "    texel = float4(mix(texel.xyz, in.fog.xyz, in.fog.w), texel.w);");
            } else {
                append_line(buf, &len, "    texel = mix(texel, in.fog.xyz, in.fog.w);");
            }
        }

        if (cc_features.opt_texture_edge && cc_features.opt_alpha) {
            append_line(buf, &len, "    if (texel.w > 0.19) texel.w = 1.0; else discard_fragment();");
        }

        if (cc_features.opt_alpha && cc_features.opt_noise) {
            append_line(buf, &len,     "texel.w *= floor(fast::clamp(random(float3(floor(in.position.xy * frameUniforms.noiseScale), float(frameUniforms.frameCount))) + texel.w, 0.0, 1.0));");
        }

        if (cc_features.opt_grayscale) {
            append_line(buf, &len, "    float intensity = (texel.x + texel.y + texel.z) / 3.0;");
            append_line(buf, &len, "    float3 new_texel = in.grayscale.xyz * intensity;");
            append_line(buf, &len, "    texel.xyz = mix(texel.xyz, new_texel, in.grayscale.w);");
        }

        if (cc_features.opt_alpha) {
            if (cc_features.opt_alpha_threshold) {
                append_line(buf, &len, "    if (texel.w < 8.0 / 256.0) discard_fragment();");
            }
            if (cc_features.opt_invisible) {
                append_line(buf, &len, "    texel.w = 0.0;");
            }
            append_line(buf, &len, "    return texel;");
        } else {
            append_line(buf, &len, "    return float4(texel, 1.0);");
        }

        append_line(buf, &len, "}");
        // end fragment shader

        vertexDescriptor.layouts[0].stride = num_floats * sizeof(float);
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;

        NSError* error = nil;
        NSString *shaderSource = [NSString stringWithCString:buf encoding:NSUTF8StringEncoding];
        id <MTLLibrary> library = [mctx.device newLibraryWithSource:shaderSource options:nil error:&error];

        if (error != nil) {
            NSLog(@"Failed to compile shader library, error %@", error);
        }

        MTLRenderPipelineDescriptor* pipelineDescriptor = [MTLRenderPipelineDescriptor new];
        pipelineDescriptor.vertexFunction = [library newFunctionWithName:@"vertexShader"];
        pipelineDescriptor.fragmentFunction = [library newFunctionWithName:@"fragmentShader"];
        pipelineDescriptor.vertexDescriptor = vertexDescriptor;

        pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
        pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
        if (cc_features.opt_alpha) {
            pipelineDescriptor.colorAttachments[0].blendingEnabled = YES;
            pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
            pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
            pipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
            pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
            pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
            pipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
            pipelineDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;
        } else {
            pipelineDescriptor.colorAttachments[0].blendingEnabled = NO;
            pipelineDescriptor.colorAttachments[0].writeMask = MTLColorWriteMaskAll;
        }

        id<MTLRenderPipelineState> pipelineState = [mctx.device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];

        if (!pipelineState) {
            // Pipeline State creation could fail if we haven't properly set up our pipeline descriptor.
            // If the Metal API validation is enabled, we can find out more information about what
            // went wrong.  (Metal API validation is enabled by default when a debug build is run
            // from Xcode)
            NSLog(@"Failed to created pipeline state");
        }

        struct ShaderProgramMetal *prg = (struct ShaderProgramMetal *)calloc(1, sizeof(struct ShaderProgramMetal));
        prg->shader_id0 = shader_id0;
        prg->shader_id1 = shader_id1;
        prg->used_textures[0] = cc_features.used_textures[0];
        prg->used_textures[1] = cc_features.used_textures[1];
        prg->num_inputs = cc_features.num_inputs;
        prg->num_floats = num_floats;

        gfx_metal_load_shader((struct ShaderProgram *)prg);

        mctx.shader_program_pool.insert({std::make_pair(shader_id0, shader_id1), *prg});
        mctx.pipeline_cache.insert({ std::make_pair(shader_id0, shader_id1), pipelineState });

        return (struct ShaderProgram *)prg;
    }
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

static void gfx_metal_delete_texture(uint32_t texID) {
    // TODO: implement
    SPDLOG_DEBUG("Metal: asked to delete texture");
}

static void gfx_metal_select_texture(int tile, uint32_t texture_id) {
    mctx.current_tile = tile;
    mctx.current_texture_ids[tile] = texture_id;
}

static void gfx_metal_upload_texture(const uint8_t *rgba32_buf, uint32_t width, uint32_t height) {
    TextureDataMetal *texture_data = &mctx.textures[mctx.current_texture_ids[mctx.current_tile]];

    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:YES];
    textureDescriptor.arrayLength = 1;
    textureDescriptor.mipmapLevelCount = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.storageMode = MTLStorageModeShared;

    id<MTLTexture> texture = [mctx.device newTextureWithDescriptor:textureDescriptor];
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    [texture replaceRegion:region mipmapLevel:0 withBytes:rgba32_buf bytesPerRow:bytesPerRow];


    if (texture_data->texture != nil)
        [texture_data->texture release];

    texture_data->texture = texture;
}

static void gfx_metal_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    TextureDataMetal *texture_data = &mctx.textures[mctx.current_texture_ids[tile]];
    texture_data->linear_filtering = linear_filter;
    
    // This function is called twice per texture, the first one only to set default values.
    // Maybe that could be skipped? Anyway, make sure to release the first default sampler
    // state before setting the actual one.
    [texture_data->sampler release];

    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    MTLSamplerMinMagFilter filter = linear_filter && mctx.current_filter_mode == FILTER_LINEAR ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
    samplerDescriptor.minFilter = filter;
    samplerDescriptor.magFilter = filter;
    samplerDescriptor.sAddressMode = gfx_cm_to_metal(cms);
    samplerDescriptor.tAddressMode = gfx_cm_to_metal(cmt);
    samplerDescriptor.rAddressMode = MTLSamplerAddressModeRepeat;

    texture_data->sampler = [mctx.device newSamplerStateWithDescriptor:samplerDescriptor];
}

static void gfx_metal_set_depth_test_and_mask(bool depth_test, bool depth_mask) {
    mctx.depth_test = depth_test;
    mctx.depth_mask = depth_mask;
}

static void gfx_metal_set_zmode_decal(bool zmode_decal) {
    mctx.zmode_decal = zmode_decal;
}

static void gfx_metal_set_viewport(int x, int y, int width, int height) {
    [mctx.current_command_encoder setViewport:(MTLViewport){ x, y, width, height, 0.0, 1.0 }];
}

static void gfx_metal_set_scissor(int x, int y, int width, int height) {
    [mctx.current_command_encoder setScissorRect:{ x, y, width, height }];
}

static void gfx_metal_set_use_alpha(bool use_alpha) {
    // Already part of the pipeline state from shader info
}

static void gfx_metal_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {
    @autoreleasepool {
        if (mctx.last_depth_test != mctx.depth_test || mctx.last_depth_mask != mctx.depth_mask) {
            mctx.last_depth_test = mctx.depth_test;
            mctx.last_depth_mask = mctx.depth_mask;

            MTLDepthStencilDescriptor* depthDescriptor = [MTLDepthStencilDescriptor new];
            [depthDescriptor setDepthWriteEnabled: mctx.depth_test || mctx.depth_mask ? YES : NO];
            [depthDescriptor setDepthCompareFunction: mctx.depth_test ? MTLCompareFunctionLessEqual : MTLCompareFunctionAlways];

            id<MTLDepthStencilState> depthStencilState = [mctx.device newDepthStencilStateWithDescriptor: depthDescriptor];
            [mctx.current_command_encoder setDepthStencilState:depthStencilState];
        }

        if (mctx.last_zmode_decal != mctx.zmode_decal) {
            mctx.last_zmode_decal = mctx.zmode_decal;

            [mctx.current_command_encoder setDepthBias:0 slopeScale:mctx.zmode_decal ? -2 : 0 clamp:0];
        }

        id<MTLBuffer> vertexBuffer = next_available_buffer();
        memcpy((char *)vertexBuffer.contents + mctx.current_vertex_buffer_offset, buf_vbo, sizeof(float) * buf_vbo_len);
        
        [mctx.current_command_encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
        [mctx.current_command_encoder setVertexBufferOffset:mctx.current_vertex_buffer_offset atIndex:0];
        [mctx.current_command_encoder setFragmentBuffer:mctx.frame_uniform_buffer offset:0 atIndex:0];
        
        for (int i = 0; i < 2; i++) {
            if (mctx.shader_program->used_textures[i]) {
                [mctx.current_command_encoder setFragmentTexture:mctx.textures[mctx.current_texture_ids[i]].texture atIndex:i];
                [mctx.current_command_encoder setFragmentSamplerState:mctx.textures[mctx.current_texture_ids[i]].sampler atIndex:i];
            }
        }


        if (mctx.last_shader_program != mctx.shader_program) {
            mctx.last_shader_program = mctx.shader_program;
            id<MTLRenderPipelineState> pipelineState = mctx.pipeline_cache.find(
                                                                                std::make_pair(mctx  .shader_program->shader_id0, mctx.shader_program->shader_id1)
                                                                                )->second;

            [mctx.current_command_encoder setRenderPipelineState:pipelineState];
            [mctx.current_command_encoder setTriangleFillMode:MTLTriangleFillModeFill];
            [mctx.current_command_encoder setCullMode:MTLCullModeNone];
            [mctx.current_command_encoder setFrontFacingWinding:MTLWindingCounterClockwise];
        }
        
        [mctx.current_command_encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:buf_vbo_num_tris * 3];
        
        mctx.current_vertex_buffer_offset += sizeof(float) * buf_vbo_len;
    }
}

static void gfx_metal_on_resize(void) {}

static void gfx_metal_start_frame(void) {
    SPDLOG_ERROR("[start frame]");

    mctx.frame_uniforms.frameCount++;
    if (mctx.frame_uniforms.frameCount > 150) {
        // No high values, as noise starts to look ugly
        mctx.frame_uniforms.frameCount = 0;
    }

    if (!mctx.frame_uniform_buffer) {
        mctx.frame_uniform_buffer = [mctx.device newBufferWithLength:sizeof(FrameUniforms) options:MTLResourceCPUCacheModeDefaultCache];
    }

    mctx.current_vertex_buffer_offset = 0;

    mctx.frame_autorelease_pool = [[NSAutoreleasePool alloc] init];

}

static void gfx_metal_finished_flush(void) {
    std::set<int>::iterator it = mctx.drawn_framebuffers.begin();
    it++;

    while (it != mctx.drawn_framebuffers.end()) {
        auto framebuffer = mctx.framebuffers[*it];

        [framebuffer.command_encoder endEncoding];
        SPDLOG_ERROR("End encoding for fb: {}", *it);
        pop_buffer_and_wait_to_requeue(framebuffer.command_buffer);
        [framebuffer.command_buffer commit];
        [framebuffer.command_buffer waitUntilCompleted];

        it++;
    }
}

void gfx_metal_end_frame(void) {
    SPDLOG_ERROR("[end frame]");
    auto screenFramebuffer = mctx.framebuffers[0];
    [screenFramebuffer.command_encoder endEncoding];
    SPDLOG_ERROR("End encoding for fb: {}", 0);

    id<CAMetalDrawable> drawable = mctx.layer.nextDrawable;
    id<MTLBlitCommandEncoder> blitEncoder = [screenFramebuffer.command_buffer blitCommandEncoder];

    // // Copy over the 0 framebuffer's texture to the drawable!
    int texture_id = mctx.framebuffers[0].texture_id;
    id<MTLTexture> texToCopy = mctx.textures[texture_id].texture;

    [blitEncoder copyFromTexture:texToCopy toTexture:drawable.texture];
    [blitEncoder endEncoding];

    [screenFramebuffer.command_buffer presentDrawable:drawable];

    pop_buffer_and_wait_to_requeue(screenFramebuffer.command_buffer);
    [screenFramebuffer.command_buffer commit];

    mctx.drawn_framebuffers.clear();

    // Cleanup states
    mctx.last_shader_program = nullptr;
    mctx.current_command_encoder = nullptr;

    for (int fb_id = 0; fb_id < (int)mctx.framebuffers.size(); fb_id++) {
        FramebufferMetal& fb = mctx.framebuffers[fb_id];
        fb.command_buffer = nullptr;
        fb.command_encoder = nullptr;
    }

    [mctx.frame_autorelease_pool release];
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

static void gfx_metal_update_framebuffer_parameters(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level, bool opengl_invert_y, bool render_target, bool has_depth_buffer, bool can_extract_depth) {
    SPDLOG_ERROR("Being asked to update framebuffer params: {}", fb_id);
    // TODO: implement
    FramebufferMetal& fb = mctx.framebuffers[fb_id];
    TextureDataMetal& tex = mctx.textures[fb.texture_id];

    width = std::max(width, 1U);
    height = std::max(height, 1U);
//    while (msaa_level > 1 && mctx.msaa_num_quality_levels[msaa_level - 1] == 0) {
//        --msaa_level;
//    }

    bool diff = tex.width != width || tex.height != height || fb.msaa_level != msaa_level;

    // TODO: something about diff and creating textures

    if (diff || (fb.render_pass_descriptor != nil) != render_target) {
        // create drawable texture
        MTLTextureDescriptor *texDescriptor = [MTLTextureDescriptor new];
        texDescriptor.textureType = MTLTextureType2D;
        texDescriptor.width = width;
        texDescriptor.height = height;
        texDescriptor.sampleCount = msaa_level;
        texDescriptor.mipmapLevelCount = 1;
        texDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;
        texDescriptor.usage = (render_target ? MTLTextureUsageRenderTarget : 0) | (msaa_level <= 1 ? MTLTextureUsageShaderRead : 0);

        tex.texture = [mctx.device newTextureWithDescriptor:texDescriptor];

        if (render_target) {
            MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];

            MTLClearColor clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
            renderPassDescriptor.colorAttachments[0].texture = tex.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            renderPassDescriptor.colorAttachments[0].clearColor = clearColor;

            fb.render_pass_descriptor = renderPassDescriptor;
        }

        tex.width = width;
        tex.height = height;
    }

    if (has_depth_buffer && (diff || !fb.has_depth_buffer || (fb.depth_texture != nil) != can_extract_depth)) {
        MTLTextureDescriptor *depthTexDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                                                                                width:width
                                                                                               height:height
                                                                                            mipmapped:YES];

        depthTexDesc.storageMode = MTLStorageModePrivate;
        depthTexDesc.sampleCount = msaa_level;
        depthTexDesc.arrayLength = 1;
        depthTexDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
        fb.depth_texture = [mctx.device newTextureWithDescriptor:depthTexDesc];
    }

    if (has_depth_buffer) {
        fb.render_pass_descriptor.depthAttachment.texture = fb.depth_texture;
        fb.render_pass_descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        fb.render_pass_descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        fb.render_pass_descriptor.depthAttachment.clearDepth = 1;
    } else {
        fb.render_pass_descriptor.depthAttachment = nil;
    }

    fb.render_target = render_target;
    fb.has_depth_buffer = has_depth_buffer;
    fb.msaa_level = msaa_level;
}

void gfx_metal_start_draw_to_framebuffer(int fb_id, float noise_scale) {
    // TODO: Implement
    FramebufferMetal& fb = mctx.framebuffers[fb_id];
    mctx.current_framebuffer = fb_id;
    SPDLOG_ERROR("Being asked to draw to framebuffer: {}", fb_id);

    if (fb.render_target && fb.command_buffer == nil && fb.command_encoder == nil) {
        fb.command_buffer = [mctx.command_queue commandBuffer];
        fb.command_buffer.label = [NSString stringWithFormat:@"FrameBuffer (%d) Command Buffer", fb_id];

        fb.command_encoder = [fb.command_buffer renderCommandEncoderWithDescriptor:fb.render_pass_descriptor];
        fb.command_encoder.label = [NSString stringWithFormat:@"FrameBuffer (%d) Render Pass", fb_id];
    }

    MTLRenderPassDescriptor *current_render_pass = mctx.framebuffers[mctx.current_framebuffer].render_pass_descriptor;
    ImGui_ImplMetal_NewFrame(current_render_pass);

    mctx.current_command_encoder = mctx.framebuffers[mctx.current_framebuffer].command_encoder;
    mctx.drawn_framebuffers.insert(fb_id);

    if (noise_scale != 0.0f) {
        mctx.frame_uniforms.noiseScale = 1.0f / noise_scale;
    }

    memcpy(mctx.frame_uniform_buffer.contents, &mctx.frame_uniforms, sizeof(FrameUniforms));
}

void gfx_metal_clear_framebuffer(void) {
    // TODO: implement
}

void gfx_metal_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    // TODO: implement
}

std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> gfx_metal_get_pixel_depth(int fb_id, const std::set<std::pair<float, float>>& coordinates) {
    // TODO: implement
    std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> res;
    return res;
}

void *gfx_metal_get_framebuffer_texture_id(int fb_id) {
    if (fb_id != 0) {
        auto texture = mctx.textures[mctx.framebuffers[fb_id].texture_id].texture;
        SPDLOG_ERROR("Being asked for texture framebuffer: {}", fb_id);
    }
    return (__bridge void*)mctx.textures[mctx.framebuffers[fb_id].texture_id].texture;
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
    return (__bridge void*)mctx.textures[fb_id].texture;
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
    gfx_metal_finished_flush,
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
