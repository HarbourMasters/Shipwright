#include "gfx_cc.h"

void gfx_cc_get_features(uint64_t shader_id0, uint32_t shader_id1, struct CCFeatures *cc_features) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                cc_features->c[i][j][k] = (shader_id0 >> (i * 32 + j * 16 + k * 4)) & 0xf;
            }
        }
    }

    cc_features->opt_alpha = (shader_id1 & SHADER_OPT_ALPHA) != 0;
    cc_features->opt_fog = (shader_id1 & SHADER_OPT_FOG) != 0;
    cc_features->opt_texture_edge = (shader_id1 & SHADER_OPT_TEXTURE_EDGE) != 0;
    cc_features->opt_noise = (shader_id1 & SHADER_OPT_NOISE) != 0;
    cc_features->opt_2cyc = (shader_id1 & SHADER_OPT_2CYC) != 0;
    cc_features->opt_alpha_threshold = (shader_id1 & SHADER_OPT_ALPHA_THRESHOLD) != 0;
    cc_features->opt_invisible = (shader_id1 & SHADER_OPT_INVISIBLE) != 0;
    cc_features->opt_grayscale = (shader_id1 & SHADER_OPT_GRAYSCALE) != 0;

    cc_features->clamp[0][0] = (shader_id1 & SHADER_OPT_TEXEL0_CLAMP_S);
    cc_features->clamp[0][1] = (shader_id1 & SHADER_OPT_TEXEL0_CLAMP_T);
    cc_features->clamp[1][0] = (shader_id1 & SHADER_OPT_TEXEL1_CLAMP_S);
    cc_features->clamp[1][1] = (shader_id1 & SHADER_OPT_TEXEL1_CLAMP_T);

    cc_features->used_textures[0] = false;
    cc_features->used_textures[1] = false;
    cc_features->num_inputs = 0;

    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                if (cc_features->c[c][i][j] >= SHADER_INPUT_1 && cc_features->c[c][i][j] <= SHADER_INPUT_7) {
                    if (cc_features->c[c][i][j] > cc_features->num_inputs) {
                        cc_features->num_inputs = cc_features->c[c][i][j];
                    }
                }
                if (cc_features->c[c][i][j] == SHADER_TEXEL0 || cc_features->c[c][i][j] == SHADER_TEXEL0A) {
                    cc_features->used_textures[0] = true;
                }
                if (cc_features->c[c][i][j] == SHADER_TEXEL1 || cc_features->c[c][i][j] == SHADER_TEXEL1A) {
                    cc_features->used_textures[1] = true;
                }
            }
        }
    }

    for (int c = 0; c < 2; c++) {
        cc_features->do_single[c][0] = cc_features->c[c][0][2] == SHADER_0;
        cc_features->do_single[c][1] = cc_features->c[c][1][2] == SHADER_0;
        cc_features->do_multiply[c][0] = cc_features->c[c][0][1] == SHADER_0 && cc_features->c[c][0][3] == SHADER_0;
        cc_features->do_multiply[c][1] = cc_features->c[c][1][1] == SHADER_0 && cc_features->c[c][1][3] == SHADER_0;
        cc_features->do_mix[c][0] = cc_features->c[c][0][1] == cc_features->c[c][0][3];
        cc_features->do_mix[c][1] = cc_features->c[c][1][1] == cc_features->c[c][1][3];
        cc_features->color_alpha_same[c] = ((shader_id0 >> c * 32) & 0xffff) == ((shader_id0 >> (c * 32 + 16)) & 0xffff);
    }
}
