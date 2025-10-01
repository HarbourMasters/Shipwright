#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "./enhancementTypes.h"

extern "C" {
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

void RegisterBonkDamage() {
    COND_HOOK(OnPlayerBonk, CVarGetInteger(CVAR_ENHANCEMENT("BonkDamageMult"), BONK_DAMAGE_NONE) != BONK_DAMAGE_NONE,
              [] {
                  uint16_t bonkDamage = 0;
                  switch (CVarGetInteger(CVAR_ENHANCEMENT("BonkDamageMult"), BONK_DAMAGE_NONE)) {
                      case BONK_DAMAGE_NONE:
                          return;
                      case BONK_DAMAGE_OHKO:
                          gSaveContext.health = 0;
                          return;
                      case BONK_DAMAGE_QUARTER_HEART:
                          bonkDamage = 4;
                          break;
                      case BONK_DAMAGE_HALF_HEART:
                          bonkDamage = 8;
                          break;
                      case BONK_DAMAGE_1_HEART:
                          bonkDamage = 16;
                          break;
                      case BONK_DAMAGE_2_HEARTS:
                          bonkDamage = 32;
                          break;
                      case BONK_DAMAGE_4_HEARTS:
                          bonkDamage = 64;
                          break;
                      case BONK_DAMAGE_8_HEARTS:
                          bonkDamage = 128;
                          break;
                      default:
                          break;
                  }

                  Health_ChangeBy(gPlayState, -bonkDamage);
                  // Set invincibility to make Link flash red as a visual damage indicator.
                  Player* player = GET_PLAYER(gPlayState);
                  player->invincibilityTimer = 28;
              });
}

static RegisterShipInitFunc registerBonkDamage(RegisterBonkDamage, { CVAR_ENHANCEMENT("BonkDamageMult") });
