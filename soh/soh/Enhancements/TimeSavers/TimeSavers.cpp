#include "TimeSavers.h"

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
        MoveMidoInKokiriForest_Register();
    FasterHeavyBlockLift_Register();
}
