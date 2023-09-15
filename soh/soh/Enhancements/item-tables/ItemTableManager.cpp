#include "ItemTableManager.h"
#include <stdexcept>

ItemTableManager::ItemTableManager() {
}

ItemTableManager::~ItemTableManager() {
    this->itemTables.clear();
}

bool ItemTableManager::AddItemTable(uint16_t tableID) {
    ItemTable newItemTable;
    return itemTables.emplace(tableID, newItemTable).second;
}

bool ItemTableManager::AddItemEntry(uint16_t tableID, uint16_t getItemID, GetItemEntry getItemEntry) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        return itemTable->emplace(getItemID, getItemEntry).second;
    } catch (const std::out_of_range& oor) { return false; }
}

GetItemEntry ItemTableManager::RetrieveItemEntry(uint16_t tableID, uint16_t getItemID) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        GetItemEntry getItemEntry = itemTable->at(getItemID);
        getItemEntry.drawItemId = getItemEntry.itemId;
        getItemEntry.drawModIndex = getItemEntry.modIndex;
        return getItemEntry;
    } catch (std::out_of_range& oor) { return GET_ITEM_NONE; }
}

bool ItemTableManager::ClearItemTable(uint16_t tableID) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        itemTable->clear();
        return true;
    } catch (const std::out_of_range& oor) { return false; }
}

ItemTable* ItemTableManager::RetrieveItemTable(uint16_t tableID) {
    try {
        return &itemTables.at(tableID);
    } catch (const std::out_of_range& oor) { throw(oor); }
}
