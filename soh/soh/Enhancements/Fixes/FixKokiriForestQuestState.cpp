#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "src/overlays/actors/ovl_En_Ko/z_en_ko.h"
s32 EnKo_GetForestQuestState(EnKo*);
}

void RegisterFixKokiriForestQuestState() {
    COND_VB_SHOULD(VB_KOKIRI_GET_FOREST_QUEST_STATE2, CVarGetInteger(CVAR_ENHANCEMENT("FixKokiriForestQuestState"), 0),
                   {
                       EnKo* enKo = va_arg(args, EnKo*);
                       enKo->forestQuestState = EnKo_GetForestQuestState(enKo);
                       *should = false;
                   });
}

static RegisterShipInitFunc initFunc(RegisterFixKokiriForestQuestState,
                                     { CVAR_ENHANCEMENT("FixKokiriForestQuestState") });
