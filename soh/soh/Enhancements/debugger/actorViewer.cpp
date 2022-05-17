#include "actorViewer.h"
#include "../../util.h"
#include "../libultraship/SohImGuiImpl.h"
#include "ImGuiHelpers.h"

#include <array>
#include <bit>
#include <map>
#include <string>

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

typedef struct {
    u16 id;
    u16 params;
    Vec3f pos;
    Vec3s rot;
} ActorInfo;

std::vector<std::string> acMapping = { "Switch",      "Background (Prop type 1)",
                                       "Player",      "Bomb",
                                       "NPC",         "Enemy",
                                       "Prop type 2", "Item/Action",
                                       "Misc.",       "Boss",
                                       "Door",        "Chest" };

std::vector<Actor*> PopulateActorDropdown(int i, std::vector<Actor*> data) {
    if (gGlobalCtx == nullptr) {
        return data;
    }
    if (data.size() != 0) {
        data.clear();
    }
    ActorListEntry currList = gGlobalCtx->actorCtx.actorLists[i];
    Actor* currAct = currList.head;
    if (currAct != nullptr) {
        while (currAct != nullptr) {
            data.push_back(currAct);
            currAct = currAct->next;
        }
    }
    return data;
}

void DrawActorViewer(bool& open) {
    if (!open) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Actor Viewer", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    static int category = 0;
    static std::vector<Actor*> list;
    static Actor display;
    static ActorOverlay* dispOverlay;

    if (ImGui::BeginCombo("Actor Type", acMapping[category].c_str())) {
        for (int i = 0; i < acMapping.size(); i++) {
            if (ImGui::Selectable(acMapping[i].c_str())) {
                category = i;
                list = PopulateActorDropdown(category, list);
                break;
            }
        }
        ImGui::EndCombo();
    }

    static std::string filler = "Please select";
    if (ImGui::BeginCombo("Actor", filler.c_str())) {
        for (int i = 0; i < list.size(); i++) {
            std::string label = acMapping[category] + " Actor " + std::to_string(i);
            if (ImGui::Selectable(label.c_str())) {
                display = *list[i];
                filler = label;
                break;
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::TreeNode("Selected Actor")) {
        dispOverlay = display.overlayEntry;
        ImGui::Text("Name: %s", dispOverlay != nullptr ? dispOverlay->name : "???");
        ImGui::Text("ID: %d", display.id);
        ImGui::Text("Parameters: %d", display.params);
        ImGui::Text("Actor Position");
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);
        ImGui::InputScalar("x pos", ImGuiDataType_Float, &display.world.pos.x);
        ImGui::SameLine();
        ImGui::InputScalar("y pos", ImGuiDataType_Float, &display.world.pos.y);
        ImGui::SameLine();
        ImGui::InputScalar("z pos", ImGuiDataType_Float, &display.world.pos.z);

        ImGui::Text("Actor Rotation");
        ImGui::InputScalar("x rot", ImGuiDataType_S16, &display.world.rot.x);
        ImGui::SameLine();
        ImGui::InputScalar("y rot", ImGuiDataType_S16, &display.world.rot.y);
        ImGui::SameLine();
        ImGui::InputScalar("z rot", ImGuiDataType_S16, &display.world.rot.z);

        if (ImGui::Button("Kill")) {
            Actor_Delete(&gGlobalCtx->actorCtx, &display, gGlobalCtx);
        }

        if (ImGui::Button("Refresh")) {
            PopulateActorDropdown(category, list);
        }

        if (ImGui::Button("Go to Actor")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            player->actor.world.pos = display.world.pos;
            player->actor.world.rot = display.world.rot;
        }

        ImGui::TreePop();
    }
    
    
    static ActorInfo newActor = { 0, 0, { 0, 0, 0 }, {0,0,0} };
    if (ImGui::TreeNode("New...")) {
        ImGui::PushItemWidth(ImGui::GetFontSize() * 10);
        ImU16 one = 1;
        ImGui::InputScalar("ID", ImGuiDataType_U16, &newActor.id, &one);
        ImGui::InputScalar("params", ImGuiDataType_U16, &newActor.params, &one);
        ImGui::InputScalar("posX", ImGuiDataType_Float, &newActor.pos.x);
        ImGui::SameLine();
        ImGui::InputScalar("posY", ImGuiDataType_Float, &newActor.pos.y);
        ImGui::SameLine();
        ImGui::InputScalar("posZ", ImGuiDataType_Float, &newActor.pos.z);

        ImGui::InputScalar("rotX", ImGuiDataType_S16, &newActor.rot.x);
        ImGui::SameLine();
        ImGui::InputScalar("rotY", ImGuiDataType_S16, &newActor.rot.y);
        ImGui::SameLine();
        ImGui::InputScalar("rotZ", ImGuiDataType_S16, &newActor.rot.z);

        if (ImGui::Button("Fetch from Link")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            Vec3f newPos = player->actor.world.pos;
            Vec3s newRot = player->actor.world.rot;
            newActor.pos = newPos;
            newActor.rot = newRot;
        }

        if (ImGui::Button("Spawn")) {
            Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, newActor.id, newActor.pos.x, newActor.pos.y, newActor.pos.z,
                        newActor.rot.x, newActor.rot.y, newActor.rot.z, newActor.params);
        }

        if (ImGui::Button("Spawn as Child")) {
            Actor_SpawnAsChild(&gGlobalCtx->actorCtx, gGlobalCtx->actorCtx.actorLists[category].head, gGlobalCtx, newActor.id, newActor.pos.x, newActor.pos.y,
                               newActor.pos.z, newActor.rot.x, newActor.rot.y, newActor.rot.z, newActor.params);
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void InitActorViewer() {
    SohImGui::AddWindow("Developer Tools", "Actor Viewer", DrawActorViewer);
}