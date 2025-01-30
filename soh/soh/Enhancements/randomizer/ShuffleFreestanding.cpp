#include "ShuffleFreestanding.h"

extern "C" {
#include "functions.h"
extern PlayState* gPlayState;
}

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

void ShuffleFreestanding_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    if (id == VB_ITEM00_DESPAWN) {
        EnItem00* item00 = va_arg(args, EnItem00*);

        // Heart pieces and small keys are handled by default non-freestanding shuffles.
        if (item00->actor.params == ITEM00_HEART_PIECE || item00->actor.params == ITEM00_SMALL_KEY) {
            return;
        }

        uint32_t params = TWO_ACTOR_PARAMS((int32_t)item00->actor.world.pos.x, (int32_t)item00->actor.world.pos.z);
        Rando::Location* loc = OTRGlobals::Instance->gRandomizer->GetCheckObjectFromActor(item00->actor.id, gPlayState->sceneNum, params);
        uint8_t isDungeon = loc->IsDungeon();
        uint8_t freestandingSetting =
            Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_FREESTANDING).Get();
        RandomizerCheck randomizerCheck = loc->GetRandomizerCheck();
        bool checkObtained = Rando::Context::GetInstance()->GetItemLocation(randomizerCheck)->HasObtained();
        
        // Don't change to randomized item if current freestanding item isn't shuffled or already obtained.
        if ((freestandingSetting == RO_SHUFFLE_FREESTANDING_OVERWORLD && isDungeon) ||
            (freestandingSetting == RO_SHUFFLE_FREESTANDING_DUNGEONS && !isDungeon) || 
            checkObtained || randomizerCheck == RC_UNKNOWN_CHECK) {
            return;
        }

        item00->randoInf = static_cast<RandomizerInf>(loc->GetCollectionCheck().flag);
        item00->randoCheck = randomizerCheck;
        item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(randomizerCheck, true);
        item00->actor.params = ITEM00_SOH_DUMMY;
        item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;

        *should = false;
    }
}
