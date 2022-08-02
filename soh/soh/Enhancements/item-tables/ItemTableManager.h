#pragma once

#include <ultra64/gbi.h>

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

typedef struct {
    /* 0x00 */ uint8_t itemId;
    /* 0x01 */ uint8_t field; // various bit-packed data
    /* 0x02 */ int8_t gi;    // defines the draw id and chest opening animation
    /* 0x03 */ uint8_t textId;
    /* 0x04 */ uint16_t objectId;
} GetItemEntry; // size = 0x06

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, OBJECT_INVALID }

#ifdef __cplusplus

#include <unordered_map>
#include "z64item.h"
#include "z64object.h"

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

#endif