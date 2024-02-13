#include "z_en_g_switch_rando.h"

void EnGSwitch_Kill(EnGSwitch* this, PlayState* play);

void EnGSwitch_Randomizer_SilverRupeeIdle(EnGSwitch* self, PlayState* play) {
    Player* player = GET_PLAYER(play);

    self->actor.shape.rot.y += 0x800;
    if (self->actor.xyzDistToPlayerSq < 900.0f) {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_SILVER_RUPEES) != RO_SILVER_SHUFFLE_VANILLA) {
            if (self->giEntry->modIndex == MOD_NONE) {
                // RANDOTOD: Move this into Item_Give() or some other more central location
                if (self->giEntry->getItemId == GI_SWORD_BGS) {
                    gSaveContext.bgsFlag = true;
                }
                Item_Give(play, self->giEntry->itemId);
            } else if (self->giEntry->modIndex == MOD_RANDOMIZER) {
                Randomizer_Item_Give(play, *self->giEntry);
            }
            Flags_SetRandomizerInf(self->randInfFlag);
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
        GetItemEntry_Draw(play, *this->giEntry);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
