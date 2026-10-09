#ifndef Z64_SELECT_H
#define Z64_SELECT_H

#include "z64game.h"
#include "z64view.h"

struct MapSelectState;

typedef void (*SelectLoadFunc)(struct MapSelectState*, u32, s32);

typedef struct {
    /* 0x0 */ char* name;
    /* 0x4 */ SelectLoadFunc loadFunc;
    /* 0x8 */ s32 entrance;
} SceneSelectEntry; // size = 0xC

typedef struct MapSelectState {
    /* 0x000 */ GameState state;
    /* 0x0A8 */ View view;
    /* 0x210 */ s32 count;
    /* 0x214 */ SceneSelectEntry* scenes;
    /* 0x218 */ s32 currentScene;
    /* 0x21C */ s32 pageDownIndex; // Index of pageDownStops
    /* 0x220 */ s32 pageDownStops[7];
    /* 0x23C */ char unk_23C[0xC];
    /* 0x248 */ s32 opt; // (Option): This name is the one the menu uses to show this value. It can be used to change in which entrance the player spawns in the selected scene
    /* 0x24C */ s32 topDisplayedScene; // The scene which is currently at the top of the screen
    /* 0x250 */ char unk_250[0xC];
    /* 0x25C */ s32 verticalInputAccumulator;
    /* 0x260 */ s32 verticalInput;
    /* 0x264 */ s32 timerUp;
    /* 0x268 */ s32 timerDown;
    /* 0x26C */ s32 lockUp;
    /* 0x270 */ s32 lockDown;
    /* 0x274 */ s32 unk_274; // unused
    /* 0x278 */ UNK_TYPE1 unk_278[0x08];
} MapSelectState; // size = 0x280

void MapSelect_Init(GameState* thisx);
void MapSelect_Destroy(GameState* thisx);
void MapSelect_LoadGame(MapSelectState* thisx, u32 entrance, s32 spawn);
void MapSelect_LoadConsoleLogo(MapSelectState* thisx, u32 entrance, s32 spawn);

#endif
