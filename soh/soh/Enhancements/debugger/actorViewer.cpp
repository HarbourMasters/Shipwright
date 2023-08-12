#include "actorViewer.h"
#include "../../util.h"
#include "../../UIWidgets.hpp"
#include "soh/ActorDB.h"

#include <array>
#include <bit>
#include <map>
#include <unordered_map>
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

#define DEKUNUTS_FLOWER 10

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

//actors that don't use params at all
static std::vector<u16> noParamsActors = {
    ACTOR_ARMS_HOOK,
    ACTOR_ARROW_FIRE,
    ACTOR_ARROW_ICE,
    ACTOR_ARROW_LIGHT,
    ACTOR_BG_BOM_GUARD,
    ACTOR_BG_DY_YOSEIZO,
    ACTOR_BG_GATE_SHUTTER,
    ACTOR_BG_GJYO_BRIDGE,
    ACTOR_BG_HIDAN_FSLIFT,
    ACTOR_BG_HIDAN_RSEKIZOU,
    ACTOR_BG_HIDAN_SYOKU,
    ACTOR_BG_JYA_GOROIWA,
    ACTOR_BG_MIZU_UZU,
    ACTOR_BG_MORI_RAKKATENJO,
    ACTOR_BG_PUSHBOX,
    ACTOR_BG_SPOT01_FUSYA,
    ACTOR_BG_SPOT01_IDOHASHIRA,
    ACTOR_BG_SPOT01_IDOMIZU,
    ACTOR_BG_SPOT01_IDOSOKO,
    ACTOR_BG_SPOT11_OASIS,
    ACTOR_BG_SPOT15_SAKU,
    ACTOR_BG_SPOT18_FUTA,
    ACTOR_BG_TOKI_SWD,
    ACTOR_BG_TREEMOUTH,
    ACTOR_BG_VB_SIMA,
    ACTOR_BOSS_DODONGO,
    ACTOR_BOSS_FD,
    ACTOR_BOSS_GOMA,
    ACTOR_DEMO_EXT,
    ACTOR_DEMO_SHD,
    ACTOR_DEMO_TRE_LGT,
    ACTOR_DOOR_TOKI,
    ACTOR_EFC_ERUPC,
    ACTOR_EN_ANI,
    ACTOR_EN_AROW_TRAP,
    ACTOR_EN_BLKOBJ,
    ACTOR_EN_BOM_BOWL_MAN,
    ACTOR_EN_BOM_BOWL_PIT,
    ACTOR_EN_BOM_CHU,
    ACTOR_EN_BUBBLE,
    ACTOR_EN_DIVING_GAME,
    ACTOR_EN_DNT_DEMO,
    ACTOR_EN_DNT_JIJI,
    ACTOR_EN_DS,
    ACTOR_EN_DU,
    ACTOR_EN_EG,
    ACTOR_EN_FU,
    ACTOR_EN_GB,
    ACTOR_EN_GE3,
    ACTOR_EN_GUEST,
    ACTOR_EN_HATA,
    ACTOR_EN_HORSE_GANON,
    ACTOR_EN_HORSE_LINK_CHILD,
    ACTOR_EN_HORSE_ZELDA,
    ACTOR_EN_HS2,
    ACTOR_EN_JS,
    ACTOR_EN_KAKASI,
    ACTOR_EN_KAKASI3,
    ACTOR_EN_MA1,
    ACTOR_EN_MA2,
    ACTOR_EN_MA3,
    ACTOR_EN_MAG,
    ACTOR_EN_MK,
    ACTOR_EN_MS,
    ACTOR_EN_NIW_LADY,
    ACTOR_EN_NWC,
    ACTOR_EN_OE2,
    ACTOR_EN_OKARINA_EFFECT,
    ACTOR_EN_SA,
    ACTOR_EN_SCENE_CHANGE,
    ACTOR_EN_SKJNEEDLE,
    ACTOR_EN_SYATEKI_ITM,
    ACTOR_EN_SYATEKI_MAN,
    ACTOR_EN_TAKARA_MAN,
    ACTOR_EN_TORYO,
    ACTOR_EN_VASE,
    ACTOR_EN_ZL1,
    ACTOR_MAGIC_DARK,
    ACTOR_MAGIC_FIRE,
    ACTOR_OBJ_DEKUJR,
    ACTOR_OCEFF_SPOT,

    ACTOR_UNSET_1,
    ACTOR_UNSET_3,
    ACTOR_UNSET_5,
    ACTOR_UNSET_6,
    ACTOR_UNSET_17,
    ACTOR_UNSET_1A,
    ACTOR_UNSET_1F,
    ACTOR_UNSET_22,
    ACTOR_UNSET_31,
    ACTOR_UNSET_36,
    ACTOR_UNSET_53,
    ACTOR_UNSET_73,
    ACTOR_UNSET_74,
    ACTOR_UNSET_75,
    ACTOR_UNSET_76,
    ACTOR_UNSET_78,
    ACTOR_UNSET_79,
    ACTOR_UNSET_7A,
    ACTOR_UNSET_7B,
    ACTOR_UNSET_7E,
    ACTOR_UNSET_7F,
    ACTOR_UNSET_83,
    ACTOR_UNSET_A0,
    ACTOR_UNSET_B2,
    ACTOR_UNSET_CE,
    ACTOR_UNSET_D8,
    ACTOR_UNSET_EA,
    ACTOR_UNSET_EB,
    ACTOR_UNSET_F2,
    ACTOR_UNSET_F3,
    ACTOR_UNSET_FB,
    ACTOR_UNSET_109,
    ACTOR_UNSET_10D,
    ACTOR_UNSET_10E,
    ACTOR_UNSET_128,
    ACTOR_UNSET_129,
    ACTOR_UNSET_134,
    ACTOR_UNSET_154,
    ACTOR_UNSET_15D,
    ACTOR_UNSET_161,
    ACTOR_UNSET_180,
    ACTOR_UNSET_1AA
};

static std::unordered_map<u16, std::function<s16(s16)>> actorSpecificData;

void CreateActorSpecificData() {
    if (!actorSpecificData.empty()) {
        return;
    }

    actorSpecificData[ACTOR_EN_DEKUNUTS] = [](s16 params) -> s16 {
        bool isFlower = params == DEKUNUTS_FLOWER;
        s16 shotsPerRound = (params >> 8) & 0xFF;
        if (shotsPerRound == 0xFF || shotsPerRound == 0) {
            shotsPerRound = 1;
        }
        ImGui::Checkbox("Flower", &isFlower);
        if (!isFlower) {
            ImGui::InputScalar("Shots Per Round", ImGuiDataType_S16, &shotsPerRound);
        }
        
        return isFlower ? DEKUNUTS_FLOWER : (shotsPerRound << 8);
    };

    actorSpecificData[ACTOR_EN_TITE] = [](s16 params) -> s16 {
        static const char* items[] = { "Blue", "Red" };
        //the + 2 is because the params are -2 & -1 instead of 0 & 1
        int selectedItem = params + 2;
        if (ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items))) {
            return selectedItem - 2;
        }
        
        return params;
    };

    actorSpecificData[ACTOR_EN_AM] = [](s16 params) -> s16 {
        static const char* items[] = { "Statue", "Enemy" };
        int selectedItem = params;
        if (ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items))) {
            return selectedItem;
        }
        
        return params;
    };

    actorSpecificData[ACTOR_BG_ICE_TURARA] = [](s16 params) -> s16 {
        static const char* items[] = { "Stalagmite", "Stalactite", "Stalactite (Regrow)" };
        int selectedItem = params;
        if (ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items))) {
            return selectedItem;
        }
        
        return params;
    };

    actorSpecificData[ACTOR_BG_BREAKWALL] = [](s16 params) -> s16 {
        static const char* items[] = { "DC Entrance", "Wall", "KD Floor", "KD Lava Cover" };
        int selectedItem = params;
        if (ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items))) {
            return selectedItem;
        }
        
        return params;
    };

    actorSpecificData[ACTOR_EN_REEBA] = [](s16 params) -> s16 {
        bool isBig = params != 0;
        ImGui::Checkbox("Big", &isBig);
        
        return isBig;
    };

    actorSpecificData[ACTOR_EN_ITEM00] = [](s16 params) -> s16 {
        bool autoCollect = params & 0x8000;
        ImGui::Checkbox("Automatically Collect", &autoCollect);
        u8 collectibleFlag = (params & 0x3F00) >> 8;
        ImGui::InputScalar("Collectible Flag", ImGuiDataType_U8, &collectibleFlag);
        if (collectibleFlag > 0x3F) {
            collectibleFlag = 0x3F;
        }
        
        static const char* items[] = {
            "Green Rupee",
            "Blue Rupee",
            "Red Rupee",
            "Recovery Heart",
            "Bombs (A)",
            "Arrow",
            "Heart Piece",
            "Heart Container",
            "Arrows (5)",
            "Arrows (10)",
            "Arrows (30)",
            "Bombs (B)",
            "Deku Nuts (5)",
            "Deku Stick",
            "Magic (Large)",
            "Magic (Small)",
            "Deku Seeds (5)",
            "Small Key",
            "Flexible",
            "Gold Rupee",
            "Purple Rupee",
            "Deku Shield",
            "Hylian Shield",
            "Zora Tunic",
            "Goron Tunic",
            "Bombs (Special)",
            "Bombchus"
        };

        int selectedItem = params & 0xFF;
        ImGui::Combo("Item", &selectedItem, items, IM_ARRAYSIZE(items));

        return autoCollect * 0x8000 + (collectibleFlag << 8) + selectedItem;
    };
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

            ImGui::Text(GetActorDescription(newActor.id).c_str());
            ImGui::InputScalar("ID", ImGuiDataType_S16, &newActor.id, &one);

            if (std::find(noParamsActors.begin(), noParamsActors.end(), newActor.id) == noParamsActors.end()) {
                CreateActorSpecificData();
                if (actorSpecificData.find(newActor.id) == actorSpecificData.end()) {
                    ImGui::InputScalar("params", ImGuiDataType_S16, &newActor.params, &one);
                } else {
                    DrawGroupWithBorder([&]() {
                        ImGui::Text("Actor Specific Data");
                        newActor.params = actorSpecificData[newActor.id](newActor.params);
                    });
                }
            }

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
