#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Bal/z_en_bal.h"
void EnBal_SetupTalk(EnBal* enBal);
}

std::map<int16_t, std::vector<RandoCheckId>> tingleMap = {
    { SCENE_BACKTOWN, { RC_CLOCK_TOWN_NORTH_TINGLE_MAP_01, RC_CLOCK_TOWN_NORTH_TINGLE_MAP_02 } },
    { SCENE_24KEMONOMITI, { RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_01, RC_ROAD_TO_SOUTHERN_SWAMP_TINGLE_MAP_02 } },
    { SCENE_17SETUGEN, { RC_TWIN_ISLANDS_TINGLE_MAP_01, RC_TWIN_ISLANDS_TINGLE_MAP_02 } },
    { SCENE_17SETUGEN2, { RC_TWIN_ISLANDS_TINGLE_MAP_01, RC_TWIN_ISLANDS_TINGLE_MAP_02 } },
    { SCENE_ROMANYMAE, { RC_MILK_ROAD_TINGLE_MAP_01, RC_MILK_ROAD_TINGLE_MAP_02 } },
    { SCENE_30GYOSON, { RC_GREAT_BAY_COAST_TINGLE_MAP_01, RC_GREAT_BAY_COAST_TINGLE_MAP_02 } },
    { SCENE_IKANA, { RC_IKANA_CANYON_TINGLE_MAP_01, RC_IKANA_CANYON_TINGLE_MAP_02 } }
};

void OnOpenShopText(u16* textId, bool* loadFromMessageTable) {
    RandoCheckId randoCheckId1 = tingleMap[gPlayState->sceneId][0];
    RandoCheckId randoCheckId2 = tingleMap[gPlayState->sceneId][1];

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;

    entry.msg = "\x02\xC3{item1}\x01 {price1} Rupees\x11"
                "\x02{item2}\x01 {price2} Rupees\x11"
                "\x02No thanks";

    CustomMessage::Replace(
        &entry.msg, "{item1}",
        Rando::StaticData::GetItemName(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId1].randoItemId, randoCheckId1),
                                       false, randoCheckId1));
    CustomMessage::Replace(
        &entry.msg, "{item2}",
        Rando::StaticData::GetItemName(Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId2].randoItemId, randoCheckId2),
                                       false, randoCheckId2));
    CustomMessage::Replace(&entry.msg, "{price1}", std::to_string(RANDO_SAVE_CHECKS[randoCheckId1].price));
    CustomMessage::Replace(&entry.msg, "{price2}", std::to_string(RANDO_SAVE_CHECKS[randoCheckId2].price));
    CustomMessage::EnsureMessageEnd(&entry.msg);
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void OnOpenCantGetText(u16* textId, bool* loadFromMessageTable) {
    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.msg = "I'm sorry, but it seems I cannot sell this to you now.";
    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
};

void Rando::ActorBehavior::InitEnBalBehavior() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_TINGLE_SHOPS];

    COND_VB_SHOULD(VB_NOT_AFFORD_TINGLE_MAP, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);
        s32* price = va_arg(args, s32*);
        auto randoCheckId = tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];

        *price = RANDO_SAVE_CHECKS[randoCheckId].price;

        *should = gSaveContext.save.saveInfo.playerData.rupees < *price;
    });

    COND_VB_SHOULD(VB_ALREADY_HAVE_TINGLE_MAP, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);

        auto randoCheckId = tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex];

        RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);

        *should = !Rando::IsItemObtainable(randoItemId, randoCheckId);
    });

    COND_VB_SHOULD(VB_TINGLE_GIVE_MAP_UNLOCK, shouldRegister, {
        EnBal* enBal = va_arg(args, EnBal*);
        RANDO_SAVE_CHECKS[tingleMap[gPlayState->sceneId][gPlayState->msgCtx.choiceIndex]].eligible = true;
        Message_StartTextbox(gPlayState, 0x1D17, &enBal->picto.actor);
        enBal->textId = 0x1D17;
        EnBal_SetupTalk(enBal);
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, 0x1D11, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D12, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D13, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D14, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D15, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D16, shouldRegister, OnOpenShopText);
    COND_ID_HOOK(OnOpenText, 0x1D09, shouldRegister, OnOpenCantGetText);

    COND_VB_SHOULD(VB_HAVE_MAGIC_FOR_TINGLE, shouldRegister, { *should = true; });
}
