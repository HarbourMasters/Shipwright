/**
 * This file handles custom messages relating to Items,
 * such as Get Item messages for non-vanilla items,
 * Vanilla/MQ hints when collecting Maps, Ice Trap messages,
 * etc.
 */
#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/Traps.h"
#include "soh/Enhancements/randomizer/item.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/ShipInit.hpp"
#include <soh/ResourceManagerHelpers.h>

#include <cstring>
#include <cstdarg>

extern "C" {
#include <variables.h>
#include <macros.h>
#include "z64item.h"
#include "textures/icon_item_static/icon_item_static.h"
extern PlayState* gPlayState;
}

// Progressive/part song pickups use TEXT_RANDOMIZER_CUSTOM_ITEM with gSongNoteTex (16x24 IA8).
static bool sTextboxSongNoteIcon = false;
static u8 sTextboxSongNotePrimR = 255;
static u8 sTextboxSongNotePrimG = 255;
static u8 sTextboxSongNotePrimB = 255;

extern "C" bool Rando_TextboxSongNoteIconActive(void) {
    return sTextboxSongNoteIcon;
}

static bool IsSongNoteTexPath(const char* path) {
    return path != nullptr && strstr(path, "gSongNoteTex") != nullptr;
}

static const SongData* FindSongDataForGet(RandomizerGet rg) {
    auto it = Rando::StaticData::songData.find(rg);
    if (it != Rando::StaticData::songData.end()) {
        return &it->second;
    }
    for (const auto& entry : Rando::StaticData::songData) {
        if (entry.second.part == rg || entry.second.realSong == rg) {
            return &entry.second;
        }
    }
    return nullptr;
}

static int GetKaleidoSongNoteColorIndex(QuestItem quest) {
    switch (quest) {
        case QUEST_SONG_MINUET:
            return 0;
        case QUEST_SONG_BOLERO:
            return 1;
        case QUEST_SONG_SERENADE:
            return 2;
        case QUEST_SONG_REQUIEM:
            return 3;
        case QUEST_SONG_NOCTURNE:
            return 4;
        case QUEST_SONG_PRELUDE:
            return 5;
        case QUEST_SONG_LULLABY:
            return 6;
        case QUEST_SONG_EPONA:
            return 7;
        case QUEST_SONG_SARIA:
            return 8;
        case QUEST_SONG_SUN:
            return 9;
        case QUEST_SONG_TIME:
            return 10;
        case QUEST_SONG_STORMS:
            return 11;
        default:
            return -1;
    }
}

// Quest-status song note tints (z_kaleido_collect.c), indexed like the pause quest songs.
static void SetTextboxSongNotePrimColor(const SongData* song) {
    static constexpr s16 kR[] = { 150, 255, 100, 255, 255, 255, 255, 255, 255, 255, 255, 255 };
    static constexpr s16 kG[] = { 255, 80, 150, 160, 100, 240, 255, 255, 255, 255, 255, 255 };
    static constexpr s16 kB[] = { 100, 40, 255, 0, 255, 100, 255, 255, 255, 255, 255, 255 };

    const int idx = GetKaleidoSongNoteColorIndex(song->quest);
    if (idx < 0) {
        sTextboxSongNotePrimR = sTextboxSongNotePrimG = sTextboxSongNotePrimB = 255;
        return;
    }

    sTextboxSongNotePrimR = static_cast<u8>(kR[idx]);
    sTextboxSongNotePrimG = static_cast<u8>(kG[idx]);
    sTextboxSongNotePrimB = static_cast<u8>(kB[idx]);
}

static const char* GetSongTextboxIconPath(Player* player) {
    if (!IS_RANDO || player->getItemEntry.modIndex != MOD_RANDOMIZER) {
        return nullptr;
    }

    const RandomizerGet rg = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
    const SongData* song = FindSongDataForGet(rg);
    if (song == nullptr) {
        return nullptr;
    }

    const ItemID songItemId = static_cast<ItemID>(Rando::StaticData::RetrieveItem(song->realSong).GetItemID());
    return static_cast<const char*>(gItemIcons[songItemId]);
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
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        rgid = player->getItemEntry.getItemId;
    } else {
        rgid = player->getItemId;
    }
    const RandomizerGet rg = static_cast<RandomizerGet>(rgid);
    CustomMessage name = CustomMessage(Rando::StaticData::RetrieveItem(rg).GetName(), TEXTBOX_TYPE_BLUE);
    CustomMessage article = CustomMessage(Rando::StaticData::RetrieveItem(rg).GetArticle(), TEXTBOX_TYPE_BLUE);
    msg.Replace("[[article]]", article);
    msg.Replace("[[color]]", Rando::StaticData::RetrieveItem(rg).GetColor());
    msg.Replace("[[name]]", name);
    // Song notes are IA8 16x24; force ITEM_CUSTOM so Load/DrawCustomItemIcon use the song path.
    if (Rando::StaticData::RetrieveItem(rg).HasCustomIcon() || FindSongDataForGet(rg) != nullptr) {
        msg.AutoFormat(ITEM_CUSTOM);
    } else {
        msg.AutoFormat();
    }
}

void LoadCustomItemIcon(bool displayAsEnglish) {
    Player* player = GET_PLAYER(gPlayState);
    const char* customIcon = nullptr;
    CustomIconSize iconSize = ICON_SIZE_32;
    sTextboxSongNoteIcon = false;

    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        RandomizerGet rgid = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
        customIcon = Rando::StaticData::RetrieveItem(rgid).GetCustomIcon();
        iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
    }
    if (customIcon == nullptr) {
        customIcon = GetSongTextboxIconPath(player);
        if (customIcon != nullptr) {
            iconSize = ICON_SIZE_24;
        }
    }

    if (customIcon != nullptr) {
        static int16_t sIconItem32XOffsets[] = { 74, 74, 74, 54 };
        static int16_t sIconItem24XOffsets[] = { 72, 72, 72, 50 };
        MessageContext* msgCtx = &gPlayState->msgCtx;
        uint8_t language = displayAsEnglish ? LANGUAGE_ENG : gSaveContext.language;
        if (IsSongNoteTexPath(customIcon)) {
            sTextboxSongNoteIcon = true;
            const RandomizerGet rg = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
            const SongData* song = FindSongDataForGet(rg);
            if (song != nullptr) {
                SetTextboxSongNotePrimColor(song);
            } else {
                sTextboxSongNotePrimR = sTextboxSongNotePrimG = sTextboxSongNotePrimB = 255;
            }
            R_TEXTBOX_ICON_XPOS = R_TEXT_INIT_XPOS - sIconItem24XOffsets[language];
            R_TEXTBOX_ICON_YPOS = (R_TEXTBOX_Y + 10) + 10;
            R_TEXTBOX_ICON_SIZE = 24;
        } else if (iconSize == ICON_SIZE_32) {
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
    if (sTextboxSongNoteIcon) {
        gDPSetPrimColor(gfx++, 0, 0, sTextboxSongNotePrimR, sTextboxSongNotePrimG, sTextboxSongNotePrimB,
                        msgCtx->textColorAlpha);
        gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_IA,
                            G_IM_SIZ_8b, 16, 24, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gfx++, (msgCtx->textPosX + R_TEXTBOX_ICON_XPOS) << 2, R_TEXTBOX_ICON_YPOS << 2,
                            (msgCtx->textPosX + R_TEXTBOX_ICON_XPOS + 16) << 2, (R_TEXTBOX_ICON_YPOS + 24) << 2,
                            G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        gDPPipeSync(gfx++);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);
    } else {
        Player* player = GET_PLAYER(gPlayState);
        CustomIconSize iconSize = ICON_SIZE_32;
        if (player->getItemEntry.objectId != OBJECT_INVALID) {
            RandomizerGet rgid = static_cast<RandomizerGet>(player->getItemEntry.getItemId);
            iconSize = Rando::StaticData::RetrieveItem(rgid).GetCustomIconSize();
        }
        if (iconSize == ICON_SIZE_24) {
            gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA,
                                G_IM_SIZ_32b, 24, 24, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock(gfx++, (uintptr_t)msgCtx->textboxSegment + MESSAGE_STATIC_TEX_SIZE, G_IM_FMT_RGBA,
                                G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        }
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