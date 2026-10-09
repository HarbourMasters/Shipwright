#ifndef Z64GAME_OVER_H
#define Z64GAME_OVER_H

#include "ultra64.h"

struct PlayState;


typedef enum GameOverState {
    /*  0 */ GAMEOVER_INACTIVE,
    /*  1 */ GAMEOVER_DEATH_START,
    /*  2 */ GAMEOVER_DEATH_WAIT_GROUND, // wait for player to fall and hit the ground
    /*  3 */ GAMEOVER_DEATH_FADE_OUT, // wait before fading out
    /* 20 */ GAMEOVER_REVIVE_START = 20,
    /* 21 */ GAMEOVER_REVIVE_RUMBLE,
    /* 22 */ GAMEOVER_REVIVE_WAIT_GROUND, // wait for player to fall and hit the ground
    /* 23 */ GAMEOVER_REVIVE_WAIT_FAIRY, // wait for the fairy to rise all the way up out of player's body
    /* 24 */ GAMEOVER_REVIVE_FADE_OUT // fade out the game over lights as player is revived and gets back up
} GameOverState;

typedef struct GameOverContext {
    /* 0x0 */ u16 state;
} GameOverContext; // size = 0x2


void GameOver_Init(struct PlayState* play);
void GameOver_FadeLights(struct PlayState* play);
void GameOver_Update(struct PlayState* play);

#endif
