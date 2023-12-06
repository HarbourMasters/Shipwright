#include "gi-bridge.h"
#include "soh/Enhancements/scripting-layer/bridge.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

void GameInteractorBridge::Initialize() {
    const auto entries = {
        BIND_FUNCTION("AddOrRemoveHealthContainers", [](uintptr_t ctx, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::AddOrRemoveHealthContainers((int16_t) amount);
            method->success();
        }),
        BIND_FUNCTION("AddOrRemoveMagic", [](uintptr_t, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::AddOrRemoveMagic(static_cast<int8_t>(amount));
            method->success();
        }),
        BIND_FUNCTION("HealOrDamagePlayer", [](uintptr_t, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::HealOrDamagePlayer(static_cast<int16_t>(amount));
            method->success();
        }),
        BIND_FUNCTION("SetPlayerHealth", [](uintptr_t, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::SetPlayerHealth(static_cast<int16_t>(amount));
            method->success();
        }),
        BIND_FUNCTION("SetLinkInvisibility", [](uintptr_t, MethodCall* method) {
            const auto active = method->GetArgument<bool>(0);
            GameInteractor::RawAction::SetLinkInvisibility(active);
            method->success();
        }),
        BIND_FUNCTION("SetWeatherStorm", [](uintptr_t, MethodCall* method) {
            const auto active = method->GetArgument<bool>(0);
            GameInteractor::RawAction::SetWeatherStorm(active);
            method->success();
        }),
        BIND_FUNCTION("ForceEquipBoots", [](uintptr_t, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::ForceEquipBoots(static_cast<int8_t>(amount));
            method->success();
        }),
        BIND_FUNCTION("FreezePlayer", [](uintptr_t, MethodCall* method) {
            GameInteractor::RawAction::FreezePlayer();
            method->success();
        }),
        BIND_FUNCTION("BurnPlayer", [](uintptr_t, MethodCall* method) {
            GameInteractor::RawAction::BurnPlayer();
            method->success();
        }),
        BIND_FUNCTION("ElectrocutePlayer", [](uintptr_t, MethodCall* method) {
            GameInteractor::RawAction::ElectrocutePlayer();
            method->success();
        }),
        BIND_FUNCTION("KnockbackPlayer", [](uintptr_t, MethodCall* method) {
            const auto strength = method->GetArgument<double>(0);
            GameInteractor::RawAction::KnockbackPlayer(static_cast<float>(strength));
            method->success();
        }),
        BIND_FUNCTION("GiveOrTakeShield", [](uintptr_t, MethodCall* method) {
            const auto amount = method->GetArgument<int>(0);
            GameInteractor::RawAction::GiveOrTakeShield(static_cast<int8_t>(amount));
            method->success();
        }),
        BIND_FUNCTION("ForceInterfaceUpdate", [](uintptr_t, MethodCall* method) {
            GameInteractor::RawAction::ForceInterfaceUpdate();
            method->success();
        }),
        BIND_FUNCTION("SpawnEnemyWithOffset", [](uintptr_t, MethodCall* method) {
            const auto enemyId     = method->GetArgument<int>(0);
            const auto enemyParams = method->GetArgument<int>(1);
            int result = GameInteractor::RawAction::SpawnEnemyWithOffset(enemyId, enemyParams);
            method->success(result);
        }),
    };

    GameBridge::Instance->Register(entries, "GameInteractor");
}