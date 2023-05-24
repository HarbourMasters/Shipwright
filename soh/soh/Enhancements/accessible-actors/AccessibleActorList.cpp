#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>

    // Begin actor-specific policy callbacks.

void accessible_en_ishi(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_OCTAROCK_ROCK);
}

void accessible_en_NPC_Gen(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_VO_NB_LAUGH);

}

void accessible_en_Sign(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_IT_REFLECTION_WOOD);
}

void accessible_en_pickups(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_NUTS_DAMAGE);
}

void ActorAccessibility_Init() {
    const int Npc_Frames = 35;
    // ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, NA_SE_EN_OCTAROCK_ROCK, 20, 1000, 1.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, "Rock", accessible_en_ishi, 20, 1000, 1.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KO, "Kikori Children", accessible_en_NPC_Gen, Npc_Frames, 1000, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MD, "Mido", accessible_en_NPC_Gen, Npc_Frames, 1000, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SA, "Saria", accessible_en_NPC_Gen, Npc_Frames, 1000, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KUSA, "Bush", NULL, 20, 1000, 1.5, 1.0, NA_SE_PL_PULL_UP_PLANT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOX, "Chest", NULL, 20, 1000, 1.5, 1.0, NA_SE_EV_TBOX_UNLOCK);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KANBAN, "Sign", accessible_en_Sign, 40, 1000, 1.6);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_A_OBJ, "Sign", accessible_en_Sign, 40, 1000, 1.6);
    // will probably just get replaced with ghost actors anyways
    // ActorAccessibility_AddSupportedActor(ACTOR_EN_HOLL, "Room Changing Plane", NULL, 30, 1000, 1.0, 1.0,
    //                                      NA_SE_EV_STONEDOOR_STOP /*NOT SURE YET*/);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BEAN, "Bean patch", NULL, 60, 1000, 1.3, 1.0, NA_SE_EN_MUSI_WALK);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ITEM00, "Collectible", accessible_en_pickups, 40, 1000, 1.4);
}
