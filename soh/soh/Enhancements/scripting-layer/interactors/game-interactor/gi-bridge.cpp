#include "gi-bridge.h"
#include "soh/Enhancements/scripting-layer/bridge.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

void GameInteractorBridge::Initialize() {
    GameBridge::Instance->BindFunction("AddOrRemoveHealthContainers", [](uintptr_t ctx, MethodCall* method) {
        auto amount = method->GetArgument<int>(0);
        GameInteractor::RawAction::AddOrRemoveHealthContainers((int16_t) amount);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("AddOrRemoveMagic", [](uintptr_t ctx, MethodCall* method) {
        auto amount = method->GetArgument<int>(0);
        GameInteractor::RawAction::AddOrRemoveMagic((int8_t) amount);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("HealOrDamagePlayer", [](uintptr_t ctx, MethodCall* method) {
        auto amount = method->GetArgument<int>(0);
        GameInteractor::RawAction::HealOrDamagePlayer((int16_t) amount);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("SetPlayerHealth", [](uintptr_t ctx, MethodCall* method) {
        auto amount = method->GetArgument<int>(0);
        GameInteractor::RawAction::SetPlayerHealth((int16_t) amount);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("SetLinkInvisibility", [](uintptr_t ctx, MethodCall* method) {
        auto active = method->GetArgument<bool>(0);
        GameInteractor::RawAction::SetLinkInvisibility(active);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("SetWeatherStorm", [](uintptr_t ctx, MethodCall* method) {
        auto active = method->GetArgument<bool>(0);
        GameInteractor::RawAction::SetWeatherStorm(active);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("ForceEquipBoots", [](uintptr_t ctx, MethodCall* method) {
        auto amount = method->GetArgument<int>(0);
        GameInteractor::RawAction::ForceEquipBoots((int8_t) amount);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("FreezePlayer", [](uintptr_t ctx, MethodCall* method) {
        GameInteractor::RawAction::FreezePlayer();
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("BurnPlayer", [](uintptr_t ctx, MethodCall* method) {
        GameInteractor::RawAction::BurnPlayer();
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("ElectrocutePlayer", [](uintptr_t ctx, MethodCall* method) {
        GameInteractor::RawAction::ElectrocutePlayer();
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("KnockbackPlayer", [](uintptr_t ctx, MethodCall* method) {
        auto strength = method->GetArgument<double>(0);
        GameInteractor::RawAction::KnockbackPlayer((float) strength);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("GiveOrTakeShield", [](uintptr_t ctx, MethodCall* method) {
        auto shield = method->GetArgument<int>(0);
        GameInteractor::RawAction::GiveOrTakeShield(shield);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("SetWeatherStorm", [](uintptr_t ctx, MethodCall* method) {
        auto status = method->GetArgument<bool>(0);
        GameInteractor::RawAction::SetWeatherStorm(status);
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("ForceInterfaceUpdate", [](uintptr_t ctx, MethodCall* method) {
        GameInteractor::RawAction::ForceInterfaceUpdate();
        method->success();
    }, "GameInteractor");
    GameBridge::Instance->BindFunction("SpawnEnemyWithOffset", [](uintptr_t ctx, MethodCall* method) {
        auto enemyId     = method->GetArgument<int>(0);
        auto enemyParams = method->GetArgument<int>(1);
        int result = GameInteractor::RawAction::SpawnEnemyWithOffset(enemyId, enemyParams);
        method->success(result);
    }, "GameInteractor");
}