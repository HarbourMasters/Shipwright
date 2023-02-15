#include "GameInteractor.h"

// MARK: - State Definitions

bool GameInteractor::State::NoUIActive = 0;
GILinkSize GameInteractor::State::LinkSize = GI_LINK_SIZE_NORMAL;
bool GameInteractor::State::InvisibleLinkActive = 0;
bool GameInteractor::State::OneHitKOActive = 0;
bool GameInteractor::State::PacifistModeActive = 0;
bool GameInteractor::State::DisableZTargetingActive = 0;
bool GameInteractor::State::ReverseControlsActive = 0;
int32_t GameInteractor::State::DefenseModifier = 0;
int32_t GameInteractor::State::RunSpeedModifier = 0;
GIGravityLevel GameInteractor::State::GravityLevel = GI_GRAVITY_LEVEL_NORMAL;

void GameInteractor::State::SetPacifistMode(bool active) {
    PacifistModeActive = active;
    // Force interface update to update the button's transparency.
    GameInteractor::RawAction::ForceInterfaceUpdate();
}

// MARK: C - Bridge

// MARK: - GameInteractor::State::NoUIActive
uint8_t GameInteractor_NoUIActive() {
    return GameInteractor::State::NoUIActive;
}

// MARK: - GameInteractor::State::LinkSize
GILinkSize GameInteractor_GetLinkSize() {
    return GameInteractor::State::LinkSize;
}

// MARK: - GameInteractor::State::LinkSize
void GameInteractor_SetLinkSize(GILinkSize size) {
    GameInteractor::State::LinkSize = size;
}

// MARK: - GameInteractor::State::InvisibleLinkActive
uint8_t GameInteractor_InvisibleLinkActive() {
    return GameInteractor::State::InvisibleLinkActive;
}

void GameInteractor_SetInvisibleLinkActive(uint8_t active) {
    GameInteractor::State::InvisibleLinkActive = active;
}

// MARK: - GameInteractor::State::OneHitKOActive
uint8_t GameInteractor_OneHitKOActive() {
    return GameInteractor::State::OneHitKOActive;
}

// MARK: - GameInteractor::State::PacifistModeActive
uint8_t GameInteractor_PacifistModeActive() {
    return GameInteractor::State::PacifistModeActive;
}

// MARK: - GameInteractor::State::DisableZTargetingActive
uint8_t GameInteractor_DisableZTargetingActive() {
    return GameInteractor::State::DisableZTargetingActive;
}

// MARK: - GameInteractor::State::DisableCameraRotationActive
uint8_t GameInteractor_ReverseControlsActive() {
    return GameInteractor::State::ReverseControlsActive;
}

// MARK: - GameInteractor::State::DisableCameraRotationActive
int32_t GameInteractor_DefenseModifier() {
    return GameInteractor::State::DefenseModifier;
}

// MARK: - GameInteractor::State::DisableCameraRotationActive
int32_t GameInteractor_RunSpeedModifier() {
    return GameInteractor::State::RunSpeedModifier;
}

// MARK: - GameInteractor::State::DisableCameraRotationActive
GIGravityLevel GameInteractor_GravityLevel() {
    return GameInteractor::State::GravityLevel;
}
