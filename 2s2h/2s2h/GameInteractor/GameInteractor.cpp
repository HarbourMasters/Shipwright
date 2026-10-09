#include "GameInteractor.h"
#include <variant>
#include <spdlog/spdlog.h>
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenPort.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "z64actor.h"
#include "variables.h"
#include "functions.h"
}

void GameInteractor_ExecuteOnGameStateMainStart() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateMainStart>();
}

void GameInteractor_ExecuteOnGameStateMainFinish() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateMainFinish>();
}

void GameInteractor_ExecuteOnGameStateDrawFinish() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateDrawFinish>();
}

void GameInteractor_ExecuteOnGameStateUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameStateUpdate>();
}

void GameInteractor_ExecuteOnConsoleLogoUpdate() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnConsoleLogoUpdate>();
}

void GameInteractor_ExecuteOnKaleidoUpdate(PauseContext* pauseCtx) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnKaleidoUpdate>(pauseCtx);
}

void GameInteractor_ExecuteBeforeKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeKaleidoDrawPage>(pauseCtx, pauseIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::BeforeKaleidoDrawPage>(pauseIndex, pauseCtx,
                                                                                       pauseIndex);
}

void GameInteractor_ExecuteAfterKaleidoDrawPage(PauseContext* pauseCtx, u16 pauseIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterKaleidoDrawPage>(pauseCtx, pauseIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterKaleidoDrawPage>(pauseIndex, pauseCtx, pauseIndex);
}

void GameInteractor_ExecuteOnSaveInit(s16 fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveInit>(fileNum);
}

void GameInteractor_ExecuteOnSaveLoad(s16 fileNum) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSaveLoad>(fileNum);
}

void GameInteractor_ExecuteOnFileSelectSaveLoad(s16 fileNum, bool isOwlSave, SaveContext* saveContext) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFileSelectSaveLoad>(fileNum, isOwlSave, saveContext);
}

void GameInteractor_ExecuteBeforeEndOfCycleSave() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeEndOfCycleSave>();
}

void GameInteractor_ExecuteAfterEndOfCycleSave() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterEndOfCycleSave>();
}

void GameInteractor_ExecuteBeforeMoonCrash() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeMoonCrash>();
}

void GameInteractor_ExecuteOnInterfaceDrawStart() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnInterfaceDrawStart>();
}

void GameInteractor_ExecuteAfterInterfaceClockDraw() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterInterfaceClockDraw>();
}

void GameInteractor_ExecuteBeforeInterfaceClockDraw() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::BeforeInterfaceClockDraw>();
}

void GameInteractor_ExecuteOnGameCompletion() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGameCompletion>();
}

void GameInteractor_ExecuteOnSceneInit(s16 sceneId, s8 spawnNum) {
    SPDLOG_DEBUG("OnSceneInit: sceneId: {}, spawnNum: {}", sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnSceneInit>(sceneId, sceneId, spawnNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneInit>(sceneId, spawnNum);
}

void GameInteractor_ExecuteOnRoomInit(s16 sceneId, s8 roomNum) {
    SPDLOG_DEBUG("OnRoomInit: sceneId: {}, roomNum: {}", sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnRoomInit>(sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnRoomInit>(sceneId, sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnRoomInit>(sceneId, roomNum);
}

void GameInteractor_ExecuteAfterRoomSceneCommands(s16 sceneId, s8 roomNum) {
    SPDLOG_DEBUG("AfterRoomSceneCommands: sceneId: {}, roomNum: {}", sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterRoomSceneCommands>(sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterRoomSceneCommands>(sceneId, sceneId, roomNum);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::AfterRoomSceneCommands>(sceneId, roomNum);
}

void GameInteractor_ExecuteOnPlayDrawWorldStart() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDrawWorldStart>();
}

void GameInteractor_ExecuteOnPlayDrawWorldEnd() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDrawWorldEnd>();
}

void GameInteractor_ExecuteOnPlayDestroy() {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayDestroy>();
}

bool GameInteractor_ShouldActorInit(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorInit>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorInit>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorInit>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorInit>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorInit(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorInit>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorInit>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorInit>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorInit>(actor);
}

bool GameInteractor_ShouldActorUpdate(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorUpdate>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorUpdate>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorUpdate>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorUpdate>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorUpdate(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorUpdate>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorUpdate>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorUpdate>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorUpdate>(actor);
}

bool GameInteractor_ShouldActorDraw(Actor* actor) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldActorDraw>(actor, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldActorDraw>(actor->id, actor, &result);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::ShouldActorDraw>((uintptr_t)actor, actor, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldActorDraw>(actor, &result);
    return result;
}

void GameInteractor_ExecuteOnActorDraw(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorDraw>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorDraw>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorDraw>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorDraw>(actor);
}

void GameInteractor_ExecuteOnActorKill(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorKill>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorKill>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorKill>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorKill>(actor);
}

void GameInteractor_ExecuteOnActorDestroy(Actor* actor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnActorDestroy>(actor);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnActorDestroy>(actor->id, actor);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnActorDestroy>((uintptr_t)actor, actor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnActorDestroy>(actor);
}

void GameInteractor_ExecuteOnPlayerPostLimbDraw(Player* player, s32 limbIndex) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerPostLimbDraw>(player, limbIndex);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnPlayerPostLimbDraw>(limbIndex, player, limbIndex);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnPlayerPostLimbDraw>(player, limbIndex);
}

void GameInteractor_ExecuteOnPlayerReleaseHeldActor(PlayState* play, Player* player, Actor* heldActor) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPlayerReleaseHeldActor>(play, player, heldActor);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnPlayerReleaseHeldActor>(play, player, heldActor);
}

void GameInteractor_ExecuteOnBossDefeated(s16 actorId) {
    SPDLOG_DEBUG("GameInteractor_ExecuteOnBossDefeated: actorId: {}", actorId);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnBossDefeated>(actorId);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnBossDefeated>(actorId, actorId);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnBossDefeated>((uintptr_t)actorId, actorId);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnBossDefeated>(actorId);
}

void GameInteractor_ExecuteOnSceneFlagSet(s16 sceneId, FlagType flagType, u32 flag) {
    SPDLOG_DEBUG("OnSceneFlagSet: sceneId: {}, flagType: {}, flag: {}", sceneId, (u32)flagType, flag);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagSet>(sceneId, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagSet>(sceneId, flagType, flag);
}

void GameInteractor_ExecuteOnSceneFlagUnset(s16 sceneId, FlagType flagType, u32 flag) {
    SPDLOG_DEBUG("OnSceneFlagUnset: sceneId: {}, flagType: {}, flag: {}", sceneId, (u32)flagType, flag);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneFlagUnset>(sceneId, flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnSceneFlagUnset>(sceneId, flagType, flag);
}

void GameInteractor_ExecuteOnFlagSet(FlagType flagType, u32 flag) {
    // This flag in particular is very spammy, so we'll suppress it
    if (!(flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80)) {
        SPDLOG_DEBUG("OnFlagSet: flagType: {}, flag: {}", (u32)flagType, flag);
    }
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagSet>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagSet>(flagType, flag);
}

void GameInteractor_ExecuteOnFlagUnset(FlagType flagType, u32 flag) {
    // This flag in particular is very spammy, so we'll suppress it
    if (!(flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_92_80)) {
        SPDLOG_DEBUG("OnFlagUnset: flagType: {}, flag: {}", (u32)flagType, flag);
    }
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnFlagUnset>(flagType, flag);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnFlagUnset>(flagType, flag);
}

void GameInteractor_ExecuteOnCameraChangeModeFlags(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnCameraChangeModeFlags>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnCameraChangeModeFlags>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnCameraChangeModeFlags>((uintptr_t)camera, camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnCameraChangeModeFlags>(camera);
}

void GameInteractor_ExecuteAfterCameraUpdate(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::AfterCameraUpdate>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::AfterCameraUpdate>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::AfterCameraUpdate>((uintptr_t)camera, camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::AfterCameraUpdate>(camera);
}

void GameInteractor_ExecuteOnCameraChangeSettingsFlags(Camera* camera) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnCameraChangeSettingsFlags>(camera);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnCameraChangeSettingsFlags>(camera->uid, camera);
    GameInteractor::Instance->ExecuteHooksForPtr<GameInteractor::OnCameraChangeSettingsFlags>((uintptr_t)camera,
                                                                                              camera);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnCameraChangeSettingsFlags>(camera);
}

void GameInteractor_ExecuteOnPassPlayerInputs(Input* input) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnPassPlayerInputs>(input);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnPassPlayerInputs>(input);
}

void GameInteractor_ExecuteOnOpenText(u16* textId, bool* loadFromMessageTable) {
    SPDLOG_DEBUG("OnOpenText: textId: {}", *textId);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnOpenText>(textId, loadFromMessageTable);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnOpenText>(*textId, textId, loadFromMessageTable);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnOpenText>(textId, loadFromMessageTable);
}

bool GameInteractor_ShouldItemGive(u8 item) {
    bool result = true;
    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldItemGive>(item, &result);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldItemGive>(item, item, &result);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldItemGive>(item, &result);
    return result;
}

void GameInteractor_ExecuteOnItemGive(u8 item) {
    SPDLOG_DEBUG("OnItemGive: item: {}", item);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnItemGive>(item);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnItemGive>(item, item);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnItemGive>(item);
}

void GameInteractor_ExecuteOnBottleContentsUpdate(u8 item) {
    SPDLOG_DEBUG("OnBottleContentsUpdate: item: {}", item);
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnBottleContentsUpdate>(item);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::OnBottleContentsUpdate>(item, item);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::OnBottleContentsUpdate>(item);
}

void GameInteractor_ExecuteOnSeqPlayerInit(int32_t playerIdx, int32_t seqId) {
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSeqPlayerInit>(playerIdx, seqId);
}

bool GameInteractor_Should(GIVanillaBehavior flag, uint32_t result, ...) {
    // Only the external function can use the Variadic Function syntax
    // To pass the va args to the next caller must be done using va_list and reading the args into it
    // Because there can be N subscribers registered to each template call, the subscribers will be responsible for
    // creating a copy of this va_list to avoid incrementing the original pointer between calls
    va_list args;
    va_start(args, result);

    // Because of default argument promotion, even though our incoming "result" is just a bool, it needs to be typed as
    // an int to be permitted to be used in `va_start`, otherwise it is undefined behavior.
    // Here we downcast back to a bool for our actual hook handlers
    bool boolResult = static_cast<bool>(result);

    GameInteractor::Instance->ExecuteHooks<GameInteractor::ShouldVanillaBehavior>(flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForID<GameInteractor::ShouldVanillaBehavior>(flag, flag, &boolResult, args);
    GameInteractor::Instance->ExecuteHooksForFilter<GameInteractor::ShouldVanillaBehavior>(flag, &boolResult, args);

    va_end(args);
    return boolResult;
}

// Returns 1 or -1 based on a number of factors like CVars or other game states.
int GameInteractor_InvertControl(GIInvertType type) {
    int result = 1;

    switch (type) {
        case GI_INVERT_CAMERA_RIGHT_STICK_X:
            if (CVarGetInteger("gEnhancements.Camera.RightStick.InvertXAxis", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_CAMERA_RIGHT_STICK_Y:
            if (CVarGetInteger("gEnhancements.Camera.RightStick.InvertYAxis", 1)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_AIM_X:
        case GI_INVERT_TELESCOPE_X:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.InvertX", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_AIM_Y:
        case GI_INVERT_TELESCOPE_Y:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.InvertY", 1)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_GYRO_X:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroInvertX", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_GYRO_Y:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroInvertY", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_RIGHT_STICK_X:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickInvertX", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_FIRST_PERSON_RIGHT_STICK_Y:
            if (CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickInvertY", 1)) {
                result *= -1;
            }
            break;
        case GI_INVERT_SHIELD_Y:
            if (CVarGetInteger("gEnhancements.Equipment.InvertShieldY", 0)) {
                result *= -1;
            }
            break;
        case GI_INVERT_ZORA_SWIM_Y:
            if (CVarGetInteger("gEnhancements.Player.InvertZoraSwimY", 1)) {
                result *= -1;
            }
            break;
    }

    // Invert all X axis inputs if the Mirrored World mode is enabled
    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        switch (type) {
            case GI_INVERT_CAMERA_RIGHT_STICK_X:
            case GI_INVERT_MOVEMENT_X:
            case GI_INVERT_SHIELD_X:
            case GI_INVERT_SHOP_X:
            case GI_INVERT_HORSE_X:
            case GI_INVERT_ZORA_SWIM_X:
            case GI_INVERT_DEBUG_DPAD_X:
            case GI_INVERT_TELESCOPE_X:
            case GI_INVERT_FIRST_PERSON_AIM_X:
            case GI_INVERT_FIRST_PERSON_GYRO_X:
            case GI_INVERT_FIRST_PERSON_RIGHT_STICK_X:
            case GI_INVERT_FIRST_PERSON_MOVING_X:
                result *= -1;
                break;
            default:
                break;
        }
    }

    /*
    if (CrowdControl::State::InvertedInputs) {
        result *= -1;
    }
    */

    return result;
}

uint32_t GameInteractor_Dpad(GIDpadType type, uint32_t buttonCombo) {
    uint32_t result = 0;

    switch (type) {
        case GI_DPAD_OCARINA:
            if (CVarGetInteger("gEnhancements.Playback.DpadOcarina", 0)) {
                result = buttonCombo;
            }
            break;
        case GI_DPAD_EQUIP:
            if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
                result = buttonCombo;
            }
            break;
    }

    return result;
}

uint32_t GameInteractor_RightStickOcarina(Input* input) {
    uint32_t result = 0;

    if (!CVarGetInteger("gEnhancements.Playback.RightStickOcarina", 0)) {
        return result;
    }

    s8 rstick_x = input->cur.right_stick_x;
    s8 rstick_y = input->cur.right_stick_y;
    const s8 sensitivity = 64;

    if (rstick_x > sensitivity) {
        result |= BTN_CRIGHT;
    } else if (rstick_x < -sensitivity) {
        result |= BTN_CLEFT;
    }

    if (rstick_y > sensitivity) {
        result |= BTN_CUP;
    } else if (rstick_y < -sensitivity) {
        result |= BTN_CDOWN;
    }

    return result;
}

uint32_t GameInteractor_CustomOcarinaControls(Input* input) {
    uint32_t result = 0;

    if (!CVarGetInteger("gEnhancements.Playback.CustomizeOcarinaControls", 0)) {
        return result;
    }

    if (input->cur.button & BTN_CUSTOM_OCARINA_NOTE_D4) {
        result |= BTN_A;
    }
    if (input->cur.button & BTN_CUSTOM_OCARINA_NOTE_F4) {
        result |= BTN_CDOWN;
    }
    if (input->cur.button & BTN_CUSTOM_OCARINA_NOTE_A4) {
        result |= BTN_CRIGHT;
    }
    if (input->cur.button & BTN_CUSTOM_OCARINA_NOTE_B4) {
        result |= BTN_CLEFT;
    }
    if (input->cur.button & BTN_CUSTOM_OCARINA_NOTE_D5) {
        result |= BTN_CUP;
    }

    if (input->cur.button & BTN_CUSTOM_OCARINA_DISABLE_SONGS) {
        result |= BTN_L;
    }

    if (input->cur.button & BTN_CUSTOM_OCARINA_PITCH_UP) {
        result |= BTN_R;
    }
    if (input->cur.button & BTN_CUSTOM_OCARINA_PITCH_DOWN) {
        result |= BTN_Z;
    }

    return result;
}

void ProcessEvents(Actor* actor) {
    Player* player = GET_PLAYER(gPlayState);

    // If the player has a message active, stop
    if (gPlayState->msgCtx.msgMode != 0) {
        return;
    }

    // If the player is in a blocking cutscene, stop
    if (Player_InBlockingCsMode(gPlayState, player)) {
        return;
    }

    // If player is dead, stop
    if (player->stateFlags1 & PLAYER_STATE1_DEAD) {
        return;
    }

    // If there is an event active, stop
    const auto& currentEvent = GameInteractor::Instance->currentEvent;
    if (auto e = std::get_if<GIEventNone>(&currentEvent)) {
        // no-op
    } else {
        return;
    }

    // If there are no events that need to happen, stop
    if (GameInteractor::Instance->events.empty()) {
        return;
    }

    GameInteractor::Instance->currentEvent = GameInteractor::Instance->events.front();
    const auto& nextEvent = GameInteractor::Instance->currentEvent;

    if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
        EnItem00* enItem00;

        s16 flags = CustomItem::HIDE_TILL_OVERHEAD | CustomItem::KEEP_ON_PLAYER;

        // If the player is climbing or in the air, deliver the item without a cutscene but freeze the player
        if (!e->showGetItemCutscene ||
            (player->stateFlags1 &
             (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 | PLAYER_STATE1_40000 |
              PLAYER_STATE1_80000 | PLAYER_STATE1_100000 | PLAYER_STATE1_200000 | PLAYER_STATE1_8000000)) ||
            (Player_GetExplosiveHeld(player) > PLAYER_EXPLOSIVE_NONE)) {

            flags |= CustomItem::GIVE_OVERHEAD;
        } else {
            flags |= CustomItem::GIVE_ITEM_CUTSCENE;
        }

        enItem00 = CustomItem::Spawn(
            player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z, 0, flags, e->param,
            [](Actor* actor, PlayState* play) {
                Player* player = GET_PLAYER(gPlayState);
                const auto& nextEvent = GameInteractor::Instance->currentEvent;
                if (auto e = std::get_if<GIEventGiveItem>(&nextEvent)) {
                    e->giveItem(actor, play);
                    if (e->showGetItemCutscene && !(CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE)) {
                        player->actor.freezeTimer = 30;
                    }
                    GameInteractor::Instance->currentEvent = GIEventNone{};
                }
            },
            e->drawItem);
        enItem00->actor.destroy = [](Actor* actor, PlayState* play) {
            if (!(CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION)) {
                // Event was not handled, requeue it
                auto lostEvent = GameInteractor::Instance->currentEvent;
                GameInteractor::Instance->currentEvent = GIEventNone{};
                GameInteractor::Instance->events.push_back(lostEvent);
            }
        };
    } else if (auto e = std::get_if<GIEventTransition>(&nextEvent)) {
        gPlayState->nextEntrance = e->entrance;
        gSaveContext.nextCutsceneIndex = e->cutsceneIndex;
        gPlayState->transitionTrigger = e->transitionTrigger;
        gPlayState->transitionType = e->transitionType;
        GameInteractor::Instance->currentEvent = GIEventNone{};
    } else if (auto e = std::get_if<GIEventSpawnActor>(&nextEvent)) {
        // if true, the coordinates are made relative to the player's position and rotation, 0 rotation is facing the
        // same direction as the player, x+ is to the players right, y+ is up, z+ is in front of the player
        if (e->relativeCoords) {
            f32 x = player->actor.world.pos.x;
            f32 y = player->actor.world.pos.y;
            f32 z = player->actor.world.pos.z;
            f32 s = sin(player->actor.world.rot.y);
            f32 c = cos(player->actor.world.rot.y);
            f32 x2 = e->posX * c - e->posZ * s;
            f32 z2 = e->posX * s + e->posZ * c;
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, e->actorId, x + x2, y + e->posY, z + z2, 0,
                        e->rotY + player->actor.world.rot.y, 0, e->params);
        } else {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, e->actorId, e->posX, e->posY, e->posZ, e->rotX, e->rotY,
                        e->rotZ, e->params);
        }
        GameInteractor::Instance->currentEvent = GIEventNone{};
    } else if (auto e = std::get_if<GIEventTrap>(&nextEvent)) {
        if (player->stateFlags1 & PLAYER_STATE1_800000) {
            // Player is riding a horse, requeue the event
            auto lostEvent = GameInteractor::Instance->currentEvent;
            GameInteractor::Instance->currentEvent = GIEventNone{};
            GameInteractor::Instance->events.push_back(lostEvent);
        } else {
            if (e->action) {
                e->action();
            }
            GameInteractor::Instance->currentEvent = GIEventNone{};
        }
    }

    GameInteractor::Instance->events.erase(GameInteractor::Instance->events.begin());
}

// On MSVC this is defined inline in the header instead; see the declaration for why.
#ifndef _MSC_VER
void GameInteractor::RemoveAllQueuedHooks() {
#include "GameInteractor_RemoveAllQueuedHooks.inc"
}
#endif

void GameInteractor::RegisterOwnHooks() {
    // Cleanup all hooks at the start of each frame
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameStateMainStart>(
        []() { GameInteractor::Instance->RemoveAllQueuedHooks(); });

    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_PLAYER, ProcessEvents);
}
