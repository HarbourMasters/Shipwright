#include "AudioSettings.h"

#include <atomic>

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/ShipInit.hpp"

static std::atomic<float> sMasterVolume{ 40.0f / 100.0f };
static std::atomic<int32_t> sOctaveDrop{ 0 };
static std::atomic<int32_t> sMirroredWorld{ 0 };

float AudioSettings_GetMasterVolume(void) {
    return sMasterVolume.load(std::memory_order_relaxed);
}

int32_t AudioSettings_GetOctaveDrop(void) {
    return sOctaveDrop.load(std::memory_order_relaxed);
}

int32_t AudioSettings_GetMirroredWorld(void) {
    return sMirroredWorld.load(std::memory_order_relaxed);
}

static void RefreshAudioSettings() {
    sMasterVolume.store((float)CVarGetInteger(CVAR_SETTING("Volume.Master"), 40) / 100.0f, std::memory_order_relaxed);
    sOctaveDrop.store(CVarGetInteger(CVAR_AUDIO("ExperimentalOctaveDrop"), 0), std::memory_order_relaxed);
    sMirroredWorld.store(CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0), std::memory_order_relaxed);
}

static RegisterShipInitFunc initAudioSettings(RefreshAudioSettings,
                                              { CVAR_SETTING("Volume.Master"), CVAR_AUDIO("ExperimentalOctaveDrop"),
                                                CVAR_ENHANCEMENT("MirroredWorld") });
