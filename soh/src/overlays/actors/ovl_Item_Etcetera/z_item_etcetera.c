/*
 * File: z_item_etcetera.c
 * Overlay: ovl_Item_Etcetera
 * Description: Collectible Items
 */

#include "z_item_etcetera.h"
#include <assert.h>

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

void ItemEtcetera_Init(Actor* thisx, PlayState* play);
void ItemEtcetera_Destroy(Actor* thisx, PlayState* play);
void ItemEtcetera_Update(Actor* thisx, PlayState* play);
void ItemEtcetera_DrawThroughLens(Actor* thisx, PlayState* play);
void ItemEtcetera_Draw(Actor* thisx, PlayState* play);

void func_80B857D0(ItemEtcetera* this, PlayState* play);
void func_80B85824(ItemEtcetera* this, PlayState* play);
void func_80B858B4(ItemEtcetera* this, PlayState* play);
void ItemEtcetera_SpawnSparkles(ItemEtcetera* this, PlayState* play);
void ItemEtcetera_MoveFireArrowDown(ItemEtcetera* this, PlayState* play);
void func_80B85B28(ItemEtcetera* this, PlayState* play);
void ItemEtcetera_UpdateFireArrow(ItemEtcetera* this, PlayState* play);

const ActorInit Item_Etcetera_InitVars = {
    ACTOR_ITEM_ETCETERA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ItemEtcetera),
    (ActorFunc)ItemEtcetera_Init,
    (ActorFunc)ItemEtcetera_Destroy,
    (ActorFunc)ItemEtcetera_Update,
    NULL,
    NULL,
};

static s16 sObjectIds[] = {
    OBJECT_GI_BOTTLE, OBJECT_GI_BOTTLE_LETTER, OBJECT_GI_SHIELD_2, OBJECT_GI_ARROWCASE, OBJECT_GI_SCALE,
    OBJECT_GI_SCALE,  OBJECT_GI_KEY,           OBJECT_GI_M_ARROW,  OBJECT_GI_RUPY,      OBJECT_GI_RUPY,
    OBJECT_GI_RUPY,   OBJECT_GI_RUPY,          OBJECT_GI_HEARTS,   OBJECT_GI_KEY,
};

// Indexes passed to the item table in z_draw.c
static s16 sDrawItemIndexes[] = {
    GID_BOTTLE,       GID_LETTER_RUTO,  GID_SHIELD_HYLIAN, GID_QUIVER_40,   GID_SCALE_SILVER,
    GID_SCALE_GOLDEN, GID_KEY_SMALL,    GID_ARROW_FIRE,    GID_RUPEE_GREEN, GID_RUPEE_BLUE,
    GID_RUPEE_RED,    GID_RUPEE_PURPLE, GID_HEART_PIECE,   GID_KEY_SMALL,
};

static s16 sGetItemIds[] = {
    GI_BOTTLE,     GI_LETTER_RUTO, GI_SHIELD_HYLIAN, GI_QUIVER_40, GI_SCALE_SILVER, GI_SCALE_GOLD, GI_KEY_SMALL,
    GI_ARROW_FIRE, GI_NONE,        GI_NONE,          GI_NONE,      GI_NONE,         GI_NONE,       GI_NONE,
};

void ItemEtcetera_SetupAction(ItemEtcetera* this, ItemEtceteraActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ItemEtcetera_Init(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    s32 pad;
    s32 type;
    s32 objBankIndex;

    type = this->actor.params & 0xFF;
    osSyncPrintf("no = %d\n", type);
    objBankIndex = Object_GetIndex(&play->objectCtx, sObjectIds[type]);
    osSyncPrintf("bank_ID = %d\n", objBankIndex);
    if (objBankIndex < 0) {
        assert(objBankIndex < 0);
    } else {
        this->objBankIndex = objBankIndex;
    }
    this->giDrawId = sDrawItemIndexes[type];
    this->getItemId = sGetItemIds[type];
    this->futureActionFunc = func_80B85824;
    this->drawFunc = ItemEtcetera_Draw;
    Actor_SetScale(&this->actor, 0.25f);
    ItemEtcetera_SetupAction(this, func_80B857D0);
    switch (type) {
        case ITEM_ETC_LETTER:
            Actor_SetScale(&this->actor, 0.5f);
            this->futureActionFunc = func_80B858B4;
            if ((Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER) && !IS_RANDO) ||
                (IS_RANDO && Flags_GetTreasure(play, 0x1E))) {
                Actor_Kill(&this->actor);
            }
            break;
        case ITEM_ETC_ARROW_FIRE:
            this->futureActionFunc = ItemEtcetera_UpdateFireArrow;
            Actor_SetScale(&this->actor, 0.5f);
            this->actor.draw = NULL;
            this->actor.shape.yOffset = 50.0f;
            break;
        case ITEM_ETC_RUPEE_GREEN_CHEST_GAME:
        case ITEM_ETC_RUPEE_BLUE_CHEST_GAME:
        case ITEM_ETC_RUPEE_RED_CHEST_GAME:
        case ITEM_ETC_RUPEE_PURPLE_CHEST_GAME:
        case ITEM_ETC_HEART_PIECE_CHEST_GAME:
        case ITEM_ETC_KEY_SMALL_CHEST_GAME:
            Actor_SetScale(&this->actor, 0.5f);
            this->futureActionFunc = func_80B85B28;
            this->drawFunc = ItemEtcetera_DrawThroughLens;
            this->actor.world.pos.y += 15.0f;
            break;
    }
}

void ItemEtcetera_Destroy(Actor* thisx, PlayState* play) {
}

void func_80B857D0(ItemEtcetera* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objBankIndex)) {
        this->actor.objBankIndex = this->objBankIndex;
        this->actor.draw = this->drawFunc;
        this->actionFunc = this->futureActionFunc;
    }
}

void func_80B85824(ItemEtcetera* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if ((this->actor.params & 0xFF) == 7) {
            if (IS_RANDO) {
                Flags_SetTreasure(play, 0x1F);
            }
        }

        if ((this->actor.params & 0xFF) == 1) {
            Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER);
            Flags_SetSwitch(play, 0xB);
        }
        Actor_Kill(&this->actor);
    } else {
        if (!IS_RANDO) {
            func_8002F434(&this->actor, play, this->getItemId, 30.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_SUN, GI_ARROW_FIRE);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 30.0f, 50.0f);
        }
    }
}

void func_80B858B4(ItemEtcetera* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if ((this->actor.params & 0xFF) == 1) {
            Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER);
            Flags_SetSwitch(play, 0xB);

            if (IS_RANDO) {
                Flags_SetTreasure(play, 0x1E);
            }
        }
        Actor_Kill(&this->actor);
    } else {
        if (0) {} // Necessary to match

        if (!IS_RANDO) {
            func_8002F434(&this->actor, play, this->getItemId, 30.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_UNDERWATER_ITEM, GI_LETTER_RUTO);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 30.0f, 50.0f);
        }

        if ((play->gameplayFrames & 0xD) == 0) {
            EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemEtcetera_SpawnSparkles(ItemEtcetera* this, PlayState* play) {
    static Vec3f velocity = { 0.0f, 0.2f, 0.0f };
    static Vec3f accel = { 0.0f, 0.05f, 0.0f };
    static Color_RGBA8 primColor = { 255, 255, 255, 0 };
    static Color_RGBA8 envColor = { 255, 50, 50, 0 };
    Vec3f pos;

    velocity.x = Rand_CenteredFloat(3.0f);
    velocity.z = Rand_CenteredFloat(3.0f);
    velocity.y = -0.05f;
    accel.y = -0.025f;
    pos.x = Rand_CenteredFloat(12.0f) + this->actor.world.pos.x;
    pos.y = (Rand_ZeroOne() * 6.0f) + this->actor.world.pos.y;
    pos.z = Rand_CenteredFloat(12.0f) + this->actor.world.pos.z;
    EffectSsKiraKira_SpawnDispersed(play, &pos, &velocity, &accel, &primColor, &envColor, 5000, 16);
}

void ItemEtcetera_MoveFireArrowDown(ItemEtcetera* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f, 5);
    Actor_MoveForward(&this->actor);
    if (!(this->actor.bgCheckFlags & 1)) {
        ItemEtcetera_SpawnSparkles(this, play);
    }
    this->actor.shape.rot.y += 0x400;
    func_80B85824(this, play);
}

void func_80B85B28(ItemEtcetera* this, PlayState* play) {
    if (Flags_GetTreasure(play, (this->actor.params >> 8) & 0x1F)) {
        Actor_Kill(&this->actor);
    }
}

void ItemEtcetera_UpdateFireArrow(ItemEtcetera* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[0] != NULL)) {
        LOG_NUM("(game_play->demo_play.npcdemopnt[0]->dousa)", play->csCtx.npcActions[0]->action);
        if (play->csCtx.npcActions[0]->action == 2) {
            this->actor.draw = ItemEtcetera_Draw;
            this->actor.gravity = -0.1f;
            this->actor.minVelocityY = -4.0f;
            this->actionFunc = ItemEtcetera_MoveFireArrowDown;
        }
    } else {
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -4.0f;
        this->actionFunc = ItemEtcetera_MoveFireArrowDown;
    }
}

void ItemEtcetera_Update(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    this->actionFunc(this, play);
}

void ItemEtcetera_DrawThroughLens(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    if (play->actorCtx.lensActive) {
        func_8002EBCC(&this->actor, play, 0);
        func_8002ED80(&this->actor, play, 0);

        if(IS_RANDO && play->sceneNum == 16) {
            GetItemEntry randoGetItem = GetChestGameRandoGetItem(this->actor.room, this->giDrawId, play);
            EnItem00_CustomItemsParticles(&this->actor, play, randoGetItem);
            if (randoGetItem.itemId != ITEM_NONE) {
                GetItemEntry_Draw(play, randoGetItem);
                return;
            }
        }
        
        GetItem_Draw(play, this->giDrawId);
    }
}

void ItemEtcetera_Draw(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    s32 type = this->actor.params & 0xFF;

    if (IS_RANDO) {
        GetItemEntry randoGetItem = (GetItemEntry)GET_ITEM_NONE;
        if (type == ITEM_ETC_ARROW_FIRE) {
            randoGetItem = Randomizer_GetItemFromKnownCheck(RC_LH_SUN, GI_ARROW_FIRE);
        } else if (type == ITEM_ETC_LETTER) {
            randoGetItem = Randomizer_GetItemFromKnownCheck(RC_LH_UNDERWATER_ITEM, GI_LETTER_RUTO);
        }

        EnItem00_CustomItemsParticles(&this->actor, play, randoGetItem);

        if (randoGetItem.itemId != ITEM_NONE) {
            func_8002EBCC(&this->actor, play, 0);
            func_8002ED80(&this->actor, play, 0);
            GetItemEntry_Draw(play, randoGetItem);
            return;
        }
    }

    func_8002EBCC(&this->actor, play, 0);
    func_8002ED80(&this->actor, play, 0);
    GetItem_Draw(play, this->giDrawId);
}
