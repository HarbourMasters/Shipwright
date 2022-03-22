#ifndef Z_KALEIDO_SCOPE_H
#define Z_KALEIDO_SCOPE_H

#include "ultra64.h"
#include "global.h"

extern u8 gAmmoItems[];
extern s16 D_8082AAEC[];
extern s16 D_8082AB2C[];
extern u8 gSlotAgeReqs[];
extern u8 gEquipAgeReqs[][4];
extern u8 gAreaGsFlags[];

void KaleidoScope_DrawQuestStatus(GlobalContext* globalCtx, GraphicsContext* gfxCtx);
s32 KaleidoScope_UpdateQuestStatusPoint(PauseContext* pauseCtx, s32 point);
void KaleidoScope_DrawDebugEditor(GlobalContext* globalCtx);
void KaleidoScope_DrawPlayerWork(GlobalContext* globalCtx);
void KaleidoScope_DrawEquipment(GlobalContext* globalCtx);
void KaleidoScope_SetCursorVtx(PauseContext* pauseCtx, u16 index, Vtx* vtx);
void KaleidoScope_DrawItemSelect(GlobalContext* globalCtx);
void KaleidoScope_UpdateItemEquip(GlobalContext* globalCtx);
void KaleidoScope_DrawDungeonMap(GlobalContext* globalCtx, GraphicsContext* gfxCtx);
void KaleidoScope_DrawWorldMap(GlobalContext* globalCtx, GraphicsContext* gfxCtx);
void KaleidoScope_UpdatePrompt(GlobalContext* globalCtx);
Gfx* KaleidoScope_QuadTextureIA4(Gfx* gfx, void* texture, s16 width, s16 height, u16 point);
Gfx* KaleidoScope_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, u16 point);
void KaleidoScope_MoveCursorToSpecialPos(GlobalContext* globalCtx, u16 specialPos);
void KaleidoScope_DrawQuadTextureRGBA32(GraphicsContext* gfxCtx, void* texture, u16 width, u16 height, u16 point);
void KaleidoScope_ProcessPlayerPreRender();
void KaleidoScope_SetupPlayerPreRender(GlobalContext* globalCtx);
void KaleidoScope_DrawCursor(GlobalContext* globalCtx, u16 pageIndex);
void KaleidoScope_UpdateDungeonMap(GlobalContext* globalCtx);

void PauseMapMark_Draw(GlobalContext* globalCtx);

void KaleidoScope_UpdateCursorSize(PauseContext* pauseCtx);

#endif
