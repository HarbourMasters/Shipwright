#include "ImGuiUtils.h"
#include <Context.h>
#include "assets/soh_assets.h"

std::map<uint32_t, ItemMapEntry> itemMapping = {
    ITEM_MAP_ENTRY(ITEM_STICK),
    ITEM_MAP_ENTRY(ITEM_NUT),
    ITEM_MAP_ENTRY(ITEM_BOMB),
    ITEM_MAP_ENTRY(ITEM_BOW),
    ITEM_MAP_ENTRY(ITEM_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_DINS_FIRE),
    ITEM_MAP_ENTRY(ITEM_SLINGSHOT),
    ITEM_MAP_ENTRY(ITEM_OCARINA_FAIRY),
    ITEM_MAP_ENTRY(ITEM_OCARINA_TIME),
    ITEM_MAP_ENTRY(ITEM_BOMBCHU),
    ITEM_MAP_ENTRY(ITEM_HOOKSHOT),
    ITEM_MAP_ENTRY(ITEM_LONGSHOT),
    ITEM_MAP_ENTRY(ITEM_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_FARORES_WIND),
    ITEM_MAP_ENTRY(ITEM_BOOMERANG),
    ITEM_MAP_ENTRY(ITEM_LENS),
    ITEM_MAP_ENTRY(ITEM_BEAN),
    ITEM_MAP_ENTRY(ITEM_HAMMER),
    ITEM_MAP_ENTRY(ITEM_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_NAYRUS_LOVE),
    ITEM_MAP_ENTRY(ITEM_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_POTION_RED),
    ITEM_MAP_ENTRY(ITEM_POTION_GREEN),
    ITEM_MAP_ENTRY(ITEM_POTION_BLUE),
    ITEM_MAP_ENTRY(ITEM_FAIRY),
    ITEM_MAP_ENTRY(ITEM_FISH),
    ITEM_MAP_ENTRY(ITEM_MILK_BOTTLE),
    ITEM_MAP_ENTRY(ITEM_LETTER_RUTO),
    ITEM_MAP_ENTRY(ITEM_BLUE_FIRE),
    ITEM_MAP_ENTRY(ITEM_BUG),
    ITEM_MAP_ENTRY(ITEM_BIG_POE),
    ITEM_MAP_ENTRY(ITEM_MILK_HALF),
    ITEM_MAP_ENTRY(ITEM_POE),
    ITEM_MAP_ENTRY(ITEM_WEIRD_EGG),
    ITEM_MAP_ENTRY(ITEM_CHICKEN),
    ITEM_MAP_ENTRY(ITEM_LETTER_ZELDA),
    ITEM_MAP_ENTRY(ITEM_MASK_KEATON),
    ITEM_MAP_ENTRY(ITEM_MASK_SKULL),
    ITEM_MAP_ENTRY(ITEM_MASK_SPOOKY),
    ITEM_MAP_ENTRY(ITEM_MASK_BUNNY),
    ITEM_MAP_ENTRY(ITEM_MASK_GORON),
    ITEM_MAP_ENTRY(ITEM_MASK_ZORA),
    ITEM_MAP_ENTRY(ITEM_MASK_GERUDO),
    ITEM_MAP_ENTRY(ITEM_MASK_TRUTH),
    ITEM_MAP_ENTRY(ITEM_SOLD_OUT),
    ITEM_MAP_ENTRY(ITEM_POCKET_EGG),
    ITEM_MAP_ENTRY(ITEM_POCKET_CUCCO),
    ITEM_MAP_ENTRY(ITEM_COJIRO),
    ITEM_MAP_ENTRY(ITEM_ODD_MUSHROOM),
    ITEM_MAP_ENTRY(ITEM_ODD_POTION),
    ITEM_MAP_ENTRY(ITEM_SAW),
    ITEM_MAP_ENTRY(ITEM_SWORD_BROKEN),
    ITEM_MAP_ENTRY(ITEM_PRESCRIPTION),
    ITEM_MAP_ENTRY(ITEM_FROG),
    ITEM_MAP_ENTRY(ITEM_EYEDROPS),
    ITEM_MAP_ENTRY(ITEM_CLAIM_CHECK),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_FIRE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_ICE),
    ITEM_MAP_ENTRY(ITEM_BOW_ARROW_LIGHT),
    ITEM_MAP_ENTRY(ITEM_SWORD_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_SWORD_MASTER),
    ITEM_MAP_ENTRY(ITEM_SWORD_BGS),
    ITEM_MAP_ENTRY(ITEM_SHIELD_DEKU),
    ITEM_MAP_ENTRY(ITEM_SHIELD_HYLIAN),
    ITEM_MAP_ENTRY(ITEM_SHIELD_MIRROR),
    ITEM_MAP_ENTRY(ITEM_TUNIC_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_TUNIC_GORON),
    ITEM_MAP_ENTRY(ITEM_TUNIC_ZORA),
    ITEM_MAP_ENTRY(ITEM_BOOTS_KOKIRI),
    ITEM_MAP_ENTRY(ITEM_BOOTS_IRON),
    ITEM_MAP_ENTRY(ITEM_BOOTS_HOVER),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BULLET_BAG_50),
    ITEM_MAP_ENTRY(ITEM_QUIVER_30),
    ITEM_MAP_ENTRY(ITEM_QUIVER_40),
    ITEM_MAP_ENTRY(ITEM_QUIVER_50),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_20),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_30),
    ITEM_MAP_ENTRY(ITEM_BOMB_BAG_40),
    ITEM_MAP_ENTRY(ITEM_BRACELET),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_SILVER),
    ITEM_MAP_ENTRY(ITEM_GAUNTLETS_GOLD),
    ITEM_MAP_ENTRY(ITEM_SCALE_SILVER),
    ITEM_MAP_ENTRY(ITEM_SCALE_GOLDEN),
    ITEM_MAP_ENTRY(ITEM_SWORD_KNIFE),
    ITEM_MAP_ENTRY(ITEM_WALLET_ADULT),
    ITEM_MAP_ENTRY(ITEM_WALLET_GIANT),
    ITEM_MAP_ENTRY(ITEM_SEEDS),
    ITEM_MAP_ENTRY(ITEM_FISHING_POLE),
    ITEM_MAP_ENTRY(ITEM_KEY_BOSS),
    ITEM_MAP_ENTRY(ITEM_COMPASS),
    ITEM_MAP_ENTRY(ITEM_DUNGEON_MAP),
    ITEM_MAP_ENTRY(ITEM_KEY_SMALL),
    ITEM_MAP_ENTRY(ITEM_HEART_CONTAINER),
    ITEM_MAP_ENTRY(ITEM_HEART_PIECE),
    ITEM_MAP_ENTRY(ITEM_MAGIC_SMALL),
    ITEM_MAP_ENTRY(ITEM_MAGIC_LARGE)
};

std::map<uint32_t, ItemMapEntry> gregMapping = {
    {ITEM_RUPEE_GREEN, {ITEM_RUPEE_GREEN, "ITEM_RUPEE_GREEN", "ITEM_RUPEE_GREEN_Faded", gRupeeCounterIconTex}}
};

std::map<uint32_t, ItemMapEntry> triforcePieceMapping = {
    {RG_TRIFORCE_PIECE, {RG_TRIFORCE_PIECE, "RG_TRIFORCE_PIECE", "RG_TRIFORCE_PIECE_Faded", gTriforcePieceTex}}
};

std::map<uint32_t, QuestMapEntry> questMapping = {
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FOREST, dgQuestIconMedallionForestTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_FIRE, dgQuestIconMedallionFireTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_WATER, dgQuestIconMedallionWaterTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SPIRIT, dgQuestIconMedallionSpiritTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_SHADOW, dgQuestIconMedallionShadowTex),
    QUEST_MAP_ENTRY(QUEST_MEDALLION_LIGHT, dgQuestIconMedallionLightTex),
    QUEST_MAP_ENTRY(QUEST_KOKIRI_EMERALD, dgQuestIconKokiriEmeraldTex),
    QUEST_MAP_ENTRY(QUEST_GORON_RUBY, dgQuestIconGoronRubyTex),
    QUEST_MAP_ENTRY(QUEST_ZORA_SAPPHIRE, dgQuestIconZoraSapphireTex),
    QUEST_MAP_ENTRY(QUEST_STONE_OF_AGONY, dgQuestIconStoneOfAgonyTex),
    QUEST_MAP_ENTRY(QUEST_GERUDO_CARD, dgQuestIconGerudosCardTex),
    QUEST_MAP_ENTRY(QUEST_SKULL_TOKEN, dgQuestIconGoldSkulltulaTex),
};

std::array<SongMapEntry, 12> songMapping = { {
    SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  224, 107, 255),
    SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 195, 60),
    SONG_MAP_ENTRY(QUEST_SONG_SARIA,    127, 255, 137),
    SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 60),
    SONG_MAP_ENTRY(QUEST_SONG_TIME,     119, 236, 255),
    SONG_MAP_ENTRY(QUEST_SONG_STORMS,   165, 165, 165),
    SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

std::array<SongMapEntry, 12> vanillaSongMapping = { {
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_LULLABY,  255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_EPONA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SARIA,    255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SUN,      255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_TIME,     255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_STORMS,   255, 255, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_MINUET,   150, 255, 100),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_BOLERO,   255, 80,  40),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_SERENADE, 100, 150, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_REQUIEM,  255, 160, 0),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_NOCTURNE, 255, 100, 255),
    VANILLA_SONG_MAP_ENTRY(QUEST_SONG_PRELUDE,  255, 240, 100),
} };

void RegisterImGuiItemIcons() {
    for (const auto& entry : itemMapping) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, ImVec4(1, 1, 1, 1));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }

    for (const auto& entry : gregMapping) {
        ImVec4 gregGreen = ImVec4(42.0f / 255.0f, 169.0f / 255.0f, 40.0f / 255.0f, 1.0f);
        ImVec4 gregFadedGreen = gregGreen;
        gregFadedGreen.w = 0.3f;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, gregGreen);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, gregFadedGreen);
    }

    for (const auto& entry : triforcePieceMapping) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, ImVec4(1, 1, 1, 1));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }

    for (const auto& entry : questMapping) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.name, entry.second.texturePath, ImVec4(1, 1, 1, 1));
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.second.nameFaded, entry.second.texturePath, ImVec4(1, 1, 1, 0.3f));
    }

    for (const auto& entry : songMapping) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.nameFaded, gSongNoteTex, fadedCol);
    }

    for (const auto& entry : vanillaSongMapping) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.name, gSongNoteTex, entry.color);
        ImVec4 fadedCol = entry.color;
        fadedCol.w = 0.3f;
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(entry.nameFaded, gSongNoteTex, fadedCol);
    }
}