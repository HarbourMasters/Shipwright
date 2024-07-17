#include "z_en_g_switch_rando.h"

void EnGSwitch_Kill(EnGSwitch* this, PlayState* play);

void EnGSwitch_Randomizer_SilverRupeeIdle(EnGSwitch* self, PlayState* play) {
    Player* player = GET_PLAYER(play);

    self->actor.shape.rot.y += 0x800;
    if (self->actor.xyzDistToPlayerSq < 900.0f) {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_SILVER_RUPEES) != RO_SILVER_SHUFFLE_VANILLA) {
            // player->getItemEntry = *self->giEntry;

            // if (self->giEntry->getItemId == RG_ICE_TRAP) {
            //     Message_StartTextbox(play, 0xF8, NULL);
            // } else {
            //     Message_StartTextbox(play, self->giEntry->textId, NULL);
            // }
            // if (self->giEntry->modIndex == MOD_NONE) {
            //     // RANDOTOD: Move this into Item_Give() or some other more central location
            //     if (self->giEntry->getItemId == GI_SWORD_BGS) {
            //         gSaveContext.bgsFlag = true;
            //     }
            //     Item_Give(play, self->giEntry->itemId);
            // } else if (self->giEntry->modIndex == MOD_RANDOMIZER && self->giEntry->getItemId != RG_ICE_TRAP) {
            //     Randomizer_Item_Give(play, *self->giEntry);
            // }
            Flags_SetRandomizerInf(self->randInfFlag);
            // if ((!CVarGetInteger("gSkulltulaFreeze", 0) || self->giEntry->itemId != ITEM_SKULL_TOKEN) &&
            //     self->giEntry->getItemId != RG_ICE_TRAP) {
            //     player->actor.freezeTimer = 20;
            // }

            // if (self->giEntry->getItemId == RG_ICE_TRAP && Message_GetState(&play->msgCtx) != TEXT_STATE_CLOSING) {
            //     player->actor.freezeTimer = 10;
            // }

            // if (IS_RANDO) {
            //     if (self->giEntry->getItemId != RG_ICE_TRAP) {
            //         Audio_PlayFanfare_Rando(*self->giEntry);
            //     } else {
            //         gSaveContext.pendingIceTrapCount++;
            //         Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
            //     }
            // } else {
            //     Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
            // }

            // player->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
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
        GetItemEntry_Draw(play, this->giEntry);
        CLOSE_DISPS(play->state.gfxCtx);
    }
}
