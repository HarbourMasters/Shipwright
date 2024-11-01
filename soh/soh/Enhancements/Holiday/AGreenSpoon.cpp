#include "Holiday.hpp"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/frame_interpolation.h"
#include "soh_assets.h"
#include "overlays/actors/ovl_En_Gs/z_en_gs.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#define AUTHOR "AGreenSpoon"
#define CVAR(v) "gHoliday." AUTHOR "." v

void EnGs_Evil(EnGs* enGs, PlayState* play) {
    Player* player = GET_PLAYER(gPlayState);
    if (!(player->stateFlags1 & PLAYER_STATE1_TALKING)) {
        Math_ApproachS(&enGs->actor.shape.rot.y, enGs->actor.yawTowardsPlayer, 5, 0xBB8);

        if (enGs->unk_200 <= 0) {
            float offsetDistance = 10.0f;
            float offsetX = sinf(enGs->actor.shape.rot.y * (M_PI / 0x8000)) * offsetDistance;
            float offsetZ = cosf(enGs->actor.shape.rot.y * (M_PI / 0x8000)) * offsetDistance;

            float dx = player->actor.world.pos.x - (enGs->actor.world.pos.x + offsetX);
            float dy = player->actor.world.pos.y - 10.0f - enGs->actor.world.pos.y; 
            float dz = player->actor.world.pos.z - (enGs->actor.world.pos.z + offsetZ);

            s16 rotX = atan2f(dy, sqrtf(dx * dx + dz * dz)) * (0x8000 / M_PI);
            s16 rotY = enGs->actor.shape.rot.y;
            s16 rotZ = atan2f(dx, dz) * (0x8000 / M_PI);

            Actor* actor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, 
                                       enGs->actor.world.pos.x + offsetX,
                                       enGs->actor.world.pos.y + 40.0f,
                                       enGs->actor.world.pos.z + offsetZ,
                                       rotX, rotY, rotZ, 
                                       100, false);

            EnClearTag* clearTag = (EnClearTag*)actor;

            enGs->unk_200 = 5;
        }
    
        enGs->unk_200--;
    }
}

static void OnConfigurationChanged() {
    COND_ID_HOOK(OnOpenText, 0x2053, CVarGetInteger(CVAR("EvilGossipStone"), 0), [](u16 * textId, bool* loadFromMessageTable) {
        Actor* actor = Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor, ACTOR_EN_GS, ACTORCAT_PROP, 100.0f);

        if (actor == NULL) {
            return;
        }

        EnGs* gs = (EnGs*)actor;
        gs->actionFunc = EnGs_Evil;
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Evil Gossip Stone", CVAR("EvilGossipStone"))) {
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
