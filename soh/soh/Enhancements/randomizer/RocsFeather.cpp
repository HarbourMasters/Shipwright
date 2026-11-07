#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <soh_assets.h>
#include <z64.h>
#include "functions.h"
#include "variables.h"
#include "macros.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

#define TEXT_GET_ROCS_FEATHER 0xE0

static uint8_t groundTimer = 0;
static f32 effectsScale = 1.0f;
static bool rocsConsumable = true;

static u8& RocsFeathersAcquired() {
    return gSaveContext.ship.quest.data.randomizer.rocsFeathersAcquired;
}

static u8& RocsFeathersRemaining() {
    return gSaveContext.ship.quest.data.randomizer.rocsFeathersRemaining;
}

static void BuildRocsFeatherGetMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;

    msg = CustomMessage(
        "You got %bRoc's Feather%w!&You now have %b[[d]]%w |use|uses| of it!",
        "<<Du erhältst %bGreifenfeder%w!&Du kannst sie nun %b[[d]]%w-mal einsetzen!>>",  // TODO - German Translation
        "<<Vous obtenez la %bPlume de Roc%w!&Vous pouvez l'utiliser %b[[d]]%w fois!>>"); // TODO - French Translation

    if (rocsConsumable) {
        msg.InsertNumber(RocsFeathersRemaining());
    } else {
        msg.InsertNumber(RocsFeathersAcquired());
    }
    msg.AutoFormat(ITEM_ROCS_FEATHER);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}
void BuildRocsShopDesc(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg("\x08%rRoc's Feather  10 Rupees%w&Use this to jump across holes and chasms.\x0A\x02",
                      "\x08%r<<Greifenfeder 10 Rubine%w&Sonderangebot!>>\x0A\x02", // TODO - German Translation
                      "\x08<<La %rPlume de Roc  10 Rubis%w&Elle vous permet de franchir les trous et les "
                      "failles.>>\x0A\x02"); // TODO - French Translation
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildRocsShopPrompt(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    msg = CustomMessage(
        "\x08%rRoc's Feather  10 Rupees%w\x09\x1B%gBuy&Don't buy%w\x09\x02",
        "\x08<<%rGreifenfeder  10 Rubine%w\x09\x1B%gKaufen&Nicht kaufen%w>>\x09\x02",       // TODO - German Translation
        "\x08<<%rLa Plume de Roc  10 Rubis%w\x09\x1B%gAcheter&Ne pas acheter%w>>\x09\x02"); // TODO - French Translation
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void UseRocsFeather(PlayState* playState, Player* player) {
    if (RocsFeathersRemaining() > 0) {
        RocsFeathersRemaining()--;

        func_80838940(player, (LinkAnimationHeader*)&gPlayerAnim_link_rocs_feather_jump, 5.8f, playState, 0);

        // Actionvar needed to prevent weird animation morph
        player->av2.actionVar2 = 1;

        // Move player forward on Roc's use
        player->linearVelocity = 5.0f;
        player->actor.world.rot.y = player->yaw = player->actor.shape.rot.y;

        if (gSaveContext.linkAge == LINK_AGE_CHILD) {
            player->actor.velocity.y = 7.0f;
            effectsScale = 1.0f;
        } else {
            player->actor.velocity.y = 7.5f;
            effectsScale = 1.5f;
        }

        Vec3f effectsPos = player->actor.home.pos;
        effectsPos.y += 3;

        EffectSsGRipple_Spawn(playState, &effectsPos, static_cast<s16>(200 * effectsScale),
                              static_cast<s16>(300 * effectsScale), 1);
        EffectSsGSplash_Spawn(playState, &effectsPos, NULL, NULL, 0, static_cast<s16>(150 * effectsScale));

        // Remove hopping state when using Roc's after sidehop/backflip to allow grabbing ledges again
        player->stateFlags2 &= ~(PLAYER_STATE2_HOPPING);

        Player_PlaySfx(&player->actor, NA_SE_PL_SKIP);
    } else {
        Sfx_PlaySfxCentered(NA_SE_SY_ERROR);
    }
}

void RegisterRocsFeather() {
    // Always register hooks - they only activate when ITEM_ROCS_FEATHER is actually used
    // Rando-specific logic (item pool, accessibility, etc.) is handled elsewhere
    bool shouldRegister = true;

    COND_HOOK(OnLoadFile, shouldRegister, [](int32_t fileNum) {
        rocsConsumable = Rando::Context::GetInstance()->GetOption(RSK_ROCS_FEATHER).Is(RO_ROCS_FEATHER_CONSUMABLE);
    });

    COND_HOOK(OnPlayerUpdate, shouldRegister, []() {
        Player* player = GET_PLAYER(gPlayState);

        // Reset Rocs count when touching the ground for 3+ frames
        if (player->actor.bgCheckFlags & 1) {
            if (groundTimer <= 3) {
                groundTimer++;
            }
        } else {
            groundTimer = 0;
        }

        if (!rocsConsumable && groundTimer >= 3) {
            RocsFeathersRemaining() = RocsFeathersAcquired();
        }
    });

    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, shouldRegister, {
        int32_t usedItem = va_arg(args, int32_t);

        // Roc's Feather behaviour
        if (usedItem == ITEM_ROCS_FEATHER) {
            *should = false;

            Player* player = GET_PLAYER(gPlayState);

            UseRocsFeather(gPlayState, player);
        }
    });

    COND_VB_SHOULD(VB_CHANGE_ACTION_AND_USE, shouldRegister, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("RocsFeatherOnA"), 0) != 1) {
            *should = true;
            return;
        }

        Player* player = va_arg(args, Player*);
        PlayState* playState = va_arg(args, PlayState*);
        Input* input = &playState->state.input[0];

        u16 action = playState->interfaceCtx.unk_1F0;

        // disabled during z-targeting, while paused, when grounded and any action
        // is displayed, while climbing ladders, when midair with something held,
        // while crawling, and when there are no rocs remaining to use
        if ((player->stateFlags1 &
             (PLAYER_STATE1_HOSTILE_LOCK_ON | PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL)) ||
            (player->stateFlags2 & PLAYER_STATE2_CRAWLING) ||
            (playState->pauseCtx.state > 0 && playState->pauseCtx.state < 18) ||
            ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && action != DO_ACTION_NONE) ||
            (!(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
             (action == DO_ACTION_DOWN || action == DO_ACTION_DROP || action == DO_ACTION_THROW ||
              action == DO_ACTION_DIVE)) ||
            (RocsFeathersRemaining() == 0)) {
            *should = true;
        } else {

            if (CHECK_BTN_ANY(input->press.button, BTN_A)) {
                *should = false;
                UseRocsFeather(playState, player);

            } else if (!(CHECK_BTN_ANY(input->press.button, BTN_A)) &&
                       player->actor.bgCheckFlags & (BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_GROUND_TOUCH)) {
                *should = false;
                playState->interfaceCtx.unk_1F0 = DO_ACTION_NONE;
            }
        }
    });

    COND_VB_SHOULD(VB_DRAW_CUSTOM_ITEM_NAME, shouldRegister, {
        u32 namedItem = va_arg(args, u32);
        if (namedItem == ITEM_ROCS_FEATHER) {
            *should = true;
            const char* textureName = gRocsFeatherItemNameENGTex;

            if (gSaveContext.language == LANGUAGE_GER) {
                textureName = gRocsFeatherItemNameGERTex;
            } else if (gSaveContext.language == LANGUAGE_FRA) {
                textureName = gRocsFeatherItemNameFRATex;
            }

            memcpy(gPlayState->pauseCtx.nameSegment, textureName, strlen(textureName) + 1);
        }
    });

    COND_VB_SHOULD(VB_SET_ACTION_TEXTURE, shouldRegister, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("RocsFeatherOnA"), 0) != 1) {
            *should = false;
            return;
        }

        u16* action = va_arg(args, u16*);
        const char** alternateTexture = va_arg(args, const char**);
        Player* player = GET_PLAYER(gPlayState);

        // clear action manually on jumping/landing to avoid issues with value not refreshing on change when direction
        // input is maintained
        if (player->actor.bgCheckFlags & (BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_GROUND_TOUCH)) {
            *action = *action == DO_ACTION_NONE ? DO_ACTION_MAX : DO_ACTION_NONE;
            *should = false;

            // disabled during z-targeting, while paused, when grounded and any action
            // is displayed, while climbing ladders, when midair with something held,
            // while crawling, and when there are no rocs remaining to use
        } else if ((player->stateFlags1 &
                    (PLAYER_STATE1_HOSTILE_LOCK_ON | PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL)) ||
                   (player->stateFlags2 & PLAYER_STATE2_CRAWLING) ||
                   (gPlayState->pauseCtx.state > 0 && gPlayState->pauseCtx.state < 18) ||
                   ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && *action != DO_ACTION_NONE) ||
                   (!(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    (*action == DO_ACTION_DOWN || *action == DO_ACTION_DROP || *action == DO_ACTION_THROW ||
                     *action == DO_ACTION_DIVE)) ||
                   (RocsFeathersRemaining() == 0)) {
            *should = false;
        } else {
            *should = true;
            *alternateTexture = gRocsFeatherTex;
        }
    });

    COND_ID_HOOK(OnOpenText, TEXT_GET_ROCS_FEATHER, shouldRegister, BuildRocsFeatherGetMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BUY_ROCS_FEATHER_DESC, shouldRegister && rocsConsumable, BuildRocsShopDesc);
    COND_ID_HOOK(OnOpenText, TEXT_BUY_ROCS_FEATHER_PROMPT, shouldRegister && rocsConsumable, BuildRocsShopPrompt);
}

static RegisterShipInitFunc registerRocsFeather(RegisterRocsFeather);
