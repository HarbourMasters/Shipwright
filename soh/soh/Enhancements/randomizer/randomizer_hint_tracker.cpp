#include "randomizer_hint_tracker.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "soh/SohGui/SohGui.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include <ship/window/gui/IconsFontAwesome4.h>

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/hint.h"
#include "soh/Enhancements/randomizer/item_category_adj.h"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/SeedContext.h"
#include "soh/Enhancements/randomizer/static_data.h"

using namespace UIWidgets;

// Defined in debugSaveEditor.cpp.
char z2ASCII(int code);
std::string decodeNTSCPlayerNameChar(int code);

namespace HintTracker {

static Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

// Guards readHints: it is written from the game thread (via the
// OnRandoHintRevealed hook) and read by SaveManager's save thread.
static std::mutex readHintsMutex;
static std::set<RandomizerHint> readHints;
static int sectionId = -1;
static RandomizerCheckArea currentArea = RCAREA_INVALID;
static bool doAreaScroll = false;

// Formatted hint text is cached per hint; invalidated on file load/init and
// when the game language changes. Only touched from the draw thread.
static std::unordered_map<RandomizerHint, std::string> hintTextCache;
static uint8_t cachedLanguage = 0xFF;

static WidgetInfo backgroundColorWidget;
static WidgetInfo windowTypeWidget;
static WidgetInfo readTextColorWidget;
static WidgetInfo unreadColorWidget;
static WidgetInfo wothColorWidget;
static WidgetInfo foolishColorWidget;
static WidgetInfo foundColorWidget;
static WidgetInfo draggableWidget;
static WidgetInfo showOnlyPausedWidget;
static WidgetInfo expandCollapseWidget;
static WidgetInfo searchInputWidget;
static WidgetInfo hintTotalsWidget;
static WidgetInfo hideFoundWidget;

static const Color_RGBA8 Color_ReadText_Default = { 179, 179, 179, 255 };
static const Color_RGBA8 Color_Unread_Default = { 128, 128, 128, 255 };
// Defaults match the in-game textbox colors these hint phrases are shown in:
// "the way of the hero" renders light blue, "a foolish choice" pink.
static const Color_RGBA8 Color_Woth_Default = { 100, 180, 255, 255 };
static const Color_RGBA8 Color_Foolish_Default = { 255, 150, 180, 255 };
// Dimmed grey for hints whose item has already been collected.
static const Color_RGBA8 Color_Found_Default = { 110, 110, 110, 255 };
static Color_RGBA8 Color_ReadText = Color_ReadText_Default;
static Color_RGBA8 Color_Unread = Color_Unread_Default;
static Color_RGBA8 Color_Woth = Color_Woth_Default;
static Color_RGBA8 Color_Foolish = Color_Foolish_Default;
static Color_RGBA8 Color_Found = Color_Found_Default;

static const CustomMessage locationsTabLabel = CustomMessage("Locations", "Orte", "Lieux");
static const CustomMessage journalTabLabel = CustomMessage("Journal", "Tagebuch", "Journal");
static const CustomMessage junkLabel = CustomMessage("Junk", "Ramsch", "Inutile");
static const CustomMessage itemsLabel = CustomMessage("Items", "Gegenstände", "Objets");
static const CustomMessage majorItemsLabel = CustomMessage("Major Items", "Wichtige Gegenstände", "Objets majeurs");
static const CustomMessage bossKeysLabel = CustomMessage("Boss Keys", "Master-Schlüssel", "Clés d'Or");
static const CustomMessage smallKeysLabel = CustomMessage("Small Keys", "Kleine Schlüssel", "Petites Clés");
static const CustomMessage skulltulaTokensLabel =
    CustomMessage("Skulltula Tokens", "Skulltula-Symbole", "Symboles de Skulltula");
static const CustomMessage heartsLabel = CustomMessage("Hearts", "Herzen", "Cœurs");
static const CustomMessage lesserItemsLabel = CustomMessage("Lesser Items", "Kleinere Gegenstände", "Objets mineurs");
static const CustomMessage junkItemsLabel = CustomMessage("Junk Items", "Nutzlose Gegenstände", "Objets inutiles");
static const CustomMessage otherHintsLabel = CustomMessage("Other Hints", "Sonstige Hinweise", "Autres indices");
static const CustomMessage hintsReadLabel = CustomMessage("Hints Read", "Hinweise gelesen", "Indices lus");
static const CustomMessage noHintsMessage =
    CustomMessage("No hints are available for this seed.", "Für diesen Seed sind keine Hinweise verfügbar.",
                  "Aucun indice n'est disponible pour cette seed.");
static const CustomMessage randoOnlyMessage =
    CustomMessage("Hint Tracker is only available on randomizer saves.",
                  "Der Hint Tracker ist nur in Randomizer-Spielständen verfügbar.",
                  "Le Hint Tracker n'est disponible que pour les parties randomizer.");
static const CustomMessage waitingMessage =
    CustomMessage("Waiting for file load...", "Warte auf Spielstand...", "En attente du chargement...");
static const CustomMessage journalEmptyMessage =
    CustomMessage("Hints you read will appear here.", "Gelesene Hinweise erscheinen hier.",
                  "Les indices que tu lis apparaîtront ici.");

static bool IsHintRead(RandomizerHint hintKey) {
    std::lock_guard<std::mutex> lock(readHintsMutex);
    return readHints.contains(hintKey);
}

static void MarkHintAsRead(RandomizerHint hintKey) {
    if (hintKey == RH_NONE || !OTRGlobals::Instance->gRandoContext->GetHint(hintKey)->IsEnabled()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(readHintsMutex);
        if (!readHints.insert(hintKey).second) {
            return;
        }
    }
    if (sectionId >= 0) {
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
    }
}

void InitHintTrackerData(bool isDebug) {
    std::lock_guard<std::mutex> lock(readHintsMutex);
    readHints.clear();
    hintTextCache.clear();
}

void SaveHintTrackerData(SaveContext* saveContext, int sectionID, bool fullSave) {
    std::vector<RandomizerHint> hints;
    {
        std::lock_guard<std::mutex> lock(readHintsMutex);
        hints.assign(readHints.begin(), readHints.end());
    }
    // Hints are stored by canonical name rather than enum value so saved
    // flags survive hints being added to or removed from the enum. find()
    // rather than operator[] so a missing name can't mutate the shared table
    // from the save thread.
    SaveManager::Instance->SaveArray("readHints", hints.size(), [&](size_t i) {
        auto name = Rando::StaticData::hintNames.find(hints[i]);
        if (name != Rando::StaticData::hintNames.end()) {
            SaveManager::Instance->SaveData("", name->second.GetEnglish(MF_CLEAN));
        }
    });
}

void LoadHintTrackerData() {
    std::set<RandomizerHint> loaded;
    SaveManager::Instance->LoadArray("readHints", RH_MAX, [&](size_t i) {
        // Read type-agnostically and skip anything that isn't a known hint
        // name, so malformed or outdated entries can't abort the save load.
        nlohmann::json value;
        SaveManager::Instance->LoadData("", value, nlohmann::json());
        if (!value.is_string()) {
            return;
        }
        auto it = Rando::StaticData::hintNameToEnum.find(value.get<std::string>());
        if (it != Rando::StaticData::hintNameToEnum.end()) {
            loaded.insert(static_cast<RandomizerHint>(it->second));
        }
    });
    std::lock_guard<std::mutex> lock(readHintsMutex);
    readHints = std::move(loaded);
}

// Decodes the current save's player name, for substituting the '@' player
// name marker that the in-game textbox resolves at draw time.
static std::string GetPlayerName() {
    std::string name;
    for (int i = 0; i < 8; i++) {
        if (gSaveContext.ship.filenameLanguage == NAME_LANGUAGE_PAL) {
            name += z2ASCII(gSaveContext.playerName[i]);
        } else {
            name += decodeNTSCPlayerNameChar(gSaveContext.playerName[i]);
        }
    }
    while (!name.empty() && name.back() == ' ') {
        name.pop_back();
    }
    return name;
}

// Variants that differ only in punctuation or whitespace carry the same
// information; compare on lowercased alphanumeric content only.
static std::string NormalizeForCompare(const std::string& text) {
    std::string key;
    for (unsigned char c : text) {
        if (std::isalnum(c)) {
            key += static_cast<char>(std::tolower(c));
        }
    }
    return key;
}

// Assembles the full text of a hint, joining multi-message hints into one
// entry. The "Buy" replacements mirror BuildHintStoneMessage
// (GossipStoneHints.cpp) and must stay in sync with it.
static const std::string& GetJoinedHintText(RandomizerHint hintKey) {
    auto cached = hintTextCache.find(hintKey);
    if (cached != hintTextCache.end()) {
        return cached->second;
    }
    Rando::Hint* hint = OTRGlobals::Instance->gRandoContext->GetHint(hintKey);
    const std::string playerName = GetPlayerName();
    std::vector<std::string> parts;
    std::vector<std::string> keys;
    size_t numMessages = hint->GetNumberOfMessages();
    for (size_t i = 0; i < numMessages; i++) {
        CustomMessage msg = hint->GetHintMessage(MF_CLEAN, i);
        msg.Replace("Buy ", "");
        msg.Replace("Acheter: ", "");
        msg.Replace(" kaufen ", "");
        msg.Replace(" kaufen", "");
        msg.Replace("@", playerName);
        std::string part = msg.GetForCurrentLanguage(MF_CLEAN);
        // Variants of a hint often share the same wording give or take
        // punctuation (e.g. Saria's in-person and ocarina messages); show
        // each distinct wording once.
        std::string key = NormalizeForCompare(part);
        if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
            keys.push_back(key);
            parts.push_back(part);
        }
    }
    std::string text;
    for (const std::string& part : parts) {
        if (!text.empty()) {
            text += "\n";
        }
        text += part;
    }
    return hintTextCache[hintKey] = text;
}

struct HintEntry {
    RandomizerHint hintKey;
    std::string name;
    // Compact entries are a bare location/area line with no hint text below.
    bool compact = false;
    // Entries sort by rank first (lower first), then by name.
    int sortRank = 0;
    // Optional color for the name line (points at one of the color statics).
    const Color_RGBA8* nameColor = nullptr;
    // True once every location this hint points at has been collected. Found
    // entries sink to the bottom of their group and render dimmed with a tick.
    bool found = false;
};

// Most valuable first, mirroring the ordering implied by chest size & color
// matches contents.
static int ItemCategoryRank(GetItemCategory category) {
    switch (category) {
        case ITEM_CATEGORY_MAJOR:
            return 0;
        case ITEM_CATEGORY_BOSS_KEY:
            return 1;
        case ITEM_CATEGORY_SMALL_KEY:
            return 2;
        case ITEM_CATEGORY_SKULLTULA_TOKEN:
            return 3;
        case ITEM_CATEGORY_HEALTH:
            return 4;
        case ITEM_CATEGORY_LESSER:
            return 5;
        case ITEM_CATEGORY_JUNK:
        default:
            return 6;
    }
}

// Group label for each ItemCategoryRank value, in rank order. Callers clamp
// the unresolved-item fallback rank (JUNK + 1) into the Junk group first.
static std::string ItemCategoryRankName(int rank) {
    static const CustomMessage* const rankLabels[] = {
        &majorItemsLabel,      // ITEM_CATEGORY_MAJOR
        &bossKeysLabel,        // ITEM_CATEGORY_BOSS_KEY
        &smallKeysLabel,       // ITEM_CATEGORY_SMALL_KEY
        &skulltulaTokensLabel, // ITEM_CATEGORY_SKULLTULA_TOKEN
        &heartsLabel,          // ITEM_CATEGORY_HEALTH
        &lesserItemsLabel,     // ITEM_CATEGORY_LESSER
        &junkItemsLabel,       // ITEM_CATEGORY_JUNK
    };
    return rankLabels[rank]->GetForCurrentLanguage(MF_CLEAN);
}

static void DrawHintEntry(const HintEntry& entry) {
    // A collected hint is dimmed and prefixed with a tick; the dim colour wins
    // over any per-type name colour (Way of the Hero / Foolish).
    const Color_RGBA8* nameColor = entry.found ? &Color_Found : entry.nameColor;
    std::string name = entry.found ? (ICON_FA_CHECK " " + entry.name) : entry.name;
    if (nameColor != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(nameColor->r / 255.0f, nameColor->g / 255.0f, nameColor->b / 255.0f,
                                                    nameColor->a / 255.0f));
        ImGui::TextUnformatted(name.c_str());
        ImGui::PopStyleColor();
    } else {
        ImGui::TextUnformatted(name.c_str());
    }
    if (entry.compact) {
        return;
    }
    ImGui::Indent();
    if (IsHintRead(entry.hintKey)) {
        const Color_RGBA8& textColor = entry.found ? Color_Found : Color_ReadText;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(textColor.r / 255.0f, textColor.g / 255.0f, textColor.b / 255.0f,
                                                    textColor.a / 255.0f));
        ImGui::TextWrapped("%s", GetJoinedHintText(entry.hintKey).c_str());
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(Color_Unread.r / 255.0f, Color_Unread.g / 255.0f,
                                                    Color_Unread.b / 255.0f, Color_Unread.a / 255.0f));
        ImGui::TextUnformatted("???");
        ImGui::PopStyleColor();
    }
    ImGui::Unindent();
    Spacer(0);
}

// Draws a tree node header labeled "<name> (read/total)" — or just
// "<name> (read)" when the total would spoil how many hints of a kind exist —
// with an identity that stays stable as the counts change.
static bool DrawGroupHeader(const std::string& name, size_t read, size_t total, uint8_t nextTreeState, bool showTotal) {
    if (nextTreeState) {
        ImGui::SetNextItemOpen(nextTreeState == 2, ImGuiCond_Always);
    } else {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    }
    std::string label =
        name + " (" + std::to_string(read) + (showTotal ? "/" + std::to_string(total) : "") + ")###HintTracker" + name;
    return ImGui::TreeNodeEx(label.c_str());
}

// Draws the Locations/Journal tab bar and returns the active view mode,
// persisted in a CVar so the selection survives restarts.
static HintTrackerViewMode DrawViewTabs() {
    HintTrackerViewMode viewMode =
        static_cast<HintTrackerViewMode>(CVarGetInteger(CVAR_TRACKER_HINT("ViewMode"), HINT_TRACKER_VIEW_LOCATIONS));
    // Force the saved tab selection once per session; afterwards ImGui owns it.
    static bool tabRestored = false;
    auto drawTab = [&](const CustomMessage& label, const char* id, HintTrackerViewMode mode, const char* tooltip) {
        ImGuiTabItemFlags flags = (!tabRestored && viewMode == mode) ? ImGuiTabItemFlags_SetSelected : 0;
        if (ImGui::BeginTabItem((label.GetForCurrentLanguage(MF_CLEAN) + id).c_str(), nullptr, flags)) {
            if (viewMode != mode && tabRestored) {
                viewMode = mode;
                CVarSetInteger(CVAR_TRACKER_HINT("ViewMode"), mode);
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tooltip);
        }
    };
    if (ImGui::BeginTabBar("##HintTrackerViewTabs")) {
        drawTab(locationsTabLabel, "###HintTrackerLocationsTab", HINT_TRACKER_VIEW_LOCATIONS,
                "Every hint location grouped by area, with unread hints masked as \"???\"");
        drawTab(journalTabLabel, "###HintTrackerJournalTab", HINT_TRACKER_VIEW_JOURNAL,
                "Hints you have already read, grouped by hint type with the most valuable first");
        tabRestored = true;
        ImGui::EndTabBar();
    }
    return viewMode;
}

static void DrawHintList() {
    auto ctx = OTRGlobals::Instance->gRandoContext;

    static ImGuiTextFilter hintSearch;
    uint8_t nextTreeState = 0;

    HintTrackerViewMode viewMode = DrawViewTabs();

    Color_ReadText = CVarGetColor(CVAR_TRACKER_HINT("ReadTextColor.Value"), Color_ReadText_Default);
    Color_Unread = CVarGetColor(CVAR_TRACKER_HINT("UnreadColor.Value"), Color_Unread_Default);
    Color_Woth = CVarGetColor(CVAR_TRACKER_HINT("WothColor.Value"), Color_Woth_Default);
    Color_Foolish = CVarGetColor(CVAR_TRACKER_HINT("FoolishColor.Value"), Color_Foolish_Default);
    Color_Found = CVarGetColor(CVAR_TRACKER_HINT("FoundColor.Value"), Color_Found_Default);

    bool showExpandCollapse = CVarGetInteger(CVAR_TRACKER_HINT("ExpandCollapseButtonsVisible"), 1);
    bool showSearch = CVarGetInteger(CVAR_TRACKER_HINT("SearchInputVisible"), 1);
    bool showTotals = CVarGetInteger(CVAR_TRACKER_HINT("HintTotalsVisible"), 1);

    if (showExpandCollapse) {
        if (Button("Collapse All",
                   ButtonOptions({ { .tooltip = "Collapse all areas" } }).Color(THEME_COLOR).Size(Sizes::Inline))) {
            nextTreeState = 1;
        }
        ImGui::SameLine();
        if (Button("Expand All",
                   ButtonOptions({ { .tooltip = "Expand all areas" } }).Color(THEME_COLOR).Size(Sizes::Inline))) {
            nextTreeState = 2;
        }
    }
    if (showSearch) {
        if (showExpandCollapse) {
            ImGui::SameLine();
        }
        if (Button("Clear",
                   ButtonOptions({ { .tooltip = "Clear the search field" } }).Color(THEME_COLOR).Size(Sizes::Inline))) {
            hintSearch.Clear();
        }

        PushStyleCombobox(THEME_COLOR);
        if (hintSearch.Draw()) {
            nextTreeState = 2;
        }
        PopStyleCombobox();
    } else if (hintSearch.IsActive()) {
        // Don't leave the list invisibly filtered by a search box that is
        // no longer shown.
        hintSearch.Clear();
    }

    if (gSaveContext.language != cachedLanguage) {
        hintTextCache.clear();
        cachedLanguage = gSaveContext.language;
    }

    const std::string otherHintsName = otherHintsLabel.GetForCurrentLanguage(MF_CLEAN);
    bool journalView = viewMode == HINT_TRACKER_VIEW_JOURNAL;

    struct HintGroup {
        std::string name;
        size_t read = 0;
        size_t total = 0;
        RandomizerCheckArea area = RCAREA_INVALID;
        std::vector<HintEntry> entries;
    };
    // Groups render in key order: area enum order in the Locations view (with
    // "Other Hints" forced last), hint type value priority in the Journal.
    std::map<size_t, HintGroup> groups;
    size_t totalHints = 0;
    size_t readCount = 0;

    auto addHint = [&](RandomizerHint hintKey, size_t groupKey, const std::string& groupName,
                       RandomizerCheckArea area) {
        bool read = IsHintRead(hintKey);
        totalHints++;
        if (read) {
            readCount++;
        }
        // The journal view only lists hints the player has already read:
        // showing which stones hold unread Way of the Hero or Foolish hints
        // would spoil exactly which stones are worth visiting.
        if (journalView && !read) {
            return;
        }
        // Way of the Hero / Foolish hints are just area statements, so in the
        // journal the hinted area name alone says everything the hint did.
        bool compact = false;
        int sortRank = 0;
        bool found = false;
        const Color_RGBA8* nameColor = nullptr;
        std::string hintName;
        // The group this hint lands in. Item hints override it with their
        // item category once it is known below.
        size_t effectiveKey = groupKey;
        std::string effectiveName = groupName;
        if (journalView) {
            Rando::Hint* hint = ctx->GetHint(hintKey);
            HintType type = hint->GetHintType();
            if (type == HINT_TYPE_WOTH || type == HINT_TYPE_FOOLISH) {
                size_t numAreas = hint->GetHintedAreas().size();
                for (size_t slot = 0; slot < numAreas; slot++) {
                    if (!hintName.empty()) {
                        hintName += ", ";
                    }
                    hintName += hint->GetAreaName(static_cast<uint8_t>(slot)).GetForCurrentLanguage(MF_CLEAN);
                }
                compact = !hintName.empty();
                if (compact) {
                    nameColor = type == HINT_TYPE_WOTH ? &Color_Woth : &Color_Foolish;
                }
            } else if ((type == HINT_TYPE_ITEM || type == HINT_TYPE_ITEM_AREA) &&
                       !CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
                // Rank item hints by their adjusted item category, so ice trap
                // disguises rank as their cover item. Mysterious Shuffle disables
                // the ranking. Pass checkObtainability = false: we want the true
                // placed item's value, otherwise a genuine major item that isn't
                // reachable yet (e.g. Anju's Lens of Truth as adult) gets masked
                // to a blue rupee and wrongly sinks to junk rank.
                sortRank = ItemCategoryRank(ITEM_CATEGORY_JUNK) + 1;
                std::vector<RandomizerCheck> hintedLocations = hint->GetHintedLocations();
                // "Found" means every location this hint points at is collected;
                // a multi-location hint still matters while any part is uncollected.
                found = !hintedLocations.empty();
                for (RandomizerCheck rc : hintedLocations) {
                    GetItemEntry itemEntry = ctx->GetFinalGIEntry(rc, false, GI_NONE);
                    sortRank = std::min(sortRank, ItemCategoryRank(Randomizer_AdjustItemCategory(itemEntry)));
                    if (!ctx->GetItemLocation(rc)->HasObtained()) {
                        found = false;
                    }
                }
                // Fan the single "Items" group out into the item categories.
                // The unresolved-item fallback rank shares the Junk group.
                int rank = std::min(sortRank, ItemCategoryRank(ITEM_CATEGORY_JUNK));
                effectiveKey = groupKey + static_cast<size_t>(rank);
                effectiveName = ItemCategoryRankName(rank);
            }
        }
        if (hintName.empty()) {
            hintName = Rando::StaticData::hintNames[hintKey].GetForCurrentLanguage(MF_CLEAN);
        }
        HintGroup& group = groups[effectiveKey];
        if (group.name.empty()) {
            group.name = effectiveName;
            group.area = area;
        }
        group.total++;
        if (read) {
            group.read++;
        }
        // "Hide found" only removes fully-collected hints from the journal view
        // (found is never set in the locations view).
        bool hideFound = found && CVarGetInteger(CVAR_TRACKER_HINT("HideFound"), 0);
        if (!hideFound && (hintSearch.PassFilter(hintName.c_str()) || hintSearch.PassFilter(effectiveName.c_str()))) {
            group.entries.push_back({ hintKey, hintName, compact, sortRank, nameColor, found });
        }
    };
    auto typeGroupKey = [](HintType type) {
        // Item and item-area hints share one "Items" group in the Journal.
        if (type == HINT_TYPE_ITEM_AREA) {
            type = HINT_TYPE_ITEM;
        }
        // High-value hint types first, junk lines last.
        static const std::vector<HintType> priority = {
            HINT_TYPE_WOTH,  HINT_TYPE_FOOLISH,     HINT_TYPE_ITEM,        HINT_TYPE_AREA,    HINT_TYPE_ENTRANCE,
            HINT_TYPE_TRIAL, HINT_TYPE_ALTAR_CHILD, HINT_TYPE_ALTAR_ADULT, HINT_TYPE_MESSAGE, HINT_TYPE_HINT_KEY,
        };
        // Keys are scaled so the "Items" slot can fan out into the item
        // categories (+0..+6) without colliding with the next hint type.
        constexpr size_t kCategorySlots = 8;
        auto rank = std::find(priority.begin(), priority.end(), type);
        if (rank == priority.end()) {
            // Unranked types each get their own group after the ranked ones.
            return (priority.size() + static_cast<size_t>(type)) * kCategorySlots;
        }
        return static_cast<size_t>(rank - priority.begin()) * kCategorySlots;
    };
    auto typeGroupName = [](HintType type) {
        if (type == HINT_TYPE_ITEM || type == HINT_TYPE_ITEM_AREA) {
            return itemsLabel.GetForCurrentLanguage(MF_CLEAN);
        }
        // Upstream calls the junk hint type "Message"; "Junk" is clearer here.
        if (type == HINT_TYPE_HINT_KEY) {
            return junkLabel.GetForCurrentLanguage(MF_CLEAN);
        }
        return Rando::StaticData::hintTypeNames[type].GetForCurrentLanguage(MF_CLEAN);
    };

    std::set<RandomizerHint> stoneHintKeys;
    for (auto& [rc, hintKey] : Rando::StaticData::gossipStoneCheckToHint) {
        stoneHintKeys.insert(hintKey);
        if (!ctx->GetHint(hintKey)->IsEnabled()) {
            continue;
        }
        if (journalView) {
            HintType type = ctx->GetHint(hintKey)->GetHintType();
            addHint(hintKey, typeGroupKey(type), typeGroupName(type), RCAREA_INVALID);
        } else {
            RandomizerCheckArea area = Rando::StaticData::GetLocation(rc)->GetArea();
            addHint(hintKey, area, RandomizerCheckObjects::GetRCAreaName(area), area);
        }
    }

    // Non-stone hints the seed has enabled (Ganondorf, Sheik, altar, warp
    // songs, NPC hints, ...). When grouping by area they form one "Other
    // Hints" group at the bottom; when grouping by type they join their type.
    for (int i = RH_NONE + 1; i < RH_MAX; i++) {
        RandomizerHint hintKey = static_cast<RandomizerHint>(i);
        if (stoneHintKeys.contains(hintKey) || !ctx->GetHint(hintKey)->IsEnabled()) {
            continue;
        }
        if (journalView) {
            HintType type = ctx->GetHint(hintKey)->GetHintType();
            addHint(hintKey, typeGroupKey(type), typeGroupName(type), RCAREA_INVALID);
        } else {
            addHint(hintKey, RCAREA_INVALID, otherHintsName, RCAREA_INVALID);
        }
    }

    if (showTotals) {
        ImGui::Text("%zu / %zu %s", readCount, totalHints, hintsReadLabel.GetForCurrentLanguage(MF_CLEAN).c_str());
    }

    if (totalHints == 0) {
        ImGui::TextWrapped("%s", noHintsMessage.GetForCurrentLanguage(MF_CLEAN).c_str());
        return;
    }
    if (journalView && readCount == 0) {
        ImGui::TextWrapped("%s", journalEmptyMessage.GetForCurrentLanguage(MF_CLEAN).c_str());
        return;
    }

    ImGui::BeginChild("ChildHintTrackerHints", ImVec2(0, -8));
    ImGui::SetWindowFontScale(CVarGetFloat(CVAR_TRACKER_HINT("FontSize"), 1.0f));
    for (auto& [groupKey, group] : groups) {
        if (group.entries.empty() && hintSearch.IsActive()) {
            continue;
        }
        std::sort(group.entries.begin(), group.entries.end(), [](const auto& left, const auto& right) {
            // Collected hints sink to the bottom of their group.
            if (left.found != right.found) {
                return !left.found;
            }
            if (left.sortRank != right.sortRank) {
                return left.sortRank < right.sortRank;
            }
            return left.name < right.name;
        });
        bool groupOpen = DrawGroupHeader(group.name, group.read, group.total, nextTreeState, !journalView);
        if (group.area != RCAREA_INVALID && group.area == currentArea && doAreaScroll) {
            ImGui::SetScrollHereY(0.0f);
            doAreaScroll = false;
        }
        if (groupOpen) {
            for (auto& entry : group.entries) {
                DrawHintEntry(entry);
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
}

void HintTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void HintTrackerWindow::DrawElement() {
    Color_Background = CVarGetColor(CVAR_TRACKER_HINT("BgColor.Value"), Color_Bg_Default);
    if (CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        if (CVarGetInteger(CVAR_TRACKER_HINT("ShowOnlyPaused"), 0) &&
            (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)) {
            return;
        }

        if (CVarGetInteger(CVAR_TRACKER_HINT("DisplayType"), TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            int comboButton1Mask = buttons[CVarGetInteger(CVAR_TRACKER_HINT("ComboButton1"), TRACKER_COMBO_BUTTON_L)];
            int comboButton2Mask = buttons[CVarGetInteger(CVAR_TRACKER_HINT("ComboButton2"), TRACKER_COMBO_BUTTON_R)];
            OSContPad* trackerButtonsPressed =
                std::dynamic_pointer_cast<LUS::ControlDeck>(Ship::Context::GetRawInstance()->GetControlDeck())
                    ->GetPads();
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld) {
                return;
            }
        }
    }

    ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
    if (Trackers::BeginFloatWindows(
            "Hint Tracker", mIsVisible, Color_Background,
            static_cast<TrackerWindowType>(CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW)),
            CVarGetInteger(CVAR_TRACKER_HINT("Draggable"), 1))) {
        ImGui::SetWindowFontScale(CVarGetFloat(CVAR_TRACKER_HINT("FontSize"), 1.0f));
        if (!GameInteractor::IsSaveLoaded()) {
            ImGui::TextUnformatted(waitingMessage.GetForCurrentLanguage(MF_CLEAN).c_str());
        } else if (!IS_RANDO) {
            ImGui::TextWrapped("%s", randoOnlyMessage.GetForCurrentLanguage(MF_CLEAN).c_str());
        } else {
            DrawHintList();
        }
    }
    Trackers::EndFloatWindows();
}

void HintTrackerWindow::InitElement() {
    SaveManager::Instance->AddInitFunction(InitHintTrackerData);
    sectionId =
        SaveManager::Instance->AddSaveFunction("hintTrackerData", 1, SaveHintTrackerData, true, SECTION_PARENT_NONE);
    SaveManager::Instance->AddLoadFunction("hintTrackerData", 1, LoadHintTrackerData);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRandoHintRevealed>(MarkHintAsRead);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>([](uint32_t sceneNum) {
        if (!GameInteractor::IsSaveLoaded()) {
            return;
        }
        currentArea = CheckTracker::GetCheckArea();
        doAreaScroll = true;
    });
}

void HintTrackerSettingsWindow::DrawElement() {
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    if (!ImGui::BeginTable("HintTrackerSettingsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::PopStyleVar();
        return;
    }
    ImGui::TableSetupColumn("General settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Colors", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    SohGui::GetSohMenu()->MenuDrawItem(backgroundColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    SohGui::GetSohMenu()->MenuDrawItem(windowTypeWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    CVarSliderFloat("Font Size", CVAR_TRACKER_HINT("FontSize"),
                    FloatSliderOptions()
                        .Tooltip("Sets the font size used in the hint tracker.")
                        .Format("%.1f")
                        .Step(0.1f)
                        .Min(0.3f)
                        .Max(2.0f)
                        .Color(THEME_COLOR)
                        .DefaultValue(1.0f));

    if (CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        SohGui::GetSohMenu()->MenuDrawItem(draggableWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                           THEME_COLOR);
        SohGui::GetSohMenu()->MenuDrawItem(showOnlyPausedWidget,
                                           static_cast<uint32_t>(ImGui::GetContentRegionAvail().x), THEME_COLOR);
        CVarCombobox("Display Mode", CVAR_TRACKER_HINT("DisplayType"), showMode,
                     ComboboxOptions()
                         .LabelPosition(LabelPositions::Far)
                         .ComponentAlignment(ComponentAlignments::Right)
                         .Color(THEME_COLOR)
                         .DefaultIndex(0));
        if (CVarGetInteger(CVAR_TRACKER_HINT("DisplayType"), TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            CVarCombobox("Combo Button 1", CVAR_TRACKER_HINT("ComboButton1"), buttonStrings,
                         ComboboxOptions()
                             .LabelPosition(LabelPositions::Far)
                             .ComponentAlignment(ComponentAlignments::Right)
                             .Color(THEME_COLOR)
                             .DefaultIndex(TRACKER_COMBO_BUTTON_L));
            CVarCombobox("Combo Button 2", CVAR_TRACKER_HINT("ComboButton2"), buttonStrings,
                         ComboboxOptions()
                             .LabelPosition(LabelPositions::Far)
                             .ComponentAlignment(ComponentAlignments::Right)
                             .Color(THEME_COLOR)
                             .DefaultIndex(TRACKER_COMBO_BUTTON_R));
        }
    }

    ImGui::SeparatorText("Tracker Header Visibility");
    SohGui::GetSohMenu()->MenuDrawItem(expandCollapseWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(searchInputWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(hintTotalsWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    ImGui::SeparatorText("Journal");
    SohGui::GetSohMenu()->MenuDrawItem(hideFoundWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    ImGui::TableNextColumn();

    SohGui::GetSohMenu()->MenuDrawItem(readTextColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(unreadColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(wothColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(foolishColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(foundColorWidget, static_cast<uint32_t>(ImGui::GetContentRegionAvail().x),
                                       THEME_COLOR);

    ImGui::EndTable();
    ImGui::PopStyleVar();
}

void RegisterHintTrackerWidgets() {
    backgroundColorWidget = { .name = "Background Color##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    backgroundColorWidget.CVar(CVAR_TRACKER_HINT("BgColor"))
        .Options(
            ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Bg_Default).UseAlpha().ShowReset().ShowRandom());
    SohGui::GetSohMenu()->AddSearchWidget({ backgroundColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    windowTypeWidget = { .name = "Window Type##HintTracker", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    windowTypeWidget.CVar(CVAR_TRACKER_HINT("WindowType"))
        .Options(ComboboxOptions()
                     .DefaultIndex(TRACKER_WINDOW_WINDOW)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Far)
                     .Color(THEME_COLOR)
                     .ComboMap(windowType));
    SohGui::GetSohMenu()->AddSearchWidget({ windowTypeWidget, "Randomizer", "Hint Tracker", "General Settings" });

    readTextColorWidget = { .name = "Read Hint Text##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    readTextColorWidget.CVar(CVAR_TRACKER_HINT("ReadTextColor"))
        .Options(ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_ReadText_Default).UseAlpha().ShowReset());
    SohGui::GetSohMenu()->AddSearchWidget({ readTextColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    unreadColorWidget = { .name = "Unread (???)##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    unreadColorWidget.CVar(CVAR_TRACKER_HINT("UnreadColor"))
        .Options(ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Unread_Default).UseAlpha().ShowReset());
    SohGui::GetSohMenu()->AddSearchWidget({ unreadColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    wothColorWidget = { .name = "Way of the Hero##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    wothColorWidget.CVar(CVAR_TRACKER_HINT("WothColor"))
        .Options(ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Woth_Default).UseAlpha().ShowReset());
    SohGui::GetSohMenu()->AddSearchWidget({ wothColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    foolishColorWidget = { .name = "Foolish##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    foolishColorWidget.CVar(CVAR_TRACKER_HINT("FoolishColor"))
        .Options(ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Foolish_Default).UseAlpha().ShowReset());
    SohGui::GetSohMenu()->AddSearchWidget({ foolishColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    foundColorWidget = { .name = "Found (collected)##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    foundColorWidget.CVar(CVAR_TRACKER_HINT("FoundColor"))
        .Options(ColorPickerOptions().Color(THEME_COLOR).DefaultValue(Color_Found_Default).UseAlpha().ShowReset());
    SohGui::GetSohMenu()->AddSearchWidget({ foundColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    draggableWidget = { .name = "Enable Dragging##HintTracker", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    draggableWidget.CVar(CVAR_TRACKER_HINT("Draggable")).Options(CheckboxOptions().Color(THEME_COLOR));
    SohGui::GetSohMenu()->AddSearchWidget({ draggableWidget, "Randomizer", "Hint Tracker", "General Settings" });

    showOnlyPausedWidget = { .name = "Only Enable While Paused##HintTracker",
                             .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    showOnlyPausedWidget.CVar(CVAR_TRACKER_HINT("ShowOnlyPaused")).Options(CheckboxOptions().Color(THEME_COLOR));
    SohGui::GetSohMenu()->AddSearchWidget({ showOnlyPausedWidget, "Randomizer", "Hint Tracker", "General Settings" });

    expandCollapseWidget = { .name = "Expand/Collapse Buttons##HintTracker", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    expandCollapseWidget.CVar(CVAR_TRACKER_HINT("ExpandCollapseButtonsVisible"))
        .Options(CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
    SohGui::GetSohMenu()->AddSearchWidget({ expandCollapseWidget, "Randomizer", "Hint Tracker", "General Settings" });

    searchInputWidget = { .name = "Search Input##HintTracker", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    searchInputWidget.CVar(CVAR_TRACKER_HINT("SearchInputVisible"))
        .Options(CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
    SohGui::GetSohMenu()->AddSearchWidget({ searchInputWidget, "Randomizer", "Hint Tracker", "General Settings" });

    hintTotalsWidget = { .name = "Hint Totals##HintTracker", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    hintTotalsWidget.CVar(CVAR_TRACKER_HINT("HintTotalsVisible"))
        .Options(CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
    SohGui::GetSohMenu()->AddSearchWidget({ hintTotalsWidget, "Randomizer", "Hint Tracker", "General Settings" });

    hideFoundWidget = { .name = "Hide Found Items##HintTracker", .type = WidgetType::WIDGET_CVAR_CHECKBOX };
    hideFoundWidget.CVar(CVAR_TRACKER_HINT("HideFound"))
        .Options(CheckboxOptions()
                     .Tooltip("Removes hints whose item you have already collected from the Journal, instead of "
                              "dimming them and sorting them to the bottom.")
                     .Color(THEME_COLOR)
                     .DefaultValue(false));
    SohGui::GetSohMenu()->AddSearchWidget({ hideFoundWidget, "Randomizer", "Hint Tracker", "General Settings" });
}

static RegisterMenuInitFunc menuInitFunc(RegisterHintTrackerWidgets);
} // namespace HintTracker
