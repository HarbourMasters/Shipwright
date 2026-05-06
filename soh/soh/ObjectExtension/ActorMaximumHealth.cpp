#include "ActorMaximumHealth.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

struct ActorMaximumHealth {
    u8 maximumHealth = 0;
};
static ObjectExtension::Register<ActorMaximumHealth> ActorMaximumHealthRegister;

u8 GetActorMaximumHealth(const Actor* actor) {
    const ActorMaximumHealth* maxHealth = ObjectExtension::GetInstance().Get<ActorMaximumHealth>(actor);
    return maxHealth != nullptr ? maxHealth->maximumHealth : ActorMaximumHealth{}.maximumHealth;
}

void SetActorMaximumHealth(const Actor* actor, u8 maximumHealth) {
    ObjectExtension::GetInstance().Set<ActorMaximumHealth>(actor, ActorMaximumHealth{ maximumHealth });
}

static void ActorMaximumHealth_Register() {
    COND_HOOK(OnActorInit, true, [](void* ptr) {
        Actor* actor = static_cast<Actor*>(ptr);
        if (actor->category == ACTORCAT_ENEMY) {
            SetActorMaximumHealth(actor, actor->colChkInfo.health);
        }
    });
}

static RegisterShipInitFunc initFunc(ActorMaximumHealth_Register);
