#ifndef ShuffleCrates_H
#define ShuffleCrates_H

#include <z64.h>
#include <soh/OTRGlobals.h>

#ifdef __cplusplus
extern "C" {
#endif
void ObjKibako2_RandomizerInit(void* actorRef);
#ifdef __cplusplus
};
#endif

void ShuffleCrates_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs);

#endif //ShuffleCrates_H