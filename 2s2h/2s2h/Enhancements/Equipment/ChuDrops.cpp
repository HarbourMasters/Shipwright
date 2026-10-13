#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "objects/gameplay_keep/gameplay_keep.h"
}

#define CVAR_NAME "gEnhancements.Equipment.ChuDrops"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void ChuDrop_Give(Actor* actor, PlayState* play) {
    Item_Give(play, ITEM_BOMBCHUS_5);
}

static Color_RGBA8 sEffectPrimColor = { 255, 255, 127, 0 };
static Color_RGBA8 sEffectEnvColor = { 255, 255, 255, 0 };
static Vec3f sEffectVelocity = { 0.0f, 0.1f, 0.0f };
static Vec3f sEffectAccel = { 0.0f, 0.01f, 0.0f };

static void ChuDrop_Draw(Actor* actor, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    // Spawn sparkles like regular drops when falling.
    // Consider promoting this to CustomItem Update itself for tossed items.
    if ((CUSTOM_ITEM_FLAGS & CustomItem::TOSS_ON_SPAWN) && actor->gravity != 0.0f &&
        !(actor->bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH))) {
        if ((play->gameplayFrames & 1) == 0) {
            Vec3f pos;
            pos.x = actor->world.pos.x + ((Rand_ZeroOne() - 0.5f) * 10.0f);
            pos.y = actor->world.pos.y + ((Rand_ZeroOne() - 0.5f) * 10.0f);
            pos.z = actor->world.pos.z + ((Rand_ZeroOne() - 0.5f) * 10.0f);
            EffectSsKirakira_SpawnSmall(play, &pos, &sEffectVelocity, &sEffectAccel, &sEffectPrimColor,
                                        &sEffectEnvColor);
        }
    }

    // Skip interpolation for one frame once the item is moved to the player
    FrameInterpolation_RecordOpenChild(actor, (CUSTOM_ITEM_FLAGS & CustomItem::KEEP_ON_PLAYER) ? 1 : 0);
    FrameInterpolation_IgnoreActorMtx();

    if (CVarGetInteger("gEnhancements.Graphics.3DItemDrops", 0)) {
        CUSTOM_ITEM_FLAGS &= ~CustomItem::STOP_SPINNING;

        Matrix_Scale(16.0f, 16.0f, 16.0f, MTXMODE_APPLY);
        GetItem_Draw(play, GID_BOMBCHU);
    } else {
        // Ideally we would update spining flag and rotation in an Update func, but that is not convenient to do right
        // now so opting to change here in the draw instead (upon toggle, the item may look incorrect for 1 frame).
        CUSTOM_ITEM_FLAGS |= CustomItem::STOP_SPINNING;
        actor->shape.rot.y = 0;

        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

        POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);

        gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)gDropBombchuTex);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gItemDropDL);
    }

    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(play->state.gfxCtx);
}

void RegisterChuDrops() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, CVAR, [](Actor* actor, bool* should) {
        u8 params = actor->params & 0xFF;
        if (params == ITEM00_BOMBS_0 || params == ITEM00_BOMBS_A || params == ITEM00_BOMBS_B) {
            if (rand() % 100 < 50) {
                *should = false;
                EnItem00* newItem =
                    CustomItem::Spawn(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
                                      CustomItem::GIVE_OVERHEAD | CustomItem::TOSS_ON_SPAWN | CustomItem::STOP_BOBBING,
                                      GID_BOMBCHU, ChuDrop_Give, ChuDrop_Draw);

                // Set actor shadow and yOffset to match normal drops
                ActorShape_Init(&newItem->actor.shape, 640.0f, ActorShadow_DrawCircle, 12.0f);
                newItem->actor.shape.shadowAlpha = 180;
                // Default gravity in CustomItem is too heavy, using a smaller value here to align with item drops
                newItem->actor.gravity = -1.0f;
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterChuDrops, { CVAR_NAME });
