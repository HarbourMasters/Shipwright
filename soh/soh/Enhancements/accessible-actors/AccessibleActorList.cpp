#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>
#include <macros.h>
#include <stdio.h>
#include <string>
#include <float.h>
//Declarations specific to chests.
#include "overlays/actors/ovl_En_Box/z_en_box.h"
extern "C" {

void EnBox_WaitOpen(EnBox*, PlayState*);
}

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
    EnBox* chest = (EnBox*)actor->actor;
    //Only chests that are "waiting to be opened" should play a sound. Chests which have not yet appeared (because some enemy has not been killed, switch has not been hit, etc) will not be in this action mode.
    if (chest->actionFunc != EnBox_WaitOpen)
        return;

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
    ActorAccessibilityPolicy
        policy; 
    ActorAccessibility_InitPolicy(policy, "Rock", accessible_en_ishi);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, policy);

    ActorAccessibility_InitPolicy(policy, "Kokiri Child", accessible_en_NPC_Gen);
    policy.n = Npc_Frames;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KO, policy);
//Mido and Saria can use the same configuration.
    policy.englishName = "Mido";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MD, policy);
    policy.englishName = "Saria";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SA, policy);
    policy.englishName = "Skull Kid";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SKJ, policy);
    policy.englishName = "Gossip Stone";
    policy.pitch = 0.75;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GS, policy);
    ActorAccessibility_InitPolicy(policy, "Bush", NULL, NA_SE_PL_PULL_UP_PLANT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KUSA, policy);
    ActorAccessibility_InitPolicy(policy, "Chest", accessible_en_chest);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOX, policy);
    ActorAccessibility_InitPolicy(policy, "Sign", accessible_en_Sign);
    policy.n = 40;
policy.pitch = 1.6;
ActorAccessibility_AddSupportedActor(ACTOR_EN_KANBAN, policy);
//ACTOR_EN_A_OBJ has exactly the same configuration.
    ActorAccessibility_AddSupportedActor(ACTOR_EN_A_OBJ, policy);
    // will probably just get replaced with ghost actors anyways
    // ActorAccessibility_AddSupporte dActor(ACTOR_EN_HOLL, "Room Changing Plane", NULL, 30, 500, 1.0, 1.0,
    //                                      NA_SE_EV_STONEDOOR_STOP /*NOT SURE YET*/);
    ActorAccessibility_InitPolicy(policy, "Bean patch", NULL, NA_SE_EN_MUSI_SINK);
    policy.n = 60;
    policy.distance = 2400;
    policy.pitch = 1.3;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BEAN, policy);
    ActorAccessibility_InitPolicy(policy, "Collectible", accessible_en_pickups);
    policy.n = 40;
    policy.pitch = 1.4;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ITEM00, policy);
    ActorAccessibility_InitPolicy(policy, "Gerudo Guard", accessible_en_gerudo);
    policy.n = Npc_Frames;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GE1, policy);
    ActorAccessibility_InitPolicy(policy, "Brown Bombable Rock", accessible_en_ishi);
    policy.n = 30;
    policy.pitch = 0.7;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BOMBIWA, policy); // Improve?
    ActorAccessibility_InitPolicy(policy, "Grotto Door", accessible_grotto);
    policy.n = 30;
    policy.pitch = 1.0;
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_ANA, policy);
    ActorAccessibility_InitPolicy(policy, "Web", NULL, NA_SE_EV_WEB_BROKEN);
    policy.distance = 5000;
    policy.pitch = 1.2;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_SP, policy);
                                         
    ActorAccessibility_InitPolicy(policy, "Shutter Door", accessible_door);
    policy.n = 30;
        policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_SHUTTER, policy);
        ActorAccessibility_InitPolicy(policy, "Switch", accessible_switch);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SWITCH, policy);
    ActorAccessibility_InitPolicy(policy, "Pushable Block", accessible_test);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_OSHIHIKI, policy);
    ActorAccessibility_InitPolicy(policy, "Torch", accessible_torches);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SYOKUDAI, policy);
    
    ActorAccessibility_InitPolicy(policy, "Pot", NULL, NA_SE_EV_POT_BROKEN);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_TSUBO, policy);
    //Virtual actor demo.
//First add support for an actor as you normally would.
    ActorAccessibility_InitPolicy(policy, "Proof of concept actor", accessible_va_prototype);
    policy.pitch = 0.5;
    ActorAccessibility_AddSupportedActor(VA_PROTOTYPE, policy);
    ActorAccessibility_InitPolicy(policy, "crawlspace", NULL, NA_SE_EN_MUSI_SINK);
    ActorAccessibility_AddSupportedActor(VA_CRAWLSPACE, policy);
    ActorAccessibility_InitPolicy(policy, "Ladder/climable", NULL, NA_SE_PL_CLIMB_CLIFF);
    policy.volume = 1.5;
    policy.pitch = 1.3;
    policy.distance = 5000;
    ActorAccessibility_AddSupportedActor(VA_CLIMB, policy);
    ActorAccessibility_InitPolicy(policy, "Door", NULL, NA_SE_OC_DOOR_OPEN);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(VA_DOOR, policy);
    ActorAccessibility_InitPolicy(policy, "Area Change", NULL,
                                  NA_SE_EV_HORSE_RUN_LEVEL); // make callback&find better sound
    ActorAccessibility_AddSupportedActor(VA_AREA_CHANGE, policy);
    // Now query a list of virtual actors for a given
                                                                // location (scene
                                                       // and room
                                           // number).
    VirtualActorList* list = ActorAccessibility_GetVirtualActorList(3, 17);//Forest temple basement.
//Now place the actor.
    ActorAccessibility_AddVirtualActor(list, VA_PROTOTYPE, { { 299.16, -779, -1807.22 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(85, 0); // Kokiri Forest
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -784.0, 120.0, 1046.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -547.0, 60.0, -1036.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -29.0, -80.0, 983.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -448.0, 0.0, -528.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -1082.0, 120.0, 383.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -27.0, 100.0, 1117.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 515.0, 0.0, 647.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 1046.0, 0.0, 549.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 848.0, 0.0, -323.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE,{ { -317.0, 373.2, -1542.00 }, {0, 14702, 0 }});
    ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE, { { -1380.0, -67.0, -288.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(85, 2); // Kokiri Forest Room with boulder and korkiri sword
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -788.0, 120.0, 1392.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(38, 0); //know-it-all house
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 12.0, 0.0, -131.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(40, 0); // mido house
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -6.6, 0.0, -179.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(52, 0); // link's house
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 2.3, 0.0, -134.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(41, 0); // saria's house
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 1.7, 0.0, -188.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(39, 0); // twins house
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 3.0, 0.0, -179.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(45, 0); // Kokiri Shop
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 0.0, 0.0, 150.00 }, { 0, 14702, 0 } });
        //Install cues for walls, ledges etc.
    ActorAccessibility_InitCues();

}
