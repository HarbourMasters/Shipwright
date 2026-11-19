#include "soh/Enhancements/custom-message/MessageCatalog.h"
/**
 * This file handles custom messages relating to Items,
 * such as Get Item messages for non-vanilla items,
 * Vanilla/MQ hints when collecting Maps, Ice Trap messages,
 * etc.
 */

#include <cstdarg>
#include "soh/Enhancements/Lang/MessageLanguage.h"
#include <algorithm>

#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/randomizerEnumStrings.h"
#include "soh/Enhancements/randomizer/Traps.h"
#include "soh/Enhancements/randomizer/item.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/ShipInit.hpp"
#include <soh/ResourceManagerHelpers.h>

extern "C" {
#include "variables.h"
#include "macros.h"
#include "functions.h"
#include "z64item.h"
extern PlayState* gPlayState;
}

void BuildTriforcePieceMessage(CustomMessage& msg) {
    auto rando = OTRGlobals::Instance->gRandomizer;
    uint8_t current = gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected + 1;
    // if any settings are off, 0 them out here as a precaution
    uint8_t bridge = rando->GetRandoSettingValue(RSK_RAINBOW_BRIDGE) == RO_BRIDGE_TRIFORCE_PIECES
                         ? rando->GetRandoSettingValue(RSK_RAINBOW_BRIDGE_TRIFORCE_COUNT)
                         : 0;
    uint8_t wincon = rando->GetRandoSettingValue(RSK_WINCON) == RO_WINCON_TRIFORCE_PIECES
                         ? rando->GetRandoSettingValue(RSK_WINCON_TRIFORCE_COUNT)
                         : 0;
    uint8_t GBK = rando->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_TRIFORCE_PIECES
                      ? rando->GetRandoSettingValue(RSK_GBK_TRIFORCE_COUNT)
                      : 0;
    uint8_t soul = rando->GetRandoSettingValue(RSK_GANONS_SOUL) == RO_GANONS_SOUL_TRIFORCE_PIECES
                       ? rando->GetRandoSettingValue(RSK_GANONS_SOUL_TRIFORCE_COUNT)
                       : 0;

    // If we reach wincon, we win!
    if (current == wincon) {
        msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.0");
        // otherwise prioritise the different triggers
    } else if (current == bridge) {
        msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.1");
    } else if (current == GBK) {
        msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.2");
    } else if (current == soul) {
        msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.3");
        // if everything is zero, then there's no goal...
    } else if (bridge + wincon + GBK + soul == 0) {
        msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.4");
    } else {
        // if nothing is complete, we need to check is we have more than we need
        uint8_t highest = std::max({ current, bridge, wincon, GBK, soul });
        if (highest == current) {
            // RANDOTODO TODO_TRANSLATE you could maybe make this sound cleaner because InsertNumber allows for dynamic
            // plurals
            msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.5");
            msg.InsertNumber(std::max({ bridge, wincon, GBK, soul }));
        } else {
            // find the next goal by setting everything below current (including failed conditions set to 0 before)
            // to a high number, then looking for the lowest.
            // if we have the exact amount, it will be caught by the first check, so no worries there
            if (bridge < current) {
                bridge = 255;
            }
            if (GBK < current) {
                GBK = 255;
            }
            if (soul < current) {
                soul = 255;
            }
            if (wincon < current) {
                wincon = 255;
            }
            uint8_t next = std::min({ bridge, GBK, soul, wincon });

            uint8_t remaining = next - current;
            float percentageCollected = (float)current / (float)next;

            if (percentageCollected <= 0.25) {
                msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.6");
            } else if (percentageCollected <= 0.5) {
                msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.7");
            } else if (percentageCollected <= 0.75) {
                msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.8");
            } else if (percentageCollected < 1.0) {
                msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.9");
            }

            // default condition is soul
            CustomMessage condition =
                CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.10");
            if (next == wincon) {
                condition =
                    CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.11");
            } else if (next == bridge) {
                condition =
                    CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.12");
            } else if (next == GBK) {
                condition =
                    CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_piece_message.13");
            }
            msg.Replace("[[condition]]", condition);
            msg.InsertNumber(remaining);
        }
    }
    msg.Replace("[[current]]", std::to_string(current));
    msg.AutoFormat(ITEM_CUSTOM);
}

void BuildTriforceMessage(CustomMessage& msg) {
    msg = CustomMessages::CreateMessage("randomizer.messages.item_messages.build_triforce_message.0");
    msg.Format(ITEM_CUSTOM);
}

void BuildCustomItemMessage(Player* player, CustomMessage& msg) {
    int16_t rgid;
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        rgid = player->getItemEntry.getItemId;
    } else {
        rgid = player->getItemId;
    }
    const std::string itemKey(EnumToString(static_cast<RandomizerGet>(rgid)));
    msg = CustomMessages::GetMessage("randomizer.get_item.items." + itemKey, "randomizer.get_item.default",
                                     TEXTBOX_TYPE_BLUE);
    CustomMessage name = CustomMessages::GetMessageOrDefault(
        "randomizer.item_names." + itemKey,
        CustomMessage(Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetName(), TEXTBOX_TYPE_BLUE));
    if (rgid == RG_OPEN_CHEST &&
        OTRGlobals::Instance->gRandoContext->GetOption(RSK_SHUFFLE_OPEN_CHEST).Is(RO_OPEN_CHEST_PROGRESSIVE)) {
        // message is built before the item is given, so the flags still say which copy this is
        name = Flags_GetRandomizerInf(RAND_INF_CAN_OPEN_CHEST)
                   ? CustomMessages::GetMessage("randomizer.open_chest.big", TEXTBOX_TYPE_BLUE)
                   : CustomMessages::GetMessage("randomizer.open_chest.small", TEXTBOX_TYPE_BLUE);
    }
    CustomMessage article = CustomMessage(
        Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetArticle(), TEXTBOX_TYPE_BLUE);
    msg.Replace("[[article]]", article);
    msg.Replace("[[color]]", Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).GetColor());
    msg.Replace("[[name]]", name);
    if (Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(rgid)).HasCustomIcon()) {
        msg.AutoFormat(ITEM_CUSTOM);
    } else {
        msg.AutoFormat();
    }
}

void LoadCustomItemIcon(bool /* legacy English-fallback hook argument */) {
    Player* player = GET_PLAYER(gPlayState);
    const char* customIcon = nullptr;
    CustomIconSize iconSize = ICON_SIZE_32;
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        RandomizerGet rgid = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
        customIcon = Rando::StaticData::RetrieveItem(rgid).GetCustomIcon();
        iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
    } else {
        // if we're seeing an icon and we don't have a GI, assume we're in the alter text showing a triforce piece
        customIcon = Rando::StaticData::RetrieveItem(RG_TRIFORCE_PIECE).GetCustomIcon();
        iconSize = Rando::StaticData::RetrieveItem(RG_TRIFORCE_PIECE).GetCustomIconSize();
    }
    if (customIcon != nullptr) {
        static int16_t sIconItem32XOffsets[] = { 74, 74, 74, 54 };
        static int16_t sIconItem24XOffsets[] = { 72, 72, 72, 50 };
        MessageContext* msgCtx = &gPlayState->msgCtx;
        const uint8_t language =
            MessageLanguage_UsesUnicode() ? static_cast<uint8_t>(LANGUAGE_JPN) : MessageLanguage_Get();
        if (iconSize == ICON_SIZE_32) {
            R_TEXTBOX_ICON_XPOS = R_TEXT_INIT_XPOS - sIconItem32XOffsets[language];
            R_TEXTBOX_ICON_YPOS = (R_TEXTBOX_Y + 10) + 6;
            R_TEXTBOX_ICON_SIZE = 32;
        } else {
            R_TEXTBOX_ICON_XPOS = R_TEXT_INIT_XPOS - sIconItem24XOffsets[language];
            R_TEXTBOX_ICON_YPOS = (R_TEXTBOX_Y + 10) + 10;
            R_TEXTBOX_ICON_SIZE = 24;
        }
        strcpy((char*)((uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE), customIcon);
        msgCtx->msgBufPos++;
        msgCtx->choiceNum = 1;
    }
}

void DrawCustomItemIcon(Gfx** p) {
    Gfx* gfx = *p;
    MessageContext* msgCtx = &gPlayState->msgCtx;
    Player* player = GET_PLAYER(gPlayState);
    CustomIconSize iconSize = ICON_SIZE_32;
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        RandomizerGet rgid = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
        iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
    }
    if (iconSize == ICON_SIZE_24) {
        gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA,
                            G_IM_SIZ_32b, 24, 24, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA,
                            G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }
    *p = gfx;
}

void BuildItemMessage(u16* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    CustomMessage msg;

    if (player->getItemEntry.getItemId == RG_ICE_TRAP) {
        Rando::Traps::BuildIceTrapMessage(msg, player->getItemEntry);
    } else if (player->getItemEntry.getItemId == RG_TRIFORCE_PIECE) {
        BuildTriforcePieceMessage(msg);
    } else if (player->getItemEntry.getItemId == RG_TRIFORCE) {
        BuildTriforceMessage(msg);
    } else {
        BuildCustomItemMessage(player, msg);
    }
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void BuildMapMessage(uint16_t* textId, bool* loadFromMessageTable) {
    GetItemEntry itemEntry = GET_PLAYER(gPlayState)->getItemEntry;
    auto ctx = OTRGlobals::Instance->gRandoContext;
    const std::string itemKey(EnumToString(static_cast<RandomizerGet>(itemEntry.getItemId)));
    CustomMessage msg =
        CustomMessages::GetMessage("randomizer.get_item.maps." + itemKey, "randomizer.get_item.map", TEXTBOX_TYPE_BLUE);
    int sceneNum = -1;
    switch (itemEntry.getItemId) {
        case RG_DEKU_TREE_MAP:
            sceneNum = SCENE_DEKU_TREE;
            break;
        case RG_DODONGOS_CAVERN_MAP:
            sceneNum = SCENE_DODONGOS_CAVERN;
            break;
        case RG_JABU_JABUS_BELLY_MAP:
            sceneNum = SCENE_JABU_JABU;
            break;
        case RG_FOREST_TEMPLE_MAP:
            sceneNum = SCENE_FOREST_TEMPLE;
            break;
        case RG_FIRE_TEMPLE_MAP:
            sceneNum = SCENE_FIRE_TEMPLE;
            break;
        case RG_WATER_TEMPLE_MAP:
            sceneNum = SCENE_WATER_TEMPLE;
            break;
        case RG_SPIRIT_TEMPLE_MAP:
            sceneNum = SCENE_SPIRIT_TEMPLE;
            break;
        case RG_SHADOW_TEMPLE_MAP:
            sceneNum = SCENE_SHADOW_TEMPLE;
            break;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            sceneNum = SCENE_BOTTOM_OF_THE_WELL;
            break;
        case RG_ICE_CAVERN_MAP:
            sceneNum = SCENE_ICE_CAVERN;
            break;
    }
    CustomMessage name =
        CustomMessage(Rando::StaticData::RetrieveItem(static_cast<RandomizerGet>(itemEntry.getItemId)).GetName());
    msg.Replace("[[name]]", name);
    if (ctx->GetOption(RSK_MQ_DUNGEON_RANDOM).Is(RO_MQ_DUNGEONS_NONE) ||
        (ctx->GetOption(RSK_MQ_DUNGEON_RANDOM).Is(RO_MQ_DUNGEONS_SET_NUMBER) &&
         ctx->GetOption(RSK_MQ_DUNGEON_COUNT).Is(MAX_MQ_DUNGEON_COUNT))) {
        msg.Replace("[[typeHint]]", "");
    } else if (ResourceMgr_IsSceneMasterQuest(static_cast<int16_t>(sceneNum))) {
        msg.Replace("[[typeHint]]", Rando::StaticData::hintTextTable[RHT_DUNGEON_MASTERFUL].GetHintMessage());
    } else {
        msg.Replace("[[typeHint]]", Rando::StaticData::hintTextTable[RHT_DUNGEON_ORDINARY].GetHintMessage());
    }
    *loadFromMessageTable = false;
    msg.AutoFormat(ITEM_DUNGEON_MAP);
    msg.LoadIntoFont();
}

void BuildBossKeyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->getItemEntry.getItemId == RG_GANONS_CASTLE_BOSS_KEY &&
        !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_GANONS_BOSS_KEY)) {
        return;
    }
    if (player->getItemEntry.getItemId != RG_GANONS_CASTLE_BOSS_KEY &&
        !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_BOSS_KEYSANITY)) {
        return;
    }
    CustomMessage msg;
    BuildCustomItemMessage(player, msg);
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void BuildSmallKeyMessage(uint16_t* textId, bool* loadFromMessageTable) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->getItemEntry.getItemId == RG_GERUDO_FORTRESS_SMALL_KEY &&
        OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_KEYS).Is(RO_GERUDO_KEYS_VANILLA)) {
        return;
    }
    if (player->getItemEntry.getItemId != RG_GERUDO_FORTRESS_SMALL_KEY &&
        DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_KEYSANITY)) {
        return;
    }
    CustomMessage msg;
    BuildCustomItemMessage(player, msg);
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void RegisterItemMessages() {
    COND_ID_HOOK(OnOpenText, TEXT_RANDOMIZER_CUSTOM_ITEM, IS_RANDO, BuildItemMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_DUNGEON_MAP, DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS),
                 BuildMapMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_COMPASS, DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS),
                 BuildItemMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_KEY_BOSS,
                 (DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_BOSS_KEYSANITY) ||
                  DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_GANONS_BOSS_KEY)),
                 BuildBossKeyMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ITEM_KEY_SMALL,
                 (OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_KEYS).IsNot(RO_GERUDO_KEYS_VANILLA) ||
                  DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_KEYSANITY)),
                 BuildSmallKeyMessage);
}

static RegisterShipInitFunc initFunc(RegisterItemMessages, { "IS_RANDO" });

void RegisterCustomIconHooks() {
    COND_VB_SHOULD(VB_LOAD_ITEM_ICON, IS_RANDO, {
        if (*should == false) {
            LoadCustomItemIcon(static_cast<bool>(va_arg(args, int)));
        }
    });
    COND_VB_SHOULD(VB_DRAW_ITEM_ICON, IS_RANDO, {
        if (*should == false) {
            DrawCustomItemIcon(va_arg(args, Gfx**));
        }
    });
}

static RegisterShipInitFunc customIconInitFunc(RegisterCustomIconHooks, { "IS_RANDO" });
