#include "MiscBehavior.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

void Rando::MiscBehavior::OnSceneInit(s16 sceneId, s8 spawnNum) {
    InterfaceContext* interfaceCtx = &gPlayState->interfaceCtx;

    // Remove mask restrictions for Clock Tower rooftop
    if (sceneId == SCENE_OKUJOU) {
        interfaceCtx->restrictions.masks = 0;
    }
}
