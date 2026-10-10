#include "2s2h/ShipInit.hpp"

extern "C" {
#include "overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.h"
#include "overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
#include "overlays/actors/ovl_En_Ishi/z_en_ishi.h"
#include "overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
#include "overlays/actors/ovl_Obj_Flowerpot/z_obj_flowerpot.h"
#include "overlays/actors/ovl_Obj_Taru/z_obj_taru.h"
#include "overlays/actors/ovl_Obj_Snowball2/z_obj_snowball2.h"
#include "overlays/actors/ovl_En_Kusa/z_en_kusa.h"
#include "overlays/actors/ovl_En_Kusa2/z_en_kusa2.h"
#include "overlays/actors/ovl_Obj_Grass/z_obj_grass.h"
#include "overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

void ObjGrass_SpawnFragments(Vec3f* basePos, PlayState* play);
void ObjGrass_DropCollectible(ObjGrassElement* grassElem, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Gameplay.ExtendedProjectileInteractionDistance"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define REMOTE_GRASS_COLLIDER_COUNT 16

typedef struct {
    Vec3f pos;
    f32 radius;
} GrassThreat;

static ColliderCylinderInit sGrassCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0580C71C, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 44, 0, { 0, 0, 0 } },
};

static std::unordered_map<Actor*, std::vector<ObjGrassCollider>> sRemoteGrassCollidersMap;

static void DestroyRemoteGrassColliders(Actor* actor) {
    auto it = sRemoteGrassCollidersMap.find(actor);
    if (it != sRemoteGrassCollidersMap.end()) {
        if (gPlayState != nullptr) {
            for (auto& slot : it->second) {
                Collider_DestroyCylinder(gPlayState, &slot.collider);
            }
        }
        sRemoteGrassCollidersMap.erase(it);
    }
}

static void DestroyAllRemoteGrassColliders() {
    if (gPlayState == nullptr) {
        sRemoteGrassCollidersMap.clear();
        return;
    }
    for (auto& pair : sRemoteGrassCollidersMap) {
        for (auto& slot : pair.second) {
            Collider_DestroyCylinder(gPlayState, &slot.collider);
        }
    }
    sRemoteGrassCollidersMap.clear();
}

static std::vector<ObjGrassCollider>& GetOrInitRemoteGrassColliders(Actor* actor) {
    auto& remoteColliders = sRemoteGrassCollidersMap[actor];
    if (remoteColliders.empty()) {
        remoteColliders.resize(REMOTE_GRASS_COLLIDER_COUNT);
        for (auto& slot : remoteColliders) {
            Collider_InitCylinder(gPlayState, &slot.collider);
            Collider_SetCylinder(gPlayState, &slot.collider, actor, &sGrassCylinderInit);
            slot.entity = nullptr;
        }
    }
    return remoteColliders;
}

static bool IsWithinExtendedRange(Actor* actor, f32 vanillaRange) {
    if (gPlayState == NULL) {
        return false;
    }
    s32 multiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    if (multiplier <= 1) {
        return false;
    }
    f32 xzDist = actor->xzDistToPlayer;
    return xzDist >= vanillaRange && xzDist < (vanillaRange * multiplier);
}

static void RegisterObjGrassNearThreat(ObjGrass* grass, const GrassThreat* threat) {
    if (gPlayState == nullptr)
        return;

    auto it = sRemoteGrassCollidersMap.find((Actor*)grass);
    if (it == sRemoteGrassCollidersMap.end() || it->second.size() != REMOTE_GRASS_COLLIDER_COUNT) {
        return;
    }
    auto& remoteColliders = it->second;

    s32 multiplier = CVarGetInteger("gEnhancements.Graphics.IncreaseActorDrawDistance", 1);
    f32 vanillaRangeSq = SQ(650.0f);
    f32 extendedRangeSq = SQ(650.0f * multiplier);
    f32 threatRadiusSq = SQ(threat->radius);

    Vec3f& playerPos = GET_PLAYER(gPlayState)->actor.world.pos;

    // Find first available slot
    s32 slotIdx = -1;
    for (s32 i = 0; i < REMOTE_GRASS_COLLIDER_COUNT; i++) {
        if (remoteColliders[i].entity == nullptr) {
            slotIdx = i;
            break;
        }
    }
    if (slotIdx == -1)
        return;

    for (s32 i = 0; i < grass->activeGrassGroups; i++) {
        ObjGrassGroup* group = &grass->grassGroups[i];

        if (Math3D_Vec3fDistSq(&group->homePos, (Vec3f*)&threat->pos) > SQ(threat->radius + 250.0f))
            continue;

        for (s32 j = 0; j < group->count; j++) {
            ObjGrassElement* elem = &group->elements[j];

            if (!(elem->flags & OBJ_GRASS_ELEM_REMOVED) && !(elem->flags & OBJ_GRASS_ELEM_ANIM)) {
                f32 distToThreatSq = Math3D_Vec3fDistSq(&elem->pos, (Vec3f*)&threat->pos);

                if (distToThreatSq < threatRadiusSq) {
                    f32 distToPlayerSq = Math3D_Vec3fDistSq(&elem->pos, &playerPos);

                    if (distToPlayerSq >= vanillaRangeSq && distToPlayerSq < extendedRangeSq) {
                        ObjGrassCollider* slot = &remoteColliders[slotIdx];

                        slot->collider.dim.pos.x = elem->pos.x;
                        slot->collider.dim.pos.y = elem->pos.y;
                        slot->collider.dim.pos.z = elem->pos.z;
                        slot->entity = elem;
                        elem->flags |= OBJ_GRASS_ELEM_ANIM;

                        CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &slot->collider.base);

                        // Find next available slot
                        slotIdx = -1;
                        for (s32 k = 0; k < REMOTE_GRASS_COLLIDER_COUNT; k++) {
                            if (remoteColliders[k].entity == nullptr) {
                                slotIdx = k;
                                break;
                            }
                        }
                        if (slotIdx == -1)
                            return;
                    }
                }
            }
        }
    }
}

void RegisterExtendedProjectileInteractionDistance() {
    if (!CVAR) {
        DestroyAllRemoteGrassColliders();
    }

    static HOOK_ID remoteGrassDestroyHookId = 0;
    if (remoteGrassDestroyHookId == 0) {
        remoteGrassDestroyHookId = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorDestroy>(
            ACTOR_OBJ_GRASS, [](Actor* actor) { DestroyRemoteGrassColliders(actor); });
    }

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_BOMBIWA, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 1400.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_BOMBIWA, CVAR, [](Actor* actor) {
        ObjBombiwa* thisx = (ObjBombiwa*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 1400.0f)) {
            if (thisx->unk_202 > 0) {
                thisx->unk_202--;
                if (thisx->unk_202 == 0) {
                    thisx->collider.base.colMaterial = COL_MATERIAL_HARD;
                } else {
                    thisx->collider.base.colMaterial = COL_MATERIAL_NONE;
                }
            }
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_HAMISHI, CVAR, [](Actor* actor) {
        ObjHamishi* thisx = (ObjHamishi*)actor;
        if (IsWithinExtendedRange(actor, 1000.0f)) {
            if (thisx->unk_1A0 <= 0) {
                CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
            }
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_TSUBO, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_TSUBO, CVAR, [](Actor* actor) {
        ObjTsubo* thisx = (ObjTsubo*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f) &&
            gSaveContext.save.entrance != ENTRANCE(GORON_RACETRACK, 1)) {
            Collider_UpdateCylinder(&thisx->actor, &thisx->cylinderCollider);
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->cylinderCollider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_ISHI, CVAR, [](Actor* actor, bool* should) {
        EnIshi* thisx = (EnIshi*)actor;
        if ((actor->draw != NULL || (thisx->flags & ISHI_FLAG_CUTSCENE_ROCK)) && IsWithinExtendedRange(actor, 600.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_ISHI, CVAR, [](Actor* actor) {
        EnIshi* thisx = (EnIshi*)actor;
        s32 cutsceneRock = thisx->flags & ISHI_FLAG_CUTSCENE_ROCK;
        if (IsWithinExtendedRange(actor, 600.0f) && !cutsceneRock && actor->draw != NULL) {
            if (thisx->hitTimer > 0) {
                thisx->hitTimer--;
                if (thisx->hitTimer == 0) {
                    thisx->collider.base.colMaterial = COL_MATERIAL_HARD;
                } else {
                    thisx->collider.base.colMaterial = COL_MATERIAL_NONE;
                }
            }
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_KIBAKO, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_KIBAKO, CVAR, [](Actor* actor) {
        ObjKibako* thisx = (ObjKibako*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f)) {
            if (!KIBAKO_BOMBER_CAN_HIDE_IN_BOX(actor)) {
                Collider_UpdateCylinder(&thisx->actor, &thisx->collider);
            }
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_FLOWERPOT, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_FLOWERPOT, CVAR, [](Actor* actor) {
        ObjFlowerpot* thisx = (ObjFlowerpot*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            Matrix_SetTranslateRotateYXZ(thisx->actor.world.pos.x, thisx->actor.world.pos.y, thisx->actor.world.pos.z,
                                         &thisx->actor.shape.rot);
            Matrix_Scale(thisx->actor.scale.x, thisx->actor.scale.y, thisx->actor.scale.z, MTXMODE_APPLY);
            Collider_UpdateSpheres(0, &thisx->collider);
            Collider_UpdateSpheres(1, &thisx->collider);
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_SNOWBALL2, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_SNOWBALL2, CVAR, [](Actor* actor) {
        ObjSnowball2* thisx = (ObjSnowball2*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 800.0f)) {
            Matrix_SetTranslateRotateYXZ(thisx->actor.world.pos.x,
                                         thisx->actor.world.pos.y + (thisx->actor.shape.yOffset * thisx->actor.scale.y),
                                         thisx->actor.world.pos.z, &thisx->actor.shape.rot);
            Matrix_Scale(thisx->actor.scale.x, thisx->actor.scale.y, thisx->actor.scale.z, MTXMODE_APPLY);
            Collider_UpdateSpheres(0, &thisx->collider);
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_TARU, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_TARU, CVAR, [](Actor* actor) {
        ObjTaru* thisx = (ObjTaru*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_KUSA, CVAR, [](Actor* actor, bool* should) {
        EnKusa* thisx = (EnKusa*)actor;
        if (actor->draw != NULL && !thisx->isCut && IsWithinExtendedRange(actor, 600.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KUSA, CVAR, [](Actor* actor) {
        EnKusa* thisx = (EnKusa*)actor;
        if (actor->draw != NULL && !thisx->isCut && IsWithinExtendedRange(actor, 600.0f)) {
            Collider_UpdateCylinder(&thisx->actor, &thisx->collider);
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KUSA2, CVAR, [](Actor* actor) {
        EnKusa2* thisx = (EnKusa2*)actor;
        if (ENKUSA2_GET_1(actor) && IsWithinExtendedRange(actor, 600.0f)) {
            Collider_UpdateCylinder(&thisx->actor, &thisx->collider);
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_KIBAKO2, CVAR, [](Actor* actor, bool* should) {
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_KIBAKO2, CVAR, [](Actor* actor) {
        ObjKibako2* thisx = (ObjKibako2*)actor;
        if (actor->draw != NULL && IsWithinExtendedRange(actor, 600.0f)) {
            CollisionCheck_SetAC(gPlayState, &gPlayState->colChkCtx, &thisx->collider.base);
        }
    });

    COND_ID_HOOK(OnActorUpdate, ACTOR_OBJ_GRASS, CVAR, [](Actor* actor) {
        if (gPlayState == nullptr)
            return;

        // Handle remote colliders lifecycle and hit processing
        auto& remoteColliders = GetOrInitRemoteGrassColliders(actor);

        for (auto& slot : remoteColliders) {
            if (slot.entity != nullptr && (slot.collider.base.acFlags & AC_HIT)) {
                ObjGrass_SpawnFragments(&slot.entity->pos, gPlayState);
                ObjGrass_DropCollectible(slot.entity, gPlayState);
                slot.entity->flags |= OBJ_GRASS_ELEM_REMOVED;
                SoundSource_PlaySfxAtFixedWorldPos(gPlayState, &slot.entity->pos, 20, NA_SE_EV_PLANT_BROKEN);
            }
            slot.entity = nullptr;
            slot.collider.base.acFlags &= ~AC_HIT;
            slot.collider.base.ocFlags1 &= ~OC1_HIT;
        }

        GrassThreat threats[16];
        s32 threatCount = 0;

        // Collect Explosions (En_Bom)
        // These update BEFORE grass, so their positions are current.
        Actor* explosive = gPlayState->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
        while (explosive != nullptr && threatCount < 16) {
            if (explosive->id == ACTOR_EN_BOM && explosive->params == BOMB_TYPE_EXPLOSION) {
                threats[threatCount].pos = explosive->world.pos;
                threats[threatCount].radius = 150.0f;
                threatCount++;
            }
            explosive = explosive->next;
        }

        if (threatCount == 0)
            return;

        // Process each threat
        for (s32 i = 0; i < threatCount; i++) {
            RegisterObjGrassNearThreat((ObjGrass*)actor, &threats[i]);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterExtendedProjectileInteractionDistance, { CVAR_NAME });
