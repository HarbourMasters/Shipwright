#pragma once

#ifndef GI_VANILLA_BEHAVIOR_H
#define GI_VANILLA_BEHAVIOR_H

typedef enum {
    // #### `result`
    // ```c
    // ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_2) && CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) ||
    // ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_3) && CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) ||
    // ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_4) && CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) ||
    // ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_5) && CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD))
    // ```
    // #### `args`
    // - `*DoorWarp1`
    VB_ACTIVATE_BOSS_WARP_PAD,

    // #### `result`
    // ```c
    // GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 3
    // ```
    // #### `args`
    // - `*EnAkindonuts` (unused)
    VB_AKINDONUTS_CONSIDER_BOMB_BAG_PURCHASED,

    // #### `result`
    // ```c
    // !((u32)INV_CONTENT(ITEM_MAGIC_BEANS) != ITEM_MAGIC_BEANS)
    // ```
    // #### `args`
    // - `*EnAkindonuts` (unused)
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BEAN_REFILL,

    // #### `result`
    // ```c
    // !(GET_CUR_UPG_VALUE(UPG_BOMB_BAG) < 2)
    // ```
    // #### `args`
    // - `*EnAkindonuts` (unused)
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_BOMB_BAG,

    // #### `result`
    // ```c
    // Inventory_HasEmptyBottle()
    // ```
    // #### `args`
    // - `*EnAkindonuts`
    VB_AKINDONUTS_CONSIDER_ELIGIBLE_FOR_POTION_REFILL,

    // #### `result`
    // ```c
    // (CURRENT_DAY != 3) || (gSaveContext.save.isNight == 0)
    // ```
    // #### `args`
    // - None
    VB_ALLOW_SONG_DOUBLE_TIME_ON_FINAL_NIGHT,

    // #### `result`
    // ```c
    // EnBal_CheckIfMapUnlocked(this, play)
    // ```
    // #### `args`
    // - `*EnBal` (unused)
    VB_ALREADY_HAVE_TINGLE_MAP,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*f32` (speed)
    VB_APPLY_AIR_CONTROL,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_APPLY_BOMB_BILLBOARDING,

    // #### `result`
    // ```c
    // cylinderOc != NULL
    // ```
    // #### `args`
    // - `*Actor` (cylinderOc)
    VB_APPLY_BONK_TO_ACTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s32` (sDistortionRequest.type)
    VB_APPLY_SCREEN_DISTORTION,

    // #### `result`
    // ```c
    // !play->interfaceCtx.perfectLettersOn
    // ```
    // #### `args`
    // - `*EnSyatekiMan` (unused)
    // - `*s32` (sBonusTimer)
    VB_ARCHERY_ADD_BONUS_POINTS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_BALLAD_PLAYED_FORM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGinkoMan`
    VB_BANKER_GIVE_REWARD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_BARREL_OR_CRATE_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (ObjComb)
    VB_BEEHIVE_ALREADY_BROKEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (ObjComb)
    VB_BEEHIVE_SPAWN_ACTOR,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_BE_CLIMBABLE_SURFACE,

    // #### `result`
    // ```c
    // CHECK_WEEKEVENTREG(WEEKEVENTREG_73_80) && !CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)
    // ```
    // #### `args`
    // - `*EnBomBowlMan` (unused)
    VB_BE_ELIGIBLE_FOR_BOMBERS_NOTEBOOK,

    // #### `result`
    // ```c
    // SurfaceType_GetData(colCtx, poly, bgId, 1) >> 17 & 1
    // ```
    // #### `args`
    // - `*CollisionPoly` (unused)
    // - `s32` (bgId, unused)
    VB_BE_HOOKSHOT_SURFACE,

    // #### `result`
    // #### In `EnDoor_Idle`, for `*EnDoor`:
    // ```c
    // fabsf(playerPosRelToDoor.z) < 50.0f
    // ```
    // #### `args`
    // - `*f32`
    // #### In `func_808A0E28`, for `*DoorShutter`:
    // ```c
    // fabsf(temp_f0) < 50.0f
    // ```
    // #### `args`
    // - `*f32`
    VB_BE_NEAR_DOOR,

    // #### `result`
    // ##### In `func_80A25D28`, for `*ObjIceblock`:
    // ```c
    // this->unk_26E[sp30] >= 11
    // ```
    // ##### In `func_809A3A74`, for `*ObjPzlblock`:
    // ```c
    // this->unk_16E[sp20] >= 11
    // ```
    // #### `args`
    // - `*ObjIceblock` or `*ObjPzlblock` (unused)
    VB_BLOCK_BEGIN_MOVE,

    // #### `result`
    // ##### In `func_80B7F290`:
    // ```c
    // Math_StepToF(this->unk_164, this->unk_168, this->unk_16C)
    // ```
    // ##### In `func_80A25E50`:
    // ```c
    // Math_StepToF(this->unk_264, this->unk_268,
    // CLAMP_MAX((Math_SinS(fabsf(this->unk_268 - *this->unk_264) * 546.13336f) * 2.8f) + 1.2f, 3.5f))
    // ```
    // ##### In `func_809A3BC0`:
    // ```c
    // Math_StepToF(this->unk_164, this->unk_168, 2.3f)
    // ```
    // #### `args`
    // - `*f32` (pValue)
    // - `f64` (target)
    // - `f64` (step)
    // - `f64` (maxStep)
    VB_BLOCK_BE_FINISHED_PULLING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBomBowlMan`
    VB_BOM_BOWL_MAN_GIVE_ITEM,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*s32` (bool)
    // - `*EnIn`
    VB_BUY_GORMAN_MILK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_CAMERA_SET_FOCAL_ACTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjGrassCarry`
    VB_CARRY_GRASS_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ```c
    // !(atElem->atDmgInfo.dmgFlags & acElem->acDmgInfo.dmgFlags)
    // ```
    // #### `args`
    // - `*ColliderElement atElem`
    // - `*ColliderElement acElem`
    VB_CHECK_BUMPER_COLLISION,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_ROOM_KEY) == ITEM_ROOM_KEY
    // ```
    // #### `args`
    // - None
    VB_CHECK_FOR_ROOM_KEY,

    // #### `result`
    // ```c
    // CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_B)
    // ```
    // #### `args`
    // - `uint16_t sDpadItemButtons[4]`
    // - `uint16_t sPlayerItemButtons[4]`
    VB_CHECK_HELD_ITEM_BUTTON_PRESS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*f32` (speed)
    VB_CLAMP_ANIMATION_SPEED,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.inventory.items[SLOT_BOW] == ITEM_NONE
    // ```
    // #### `args`
    // - None
    VB_CLEAR_B_BUTTON_FOR_NO_BOW,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE
    // ```
    // #### `args`
    // - `*EnTest4` (unused)
    VB_CLOCK_TOWER_OPENING_CONSIDER_THIS_FIRST_CYCLE,

    // #### `result`
    // ```c
    // this->collider.base.ocFlags1 & OC1_HIT
    // ```
    // #### `args`
    // - `*EnGamelupy`
    VB_COLLECT_PLAYGROUND_RUPEE,

    // #### `result`
    // ```c
    // this->currentMask == PLAYER_MASK_BUNNY
    // ```
    // #### `args`
    // - `*Player`
    VB_CONSIDER_BUNNY_HOOD_EQUIPPED,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_GORON) == ITEM_MASK_GORON
    // ```
    // #### `args`
    // - None
    VB_CONSIDER_DARMANI_HEALED,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_ZORA) == ITEM_MASK_ZORA
    // ```
    // ##### Changed to `!=` in `func_80A44DE8`
    // #### `args`
    // - `bool` (false if `result` operator is `!=`, true otherwise)
    VB_CONSIDER_MIKAU_HEALED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_CONSUME_EPONA_CARROT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGinkoMan`
    VB_CONTINUE_BANKER_DIALOGUE,

    // #### `result`
    // ```c
    // (bean->unk_1E4 == 2) || (bean->unk_1E4 == 1)
    // ```
    // #### `args`
    // - `ObjBean*`
    VB_COUNT_BURROWED_BUGS,

    // #### `result`
    // ```c
    // gHorsePlayedEponasSong
    // ```
    // #### `args`
    // - `*EnCow`
    VB_COW_CONSIDER_EPONAS_SONG_PLAYED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjKibako`
    VB_CRATE_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s32`
    // - `*DamageTable`
    // - `*u32`
    // - `*Actor`
    VB_DAMAGE_EFFECT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s32`
    // - `*DamageTable`
    // - `*f32`
    // - `f32 sDamageMultipliers[16]`
    VB_DAMAGE_MULTIPLIER,

    // #### `result`
    // ```c
    // this->animIndex == EN_DNO_ANIM_SHOCK_LOOP
    // ```
    // #### `args`
    // - `*EnDno` (this)
    // - `*u8` (changeAnim)
    VB_DEKU_BUTLER_CHANGE_SHOCK_ANIMATION,

    // #### `result`
    // ```c
    // gSaveContext.save.isNight
    // ```
    // #### `args`
    // - None
    VB_DEKU_GUARD_SHOW_SEARCH_BALLS,

    // #### `result`
    // ```c
    // this->remainingHopsCounter == 0
    // ```
    // #### `args`
    // - None
    VB_DEKU_LINK_SPIN_ON_LAST_HOP,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_DELETE_OWL_SAVE,

    // #### `result`
    // ```c
    // CHECK_WEEKEVENTREG(sIsFrogReturnedFlags[this->frogIndex])
    // ```
    // #### `args`
    // - `*EnMinifrog`
    VB_DESPAWN_FROG,

    // #### `result`
    // ```c
    // (play->sceneId != SCENE_MITURIN_BS) &&
    // (play->sceneId != SCENE_HAKUGIN_BS) &&
    // (play->sceneId != SCENE_SEA_BS) &&
    // (play->sceneId != SCENE_INISIE_BS) &&
    // (play->sceneId != SCENE_LAST_BS)
    // ```
    // #### `args`
    // - None
    VB_DISABLE_FD_MASK,

    // #### `result`
    // ```c
    // GET_CUR_FORM_BTN_ITEM(i) != ITEM_MASK_ZORA
    // ```
    // ##### Alt: `DPAD_GET_CUR_FORM_BTN_ITEM`
    // #### `args`
    // - `s32` (item)
    VB_DISABLE_ITEM_UNDERWATER,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_DISABLE_LETTERBOX,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.playerData.rupees >= CUR_CAPACITY(UPG_WALLET)
    // ```
    // #### `args`
    // - None
    VB_DISCARD_EXCESS_RUPEES,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_DISPLAY_SONG_OF_DOUBLE_TIME_PROMPT,

    // #### `result`
    // ```c
    // this->actor.speed > 7.5f
    // ```
    // #### `args`
    // - None
    VB_DOGGY_RACE_SET_MAX_SPEED,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.playerData.healthCapacity < (DOORSHUTTER_GET_1F(&this->slidingDoor.dyna.actor) * 0x10)
    // ```
    // #### `args`
    // - `*DoorShutter` (unused)
    VB_DOOR_HEALTH_CHECK_FAIL,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*DmHina`
    VB_DRAW_BOSS_REMAINS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s16` (button)
    // - `s16` (alpha)
    // - `bool` (isDpad)
    VB_DRAW_HUD_AMMO_COUNT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_DRAW_LOCK_ON_ARROW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    // - `*u8` (type)
    VB_DRAW_DAMAGE_EFFECT,

    // #### `result`
    // ```c
    // this->isOwlSave[fileIndex + FILE_NUM_OWL_SAVE_OFFSET]
    // ```
    // #### `args`
    // - `s16` (file index)
    VB_DRAW_FILE_SELECT_EXTRA_INFO_DETAILS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s16` (file index)
    VB_DRAW_FILE_SELECT_OWL_SAVE_ICON,

    // #### `result`
    // ```c
    // this->isOwlSave[i + FILE_NUM_OWL_SAVE_OFFSET]
    // ```
    // #### `args`
    // - `s16` (file index)
    VB_DRAW_FILE_SELECT_SMALL_EXTRA_INFO_BOX,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ItemId`
    // - `s32` (slot)
    // - `s32` (isDpad)
    // - `s32` (pageIndex)
    VB_DRAW_ITEM_EQUIPPED_OUTLINE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `GetItemDrawId`
    // - `DmChar05*`
    VB_DRAW_ITEM_FROM_DMCHAR05,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (unused)
    VB_DRAW_ITEM_FROM_SOB1,

    // #### `result`
    // ```c
    // (play->sceneId != SCENE_LOST_WOODS) || (gSaveContext.sceneLayer != 1)
    // ```
    // #### `args`
    // - `*DmStk` (unused)
    VB_DRAW_OCARINA_IN_STK_HAND,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_DRAW_RUPEE_COUNTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_DRAW_RUPEE_ICON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_DRAW_OCARINA_STAFF,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSlime`
    VB_DRAW_SLIME_BODY_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSlime`
    VB_DRAW_SLIME_RANDO_ITEM,

    // #### `result`
    // #### In `Item_DropCollectible`:
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Vec3f` spawnPos
    // - `u32` params
    // #### In `Item_DropCollectibleRandom`:
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Vec3f` spawnPos
    // - `u16` params
    VB_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // (CURRENT_DAY == 3) && (CutsceneManager_GetCurrentCsId() == CS_ID_NONE)
    // ```
    // #### `args`
    // - None
    VB_EARTHQUAKE_ON_DAY_3,

    // #### `result`
    // ```c
    // (play->sceneId == SCENE_F40) || (play->sceneId == SCENE_F41) ||
    // (play->sceneId == SCENE_IKANAMAE) || (play->sceneId == SCENE_CASTLE) ||
    // (play->sceneId == SCENE_IKNINSIDE) || (play->sceneId == SCENE_IKANA) ||
    // (play->sceneId == SCENE_INISIE_N) || (play->sceneId == SCENE_INISIE_R) ||
    // (play->sceneId == SCENE_INISIE_BS) || (play->sceneId == SCENE_RANDOM) ||
    // (play->sceneId == SCENE_REDEAD) || (play->sceneId == SCENE_TOUGITES)
    // ```
    // #### `args`
    // - None
    VB_ELEGY_CHECK_SCENE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnTorch2`
    // - `*u16` (target alpha)
    VB_ELEGY_STATUE_FADE_IN_OUT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `s16` (object ID)
    VB_ENABLE_OBJECT_DEPENDENCY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u8`
    // - `*Actor`
    // - `*MsgScript`
    // - `*MsgScriptCallback`
    VB_EXEC_MSG_EVENT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_EXIT_FIRST_PERSON_MODE_FROM_BUTTON,

    // #### `result`
    // ```c
    // gSaveContext.minigameHiddenScore >= 10
    // ```
    // #### `args`
    // - None
    VB_FAIL_BOAT_ARCHERY,

    // #### `result`
    // ```c
    // gSaveContext.minigameScore < 20
    // ```
    // #### `args`
    // - None
    VB_FAIL_FISHERMAN_JUMPING_GAME,

    // #### `result`
    // ```c
    // (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) && (play->envCtx.sceneTimeSpeed != 0)
    // ```
    // #### `args`
    // - None
    VB_FASTER_FIRST_CYCLE,

    // #### `result`
    // ```c
    // (i >= EQUIP_SLOT_A) && (this->transformation == PLAYER_FORM_FIERCE_DEITY) &&
    // (this->heldItemAction != PLAYER_IA_SWORD_TWO_HANDED)
    // ```
    // #### `args`
    // - None
    VB_FD_ALWAYS_WIELD_SWORD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*FileSelectState`
    VB_FILE_SELECT_ROTATE_TO_NAME_ENTRY,

    // #### `result`
    // ```c
    // this->myPotIndex == 4
    // ```
    // #### `args`
    // - None
    VB_FIND_ROCK_SIRLOIN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_FIRST_PERSON_CAMERA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnFish2`
    VB_FISH2_SPAWN_HEART_PIECE,

    // #### `result`
    // ```c
    // (freezeFlashTimer > 0) && ((freezeFlashTimer % 2) != 0)
    // ```
    // #### `args`
    // - None
    VB_FLASH_SCREEN_FOR_ENEMY_KILL,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_EN_FSN_HAS_ITEMS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGs`
    // - `*PlayState`
    VB_EN_GS_BEFORE_GOSSIP_GROTTO_SEQUENCE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGs`
    // - `*PlayState`
    VB_EN_GS_FINISH_OCARINA_ON_RESET,

    // #### `result`
    // ```c
    // itemAction == GET_IA_FROM_MASK(this->currentMask)
    // ```
    // #### `args`
    // - `PlayerItemAction`
    VB_GET_ITEM_ACTION_FROM_MASK,

    // #### `result`
    // #### In `Player_GetItemOnButton`:
    // ```c
    // item
    // ```
    // #### `args`
    // - `EquipSlot`
    // - `*ItemId`
    VB_GET_ITEM_ON_BUTTON,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `PlayerItemAction` (requested item)
    // - `PlayerItemAction` (presented item)
    // - `**EnTalkGibudRequestedItem`
    VB_GIBDO_TRADE_SEQUENCE_ACCEPT_RED_POTION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnTalkGibud`
    // - `int` (bool, should show end trade message?)
    VB_GIBDO_TRADE_SEQUENCE_DO_TRADE,

    // #### `result`
    // ```c
    // AMMO(requestedItem->item) >= requestedItem->amount
    // ```
    // #### `args`
    // - `ItemId` (requested item)
    VB_GIBDO_TRADE_SEQUENCE_SUFFICIENT_QUANTITY_PRESENTED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnTalkGibudRequestedItem`
    VB_GIBDO_TRADE_SEQUENCE_TAKE_MORE_THAN_ONE_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGeg`
    VB_GIVE_DON_GERO_MASK,

    // unused
    VB_GIVE_HONEY_DARLING_REWARD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBox` (unused)
    VB_GIVE_ITEM_FROM_CHEST,

    // #### `result`
    // ```c
    // (this->actor.xzDistToPlayer < 150.0f) &&
    // ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 25000
    // ```
    // #### `args`
    // - `*EnCow`
    VB_GIVE_ITEM_FROM_COW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `ItemId`
    VB_GIVE_ITEM_FROM_DMCHAR05,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnElforg`
    VB_GIVE_ITEM_FROM_ELFORG,

    // #### `result`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FROM_GK_LULLABY,

    // #### `result`
    // ```c
    // !Flags_GetSwitch(play, GREAT_FAIRY_GET_SWITCHFLAG(&this->actor))
    // ```
    // #### `args`
    // - `*BgDyYoseizo`
    VB_GIVE_ITEM_FROM_GREAT_FAIRY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnItem00` (unused)
    VB_GIVE_ITEM_FROM_ITEM00,

    // #### `result`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FROM_JG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FROM_KNIGHT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FROM_MNK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjMoonStone`
    VB_GIVE_ITEM_FROM_MOONS_TEAR,

    // #### `result`
    // ```c
    // (getItemId != GI_NONE) || (player->getItemDirection < absYawDiff)
    // ```
    // #### `args`
    // - `*GetItemId`
    // - `*Actor`
    VB_GIVE_ITEM_FROM_OFFER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOsn` (unused)
    VB_GIVE_ITEM_FROM_OSN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMa4` (unused)
    VB_GIVE_ITEM_FROM_ROMANI,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSi`
    VB_GIVE_ITEM_FROM_SI,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnElfgrp`
    VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnFsn`
    VB_GIVE_KEATON_MASK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnFsn`
    VB_GIVE_LETTER_TO_MAMA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKujiya`
    VB_GIVE_LOTTERY_WINNINGS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMk` (unused)
    VB_GIVE_NEW_WAVE_BOSSA_NOVA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMaYto`
    VB_GIVE_ROMANI_MASK,

    // #### `result`
    // ```c
    // (this->iceCollider.base.acFlags & AC_HIT) && (this->iceCollider.elem.acHitElem->atDmgInfo.dmgFlags ==
    // DMG_FIRE_ARROW)
    // ```
    // #### `args`
    // - None
    VB_GOHT_UNFREEZE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `ObjDora*`
    VB_GONG_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // phi_f0 > 0.0f
    // ```
    // #### `args`
    // - `*f32` (phi_f0: rubberbanded speed)
    // - `*f32` (phi_f2: player's speed or 20, whichever is greater)
    VB_GORON_RACE_RUBBERBANDING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GORON_ROLL_CONSUME_MAGIC,

    // #### `result`
    // ```c
    // (this->stateFlags3 & PLAYER_STATE3_80000) &&
    // (!CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_A) ||
    //  (gSaveContext.save.saveInfo.playerData.magic == 0) ||
    //  ((this->av1.actionVar1 == 4) && (this->unk_B08 < 12.0f)))
    // ```
    // #### `args`
    // - None
    VB_GORON_ROLL_DISABLE_SPIKE_MODE,

    // #### `result`
    // ```c
    // (gSaveContext.magicState == MAGIC_STATE_IDLE) &&
    // (gSaveContext.save.saveInfo.playerData.magic >= 2) &&
    // (this->av2.actionVar2 >= 0x36B0)
    // ```
    // #### `args`
    // - None
    VB_GORON_ROLL_INCREASE_SPIKE_LEVEL,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `ActorId`
    // - `*Actor` (type matches ActorId)
    VB_GRASS_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GRANNY_STORY_INCREMENT_DAY,

    // #### `result`
    // ```c
    // gSaveContext.isMagicRequested
    // ```
    // #### `args`
    // - None
    VB_GRANT_MAGIC_UPON_REQUEST,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*BgDblueMovebg`
    VB_GREAT_BAY_GEAR_CLAMP_PUSH_SPEED,

    // #### `result`
    // ```c
    // (this->timer < 50) && (GREAT_FAIRY_GET_TYPE(&this->actor) == GREAT_FAIRY_TYPE_COURAGE)
    // ```
    // #### `args`
    // - `*BgDyYoseizo` (unused)
    VB_GREAT_FAIRY_GIVE_DOUBLE_DEFENSE_HEARTS,

    // #### `result`
    // ```c
    // Player_GetMask(play) == PLAYER_MASK_TRUTH
    // ```
    // #### `args`
    // - `*EnGs` (unused)
    VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGs` (unused)
    VB_GS_CONTINUE_TEXTBOX,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnRuppecrow`
    VB_GUAY_DROP_RUPEE,

    // #### `result`
    // ```c
    // Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED
    // ```
    // #### `args`
    // - None
    VB_HAVE_ALL_SKULLTULA_TOKENS,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST
    // ```
    // #### `args`
    // - None
    VB_HAVE_BLAST_MASK,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_GARO) == ITEM_MASK_GARO
    // ```
    // #### `args`
    // - None
    VB_HAVE_GARO_MASK,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_GIBDO) == ITEM_MASK_GIBDO
    // ```
    // #### `args`
    // - None
    VB_HAVE_HEALED_PAMELAS_FATHER,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_KAMARO) == ITEM_MASK_KAMARO
    // ```
    // #### `args`
    // - None
    VB_HAVE_KAMAROS_MASK,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.playerData.isMagicAcquired
    // ```
    // #### `args`
    // - None
    VB_HAVE_MAGIC_FOR_TINGLE,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_ROMANI) == ITEM_MASK_ROMANI
    // ```
    // #### `args`
    // - None
    VB_HAVE_ROMANI_MASK,

    // #### `result`
    // ```c
    // (!DynaPolyActor_IsPlayerAbove((DynaPolyActor*)this->actor.child) &&
    //  (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) ||
    // (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] <= SECONDS_TO_TIMER(0)) ||
    // (this->unk_548 == this->unk_54C)
    // ```
    // #### `args`
    // - `*EnFu`
    VB_HONEY_AND_DARLING_MINIGAME_FINISH,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u8` (btn)
    VB_INTERFACE_LOAD_DPAD_ITEM_ICON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u8` (btn)
    VB_INTERFACE_LOAD_ITEM_ICON,

    // #### `result`
    // ```c
    // !gPlayerFormItemRestrictions[GET_PLAYER_FORM][itemId]
    // ```
    // #### `args`
    // - `*ItemId`
    VB_ITEM_BE_RESTRICTED,

    // #### `result`
    // ```c
    // bubble != NULL
    // ```
    // #### `args`
    // - `*Player`
    VB_DEKU_COMMON_HEAD_OVERRIDE_HELD_ACTOR,

    // #### `result`
    // ```c
    // player->heldActor != NULL
    // ```
    // #### `args`
    // - `*Actor` (player->heldActor)
    VB_DEKU_COMMON_UPPER_LIMB_OVERRIDE_HELD_ACTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*u8` (`*ItemId`)
    VB_ITEM_GIVE_SWORD_SET_FORM_EQUIP,

    // #### `result`
    // ```c
    // CHECK_WEEKEVENTREG(WEEKEVENTREG_24_40) ||
    // CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ||
    // CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)
    // ```
    // #### `args`
    // - `*EnJg` (unused)
    VB_JG_THINK_YOU_KNOW_LULLABY,

    // #### `result`
    // ```c
    // func_80968E38(0) >= 20
    // ```
    // #### `args`
    // - None
    VB_JS_CONSIDER_ELIGIBLE_FOR_DEITY,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*s32`
    // - `*bool`
    VB_JS_OVERRIDE_MASK_CHECK,

    // #### `result`
    // ```c
    // gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] == SECONDS_TO_TIMER(0)
    // ```
    // #### `args`
    // - None
    VB_JUMPING_GAME_END,

    // #### `result`
    // ```c
    // this->unk_2FC > 100
    // ```
    // #### `args`
    // - `s32` (torch burn time)
    VB_JUMPING_GAME_TORCH_RUN_OUT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u16` (item under cursor)
    VB_KALEIDO_DISPLAY_ITEM_TEXT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `PauseContext*`
    // - `GraphicsContext*`
    // - `s16` (item)
    // - `u16` (ammoIndex)
    VB_KALEIDO_DRAW_AMMO_COUNT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `ItemId*` (draw item, mutable)
    VB_KALEIDO_DRAW_EQUIP_ANIM_ICON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u16` (cursor slot)
    // - `u16` (cursor item)
    VB_KALEIDO_EQUIP_ITEM_TO_BUTTON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u16` (button - BTN_DLEFT or BTN_DRIGHT)
    VB_KALEIDO_SWITCH_PAGE_WITH_DPAD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_KALEIDO_UNPAUSE_CLOSE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKusa2`
    // - `s32` (index within the patch)
    VB_KEATON_GRASS_ATTACH_CHILD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKusa2`
    VB_KEATON_GRASS_DRAW,

    // #### `result`
    // ```c
    // CHECK_WEEKEVENTREG(WEEKEVENTREG_08_80)
    // ```
    // #### `args`
    // - `*EnElforg` (unused)
    VB_KILL_CLOCK_TOWN_STRAY_FAIRY,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.inventory.items[ITEM_LENS_OF_TRUTH] == ITEM_LENS_OF_TRUTH
    // ```
    // #### `args`
    // - None
    VB_KILL_GORON_VILLAGE_OWL,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_KOUME_TAKE_DAMAGE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKusa`
    VB_KUSA_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ```c
    // play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].length >= 3
    // ```
    // #### `args`
    // - None
    VB_LIMIT_EXPLOSIVES,

    // #### `result`
    // ```c
    // WaterBox_GetSurface1(play, &play->colCtx, sp4C.x, sp4C.z, &sp44, &waterBox) && ((sp44 - sp48) > 50.0f)
    // ```
    // #### `args`
    // - None
    VB_LINK_DIVE_OVER_WATER,

    // unused
    // #### `args`
    // - `*u16` (sfxId)
    VB_LINK_VOICE_PITCH_MULTIPLIER,

    // #### `result`
    // ```c
    // task != NULL
    // ```
    // #### `args`
    // - `*AnimTask task`
    // - `*PlayerAnimationHeader animation`
    // - `s32 frame`
    // - `s32 limbCount`
    // - `*Vec3s frameTable`
    VB_LOAD_PLAYER_ANIMATION_FRAME,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_LOWER_RAZOR_SWORD_DURABILITY,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_MASK_KAFEIS_MASK) != ITEM_MASK_KAFEIS_MASK
    // ```
    // #### `args`
    // - `*EnAl` (unused)
    VB_MADAME_AROMA_ASK_FOR_HELP,

    // #### `result`
    // ```c
    // this->transformation == PLAYER_FORM_HUMAN
    // ```
    // #### `args`
    // - None
    VB_MAGIC_SPIN_ATTACK_CHECK_FORM,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA) && CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT) &&
    // CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG) && CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)
    // ```
    // #### `args`
    // - None
    VB_MEET_MOON_REQUIREMENTS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_MINIMAP_TOGGLE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMnk`
    VB_MONKEY_WAIT_TO_TALK_AFTER_APPROACH,

    // #### `result`
    // ```c
    // CHECK_BTN_ALL(controller->press.button, BTN_A) || ...
    // ```
    // #### `args`
    // - None
    VB_MSG_ADVANCE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_MSG_CAPTURE_MSGMODE_TEXT_DONE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_MSG_CAPTURE_MSGMODE_TEXT_CLOSING_OCARINA_ACTION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_MSG_LOAD_RUPEES_TEXT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_MSG_PLAY_INPUT_COUNT_SOUND,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    // - `s16` (item ID)
    VB_MSG_SCRIPT_DEL_ITEM,

    // #### `result`
    // ```c
    // this->currentMask == PLAYER_MASK_GIANT
    // ```
    // #### `args`
    // - `*s32` (damage)
    VB_MULTIPLY_INFLICTED_DMG,

    // #### `result`
    // ```c
    // CHECK_WEEKEVENTREG(WEEKEVENTREG_79_08) && (this->picto.actor.xzDistToPlayer < this->songSummonDist) &&
    // ((BREG(1) != 0) || (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SCARECROW_SPAWN))
    // ```
    // #### `args`
    // - `*EnKakasi`
    VB_NEED_SCARECROW_SONG,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.playerData.rupees < price
    // ```
    // #### `args`
    // - `*EnBal` (unused)
    // - `*s32` (price)
    VB_NOT_AFFORD_TINGLE_MAP,

    // #### `result`
    // ```c
    // Inventory_GetSkullTokenCount(play->sceneId) < SPIDER_HOUSE_TOKENS_REQUIRED
    // ```
    // #### `args`
    // - `*EnSth` (unused)
    VB_NOT_HAVE_ALL_SKULLTULA_TOKENS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjGrass`
    // - `*ObjGrassElement`
    // - `*s32` (index of grass element in group)
    VB_OBJGRASS_OPA_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjGrass`
    // - `*ObjGrassElement`
    VB_OBJGRASS_XLU_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ```c
    // this->actors[i] != NULL
    // ```
    // #### `args`
    // - `*ObjMure2`
    // - `s32` (index of child to spawn)
    VB_OBJ_MURE2_SET_CHILD_ROOM,

    // #### `result`
    // ```c
    // !((this->unk164 >> i) & 1)
    // ```
    // #### `args`
    // - `*ObjMure3 this`
    // - `s32 i`
    VB_OBJ_MURE3_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // (player->transformation == PLAYER_FORM_ZORA) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
    // (play->msgCtx.lastPlayedSong == OCARINA_SONG_NEW_WAVE)
    // ```
    // #### `args`
    // - `*DmChar08` (unused)
    VB_OPEN_GREAT_BAY_FROM_SONG,

    // #### `result`
    // ```c
    // (player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
    // (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY)
    // ```
    // #### `args`
    // - `*EnDai` (unused)
    VB_OPEN_SNOWHEAD_FROM_SONG,

    // #### `result`
    // ```c
    // (player->transformation == PLAYER_FORM_DEKU) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
    // (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA)
    // ```
    // #### `args`
    // - `*DmChar01` (unused)
    VB_OPEN_WOODFALL_FROM_SONG,

    // #### `result`
    // ```c
    // (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) && !CHECK_QUEST_ITEM(QUEST_SONG_HEALING)
    // ```
    // #### `args`
    // - `*EnOsn` (unused)
    VB_OSN_CONSIDER_ELIGIBLE_FOR_SONG_OF_HEALING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOsn`
    VB_OSN_TEACH_SONG_OF_HEALING,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `**Gfx` (dList)
    VB_OVERRIDE_CHAR02_LIMB,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*OcarinaStaff` (the currently playing staff; state can be overridden to force a song)
    VB_OVERRIDE_OCARINA_STAFF_STATE,

    // #### `result`
    // ```c
    // player->stateFlags3 & PLAYER_STATE3_10000000
    // ```
    // #### `args`
    // - `*EnOwl`
    VB_OWL_DESPAWN_FEATHER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOwl`
    VB_OWL_SPAWN_FEATHER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `u8`
    VB_OWL_STATUE_ACTIVATE,

    // #### `result`
    // ```c
    // GET_OWL_STATUE_ACTIVATED(OBJ_WARPSTONE_GET_OWL_WARP_ID(&this->dyna.actor))
    // ```
    // #### `args`
    // - `u8`
    VB_OWL_STATUE_BE_ACTIVE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOwl`
    VB_OWL_TELL_ABOUT_SHRINE,

    // #### `result`
    // ```c
    // pauseCtx->state == PAUSE_STATE_OWL_WARP_SELECT
    // ```
    // #### `args`
    // - None
    VB_OWL_WARP_MENU_USE_LINEAR_CURSOR,

    // #### `result`
    // ```c
    // (HS_GET_BANK_RUPEES() >= 200) && (this->previousBankValue < 200) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_40)
    // ```
    // #### `args`
    // - `*EnGinkoMan`
    VB_PASS_FIRST_BANK_THRESHOLD,

    // #### `result`
    // ```c
    // (HS_GET_BANK_RUPEES() >= 1000) && (this->previousBankValue < 1000) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_80)
    // ```
    // #### `args`
    // - `*EnGinkoMan`
    VB_PASS_INTEREST_BANK_THRESHOLD,

    // #### `result`
    // ```c
    // HS_GET_BANK_RUPEES() >= 5000
    // ```
    // #### `args`
    // - `*EnGinkoMan` (unused)
    VB_PASS_SECOND_BANK_THRESHOLD,

    // #### `result`
    // ```c
    // (this->previousBankValue < 5000) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_60_01)
    // ```
    // #### `args`
    // - `*EnGinkoMan`
    VB_PASS_SECOND_BANK_THRESHOLD_ALT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PATCH_POWER_CROUCH_STAB,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PATCH_SIDEROLL,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Collider atCol`
    // - `*Collider acCol`
    // - `*ColliderElement atElem` (unused)
    // - `*ColliderElement acElem`
    VB_PERFORM_AC_COLLISION,

    // #### `result`
    // ```c
    // play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON
    // ```
    // #### `args`
    // - None
    VB_PICTO_ACTIVATE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PICTO_DISPLAY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*PreRender` (prerender)
    VB_PICTO_TAKE,

    // #### `result`
    // ```c
    // i < ARRAY_COUNT(D_8085D798)
    // ```
    // #### `args`
    // - `*Player`
    // - `*Actor` (interactRangeActor)
    // - `ItemId`
    // - `PlayerItemAction`
    VB_PLAY_BOTTLE_CATCH_TEXT,

    // !Play_InCsMode(play) && (play->roomCtx.curRoom.num == 2)
    // ```
    // #### `args`
    // - `*EnTest3` (Kafei)
    // - `*s32` (cutscene phase)
    VB_PLAY_COUPLES_MASK_CS,

    // #### `result`
    // ```c
    // Rand_Next() & 0x80
    // ```
    // #### `args`
    // - None
    VB_PLAY_CREMIA_HUG_CUTSCENE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_DEFEAT_CAPTAIN_SEQUENCE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_ENEMY_PROXIMITY_MUSIC,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_ENTRANCE_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_GORON_CHILD_CRY,

    // #### `result`
    // ```c
    // (this->getItemId == GI_HEART_CONTAINER) ||
    // ((this->getItemId == GI_HEART_PIECE) && EQ_MAX_QUEST_HEART_PIECE_COUNT)
    // ```
    // #### `args`
    // - `GetItemId`
    VB_PLAY_HEART_CONTAINER_GET_FANFARE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_LOW_HP_ALARM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*u8` (current Ocarina button input index)
    // - `*u8` (current Ocarina pitch)
    VB_PLAY_OCARINA_NOTE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*u16` (requested scene sequence ID)
    // - `*u16` (previous main BGM sequence ID)
    // - `*u16` (sequence ID)
    VB_PLAY_SCENE_SEQUENCE,

    // #### `result`
    // ```c
    // (giEntry->itemId != ITEM_NONE) && (giEntry->gid >= 0) && (Item_CheckObtainability(giEntry->itemId) == ITEM_NONE)
    // ```
    // #### `args`
    // - `*EnBox` (unused)
    VB_PLAY_SLOW_CHEST_CS,

    // #### `result`
    // ```c
    // this->getItemId == GI_OCARINA_OF_TIME
    // ```
    // #### `args`
    // - `*Player` (unused)
    VB_PLAY_SONG_OF_TIME_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_TATL_CALL_AUDIO,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_TRANSITION_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_PLAYER_CUTSCENE_ACTION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (unused)
    VB_POST_CHAR02_LIMB,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjTsubo`
    VB_POT_DRAW_BE_OVERRIDDEN,

    // #### `result`
    // ##### In `func_8092762C`:
    // ```c
    // !OBJ_TSUBO_P0010(&this->actor) && (OBJ_TSUBO_ZROT(&this->actor) != 2)
    // ```
    // ##### In `func_80927690`:
    // ```c
    // !this->unk_197 && (OBJ_TSUBO_ZROT(&this->actor) != 2)
    // ```
    // #### `args`
    // - `*ObjTsubo`
    VB_POT_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // ammo == 1
    // ```
    // #### `args`
    // - None
    VB_POWDER_KEG_AMMO_AT_CAPACITY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_POWDER_KEG_CAP_AMMO,

    // #### `result`
    // ```c
    // (AMMO(ITEM_POWDER_KEG) != 0) || (play->actorCtx.flags & ACTORCTX_FLAG_0)
    // ```
    // #### `args`
    // - None
    VB_POWDER_KEG_CHECK_HAS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_POWDER_KEG_SET_AMMO_ON_GIVE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_PREVENT_CLOCK_DISPLAY,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_PREVENT_MASK_TRANSFORMATION_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjOshihiki`
    VB_PUSH_BLOCK_SET_SPEED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjOshihiki` or `*BgIkanaBlock`
    VB_PUSH_BLOCK_SET_TIMER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*s16` (cutscene ID)
    VB_QUEUE_CUTSCENE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjMoonStone`
    VB_REVEAL_MOON_STONE_IN_CRATER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_RESET_PUTAWAY_TIMER,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_SONG_EPONA)
    // ```
    // #### `args`
    // - `bool` (unused)
    VB_ROMANI_CONSIDER_EPONA_SONG_GIVEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_ROTATE_GROTTO_ENTRANCE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSuttari`
    VB_SAKON_TAKE_DAMAGE,

    // #### `result`
    // ```c
    // OSTIME_TO_TIMER(osGetTime() - sramCtx->startWriteOsTime) >= SECONDS_TO_TIMER(2))
    // ```
    // #### `args`
    // - None
    VB_SAVE_DELAY,

    // #### `result`
    // ```c
    // gSaveContext.save.isOwlSave
    // ```
    // #### `args`
    // - None
    VB_SAVE_USE_OWL_SAVE_TIMING,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_SAVE_ON_B_BUTTON_IN_PAUSE_MENU,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SCARECROW_DANCE_SET_TIME,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.inventory.items[ITEM_OCARINA_OF_TIME] == ITEM_NONE
    // ```
    // #### `args`
    // - None
    VB_SCOPENUTS_CONSIDER_FIRST_CYCLE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*s32` (spawn time)
    // - `s32` (index)
    VB_SET_ALIEN_SPEED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SET_BLAST_MASK_COOLDOWN_TIMER,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_SET_BUTTON_ENV_COLOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SET_CAMERA_AT_EYE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SET_CAMERA_FOV,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*f32` (direction, which factors into speed)
    VB_SET_CLIMB_SPEED,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnElforg`
    VB_SET_DRAW_FOR_SAVED_STRAY_FAIRY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Player`
    // - `u32` (damage flags)
    VB_SET_PLAYER_CYLINDER_OC_FLAGS,

    // #### `result`
    // ```c
    // (gSaveContext.save.entrance == ENTRANCE(EAST_CLOCK_TOWN, 2)) &&
    // GameInteractor_Should(VB_BE_ELIGIBLE_FOR_BOMBERS_NOTEBOOK, ...)
    // ```
    // #### `args`
    // - `*EnBomBowlMan` (unused)
    VB_SETUP_EAST_CLOCK_TOWN_BOM_BOWL_MAN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SETUP_TRANSITION,

    // #### `result`
    // ```c
    // CHECK_BTN_ALL(sPlayerControlInput->cur.button, BTN_R)
    // ```
    // #### `args`
    // - None
    VB_SHIELD_FROM_BUTTON_HOLD,

    // #### `result`
    // ##### In `Player_ActionHandler_6`:
    // ```c
    // this->transformation != PLAYER_FORM_FIERCE_DEITY
    // ```
    // ##### In `Player_UpdateInterface`:
    // ```c
    // (this->transformation == PLAYER_FORM_HUMAN) || (this->transformation == PLAYER_FORM_ZORA)
    // ```
    // #### `args`
    // - None
    VB_SHOULD_PUTAWAY,

    // #### `result`
    // ```c
    // gSaveContext.showTitleCard
    // ```
    // #### `args`
    // - None
    VB_SHOW_TITLE_CARD,

    // #### `result`
    // ```c
    // !(this->unk_1C1 & 2) && (this->unk_172[sp2C] > 10) && (D_80A22A10 == 0) &&
    // !func_80A216D4(this, play, 2.0f, &sp20) && !Player_InCsMode(play)
    // ```
    // #### `args`
    // - `*ObjSkateblock` (unused)
    VB_SKATE_BLOCK_BEGIN_MOVE,

    // #### `result`
    // ```c
    // CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_SWORD_GILDED
    // ```
    // #### `args`
    // - None
    VB_SMITHY_CHECK_FOR_GILDED_SWORD,

    // #### `result`
    // ```c
    // CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_SWORD_RAZOR
    // ```
    // #### `args`
    // - None
    VB_SMITHY_CHECK_FOR_RAZOR_SWORD,

    // #### `result`
    // ```c
    // ((CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_KOKIRI) &&
    //  (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_RAZOR) &&
    //  (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_GILDED))
    // ```
    // #### `args`
    // - None
    VB_SMITHY_CHECK_FOR_SWORD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKgy`
    VB_SMITHY_START_UPGRADING_SWORD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjSnowball` or `*ObjSnowball2`
    VB_SNOWBALL_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // this->actor.home.rot.y == 5
    // ```
    // #### `args`
    // - `*ObjSnowball`
    // - `s16` (actor ID to spawn, unused)
    // - `ObjSnowballActionFunc`
    VB_SNOWBALL_SET_FLAG,

    // #### `result`
    // ##### In `AudioOcarina_CheckSongsWithoutMusicStaff`
    // ```c
    // (u32)sOcarinaAvailableSongFlags & (1 << songIndex)
    // ```
    // ##### In `Message_DrawMain`
    // ```c
    // (msgCtx->ocarinaStaff->state == OCARINA_SONG_SCARECROW_SPAWN) ||
    // (msgCtx->ocarinaStaff->state == OCARINA_SONG_INVERTED_TIME) ||
    // (msgCtx->ocarinaStaff->state == OCARINA_SONG_DOUBLE_TIME) ||
    // (msgCtx->ocarinaStaff->state == OCARINA_SONG_GORON_LULLABY_INTRO) ||
    // (msgCtx->ocarinaStaff->state != 0xFE &&
    //  msgCtx->ocarinaStaff->state != 0xFF &&
    //  CHECK_QUEST_ITEM(QUEST_SONG_SONATA + msgCtx->ocarinaStaff->state))
    // ```
    // #### `args`
    // - `u8` (song index)
    VB_SONG_AVAILABLE_TO_PLAY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*s32` (which boss remains to return, see `func_808B849C`)
    VB_SPAWN_BOSS_REMAINS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - '*f32' (incrStep)
    // - '*f32' (maxSpeed)
    // - '*f32' (speed)
    // - '*f32' (speedTarget)
    VB_SPEED_MODIFIER_SWIM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SPEED_MODIFIER_WALK,

    // #### `result`
    // ```c
    // this->actor.xzDistToPlayer < 350.0f
    // ```
    // #### `args`
    // - `*s16` (cutscene ID)
    // - `*Actor`
    VB_START_CUTSCENE,

    // #### `result`
    // ```c
    // this->actor.xzDistToPlayer < 350.0f
    // ```
    // #### `args`
    // - `*EnElfgrp`
    VB_START_GREAT_FAIRY_CUTSCENE,

    // #### `result`
    // ```c
    // !(this->stateFlags1 & PLAYER_STATE1_8000000) &&
    // (Player_GetMeleeWeaponHeld(this) != PLAYER_MELEEWEAPON_NONE) &&
    // Player_CanUpdateItems(this) &&
    // (this->transformation != PLAYER_FORM_GORON)
    // ```
    // #### `args`
    // - None
    VB_START_JUMPSLASH,

    // #### `result`
    // ```c
    // gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE
    // ```
    // #### `args`
    // - None
    VB_STK_HAVE_OCARINA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_STONE_HEISHI_SET_ACTION,

    // #### `result`
    // ```c
    // !CutsceneManager_IsNext(CS_ID_GLOBAL_TALK)
    // ```
    // #### `args`
    // - None
    VB_TATL_CONVERSATION_AVAILABLE,

    // #### `result`
    // ```c
    // (player->tatlActor != NULL) && ((func_8092E1FC(this)))
    // ```
    // #### `args`
    // - `*ElfMsg` (unused)
    VB_TATL_INTERRUPT_MSG,

    // #### `result`
    // ```c
    // (((((player->tatlActor != NULL) &&
    //     (fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < (100.0f * this->actor.scale.x))) &&
    //    (this->actor.world.pos.y <= player->actor.world.pos.y))
    //   && ((player->actor.world.pos.y - this->actor.world.pos.y) < (100.0f * this->actor.scale.y))) &&
    //  (fabsf(player->actor.world.pos.z - this->actor.world.pos.z) < (100.0f * this->actor.scale.z)))
    // ```
    // #### `args`
    // - `*ElfMsg3`
    VB_TATL_INTERRUPT_MSG3,

    // #### `result`
    // ```c
    // (player->tatlActor != NULL) && func_80AFD5E0(this)
    // ```
    // #### `args`
    // - `*ElfMsg4`
    VB_TATL_INTERRUPT_MSG4,

    // #### `result`
    // ```c
    // ((this->actor.xzDistToPlayer < (100.0f * this->actor.scale.x)) &&
    //  ((this->actor.playerHeightRel >= 0.0f) && (this->actor.playerHeightRel < (100.0f * this->actor.scale.y))))
    // ```
    // #### `args`
    // - `*ElfMsg6`
    VB_TATL_INTERRUPT_MSG6,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_OCARINA_OF_TIME) != ITEM_OCARINA_OF_TIME
    // ```
    // #### `args`
    // - None
    VB_TERMINA_FIELD_BE_EMPTY,

    // #### `result`
    // ```c
    // Rand_ZeroOne() < 0.5f
    // ```
    // #### `args`
    // - None
    VB_THIEF_BIRD_STEAL,

    // #### `result`
    // ```c
    // TIME_UNTIL_MOON_CRASH
    // ```
    // #### `args`
    // - `*u32` (time variable)
    VB_TIME_UNTIL_MOON_CRASH_CALCULATION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBal`
    VB_TINGLE_GIVE_MAP_UNLOCK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBjt` (unused)
    VB_TOILET_HAND_TAKE_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnToto`
    VB_TOTO_START_SOUND_CHECK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMThunder`
    VB_TRANSFORM_THUNDER_MATRIX,

    // #### `result`
    // #### In `EnSnowwd_Idle`:
    // ```c
    // !SNOWWD_DROPPED_COLLECTIBLE(&this->actor)
    // ```
    // #### `args`
    // - `*EnSnowwd`
    // #### In `ObjYasi_Update`:
    // ```c
    // CAN_DROP_NUT(thisx)
    // ```
    // #### `args`
    // - `*ObjYasi`
    VB_TREE_DROP_COLLECTIBLE,

    // #### `result`
    // ```c
    // btn <= EQUIP_SLOT_NONE
    // ```
    // #### `args`
    // - `*Player`
    // - `*s32` (button)
    VB_UNEQUIP_MASK_NOT_ON_BUTTON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `Player*`
    VB_USE_BOTTLE_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Camera`
    VB_USE_CUSTOM_CAMERA,

    // #### `result`
    // ```c
    // (this->modelAnimType != PLAYER_ANIMTYPE_3) && (this->heldItemAction != PLAYER_IA_DEKU_STICK) &&
    // (play->bButtonAmmoPlusOne == 0)
    // ```
    // #### `args`
    // - `*Player`
    VB_USE_HELD_ITEM_AFTER_CHANGE,

    // #### `result`
    // ```c
    // Player_ItemToItemAction(this, item) == PLAYER_IA_MASK_ZORA
    // ```
    // #### `args`
    // - `*PlayerItemAction`
    VB_USE_ITEM_CONSIDER_ITEM_ACTION,

    // #### `result`
    // ```c
    // this->transformation == PLAYER_FORM_HUMAN
    // ```
    // #### `args`
    // - `*PlayerItemAction`
    VB_USE_ITEM_CONSIDER_LINK_HUMAN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*PlayerMask`
    VB_USE_ITEM_EQUIP_MASK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    // - `*Vec3f` (body part positions)
    // - `int` (body part count)
    VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS,

    // #### `result`
    // ```c
    // this->poppedBalloonCounter == 10
    // ```
    // #### `args`
    // - `*EnMa4`
    VB_WIN_ROMANI_PRACTICE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*f32` (speed)
    VB_ZTARGET_SPEED_CHECK,
} GIVanillaBehavior;

#endif
