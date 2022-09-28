/*
 * File: z_en_ex_item.c
 * Overlay: ovl_En_Ex_Item
 * Description: Minigame prize items
 */

#include "z_en_ex_item.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnExItem_Init(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Update(Actor* thisx, GlobalContext* globalCtx);
void EnExItem_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnExItem_DrawItems(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_DrawHeartPiece(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_DrawRupee(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_DrawKey(EnExItem* this, GlobalContext* globalCtx, s32 index);
void EnExItem_DrawMagic(EnExItem* this, GlobalContext* globalCtx, s16 magicIndex);

void EnExItem_WaitForObject(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_BowlPrize(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_SetupBowlCounter(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_BowlCounter(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_ExitChest(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_FairyMagic(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_TargetPrizeApproach(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_TargetPrizeGive(EnExItem* this, GlobalContext* globalCtx);
void EnExItem_TargetPrizeFinish(EnExItem* this, GlobalContext* globalCtx);

const ActorInit En_Ex_Item_InitVars = {
    ACTOR_EN_EX_ITEM,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnExItem),
    (ActorFunc)EnExItem_Init,
    (ActorFunc)EnExItem_Destroy,
    (ActorFunc)EnExItem_Update,
    (ActorFunc)EnExItem_Draw,
    NULL,
};

void EnExItem_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnExItem_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_0;
    this->type = this->actor.params & 0xFF;
    this->unusedParam = (this->actor.params >> 8) & 0xFF;
    osSyncPrintf("\n\n");
    // "What will come out?"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, this->type);
    // "What will come out?"
    osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ なにがでるかな？ ☆☆☆☆☆ %d\n" VT_RST, this->unusedParam);
    this->initPos = this->actor.world.pos;
    this->getItemObjId = -1;
    switch (this->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
            this->getItemObjId = OBJECT_GI_BOMBPOUCH;
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            this->getItemObjId = OBJECT_GI_HEARTS;
            break;
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBCHUS_COUNTER:
            this->getItemObjId = OBJECT_GI_BOMB_2;
            break;
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMBS_COUNTER:
            this->getItemObjId = OBJECT_GI_BOMB_1;
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            this->getItemObjId = OBJECT_GI_RUPY;
            break;
        case EXITEM_SMALL_KEY_CHEST:
            this->scale = 0.05f;
            this->actor.velocity.y = 10.0f;
            this->timer = 7;
            this->actionFunc = EnExItem_ExitChest;
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            this->getItemObjId = OBJECT_GI_GODDESS;
            break;
        case EXITEM_BULLET_BAG:
            this->getItemObjId = OBJECT_GI_DEKUPOUCH;
    }

    if (this->getItemObjId >= 0) {
        this->objectIdx = Object_GetIndex(&globalCtx->objectCtx, this->getItemObjId);
        this->actor.draw = NULL;
        if (this->objectIdx < 0) {
            Actor_Kill(&this->actor);
            // "What?"
            osSyncPrintf("なにみの？ %d\n", this->actor.params);
            // "bank is funny"
            osSyncPrintf(VT_FGCOL(PURPLE) " バンクおかしいしぞ！%d\n" VT_RST "\n", this->actor.params);
            return;
        }
        this->actionFunc = EnExItem_WaitForObject;
    }
}

void EnExItem_WaitForObject(EnExItem* this, GlobalContext* globalCtx) {
    s32 onCounter;

    if (Object_IsLoaded(&globalCtx->objectCtx, this->objectIdx)) {
        // "End of transfer"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params, this);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params, this);
        osSyncPrintf(VT_FGCOL(BLUE) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params, this);
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n" VT_RST, this->actor.params, this);
        osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ 転送終了 ☆☆☆☆☆ %d\n\n" VT_RST, this->actor.params, this);
        this->actor.objBankIndex = this->objectIdx;
        this->actor.draw = EnExItem_Draw;
        this->stopRotate = false;
        onCounter = false;
        switch (this->type) {
            case EXITEM_BOMB_BAG_COUNTER:
                onCounter = true;
            case EXITEM_BOMB_BAG_BOWLING:
                this->unk_17C = func_8002EBCC;
                if (gSaveContext.n64ddFlag) {
                    this->giDrawId =
                        Randomizer_GetItemFromKnownCheck(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, GI_BOMB_BAG_20).gid;
                } else {
                    this->giDrawId = GID_BOMB_BAG_30;
                }
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (onCounter == 0) {
                    this->actionFunc = EnExItem_BowlPrize;
                } else {
                    this->actionFunc = EnExItem_SetupBowlCounter;
                    this->actor.shape.yOffset = gSaveContext.n64ddFlag ? -10.0f : -18.0f;
                }
                break;
            case EXITEM_HEART_PIECE_COUNTER:
                onCounter = true;
            case EXITEM_HEART_PIECE_BOWLING:
                this->unk_17C = func_8002ED80;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (!onCounter) {
                    func_80078884(NA_SE_SY_PIECE_OF_HEART);
                    this->actionFunc = EnExItem_BowlPrize;
                } else {
                    this->actionFunc = EnExItem_SetupBowlCounter;
                    this->actor.shape.yOffset = -10.0f;
                }
                break;
            case EXITEM_BOMBCHUS_COUNTER:
                onCounter = true;
            case EXITEM_BOMBCHUS_BOWLING:
                this->unk_17C = func_8002EBCC;
                if (gSaveContext.n64ddFlag) {
                    this->giDrawId = Randomizer_GetItemFromKnownCheck(RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS, GI_BOMBCHUS_10).gid;
                } else {
                    this->giDrawId = GID_BOMBCHU;
                }
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                if (!onCounter) {
                    this->actionFunc = EnExItem_BowlPrize;
                } else {
                    this->actionFunc = EnExItem_SetupBowlCounter;
                }
                break;
            case EXITEM_BOMBS_BOWLING:
            case EXITEM_BOMBS_COUNTER:
                this->unk_17C = func_8002EBCC;
                this->giDrawId = GID_BOMB;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                if (this->type == EXITEM_BOMBS_BOWLING) {
                    this->actionFunc = EnExItem_BowlPrize;
                } else {
                    this->actionFunc = EnExItem_SetupBowlCounter;
                    this->actor.shape.yOffset = 10.0f;
                }
                break;
            case EXITEM_PURPLE_RUPEE_BOWLING:
            case EXITEM_PURPLE_RUPEE_COUNTER:
                this->unk_17C = func_8002EBCC;
                this->unk_180 = func_8002ED80;
                this->giDrawId = GID_RUPEE_PURPLE;
                this->timer = 65;
                this->prizeRotateTimer = 35;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                if (this->type == EXITEM_PURPLE_RUPEE_BOWLING) {
                    this->actionFunc = EnExItem_BowlPrize;
                } else {
                    this->actionFunc = EnExItem_SetupBowlCounter;
                    this->actor.shape.yOffset = 40.0f;
                }
                break;
            case EXITEM_GREEN_RUPEE_CHEST:
            case EXITEM_BLUE_RUPEE_CHEST:
            case EXITEM_RED_RUPEE_CHEST:
            case EXITEM_13:
            case EXITEM_14:
                this->unk_17C = func_8002EBCC;
                this->unk_180 = func_8002ED80;
                this->timer = 7;
                this->scale = 0.5f;
                this->unkFloat = 0.5f;
                this->actor.velocity.y = 10.0f;
                if (!gSaveContext.n64ddFlag || !Randomizer_GetSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                    switch (this->type) {
                        case EXITEM_GREEN_RUPEE_CHEST:
                            this->giDrawId = GID_RUPEE_GREEN;
                            break;
                        case EXITEM_BLUE_RUPEE_CHEST:
                            this->giDrawId = GID_RUPEE_BLUE;
                            break;
                        case EXITEM_RED_RUPEE_CHEST:
                            this->giDrawId = GID_RUPEE_RED;
                            break;
                        case EXITEM_14:
                            this->giDrawId = GID_RUPEE_PURPLE;
                            break;
                    }
                } else {
                    if (globalCtx->sceneNum == 16) {
                        this->giDrawId = GetChestGameRandoGiDrawId(globalCtx->roomCtx.curRoom.num, GID_RUPEE_GREEN, globalCtx);
                    }
                }
                this->actionFunc = EnExItem_ExitChest;
                break;
            case EXITEM_MAGIC_FIRE:
            case EXITEM_MAGIC_WIND:
            case EXITEM_MAGIC_DARK:
                this->scale = 0.35f;
                this->actionFunc = EnExItem_FairyMagic;
                break;
            case EXITEM_BULLET_BAG:
                this->unk_17C = func_8002EBCC;
                this->giDrawId = GID_BULLET_BAG;
                this->scale = 0.1f;
                this->timer = 80;
                this->prizeRotateTimer = 35;
                this->actionFunc = EnExItem_TargetPrizeApproach;
                break;
            case EXITEM_SMALL_KEY_CHEST:
                break;
        }
    }
}

void EnExItem_BowlPrize(EnExItem* this, GlobalContext* globalCtx) {
    s32 pad;
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;
    f32 sp3C;

    if (!this->stopRotate) {
        this->actor.shape.rot.y += 0x1000;
        if ((this->prizeRotateTimer == 0) && ((this->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            this->stopRotate++;
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, 0, 5, 0x1000, 0);
    }
    if (this->timer != 0) {
        if (this->prizeRotateTimer != 0) {
            sp3C = 250.0f;
            if (this->type == EXITEM_BOMBCHUS_BOWLING) {
                sp3C = 220.0f;
            }
            tmpf1 = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
            tmpf2 = globalCtx->view.lookAt.y - globalCtx->view.eye.y;
            tmpf3 = globalCtx->view.lookAt.z + sp3C - globalCtx->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = globalCtx->view.eye.x + tmpf5 - this->actor.world.pos.x;
            tmpf2 = globalCtx->view.eye.y + tmpf6 - this->actor.world.pos.y;
            tmpf3 = globalCtx->view.eye.z + tmpf7 - this->actor.world.pos.z;

            this->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            this->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            this->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        // "parent"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 母親ー？     ☆☆☆☆☆ %x\n" VT_RST, this->actor.parent);
        // "Can it move?"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 動いてねー？ ☆☆☆☆☆ %x\n" VT_RST, this->actor.parent->update);
        if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL)) {
            ((EnBomBowlPit*)this->actor.parent)->exItemDone = 1;
            // "It can't move!"
            osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ さぁきえるぞ！ ☆☆☆☆☆ \n" VT_RST);
        }
        Actor_Kill(&this->actor);
    }
}

void EnExItem_SetupBowlCounter(EnExItem* this, GlobalContext* globalCtx) {
    this->actor.world.rot.y = this->actor.shape.rot.y = 0x4268;
    this->actionFunc = EnExItem_BowlCounter;
}

void EnExItem_BowlCounter(EnExItem* this, GlobalContext* globalCtx) {
    this->actor.shape.rot.y += 0x800;
    if (this->killItem) {
        Actor_Kill(&this->actor);
    }
}

void EnExItem_ExitChest(EnExItem* this, GlobalContext* globalCtx) {
    this->actor.shape.rot.y += 0x1000;
    if (this->timer != 0) {
        if (this->timer == 1) {
            this->chestKillTimer = 20;
        }
    } else {
        this->actor.velocity.y = 0.0f;
        if (this->chestKillTimer == 0) {
            Actor_Kill(&this->actor);
        }
    }
    Actor_MoveForward(&this->actor);
}

void EnExItem_FairyMagic(EnExItem* this, GlobalContext* globalCtx) {
    this->actor.shape.rot.y += 0x800;
}

void EnExItem_TargetPrizeApproach(EnExItem* this, GlobalContext* globalCtx) {
    f32 tmpf1;
    f32 tmpf2;
    f32 tmpf3;
    f32 tmpf4;
    f32 tmpf5;
    f32 tmpf6;
    f32 tmpf7;

    Math_ApproachF(&this->scale, 0.8f, 0.1f, 0.02f);
    if (!this->stopRotate) {
        this->actor.shape.rot.y += 0x1000;
        if ((this->prizeRotateTimer == 0) && ((this->actor.shape.rot.y & 0xFFFF) == 0x9000)) {
            this->stopRotate++;
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, -0x4000, 5, 0x1000, 0);
    }

    if (!gSaveContext.n64ddFlag && this->timer != 0) {
        if (this->prizeRotateTimer != 0) {
            tmpf1 = globalCtx->view.lookAt.x - globalCtx->view.eye.x;
            tmpf2 = globalCtx->view.lookAt.y - 10.0f - globalCtx->view.eye.y;
            tmpf3 = globalCtx->view.lookAt.z + 10.0f - globalCtx->view.eye.z;
            tmpf4 = sqrtf(SQ(tmpf1) + SQ(tmpf2) + SQ(tmpf3));

            tmpf5 = (tmpf1 / tmpf4) * 5.0f;
            tmpf6 = (tmpf2 / tmpf4) * 5.0f;
            tmpf7 = (tmpf3 / tmpf4) * 5.0f;

            tmpf1 = globalCtx->view.eye.x + tmpf5 - this->actor.world.pos.x;
            tmpf2 = globalCtx->view.eye.y - 10.0f + tmpf6 - this->actor.world.pos.y;
            tmpf3 = globalCtx->view.eye.z + 10.0f + tmpf7 - this->actor.world.pos.z;

            this->actor.world.pos.x += (tmpf1 / tmpf4) * 5.0f;
            this->actor.world.pos.y += (tmpf2 / tmpf4) * 5.0f;
            this->actor.world.pos.z += (tmpf3 / tmpf4) * 5.0f;
        }
    } else {
        GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        s32 getItemId;

        this->actor.draw = NULL;
        func_8002DF54(globalCtx, NULL, 7);
        this->actor.parent = NULL;
        if (gSaveContext.n64ddFlag) {
            GET_PLAYER(globalCtx)->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_11);
            getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_TARGET_IN_WOODS, GI_BULLET_BAG_50);
            getItemId = getItemEntry.getItemId;
        } else {
            if (CUR_UPG_VALUE(UPG_BULLET_BAG) == 1) {
                getItemId = GI_BULLET_BAG_40;
            } else {
                getItemId = GI_BULLET_BAG_50;
            }
        }

        if (!gSaveContext.n64ddFlag || getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, globalCtx, getItemId, 2000.0f, 1000.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 2000.0f, 1000.0f);
        }
        this->actionFunc = EnExItem_TargetPrizeGive;
    }
}

void EnExItem_TargetPrizeGive(EnExItem* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->actionFunc = EnExItem_TargetPrizeFinish;
    } else {
        if (!gSaveContext.n64ddFlag) {
            s32 getItemId = (CUR_UPG_VALUE(UPG_BULLET_BAG) == 2) ? GI_BULLET_BAG_50 : GI_BULLET_BAG_40;
            func_8002F434(&this->actor, globalCtx, getItemId, 2000.0f, 1000.0f);
        } else {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LW_TARGET_IN_WOODS, GI_BULLET_BAG_50);
            GiveItemEntryFromActor(&this->actor, globalCtx, getItemEntry, 2000.0f, 1000.0f);
        }

    }
}

void EnExItem_TargetPrizeFinish(EnExItem* this, GlobalContext* globalCtx) {
    if ((Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(globalCtx)) {
        // "Successful completion"
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 正常終了 ☆☆☆☆☆ \n" VT_RST);
        gSaveContext.itemGetInf[1] |= 0x2000;
        Actor_Kill(&this->actor);
    }
}

void EnExItem_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->chestKillTimer != 0) {
        this->chestKillTimer--;
    }
    if (this->prizeRotateTimer != 0) {
        this->prizeRotateTimer--;
    }
    this->actionFunc(this, globalCtx);
}

void EnExItem_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    EnExItem* this = (EnExItem*)thisx;
    s32 magicType;

    Actor_SetScale(&this->actor, this->scale);
    switch (this->type) {
        case EXITEM_BOMB_BAG_BOWLING:
        case EXITEM_BOMBCHUS_BOWLING:
        case EXITEM_BOMBS_BOWLING:
        case EXITEM_BOMB_BAG_COUNTER:
        case EXITEM_BOMBCHUS_COUNTER:
        case EXITEM_BOMBS_COUNTER:
        case EXITEM_BULLET_BAG:
            EnExItem_DrawItems(this, globalCtx);
            break;
        case EXITEM_HEART_PIECE_BOWLING:
        case EXITEM_HEART_PIECE_COUNTER:
            EnExItem_DrawHeartPiece(this, globalCtx);
            break;
        case EXITEM_PURPLE_RUPEE_BOWLING:
        case EXITEM_PURPLE_RUPEE_COUNTER:
        case EXITEM_GREEN_RUPEE_CHEST:
        case EXITEM_BLUE_RUPEE_CHEST:
        case EXITEM_RED_RUPEE_CHEST:
        case EXITEM_13:
        case EXITEM_14:
            EnExItem_DrawRupee(this, globalCtx);
            break;
        case EXITEM_SMALL_KEY_CHEST:
            EnExItem_DrawKey(this, globalCtx, 0);
            break;
        case EXITEM_MAGIC_FIRE:
        case EXITEM_MAGIC_WIND:
        case EXITEM_MAGIC_DARK:
            magicType = this->type - EXITEM_MAGIC_FIRE;
            EnExItem_DrawMagic(this, globalCtx, magicType);
            break;
    }
}

void EnExItem_DrawItems(EnExItem* this, GlobalContext* globalCtx) {
    if (this->unk_17C != NULL) {
        this->unk_17C(&this->actor, globalCtx, 0);
    }
    if (this) {}
    func_8002ED80(&this->actor, globalCtx, 0);
    if (gSaveContext.n64ddFlag) {
        GetItemEntry randoGetItem = (GetItemEntry)GET_ITEM_NONE;
        switch (this->type) {
            case EXITEM_BOMB_BAG_BOWLING:
            case EXITEM_BOMB_BAG_COUNTER:
                randoGetItem = Randomizer_GetItemFromKnownCheck(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, GI_BOMB_BAG_20);
                break;
            case EXITEM_BOMBCHUS_BOWLING:
            case EXITEM_BOMBCHUS_COUNTER:
                randoGetItem = Randomizer_GetItemFromKnownCheck(RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS, GI_BOMBCHUS_10);
                break;
            case EXITEM_BULLET_BAG:
                randoGetItem = Randomizer_GetItemFromKnownCheck(RC_LW_TARGET_IN_WOODS, GI_BULLET_BAG_50);
                break;
        }

        if (randoGetItem.getItemId != GI_NONE) {
            EnItem00_CustomItemsParticles(&this->actor, globalCtx, randoGetItem);
            GetItemEntry_Draw(globalCtx, randoGetItem);
            return;
        }
    }

    GetItem_Draw(globalCtx, this->giDrawId);
}

void EnExItem_DrawHeartPiece(EnExItem* this, GlobalContext* globalCtx) {
    func_8002ED80(&this->actor, globalCtx, 0);

    if (gSaveContext.n64ddFlag) {
        GetItemEntry randoGetItem =
            Randomizer_GetItemFromKnownCheck(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, GI_HEART_PIECE);
        EnItem00_CustomItemsParticles(&this->actor, globalCtx, randoGetItem);
        GetItemEntry_Draw(globalCtx, randoGetItem);
    } else {
        GetItem_Draw(globalCtx, GID_HEART_PIECE);
    }
}

void EnExItem_DrawMagic(EnExItem* this, GlobalContext* globalCtx, s16 magicIndex) {
    static s16 giDrawIds[] = { GID_DINS_FIRE, GID_FARORES_WIND, GID_NAYRUS_LOVE };

    func_8002ED80(&this->actor, globalCtx, 0);
    GetItem_Draw(globalCtx, giDrawIds[magicIndex]);
}

void EnExItem_DrawKey(EnExItem* this, GlobalContext* globalCtx, s32 index) {
    static void* keySegments[] = { gDropKeySmallTex };

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_8009460C(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(keySegments[index]));
    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void EnExItem_DrawRupee(EnExItem* this, GlobalContext* globalCtx) {
    if (this->unk_17C != NULL) {
        this->unk_17C(&this->actor, globalCtx, 0);
    }
    if (this->unk_180 != NULL) {
        this->unk_180(&this->actor, globalCtx, 0);
    }
    GetItem_Draw(globalCtx, this->giDrawId);
}
