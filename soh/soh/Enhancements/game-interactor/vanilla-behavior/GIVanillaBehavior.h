#pragma once

#ifndef GI_VANILLA_BEHAVIOR_H
#define GI_VANILLA_BEHAVIOR_H

typedef enum {
    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKz`
    VB_ADULT_KING_ZORA_ITEM_GIVE,

    // #### `result`
    // ```c
    // requiredAge == 2
    // ```
    // #### `args`
    // - `int32_t` (entrance index) (promoted from `uint16_t` by va_arg)
    VB_ALLOW_ENTRANCE_CS_FOR_EITHER_AGE,

    // #### `result`
    // ```c
    // sBgPoEventPuzzleState == 0xF
    // ```
    // #### `args`
    // - None
    VB_AMY_SOLVE,

    // #### `result`
    // ```c
    // this->actor.textId == 0x401A
    // ```
    // #### `args`
    // - `*EnKz`
    VB_BE_ABLE_TO_EXCHANGE_RUTOS_LETTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_BE_ABLE_TO_OPEN_DOORS,

    // #### `result`
    // ```c
    // (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) || BREG(2)
    // ```
    // #### `args`
    // - None
    VB_BE_ABLE_TO_PLAY_BOMBCHU_BOWLING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_BE_ABLE_TO_SAVE,

    // #### `result`
    // ```c
    // this->currentReward == 3
    // ```
    // #### `args`
    // - `*EnTk`
    VB_BE_DAMPE_GRAVEDIGGING_GRAND_PRIZE,

    // #### `result`
    // ```c
    // (this->getItemId == GI_QUIVER_40) || (this->getItemId == GI_QUIVER_50)
    // ```
    // #### `args`
    // - `*EnSyatekiMan`
    VB_BE_ELIGIBLE_FOR_ADULT_SHOOTING_GAME_REWARD,

    // #### `result`
    // ```c
    // CUR_CAPACITY(UPG_BOMB_BAG) >= 20
    // ```
    // #### `args`
    // - `*EnGo2`
    VB_BE_ELIGIBLE_FOR_CHILD_ROLLING_GORON_REWARD,

    // #### `result`
    // ```c
    // CUR_UPG_VALUE(UPG_STRENGTH) <= 0
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD,

    // #### `result`
    // ```c
    // !CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE,

    // #### `result`
    // See `isEligible` logic in
    // ```c
    // BgDyYoseizo_CheckMagicAcquired()
    // ```
    // #### `args`
    // - `*BgDyYoseizo`
    VB_BE_ELIGIBLE_FOR_GREAT_FAIRY_REWARD,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
    // LINK_IS_ADULT &&
    // !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) &&
    // (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TEMPLE_OF_TIME)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS,

    // #### `result`
    // ```c
    // gSaveContext.rupees >= sPrices[BEANS_BOUGHT]
    // ```
    // #### `args`
    // - `*EnMs`
    VB_BE_ELIGIBLE_FOR_MAGIC_BEANS_PURCHASE,

    // #### `result`
    // ```c
    // (gSaveContext.entranceIndex == ENTR_KAKARIKO_VILLAGE_FRONT_GATE) &&
    // LINK_IS_ADULT &&
    // Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP) &&
    // Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP) &&
    // Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP) &&
    // !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW,

    // #### `result`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
    // Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) &&
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
    // (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) &&
    // CheckPlayerPosition(player, play)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_RAINBOW_BRIDGE,

    // #### `result`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_SONG_SARIA)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_SARIAS_SONG,

    // #### `result`
    // ```c
    // CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) &&
    // !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)
    // ```
    // #### `args`
    // - None
    VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER,

    // #### `result`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_OPENED_THE_DOOR_OF_TIME)
    // ```
    // #### `args`
    // - `*EnOkarinaTag`
    VB_BE_ELIGIBLE_TO_OPEN_DOT,

    // #### `result`
    // ```c
    // this->validDigHere == 1
    // ```
    // #### `args`
    // - `*EnTk`
    VB_BE_VALID_GRAVEDIGGING_SPOT,

    // #### `result`
    // ```c
    // this->collider.base.acFlags & 2 || blueFireArrowHit
    // ```
    // #### `args`
    // - None
    VB_BG_BREAKWALL_BREAK,

    // #### `result`
    // ```c
    // gSaveContext.bgsFlag
    // ```
    // #### `args`
    // - None
    VB_BIGGORON_CONSIDER_SWORD_COLLECTED,

    // #### `result`
    // ```c
    // Environment_GetBgsDayCount() >= 3
    // ```
    // #### `args`
    // - None
    VB_BIGGORON_CONSIDER_SWORD_FORGED,

    // #### `result`
    // ```c
    // gSaveContext.bgsFlag
    // ```
    // #### `args`
    // - None
    VB_BIGGORON_CONSIDER_TRADE_COMPLETE,

    // #### `result`
    // Actor is ACTOR_EN_ELF, ACTOR_EN_FISH, ACTOR_EN_ICE_HONO, or ACTOR_EN_INSECT
    // ```c
    // i < ARRAY_COUNT(sBottleCatchInfo)
    // ```
    // #### `args`
    // - `*Actor` (interactRangeActor)
    VB_BOTTLE_ACTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnPoField`
    VB_BOTTLE_BIG_POE,

    // #### `result`
    // ```c
    // ((this->actor.params == DNS_TYPE_HEART_PIECE) && (Flags_GetItemGetInf(ITEMGETINF_DEKU_SCRUB_HEART_PIECE))) ||
    // ((this->actor.params == DNS_TYPE_DEKU_STICK_UPGRADE) && (Flags_GetInfTable(INFTABLE_BOUGHT_STICK_UPGRADE))) ||
    // ((this->actor.params == DNS_TYPE_DEKU_NUT_UPGRADE) && (Flags_GetInfTable(INFTABLE_BOUGHT_NUT_UPGRADE)))
    // ```
    // #### `args`
    // - `*EnShopnuts`
    VB_BUSINESS_SCRUB_DESPAWN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `int32_t` (item)
    VB_CHANGE_HELD_ITEM_AND_USE_ITEM,

    // #### `result`
    // ```c
    // gSaveContext.rupees < 200
    // ```
    // #### `args`
    // - `*EnJs`
    VB_CHECK_RANDO_PRICE_OF_CARPET_SALESMAN,

    // #### `result`
    // ```c
    // gSaveContext.rupees < 200
    // ```
    // #### `args`
    // - `*EnGm`
    VB_CHECK_RANDO_PRICE_OF_MEDIGORON,

    // #### `result`
    // ```c
    // CHECK_BTN_ALL(input->press.button, BTN_START)
    // ```
    // #### `args`
    // - None
    VB_CLOSE_PAUSE_MENU,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDoor`
    VB_CONSUME_SMALL_KEY,

    // #### `result`
    // ```c
    // itemDropped >= 0 && itemDropped < 0x1A
    // ```
    // #### `args`
    // - `*ObjKibako2`
    VB_CRATE_DROP_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjKibako2`
    VB_CRATE_SETUP_DRAW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_CRAWL_SPEED_ENTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_CRAWL_SPEED_EXIT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Camera`
    // - 'int16_t' (csId)
    // - 'int16_t' (actionParameters)
    // - 'int16_t' (initTimer)
    // - 'CutsceneCameraPoint*' (atPoints)
    // - 'CutsceneCameraPoint*' (eyePoints)
    VB_CRAWL_SPEED_EXIT_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_CRAWL_SPEED_INCREASE,

    // #### `result`
    // ```c
    // this->actionTimer == 0 && Rand_ZeroOne() < 0.03f
    // ```
    // #### `args`
    // - `*EnPoRelay`
    VB_DAMPE_DROP_FLAME,

    // #### `result`
    // ```c
    // !Flags_GetItemGetInf(ITEMGETINF_1C)
    // ```
    // #### `args`
    // - `*EnTk`
    VB_DAMPE_GRAVEDIGGING_GRAND_PRIZE_BE_HEART_PIECE,

    // #### `result`
    // ```c
    // gSaveContext.dayTime <= 0xC000 || gSaveContext.dayTime >= 0xE000 || LINK_IS_ADULT || play->sceneNum !=
    // SCENE_GRAVEYARD
    // ```
    // #### `args`
    // - `*EnTk`
    VB_DAMPE_IN_GRAVEYARD_DESPAWN,

    // #### `result`
    // ```c
    // !Flags_GetItemGetInf(ITEMGETINF_OBTAINED_NUT_UPGRADE_FROM_STAGE) && (Player_GetMask(play) != PLAYER_MASK_SKULL)
    // ```
    // #### `args`
    // - None
    VB_DEKU_SCRUBS_REACT_TO_MASK_OF_TRUTH,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)
    // ```
    // #### `args`
    // - `*ObjDekujr`
    VB_DEKU_JR_CONSIDER_FOREST_TEMPLE_FINISHED,

    // #### `result`
    // ```c
    // this->unk_860 != 0
    // ```
    // #### `args`
    // - None
    VB_DEKU_STICK_BE_ON_FIRE,

    // #### `result`
    // ```c
    // AMMO(ITEM_STICK) != 0
    // ```
    // #### `args`
    // - None
    VB_DEKU_STICK_BREAK,

    // #### `result`
    // ```c
    // this->unk_860 < 20
    // ```
    // #### `args`
    // - None
    VB_DEKU_STICK_BURN_DOWN,

    // #### `result`
    // ##### In `Player_UpdateBurningDekuStick`
    // ```c
    // this->unk_85C == 0.0f
    // ```
    // ##### Also in `Player_UpdateBurningDekuStick`
    // ```c
    // DECR(this->unk_860) == 0;
    // ```
    // #### `args`
    // - None
    VB_DEKU_STICK_BURN_OUT,

    // #### `result`
    // ```c
    // Flags_GetItemGetInf(ITEMGETINF_30)
    // ```
    // #### `args`
    // - `*EnHs`
    VB_DESPAWN_GROG,

    // #### `result`
    // ```c
    // play->sceneNum == SCENE_LINKS_HOUSE && (!LINK_IS_ADULT ||
    // !Flags_GetEventChkInf(EVENTCHKINF_WON_COW_IN_MALONS_RACE)
    // ```
    // #### `args`
    // - `*EnCow`
    VB_DESPAWN_HORSE_RACE_COW,

    // #### `result`
    // ```c
    // !Flags_GetSwitch(play, this->actor.params & 0x3F)
    // ```
    // #### `args`
    // - `*EnDoor`
    VB_DOOR_BE_LOCKED,

    // #### `result`
    // ```c
    // ((doorActor->params >> 7) & 7) == 3
    // ```
    // #### `args`
    // - `*Actor`
    VB_DOOR_PLAY_SCENE_TRANSITION,
    // Vanilla condition: true
    VB_HATCH_CUCCO_OR_CHICKEN,
    // Vanilla condition: exchangeItemId == EXCH_ITEM_LETTER_ZELDA
    // Opt: s32
    VB_HEISHI2_ACCEPT_ITEM_AS_ZELDAS_LETTER,

    // #### `result`
    // In `Interface_DrawAmmoCount`:
    // ```c
    // (i == ITEM_STICK) ||
    // (i == ITEM_NUT) ||
    // (i == ITEM_BOMB) ||
    // (i == ITEM_BOW) ||
    // (
    //     (i >= ITEM_BOW_ARROW_FIRE) &&
    //     (i <= ITEM_BOW_ARROW_LIGHT)
    // ) ||
    // (i == ITEM_SLINGSHOT) ||
    // (i == ITEM_BOMBCHU) ||
    // (i == ITEM_BEAN)
    // ```
    // In `KaleidoScope_DrawAmmoCount`:
    // ```c
    // true
    // ```
    // #### `args`
    // - `*int16_t` (item id)
    VB_DRAW_AMMO_COUNT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - Player*
    VB_EMPTYING_BOTTLE,

    // #### `result`
    // ```c
    // (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)
    // ```
    // #### `args`
    // - None
    VB_END_GERUDO_MEMBERSHIP_TALK,

    // #### `result`
    // ```c
    // !(this->stateFlags3 & PLAYER_STATE3_PAUSE_ACTION_FUNC)
    // ```
    // #### `args`
    // - `*Player`
    // - `*Input`
    VB_EXECUTE_PLAYER_ACTION_FUNC,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `int32_t` (startMode)
    VB_EXECUTE_PLAYER_STARTMODE_FUNC,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnElf`
    VB_FAIRY_HEAL,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_FIX_SAW_SOFTLOCK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*BgHeavyBlock`
    VB_FREEZE_LINK_FOR_BLOCK_THROW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_FREEZE_LINK_FOR_FOREST_PILLARS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_FREEZE_ON_SKULL_TOKEN,

    // #### `result`
    // ```c
    // this->reward == GI_NONE
    // ```
    // #### `args`
    // - `*EnFr`
    VB_FROGS_GO_TO_IDLE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GANON_HEAL_BEFORE_FIGHT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGe2`
    VB_GERUDO_GUARD_SET_ACTION_AFTER_TALK,

    // #### `result`
    // See logic in
    // ```c
    // EnGe1_CheckCarpentersFreed()
    // ```
    // and
    // ```c
    // EnGe2_CheckCarpentersFreed()
    // ```
    // #### `args`
    // - None
    VB_GERUDOS_BE_FRIENDLY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnJs`
    VB_GIVE_BOMBCHUS_FROM_CARPET_SALESMAN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_BOLERO_OF_FIRE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_EPONAS_SONG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FAIRY_OCARINA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FIRE_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FOREST_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnNiwLady`
    VB_GIVE_ITEM_FROM_ANJU_AS_ADULT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnNiwLady`
    VB_GIVE_ITEM_FROM_ANJU_AS_CHILD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `int32_t` (ItemID)
    VB_GIVE_ITEM_FROM_BLUE_WARP,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBomBowlPit`
    VB_GIVE_ITEM_FROM_BOMBCHU_BOWLING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDns`
    VB_GIVE_ITEM_FROM_BUSINESS_SCRUB,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*EnJs`
    VB_GIVE_ITEM_FROM_CARPET_SALESMAN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnBox`
    VB_GIVE_ITEM_FROM_CHEST,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnCow`
    VB_GIVE_ITEM_FROM_COW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (EnDivingGame - &this->actor)
    VB_GIVE_ITEM_FROM_DIVING_MINIGAME,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // ##### In `EnGo2_GetItem`
    // - `*EnGo2`
    // ##### Everywhere else
    // - None
    VB_GIVE_ITEM_FROM_GORON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDs`
    VB_GIVE_ITEM_FROM_GRANNYS_SHOP,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGe1`
    VB_GIVE_ITEM_FROM_HORSEBACK_ARCHERY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnItem00`
    VB_GIVE_ITEM_FROM_ITEM_00,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMk`
    VB_GIVE_ITEM_FROM_LAB_DIVE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnHy`
    VB_GIVE_ITEM_FROM_LOST_DOG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMs`
    VB_GIVE_ITEM_FROM_MAGIC_BEAN_SALESMAN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_FROM_MAN_ON_ROOF,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGm`
    VB_GIVE_ITEM_FROM_MEDIGORON,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSkj`
    VB_GIVE_ITEM_FROM_OCARINA_MEMORY_GAME,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGb`
    VB_GIVE_ITEM_FROM_POE_COLLECTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSyatekiMan`
    VB_GIVE_ITEM_FROM_SHOOTING_GALLERY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSkj`
    VB_GIVE_ITEM_FROM_SKULL_KID_SARIAS_SONG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSth`
    VB_GIVE_ITEM_FROM_SKULLTULA_REWARD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (EnTa - &this->actor)
    VB_GIVE_ITEM_FROM_TALONS_CHICKENS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor` (EnExItem - &this->actor)
    VB_GIVE_ITEM_FROM_TARGET_IN_WOODS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_GERUDO_MEMBERSHIP_CARD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_LIGHT_ARROW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_LIGHT_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_MASTER_SWORD,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_MINUET_OF_FOREST,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_NOCTURNE_OF_SHADOW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_OCARINA_OF_TIME,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_PRELUDE_OF_LIGHT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_REQUIEM_OF_SPIRIT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SARIAS_SONG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SERENADE_OF_WATER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SHADOW_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSi`
    VB_GIVE_ITEM_SKULL_TOKEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SONG_OF_STORMS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SONG_OF_TIME,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SPIRIT_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_STRENGTH_1,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_SUNS_SONG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_WATER_MEDALLION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_WEIRD_EGG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_ZELDAS_LETTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_GIVE_ITEM_ZELDAS_LULLABY,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*VBFishingData`
    VB_GIVE_RANDO_FISHING_PRIZE,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - '*Fishing'
    VB_GIVE_RANDO_GLITCH_FISHING_PRIZE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGo2`
    VB_GORON_LINK_BE_SCARED,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_GORON_RUBY)
    // ```
    // #### `args`
    // - None
    VB_GORONS_CONSIDER_DODONGOS_CAVERN_FINISHED,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)
    // ```
    // #### `args`
    // - None
    VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED,

    // #### `result`
    // ```c
    // CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)
    // ```
    // #### `args`
    // - None
    VB_GORONS_CONSIDER_TUNIC_COLLECTED,

    // #### `result`
    // ```c
    // gSaveContext.rupees < 100
    // ```
    // #### `args`
    // - `NULL`
    VB_GRANNY_SAY_INSUFFICIENT_RUPEES,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDs`
    VB_GRANNY_TAKE_MONEY,

    // ### `result`
    // ```c
    // false
    // ```
    // ### `args`
    // - `*EnKusa`
    VB_GRASS_DROP_ITEM,

    // ### `result`
    // ```c
    // true
    // ```
    // ### `args`
    // - `*EnKusa`
    VB_GRASS_SETUP_DRAW,

    // #### `result`
    // ```c
    // Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)
    // ```
    // #### `args`
    // - None
    VB_GTG_GATE_BE_OPEN,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnCrow`
    VB_GUAY_DO_DIVE_ATTACK,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*EnCrow`
    VB_GUAY_FORCE_FLY_AWAY,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_HAVE_OCARINA_NOTE_A4,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_HAVE_OCARINA_NOTE_B4,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_HAVE_OCARINA_NOTE_D4,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_HAVE_OCARINA_NOTE_D5,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_HAVE_OCARINA_NOTE_F4,

    // #### `result`
    // ```c
    // var >= gSaveContext.health && gSaveContext.health > 0
    // ```
    // #### `args`
    // - None
    VB_HEALTH_METER_BE_CRITICAL,

    // #### `result`
    // ```c
    // (respawnFlag == 1) || (respawnFlag == -1)
    // ```
    // #### `args`
    // - `int32_t` (respawnFlag)
    VB_INFLICT_VOID_DAMAGE,

    // #### `result`
    // ```c
    // item >= ITEM_NONE_FE
    // ```
    // #### `args`
    // - `int32_t` (item)
    VB_ITEM_ACTION_BE_NONE,

    // #### `result`
    // ```c
    // Flags_GetCollectible(play, 0x1F)
    // ```
    // #### `args`
    // - `*ItemBHeart`
    VB_ITEM_B_HEART_DESPAWN,

    // #### `result`
    // ```c
    // Flags_GetCollectible(play, this->collectibleFlag)
    // ```
    // #### `args`
    // - `*EnItem00`
    VB_ITEM00_DESPAWN,

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
    // - `*EnFirefly`
    VB_KEESE_DO_DIVE_ATTACK,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*EnFirefly`
    VB_KEESE_FORCE_FLY_AWAY,

    // #### `result`
    // ```c
    // Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)
    // ```
    // #### `args`
    // - `*EnKz`
    VB_KING_ZORA_BE_MOVED,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)
    // ```
    // #### `args`
    // - `*EnKz`
    VB_KING_ZORA_THANK_CHILD,

    // #### `result`
    // ```c
    // CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA)
    // ```
    // #### `args`
    // - `*EnKz`
    VB_KING_ZORA_TUNIC_CHECK,

    // #### `result`
    // ```c
    // !Flags_GetSwitch(play, this->dyna.actor.params & 0x3F)
    // ```
    // #### `args`
    // - `*DoorShutter`
    VB_LOCK_BOSS_DOOR,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_SONG_EPONA)
    // ```
    // #### `args`
    // - None
    VB_MALON_ALREADY_TAUGHT_EPONAS_SONG,

    // #### `result`
    // ```c
    // Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)
    // ```
    // #### `args`
    // - None
    VB_MALON_RETURN_FROM_CASTLE,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    // ```
    // #### `args`
    // - `*EnMd`
    VB_MIDO_CONSIDER_DEKU_TREE_DEAD,

    // #### `result`
    // ```c
    // EnMd_ShouldSpawn(this, play)
    // ```
    // #### `args`
    // - `*EnMd`
    VB_MIDO_SPAWN,

    // #### `result`
    // ```c
    // this->interactInfo.talkState == NPC_TALK_STATE_ACTION
    // ```
    // #### `args`
    // - `*EnMd`
    //
    // ### Note:
    // When overriding this, ensure you're not in the intro cutscene as Mido's path has not been loaded
    VB_MOVE_MIDO_IN_KOKIRI_FOREST,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_MOVE_THROWN_ACTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnIk`
    VB_NABOORU_KNUCKLE_DEATH_SCENE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ElfMsg`
    VB_NAVI_TALK,

    // #### `result`
    // ```c
    // Inventory_HasEmptyBottle() == 0
    // ```
    // #### `args`
    // - None
    VB_NEED_BOTTLE_FOR_GRANNYS_ITEM,

    // #### `result`
    // ```c
    // Flags_GetInfTable(INFTABLE_GREETED_BY_SARIA)
    // ```
    // #### `args`
    // - None
    VB_NOT_BE_GREETED_BY_SARIA,

    // #### `result`
    // ```c
    // gSaveContext.inventory.dungeonKeys[gSaveContext.mapIndex] <= 0
    // ```
    // #### `args`
    // - `*EnDoor`
    VB_NOT_HAVE_SMALL_KEY,

    // #### `result`
    // ```c
    // Flags_GetItemGetInf(ITEMGETINF_30)
    // ```
    // #### `args`
    // - `*EnDs`
    VB_OFFER_BLUE_POTION,

    // #### `result`
    // ```c
    // CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)
    // ```
    // #### `args`
    // - `*EnKo`
    VB_OPEN_KOKIRI_FOREST,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*uint16_t` (overrideTextId)
    VB_OVERRIDE_LINK_THE_GORON_DIALOGUE,

    // #### `result`
    // ```c
    // this->actor.xzDistToPlayer < targetDist
    // ```
    // #### `args`
    // - `*EnOwl`
    VB_OWL_INTERACTION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*BossGanondrof`
    VB_PHANTOM_GANON_DEATH_SCENE,

    // #### `result`
    // ##### In `DoorWarp1_ChildWarpOut` - `SCENE_DODONGOS_CAVERN_BOSS`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)
    // ```
    // ##### In `DoorWarp1_ChildWarpOut` - `SCENE_DEKU_TREE_BOSS`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD)
    // ```
    // ##### In `DoorWarp1_RutoWarpOut`
    // ```c
    // true
    // ```
    // ##### In `DoorWarp1_AdultWarpOut` - `SCENE_FOREST_TEMPLE_BOSS`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP)
    // ```
    // ##### In `DoorWarp1_AdultWarpOut` - `SCENE_FIRE_TEMPLE_BOSS`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP)
    // ```
    // ##### In `DoorWarp1_AdultWarpOut` - `SCENE_WATER_TEMPLE_BOSS`
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP)
    // ```
    // ##### In `DoorWarp1_AdultWarpOut` - `SCENE_SPIRIT_TEMPLE_BOSS`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)
    // ```
    // ##### In `DoorWarp1_AdultWarpOut` - `SCENE_SHADOW_TEMPLE_BOSS`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)
    // ```
    // #### `args`
    // - `int32_t` (EVENTCHKINF / RAND_INF_DUNGEONS_DONE)
    VB_PLAY_BLUE_WARP_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_BOLERO_OF_FIRE_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDaiku`
    VB_PLAY_CARPENTER_FREE_CS,

    // #### `result`
    // Close enough & various cutscene checks
    // ```c
    // (func_80AEC5FC(this, play)) && (!Play_InCsMode(play)) &&
    // (!(player->stateFlags1 & (PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
    // PLAYER_STATE1_CLIMBING_LADDER))) &&
    // (player->actor.bgCheckFlags & 1)
    // ```
    // #### `args`
    // - `*EnRu1`
    VB_PLAY_CHILD_RUTO_INTRO,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_DARUNIAS_JOY_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*BgTreemouth`
    VB_PLAY_DEKU_TREE_INTRO_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*DemoKekkai`
    VB_PLAY_DISPEL_BARRIER_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOkarinaTag`
    VB_PLAY_DOOR_OF_TIME_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOkarinaTag`
    VB_PLAY_DRAIN_WELL_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_DROP_FISH_FOR_JABU_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // ##### In `z_demo.c`:
    // - `int32_t` (entranceCutscene->flag) (promoted from `uint8_t` by va_arg)
    // - `int32_t` (entranceCutscene->entrance) (promoted from `uint16_t` by va_arg)
    // ##### In `z_bg_breakwall.c` and `z_bg_toki_swd.c`:
    // - `int32_t` (EVENTCHKINF)
    // - `int32_t` (gSaveContext.entranceIndex)
    VB_PLAY_ENTRANCE_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // ##### When called from `z_en_ds.c`
    // - `*EnDs`
    // ##### When called from `z_en_mk.c`
    // - `*EnMk`
    VB_PLAY_EYEDROP_CREATION_ANIM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_EYEDROPS_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_FIRE_ARROW_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnHeishi2`
    // - `bool` (clearCamera - true if the code clears a sub-camera, false otherwise)
    VB_PLAY_GATE_OPENING_OR_CLOSING_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_GORON_FREE_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_MINUET_OF_FOREST_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_MWEEP_CS,

    // #### `result`
    // ```c
    // this->getItemId == GI_GAUNTLETS_SILVER
    // ```
    // #### `args`
    // - None
    VB_PLAY_NABOORU_CAPTURED_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_PLAY_ONEPOINT_ACTOR_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*int16_t` (cutscene id)
    VB_PLAY_ONEPOINT_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_PRELUDE_OF_LIGHT_CS,

    // #### `result`
    // ##### In `z_demo.c` - `Cutscene_Command_Terminator` (setting `shouldSkipCommand`)
    // ```c
    // true
    // ```
    // ##### In `z_demo.c` - `Cutscene_Command_Terminator` (inside if block and switch)
    // ```c
    // !Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)
    // ```
    // #### `args`
    // - None
    VB_PLAY_PULL_MASTER_SWORD_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_RAINBOW_BRIDGE_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnOkarinaTag`
    VB_PLAY_ROYAL_FAMILY_TOMB_CS,

    // #### `result`
    // ```c
    // play->csCtx.state != 0 && play->csCtx.npcActions[3] != NULL && play->csCtx.npcActions[3]->action == 2
    // ```
    // #### `args`
    // - `*BgSpot02Objects`
    VB_PLAY_ROYAL_FAMILY_TOMB_EXPLODE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnSa`
    VB_PLAY_SARIAS_SONG_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_SERENADE_OF_WATER_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_PLAY_SHIEK_BLOCK_MASTER_SWORD_CS,

    // #### `result`
    // ```c
    // (giEntry.itemId != ITEM_NONE) && (giEntry.gi >= 0) && (Item_CheckObtainability(giEntry.itemId) == ITEM_NONE)
    // ```
    // #### `args`
    // - `*EnBox`
    VB_PLAY_SLOW_CHEST_CS,

    // #### `result`
    // ```c
    // !CHECK_QUEST_ITEM(QUEST_SONG_SUN)
    // ```
    // #### `args`
    // - `*EnOkarinaTag`
    VB_PLAY_SUNS_SONG_CS,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*LinkAnimationHeader`
    VB_PLAY_THROW_ANIMATION,

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
    // - `*DemoIm`
    VB_PLAY_ZELDAS_LULLABY_CS,

    // #### `result`
    // ```c
    // item == ITEM_SAW
    // ```
    // #### `args`
    // - None
    VB_POACHERS_SAW_SET_DEKU_NUT_UPGRADE_FLAG,

    // #### `result`
    // ```c
    // (dropParams >= ITEM00_RUPEE_GREEN) && (dropParams <= ITEM00_BOMBS_SPECIAL)
    // ```
    // #### `args`
    // - `*ObjTsubo`
    VB_POT_DROP_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjTsubo`
    VB_POT_SETUP_DRAW,

    // #### `result`
    // ```c
    // dropId == ITEM00_STICK
    // ```
    // #### `args`
    // - None
    VB_PREVENT_ADULT_STICK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnRd`
    VB_REDEAD_GIBDO_FREEZE_LINK,

    // #### `result`
    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_RENDER_KEY_COUNTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_RENDER_RUPEE_COUNTER,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `**Gfx` (`&POLY_OPA_DISP`)
    VB_RENDER_YES_ON_CONTINUE_PROMPT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_REVERT_SPOILING_ITEMS,

    // #### `result`
    // ```c
    // !Flags_GetInfTable(INFTABLE_145)
    // ```
    // #### `args`
    // - `*EnRu1`
    VB_RUTO_BE_CONSIDERED_NOT_KIDNAPPED,

    // #### `result`
    // Landed on the platform in the big okto room
    // ```c
    // dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_BG_BDAN_OBJECTS &&
    // dynaPolyActor->actor.params == 0 && !Player_InCsMode(play) && play->msgCtx.msgLength == 0
    // ```
    // #### `args`
    // - `*EnRu1`
    // - `*DynaPolyActor`
    VB_RUTO_RUN_TO_SAPPHIRE,

    // #### `result`
    // in the big okto room and flag
    // ```c
    // !Flags_GetInfTable(INFTABLE_RUTO_IN_JJ_WANTS_TO_BE_TOSSED_TO_SAPPHIRE) && (func_80AEB124(play) != 0)
    // ```
    // #### `args`
    // - `*EnRu1`
    VB_RUTO_WANT_TO_BE_TOSSED_TO_SAPPHIRE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnGb`
    VB_SELL_POES_TO_POE_COLLECTOR,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `int32_t` (button - promoted from `s16`)
    VB_SET_BUTTON_ITEM_FROM_C_BUTTON_SLOT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnNiwLady`
    VB_SET_CUCCO_COUNT,

    // #### `result`
    // ```c
    // SurfaceType_GetSlope(&play->colCtx, poly, bgId) == 2
    // ```
    // #### `args`
    // - `*int16_t` - original next entrance index (`play->setupExitList[exitIndex - 1]`)
    VB_SET_VOIDOUT_FROM_SURFACE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - BgHakaShip*
    // - PlayState*
    VB_SHADOW_SHIP_SET_SPEED,

    // #### `result`
    // ```c
    // (!CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) &&
    // !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER)) && LINK_IS_ADULT
    // ```
    // #### `args`
    // - None
    VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_SHORT_CIRCUIT_GIVE_ITEM_PROCESS,

    // #### `result`
    // ```c
    // (s16)sFishingRecordLength < (s16)sFishOnHandLength
    // ```
    // #### `args`
    // - `*f32` (sFishOnHandLength)
    VB_SHOULD_CHECK_FOR_FISHING_RECORD,

    // #### `result`
    // ##### In `Fishing_HandleOwnerDialog` - `LINK_AGE_CHILD`
    // ```c
    // (sFishingRecordLength >= 50.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD)
    // ```
    // ##### In `Fishing_HandleOwnerDialog` - `!LINK_AGE_CHILD`
    // ```c
    // (sFishingRecordLength >= 60.0f) && !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT)
    // ```
    // #### `args`
    // - `*VBFishingData`
    VB_SHOULD_GIVE_VANILLA_FISHING_PRIZE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*VBFishingData`
    VB_SHOULD_SET_FISHING_RECORD,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - `*s8 iter2`
    // - `s8 sp3C[4]`
    VB_SHOULD_QUICKSPIN,

    // #### `result`
    // ```c
    // (interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT
    // ```
    // #### `args`
    // - None
    VB_SHOW_MASTER_SWORD_TO_PLACE_IN_PEDESTAL,

    // #### `result`
    // ```c
    // gSaveContext.showTitleCard
    // ```
    // #### `args`
    // - None
    VB_SHOW_TITLE_CARD,

    // #### `result`
    // ```c
    // (collectible >= 0) && (collectible <= 0x19
    // ```
    // #### `args`
    // - `*ObjKibako`
    VB_SMALL_CRATE_DROP_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjKibako`
    VB_SMALL_CRATE_SETUP_DRAW,

    // #### `result`
    // ```c
    // false
    // ```
    // #### `args`
    // - None
    VB_SKIP_SCARECROWS_SONG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ObjBean`
    VB_SPAWN_BEAN_STALK_FAIRIES,

    // #### `result`
    // ```c
    // this->timer >= 60
    // ```
    // #### `args`
    // - `None`
    VB_SPAWN_BEAN_SKULLTULA,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // ##### In `z_boss_dodongo.c`:
    // - `*BossDodongo`
    //
    // ##### In `z_boss_fd2.c`
    // - `*BossFd2`
    //
    // ##### In `z_boss_ganondrof.c`:
    // - `*BossGanondrof`
    //
    // ##### In `z_boss_goma.c`:
    // - `*BossGoma`
    //
    // ##### In `z_boss_mo.c`:
    // - `*BossMo`
    //
    // ##### In `z_boss_sst.c`:
    // - `*BossSst`
    //
    // ##### In `z_boss_tw.c`:
    // - `*BossTw`
    //
    // ##### In `z_boss_va.c`:
    // - `*BossVa`
    VB_SPAWN_BLUE_WARP,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE
    // ```
    // #### `args`
    // - None
    VB_SPAWN_FIRE_ARROW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnElf`
    VB_SPAWN_FOUNTAIN_FAIRIES,

    // #### `result`
    // ```c
    // this->unk_19D & 1
    // ```
    // #### `args`
    // - `*EnGs`
    VB_SPAWN_GOSSIP_STONE_FAIRY,

    // #### `result`
    // ##### In `BossFd_Fly` (`case BOSSFD_SKULL_BURN`)
    // ```c
    // this->timers[0] == 7
    // ```
    // ##### Everywhere else
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_SPAWN_HEART_CONTAINER,

    // #### `result`
    // ```c
    // (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_ODD_POTION) ? true : false
    // ```
    // #### `args`
    // - `*EnKo`
    VB_SPAWN_LW_FADO,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*ShotSun`
    VB_SPAWN_SONG_FAIRY,

    // #### `result`
    // ```c
    // varies, never set should to true
    // ```
    // #### `args`
    // - `*Actor`
    // - `*s16` - timer value
    VB_SWITCH_TIMER_TICK,

    // #### `result`
    // ```c
    // (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) &&
    // CHECK_BTN_ANY(sControlInput->press.button, buttonsToCheck)
    // ```
    // #### `args`
    // - `*Input`
    VB_THROW_OR_PUT_DOWN_HELD_ITEM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnHs`
    VB_TRADE_COJIRO,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMk`
    VB_TRADE_FROG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnDs`
    VB_TRADE_ODD_MUSHROOM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnKo`
    VB_TRADE_ODD_POTION,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnNiwLady`
    VB_TRADE_POCKET_CUCCO,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnToryo`
    VB_TRADE_SAW,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnMk`
    VB_TRADE_TIMER_EYEDROPS,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_FROG)
    // ```
    // #### `args`
    // - None
    VB_TRADE_TIMER_FROG,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - None
    VB_TRADE_TIMER_ODD_MUSHROOM,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*PauseContext`
    VB_TRANSITION_TO_SAVE_SCREEN_ON_DEATH,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnWood02`
    VB_TREE_DROP_COLLECTIBLE,

    // ```c
    // true
    // ```
    // #### `args`
    // - `int32_t` (button - promoted from `u8`)
    // - `int32_t` (item - promoted from `u8`)
    VB_UPDATE_BOTTLE_ITEM,

    // #### `result`
    // ```c
    // INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYEDROPS
    // ```
    // #### `args`
    // - `*EnMk`
    VB_USE_EYEDROP_DIALOGUE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnWonderTalk2`
    VB_WONDER_TALK,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_TRIGGER_VOIDOUT,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_TORCH2_HANDLE_CLANKING,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*Actor`
    VB_RECIEVE_FALL_DAMAGE,

    // #### `result`
    // ```c
    // true
    // ```
    // #### `args`
    // - `*EnRr`
    VB_LIKE_LIKE_GRAB_PLAYER,

    // #### `result`
    // ```c
    // entry != NULL
    // ```
    // #### `args`
    // - `*AnimationEntry`
    // - `*LinkAnimationHeader`
    // - `s32` frame
    // - `s32` limbCount
    // - `*Vec3s` frameTable
    VB_LOAD_PLAYER_ANIMATION_FRAME,

    // #### `result`
    // ```c
    // DoorWarp1_PlayerInRange(this, play)
    // ```
    // #### `args`
    // - `*DoorWarp1`
    VB_BLUE_WARP_CONSIDER_ADULT_IN_RANGE,

    // #### `result`
    // ```c
    // (CVarGetInteger(CVAR_GAMEPLAY_STATS("ShowIngameTimer"), 0) && gSaveContext.fileNum >= 0 && gSaveContext.fileNum
    // <= 2)
    // ```
    // #### `args`
    // - `*PlayState`
    VB_SHOW_GAMEPLAY_TIMER,
    // (this->dyna.actor.params >> 5 & 0x7F) == GI_ICE_TRAP && this->actionFunc == EnBox_Open &&
    // this->skelanime.curFrame > 45 && this->iceSmokeTimer < 100
    // ```
    // #### `args`
    // - `*EnBox`
    VB_CHEST_USE_ICE_EFFECT,
} GIVanillaBehavior;

#endif
