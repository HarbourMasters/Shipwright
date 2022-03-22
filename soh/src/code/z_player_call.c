#include "global.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25 | ACTOR_FLAG_26)

void (*sPlayerCallInitFunc)(Actor* thisx, GlobalContext* globalCtx);
void (*sPlayerCallDestroyFunc)(Actor* thisx, GlobalContext* globalCtx);
void (*sPlayerCallUpdateFunc)(Actor* thisx, GlobalContext* globalCtx);
void (*sPlayerCallDrawFunc)(Actor* thisx, GlobalContext* globalCtx);

void PlayerCall_Init(Actor* thisx, GlobalContext* globalCtx);
void PlayerCall_Destroy(Actor* thisx, GlobalContext* globalCtx);
void PlayerCall_Update(Actor* thisx, GlobalContext* globalCtx);
void PlayerCall_Draw(Actor* thisx, GlobalContext* globalCtx);

void Player_Init(Actor* thisx, GlobalContext* globalCtx);
void Player_Destroy(Actor* thisx, GlobalContext* globalCtx);
void Player_Update(Actor* thisx, GlobalContext* globalCtx);
void Player_Draw(Actor* thisx, GlobalContext* globalCtx);

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

void PlayerCall_Init(Actor* thisx, GlobalContext* globalCtx) {
    KaleidoScopeCall_LoadPlayer();
    PlayerCall_InitFuncPtrs();
    sPlayerCallInitFunc(thisx, globalCtx);
}

void PlayerCall_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallDestroyFunc(thisx, globalCtx);
}

void PlayerCall_Update(Actor* thisx, GlobalContext* globalCtx) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallUpdateFunc(thisx, globalCtx);
}

void PlayerCall_Draw(Actor* thisx, GlobalContext* globalCtx) {
    KaleidoScopeCall_LoadPlayer();
    sPlayerCallDrawFunc(thisx, globalCtx);
}
