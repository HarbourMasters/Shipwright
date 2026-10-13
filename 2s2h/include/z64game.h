#ifndef Z64GAME_H
#define Z64GAME_H

#include "ultra64.h"

#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "gamealloc.h"
#include "padmgr.h"
#include "padutils.h"
#include "tha.h"
#include "unk.h"

struct GraphicsContext;
struct GameState;


#define DEFINE_GAMESTATE_INTERNAL(typeName, enumName) enumName,
#define DEFINE_GAMESTATE(typeName, enumName, name) DEFINE_GAMESTATE_INTERNAL(typeName, enumName)

typedef enum GameStateId {
#include "tables/gamestate_table.h"
    /* 0x07 */ GAMESTATE_ID_MAX
} GameStateId;

#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL


typedef void (*GameStateFunc)(struct GameState* gameState);

typedef struct {
    /* 0x00 */ void*         loadedRamAddr;
    /* 0x04 */ uintptr_t     vromStart; // if applicable
    /* 0x08 */ uintptr_t     vromEnd;   // if applicable
    /* 0x0C */ void*         vramStart; // if applicable
    /* 0x10 */ void*         vramEnd;   // if applicable
    /* 0x14 */ UNK_PTR       unk_14;
    /* 0x18 */ GameStateFunc init;    // initializes and executes the given context
    /* 0x1C */ GameStateFunc destroy; // deconstructs the context, and sets the next context to load
    /* 0x20 */ UNK_PTR       unk_20;
    /* 0x24 */ UNK_PTR       unk_24;
    /* 0x28 */ UNK_TYPE      unk_28;
    /* 0x2C */ size_t        instanceSize;
} GameStateOverlay; // size = 0x30

typedef struct GameState {
    /* 0x00 */ struct GraphicsContext* gfxCtx;
    /* 0x04 */ GameStateFunc main;
    /* 0x08 */ GameStateFunc destroy;
    /* 0x0C */ GameStateFunc init; // Usually the current game state's init, though after stopping, the graph thread will look here to determine the next game state to load.
    /* 0x10 */ size_t size;
    /* 0x14 */ Input input[MAXCONTROLLERS];
    /* 0x74 */ TwoHeadArena tha;
    /* 0x84 */ GameAlloc alloc;
    /* 0x98 */ UNK_TYPE1 pad98[0x3];
    /* 0x9B */ u8 running; // If 0, switch to next game state
    /* 0x9C */ u32 frames;
    /* 0xA0 */ u8 padA0[0x2];
    /* 0xA2 */ u8 framerateDivisor;
    /* 0xA3 */ u8 unk_A3;
} GameState; // size = 0xA4


void GameState_UpdateFramerateDivisors(s32 divisor);
void GameState_SetFramerateDivisor(GameState* gameState, s32 divisor);
void GameState_SetFBFilter(Gfx** gfxP, void* zbuffer);
void GameState_Noop(GameState* gameState);
void GameState_Draw(GameState* gameState, struct GraphicsContext* gfxCtx);
void GameState_SetFrameBuffer(struct GraphicsContext* gfxCtx);
void GameState_DrawEnd(struct GraphicsContext* gfxCtx);
void GameState_GetInput(GameState* gameState);
void GameState_Update(GameState* gameState);
void GameState_IncrementFrameCount(GameState* gameState);
void GameState_InitArena(GameState* gameState, size_t size);
void GameState_Realloc(GameState* gameState, size_t size);
void GameState_Init(GameState* gameState, GameStateFunc init, struct GraphicsContext* gfxCtx);
void GameState_Destroy(GameState* gameState);
GameStateFunc GameState_GetInit(GameState* gameState);
size_t GameState_GetSize(GameState* gameState);
u32 GameState_IsRunning(GameState* gameState);
s32 GameState_GetArenaSize(GameState* gameState);
s32 func_80173B48(GameState* gameState);

extern s32 gFramerateDivisor;
extern f32 gFramerateDivisorF;
extern f32 gFramerateDivisorHalf;
extern f32 gFramerateDivisorThird;


extern GameStateOverlay gGameStateOverlayTable[GAMESTATE_ID_MAX];
extern GameStateId gGraphNumGameStates;


#define CONTROLLER1(gameState) (&(gameState)->input[0])
#define CONTROLLER2(gameState) (&(gameState)->input[1])
#define CONTROLLER3(gameState) (&(gameState)->input[2])
#define CONTROLLER4(gameState) (&(gameState)->input[3])

#define STOP_GAMESTATE(curState)     \
    do {                             \
        GameState* state = curState; \
                                     \
        state->running = false;      \
    } while(0)

#define SET_NEXT_GAMESTATE(curState, nextInit, nextSize) \
    do {                                                 \
        GameState* state = curState;                     \
                                                         \
        (state)->init = nextInit;                        \
        (state)->size = nextSize;                        \
    } while (0)


#endif
