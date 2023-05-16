#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
#include <exception>

#include "../../../include/z64item.h"
#include "../../../include/message_data_textbox_types.h"

#undef MESSAGE_END

#define QM_WHITE 0x00
#define QM_RED 0x41
#define QM_GREEN 0x42
#define QM_BLUE 0x43
#define QM_LBLUE 0x44
#define QM_PINK 0x45
#define QM_YELLOW 0x46
#define QM_BLACK 0x47

class CustomMessage {
  public:
  CustomMessage() = default;
  CustomMessage(std::string english_, std::string german_, std::string french_, TextBoxType type_ = TEXTBOX_TYPE_BLACK,
                TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);

  const std::string& GetEnglish() const;
  const std::string& GetFrench() const;
  const std::string& GetGerman() const;
  const TextBoxType& GetTextBoxType() const;
  const TextBoxPosition& GetTextBoxPosition() const;

  CustomMessage operator+ (const CustomMessage& right) const;

  CustomMessage operator+ (const std::string& right) const;

  void operator += (const std::string& right);

  bool operator== (const CustomMessage& right) const;

  bool operator!= (const CustomMessage& right) const;

  void Replace(std::string&& oldStr, std::string&& newStr);

  void Replace(std::string&& oldStr, std::string&& newEnglish, std::string&& newGerman, std::string&& newFrench);

  void Capitalize();

  void ReplaceSpecialCharacters();
  void ReplaceColors();

  void Format(ItemID iid);

  void Format();

  private:
    const std::string MESSAGE_END() const;
    const std::string ITEM_OBTAINED(uint8_t x) const;
    const std::string NEWLINE() const;
    const std::string COLOR(uint8_t x) const;
    const std::string WAIT_FOR_INPUT() const;
    const std::string PLAYER_NAME() const;

    std::string english = "";
    std::string french = "";
    std::string german = "";
    TextBoxType type = TEXTBOX_TYPE_BLACK;
    TextBoxPosition position = TEXTBOX_POS_BOTTOM;
};

typedef std::unordered_map<uint16_t, CustomMessage> CustomMessageTable;

class CustomMessageManager {
  private: 
    std::unordered_map<std::string, CustomMessageTable> messageTables;

    void ReplaceSpecialCharacters(std::string &string);
    void ReplaceColors(std::string& string);
    bool InsertCustomMessage(std::string tableID, uint16_t textID, CustomMessage messages);

  public:
    static CustomMessageManager* Instance;

    CustomMessageManager() = default;

    /*
    Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
    with the provided giid (getItemID) as its key. This function also inserts the icon corresponding to
    the provided iid (itemID) at the beginning of each page of the textbox.
    */
    bool CreateGetItemMessage(std::string tableID, uint16_t giid, ItemID iid, CustomMessage messages);

    /*
    Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
    with the provided textID as its key.
    */
    bool CreateMessage(std::string tableID, uint16_t textID, CustomMessage messages);

    /*
    Retrieves a message from the table with id tableID with the provided textID.
    Returns a NULL_CUSTOM_MESSAGE if the message or table does not exist.
    */
    CustomMessage RetrieveMessage(std::string tableID, uint16_t textID);

    /*
    Empties out the message table identified by tableID.
    Returns true if successful and false if not (for instance
    if a table with the provided tableID does not exist).
    */
    bool ClearMessageTable(std::string tableID);

    /*
    Creates an empty CustomMessageTable accessible at the provided
    tableID, returns true if creation was successful and false
    if not.
    */
    bool AddCustomMessageTable(std::string tableID);
};

class MessageNotFoundException : public std::exception {
  private:
    std::string messageTableId;
    uint16_t textId;

  public:
    MessageNotFoundException(std::string messageTableId_, uint16_t textId_)
        : messageTableId(messageTableId_), textId(textId) {
    }
    MessageNotFoundException(std::string&& messageTableId_, uint16_t textId_)
        : messageTableId(std::move(messageTableId_)), textId(textId_) {
    }
    virtual const char* what() const {
      char* message;
      sprintf(message, "Message from table %s with textId %u was not found", messageTableId.c_str(), textId);
      return message;
    }
};