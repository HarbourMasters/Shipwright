/*
 * File: z_item_ocarina.c
 * Overlay: ovl_Item_Ocarina
 * Description: Ocarina of Time
 */

#include "z_item_ocarina.h"
#include "scenes/overworld/spot00/spot00_scene.h"

#define FLAGS ACTOR_FLAG_4

void ItemOcarina_Init(Actor* thisx, PlayState* play);
void ItemOcarina_Destroy(Actor* thisx, PlayState* play);
void ItemOcarina_Update(Actor* thisx, PlayState* play);
void ItemOcarina_Draw(Actor* thisx, PlayState* play);

void ItemOcarina_GetThrown(ItemOcarina* this, PlayState* play);
void ItemOcarina_Fly(ItemOcarina* this, PlayState* play);
void ItemOcarina_WaitInWater(ItemOcarina* this, PlayState* play);
void ItemOcarina_StartSoTCutscene(ItemOcarina* this, PlayState* play);
void func_80B864EC(ItemOcarina* this, PlayState* play);
void func_80B865E0(ItemOcarina* this, PlayState* play);
void ItemOcarina_DoNothing(ItemOcarina* this, PlayState* play);

const ActorInit Item_Ocarina_InitVars = {
    ACTOR_ITEM_OCARINA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GI_OCARINA,
    sizeof(ItemOcarina),
    (ActorFunc)ItemOcarina_Init,
    (ActorFunc)ItemOcarina_Destroy,
    (ActorFunc)ItemOcarina_Update,
    (ActorFunc)ItemOcarina_Draw,
    NULL,
};

void ItemOcarina_SetupAction(ItemOcarina* this, ItemOcarinaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void ItemOcarina_Init(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;
    s32 params = thisx->params;

    ActorShape_Init(&this->actor.shape, 0, 0, 0);
    Actor_SetScale(&this->actor, 0.1f);

    switch (params) {
        case 0:
            ItemOcarina_SetupAction(this, ItemOcarina_GetThrown);
            break;
        case 1:
            ItemOcarina_SetupAction(this, func_80B865E0);
            break;
        case 2:
            ItemOcarina_SetupAction(this, ItemOcarina_DoNothing);
            break;
        case 3:
            ItemOcarina_SetupAction(this, ItemOcarina_WaitInWater);
            if (!(gSaveContext.eventChkInf[8] & 1) || (gSaveContext.eventChkInf[4] & 8)) {
                Actor_Kill(thisx);
                return;
            }
            Actor_Spawn(&play->actorCtx, play, ACTOR_ELF_MSG2, 299.0f, -140.0f, 884.0f, 0, 4, 1, 0x3800, true);
            Actor_SetScale(thisx, 0.2f);
            break;
        default:
            Actor_Kill(thisx);
            return;
    }

    LOG_NUM("no", params);
    this->spinRotOffset = 0x400;
}

void ItemOcarina_Destroy(Actor* thisx, PlayState* play) {
}

void ItemOcarina_Fly(ItemOcarina* this, PlayState* play) {
    Vec3f ripplePos;

    func_8002D7EC(&this->actor);
    this->actor.shape.rot.x += this->spinRotOffset * 2;
    this->actor.shape.rot.y += this->spinRotOffset * 3;

    if (this->actor.minVelocityY < this->actor.velocity.y) {
        this->actor.velocity.y += this->actor.gravity;
        if (this->actor.velocity.y < this->actor.minVelocityY) {
            this->actor.velocity.y = this->actor.minVelocityY;
        }
    }

    if (play->csCtx.frames == 881) {
        this->actor.world.pos.x = 250.0f;
        this->actor.world.pos.y = 60.0f;
        this->actor.world.pos.z = 1075.0f;
        this->actor.velocity.x = 1.0f;
        this->actor.velocity.y = -5.0f;
        this->actor.velocity.z = -7.0f;
    }

    if (play->csCtx.frames == 897) {
        EffectSsGRipple_Spawn(play, &this->actor.world.pos, 100, 500, 0);
        EffectSsGSplash_Spawn(play, &this->actor.world.pos, 0, 0, 1, 0);
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -0.5f;
        this->spinRotOffset = 0;
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
    }

    // landed in water
    if (play->csCtx.frames == 906) {
        ripplePos.x = 274.0f;
        ripplePos.y = -60.0f;
        ripplePos.z = 907.0f;
        EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 0);
    }
}

void ItemOcarina_GetThrown(ItemOcarina* this, PlayState* play) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 6.0f;
    this->actor.velocity.z = 0.0f;
    ItemOcarina_SetupAction(this, ItemOcarina_Fly);
}

void func_80B864EC(ItemOcarina* this, PlayState* play) {
    func_8002D7EC(&this->actor);
    this->actor.shape.rot.x += this->spinRotOffset * 2;
    this->actor.shape.rot.y += this->spinRotOffset * 3;

    if (this->actor.minVelocityY < this->actor.velocity.y) {
        this->actor.velocity.y += this->actor.gravity;

        if (this->actor.velocity.y < this->actor.minVelocityY) {
            this->actor.velocity.y = this->actor.minVelocityY;
        }
    }

    if (play->csCtx.frames == 220) {
        this->actor.world.pos.x = 144.0f;
        this->actor.world.pos.y = 80.0f;
        this->actor.world.pos.z = 1686.0f;
        this->actor.velocity.x = 1.0f;
        this->actor.velocity.y = 2.0f;
        this->actor.velocity.z = -7.0f;
        this->actor.gravity = -0.15f;
        this->actor.minVelocityY = -5.0f;
    }
}

void func_80B865E0(ItemOcarina* this, PlayState* play) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 4.0f;
    this->actor.velocity.z = 6.0f;
    ItemOcarina_SetupAction(this, func_80B864EC);
}

void ItemOcarina_DoNothing(ItemOcarina* this, PlayState* play) {
}

void ItemOcarina_StartSoTCutscene(ItemOcarina* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        if (!gSaveContext.n64ddFlag) {
            play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gHyruleFieldZeldaSongOfTimeCs);
            gSaveContext.cutsceneTrigger = 1;
        } else {
            play->sceneLoadFlag = 0x14;
            play->fadeTransition = 3;
            gSaveContext.nextTransitionType = 3;
            play->nextEntranceIndex = 0x050F;
            gSaveContext.nextCutsceneIndex = 0;
        }
    }
}

void ItemOcarina_WaitInWater(ItemOcarina* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        gSaveContext.eventChkInf[4] |= 8;
        Flags_SetSwitch(play, 3);
        this->actionFunc = ItemOcarina_StartSoTCutscene;
        this->actor.draw = NULL;
    } else {
        if (!gSaveContext.n64ddFlag) {
            func_8002F434(&this->actor, play, GI_OCARINA_OOT, 30.0f, 50.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_HF_OCARINA_OF_TIME_ITEM, GI_OCARINA_OOT);
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, 30.0f, 50.0f);
        }

        if ((play->gameplayFrames & 13) == 0) {
            EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemOcarina_Update(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    this->actionFunc(this, play);
}

void ItemOcarina_Draw(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    func_8002EBCC(thisx, play, 0);
    func_8002ED80(thisx, play, 0);

    if (gSaveContext.n64ddFlag) {
        GetItemEntry randoGetItem = Randomizer_GetItemFromKnownCheck(RC_HF_OCARINA_OF_TIME_ITEM, GI_OCARINA_OOT);
        EnItem00_CustomItemsParticles(&this->actor, play, randoGetItem);
        GetItemEntry_Draw(play, randoGetItem);
        return;
    }

    GetItem_Draw(play, GID_OCARINA_TIME);
}
