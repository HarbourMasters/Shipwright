#include "z_en_g_switch_rando.h"

void EnGSwitch_Kill(EnGSwitch* this, PlayState* play);

void EnGSwitch_Randomizer_SilverRupeeIdle(EnGSwitch* self, PlayState* play) {
    Player* player = GET_PLAYER(play);

    self->actor.shape.rot.y += 0x800;
    if (self->actor.xyzDistToPlayerSq < 900.0f) {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_SILVER_RUPEES) != RO_SILVER_SHUFFLE_VANILLA) {
            GetItemEntry getItem = ItemTable_RetrieveEntry(MOD_NONE, GI_NUTS_5);
            if (getItem.modIndex == MOD_NONE) {
                // RANDOTOD: Move this into Item_Give() or some other more central location
                if (getItem.getItemId == GI_SWORD_BGS) {
                    gSaveContext.bgsFlag = true;
                }
                Item_Give(play, getItem.itemId);
            } else if (getItem.modIndex == MOD_RANDOMIZER) {
                Randomizer_Item_Give(play, getItem);
            }
            self->killTimer = 0;
            self->actionFunc = EnGSwitch_Kill;
        }
    }
}

void EnGSwitch_Randomizer_Draw(Actor* thisx, PlayState* play) {
    EnGSwitch* this = (EnGSwitch*)thisx;
    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_SILVER_RUPEES) != RO_SILVER_SHUFFLE_VANILLA &&
        this->type == ENGSWITCH_SILVER_RUPEE) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Scale(17.5f, 17.5f, 17.5f, MTXMODE_APPLY);
        GetItemEntry getItem = ItemTable_RetrieveEntry(MOD_NONE, GI_NUTS_5);
        GetItemEntry_Draw(play, getItem);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
