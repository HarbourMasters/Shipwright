#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#include "macros.h"
extern PlayState* gPlayState;
void Player_AnimPlayLoop(PlayState* play, Player* player, LinkAnimationHeader* anim);
}

std::map<ImGuiKey, std::string> emoteMap = {
    { ImGuiKey_0, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_0" },
    { ImGuiKey_1, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_1" },
    { ImGuiKey_2, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_2" },
    { ImGuiKey_3, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_3" },
    { ImGuiKey_4, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_4" },
    { ImGuiKey_5, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_5" },
    { ImGuiKey_6, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_6" },
    { ImGuiKey_7, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_7" },
    { ImGuiKey_8, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_8" },
    { ImGuiKey_9, "__OTR__objects/gameplay_keep/gPlayerAnim_link_emote_9" },
};

void RegisterEmote() {
    // Loop over map, check if each animation exists, if not remove it from the map
    std::vector<ImGuiKey> keysToRemove;
    for (const auto& [key, animPath] : emoteMap) {
        auto file = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(animPath);
        if (file == nullptr) {
            keysToRemove.push_back(key);
        }
    }
    for (auto key : keysToRemove) {
        emoteMap.erase(key);
    }

    COND_HOOK(OnPlayerUpdate, true, [] {
        Player* player = GET_PLAYER(gPlayState);

        for (const auto& [key, animPath] : emoteMap) {
            if (ImGui::IsKeyDown(key)) {
                Player_AnimPlayLoop(gPlayState, player, (LinkAnimationHeader*)animPath.c_str());
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterEmote, {});
