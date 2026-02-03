/**
 * object_whip.c - Whip 3D model and draw functions
 *
 * Draws the snake whip with animated segments during use.
 * Visual: Orange-red snake body, diamond head with green eyes, purple crystal handle.
 */

#include "z64.h"
#include "../custom_items.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include <math.h>

// State constants (must match item_whip.h)
#define WHIP_STATE_EQUIP      1
#define WHIP_STATE_EXTENDING  2
#define WHIP_STATE_HIT_ENEMY  3
#define WHIP_STATE_ATTACHED   4
#define WHIP_STATE_SWINGING   5
#define WHIP_STATE_RETRACTING 6

// Visual constants
#define WHIP_BODY_SEGMENT  15.0f   // World units per body segment
#define WHIP_BODY_MAX_SEGS 40
#define WHIP_BODY_SCALE    0.015f  // Body segment render scale
#define WHIP_HEAD_SCALE    0.022f  // Snake head render scale
#define WHIP_TAIL_SCALE    0.018f  // Tail crystal render scale
#define WHIP_COIL_SEGMENTS 5       // Body segments in coiled state
#define WHIP_COIL_RADIUS   6.0f    // Coil spiral radius
#define WHIP_COIL_SCALE    0.010f  // Slightly smaller for coil

// =============================================================================
// SNAKE BODY SEGMENT — Hexagonal tube along Z axis
// =============================================================================
// 12 vertices: bottom ring (z=-500) and top ring (z=500), radius 300
// At scale 0.015: ~9 units diameter, ~15 units long
static Vtx sWhipBodyVtx[] = {
    // Bottom ring (z=-500)
    {{{ 300,    0, -500}, 0, {0, 0}, {127, 0, 0, 255}}},     // [0]
    {{{ 150,  260, -500}, 0, {0, 0}, {64, 110, 0, 255}}},    // [1]
    {{{-150,  260, -500}, 0, {0, 0}, {-64, 110, 0, 255}}},   // [2]
    {{{-300,    0, -500}, 0, {0, 0}, {-127, 0, 0, 255}}},    // [3]
    {{{-150, -260, -500}, 0, {0, 0}, {-64, -110, 0, 255}}},  // [4]
    {{{ 150, -260, -500}, 0, {0, 0}, {64, -110, 0, 255}}},   // [5]
    // Top ring (z=500)
    {{{ 300,    0,  500}, 0, {0, 0}, {127, 0, 0, 255}}},     // [6]
    {{{ 150,  260,  500}, 0, {0, 0}, {64, 110, 0, 255}}},    // [7]
    {{{-150,  260,  500}, 0, {0, 0}, {-64, 110, 0, 255}}},   // [8]
    {{{-300,    0,  500}, 0, {0, 0}, {-127, 0, 0, 255}}},    // [9]
    {{{-150, -260,  500}, 0, {0, 0}, {-64, -110, 0, 255}}},  // [10]
    {{{ 150, -260,  500}, 0, {0, 0}, {64, -110, 0, 255}}},   // [11]
};

static Gfx sWhipBodyDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_CULL_FRONT | G_TEXTURE_GEN),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsDPSetPrimColor(0, 0, 230, 90, 20, 255),  // Orange-red
    gsSPVertex(sWhipBodyVtx, 12, 0),
    // 6 side quads = 12 triangles
    gsSP2Triangles(0, 6, 7, 0, 0, 7, 1, 0),
    gsSP2Triangles(1, 7, 8, 0, 1, 8, 2, 0),
    gsSP2Triangles(2, 8, 9, 0, 2, 9, 3, 0),
    gsSP2Triangles(3, 9, 10, 0, 3, 10, 4, 0),
    gsSP2Triangles(4, 10, 11, 0, 4, 11, 5, 0),
    gsSP2Triangles(5, 11, 6, 0, 5, 6, 0, 0),
    gsSPEndDisplayList(),
};

// =============================================================================
// SNAKE HEAD — Diamond shape along Z axis (mouth at +Z)
// =============================================================================
// Front pyramid (mouth tip to widest base) + back pyramid (base to body connection)
// Plus small green eye triangles on upper front surface
static Vtx sWhipHeadVtx[] = {
    // [0] Mouth tip
    {{{   0,   20,  500}, 0, {0, 0}, {0, 0, 127, 255}}},
    // [1-4] Base rectangle (widest point at z=0)
    {{{-250,  120,    0}, 0, {0, 0}, {-90, 90, 0, 255}}},    // [1] top-left
    {{{ 250,  120,    0}, 0, {0, 0}, {90, 90, 0, 255}}},     // [2] top-right
    {{{ 250,  -80,    0}, 0, {0, 0}, {90, -60, 0, 255}}},    // [3] bottom-right
    {{{-250,  -80,    0}, 0, {0, 0}, {-90, -60, 0, 255}}},   // [4] bottom-left
    // [5] Back tip (connects to body)
    {{{   0,   10, -250}, 0, {0, 0}, {0, 0, -127, 255}}},
    // [6-11] Eye vertices (green triangles on upper front)
    {{{-100,  100,  300}, 0, {0, 0}, {0, 127, 0, 255}}},     // [6] left eye top
    {{{-140,   80,  270}, 0, {0, 0}, {0, 127, 0, 255}}},     // [7] left eye outer
    {{{ -80,   80,  270}, 0, {0, 0}, {0, 127, 0, 255}}},     // [8] left eye inner
    {{{ 100,  100,  300}, 0, {0, 0}, {0, 127, 0, 255}}},     // [9] right eye top
    {{{ 140,   80,  270}, 0, {0, 0}, {0, 127, 0, 255}}},     // [10] right eye outer
    {{{  80,   80,  270}, 0, {0, 0}, {0, 127, 0, 255}}},     // [11] right eye inner
};

static Gfx sWhipHeadDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_CULL_FRONT | G_TEXTURE_GEN),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    // Orange-red head body
    gsDPSetPrimColor(0, 0, 220, 80, 15, 255),
    gsSPVertex(sWhipHeadVtx, 12, 0),
    // Front pyramid (mouth)
    gsSP2Triangles(0, 2, 1, 0, 0, 3, 2, 0),
    gsSP2Triangles(0, 4, 3, 0, 0, 1, 4, 0),
    // Back pyramid (body connection)
    gsSP2Triangles(5, 1, 2, 0, 5, 2, 3, 0),
    gsSP2Triangles(5, 3, 4, 0, 5, 4, 1, 0),
    // Green eyes
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 0, 255, 0, 255),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSPEndDisplayList(),
};

// =============================================================================
// TAIL CRYSTAL — Purple octahedron
// =============================================================================
static Vtx sWhipTailVtx[] = {
    {{{   0,  250,    0}, 0, {0, 0}, {0, 127, 0, 255}}},     // [0] Top
    {{{ 150,    0,  150}, 0, {0, 0}, {90, 0, 90, 255}}},     // [1] Front-right
    {{{ 150,    0, -150}, 0, {0, 0}, {90, 0, -90, 255}}},    // [2] Back-right
    {{{-150,    0, -150}, 0, {0, 0}, {-90, 0, -90, 255}}},   // [3] Back-left
    {{{-150,    0,  150}, 0, {0, 0}, {-90, 0, 90, 255}}},    // [4] Front-left
    {{{   0, -180,    0}, 0, {0, 0}, {0, -127, 0, 255}}},    // [5] Bottom
};

static Gfx sWhipTailDL[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_CULL_FRONT | G_TEXTURE_GEN),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsDPSetPrimColor(0, 0, 130, 40, 180, 255),  // Purple
    gsSPVertex(sWhipTailVtx, 6, 0),
    // Top pyramid
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 3, 4, 0, 0, 4, 1, 0),
    // Bottom pyramid
    gsSP2Triangles(5, 2, 1, 0, 5, 3, 2, 0),
    gsSP2Triangles(5, 4, 3, 0, 5, 1, 4, 0),
    gsSPEndDisplayList(),
};

// =============================================================================
// Draw snake body segments between two points
// =============================================================================
static void Whip_DrawSnakeBody(PlayState* play, Vec3f* start, Vec3f* end, f32 sag) {
    f32 dx = end->x - start->x;
    f32 dy = end->y - start->y;
    f32 dz = end->z - start->z;
    f32 dist = sqrtf(dx * dx + dy * dy + dz * dz);
    s32 segCount = (s32)(dist / WHIP_BODY_SEGMENT);
    s32 i;
    f32 yaw, pitch;

    OPEN_DISPS(play->state.gfxCtx);

    if (segCount < 1) segCount = 1;
    if (segCount > WHIP_BODY_MAX_SEGS) segCount = WHIP_BODY_MAX_SEGS;

    yaw = Math_FAtan2F(dx, dz);
    pitch = Math_FAtan2F(-dy, sqrtf(dx * dx + dz * dz));

    for (i = 0; i <= segCount; i++) {
        f32 t = (f32)i / (f32)segCount;
        Vec3f segPos;
        f32 sagOffset = sag * 4.0f * t * (1.0f - t);

        segPos.x = start->x + dx * t;
        segPos.y = start->y + dy * t - sagOffset;
        segPos.z = start->z + dz * t;

        Matrix_Translate(segPos.x, segPos.y, segPos.z, MTXMODE_NEW);
        Matrix_RotateY(yaw, MTXMODE_APPLY);
        Matrix_RotateX(pitch, MTXMODE_APPLY);
        Matrix_Scale(WHIP_BODY_SCALE, WHIP_BODY_SCALE, WHIP_BODY_SCALE, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, sWhipBodyDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Draw snake head at position, facing away from fromPos
// =============================================================================
static void Whip_DrawSnakeHead(PlayState* play, Vec3f* headPos, Vec3f* fromPos) {
    f32 dx = headPos->x - fromPos->x;
    f32 dy = headPos->y - fromPos->y;
    f32 dz = headPos->z - fromPos->z;
    f32 yaw, pitch;

    OPEN_DISPS(play->state.gfxCtx);

    yaw = Math_FAtan2F(dx, dz);
    pitch = Math_FAtan2F(-dy, sqrtf(dx * dx + dz * dz));

    Matrix_Translate(headPos->x, headPos->y, headPos->z, MTXMODE_NEW);
    Matrix_RotateY(yaw, MTXMODE_APPLY);
    Matrix_RotateX(pitch, MTXMODE_APPLY);
    Matrix_Scale(WHIP_HEAD_SCALE, WHIP_HEAD_SCALE, WHIP_HEAD_SCALE, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sWhipHeadDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Draw snake head at surface (attached/swinging — mouth bites into surface)
// =============================================================================
static void Whip_DrawSnakeHeadAtSurface(PlayState* play, Vec3f* pos, Vec3f* normal) {
    f32 normalYaw, normalPitch;

    OPEN_DISPS(play->state.gfxCtx);

    normalYaw = Math_FAtan2F(normal->x, normal->z);
    normalPitch = Math_FAtan2F(-normal->y, sqrtf(normal->x * normal->x + normal->z * normal->z));

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_RotateY(normalYaw, MTXMODE_APPLY);
    Matrix_RotateX(normalPitch, MTXMODE_APPLY);
    Matrix_Scale(WHIP_HEAD_SCALE, WHIP_HEAD_SCALE, WHIP_HEAD_SCALE, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sWhipHeadDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Draw purple tail crystal at position
// =============================================================================
static void Whip_DrawTailCrystal(PlayState* play, Vec3f* pos) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_Scale(WHIP_TAIL_SCALE, WHIP_TAIL_SCALE, WHIP_TAIL_SCALE, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sWhipTailDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Draw coiled snake near hand (equip state)
// =============================================================================
static void Whip_DrawCoiledSnake(PlayState* play, Vec3f* handPos, Player* player) {
    s32 i;
    f32 angle, tangentYaw;
    Vec3f segPos, headPos;

    OPEN_DISPS(play->state.gfxCtx);

    // Body segments in a helical coil
    for (i = 0; i < WHIP_COIL_SEGMENTS; i++) {
        angle = (f32)i * (2.0f * M_PI / (f32)WHIP_COIL_SEGMENTS);

        segPos.x = handPos->x + cosf(angle) * WHIP_COIL_RADIUS;
        segPos.y = handPos->y + (f32)i * 2.0f;
        segPos.z = handPos->z + sinf(angle) * WHIP_COIL_RADIUS;

        // Each segment tangent to the coil circle
        tangentYaw = angle + (M_PI / 2.0f);

        Matrix_Translate(segPos.x, segPos.y, segPos.z, MTXMODE_NEW);
        Matrix_RotateY(tangentYaw, MTXMODE_APPLY);
        Matrix_Scale(WHIP_COIL_SCALE, WHIP_COIL_SCALE, WHIP_COIL_SCALE, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, sWhipBodyDL);
    }

    // Snake head on top of coil, facing player direction
    headPos.x = handPos->x;
    headPos.y = handPos->y + (f32)WHIP_COIL_SEGMENTS * 2.0f + 5.0f;
    headPos.z = handPos->z;

    tangentYaw = (f32)player->actor.shape.rot.y * (M_PI / 32768.0f);

    Matrix_Translate(headPos.x, headPos.y, headPos.z, MTXMODE_NEW);
    Matrix_RotateY(tangentYaw, MTXMODE_APPLY);
    Matrix_Scale(WHIP_HEAD_SCALE * 0.7f, WHIP_HEAD_SCALE * 0.7f, WHIP_HEAD_SCALE * 0.7f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sWhipHeadDL);

    // Purple tail crystal at the base of the coil
    Matrix_Translate(handPos->x, handPos->y - 3.0f, handPos->z, MTXMODE_NEW);
    Matrix_Scale(WHIP_TAIL_SCALE * 0.6f, WHIP_TAIL_SCALE * 0.6f, WHIP_TAIL_SCALE * 0.6f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sWhipTailDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

// =============================================================================
// Main Draw Function
// =============================================================================
void CustomItems_DrawWhip(Player* player, PlayState* play) {
    Vec3f handPos;
    u8 state;

    if (!gCustomItemState.whipActive) return;

    handPos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];
    state = gCustomItemState.whipState;

    switch (state) {
        case WHIP_STATE_EQUIP:
            Whip_DrawCoiledSnake(play, &handPos, player);
            break;

        case WHIP_STATE_EXTENDING:
        case WHIP_STATE_RETRACTING:
        case WHIP_STATE_HIT_ENEMY:
            Whip_DrawTailCrystal(play, &handPos);
            Whip_DrawSnakeBody(play, &handPos, &gCustomItemState.whipTipPos, 0.0f);
            Whip_DrawSnakeHead(play, &gCustomItemState.whipTipPos, &handPos);
            break;

        case WHIP_STATE_ATTACHED:
        case WHIP_STATE_SWINGING:
            Whip_DrawTailCrystal(play, &handPos);
            Whip_DrawSnakeBody(play, &handPos, &gCustomItemState.whipAttachPos, 15.0f);
            Whip_DrawSnakeHeadAtSurface(play, &gCustomItemState.whipAttachPos,
                                         &gCustomItemState.whipAttachNormal);
            break;

        default:
            break;
    }
}
