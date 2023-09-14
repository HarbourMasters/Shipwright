#include "colViewer.h"
#include "../../frame_interpolation.h"
#include "../../UIWidgets.hpp"

#include <vector>
#include <string>
#include <cmath>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}

enum class ColRenderSetting { Disabled, Solid, Transparent };

static const char* ColRenderSettingNames[] = {
    "Disabled",
    "Solid",
    "Transparent",
};

ImVec4 scene_col;
ImVec4 hookshot_col;
ImVec4 entrance_col;
ImVec4 specialSurface_col;
ImVec4 interactable_col;
ImVec4 slope_col;
ImVec4 void_col;

ImVec4 oc_col;
ImVec4 ac_col;
ImVec4 at_col;

ImVec4 waterbox_col;

static std::vector<Gfx> opaDl;
static std::vector<Gfx> xluDl;
static std::vector<Vtx> vtxDl;
static std::vector<Mtx> mtxDl;

// These DLs contain a cylinder/sphere model scaled to 128x (to have less error)
// The idea is to push a model view matrix, then draw the DL, to draw the shape somewhere with a certain size
static std::vector<Gfx> cylinderGfx;
static std::vector<Vtx> cylinderVtx;
static std::vector<Gfx> sphereGfx;
static std::vector<Vtx> sphereVtx;

// Draws the ImGui window for the collision viewer
void ColViewerWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Collision Viewer", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    UIWidgets::EnhancementCheckbox("Enabled", "gColViewerEnabled");

    UIWidgets::LabeledRightAlignedEnhancementCombobox("Scene", "gColViewerScene", ColRenderSettingNames, COLVIEW_DISABLED);
    UIWidgets::LabeledRightAlignedEnhancementCombobox("Bg Actors", "gColViewerBgActors", ColRenderSettingNames, COLVIEW_DISABLED);
    UIWidgets::LabeledRightAlignedEnhancementCombobox("Col Check", "gColViewerColCheck", ColRenderSettingNames, COLVIEW_DISABLED);
    UIWidgets::LabeledRightAlignedEnhancementCombobox("Waterbox", "gColViewerWaterbox", ColRenderSettingNames, COLVIEW_DISABLED);

    UIWidgets::EnhancementCheckbox("Apply as decal", "gColViewerDecal");
    UIWidgets::InsertHelpHoverText("Applies the collision as a decal display. This can be useful if there is z-fighting occuring "
                        "with the scene geometry, but can cause other artifacts.");
    UIWidgets::EnhancementCheckbox("Shaded", "gColViewerShaded");
    UIWidgets::InsertHelpHoverText("Applies the scene's shading to the collision display.");

    // This has to be duplicated in both code paths due to the nature of ImGui::IsItemHovered()
    const std::string colorHelpText = "View and change the colors used for collision display.";
    if (ImGui::TreeNode("Colors")) {
        UIWidgets::InsertHelpHoverText(colorHelpText);

        UIWidgets::EnhancementColor("Normal", "gColViewerColorNormal", scene_col, ImVec4(255, 255, 255, 255), false);
        UIWidgets::EnhancementColor("Hookshot", "gColViewerColorHookshot", hookshot_col, ImVec4(128, 128, 255, 255),
                                   false);
        UIWidgets::EnhancementColor("Entrance", "gColViewerColorEntrance", entrance_col, ImVec4(0, 255, 0, 255), false);
        UIWidgets::EnhancementColor("Special Surface (Grass/Sand/Etc)", "gColViewerColorSpecialSurface",
                                   specialSurface_col, ImVec4(192, 255, 192, 255), false);
        UIWidgets::EnhancementColor("Interactable (Vines/Crawlspace/Etc)", "gColViewerColorInteractable",
                                   interactable_col, ImVec4(192, 0, 192, 255), false);
        UIWidgets::EnhancementColor("Slope", "gColViewerColorSlope", slope_col, ImVec4(255, 255, 128, 255), false);
        UIWidgets::EnhancementColor("Void", "gColViewerColorVoid", void_col, ImVec4(255, 0, 0, 255), false);
        UIWidgets::EnhancementColor("OC", "gColViewerColorOC", oc_col, ImVec4(255, 255, 255, 255), false);
        UIWidgets::EnhancementColor("AC", "gColViewerColorAC", ac_col, ImVec4(0, 0, 255, 255), false);
        UIWidgets::EnhancementColor("AT", "gColViewerColorAT", at_col, ImVec4(255, 0, 0, 255), false);
        UIWidgets::EnhancementColor("Waterbox", "gColViewerColorWaterbox", waterbox_col, ImVec4(0, 0, 255, 255), false);

        ImGui::TreePop();
    } else {
        UIWidgets::InsertHelpHoverText(colorHelpText);
    }

    ImGui::End();
}

// Calculates the normal for a triangle at the 3 specified points
void CalcTriNorm(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, Vec3f& norm) {
    norm.x = (v2.y - v1.y) * (v3.z - v1.z) - (v2.z - v1.z) * (v3.y - v1.y);
    norm.y = (v2.z - v1.z) * (v3.x - v1.x) - (v2.x - v1.x) * (v3.z - v1.z);
    norm.z = (v2.x - v1.x) * (v3.y - v1.y) - (v2.y - v1.y) * (v3.x - v1.x);
    float norm_d = sqrtf(norm.x * norm.x + norm.y * norm.y + norm.z * norm.z);
    if (norm_d != 0.f) {
        norm.x *= 127.f / norm_d;
        norm.y *= 127.f / norm_d;
        norm.z *= 127.f / norm_d;
    }
}

// Various macros used for creating verticies and rendering that aren't in gbi.h
#define G_CC_MODULATERGB_PRIM_ENVA PRIMITIVE, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT
#define G_CC_PRIMITIVE_ENVA 0, 0, 0, PRIMITIVE, 0, 0, 0, ENVIRONMENT
#define qs105(n) ((int16_t)((n)*0x0020))
#define gdSPDefVtxN(x, y, z, s, t, nx, ny, nz, ca)                                            \
    {                                                                                         \
        .n = {.ob = { x, y, z }, .tc = { qs105(s), qs105(t) }, .n = { nx, ny, nz }, .a = ca } \
    }

void CreateCylinderData() {
    constexpr int32_t CYL_DIVS = 12;
    cylinderGfx.reserve(5 + CYL_DIVS * 2);
    cylinderVtx.reserve(2 + CYL_DIVS * 2);

    cylinderVtx.push_back(gdSPDefVtxN(0, 0, 0, 0, 0, 0, -127, 0, 0xFF));  // Bottom center vertex
    cylinderVtx.push_back(gdSPDefVtxN(0, 128, 0, 0, 0, 0, 127, 0, 0xFF)); // Top center vertex
    // Create two rings of vertices
    for (int i = 0; i < CYL_DIVS; ++i) {
        short vtx_x = floorf(0.5f + cosf(2.f * M_PI * i / CYL_DIVS) * 128.f);
        short vtx_z = floorf(0.5f - sinf(2.f * M_PI * i / CYL_DIVS) * 128.f);
        signed char norm_x = cosf(2.f * M_PI * i / CYL_DIVS) * 127.f;
        signed char norm_z = -sinf(2.f * M_PI * i / CYL_DIVS) * 127.f;
        cylinderVtx.push_back(gdSPDefVtxN(vtx_x, 0, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF));
        cylinderVtx.push_back(gdSPDefVtxN(vtx_x, 128, vtx_z, 0, 0, norm_x, 0, norm_z, 0xFF));
    }

    // Draw edges
    cylinderGfx.push_back(gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH));
    cylinderGfx.push_back(gsSPVertex((uintptr_t)cylinderVtx.data(), 2 + CYL_DIVS * 2, 0));
    for (int i = 0; i < CYL_DIVS; ++i) {
        int p = (i + CYL_DIVS - 1) % CYL_DIVS;
        int v[4] = {
            2 + p * 2 + 0,
            2 + i * 2 + 0,
            2 + i * 2 + 1,
            2 + p * 2 + 1,
        };
        cylinderGfx.push_back(gsSP2Triangles(v[0], v[1], v[2], 0, v[0], v[2], v[3], 0));
    }

    // Draw top & bottom
    cylinderGfx.push_back(gsSPClearGeometryMode(G_SHADING_SMOOTH));
    for (int i = 0; i < CYL_DIVS; ++i) {
        int p = (i + CYL_DIVS - 1) % CYL_DIVS;
        int v[4] = {
            2 + p * 2 + 0,
            2 + i * 2 + 0,
            2 + i * 2 + 1,
            2 + p * 2 + 1,
        };
        cylinderGfx.push_back(gsSP2Triangles(0, v[1], v[0], 0, 1, v[3], v[2], 0));
    }

    cylinderGfx.push_back(gsSPClearGeometryMode(G_CULL_BACK));
    cylinderGfx.push_back(gsSPEndDisplayList());
}

// This subdivides a face into four tris by placing new verticies at the midpoints of the sides (Like a triforce!), then
// blowing up the verticies so they are on the unit sphere
void CreateSphereFace(std::vector<std::tuple<size_t, size_t, size_t>>& faces, int32_t v0Index, int32_t v1Index,
                      int32_t v2Index) {
    size_t nextIndex = sphereVtx.size();

    size_t v01Index = nextIndex;
    size_t v12Index = nextIndex + 1;
    size_t v20Index = nextIndex + 2;

    faces.emplace_back(v0Index, v01Index, v20Index);
    faces.emplace_back(v1Index, v12Index, v01Index);
    faces.emplace_back(v2Index, v20Index, v12Index);
    faces.emplace_back(v01Index, v12Index, v20Index);

    const Vtx& v0 = sphereVtx[v0Index];
    const Vtx& v1 = sphereVtx[v1Index];
    const Vtx& v2 = sphereVtx[v2Index];

    // Create 3 new verticies at the midpoints
    Vec3f vs[3] = {
        Vec3f{(v0.n.ob[0] + v1.n.ob[0]) / 2.0f, (v0.n.ob[1] + v1.n.ob[1]) / 2.0f, (v0.n.ob[2] + v1.n.ob[2]) / 2.0f},
        Vec3f{(v1.n.ob[0] + v2.n.ob[0]) / 2.0f, (v1.n.ob[1] + v2.n.ob[1]) / 2.0f, (v1.n.ob[2] + v2.n.ob[2]) / 2.0f},
        Vec3f{(v2.n.ob[0] + v0.n.ob[0]) / 2.0f, (v2.n.ob[1] + v0.n.ob[1]) / 2.0f, (v2.n.ob[2] + v0.n.ob[2]) / 2.0f}
    };

    // Normalize vertex positions so they are on the sphere
    for (int32_t vAddIndex = 0; vAddIndex < 3; vAddIndex++) {
        Vec3f& v = vs[vAddIndex];
        float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        v.x /= mag;
        v.y /= mag;
        v.z /= mag;
        sphereVtx.push_back(gdSPDefVtxN((short)(v.x * 127), (short)(v.y * 127), (short)(v.z * 127), 0, 0,
                                        (signed char)(v.x * 127), (signed char)(v.y * 127), (signed char)(v.z * 127),
                                        0xFF));
    }
}

// Creates a sphere following the idea in here:
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html Spcifically, create a icosahedron by
// realizing that the points can be placed on 3 rectangles that are on each unit plane. Then, subdividing each face.
void CreateSphereData() {
    std::vector<Vec3f> base;

    float d = (1.0f + sqrtf(5.0f)) / 2.0f;

    // Create the 12 starting verticies, 4 on each rectangle
    base.emplace_back(Vec3f({-1, d, 0}));
    base.emplace_back(Vec3f({1, d, 0}));
    base.emplace_back(Vec3f({-1, -d, 0}));
    base.emplace_back(Vec3f({1, -d, 0}));

    base.emplace_back(Vec3f({0, -1, d}));
    base.emplace_back(Vec3f({0, 1, d}));
    base.emplace_back(Vec3f({0, -1, -d}));
    base.emplace_back(Vec3f({0, 1, -d}));

    base.emplace_back(Vec3f({d, 0, -1}));
    base.emplace_back(Vec3f({d, 0, 1}));
    base.emplace_back(Vec3f({-d, 0, -1}));
    base.emplace_back(Vec3f({-d, 0, 1}));

    // Normalize verticies so they are on the unit sphere
    for (Vec3f& v : base) {
        float mag = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        v.x /= mag;
        v.y /= mag;
        v.z /= mag;
        sphereVtx.push_back(gdSPDefVtxN((short)(v.x * 128), (short)(v.y * 128), (short)(v.z * 128), 0, 0,
                                        (signed char)(v.x * 127), (signed char)(v.y * 127), (signed char)(v.z * 127),
                                        0xFF));
    }

    std::vector<std::tuple<size_t, size_t, size_t>> faces;

    // Subdivide faces
    CreateSphereFace(faces, 0, 11, 5);
    CreateSphereFace(faces, 0, 5, 1);
    CreateSphereFace(faces, 0, 1, 7);
    CreateSphereFace(faces, 0, 7, 10);
    CreateSphereFace(faces, 0, 10, 11);

    CreateSphereFace(faces, 1, 5, 9);
    CreateSphereFace(faces, 5, 11, 4);
    CreateSphereFace(faces, 11, 10, 2);
    CreateSphereFace(faces, 10, 7, 6);
    CreateSphereFace(faces, 7, 1, 8);

    CreateSphereFace(faces, 3, 9, 4);
    CreateSphereFace(faces, 3, 4, 2);
    CreateSphereFace(faces, 3, 2, 6);
    CreateSphereFace(faces, 3, 6, 8);
    CreateSphereFace(faces, 3, 8, 9);

    CreateSphereFace(faces, 4, 9, 5);
    CreateSphereFace(faces, 2, 4, 11);
    CreateSphereFace(faces, 6, 2, 10);
    CreateSphereFace(faces, 8, 6, 7);
    CreateSphereFace(faces, 9, 8, 1);

    size_t vtxStartIndex = sphereVtx.size();
    sphereVtx.reserve(sphereVtx.size() + faces.size() * 3);
    for (int32_t faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
        sphereVtx.push_back(sphereVtx[std::get<0>(faces[faceIndex])]);
        sphereVtx.push_back(sphereVtx[std::get<1>(faces[faceIndex])]);
        sphereVtx.push_back(sphereVtx[std::get<2>(faces[faceIndex])]);
        sphereGfx.push_back(gsSPVertex((uintptr_t)(sphereVtx.data() + vtxStartIndex + faceIndex * 3), 3, 0));
        sphereGfx.push_back(gsSP1Triangle(0, 1, 2, 0));
    }

    sphereGfx.push_back(gsSPEndDisplayList());
}

void ColViewerWindow::InitElement() {
    CreateCylinderData();
    CreateSphereData();
}

// Initializes the display list for a ColRenderSetting
void InitGfx(std::vector<Gfx>& gfx, ColRenderSetting setting) {
    uint32_t rm;
    uint32_t blc1;
    uint32_t blc2;
    uint8_t alpha;
    uint64_t cm;
    uint32_t gm;

    if (setting == ColRenderSetting::Transparent) {
        rm = Z_CMP | IM_RD | CVG_DST_FULL | FORCE_BL;
        blc1 = GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
        blc2 = GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA);
        alpha = 0x80;
    } else {
        rm = Z_CMP | Z_UPD | CVG_DST_CLAMP | FORCE_BL;
        blc1 = GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        blc2 = GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1);
        alpha = 0xFF;
    }

    if (CVarGetInteger("gColViewerDecal", 0) != 0) {
        rm |= ZMODE_DEC;
    } else if (setting == ColRenderSetting::Transparent) {
        rm |= ZMODE_XLU;
    } else {
        rm |= ZMODE_OPA;
    }

    gfx.push_back(gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF));
    gfx.push_back(gsDPSetCycleType(G_CYC_1CYCLE));
    gfx.push_back(gsDPSetRenderMode(rm | blc1, rm | blc2));

    if (CVarGetInteger("gColViewerShaded", 0) != 0) {
        gfx.push_back(gsDPSetCombineMode(G_CC_MODULATERGB_PRIM_ENVA, G_CC_MODULATERGB_PRIM_ENVA));
        gfx.push_back(gsSPLoadGeometryMode(G_CULL_BACK | G_ZBUFFER | G_LIGHTING));
    } else {
        gfx.push_back(gsDPSetCombineMode(G_CC_PRIMITIVE_ENVA, G_CC_PRIMITIVE_ENVA));
        gfx.push_back(gsSPLoadGeometryMode(G_ZBUFFER));
    }

    gfx.push_back(gsDPSetEnvColor(0xFF, 0xFF, 0xFF, alpha));
}

// Draws a dynapoly structure (scenes or Bg Actors)
void DrawDynapoly(std::vector<Gfx>& dl, CollisionHeader* col, int32_t bgId) {
    uint32_t colorR = CVarGetInteger("gColViewerColorNormalR", 255);
    uint32_t colorG = CVarGetInteger("gColViewerColorNormalG", 255);
    uint32_t colorB = CVarGetInteger("gColViewerColorNormalB", 255);
    uint32_t colorA = 255;

    uint32_t lastColorR = colorR;
    uint32_t lastColorG = colorG;
    uint32_t lastColorB = colorB;

    dl.push_back(gsDPSetPrimColor(0, 0, colorR, colorG, colorB, colorA));

    // This keeps track of if we have processed a poly, but not drawn it yet so we can batch them.
    // This saves several hundred commands in larger scenes
    bool previousPoly = false;

    for (int i = 0; i < col->numPolygons; i++) {
        CollisionPoly* poly = &col->polyList[i];

        if (SurfaceType_IsHookshotSurface(&gPlayState->colCtx, poly, bgId)) {
            colorR = CVarGetInteger("gColViewerColorHookshotR", 128);
            colorG = CVarGetInteger("gColViewerColorHookshotG", 128);
            colorB = CVarGetInteger("gColViewerColorHookshotB", 255);
        } else if (func_80041D94(&gPlayState->colCtx, poly, bgId) > 0x01) {
            colorR = CVarGetInteger("gColViewerColorInteractableR", 192);
            colorG = CVarGetInteger("gColViewerColorInteractableG", 0);
            colorB = CVarGetInteger("gColViewerColorInteractableB", 192);
        } else if (func_80041E80(&gPlayState->colCtx, poly, bgId) == 0x0C) {
            colorR = CVarGetInteger("gColViewerColorVoidR", 255);
            colorG = CVarGetInteger("gColViewerColorVoidG", 0);
            colorB = CVarGetInteger("gColViewerColorVoidB", 0);
        } else if (SurfaceType_GetSceneExitIndex(&gPlayState->colCtx, poly, bgId) ||
                   func_80041E80(&gPlayState->colCtx, poly, bgId) == 0x05) {
            colorR = CVarGetInteger("gColViewerColorEntranceR", 0);
            colorG = CVarGetInteger("gColViewerColorEntranceG", 255);
            colorB = CVarGetInteger("gColViewerColorEntranceB", 0);
        } else if (func_80041D4C(&gPlayState->colCtx, poly, bgId) != 0 ||
                   SurfaceType_IsWallDamage(&gPlayState->colCtx, poly, bgId)) {
            colorR = CVarGetInteger("gColViewerColorSpecialSurfaceR", 192);
            colorG = CVarGetInteger("gColViewerColorSpecialSurfaceG", 255);
            colorB = CVarGetInteger("gColViewerColorSpecialSurfaceB", 192);
        } else if (SurfaceType_GetSlope(&gPlayState->colCtx, poly, bgId) == 0x01) {
            colorR = CVarGetInteger("gColViewerColorSlopeR", 255);
            colorG = CVarGetInteger("gColViewerColorSlopeG", 255);
            colorB = CVarGetInteger("gColViewerColorSlopeB", 128);
        } else {
            colorR = CVarGetInteger("gColViewerColorNormalR", 255);
            colorG = CVarGetInteger("gColViewerColorNormalG", 255);
            colorB = CVarGetInteger("gColViewerColorNormalB", 255);
        }

        if (colorR != lastColorR || colorG != lastColorG || colorB != lastColorB) {
            // Color changed, flush previous poly
            if (previousPoly) {
                dl.push_back(gsSPVertex((uintptr_t)&vtxDl.at(vtxDl.size() - 3), 3, 0));
                dl.push_back(gsSP1Triangle(0, 1, 2, 0));
                previousPoly = false;
            }
            dl.push_back(gsDPSetPrimColor(0, 0, colorR, colorG, colorB, colorA));
        }
        lastColorR = colorR;
        lastColorG = colorG;
        lastColorB = colorB;

        Vec3s* va = &col->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIA)];
        Vec3s* vb = &col->vtxList[COLPOLY_VTX_INDEX(poly->flags_vIB)];
        Vec3s* vc = &col->vtxList[COLPOLY_VTX_INDEX(poly->vIC)];
        vtxDl.push_back(gdSPDefVtxN(va->x, va->y, va->z, 0, 0, (signed char)(poly->normal.x / 0x100),
                                    (signed char)(poly->normal.y / 0x100), (signed char)(poly->normal.z / 0x100),
                                    0xFF));
        vtxDl.push_back(gdSPDefVtxN(vb->x, vb->y, vb->z, 0, 0, (signed char)(poly->normal.x / 0x100),
                                    (signed char)(poly->normal.y / 0x100), (signed char)(poly->normal.z / 0x100),
                                    0xFF));
        vtxDl.push_back(gdSPDefVtxN(vc->x, vc->y, vc->z, 0, 0, (signed char)(poly->normal.x / 0x100),
                                    (signed char)(poly->normal.y / 0x100), (signed char)(poly->normal.z / 0x100),
                                    0xFF));

        if (previousPoly) {
            dl.push_back(gsSPVertex((uintptr_t)&vtxDl.at(vtxDl.size() - 6), 6, 0));
            dl.push_back(gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0));
            previousPoly = false;
        } else {
            previousPoly = true;
        }
    }

    // Flush previous poly if this is the end and there's no more coming
    if (previousPoly) {
        dl.push_back(gsSPVertex((uintptr_t)&vtxDl.at(vtxDl.size() - 3), 3, 0));
        dl.push_back(gsSP1Triangle(0, 1, 2, 0));
        previousPoly = false;
    }
}

// Draws the scene
void DrawSceneCollision() {
    ColRenderSetting showSceneColSetting = (ColRenderSetting)CVarGetInteger("gColViewerScene", COLVIEW_DISABLED);

    if (showSceneColSetting == ColRenderSetting::Disabled || !CVarGetInteger("gColViewerEnabled", 0)) {
        return;
    }

    std::vector<Gfx>& dl = (showSceneColSetting == ColRenderSetting::Transparent) ? xluDl : opaDl;
    InitGfx(dl, showSceneColSetting);
    dl.push_back(gsSPMatrix(&gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH));

    DrawDynapoly(dl, gPlayState->colCtx.colHeader, BGCHECK_SCENE);
}

// Draws all Bg Actors
void DrawBgActorCollision() {
    ColRenderSetting showBgActorSetting = (ColRenderSetting)CVarGetInteger("gColViewerBgActors", COLVIEW_DISABLED);
    if (showBgActorSetting == ColRenderSetting::Disabled || !CVarGetInteger("gColViewerEnabled", 0)) {
        return;
    }

    std::vector<Gfx>& dl = (showBgActorSetting == ColRenderSetting::Transparent) ? xluDl : opaDl;
    InitGfx(dl, showBgActorSetting);
    dl.push_back(gsSPMatrix(&gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH));

    for (int32_t bgIndex = 0; bgIndex < BG_ACTOR_MAX; bgIndex++) {
        if (gPlayState->colCtx.dyna.bgActorFlags[bgIndex] & 1) {
            BgActor& bg = gPlayState->colCtx.dyna.bgActors[bgIndex];
            Mtx m;
            MtxF mf;
            SkinMatrix_SetTranslateRotateYXZScale(&mf, bg.curTransform.scale.x, bg.curTransform.scale.y,
                                                  bg.curTransform.scale.z, bg.curTransform.rot.x, bg.curTransform.rot.y,
                                                  bg.curTransform.rot.z, bg.curTransform.pos.x, bg.curTransform.pos.y,
                                                  bg.curTransform.pos.z);
            guMtxF2L(&mf, &m);
            mtxDl.push_back(m);
            dl.push_back(gsSPMatrix(&mtxDl.back(), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH));

            DrawDynapoly(dl, bg.colHeader, bgIndex);

            dl.push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
        }
    }
}

// Draws a quad
void DrawQuad(std::vector<Gfx>& dl, Vec3f& v0, Vec3f& v1, Vec3f& v2, Vec3f& v3) {
    Vec3f norm;
    CalcTriNorm(v0, v1, v2, norm);

    vtxDl.push_back(gdSPDefVtxN((short)v0.x, (short)v0.y, (short)v0.z, 0, 0, (signed char)norm.x, (signed char)norm.y,
                                (signed char)norm.z, 0xFF));
    vtxDl.push_back(gdSPDefVtxN((short)v1.x, (short)v1.y, (short)v1.z, 0, 0, (signed char)norm.x, (signed char)norm.y,
                                (signed char)norm.z, 0xFF));
    vtxDl.push_back(gdSPDefVtxN((short)v2.x, (short)v2.y, (short)v2.z, 0, 0, (signed char)norm.x, (signed char)norm.y,
                                (signed char)norm.z, 0xFF));
    vtxDl.push_back(gdSPDefVtxN((short)v3.x, (short)v3.y, (short)v3.z, 0, 0, (signed char)norm.x, (signed char)norm.y,
                                (signed char)norm.z, 0xFF));
    dl.push_back(gsSPVertex((uintptr_t)&vtxDl.at(vtxDl.size() - 4), 4, 0));
    dl.push_back(gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0));
}

// Draws a list of Col Check objects
void DrawColCheckList(std::vector<Gfx>& dl, Collider** objects, int32_t count) {
    for (int32_t colIndex = 0; colIndex < count; colIndex++) {
        Collider* col = objects[colIndex];
        switch (col->shape) {
            case COLSHAPE_JNTSPH: {
                ColliderJntSph* jntSph = (ColliderJntSph*)col;

                for (int32_t sphereIndex = 0; sphereIndex < jntSph->count; sphereIndex++) {
                    ColliderJntSphElement* sph = &jntSph->elements[sphereIndex];

                    Mtx m;
                    MtxF mf;
                    SkinMatrix_SetTranslate(&mf, sph->dim.worldSphere.center.x, sph->dim.worldSphere.center.y,
                                            sph->dim.worldSphere.center.z);
                    MtxF ms;
                    int32_t radius = sph->dim.worldSphere.radius == 0 ? 1 : sph->dim.worldSphere.radius;
                    SkinMatrix_SetScale(&ms, radius / 128.0f, radius / 128.0f, radius / 128.0f);
                    MtxF dest;
                    SkinMatrix_MtxFMtxFMult(&mf, &ms, &dest);
                    guMtxF2L(&dest, &m);
                    mtxDl.push_back(m);

                    dl.push_back(gsSPMatrix(&mtxDl.back(), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH));
                    dl.push_back(gsSPDisplayList(sphereGfx.data()));
                    dl.push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
                }
            } break;
            case COLSHAPE_CYLINDER: {
                ColliderCylinder* cyl = (ColliderCylinder*)col;

                Mtx m;
                MtxF mt;
                SkinMatrix_SetTranslate(&mt, cyl->dim.pos.x, cyl->dim.pos.y + cyl->dim.yShift, cyl->dim.pos.z);
                MtxF ms;
                int32_t radius = cyl->dim.radius == 0 ? 1 : cyl->dim.radius;
                SkinMatrix_SetScale(&ms, radius / 128.0f, cyl->dim.height / 128.0f, radius / 128.0f);
                MtxF dest;
                SkinMatrix_MtxFMtxFMult(&mt, &ms, &dest);
                guMtxF2L(&dest, &m);
                mtxDl.push_back(m);

                dl.push_back(gsSPMatrix(&mtxDl.back(), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH));
                dl.push_back(gsSPDisplayList(cylinderGfx.data()));
                dl.push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
            } break;
            case COLSHAPE_TRIS: {
                ColliderTris* tris = (ColliderTris*)col;
                for (int32_t triIndex = 0; triIndex < tris->count; triIndex++) {
                    ColliderTrisElement* tri = &tris->elements[triIndex];

                    vtxDl.push_back(gdSPDefVtxN((short)tri->dim.vtx[0].x, (short)tri->dim.vtx[0].y,
                                                (short)tri->dim.vtx[0].z, 0, 0, (signed char)tri->dim.plane.normal.x,
                                                (signed char)tri->dim.plane.normal.y,
                                                (signed char)tri->dim.plane.normal.z, 0xFF));
                    vtxDl.push_back(gdSPDefVtxN((short)tri->dim.vtx[1].x, (short)tri->dim.vtx[1].y,
                                                (short)tri->dim.vtx[1].z, 0, 0, (signed char)tri->dim.plane.normal.x,
                                                (signed char)tri->dim.plane.normal.y,
                                                (signed char)tri->dim.plane.normal.z, 0xFF));
                    vtxDl.push_back(gdSPDefVtxN((short)tri->dim.vtx[2].x, (short)tri->dim.vtx[2].y,
                                                (short)tri->dim.vtx[2].z, 0, 0, (signed char)tri->dim.plane.normal.x,
                                                (signed char)tri->dim.plane.normal.y,
                                                (signed char)tri->dim.plane.normal.z, 0xFF));
                    dl.push_back(gsSPVertex((uintptr_t)&vtxDl.at(vtxDl.size() - 3), 3, 0));
                    dl.push_back(gsSP1Triangle(0, 1, 2, 0));
                }
            } break;
            case COLSHAPE_QUAD: {
                ColliderQuad* quad = (ColliderQuad*)col;
                DrawQuad(dl, quad->dim.quad[0], quad->dim.quad[2], quad->dim.quad[3], quad->dim.quad[1]);
            } break;
            default:
                break;
        }
    }
}

// Draws all Col Check objects
void DrawColCheckCollision() {
    ColRenderSetting showColCheckSetting = (ColRenderSetting)CVarGetInteger("gColViewerColCheck", COLVIEW_DISABLED);
    if (showColCheckSetting == ColRenderSetting::Disabled || !CVarGetInteger("gColViewerEnabled", 0)) {
        return;
    }

    std::vector<Gfx>& dl = (showColCheckSetting == ColRenderSetting::Transparent) ? xluDl : opaDl;
    InitGfx(dl, showColCheckSetting);
    dl.push_back(gsSPMatrix(&gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH));

    CollisionCheckContext& col = gPlayState->colChkCtx;

    dl.push_back(gsDPSetPrimColor(0, 0, CVarGetInteger("gColViewerColorOCR", 255), CVarGetInteger("gColViewerColorOCG", 255),
                                  CVarGetInteger("gColViewerColorOCB", 255), 255));
    DrawColCheckList(dl, col.colOC, col.colOCCount);
    dl.push_back(gsDPSetPrimColor(0, 0, CVarGetInteger("gColViewerColorACR", 0), CVarGetInteger("gColViewerColorACG", 0),
                                  CVarGetInteger("gColViewerColorACB", 255), 255));
    DrawColCheckList(dl, col.colAC, col.colACCount);
    dl.push_back(gsDPSetPrimColor(0, 0, CVarGetInteger("gColViewerColorATR", 255), CVarGetInteger("gColViewerColorATG", 0),
                                  CVarGetInteger("gColViewerColorATB", 0), 255));

    DrawColCheckList(dl, col.colAT, col.colATCount);
}

// Draws a waterbox
void DrawWaterbox(std::vector<Gfx>& dl, WaterBox* water, float water_max_depth = -4000.0f) {
    // Skip waterboxes that would be disabled in current room
    int32_t room = ((water->properties >> 13) & 0x3F);
    if ((room != gPlayState->roomCtx.curRoom.num) && (room != 0x3F)) {
        return;
    }

    Vec3f vtx[] = {
        { water->xMin, water->ySurface, water->zMin + water->zLength },
        { water->xMin + water->xLength, water->ySurface, water->zMin + water->zLength },
        { water->xMin + water->xLength, water->ySurface, water->zMin },
        { water->xMin, water->ySurface, water->zMin },
        { water->xMin, water_max_depth, water->zMin + water->zLength },
        { water->xMin + water->xLength, water_max_depth, water->zMin + water->zLength },
        { water->xMin + water->xLength, water_max_depth, water->zMin },
        { water->xMin, water_max_depth, water->zMin },
    };
    DrawQuad(dl, vtx[0], vtx[1], vtx[2], vtx[3]);
    DrawQuad(dl, vtx[0], vtx[3], vtx[7], vtx[4]);
    DrawQuad(dl, vtx[1], vtx[0], vtx[4], vtx[5]);
    DrawQuad(dl, vtx[2], vtx[1], vtx[5], vtx[6]);
    DrawQuad(dl, vtx[3], vtx[2], vtx[6], vtx[7]);
}

extern "C" WaterBox zdWaterBox;
extern "C" f32 zdWaterBoxMinY;

// Draws all waterboxes
void DrawWaterboxList() {
    ColRenderSetting showWaterboxSetting = (ColRenderSetting)CVarGetInteger("gColViewerWaterbox", COLVIEW_DISABLED);
    if (showWaterboxSetting == ColRenderSetting::Disabled || !CVarGetInteger("gColViewerEnabled", 0)) {
        return;
    }

    std::vector<Gfx>& dl = (showWaterboxSetting == ColRenderSetting::Transparent) ? xluDl : opaDl;
    InitGfx(dl, showWaterboxSetting);
    dl.push_back(gsSPMatrix(&gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH));

    dl.push_back(gsDPSetPrimColor(0, 0, CVarGetInteger("gColViewerColorWaterboxR", 0),
                                  CVarGetInteger("gColViewerColorWaterboxG", 0),
                                  CVarGetInteger("gColViewerColorWaterboxB", 255), 255));

    CollisionHeader* col = gPlayState->colCtx.colHeader;
    for (int32_t waterboxIndex = 0; waterboxIndex < col->numWaterBoxes; waterboxIndex++) {
        WaterBox* water = &col->waterBoxes[waterboxIndex];
        DrawWaterbox(dl, water);
    }

    // Zora's Domain has a special, hard-coded waterbox with a bottom so you can go under the waterfall
    if (gPlayState->sceneNum == SCENE_ZORAS_DOMAIN) {
        DrawWaterbox(dl, &zdWaterBox, zdWaterBoxMinY);
    }
}

// Resets a vector for the next frame and returns the capacity
template <typename T> size_t ResetVector(T& vec) {
    size_t oldSize = vec.size();
    vec.clear();
    // Reserve slightly more space than last frame to account for variance (such as different amounts of bg actors)
    vec.reserve(oldSize * 1.2);
    return vec.capacity();
}

extern "C" void DrawColViewer() {
    if (gPlayState == nullptr) {
        return;
    }

    ResetVector(opaDl);
    ResetVector(xluDl);
    size_t vtxDlCapacity = ResetVector(vtxDl);
    size_t mtxDlCapacity = ResetVector(mtxDl);

    DrawSceneCollision();
    DrawBgActorCollision();
    DrawColCheckCollision();
    DrawWaterboxList();

    // Check if we used up more space than we reserved. If so, redo the drawing with our new sizes.
    // This is because we resized the vectors while drawing, invalidating pointers to them.
    // This only matters for the Vtx and Mtx vectors.
    if ((vtxDl.size() > vtxDlCapacity) || (mtxDl.size() > mtxDlCapacity)) {
        ResetVector(opaDl);
        ResetVector(xluDl);
        vtxDlCapacity = ResetVector(vtxDl);
        mtxDlCapacity = ResetVector(mtxDl);

        DrawSceneCollision();
        DrawBgActorCollision();
        DrawColCheckCollision();
        DrawWaterboxList();
    }

    if ((vtxDl.size() > vtxDlCapacity) || (mtxDl.size() > mtxDlCapacity)) {
        // If the sizes somehow changed between the two draws, we can't continue because we may be using invalid data
        printf("Error drawing collision, vertex/matrix sizes didn't settle.\n");
        return;
    }

    OPEN_DISPS(gPlayState->state.gfxCtx);

    uint8_t mirroredWorld = CVarGetInteger("gMirroredWorld", 0);
    // Col viewer needs inverted culling in mirror mode for both OPA and XLU buffers
    if (mirroredWorld) {
        gSPSetExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
        gSPSetExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
    }

    opaDl.push_back(gsSPEndDisplayList());
    gSPDisplayList(POLY_OPA_DISP++, opaDl.data());

    xluDl.push_back(gsSPEndDisplayList());
    gSPDisplayList(POLY_XLU_DISP++, xluDl.data());

    if (mirroredWorld) {
        gSPClearExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
        gSPClearExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
    }

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}
