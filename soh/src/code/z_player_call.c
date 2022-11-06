#include "global.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25 | ACTOR_FLAG_26)

void (*sPlayerCallInitFunc)(Actor* thisx, PlayState* play);
void (*sPlayerCallDestroyFunc)(Actor* thisx, PlayState* play);
void (*sPlayerCallUpdateFunc)(Actor* thisx, PlayState* play);
void (*sPlayerCallDrawFunc)(Actor* thisx, PlayState* play);

void PlayerCall_Init(Actor* thisx, PlayState* play);
void PlayerCall_Destroy(Actor* thisx, PlayState* play);
void PlayerCall_Update(Actor* thisx, PlayState* play);
void PlayerCall_Draw(Actor* thisx, PlayState* play);

void Player_Init(Actor* thisx, PlayState* play);
void Player_Destroy(Actor* thisx, PlayState* play);
void Player_Update(Actor* thisx, PlayState* play);
void Player_Draw(Actor* thisx, PlayState* play);

const ActorInit Player_InitVars = {
    ACTOR_PLAYER,
    ACTORCAT_PLAYER,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(Player),
    (ActorFunc)PlayerCall_Init,
    (ActorFunc)PlayerCall_Destroy,
    (ActorFunc)PlayerCall_Update,
    (ActorFunc)PlayerCall_Draw,
    NULL,
};

void PlayerCall_InitFuncPtrs(void) {
    sPlayerCallInitFunc = KaleidoManager_GetRamAddr(Player_Init);
    sPlayerCallDestroyFunc = KaleidoManager_GetRamAddr(Player_Destroy);
    sPlayerCallUpdateFunc = KaleidoManager_GetRamAddr(Player_Update);
    sPlayerCallDrawFunc = KaleidoManager_GetRamAddr(Player_Draw);
}

void PlayerCall_Init(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();
    PlayerCall_InitFuncPtrs();
    sPlayerCallInitFunc(thisx, play);
}

void PlayerCall_Destroy(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallDestroyFunc(thisx, play);
}

void PlayerCall_Update(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallUpdateFunc(thisx, play);
}

void PlayerCall_Draw(Actor* thisx, PlayState* play) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallDrawFunc(thisx, play);
}
