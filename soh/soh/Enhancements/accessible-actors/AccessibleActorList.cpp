#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>
#include <macros.h>
#include <stdio.h>
#include <string>
#include <float.h>
#include "overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
//Declarations specific to chests.
#include "overlays/actors/ovl_En_Box/z_en_box.h"
extern "C" {

void EnBox_WaitOpen(EnBox*, PlayState*);
}
//Declarations specific to Babas.
#include "overlays/actors/ovl_En_Karebaba/z_en_karebaba.h"
extern "C" {

void EnKarebaba_DeadItemDrop(EnKarebaba*, PlayState*);
}
//Declarations specific to Torches
#include "overlays\actors\ovl_Obj_Syokudai\z_obj_syokudai.h"
//Declarations specific to dogs
#include "overlays\actors\ovl_En_Dog\z_en_dog.h"
extern "C" {
void EnDog_FollowPlayer(EnDog*, PlayState*);
s8 EnDog_CanFollow(EnDog*, PlayState*);
}
    //User data for the general helper VA.
typedef struct
{
    s16 currentScene;
    s8 currentRoom;
    bool currentRoomClear;

}GeneralHelperData;
typedef struct
{
    f32 linearVelocity;
    int framesUntilChime;

}AudioCompassData;
typedef struct
{
    int framesUntilAboveChime;

}SwitchData;

    // Begin actor-specific policy callbacks.

void accessible_en_ishi(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_OCTAROCK_ROCK, false);
    //ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_OCTAROCK_ROCK);
}

void accessible_en_NPC_Gen(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_VO_NB_LAUGH, false);

}
void accessible_en_chest(AccessibleActor* actor) {
   
    
    Player* player = GET_PLAYER(actor->play);
    EnBox* chest = (EnBox*)actor->actor;
    if (chest->actionFunc != EnBox_WaitOpen)
        return;
    s32 treasureFlag = actor->actor->params & 0x1F;
    s8 size;
    if (chest->type <= 8 && chest->type >= 5) {
        size = 15; // small
    } else {
        size = 30;//large
    }
    if (!(treasureFlag >= 20 && treasureFlag < 32)) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_TBOX_UNLOCK, false);
    }
    //Only chests that are "waiting to be opened" should play a sound. Chests which have not yet appeared (because some enemy has not been killed, switch has not been hit, etc) will not be in this action mode.
    f32 leftAngle = actor->actor->world.rot.y - 16384;
    f32 velocityXRight = Math_SinS(leftAngle);
    f32 velocityZRight = Math_CosS(leftAngle);
    
    f32 frontAngle = actor->actor->world.rot.y;
    f32 velocityXFront = Math_SinS(frontAngle);
    f32 velocityZFront = Math_CosS(frontAngle);

    f32 xdist = (player->actor.world.pos.x - actor->actor->world.pos.x) * velocityXFront + 
        (player->actor.world.pos.z-actor->actor->world.pos.z) * velocityZFront;
    f32 zdist = fabs((player->actor.world.pos.x - actor->actor->world.pos.x) * velocityXRight + 
        (player->actor.world.pos.z-actor->actor->world.pos.z) * velocityZRight);


    if ((xdist - size / 2) < 0) {
        ActorAccessibility_SetSoundPitch(actor, 0, 0.5);
    } else if ((xdist + size / 2) > 0 && zdist < size / 2 && xdist < 150.0) {
        ActorAccessibility_PlaySoundForActor(actor, 1, NA_SE_EV_DIAMOND_SWITCH, false);
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
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_TRAP_BOUND, false);
}

void accessible_grotto(AccessibleActor* actor) {
    if ((actor->actor->params & 0x300) == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DROP_FALL, false);
    }
}

void accessible_torches(AccessibleActor* actor) {

    ObjSyokudai* torche = (ObjSyokudai*)actor->actor;
    // temperary torches
    if ((actor->actor->params) == 4230 || (actor->actor->params) == 4220 || (actor->actor->params) == 4227 ||
        (actor->actor->params) == 4380 || actor->actor->params == 4321) {
        if (torche->litTimer != 0) {
            actor->policy.volume = 0.1;
            if (actor->frameCount % 30 != 0) {
                return;
            }
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_BOMB_IGNIT, false);
        } else {
            if (actor->policy.volume != 1.0) {
                actor->policy.volume = 1.0;
            }
        }
        if (actor->frameCount % 30 != 0) {
            return;
        }
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_BOMB_IGNIT, false);
    }
    if (actor->frameCount % 30 != 0) {
        return;
    }

    //unlit permanent torches
    if ((actor->actor->params) == 8192) {
        if (torche->litTimer == 0) {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_BOMB_IGNIT, false);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_ANUBIS_FIRE, false);
        }
    }
    //lit permanent torches
    if ((actor->actor->params) == 9216 || (actor->actor->params) == 962) {

        actor->policy.volume = 0.5;
        actor->policy.distance = 200.0;
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_ANUBIS_FIRE, false);
    }
}

void accessible_hasi(AccessibleActor* actor) {
    if ((actor->actor->params) == 0) {
        actor->policy.ydist = 1000;
        actor->policy.distance = 1000;
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_OCTAROCK_ROCK, false);
    }

    else if ((actor->actor->params) == 1) {
        actor->policy.ydist = 1000;
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_PL_DAMAGE, false);
    }
}
bool accessible_switch_init(AccessibleActor* actor) {
    SwitchData* data = (SwitchData*)malloc(sizeof(SwitchData));
    data->framesUntilAboveChime = 0;
    if (data == NULL)
        return false;//failure to allocate memory.
    actor->userData = (void*) data;
    return true;

}
void accessible_switch_cleanup(AccessibleActor* actor)
{
    free(actor->userData);

}

    void accessible_switch(AccessibleActor * actor) {

        SwitchData* data = (SwitchData*)actor->userData;

    Player* player = GET_PLAYER(actor->play);
    ObjSwitch* sw = (ObjSwitch*)actor->actor;
    Vec3f& scale = actor->actor->scale;
    if ((actor->actor->params & 7) == 0) {
        if (actor->xyzDistToPlayer > 800) {
            return;
        }
        if (scale.y >= 33.0f / 200.0f) {
            if (actor->play->sceneNum == 0 && actor->play->roomCtx.curRoom.num == 5 && actor->xzDistToPlayer < 20) {
                ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);//Should result in same behaviour.
            }
            if (actor->frameCount % 30 != 0) {
                return;
            }
            ActorAccessibility_PlaySoundForActor(actor, 1, NA_SE_EV_FOOT_SWITCH, false);
}
    }
    else if (actor->frameCount % 30 != 0) {
        return;
    }
    else if ((actor->actor->params & 7) == 1) {
        if (actor->xyzDistToPlayer > 800) {
            return;
        }
        if (scale.y >= 33.0f / 200.0f) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F)))) {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_HAMMER_HIT, false);
        }
    } 
    else if ((actor->actor->params & 7) == 2) {
        if (sw->eyeTexIndex == 0) { //(!(Flags_GetSwitch(actor->play, (actor->params >> 8 & 0x3F))))
                                    // make it only play for open eye
            actor->policy.aimAssist.isProvider = true;
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_FOOT_SWITCH, false);
        }
    }

    else {
        if (actor->xyzDistToPlayer > 800) {
            return;
        }
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
    Player* player = GET_PLAYER(actor->play);
    actor->policy.distance = 1500;
    actor->policy.ydist = 2000;

    if (actor->yDistToPlayer > 500.0 && actor->sceneIndex != 96 && actor->play->sceneNum != 81 &&
        actor->play->sceneNum != 82) {
        return;
    }
    /*switch (actor->sceneIndex) { 
        case 85 || 91:
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SARIA_MELODY, false);
        case 81:
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_INGO_HORSE_NEIGH, false);
        case 0:
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_FANTOM_WARP_L, false);
    }*/
    
        //hyrule field attenuation
        if (actor->play->sceneNum == 81) {
        if (actor->xzDistToPlayer > 700) {
            actor->policy.distance = actor->xzDistToPlayer * 1.2;
            if (actor->xzDistToPlayer > 8000) {
                return;
            }
        } else {
            actor->policy.distance = 1500;
            if (actor->xzDistToPlayer > 1500) {
                return;
            }
        }
    }
        //kakariko village attenuation
        else if (actor->play->sceneNum == 82) {
            if (actor->sceneIndex == 83 || actor->sceneIndex == 81 || actor->sceneIndex == 96) {
                actor->policy.runsAlways == true;
                actor->policy.ydist = 5000;
                if (actor->xzDistToPlayer > 700) {
                    if (actor->sceneIndex == 81) {
                        actor->policy.distance = actor->xyzDistToPlayer * 1.4;
                    } else {
                        actor->policy.distance = actor->xyzDistToPlayer * 1.2;
                    }
                    if (actor->xzDistToPlayer > 8000) {
                        return;
                    }
                } else {
                    actor->policy.distance = 1500;
                    if (actor->xzDistToPlayer > 1500) {
                        return;
                    }
                }
            } else if (actor->sceneIndex == 8) {
                if (!(((gSaveContext.eventChkInf[6]) >> (7)) & 1))
                    return;
            }
                else {
                actor->policy.ydist = 500;
                    actor->policy.distance = 1000;
                if (actor->xzDistToPlayer > 1000) {
                    return;
                }
                } 
            }
            
        
        else if (actor->play->sceneNum == 91 || actor->play->sceneNum == 69 || actor->play->sceneNum == 70) {
            actor->policy.distance = 1000;
            if (actor->xzDistToPlayer > 1000) {
                return;
            }
        }
   /* if (actor->play->sceneNum <= 11) {
        actor->policy.distance = 500;
         }*/
        else {
            if (actor->xzDistToPlayer > 1500) {
                return;
            }
        }
    if (actor->sceneIndex == 85 || actor->sceneIndex == 91) {
        if (actor->play->sceneNum == 91 && gSaveContext.entranceIndex != 1504 && gSaveContext.entranceIndex != 1246) {
            return;
        }
        if (actor->play->sceneNum == 85 && actor->world.pos.y < 0) {
            
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_HORSE_RUN_LEVEL, false);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SARIA_MELODY, false);
        }
        //kokiri forest and lost woods
    } else if (actor->play->sceneNum >= 17 && actor->play->sceneNum <= 25) {
        return; // dont check for entrances while in boss rooms
    } else if (actor->sceneIndex == 81) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_HORSE_RUN_LEVEL, false);
        //hyrule field
    } else if (actor->sceneIndex == 10 && actor->play->sceneNum != 85) {//temp
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_OC_DOOR_OPEN, false);
    } else if (actor->sceneIndex <= 11) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_FANTOM_WARP_L, false);
        //dungeons
    } else if (actor->sceneIndex >= 27 && actor->sceneIndex <= 29) {
        if (actor->play->sceneNum >= 32 && actor->play->sceneNum <= 34) {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_HORSE_RUN_LEVEL, false);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SMALL_DOG_BARK, false);
       }
    } else if (actor->sceneIndex >= 30 && actor->sceneIndex <= 33) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SMALL_DOG_BARK, false);
        //market sound
    } else if ((actor->sceneIndex >= 34 && actor->sceneIndex <= 36) || actor->sceneIndex == 67) {
        if (actor->play->sceneNum == 67) {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SMALL_DOG_BARK, false);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_STONE_BOUND, false);
        }
        
        //ToT sound
    } else if (actor->sceneIndex == 69 || actor->sceneIndex == 70) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_MUSI_SINK, false);
    } else if (actor->sceneIndex == 82) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_CHICKEN_CRY_M, false);
        //kakariko sound
    } else if (actor->sceneIndex == 83) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_PO_APPEAR, false);
        //graveyard sound
    } else if (actor->sceneIndex == 84 || actor->sceneIndex == 88 || actor->sceneIndex == 89) { //last one is zora fountain maybe seperate?
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_RIVER_STREAM_S, false);
        //zora sound
    } else if (actor->sceneIndex == 86) {//might not need to exist
        //forest medow sound
    } else if (actor->sceneIndex == 87) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_WATER_WALL, false);
        //Lake Hylia sound
    } else if (actor->sceneIndex == 90 || actor->sceneIndex == 93) { //gerudo valley and fortress
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_GERUDOFT_BREATH, false);
        //gerudo valley sound
    } else if (actor->sceneIndex == 92 || actor->sceneIndex == 94) {//haunted wasteland and desert colosus
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SAND_STORM, false);
        
    }  else if (actor->sceneIndex == 100 || actor->sceneIndex ==95) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_BRIDGE_OPEN, false);
        //Hyrule Castle sound
    } else if (actor->sceneIndex == 96) {

        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_DODO_K_ROLL, false);
        //DMT sound
    } else if (actor->sceneIndex == 97) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_DODO_K_LAVA, false);
        //DMC sound
    } else if (actor->sceneIndex == 98) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_DARUNIA_HIT_BREAST, false);
        //Goron City
    } else if (actor->sceneIndex == 99) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_COW_CRY, false);
        //Lon Lon
    } else if (actor->sceneIndex >= 17 && actor->sceneIndex <= 25) {
        return;//boss rooms
    }
    else {
        actor->policy.distance = 500;
        if (actor->play->sceneNum == 83) {
            actor->policy.ydist = 0;
        }
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_OC_DOOR_OPEN, false);

    }
}

void accessible_231_dekus(AccessibleActor* actor) {
    
    if (actor->actor->params == 1) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_FAINT, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 1.0);
    }
    if (actor->actor->params == 2) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_FAINT, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 0.5);
    }
    if (actor->actor->params == 3) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_FAINT, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 1.5);
    } else {
        return;
    }

}

void accessible_hana(AccessibleActor* actor) {
    if (actor->actor->params == 1) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_OCTAROCK_ROCK, false);
    } else if (actor->actor->params == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIG_UP, false);
    }
    
}

void accessible_climable(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    f32 waterLoc = player->actor.yDistToWater + player->actor.world.pos.y;
    if (actor->world.pos.y < waterLoc) {
        actor->world.pos.y = waterLoc;
    }
    if (actor != NULL && actor->yDistToPlayer < 80)
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_PL_LAND_LADDER, false);
}

void accessible_en_guard(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    f32 guardsfx = NA_SE_IT_SWORD_IMPACT;
    if (fabs(actor->actor->world.pos.x - player->actor.world.pos.x) >
        fabs(actor->actor->world.pos.z - player->actor.world.pos.z)) {
    if (fabs(actor->actor->shape.rot.y - 16384) <1000) {        if (actor->actor->world.pos.x < player->actor.world.pos.x) {
            ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
            ActorAccessibility_SetSoundPitch(actor, 0, 2.0);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
            ActorAccessibility_SetSoundPitch(actor, 0, 0.2); 
        }
    } else if ((actor->actor->shape.rot.y + 16384)<1000) {
        if (actor->actor->world.pos.x < player->actor.world.pos.x) {
            ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
            ActorAccessibility_SetSoundPitch(actor, 0, 0.2);
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
            ActorAccessibility_SetSoundPitch(actor, 0, 2.0);
        }
    } else {
        ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 1.0);
    }
    } else {
        if (fabs(actor->actor->shape.rot.y) < 1000) {
             if (actor->actor->world.pos.z < player->actor.world.pos.z) {
                ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
                ActorAccessibility_SetSoundPitch(actor, 0, 2.0);
            } else {
                ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
                ActorAccessibility_SetSoundPitch(actor, 0, 0.2);
            }
        } else if (fabs(actor->actor->shape.rot.y+32768)<1000) {
            if (actor->actor->world.pos.z < player->actor.world.pos.z) {
                ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
                ActorAccessibility_SetSoundPitch(actor, 0, 0.2);
            } else {
                ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
                ActorAccessibility_SetSoundPitch(actor, 0, 2.0);
            }
        } else {
            ActorAccessibility_PlaySoundForActor(actor, 0, guardsfx, false);
            ActorAccessibility_SetSoundPitch(actor, 0, 1.0);
        }
    }
    

}

void accessible_en_dogs(AccessibleActor* actor) {
    EnDog* dog = (EnDog*)actor->actor;
    if (EnDog_CanFollow(dog, actor->play) == 1) {
        dog->actionFunc = EnDog_FollowPlayer;
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 1.0);

    }
    if (actor->frameCount % 30 != 0) {
        return;
    }
    if (actor->actor->params == 608 || actor->actor->params == 336 || actor->actor->params == 304 ||
        actor->actor->params == 3088 ||
        actor->actor->params == 2576 || actor->actor->params <0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SMALL_DOG_BARK, false);
        
        ActorAccessibility_SetSoundPitch(actor, 0, 2.0);
    } else {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_SMALL_DOG_BARK, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 0.5);
    }

}

void accessible_goma(AccessibleActor* actor) {
    BossGoma* goma = (BossGoma*)actor->actor;
    if (goma->visualState == 0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);
    }
}

void accessible_door_of_time(AccessibleActor* actor) {
    ActorAccessibility_PlaySampleForActor(actor, 0, "Chanting", false);
    ActorAccessibility_SetSoundPitch(actor, 0, 1.0);

    //ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_DIAMOND_SWITCH, false);
}

void accessible_sticks(AccessibleActor* actor) {
    EnKarebaba* baba = (EnKarebaba*)actor->actor;
    
    if (baba->actionFunc != EnKarebaba_DeadItemDrop)
        return;
    if (actor->actor->flags == 80) {

        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EN_NUTS_DAMAGE, false);
    }

}

void accessible_graveyard_soil(AccessibleActor* actor) {
    ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_IT_WOODSTICK_BROKEN, false);
}

void accessible_cucco(AccessibleActor* actor) {
    if (actor->actor->params == 14) {
        
    } else if (actor->actor->params == 13) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_CHICKEN_CRY_N, false);
        ActorAccessibility_SetSoundPitch(actor, 0, 1.5);
    } else {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_EV_CHICKEN_CRY_N, false);
    }
}

bool accessible_general_helper_init(AccessibleActor* actor) {
    GeneralHelperData* data = (GeneralHelperData*)malloc(sizeof(GeneralHelperData));
    if (data == NULL)
        return false;
    data->currentRoom = -1;
    data->currentRoomClear = false;
    data->currentScene = -1;

    actor->userData = data;
    return true;

}
void accessible_general_helper_cleanup(AccessibleActor* actor)
{
    free(actor->userData);
    actor->userData = NULL;
}
void accessible_va_general_helper(AccessibleActor* actor)
{
    GeneralHelperData* data = (GeneralHelperData*)actor->userData;
    if (data->currentScene == actor->play->sceneNum && data->currentRoom != actor->play->roomCtx.curRoom.num)
    {
        ActorAccessibility_AnnounceRoomNumber(actor->play);
        data->currentRoom = actor->play->roomCtx.curRoom.num;
        data->currentRoomClear = Flags_GetClear(actor->play, data->currentRoom);
    }
    if (data->currentScene != actor->play->sceneNum)
    {
        ActorAccessibility_InterpretCurrentScene(actor->play);
        data->currentScene = actor->play->sceneNum;
        data->currentRoom = actor->play->roomCtx.curRoom.num;
        data->currentRoomClear = Flags_GetClear(actor->play, data->currentRoom);

    }
//Report when a room is completed.
    if (!data->currentRoomClear && Flags_GetClear(actor->play, data->currentRoom))
    {
        data->currentRoomClear = Flags_GetClear(actor->play, data->currentRoom);
        ActorAccessibility_AnnounceRoomNumber(actor->play);

    }
}
bool accessible_audio_compass_init(AccessibleActor* actor)
{
    AudioCompassData* data = (AudioCompassData*)malloc(sizeof(AudioCompassData));
    if (data == NULL)
        return false;
    data->linearVelocity = 0;
    data->framesUntilChime = 0;

    actor->userData = data;
    return true;

}
void accessible_audio_compass_cleanup(AccessibleActor* actor)
{
    free(actor->userData);
}
void accessible_audio_compass(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    if (player->stateFlags1 & PLAYER_STATE1_TARGETING || player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER)
        return;

    actor->world.pos = player->actor.world.pos;
    actor->world.pos.z -= 50;
    bool shouldChime = false;
    if (actor->world.rot.y != player->actor.world.rot.y) {
        actor->world.rot.y = player->actor.world.rot.y;
        if (player->linearVelocity == 0)
        shouldChime = true;
    }
    AudioCompassData* data = (AudioCompassData*)actor->userData;
    if (data->linearVelocity == 0.0 && player->linearVelocity > 0.0) {
        shouldChime = true;

    }
    data->linearVelocity = player->linearVelocity;
    if (data->framesUntilChime > 0)
        data->framesUntilChime--;
    if (shouldChime && data->framesUntilChime <= 0) {

        ActorAccessibility_PlaySoundForActor(actor, 0, actor->policy.sound, false);
        data->framesUntilChime = 10;

    }

}

void accessible_stick_warning(AccessibleActor* actor) {
    Player* player = GET_PLAYER(actor->play);
    actor->world.pos = player->actor.world.pos;
    actor->world.pos.z -= 50;
    if (fabs(player->unk_860 - 25) < 24.0 && player->heldItemId==0) {
        ActorAccessibility_PlaySoundForActor(actor, 0, NA_SE_SY_WARNING_COUNT_N, false);
    }
}

    void ActorAccessibility_InitActors() {
    const int Npc_Frames = 35;
    ActorAccessibilityPolicy policy;
    ActorAccessibility_InitPolicy(&policy, "Rock", accessible_en_ishi, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, policy);

    ActorAccessibility_InitPolicy(&policy, "Story NPCs", NULL, NA_SE_VO_NA_HELLO_0);
    policy.englishName = "Mido";
    policy.n = Npc_Frames;
    policy.distance = 1000;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MD, policy);
    policy.englishName = "Malon";
    policy.distance = 500;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MA1, policy);
    policy.englishName = "Talon";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_TA, policy);
    policy.englishName = "Child Zelda";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ZL4, policy);
    policy.englishName = "Ingo";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_IN, policy);
    policy.englishName = "Cucco Lady";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_NIW_LADY, policy);
    policy.englishName = "Windmill Man";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_FU, policy);
    policy.englishName = "Durania";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_DU, policy);
    policy.englishName = "Owl";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_OWL, policy);
    ActorAccessibility_InitPolicy(&policy, "Catching Guards", accessible_en_guard, 0);
    policy.n = 10;
    policy.distance = 500;
    policy.ydist = 300;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HEISHI1, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HEISHI3, policy);

    ActorAccessibility_InitPolicy(&policy, "Passive Guards", NULL, NA_SE_IT_SWORD_IMPACT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HEISHI2, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HEISHI4, policy);

    ActorAccessibility_InitPolicy(&policy, "Shopkeepers", NULL, NA_SE_VO_NA_HELLO_1);
    policy.pitch = 0.6;
    policy.n = 30;
    policy.englishName = "Shooting Gallery Man";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SYATEKI_MAN, policy);
    policy.englishName = "Bombchu Bowling Alley Lady";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOM_BOWL_MAN, policy);
    policy.englishName = "ShopKeeper";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_OSSAN, policy);
    policy.englishName = "Potion Shop Granny";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_DS, policy);

    // general NPCs
    ActorAccessibility_InitPolicy(&policy, "Kokiri Child", accessible_en_NPC_Gen, 0);
    policy.n = Npc_Frames;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KO, policy);
    policy.englishName = "Gorons";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GO2, policy);
    policy.englishName = "Saria";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SA, policy);
    policy.englishName = "Graveyard Kid";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_CS, policy);
    policy.englishName = "Dampe (Alive)";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_TK, policy);
    policy.englishName = "Happy Mask Shop Customer";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GUEST, policy);
    policy.englishName = "Market Npc";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HY, policy);
    policy.englishName = "Girl Chassing Cucco";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_NIW_GIRL, policy);
    policy.englishName = "Honey & Darling";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_TG, policy);
    policy.englishName = "Haggling Townspeople";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_MU, policy);
    policy.englishName = "Skull Kid";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SKJ, policy);
    policy.englishName = "Boss Carpenter";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_TORYO, policy);
    policy.englishName = "Carpenters (Kakariko)";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_DAIKU_KAKARIKO, policy);
    policy.englishName = "Kakariko Rooftop Man";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ANI, policy);
    policy.englishName = "Cursed Skulltula People";
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SSH, policy);
    policy.englishName = "Gossip Stone";
    policy.pitch = 0.75;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GS, policy);

    ActorAccessibility_InitPolicy(&policy, "Dogs", accessible_en_dogs, 0);
    policy.n = 1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_DOG, policy);

    ActorAccessibility_InitPolicy(&policy, "Horses", NULL, NA_SE_EV_HORSE_NEIGH);
    policy.n = 30;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HORSE_NORMAL, policy);
    ActorAccessibility_InitPolicy(&policy, "Cows", NULL, NA_SE_EV_COW_CRY_LV);
    policy.n = 30;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_COW, policy);
    ActorAccessibility_InitPolicy(&policy, "Cuccos", accessible_cucco, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_NIW, policy);
    ActorAccessibility_InitPolicy(&policy, "Bush", NULL, NA_SE_PL_PULL_UP_PLANT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KUSA, policy);
    ActorAccessibility_InitPolicy(&policy, "Trees", NULL, NA_SE_EV_TREE_CUT);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_WOOD02, policy);

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
    ActorAccessibility_InitPolicy(&policy, "Large Crate", NULL, NA_SE_EV_WOODBOX_BREAK);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_KIBAKO2, policy);
    ActorAccessibility_InitPolicy(&policy, "deku stick drops", accessible_sticks, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_DEKUBABA, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_KAREBABA, policy);
    ActorAccessibility_InitPolicy(&policy, "Owl", NULL, NA_SE_EN_OWL_FLUTTER);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_OWL, policy);
    // will probably just get replaced with ghost actors anyways
    // ActorAccessibility_AddSupporte dActor(ACTOR_EN_HOLL, "Room Changing Plane", NULL, 30, 500, 1.0, 1.0,
    //                                      NA_SE_EV_STONEDOOR_STOP /*NOT SURE YET*/);
    ActorAccessibility_InitPolicy(&policy, "Bean patch", NULL, NA_SE_EN_MUSI_SINK);
    policy.n = 60;
    policy.distance = 2400;
    policy.pitch = 1.3;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_BEAN, policy);
    ActorAccessibility_InitPolicy(&policy, "GraveYard Digging spots", accessible_graveyard_soil, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_IT, policy);
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
    policy.ydist = 2000;
    policy.distance = 2000;
    policy.pitch = 1.2;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_SP, policy);
                                         
    ActorAccessibility_InitPolicy(&policy, "Shutter Door", accessible_door, 0);
    policy.n = 30;
    policy.distance = 1000;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_DOOR_SHUTTER, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_SPOT18_SHUTTER, policy);
    ActorAccessibility_InitPolicy(&policy, "Switch", accessible_switch, 0);
    policy.distance = 2000;
    policy.cleanupUserData = accessible_switch_cleanup;
    policy.initUserData = accessible_switch_init;
    policy.n = 1;
    policy.ydist = 200;
    policy.pitch = 1.1;
    

    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SWITCH, policy);
    ActorAccessibility_InitPolicy(&policy, "Ocarina Spots", NULL, NA_SE_EV_DIAMOND_SWITCH);
    policy.n = 30;
    policy.distance = 800;
    policy.pitch = 1.1;
    policy.ydist = 500;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_OKARINA_TAG, policy);
    ActorAccessibility_InitPolicy(&policy, "Pushable Block", accessible_test, 0);
    policy.n = 30;
    policy.distance = 800;
    policy.pitch = 1.1;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_OSHIHIKI, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_SPOT15_RRBOX, policy);
    ActorAccessibility_InitPolicy(&policy, "Torch", accessible_torches, 0);
    policy.n = 1;
    policy.pitch = 1.1;
    policy.distance = 800;
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_SYOKUDAI, policy);
    ActorAccessibility_InitPolicy(&policy, "Deku Tree Moving Platform", accessible_hasi, 0);
    //policy.volume = 1.3;
    policy.distance = 1000;
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_HASI, policy);
    ActorAccessibility_InitPolicy(&policy, "Pot", NULL, NA_SE_EV_POT_BROKEN);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_TSUBO, policy);
    //ActorAccessibility_InitPolicy(&policy, "Deku Tree Entrance", NULL, NA_SE_EV_FANTOM_WARP_L);
    //policy.distance = 5000;
    //ActorAccessibility_AddSupportedActor(ACTOR_BG_TREEMOUTH, policy);
    ActorAccessibility_InitPolicy(&policy, "Platform collapsable", NULL, NA_SE_EV_BLOCK_SHAKE);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_LIFT, policy);
    ActorAccessibility_InitPolicy(&policy, "Ladder in Slingshot Room", accessible_maruta, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_YDAN_MARUTA, policy);
    ActorAccessibility_InitPolicy(&policy, "bombable wall", NULL, NA_SE_EN_OCTAROCK_ROCK);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_BREAKWALL, policy);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_BOMBWALL, policy);
    ActorAccessibility_InitPolicy(&policy, "231 dekus", accessible_231_dekus, 0);
    policy.distance = 2000;
    policy.n = 50;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_HINTNUTS, policy);
    ActorAccessibility_InitPolicy(&policy, "Flame Circle", NULL, NA_SE_EV_FIRE_PILLAR);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_HIDAN_CURTAIN, policy);
    ActorAccessibility_InitPolicy(&policy, "uninteractable rocks in kokiri forest", accessible_hana, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_OBJ_HANA, policy);
    ActorAccessibility_InitPolicy(&policy, "gold skulltula token", accessible_en_pickups, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SI, policy);
    ActorAccessibility_InitPolicy(&policy, "Gold and Wall skulltulas", NULL, 0);
    policy.aimAssist.isProvider = true;
    policy.n = 1;
    policy.ydist = 500;
    policy.distance = 750;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_SW, policy);
    ActorAccessibility_InitPolicy(&policy, "goma larva egg", accessible_larva, 0);
    policy.distance = 1000;
    policy.ydist = 1000;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_GOMA, policy);
    ActorAccessibility_InitPolicy(&policy, "redead", NULL, NA_SE_EN_REDEAD_CRY);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_RD, policy);
    ActorAccessibility_InitPolicy(&policy, "Beamos", NULL, NA_SE_EN_BIMOS_AIM);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_VM, policy);
    ActorAccessibility_InitPolicy(&policy, "heart canister", accessible_en_pickups, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_ITEM_B_HEART, policy);
    ActorAccessibility_InitPolicy(&policy, "Goma", accessible_goma, 0);
    policy.distance = 5000;
    policy.ydist = 2000;
    ActorAccessibility_AddSupportedActor(ACTOR_BOSS_GOMA, policy);
    ActorAccessibility_InitPolicy(&policy, "bombflowers", NULL, NA_SE_EV_BOMB_BOUND);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_BOMBF, policy);
    ActorAccessibility_InitPolicy(&policy, "Amos Statue", NULL, NA_SE_EN_AMOS_WAVE);
    policy.n = 30;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_AM, policy);
    ActorAccessibility_InitPolicy(&policy, "door of time", accessible_door_of_time, 0);
    ActorAccessibility_AddSupportedActor(ACTOR_BG_MJIN, policy);
    ActorAccessibility_InitPolicy(&policy, "shooting gallery rupees", NULL, 0);
    policy.aimAssist.isProvider = true;
    policy.distance = 1000;
    policy.n = 1;
    ActorAccessibility_AddSupportedActor(ACTOR_EN_G_SWITCH, policy);
    ActorAccessibility_InitPolicy(&policy, "crawlspace", NULL, NA_SE_EN_MUSI_SINK);
    policy.volume = 1.5;
    policy.distance = 2000;
    ActorAccessibility_AddSupportedActor(VA_CRAWLSPACE, policy);
    ActorAccessibility_InitPolicy(&policy, "Ladder/climable", accessible_climable, 0);
    //policy.volume = 1.5;
    policy.pitch = 1.3;
    //policy.distance = 2000;
    ActorAccessibility_AddSupportedActor(VA_CLIMB, policy);
    ActorAccessibility_InitPolicy(&policy, "Door", NULL, NA_SE_OC_DOOR_OPEN);
    policy.n = 30;
    policy.pitch = 1.1;
    policy.distance = 1000;
    ActorAccessibility_AddSupportedActor(VA_DOOR, policy);
    
    ActorAccessibility_InitPolicy(&policy, "Area Change", accessible_area_change, 0);
    policy.n = 60;
    policy.distance = 100000;
    ActorAccessibility_AddSupportedActor(VA_AREA_CHANGE, policy);
    ActorAccessibility_InitPolicy(&policy, "marker", NULL,
                                  NA_SE_EV_DIAMOND_SWITCH); 
    policy.distance = 1000;
    policy.pitch = 1.7;
    ActorAccessibility_AddSupportedActor(VA_MARKER, policy);
    //ActorAccessibility_InitPolicy(&policy, "Spike", NULL, NA_SE_EV_DIAMOND_SWITCH);
    //policy.distance = 200;
    //policy.pitch = 0.5;
    //ActorAccessibility_AddSupportedActor(VA_SPIKE, policy);
    ActorAccessibility_InitPolicy(&policy, "Stick Burnout Warning", accessible_stick_warning, 0);
    policy.n = 1;
    policy.runsAlways = true;
    ActorAccessibility_AddSupportedActor(VA_STICK_WARNING, policy);
    ActorAccessibility_InitPolicy(&policy, "System general helper", accessible_va_general_helper, 0);
    policy.n = 1;
    policy.cleanupUserData = accessible_general_helper_cleanup;
    policy.initUserData = accessible_general_helper_init;
    policy.runsAlways = true;
    ActorAccessibility_AddSupportedActor(VA_GENERAL_HELPER, policy);
    ActorAccessibility_InitPolicy(&policy, "Audio Compass", accessible_audio_compass, 0);
    policy.n = 1;
    policy.cleanupUserData = accessible_audio_compass_cleanup;
    policy.initUserData = accessible_audio_compass_init;
    policy.runsAlways = true;
    policy.sound = NA_SE_EV_SHIP_BELL;//Setting this here so it's easy to change if we ever decide to change it.
    policy.pitch = 0.5;


    ActorAccessibility_AddSupportedActor(VA_AUDIO_COMPASS, policy);

    // Now query a list of virtual actors for a given
                                                                // location (scene
                                                       // and room
                                           // number).
    VirtualActorList* list = (VirtualActorList*)ActorAccessibility_GetVirtualActorList(EVERYWHERE, 0);//Global/ omnipresent.

    // Now place the actor.
    ActorAccessibility_AddVirtualActor(list, VA_GENERAL_HELPER, { { 0.0, 0.0, 0.0 }, { 0, 0, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_AUDIO_COMPASS, { { 0.0, 0.0, 0.0}, { 0, 0, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_STICK_WARNING, { { 0.0, 0.0, 0.0 }, { 0, 0, 0 } });
    list = ActorAccessibility_GetVirtualActorList(85, 0); // Kokiri Forest
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -784.0, 120.0, 1046.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 2146.5, 1.0, -142.8 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -547.0, 60.0, -1036.00 }, { 0, 14702, 0 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -29.0, -80.0, 983.00 }, { 0, 14702, 0 } });
    /*ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -448.0, 0.0, -528.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -1082.0, 120.0, 383.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -27.0, 100.0, 1117.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 515.0, 0.0, 647.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 1046.0, 0.0, 549.00 }, { 0, 14702, 0 } });
    ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 848.0, 0.0, -323.00 }, { 0, 14702, 0 } });
    */
    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE,{ { -317.0, 373.2, -1542.00 }, {0, 14702, 0 }}, AREA_KORIRI);
 
    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE, { { -1380.0, -67.0, -288.00 }, { 0, 14702, 0 } }, AREA_HYRULE_FIELD);

    list = ActorAccessibility_GetVirtualActorList(85, 2); // Kokiri Forest Room with boulder and kokiri sword
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -788.0, 120.0, 1392.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(38, 0); //know-it-all house
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 12.0, 0.0, -131.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(40, 0); // mido house
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { -6.6, 0.0, -179.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(52, 0); // link's house
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 2.3, 0.0, -134.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(41, 0); // saria's house
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 1.7, 0.0, -188.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(39, 0); // twins house
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 3.0, 0.0, -179.00 }, { 0, 14702, 0 } });

    //list = ActorAccessibility_GetVirtualActorList(45, 0); // Kokiri Shop
    //ActorAccessibility_AddVirtualActor(list, VA_DOOR, { { 0.0, 0.0, 150.00 }, { 0, 14702, 0 } });

    list = ActorAccessibility_GetVirtualActorList(0, 0);//deku tree main room
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -226.7, 0, 197.0 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 118.6, 0, -286.6 } });

    //ActorAccessibility_AddVirtualActor(list, VA_AREA_CHANGE, { {0, 0, 640} }, AREA_KORIRI);
    
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 287.4, 368.0, 347.0 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 419.4, 368.0, 173.6 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 323, 567.0, 314.6 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 127.5, 897.0, 433.6 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 440.9, 897.0, 101.6 } });

    list = ActorAccessibility_GetVirtualActorList(0, 2); // deku tree slingshot room
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1159, 288.0, 1403.0 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1179.6, 480.0, 1463.6 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -1398.9, 288.0, 1161.6 } });

    list = ActorAccessibility_GetVirtualActorList(0, 10); // deku tree compass room
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -762, 733.0, 151.0 } });
    /*ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -935, 780.0, -113 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1031.0, 800.0, 109.7 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1184, 820.0, -103.4 } });*/

    list = ActorAccessibility_GetVirtualActorList(0, 7); // deku tree bombable wall room
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -1209, -820.0, 3.5 } });

    list = ActorAccessibility_GetVirtualActorList(0, 3); // deku tree basement 1 lobby
    ActorAccessibility_AddVirtualActor(list, VA_CRAWLSPACE, { { -901, -820.0, 0.5 } });
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -181.761, -905.0, -28.3 } });
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { 108, -919.5, 5.0 } });

    list = ActorAccessibility_GetVirtualActorList(0, 9); // deku tree b2 lobby
    //ActorAccessibility_AddVirtualActor(list, VA_CLIMB, { { -639, -1912.5, 188.0 } });
        //Install cues for walls, ledges etc.
    list = ActorAccessibility_GetVirtualActorList(1, 2);//dodongo bombflower stairs room
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1958, 20, -1297 } });
    list = ActorAccessibility_GetVirtualActorList(69, 0);//hyrule courtyard
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    AccessibleActor* temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 0.3;
    temp->policy.volume = 0.5;
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1040.0, 0.0, 140.514 } });
    temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 0.6;
    temp->policy.volume = 0.5;
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 230.0, 0.0, 188.514 } });
    temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 0.9;
    temp->policy.volume = 0.5;
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -426.0, 0.0, 130.514 } });
    temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 1.2;
    temp->policy.volume = 0.5;
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1206.0, 0.0, 133.514 } });
    temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 1.5;
    temp->policy.volume = 0.5;
    ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { -1571.0, 0.0, -834.514 } });
    temp = ActorAccessibility_AddVirtualActor(list, VA_MARKER, { { 1734.0, 0.0, 140.514 } });
    temp->policy.pitch = 1.8;
    temp->policy.volume = 0.5;
    ActorAccessibility_InitCues();

}
