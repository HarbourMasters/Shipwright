#include "EventLog.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include <z64.h>
}

typedef enum {
    EVENT_LOG_ENTRY_TYPE_FLAG_SET,
    EVENT_LOG_ENTRY_TYPE_FLAG_UNSET,
    EVENT_LOG_ENTRY_TYPE_SCENE_FLAG_SET,
    EVENT_LOG_ENTRY_TYPE_SCENE_FLAG_UNSET,
    EVENT_LOG_ENTRY_TYPE_ACTOR_INIT,
    EVENT_LOG_ENTRY_TYPE_ACTOR_KILL,
    EVENT_LOG_ENTRY_TYPE_SCENE_INIT,
    EVENT_LOG_ENTRY_TYPE_ROOM_INIT,
    EVENT_LOG_ENTRY_TYPE_OPEN_TEXT,
    EVENT_LOG_ENTRY_TYPE_ITEM_GIVE,
} EventLogEntryType;

const char* eventLogEntryTypeNames[] = {
    "FlagSet",   "FlagUnset", "SceneFlagSet", "SceneFlagUnset", "ActorInit",
    "ActorKill", "SceneInit", "RoomInit",     "OpenText",       "ItemGive",
};

struct EventLogEntry {
    std::string timestamp;
    EventLogEntryType type;
    std::string meta;
};

std::vector<EventLogEntry> eventLogEntries;

const char* flagTypeNames[] = {
    "",        "weekEventReg", "weekEventHorse", "eventInf",      "scenesVisible", "owlActivation",
    "chest",   "switch",       "clearedRoom",    "collectible",   "unk_14",        "rooms",
    "chest *", "switch *",     "clearedRoom *",  "collectible *", "randoInf",
};

#define DEFINE_ACTOR(name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_INTERNAL(_name, _enumValue, _allocType, _debugName, _humanName) { _enumValue, _debugName },
#define DEFINE_ACTOR_UNSET(_enumValue) { _enumValue, "Unset" },

std::unordered_map<s16, const char*> actorNames = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

static HOOK_ID onFlagSetHookId = 0;
static HOOK_ID onFlagUnsetHookId = 0;
static HOOK_ID onSceneFlagSetHookId = 0;
static HOOK_ID onSceneFlagUnsetHookId = 0;
static HOOK_ID onActorInitHookId = 0;
static HOOK_ID onActorKillHookId = 0;
static HOOK_ID onSceneInitHookId = 0;
static HOOK_ID onRoomInitHookId = 0;
static HOOK_ID onOpenTextHookId = 0;
static HOOK_ID onItemGiveHookId = 0;

std::string CurrentTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%I:%M:%S");
    return oss.str();
}

void TrimEventLog() {
    while (eventLogEntries.size() > CVarGetInteger("gEventLog.MaxEntries", 1000)) {
        eventLogEntries.pop_back();
    }
}

void RegisterEventLogHooks() {
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagUnset>(onFlagUnsetHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagUnset>(onSceneFlagUnsetHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorKill>(onActorKillHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnRoomInit>(onRoomInitHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnOpenText>(onOpenTextHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemGive>(onItemGiveHookId);
    onFlagSetHookId = 0;
    onFlagUnsetHookId = 0;
    onSceneFlagSetHookId = 0;
    onSceneFlagUnsetHookId = 0;
    onActorInitHookId = 0;
    onActorKillHookId = 0;
    onSceneInitHookId = 0;
    onRoomInitHookId = 0;
    onOpenTextHookId = 0;
    onItemGiveHookId = 0;

    if (!CVarGetInteger("gEventLog.Enabled", 0)) {
        return;
    }

    onFlagSetHookId =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>([](FlagType type, u32 flag) {
            if (type == FlagType::FLAG_WEEK_EVENT_REG) {
                eventLogEntries.insert(eventLogEntries.begin(),
                                       {
                                           .timestamp = CurrentTime(),
                                           .type = EVENT_LOG_ENTRY_TYPE_FLAG_SET,
                                           .meta = fmt::format("WEEKEVENTREG_{:02d}_{:02x}", flag >> 8, flag & 0xFF),
                                       });
            } else if (type == FlagType::FLAG_EVENT_INF) {
                eventLogEntries.insert(eventLogEntries.begin(), {
                                                                    .timestamp = CurrentTime(),
                                                                    .type = EVENT_LOG_ENTRY_TYPE_FLAG_SET,
                                                                    .meta = fmt::format("EVENTINF_{:02x}", flag),
                                                                });
            } else if (type == FlagType::FLAG_WEEK_EVENT_REG_HORSE_RACE) {
                eventLogEntries.insert(eventLogEntries.begin(),
                                       {
                                           .timestamp = CurrentTime(),
                                           .type = EVENT_LOG_ENTRY_TYPE_FLAG_SET,
                                           .meta = fmt::format("WEEKEVENTREG_HORSE_RACE_STATE {:03b}", flag),
                                       });
            } else {
                eventLogEntries.insert(
                    eventLogEntries.begin(),
                    {
                        .timestamp = CurrentTime(),
                        .type = EVENT_LOG_ENTRY_TYPE_FLAG_SET,
                        .meta = fmt::format("{}({:02d}) {:02x}", flagTypeNames[type], flag >> 8, flag & 0xFF),
                    });
            }
            TrimEventLog();
        });

    onFlagUnsetHookId =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagUnset>([](FlagType type, u32 flag) {
            if (type == FlagType::FLAG_WEEK_EVENT_REG) {
                eventLogEntries.insert(eventLogEntries.begin(),
                                       {
                                           .timestamp = CurrentTime(),
                                           .type = EVENT_LOG_ENTRY_TYPE_FLAG_UNSET,
                                           .meta = fmt::format("WEEKEVENTREG_{:02d}_{:02x}", flag >> 8, flag & 0xFF),
                                       });
            } else if (type == FlagType::FLAG_EVENT_INF) {
                eventLogEntries.insert(eventLogEntries.begin(), {
                                                                    .timestamp = CurrentTime(),
                                                                    .type = EVENT_LOG_ENTRY_TYPE_FLAG_UNSET,
                                                                    .meta = fmt::format("EVENTINF_{:02x}", flag),
                                                                });
            } else {
                eventLogEntries.insert(
                    eventLogEntries.begin(),
                    {
                        .timestamp = CurrentTime(),
                        .type = EVENT_LOG_ENTRY_TYPE_FLAG_UNSET,
                        .meta = fmt::format("{}({:02d}) {:02x}", flagTypeNames[type], flag >> 8, flag & 0xFF),
                    });
            }
            TrimEventLog();
        });

    onSceneFlagSetHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(
        [](s16 sceneId, FlagType type, u32 flag) {
            eventLogEntries.insert(
                eventLogEntries.begin(),
                {
                    .timestamp = CurrentTime(),
                    .type = EVENT_LOG_ENTRY_TYPE_SCENE_FLAG_SET,
                    .meta = fmt::format("{}({:02d}) {:02x}", flagTypeNames[type], flag >> 8, flag & 0xFF),
                });
            TrimEventLog();
        });

    onSceneFlagUnsetHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagUnset>(
        [](s16 sceneId, FlagType type, u32 flag) {
            eventLogEntries.insert(
                eventLogEntries.begin(),
                {
                    .timestamp = CurrentTime(),
                    .type = EVENT_LOG_ENTRY_TYPE_SCENE_FLAG_UNSET,
                    .meta = fmt::format("{}[{:02d}] {:02x}", flagTypeNames[type], flag >> 8, flag & 0xFF),
                });
            TrimEventLog();
        });

    onActorInitHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](Actor* actor) {
        eventLogEntries.insert(eventLogEntries.begin(),
                               {
                                   .timestamp = CurrentTime(),
                                   .type = EVENT_LOG_ENTRY_TYPE_ACTOR_INIT,
                                   .meta = fmt::format("{} {}", actorNames[actor->id], actor->params),
                               });
        TrimEventLog();
    });

    onActorKillHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorKill>([](Actor* actor) {
        eventLogEntries.insert(eventLogEntries.begin(),
                               {
                                   .timestamp = CurrentTime(),
                                   .type = EVENT_LOG_ENTRY_TYPE_ACTOR_KILL,
                                   .meta = fmt::format("{} {}", actorNames[actor->id], actor->params),
                               });
        TrimEventLog();
    });

    onSceneInitHookId =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s16 sceneId, s8 spawnNum) {
            eventLogEntries.insert(eventLogEntries.begin(),
                                   {
                                       .timestamp = CurrentTime(),
                                       .type = EVENT_LOG_ENTRY_TYPE_SCENE_INIT,
                                       .meta = fmt::format("{} {}", Ship_GetSceneName(sceneId), spawnNum),
                                   });
            TrimEventLog();
        });

    onRoomInitHookId =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnRoomInit>([](s16 sceneId, s8 roomId) {
            eventLogEntries.insert(eventLogEntries.begin(),
                                   {
                                       .timestamp = CurrentTime(),
                                       .type = EVENT_LOG_ENTRY_TYPE_ROOM_INIT,
                                       .meta = fmt::format("{} {}", Ship_GetSceneName(sceneId), roomId),
                                   });
            TrimEventLog();
        });

    onOpenTextHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnOpenText>(
        [](u16* textId, bool* loadFromMessageTable) {
            eventLogEntries.insert(eventLogEntries.begin(), {
                                                                .timestamp = CurrentTime(),
                                                                .type = EVENT_LOG_ENTRY_TYPE_OPEN_TEXT,
                                                                .meta = fmt::format("0x{:02x}", *textId),
                                                            });
            TrimEventLog();
        });

    onItemGiveHookId = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemGive>([](u8 item) {
        eventLogEntries.insert(eventLogEntries.begin(), {
                                                            .timestamp = CurrentTime(),
                                                            .type = EVENT_LOG_ENTRY_TYPE_ITEM_GIVE,
                                                            .meta = fmt::format("0x{:02x}", item),
                                                        });
        TrimEventLog();
    });
}

void EventLogWindow::DrawElement() {
    if (UIWidgets::CVarCheckbox("Enable", "gEventLog.Enabled")) {
        RegisterEventLogHooks();
    }

    ImGui::SameLine();

    if (UIWidgets::Button("Filters", UIWidgets::ButtonOptions{}.Size(UIWidgets::Sizes::Inline))) {
        ImGui::OpenPopup("eventLogFiltersPopup");
    }

    if (ImGui::BeginPopup("eventLogFiltersPopup")) {
        bool allChecked =
            CVarGetInteger("gEventLog.Filter.FlagSet", 1) && CVarGetInteger("gEventLog.Filter.FlagUnset", 1) &&
            CVarGetInteger("gEventLog.Filter.SceneFlagSet", 1) &&
            CVarGetInteger("gEventLog.Filter.SceneFlagUnset", 1) && CVarGetInteger("gEventLog.Filter.ActorInit", 1) &&
            CVarGetInteger("gEventLog.Filter.ActorKill", 1) && CVarGetInteger("gEventLog.Filter.SceneInit", 1) &&
            CVarGetInteger("gEventLog.Filter.RoomInit", 1) && CVarGetInteger("gEventLog.Filter.OpenText", 1) &&
            CVarGetInteger("gEventLog.Filter.ItemGive", 1);
        bool someChecked =
            CVarGetInteger("gEventLog.Filter.FlagSet", 1) || CVarGetInteger("gEventLog.Filter.FlagUnset", 1) ||
            CVarGetInteger("gEventLog.Filter.SceneFlagSet", 1) ||
            CVarGetInteger("gEventLog.Filter.SceneFlagUnset", 1) || CVarGetInteger("gEventLog.Filter.ActorInit", 1) ||
            CVarGetInteger("gEventLog.Filter.ActorKill", 1) || CVarGetInteger("gEventLog.Filter.SceneInit", 1) ||
            CVarGetInteger("gEventLog.Filter.RoomInit", 1) || CVarGetInteger("gEventLog.Filter.OpenText", 1) ||
            CVarGetInteger("gEventLog.Filter.ItemGive", 1);

        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiItemFlags backup_item_flags = g->CurrentItemFlags;
        if (!allChecked && someChecked)
            g->CurrentItemFlags |= ImGuiItemFlags_MixedValue;
        if (UIWidgets::Checkbox("All", &allChecked)) {
            if (allChecked) {
                CVarSetInteger("gEventLog.Filter.FlagSet", 1);
                CVarSetInteger("gEventLog.Filter.FlagUnset", 1);
                CVarSetInteger("gEventLog.Filter.SceneFlagSet", 1);
                CVarSetInteger("gEventLog.Filter.SceneFlagUnset", 1);
                CVarSetInteger("gEventLog.Filter.ActorInit", 1);
                CVarSetInteger("gEventLog.Filter.ActorKill", 1);
                CVarSetInteger("gEventLog.Filter.SceneInit", 1);
                CVarSetInteger("gEventLog.Filter.RoomInit", 1);
                CVarSetInteger("gEventLog.Filter.OpenText", 1);
                CVarSetInteger("gEventLog.Filter.ItemGive", 1);
            } else {
                CVarSetInteger("gEventLog.Filter.FlagSet", 0);
                CVarSetInteger("gEventLog.Filter.FlagUnset", 0);
                CVarSetInteger("gEventLog.Filter.SceneFlagSet", 0);
                CVarSetInteger("gEventLog.Filter.SceneFlagUnset", 0);
                CVarSetInteger("gEventLog.Filter.ActorInit", 0);
                CVarSetInteger("gEventLog.Filter.ActorKill", 0);
                CVarSetInteger("gEventLog.Filter.SceneInit", 0);
                CVarSetInteger("gEventLog.Filter.RoomInit", 0);
                CVarSetInteger("gEventLog.Filter.OpenText", 0);
                CVarSetInteger("gEventLog.Filter.ItemGive", 0);
            }
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
        g->CurrentItemFlags = backup_item_flags;

        UIWidgets::CVarCheckbox("FlagSet", "gEventLog.Filter.FlagSet", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("FlagUnset", "gEventLog.Filter.FlagUnset", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("SceneFlagSet", "gEventLog.Filter.SceneFlagSet", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("SceneFlagUnset", "gEventLog.Filter.SceneFlagUnset", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("ActorInit", "gEventLog.Filter.ActorInit", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("ActorKill", "gEventLog.Filter.ActorKill", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("SceneInit", "gEventLog.Filter.SceneInit", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("RoomInit", "gEventLog.Filter.RoomInit", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("OpenText", "gEventLog.Filter.OpenText", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("ItemGive", "gEventLog.Filter.ItemGive", { .defaultValue = 1 });
        ImGui::EndPopup();
    }

    ImGui::SameLine(0);

    std::string maxEntriesStr = std::string("Max Entries: ") + std::to_string(eventLogEntries.size()) + "/";
    ImGui::Text("%s", maxEntriesStr.c_str());

    ImGui::SameLine(0);

    UIWidgets::PushStyleCombobox();
    ImGui::PushItemWidth(50);
    s32 maxEntries = CVarGetInteger("gEventLog.MaxEntries", 1000);
    if (ImGui::InputScalar("##maxEntriesInput", ImGuiDataType_S32, &maxEntries)) {
        CVarSetInteger("gEventLog.MaxEntries", MAX(0, maxEntries));
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        TrimEventLog();
    }
    ImGui::PopItemWidth();
    UIWidgets::PopStyleCombobox();

    ImGui::SameLine(0);

    if (UIWidgets::Button("Clear",
                          UIWidgets::ButtonOptions{ .color = UIWidgets::Colors::Red }.Size(UIWidgets::Sizes::Inline))) {
        eventLogEntries.clear();
    }

    if (ImGui::BeginTable("events", 3, ImGuiTableFlags_ScrollY, ImVec2(0, 0))) {
        ImGui::TableSetupColumn("Timestamp", ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("Meta", ImGuiTableColumnFlags_WidthStretch, 0);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (int i = 0; i < eventLogEntries.size(); i++) {
            if (!CVarGetInteger(
                    (std::string("gEventLog.Filter.") + eventLogEntryTypeNames[eventLogEntries[i].type]).c_str(), 1)) {
                continue;
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", eventLogEntries[i].timestamp.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", eventLogEntryTypeNames[eventLogEntries[i].type]);
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", eventLogEntries[i].meta.c_str());
        }

        ImGui::EndTable();
    }
}

void EventLogWindow::InitElement() {
    RegisterEventLogHooks();
}
