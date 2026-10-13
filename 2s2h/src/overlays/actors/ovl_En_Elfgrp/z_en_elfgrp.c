/**
 * @file z_en_elfgrp.c
 * Overlay: ovl_En_Elfgrp
 * Description: Manager for group of Stray Fairies and Great Fairy in Fairy's Fountains
 *
 * There are many different quantities associated to the Stray Fairies in each of the 3 places (missing, in the
 * Fountain, held by Player)
 *  - Total number of fairies (always 25 in the original game)
 *  - Original number in the Fountain (24 for Clock Town Fairy Fountain, 10 for the other 4)
 *  - Current number in the Fountain (bitpacked in Fountain scene flags, see below)
 *  - Total number found (stored in save context)
 *  - Current number held (i.e. found but not returned)
 *
 * The permanentSceneFlags for Fairy Fountains used in this actor are of the form
 * ```c
 * struct {
 *     u32 clockTown : 1;
 *     u32 fountains[4] : 5;
 * } FairyFountains;
 * ```
 * where arg1 is:
 *  - 0: Clock Town
 *  - 1: Woodfall,
 *  - 2: Snowhead,
 *  - 3: Great Bay,
 *  - 4: Ikana
 *
 * Clock Town is handled separately, and then the fountains are looked up with manual array indexing
 *
 * @note Clock Town is handled separately in a number of places
 */

#include "z_en_elfgrp.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

//! TODO: this file require macros for its uses of weekEventReg

void EnElfgrp_Init(Actor* thisx, PlayState* play);
void EnElfgrp_Destroy(Actor* thisx, PlayState* play);
void EnElfgrp_Update(Actor* thisx, PlayState* play);

s32 EnElfgrp_GetHeldFairiesCount(PlayState* play, s32 type);
s32 EnElfgrp_GetFountainFairiesCount(PlayState* play, s32 type);
void EnElfgrp_SpawnStrayFairies(EnElfgrp* this, PlayState* play, s32 count, s32 fairyType);
void func_80A3A398(EnElfgrp* this, PlayState* play);
void func_80A3A520(EnElfgrp* this, PlayState* play);
void EnElfgrp_DoNothing(EnElfgrp* this, PlayState* play);
void func_80A3A7FC(EnElfgrp* this, PlayState* play);
void func_80A3A8F8(EnElfgrp* this, PlayState* play);

// State flags
#define ELFGRP_STATE_0 (1 << 0)
#define ELFGRP_STATE_1 (1 << 1)
#define ELFGRP_STATE_2 (1 << 2)
#define ELFGRP_STATE_3 (1 << 3)
#define ELFGRP_STATE_4 (1 << 4)

// Used for the type in EnElfgrp_SpawnStrayFairies
typedef enum ElfgrpSpawnedFairyTypes {
    /* 0 */ SPAWNED_STRAY_FAIRY_TYPE_PRESENT,  // STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN
    /* 1 */ SPAWNED_STRAY_FAIRY_TYPE_RETURNING // STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN
} ElfgrpSpawnedFairyTypes;

ActorProfile En_Elfgrp_Profile = {
    /**/ ACTOR_EN_ELFGRP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnElfgrp),
    /**/ EnElfgrp_Init,
    /**/ EnElfgrp_Destroy,
    /**/ EnElfgrp_Update,
    /**/ NULL,
};

/**
 * Set the actor's cutscene id to a later one, or -1 if run out.
 *
 * @note This is only expected to be called in Init when this actor has the first cutscene set, but can also work for
 * later cutscenes if \p numCutscenes is set correcly.
 *
 * @param numCutscenes Number of cutscenes in the list to skip forward to set this one.
 */
void EnElfgrp_SetCutscene(EnElfgrp* this, s32 numCutscenes) {
    while (numCutscenes > 0) {
        if (this->actor.csId == CS_ID_NONE) {
            break;
        }
        this->actor.csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);

        numCutscenes--;
    }
}

void EnElfgrp_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnElfgrp* this = (EnElfgrp*)thisx;
    s32 numberInFountain;

    this->type = ENELFGRP_GET_TYPE(&this->actor);
    this->unk_148 = 0;
    this->stateFlags = 0;
    this->actor.focus.pos.y += 40.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    switch (this->type) {
        case ENELFGRP_TYPE_POWER:
        case ENELFGRP_TYPE_WISDOM:
        case ENELFGRP_TYPE_COURAGE:
        case ENELFGRP_TYPE_KINDNESS:
            this->unk_148 = this->type - 1;
            numberInFountain = EnElfgrp_GetFountainFairiesCount(play, this->type);
            this->talkedOnceFlag = 1 << this->type;

            if (numberInFountain < STRAY_FAIRY_TOTAL) {
                EnElfgrp_SpawnStrayFairies(this, play, numberInFountain, SPAWNED_STRAY_FAIRY_TYPE_PRESENT);
            }

            if (numberInFountain >= STRAY_FAIRY_TOTAL) {
                this->actionFunc = func_80A3A520;
                EnElfgrp_SetCutscene(this, 2);
                break;
            }

            if ((EnElfgrp_GetHeldFairiesCount(play, this->type) + numberInFountain) >= STRAY_FAIRY_TOTAL) {
                this->actionFunc = func_80A3A398;

                switch (this->type) {
                    case ENELFGRP_TYPE_POWER:
                        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK)) {
                            EnElfgrp_SetCutscene(this, 1);
                        } else {
                            this->stateFlags |= ELFGRP_STATE_2;
                        }
                        break;

                    case ENELFGRP_TYPE_WISDOM:
                        if (gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired == true) {
                            EnElfgrp_SetCutscene(this, 1);
                        }
                        break;

                    case ENELFGRP_TYPE_COURAGE:
                        if (gSaveContext.save.saveInfo.playerData.doubleDefense) {
                            EnElfgrp_SetCutscene(this, 1);
                        }
                        break;

                    case ENELFGRP_TYPE_KINDNESS:
                        if (INV_CONTENT(ITEM_SWORD_GREAT_FAIRY) == ITEM_SWORD_GREAT_FAIRY) {
                            EnElfgrp_SetCutscene(this, 1);
                        } else {
                            this->stateFlags |= ELFGRP_STATE_4;
                        }
                        break;

                    default:
                        break;
                }
            } else if (EnElfgrp_GetHeldFairiesCount(play, this->type)) {
                this->actionFunc = func_80A3A7FC;
                this->actor.textId = (this->type * 3) + 0x581;
            } else {
                this->actionFunc = func_80A3A8F8;

                if ((gSaveContext.save.saveInfo.weekEventReg[9] & this->talkedOnceFlag)) { // talked for first time
                    this->actor.textId = (this->type * 3) + 0x580;
                } else {
                    this->actor.textId = (this->type * 3) + 0x57F;
                }
            }
            break;

        default: // ENELFGRP_TYPE_MAGIC
            numberInFountain = EnElfgrp_GetFountainFairiesCount(play, ENELFGRP_TYPE_MAGIC);
            this->talkedOnceFlag = 1 << ENELFGRP_TYPE_MAGIC;

            if (numberInFountain >= STRAY_FAIRY_TOTAL) {
                this->actionFunc = func_80A3A520;

                if ((ENELFGRP_GET_SWITCHFLAG_ROT(&this->actor) != 0) &&
                    Flags_GetSwitch(play, ENELFGRP_GET_SWITCHFLAG_ROT(&this->actor))) {
                    this->actionFunc = EnElfgrp_DoNothing;
                } else if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
                    EnElfgrp_SetCutscene(this, 4);
                } else if (INV_CONTENT(ITEM_MASK_DEKU) != ITEM_MASK_DEKU) {
                    EnElfgrp_SetCutscene(this, 5);
                } else {
                    this->stateFlags |= ELFGRP_STATE_1;
                    EnElfgrp_SetCutscene(this, 6);
                }
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)) {
                EnElfgrp_SpawnStrayFairies(this, play, STRAY_FAIRY_TOTAL - 1, SPAWNED_STRAY_FAIRY_TYPE_PRESENT);
                this->actionFunc = func_80A3A398;

                if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU) {
                    if (INV_CONTENT(ITEM_MASK_GREAT_FAIRY) == ITEM_MASK_GREAT_FAIRY) {
                        EnElfgrp_SetCutscene(this, 2);
                    } else {
                        EnElfgrp_SetCutscene(this, 3);
                        this->stateFlags |= ELFGRP_STATE_1;
                    }
                } else if (gSaveContext.save.saveInfo.playerData.isMagicAcquired == true) {
                    EnElfgrp_SetCutscene(this, 1);
                }
            } else {
                EnElfgrp_SpawnStrayFairies(this, play, STRAY_FAIRY_TOTAL - 1, SPAWNED_STRAY_FAIRY_TYPE_PRESENT);
                this->actionFunc = func_80A3A8F8;
                if ((gSaveContext.save.saveInfo.weekEventReg[9] & this->talkedOnceFlag)) {
                    this->actor.textId = 0x580;
                } else {
                    this->actor.textId = 0x578;
                }
                this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
            }
            break;
    }
}

void EnElfgrp_Destroy(Actor* thisx, PlayState* play) {
}

// Number of Stray Fairies currently held by Player
s32 EnElfgrp_GetHeldFairiesCount(PlayState* play, s32 type) {
    if ((type <= ENELFGRP_TYPE_MAGIC) || (type > ENELFGRP_TYPE_KINDNESS)) {
        return 0;
    }

    // Number in fountain originally + total number collected - number currently in fountain
    return (STRAY_FAIRY_TOTAL - STRAY_FAIRY_SCATTERED_TOTAL) +
           ((void)0, gSaveContext.save.saveInfo.inventory.strayFairies[type - 1]) -
           EnElfgrp_GetFountainFairiesCount(play, type);
}

// Number of Stray Fairies in currently in Fountain
s32 EnElfgrp_GetFountainFairiesCount(PlayState* play, s32 type) {
    s32 numberInFountain;

    if ((type < ENELFGRP_TYPE_MAGIC) || (type > ENELFGRP_TYPE_KINDNESS)) {
        return 0;
    }

    if (type == ENELFGRP_TYPE_MAGIC) {
        if (gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 1) {
            return STRAY_FAIRY_TOTAL;
        } else {
            return STRAY_FAIRY_TOTAL - 1;
        }
    }

    numberInFountain =
        (gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 >> (((type - 1) * 5) + 1)) & 0x1F;
    if (numberInFountain < STRAY_FAIRY_TOTAL - STRAY_FAIRY_SCATTERED_TOTAL) {
        numberInFountain = STRAY_FAIRY_TOTAL - STRAY_FAIRY_SCATTERED_TOTAL;
    } else if (numberInFountain > STRAY_FAIRY_TOTAL) {
        numberInFountain = STRAY_FAIRY_TOTAL;
    }
    return numberInFountain;
}

// Update number of Stray Fairies in Fountain
void EnElfgrp_SetFountainFairiesCount(PlayState* play, s32 type, s32 newCount) {
    if ((type < ENELFGRP_TYPE_MAGIC) || (type > ENELFGRP_TYPE_KINDNESS) ||
        (newCount < (STRAY_FAIRY_TOTAL - STRAY_FAIRY_SCATTERED_TOTAL)) || (newCount > STRAY_FAIRY_TOTAL)) {
        return;
    }

    if (type == ENELFGRP_TYPE_MAGIC) {
        if (newCount == STRAY_FAIRY_TOTAL) {
            gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= 1;
        } else {
            gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 &= ~1;
        }
    } else {
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 &= ~(0x1F << ((type * 5) - 4));
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= newCount << ((type * 5) - 4);
    }
}

void EnElfgrp_SpawnStrayFairies(EnElfgrp* this, PlayState* play, s32 count, s32 fairyType) {
    s32 pad;
    s32 i;
    Actor* strayFairy;
    s32 strayFairyParams;
    Vec3f spawnCenterPos;
    Player* player = GET_PLAYER(play);

    if (fairyType == 0) {
        this->stateFlags |= ELFGRP_STATE_3;
    }

    if (fairyType == 0) {
        spawnCenterPos = this->actor.world.pos;
        spawnCenterPos.y += 20.0f;
        strayFairyParams = STRAY_FAIRY_PARAMS(0, this->type, STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN);
    } else {
        spawnCenterPos = player->actor.world.pos;
        spawnCenterPos.y += 20.0f;
        strayFairyParams = STRAY_FAIRY_PARAMS(0, this->type, STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN);
    }

    for (i = 0; i < count; i++) {
        strayFairy =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELFORG, Rand_CenteredFloat(20.0f) + spawnCenterPos.x,
                        spawnCenterPos.y, Rand_CenteredFloat(20.0f) + spawnCenterPos.z, 0, 0, 0, strayFairyParams);

        if (strayFairy == NULL) {
            continue;
        }
        strayFairy->home.pos.x = this->actor.home.pos.x;
        strayFairy->home.pos.y = this->actor.home.pos.y + 20.0f;
        strayFairy->home.pos.z = this->actor.home.pos.z;
    }
}

/**
 * Tell any spawned Stray Fairies to come to the Fountain center.
 *
 * @return s32 always 0
 */
s32 EnElfgrp_SummonStrayFairies(PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    EnElforg* strayFairy;

    while (itemAction != NULL) {
        if ((itemAction->id != ACTOR_EN_ELFORG) ||
            ((STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN) &&
             (STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN))) {
            itemAction = itemAction->next;
            continue;
        }

        strayFairy = (EnElforg*)itemAction;
        if (!(strayFairy->strayFairyFlags & STRAY_FAIRY_FLAG_MOVES_QUICKLY_TO_HOME)) {
            strayFairy->strayFairyFlags |= STRAY_FAIRY_FLAG_MOVES_QUICKLY_TO_HOME;
        }
        itemAction = itemAction->next;
    }

    return 0;
}

/**
 * Make the Stray Fairies in the fountain spin quickly when healing Player.
 *
 * @return s32 time to spend in healing action.
 */
s32 EnElfgrp_SpinStrayFairies(PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    EnElforg* strayFairy;
    s32 timer = 30;

    while (itemAction != NULL) {
        if ((itemAction->id != ACTOR_EN_ELFORG) ||
            ((STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN) &&
             (STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN))) {
            itemAction = itemAction->next;
            continue;
        }

        strayFairy = (EnElforg*)itemAction;
        if (!(strayFairy->strayFairyFlags & STRAY_FAIRY_FLAG_CIRCLES_QUICKLY_IN_FOUNTAIN)) {
            strayFairy->strayFairyFlags |= STRAY_FAIRY_FLAG_CIRCLES_QUICKLY_IN_FOUNTAIN;
            if (timer >= 100) {
                return timer;
            }
            strayFairy->secondaryTimer = timer;
            timer += 5;
        }

        itemAction = itemAction->next;
    }

    return timer;
}

/**
 * Tell the Stray Fairies to disappear, before reviving the Great Fairy
 */
void EnElfgrp_VanishStrayFairies(PlayState* play) {
    Actor* itemAction = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    EnElforg* strayFairy;

    while (itemAction != NULL) {
        if ((itemAction->id != ACTOR_EN_ELFORG) ||
            ((STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_FAIRY_FOUNTAIN) &&
             (STRAY_FAIRY_TYPE(itemAction) != STRAY_FAIRY_TYPE_RETURNING_TO_FOUNTAIN))) {
            itemAction = itemAction->next;
            continue;
        }

        strayFairy = (EnElforg*)itemAction;
        STRAY_FAIRY_SPARKLE_COUNT(&strayFairy->actor) = 20;
        strayFairy->strayFairyFlags |= STRAY_FAIRY_FLAG_SPARKLES_AND_SHRINKS;

        itemAction = itemAction->next;
    }
}

void func_80A3A0AC(EnElfgrp* this, PlayState* play) {
    if (!Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_100)) {
        this->actionFunc = EnElfgrp_DoNothing;
        CutsceneManager_Stop(this->actor.csId);
    }
}

void func_80A3A0F4(EnElfgrp* this, PlayState* play) {
    if (this->timer == 10) {
        Audio_PlaySfx(NA_SE_SY_WHITE_OUT_T);
        if (ENELFGRP_GET_TYPE(&this->actor) < ENELFGRP_TYPE_KINDNESS) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y + 30.0f, this->actor.world.pos.z, 0, 0, 0,
                        ENELFGRP_GET_TYPE(&this->actor) + DEMO_EFFECT_TYPE_LIGHT_BASE);
        } else { // ENELFGRP_TYPE_KINDNESS
            Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->actor.world.pos.x,
                        this->actor.world.pos.y + 30.0f, this->actor.world.pos.z, 0, 0, 0,
                        DEMO_EFFECT_TYPE_LIGHT_DARK_YELLOW);
        }
    }

    if ((this->timer > 10) && (this->stateFlags & ELFGRP_STATE_0)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FAIRY_GROUP_FRY - SFX_FLAG);
    }

    if (this->timer == 0) {
        this->actionFunc = func_80A3A0AC;
    }
}

void func_80A3A210(EnElfgrp* this, PlayState* play) {
    if (this->timer == 0) {
        this->actionFunc = func_80A3A0F4;
        EnElfgrp_VanishStrayFairies(play);
        this->timer = 30;
    }

    if (this->stateFlags & ELFGRP_STATE_0) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FAIRY_GROUP_FRY - SFX_FLAG);
    }
}

void func_80A3A274(EnElfgrp* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_100)) {
        if (this->stateFlags & ELFGRP_STATE_0) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_CHIBI_FAIRY_HEAL - SFX_FLAG);
        }

        switch (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_100)]->id) {
            case 2:
                if (!(this->stateFlags & ELFGRP_STATE_0)) {
                    if (this->type == ENELFGRP_TYPE_MAGIC) { // Clock Town
                        EnElfgrp_SpawnStrayFairies(this, play, 1, SPAWNED_STRAY_FAIRY_TYPE_RETURNING);
                    } else {
                        EnElfgrp_SpawnStrayFairies(this, play, EnElfgrp_GetHeldFairiesCount(play, this->type),
                                                   SPAWNED_STRAY_FAIRY_TYPE_RETURNING);
                    }
                    this->stateFlags |= ELFGRP_STATE_0;
                    EnElfgrp_SetFountainFairiesCount(play, this->type, STRAY_FAIRY_TOTAL);
                }
                break;

            case 3:
                EnElfgrp_SummonStrayFairies(play);
                this->actionFunc = func_80A3A210;
                this->timer = 90;
                break;

            default:
                break;
        }
    }
}

void func_80A3A398(EnElfgrp* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->actionFunc = func_80A3A274;
        Flags_UnsetSwitch(play, ENELFGRP_GET_SWITCH_FLAG_PARAMS(&this->actor));

        if (!GameInteractor_Should(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, true, this)) {
            goto skipGiveItem;
        }

        if (this->stateFlags & ELFGRP_STATE_1) {
            Item_Give(play, ITEM_MASK_GREAT_FAIRY);
        }

        if (this->stateFlags & ELFGRP_STATE_2) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK);
        }

        if (this->stateFlags & ELFGRP_STATE_4) {
            Item_Give(play, ITEM_SWORD_GREAT_FAIRY);
        }

    skipGiveItem: // #2S2H
        this->stateFlags &= ~ELFGRP_STATE_3;
    } else if (GameInteractor_Should(VB_START_GREAT_FAIRY_CUTSCENE, this->actor.xzDistToPlayer < 350.0f, this)) {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80A3A484(EnElfgrp* this, PlayState* play) {
    if (this->timer == 0) {
        this->actionFunc = func_80A3A0F4;
        this->timer = 30;
    }
}

void func_80A3A4AC(EnElfgrp* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_100)) {
        s32 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_100)]->id;

        if (cueId == 3) {
            this->actionFunc = func_80A3A484;
            this->timer = 90;
        }
    }
}

void func_80A3A520(EnElfgrp* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_103)) {
        this->actionFunc = EnElfgrp_DoNothing;
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        this->actionFunc = func_80A3A4AC;
        Flags_SetSwitch(play, ENELFGRP_GET_SWITCH_FLAG_PARAMS(&this->actor));

        if (GameInteractor_Should(VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, true, this)) {
            if (this->stateFlags & ELFGRP_STATE_1) {
                Item_Give(play, ITEM_MASK_GREAT_FAIRY);
            }
        }

        if (ENELFGRP_GET_SWITCHFLAG_ROT(&this->actor) != 0) {
            Flags_SetSwitch(play, ENELFGRP_GET_SWITCHFLAG_ROT(&this->actor));
        }
    } else if (GameInteractor_Should(VB_START_GREAT_FAIRY_CUTSCENE, this->actor.xzDistToPlayer < 350.0f, this)) {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void EnElfgrp_DoNothing(EnElfgrp* this, PlayState* play) {
}

void EnElfgrp_HealPlayer(EnElfgrp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer == 60) {
        Magic_Add(play, MAGIC_FILL_TO_CAPACITY);
        gSaveContext.healthAccumulator = 0x140;
    }

    if (this->timer > 0) {
        player->actor.freezeTimer = 100;
        player->stateFlags1 |= PLAYER_STATE1_20000000;
        Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_GROUP_HEAL - SFX_FLAG);
    } else {
        player->actor.freezeTimer = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20000000;
        this->actionFunc = EnElfgrp_DoNothing;
        this->stateFlags |= ELFGRP_STATE_3;
    }
}

void func_80A3A6F4(EnElfgrp* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        player->actor.freezeTimer = 100;
        player->stateFlags1 |= PLAYER_STATE1_20000000;
        this->timer = EnElfgrp_SpinStrayFairies(play);
        this->actionFunc = EnElfgrp_HealPlayer;
        this->stateFlags &= ~ELFGRP_STATE_3;
    }
}

void func_80A3A77C(EnElfgrp* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.freezeTimer = 100;
    player->stateFlags1 |= PLAYER_STATE1_20000000;
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->timer = EnElfgrp_SpinStrayFairies(play);
        this->actionFunc = EnElfgrp_HealPlayer;
        this->stateFlags &= ~ELFGRP_STATE_3;
    }
}

void func_80A3A7FC(EnElfgrp* this, PlayState* play) {
    s32 curTotalFairies;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        gSaveContext.save.saveInfo.weekEventReg[9] |= this->talkedOnceFlag;
        this->actionFunc = func_80A3A6F4;

        curTotalFairies = EnElfgrp_GetHeldFairiesCount(play, this->type);
        EnElfgrp_SpawnStrayFairies(this, play, curTotalFairies, SPAWNED_STRAY_FAIRY_TYPE_RETURNING);
        curTotalFairies += EnElfgrp_GetFountainFairiesCount(play, this->type);
        if (curTotalFairies > STRAY_FAIRY_TOTAL) {
            curTotalFairies = STRAY_FAIRY_TOTAL;
        }

        EnElfgrp_SetFountainFairiesCount(play, this->type, curTotalFairies);
    } else if (this->actor.xzDistToPlayer < 280.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 300.0f);
    }
}

void func_80A3A8F8(EnElfgrp* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        gSaveContext.save.saveInfo.weekEventReg[9] |= this->talkedOnceFlag;
        this->actionFunc = func_80A3A6F4;
        return;
    }

    if (this->type != ENELFGRP_TYPE_MAGIC) {
        if (EnElfgrp_GetHeldFairiesCount(play, this->type) > 0) {
            this->actionFunc = func_80A3A7FC;
            return;
        }
    }

    if (this->actor.xzDistToPlayer < 30.0f) {
        if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            player->actor.freezeTimer = 100;
            player->stateFlags1 |= PLAYER_STATE1_20000000;
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            this->actionFunc = func_80A3A77C;
            gSaveContext.save.saveInfo.weekEventReg[9] |= this->talkedOnceFlag;
        } else {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void EnElfgrp_Update(Actor* thisx, PlayState* play) {
    EnElfgrp* this = (EnElfgrp*)thisx;

    this->actionFunc(this, play);

    if (this->stateFlags & ELFGRP_STATE_3) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_FAIRY_GROUP_FRY - SFX_FLAG);
    }

    if (this->timer != 0) {
        this->timer--;
    }
}
