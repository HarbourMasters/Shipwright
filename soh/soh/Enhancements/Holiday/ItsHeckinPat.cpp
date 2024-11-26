#include "Holiday.hpp"
#include "soh_assets.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/frame_interpolation.h"
#include "soh/Notification/Notification.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/util.h"
#include "soh/Enhancements/randomizer/randomizer.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}
extern GetItemEntry vanillaQueuedItemEntry;

#define AUTHOR "ItsHeckinPat"
#define CVAR(v) "gHoliday." AUTHOR "." v

bool spawningPresents = false;

int collectedPresent = 0;

struct Present {
};

std::unordered_map<Actor*, Present> presents;

void Present_Init(Actor* actor, PlayState* play) {
    Present present;
    presents[actor] = present;

    actor->gravity = -1;
    Actor_MoveForward(actor);
    actor->shape.rot.y = Random(0, 0xFFFF);

    Actor_UpdateBgCheckInfo(play, actor, 10.0f, 10.0f, 0.0f, 0xFF);
}

void Present_Update(Actor* actor, PlayState* play) {
    Present* present = &presents[actor];

    if (actor->xzDistToPlayer < 50.0f && actor->yDistToPlayer < 50.0f) {
        collectedPresent++;
        Notification::Emit({
            .itemIcon = "RG_TRIFORCE_PIECE",
            .message = "You collected a present!",
            .messageColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
        });
        Actor_Kill(actor);
    }
}

void Present_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
    Matrix_Translate(49.20f, 0.0f, -106.60f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_MODELVIEW | G_MTX_LOAD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gXmasDecor100DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Present_Destroy(Actor* actor, PlayState* play) {
    presents.erase(actor);
}

static CollisionPoly presentPoly;
static f32 raycastResult;

void OnConfigChanged() {
    COND_HOOK(OnSceneSpawnActors, CVarGetInteger(CVAR("GiftsForNPCs"), 0), []() {
        presents.clear();
        Vec3f pos;
        pos.y = 9999.0f;
        int spawnAttempts = 0;
        while (spawnAttempts < 50) {
            if (GET_PLAYER(gPlayState) != nullptr) {
                pos.x = GET_PLAYER(gPlayState)->actor.world.pos.x;
                pos.z = GET_PLAYER(gPlayState)->actor.world.pos.z;
            } else {
                pos.x = 0;
                pos.z = 0;
            }
            // X/Z anywhere from -1000.0 to +1000.0 from player 
            pos.x += (float)(Random(0, 2000)) - 1000.0f;
            pos.z += (float)(Random(0, 2000)) - 1000.0f;

            raycastResult = BgCheck_AnyRaycastFloor1(&gPlayState->colCtx, &presentPoly, &pos);

            if (raycastResult > BGCHECK_Y_MIN) {
                spawningPresents = true;
                Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_OE2, pos.x, raycastResult, pos.z, 0, 0, 0, 0, false);
                spawningPresents = false;
                // break;
            }

            spawnAttempts++;
        }
    });

    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_OE2, CVarGetInteger(CVAR("GiftsForNPCs"), 0), [](void* actorRef, bool* should) {
        Actor* actor = (Actor*)actorRef;
        if (spawningPresents) {
            actor->init = Present_Init;
            actor->update = Present_Update;
            actor->draw = Present_Draw;
            actor->destroy = Present_Destroy;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x1019, CVarGetInteger(CVAR("GiftsForNPCs"), 0), [](u16 * textId, bool* loadFromMessageTable) {
        if (collectedPresent <= 0) {
            return;
        }

        auto messageEntry = CustomMessage("A present??? FOR ME???");
        messageEntry.Format();
        messageEntry.LoadIntoFont();
        *loadFromMessageTable = false;

        vanillaQueuedItemEntry = Rando::StaticData::RetrieveItem(RG_PIECE_OF_HEART).GetGIEntry_Copy();

        collectedPresent--;
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Gifts for NPCs", CVAR("GiftsForNPCs"))) {
        OnConfigChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    OnConfigChanged();
    // #endregion

    // TODO: Anything you want to run once on startup
}

// TODO: Uncomment this line to enable the mod
static Holiday holiday(DrawMenu, RegisterMod);
