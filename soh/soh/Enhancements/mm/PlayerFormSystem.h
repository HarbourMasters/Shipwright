#pragma once

#include <stdbool.h>

// MM-style transformation forms (subset of 2Ship's PlayerTransformation, remapped so HUMAN = 0).
typedef enum PlayerShipForm {
    PLAYER_SHIPFORM_HUMAN,
    PLAYER_SHIPFORM_GORON,
    PLAYER_SHIPFORM_ZORA,
    PLAYER_SHIPFORM_MAX,
} PlayerShipForm;

#define CVAR_TRANSFORMATION_MASKS_NAME CVAR_ENHANCEMENT("TransformationMasks")

#ifdef __cplusplus
extern "C" {
#endif

struct PlayState;
struct Player;

// Form currently baked into the player's skeleton
PlayerShipForm PlayerForm_GetApplied(void);
// Form the player should be in, derived from the equipped mask (and the developer override CVar)
PlayerShipForm PlayerForm_GetTarget(void);
// Whether the MM Goron/Zora Link skeletons are resolvable (i.e. a suitable mm.o2r is in mods/)
bool PlayerForm_AssetsAvailable(void);

void PlayerForm_Apply(struct PlayState* play, struct Player* player, PlayerShipForm form);
// Per-frame upkeep while transformed: ageProperties pointer, shadow scale, mass
void PlayerForm_EnforceInvariants(struct Player* player);

#ifdef __cplusplus
}
#endif
