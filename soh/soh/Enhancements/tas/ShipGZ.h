#pragma once
/*
 * ShipGZ - a GZ-style tool-assisted-speedrun toolkit for Ship of Harkinian.
 *
 * Mirrors the macro / movie engine of gz (github.com/glankk/gz, src/gz/gz.c, gz_movie.c, gz_macro.c):
 *   - pause / frame advance               (gz: frames_queued, state_main_hook)
 *   - input macro recording and playback  (gz: input_hook, movie_input)
 *   - RNG reseed recording / restoring    (gz: srand_hook, movie_seed)
 *   - deterministic audio RNG for macros  (gz: afx_rand_hook)
 *   - savestates that carry the macro frame, rerecord counting, quick record / quick play
 *   - virtual controllers and bindable commands
 *
 * The C entry points below expose menu hotkeys and frame status to the UI and rendering layer;
 * gameplay and savestate integration is registered through GameInteractor hooks.
 */

#include <stdint.h>
#include <libultraship/libultra/controller.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- menu hotkeys / OSD --- */
void ShipGZ_InitHooks(void);
void ShipGZ_Update(int32_t commandCaptureActive);
int32_t ShipGZ_IsFrozen(void);
float ShipGZ_GetSpeed(void);

#ifdef __cplusplus
}
#endif
