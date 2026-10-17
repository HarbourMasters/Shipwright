#include "CheckTrackerKnown.h"

#include <cmath>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <fast/lus_gbi.h>
#include <fast/resource/ResourceType.h>
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/type/Vertex.h>
#include <libultraship/bridge/consolevariablebridge.h>
#include <libultraship/bridge/resourcebridge.h>

#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SaveManager.h"
#include "soh/resource/type/Array.h"
#include "soh/resource/type/SohResourceType.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/static_data.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "objects/object_demo_kekkai/object_demo_kekkai.h"
#include "objects/object_haka_objects/object_haka_objects.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"
#include "objects/object_menkuri_objects/object_menkuri_objects.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"
#include "overlays/actors/ovl_En_Goroiwa/z_en_goroiwa.h"
#include "overlays/actors/ovl_En_Ishi/z_en_ishi.h"
#include "overlays/actors/ovl_En_Kusa/z_en_kusa.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
#include "overlays/actors/ovl_En_Wood02/z_en_wood02.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "overlays/actors/ovl_Item_Etcetera/z_item_etcetera.h"
#include "overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.h"
#include "overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.h"
#include "overlays/actors/ovl_Obj_Hana/z_obj_hana.h"
#include "overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
extern PlayState* gPlayState;
float OTRGetAspectRatio(void);
void EnGSwitch_RandomizerDraw(Actor* thisx, PlayState* play);
}

namespace CheckTracker {

// Consecutive gameplay frames an item must pass every test before it is marked.
static constexpr int kKnownFramesRequired = 8;
// Smallest projected item radius, in native 240-line screen lines, that counts as readable.
static constexpr float kKnownMinScreenLines = 4.0f;
// Most fog an item may be blended with, from 0 (clear) to 1 (only fog color), and still count as readable.
static constexpr float kKnownMaxFog = 0.5f;
// The four edge rays aim at this fraction of the item's radius from its center.
static constexpr float kKnownRayOffsetFraction = 0.8f;
// Edge rays that must be clear, alongside the center ray, for an item to count as seen.
static constexpr int kKnownMinEdgeRays = 2;
// Approximate drawn radius of a shuffled item model.
static constexpr float kKnownItemRadius = 15.0f;
// Heart containers draw larger than other items.
static constexpr float kKnownHeartContainerRadius = 25.0f;
// EnItem00 drops rest on the ground, so their center sits above the actor position.
static constexpr float kKnownGroundedYOffset = 15.0f;
// Webs a sight ray may pass through before it counts as blocked.
static constexpr int kKnownMaxWebHops = 4;
// How far past a web hit the ray is re-cast from.
static constexpr float kKnownWebStep = 2.0f;
// Nested display list calls followed before a fake wall model counts as undecodable.
static constexpr int kKnownMaxDListDepth = 16;
// Size of the RSP vertex buffer in libultraship's interpreter.
static constexpr int kKnownVertexSlots = 64;

struct KnownCandidate {
    RandomizerCheck rc = RC_UNKNOWN_CHECK;
    GetItemEntry drawnEntry = {};
    float radius = kKnownItemRadius;
    float yOffset = 0.0f;
};

// Keyed by check, not actor, so nothing dangles across frames and duplicate actors count once.
static std::unordered_map<RandomizerCheck, int> knownFrames;
static int16_t lastSceneNum = -1;

// Check types describe the vanilla item, not how it is shown, so only exclude the ones where a drawn model does not
// mean the player has seen the item. Identification already limits marking to actors that draw the real item.
static bool KnownTypeAllowed(RandomizerCheck rc) {
    switch (Rando::StaticData::GetLocation(rc)->GetRCType()) {
        case RCTYPE_SHOP:
        case RCTYPE_SCRUB:
        case RCTYPE_MERCHANT:
        case RCTYPE_CHEST_GAME:
            return false;
        default:
            return true;
    }
}

// Resolves an actor's check and the item entry its draw function renders.
static bool IdentifyCandidate(Actor* actor, KnownCandidate& candidate) {
    switch (actor->id) {
        case ACTOR_EN_ITEM00: {
            EnItem00* item00 = reinterpret_cast<EnItem00*>(actor);
            if (actor->params != ITEM00_SOH_DUMMY) {
                return false;
            }
            if (item00->randoCheck != RC_UNKNOWN_CHECK) {
                candidate.rc = item00->randoCheck;
            } else if (item00->randoInf != RAND_INF_MAX) {
                candidate.rc = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(item00->randoInf);
            } else {
                // Heart piece and small key drops only stamp the entry, not the check.
                candidate.rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum,
                                                                                    item00->ogParams);
            }
            candidate.drawnEntry = item00->itemEntry;
            candidate.yOffset = kKnownGroundedYOffset;
            break;
        }
        case ACTOR_EN_SI: {
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<EnSi*>(actor)->sohGetItemEntry;
            break;
        }
        case ACTOR_ITEM_B_HEART: {
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<ItemBHeart*>(actor)->sohItemEntry;
            candidate.radius = kKnownHeartContainerRadius;
            break;
        }
        case ACTOR_ITEM_ETCETERA: {
            // Excludes the chest-game rupees: they draw through the Lens of Truth.
            int32_t type = actor->params & 0xFF;
            if (type != ITEM_ETC_LETTER && type != ITEM_ETC_ARROW_FIRE) {
                return false;
            }
            candidate.rc =
                OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
            candidate.drawnEntry = reinterpret_cast<ItemEtcetera*>(actor)->sohItemEntry;
            break;
        }
        case ACTOR_EN_EX_ITEM: {
            // Only the prizes on display at the Bombchu Bowling counter; the other variants fly straight to Link.
            EnExItem* exItem = reinterpret_cast<EnExItem*>(actor);
            switch (exItem->type) {
                case EXITEM_BOMB_BAG_COUNTER:
                    candidate.rc = RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE;
                    break;
                case EXITEM_HEART_PIECE_COUNTER:
                    candidate.rc = RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE;
                    break;
                default:
                    return false;
            }
            candidate.drawnEntry = exItem->sohItemEntry;
            break;
        }
        case ACTOR_EN_G_SWITCH: {
            // The randomized draw is only installed on shuffled, uncollected silver rupees.
            EnGSwitch* silver = reinterpret_cast<EnGSwitch*>(actor);
            if (silver->type != ENGSWITCH_SILVER_RUPEE || actor->draw != EnGSwitch_RandomizerDraw) {
                return false;
            }
            candidate.rc = OTRGlobals::Instance->gRandomizer->IdentifySilver(gPlayState->sceneNum, actor->world.pos)
                               .randomizerCheck;
            if (candidate.rc == RC_UNKNOWN_CHECK) {
                return false;
            }
            candidate.drawnEntry = OTRGlobals::Instance->gRandoContext->GetFinalGIEntry(candidate.rc, true, GI_NONE);
            break;
        }
        default:
            return false;
    }
    return candidate.rc != RC_UNKNOWN_CHECK;
}

static bool PassesFilter(const KnownCandidate& candidate) {
    if (!KnownTypeAllowed(candidate.rc) ||
        OTRGlobals::Instance->gRandoContext->GetItemLocation(candidate.rc)->GetCheckStatus() != RCSHOW_UNCHECKED) {
        return false;
    }
    // Unobtainable items draw as a blue rupee; only mark when the drawn model matches the truth (disguises match
    // both sides).
    GetItemEntry truth = OTRGlobals::Instance->gRandoContext->GetFinalGIEntry(candidate.rc, false, GI_NONE);
    return candidate.drawnEntry.modIndex == truth.modIndex && candidate.drawnEntry.getItemId == truth.getItemId;
}

static bool InSight(Actor* actor, const KnownCandidate& candidate) {
    // projectedPos and projectedW are clip coordinates from Actor_DrawAll, earlier this frame.
    float w = actor->projectedW;
    if (w <= 0.0f) {
        return false;
    }
    // The renderer scales clip x by (4:3 / aspect), so a wider window shows more to each side.
    float xBound = w * OTRGetAspectRatio() / (4.0f / 3.0f);
    // Cutscene letterbox bars cover this many native lines at the top and at the bottom.
    float yBound = w * (1.0f - 2.0f * ShrinkWindow_GetCurrentVal() / SCREEN_HEIGHT);
    if (std::fabs(actor->projectedPos.x) > xBound || std::fabs(actor->projectedPos.y) > yBound) {
        return false;
    }
    float screenLines =
        candidate.radius / (w * std::tan(DEG_TO_RAD(gPlayState->view.fovy) / 2.0f)) * (SCREEN_HEIGHT / 2);
    if (screenLines < kKnownMinScreenLines) {
        return false;
    }
    // Play_SetFog passes (fogNear, 1000) to Gfx_SetFog: below 0 everything is fogged, from 1000 nothing is, and
    // otherwise gSPFogPosition ramps the fog from NDC depth (fogNear - 500) / 500 to full at NDC depth 1.
    s16 fogNear = gPlayState->lightCtx.fogNear;
    if (fogNear < 0) {
        return false;
    }
    if (fogNear < 1000 &&
        (500.0f * actor->projectedPos.z / w + 500.0f - fogNear) / (1000.0f - fogNear) > kKnownMaxFog) {
        return false;
    }
    return actor->room == gPlayState->roomCtx.curRoom.num || actor->room < 0;
}

// Occluders that register their OC collider only near Link, so the OC list misses them farther away.
static ColliderCylinder* GatedOccluderCollider(Actor* actor) {
    switch (actor->id) {
        case ACTOR_EN_KUSA:
            // A cut bush is a stump until it regrows.
            if (actor->flags & ACTOR_FLAG_GRASS_DESTROYED) {
                return nullptr;
            }
            return &reinterpret_cast<EnKusa*>(actor)->collider;
        case ACTOR_EN_ISHI:
            return &reinterpret_cast<EnIshi*>(actor)->collider;
        case ACTOR_OBJ_HANA:
            return &reinterpret_cast<ObjHana*>(actor)->collider;
        case ACTOR_EN_WOOD02:
            return &reinterpret_cast<EnWood02*>(actor)->collider;
        case ACTOR_OBJ_HAMISHI:
            return &reinterpret_cast<ObjHamishi*>(actor)->collider;
        case ACTOR_OBJ_KIBAKO:
            return &reinterpret_cast<ObjKibako*>(actor)->collider;
        case ACTOR_OBJ_BOMBIWA:
            return &reinterpret_cast<ObjBombiwa*>(actor)->collider;
        case ACTOR_OBJ_TSUBO:
            return &reinterpret_cast<ObjTsubo*>(actor)->collider;
        default:
            return nullptr;
    }
}

struct GatedOccluders {
    std::vector<Cylinder16> cylinders;
    std::vector<Sphere16> spheres;
};

// Built once per frame, so each ray only walks the occluders rather than every loaded actor.
static GatedOccluders CollectGatedOccluders() {
    GatedOccluders occluders;
    for (int i = 0; i < ACTORCAT_MAX; i++) {
        for (Actor* actor = gPlayState->actorCtx.actorLists[i].head; actor != nullptr; actor = actor->next) {
            // A killed container (the pot an item just dropped from) stays listed until the next update.
            if (actor->update == nullptr) {
                continue;
            }
            if (actor->id == ACTOR_EN_GOROIWA) {
                // Rolling boulders only refresh and register their sphere within 300 units, so rebuild it the way
                // EnGoroiwa_UpdateCollider does.
                Sphere16 sphere = reinterpret_cast<EnGoroiwa*>(actor)->collider.elements[0].dim.worldSphere;
                if (sphere.radius > 0) {
                    sphere.center.x = actor->world.pos.x;
                    sphere.center.y = actor->world.pos.y + (((actor->params >> 10) & 1) ? 59.5f : 0.0f);
                    sphere.center.z = actor->world.pos.z;
                    occluders.spheres.push_back(sphere);
                }
                continue;
            }
            ColliderCylinder* collider = GatedOccluderCollider(actor);
            // Some variants (flowers without collision, bush-type En_Wood02) never set up their collider.
            if (collider == nullptr || collider->base.actor == nullptr || collider->dim.radius <= 0 ||
                !(collider->base.ocFlags1 & OC1_ON) || !(collider->info.ocElemFlags & OCELEM_ON)) {
                continue;
            }
            // dim.pos is only refreshed inside the update distance, so rebuild it the way Collider_UpdateCylinder
            // does.
            Cylinder16 cylinder = collider->dim;
            cylinder.pos.x = actor->world.pos.x;
            cylinder.pos.y = actor->world.pos.y;
            cylinder.pos.z = actor->world.pos.z;
            occluders.cylinders.push_back(cylinder);
        }
    }
    return occluders;
}

static bool GatedOccluderHit(GatedOccluders& occluders, Vec3f& from, Vec3f& to) {
    for (Cylinder16& cylinder : occluders.cylinders) {
        Vec3f intersectA;
        Vec3f intersectB;
        if (Math3D_CylVsLineSeg(&cylinder, &from, &to, &intersectA, &intersectB) != 0) {
            return true;
        }
    }
    Linef line = { from, to };
    for (Sphere16& sphere : occluders.spheres) {
        if (Math3D_LineVsSph(&sphere, &line)) {
            return true;
        }
    }
    return false;
}

static Vec3f Cross(const Vec3f& a, const Vec3f& b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

static float Dot(const Vec3f& a, const Vec3f& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3f Sub(const Vec3f& a, const Vec3f& b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

static bool Normalize(Vec3f& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length < 0.001f) {
        return false;
    }
    v = { v.x / length, v.y / length, v.z / length };
    return true;
}

// Model-space triangles of a fake wall display list.
struct FakeWallMesh {
    bool valid = false;
    std::vector<Vec3f> corners; // three per triangle
    Vec3f min = { 0.0f, 0.0f, 0.0f };
    Vec3f max = { 0.0f, 0.0f, 0.0f };
};

// A fake wall drawn opaque this frame.
struct FakeWall {
    Actor* actor;
    const FakeWallMesh* mesh; // nullptr when its model could not be decoded
    MtxF worldToModel;
};

struct DListDecodeState {
    Vec3f slots[kKnownVertexSlots];
    bool loaded[kKnownVertexSlots] = {};
    FakeWallMesh& mesh;
};

// Keyed by display list path; node-based, so FakeWall::mesh stays valid while entries are added.
static std::unordered_map<std::string, FakeWallMesh> fakeWallMeshes;

template <class T>
static std::shared_ptr<T> AsResource(std::shared_ptr<Ship::IResource> resource, Fast::ResourceType type) {
    if (resource == nullptr || resource->GetInitData()->Type != static_cast<uint32_t>(type)) {
        return nullptr;
    }
    return std::static_pointer_cast<T>(resource);
}

struct VertexSpan {
    const Vtx* data = nullptr;
    size_t size = 0;
};

// Object archives store vertices as SoH Array resources; libultraship's own Vertex type is also accepted.
static VertexSpan GetVertices(std::shared_ptr<Ship::IResource> resource) {
    if (auto vertices = AsResource<Fast::Vertex>(resource, Fast::ResourceType::Vertex)) {
        return { (const Vtx*)vertices->VertexList.data(), vertices->VertexList.size() };
    }
    if (resource != nullptr && resource->GetInitData()->Type == static_cast<uint32_t>(SOH::ResourceType::SOH_Array)) {
        auto array = std::static_pointer_cast<SOH::Array>(resource);
        if (array->ArrayType == SOH::ArrayResourceType::Vertex) {
            return { (const Vtx*)array->Vertices.data(), array->Vertices.size() };
        }
    }
    return {};
}

static bool LoadVertices(const VertexSpan& vertices, size_t first, int count, int dest, DListDecodeState& state) {
    if (vertices.data == nullptr || count < 0 || dest < 0 || dest + count > kKnownVertexSlots ||
        first + count > vertices.size) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        const Vtx& vtx = vertices.data[first + i];
        state.slots[dest + i] = { (float)vtx.v.ob[0], (float)vtx.v.ob[1], (float)vtx.v.ob[2] };
        state.loaded[dest + i] = true;
    }
    return true;
}

static bool AddTriangle(DListDecodeState& state, uint32_t a, uint32_t b, uint32_t c) {
    for (uint32_t index : { a, b, c }) {
        if (index >= kKnownVertexSlots || !state.loaded[index]) {
            return false;
        }
        const Vec3f& corner = state.slots[index];
        FakeWallMesh& mesh = state.mesh;
        if (mesh.corners.empty()) {
            mesh.min = corner;
            mesh.max = corner;
        }
        mesh.min = { std::fmin(mesh.min.x, corner.x), std::fmin(mesh.min.y, corner.y),
                     std::fmin(mesh.min.z, corner.z) };
        mesh.max = { std::fmax(mesh.max.x, corner.x), std::fmax(mesh.max.y, corner.y),
                     std::fmax(mesh.max.z, corner.z) };
        mesh.corners.push_back(corner);
    }
    return true;
}

// Walks a display list the way libultraship's F3DEX2 interpreter does, keeping only vertex loads, triangles and
// calls. Segmented addresses cannot be resolved here, so a command that uses one fails the decode.
static bool DecodeDList(std::shared_ptr<Ship::IResource> resource, DListDecodeState& state, int depth) {
    auto dList = AsResource<Fast::DisplayList>(resource, Fast::ResourceType::DisplayList);
    if (dList == nullptr || dList->UCode != ucode_f3dex2 || depth > kKnownMaxDListDepth) {
        return false;
    }
    const std::vector<Gfx>& cmds = dList->Instructions;
    for (size_t i = 0; i < cmds.size(); i++) {
        uintptr_t w0 = cmds[i].words.w0;
        uintptr_t w1 = cmds[i].words.w1;
        uint8_t opcode = (w0 >> 24) & 0xFF;
        switch (opcode) {
            case G_VTX_OTR_HASH: {
                if (++i >= cmds.size()) {
                    return false;
                }
                uint64_t hash = ((uint64_t)cmds[i].words.w0 << 32) + cmds[i].words.w1;
                VertexSpan vertices = GetVertices(ResourceLoad(hash));
                if (vertices.data == nullptr) {
                    return false;
                }
                // After the first draw the interpreter overwrites the byte offset with the resolved pointer.
                uintptr_t offset = w1 > 0xFFFFF ? w1 - (uintptr_t)vertices.data : w1;
                if (offset % sizeof(Vtx) != 0) {
                    return false;
                }
                int count = (w0 >> 12) & 0xFF;
                if (!LoadVertices(vertices, offset / sizeof(Vtx), count, (int)((w0 >> 1) & 0x7F) - count, state)) {
                    return false;
                }
                break;
            }
            case G_VTX_OTR_FILEPATH: {
                if (++i >= cmds.size()) {
                    return false;
                }
                VertexSpan vertices = GetVertices(ResourceLoad((const char*)w1));
                uintptr_t layout = cmds[i].words.w1;
                if (!LoadVertices(vertices, layout & 0xFFFF, (int)cmds[i].words.w0, (int)(layout >> 16), state)) {
                    return false;
                }
                break;
            }
            case G_TRI1:
                if (!AddTriangle(state, ((w0 >> 16) & 0xFF) / 2, ((w0 >> 8) & 0xFF) / 2, (w0 & 0xFF) / 2)) {
                    return false;
                }
                break;
            case G_TRI1_OTR:
                if (!AddTriangle(state, w0 & 0xFFFF, (w1 >> 16) & 0xFFFF, w1 & 0xFFFF)) {
                    return false;
                }
                break;
            case G_TRI2:
                if (!AddTriangle(state, (w0 >> 17) & 0x7F, (w0 >> 9) & 0x7F, (w0 >> 1) & 0x7F) ||
                    !AddTriangle(state, (w1 >> 17) & 0x7F, (w1 >> 9) & 0x7F, (w1 >> 1) & 0x7F)) {
                    return false;
                }
                break;
            case G_QUAD:
                if (!AddTriangle(state, ((w0 >> 16) & 0xFF) / 2, ((w0 >> 8) & 0xFF) / 2, (w0 & 0xFF) / 2) ||
                    !AddTriangle(state, ((w1 >> 16) & 0xFF) / 2, ((w1 >> 8) & 0xFF) / 2, (w1 & 0xFF) / 2)) {
                    return false;
                }
                break;
            case G_DL_OTR_HASH:
            case G_DL_OTR_FILEPATH: {
                std::shared_ptr<Ship::IResource> child;
                if (opcode == G_DL_OTR_HASH) {
                    if (++i >= cmds.size()) {
                        return false;
                    }
                    child = ResourceLoad(((uint64_t)cmds[i].words.w0 << 32) + cmds[i].words.w1);
                } else {
                    child = ResourceLoad((const char*)w1);
                }
                if (!DecodeDList(child, state, depth + 1)) {
                    return false;
                }
                // A branch never returns to this list.
                if ((w0 >> 16) & 1) {
                    return true;
                }
                break;
            }
            case G_ENDDL:
                return true;
            case G_VTX:
            case G_DL:
            case G_DL_INDEX:
            // The decoder does not track matrices, so triangles after one would land in the wrong place.
            case G_MTX:
            case G_MTX_OTR:
            case G_POPMTX:
                return false;
            case G_SETTIMG_OTR_HASH:
            case G_BRANCH_Z_OTR:
            case G_MARKER:
            case G_MOVEMEM_OTR:
                // The second half of a 128-bit command.
                i++;
                break;
            default:
                break;
        }
    }
    return false;
}

static const FakeWallMesh* GetFakeWallMesh(const char* path) {
    auto it = fakeWallMeshes.find(path);
    if (it == fakeWallMeshes.end()) {
        it = fakeWallMeshes.emplace(path, FakeWallMesh()).first;
        DListDecodeState state = { .mesh = it->second };
        try {
            // A model with no triangles means ops this decoder skips, so it cannot be trusted to block anything.
            it->second.valid =
                DecodeDList(ResourceMgr_GetResourceByNameHandlingMQ(path), state, 0) && !it->second.corners.empty();
        } catch ([[maybe_unused]] const std::exception& e) { it->second.valid = false; }
    }
    return it->second.valid ? &it->second : nullptr;
}

// Looks up the display list a fake wall's draw function renders; path is nullptr for an unknown variant.
static bool FakeWallDList(Actor* actor, const char*& path) {
    static const char* const sGndNisekabeDLists[] = {
        gLightTrialFakeWallDL,
        gGanonsCastleUnusedFakeWallDL,
        gGanonsCastleScrubsFakeWallDL,
    };
    static const char* const sMenkuriNisekabeDLists[] = { gGTGFakeWallDL, gGTGFakeCeilingDL };
    // gBotwBloodSplatterDL, drawn on top of variant 0, is always translucent.
    static const char* const sHakaMeganeDLists[] = {
        gBotwFakeWallsAndFloorsDL,     gBotwThreeFakeFloorsDL,        gBotwHoleTrap2DL,
        object_haka_objects_DL_0040F0, object_haka_objects_DL_0043B0, object_haka_objects_DL_001120,
        object_haka_objects_DL_0045A0, object_haka_objects_DL_0047F0, object_haka_objects_DL_0018F0,
        object_haka_objects_DL_0049B0, object_haka_objects_DL_003CF0, object_haka_objects_DL_004B70,
        object_haka_objects_DL_002ED0,
    };
    const char* const* dLists;
    size_t dListCount;
    size_t index;
    switch (actor->id) {
        case ACTOR_BG_GND_NISEKABE:
            dLists = sGndNisekabeDLists;
            dListCount = ARRAY_COUNT(sGndNisekabeDLists);
            index = actor->params & 0xFF;
            break;
        case ACTOR_BG_MENKURI_NISEKABE:
            dLists = sMenkuriNisekabeDLists;
            dListCount = ARRAY_COUNT(sMenkuriNisekabeDLists);
            index = actor->params & 0xFF;
            break;
        case ACTOR_BG_HAKA_MEGANE:
            dLists = sHakaMeganeDLists;
            dListCount = ARRAY_COUNT(sHakaMeganeDLists);
            index = actor->params < 0 ? dListCount : actor->params;
            break;
        default:
            return false;
    }
    path = index < dListCount ? dLists[index] : nullptr;
    return true;
}

static std::vector<FakeWall> CollectFakeWalls() {
    std::vector<FakeWall> walls;
    for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head; actor != nullptr; actor = actor->next) {
        const char* path;
        // Actor_DrawAll only draws a REACT_TO_LENS actor in its own pass when it ends up drawing Opa; with the lens
        // up these draw Xlu from Actor_DrawLensActors and leave isDrawn false.
        if (!FakeWallDList(actor, path) || !actor->isDrawn || (actor->flags & ACTOR_FLAG_REACT_TO_LENS)) {
            continue;
        }
        FakeWall wall = { actor, path != nullptr ? GetFakeWallMesh(path) : nullptr, {} };
        // Same transform as Actor_Draw.
        MtxF modelToWorld;
        SkinMatrix_SetTranslateRotateYXZScale(
            &modelToWorld, actor->scale.x, actor->scale.y, actor->scale.z, actor->shape.rot.x, actor->shape.rot.y,
            actor->shape.rot.z, actor->world.pos.x, actor->world.pos.y + actor->shape.yOffset * actor->scale.y,
            actor->world.pos.z);
        if (SkinMatrix_Invert(&modelToWorld, &wall.worldToModel) != 0) {
            wall.mesh = nullptr;
        }
        walls.push_back(wall);
    }
    return walls;
}

static bool SegmentHitsBox(const Vec3f& start, const Vec3f& delta, const Vec3f& min, const Vec3f& max) {
    const float starts[] = { start.x, start.y, start.z };
    const float deltas[] = { delta.x, delta.y, delta.z };
    const float mins[] = { min.x, min.y, min.z };
    const float maxs[] = { max.x, max.y, max.z };
    float tMin = 0.0f;
    float tMax = 1.0f;
    for (int axis = 0; axis < 3; axis++) {
        if (std::fabs(deltas[axis]) < 1e-6f) {
            if (starts[axis] < mins[axis] || starts[axis] > maxs[axis]) {
                return false;
            }
            continue;
        }
        float t1 = (mins[axis] - starts[axis]) / deltas[axis];
        float t2 = (maxs[axis] - starts[axis]) / deltas[axis];
        tMin = std::fmax(tMin, std::fmin(t1, t2));
        tMax = std::fmin(tMax, std::fmax(t1, t2));
        if (tMin > tMax) {
            return false;
        }
    }
    return true;
}

// Moller-Trumbore, both faces.
static bool SegmentHitsTriangle(const Vec3f& start, const Vec3f& delta, const Vec3f* corners) {
    Vec3f edge1 = Sub(corners[1], corners[0]);
    Vec3f edge2 = Sub(corners[2], corners[0]);
    Vec3f p = Cross(delta, edge2);
    float det = Dot(edge1, p);
    if (std::fabs(det) < 1e-6f) {
        return false;
    }
    Vec3f s = Sub(start, corners[0]);
    float u = Dot(s, p) / det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    Vec3f q = Cross(s, edge1);
    float v = Dot(delta, q) / det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    float t = Dot(edge2, q) / det;
    return t >= 0.0f && t <= 1.0f;
}

static bool FakeWallHit(std::vector<FakeWall>& fakeWalls, const Vec3f& from, const Vec3f& to, Actor* item) {
    for (FakeWall& wall : fakeWalls) {
        if (wall.mesh == nullptr) {
            if (wall.actor->room < 0 || item->room < 0 || wall.actor->room == item->room) {
                return true;
            }
            continue;
        }
        Vec3f worldFrom = from;
        Vec3f worldTo = to;
        Vec3f start;
        Vec3f end;
        SkinMatrix_Vec3fMtxFMultXYZ(&wall.worldToModel, &worldFrom, &start);
        SkinMatrix_Vec3fMtxFMultXYZ(&wall.worldToModel, &worldTo, &end);
        Vec3f delta = Sub(end, start);
        if (!SegmentHitsBox(start, delta, wall.mesh->min, wall.mesh->max)) {
            continue;
        }
        for (size_t i = 0; i + 2 < wall.mesh->corners.size(); i += 3) {
            if (SegmentHitsTriangle(start, delta, &wall.mesh->corners[i])) {
                return true;
            }
        }
    }
    return false;
}

// Webs collide but can be seen through, so the scene ray is re-cast from just past each web it hits.
static bool SceneRayClear(const Vec3f& eye, const Vec3f& target) {
    Vec3f from = eye;
    Vec3f to = target;
    for (int hop = 0;; hop++) {
        Vec3f hitPos;
        CollisionPoly* poly;
        s32 bgId;
        // Both faces: a one-sided floor seen from below is still drawn solid by its room mesh.
        if (!BgCheck_AnyLineTest3(&gPlayState->colCtx, &from, &to, &hitPos, &poly, true, true, true, false, &bgId)) {
            return true;
        }
        DynaPolyActor* dyna = DynaPoly_GetActor(&gPlayState->colCtx, bgId);
        if (dyna == nullptr || dyna->actor.id != ACTOR_BG_YDAN_SP || hop >= kKnownMaxWebHops) {
            return false;
        }
        Vec3f remaining = Sub(to, hitPos);
        float length = std::sqrt(Dot(remaining, remaining));
        if (length <= kKnownWebStep) {
            return true;
        }
        float step = kKnownWebStep / length;
        from = { hitPos.x + remaining.x * step, hitPos.y + remaining.y * step, hitPos.z + remaining.z * step };
    }
}

static bool RayClear(const Vec3f& eye, const Vec3f& target, Actor* item, GatedOccluders& occluders,
                     std::vector<FakeWall>& fakeWalls) {
    if (!SceneRayClear(eye, target)) {
        return false;
    }
    Vec3f from = eye;
    Vec3f to = target;
    // Play_Update clears the OC list before Actor_UpdateAll, so it still holds this frame's colliders here.
    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return false;
    }
    Actor* exclusions[] = { item, &player->actor, player->naviActor };
    if (CollisionCheck_LineOCCheck(gPlayState, &gPlayState->colChkCtx, &from, &to, exclusions,
                                   ARRAY_COUNT(exclusions))) {
        return false;
    }
    if (GatedOccluderHit(occluders, from, to)) {
        return false;
    }
    return fakeWalls.empty() || !FakeWallHit(fakeWalls, from, to, item);
}

// Casts to the item center and four points around it, square to the view direction.
static bool RaysClear(Actor* actor, const KnownCandidate& candidate, GatedOccluders& occluders,
                      std::vector<FakeWall>& fakeWalls) {
    const View& view = gPlayState->view;
    Vec3f forward = { view.lookAt.x - view.eye.x, view.lookAt.y - view.eye.y, view.lookAt.z - view.eye.z };
    Vec3f right = Cross(forward, view.up);
    Vec3f up = Cross(right, forward);
    if (!Normalize(right) || !Normalize(up)) {
        return false;
    }
    Vec3f center = actor->world.pos;
    center.y += candidate.yOffset;
    float offset = candidate.radius * kKnownRayOffsetFraction;
    const Vec3f targets[] = {
        center,
        { center.x + right.x * offset, center.y + right.y * offset, center.z + right.z * offset },
        { center.x - right.x * offset, center.y - right.y * offset, center.z - right.z * offset },
        { center.x + up.x * offset, center.y + up.y * offset, center.z + up.z * offset },
        { center.x - up.x * offset, center.y - up.y * offset, center.z - up.z * offset },
    };
    // The center must be in view, plus enough of the outline that a wall or rim beside the item does not hide it.
    int edgesClear = 0;
    int edgesBlocked = 0;
    for (size_t i = 0; i < ARRAY_COUNT(targets); i++) {
        Vec3f target = targets[i];
        // Edge rays stop one radius short, so the floor or wall the item rests on does not hide it; the center ray
        // still has to reach the item.
        if (i > 0) {
            Vec3f toEye = Sub(view.eye, target);
            float length = std::sqrt(Dot(toEye, toEye));
            if (length > candidate.radius) {
                float step = candidate.radius / length;
                target = { target.x + toEye.x * step, target.y + toEye.y * step, target.z + toEye.z * step };
            }
        }
        if (RayClear(view.eye, target, actor, occluders, fakeWalls)) {
            if (i > 0 && ++edgesClear >= kKnownMinEdgeRays) {
                return true;
            }
        } else if (i == 0 || ++edgesBlocked > ARRAY_COUNT(targets) - 1 - kKnownMinEdgeRays) {
            return false;
        }
    }
    return false;
}

void CheckTrackerKnownFrame(int saveSectionId) {
    if (gPlayState == nullptr || !GameInteractor::IsSaveLoaded() || !IS_RANDO || IS_BOSS_RUSH ||
        !CVarGetInteger(CVAR_TRACKER_CHECK("KnownItems"), 0) ||
        CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        knownFrames.clear();
        return;
    }
    if (gPlayState->sceneNum != lastSceneNum) {
        lastSceneNum = gPlayState->sceneNum;
        knownFrames.clear();
    }
    // Play_Draw skips Actor_DrawAll while paused, so isDrawn and projectedPos are stale.
    if (gPlayState->pauseCtx.state != 0 || gDbgCamEnabled || CVarGetInteger(CVAR_CHEAT("NoClip"), 0)) {
        knownFrames.clear();
        return;
    }
    // Halted actors leave the OC list empty, and fades or a filled screen draw over the actors.
    if (gPlayState->haltAllActors || gPlayState->transitionTrigger != TRANS_TRIGGER_OFF ||
        gPlayState->transitionMode != TRANS_MODE_OFF || gPlayState->envCtx.fillScreen) {
        knownFrames.clear();
        return;
    }

    GatedOccluders occluders = CollectGatedOccluders();
    std::vector<FakeWall> fakeWalls = CollectFakeWalls();
    std::unordered_set<RandomizerCheck> passing;
    for (int i = 0; i < ACTORCAT_MAX; i++) {
        for (Actor* actor = gPlayState->actorCtx.actorLists[i].head; actor != nullptr; actor = actor->next) {
            if (!actor->isDrawn) {
                continue;
            }
            KnownCandidate candidate;
            if (!IdentifyCandidate(actor, candidate) || passing.contains(candidate.rc) || !PassesFilter(candidate) ||
                !InSight(actor, candidate) || !RaysClear(actor, candidate, occluders, fakeWalls)) {
                continue;
            }
            passing.insert(candidate.rc);
        }
    }

    // A check missing from this frame's passing set starts over.
    std::unordered_map<RandomizerCheck, int> counts;
    bool anyMarked = false;
    for (RandomizerCheck rc : passing) {
        auto it = knownFrames.find(rc);
        int frames = (it == knownFrames.end() ? 0 : it->second) + 1;
        if (frames >= kKnownFramesRequired) {
            OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)->SetCheckStatus(RCSHOW_KNOWN);
            anyMarked = true;
        } else {
            counts[rc] = frames;
        }
    }
    knownFrames.swap(counts);
    if (anyMarked) {
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, saveSectionId, true);
    }
}

void CheckTrackerKnownTeardown() {
    knownFrames.clear();
    lastSceneNum = -1;
    fakeWallMeshes.clear();
}

} // namespace CheckTracker
