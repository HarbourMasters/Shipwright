#include "actorViewer.h"
#include "../../util.h"
#include "../../UIWidgets.hpp"
#include "soh/ActorDB.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/nametag.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include <libultraship/libultraship.h>

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

#define DEBUG_ACTOR_NAMETAG_TAG "debug_actor_viewer"

typedef struct {
    u16 id;
    u16 params;
    Vec3f pos;
    Vec3s rot;
} ActorInfo;

typedef enum {
    LIST,
    TARGET,
    HELD,
    INTERACT
} RetrievalMethod;

std::array<const char*, 12> acMapping = { 
    "Switch",
    "Background (Prop type 1)",
    "Player",
    "Bomb",
    "NPC",
    "Enemy",
    "Prop type 2",
    "Item/Action",
    "Misc.",
    "Boss",
    "Door",
    "Chest"
};

typedef enum {
    ACTORVIEWER_NAMETAGS_NONE,
    ACTORVIEWER_NAMETAGS_DESC,
    ACTORVIEWER_NAMETAGS_NAME,
    ACTORVIEWER_NAMETAGS_BOTH,
} ActorViewerNameTagsType;

const std::string GetActorDescription(u16 id) {
    return ActorDB::Instance->RetrieveEntry(id).entry.valid ? ActorDB::Instance->RetrieveEntry(id).entry.desc : "???";
}

template <typename T> void DrawGroupWithBorder(T&& drawFunc) {
    // First group encapsulates the inner portion and border
    ImGui::BeginGroup();

    ImVec2 padding = ImGui::GetStyle().FramePadding;
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));

    // Second group encapsulates just the inner portion
    ImGui::BeginGroup();

    drawFunc();

    ImGui::Dummy(padding);
    ImGui::EndGroup();

    ImVec2 p1 = ImGui::GetItemRectMax();
    p1.x += padding.x;
    ImVec4 borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
    ImGui::GetWindowDrawList()->AddRect(
        p0, p1, IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255));

    ImGui::EndGroup();
}

void PopulateActorDropdown(int i, std::vector<Actor*>& data) {
    if (data.size() != 0) {
        data.clear();
    }
    if (gPlayState != nullptr) {
        ActorListEntry currList = gPlayState->actorCtx.actorLists[i];
        Actor* currAct = currList.head;
        if (currAct != nullptr) {
            while (currAct != nullptr) {
                data.push_back(currAct);
                currAct = currAct->next;
            }
        }
    }
}

void ActorViewer_AddTagForActor(Actor* actor) {
    int val = CVarGetInteger("gDebugActorViewerNameTags", ACTORVIEWER_NAMETAGS_NONE);
    auto entry = ActorDB::Instance->RetrieveEntry(actor->id);
    std::string tag;

    if (val > 0 && entry.entry.valid) {
        switch (val) {
            case ACTORVIEWER_NAMETAGS_DESC:
                tag = entry.desc;
                break;
            case ACTORVIEWER_NAMETAGS_NAME:
                tag = entry.name;
                break;
            case ACTORVIEWER_NAMETAGS_BOTH:
                tag = entry.name + '\n' + entry.desc;
                break;
        }

        NameTag_RegisterForActorWithOptions(actor, tag.c_str(), { .tag = DEBUG_ACTOR_NAMETAG_TAG });
    }
}

void ActorViewer_AddTagForAllActors() {
    if (gPlayState == nullptr) {
        return;
    }

    for (size_t i = 0; i < ARRAY_COUNT(gPlayState->actorCtx.actorLists); i++) {
        ActorListEntry currList = gPlayState->actorCtx.actorLists[i];
        Actor* currAct = currList.head;
        while (currAct != nullptr) {
            ActorViewer_AddTagForActor(currAct);
            currAct = currAct->next;
        }
    }
}

void ActorViewerWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Actor Viewer", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    static Actor* display;
    static Actor empty{};
    static Actor* fetch = NULL;
    static ActorInfo newActor = {0,0, {0, 0, 0}, {0, 0, 0}};
    static bool needs_reset = false;
    static ImU16 one = 1;
    static int actor;
    static int category = 0;
    static RetrievalMethod rm;
    static std::string filler = "Please select";
    static std::vector<Actor*> list;
    static u16 lastSceneId = 0;

    if (gPlayState != nullptr) {
        needs_reset = lastSceneId != gPlayState->sceneNum;
        if (needs_reset) {
            display = &empty;
            fetch = nullptr;
            actor = category = 0;
            filler = "Please Select";
            list.clear();
            needs_reset = false;
        }
        lastSceneId = gPlayState->sceneNum;
        if (ImGui::BeginCombo("Actor Type", acMapping[category])) {
            for (int i = 0; i < acMapping.size(); i++) {
                if (ImGui::Selectable(acMapping[i])) {
                    category = i;
                    PopulateActorDropdown(category, list);
                    break;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Actor", filler.c_str())) {
            if (gPlayState != nullptr && lastSceneId != gPlayState->sceneNum) {
                PopulateActorDropdown(category, list);
                lastSceneId = gPlayState->sceneNum;
            }
            for (int i = 0; i < list.size(); i++) {
                std::string label = std::to_string(i) + ": " + ActorDB::Instance->RetrieveEntry(list[i]->id).name;
                std::string description = GetActorDescription(list[i]->id);
                if (description != "")
                    label += " (" + description + ")";

                if (ImGui::Selectable(label.c_str())) {
                    rm = LIST;
                    display = list[i];
                    actor = i;
                    filler = label;
                    break;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::TreeNode("Selected Actor")) {
            DrawGroupWithBorder([&]() {
                ImGui::Text("Name: %s", ActorDB::Instance->RetrieveEntry(display->id).name.c_str());
                ImGui::Text("Description: %s", GetActorDescription(display->id).c_str());
                ImGui::Text("Category: %s", acMapping[display->category]);
                ImGui::Text("ID: %d", display->id);
                ImGui::Text("Parameters: %d", display->params);
            });

            ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

            DrawGroupWithBorder([&]() {
                ImGui::Text("Actor Position");
                ImGui::InputScalar("x pos", ImGuiDataType_Float, &display->world.pos.x);
                ImGui::SameLine();
                ImGui::InputScalar("y pos", ImGuiDataType_Float, &display->world.pos.y);
                ImGui::SameLine();
                ImGui::InputScalar("z pos", ImGuiDataType_Float, &display->world.pos.z);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("Actor Rotation");
                ImGui::InputScalar("x rot", ImGuiDataType_S16, &display->world.rot.x);
                ImGui::SameLine();
                ImGui::InputScalar("y rot", ImGuiDataType_S16, &display->world.rot.y);
                ImGui::SameLine();
                ImGui::InputScalar("z rot", ImGuiDataType_S16, &display->world.rot.z);
            });

            if (display->category == ACTORCAT_BOSS || display->category == ACTORCAT_ENEMY) {
                ImGui::InputScalar("Enemy Health", ImGuiDataType_U8, &display->colChkInfo.health);
                UIWidgets::InsertHelpHoverText("Some actors might not use this!");
            }

            DrawGroupWithBorder([&]() {
                ImGui::Text("flags");
                UIWidgets::DrawFlagArray32("flags", display->flags);
            });

            ImGui::SameLine();

            DrawGroupWithBorder([&]() {
                ImGui::Text("bgCheckFlags");
                UIWidgets::DrawFlagArray16("bgCheckFlags", display->bgCheckFlags);
            });

            if (ImGui::Button("Refresh")) {
                PopulateActorDropdown(category, list);
                switch (rm) {
                    case INTERACT:
                    case HELD:
                    case TARGET:
                        display = fetch;
                        break;
                    case LIST:
                        display = list[actor];
                        break;
                    default:
                        break;
                }
            }

            if (ImGui::Button("Go to Actor")) {
                Player* player = GET_PLAYER(gPlayState);
                Math_Vec3f_Copy(&player->actor.world.pos, &display->world.pos);
                Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
            }

            if (ImGui::Button("Fetch from Target")) {
                Player* player = GET_PLAYER(gPlayState);
                fetch = player->targetActor;
                if (fetch != NULL) {
                    display = fetch;
                    category = fetch->category;
                    PopulateActorDropdown(category, list);
                    rm = TARGET;
                }
            }
            UIWidgets::InsertHelpHoverText("Grabs actor with target arrow above it. You might need C-Up for enemies");
            if (ImGui::Button("Fetch from Held")) {
                Player* player = GET_PLAYER(gPlayState);
                fetch = player->heldActor;
                if (fetch != NULL) {
                    display = fetch;
                    category = fetch->category;
                    PopulateActorDropdown(category, list);
                    rm = HELD;
                }
            }
            UIWidgets::InsertHelpHoverText("Grabs actor that Link is holding");
            if (ImGui::Button("Fetch from Interaction")) {
                Player* player = GET_PLAYER(gPlayState);
                fetch = player->interactRangeActor;
                if (fetch != NULL) {
                    display = fetch;
                    category = fetch->category;
                    PopulateActorDropdown(category, list);
                    rm = INTERACT;
                }
            }
            UIWidgets::InsertHelpHoverText("Grabs actor from \"interaction range\"");

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("New...")) {
            ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

            ImGui::Text("%s", GetActorDescription(newActor.id).c_str());
            ImGui::InputScalar("ID", ImGuiDataType_S16, &newActor.id, &one);
            ImGui::InputScalar("params", ImGuiDataType_S16, &newActor.params, &one);

            ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

            DrawGroupWithBorder([&]() {
                ImGui::Text("New Actor Position");
                ImGui::InputScalar("posX", ImGuiDataType_Float, &newActor.pos.x);
                ImGui::SameLine();
                ImGui::InputScalar("posY", ImGuiDataType_Float, &newActor.pos.y);
                ImGui::SameLine();
                ImGui::InputScalar("posZ", ImGuiDataType_Float, &newActor.pos.z);
            });

            DrawGroupWithBorder([&]() {
                ImGui::Text("New Actor Rotation");
                ImGui::InputScalar("rotX", ImGuiDataType_S16, &newActor.rot.x);
                ImGui::SameLine();
                ImGui::InputScalar("rotY", ImGuiDataType_S16, &newActor.rot.y);
                ImGui::SameLine();
                ImGui::InputScalar("rotZ", ImGuiDataType_S16, &newActor.rot.z);
            });

            if (ImGui::Button("Fetch from Link")) {
                Player* player = GET_PLAYER(gPlayState);
                Vec3f newPos = player->actor.world.pos;
                Vec3s newRot = player->actor.world.rot;
                newActor.pos = newPos;
                newActor.rot = newRot;
            }

            if (ImGui::Button("Spawn")) {
                if (ActorDB::Instance->RetrieveEntry(newActor.id).entry.valid) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, newActor.id, newActor.pos.x, newActor.pos.y,
                                newActor.pos.z, newActor.rot.x, newActor.rot.y, newActor.rot.z, newActor.params, 0);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
            }

            if (ImGui::Button("Spawn as Child")) {
                Actor* parent = display;
                if (parent != NULL) {
                    if (newActor.id >= 0 && newActor.id < ACTOR_ID_MAX &&
                        ActorDB::Instance->RetrieveEntry(newActor.id).entry.valid) {
                        Actor_SpawnAsChild(&gPlayState->actorCtx, parent, gPlayState, newActor.id, newActor.pos.x,
                                           newActor.pos.y, newActor.pos.z, newActor.rot.x, newActor.rot.y,
                                           newActor.rot.z, newActor.params);
                    } else {
                        func_80078884(NA_SE_SY_ERROR);
                    }
                }
            }

            if (ImGui::Button("Reset")) {
                newActor = { 0, 0, { 0, 0, 0 }, { 0, 0, 0 } };
            }

            ImGui::TreePop();
        }

        static const char* nameTagOptions[] = {
            "None",
            "Short Description",
            "Actor ID",
            "Both"
        };

        UIWidgets::Spacer(0);

        ImGui::Text("Actor Name Tags");
        if (UIWidgets::EnhancementCombobox("gDebugActorViewerNameTags", nameTagOptions, ACTORVIEWER_NAMETAGS_NONE)) {
            NameTag_RemoveAllByTag(DEBUG_ACTOR_NAMETAG_TAG);
            ActorViewer_AddTagForAllActors();
        }
        UIWidgets::Tooltip("Adds \"name tags\" above actors for identification");
    } else {
        ImGui::Text("Global Context needed for actor info!");
        if (needs_reset) {
            fetch = nullptr;
            actor = category = 0;
            filler = "Please Select";
            list.clear();
            needs_reset = false;
        }
    }

    ImGui::End();
}

void ActorViewerWindow::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>([](void* refActor) {
        Actor* actor = static_cast<Actor*>(refActor);
        ActorViewer_AddTagForActor(actor);
    });
}
