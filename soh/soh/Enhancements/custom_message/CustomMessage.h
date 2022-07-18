#pragma once
#include <string>
#include <unordered_map>
#include "variables.h"

#define QM_WHITE 0x00
#define QM_RED 0x41
#define QM_GREEN 0x42
#define QM_BLUE 0x43
#define QM_LBLUE 0x44
#define QM_PINK 0x45
#define QM_YELLOW 0x46
#define QM_BLACK 0x47

typedef struct {
    std::string english;
    std::string german;
    std::string french;
} CustomMessageEntry;

typedef std::unordered_map<uint16_t, CustomMessageEntry> CustomMessageTable;

class CustomMessage {
  private: 
    std::unordered_map<std::string, char> textBoxSpecialCharacters;
    std::unordered_map<std::string, char> colors;
    std::unordered_map<std::string, CustomMessageTable> messageTables;

    void ReplaceSpecialCharacters(std::string &string);
    void ReplaceColors(std::string& string);
    void FormatMessage(std::string& message, ItemID iid);

    std::string MESSAGE_END();
    std::string ITEM_OBTAINED(uint8_t x);
    std::string NEWLINE();
    std::string COLOR(uint8_t x);
    std::string WAIT_FOR_INPUT();
    std::string PLAYER_NAME();

  public:
    static CustomMessage* Instance;

    CustomMessage();
    ~CustomMessage();

    bool CreateGetItemMessage(std::string tableID, GetItemID giid, ItemID iid, CustomMessageEntry messages);
    std::string RetrieveMessage(std::string tableID, uint16_t textID);
    bool AddCustomMessageTable(std::string tableID);
};