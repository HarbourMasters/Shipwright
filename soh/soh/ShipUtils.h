#ifndef SHIP_UTILS_H
#define SHIP_UTILS_H

#include <libultraship/libultraship.h>
//#include "PR/ultratypes.h"

#ifdef __cplusplus

void LoadGuiTextures();

extern "C" {
#endif

struct PlayState;
struct Actor;

f32 Ship_GetExtendedAspectRatioMultiplier();
void Ship_ExtendedCullingActorAdjustProjectedZ(Actor* actor);
void Ship_ExtendedCullingActorAdjustProjectedX(Actor* actor);
void Ship_ExtendedCullingActorRestoreProjectedPos(PlayState* play, Actor* actor);

bool Ship_IsCStringEmpty(const char* str);
void Ship_CreateQuadVertexGroup(Vtx* vtxList, s32 xStart, s32 yStart, s32 width, s32 height, u8 flippedH);
f32 Ship_GetCharFontWidth(u8 character);
void* Ship_GetCharFontTexture(u8 character);

#ifdef __cplusplus
}
#endif

#endif // SHIP_UTILS_H
