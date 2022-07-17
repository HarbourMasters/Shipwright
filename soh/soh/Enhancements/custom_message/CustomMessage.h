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

class CustomMessage {
  private: 
    std::unordered_map<std::string, char> textBoxSpecialCharacters;
    std::unordered_map<std::string, char> colors;
    std::unordered_map<GetItemID, std::string> getItemMessageTable[LANGUAGE_MAX];

    void ReplaceSpecialCharacters(std::string &string);
    void ReplaceColors(std::string& string);

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

    void CreateGetItemMessage(GetItemID giid, ItemID iid, std::string messages[LANGUAGE_MAX]);
    std::string RetrieveGetItemMessage(GetItemID giid);
};