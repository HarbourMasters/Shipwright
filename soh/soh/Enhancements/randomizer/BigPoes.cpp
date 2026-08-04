#include <soh/OTRGlobals.h>

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "overlays/actors/ovl_En_Gb/z_en_gb.h"
extern PlayState* gPlayState;
}

void BuildBigPoeCollectedMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("You have #" + CustomMessage::POINTS("\x01") + "# points.", { QM_RED });
    msg.AutoFormat();
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void RegisterBigPoeHooks() {
    COND_VB_SHOULD(VB_BOTTLE_BIG_POE, IS_RANDO, {
        EnPoField* enPoe = va_arg(args, EnPoField*);
        enPoe->actor.textId = 0x5090;
        Flags_SetSwitch(gPlayState, enPoe->actor.params & 0xFF);
        HIGH_SCORE(HS_POE_POINTS) += 100;
        if (HIGH_SCORE(HS_POE_POINTS) > 1100) {
            HIGH_SCORE(HS_POE_POINTS) = 1100;
        }
        *should = false;
    });
    COND_VB_SHOULD(VB_SELL_POES_TO_POE_COLLECTOR, IS_RANDO, {
        if (!Flags_GetRandomizerInf(RAND_INF_10_BIG_POES) && HIGH_SCORE(HS_POE_POINTS) >= 1000 &&
            !(GET_PLAYER(gPlayState)->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS)) {
            EnGb* enGb = va_arg(args, EnGb*);
            enGb->textId = 0x70F8;
            Message_ContinueTextbox(gPlayState, enGb->textId);
            enGb->actionFunc = func_80A2FB40;
            *should = false;
        }
    });
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_POE_COLLECTOR, IS_RANDO, {
        EnGb* enGb = va_arg(args, EnGb*);
        if (!Flags_GetRandomizerInf(RAND_INF_10_BIG_POES)) {
            Flags_SetInfTable(INFTABLE_SPOKE_TO_POE_COLLECTOR_IN_RUINED_MARKET);
            Flags_SetRandomizerInf(RAND_INF_10_BIG_POES);
            enGb->textId = 0x70F5;
            enGb->dyna.actor.parent = NULL;
            enGb->actionFunc = func_80A2F83C;
            *should = false;
        }
    });
    COND_ID_HOOK(OnOpenText, TEXT_BIG_POE_COLLECTED_RANDO, IS_RANDO, BuildBigPoeCollectedMessage);
}

static RegisterShipInitFunc initFunc(RegisterBigPoeHooks, { "IS_RANDO" });