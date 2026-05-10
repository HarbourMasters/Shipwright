/**
 * This file handles custom messages relating to Items,
 * such as Get Item messages for non-vanilla items,
 * Vanilla/MQ hints when collecting Maps, Ice Trap messages,
 * etc.
 */
#include <libultraship/libultraship.h>
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/randomizer/split_songs.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/Traps.h"
#include "soh/Enhancements/randomizer/item.h"
#include "soh/ShipInit.hpp"
#include <soh/ResourceManagerHelpers.h>

#include <cstdint>
#include <cstdarg>
#include <cstring>

extern "C" {
#include <variables.h>
#include <macros.h>
#include "z64item.h"
#include "z64player.h"
#include "z64save.h"
extern PlayState* gPlayState;
void Message_LoadItemIcon(PlayState* play, u16 itemId, s16 y);
GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
}

/**
 * Match `Player`'s get-item display (`z_player` func_8084DFF4): if `getItemId` does not match `getItemEntry`,
 * the table row keyed by `getItemId` wins. `Message_OpenText` runs before the entry is synced onto the player,
 * so reading only `player->getItemEntry` here can embed the wrong textbox icon versus the real gift.
 */
static GetItemEntry ResolveGiftGetItemEntry(const Player* player) {
    if (player == nullptr) {
        return GET_ITEM_NONE;
    }
    if (player->getItemEntry.objectId == OBJECT_INVALID || player->getItemId != player->getItemEntry.getItemId) {
        if (IS_RANDO && player->getItemId > RG_NONE && player->getItemId < RG_MAX) {
            return ItemTable_RetrieveEntry(MOD_RANDOMIZER, player->getItemId);
        }
        return ItemTable_Retrieve(player->getItemId);
    }
    return player->getItemEntry;
}

static bool LoadCustomItemIcon(bool displayAsEnglish);
static bool RefreshCustomItemIconForTextbox(bool displayAsEnglish, bool incrementDecodeState);
static void ApplyCustomIconPathToTextbox(const char* customIcon, CustomIconSize iconSize, bool displayAsEnglish,
                                         bool incrementDecodeState);
static void DrawTextboxItemIconFromSegment(Gfx** p);
static RandomizerGet RandomizerGetFromPlayerItemEntry(const Player* player);

/** Cache the custom icon chosen during decode; draw can reuse if player context drifts before render. */
static const char* g_LastDecodedCustomIconPath = nullptr;
static CustomIconSize g_LastDecodedCustomIconSize = ICON_SIZE_32;

/** Called from z_message_PAL MESSAGE_ITEM_ICON decode — ITEM_CUSTOM uses OTR path then vanilla fallback. */
extern "C" void Randomizer_Message_DecodeLoadItemIcon(PlayState* play, u16 iconToLoad, s32 displayAsEnglishAsInt) {
    if (play == nullptr || iconToLoad < ITEM_CUSTOM) {
        return;
    }
    const bool displayAsEnglish = displayAsEnglishAsInt != 0;
    if (gSaveContext.ship.quest.id == QUEST_RANDOMIZER) {
        if (LoadCustomItemIcon(displayAsEnglish)) {
            return;
        }
    }
    Message_LoadItemIcon(play, iconToLoad, (s16)(R_TEXTBOX_Y + 10));
}

extern "C" void Randomizer_Message_RefreshCustomItemIconAtDraw(s32 displayAsEnglishAsInt) {
    const bool displayAsEnglish = displayAsEnglishAsInt != 0;
    (void)RefreshCustomItemIconForTextbox(displayAsEnglish, false);
}

extern "C" void Randomizer_Message_AppendSegmentIconGfx(Gfx** gfxp) {
    if (gfxp != nullptr) {
        DrawTextboxItemIconFromSegment(gfxp);
    }
}

static ItemID VanillaItemIdForFullSong(RandomizerGet fullSongRg) {
    switch (fullSongRg) {
        case RG_ZELDAS_LULLABY:
            return ITEM_SONG_LULLABY;
        case RG_EPONAS_SONG:
            return ITEM_SONG_EPONA;
        case RG_SARIAS_SONG:
            return ITEM_SONG_SARIA;
        case RG_SUNS_SONG:
            return ITEM_SONG_SUN;
        case RG_SONG_OF_TIME:
            return ITEM_SONG_TIME;
        case RG_SONG_OF_STORMS:
            return ITEM_SONG_STORMS;
        case RG_MINUET_OF_FOREST:
            return ITEM_SONG_MINUET;
        case RG_BOLERO_OF_FIRE:
            return ITEM_SONG_BOLERO;
        case RG_SERENADE_OF_WATER:
            return ITEM_SONG_SERENADE;
        case RG_REQUIEM_OF_SPIRIT:
            return ITEM_SONG_REQUIEM;
        case RG_NOCTURNE_OF_SHADOW:
            return ITEM_SONG_NOCTURNE;
        case RG_PRELUDE_OF_LIGHT:
            return ITEM_SONG_PRELUDE;
        default:
            return ITEM_NONE;
    }
}

/** Same rules as `BuildCustomItemMessage` for resolving `RandomizerGet` from the current get-item context. */
static RandomizerGet RandomizerGetFromPlayerItemEntry(const Player* player) {
    if (player == nullptr) {
        return RG_NONE;
    }
    const GetItemEntry gift = ResolveGiftGetItemEntry(player);
    int16_t rgid;
    if (gift.modIndex == MOD_RANDOMIZER) {
        rgid = gift.getItemId;
    } else if (gift.objectId != OBJECT_INVALID) {
        rgid = gift.getItemId;
    } else {
        rgid = player->getItemId;
    }
    if (rgid < 0) {
        rgid = static_cast<int16_t>(-rgid);
    }
    return static_cast<RandomizerGet>(rgid);
}

/** Fallback: vanilla pause `gItemIcons` quest song row if tracker mapping is unavailable. */
static ItemID VanillaSongIconForSplitOrProgressive(RandomizerGet rg) {
    if (Rando::SplitSongs::IsSongPart(rg)) {
        const Rando::SplitSongDef* def = Rando::SplitSongs::GetSongDefFromPart(rg);
        return def != nullptr ? VanillaItemIdForFullSong(def->fullSong) : ITEM_NONE;
    }
    if (Rando::SplitSongs::IsProgressiveSong(rg)) {
        RandomizerGet stage = Rando::SplitSongs::ResolveProgressiveSongStage(rg);
        if (stage == RG_NONE) {
            const Rando::SplitSongDef* def = Rando::SplitSongs::GetSongDefFromProgressive(rg);
            if (def != nullptr) {
                stage = def->part1;
            }
        }
        const Rando::SplitSongDef* def = stage != RG_NONE ? Rando::SplitSongs::GetSongDefFromPart(stage) : nullptr;
        return def != nullptr ? VanillaItemIdForFullSong(def->fullSong) : ITEM_NONE;
    }
    return ITEM_NONE;
}

static ItemID DirectSongIconForRandomizerGet(RandomizerGet rg) {
    switch (rg) {
        case RG_ZELDAS_LULLABY:
        case RG_PROGRESSIVE_ZELDAS_LULLABY:
        case RG_ZELDAS_LULLABY_PART1:
        case RG_ZELDAS_LULLABY_PART2:
            return ITEM_SONG_LULLABY;
        case RG_EPONAS_SONG:
        case RG_PROGRESSIVE_EPONAS_SONG:
        case RG_EPONAS_SONG_PART1:
        case RG_EPONAS_SONG_PART2:
            return ITEM_SONG_EPONA;
        case RG_SARIAS_SONG:
        case RG_PROGRESSIVE_SARIAS_SONG:
        case RG_SARIAS_SONG_PART1:
        case RG_SARIAS_SONG_PART2:
            return ITEM_SONG_SARIA;
        case RG_SUNS_SONG:
        case RG_PROGRESSIVE_SUNS_SONG:
        case RG_SUNS_SONG_PART1:
        case RG_SUNS_SONG_PART2:
            return ITEM_SONG_SUN;
        case RG_SONG_OF_TIME:
        case RG_PROGRESSIVE_SONG_OF_TIME:
        case RG_SONG_OF_TIME_PART1:
        case RG_SONG_OF_TIME_PART2:
            return ITEM_SONG_TIME;
        case RG_SONG_OF_STORMS:
        case RG_PROGRESSIVE_SONG_OF_STORMS:
        case RG_SONG_OF_STORMS_PART1:
        case RG_SONG_OF_STORMS_PART2:
            return ITEM_SONG_STORMS;
        case RG_MINUET_OF_FOREST:
        case RG_PROGRESSIVE_MINUET_OF_FOREST:
        case RG_MINUET_OF_FOREST_PART1:
        case RG_MINUET_OF_FOREST_PART2:
            return ITEM_SONG_MINUET;
        case RG_BOLERO_OF_FIRE:
        case RG_PROGRESSIVE_BOLERO_OF_FIRE:
        case RG_BOLERO_OF_FIRE_PART1:
        case RG_BOLERO_OF_FIRE_PART2:
            return ITEM_SONG_BOLERO;
        case RG_SERENADE_OF_WATER:
        case RG_PROGRESSIVE_SERENADE_OF_WATER:
        case RG_SERENADE_OF_WATER_PART1:
        case RG_SERENADE_OF_WATER_PART2:
            return ITEM_SONG_SERENADE;
        case RG_REQUIEM_OF_SPIRIT:
        case RG_PROGRESSIVE_REQUIEM_OF_SPIRIT:
        case RG_REQUIEM_OF_SPIRIT_PART1:
        case RG_REQUIEM_OF_SPIRIT_PART2:
            return ITEM_SONG_REQUIEM;
        case RG_NOCTURNE_OF_SHADOW:
        case RG_PROGRESSIVE_NOCTURNE_OF_SHADOW:
        case RG_NOCTURNE_OF_SHADOW_PART1:
        case RG_NOCTURNE_OF_SHADOW_PART2:
            return ITEM_SONG_NOCTURNE;
        case RG_PRELUDE_OF_LIGHT:
        case RG_PROGRESSIVE_PRELUDE_OF_LIGHT:
        case RG_PRELUDE_OF_LIGHT_PART1:
        case RG_PRELUDE_OF_LIGHT_PART2:
            return ITEM_SONG_PRELUDE;
        default:
            return ITEM_NONE;
    }
}

static ItemID ResolveSongMessageIcon(RandomizerGet rg, RandomizerGet resolvedStage = RG_NONE) {
    ItemID icon = DirectSongIconForRandomizerGet(rg);
    if (icon != ITEM_NONE) {
        return icon;
    }
    if (resolvedStage != RG_NONE) {
        icon = DirectSongIconForRandomizerGet(resolvedStage);
        if (icon != ITEM_NONE) {
            return icon;
        }
    }
    icon = VanillaSongIconForSplitOrProgressive(rg);
    if (icon != ITEM_NONE) {
        return icon;
    }
    if (resolvedStage != RG_NONE) {
        const auto* stageDef = Rando::SplitSongs::GetSongDefFromPart(resolvedStage);
        if (stageDef != nullptr) {
            icon = VanillaItemIdForFullSong(stageDef->fullSong);
            if (icon != ITEM_NONE) {
                return icon;
            }
        }
    }
    // Never return ITEM_CUSTOM for split/progressive song messages; avoid static/noise tile path.
    return ITEM_SONG_LULLABY;
}

extern "C" u16 Message_GetRandoTextboxItemIconOverride(PlayState* play, u16 itemId) {
    (void)play;
    return itemId;
}

extern "C" u16 Randomizer_ResolveSongIconAtDrawTime(PlayState* play, u16 itemId) {
    (void)play;
    return itemId;
}

void BuildTriforcePieceMessage(CustomMessage& msg) {
    uint8_t current = gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected + 1;
    uint8_t required = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED) + 1;
    uint8_t remaining = required - current;
    float percentageCollected = (float)current / (float)required;

    if (percentageCollected <= 0.25) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. It's a start!",
                "Ein %yTriforce-Splitter%w! Du hast&%g[[current]]%w von %c[[required]]%w gefunden. Es ist ein&Anfang!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. C'est un début!" };
    } else if (percentageCollected <= 0.5) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Progress!",
                "Ein %yTriforce-Splitter%w! Du hast&%g[[current]]%w von %c[[required]]%w gefunden. Es geht voran!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. Ça avance!" };
    } else if (percentageCollected <= 0.75) {
        msg = { "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Over half-way&there!",
                "Ein %yTriforce-Splitter%w! Du hast&schon %g[[current]]%w von %c[[required]]%w gefunden. Schon&über "
                "die Hälfte!",
                "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste "
                "%c[[remaining]]%w à trouver. Il en reste un&peu moins que la moitié!" };
    } else if (percentageCollected < 1.0) {
        msg = {
            "You found a %yTriforce Piece%w!&%g[[current]]%w down, %c[[remaining]]%w to go. Almost done!",
            "Ein %yTriforce-Splitter%w! Du hast&schon %g[[current]]%w von %c[[required]]%w gefunden. Fast&geschafft!",
            "Vous trouvez un %yFragment de la&Triforce%w! Vous en avez %g[[current]]%w, il en&reste %c[[remaining]]%w "
            "à trouver. C'est presque&terminé!"
        };
    } else if (current == required) {
        msg = { "You completed the %yTriforce of&Courage%w! %gGG%w!",
                "Das %yTriforce des Mutes%w! Du hast&alle Splitter gefunden. %gGut gemacht%w!",
                "Vous avez complété la %yTriforce&du Courage%w! %gFélicitations%w!" };
    } else {
        msg = { "You found a spare %yTriforce Piece%w!&You only needed %c[[required]]%w, but you have %g[[current]]%w!",
                "Ein übriger %yTriforce-Splitter%w! Du&hast nun %g[[current]]%w von %c[[required]]%w nötigen gefunden.",
                "Vous avez trouvé un %yFragment de&Triforce%w en plus! Vous n'aviez besoin&que de %c[[required]]%w, "
                "mais vous en avez %g[[current]]%w en&tout!" };
    }
    msg.Replace("[[current]]", std::to_string(current));
    msg.Replace("[[remaining]]", std::to_string(remaining));
    msg.Replace("[[required]]", std::to_string(required));
    msg.Format(ITEM_CUSTOM);
}

void BuildCustomItemMessage(Player* player, CustomMessage& msg) {
    int16_t rgid;
    msg = CustomMessage("You found [[article]][[color]][[name]]%w!",
                        "Du erhältst [[article]][[color]][[name]]%w gefunden!",
                        "Vous avez trouvé [[article]][[color]][[name]]%w!", TEXTBOX_TYPE_BLUE);
    const GetItemEntry gift = ResolveGiftGetItemEntry(player);
    if (gift.modIndex == MOD_RANDOMIZER) {
        rgid = gift.getItemId;
    } else if (gift.objectId != OBJECT_INVALID) {
        rgid = gift.getItemId;
    } else {
        rgid = player->getItemId;
    }
    if (gift.modIndex == MOD_RANDOMIZER && rgid < 0) {
        rgid = (s16)-rgid;
    }
    const RandomizerGet rgEnum = static_cast<RandomizerGet>(rgid);
    // Song icons: ResolveSongMessageIcon maps RG_* → vanilla ITEM_SONG_*; Format() writes MESSAGE_ITEM_ICON bytes only.
    // Decode loads icon strictly from that buffer (same contract as non-song items using GetGIEntry()->itemId).
    if (IS_RANDO && Rando::SplitSongs::IsProgressiveSong(rgEnum)) {
        RandomizerGet stage = Rando::SplitSongs::ResolveProgressiveSongStage(rgEnum);
        if (stage == RG_NONE) {
            const Rando::SplitSongDef* def = Rando::SplitSongs::GetSongDefFromProgressive(rgEnum);
            if (def != nullptr) {
                stage = def->part1;
            }
        }
        const ItemID iconFallback = ResolveSongMessageIcon(rgEnum, stage);
        if (stage != RG_NONE) {
            const auto& nm = Rando::StaticData::RetrieveItem(stage).GetName();
            CustomMessage getItemText(nm.GetEnglish(), nm.GetGerman(), nm.GetFrench(), TEXTBOX_TYPE_BLUE,
                                      TEXTBOX_POS_BOTTOM);
            getItemText.Format(iconFallback);
            msg = getItemText;
            return;
        }
        // Stage could not be resolved to a part yet; still show progressive name with a valid vanilla song icon.
        if (iconFallback != ITEM_NONE) {
            const auto& nm = Rando::StaticData::RetrieveItem(rgEnum).GetName();
            CustomMessage getItemText(nm.GetEnglish(), nm.GetGerman(), nm.GetFrench(), TEXTBOX_TYPE_BLUE,
                                      TEXTBOX_POS_BOTTOM);
            getItemText.Format(iconFallback);
            msg = getItemText;
            return;
        }
    }
    if (IS_RANDO && Rando::SplitSongs::IsSongPart(rgEnum)) {
        const auto& songPartItem = Rando::StaticData::RetrieveItem(rgEnum);
        const ItemID iconFallback = ResolveSongMessageIcon(rgEnum, rgEnum);
        const auto& nm = songPartItem.GetName();
        CustomMessage getItemText(nm.GetEnglish(), nm.GetGerman(), nm.GetFrench(), TEXTBOX_TYPE_BLUE,
                                  TEXTBOX_POS_BOTTOM);
        getItemText.Format(iconFallback);
        msg = getItemText;
        return;
    }
    auto& itemForMsg = Rando::StaticData::RetrieveItem(rgEnum);
    CustomMessage name = CustomMessage(itemForMsg.GetName(), TEXTBOX_TYPE_BLUE);
    CustomMessage article = CustomMessage(itemForMsg.GetArticle(), TEXTBOX_TYPE_BLUE);
    msg.Replace("[[article]]", article);
    msg.Replace("[[color]]", itemForMsg.GetColor());
    msg.Replace("[[name]]", name);
    if (itemForMsg.HasCustomIcon()) {
        msg.AutoFormat(ITEM_CUSTOM);
    } else {
        // Vanilla pause-menu item id: embed a real icon byte so load/draw use gItemIcons (not stale segment data).
        const uint16_t pauseIcon = itemForMsg.GetGIEntry()->itemId;
        if (pauseIcon < ITEM_CUSTOM && pauseIcon != ITEM_NONE) {
            msg.AutoFormat(static_cast<ItemID>(pauseIcon));
        } else {
            msg.AutoFormat();
        }
    }
}

/** @return true if an OTR icon path was copied into the textbox segment (skip vanilla ITEM_CUSTOM load). */
static bool LoadCustomItemIcon(bool displayAsEnglish) {
    return RefreshCustomItemIconForTextbox(displayAsEnglish, true);
}

static bool RefreshCustomItemIconForTextbox(bool displayAsEnglish, bool incrementDecodeState) {
    Player* player = GET_PLAYER(gPlayState);
    const char* customIcon = nullptr;
    CustomIconSize iconSize = ICON_SIZE_32;
    RandomizerGet rgid = RG_NONE;
    const GetItemEntry gift = ResolveGiftGetItemEntry(player);
    if (gift.modIndex == MOD_RANDOMIZER) {
        rgid = RandomizerGetFromPlayerItemEntry(player);
        if (rgid != RG_NONE) {
            customIcon = Rando::StaticData::RetrieveItem(rgid).GetCustomIcon();
            iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
        }
    } else if (gift.objectId != OBJECT_INVALID) {
        rgid = static_cast<RandomizerGet>(gift.getItemId);
        customIcon = Rando::StaticData::RetrieveItem(rgid).GetCustomIcon();
        iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
    }
    if (customIcon != nullptr) {
        g_LastDecodedCustomIconPath = customIcon;
        g_LastDecodedCustomIconSize = iconSize;
        ApplyCustomIconPathToTextbox(customIcon, iconSize, displayAsEnglish, incrementDecodeState);
        return true;
    }
    if (!incrementDecodeState && g_LastDecodedCustomIconPath != nullptr) {
        ApplyCustomIconPathToTextbox(g_LastDecodedCustomIconPath, g_LastDecodedCustomIconSize, displayAsEnglish, false);
        return true;
    }
    if (incrementDecodeState) {
        g_LastDecodedCustomIconPath = nullptr;
    }
    return false;
}

static void ApplyCustomIconPathToTextbox(const char* customIcon, CustomIconSize iconSize, bool displayAsEnglish,
                                         bool incrementDecodeState) {
    static int16_t sIconItem32XOffsets[] = { 74, 74, 74, 54 };
    static int16_t sIconItem24XOffsets[] = { 72, 72, 72, 50 };
    MessageContext* msgCtx = &gPlayState->msgCtx;
    uint8_t language = displayAsEnglish ? LANGUAGE_ENG : gSaveContext.language;
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
    if (incrementDecodeState) {
        msgCtx->msgBufPos++;
        msgCtx->choiceNum = 1;
    }
}

static void DrawTextboxItemIconFromSegment(Gfx** p) {
    Gfx* gfx = *p;
    MessageContext* msgCtx = &gPlayState->msgCtx;
    if (R_TEXTBOX_ICON_SIZE == 24) {
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
    const GetItemEntry giftEntry = ResolveGiftGetItemEntry(player);

    if (giftEntry.modIndex == MOD_RANDOMIZER && giftEntry.getItemId == RG_ICE_TRAP) {
        Rando::Traps::BuildIceTrapMessage(msg, giftEntry);
    } else if (giftEntry.modIndex == MOD_RANDOMIZER && giftEntry.getItemId == RG_TRIFORCE_PIECE) {
        BuildTriforcePieceMessage(msg);
    } else {
        BuildCustomItemMessage(player, msg);
    }
    *loadFromMessageTable = false;
    msg.LoadIntoFont();
}

void BuildMapMessage(uint16_t* textId, bool* loadFromMessageTable) {
    GetItemEntry itemEntry = GET_PLAYER(gPlayState)->getItemEntry;
    auto ctx = OTRGlobals::Instance->gRandoContext;
    CustomMessage msg =
        CustomMessage("You found the %g[[name]]%w! [[typeHint]]", "Du erhältst das %g[[name]]%w! [[typeHint]]",
                      "Vous ebtenez %g[[name]]%w! [[typeHint]]", TEXTBOX_TYPE_BLUE);
    int sceneNum;
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
    } else if (ResourceMgr_IsSceneMasterQuest(sceneNum)) {
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
    if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("LogTextboxItemIcons"), 0) != 0) {
        SPDLOG_INFO("[TextboxItemIcon] Logging is ON (menu: Dev Tools → General → Log textbox item icons). Watch for "
                    "decode + DRAW lines when the textbox parses an icon control code.");
    }
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