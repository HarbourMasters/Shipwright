#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/frame_interpolation.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
#include "functions.h"

extern PlayState* gPlayState;
uint64_t osGetTime(void);
}

// SoH [Enhancement] Majora's Mask style star field, ported from the 2S2H "some_love_for_stars" branch.
// OOT has no stars of its own, so they are drawn as geometry over the skybox, between the sky and the sun and moon.

static constexpr const char* CVAR_STAR_FIELD = CVAR_ENHANCEMENT("MajoraStarField");
static constexpr const char* CVAR_STAR_TELESCOPE = CVAR_ENHANCEMENT("MajoraStarTelescope");
static constexpr const char* CVAR_STAR_SIZE = CVAR_ENHANCEMENT("MajoraStarSize");
static constexpr const char* CVAR_STAR_INTENSITY = CVAR_ENHANCEMENT("MajoraStarIntensity");
static constexpr const char* CVAR_STAR_TWINKLE = CVAR_ENHANCEMENT("MajoraStarTwinkle");
static constexpr const char* CVAR_STAR_TWINKLE_FREQUENCY = CVAR_ENHANCEMENT("MajoraStarTwinkleFrequency");
static constexpr const char* CVAR_STAR_COUNT = CVAR_ENHANCEMENT("MajoraStarCount");

static constexpr s32 SKYBOX_STAR_SUNRISE_INDEX = 0; // sSkyboxTable entries picked by D_8011FC1C
static constexpr s32 SKYBOX_STAR_SUNSET_INDEX = 2;
static constexpr s32 SKYBOX_STAR_NIGHT_INDEX = 3;
static constexpr u16 SKYBOX_STAR_BASE_COUNT = 100; // before the "Star Count" multiplier
static constexpr f32 SKYBOX_STAR_COUNT_MULTIPLIER_MIN = 1.0f;
static constexpr f32 SKYBOX_STAR_COUNT_MULTIPLIER_MAX = 5.0f;
static constexpr s32 SKYBOX_STAR_MIN_FREE_BYTES = 8192; // polyOpa headroom kept while adding extra stars

// The telescope shape: a round core with four spikes, carried by the vertex alphas of a static array.
static constexpr s32 SKYBOX_STAR_CORE_SEGMENTS = 12;
static constexpr f32 SKYBOX_STAR_CORE_CENTER_ALPHA = 235.0f;
static constexpr f32 SKYBOX_STAR_CORE_INNER_RADIUS = 0.16f;
static constexpr f32 SKYBOX_STAR_CORE_INNER_ALPHA = 105.0f;
static constexpr f32 SKYBOX_STAR_CORE_RADIUS = 0.48f;
static constexpr f32 SKYBOX_STAR_ARM_MID_T = 0.55f; // ring position along the spike
static constexpr f32 SKYBOX_STAR_ARM_LENGTH_H = 0.55f;
static constexpr f32 SKYBOX_STAR_ARM_LENGTH_V = 0.47f;
static constexpr f32 SKYBOX_STAR_ARM_WIDTH_BASE = 0.060f;
static constexpr f32 SKYBOX_STAR_ARM_WIDTH_MID = 0.038f;
static constexpr f32 SKYBOX_STAR_ARM_WIDTH_TIP = 0.020f;
static constexpr f32 SKYBOX_STAR_ARM_ALPHA_H = 130.0f;
static constexpr f32 SKYBOX_STAR_ARM_ALPHA_V = 110.0f;
static constexpr f32 SKYBOX_STAR_ARM_MID_ALPHA = 0.45f; // ring alpha, relative to the base
static constexpr f32 SKYBOX_STAR_VTX_SCALE = 1000.0f;   // Vtx positions are s16, hence pre-multiplied

// The cheap shape used by the stars the count multiplier adds: a fan to four spike tips and four inner corners.
static constexpr s32 SKYBOX_STAR_SIMPLE_RIM_COUNT = 8;
static constexpr s32 SKYBOX_STAR_SIMPLE_VTX_COUNT = 1 + SKYBOX_STAR_SIMPLE_RIM_COUNT;
static constexpr f32 SKYBOX_STAR_SIMPLE_CENTER_ALPHA = 235.0f;
static constexpr f32 SKYBOX_STAR_SIMPLE_CORNER_RADIUS = 0.20f;
static constexpr f32 SKYBOX_STAR_SIMPLE_CORNER_ALPHA = 150.0f;
static constexpr f32 SKYBOX_STAR_SIMPLE_TIP_RADIUS = 0.52f;

static constexpr f32 SKYBOX_STAR_REFERENCE_INTENSITY = 0.5f; // intensity the shape alphas are tuned for
static constexpr f32 SKYBOX_STAR_TWINKLE_MAX_FREQUENCY = 2.0f;
static constexpr f32 SKYBOX_STAR_TWINKLE_MIN_RATIO = 0.15f;
static constexpr f32 SKYBOX_STAR_TWINKLE_DEPTH = 0.5f;            // 0.5 dips to half brightness, 1.0 to invisible
static constexpr f32 SKYBOX_STAR_CYCLES_PER_SECOND = 46875000.0f; // osGetTime() counts N64 cycles
static constexpr f32 SKYBOX_STAR_FADE_SECONDS = 2.0f;
static constexpr f32 SKYBOX_STAR_FADE_MAX_STEP = 0.25f; // a long stall must not swallow the whole fade

static constexpr s32 SKYBOX_STAR_CENTER_VTX = 0;
static constexpr s32 SKYBOX_STAR_CORE_VTX_COUNT = 1 + (2 * SKYBOX_STAR_CORE_SEGMENTS);
static constexpr s32 SKYBOX_STAR_VTX_COUNT = SKYBOX_STAR_CORE_VTX_COUNT + (4 * 6); // two G_VTX commands

#define SKYBOX_STAR_CORE_INNER_VTX(i) (1 + (i))
#define SKYBOX_STAR_CORE_OUTER_VTX(i) ((1 + SKYBOX_STAR_CORE_SEGMENTS) + (i))
#define SKYBOX_STAR_ARM_VTX(arm, v) ((1 + (2 * SKYBOX_STAR_CORE_SEGMENTS)) + (6 * (arm)) + (v))

static u16 sEnvSkyboxNumStars = 0;
// Dawn and dusk fade of the whole field
static f32 sSkyboxStarsAlpha = 0.0f;
// Star fade, 0 hides the field and 1 shows it at the sky brightness below
static f32 sSkyboxStarFade = 0.0f;
// Brightness of the last starry sky, kept so a sky turning cloudy or stormy can fade the field out
static f32 sSkyboxStarSkyAlpha = 0.0f;
// Scene the fade runs in, so a scene change snaps instead of fading
static s32 sSkyboxStarScene = -1;
static u64 sSkyboxStarFadeTime = 0;
// Branch point of the field in polyOpa, NULL when no stars are drawn this frame
static Gfx* sSkyboxStarsDList = NULL;
static Vtx sSkyboxStarVtx[SKYBOX_STAR_VTX_COUNT];
static bool sSkyboxStarVtxBuilt = false;
// Alpha the vertices were built with, -1 forces the first build
static f32 sSkyboxStarAlphaScale = -1.0f;
static Vtx sSkyboxSimpleStarVtx[SKYBOX_STAR_SIMPLE_VTX_COUNT];
static bool sSkyboxSimpleStarVtxBuilt = false;
static f32 sSkyboxSimpleStarAlphaScale = -1.0f;

// The branch point must land after the skybox and before the sun and moon, so the moon still covers the stars.
static void MajoraStarField_Setup(PlayState* play) {
    const u64 now = osGetTime();
    const f32 elapsed =
        (sSkyboxStarFadeTime != 0) ? (f32)(now - sSkyboxStarFadeTime) / SKYBOX_STAR_CYCLES_PER_SECOND : 0.0f;
    f32 blend = (f32)play->envCtx.skyboxBlend / 255.0f;
    f32 alpha = 0.0f;
    bool starry = false;

    sSkyboxStarFadeTime = now;

    // Follows the drawn sky, not the clock: skyboxTime, which picks these indices, is snapped to a canonical hour
    // in frozen time scenes and cutscenes, so it can disagree with dayTime.
    if (CVarGetInteger(CVAR_STAR_FIELD, 0) && (play->skyboxId == SKYBOX_NORMAL_SKY) && !play->envCtx.skyboxDisabled) {
        if ((play->envCtx.skybox1Index == SKYBOX_STAR_NIGHT_INDEX) &&
            (play->envCtx.skybox2Index == SKYBOX_STAR_NIGHT_INDEX)) {
            // Clear night
            alpha = 1.0f;
            starry = true;
        } else if ((play->envCtx.skybox1Index == SKYBOX_STAR_SUNSET_INDEX) &&
                   (play->envCtx.skybox2Index == SKYBOX_STAR_NIGHT_INDEX)) {
            // Sunset blending into the night sky
            alpha = blend;
            starry = true;
        } else if ((play->envCtx.skybox1Index == SKYBOX_STAR_NIGHT_INDEX) &&
                   (play->envCtx.skybox2Index == SKYBOX_STAR_SUNRISE_INDEX)) {
            // Night sky blending into sunrise
            alpha = 1.0f - blend;
            starry = true;
        }
    }

    if (play->sceneNum != sSkyboxStarScene) {
        // A scene change shows the field as the new sky is, it never fades across the transition
        sSkyboxStarScene = play->sceneNum;
        sSkyboxStarFade = starry ? 1.0f : 0.0f;
    } else {
        const f32 target = starry ? 1.0f : 0.0f;
        // A long stall should not swallow the whole fade at once
        const f32 step = CLAMP(elapsed, 0.0f, SKYBOX_STAR_FADE_MAX_STEP) / SKYBOX_STAR_FADE_SECONDS;

        if (sSkyboxStarFade < target) {
            sSkyboxStarFade += step;
            if (sSkyboxStarFade > target) {
                sSkyboxStarFade = target;
            }
        } else if (sSkyboxStarFade > target) {
            sSkyboxStarFade -= step;
            if (sSkyboxStarFade < target) {
                sSkyboxStarFade = target;
            }
        }
    }

    // The sky brightness is only meaningful while the sky is a starry one, but the fade keeps using the last value
    // so that a sky turning cloudy or stormy fades the field out instead of cutting it
    if (starry) {
        sSkyboxStarSkyAlpha = CLAMP(alpha, 0.0f, 1.0f);
    }

    sSkyboxStarsAlpha = sSkyboxStarSkyAlpha * sSkyboxStarFade;
    sEnvSkyboxNumStars = (sSkyboxStarFade > 0.0f) ? SKYBOX_STAR_BASE_COUNT : 0;

    if (sEnvSkyboxNumStars != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        sSkyboxStarsDList = POLY_OPA_DISP;

        gSPNoOp(POLY_OPA_DISP++);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        sSkyboxStarsDList = NULL;
    }
}

// Vanilla quad shape, drawn through a recorded matrix so the frame interpolation can smooth it.
static void Ship_DrawSkyboxStarInterpolated(Gfx** gfxP, PlayState* play, Vec3f pos, u32 width, u32 height,
                                            s32 starNum) {
    static Vtx sStarVtxData[] = {
        VTX(-1, -1, 0, 0, 0, 0, 0, 0, 255),
        VTX(1, -1, 0, 0, 0, 0, 0, 0, 255),
        VTX(-1, 1, 0, 0, 0, 0, 0, 0, 255),
        VTX(1, 1, 0, 0, 0, 0, 0, 0, 255),
    };

    Gfx* gfx = *gfxP;
    f32 eyeDist;
    f32 distRatio;
    f32 x;
    f32 y;
    f32 z;
    f32 scaleMultiplier;

    // One time logic before drawing the first star
    if (starNum == 0) {
        gSPLoadGeometryMode(gfx++, G_CULL_BACK);
    }

    if (width >= 2) {
        // Pull the star to 5500 units from the eye so the far plane cannot clip it
        eyeDist = Math3D_Vec3f_DistXYZ(&pos, &play->view.eye);

        if (eyeDist > 0.0f) {
            distRatio = 5500.0f / eyeDist;

            x = play->view.eye.x + ((pos.x - play->view.eye.x) * distRatio);
            y = play->view.eye.y + ((pos.y - play->view.eye.y) * distRatio);
            z = play->view.eye.z + ((pos.z - play->view.eye.z) * distRatio);

            // Cancel the field of view so the apparent size stays the same
            scaleMultiplier = 3.0f / (60.0f / play->view.fovy);

            Matrix_Push();

            Matrix_Translate(x, y, z, MTXMODE_NEW);
            Matrix_Scale(width * scaleMultiplier, height * scaleMultiplier, 1.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            gSPMatrix(gfx++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPVertex(gfx++, (uintptr_t)sStarVtxData, ARRAY_COUNT(sStarVtxData), 0);
            gSP2Triangles(gfx++, 0, 1, 2, 0, 1, 3, 2, 0);

            Matrix_Pop();
        }
    }

    *gfxP = gfx;
}

static s16 Ship_SkyboxStarVtxCoord(f32 value) {
    f32 scaled = value * SKYBOX_STAR_VTX_SCALE;

    // Round away from zero to keep the star symmetrical
    return (s16)((scaled >= 0.0f) ? (scaled + 0.5f) : (scaled - 0.5f));
}

// Applies the intensity setting to a shape alpha
static u8 Ship_SkyboxStarAlpha(f32 baseAlpha, f32 alphaScale) {
    return (u8)(CLAMP(baseAlpha * alphaScale, 0.0f, 255.0f) + 0.5f);
}

static void Ship_SetSkyboxStarVtx(Vtx* vtx, f32 x, f32 y, u8 alpha) {
    vtx->v.ob[0] = Ship_SkyboxStarVtxCoord(x);
    vtx->v.ob[1] = Ship_SkyboxStarVtxCoord(y);
    vtx->v.ob[2] = 0;
    vtx->v.flag = 0;
    vtx->v.tc[0] = 0;
    vtx->v.tc[1] = 0;
    vtx->v.cn[0] = 255;
    vtx->v.cn[1] = 255;
    vtx->v.cn[2] = 255;
    vtx->v.cn[3] = alpha;
}

// One spike; the perpendicular comes from the direction so every arm keeps the same winding.
static void Ship_BuildSkyboxStarArm(s32 arm, f32 alongX, f32 alongY, f32 length, f32 alpha, f32 alphaScale) {
    f32 acrossX = -alongY;
    f32 acrossY = alongX;
    f32 midX = alongX * length * SKYBOX_STAR_ARM_MID_T;
    f32 midY = alongY * length * SKYBOX_STAR_ARM_MID_T;
    f32 tipX = alongX * length;
    f32 tipY = alongY * length;
    u8 baseAlpha = Ship_SkyboxStarAlpha(alpha, alphaScale);
    u8 midAlpha = Ship_SkyboxStarAlpha(alpha * SKYBOX_STAR_ARM_MID_ALPHA, alphaScale);

    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 0)], acrossX * SKYBOX_STAR_ARM_WIDTH_BASE,
                          acrossY * SKYBOX_STAR_ARM_WIDTH_BASE, baseAlpha);
    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 1)], -acrossX * SKYBOX_STAR_ARM_WIDTH_BASE,
                          -acrossY * SKYBOX_STAR_ARM_WIDTH_BASE, baseAlpha);
    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 2)], midX - (acrossX * SKYBOX_STAR_ARM_WIDTH_MID),
                          midY - (acrossY * SKYBOX_STAR_ARM_WIDTH_MID), midAlpha);
    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 3)], midX + (acrossX * SKYBOX_STAR_ARM_WIDTH_MID),
                          midY + (acrossY * SKYBOX_STAR_ARM_WIDTH_MID), midAlpha);
    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 4)], tipX - (acrossX * SKYBOX_STAR_ARM_WIDTH_TIP),
                          tipY - (acrossY * SKYBOX_STAR_ARM_WIDTH_TIP), 0);
    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_ARM_VTX(arm, 5)], tipX + (acrossX * SKYBOX_STAR_ARM_WIDTH_TIP),
                          tipY + (acrossY * SKYBOX_STAR_ARM_WIDTH_TIP), 0);
}

static void Ship_BuildSkyboxStarGeometry(f32 alphaScale) {
    const s32 angleStep = 0x10000 / SKYBOX_STAR_CORE_SEGMENTS;
    s32 i;

    Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_CENTER_VTX], 0.0f, 0.0f,
                          Ship_SkyboxStarAlpha(SKYBOX_STAR_CORE_CENTER_ALPHA, alphaScale));

    for (i = 0; i < SKYBOX_STAR_CORE_SEGMENTS; i++) {
        s16 angle = (s16)(angleStep * i);
        f32 cosA = Math_CosS(angle);
        f32 sinA = Math_SinS(angle);

        Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_CORE_INNER_VTX(i)], cosA * SKYBOX_STAR_CORE_INNER_RADIUS,
                              sinA * SKYBOX_STAR_CORE_INNER_RADIUS,
                              Ship_SkyboxStarAlpha(SKYBOX_STAR_CORE_INNER_ALPHA, alphaScale));
        Ship_SetSkyboxStarVtx(&sSkyboxStarVtx[SKYBOX_STAR_CORE_OUTER_VTX(i)], cosA * SKYBOX_STAR_CORE_RADIUS,
                              sinA * SKYBOX_STAR_CORE_RADIUS, 0);
    }

    // The horizontal spikes are longer and brighter than the vertical ones
    Ship_BuildSkyboxStarArm(0, 1.0f, 0.0f, SKYBOX_STAR_ARM_LENGTH_H, SKYBOX_STAR_ARM_ALPHA_H, alphaScale);
    Ship_BuildSkyboxStarArm(1, -1.0f, 0.0f, SKYBOX_STAR_ARM_LENGTH_H, SKYBOX_STAR_ARM_ALPHA_H, alphaScale);
    Ship_BuildSkyboxStarArm(2, 0.0f, 1.0f, SKYBOX_STAR_ARM_LENGTH_V, SKYBOX_STAR_ARM_ALPHA_V, alphaScale);
    Ship_BuildSkyboxStarArm(3, 0.0f, -1.0f, SKYBOX_STAR_ARM_LENGTH_V, SKYBOX_STAR_ARM_ALPHA_V, alphaScale);

    sSkyboxStarAlphaScale = alphaScale;
    sSkyboxStarVtxBuilt = true;
}

// The intensity lives in the vertex alphas, so rebuild when it changes
static void Ship_EnsureSkyboxStarGeometry(f32 alphaScale) {
    if (!sSkyboxStarVtxBuilt || (sSkyboxStarAlphaScale != alphaScale)) {
        Ship_BuildSkyboxStarGeometry(alphaScale);
    }
}

// Cheaper shape with the same silhouette: a fan from the centre to four spike tips and four inner corners
static void Ship_BuildSimpleSkyboxStarGeometry(f32 alphaScale) {
    const s32 angleStep = 0x10000 / SKYBOX_STAR_SIMPLE_RIM_COUNT;
    s32 i;

    Ship_SetSkyboxStarVtx(&sSkyboxSimpleStarVtx[0], 0.0f, 0.0f,
                          Ship_SkyboxStarAlpha(SKYBOX_STAR_SIMPLE_CENTER_ALPHA, alphaScale));

    for (i = 0; i < SKYBOX_STAR_SIMPLE_RIM_COUNT; i++) {
        s16 angle = (s16)(angleStep * i);
        f32 cosA = Math_CosS(angle);
        f32 sinA = Math_SinS(angle);

        // Every other rim vertex is a spike tip, the ones between are the inner corners
        if ((i & 1) == 0) {
            Ship_SetSkyboxStarVtx(&sSkyboxSimpleStarVtx[1 + i], cosA * SKYBOX_STAR_SIMPLE_TIP_RADIUS,
                                  sinA * SKYBOX_STAR_SIMPLE_TIP_RADIUS, 0);
        } else {
            Ship_SetSkyboxStarVtx(&sSkyboxSimpleStarVtx[1 + i], cosA * SKYBOX_STAR_SIMPLE_CORNER_RADIUS,
                                  sinA * SKYBOX_STAR_SIMPLE_CORNER_RADIUS,
                                  Ship_SkyboxStarAlpha(SKYBOX_STAR_SIMPLE_CORNER_ALPHA, alphaScale));
        }
    }

    sSkyboxSimpleStarAlphaScale = alphaScale;
    sSkyboxSimpleStarVtxBuilt = true;
}

static void Ship_EnsureSimpleSkyboxStarGeometry(f32 alphaScale) {
    if (!sSkyboxSimpleStarVtxBuilt || (sSkyboxSimpleStarAlphaScale != alphaScale)) {
        Ship_BuildSimpleSkyboxStarGeometry(alphaScale);
    }
}

static void Ship_SetupSkyboxStarGfx(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    // RGB from the per star primitive color, alpha from the vertex color that holds the shape
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, SHADE, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, SHADE, 0, PRIMITIVE, 0);

    *gfxP = gfx;
}

// Per star hash, outside the layout's randInt stream so the layout never changes
static u32 Ship_SkyboxStarHash(u32 seed, s32 starNum) {
    u32 hash = seed ^ ((u32)starNum * 0x9E3779B1);

    hash ^= hash >> 16;
    hash *= 0x7FEB352D;
    hash ^= hash >> 15;
    hash *= 0x846CA68B;
    hash ^= hash >> 16;

    return hash;
}

// Per star twinkle factor, oscillating between (1 - SKYBOX_STAR_TWINKLE_DEPTH) and 1
static f32 Ship_SkyboxStarTwinkle(u32 seed, s32 starNum, f32 time, f32 maxFrequency) {
    u32 hash = Ship_SkyboxStarHash(seed, starNum);
    f32 frequency;
    f32 turns;
    f32 fraction;
    f32 halfDepth;

    // A maximum of 0 means no twinkling
    if (maxFrequency <= 0.0f) {
        return 1.0f;
    }

    frequency = maxFrequency * (SKYBOX_STAR_TWINKLE_MIN_RATIO +
                                ((1.0f - SKYBOX_STAR_TWINKLE_MIN_RATIO) * ((f32)(hash & 0xFF) / 255.0f)));
    // Random phase, in turns
    turns = (frequency * time) + ((f32)((hash >> 8) & 0xFFFF) / 65536.0f);
    fraction = turns - floorf(turns);
    halfDepth = SKYBOX_STAR_TWINKLE_DEPTH * 0.5f;

    // Math_SinS takes a 16 bit angle where a full turn is 0x10000
    return (1.0f - halfDepth) + (halfDepth * Math_SinS((s16)(s32)(fraction * 65536.0f)));
}

static void Ship_DrawSkyboxStarTelescope(Gfx** gfxP, PlayState* play, Vec3f pos, u32 size, s32 starNum,
                                         f32 sizeMultiplier, f32 alphaScale, bool simple) {
    Gfx* gfx = *gfxP;
    f32 eyeDist;
    f32 distRatio;
    f32 x;
    f32 y;
    f32 z;
    f32 scaleMultiplier;
    f32 starScale;
    f32 spikeScaleX;
    f32 spikeScaleY;
    u32 variation;
    s32 i;

    // One time logic before drawing the first star
    if (starNum == 0) {
        Ship_EnsureSkyboxStarGeometry(alphaScale);

        gSPLoadGeometryMode(gfx++, G_CULL_BACK);
    }

    eyeDist = Math3D_Vec3f_DistXYZ(&pos, &play->view.eye);

    if ((size >= 2) && (eyeDist > 0.0f)) {
        // Pull the star to 5500 units from the eye so the far plane cannot clip it
        distRatio = 5500.0f / eyeDist;

        x = play->view.eye.x + ((pos.x - play->view.eye.x) * distRatio);
        y = play->view.eye.y + ((pos.y - play->view.eye.y) * distRatio);
        z = play->view.eye.z + ((pos.z - play->view.eye.z) * distRatio);

        // Cancel the field of view so the apparent size stays the same
        scaleMultiplier = 3.0f / (60.0f / play->view.fovy);

        // Vary the spike lengths from star to star
        variation = (u32)starNum * 0x9E3779B1;
        spikeScaleX = 0.9f + ((variation >> 4) & 0x3) * 0.1f;
        spikeScaleY = 0.9f + ((variation >> 8) & 0x3) * 0.1f;

        // Undo the vertex pre-multiplication
        starScale = ((f32)size * sizeMultiplier * scaleMultiplier) / SKYBOX_STAR_VTX_SCALE;

        Matrix_Push();

        Matrix_Translate(x, y, z, MTXMODE_NEW);
        Matrix_Scale(starScale * spikeScaleX, starScale * spikeScaleY, 1.0f, MTXMODE_APPLY);
        // The billboard matrix is kept up to date by Play_Draw, so the star always faces the camera
        Matrix_ReplaceRotation(&play->billboardMtxF);

        gSPMatrix(gfx++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        if (simple) {
            Ship_EnsureSimpleSkyboxStarGeometry(alphaScale);
            gSPVertex(gfx++, (uintptr_t)sSkyboxSimpleStarVtx, SKYBOX_STAR_SIMPLE_VTX_COUNT, 0);

            // Fan from the centre to each pair of rim vertices
            for (i = 0; i < SKYBOX_STAR_SIMPLE_RIM_COUNT; i += 2) {
                s32 next = 1 + ((i + 1) % SKYBOX_STAR_SIMPLE_RIM_COUNT);
                s32 nextNext = 1 + ((i + 2) % SKYBOX_STAR_SIMPLE_RIM_COUNT);

                gSP2Triangles(gfx++, 0, 1 + i, next, 0, 0, next, nextNext, 0);
            }
        } else {
            // Too many vertices for one G_VTX command, so load them in two batches
            gSPVertex(gfx++, (uintptr_t)sSkyboxStarVtx, SKYBOX_STAR_CORE_VTX_COUNT, 0);
            gSPVertex(gfx++, (uintptr_t)(sSkyboxStarVtx + SKYBOX_STAR_CORE_VTX_COUNT),
                      SKYBOX_STAR_VTX_COUNT - SKYBOX_STAR_CORE_VTX_COUNT, SKYBOX_STAR_CORE_VTX_COUNT);

            // Core fan
            for (i = 0; i < SKYBOX_STAR_CORE_SEGMENTS; i += 2) {
                s32 next = (i + 1) % SKYBOX_STAR_CORE_SEGMENTS;
                s32 nextNext = (i + 2) % SKYBOX_STAR_CORE_SEGMENTS;

                gSP2Triangles(gfx++, SKYBOX_STAR_CENTER_VTX, SKYBOX_STAR_CORE_INNER_VTX(i),
                              SKYBOX_STAR_CORE_INNER_VTX(next), 0, SKYBOX_STAR_CENTER_VTX,
                              SKYBOX_STAR_CORE_INNER_VTX(next), SKYBOX_STAR_CORE_INNER_VTX(nextNext), 0);
            }

            // Core outer band, which softens the glow edge
            for (i = 0; i < SKYBOX_STAR_CORE_SEGMENTS; i++) {
                s32 next = (i + 1) % SKYBOX_STAR_CORE_SEGMENTS;

                gSP2Triangles(gfx++, SKYBOX_STAR_CORE_INNER_VTX(i), SKYBOX_STAR_CORE_OUTER_VTX(i),
                              SKYBOX_STAR_CORE_OUTER_VTX(next), 0, SKYBOX_STAR_CORE_INNER_VTX(i),
                              SKYBOX_STAR_CORE_OUTER_VTX(next), SKYBOX_STAR_CORE_INNER_VTX(next), 0);
            }

            // Spikes, base band then tip band
            for (i = 0; i < 4; i++) {
                gSP2Triangles(gfx++, SKYBOX_STAR_ARM_VTX(i, 0), SKYBOX_STAR_ARM_VTX(i, 1), SKYBOX_STAR_ARM_VTX(i, 2), 0,
                              SKYBOX_STAR_ARM_VTX(i, 0), SKYBOX_STAR_ARM_VTX(i, 2), SKYBOX_STAR_ARM_VTX(i, 3), 0);
                gSP2Triangles(gfx++, SKYBOX_STAR_ARM_VTX(i, 3), SKYBOX_STAR_ARM_VTX(i, 2), SKYBOX_STAR_ARM_VTX(i, 4), 0,
                              SKYBOX_STAR_ARM_VTX(i, 3), SKYBOX_STAR_ARM_VTX(i, 4), SKYBOX_STAR_ARM_VTX(i, 5), 0);
            }
        }

        Matrix_Pop();
    }

    *gfxP = gfx;
}

static void DrawSkyboxStarsImpl(PlayState* play, Gfx** gfxP) {
    // The 16 fixed stars of Majora's Mask, as offsets from the eye. The hex values above 0x7FFF are the negative
    // offsets the original table stores, hence the casts.
    static const Vec3s sStarPositions[] = {
        { 0x0384, 0x2328, (s16)0xD508 }, { 0x09C4, 0x2328, (s16)0xDA1C }, { 0x0E74, 0x22D8, (s16)0xDA1C },
        { 0x1450, 0x2468, (s16)0xD8F0 }, { 0x1C84, 0x28A0, (s16)0xCBA8 }, { 0x1F40, 0x2134, (s16)0xD8F0 },
        { 0x1F40, 0x28A0, (s16)0xDAE4 }, { (s16)0xE4A8, 0x4A38, 0x4A38 }, { (s16)0xD058, 0x4C2C, 0x3A98 },
        { (s16)0xD8F0, 0x36B0, 0x47E0 }, { (s16)0xD954, 0x3264, 0x3E1C }, { (s16)0xD8F0, 0x3070, 0x37DC },
        { (s16)0xD8F0, 0x1F40, 0x5208 }, { (s16)0xD760, 0x1838, 0x27D8 }, { 0x0000, 0x4E20, 0x4A38 },
        { 0x076C, 0x2328, (s16)0xDCD8 },
    };
    static const Color_RGBA8 sStarColors[] = {
        { 65, 164, 255, 255 },  { 131, 164, 230, 255 }, { 98, 205, 255, 255 }, { 82, 82, 255, 255 },
        { 123, 164, 164, 255 }, { 98, 205, 255, 255 },  { 98, 164, 230, 255 }, { 255, 90, 0, 255 },
    };
    static const Color_RGBA8 sRandomStarColors[] = {
        { 64, 80, 112, 255 },   { 96, 96, 128, 255 },   { 128, 112, 144, 255 }, { 160, 128, 160, 255 },
        { 192, 144, 168, 255 }, { 224, 160, 176, 255 }, { 224, 160, 176, 255 }, { 104, 104, 136, 255 },
        { 136, 120, 152, 255 }, { 168, 136, 168, 255 }, { 200, 152, 184, 255 }, { 232, 168, 184, 255 },
        { 224, 176, 184, 255 }, { 240, 192, 192, 255 }, { 232, 184, 192, 255 }, { 248, 200, 192, 255 },
    };

    Vec3f pos;
    Gfx* gfx;
    s32 randomColorIndex;
    s32 i;
    u32 randInt;
    u32 imgWidth;
    s32 telescopeStars;
    f32 telescopeStarSize;
    f32 telescopeStarAlphaScale;
    s32 twinkleStars;
    f32 twinkleMaxFrequency;
    f32 twinkleTime;
    u32 starSeed;
    Color_RGBA8 starColor;
    bool primColorChanged;
    f32 starCountMultiplier;
    s32 starCount;

    gfx = *gfxP;

    telescopeStars = CVarGetInteger(CVAR_STAR_TELESCOPE, 1);
    telescopeStarSize = CLAMP(CVarGetFloat(CVAR_STAR_SIZE, 2.0f), 1.0f, 3.0f);
    telescopeStarAlphaScale = CLAMP(CVarGetFloat(CVAR_STAR_INTENSITY, SKYBOX_STAR_REFERENCE_INTENSITY), 0.0f, 1.0f) /
                              SKYBOX_STAR_REFERENCE_INTENSITY;
    twinkleStars = CVarGetInteger(CVAR_STAR_TWINKLE, 0);
    twinkleMaxFrequency =
        CLAMP(CVarGetFloat(CVAR_STAR_TWINKLE_FREQUENCY, 1.0f), 0.0f, SKYBOX_STAR_TWINKLE_MAX_FREQUENCY);
    twinkleTime = (f32)(osGetTime() / SKYBOX_STAR_CYCLES_PER_SECOND);
    starCountMultiplier = CLAMP(CVarGetFloat(CVAR_STAR_COUNT, SKYBOX_STAR_COUNT_MULTIPLIER_MIN),
                                SKYBOX_STAR_COUNT_MULTIPLIER_MIN, SKYBOX_STAR_COUNT_MULTIPLIER_MAX);
    starCount = (s32)(sEnvSkyboxNumStars * starCountMultiplier);

    // Play_Draw keeps billboardMtxF and viewProjectionMtxF up to date, so the engine matrices are reused as is.
    randomColorIndex = 0;

    gDPPipeSync(gfx++);
    gDPSetEnvColor(gfx++, 255, 255, 255, (u8)(255.0f * sSkyboxStarsAlpha));
    gDPSetCombineLERP(gfx++, PRIMITIVE, 0, ENVIRONMENT, 0, PRIMITIVE, 0, ENVIRONMENT, 0, PRIMITIVE, 0, ENVIRONMENT, 0,
                      PRIMITIVE, 0, ENVIRONMENT, 0);
    gDPSetOtherMode(gfx++,
                    G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PRIM | G_RM_AA_XLU_LINE | G_RM_AA_XLU_LINE2);

    // Fast3D only applies the alpha logic on XLU_SURF
    gDPSetRenderMode(gfx++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

    // The telescope shape carries its silhouette in the vertex alpha, so it needs its own combine mode
    if (telescopeStars) {
        Ship_SetupSkyboxStarGfx(&gfx);
    }

    randInt = ((u32)gSaveContext.playerName[0] << 0x18) ^ ((u32)gSaveContext.playerName[1] << 0x14) ^
              ((u32)gSaveContext.playerName[2] << 0x10) ^ ((u32)gSaveContext.playerName[3] << 0xC) ^
              ((u32)gSaveContext.playerName[4] << 8) ^ ((u32)gSaveContext.playerName[5] << 4) ^
              ((u32)gSaveContext.playerName[6] << 0) ^ ((u32)gSaveContext.playerName[7] >> 4) ^
              ((u32)gSaveContext.playerName[7] << 0x1C);

    // Same seed as the layout, captured before the loop mutates randInt
    starSeed = randInt;

    for (i = 0; i < starCount; i++) {
        // Extra stars continue the same random stream, so existing ones do not move. Stop before polyOpa fills up.
        if ((i >= sEnvSkyboxNumStars) &&
            ((s32)((u8*)play->state.gfxCtx->polyOpa.d - (u8*)gfx) < SKYBOX_STAR_MIN_FREE_BYTES)) {
            break;
        }

        if (i < 16) {
            pos.x = play->view.eye.x + (s32)sStarPositions[i].x;
            pos.y = play->view.eye.y + (s32)sStarPositions[i].y;
            pos.z = play->view.eye.z + (s32)sStarPositions[i].z;
            imgWidth = 8;
        } else {
            f32 temp_f4;
            f32 temp_f20;
            f32 temp_f2;

            // Same random sequence as Majora's Mask, so the layout of the stars is identical
            temp_f4 = Rand_ZeroOne_Variable(&randInt);
            temp_f20 = (Rand_ZeroOne_Variable(&randInt) + temp_f4) * 0.5f;

            randInt = Rand_Next_Variable(&randInt);

            pos.y = play->view.eye.y + (SQ(temp_f20) * SQ(128.0f)) - 1000.0f;
            pos.x = play->view.eye.x + (Math_SinS((s16)randInt) * (1.2f - temp_f20) * SQ(128.0f));
            pos.z = play->view.eye.z + (Math_CosS((s16)randInt) * (1.2f - temp_f20) * SQ(128.0f));

            temp_f2 = Rand_ZeroOne_Variable(&randInt);
            imgWidth = (u32)((SQ(temp_f2) * 8.0f) + 2.0f);
        }

        if ((i < 15) || ((i == 15) && ((((void)0, gSaveContext.totalDays) % 7) == 0))) {
            starColor = sStarColors[i % ARRAY_COUNTU(sStarColors)];
            primColorChanged = true;
        } else if (((i & 0x3F) == 0) || (i == 16)) {
            starColor = sRandomStarColors[randomColorIndex % ARRAY_COUNTU(sRandomStarColors)];
            randomColorIndex++;
            primColorChanged = true;
        } else {
            primColorChanged = false;
        }

        // The per star twinkle needs its own primitive alpha, so the color is resent every star
        if (primColorChanged || twinkleStars) {
            // The telescope combine mode drops the environment color, so the dawn and dusk fade is folded in here
            f32 primAlpha = telescopeStars ? (starColor.a * sSkyboxStarsAlpha) : starColor.a;

            if (twinkleStars) {
                primAlpha *= Ship_SkyboxStarTwinkle(starSeed, i, twinkleTime, twinkleMaxFrequency);
            }

            gDPSetPrimColor(gfx++, 0, 0, starColor.r, starColor.g, starColor.b, (u8)CLAMP(primAlpha, 0.0f, 255.0f));
        }

        // Both shapes go through a recorded matrix so the frame interpolation can smooth them
        if (telescopeStars) {
            // The stars the multiplier adds use the cheap shape, which keeps the display list small
            Ship_DrawSkyboxStarTelescope(&gfx, play, pos, imgWidth, i, telescopeStarSize, telescopeStarAlphaScale,
                                         i >= sEnvSkyboxNumStars);
        } else {
            Ship_DrawSkyboxStarInterpolated(&gfx, play, pos, imgWidth, 4, i);
        }
    }

    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

// Fills the branch point placed by MajoraStarField_Setup, now that the view matrix of the frame is final.
static void MajoraStarField_Draw(PlayState* play) {
    Gfx* nextOpa;
    Gfx* opa;

    if (sSkyboxStarsDList != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        opa = POLY_OPA_DISP;
        nextOpa = Graph_GfxPlusOne(opa);

        gSPDisplayList(sSkyboxStarsDList, nextOpa);

        DrawSkyboxStarsImpl(play, &nextOpa);

        gSPEndDisplayList(nextOpa++);

        Graph_BranchDlist(opa, nextOpa);

        POLY_OPA_DISP = nextOpa;
        sSkyboxStarsDList = NULL;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

static void RegisterMajoraStarField() {
    COND_HOOK(OnPlayDrawSkyboxEnd, CVarGetInteger(CVAR_STAR_FIELD, 0), []() { MajoraStarField_Setup(gPlayState); });
    COND_HOOK(OnPlayDrawViewFinal, CVarGetInteger(CVAR_STAR_FIELD, 0), []() { MajoraStarField_Draw(gPlayState); });

    sSkyboxStarsDList = NULL;
    sSkyboxStarScene = -1;
}

static RegisterShipInitFunc initFunc(RegisterMajoraStarField, { CVAR_STAR_FIELD });
