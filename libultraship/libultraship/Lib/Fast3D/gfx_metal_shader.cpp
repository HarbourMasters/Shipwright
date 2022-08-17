//
//  gfx_metal_shader.cpp
//  libultraship
//
//  Created by David Chavez on 16.08.22.
//

#ifdef ENABLE_METAL

#include <Metal/Metal.hpp>

#include "gfx_metal_shader.hpp"

// MARK: - String Helpers

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

static void append_formula(char *buf, size_t *len, const uint8_t c[2][4], bool do_single, bool do_multiply, bool do_mix, bool with_alpha, bool only_alpha, bool opt_alpha) {
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

// MARK: - Public Methods

MTL::VertexDescriptor* gfx_metal_build_shader(char buf[4096], size_t& num_floats, const CCFeatures& cc_features, bool three_point_filtering) {

    size_t len = 0;
    int vertex_index = 0;

    MTL::VertexDescriptor* vertex_descriptor = MTL::VertexDescriptor::vertexDescriptor();

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
    len += sprintf(buf + len, "    float4 position [[attribute(%d)]];\n", vertex_index);
    vertex_descriptor->attributes()->object(vertex_index)->setFormat(MTL::VertexFormatFloat4);
    vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
    vertex_descriptor->attributes()->object(vertex_index++)->setOffset(0);
    num_floats += 4;

    for (int i = 0; i < 2; i++) {
        if (cc_features.used_textures[i]) {
            len += sprintf(buf + len, "    float2 texCoord%d [[attribute(%d)]];\n", i, vertex_index);
            vertex_descriptor->attributes()->object(vertex_index)->setFormat(MTL::VertexFormatFloat2);
            vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
            vertex_descriptor->attributes()->object(vertex_index++)->setOffset(num_floats * sizeof(float));
            num_floats += 2;
            for (int j = 0; j < 2; j++) {
                if (cc_features.clamp[i][j]) {
                    len += sprintf(buf + len, "    float texClamp%s%d [[attribute(%d)]];\n", j == 0 ? "S" : "T", i, vertex_index);
                    vertex_descriptor->attributes()->object(vertex_index)->setFormat(MTL::VertexFormatFloat);
                    vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
                    vertex_descriptor->attributes()->object(vertex_index++)->setOffset(num_floats * sizeof(float));
                    num_floats += 1;
                }
            }
        }
    }
    if (cc_features.opt_fog) {
        len += sprintf(buf + len, "    float4 fog [[attribute(%d)]];\n", vertex_index);
        vertex_descriptor->attributes()->object(vertex_index)->setFormat(MTL::VertexFormatFloat4);
        vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
        vertex_descriptor->attributes()->object(vertex_index++)->setOffset(num_floats * sizeof(float));
        num_floats += 4;
    }
    if (cc_features.opt_grayscale) {
        len += sprintf(buf + len, "    float4 grayscale [[attribute(%d)]];\n", vertex_index);
        vertex_descriptor->attributes()->object(vertex_index)->setFormat(MTL::VertexFormatFloat4);
        vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
        vertex_descriptor->attributes()->object(vertex_index++)->setOffset(num_floats * sizeof(float));
        num_floats += 4;
    }
    for (int i = 0; i < cc_features.num_inputs; i++) {
        len += sprintf(buf + len, "    float%d input%d [[attribute(%d)]];\n",  cc_features.opt_alpha ? 4 : 3, i + 1, vertex_index);
        vertex_descriptor->attributes()->object(vertex_index)->setFormat(cc_features.opt_alpha ? MTL::VertexFormatFloat4 : MTL::VertexFormatFloat3);
        vertex_descriptor->attributes()->object(vertex_index)->setBufferIndex(0);
        vertex_descriptor->attributes()->object(vertex_index++)->setOffset(num_floats * sizeof(float));
        num_floats += cc_features.opt_alpha ? 4 : 3;
    }
    append_line(buf, &len, "};");
    // end vertex struct

    // vertex output struct
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
    // end vertex output struct

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
    if (three_point_filtering) {
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

    append_line(buf, &len, "float random(float3 value) {");
    append_line(buf, &len, "    float random = dot(sin(value), float3(12.9898, 78.233, 37.719));");
    append_line(buf, &len, "    return fract(sin(random) * 143758.5453);");
    append_line(buf, &len, "}");

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
        append_line(buf, &len, "    texel.w *= floor(fast::clamp(random(float3(floor(in.position.xy * frameUniforms.noiseScale), float(frameUniforms.frameCount))) + texel.w, 0.0, 1.0));");
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

    vertex_descriptor->layouts()->object(0)->setStride(num_floats * sizeof(float));
    vertex_descriptor->layouts()->object(0)->setStepFunction(MTL::VertexStepFunctionPerVertex);

    return vertex_descriptor;
}

#endif
