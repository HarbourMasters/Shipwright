#include "CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "z64actor.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
}

// #region These were copied from z_en_item00.c
static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AT_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000010, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};
// #endregion

EnItem00* CustomItem::Spawn(f32 posX, f32 posY, f32 posZ, s16 rot, s16 flags, s16 params, ActorFunc actionFunc,
                            ActorFunc drawFunc) {
    if (!gPlayState) {
        return nullptr;
    }

    Actor* actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_ITEM00, posX, posY, posZ, flags, rot, params,
                               ITEM00_NOTHING);
    EnItem00* enItem00 = (EnItem00*)actor;

    if (actionFunc != NULL) {
        enItem00->actionFunc = (EnItem00ActionFunc)actionFunc;
    }

    if (drawFunc != NULL) {
        actor->draw = drawFunc;
    }

    return enItem00;
}

void CustomItem00_Init(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;

    if (CUSTOM_ITEM_FLAGS & CustomItem::STOP_BOBBING) {
        actor->shape.yOffset = 1250.0f;
    } else {
        actor->shape.yOffset = (Math_SinS(actor->shape.rot.y) * 150.0f) + 1250.0f;
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::HIDE_TILL_OVERHEAD) {
        Actor_SetScale(actor, 0.0f);
    } else {
        Actor_SetScale(actor, 0.015f);
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::KEEP_ON_PLAYER) {
        Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::TOSS_ON_SPAWN) {
        actor->velocity.y = 8.0f;
        actor->speed = 2.0f;
        actor->gravity = -1.4f;
        actor->world.rot.y = Rand_ZeroOne() * 40000.0f;
    }

    Actor_ProcessInitChain(actor, sInitChain);
    Collider_InitAndSetCylinder(play, &enItem00->collider, actor, &sCylinderInit);

    enItem00->unk152 = -1;
}

// By default this will just assume the GID was passed in as the rot z, if you want different functionality you should
// override the draw
void CustomItem00_Draw(Actor* actor, PlayState* play) {
    Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
    GetItem_Draw(play, CUSTOM_ITEM_PARAM);
}

// Once the item is touched we need to clear movement vars so the item doesn't sink in the players hands/above head
void CustomItem00_ItemTouched(Actor* actor, PlayState* play) {
    actor->speed = 0.0f;
    actor->velocity.y = 0.0f;
    actor->gravity = 0.0f;
    actor->shape.yOffset = 1250.0f;
}

void CustomItem00_Update(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;
    Player* player = GET_PLAYER(play);

    if (!(CUSTOM_ITEM_FLAGS & CustomItem::STOP_SPINNING)) {
        actor->shape.rot.y += 960;
    }

    if (!(CUSTOM_ITEM_FLAGS & CustomItem::STOP_BOBBING)) {
        actor->shape.yOffset = (Math_SinS(actor->shape.rot.y) * 150.0f) + 1250.0f;
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::HIDE_TILL_OVERHEAD) {
        Actor_SetScale(actor, 0.0f);
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::KEEP_ON_PLAYER) {
        Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
    }

    // Player range check accounting for goron rolling behavior. Matches EnItem00 range check.
    bool playerInGoronRoll = player->stateFlags3 & PLAYER_STATE3_1000;
    bool playerInRangeOfPickup =
        playerInGoronRoll ? ((actor->xzDistToPlayer <= 60.0f) && (fabsf(actor->playerHeightRel) <= fabsf(100.0f)))
                          : ((actor->xzDistToPlayer <= 30.0f) && (fabsf(actor->playerHeightRel) <= fabsf(50.0f)));

    if (CUSTOM_ITEM_FLAGS & CustomItem::KILL_ON_TOUCH) {
        // Pretty self explanatory, if the player is within range, kill the actor and call the action function
        if (playerInRangeOfPickup) {
            if (enItem00->actionFunc != NULL) {
                enItem00->actionFunc(enItem00, play);
                CUSTOM_ITEM_FLAGS |= CustomItem::CALLED_ACTION;
            }
            Actor_Kill(actor);
        }
    } else if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_OVERHEAD) {
        // If the item hasn't been picked up (unk152 == -1) and the player is within range
        if (enItem00->unk152 == -1 && playerInRangeOfPickup) {
            // Fire the action function
            if (enItem00->actionFunc != NULL) {
                enItem00->actionFunc(enItem00, play);
                CUSTOM_ITEM_FLAGS |= CustomItem::CALLED_ACTION;
            }
            Audio_PlaySfx(NA_SE_SY_GET_ITEM);
            // Set the unk152 to 15, this indicates the item has been picked up and will start the overhead animation
            enItem00->unk152 = 15;
            CUSTOM_ITEM_FLAGS |= CustomItem::STOP_BOBBING;
            CUSTOM_ITEM_FLAGS |= CustomItem::KEEP_ON_PLAYER;
            CustomItem00_ItemTouched(actor, play);
            // Move to player right away on this frame
            Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
        }

        // If the item has been picked up
        if (enItem00->unk152 > 0) {
            // Reduce the size a bit, but also makes it visible for HIDE_TILL_OVERHEAD
            Actor_SetScale(actor, 0.010f);

            // Decrement the unk152, which will be used to bob the item up and down
            enItem00->unk152--;

            // Account for the different heights of the player forms
            f32 height = 45.0f;
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_DEKU:
                    height = 35.0f;
                    break;
                case PLAYER_FORM_FIERCE_DEITY:
                    height = 90.0f;
                    break;
                case PLAYER_FORM_GORON:
                    height = 75.0f;
                    break;
                case PLAYER_FORM_ZORA:
                    height = 60.0f;
                    break;
            }

            // Bob the item up and down
            actor->world.pos.y += (height + (Math_SinS(enItem00->unk152 * 15000) * (enItem00->unk152 * 0.3f)));
        }

        // Finally, once the bobbing animation is done, kill the actor
        if (enItem00->unk152 == 0) {
            Actor_Kill(actor);
        }
    } else if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
        // If the item hasn't been picked up and the player is within range
        if (!Actor_HasParent(actor, play) && enItem00->unk152 == -1) {
            Actor_OfferGetItem(actor, play, GI_SHIP, 30.0f, 35.0f);
        } else {
            if (enItem00->unk152 == -1) {
                // actor->shape.yOffset = 1250.0f;
                CUSTOM_ITEM_FLAGS |= CustomItem::STOP_BOBBING;
                // Math_Vec3f_Copy(&actor->world.pos, &GET_PLAYER(play)->actor.world.pos);
                CUSTOM_ITEM_FLAGS |= CustomItem::KEEP_ON_PLAYER;
                // Actor_SetScale(actor, 0.0f);
                CUSTOM_ITEM_FLAGS |= CustomItem::HIDE_TILL_OVERHEAD;
                CustomItem00_ItemTouched(actor, play);
            }

            // Begin incrementing the unk152, indicating the item has been picked up
            enItem00->unk152++;

            // For the first 20 frames, wait while the player's animation plays
            if (enItem00->unk152 >= 20) {
                // After the first 20 frames, show the item and call the action function
                if (enItem00->unk152 == 20 && enItem00->actionFunc != NULL) {
                    enItem00->actionFunc(enItem00, play);
                    CUSTOM_ITEM_FLAGS |= CustomItem::CALLED_ACTION;
                }
                // Override the bobbing animation to be a fixed height
                actor->shape.yOffset = 900.0f;
                Actor_SetScale(actor, 0.007f);

                f32 height = 45.0f;
                switch (GET_PLAYER_FORM) {
                    case PLAYER_FORM_DEKU:
                        height = 35.0f;
                        break;
                    case PLAYER_FORM_FIERCE_DEITY:
                        height = 100.0f;
                        break;
                    case PLAYER_FORM_GORON:
                        height = 90.0f;
                        break;
                    case PLAYER_FORM_ZORA:
                        height = 75.0f;
                        break;
                }

                actor->world.pos.y += height;
            }

            // Once the player is no longer in the "Give Item" state, kill the actor
            if (!(player->stateFlags1 & PLAYER_STATE1_400)) {
                Actor_Kill(actor);
            }
        }
    }

    if (actor->gravity != 0.0f) {
        Actor_MoveWithGravity(actor);
        Actor_UpdateBgCheckInfo(play, actor, 20.0f, 15.0f, 15.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);
    }

    if (actor->bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        actor->speed = 0.0f;
    }

    if (CUSTOM_ITEM_FLAGS & CustomItem::ABLE_TO_ZORA_RANG) {
        Collider_UpdateCylinder(actor, &enItem00->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &enItem00->collider.base);
    }
}

void CustomItem00_Destroy(Actor* actor, PlayState* play) {
    EnItem00* enItem00 = (EnItem00*)actor;

    Collider_DestroyCylinder(play, &enItem00->collider);
}

void CustomItem::RegisterHooks() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::ShouldActorInit>(
        ACTOR_EN_ITEM00, [](Actor* actor, bool* should) {
            if (actor->params != ITEM00_NOTHING) {
                return;
            }

            actor->init = CustomItem00_Init;
            actor->update = CustomItem00_Update;
            actor->draw = CustomItem00_Draw;
            actor->destroy = CustomItem00_Destroy;

            // Set the rotX/rotZ back to 0, the original values can be accessed from actor->home
            actor->world.rot.x = 0;
            actor->world.rot.z = 0;
            actor->shape.rot.x = 0;
            actor->shape.rot.y = 0;
            actor->shape.rot.z = 0;
        });
}
