#include "TimeSavers.h"
#include "soh/ShipInit.hpp"

void TimeSavers_Register() {
    // SkipCutscene
        // Story
            SkipBlueWarp_Register();
            SkipDekuTreeIntro_Register();
            SkipLostWoodsBridge_Register();
            SkipToGivingZeldasLetter_Register();
            SkipZeldaFleeingCastle_Register();
        SkipIntro_Register();
    // SkipMiscInteractions
        MoveJabuJabuElevator_Register();
        MoveMidoInKokiriForest_Register();
        SkipChildRutoInteractions_Register();
    FasterHeavyBlockLift_Register();
    FasterRupeeAccumulator_Register();
}

static RegisterShipInitFunc initFunc(TimeSavers_Register);
