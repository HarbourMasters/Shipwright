#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// cvars used by the audio thread should go here
// direct use can result in a race condition with the main thread
float AudioSettings_GetMasterVolume(void);
int32_t AudioSettings_GetOctaveDrop(void);
int32_t AudioSettings_GetMirroredWorld(void);

#ifdef __cplusplus
}
#endif
