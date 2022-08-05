#pragma once
#include "ItemTableTypes.h"

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define GET_ITEM_NONE \
    { 0, 0, 0, 0, 0 }

#include <unordered_map>

typedef std::unordered_map<uint16_t, GetItemEntry> ItemTable;

class ItemTableManager {
  public:
      static ItemTableManager* Instance;
      ItemTableManager();
      ~ItemTableManager();
      bool AddItemTable(uint16_t tableID);
      bool AddItemEntry(uint16_t tableID, uint16_t getItemID, uint16_t itemID, uint16_t objectID, int16_t drawID, uint16_t textID, uint16_t field, bool chestAnim);
      bool AddItemEntry(uint16_t tableID, uint16_t getItemID, GetItemEntry getItemEntry);
      GetItemEntry RetrieveItemEntry(uint16_t tableID, uint16_t itemID);
      bool ClearItemTable(uint16_t tableID);

  private:
      std::unordered_map<uint16_t, ItemTable> itemTables;

      ItemTable* RetrieveItemTable(uint16_t tableID);
};
