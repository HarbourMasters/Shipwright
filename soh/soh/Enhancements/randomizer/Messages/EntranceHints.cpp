#include "soh/Enhancements/randomizer/entrance.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include <soh/OTRGlobals.h>

extern "C" {
#include <variables.h>
extern PlayState* gPlayState;
}

// clang-format off
// clang-format attempts to format this strangely for some reason, so temporarily turning it off
#define ENTRANCES_SHUFFLED                                                                     \
    IS_RANDO &&                                                                                \
    OTRGlobals::Instance->gRandoContext->GetOption(RSK_SHUFFLE_ENTRANCES).Is(RO_GENERIC_ON) && \
    (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("EntrancesOnSigns"), 0) == 1)
// clang-format on

void BuildEntranceHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    auto ctx = OTRGlobals::Instance->gRandoContext;
    s16 entrance = -1;
    switch (*textId) {
        case TEXT_WATERFALL:
            entrance = ENTR_ZORAS_DOMAIN_ENTRANCE;
            break;
        case TEXT_OUTSIDE_FISHING_POND:
            entrance = ENTR_FISHING_POND_0;
            break;
        case TEXT_HF_SIGN:
            if (gPlayState->sceneNum == SCENE_KAKARIKO_VILLAGE) {
                entrance = ENTR_HYRULE_FIELD_STAIRS_EXIT;
            } else if (gPlayState->sceneNum == SCENE_GERUDO_VALLEY) {
                entrance = ENTR_HYRULE_FIELD_ROCKY_PATH;
            } else if (gPlayState->sceneNum == SCENE_LAKE_HYLIA) {
                entrance = ENTR_HYRULE_FIELD_FENCE_EXIT;
            }
            break;
        case TEXT_HC_GREAT_FAIRY_SIGN:
            entrance = ENTR_GREAT_FAIRYS_FOUNTAIN_SPELLS_DINS_HC;
            break;
        case TEXT_DMT_KAK_SIGN:
            if (gPlayState->sceneNum == SCENE_HYRULE_FIELD) {
                entrance = ENTR_KAKARIKO_VILLAGE_FRONT_GATE;
            } else {
                entrance = ENTR_KAKARIKO_VILLAGE_GUARD_GATE;
            }
            break;
        case TEXT_KAK_TO_GY_SIGN:
            entrance = ENTR_GRAVEYARD_ENTRANCE;
            break;
        case TEXT_KAK_WELL_SIGN:
            entrance = ENTR_BOTTOM_OF_THE_WELL_ENTRANCE;
            break;
        case TEXT_KAK_DMT_SIGN:
            entrance = ENTR_DEATH_MOUNTAIN_TRAIL_BOTTOM_EXIT;
            break;
        case TEXT_DMT_SIGN:
            entrance = ENTR_GROTTOS_13;
            break;
        case TEXT_DMT_DC_SIGN:
            entrance = ENTR_DODONGOS_CAVERN_ENTRANCE;
            break;
        case TEXT_DMT_GC_SIGN:
            entrance = ENTR_GORON_CITY_UPPER_EXIT;
            break;
        case TEXT_GC_SIGN:
            if (gPlayState->sceneNum == SCENE_DEATH_MOUNTAIN_TRAIL) {
                entrance = ENTR_GORON_CITY_UPPER_EXIT;
            } else {
                entrance = ENTR_GORON_CITY_DARUNIA_ROOM_EXIT;
            }
            break;
        case TEXT_DMT_DMC_SIGN:
            entrance = ENTR_DEATH_MOUNTAIN_CRATER_UPPER_EXIT;
            break;
        case TEXT_DMT_SUMMIT_SIGN:
            entrance = ENTR_GREAT_FAIRYS_FOUNTAIN_MAGIC_DMT;
            break;
        case TEXT_HF_ZR_SIGN:
            entrance = ENTR_ZORAS_RIVER_WEST_EXIT;
            break;
        case TEXT_GF_GTG_SIGN:
            entrance = ENTR_GERUDO_TRAINING_GROUND_ENTRANCE;
            break;
        case TEXT_KF_SHOP_SIGN:
            entrance = ENTR_KOKIRI_SHOP_0;
            break;
        case TEXT_LINKS_HOUSE_SIGN:
            entrance = ENTR_LINKS_HOUSE_1;
            break;
        case TEXT_KOKIRI_EXIT_SIGN:
            entrance = ENTR_LOST_WOODS_BRIDGE_EAST_EXIT;
            break;
        case TEXT_ZD_SIGN:
            if (gPlayState->sceneNum == SCENE_ZORAS_DOMAIN) {
                entrance = ENTR_ZORAS_RIVER_WATERFALL_EXIT;
            } else {
                entrance = ENTR_ZORAS_DOMAIN_KING_ZORA_EXIT;
            }
            break;
        case TEXT_ZF_JABU_SIGN:
            entrance = ENTR_JABU_JABU_ENTRANCE;
            break;
        case TEXT_KF_LW_SIGN:
            entrance = ENTR_LOST_WOODS_SOUTH_EXIT;
            break;
        case TEXT_HF_LON_LON_SIGN:
            entrance = ENTR_LON_LON_RANCH_ENTRANCE;
            break;
        case TEXT_LA_SIGN:
            entrance = ENTR_LAKE_HYLIA_NORTH_EXIT;
            break;
        case TEXT_LA_LAB_SIGN:
            entrance = ENTR_LAKESIDE_LABORATORY_0;
            break;
        case TEXT_GV_SIGN:
            if (gPlayState->sceneNum == SCENE_HYRULE_FIELD) {
                entrance = ENTR_GERUDO_VALLEY_EAST_EXIT;
            } else {
                entrance = ENTR_GERUDO_VALLEY_WEST_EXIT;
            }
            break;
        case TEXT_ZD_SHOP_SIGN:
            entrance = ENTR_ZORA_SHOP_0;
            break;
        case TEXT_OUTSIDE_KOKIRI_SIGN:
            entrance = ENTR_MARKET_ENTRANCE_NEAR_GUARD_EXIT;
            break;
        case TEXT_OUTSIDE_MARKET_SIGN:
            entrance = ENTR_LON_LON_RANCH_ENTRANCE;
            break;
        case TEXT_MIDO_HOUSE_SIGN:
            entrance = ENTR_MIDOS_HOUSE_0;
            break;
        case TEXT_KNOW_IT_ALL_HOUSE_SIGN:
            entrance = ENTR_KNOW_IT_ALL_BROS_HOUSE_0;
            break;
        case TEXT_TWINS_HOUSE_SIGN:
            entrance = ENTR_TWINS_HOUSE_0;
            break;
        case TEXT_SARIAS_HOUSE_SIGN:
            entrance = ENTR_SARIAS_HOUSE_0;
            break;
        case TEXT_NO_DIVING_SIGN:
            entrance = ENTR_LAKE_HYLIA_RIVER_EXIT;
            break;
    }
    if (entrance != -1) {
        auto entranceCtx = ctx->GetEntranceShuffler();
        for (size_t i = 0; i < ENTRANCE_OVERRIDES_MAX_COUNT; i++) {
            if (Entrance_EntranceIsNull(&entranceCtx->entranceOverrides[i])) {
                *loadFromMessageTable = true;
                return;
            }
            if (entranceCtx->entranceOverrides[i].index == entrance) {
                s16 overrideIndex = entranceCtx->entranceOverrides[i].override;
                Entrance_SetEntranceDiscovered(entrance, false);
                auto data = EntranceTracker::GetEntranceData(overrideIndex);
                CustomMessage msg = CustomMessage("[[name]]");
                msg.Replace("[[name]]", data->destination);
                msg.SetTextBoxType(TEXTBOX_TYPE_WOODEN);
                msg.SetTextBoxPosition(TEXTBOX_POS_BOTTOM);
                *loadFromMessageTable = false;
                msg.AutoFormat();
                msg.LoadIntoFont();
                return;
            }
        }
    }
}

void RegisterEntranceHintMessages() {
    COND_HOOK(OnOpenText, ENTRANCES_SHUFFLED, BuildEntranceHintMessage);
}

static RegisterShipInitFunc initFunc(RegisterEntranceHintMessages, { "IS_RANDO" });