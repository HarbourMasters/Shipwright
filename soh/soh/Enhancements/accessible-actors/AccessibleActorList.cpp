#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>
#include <macros.h>
#include <stdio.h>
#include <string>
void accessible_va_ledge_cue(AccessibleActor* actor);
void accessible_va_wall_cue(AccessibleActor* actor);

    // Begin actor-specific policy callbacks.

void accessible_en_ishi(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_OCTAROCK_ROCK);
}

void accessible_en_NPC_Gen(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_VO_NB_LAUGH);

}
void accessible_en_chest(AccessibleActor* actor) {
    s32 treasureFlag = actor->actor->params & 0x1F;

    if (!(treasureFlag >= 20 && treasureFlag < 32)) {
        ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_TBOX_UNLOCK);
    }
}

void accessible_en_gerudo(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_VO_NB_LAUGH);//update sound for ones that detect you
}

void accessible_en_Sign(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_IT_REFLECTION_WOOD);
}

void accessible_en_pickups(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_NUTS_DAMAGE);
}

void accessible_test(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_FOOT_SWITCH);
}

void accessible_grotto(AccessibleActor* actor) {
    if ((actor->actor->params & 0x300) == 0) {
        ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_DROP_FALL);
    }
}

void accessible_torches(AccessibleActor* actor) {
    if ((actor->actor->params) == 4230) {
        ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_ANUBIS_FIRE);
    }
}
void accessible_switch(AccessibleActor* actor) {
    ObjSwitch* sw = (ObjSwitch*)actor->actor;
    Vec3f& scale = actor->actor->scale;
    if ((actor->actor->params & 7) == 0) {
        if (scale.y >= 33.0f / 200.0f) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F)))) {
            ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_FOOT_SWITCH);
}
    }
    else if ((actor->actor->params & 7) == 1) {
        if (scale.y >= 33.0f / 200.0f) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F)))) {
            ActorAccessibility_PlaySpecialSound(actor, NA_SE_IT_HAMMER_HIT);
        }
    } 
    else if ((actor->actor->params & 7) == 2) {
        if (sw->eyeTexIndex == 0) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F))))
                                    // make it only play for open eye

            ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_FOOT_SWITCH);
        }
    }

    else {
        ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_DIAMOND_SWITCH);

    }

}

void accessible_door(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_OC_DOOR_OPEN);
}
void accessible_va_prototype(AccessibleActor* actor) {

    Player* player = GET_PLAYER(actor->play);
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_IT_BOMB_EXPLOSION);

}
void ActorAccessibility_Init() {
    const int Npc_Frames = 35;
    // ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, NA_SE_EN_OCTAROCK_ROCK, 20, 500, 1.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, "Rock", accessible_en_ishi, 20, 500, 1.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KO, "Kikiri Child", accessible_en_NPC_Gen, Npc_Frames, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MD, "Mido", accessible_en_NPC_Gen, Npc_Frames, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SA, "Saria", accessible_en_NPC_Gen, Npc_Frames, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KUSA, "Bush", NULL, 20, 500, 1.5, 1.0, NA_SE_PL_PULL_UP_PLANT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOX, "Chest", accessible_en_chest, 20, 500, 1.5, 1.0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KANBAN, "Sign", accessible_en_Sign, 40, 500, 1.6);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_A_OBJ, "Sign", accessible_en_Sign, 40, 500, 1.6);
    // will probably just get replaced with ghost actors anyways
    // ActorAccessibility_AddSupporte dActor(ACTOR_EN_HOLL, "Room Changing Plane", NULL, 30, 500, 1.0, 1.0,
    //                                      NA_SE_EV_STONEDOOR_STOP /*NOT SURE YET*/);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BEAN, "Bean patch", NULL, 60, 2400, 1.3, 1.0,
                                         NA_SE_EN_MUSI_SINK); 
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ITEM00, "Collectible", accessible_en_pickups, 40, 500, 1.4);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GS, "Gossip Stone", accessible_en_NPC_Gen, Npc_Frames, 500, 0.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GE1, "Gerudo Guard", accessible_en_gerudo, Npc_Frames, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SKJ, "Skull Kid", accessible_en_NPC_Gen, Npc_Frames, 500,
                                         1.1); 
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BOMBIWA, "Brown Bombable Rock", accessible_en_ishi, 30, 500, 0.7);//Improve?
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_ANA, "Grotto Door", accessible_grotto, 30, 500, 1.0); 
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_SP, "Webs", NULL, 20, 5000, 1.2, 1.0,
                                         NA_SE_EV_WEB_BROKEN);
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_SHUTTER, "Shutter Door", accessible_door, 30, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SWITCH, "Switch", accessible_switch, 30, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_OSHIHIKI, "Pushable Block", accessible_test, 30, 500, 1.1);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SYOKUDAI, "Torches", accessible_torches, 30, 500, 1.1);
    //Virtual actor demo.
//First add support for an actor as you normally would.
    ActorAccessibility_AddSupportedActor(VA_PROTOTYPE, "Proof of concept actor", accessible_va_prototype, 20, 500, 0.5);
    //Now query a list of virtual actors for a given location (scene and room number).
    VirtualActorList* list = ActorAccessibility_GetVirtualActorList(3, 17);//Forest temple basement.
//Now place the actor.
    ActorAccessibility_AddVirtualActor(list, VA_PROTOTYPE, { { 299.16, -779, -1807.22 }, { 0, 14702, 0 } });
//Install dcvz's ledge and wall cues.
    ActorAccessibility_AddSupportedActor(VA_LEDGE_CUE, "Ledge cue helper", accessible_va_ledge_cue, 1);
    ActorAccessibility_AddSupportedActor(VA_WALL_CUE, "Wall cue helper", accessible_va_wall_cue, 1);
    list = ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);
    ActorAccessibility_AddVirtualActor(list, VA_WALL_CUE, { { 0, 0, 0 }, { 0, 0, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_LEDGE_CUE, { { 0, 0, 0 }, { 0, 0, 0 } });
}
