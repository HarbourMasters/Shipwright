#include "GameInteractor.h"

// MARK: - GameInteractor::State::NoUIActive
uint32_t GameInteractor_NoUIActive() {
    return GameInteractor::State::NoUIActive;
}

// MARK: - GameInteractor::State::GiantLinkActive
uint32_t GameInteractor_GiantLinkActive() {
    return GameInteractor::State::GiantLinkActive;
}

// MARK: - GameInteractor::State::MinishLinkActive
uint32_t GameInteractor_MinishLinkActive() {
    return GameInteractor::State::MinishLinkActive;
}

// MARK: - GameInteractor::State::PaperLinkActive
uint32_t GameInteractor_PaperLinkActive() {
    return GameInteractor::State::PaperLinkActive;
}

// MARK: - GameInteractor::State::InvisibleLinkActive
uint32_t GameInteractor_InvisibleLinkActive() {
    return GameInteractor::State::InvisibleLinkActive;
}

// MARK: - GameInteractor::State::ResetLinkScale
uint32_t GameInteractor_ResetLinkScale() {
    return GameInteractor::State::ResetLinkScale;
}

void GameInteractor_SetResetLinkScale(uint32_t scale) {
    GameInteractor::State::ResetLinkScale = scale;
}

// MARK: - GameInteractor::State::OneHitKOActive
uint32_t GameInteractor_OneHitKOActive() {
    return GameInteractor::State::OneHitKOActive;
}

// MARK: - GameInteractor::State::PacifistModeActive
uint32_t GameInteractor_PacifistModeActive() {
    return GameInteractor::State::PacifistModeActive;
}

// MARK: - GameInteractor::State::DisableZTargetingActive
uint32_t GameInteractor_DisableZTargetingActive() {
    return GameInteractor::State::DisableZTargetingActive;
}

// MARK: - GameInteractor::State::DisableCameraRotationActive
uint32_t GameInteractor_ReverseControlsActive() {
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
uint32_t GameInteractor_GravityLevel() {
    return GameInteractor::State::GravityLevel;
}
