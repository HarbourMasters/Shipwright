/*
 * File: z_item_ocarina.c
 * Overlay: ovl_Item_Ocarina
 * Description: Ocarina of Time
 */

#include "z_item_ocarina.h"
#include "scenes/overworld/spot00/spot00_scene.h"

#define FLAGS ACTOR_FLAG_4

void ItemOcarina_Init(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Update(Actor* thisx, GlobalContext* globalCtx);
void ItemOcarina_Draw(Actor* thisx, GlobalContext* globalCtx);

void ItemOcarina_GetThrown(ItemOcarina* this, GlobalContext* globalCtx);
void ItemOcarina_Fly(ItemOcarina* this, GlobalContext* globalCtx);
void ItemOcarina_WaitInWater(ItemOcarina* this, GlobalContext* globalCtx);
void ItemOcarina_StartSoTCutscene(ItemOcarina* this, GlobalContext* globalCtx);
void func_80B864EC(ItemOcarina* this, GlobalContext* globalCtx);
void func_80B865E0(ItemOcarina* this, GlobalContext* globalCtx);
void ItemOcarina_DoNothing(ItemOcarina* this, GlobalContext* globalCtx);

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

void ItemOcarina_Init(Actor* thisx, GlobalContext* globalCtx) {
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
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_ELF_MSG2, 299.0f, -140.0f, 884.0f, 0, 4, 1, 0x3800);
            Actor_SetScale(thisx, 0.2f);
            break;
        default:
            Actor_Kill(thisx);
            return;
    }

    LOG_NUM("no", params);
    this->spinRotOffset = 0x400;
}

void ItemOcarina_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void ItemOcarina_Fly(ItemOcarina* this, GlobalContext* globalCtx) {
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

    if (globalCtx->csCtx.frames == 881) {
        this->actor.world.pos.x = 250.0f;
        this->actor.world.pos.y = 60.0f;
        this->actor.world.pos.z = 1075.0f;
        this->actor.velocity.x = 1.0f;
        this->actor.velocity.y = -5.0f;
        this->actor.velocity.z = -7.0f;
    }

    if (globalCtx->csCtx.frames == 897) {
        EffectSsGRipple_Spawn(globalCtx, &this->actor.world.pos, 100, 500, 0);
        EffectSsGSplash_Spawn(globalCtx, &this->actor.world.pos, 0, 0, 1, 0);
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -0.5f;
        this->spinRotOffset = 0;
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
    }

    // landed in water
    if (globalCtx->csCtx.frames == 906) {
        ripplePos.x = 274.0f;
        ripplePos.y = -60.0f;
        ripplePos.z = 907.0f;
        EffectSsGRipple_Spawn(globalCtx, &ripplePos, 100, 500, 0);
    }
}

void ItemOcarina_GetThrown(ItemOcarina* this, GlobalContext* globalCtx) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 6.0f;
    this->actor.velocity.z = 0.0f;
    ItemOcarina_SetupAction(this, ItemOcarina_Fly);
}

void func_80B864EC(ItemOcarina* this, GlobalContext* globalCtx) {
    func_8002D7EC(&this->actor);
    this->actor.shape.rot.x += this->spinRotOffset * 2;
    this->actor.shape.rot.y += this->spinRotOffset * 3;

    if (this->actor.minVelocityY < this->actor.velocity.y) {
        this->actor.velocity.y += this->actor.gravity;

        if (this->actor.velocity.y < this->actor.minVelocityY) {
            this->actor.velocity.y = this->actor.minVelocityY;
        }
    }

    if (globalCtx->csCtx.frames == 220) {
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

void func_80B865E0(ItemOcarina* this, GlobalContext* globalCtx) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 4.0f;
    this->actor.velocity.z = 6.0f;
    ItemOcarina_SetupAction(this, func_80B864EC);
}

void ItemOcarina_DoNothing(ItemOcarina* this, GlobalContext* globalCtx) {
}

void ItemOcarina_StartSoTCutscene(ItemOcarina* this, GlobalContext* globalCtx) {
    if (Actor_TextboxIsClosing(&this->actor, globalCtx) ||
        Randomizer_ObtainedFreestandingIceTrap(RC_HF_OCARINA_OF_TIME_ITEM, GI_OCARINA_OOT, &this->actor)) {
        if (!gSaveContext.n64ddFlag) {
            globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gHyruleFieldZeldaSongOfTimeCs);
            gSaveContext.cutsceneTrigger = 1;
        } else {
            globalCtx->sceneLoadFlag = 0x14;
            globalCtx->fadeTransition = 3;
            gSaveContext.nextTransition = 3;
            globalCtx->nextEntranceIndex = 0x050F;
            gSaveContext.nextCutsceneIndex = 0;
        }
    }
}

void ItemOcarina_WaitInWater(ItemOcarina* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        gSaveContext.eventChkInf[4] |= 8;
        Flags_SetSwitch(globalCtx, 3);
        this->actionFunc = ItemOcarina_StartSoTCutscene;
        this->actor.draw = NULL;
    } else {
        func_8002F434(&this->actor, globalCtx,
                      gSaveContext.n64ddFlag
                          ? Randomizer_GetItemIdFromKnownCheck(RC_HF_OCARINA_OF_TIME_ITEM, GI_OCARINA_OOT)
                          : GI_OCARINA_OOT,
                      30.0f, 50.0f);

        if ((globalCtx->gameplayFrames & 13) == 0) {
            EffectSsBubble_Spawn(globalCtx, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemOcarina_Update(Actor* thisx, GlobalContext* globalCtx) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    this->actionFunc(this, globalCtx);
}

void ItemOcarina_Draw(Actor* thisx, GlobalContext* globalCtx) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    func_8002EBCC(thisx, globalCtx, 0);
    func_8002ED80(thisx, globalCtx, 0);

    if (gSaveContext.n64ddFlag) {
        s32 randoGetItemId = Randomizer_GetItemIdFromKnownCheck(RC_HF_OCARINA_OF_TIME_ITEM, GI_OCARINA_OOT);
        if (randoGetItemId >= GI_MINUET_OF_FOREST && randoGetItemId <= GI_DOUBLE_DEFENSE) {
            EnItem00_CustomItemsParticles(&this->actor, globalCtx, randoGetItemId);
        }
        GetItem_Draw(globalCtx, Randomizer_GetItemModelFromId(randoGetItemId));
        return;
    }

    GetItem_Draw(globalCtx, GID_OCARINA_TIME);
}
