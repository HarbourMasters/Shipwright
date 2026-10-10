#include "ActorViewer.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/NameTag/NameTag.h"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include <spdlog/fmt/fmt.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64actor.h"
#include "functions.h"
#include "variables.h"
}

typedef struct ActorInfo {
    u16 id;
    u16 params;
    Vec3f pos;
    Vec3s rot;
} ActorInfo;

#define DEBUG_ACTOR_NAMETAG_TAG "debug_actor_viewer"
#define CVAR_ACTOR_NAME_TAGS(val) "gDeveloperTools.ActorNameTags." val

std::vector<Actor*> GetCurrentSceneActors() {
    if (!gPlayState) {
        return {};
    }

    std::vector<Actor*> sceneActors;
    for (size_t category = ACTORCAT_SWITCH; category < ACTORCAT_MAX; category++) {
        Actor* currentActor = gPlayState->actorCtx.actorLists[category].first;
        if (currentActor != nullptr) {
            while (currentActor != nullptr) {
                sceneActors.push_back(currentActor);
                currentActor = currentActor->next;
            }
        }
    }
    return sceneActors;
}

static Actor* selectedActor = nullptr;
static ActorInfo newActor;
static s16 lastSceneId = -1;
static std::string comboBoxLabel = "Please select";

static HOOK_ID preventActorDrawHookId = 0;
static HOOK_ID preventActorUpdateHookId = 0;
static HOOK_ID actorDeleteHookID = 0;

void ResetVariables() {
    selectedActor = nullptr;
    comboBoxLabel = "Please select";
    GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::ShouldActorDraw>(preventActorDrawHookId);
    GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::ShouldActorUpdate>(preventActorUpdateHookId);
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorDestroy>(actorDeleteHookID);
    preventActorDrawHookId = 0;
    preventActorUpdateHookId = 0;
    actorDeleteHookID = 0;
}

void SetSelectedActor(Actor* actor) {
    ResetVariables();
    selectedActor = actor;

    actorDeleteHookID = GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::OnActorDestroy>(
        (uintptr_t)selectedActor, [](Actor* _) { ResetVariables(); });
}

void ActorViewer_AddTagForActor(Actor* actor) {
    if (!CVarGetInteger(CVAR_ACTOR_NAME_TAGS("Enabled"), 0)) {
        return;
    }

    std::vector<std::string> parts;

    if (CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayID"), 0)) {
        parts.push_back(GetActorDebugName(actor->id));
    }
    if (CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayDescription"), 0)) {
        parts.push_back(GetActorDescription(actor->id));
    }
    if (CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayCategory"), 0)) {
        parts.push_back(GetActorCategoryName(actor->category));
    }
    if (CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayParams"), 0)) {
        parts.push_back(fmt::format("0x{:04X} ({})", (u16)actor->params, actor->params));
    }
    if (CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayActorListIndex"), 0)) {
        parts.push_back(fmt::format("{}", GetActorListIndex(actor)));
    }

    std::string tag = "";
    for (size_t i = 0; i < parts.size(); i++) {
        if (i != 0) {
            tag += "\n";
        }
        tag += parts.at(i);
    }

    bool withZBuffer = CVarGetInteger(CVAR_ACTOR_NAME_TAGS("WithZBuffer"), 0);

    NameTag_RegisterForActorWithOptions(actor, tag.c_str(),
                                        { .tag = DEBUG_ACTOR_NAMETAG_TAG, .noZBuffer = !withZBuffer });
}

void ActorViewer_AddTagForAllActors() {
    if (gPlayState == nullptr) {
        return;
    }

    for (size_t i = 0; i < ARRAY_COUNT(gPlayState->actorCtx.actorLists); i++) {
        ActorListEntry currList = gPlayState->actorCtx.actorLists[i];
        Actor* currAct = currList.first;
        while (currAct != nullptr) {
            ActorViewer_AddTagForActor(currAct);
            currAct = currAct->next;
        }
    }
}

void ActorViewer_RegisterNameTagHooks() {
    static HOOK_ID actorInitHookID = 0;
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(actorInitHookID);
    actorInitHookID = 0;

    if (!CVarGetInteger(CVAR_ACTOR_NAME_TAGS("Enabled"), 0)) {
        return;
    }

    actorInitHookID =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(ActorViewer_AddTagForActor);
}

void ActorViewerWindow::UpdateElement() {
    static bool lastVisibleState = mIsVisible;

    if (!mIsVisible && lastVisibleState != mIsVisible) {
        ResetVariables();
    }

    lastVisibleState = mIsVisible;
}

void ActorViewerWindow::DrawElement() {
    if (gPlayState != nullptr) {
        if (lastSceneId != gPlayState->sceneId) {
            ResetVariables();
            lastSceneId = gPlayState->sceneId;
        }

        if (ImGui::BeginChild("options", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY)) {
            bool toggled = false;
            bool optionChange = false;

            ImGui::SeparatorText("Options");

            toggled = UIWidgets::CVarCheckbox("Actor Name Tags", CVAR_ACTOR_NAME_TAGS("Enabled"),
                                              { { .tooltip = "Adds \"name tags\" above actors for identification" } });

            ImGui::SameLine();

            UIWidgets::Button("Display Items", { { .tooltip = "Click to add display items on the name tags" } });

            if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft | ImGuiPopupFlags_NoReopen)) {
                optionChange |= UIWidgets::CVarCheckbox("ID", CVAR_ACTOR_NAME_TAGS("DisplayID"));
                optionChange |= UIWidgets::CVarCheckbox("Description", CVAR_ACTOR_NAME_TAGS("DisplayDescription"));
                optionChange |= UIWidgets::CVarCheckbox("Category", CVAR_ACTOR_NAME_TAGS("DisplayCategory"));
                optionChange |= UIWidgets::CVarCheckbox("Params", CVAR_ACTOR_NAME_TAGS("DisplayParams"));
                optionChange |=
                    UIWidgets::CVarCheckbox("Actor List Index", CVAR_ACTOR_NAME_TAGS("DisplayActorListIndex"));

                ImGui::EndPopup();
            }

            optionChange |= UIWidgets::CVarCheckbox(
                "Name tags with Z-Buffer", CVAR_ACTOR_NAME_TAGS("WithZBuffer"),
                { { .tooltip = "Allow name tags to be obstructed when behind geometry and actors" } });

            if (toggled || optionChange) {
                bool tagsEnabled = CVarGetInteger(CVAR_ACTOR_NAME_TAGS("Enabled"), 0);
                bool noOptionsEnabled = !CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayID"), 0) &&
                                        !CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayDescription"), 0) &&
                                        !CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayCategory"), 0) &&
                                        !CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayParams"), 0) &&
                                        !CVarGetInteger(CVAR_ACTOR_NAME_TAGS("DisplayActorListIndex"), 0);

                // Save the user an extra click and prevent adding "empty" tags by enabling,
                // disabling, or setting an option based on what changed
                if (tagsEnabled && noOptionsEnabled) {
                    if (toggled) {
                        CVarSetInteger(CVAR_ACTOR_NAME_TAGS("DisplayID"), 1);
                    } else {
                        CVarSetInteger(CVAR_ACTOR_NAME_TAGS("Enabled"), 0);
                    }
                } else if (optionChange && !tagsEnabled && !noOptionsEnabled) {
                    CVarSetInteger(CVAR_ACTOR_NAME_TAGS("Enabled"), 1);
                }

                ActorViewer_RegisterNameTagHooks();
                NameTag_RemoveAllByTag(DEBUG_ACTOR_NAMETAG_TAG);
                ActorViewer_AddTagForAllActors();
            }
        }
        ImGui::EndChild();

        if (ImGui::TreeNode("Select existing Actor")) {
            if (ImGui::BeginCombo("Actor", comboBoxLabel.c_str())) {
                auto list = GetCurrentSceneActors();
                lastSceneId = gPlayState->sceneId;

                if (!list.empty()) {
                    u8 count = 0;
                    u8 prev_category = 0xff;
                    for (size_t i = 0; i < list.size(); i++) {
                        std::string label = GetActorCategoryName(list[i]->category);
                        if (list[i]->category != prev_category) {
                            prev_category = list[i]->category;
                            count = 1;
                        } else {
                            count++;
                        }
                        label += " " + std::to_string(count) + ": " + GetActorDescription(list[i]->id);
                        if (ImGui::Selectable(label.c_str())) {
                            SetSelectedActor(list[i]);
                            comboBoxLabel = label;

                            break;
                        }
                    }
                }
                ImGui::EndCombo();
            }

            if (selectedActor != nullptr) {
                ImGui::BeginGroup();
                ImGui::Text("ID: %hd", selectedActor->id);
                ImGui::Text("Description: %s", GetActorDescription(selectedActor->id).c_str());
                ImGui::Text("Category: %s", GetActorCategoryName(selectedActor->category).c_str());
                ImGui::Text("Params: %hd", selectedActor->params);
                ImGui::EndGroup();

                ImGui::BeginGroup();
                if (preventActorDrawHookId) {
                    if (ImGui::Button("Continue Drawing", ImVec2(ImGui::GetFontSize() * 10, 0))) {
                        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::ShouldActorDraw>(
                            preventActorDrawHookId);
                        preventActorDrawHookId = 0;
                    }
                } else {
                    if (ImGui::Button("Stop Drawing", ImVec2(ImGui::GetFontSize() * 10, 0))) {
                        preventActorDrawHookId =
                            GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::ShouldActorDraw>(
                                (uintptr_t)selectedActor, [](Actor* _, bool* result) { *result = false; });
                    }
                }
                ImGui::SameLine();
                if (preventActorUpdateHookId) {
                    if (ImGui::Button("Continue Updating", ImVec2(ImGui::GetFontSize() * 10, 0))) {
                        GameInteractor::Instance->UnregisterGameHookForPtr<GameInteractor::ShouldActorUpdate>(
                            preventActorUpdateHookId);
                        preventActorUpdateHookId = 0;
                    }
                } else {
                    if (ImGui::Button("Stop Updating", ImVec2(ImGui::GetFontSize() * 10, 0))) {
                        preventActorUpdateHookId =
                            GameInteractor::Instance->RegisterGameHookForPtr<GameInteractor::ShouldActorUpdate>(
                                (uintptr_t)selectedActor, [](Actor* _, bool* result) { *result = false; });
                    }
                }
                ImGui::EndGroup();

                ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

                ImGui::BeginGroup();
                ImGui::Text("Actor Position");
                ImGui::InputScalar("x", ImGuiDataType_Float, &selectedActor->world.pos.x);
                ImGui::SameLine();
                ImGui::InputScalar("y", ImGuiDataType_Float, &selectedActor->world.pos.y);
                ImGui::SameLine();
                ImGui::InputScalar("z", ImGuiDataType_Float, &selectedActor->world.pos.z);
                ImGui::EndGroup();

                ImGui::BeginGroup();
                ImGui::Text("Actor Rotation");
                ImGui::InputScalar("rx", ImGuiDataType_S16, &selectedActor->world.rot.x);
                ImGui::SameLine();
                ImGui::InputScalar("ry", ImGuiDataType_S16, &selectedActor->world.rot.y);
                ImGui::SameLine();
                ImGui::InputScalar("rz", ImGuiDataType_S16, &selectedActor->world.rot.z);
                ImGui::EndGroup();

                ImGui::PopItemWidth();
                ImGui::PushItemWidth(ImGui::GetFontSize() * 5);

                if ((selectedActor->category == ACTORCAT_BOSS || selectedActor->category == ACTORCAT_ENEMY) &&
                    selectedActor->colChkInfo.health > 0) {
                    ImGui::InputScalar("Enemy Health", ImGuiDataType_U8, &selectedActor->colChkInfo.health);
                }
                ImGui::PopItemWidth();

                ImGui::BeginGroup();
                ImGui::Text("Flags");
                UIWidgets::DrawFlagArray32("flags", selectedActor->flags);
                ImGui::EndGroup();

                ImGui::BeginGroup();
                ImGui::Text("BG Check");
                UIWidgets::DrawFlagArray16("bgCheckFlags", selectedActor->bgCheckFlags);
                ImGui::EndGroup();
            }

            if (UIWidgets::Button("Go to Actor")) {
                Player* player = GET_PLAYER(gPlayState);
                if (selectedActor != nullptr) {
                    Math_Vec3f_Copy(&player->actor.world.pos, &selectedActor->world.pos);
                    Math_Vec3f_Copy(&player->actor.home.pos, &selectedActor->world.pos);
                }
            }

            if (UIWidgets::Button("Fetch: Target", { { .tooltip = "Grabs actor with target arrow above it." } })) {
                Player* player = GET_PLAYER(gPlayState);
                if (player->focusActor != nullptr) {
                    SetSelectedActor(player->focusActor);
                }
            }
            if (UIWidgets::Button("Fetch: Held", { { .tooltip = "Grabs actor Link is currently holding." } })) {
                Player* player = GET_PLAYER(gPlayState);
                if (player->heldActor != nullptr) {
                    SetSelectedActor(player->heldActor);
                }
            }

            if (UIWidgets::Button("Kill", { .color = UIWidgets::Colors::Red }) && selectedActor != nullptr &&
                selectedActor->id != ACTOR_PLAYER) {
                Actor_Kill(selectedActor);
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Spawn new Actor")) {
            ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
            ImU16 one = 1;

            ImGui::Text("%s", GetActorDescription(newActor.id).c_str());
            ImGui::InputScalar("ID", ImGuiDataType_S16, &newActor.id, &one);
            ImGui::InputScalar("Params", ImGuiDataType_S16, &newActor.params, &one);

            ImGui::BeginGroup();
            ImGui::Text("New Actor Position");
            ImGui::InputScalar("x", ImGuiDataType_Float, &newActor.pos.x);
            ImGui::SameLine();
            ImGui::InputScalar("y", ImGuiDataType_Float, &newActor.pos.y);
            ImGui::SameLine();
            ImGui::InputScalar("z", ImGuiDataType_Float, &newActor.pos.z);
            ImGui::EndGroup();

            ImGui::BeginGroup();
            ImGui::Text("New Actor Rotation");
            ImGui::InputScalar("rX", ImGuiDataType_S16, &newActor.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("rY", ImGuiDataType_S16, &newActor.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("rZ", ImGuiDataType_S16, &newActor.rot.z);
            ImGui::EndGroup();

            UIWidgets::CVarCheckbox("Disable Object Dependency", "gDeveloperTools.DisableObjectDependency");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextColored(
                    ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                    "Warning: This will cause some actors to spawn in places they normally wouldn't,\nplease ensure "
                    "this is disabled before reporting any actor related issues");
                ImGui::EndTooltip();
            }

            if (UIWidgets::Button("Fetch from Link")) {
                Player* player = GET_PLAYER(gPlayState);
                newActor.pos = player->actor.world.pos;
                newActor.rot = player->actor.world.rot;
            }

            if (UIWidgets::Button("Spawn", { .color = UIWidgets::Colors::Green })) {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, newActor.id, newActor.pos.x, newActor.pos.y,
                            newActor.pos.z, newActor.rot.x, newActor.rot.y, newActor.rot.z, newActor.params);
            }

            if (UIWidgets::Button("Spawn as Child", { .color = UIWidgets::Colors::Green })) {
                Actor* parent = selectedActor;
                if (parent != nullptr && parent->child == nullptr) {
                    Actor_SpawnAsChild(&gPlayState->actorCtx, parent, gPlayState, newActor.id, newActor.pos.x,
                                       newActor.pos.y, newActor.pos.z, newActor.rot.x, newActor.rot.y, newActor.rot.z,
                                       newActor.params);
                } else {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                }
            }

            if (UIWidgets::Button("Reset")) {
                newActor = {};
            }

            ImGui::TreePop();
        }
    } else {
        ImGui::Text("Playstate needed for actors!");
    }
}

void ActorViewerWindow::InitElement() {
    ActorViewer_RegisterNameTagHooks();
}
