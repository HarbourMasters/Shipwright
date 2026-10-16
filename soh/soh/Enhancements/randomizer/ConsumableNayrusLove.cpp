#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <z64.h>
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

#define NAYRUS_LOVE_USES_MAX 99

#define TEXT_GET_NAYRUS_LOVE 0xAF

static u8& NayrusLoveUses() {
    return gSaveContext.ship.quest.data.randomizer.nayrusLoveUses;
}

// The text opens before Item_Give runs, so the stored count doesn't include this item yet
static void BuildNayrusLoveMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("You got %bNayru's Love%w!&You now have %b[[d]]%w |use|uses| of it!",
                                      "Du erhältst %bNayrus Umarmung%w!&Du kannst sie nun %b[[d]]%w-mal einsetzen!",
                                      "Vous obtenez l'%bAmour de Nayru%w!&Vous pouvez l'utiliser %b[[d]]%w fois!");
    uint8_t uses = NayrusLoveUses() < NAYRUS_LOVE_USES_MAX ? NayrusLoveUses() + 1 : NAYRUS_LOVE_USES_MAX;
    msg.InsertNumber(uses);
    msg.AutoFormat(ITEM_NAYRUS_LOVE);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterConsumableNayrusLove() {
    bool shouldRegister = IS_RANDO && RAND_GET_OPTION(RSK_NAYRUS_LOVE_USES).Get() > 0;

    COND_HOOK(OnItemReceive, shouldRegister, [](GetItemEntry itemEntry) {
        if (itemEntry.modIndex != MOD_NONE || itemEntry.itemId != ITEM_NAYRUS_LOVE) {
            return;
        }
        if (NayrusLoveUses() < NAYRUS_LOVE_USES_MAX) {
            NayrusLoveUses()++;
        }
    });

    COND_VB_SHOULD(VB_CHANGE_HELD_ITEM_AND_USE_ITEM, shouldRegister, {
        int32_t usedItem = va_arg(args, int32_t);
        if (usedItem == ITEM_NAYRUS_LOVE && NayrusLoveUses() == 0) {
            *should = false;
            Sfx_PlaySfxCentered(NA_SE_SY_ERROR);
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_MAGIC_DARK, shouldRegister, [](void*) {
        // Don't consume a usage if the timer is still active
        if (gSaveContext.nayrusLoveTimer != 0) {
            return;
        }
        if (NayrusLoveUses() > 0) {
            NayrusLoveUses()--;
        }
    });

    COND_VB_SHOULD(VB_OVERRIDE_AMMO_COUNT, shouldRegister, {
        int16_t item = *va_arg(args, int16_t*);
        int16_t* ammo = va_arg(args, int16_t*);
        if (item == ITEM_NAYRUS_LOVE) {
            *ammo = NayrusLoveUses();
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_COLOR_AMMO_GREEN, shouldRegister, {
        int16_t item = va_arg(args, int);
        if (item == ITEM_NAYRUS_LOVE && NayrusLoveUses() >= RAND_GET_OPTION(RSK_NAYRUS_LOVE_USES).Get()) {
            *should = true;
        }
    });

    COND_ID_HOOK(OnOpenText, TEXT_GET_NAYRUS_LOVE, shouldRegister, BuildNayrusLoveMessage);
}

static RegisterShipInitFunc initFunc(RegisterConsumableNayrusLove, { "IS_RANDO" });
