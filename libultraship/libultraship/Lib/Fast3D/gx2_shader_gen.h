#pragma once

#include "gfx_cc.h"
#include <gx2/shaders.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ShaderGroup {
    GX2VertexShader vertexShader;
    GX2PixelShader pixelShader;
    GX2FetchShader fetchShader;

    uint32_t stride;

    uint32_t numAttributes;
    GX2AttribStream attributes[13];
};

int gx2GenerateShaderGroup(struct ShaderGroup *group, struct CCFeatures *cc_features);

void gx2FreeShaderGroup(struct ShaderGroup *group);

#ifdef __cplusplus
}
#endif
