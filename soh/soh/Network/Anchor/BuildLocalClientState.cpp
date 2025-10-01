#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"

AnchorClient Anchor::BuildLocalClientState() {
    AnchorClient self;
    self.clientId = getOwnClientId();
    self.name = CVarGetString(CVAR_REMOTE_ANCHOR("Name"), "");
    self.color = CVarGetColor24(CVAR_REMOTE_ANCHOR("Color"), { 100, 255, 100 });
    self.clientVersion = clientVersion;
    self.teamId = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    self.online = true;
    self.self = true;

    if (IsSaveLoaded()) {
        self.seed = IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : 0;
        self.isSaveLoaded = true;
        self.isGameComplete = gSaveContext.ship.stats.gameComplete;
        self.sceneNum = gPlayState->sceneNum;
        self.entranceIndex = gSaveContext.entranceIndex;
    } else {
        self.seed = 0;
        self.isSaveLoaded = false;
        self.isGameComplete = false;
        self.sceneNum = SCENE_ID_MAX;
        self.entranceIndex = 0x00;
    }

    return self;
}

#endif //ENABLE_REMOTE_CONTROL