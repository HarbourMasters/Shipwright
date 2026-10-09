#include "AudioEditor.h"

#include <map>
#include <set>
#include <string>
#include <libultraship/bridge/consolevariablebridge.h>
#include <functions.h>
#include <sequence.h>
#include "../../BenPort.h"
#include <ship/utils/StringHelper.h>
#include "../../BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenMenu.h"
#include "2s2h/BenGui/BenGui.hpp"
#include "AudioCollection.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <random>

extern "C" Vec3f gZeroVec3f;
extern "C" f32 gSfxDefaultFreqAndVolScale;
extern "C" s8 gSfxDefaultReverb;

using namespace UIWidgets;

#define CVAR_PREFIX_AUDIO "gAudioEditor"
#define CVAR_AUDIO(var) CVAR_PREFIX_AUDIO "." var

static WidgetInfo lowHpAlarm;
static WidgetInfo muteCarpenterSfx;
static WidgetInfo childGoronCry;
static WidgetInfo tatlCall;
static WidgetInfo enemyProx;
static WidgetInfo displaySeqName;
static WidgetInfo ovlDuration;
static WidgetInfo voicePitch;
static WidgetInfo voicePitchEnable;
static WidgetInfo randoMusicOnSceneChange;
static WidgetInfo randomAudioOnSeedGen;
static WidgetInfo useSongCategories;

namespace AudioPreview {

// Prevents race conditions with async audio engine updates.
#define AUDIO_PREVIEW_GRACE_FRAMES 30

extern "C" {
extern u8 sSeqFlags[];
#define SEQ_FLAG_FANFARE (1 << 1)
#define SEQ_FLAG_FANFARE_KAMARO (1 << 2)
}

static u16 sPreviewRestoreSeqId = NA_BGM_DISABLED;
static bool sIsBgmPreviewing = false;
static uint8_t sGraceFrames = 0;

void Update() {
    if (sGraceFrames > 0) {
        sGraceFrames--;
        return;
    }

    u16 playingSeqId = CVarGetInteger(CVAR_AUDIO("Playing"), 0);
    if (playingSeqId == 0) {
        return;
    }

    // Previews may run on players mismatched to their flags (e.g. fanfare songs on main BGM player).
    // Manually check both to avoid polluting engine code.
    u16 activeBgmId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    u16 activeFanfareId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_FANFARE);

    // Sequences < MAX_AUTHENTIC_SEQID use 8-bit IDs (masked with 0xFF),
    // custom sequences >= MAX_AUTHENTIC_SEQID use full 16-bit IDs
    u16 mask = (playingSeqId < MAX_AUTHENTIC_SEQID) ? 0xFF : 0xFFFF;
    u16 matchId = playingSeqId & mask;

    bool isPlaying = (activeBgmId != NA_BGM_DISABLED && (activeBgmId & mask) == matchId) ||
                     (activeFanfareId != NA_BGM_DISABLED && (activeFanfareId & mask) == matchId);

    if (!isPlaying) {
        CVarSetInteger(CVAR_AUDIO("Playing"), 0);
        sIsBgmPreviewing = false;
    }
}

void Stop() {
    u16 playingSeqId = CVarGetInteger(CVAR_AUDIO("Playing"), 0);
    if (playingSeqId == 0) {
        return;
    }

    if (sPreviewRestoreSeqId != NA_BGM_DISABLED) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, sPreviewRestoreSeqId + SEQ_FLAG_ASYNC);
        sPreviewRestoreSeqId = NA_BGM_DISABLED;
    } else if (sIsBgmPreviewing) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
    }
    sIsBgmPreviewing = false;

    // Ensure fanfares/ocarina sequences stop when ending a preview
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 10);

    CVarSetInteger(CVAR_AUDIO("Playing"), 0);
}

void Play(u16 sequenceId, SeqType sequenceType) {

    // Restore original BGM before switching to non-BGM preview (songs/fanfares/ocarina).
    // Prevents the preview BGM from becoming orphaned without a stop button.
    if (sPreviewRestoreSeqId != NA_BGM_DISABLED && !(sequenceType & (SEQ_BGM_WORLD | SEQ_BGM_EVENT | SEQ_BGM_BATTLE))) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, sPreviewRestoreSeqId + SEQ_FLAG_ASYNC);
        sPreviewRestoreSeqId = NA_BGM_DISABLED;
        sIsBgmPreviewing = false;
    }

    if (sequenceType & (SEQ_SFX | SEQ_VOICE)) {
        AudioSfx_PlaySfx(sequenceId, &gZeroVec3f, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultReverb);
    } else if (sequenceType & SEQ_INSTRUMENT) {
        AudioOcarina_SetInstrument(sequenceId - INSTRUMENT_OFFSET);
        AudioOcarina_SetPlaybackSong(9, 1);
    } else if (sequenceType & (SEQ_FANFARE | SEQ_OCARINA | SEQ_BGM_SONGS) ||
               (sequenceId < MAX_AUTHENTIC_SEQID &&
                (sSeqFlags[sequenceId] & (SEQ_FLAG_FANFARE | SEQ_FLAG_FANFARE_KAMARO)))) {
        Audio_PlayFanfare(sequenceId);
        sGraceFrames = AUDIO_PREVIEW_GRACE_FRAMES;
        CVarSetInteger(CVAR_AUDIO("Playing"), sequenceId);
    } else {
        // Stop active fanfares to prevent BGM suppression
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 1);

        u16 curSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
        // Only capture restore point if not already previewing
        if (sPreviewRestoreSeqId == NA_BGM_DISABLED && curSeqId != NA_BGM_DISABLED) {
            sPreviewRestoreSeqId = curSeqId;
        }

        Audio_SetSequenceMode(SEQ_MODE_IGNORE);
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1, sequenceId);
        sIsBgmPreviewing = true;

        sGraceFrames = AUDIO_PREVIEW_GRACE_FRAMES;
        CVarSetInteger(CVAR_AUDIO("Playing"), sequenceId);
    }
}
} // namespace AudioPreview

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
}

// Authentic sequence counts
// used to ensure we have enough to shuffle
#define SEQ_COUNT_BGM_WORLD 30
#define SEQ_COUNT_BGM_BATTLE 6
#define SEQ_COUNT_FANFARE 15
#define SEQ_COUNT_OCARINA 12
#define SEQ_COUNT_NOSHUFFLE 6
#define SEQ_COUNT_BGM_EVENT 17
#define SEQ_COUNT_INSTRUMENT 6
#define SEQ_COUNT_SFX 57
#define SEQ_COUNT_VOICE 108

size_t AuthenticCountBySequenceType(SeqType type) {
    return AudioCollection::Instance->CountSequencesByType(type);
}

// Grabs the current BGM sequence ID and replays it
// which will lookup the proper override, or reset back to vanilla
void ReplayCurrentBGM() {
    u16 curSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    // TODO: replace with Audio_StartSeq when the macro is shared
    // The fade time and audio player flags will always be 0 in the case of replaying the BGM, so they are not set here
    // AudioSeq_QueueSeqCmd(0x00000000 | curSeqId);
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, curSeqId);
}

// Attempt to update the BGM if it matches the current sequence that is being played
// The seqKey that is passed in should be the vanilla ID, not the override ID
void UpdateCurrentBGM(u16 seqKey, SeqType seqType) {
    if (seqType != SEQ_BGM_WORLD) {
        return;
    }

    u16 curSeqId = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
    if (curSeqId == seqKey) {
        ReplayCurrentBGM();
    }
}

void RandomizeGroup(SeqType type) {
    bool useCategories = CVarGetInteger("gAudioEditor.UseSongCategories", 0);
    std::vector<std::shared_ptr<SequenceReplacement>> uncategorized;

    // An empty IncludedSequences set means that the AudioEditor window has never been drawn
    if (AudioCollection::Instance->GetIncludedSequences().empty()) {
        AudioCollection::Instance->InitializeShufflePool();
    }

    // use a while loop to add duplicates if we don't have enough included sequences
    while (uncategorized.size() < AudioCollection::Instance->CountSequencesByType(type)) {
        size_t initialSize = uncategorized.size();
        for (const auto& seqData : AudioCollection::Instance->GetIncludedSequences()) {
            if (seqData->type & type && seqData->canBeUsedAsReplacement) {
                SequenceReplacement rep = { seqData, false };
                uncategorized.push_back(std::make_shared<SequenceReplacement>(rep));
            }
        }

        // if we didn't add any new values, return early to prevent an infinite loop
        if (uncategorized.size() == initialSize) {
            return;
        }
    }

    if (uncategorized.empty()) {
        return;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(uncategorized.begin(), uncategorized.end(), g);

    std::map<int, std::vector<std::shared_ptr<SequenceReplacement>>> seqPools = {};
    std::map<int, std::vector<std::shared_ptr<SequenceReplacement>>> seqIdPools = {};
    std::vector<int> includedCategories;
    if (useCategories) {
        // If this group matches *any* fanfare groups
        if (type & SEQ_BGM_CUSTOM_FANFARE) {
            includedCategories = { SEQ_CAT_FAN_GETITEM, SEQ_CAT_FAN_GAMEOVER, SEQ_CAT_FAN_CLEAR,
                                   SEQ_CAT_ID_REPLACEMENT };
        } else {
            includedCategories = {
                SEQ_CAT_FIELD,  SEQ_CAT_TOWN, SEQ_CAT_DUNGEON, SEQ_CAT_INDOOR, SEQ_CAT_MINIGAME,
                SEQ_CAT_ACTION, SEQ_CAT_CALM, SEQ_CAT_BOSS,    SEQ_CAT_TITLE,  SEQ_CAT_ID_REPLACEMENT
            };
        }
        for (auto& category : includedCategories) {
            seqPools.insert({ category, std::vector<std::shared_ptr<SequenceReplacement>>{} });
        }
        // Sort into vectors by component categories
        for (auto& rep : uncategorized) {
            const int filter = rep->seq->categoryFlags;
            std::shared_ptr<std::vector<int>> seqIdReps = rep->seq->seqIdReplacements;
            // Check for sequence specific replacements
            if (seqIdReps != nullptr) {
                for (auto& seqId : *seqIdReps) {
                    if (!seqIdPools.contains(seqId)) {
                        seqIdPools.insert({ seqId, std::vector<std::shared_ptr<SequenceReplacement>>{} });
                    }
                    seqIdPools[seqId].push_back(rep);
                }
            }
            // Check for generic categories
            if (filter != SEQ_CAT_NONE) {
                int compositeCategory = SEQ_CAT_NONE;
                for (auto& category : includedCategories) {
                    if (filter & category) {
                        seqPools[category].push_back(rep);
                        // All applicable categories have been seen.
                        if ((compositeCategory |= category) == filter) {
                            break;
                        }
                    }
                }
            }
        }
    }
    // Checking through a permutation of the categories allows for a sequence slot to pull from any component category,
    // instead of just the first in numerical order.
    std::shuffle(includedCategories.begin(), includedCategories.end(), g);
    std::vector<int>::iterator randomCategory = includedCategories.begin();
    for (const auto& [seqId, seqData] : AudioCollection::Instance->GetAllSequences()) {
        const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
        const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
        // don't randomize locked entries
        if ((seqData.type & type) && CVarGetInteger(cvarLockKey.c_str(), 0) == 0) {
            // Only save authentic sequence CVars
            if ((((seqData.type & SEQ_BGM_CUSTOM) || seqData.type == SEQ_FANFARE) &&
                 seqData.sequenceId >= MAX_AUTHENTIC_SEQID) ||
                seqData.canBeReplaced == false) {
                continue;
            }
            int targetFilter = seqData.categoryFlags;
            if (seqIdPools.contains(seqId) && !seqIdPools[seqId].empty()) {
                // Category selection now checks for sequences targeting specific seqIds.
                // There's probably a better way to do this, though.
                targetFilter |= SEQ_CAT_ID_REPLACEMENT;
                seqPools[SEQ_CAT_ID_REPLACEMENT] = seqIdPools[seqId];
            }
            std::vector<std::shared_ptr<SequenceReplacement>>* chosenSeqPool = nullptr;
            // If category matching is disabled, always use the uncategorized pool.
            if (!useCategories) {
                chosenSeqPool = &uncategorized;
            }
            // Used to check if every applicable category has been seen.
            int compositeCategory = SEQ_CAT_NONE;
            // Even if a sequence pool has been selected, it could've been emptied by the end of the loop. Double check.
            while (!uncategorized.empty() && (chosenSeqPool == nullptr || chosenSeqPool->empty())) {
                int currentCategory, matchFlag = SEQ_CAT_NONE;
                // Stop looking for a valid category after all applicable ones have been seen, or when one is found.
                while (compositeCategory != targetFilter && (!matchFlag || seqPools[currentCategory].empty())) {
                    if (randomCategory == includedCategories.end()) {
                        std::shuffle(includedCategories.begin(), includedCategories.end(), g);
                        randomCategory = includedCategories.begin();
                    }
                    currentCategory = *randomCategory++;
                    matchFlag = targetFilter & currentCategory;
                    compositeCategory |= matchFlag;
                }
                // A valid, matching category was found.
                if (matchFlag && !seqPools[currentCategory].empty()) {
                    chosenSeqPool = &seqPools[currentCategory];
                } else {
                    chosenSeqPool = &uncategorized;
                }
                // Remove trailing replacement entries that were already used in other pools.
                while (!chosenSeqPool->empty() && chosenSeqPool->back()->hasBeenUsed) {
                    chosenSeqPool->pop_back();
                }
            }
            if (chosenSeqPool != nullptr && !chosenSeqPool->empty()) {
                SequenceReplacement& rep = *chosenSeqPool->back();
                const int randomValue = rep.seq->sequenceId;
                CVarSetInteger(cvarKey.c_str(), randomValue);
                rep.hasBeenUsed = true;
                chosenSeqPool->pop_back();
            }
        }
    }
}

void ResetGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.type == type) {
            // Only save authentic sequence CVars
            if (seqData.type == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            if (CVarGetInteger(cvarLockKey.c_str(), 0) == 0) {
                CVarClear(cvarKey.c_str());
            }
        }
    }
}

void LockGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.type == type) {
            // Only save authentic sequence CVars
            if (seqData.type == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            CVarSetInteger(cvarLockKey.c_str(), 1);
        }
    }
}

void UnlockGroup(const std::map<u16, SequenceInfo>& map, SeqType type) {
    for (const auto& [defaultValue, seqData] : map) {
        if (seqData.type == type) {
            // Only save authentic sequence CVars
            if (seqData.type == SEQ_FANFARE && defaultValue >= MAX_AUTHENTIC_SEQID) {
                continue;
            }
            const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
            const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
            CVarSetInteger(cvarLockKey.c_str(), 0);
        }
    }
}
void DrawPreviewButton(uint16_t sequenceId, std::string sfxKey, SeqType sequenceType) {
    const std::string cvarKey = AudioCollection::Instance->GetCvarKey(sfxKey);
    const std::string hiddenKey = "##" + cvarKey;
    const std::string stopButton = ICON_FA_STOP + hiddenKey;
    const std::string previewButton = ICON_FA_PLAY + hiddenKey;

    UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(10.0f, 6.0f));
    if (CVarGetInteger(CVAR_AUDIO("Playing"), 0) == sequenceId) {
        if (ImGui::Button(stopButton.c_str())) {
            AudioPreview::Stop();
        }
        UIWidgets::Tooltip("Stop Preview");
    } else {
        if (ImGui::Button(previewButton.c_str())) {
            AudioPreview::Play(sequenceId, sequenceType);
        }
        UIWidgets::Tooltip("Play Preview");
    }
    UIWidgets::PopStyleButton();
}

void Draw_SfxTab(const std::string& tabId, SeqType type) {
    const std::map<u16, SequenceInfo>& map = AudioCollection::Instance->GetAllSequences();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));

    const std::string hiddenTabId = "##" + tabId;
    const std::string resetAllButton = "Reset All" + hiddenTabId;
    const std::string randomizeAllButton = "Randomize All" + hiddenTabId;
    const std::string lockAllButton = "Lock All" + hiddenTabId;
    const std::string unlockAllButton = "Unlock All" + hiddenTabId;
    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button(resetAllButton.c_str())) {
        auto currentBGM = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        ResetGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(randomizeAllButton.c_str())) {
        auto currentBGM = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        RandomizeGroup(type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(lockAllButton.c_str())) {
        auto currentBGM = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        LockGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(unlockAllButton.c_str())) {
        auto currentBGM = AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN);
        auto prevReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        UnlockGroup(map, type);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        auto curReplacement = AudioCollection::Instance->GetReplacementSequence(currentBGM);
        if (type == SEQ_BGM_WORLD && prevReplacement != curReplacement) {
            ReplayCurrentBGM();
        }
    }
    UIWidgets::PopStyleButton();

    ImGui::BeginTable(tabId.c_str(), 3, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    // Cache valid replacements for this group
    std::unordered_map<u16, const char*> baseFilteredMap;
    for (const auto& [value, seqData] : map) {
        if ((seqData.type & type) && seqData.canBeUsedAsReplacement) {
            baseFilteredMap[value] = seqData.label.c_str();
        }
    }

    for (const auto& [defaultValue, seqData] : map) {
        if (~(seqData.type) & type) {
            continue;
        }
        // Do not display custom sequences in the list
        if ((((seqData.type & SEQ_BGM_CUSTOM) || seqData.type == SEQ_BGM_CUSTOM_FANFARE) &&
             defaultValue >= MAX_AUTHENTIC_SEQID) ||
            seqData.canBeReplaced == false) {
            continue;
        }

        const std::string initialSfxKey = seqData.sfxKey;
        const std::string cvarKey = AudioCollection::Instance->GetCvarKey(seqData.sfxKey);
        const std::string cvarLockKey = AudioCollection::Instance->GetCvarLockKey(seqData.sfxKey);
        const std::string hiddenKey = "##" + cvarKey;
        const std::string resetButton = ICON_FA_UNDO + hiddenKey;
        const std::string randomizeButton = ICON_FA_RANDOM + hiddenKey;
        const std::string lockedButton = ICON_FA_LOCK + hiddenKey;
        const std::string unlockedButton = ICON_FA_UNLOCK + hiddenKey;
        const int currentValue = CVarGetInteger(cvarKey.c_str(), defaultValue);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", seqData.label.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        const int initialValue = map.contains(currentValue) ? currentValue : defaultValue;

        // Use cached map unless current value is hidden/invalid.
        const std::unordered_map<u16, const char*>* mapToUse = &baseFilteredMap;
        std::unordered_map<u16, const char*> tempMap;

        // Ensures they remain visible in the dropdown.
        if (!baseFilteredMap.contains(initialValue) && map.contains(initialValue)) {
            const auto& currentSeqData = map.at(initialValue);
            if (currentSeqData.type & type) {
                tempMap = baseFilteredMap;
                tempMap[initialValue] = currentSeqData.label.c_str();
                mapToUse = &tempMap;
            }
        }

        u16 tempValue = static_cast<u16>(currentValue);
        if (UIWidgets::ComboboxWithSearch(
                hiddenKey.c_str(), &tempValue, mapToUse,
                { .labelPosition = UIWidgets::LabelPosition::None, .color = THEME_COLOR, .width = -FLT_MIN })) {
            CVarSetInteger(cvarKey.c_str(), tempValue);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            UpdateCurrentBGM(defaultValue, type);
        }

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        // BENTODO ship checks for some values and passes either the replaced value or the default value. For some
        // reason, likely an edge case, the original -> replacement lookup is done twice. Passing the original value
        // here seems to work.
        DrawPreviewButton(defaultValue, seqData.sfxKey, type);
        auto locked = CVarGetInteger(cvarLockKey.c_str(), 0) == 1;
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(10.0f, 6.0f));
        if (ImGui::Button(resetButton.c_str())) {
            CVarClear(cvarKey.c_str());
            CVarClear(cvarLockKey.c_str());
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            UpdateCurrentBGM(defaultValue, seqData.type);
        }
        UIWidgets::Tooltip("Reset to default");
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(randomizeButton.c_str())) {
            std::vector<SequenceInfo*> validSequences = {};
            for (const auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                if (seqInfo->type & type) {
                    validSequences.push_back(seqInfo);
                }
            }

            if (validSequences.size()) {
                auto it = validSequences.begin();
                const auto& seqData = *std::next(it, rand() % validSequences.size());
                CVarSetInteger(cvarKey.c_str(), seqData->sequenceId);
                if (locked) {
                    CVarClear(cvarLockKey.c_str());
                }
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
                UpdateCurrentBGM(defaultValue, type);
            }
        }
        UIWidgets::Tooltip("Randomize this sound");
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::Button(locked ? lockedButton.c_str() : unlockedButton.c_str())) {
            if (locked) {
                CVarClear(cvarLockKey.c_str());
            } else {
                CVarSetInteger(cvarLockKey.c_str(), 1);
            }
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
        UIWidgets::Tooltip(locked ? "Sound locked" : "Sound unlocked");
        UIWidgets::PopStyleButton();
    }
    ImGui::EndTable();
    ImGui::PopStyleVar(1);
}

extern "C" u16 AudioEditor_GetReplacementSeq(u16 seqId) {
    return AudioCollection::Instance->GetReplacementSequence(seqId);
}

extern "C" u16 AudioEditor_GetOriginalSeq(u16 seqId) {
    return AudioCollection::Instance->GetOriginalSequence(seqId);
}

const char* GetSequenceTypeName(SeqType type) {
    switch (type) {
        case SEQ_NOSHUFFLE:
            return "No Shuffle";
        case SEQ_BGM_WORLD:
            return "World";
        case SEQ_BGM_EVENT:
            return "Event";
        case SEQ_BGM_BATTLE:
            return "Battle";
        case SEQ_OCARINA:
            return "Ocarina";
        case SEQ_FANFARE:
            return "Fanfare";
        case SEQ_BGM_ERROR:
            return "Error";
        case SEQ_SFX:
            return "SFX";
        case SEQ_VOICE:
            return "Voice";
        case SEQ_INSTRUMENT:
            return "Instrument";
        case SEQ_BGM_CUSTOM:
            return "Custom";
        default:
            return "No Sequence Type";
    }
}

ImVec4 GetSequenceTypeColor(SeqType type) {
    switch (type) {
        case SEQ_BGM_WORLD:
            return ImVec4(0.0f, 0.2f, 0.0f, 1.0f);
        case SEQ_BGM_EVENT:
            return ImVec4(0.3f, 0.0f, 0.15f, 1.0f);
        case SEQ_BGM_BATTLE:
            return ImVec4(0.2f, 0.07f, 0.0f, 1.0f);
        case SEQ_OCARINA:
            return ImVec4(0.0f, 0.0f, 0.4f, 1.0f);
        case SEQ_FANFARE:
            return ImVec4(0.3f, 0.0f, 0.3f, 1.0f);
        case SEQ_SFX:
            return ImVec4(0.4f, 0.33f, 0.0f, 1.0f);
        case SEQ_VOICE:
            return ImVec4(0.3f, 0.42f, 0.09f, 1.0f);
        case SEQ_INSTRUMENT:
            return ImVec4(0.0f, 0.25f, 0.5f, 1.0f);
        case SEQ_BGM_CUSTOM:
            return ImVec4(0.9f, 0.0f, 0.9f, 1.0f);
        default:
            return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
}

void DrawTypeChip(SeqType type) {
    ImGui::BeginDisabled();
    ImGui::PushStyleColor(ImGuiCol_Button, GetSequenceTypeColor(type));
    ImGui::SmallButton(GetSequenceTypeName(type));
    ImGui::PopStyleColor();
    ImGui::EndDisabled();
}

void AudioEditorRegisterRandomizeAllOnNewScene() {
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVarGetInteger(CVAR_AUDIO("RandomizeAllOnNewScene"), 0),
                   { AudioEditor_RandomizeAll(); });
}

void AudioEditor::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoSeedGeneration>([]() {
        if (CVarGetInteger(CVAR_AUDIO("RandomizeAllOnRandoGen"), 0)) {
            AudioEditor_RandomizeAll();
        }
    });

    // This prevents preview state from getting out of sync when the menu is closed
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateUpdate>(AudioPreview::Update);
}

void AudioEditor::DrawElement() {
    AudioCollection::Instance->InitializeShufflePool();

    ImGui::BeginTable("##TopButtons", 4, ImGuiTableFlags_SizingStretchSame);
    ImGui::TableNextColumn();
    UIWidgets::PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Randomize All Groups", ImVec2(-FLT_MIN, 0))) {
        AudioEditor_RandomizeAll();
    }
    UIWidgets::Tooltip("Randomizes all unlocked music and sound effects across tab groups");
    ImGui::TableNextColumn();
    if (ImGui::Button("Reset All Groups", ImVec2(-FLT_MIN, 0))) {
        AudioEditor_ResetAll();
    }
    UIWidgets::Tooltip("Resets all unlocked music and sound effects across tab groups");
    ImGui::TableNextColumn();
    if (ImGui::Button("Lock All Groups", ImVec2(-FLT_MIN, 0))) {
        AudioEditor_LockAll();
    }
    UIWidgets::Tooltip("Locks all music and sound effects across tab groups");
    ImGui::TableNextColumn();
    if (ImGui::Button("Unlock All Groups", ImVec2(-FLT_MIN, 0))) {
        AudioEditor_UnlockAll();
    }
    UIWidgets::Tooltip("Unlocks all music and sound effects across tab groups");
    UIWidgets::PopStyleButton();
    ImGui::EndTable();

    UIWidgets::PushStyleTabs(THEME_COLOR);
    if (ImGui::BeginTabBar("SfxContextTabBar", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        static ImVec2 cellPadding(8.0f, 8.0f);
        if (ImGui::BeginTabItem("Audio Options")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            ImGui::BeginTable("Audio Options", 1, ImGuiTableFlags_SizingStretchSame);
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::BeginChild("SfxOptions", ImVec2(0, -8))) {
                BenGui::mBenMenu->MenuDrawItem(lowHpAlarm, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(muteCarpenterSfx, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(childGoronCry, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(tatlCall, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(enemyProx, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(randoMusicOnSceneChange, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(randomAudioOnSeedGen, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(displaySeqName, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(useSongCategories, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(ovlDuration, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(voicePitchEnable, ImGui::GetContentRegionAvail().x, THEME_COLOR);
                BenGui::mBenMenu->MenuDrawItem(voicePitch, ImGui::GetContentRegionAvail().x, THEME_COLOR);
            }
            ImGui::EndChild();
            ImGui::EndTable();
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Background Music")) {
            Draw_SfxTab("backgroundMusic", SEQ_BGM_WORLD);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fanfares")) {
            Draw_SfxTab("fanfares", SEQ_FANFARE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Events")) {
            Draw_SfxTab("event", SEQ_BGM_EVENT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Battle Music")) {
            Draw_SfxTab("battleMusic", SEQ_BGM_BATTLE);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Ocarina")) {
            Draw_SfxTab("ocarina", SEQ_OCARINA);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Instrument")) {
            Draw_SfxTab("instrument", SEQ_INSTRUMENT);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Songs")) {
            Draw_SfxTab("songs", SEQ_BGM_SONGS);
            ImGui::EndTabItem();
        }
#if 0
        if (ImGui::BeginTabItem("Sound Effects")) {
            Draw_SfxTab("sfx", SEQ_SFX);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Voices")) {
            Draw_SfxTab("voice", SEQ_VOICE);
            ImGui::EndTabItem();
#endif

        static bool excludeTabOpen = false;
        if (ImGui::BeginTabItem("Audio Shuffle Pool Management")) {
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
            if (!excludeTabOpen) {
                excludeTabOpen = true;
            }

            static std::map<SeqType, bool> showType{ { SEQ_BGM_WORLD, true },  { SEQ_BGM_EVENT, true },
                                                     { SEQ_BGM_BATTLE, true }, { SEQ_OCARINA, true },
                                                     { SEQ_FANFARE, true },    { SEQ_SFX, true },
                                                     { SEQ_VOICE, true },      { SEQ_INSTRUMENT, true },
                                                     { SEQ_BGM_CUSTOM, true } };

            // make temporary sets because removing from the set we're iterating through crashes ImGui
            std::set<SequenceInfo*> seqsToInclude = {};
            std::set<SequenceInfo*> seqsToExclude = {};

            static ImGuiTextFilter sequenceSearch;
            UIWidgets::PushStyleInput(THEME_COLOR);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
            sequenceSearch.Draw("Filter (inc,-exc)", 490.0f);
            ImGui::PopStyleVar();
            UIWidgets::PopStyleInput();
            ImGui::SameLine();
            UIWidgets::PushStyleButton(THEME_COLOR);
            if (ImGui::Button("Exclude All")) {
                for (auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->type]) {
                        seqsToExclude.insert(seqInfo);
                    }
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Include All")) {
                for (auto seqInfo : AudioCollection::Instance->GetExcludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->type]) {
                        seqsToInclude.insert(seqInfo);
                    }
                }
            }
            UIWidgets::PopStyleButton();

            ImGui::BeginTable("sequenceTypes", 9,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_WORLD));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_WORLD), &showType[SEQ_BGM_WORLD]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_EVENT));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_EVENT), &showType[SEQ_BGM_EVENT]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_BATTLE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_BATTLE), &showType[SEQ_BGM_BATTLE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_OCARINA));
            ImGui::Selectable(GetSequenceTypeName(SEQ_OCARINA), &showType[SEQ_OCARINA]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_FANFARE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_FANFARE), &showType[SEQ_FANFARE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_SFX));
            ImGui::Selectable(GetSequenceTypeName(SEQ_SFX), &showType[SEQ_SFX]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_VOICE));
            ImGui::Selectable(GetSequenceTypeName(SEQ_VOICE), &showType[SEQ_VOICE]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_INSTRUMENT));
            ImGui::Selectable(GetSequenceTypeName(SEQ_INSTRUMENT), &showType[SEQ_INSTRUMENT]);
            ImGui::PopStyleColor(1);

            ImGui::TableNextColumn();
            ImGui::PushStyleColor(ImGuiCol_Header, GetSequenceTypeColor(SEQ_BGM_CUSTOM));
            ImGui::Selectable(GetSequenceTypeName(SEQ_BGM_CUSTOM), &showType[SEQ_BGM_CUSTOM]);
            ImGui::PopStyleColor(1);

            ImGui::EndTable();

            if (ImGui::BeginTable("tableAllSequences", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Included", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableSetupColumn("Excluded", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                ImGui::TableHeadersRow();
                ImGui::TableNextRow();

                // COLUMN 1 - INCLUDED SEQUENCES
                ImGui::TableNextColumn();

                ImGui::BeginChild("ChildIncludedSequences", ImVec2(0, -8));
                UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(10.0f, 6.0f));
                for (auto seqInfo : AudioCollection::Instance->GetIncludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->type]) {
                        ImGui::PushID(seqInfo->sfxKey.c_str());
                        if (ImGui::Button(std::string(ICON_FA_TIMES "##" + seqInfo->sfxKey).c_str())) {
                            seqsToExclude.insert(seqInfo);
                        }
                        ImGui::SameLine();
                        DrawPreviewButton(seqInfo->sequenceId, seqInfo->sfxKey, seqInfo->type);
                        ImGui::SameLine();
                        DrawTypeChip(seqInfo->type);
                        ImGui::SameLine();
                        ImGui::Text("%s", seqInfo->label.c_str());
                        ImGui::PopID();
                    }
                }
                UIWidgets::PopStyleButton();
                ImGui::EndChild();

                // remove the sequences we added to the temp set
                for (auto seqInfo : seqsToExclude) {
                    AudioCollection::Instance->RemoveFromShufflePool(seqInfo);
                }

                // COLUMN 2 - EXCLUDED SEQUENCES
                ImGui::TableNextColumn();

                ImGui::BeginChild("ChildExcludedSequences", ImVec2(0, -8));
                UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(10.0f, 6.0f));
                for (auto seqInfo : AudioCollection::Instance->GetExcludedSequences()) {
                    if (sequenceSearch.PassFilter(seqInfo->label.c_str()) && showType[seqInfo->type]) {
                        ImGui::PushID(seqInfo->sfxKey.c_str());
                        if (ImGui::Button(std::string(ICON_FA_PLUS "##" + seqInfo->sfxKey).c_str())) {
                            seqsToInclude.insert(seqInfo);
                        }
                        ImGui::SameLine();
                        DrawPreviewButton(seqInfo->sequenceId, seqInfo->sfxKey, seqInfo->type);
                        ImGui::SameLine();
                        DrawTypeChip(seqInfo->type);
                        ImGui::SameLine();
                        ImGui::Text("%s", seqInfo->label.c_str());
                        ImGui::PopID();
                    }
                }
                UIWidgets::PopStyleButton();
                ImGui::EndChild();

                // add the sequences we added to the temp set
                for (auto seqInfo : seqsToInclude) {
                    AudioCollection::Instance->AddToShufflePool(seqInfo);
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar(1);
            ImGui::EndTabItem();
        } else {
            excludeTabOpen = false;
        }

        ImGui::EndTabBar();
    }

    UIWidgets::PopStyleTabs();
}

static std::array<SeqType, 8> allTypes = { SEQ_BGM_WORLD, SEQ_BGM_EVENT,  SEQ_BGM_BATTLE, SEQ_OCARINA,
                                           SEQ_FANFARE,   SEQ_INSTRUMENT, SEQ_SFX,        SEQ_VOICE };

void AudioEditor_RandomizeAll() {
    for (auto type : allTypes) {
        RandomizeGroup(type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_RandomizeGroup(SeqType group) {
    RandomizeGroup(group);

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_ResetAll() {
    for (auto type : allTypes) {
        ResetGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_ResetGroup(SeqType group) {
    ResetGroup(AudioCollection::Instance->GetAllSequences(), group);

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    ReplayCurrentBGM();
}

void AudioEditor_LockAll() {
    for (auto type : allTypes) {
        LockGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AudioEditor_UnlockAll() {
    for (auto type : allTypes) {
        UnlockGroup(AudioCollection::Instance->GetAllSequences(), type);
    }

    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

void AddAudioSearchWidget(WidgetInfo& widgetInfo) {
    BenGui::mBenMenu->AddSearchWidget({ widgetInfo, "Enhancements", "Audio Editor", "Audio Options" });
}

void RegisterAudioWidgets() {

    lowHpAlarm = { .name = "Mute Low HP Alarm", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    lowHpAlarm.CVar(CVAR_AUDIO("LowHpAlarm"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Mutes the beeping alarm when you are critically low on health."));
    AddAudioSearchWidget(lowHpAlarm);

    muteCarpenterSfx = { .name = "Mute Carpenter Sounds", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    muteCarpenterSfx.CVar(CVAR_AUDIO("MuteCarpenterSfx"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Requires scene reload to take effect. Mutes the carpenter sounds coming "
                              "from the tower in South Clock Town."));
    AddAudioSearchWidget(muteCarpenterSfx);

    childGoronCry = { .name = "Mute Crying Goron Child", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    childGoronCry.CVar(CVAR_AUDIO("ChildGoronCry"))
        .Options(CheckboxOptions().Color(THEME_COLOR).Tooltip("Mutes the crying Goron child inside Goron Shrine."));
    AddAudioSearchWidget(childGoronCry);

    tatlCall = { .name = "Disable Tatl Call Audio", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    tatlCall.CVar(CVAR_AUDIO("DisableTatlCallAudio"))
        .Options(CheckboxOptions().Color(THEME_COLOR).Tooltip("Disables the bell audio when Tatl calls you."));
    AddAudioSearchWidget(tatlCall);

    enemyProx = { .name = "Disable Enemy Proximity Music", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    enemyProx.CVar(CVAR_AUDIO("EnemyBGMDisable"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Disables the music change when getting close to enemies. Useful for hearing "
                              "your custom music for each scene more often."));
    AddAudioSearchWidget(enemyProx);

    randoMusicOnSceneChange = { .name = "Randomize All Music and Sound Effects on New Scene",
                                .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    randoMusicOnSceneChange.CVar(CVAR_AUDIO("RandomizeAllOnNewScene"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Enables randomizing all unlocked music and sound effects when you enter a new scene."));
    AddAudioSearchWidget(randoMusicOnSceneChange);

    randomAudioOnSeedGen = { .name = "Randomize All Music and Sound Effects on Randomizer Generation",
                             .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    randomAudioOnSeedGen.CVar(CVAR_AUDIO("RandomizeAllOnRandoGen"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Enables randomizing all unlocked music and sound effects when you generate a new "
                              "randomizer. Respects locks already in place."));
    AddAudioSearchWidget(randomAudioOnSeedGen);

    displaySeqName = { .name = "Display Sequence Name on Overlay", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    displaySeqName.CVar(CVAR_AUDIO("SeqNameNotification"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Displays the name of the current sequence in the corner of the screen whenever a new "
                              "sequence "
                              "is loaded to the main sequence player (does not apply to fanfares or enemy BGM)."));
    AddAudioSearchWidget(displaySeqName);

    useSongCategories = { .name = "Use Song Categories", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    useSongCategories.CVar(CVAR_AUDIO("UseSongCategories"))
        .Options(CheckboxOptions()
                     .Color(THEME_COLOR)
                     .Tooltip("Tries to better match replaced sequences using categories, instead of any background "
                              "sequence being a valid replacement for any other background sequence."));
    AddAudioSearchWidget(useSongCategories);

    ovlDuration = { .name = "Overlay Duration: %d seconds", .type = WidgetType::WIDGET_CVAR_SLIDER_INT };
    ovlDuration.CVar(CVAR_AUDIO("SeqNameNotificationDuration"))
        .Options(IntSliderOptions().Color(THEME_COLOR).Min(1).Max(10).DefaultValue(5).Size(ImVec2(600.0f, 0.0f)));
    AddAudioSearchWidget(ovlDuration);

    voicePitchEnable = { .name = "Enable Link's Voice Pitch Multiplier", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    voicePitchEnable.CVar(CVAR_AUDIO("LinkVoiceFreqMultiplier.Enable"))
        .Options(CheckboxOptions().Color(THEME_COLOR).Tooltip("Enables Link's Voice Pitch Multiplier."));
    AddAudioSearchWidget(voicePitchEnable);

    voicePitch = { .name = "Link's Voice Pitch Multiplier", .type = WidgetType::WIDGET_CVAR_SLIDER_FLOAT };
    voicePitch.CVar(CVAR_AUDIO("LinkVoiceFreqMultiplier.Scale"))
        .PreFunc([](WidgetInfo& info) {
            if (BenGui::mBenMenu->GetDisabledMap().at(DISABLE_FOR_LINKS_VOICE_PITCH_MULTIPLIER_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_LINKS_VOICE_PITCH_MULTIPLIER_OFF);
            }
        })
        .Options(FloatSliderOptions()
                     .Color(THEME_COLOR)
                     .IsPercentage()
                     .Min(0.4f)
                     .Max(2.5f)
                     .DefaultValue(1.0f)
                     .Size(ImVec2(600.0f, 0.0f))
                     .Tooltip("Adjust Link's Voice Pitch Multiplier.Limits are 40% to 250%"));
    AddAudioSearchWidget(voicePitch);
}

static RegisterMenuInitFunc initAudioWidgets(RegisterAudioWidgets);

static RegisterShipInitFunc initFuncRandomizeAllOnNewScene(AudioEditorRegisterRandomizeAllOnNewScene,
                                                           { CVAR_AUDIO("RandomizeAllOnNewScene") });