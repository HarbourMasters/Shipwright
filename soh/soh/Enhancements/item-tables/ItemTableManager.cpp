#include "ItemTableManager.h"
#include "stddef.h"

ItemTableManager::ItemTableManager() {
}

ItemTableManager::~ItemTableManager() {
    this->itemTables.clear();
}

bool ItemTableManager::AddItemTable(uint16_t tableID) {
    ItemTable newItemTable;
    return itemTables.emplace(tableID, newItemTable).second;
}

bool ItemTableManager::AddItemEntry(uint16_t tableID, uint16_t getItemID, uint16_t itemID, uint16_t objectID, int16_t drawID, uint16_t textID, uint16_t field, bool chestAnim) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable == NULL) {
        return false;
    }
    GetItemEntry getItemEntry = GET_ITEM(itemID, objectID, drawID, textID, field, chestAnim);
    return itemTable->emplace(getItemID, getItemEntry).second;
}

bool ItemTableManager::AddItemEntry(uint16_t tableID, uint16_t getItemID, GetItemEntry getItemEntry) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable == NULL) {
        return false;
    }
    return itemTable->emplace(getItemID, getItemEntry).second;
}

GetItemEntry ItemTableManager::RetrieveItemEntry(uint16_t tableID, uint16_t itemID) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable != NULL) {
        auto foundItemEntry = itemTable->find(itemID);
        if (foundItemEntry != itemTable->end()) {
            return foundItemEntry->second;
        }
    }
    return GET_ITEM_NONE;
}

bool ItemTableManager::ClearItemTable(uint16_t tableID) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable != NULL) {
        itemTable->clear();
        return true;
    }
    return false;
}

ItemTable* ItemTableManager::RetrieveItemTable(uint16_t tableID) {
    auto foundItemTable = itemTables.find(tableID);
    if (foundItemTable == itemTables.end()) {
        return nullptr;
    }
    ItemTable& itemTable = foundItemTable->second;
    return &itemTable;
}
