#include "variables.h"
#include "functions.h"
#include "custom-message/CustomMessageTypes.h"
#include "global.h"
#include "macros.h"

#include "luslog.h"


static const int warpTextMap[] = { 
    TEXT_WARP_MINUET_OF_FOREST, 
    TEXT_WARP_BOLERO_OF_FIRE, 
    TEXT_WARP_SERENADE_OF_WATER, 
    TEXT_WARP_REQUIEM_OF_SPIRIT, 
    TEXT_WARP_NOCTURNE_OF_SHADOW, 
    TEXT_WARP_PRELUDE_OF_LIGHT 
};

void PauseWarp_Text() {
    LUSLOG_CRITICAL("PauseWarp_Text called\n");
}

void Start_WarpTextbox() {
}

void PauseWarp_Main() {
    LUSLOG_CRITICAL("PauseWarp_Main called\n");
}
