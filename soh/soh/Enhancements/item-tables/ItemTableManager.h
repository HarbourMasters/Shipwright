#pragma once
#include "ItemTableTypes.h"
#include "z64item.h"

#include <unordered_map>

typedef std::unordered_map<uint16_t, GetItemEntry> ItemTable;

class ItemTableManager {
  public:
      static ItemTableManager* Instance;
      ItemTableManager();
      ~ItemTableManager();
      bool AddItemTable(uint16_t tableID);
      bool AddItemEntry(uint16_t tableID, uint16_t getItemID, GetItemEntry getItemEntry);
      GetItemEntry RetrieveItemEntry(uint16_t tableID, uint16_t getItemID);
      bool ClearItemTable(uint16_t tableID);

  private:
      std::unordered_map<uint16_t, ItemTable> itemTables;

      ItemTable* RetrieveItemTable(uint16_t tableID);
};
