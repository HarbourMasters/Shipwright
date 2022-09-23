#ifndef Z64INTERFACE_H
#define Z64INTERFACE_H

/**
 * Button HUD Positions (Upper Left)
 */
#define A_BUTTON_X 186
#define A_BUTTON_Y 9

#define B_BUTTON_X 160
#define B_BUTTON_Y 17

#define C_LEFT_BUTTON_X 227
#define C_LEFT_BUTTON_Y 18

#define C_DOWN_BUTTON_X 249
#define C_DOWN_BUTTON_Y 34

#define C_RIGHT_BUTTON_X 271
#define C_RIGHT_BUTTON_Y 18

#define C_UP_BUTTON_X 254
#define C_UP_BUTTON_Y 16

#define DPAD_UP_X 279
#define DPAD_UP_Y 47

#define DPAD_DOWN_X 279
#define DPAD_DOWN_Y 79

#define DPAD_LEFT_X 263
#define DPAD_LEFT_Y 63

#define DPAD_RIGHT_X 295
#define DPAD_RIGHT_Y 63

#define DPAD_X 271
#define DPAD_Y 55

/**
 * These are the colors for the hearts in the interface. The prim color is the red color of the heart
 * for the base hearts, while the prim color for the double defense hearts is the white outline. The
 * env color for the base hearts is the purple-ish outline, while the env color for the double defense
 * hearts is the red color of the hearts.
 */

#define HEARTS_PRIM_R 255
#define HEARTS_PRIM_G 70
#define HEARTS_PRIM_B 50

#define HEARTS_ENV_R 50
#define HEARTS_ENV_G 40
#define HEARTS_ENV_B 60

#define HEARTS_DD_PRIM_R 255
#define HEARTS_DD_PRIM_G 255
#define HEARTS_DD_PRIM_B 255

#define HEARTS_DD_ENV_R 200
#define HEARTS_DD_ENV_G 0
#define HEARTS_DD_ENV_B 0

/**
 * The burn and drown colors listed here are unused. Prerelease footage of the game confirms that at one
 * point in development the orange color was to be used while taking damage from hot environments.
 * Based on this, we can assume that the blue heart color was to be used while drowning.
 * In the final game these environments only have a timer and do not damage you continuously.
 */

#define HEARTS_BURN_PRIM_R 255
#define HEARTS_BURN_PRIM_G 190
#define HEARTS_BURN_PRIM_B 0

#define HEARTS_BURN_ENV_R 255
#define HEARTS_BURN_ENV_G 0
#define HEARTS_BURN_ENV_B 0

#define HEARTS_DROWN_PRIM_R 100
#define HEARTS_DROWN_PRIM_G 100
#define HEARTS_DROWN_PRIM_B 255

#define HEARTS_DROWN_ENV_R 0
#define HEARTS_DROWN_ENV_G 0
#define HEARTS_DROWN_ENV_B 255

#endif
