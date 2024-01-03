#ifndef _RANDO_GROTTO_H_
#define _RANDO_GROTTO_H_

#include "z64math.h"

#define NUM_GROTTOS GROTTO_OFFSET_MAX
#define NOT_GROTTO 0
#define GROTTO_LOAD 1
#define GROTTO_RETURN 2

typedef struct {
    s16 entranceIndex;
    s8 content;
    s8 scene;
} GrottoLoadInfo;

typedef struct {
    s16 entranceIndex;
    s8 room;
    s16 angle;
    Vec3f pos;
} GrottoReturnInfo;

void Grotto_InitExitAndLoadLists(void);
void Grotto_SetExitOverride(s16 originalIndex, s16 overrideIndex);
void Grotto_SetLoadOverride(s16 originalIndex, s16 overrideIndex);
s16  Grotto_OverrideSpecialEntrance(s16 nextEntranceIndex);
void Grotto_ForceGrottoReturnOnSpecialEntrance(void);
void Grotto_ForceGrottoReturn(void);
void Grotto_ForceRegularVoidOut(void);
void Grotto_SanitizeEntranceType(void);
s16 Grotto_GetRenamedGrottoIndexFromOriginal(s8 content, s8 scene);

#endif //_RANDO_GROTTO_H_
