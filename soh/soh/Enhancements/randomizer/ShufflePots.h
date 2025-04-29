#ifndef SHUFFLEPOTS_H
#define SHUFFLEPOTS_H

#include <z64.h>
#include <soh/OTRGlobals.h>

#ifdef __cplusplus
extern "C" {
#endif
void ObjTsubo_RandomizerInit(void* actorRef);
#ifdef __cplusplus
};
#endif

void ShufflePots_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs);

#endif // SHUFFLEPOTS_H
