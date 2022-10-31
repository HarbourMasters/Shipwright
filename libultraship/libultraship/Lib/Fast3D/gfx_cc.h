#ifndef GFX_CC_H
#define GFX_CC_H

#include <stdint.h>
#include <stdbool.h>

/*enum {
    CC_0,
    CC_TEXEL0,
    CC_TEXEL1,
    CC_PRIM,
    CC_SHADE,
    CC_ENV,
    CC_TEXEL0A,
    CC_LOD
};*/

enum {
    SHADER_0,
    SHADER_INPUT_1,
    SHADER_INPUT_2,
    SHADER_INPUT_3,
    SHADER_INPUT_4,
    SHADER_INPUT_5,
    SHADER_INPUT_6,
    SHADER_INPUT_7,
    SHADER_TEXEL0,
    SHADER_TEXEL0A,
    SHADER_TEXEL1,
    SHADER_TEXEL1A,
    SHADER_1,
    SHADER_COMBINED,
    SHADER_NOISE
};

#define SHADER_OPT_ALPHA (1 << 0)
#define SHADER_OPT_FOG (1 << 1)
#define SHADER_OPT_TEXTURE_EDGE (1 << 2)
#define SHADER_OPT_NOISE (1 << 3)
#define SHADER_OPT_2CYC (1 << 4)
#define SHADER_OPT_ALPHA_THRESHOLD (1 << 5)
#define SHADER_OPT_INVISIBLE (1 << 6)
#define SHADER_OPT_GRAYSCALE (1 << 7)
#define SHADER_OPT_TEXEL0_CLAMP_S (1 << 8)
#define SHADER_OPT_TEXEL0_CLAMP_T (1 << 9)
#define SHADER_OPT_TEXEL1_CLAMP_S (1 << 10)
#define SHADER_OPT_TEXEL1_CLAMP_T (1 << 11)
#define CC_SHADER_OPT_POS 56

struct CCFeatures {
    uint8_t c[2][2][4];
    bool opt_alpha;
    bool opt_fog;
    bool opt_texture_edge;
    bool opt_noise;
    bool opt_2cyc;
    bool opt_alpha_threshold;
    bool opt_invisible;
    bool opt_grayscale;
    bool used_textures[2];
    bool clamp[2][2];
    int num_inputs;
    bool do_single[2][2];
    bool do_multiply[2][2];
    bool do_mix[2][2];
    bool color_alpha_same[2];
};

void gfx_cc_get_features(uint64_t shader_id0, uint32_t shader_id1, struct CCFeatures *cc_features);

#endif
