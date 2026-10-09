#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

EquipSlot func_8082FDC4(void);
}

#define CVAR_NAME "gEnhancements.Masks.BlastMaskKeg"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterBlastMaskKeg() {
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_BOM, CVAR, [](Actor* actor, bool* should) {
        Player* player = GET_PLAYER(gPlayState);
        ItemId item;
        EquipSlot i = func_8082FDC4();

        i = ((i >= EQUIP_SLOT_A) && (player->transformation == PLAYER_FORM_FIERCE_DEITY) &&
             (player->heldItemAction != PLAYER_IA_SWORD_TWO_HANDED))
                ? EQUIP_SLOT_B
                : i;

        item = Player_GetItemOnButton(gPlayState, player, i);
        if (item == ITEM_F0) {
            actor->shape.rot.x = BOMB_EXPLOSIVE_TYPE_POWDER_KEG;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterBlastMaskKeg, { CVAR_NAME });
