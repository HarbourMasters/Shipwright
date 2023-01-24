#include "GameInteractor.h"

// MARK: - State Definitions

bool GameInteractor::State::NoUIActive = 0;
bool GameInteractor::State::GiantLinkActive = 0;
bool GameInteractor::State::MinishLinkActive = 0;
bool GameInteractor::State::PaperLinkActive = 0;
bool GameInteractor::State::InvisibleLinkActive = 0;
bool GameInteractor::State::ResetLinkScale = 0;
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

// MARK: - GameInteractor::State::GiantLinkActive
uint8_t GameInteractor_GiantLinkActive() {
    return GameInteractor::State::GiantLinkActive;
}

// MARK: - GameInteractor::State::MinishLinkActive
uint8_t GameInteractor_MinishLinkActive() {
    return GameInteractor::State::MinishLinkActive;
}

// MARK: - GameInteractor::State::PaperLinkActive
uint8_t GameInteractor_PaperLinkActive() {
    return GameInteractor::State::PaperLinkActive;
}

// MARK: - GameInteractor::State::InvisibleLinkActive
uint8_t GameInteractor_InvisibleLinkActive() {
    return GameInteractor::State::InvisibleLinkActive;
}

void GameInteractor_SetInvisibleLinkActive(uint8_t active) {
    GameInteractor::State::InvisibleLinkActive = active;
}

// MARK: - GameInteractor::State::ResetLinkScale
uint8_t GameInteractor_ResetLinkScale() {
    return GameInteractor::State::ResetLinkScale;
}

void GameInteractor_SetResetLinkScale(uint8_t scale) {
    GameInteractor::State::ResetLinkScale = scale;
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
