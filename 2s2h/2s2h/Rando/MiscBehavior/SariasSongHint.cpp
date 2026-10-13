#include "2s2h/CustomMessage/CustomMessage.h"
#include "MiscBehavior.h"
#include "2s2h/ShipUtils.h"
#include <algorithm>
#include <set>
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/SaveManager/SaveManager.h"
#include <libultraship/libultraship.h>
#include <libultraship/bridge/consolevariablebridge.h>

extern "C" {
#include <variables.h>
#include <z64ocarina.h>
s32 Message_ShouldAdvanceSilent(PlayState* play);
extern s16 sOcarinaSongFanfares[17];
extern s16 sLastPlayedSong;
}

std::vector<RandoItemId> Rando::GetDefaultSariaPriorityItems() {
    return {
        RI_BOW,       RI_HOOKSHOT,          RI_MASK_BLAST,  RI_BOMB_BAG_20,  RI_MASK_DEKU, RI_MASK_GORON,
        RI_MASK_ZORA, RI_MASK_FIERCE_DEITY, RI_SONG_SONATA, RI_SONG_LULLABY, RI_SONG_NOVA, RI_SONG_SOARING,
    };
}

std::vector<RandoItemId> Rando::GetSariaPriorityItemsFromSpoiler(nlohmann::json& spoiler) {
    auto priorityItemsStrings = spoiler["sariaPriorityItems"].get<std::vector<std::string>>();
    std::vector<RandoItemId> priorityItems;

    for (auto& itemName : priorityItemsStrings) {
        auto randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            priorityItems.push_back(randoItemId);
        }
    }

    return priorityItems;
}

void Rando::SetSariaPriorityItemsInSpoiler(nlohmann::json& spoiler, std::vector<RandoItemId>& priorityItems) {
    std::vector<std::string> priorityItemsJson;
    for (auto& randoItemId : priorityItems) {
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            priorityItemsJson.push_back(Rando::StaticData::Items[randoItemId].spoilerName);
        }
    }
    spoiler["sariaPriorityItems"] = priorityItemsJson;
}

std::vector<RandoItemId> Rando::GetSariaPriorityItemsFromSave(RandoSaveInfo& randoSaveInfo) {
    std::vector<RandoItemId> priorityItems;

    for (int i = 0; i < ARRAY_COUNT(randoSaveInfo.sariaPriorityItems); i++) {
        if (randoSaveInfo.sariaPriorityItems[i] > RI_UNKNOWN && randoSaveInfo.sariaPriorityItems[i] < RI_MAX) {
            priorityItems.push_back((RandoItemId)randoSaveInfo.sariaPriorityItems[i]);
        }
    }

    return priorityItems;
}

void Rando::SetSariaPriorityItemsInSave(RandoSaveInfo& randoSaveInfo, std::vector<RandoItemId>& priorityItems) {
    memset(&randoSaveInfo.sariaPriorityItems, 0, sizeof(randoSaveInfo.sariaPriorityItems));

    size_t index = 0;
    for (auto& randoItemId : priorityItems) {
        if (index >= ARRAY_COUNT(randoSaveInfo.sariaPriorityItems)) {
            break;
        }
        randoSaveInfo.sariaPriorityItems[index++] = randoItemId;
    }
}

std::vector<RandoItemId> Rando::GetSariaPriorityItemsFromConfig() {
    auto allConfig = Ship::Context::GetRawInstance()->GetConfig()->GetNestedJson();
    std::vector<RandoItemId> priorityItems = Rando::GetDefaultSariaPriorityItems();

    if (allConfig.find("CVars") != allConfig.end() && allConfig["CVars"].is_object() &&
        allConfig["CVars"].find("gRando") != allConfig["CVars"].end() && allConfig["CVars"]["gRando"].is_object() &&
        allConfig["CVars"]["gRando"].find("SariaPriorityItems") != allConfig["CVars"]["gRando"].end()) {

        if (allConfig["CVars"]["gRando"]["SariaPriorityItems"].is_array()) {
            priorityItems.clear();

            auto priorityItemsStrings =
                allConfig["CVars"]["gRando"]["SariaPriorityItems"].get<std::vector<std::string>>();
            for (auto& itemName : priorityItemsStrings) {
                auto randoItemId = Rando::StaticData::GetItemIdFromName(itemName.c_str());
                if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
                    priorityItems.push_back(randoItemId);
                }
            }
        } else if (allConfig["CVars"]["gRando"]["SariaPriorityItems"].is_string()) {
            CVarClear("gRando.SariaPriorityItems");
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        } else if (allConfig["CVars"]["gRando"]["SariaPriorityItems"].is_null()) {
            priorityItems.clear();
        }
    }

    return priorityItems;
}

void Rando::SetSariaPriorityItemsInConfig(std::vector<RandoItemId>& priorityItems) {
    auto priorityItemsJson = nlohmann::json::array();
    for (auto& randoItemId : priorityItems) {
        if (randoItemId > RI_UNKNOWN && randoItemId < RI_MAX) {
            priorityItemsJson.push_back(Rando::StaticData::Items[randoItemId].spoilerName);
        }
    }
    // SetBlock() already persists to disk internally - no separate Save() call needed here.
    Ship::Context::GetRawInstance()->GetConfig()->SetBlock("CVars.gRando.SariaPriorityItems", priorityItemsJson);
}

static bool IsExcludedFromSariaPriorityItemCandidates(RandoItemId randoItemId) {
    switch (randoItemId) {
        case RI_PROGRESSIVE_SWORD:
        case RI_PROGRESSIVE_BOW:
        case RI_PROGRESSIVE_BOMB_BAG:
        case RI_PROGRESSIVE_MAGIC:
        case RI_PROGRESSIVE_WALLET:
        case RI_PROGRESSIVE_LULLABY:
        case RI_TIME_PROGRESSIVE:
        case RI_TRIFORCE_PIECE_PREVIOUS:
            return true;
        default:
            return false;
    }
}

// List of items for the UI
std::vector<RandoItemId> Rando::GetSariaPriorityItemCandidates() {
    static const std::vector<RandoItemId> candidates = [] {
        std::vector<RandoItemId> result;
        for (auto& [randoItemId, randoStaticItem] : Rando::StaticData::Items) {
            if ((randoStaticItem.randoItemType == RITYPE_MAJOR || randoStaticItem.randoItemType == RITYPE_MASK) &&
                !IsExcludedFromSariaPriorityItemCandidates(randoItemId)) {
                result.push_back(randoItemId);
            }
        }
        return result;
    }();
    return candidates;
}

static int playedSariasSongState = 0;

RandoCheckId GetProgressiveCheckInLogic() {
    std::vector<RandoItemId> priorityItems = Rando::GetSariaPriorityItemsFromSave(gSaveContext.save.shipSaveInfo.rando);

    std::unordered_map<RandoRegionId, Rando::Logic::RegionTimeState> regionTimeStates =
        Rando::Logic::InitializeRegionTimeStates(RR_MAX);
    std::set<RandoRegionId> reachableRegions = {};
    // Get connected entrances from starting & warp points
    Rando::Logic::FindReachableRegions(RR_MAX, reachableRegions, regionTimeStates);
    // Get connected regions from current entrance (TODO: Make this optional)
    Rando::Logic::FindReachableRegions(Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
                                       reachableRegions, regionTimeStates);

    std::unordered_map<RandoItemId, RandoCheckId> priorityItemChecks = {};
    std::vector<RandoCheckId> otherChecks = {};

    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            if (accessLogicFunc.first() && RANDO_SAVE_CHECKS[randoCheckId].shuffled &&
                !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
                RandoItemId itemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
                auto type = Rando::StaticData::Items[itemId].randoItemType;

                if (std::find(priorityItems.begin(), priorityItems.end(), itemId) != priorityItems.end()) {
                    priorityItemChecks.try_emplace(itemId, randoCheckId);
                } else if (type == RITYPE_MAJOR || type == RITYPE_MASK) {
                    otherChecks.push_back(randoCheckId);
                }
            }
        }
    }

    // Walk the user-ordered priority list and return the check for the first entry that's actually available
    // right now.
    for (RandoItemId priorityItemId : priorityItems) {
        auto priorityCheck = priorityItemChecks.find(priorityItemId);
        if (priorityCheck != priorityItemChecks.end()) {
            return priorityCheck->second;
        }
    }

    // None of the priority items are currently available; fall back to a random reachable major item/mask check.
    return otherChecks.empty() ? RC_UNKNOWN : otherChecks[Ship_Random(0, otherChecks.size())];
}

void Rando::MiscBehavior::SariasSongHint() {
    bool shouldRegister = IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONG_SARIA];

    // Fix vanilla issue where saria's song plays the majoras lair fanfare
    if (shouldRegister) {
        sOcarinaSongFanfares[OCARINA_SONG_SARIAS] = NA_BGM_SARIAS_SONG;
    } else {
        sOcarinaSongFanfares[OCARINA_SONG_SARIAS] = NA_BGM_MAJORAS_LAIR;
    }

    COND_VB_SHOULD(VB_SONG_AVAILABLE_TO_PLAY, shouldRegister, {
        uint8_t* songIndex = va_arg(args, uint8_t*);
        if (*songIndex == OCARINA_SONG_SARIAS && *should) {
            *should = gSaveContext.save.shipSaveInfo.rando.sariaHintsAvailable > 0;
        }
    });

    COND_VB_SHOULD(VB_MSG_CAPTURE_MSGMODE_TEXT_DONE, shouldRegister, {
        if (playedSariasSongState && gPlayState->msgCtx.ocarinaMode == OCARINA_MODE_PROCESS_RESTRICTED_SONG) {
            *should = true;

            Input* input = CONTROLLER1(&gPlayState->state);

            if (playedSariasSongState == 1) {
                // Check BTN_A directly instead of Message_ShouldAdvanceSilent(): its two/three-choice branch
                // already reduces to exactly this (a fresh A press), but only once msgCtx->textboxEndType has
                // actually been finalized to TEXTBOX_ENDTYPE_TWO_CHOICE. Until then it falls through to the
                // plain-textbox branch instead, which (with Fast Text on) treats a still-held B as an advance -
                // letting a button held from confirming the "call out" prompt moments earlier auto-confirm
                // this choice before the player has seen it, always reading choiceIndex's default of 0 ("Yes").
                if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    if (gPlayState->msgCtx.choiceIndex == 0) {
                        playedSariasSongState = 2;
                        Audio_PlaySfx(NA_SE_SY_DECIDE);
                        Message_ContinueTextbox(gPlayState, 0x1B95);
                    } else {
                        playedSariasSongState = 0;
                        Audio_PlaySfx(NA_SE_SY_DECIDE);
                        Message_CloseTextbox(gPlayState);
                        gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
                    }
                }
            } else {
                // playedSariasSongState == 2: the hint (or "no response") text is up. Require a genuine fresh
                // press here instead of Message_ShouldAdvanceSilent(), which treats a held B as an advance when
                // the Fast Text enhancement is on. Otherwise, still holding B from confirming the prompt above
                // closes this text the instant it finishes drawing, before it can be read.
                if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_B) ||
                    CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                    playedSariasSongState = 0;
                    Audio_PlaySfx(NA_SE_SY_DECIDE);
                    Message_CloseTextbox(gPlayState);
                    gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_END;
                }
            }
        }
    });

    COND_VB_SHOULD(VB_MSG_CAPTURE_MSGMODE_TEXT_CLOSING_OCARINA_ACTION, shouldRegister, {
        MessageContext* msgCtx = &gPlayState->msgCtx;

        if (sLastPlayedSong == OCARINA_SONG_SARIAS) {
            *should = true;
            playedSariasSongState = 1;
            Message_StartTextbox(gPlayState, 0x1B95, NULL);
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_PROCESS_RESTRICTED_SONG;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x1B95, shouldRegister, [](u16* textId, bool* loadFromMessageTable) {
        CustomMessage::Entry entry;
        if (playedSariasSongState == 1) {
            entry.nextMessageID = 0x1B95;
            entry.msg = "Call out to an old friend for help? You can only do this once.\x02\x11\xC2Yes\x11No";
        } else if (playedSariasSongState == 2) {
            RandoCheckId randoCheckId = GetProgressiveCheckInLogic();
            entry.textboxType = TEXTBOX_TYPE_2;

            if (randoCheckId == RC_UNKNOWN) {
                entry.msg = "... You call out but there is no response ...";
            } else {
                entry.msg = "%g... Link? Is that you? Where have you been..?! Zelda has been worried sick about you! "
                            "... You need my help?\x10 Alright but just this once. Search %y{{location}}%g, you will "
                            "find what you need. Hurry now!";
                CustomMessage::Replace(&entry.msg, "{{location}}",
                                       Rando::StaticData::GetLocationNameForHint(randoCheckId, true));
                Rando::RemoveItem(RI_SONG_SARIA);
                SaveManager_PersistSariaHintsAvailable();
            }
        } else if (playedSariasSongState == 0) {
            return;
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
