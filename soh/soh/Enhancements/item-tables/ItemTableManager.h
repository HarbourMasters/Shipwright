#pragma once
#include "ItemTableTypes.h"

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define GET_ITEM_NONE \
    { 0, 0, 0, 0, 0 }

#include <unordered_map>

typedef std::unordered_map<uint8_t, GetItemEntry> ItemTable;

class ItemTableManager {
  public:
      static ItemTableManager* Instance;
      ItemTableManager();
      ~ItemTableManager();
      bool AddItemTable(std::string tableID);
      bool AddItemEntry(std::string tableID, uint8_t getItemID, uint8_t itemID, uint16_t objectID, int8_t drawID, uint8_t textID, uint8_t field, bool chestAnim);
      bool AddItemEntry(std::string tableID, uint8_t getItemID, GetItemEntry getItemEntry);
      GetItemEntry RetrieveItemEntry(std::string tableID, uint8_t itemID);
      bool ClearItemTable(std::string tableID);

  private:
      std::unordered_map<std::string, ItemTable> itemTables;

      ItemTable* RetrieveItemTable(std::string tableID);
};
