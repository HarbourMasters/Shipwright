// Wind Waker-style toon lighting — game-side policy.
//
// libultraship owns the per-pixel transport: it relights the draws SoH brackets with gSPToon, using
// one dominant light (gSPToonKey) and a generic ramp (SetToonRamp). This module owns the OoT-specific
// policy that the framework must never know about:
//   - which light is the key (closest in-range point light, else the day/night sun/moon),
//   - how the key eases from one source to another (per-actor persistent state),
//   - the look tuning (ramp parameters), pushed once per frame.
// The framework never reads SoH's CVars; everything it needs is pushed from here.

#include <libultraship/bridge.h>
#include <ship/Context.h>
#include <fast/Fast3dWindow.h>
#include <fast/interpreter.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"
// Declares the FrameInterpolation_Record* functions (with C linkage) that the OPEN_DISPS/CLOSE_DISPS
// macros call, so their references in this TU match the definitions. Must precede any OPEN_DISPS use.
#include "soh/frame_interpolation.h"

#include <memory>
#include <unordered_map>

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

// ---------------------------------------------------------------------------------------------------
// Ramp parameters (frame-global look tuning)
// ---------------------------------------------------------------------------------------------------

// Defaults mirror libultraship's TOON_SHADING_DEFAULT_* and the GUI slider defaults, so the look is
// identical whether or not the app has pushed values yet.
static constexpr float kDefaultRampCenter = 0.5f;
static constexpr float kDefaultRampSoftness = 0.1f;
static constexpr float kDefaultHighlightIntensity = 1.0f;
static constexpr float kDefaultShadowIntensity = 1.0f;

// Selection defaults (game-side only — these never reach the framework).
static constexpr float kDefaultPointLightRange = 1.0f;
static constexpr float kDefaultTransitionTime = 2.5f;

// The Fast3D rendering backend, if the active window is the Fast3D window. Null on other windows
// (e.g. headless), in which case there is nothing to relight and pushing is simply skipped.
static Fast::GfxRenderingAPI* GetRenderingApi() {
    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    if (wnd == nullptr) {
        return nullptr;
    }
    auto interpreter = wnd->GetInterpreterWeak().lock();
    if (interpreter == nullptr) {
        return nullptr;
    }
    return interpreter->GetCurrentRenderingAPI();
}

// The toon key most recently emitted into the display-list stream this pass, stored as the quantized
// bytes gSPToonKey carries (s8 direction, u8 color). A later actor whose key quantizes to the same
// bytes reuses it instead of re-emitting: with no new gSPToonKey, the interpreter has no per-object key
// to flush, so same-key actors (e.g. everything lit by the sun) batch together. Cleared once per frame
// in OnToonFrameUpdate, so the first actor of every actor-draw pass always emits a fresh key.
static bool sHaveLastKey = false;
static s8 sLastKeyDir[3];
static u8 sLastKeyCol[3];

// Runs once per frame (game-frame-update hook, after the frame's draw). Pushes the frame-global ramp
// shape to the renderer and clears the per-pass key-dedup state so the next frame's first actor
// re-emits. The ramp is pure look-tuning, so it lives in SoH's CVars; reading it here (once per frame)
// keeps those strings out of the framework.
static void OnToonFrameUpdate() {
    // Clear before any early-out, so the dedup state resets even on a headless window (no renderer).
    sHaveLastKey = false;

    Fast::GfxRenderingAPI* rapi = GetRenderingApi();
    if (rapi == nullptr) {
        return;
    }
    rapi->SetToonRamp(CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.RampCenter"), kDefaultRampCenter),
                      CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.RampSoftness"), kDefaultRampSoftness),
                      CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.HighlightIntensity"),
                                   kDefaultHighlightIntensity),
                      CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.ShadowIntensity"),
                                   kDefaultShadowIntensity));
}

// ---------------------------------------------------------------------------------------------------
// Per-actor key-light state (eased "travel" between light sources)
// ---------------------------------------------------------------------------------------------------

// The key light animates smoothly toward its target, so the transition persists across frames. State
// is keyed by the actor pointer and evicted on the actor's destroy hook, so two live actors never
// collide and stale pointers never linger.
typedef struct {
    f32 dir[3];
    f32 col[3];
    f32 colVel[3];
} ToonKeyState;

static std::unordered_map<Actor*, ToonKeyState> sToonKeyStates;

// Critically-damped smoothing (Unity-style SmoothDamp): eases a value toward a moving target with no
// overshoot, accelerating then decelerating, for a smooth toon-key "travel".
static f32 ToonSmoothDamp(f32 current, f32 target, f32* vel, f32 smoothTime, f32 dt) {
    if (smoothTime < 0.0001f) {
        smoothTime = 0.0001f;
    }
    f32 omega = 2.0f / smoothTime;
    f32 x = omega * dt;
    f32 expTerm = 1.0f / (1.0f + x + (0.48f * x * x) + (0.235f * x * x * x));
    f32 change = current - target;
    f32 temp = (*vel + (omega * change)) * dt;
    *vel = (*vel - (omega * temp)) * expTerm;
    return target + ((change + temp) * expTerm);
}

// Antipode-safe spherical interpolation of a unit direction by fraction t. Rotating along the sphere
// (never through the centre) keeps the key from snapping when the dominant light swings to the far
// side — e.g. when a fairy's light switches off. The near-aligned fast path uses no trig, so the
// common per-frame case is cheap.
static void ToonSlerp(f32 from[3], f32 to[3], f32 t, f32 out[3]) {
    f32 dot = (from[0] * to[0]) + (from[1] * to[1]) + (from[2] * to[2]);
    f32 len;

    dot = (dot < -1.0f) ? -1.0f : ((dot > 1.0f) ? 1.0f : dot);

    if (dot > 0.9995f) {
        // Almost aligned: cheap linear step + renormalize (no trig).
        out[0] = from[0] + ((to[0] - from[0]) * t);
        out[1] = from[1] + ((to[1] - from[1]) * t);
        out[2] = from[2] + ((to[2] - from[2]) * t);
        len = sqrtf((out[0] * out[0]) + (out[1] * out[1]) + (out[2] * out[2]));
        if (len > 0.0001f) {
            out[0] /= len, out[1] /= len, out[2] /= len;
        }
        return;
    }

    if (dot < -0.9995f) {
        // Almost opposite: the great-circle path is ambiguous, so rotate around an arbitrary
        // perpendicular axis to reach the far side smoothly.
        f32 ref[3], perp[3], d, ang, c, s;

        if (fabsf(from[0]) < 0.9f) {
            ref[0] = 1.0f, ref[1] = 0.0f, ref[2] = 0.0f;
        } else {
            ref[0] = 0.0f, ref[1] = 1.0f, ref[2] = 0.0f;
        }
        d = (ref[0] * from[0]) + (ref[1] * from[1]) + (ref[2] * from[2]);
        perp[0] = ref[0] - (from[0] * d), perp[1] = ref[1] - (from[1] * d), perp[2] = ref[2] - (from[2] * d);
        len = sqrtf((perp[0] * perp[0]) + (perp[1] * perp[1]) + (perp[2] * perp[2]));
        if (len > 0.0001f) {
            perp[0] /= len, perp[1] /= len, perp[2] /= len;
        }
        ang = t * 3.14159265f;
        c = cosf(ang), s = sinf(ang);
        out[0] = (from[0] * c) + (perp[0] * s);
        out[1] = (from[1] * c) + (perp[1] * s);
        out[2] = (from[2] * c) + (perp[2] * s);
        return;
    }

    f32 theta = acosf(dot);
    f32 sinTheta = sinf(theta);
    f32 s0 = sinf((1.0f - t) * theta) / sinTheta;
    f32 s1 = sinf(t * theta) / sinTheta;
    out[0] = (s0 * from[0]) + (s1 * to[0]);
    out[1] = (s0 * from[1]) + (s1 * to[1]);
    out[2] = (s0 * from[2]) + (s1 * to[2]);
}

// ---------------------------------------------------------------------------------------------------
// Key-light selection
// ---------------------------------------------------------------------------------------------------

// Find the CLOSEST in-range point light (fairy, torch, bomb, ...) to the actor and, if any, fill the
// key direction (toward the light) + its colour. Brightness is intentionally ignored — proximity
// alone decides — so flickering torches are perfectly stable and the nearer of two always wins. A
// light is "in range" out to its radius × pointRange (raise pointRange to extend reach).
static bool ToonClosestPointLight(PlayState* play, Actor* actor, f32 pointRange, f32 dirOut[3], f32 colOut[3]) {
    LightNode* node = play->lightCtx.listHead;
    f32 bestDistSq = -1.0f;

    while (node != NULL) {
        LightInfo* info = node->info;

        if ((info != NULL) && (info->type != LIGHT_DIRECTIONAL)) {
            f32 dx = info->params.point.x - actor->world.pos.x;
            f32 dy = info->params.point.y - actor->world.pos.y;
            f32 dz = info->params.point.z - actor->world.pos.z;
            f32 radius = info->params.point.radius * pointRange;
            f32 distSq = (dx * dx) + (dy * dy) + (dz * dz);

            if ((radius > 0.0f) && (distSq > 0.0001f) && (distSq < (radius * radius)) &&
                ((bestDistSq < 0.0f) || (distSq < bestDistSq))) {
                f32 dist = sqrtf(distSq);

                bestDistSq = distSq;
                dirOut[0] = dx / dist, dirOut[1] = dy / dist, dirOut[2] = dz / dist;
                colOut[0] = info->params.point.color[0] / 255.0f;
                colOut[1] = info->params.point.color[1] / 255.0f;
                colOut[2] = info->params.point.color[2] / 255.0f;
            }
        }
        node = node->next;
    }
    return bestDistSq >= 0.0f;
}

// Key light from the environment directionals: the sun or the moon, whichever is currently brighter
// (so it tracks day/night). Always fills dirOut/colOut.
static void ToonEnvKey(PlayState* play, f32 dirOut[3], f32 colOut[3]) {
    LightInfo* sun = &play->envCtx.dirLight1;
    LightInfo* moon = &play->envCtx.dirLight2;
    s32 sunLum = sun->params.dir.color[0] + sun->params.dir.color[1] + sun->params.dir.color[2];
    s32 moonLum = moon->params.dir.color[0] + moon->params.dir.color[1] + moon->params.dir.color[2];
    LightInfo* env = (moonLum > sunLum) ? moon : sun;
    f32 d0 = env->params.dir.x, d1 = env->params.dir.y, d2 = env->params.dir.z;
    f32 len = sqrtf((d0 * d0) + (d1 * d1) + (d2 * d2));

    if (len > 0.001f) {
        dirOut[0] = d0 / len, dirOut[1] = d1 / len, dirOut[2] = d2 / len;
        colOut[0] = env->params.dir.color[0] / 255.0f;
        colOut[1] = env->params.dir.color[1] / 255.0f;
        colOut[2] = env->params.dir.color[2] / 255.0f;
    }
}

// ---------------------------------------------------------------------------------------------------
// Debug visualizer (dev-tools only)
// ---------------------------------------------------------------------------------------------------

// A thin 4-sided spike along +Y (base at the origin, tip at y=1). Scaled/rotated, it becomes a "light
// ray" pointing from an actor toward a light.
static Vtx sToonRayVtx[5] = {
    VTX(-1, 0, -1, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF), // base
    VTX(1, 0, -1, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(1, 0, 1, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-1, 0, 1, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 1, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF), // tip
};

static Gfx sToonRayDL[] = {
    gsSPVertex(sToonRayVtx, 5, 0),
    gsSP2Triangles(0, 1, 4, 0, 1, 2, 4, 0),
    gsSP2Triangles(2, 3, 4, 0, 3, 0, 4, 0),
    gsSP2Triangles(0, 2, 1, 0, 0, 3, 2, 0),
    gsSPEndDisplayList(),
};

// A flat 12-segment ring (annulus) in the XZ plane at base radius 100 (interleaved inner/outer
// verts). Scaled and centred at a point light, it shows that light's effective range so the Point
// Light Range slider's reach is visible.
static Vtx sToonRingVtx[24] = {
    VTX(97, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(103, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(84, 0, 48, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(89, 0, 52, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, 0, 84, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(52, 0, 89, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, 97, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),    VTX(0, 0, 103, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-48, 0, 84, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-52, 0, 89, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-84, 0, 48, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(-89, 0, 52, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-97, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(-103, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-84, 0, -48, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-89, 0, -52, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(-48, 0, -84, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF), VTX(-52, 0, -89, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(0, 0, -97, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),   VTX(0, 0, -103, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(48, 0, -84, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(52, 0, -89, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX(84, 0, -48, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),  VTX(89, 0, -52, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

static Gfx sToonRingDL[] = {
    gsSPVertex(sToonRingVtx, 24, 0),
    gsSP2Triangles(0, 1, 3, 0, 0, 3, 2, 0),
    gsSP2Triangles(2, 3, 5, 0, 2, 5, 4, 0),
    gsSP2Triangles(4, 5, 7, 0, 4, 7, 6, 0),
    gsSP2Triangles(6, 7, 9, 0, 6, 9, 8, 0),
    gsSP2Triangles(8, 9, 11, 0, 8, 11, 10, 0),
    gsSP2Triangles(10, 11, 13, 0, 10, 13, 12, 0),
    gsSP2Triangles(12, 13, 15, 0, 12, 15, 14, 0),
    gsSP2Triangles(14, 15, 17, 0, 14, 17, 16, 0),
    gsSP2Triangles(16, 17, 19, 0, 16, 19, 18, 0),
    gsSP2Triangles(18, 19, 21, 0, 18, 21, 20, 0),
    gsSP2Triangles(20, 21, 23, 0, 20, 23, 22, 0),
    gsSP2Triangles(22, 23, 1, 0, 22, 1, 0, 0),
    gsSPEndDisplayList(),
};

// Draw one debug "light ray": a flat-coloured spike from `base` pointing along the unit direction
// `dir`, `length` long and `thickness` wide. Drawn translucent and depth-test-free so every ray is
// visible (debug only — gated by the caller).
static void DrawDebugRay(PlayState* play, Vec3f* base, f32 dir[3], u8 r, u8 g, u8 b, f32 length, f32 thickness) {
    Vec3f axis;
    f32 horiz;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING | G_CULL_BACK | G_CULL_FRONT);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0,
                      PRIMITIVE);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, r, g, b, 200);

    // Orient the spike's +Y axis to `dir` (rotate +Y onto dir about their perpendicular).
    Matrix_Translate(base->x, base->y, base->z, MTXMODE_NEW);
    horiz = sqrtf((dir[0] * dir[0]) + (dir[2] * dir[2]));
    if (horiz > 0.001f) {
        axis.x = dir[2] / horiz; // normalized cross((0,1,0), dir) = (dir.z, 0, -dir.x)
        axis.y = 0.0f;
        axis.z = -dir[0] / horiz;
        Matrix_RotateAxis(Math_FAtan2F(horiz, dir[1]), &axis, MTXMODE_APPLY);
    } else if (dir[1] < 0.0f) {
        axis.x = 1.0f, axis.y = 0.0f, axis.z = 0.0f; // pointing straight down
        Matrix_RotateAxis(M_PI, &axis, MTXMODE_APPLY);
    }
    Matrix_Scale(thickness, length, thickness, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, sToonRayDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// Draw a flat debug ring of the given world `radius` centred at `center`, used to show a point
// light's effective range. Depth-test-free so it's always visible (debug only).
static void DrawDebugRing(PlayState* play, Vec3f* center, f32 radius, u8 r, u8 g, u8 b, u8 a) {
    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gSPClearGeometryMode(POLY_XLU_DISP++, G_LIGHTING | G_CULL_BACK | G_CULL_FRONT);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0,
                      PRIMITIVE);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, r, g, b, a);

    Matrix_Translate(center->x, center->y, center->z, MTXMODE_NEW);
    Matrix_Scale(radius * 0.01f, 1.0f, radius * 0.01f, MTXMODE_APPLY); // ring geometry is at base radius 100
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, sToonRingDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// The candidate-light rays, range rings, and the chosen-key needle (see the per-control help in the
// dev-tools viewer toggle). Gated by the caller; reads the same lights the selection does.
static void DrawDebugOverlay(PlayState* play, Actor* actor, f32 pointRange, f32 chosenDir[3]) {
    Player* player = GET_PLAYER(play);
    Vec3f base = { actor->world.pos.x, actor->world.pos.y + 30.0f, actor->world.pos.z };
    f32 cdir[3];
    LightNode* dn;
    // Draw the per-light range rings just once (on the player's pass) so they aren't redrawn by every
    // actor in the scene.
    s32 isPlayer = (player != NULL) && (actor == &player->actor);

    for (s32 i = 0; i < 2; i++) {
        LightInfo* env = (i == 0) ? &play->envCtx.dirLight1 : &play->envCtx.dirLight2;
        f32 e0 = env->params.dir.x, e1 = env->params.dir.y, e2 = env->params.dir.z;
        f32 elen = sqrtf((e0 * e0) + (e1 * e1) + (e2 * e2));
        f32 elum = (env->params.dir.color[0] + env->params.dir.color[1] + env->params.dir.color[2]) / (3.0f * 255.0f);

        if (elen > 0.001f) {
            cdir[0] = e0 / elen, cdir[1] = e1 / elen, cdir[2] = e2 / elen;
            DrawDebugRay(play, &base, cdir, env->params.dir.color[0], env->params.dir.color[1],
                         env->params.dir.color[2], 10.0f + (elum > 1.0f ? 1.0f : elum) * 15.0f, 1.2f);
        }
    }

    dn = play->lightCtx.listHead;
    while (dn != NULL) {
        LightInfo* info = dn->info;

        if ((info != NULL) && (info->type != LIGHT_DIRECTIONAL)) {
            f32 dx = info->params.point.x - actor->world.pos.x;
            f32 dy = info->params.point.y - actor->world.pos.y;
            f32 dz = info->params.point.z - actor->world.pos.z;
            f32 radius = info->params.point.radius * pointRange;
            f32 dsq = (dx * dx) + (dy * dy) + (dz * dz);

            // Cyan range ring at each point light (once, on the player pass), for ALL point lights
            // regardless of distance, so the Point Light Range slider's reach is visible.
            if (isPlayer && (radius > 0.0f)) {
                Vec3f lpos = { info->params.point.x, info->params.point.y, info->params.point.z };
                DrawDebugRing(play, &lpos, radius, 0, 255, 255, 110);
            }

            if ((radius > 0.0f) && (dsq < (radius * radius))) {
                f32 dist = sqrtf(dsq);
                f32 scale = 1.0f - ((dist / radius) * (dist / radius));
                f32 att = 0.5f + (0.5f * scale); // distance falloff, just for the visual length
                f32 plum = ((info->params.point.color[0] + info->params.point.color[1] +
                             info->params.point.color[2]) / (3.0f * 255.0f)) * att;

                if (dist > 0.001f) {
                    cdir[0] = dx / dist, cdir[1] = dy / dist, cdir[2] = dz / dist;
                    DrawDebugRay(play, &base, cdir, (u8)(info->params.point.color[0] * att),
                                 (u8)(info->params.point.color[1] * att), (u8)(info->params.point.color[2] * att),
                                 10.0f + (plum > 2.0f ? 2.0f : plum) * 12.5f, 1.2f);
                }
            }
        }
        dn = dn->next;
    }

    // The chosen key — a thin MAGENTA needle down the centre of the chosen light's cone, so the
    // direction is marked while the light's own colour is still visible in its (wider) cone.
    DrawDebugRay(play, &base, chosenDir, 255, 0, 255, 35.0f, 0.3f);
}

// ---------------------------------------------------------------------------------------------------
// Per-actor draw: choose, ease, and emit this actor's key light
// ---------------------------------------------------------------------------------------------------

// Wind Waker-style and simple — if any point light (fairy, torch, bomb, ...) is within range, the
// CLOSEST one is the key; otherwise the key is the sun or moon. The key animates toward its target
// with an eased, antipode-safe slerp, then is sent to the renderer via gSPToonKey. Runs inside
// Actor_Draw's display-list scope (via the OnActorDraw hook), so the key precedes the actor's geometry.
static void HandleActorDraw(void* actorPtr) {
    PlayState* play = gPlayState;
    Actor* actor = (Actor*)actorPtr;
    if (play == NULL || actor == NULL) {
        return;
    }

    f32 targetDir[3] = { 0.0f, 1.0f, 0.0f }; // default: lit from above
    f32 targetCol[3] = { 1.0f, 1.0f, 1.0f };
    // How far a point light reaches (× its radius) for selection, and how long the eased travel takes.
    f32 pointRange = CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.PointLightRange"), kDefaultPointLightRange);
    f32 transitionTime =
        CVarGetFloat(CVAR_ENHANCEMENT("Graphics.ToonLighting.TransitionTime"), kDefaultTransitionTime);

    OPEN_DISPS(play->state.gfxCtx);

    // Closest in-range point light wins outright; with none in range, fall back to the sun/moon.
    if (!ToonClosestPointLight(play, actor, pointRange, targetDir, targetCol)) {
        ToonEnvKey(play, targetDir, targetCol);
    }

    // Animate the key toward the chosen light with an eased "travel" (per-actor persistent state).
    auto [it, isNew] = sToonKeyStates.try_emplace(actor);
    ToonKeyState& st = it->second;
    if (isNew) {
        st.colVel[0] = st.colVel[1] = st.colVel[2] = 0.0f;
        st.dir[0] = targetDir[0], st.dir[1] = targetDir[1], st.dir[2] = targetDir[2];
        st.col[0] = targetCol[0], st.col[1] = targetCol[1], st.col[2] = targetCol[2];
    } else {
        // Seconds elapsed per draw. This draw runs once per game update, whose rate is the game's own
        // R_UPDATE_RATE divisor (3 = 20 fps in normal play, 1 = 60 fps during special transitions);
        // deriving dt from it keeps the eased "travel" the labelled number of seconds at any rate. Frame
        // interpolation replays this draw's output without re-running it, so it doesn't affect dt.
        f32 toonKeyDt = (R_UPDATE_RATE > 0 ? R_UPDATE_RATE : 3) / 60.0f;
        // Direction: antipode-safe eased slerp; alpha reaches ~99% in transitionTime seconds.
        f32 alpha = 1.0f - expf(-4.6f * toonKeyDt / (transitionTime < 0.05f ? 0.05f : transitionTime));
        f32 newDir[3];

        ToonSlerp(st.dir, targetDir, alpha, newDir);
        st.dir[0] = newDir[0], st.dir[1] = newDir[1], st.dir[2] = newDir[2];
        for (s32 i = 0; i < 3; i++) {
            st.col[i] = ToonSmoothDamp(st.col[i], targetCol[i], &st.colVel[i], transitionTime, toonKeyDt);
        }
    }

    {
        s8 dx = (s8)(st.dir[0] * 127.0f);
        s8 dy = (s8)(st.dir[1] * 127.0f);
        s8 dz = (s8)(st.dir[2] * 127.0f);
        u8 r = (u8)((st.col[0] < 0.0f ? 0.0f : (st.col[0] > 1.0f ? 1.0f : st.col[0])) * 255.0f);
        u8 g = (u8)((st.col[1] < 0.0f ? 0.0f : (st.col[1] > 1.0f ? 1.0f : st.col[1])) * 255.0f);
        u8 b = (u8)((st.col[2] < 0.0f ? 0.0f : (st.col[2] > 1.0f ? 1.0f : st.col[2])) * 255.0f);

        // Emit only when the quantized key differs from the last one emitted this pass. An identical
        // key would produce a byte-identical gSPToonKey (same lighting), so skipping it is free — and
        // it spares the interpreter a per-object flush, letting same-key actors batch. The key goes to
        // both display lists together, so they stay in lockstep. (Reset each frame; see OnToonFrameUpdate.)
        bool keyChanged = !sHaveLastKey || dx != sLastKeyDir[0] || dy != sLastKeyDir[1] || dz != sLastKeyDir[2] ||
                          r != sLastKeyCol[0] || g != sLastKeyCol[1] || b != sLastKeyCol[2];
        if (keyChanged) {
            gSPToonKey(POLY_OPA_DISP++, dx, dy, dz, r, g, b);
            gSPToonKey(POLY_XLU_DISP++, dx, dy, dz, r, g, b);
            sLastKeyDir[0] = dx, sLastKeyDir[1] = dy, sLastKeyDir[2] = dz;
            sLastKeyCol[0] = r, sLastKeyCol[1] = g, sLastKeyCol[2] = b;
            sHaveLastKey = true;
        }
    }

    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("ToonLighting.ShowDebug"), 0)) {
        DrawDebugOverlay(play, actor, pointRange, st.dir);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// Drop a destroyed actor's eased state so its slot can't be reused stale and the map can't grow
// unbounded over a session.
static void HandleActorDestroy(void* actorPtr) {
    sToonKeyStates.erase((Actor*)actorPtr);
}

void RegisterToonLighting() {
    bool enabled = CVarGetInteger(CVAR_ENHANCEMENT("Graphics.ToonLighting.Enabled"), 0);
    // Only hook while enabled, so a disabled feature adds no per-frame or per-actor work.
    COND_HOOK(OnGameFrameUpdate, enabled, OnToonFrameUpdate);
    COND_HOOK(OnActorDraw, enabled, HandleActorDraw);
    COND_HOOK(OnActorDestroy, enabled, HandleActorDestroy);
    // Drop the key-dedup state so the first actor after a (re-)enable always emits, before the
    // end-of-frame OnToonFrameUpdate reset has had a chance to run.
    sHaveLastKey = false;
    if (!enabled) {
        sToonKeyStates.clear();
    }
}

static RegisterShipInitFunc initFunc(RegisterToonLighting, { CVAR_ENHANCEMENT("Graphics.ToonLighting.Enabled") });
