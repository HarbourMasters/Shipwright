#include "ShipSaveContextData.h"
#include "ObjectExtension.h"

extern "C" SaveContext gSaveContext;

uint8_t IsRando() {
    if (ObjectExtension::GetInstance().Has<ShipSaveContextData>(&gSaveContext)) {
        return ObjectExtension::GetInstance().Get<ShipSaveContextData>(&gSaveContext)->quest.id == QUEST_RANDOMIZER;
    }
    return 0;
}

uint8_t IsBossRush() {
    if (ObjectExtension::GetInstance().Has<ShipSaveContextData>(&gSaveContext)) {
        return ObjectExtension::GetInstance().Get<ShipSaveContextData>(&gSaveContext)->quest.id == QUEST_BOSSRUSH;
    }
    return 0;
}

uint8_t IsVanilla() {
    if (ObjectExtension::GetInstance().Has<ShipSaveContextData>(&gSaveContext)) {
        return ObjectExtension::GetInstance().Get<ShipSaveContextData>(&gSaveContext)->quest.id == QUEST_NORMAL;
    }
    return 0;
}

uint8_t IsMasterQuest() {
    if (ObjectExtension::GetInstance().Has<ShipSaveContextData>(&gSaveContext)) {
        return ObjectExtension::GetInstance().Get<ShipSaveContextData>(&gSaveContext)->quest.id == QUEST_MASTER;
    }
    return 0;
}

ShipSaveContextData* GetGlobalShipSaveContextData() {
    return ObjectExtension::GetInstance().Get<ShipSaveContextData>(&gSaveContext);
}

ShipSaveContextData* GetShipSaveContextData(SaveContext* saveContext) {
    if (saveContext == nullptr) {
        saveContext = &gSaveContext;
    }
    return ObjectExtension::GetInstance().Get<ShipSaveContextData>(saveContext);
}

void ShipSaveContextData_Init(SaveContext* saveContext) {
    ShipSaveContextData shipSaveContext;
    memset(&shipSaveContext, 0, sizeof(shipSaveContext));
    ObjectExtension::GetInstance().Set<ShipSaveContextData>(saveContext, std::move(shipSaveContext));
}

void ShipSaveContextData_Free(SaveContext* saveContext) {
    ObjectExtension::GetInstance().Free(saveContext);
}

static ObjectExtension::Register<ShipSaveContextData> RegisterShipSaveContextData;