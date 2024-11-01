#include "Holiday.hpp"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/frame_interpolation.h"
#include "soh_assets.h"
#include "overlays/actors/ovl_En_Nutsball/z_en_nutsball.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"

#include "objects/gameplay_field_keep/gameplay_field_keep.h"
extern PlayState* gPlayState;
void func_80ABBBA8(EnNutsball* nut, PlayState* play);
void EnNutsball_Draw(Actor* nut, PlayState* play);
}

#define AUTHOR "Grimey"
#define CVAR(v) "gHoliday." AUTHOR "." v

static bool spawningPenguins = false;

typedef enum {
    PENGUIN_STATE_IDLE,
    PENGUIN_STATE_WALK,
} PenguinState;

struct Penguin {
    PenguinState state;
    s16 timer;
    s16 targetRot;
};

std::unordered_map<Actor*, Penguin> penguins;

void Penguin_Init(Actor* actor, PlayState* play) {
    Penguin penguin;
    penguin.state = PENGUIN_STATE_IDLE;
    penguin.timer = 0;
    actor->world.rot.y = penguin.targetRot = rand() % 0x10000;
    penguins[actor] = penguin;
    actor->gravity = -1.0f;
    actor->flags &= ~ACTOR_FLAG_TARGETABLE;
}

void Penguin_Update(Actor* actor, PlayState* play) {
    Penguin* penguin = &penguins[actor];

    if (penguin->timer <= 0) {
        if (penguin->state == PENGUIN_STATE_IDLE) {
            penguin->state = (PenguinState)(rand() % 3);
            penguin->timer = rand() % (20 * 10) + (20 * 3);
        } else {
            penguin->state = PENGUIN_STATE_IDLE;
            penguin->timer = rand() % (20 * 10) + (20 * 3);
        }
    } else {
        penguin->timer--;
    }

    if (rand() % 100 == 0) {
        penguin->targetRot = rand() % 0x10000;
    }

    switch (penguin->state) {
        case PENGUIN_STATE_IDLE:
            break;
        case PENGUIN_STATE_WALK:
            actor->speedXZ = 0.5f;
            break;
    }
    
    Math_SmoothStepToS(&actor->world.rot.y, penguin->targetRot, 1, 200, 0);
    actor->shape.rot.y = actor->world.rot.y;

    if (actor->speedXZ < 0.0f) {
        actor->speedXZ = 0.0f;
    }

    Actor_MoveForward(actor);

    Actor_UpdateBgCheckInfo(play, actor, 10.0f, 10.0f, 0.0f, 0xFF);
}

void Penguin_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
    Matrix_Translate(0, 2000.0f, 0, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gPenguinDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Penguin_Destroy(Actor* actor, PlayState* play) {
    penguins.erase(actor);
}

static void OnConfigurationChanged() {
    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("Hailstorm"), 0), []() {
        // Every frame has a 1/300 chance of spawning hail
        if (rand() % 300 == 0) { 
            int spawned = 0;
            while (spawned < 1) {
                Vec3f pos = GET_PLAYER(gPlayState)->actor.world.pos;
                pos.x += (float)Random(0, 100) - 50.0f;
                pos.z += (float)Random(0, 100) - 50.0f;
                pos.y += 200.0f;

                Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_NUTSBALL, pos.x, pos.y, pos.z, 0, 0, 0, 0, false);
                EnNutsball* nut = (EnNutsball*)actor;
                nut->actor.draw = EnNutsball_Draw;
                nut->actor.shape.rot.y = 0;
                nut->timer = 0;
                nut->actionFunc = func_80ABBBA8;
                nut->actor.speedXZ = 0.0f;
                nut->actor.gravity = -2.0f;
                spawned++;
            }
        }
    });

    COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("Penguins"), 0), []() {
        penguins.clear();

        if (gPlayState->sceneNum != SCENE_HYRULE_FIELD) {
            return;
        }

        static Vec3f huddlePos;
        static Vec3f spawnPos;
        static f32 raycastResult;
        static CollisionPoly poly;

        spawningPenguins = true;

        int huddlesSpawned = 0;
        while (huddlesSpawned < 10) {
            huddlePos.x = (float)(Random(
                (gPlayState->sceneNum == SCENE_HYRULE_FIELD ? -10000 : -2700) + 10000,
                (gPlayState->sceneNum == SCENE_HYRULE_FIELD ? 5000 : 2000) + 10000
            ) - (float)10000.0f);
            huddlePos.y = 5000;
            huddlePos.z = (float)(Random(
                (gPlayState->sceneNum == SCENE_HYRULE_FIELD ? -1000 : -2000) + 10000,
                (gPlayState->sceneNum == SCENE_HYRULE_FIELD ? 15000 : 2000) + 10000
            ) - (float)10000.0f);

            if (BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &poly, &huddlePos) <= BGCHECK_Y_MIN) {
                continue;
            }

            // 5-10
            int huddleSize = rand() % 6 + 5;
            int penguinsSpawned = 0;
            while (penguinsSpawned < huddleSize) {
                spawnPos.x = huddlePos.x + rand() % 100 - 50;
                spawnPos.y = huddlePos.y;
                spawnPos.z = huddlePos.z + rand() % 100 - 50;

                raycastResult = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &poly, &spawnPos);

                if (raycastResult > BGCHECK_Y_MIN) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_OE2, spawnPos.x, raycastResult, spawnPos.z, 0, 0, 0, 0, false);
                    penguinsSpawned++;
                }
            }
            huddlesSpawned++;
        }

        spawningPenguins = false;
    });
    
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_OE2, CVarGetInteger(CVAR("Penguins"), 0), [](void* actorRef, bool* should) {
        Actor* actor = (Actor*)actorRef;
        if (spawningPenguins) {
            actor->init = Penguin_Init;
            actor->update = Penguin_Update;
            actor->draw = Penguin_Draw;
            actor->destroy = Penguin_Destroy;
        }
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Penguins", CVAR("Penguins"))) {
        OnConfigurationChanged();
    }
    if (UIWidgets::EnhancementCheckbox("Hailstorm", CVAR("Hailstorm"))) {
        OnConfigurationChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    OnConfigurationChanged();
    // #endregion

    // TODO: Anything you want to run once on startup
}

// TODO: Uncomment this line to enable the mod
static Holiday holiday(DrawMenu, RegisterMod);
