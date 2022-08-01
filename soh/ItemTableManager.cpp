#include "ItemTableManager.h"

ItemTableManager::ItemTableManager() {
}

ItemTableManager::~ItemTableManager() {
    this->itemTables.clear();
}

bool ItemTableManager::AddItemTable(std::string tableID) {
    ItemTable newItemTable;
    return itemTables.emplace(tableID, newItemTable).second;
}

bool ItemTableManager::AddItemEntry(std::string tableID, uint8_t getItemID, uint8_t itemID, uint16_t objectID,
                                    int8_t drawID,
                                    uint8_t textID, uint8_t field, bool chestAnim) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable == NULL) {
        return false;
    }
    GetItemEntry getItemEntry = GET_ITEM(itemID, objectID, drawID, textID, field, chestAnim);
    return itemTable->emplace(getItemID, getItemEntry).second;
}

GetItemEntry ItemTableManager::RetrieveItemEntry(std::string tableID, uint8_t itemID) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable != NULL) {
        auto foundItemEntry = itemTable->find(itemID);
        if (foundItemEntry != itemTable->end()) {
            return foundItemEntry->second;
        }
    }
    return GET_ITEM_NONE;
}

bool ItemTableManager::ClearItemTable(std::string tableID) {
    ItemTable* itemTable = RetrieveItemTable(tableID);
    if (itemTable != NULL) {
        itemTable->clear();
        return true;
    }
    return false;
}

ItemTable* ItemTableManager::RetrieveItemTable(std::string tableID) {
    auto foundItemTable = itemTables.find(tableID);
    if (foundItemTable == itemTables.end()) {
        return nullptr;
    }
    ItemTable& itemTable = foundItemTable->second;
    return &itemTable;
}
