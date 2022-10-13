#if defined(ENABLE_DX11) || defined(ENABLE_DX12)

#include <cstdio>

#include "gfx_direct3d_common.h"
#include "gfx_cc.h"

static void append_str(char *buf, size_t *len, const char *str) {
    while (*str != '\0') buf[(*len)++] = *str++;
}

static void append_line(char *buf, size_t *len, const char *str) {
    while (*str != '\0') buf[(*len)++] = *str++;
    buf[(*len)++] = '\r';
    buf[(*len)++] = '\n';
}

#define RAND_NOISE "((random(float3(floor(screenSpace.xy * noise_scale), noise_frame)) + 1.0) / 2.0)"

static const char *shader_item_to_str(uint32_t item, bool with_alpha, bool only_alpha, bool inputs_have_alpha, bool hint_single_element) {
    if (!only_alpha) {
        switch (item) {
            default:
            case SHADER_0:
                return with_alpha ? "float4(0.0, 0.0, 0.0, 0.0)" : "float3(0.0, 0.0, 0.0)";
            case SHADER_1:
                return with_alpha ? "float4(1.0, 1.0, 1.0, 1.0)" : "float3(1.0, 1.0, 1.0)";
            case SHADER_INPUT_1:
                return with_alpha || !inputs_have_alpha ? "input.input1" : "input.input1.rgb";
            case SHADER_INPUT_2:
                return with_alpha || !inputs_have_alpha ? "input.input2" : "input.input2.rgb";
            case SHADER_INPUT_3:
                return with_alpha || !inputs_have_alpha ? "input.input3" : "input.input3.rgb";
            case SHADER_INPUT_4:
                return with_alpha || !inputs_have_alpha ? "input.input4" : "input.input4.rgb";
            case SHADER_TEXEL0:
                return with_alpha ? "texVal0" : "texVal0.rgb";
            case SHADER_TEXEL0A:
                return hint_single_element ? "texVal0.a" : (with_alpha ? "float4(texVal0.a, texVal0.a, texVal0.a, texVal0.a)" : "float3(texVal0.a, texVal0.a, texVal0.a)");
            case SHADER_TEXEL1A:
                return hint_single_element ? "texVal1.a" : (with_alpha ? "float4(texVal1.a, texVal1.a, texVal1.a, texVal1.a)" : "float3(texVal1.a, texVal1.a, texVal1.a)");
            case SHADER_TEXEL1:
                return with_alpha ? "texVal1" : "texVal1.rgb";
            case SHADER_COMBINED:
                return with_alpha ? "texel" : "texel.rgb";
            case SHADER_NOISE:
                return with_alpha ? "float4(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")" : "float3(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")";
        }
    } else {
        switch (item) {
            default:
            case SHADER_0:
                return "0.0";
            case SHADER_1:
                return "1.0";
            case SHADER_INPUT_1:
                return "input.input1.a";
            case SHADER_INPUT_2:
                return "input.input2.a";
            case SHADER_INPUT_3:
                return "input.input3.a";
            case SHADER_INPUT_4:
                return "input.input4.a";
            case SHADER_TEXEL0:
                return "texVal0.a";
            case SHADER_TEXEL0A:
                return "texVal0.a";
            case SHADER_TEXEL1A:
                return "texVal1.a";
            case SHADER_TEXEL1:
                return "texVal1.a";
            case SHADER_COMBINED:
                return "texel.a";
            case SHADER_NOISE:
                return RAND_NOISE;
        }
    }
}

#undef RAND_NOISE

static void append_formula(char *buf, size_t *len, const uint8_t c[2][4], bool do_single, bool do_multiply, bool do_mix, bool with_alpha, bool only_alpha, bool opt_alpha) {
    if (do_single) {
        append_str(buf, len, shader_item_to_str(c[only_alpha][3], with_alpha, only_alpha, opt_alpha, false));
    } else if (do_multiply) {
        append_str(buf, len, shader_item_to_str(c[only_alpha][0], with_alpha, only_alpha, opt_alpha, false));
        append_str(buf, len, " * ");
        append_str(buf, len, shader_item_to_str(c[only_alpha][2], with_alpha, only_alpha, opt_alpha, true));
    } else if (do_mix) {
        append_str(buf, len, "lerp(");
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

void gfx_direct3d_common_build_shader(char buf[4096], size_t& len, size_t& num_floats, const CCFeatures& cc_features, bool include_root_signature, bool three_point_filtering) {
    len = 0;
    num_floats = 4;

    // Pixel shader input struct

    if (include_root_signature) {
        append_str(buf, &len, "#define RS \"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | DENY_VERTEX_SHADER_ROOT_ACCESS)");
        append_str(buf, &len, ",CBV(b0, visibility = SHADER_VISIBILITY_PIXEL)");
        if (cc_features.used_textures[0]) {
            append_str(buf, &len, ",DescriptorTable(SRV(t0), visibility = SHADER_VISIBILITY_PIXEL)");
            append_str(buf, &len, ",DescriptorTable(Sampler(s0), visibility = SHADER_VISIBILITY_PIXEL)");
        }
        if (cc_features.used_textures[1]) {
            append_str(buf, &len, ",DescriptorTable(SRV(t1), visibility = SHADER_VISIBILITY_PIXEL)");
            append_str(buf, &len, ",DescriptorTable(Sampler(s1), visibility = SHADER_VISIBILITY_PIXEL)");
        }
        append_line(buf, &len, "\"");
    }

    append_line(buf, &len, "struct PSInput {");
    append_line(buf, &len, "    float4 position : SV_POSITION;");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            len += sprintf(buf + len, "    float2 uv%d : TEXCOORD%d;\r\n", i, i);
            num_floats += 2;
            for (int j = 0; j < 2; j++) {
                if (cc_features.clamp[i][j]) {
                    len += sprintf(buf + len, "    float texClamp%s%d : TEXCLAMP%s%d;\r\n", j == 0 ? "S" : "T", i, j == 0 ? "S" : "T", i);
                    num_floats += 1;
                }
            }
        }
    }
    if (cc_features.opt_fog) {
        append_line(buf, &len, "    float4 fog : FOG;");
        num_floats += 4;
    }
    if (cc_features.opt_grayscale) {
        append_line(buf, &len, "    float4 grayscale : GRAYSCALE;");
        num_floats += 4;
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        len += sprintf(buf + len, "    float%d input%d : INPUT%d;\r\n", cc_features.opt_alpha ? 4 : 3, i + 1, i);
        num_floats += cc_features.opt_alpha ? 4 : 3;
    }
    append_line(buf, &len, "};");

    // Textures and samplers

    if (cc_features.used_textures[0]) {
        append_line(buf, &len, "Texture2D g_texture0 : register(t0);");
        append_line(buf, &len, "SamplerState g_sampler0 : register(s0);");
    }
    if (cc_features.used_textures[1]) {
        append_line(buf, &len, "Texture2D g_texture1 : register(t1);");
        append_line(buf, &len, "SamplerState g_sampler1 : register(s1);");
    }

    // Constant buffer and random function

    append_line(buf, &len, "cbuffer PerFrameCB : register(b0) {");
    append_line(buf, &len, "    uint noise_frame;");
    append_line(buf, &len, "    float noise_scale;");
    append_line(buf, &len, "}");

    append_line(buf, &len, "float random(in float3 value) {");
    append_line(buf, &len, "    float random = dot(value, float3(12.9898, 78.233, 37.719));");
    append_line(buf, &len, "    return frac(sin(random) * 143758.5453);");
    append_line(buf, &len, "}");

    // 3 point texture filtering
    // Original author: ArthurCarvalho
    // Based on GLSL implementation by twinaphex, mupen64plus-libretro project.

    if (three_point_filtering && (cc_features.used_textures[0] || cc_features.used_textures[1])) {
        append_line(buf, &len, "cbuffer PerDrawCB : register(b1) {");
        append_line(buf, &len, "    struct {");
        append_line(buf, &len, "        uint width;");
        append_line(buf, &len, "        uint height;");
        append_line(buf, &len, "        bool linear_filtering;");
        append_line(buf, &len, "    } textures[2];");
        append_line(buf, &len, "}");
        append_line(buf, &len, "#define TEX_OFFSET(tex, tSampler, texCoord, off, texSize) tex.Sample(tSampler, texCoord - off / texSize)");
        append_line(buf, &len, "float4 tex2D3PointFilter(in Texture2D tex, in SamplerState tSampler, in float2 texCoord, in float2 texSize) {");
        append_line(buf, &len, "    float2 offset = frac(texCoord * texSize - float2(0.5, 0.5));");
        append_line(buf, &len, "    offset -= step(1.0, offset.x + offset.y);");
        append_line(buf, &len, "    float4 c0 = TEX_OFFSET(tex, tSampler, texCoord, offset, texSize);");
        append_line(buf, &len, "    float4 c1 = TEX_OFFSET(tex, tSampler, texCoord, float2(offset.x - sign(offset.x), offset.y), texSize);");
        append_line(buf, &len, "    float4 c2 = TEX_OFFSET(tex, tSampler, texCoord, float2(offset.x, offset.y - sign(offset.y)), texSize);");
        append_line(buf, &len, "    return c0 + abs(offset.x)*(c1-c0) + abs(offset.y)*(c2-c0);");
        append_line(buf, &len, "}");
    }

    // Vertex shader

    append_str(buf, &len, "PSInput VSMain(float4 position : POSITION");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            len += sprintf(buf + len, ", float2 uv%d : TEXCOORD%d", i, i);
            for (int j = 0; j < 2; j++) {
                if (cc_features.clamp[i][j]) {
                    len += sprintf(buf + len, ", float texClamp%s%d : TEXCLAMP%s%d", j == 0 ? "S" : "T", i, j == 0 ? "S" : "T", i);
                }
            }
        }
    }
    if (cc_features.opt_fog) {
        append_str(buf, &len, ", float4 fog : FOG");
    }
    if (cc_features.opt_grayscale) {
        append_str(buf, &len, ", float4 grayscale : GRAYSCALE");
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        len += sprintf(buf + len, ", float%d input%d : INPUT%d", cc_features.opt_alpha ? 4 : 3, i + 1, i);
    }
    append_line(buf, &len, ") {");
    append_line(buf, &len, "    PSInput result;");
    append_line(buf, &len, "    result.position = position;");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            len += sprintf(buf + len, "    result.uv%d = uv%d;\r\n", i, i);
            for (int j = 0; j < 2; j++) {
                if (cc_features.clamp[i][j]) {
                    len += sprintf(buf + len, "    result.texClamp%s%d = texClamp%s%d;\r\n", j == 0 ? "S" : "T", i, j == 0 ? "S" : "T", i);
                }
            }
        }
    }

    if (cc_features.opt_fog) {
        append_line(buf, &len, "    result.fog = fog;");
    }
    if (cc_features.opt_grayscale) {
        append_line(buf, &len, "    result.grayscale = grayscale;");
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        len += sprintf(buf + len, "    result.input%d = input%d;\r\n", i + 1, i + 1);
    }
    append_line(buf, &len, "    return result;");
    append_line(buf, &len, "}");

    // Pixel shader
    if (include_root_signature) {
        append_line(buf, &len, "[RootSignature(RS)]");
    }
    append_line(buf, &len, "float4 PSMain(PSInput input, float4 screenSpace : SV_Position) : SV_TARGET {");
    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            len += sprintf(buf + len, "    float2 tc%d = input.uv%d;\r\n", i, i);
            bool s = cc_features.clamp[i][0], t = cc_features.clamp[i][1];
            if (!s && !t) {
            } else {
                len += sprintf(buf + len, "    int2 texSize%d;\r\n", i);
                len += sprintf(buf + len, "    g_texture%d.GetDimensions(texSize%d.x, texSize%d.y);\r\n", i, i, i);
                if (s && t) {
                    len += sprintf(buf + len, "    tc%d = clamp(tc%d, 0.5 / texSize%d, float2(input.texClampS%d, input.texClampT%d));\r\n", i, i, i, i, i);
                } else if (s) {
                    len += sprintf(buf + len, "    tc%d = float2(clamp(tc%d.x, 0.5 / texSize%d.x, input.texClampS%d), tc%d.y);\n", i, i, i, i, i);
                } else {
                    len += sprintf(buf + len, "    tc%d = float2(tc%d.x, clamp(tc%d.y, 0.5 / texSize%d.y, input.texClampT%d));\n", i, i, i, i, i);
                }
            }
            if (three_point_filtering) {
                len += sprintf(buf + len, "    float4 texVal%d;\r\n", i);
                len += sprintf(buf + len, "    if (textures[%d].linear_filtering)\r\n", i);
                len += sprintf(buf + len, "        texVal%d = tex2D3PointFilter(g_texture%d, g_sampler%d, tc%d, float2(textures[%d].width, textures[%d].height));\r\n", i, i, i, i, i, i);
                len += sprintf(buf + len, "    else\r\n");
                len += sprintf(buf + len, "        texVal%d = g_texture%d.Sample(g_sampler%d, tc%d);\r\n", i, i, i, i);
            } else {
                len += sprintf(buf + len, "    float4 texVal%d = g_texture%d.Sample(g_sampler%d, tc%d);\r\n", i, i, i, i);
            }
        }
    }

    append_str(buf, &len, cc_features.opt_alpha ? "    float4 texel;" : "    float3 texel;");
    for (int c = 0; c < (cc_features.opt_2cyc ? 2 : 1); c++) {
        append_str(buf, &len, "texel = ");
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

    if (cc_features.opt_texture_edge && cc_features.opt_alpha) {
        append_line(buf, &len, "    if (texel.a > 0.19) texel.a = 1.0; else discard;");
    }
    // TODO discard if alpha is 0?
    if (cc_features.opt_fog) {
        if (cc_features.opt_alpha) {
            append_line(buf, &len, "    texel = float4(lerp(texel.rgb, input.fog.rgb, input.fog.a), texel.a);");
        } else {
            append_line(buf, &len, "    texel = lerp(texel, input.fog.rgb, input.fog.a);");
        }
    }

    if (cc_features.opt_grayscale) {
        append_line(buf, &len, "float intensity = (texel.r + texel.g + texel.b) / 3.0;");
        append_line(buf, &len, "float3 new_texel = input.grayscale.rgb * intensity;");
        append_line(buf, &len, "texel.rgb = lerp(texel.rgb, new_texel, input.grayscale.a);");
    }

    if (cc_features.opt_alpha && cc_features.opt_noise) {
        append_line(buf, &len, "    float2 coords = screenSpace.xy * noise_scale;");
        append_line(buf, &len, "    texel.a *= round(saturate(random(float3(floor(coords), noise_frame)) + texel.a - 0.5));");
    }

    if (cc_features.opt_alpha) {
        if (cc_features.opt_alpha_threshold) {
            append_line(buf, &len, "    if (texel.a < 8.0 / 256.0) discard;");
        }
        if (cc_features.opt_invisible) {
            append_line(buf, &len, "    texel.a = 0.0;");
        }
        append_line(buf, &len, "    return texel;");
    } else {
        append_line(buf, &len, "    return float4(texel, 1.0);");
    }
    append_line(buf, &len, "}");
}

#endif
