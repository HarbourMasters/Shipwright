#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>
#include <macros.h>
#include <stdio.h>
#include <string>
#include <float.h>
#include "overlays\actors\ovl_Boss_Goma\z_boss_goma.h"
//Declarations specific to chests.
#include "overlays/actors/ovl_En_Box/z_en_box.h"
extern "C" {

void EnBox_WaitOpen(EnBox*, PlayState*);
}

void accessible_va_ledge_cue(AccessibleActor* actor);
void accessible_va_wall_cue(AccessibleActor* actor);

    // Begin actor-specific policy callbacks.

void accessible_en_ishi(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_OCTAROCK_ROCK, false);
    //ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_OCTAROCK_ROCK);
}

void accessible_en_NPC_Gen(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_VO_NB_LAUGH, false);

}
void accessible_en_chest(AccessibleActor* actor) {
    EnBox* chest = (EnBox*)actor->actor;
    //Only chests that are "waiting to be opened" should play a sound. Chests which have not yet appeared (because some enemy has not been killed, switch has not been hit, etc) will not be in this action mode.
    if (chest->actionFunc != EnBox_WaitOpen)
        return;

    s32 treasureFlag = actor->actor->params & 0x1F;

    if (!(treasureFlag >= 20 && treasureFlag < 32)) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_TBOX_UNLOCK, false);
    }
}

void accessible_en_gerudo(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_VO_NB_LAUGH, false);//update sound for ones that detect you
}

void accessible_en_Sign(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_REFLECTION_WOOD, false);
}

void accessible_en_pickups(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_DAMAGE, false);
}

void accessible_test(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_FOOT_SWITCH, false);
}

void accessible_grotto(AccessibleActor* actor) {
    if ((actor->actor->params & 0x300) == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DROP_FALL, false);
    }
}

void accessible_torches(AccessibleActor* actor) {
    if ((actor->actor->params) == 4230 || (actor->actor->params) == 4220 || (actor->actor->params) == 4227) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_BOMB_IGNIT, false);
    }
    if ((actor->actor->params) == 9216 || (actor->actor->params) == 962) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_ANUBIS_FIRE, false);
    }
}

void accessible_hasi(AccessibleActor* actor) {
    if ((actor->actor->params) == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_BLOCK_SHAKE, false);
    }

    else if ((actor->actor->params) == 1) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_PL_DAMAGE, false);
    }
}
    void accessible_switch(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    ObjSwitch* sw = (ObjSwitch*)actor->actor;
    Vec3f& scale = actor->actor->scale;
    if ((actor->actor->params & 7) == 0) {
        if (scale.y >= 33.0f / 200.0f) {
            if (actor->play->sceneNum == 0 && actor->play->roomCtx.curRoom.num == 5 && actor->xzDistToPlayer < 20) {
                ActorAccessibility_PlaySound(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);//Should result in same behaviour.
            }
            ActorAccessibility_PlaySoundForActor(actor, 1, NA_SE_EV_FOOT_SWITCH, false);
}
    }
    else if ((actor->actor->params & 7) == 1) {
        if (scale.y >= 33.0f / 200.0f) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F)))) {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_HAMMER_HIT, false);
        }
    } 
    else if ((actor->actor->params & 7) == 2) {
        if (sw->eyeTexIndex == 0) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F))))
                                    // make it only play for open eye

            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_FOOT_SWITCH, false);
        }
    }

    else {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);

    }

}

void accessible_larva(AccessibleActor* actor) {
    if (actor->actor->bgCheckFlags == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_GOMA_BJR_EGG1, false);
    }
}

void accessible_door(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_OC_DOOR_OPEN, false);
}
void accessible_va_prototype(AccessibleActor* actor) {

    Player* player = GET_PLAYER(actor->play);
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_BOMB_EXPLOSION, false);

}

void accessible_maruta(AccessibleActor* actor) {
    if (actor->actor->params == 1) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_PL_LAND_LADDER, false);
    }
}

void accessible_area_change(AccessibleActor* actor) {
    if (actor->variety == AREA_KOKIRI) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SARIA_MELODY, false);
    } else if (actor->variety == AREA_HYRULE_FIELD) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_HORSE_RUN_LEVEL, false);
    }
}

void accessible_231_dekus(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_FAINT, false);
    if (actor->actor->params == 1) {
        ActorAccessibility_SetSoundPitch(actor, 0, 1.0);
    }
    if (actor->actor->params == 2) {
        ActorAccessibility_SetSoundPitch(actor, 0, 0.5);
    }
    if (actor->actor->params == 3) {
        ActorAccessibility_SetSoundPitch(actor, 0, 1.5);
    }

}

void accessible_hana(AccessibleActor* actor) {
    if (actor->actor->params == 1) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_OCTAROCK_ROCK, false);
    }

}

void accessible_goma(AccessibleActor* actor) {
    BossGoma* goma = (BossGoma*)actor->actor;
    if (goma->visualState == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);
    }
}

void accessible_door_of_time(AccessibleActor* actor) {
    //ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EV_DIAMOND_SWITCH);
}

void ActorAccessibility_InitActors() {
    const int Npc_Frames = 35;
    ActorAccessibilityPolicy
        policy; 
    ActorAccessibility_InitPolicy(&policy, "Rock", accessible_en_ishi, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, policy);

    ActorAccessibility_InitPolicy(&policy, "Kokiri Child", accessible_en_NPC_Gen, 0);
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
    ActorAccessibility_InitPolicy(&policy, "Bush", NULL, NA_SE_PL_PULL_UP_PLANT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KUSA, policy);
    ActorAccessibility_InitPolicy(&policy, "Chest", accessible_en_chest, 0);
    policy.pitch = 1.1;
    policy.distance = 1000;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOX, policy);
    ActorAccessibility_InitPolicy(&policy, "Sign", accessible_en_Sign, 0);
    policy.n = 40;
    policy.pitch = 1.6;
    policy.distance = 800;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KANBAN, policy);
    //ACTOR_EN_A_OBJ has exactly the same configuration.
    ActorAccessibility_AddSupportedActor(ACTOR_EN_A_OBJ, policy);
    // will probably just get replaced with ghost actors anyways
    // ActorAccessibility_AddSupporte dActor(ACTOR_EN_HOLL, "Room Changing Plane", NULL, 30, 500, 1.0, 1.0,
    //                                      NA_SE_EV_STONEDOOR_STOP /*NOT SURE YET*/);
    ActorAccessibility_InitPolicy(&policy, "Bean patch", NULL, NA_SE_EN_MUSI_SINK);
    policy.n = 60;
    policy.distance = 2400;
    policy.pitch = 1.3;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BEAN, policy);
    ActorAccessibility_InitPolicy(&policy, "Collectible", accessible_en_pickups, 0);
    policy.n = 40;
    policy.pitch = 1.4;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ITEM00, policy);
    ActorAccessibility_InitPolicy(&policy, "Gerudo Guard", accessible_en_gerudo, 0);
    policy.n = Npc_Frames;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GE1, policy);
    ActorAccessibility_InitPolicy(&policy, "Brown Bombable Rock", accessible_en_ishi, 0);
    policy.n = 30;
    policy.pitch = 0.7;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BOMBIWA, policy); // Improve?
    ActorAccessibility_InitPolicy(&policy, "Grotto Door", accessible_grotto, 0);
    policy.n = 30;
    policy.pitch = 1.0;
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_ANA, policy);
    ActorAccessibility_InitPolicy(&policy, "Web", NULL, NA_SE_EV_WEB_BROKEN);
    policy.n = 40;
    policy.distance = 2000;
    policy.pitch = 1.2;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_SP, policy);
                                         
    ActorAccessibility_InitPolicy(&policy, "Shutter Door", accessible_door, 0);
    policy.n = 30;
        policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_SHUTTER, policy);
        ActorAccessibility_InitPolicy(&policy, "Switch", accessible_switch, 0);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SWITCH, policy);
    ActorAccessibility_InitPolicy(&policy, "Pushable Block", accessible_test, 0);
    policy.n = 30;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_OSHIHIKI, policy);
    ActorAccessibility_InitPolicy(&policy, "Torch", accessible_torches, 0);
    policy.n = 50;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SYOKUDAI, policy);
    ActorAccessibility_InitPolicy(&policy, "Deku Tree Moving Platform", accessible_hasi, 0);
    policy.volume = 1.3;
    policy.distance = 2000;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_HASI, policy);
    ActorAccessibility_InitPolicy(&policy, "Pot", NULL, NA_SE_EV_POT_BROKEN);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_TSUBO, policy);
    ActorAccessibility_InitPolicy(&policy, "Deku Tree Entrance", NULL, NA_SE_EV_FANTOM_WARP_L);
    policy.distance = 5000;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_TREEMOUTH, policy);
    ActorAccessibility_InitPolicy(&policy, "Platform collapsable", NULL, NA_SE_EV_BLOCK_SHAKE);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_LIFT, policy);
    ActorAccessibility_InitPolicy(&policy, "Ladder in Slingshot Room", accessible_maruta, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_MARUTA, policy);
    ActorAccessibility_InitPolicy(&policy, "bombable wall", NULL, NA_SE_EN_OCTAROCK_ROCK);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_BREAKWALL, policy);
    ActorAccessibility_InitPolicy(&policy, "231 dekus", accessible_231_dekus, 0);
    policy.distance = 2000;
    policy.n = 50;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HINTNUTS, policy);
    ActorAccessibility_InitPolicy(&policy, "uninteractable rocks in kokiri forest", accessible_hana, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_HANA, policy);
    ActorAccessibility_InitPolicy(&policy, "gold skulltula token", accessible_en_pickups, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SI, policy);
    ActorAccessibility_InitPolicy(&policy, "goma larva egg", accessible_larva, 0);
    policy.distance = 1000;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GOMA, policy);
    ActorAccessibility_InitPolicy(&policy, "heart canister", accessible_en_pickups, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_ITEM_B_HEART, policy);
    ActorAccessibility_InitPolicy(&policy, "Goma", accessible_goma, 0);
    policy.distance = 5000;
    ActorAccessibility_AddSupportedActor(ACTOR_BOSS_GOMA, policy);

    ActorAccessibility_InitPolicy(&policy, "door of time", accessible_door_of_time, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_MJIN, policy);

    ActorAccessibility_InitPolicy(&policy, "crawlspace", NULL, NA_SE_EN_MUSI_SINK);
    policy.volume = 1.5;
    policy.distance = 2000;
    ActorAccessibility_AddSupportedActor(VA_CRAWLSPACE, policy);
    //ActorAccessibility_InitPolicy(&policy, "Ladder/climable", NULL, NA_SE_PL_LAND_LADDER);
    //policy.volume = 1.5;
    //policy.pitch = 1.3;
    //policy.distance = 2000;
    //ActorAccessibility_AddSupportedActor(VA_CLIMB, policy);
    ActorAccessibility_InitPolicy(&policy, "Door", NULL, NA_SE_OC_DOOR_OPEN);
    policy.n = 30;
    policy.pitch = 1.1;
    policy.distance = 1000;
    ActorAccessibility_AddSupportedActor(VA_DOOR, policy);
    ActorAccessibility_InitPolicy(&policy, "Area Change", accessible_area_change, 0);
    policy.distance = 5000;
    ActorAccessibility_AddSupportedActor(VA_AREA_CHANGE, policy);
    ActorAccessibility_InitPolicy(&policy, "marker", NULL,
                                  NA_SE_EV_DIAMOND_SWITCH); 
    policy.pitch = 1.7;
    ActorAccessibility_AddSupportedActor(VA_MARKER, policy);
    // Now query a list of virtual actors for a given
                                                                // location (scene
                                                       // and room
                                           // number).
                                                VirtualActorList* list = ActorAccessibility_GetVirtualActorList(85, 0); // Kokiri Forest

//Now place the actor.
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -784.0, 120.0, 1046.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -547.0, 60.0, -1036.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -29.0, -80.0, 983.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -448.0, 0.0, -528.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -1082.0, 120.0, 383.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -27.0, 100.0, 1117.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 515.0, 0.0, 647.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 1046.0, 0.0, 549.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 848.0, 0.0, -323.00 }, { 0, 14702, 0 } });
    
    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE,{ { -317.0, 373.2, -1542.00 }, {0, 14702, 0 }}, AREA_KORIRI);
 
    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE, { { -1380.0, -67.0, -288.00 }, { 0, 14702, 0 } }, AREA_HYRULE_FIELD);

    list = ActorAccessibility_GetVirtualActorList(85, 2); // Kokiri Forest Room with boulder and kokiri sword
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

    list = ActorAccessibility_GetVirtualActorList(0, 0);//deku tree main room
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -226.7, 0, 197.0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 118.6, 0, -286.6 } });

    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE, { {0, 0, 640} }, AREA_KORIRI);
    
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 287.4, 368.0, 347.0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 419.4, 368.0, 173.6 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 323, 567.0, 314.6 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 127.5, 897.0, 433.6 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 440.9, 897.0, 101.6 } });

    list = ActorAccessibility_GetVirtualActorList(0, 2); // deku tree slingshot room
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1159, 288.0, 1403.0 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1179.6, 480.0, 1463.6 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1398.9, 288.0, 1161.6 } });

    list = ActorAccessibility_GetVirtualActorList(0, 10); // deku tree compass room
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -762, 733.0, 151.0 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -935, 780.0, -113 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1031.0, 800.0, 109.7 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1184, 820.0, -103.4 } });

    list = ActorAccessibility_GetVirtualActorList(0, 7); // deku tree bombable wall room
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -1209, -820.0, 3.5 } });

    list = ActorAccessibility_GetVirtualActorList(0, 3); // deku tree basement 1 lobby
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -901, -820.0, 0.5 } });
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 108, -919.5, 5.0 } });

    list = ActorAccessibility_GetVirtualActorList(0, 9); // deku tree b2 lobby
    ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -639, -1912.5, 188.0 } });
        //Install cues for walls, ledges etc.
    ActorAccessibility_InitCues();

}
