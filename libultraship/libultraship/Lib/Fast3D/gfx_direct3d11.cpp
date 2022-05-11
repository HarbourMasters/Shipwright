#ifdef ENABLE_DX11

#include <cstdio>
#include <vector>
#include <cmath>

#include <windows.h>
#include <versionhelpers.h>
#include <wrl/client.h>

#include <dxgi1_3.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include "PR/ultra64/gbi.h"

#include "gfx_window_manager_api.h"
#include "gfx_direct3d_common.h"

#define DECLARE_GFX_DXGI_FUNCTIONS
#include "gfx_dxgi.h"

#include "gfx_screen_config.h"
#include "../../SohImGuiImpl.h"

#include "gfx_cc.h"
#include "gfx_rendering_api.h"
#include "gfx_pc.h"
#define DEBUG_D3D 0

using namespace Microsoft::WRL; // For ComPtr

namespace {

struct PerFrameCB {
    uint32_t noise_frame;
    float noise_scale;
    uint32_t padding[2]; // constant buffers must be multiples of 16 bytes in size
};

struct PerDrawCB {
    struct Texture {
        uint32_t width;
        uint32_t height;
        uint32_t linear_filtering;
        uint32_t padding;
    } textures[2];
};

struct Coord {
    int x, y;
};

struct TextureData {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11ShaderResourceView> resource_view;
    ComPtr<ID3D11SamplerState> sampler_state;
    uint32_t width;
    uint32_t height;
    bool linear_filtering;
};

struct Framebuffer {
    ComPtr<ID3D11RenderTargetView> render_target_view;
    ComPtr<ID3D11DepthStencilView> depth_stencil_view;
    ComPtr<ID3D11ShaderResourceView> depth_stencil_srv;
    uint32_t texture_id;
    bool has_depth_buffer;
    uint32_t msaa_level;
};

struct ShaderProgramD3D11 {
    ComPtr<ID3D11VertexShader> vertex_shader;
    ComPtr<ID3D11PixelShader> pixel_shader;
    ComPtr<ID3D11InputLayout> input_layout;
    ComPtr<ID3D11BlendState> blend_state;

    uint64_t shader_id0;
    uint32_t shader_id1;
    uint8_t num_inputs;
    uint8_t num_floats;
    bool used_textures[2];
};

static struct {
    HMODULE d3d11_module;
    PFN_D3D11_CREATE_DEVICE D3D11CreateDevice;

    HMODULE d3dcompiler_module;
    pD3DCompile D3DCompile;

    D3D_FEATURE_LEVEL feature_level;
    uint32_t msaa_num_quality_levels[D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT];

    ComPtr<ID3D11Device> device;
    ComPtr<IDXGISwapChain1> swap_chain;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11RasterizerState> rasterizer_state;
    ComPtr<ID3D11DepthStencilState> depth_stencil_state;
    ComPtr<ID3D11Buffer> vertex_buffer;
    ComPtr<ID3D11Buffer> per_frame_cb;
    ComPtr<ID3D11Buffer> per_draw_cb;
    ComPtr<ID3D11Buffer> coord_buffer;
    ComPtr<ID3D11ShaderResourceView> coord_buffer_srv;
    ComPtr<ID3D11Buffer> depth_value_output_buffer;
    ComPtr<ID3D11Buffer> depth_value_output_buffer_copy;
    ComPtr<ID3D11UnorderedAccessView> depth_value_output_uav;
    ComPtr<ID3D11ComputeShader> compute_shader;
    ComPtr<ID3D11ComputeShader> compute_shader_msaa;
    ComPtr<ID3DBlob> compute_shader_msaa_blob;
    size_t coord_buffer_size;

#if DEBUG_D3D
    ComPtr<ID3D11Debug> debug;
#endif

    PerFrameCB per_frame_cb_data;
    PerDrawCB per_draw_cb_data;

    std::map<std::pair<uint64_t, uint32_t>, struct ShaderProgramD3D11> shader_program_pool;

    std::vector<struct TextureData> textures;
    int current_tile;
    uint32_t current_texture_ids[2];

    std::vector<Framebuffer> framebuffers;

    // Current state

    struct ShaderProgramD3D11 *shader_program;

    //uint32_t current_width, current_height;
    uint32_t render_target_height;
    int current_framebuffer;
    FilteringMode current_filter_mode = NONE;

    int8_t depth_test;
    int8_t depth_mask;
    int8_t zmode_decal;

    // Previous states (to prevent setting states needlessly)

    struct ShaderProgramD3D11 *last_shader_program = nullptr;
    uint32_t last_vertex_buffer_stride = 0;
    ComPtr<ID3D11BlendState> last_blend_state = nullptr;
    ComPtr<ID3D11ShaderResourceView> last_resource_views[2] = { nullptr, nullptr };
    ComPtr<ID3D11SamplerState> last_sampler_states[2] = { nullptr, nullptr };
    int8_t last_depth_test = -1;
    int8_t last_depth_mask = -1;
    int8_t last_zmode_decal = -1;
    D3D_PRIMITIVE_TOPOLOGY last_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
} d3d;

static LARGE_INTEGER last_time, accumulated_time, frequency;

int gfx_d3d11_create_framebuffer(void);

static void create_depth_stencil_objects(uint32_t width, uint32_t height, uint32_t msaa_count, ID3D11DepthStencilView **view, ID3D11ShaderResourceView **srv) {
    D3D11_TEXTURE2D_DESC texture_desc;
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = d3d.feature_level >= D3D_FEATURE_LEVEL_10_0 ?
        DXGI_FORMAT_R32_TYPELESS : DXGI_FORMAT_R24G8_TYPELESS;
    texture_desc.SampleDesc.Count = msaa_count;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | (srv != nullptr ? D3D11_BIND_SHADER_RESOURCE : 0);
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    ComPtr<ID3D11Texture2D> texture;
    ThrowIfFailed(d3d.device->CreateTexture2D(&texture_desc, nullptr, texture.GetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC view_desc;
    view_desc.Format = d3d.feature_level >= D3D_FEATURE_LEVEL_10_0 ?
        DXGI_FORMAT_D32_FLOAT : DXGI_FORMAT_D24_UNORM_S8_UINT;
    view_desc.Flags = 0;
    if (msaa_count > 1) {
        view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        view_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
    } else {
        view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        view_desc.Texture2D.MipSlice = 0;
    }

    ThrowIfFailed(d3d.device->CreateDepthStencilView(texture.Get(), &view_desc, view));

    if (srv != nullptr) {
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        srv_desc.Format = d3d.feature_level >= D3D_FEATURE_LEVEL_10_0 ?
            DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srv_desc.ViewDimension = msaa_count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MostDetailedMip = 0;
        srv_desc.Texture2D.MipLevels = -1;

        ThrowIfFailed(d3d.device->CreateShaderResourceView(texture.Get(), &srv_desc, srv));
    }
}

static void gfx_d3d11_init(void) {
    // Load d3d11.dll
    d3d.d3d11_module = LoadLibraryW(L"d3d11.dll");
    if (d3d.d3d11_module == nullptr) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), gfx_dxgi_get_h_wnd(), "d3d11.dll not found");
    }
    d3d.D3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(d3d.d3d11_module, "D3D11CreateDevice");

    // Load D3DCompiler_47.dll
    d3d.d3dcompiler_module = LoadLibraryW(L"D3DCompiler_47.dll");
    if (d3d.d3dcompiler_module == nullptr) {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()), gfx_dxgi_get_h_wnd(), "D3DCompiler_47.dll not found");
    }
    d3d.D3DCompile = (pD3DCompile)GetProcAddress(d3d.d3dcompiler_module, "D3DCompile");

    // Create D3D11 device

    gfx_dxgi_create_factory_and_device(DEBUG_D3D, 11, [](IDXGIAdapter1 *adapter, bool test_only) {
#if DEBUG_D3D
        UINT device_creation_flags = D3D11_CREATE_DEVICE_DEBUG;
#else
        UINT device_creation_flags = 0;
#endif
        D3D_FEATURE_LEVEL FeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };

        HRESULT res = d3d.D3D11CreateDevice(
            adapter,
            D3D_DRIVER_TYPE_UNKNOWN, // since we use a specific adapter
            nullptr,
            device_creation_flags,
            FeatureLevels,
            ARRAYSIZE(FeatureLevels),
            D3D11_SDK_VERSION,
            test_only ? nullptr : d3d.device.GetAddressOf(),
            &d3d.feature_level,
            test_only ? nullptr : d3d.context.GetAddressOf());

        if (test_only) {
            return SUCCEEDED(res);
        } else {
            ThrowIfFailed(res, gfx_dxgi_get_h_wnd(), "Failed to create D3D11 device.");
            return true;
        }
    });

    // Create the swap chain
    d3d.swap_chain = gfx_dxgi_create_swap_chain(d3d.device.Get());

    // Create D3D Debug device if in debug mode

#if DEBUG_D3D
    ThrowIfFailed(d3d.device->QueryInterface(__uuidof(ID3D11Debug), (void **) d3d.debug.GetAddressOf()),
                  gfx_dxgi_get_h_wnd(), "Failed to get ID3D11Debug device.");
#endif

    // Create the default framebuffer which represents the window
    Framebuffer& fb = d3d.framebuffers[gfx_d3d11_create_framebuffer()];

    // Check the size of the window
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    ThrowIfFailed(d3d.swap_chain->GetDesc1(&swap_chain_desc));
    d3d.textures[fb.texture_id].width = swap_chain_desc.Width;
    d3d.textures[fb.texture_id].height = swap_chain_desc.Height;
    fb.msaa_level = 1;

    for (uint32_t sample_count = 1; sample_count <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; sample_count++) {
        ThrowIfFailed(d3d.device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sample_count, &d3d.msaa_num_quality_levels[sample_count - 1]));
    }

    // Create main vertex buffer

    D3D11_BUFFER_DESC vertex_buffer_desc;
    ZeroMemory(&vertex_buffer_desc, sizeof(D3D11_BUFFER_DESC));

    vertex_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    vertex_buffer_desc.ByteWidth = 256 * 32 * 3 * sizeof(float); // Same as buf_vbo size in gfx_pc
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertex_buffer_desc.MiscFlags = 0;

    ThrowIfFailed(d3d.device->CreateBuffer(&vertex_buffer_desc, nullptr, d3d.vertex_buffer.GetAddressOf()),
                  gfx_dxgi_get_h_wnd(), "Failed to create vertex buffer.");

    // Create per-frame constant buffer

    D3D11_BUFFER_DESC constant_buffer_desc;
    ZeroMemory(&constant_buffer_desc, sizeof(D3D11_BUFFER_DESC));

    constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    constant_buffer_desc.ByteWidth = sizeof(PerFrameCB);
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constant_buffer_desc.MiscFlags = 0;

    ThrowIfFailed(d3d.device->CreateBuffer(&constant_buffer_desc, nullptr, d3d.per_frame_cb.GetAddressOf()),
                  gfx_dxgi_get_h_wnd(), "Failed to create per-frame constant buffer.");

    d3d.context->PSSetConstantBuffers(0, 1, d3d.per_frame_cb.GetAddressOf());

    // Create per-draw constant buffer

    constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    constant_buffer_desc.ByteWidth = sizeof(PerDrawCB);
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constant_buffer_desc.MiscFlags = 0;

    ThrowIfFailed(d3d.device->CreateBuffer(&constant_buffer_desc, nullptr, d3d.per_draw_cb.GetAddressOf()),
                  gfx_dxgi_get_h_wnd(), "Failed to create per-draw constant buffer.");

    d3d.context->PSSetConstantBuffers(1, 1, d3d.per_draw_cb.GetAddressOf());

    // Create compute shader that can be used to retrieve depth buffer values

    const char* shader_source = R"(
sampler my_sampler : register(s0);
Texture2D<float> tex : register(t0);
StructuredBuffer<int2> coord : register(t1);
RWStructuredBuffer<float> output : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    output[DTid.x] = tex.Load(int3(coord[DTid.x], 0));
}
)";

    const char* shader_source_msaa = R"(
sampler my_sampler : register(s0);
Texture2DMS<float, 2> tex : register(t0);
StructuredBuffer<int2> coord : register(t1);
RWStructuredBuffer<float> output : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID) {
    output[DTid.x] = tex.Load(coord[DTid.x], 0);
}
)";

#if DEBUG_D3D
    UINT compile_flags = D3DCOMPILE_DEBUG;
#else
    UINT compile_flags = D3DCOMPILE_OPTIMIZATION_LEVEL2;
#endif

    ComPtr<ID3DBlob> cs, error_blob;
    HRESULT hr;

    hr = d3d.D3DCompile(shader_source, strlen(shader_source), nullptr, nullptr, nullptr, "CSMain", "cs_4_0", compile_flags, 0, cs.GetAddressOf(), error_blob.GetAddressOf());

    if (FAILED(hr)) {
        char* err = (char*)error_blob->GetBufferPointer();
        MessageBoxA(gfx_dxgi_get_h_wnd(), err, "Error", MB_OK | MB_ICONERROR);
        throw hr;
    }

    ThrowIfFailed(d3d.device->CreateComputeShader(cs->GetBufferPointer(), cs->GetBufferSize(), nullptr, d3d.compute_shader.GetAddressOf()));

    hr = d3d.D3DCompile(shader_source_msaa, strlen(shader_source_msaa), nullptr, nullptr, nullptr, "CSMain", "cs_4_1", compile_flags, 0, d3d.compute_shader_msaa_blob.GetAddressOf(), error_blob.ReleaseAndGetAddressOf());

    if (FAILED(hr)) {
        char* err = (char*)error_blob->GetBufferPointer();
        MessageBoxA(gfx_dxgi_get_h_wnd(), err, "Error", MB_OK | MB_ICONERROR);
        throw hr;
    }

    // Create ImGui

    SohImGui::WindowImpl window_impl;
    window_impl.backend = SohImGui::Backend::DX11;
    window_impl.dx11 = { gfx_dxgi_get_h_wnd(), d3d.context.Get(), d3d.device.Get() };
    SohImGui::Init(window_impl);
}


static struct GfxClipParameters gfx_d3d11_get_clip_parameters(void) {
    return { true, false };
}

static void gfx_d3d11_unload_shader(struct ShaderProgram *old_prg) {
}

static void gfx_d3d11_load_shader(struct ShaderProgram *new_prg) {
    d3d.shader_program = (struct ShaderProgramD3D11 *)new_prg;
}

static struct ShaderProgram *gfx_d3d11_create_and_load_new_shader(uint64_t shader_id0, uint32_t shader_id1) {
    CCFeatures cc_features;
    gfx_cc_get_features(shader_id0, shader_id1, &cc_features);

    char buf[4096];
    size_t len, num_floats;

    gfx_direct3d_common_build_shader(buf, len, num_floats, cc_features, false, d3d.current_filter_mode == THREE_POINT);

    ComPtr<ID3DBlob> vs, ps;
    ComPtr<ID3DBlob> error_blob;

#if DEBUG_D3D
    UINT compile_flags = D3DCOMPILE_DEBUG;
#else
    UINT compile_flags = D3DCOMPILE_OPTIMIZATION_LEVEL2;
#endif

    HRESULT hr = d3d.D3DCompile(buf, len, nullptr, nullptr, nullptr, "VSMain", "vs_4_0", compile_flags, 0, vs.GetAddressOf(), error_blob.GetAddressOf());

    if (FAILED(hr)) {
        char* err = (char*)error_blob->GetBufferPointer();
        MessageBoxA(gfx_dxgi_get_h_wnd(), err, "Error", MB_OK | MB_ICONERROR);
        throw hr;
    }

    hr = d3d.D3DCompile(buf, len, nullptr, nullptr, nullptr, "PSMain", "ps_4_0", compile_flags, 0, ps.GetAddressOf(), error_blob.GetAddressOf());

    if (FAILED(hr)) {
        char* err = (char*)error_blob->GetBufferPointer();
        MessageBoxA(gfx_dxgi_get_h_wnd(), err, "Error", MB_OK | MB_ICONERROR);
        throw hr;
    }

    struct ShaderProgramD3D11 *prg = &d3d.shader_program_pool[std::make_pair(shader_id0, shader_id1)];

    ThrowIfFailed(d3d.device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, prg->vertex_shader.GetAddressOf()));
    ThrowIfFailed(d3d.device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, prg->pixel_shader.GetAddressOf()));

    // Input Layout

    D3D11_INPUT_ELEMENT_DESC ied[16];
    uint8_t ied_index = 0;
    ied[ied_index++] = { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    for (UINT i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            ied[ied_index++] = { "TEXCOORD", i, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            if (cc_features.clamp[i][0]) {
                ied[ied_index++] = { "TEXCLAMPS", i, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            }
            if (cc_features.clamp[i][1]) {
                ied[ied_index++] = { "TEXCLAMPT", i, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            }
        }
    }
    if (cc_features.opt_fog) {
        ied[ied_index++] = { "FOG", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    }
    if (cc_features.opt_grayscale) {
        ied[ied_index++] = { "GRAYSCALE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    }
    for (unsigned int i = 0; i < cc_features.num_inputs; i++) {
        DXGI_FORMAT format = cc_features.opt_alpha ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
        ied[ied_index++] = { "INPUT", i, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
    }

    ThrowIfFailed(d3d.device->CreateInputLayout(ied, ied_index, vs->GetBufferPointer(), vs->GetBufferSize(), prg->input_layout.GetAddressOf()));

    // Blend state

    D3D11_BLEND_DESC blend_desc;
    ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

    if (cc_features.opt_alpha) {
        blend_desc.RenderTarget[0].BlendEnable = true;
        blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; // We initially clear alpha to 1.0f and want to keep it at 1.0f
        blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    } else {
        blend_desc.RenderTarget[0].BlendEnable = false;
        blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    ThrowIfFailed(d3d.device->CreateBlendState(&blend_desc, prg->blend_state.GetAddressOf()));

    // Save some values

    prg->shader_id0 = shader_id0;
    prg->shader_id1 = shader_id1;
    prg->num_inputs = cc_features.num_inputs;
    prg->num_floats = num_floats;
    prg->used_textures[0] = cc_features.used_textures[0];
    prg->used_textures[1] = cc_features.used_textures[1];

    return (struct ShaderProgram *)(d3d.shader_program = prg);
}

static struct ShaderProgram *gfx_d3d11_lookup_shader(uint64_t shader_id0, uint32_t shader_id1) {
    auto it = d3d.shader_program_pool.find(std::make_pair(shader_id0, shader_id1));
    return it == d3d.shader_program_pool.end() ? nullptr : (struct ShaderProgram *)&it->second;
}

static void gfx_d3d11_shader_get_info(struct ShaderProgram *prg, uint8_t *num_inputs, bool used_textures[2]) {
    struct ShaderProgramD3D11 *p = (struct ShaderProgramD3D11 *)prg;

    *num_inputs = p->num_inputs;
    used_textures[0] = p->used_textures[0];
    used_textures[1] = p->used_textures[1];
}

static uint32_t gfx_d3d11_new_texture(void) {
    d3d.textures.resize(d3d.textures.size() + 1);
    return (uint32_t)(d3d.textures.size() - 1);
}

static void gfx_d3d11_delete_texture(uint32_t texID) {
    //glDeleteTextures(1, &texID);
}

static void gfx_d3d11_select_texture(int tile, uint32_t texture_id) {
    d3d.current_tile = tile;
    d3d.current_texture_ids[tile] = texture_id;
}

static D3D11_TEXTURE_ADDRESS_MODE gfx_cm_to_d3d11(uint32_t val) {
    // TODO: handle G_TX_MIRROR | G_TX_CLAMP
    if (val & G_TX_CLAMP) {
        return D3D11_TEXTURE_ADDRESS_CLAMP;
    }
    return (val & G_TX_MIRROR) ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
}

static void gfx_d3d11_upload_texture(const uint8_t *rgba32_buf, uint32_t width, uint32_t height) {
    // Create texture

    TextureData *texture_data = &d3d.textures[d3d.current_texture_ids[d3d.current_tile]];
    texture_data->width = width;
    texture_data->height = height;

    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(D3D11_TEXTURE2D_DESC));

    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0; // D3D11_RESOURCE_MISC_GENERATE_MIPS ?
    texture_desc.ArraySize = 1;
    texture_desc.MipLevels = 1;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;

    D3D11_SUBRESOURCE_DATA resource_data;
    resource_data.pSysMem = rgba32_buf;
    resource_data.SysMemPitch = width * 4;
    resource_data.SysMemSlicePitch = resource_data.SysMemPitch * height;

    ThrowIfFailed(d3d.device->CreateTexture2D(&texture_desc, &resource_data, texture_data->texture.ReleaseAndGetAddressOf()));

    // Create shader resource view from texture

    ThrowIfFailed(d3d.device->CreateShaderResourceView(texture_data->texture.Get(), nullptr, texture_data->resource_view.ReleaseAndGetAddressOf()));
}

static void gfx_d3d11_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt) {
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(D3D11_SAMPLER_DESC));

    sampler_desc.Filter = linear_filter && d3d.current_filter_mode == LINEAR ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;

    sampler_desc.AddressU = gfx_cm_to_d3d11(cms);
    sampler_desc.AddressV = gfx_cm_to_d3d11(cmt);
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    TextureData *texture_data = &d3d.textures[d3d.current_texture_ids[tile]];
    texture_data->linear_filtering = linear_filter;

    // This function is called twice per texture, the first one only to set default values.
    // Maybe that could be skipped? Anyway, make sure to release the first default sampler
    // state before setting the actual one.
    texture_data->sampler_state.Reset();

    ThrowIfFailed(d3d.device->CreateSamplerState(&sampler_desc, texture_data->sampler_state.GetAddressOf()));
}

static void gfx_d3d11_set_depth_test_and_mask(bool depth_test, bool depth_mask) {
    d3d.depth_test = depth_test;
    d3d.depth_mask = depth_mask;
}

static void gfx_d3d11_set_zmode_decal(bool zmode_decal) {
    d3d.zmode_decal = zmode_decal;
}

static void gfx_d3d11_set_viewport(int x, int y, int width, int height) {
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = x;
    viewport.TopLeftY = d3d.render_target_height - y - height;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    d3d.context->RSSetViewports(1, &viewport);
}

static void gfx_d3d11_set_scissor(int x, int y, int width, int height) {
    D3D11_RECT rect;
    rect.left = x;
    rect.top = d3d.render_target_height - y - height;
    rect.right = x + width;
    rect.bottom = d3d.render_target_height - y;

    d3d.context->RSSetScissorRects(1, &rect);
}

static void gfx_d3d11_set_use_alpha(bool use_alpha) {
    // Already part of the pipeline state from shader info
}

static void gfx_d3d11_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris) {

    if (d3d.last_depth_test != d3d.depth_test || d3d.last_depth_mask != d3d.depth_mask) {
        d3d.last_depth_test = d3d.depth_test;
        d3d.last_depth_mask = d3d.depth_mask;

        d3d.depth_stencil_state.Reset();

        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
        ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

        depth_stencil_desc.DepthEnable = d3d.depth_test || d3d.depth_mask;
        depth_stencil_desc.DepthWriteMask = d3d.depth_mask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depth_stencil_desc.DepthFunc = d3d.depth_test ? D3D11_COMPARISON_LESS_EQUAL : D3D11_COMPARISON_ALWAYS;
        depth_stencil_desc.StencilEnable = false;

        ThrowIfFailed(d3d.device->CreateDepthStencilState(&depth_stencil_desc, d3d.depth_stencil_state.GetAddressOf()));
        d3d.context->OMSetDepthStencilState(d3d.depth_stencil_state.Get(), 0);
    }

    if (d3d.last_zmode_decal != d3d.zmode_decal) {
        d3d.last_zmode_decal = d3d.zmode_decal;

        d3d.rasterizer_state.Reset();

        D3D11_RASTERIZER_DESC rasterizer_desc;
        ZeroMemory(&rasterizer_desc, sizeof(D3D11_RASTERIZER_DESC));

        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.FrontCounterClockwise = true;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.SlopeScaledDepthBias = d3d.zmode_decal ? -2.0f : 0.0f;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = false;
        rasterizer_desc.ScissorEnable = true;
        rasterizer_desc.MultisampleEnable = false;
        rasterizer_desc.AntialiasedLineEnable = false;

        ThrowIfFailed(d3d.device->CreateRasterizerState(&rasterizer_desc, d3d.rasterizer_state.GetAddressOf()));
        d3d.context->RSSetState(d3d.rasterizer_state.Get());
    }

    bool textures_changed = false;

    for (int i = 0; i < 2; i++) {
        if (d3d.shader_program->used_textures[i]) {
            if (d3d.last_resource_views[i].Get() != d3d.textures[d3d.current_texture_ids[i]].resource_view.Get()) {
                d3d.last_resource_views[i] = d3d.textures[d3d.current_texture_ids[i]].resource_view.Get();
                d3d.context->PSSetShaderResources(i, 1, d3d.textures[d3d.current_texture_ids[i]].resource_view.GetAddressOf());

                if (d3d.current_filter_mode == THREE_POINT) {
                    d3d.per_draw_cb_data.textures[i].width = d3d.textures[d3d.current_texture_ids[i]].width;
                    d3d.per_draw_cb_data.textures[i].height = d3d.textures[d3d.current_texture_ids[i]].height;
                    d3d.per_draw_cb_data.textures[i].linear_filtering = d3d.textures[d3d.current_texture_ids[i]].linear_filtering;
                    textures_changed = true;
                }

                if (d3d.last_sampler_states[i].Get() != d3d.textures[d3d.current_texture_ids[i]].sampler_state.Get()) {
                    d3d.last_sampler_states[i] = d3d.textures[d3d.current_texture_ids[i]].sampler_state.Get();
                    d3d.context->PSSetSamplers(i, 1, d3d.textures[d3d.current_texture_ids[i]].sampler_state.GetAddressOf());
                }
            }
        }
    }

    // Set per-draw constant buffer

    if (textures_changed) {
        D3D11_MAPPED_SUBRESOURCE ms;
        ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
        d3d.context->Map(d3d.per_draw_cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, &d3d.per_draw_cb_data, sizeof(PerDrawCB));
        d3d.context->Unmap(d3d.per_draw_cb.Get(), 0);
    }

    // Set vertex buffer data

    D3D11_MAPPED_SUBRESOURCE ms;
    ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
    d3d.context->Map(d3d.vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, buf_vbo, buf_vbo_len * sizeof(float));
    d3d.context->Unmap(d3d.vertex_buffer.Get(), 0);

    uint32_t stride = d3d.shader_program->num_floats * sizeof(float);
    uint32_t offset = 0;

    if (d3d.last_vertex_buffer_stride != stride) {
        d3d.last_vertex_buffer_stride = stride;
        d3d.context->IASetVertexBuffers(0, 1, d3d.vertex_buffer.GetAddressOf(), &stride, &offset);
    }

    if (d3d.last_shader_program != d3d.shader_program) {
        d3d.last_shader_program = d3d.shader_program;
        d3d.context->IASetInputLayout(d3d.shader_program->input_layout.Get());
        d3d.context->VSSetShader(d3d.shader_program->vertex_shader.Get(), 0, 0);
        d3d.context->PSSetShader(d3d.shader_program->pixel_shader.Get(), 0, 0);

        if (d3d.last_blend_state.Get() != d3d.shader_program->blend_state.Get()) {
            d3d.last_blend_state = d3d.shader_program->blend_state.Get();
            d3d.context->OMSetBlendState(d3d.shader_program->blend_state.Get(), 0, 0xFFFFFFFF);
        }
    }

    if (d3d.last_primitive_topology != D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {
        d3d.last_primitive_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        d3d.context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    d3d.context->Draw(buf_vbo_num_tris * 3, 0);
}

static void gfx_d3d11_on_resize(void) {
    //create_render_target_views(true);
}

static void gfx_d3d11_start_frame(void) {
    // Set per-frame constant buffer

    d3d.per_frame_cb_data.noise_frame++;
    if (d3d.per_frame_cb_data.noise_frame > 150) {
        // No high values, as noise starts to look ugly
        d3d.per_frame_cb_data.noise_frame = 0;
    }
}

static void gfx_d3d11_end_frame(void) {
    d3d.context->Flush();
}

static void gfx_d3d11_finish_render(void) {
    d3d.context->Flush();
}

int gfx_d3d11_create_framebuffer(void) {
    uint32_t texture_id = gfx_d3d11_new_texture();
    TextureData& t = d3d.textures[texture_id];

    size_t index = d3d.framebuffers.size();
    d3d.framebuffers.resize(d3d.framebuffers.size() + 1);
    Framebuffer& data = d3d.framebuffers.back();
    data.texture_id = texture_id;

    uint32_t tile = 0;
    uint32_t saved = d3d.current_texture_ids[tile];
    d3d.current_texture_ids[tile] = texture_id;
    gfx_d3d11_set_sampler_parameters(0, true, G_TX_WRAP, G_TX_WRAP);
    d3d.current_texture_ids[tile] = saved;

    return (int)index;
}

static void gfx_d3d11_update_framebuffer_parameters(int fb_id, uint32_t width, uint32_t height, uint32_t msaa_level, bool opengl_invert_y, bool render_target, bool has_depth_buffer, bool can_extract_depth) {
    Framebuffer& fb = d3d.framebuffers[fb_id];
    TextureData& tex = d3d.textures[fb.texture_id];

    width = max(width, 1U);
    height = max(height, 1U);
    while (msaa_level > 1 && d3d.msaa_num_quality_levels[msaa_level - 1] == 0) {
        --msaa_level;
    }

    bool diff = tex.width != width || tex.height != height || fb.msaa_level != msaa_level;

    if (diff || (fb.render_target_view.Get() != nullptr) != render_target) {
        if (fb_id != 0) {
            D3D11_TEXTURE2D_DESC texture_desc;
            texture_desc.Width = width;
            texture_desc.Height = height;
            texture_desc.Usage = D3D11_USAGE_DEFAULT;
            texture_desc.BindFlags = (msaa_level <= 1 ? D3D11_BIND_SHADER_RESOURCE : 0) | (render_target ? D3D11_BIND_RENDER_TARGET : 0);
            texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            texture_desc.CPUAccessFlags = 0;
            texture_desc.MiscFlags = 0;
            texture_desc.ArraySize = 1;
            texture_desc.MipLevels = 1;
            texture_desc.SampleDesc.Count = msaa_level;
            texture_desc.SampleDesc.Quality = 0;

            ThrowIfFailed(d3d.device->CreateTexture2D(&texture_desc, nullptr, tex.texture.ReleaseAndGetAddressOf()));

            if (msaa_level <= 1) {
                ThrowIfFailed(d3d.device->CreateShaderResourceView(tex.texture.Get(), nullptr, tex.resource_view.ReleaseAndGetAddressOf()));
            }
        } else if (diff) {
            DXGI_SWAP_CHAIN_DESC1 desc1;
            ThrowIfFailed(d3d.swap_chain->GetDesc1(&desc1));
            if (desc1.Width != width || desc1.Height != height) {
                fb.render_target_view.Reset();
                tex.texture.Reset();
                ThrowIfFailed(d3d.swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, desc1.Flags));
            }
            ThrowIfFailed(d3d.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)tex.texture.ReleaseAndGetAddressOf()));
        }
        if (render_target) {
            ThrowIfFailed(d3d.device->CreateRenderTargetView(tex.texture.Get(), nullptr, fb.render_target_view.ReleaseAndGetAddressOf()));
        }

        tex.width = width;
        tex.height = height;
    }

    if (has_depth_buffer && (diff || !fb.has_depth_buffer || (fb.depth_stencil_srv.Get() != nullptr) != can_extract_depth)) {
        fb.depth_stencil_srv.Reset();
        create_depth_stencil_objects(width, height, msaa_level, fb.depth_stencil_view.ReleaseAndGetAddressOf(), can_extract_depth ? fb.depth_stencil_srv.GetAddressOf() : nullptr);
    }
    if (!has_depth_buffer) {
        fb.depth_stencil_view.Reset();
        fb.depth_stencil_srv.Reset();
    }

    fb.has_depth_buffer = has_depth_buffer;
    fb.msaa_level = msaa_level;
}

void gfx_d3d11_start_draw_to_framebuffer(int fb_id, float noise_scale) {
    Framebuffer& fb = d3d.framebuffers[fb_id];
    d3d.render_target_height = d3d.textures[fb.texture_id].height;

    d3d.context->OMSetRenderTargets(1, fb.render_target_view.GetAddressOf(), fb.has_depth_buffer ? fb.depth_stencil_view.Get() : nullptr);

    d3d.current_framebuffer = fb_id;

    if (noise_scale != 0.0f) {
        d3d.per_frame_cb_data.noise_scale = 1.0f / noise_scale;
    }

    D3D11_MAPPED_SUBRESOURCE ms;
    ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
    d3d.context->Map(d3d.per_frame_cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    memcpy(ms.pData, &d3d.per_frame_cb_data, sizeof(PerFrameCB));
    d3d.context->Unmap(d3d.per_frame_cb.Get(), 0);
}

void gfx_d3d11_clear_framebuffer(void) {
    Framebuffer& fb = d3d.framebuffers[d3d.current_framebuffer];
    const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    d3d.context->ClearRenderTargetView(fb.render_target_view.Get(), clearColor);
    if (fb.has_depth_buffer) {
        d3d.context->ClearDepthStencilView(fb.depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
}

void gfx_d3d11_resolve_msaa_color_buffer(int fb_id_target, int fb_id_source) {
    Framebuffer& fb_dst = d3d.framebuffers[fb_id_target];
    Framebuffer& fb_src = d3d.framebuffers[fb_id_source];

    d3d.context->ResolveSubresource(d3d.textures[fb_dst.texture_id].texture.Get(), 0, d3d.textures[fb_src.texture_id].texture.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void *gfx_d3d11_get_framebuffer_texture_id(int fb_id) {
    return (void *)d3d.textures[d3d.framebuffers[fb_id].texture_id].resource_view.Get();
}

void gfx_d3d11_select_texture_fb(int fbID) {
    int tile = 0;
    gfx_d3d11_select_texture(tile, d3d.framebuffers[fbID].texture_id);
}

void gfx_d3d11_set_texture_filter(FilteringMode mode) {
    d3d.current_filter_mode = mode;
    gfx_texture_cache_clear();
}

FilteringMode gfx_d3d11_get_texture_filter(void) {
    return d3d.current_filter_mode;
}

std::map<std::pair<float, float>, uint16_t> gfx_d3d11_get_pixel_depth(int fb_id, const std::set<std::pair<float, float>>& coordinates) {
    Framebuffer& fb = d3d.framebuffers[fb_id];
    TextureData& td = d3d.textures[fb.texture_id];

    if (coordinates.size() > d3d.coord_buffer_size) {
        d3d.coord_buffer.Reset();
        d3d.coord_buffer_srv.Reset();
        d3d.depth_value_output_buffer.Reset();
        d3d.depth_value_output_uav.Reset();
        d3d.depth_value_output_buffer_copy.Reset();

        D3D11_BUFFER_DESC coord_buf_desc;
        coord_buf_desc.Usage = D3D11_USAGE_DYNAMIC;
        coord_buf_desc.ByteWidth = sizeof(Coord) * coordinates.size();
        coord_buf_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        coord_buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        coord_buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        coord_buf_desc.StructureByteStride = sizeof(Coord);

        ThrowIfFailed(d3d.device->CreateBuffer(&coord_buf_desc, nullptr, d3d.coord_buffer.GetAddressOf()));

        D3D11_SHADER_RESOURCE_VIEW_DESC coord_buf_srv_desc;
        coord_buf_srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        coord_buf_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        coord_buf_srv_desc.Buffer.FirstElement = 0;
        coord_buf_srv_desc.Buffer.NumElements = coordinates.size();

        ThrowIfFailed(d3d.device->CreateShaderResourceView(d3d.coord_buffer.Get(), &coord_buf_srv_desc, d3d.coord_buffer_srv.GetAddressOf()));

        D3D11_BUFFER_DESC output_buffer_desc;
        output_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        output_buffer_desc.ByteWidth = sizeof(float) * coordinates.size();
        output_buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        output_buffer_desc.CPUAccessFlags = 0;
        output_buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        output_buffer_desc.StructureByteStride = sizeof(float);
        ThrowIfFailed(d3d.device->CreateBuffer(&output_buffer_desc, nullptr, d3d.depth_value_output_buffer.GetAddressOf()));

        D3D11_UNORDERED_ACCESS_VIEW_DESC output_buffer_uav_desc;
        output_buffer_uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        output_buffer_uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        output_buffer_uav_desc.Buffer.FirstElement = 0;
        output_buffer_uav_desc.Buffer.NumElements = coordinates.size();
        output_buffer_uav_desc.Buffer.Flags = 0;
        ThrowIfFailed(d3d.device->CreateUnorderedAccessView(d3d.depth_value_output_buffer.Get(), &output_buffer_uav_desc, d3d.depth_value_output_uav.GetAddressOf()));

        output_buffer_desc.Usage = D3D11_USAGE_STAGING;
        output_buffer_desc.BindFlags = 0;
        output_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        ThrowIfFailed(d3d.device->CreateBuffer(&output_buffer_desc, nullptr, d3d.depth_value_output_buffer_copy.GetAddressOf()));

        d3d.coord_buffer_size = coordinates.size();
    }

    D3D11_MAPPED_SUBRESOURCE ms;

    if (fb.msaa_level > 1 && d3d.compute_shader_msaa.Get() == nullptr) {
        ThrowIfFailed(d3d.device->CreateComputeShader(d3d.compute_shader_msaa_blob->GetBufferPointer(), d3d.compute_shader_msaa_blob->GetBufferSize(), nullptr, d3d.compute_shader_msaa.GetAddressOf()));
    }

    // ImGui overwrites these values, so we cannot set them once at init
    d3d.context->CSSetShader(fb.msaa_level > 1 ? d3d.compute_shader_msaa.Get() : d3d.compute_shader.Get(), nullptr, 0);
    d3d.context->CSSetUnorderedAccessViews(0, 1, d3d.depth_value_output_uav.GetAddressOf(), nullptr);

    ThrowIfFailed(d3d.context->Map(d3d.coord_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms));
    Coord *coord_cb = (Coord *)ms.pData;
    {
        size_t i = 0;
        for (const auto& coord : coordinates) {
            coord_cb[i].x = coord.first;
            // We invert y because the gfx_pc assumes OpenGL coordinates (bottom-left corner is origin), while DX's origin is top-left corner
            coord_cb[i].y = td.height - 1 - coord.second;
            ++i;
        }
    }
    d3d.context->Unmap(d3d.coord_buffer.Get(), 0);

    // The depth stencil texture can only have one mapping at a time, so unbind from the OM
    ID3D11RenderTargetView* null_arr1[1] = { nullptr };
    d3d.context->OMSetRenderTargets(1, null_arr1, nullptr);

    ID3D11ShaderResourceView *srvs[] = { fb.depth_stencil_srv.Get(), d3d.coord_buffer_srv.Get() };
    d3d.context->CSSetShaderResources(0, 2, srvs);

    d3d.context->Dispatch(coordinates.size(), 1, 1);

    d3d.context->CopyResource(d3d.depth_value_output_buffer_copy.Get(), d3d.depth_value_output_buffer.Get());
    ThrowIfFailed(d3d.context->Map(d3d.depth_value_output_buffer_copy.Get(), 0, D3D11_MAP_READ, 0, &ms));
    std::map<std::pair<float, float>, uint16_t> res;
    {
        size_t i = 0;
        for (const auto& coord : coordinates) {
            res.emplace(coord, ((float *)ms.pData)[i++] * 65532.0f);
        }
    }
    d3d.context->Unmap(d3d.depth_value_output_buffer_copy.Get(), 0);

    ID3D11ShaderResourceView *null_arr[2] = { nullptr, nullptr };
    d3d.context->CSSetShaderResources(0, 2, null_arr);

    return res;
}

} // namespace

ImTextureID gfx_d3d11_get_texture_by_id(int id) {
    return d3d.textures[id].resource_view.Get();
}

struct GfxRenderingAPI gfx_direct3d11_api = {
    gfx_d3d11_get_clip_parameters,
    gfx_d3d11_unload_shader,
    gfx_d3d11_load_shader,
    gfx_d3d11_create_and_load_new_shader,
    gfx_d3d11_lookup_shader,
    gfx_d3d11_shader_get_info,
    gfx_d3d11_new_texture,
    gfx_d3d11_select_texture,
    gfx_d3d11_upload_texture,
    gfx_d3d11_set_sampler_parameters,
    gfx_d3d11_set_depth_test_and_mask,
    gfx_d3d11_set_zmode_decal,
    gfx_d3d11_set_viewport,
    gfx_d3d11_set_scissor,
    gfx_d3d11_set_use_alpha,
    gfx_d3d11_draw_triangles,
    gfx_d3d11_init,
    gfx_d3d11_on_resize,
    gfx_d3d11_start_frame,
    gfx_d3d11_end_frame,
    gfx_d3d11_finish_render,
    gfx_d3d11_create_framebuffer,
    gfx_d3d11_update_framebuffer_parameters,
    gfx_d3d11_start_draw_to_framebuffer,
    gfx_d3d11_clear_framebuffer,
    gfx_d3d11_resolve_msaa_color_buffer,
    gfx_d3d11_get_pixel_depth,
    gfx_d3d11_get_framebuffer_texture_id,
    gfx_d3d11_select_texture_fb,
    gfx_d3d11_delete_texture,
    gfx_d3d11_set_texture_filter,
    gfx_d3d11_get_texture_filter
};

#endif
