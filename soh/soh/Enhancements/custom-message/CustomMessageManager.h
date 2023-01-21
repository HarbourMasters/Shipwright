#pragma once
#include <string>
#include <unordered_map>
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

typedef struct {
    TextBoxType textBoxType;
    TextBoxPosition textBoxPos;
    std::string english;
    std::string german;
    std::string french;
} CustomMessageEntry;

// Message Entry without the text type and position, useful for when
// you need an array of these to loop over for registration
// that will all have the same textbox type and position.
typedef struct {
    std::string english;
    std::string german;
    std::string french;
} CustomMessageMinimal;

#define NULL_CUSTOM_MESSAGE \
    { (TextBoxType)(-1), (TextBoxPosition)(-1), "", "", "" }

typedef std::unordered_map<uint16_t, CustomMessageEntry> CustomMessageTable;

class CustomMessageManager {
  private: 
    std::unordered_map<std::string, char> textBoxSpecialCharacters;
    std::unordered_map<std::string, char> colors;
    std::unordered_map<std::string, CustomMessageTable> messageTables;

    void ReplaceSpecialCharacters(std::string &string);
    void ReplaceColors(std::string& string);
    bool InsertCustomMessage(std::string tableID, uint16_t textID, CustomMessageEntry messages);

    std::string MESSAGE_END();
    std::string ITEM_OBTAINED(uint8_t x);
    std::string NEWLINE();
    std::string COLOR(uint8_t x);
    std::string WAIT_FOR_INPUT();
    std::string PLAYER_NAME();

  public:
    static CustomMessageManager* Instance;

    /*
    Replaces the specified string in a CustomMessageEntry with the provided value
    */
    static void ReplaceStringInMessage(CustomMessageEntry& messageEntry, std::string textToReplace, std::string value);
    static void ReplaceStringInMessage(CustomMessageEntry& messageEntry, std::string textToReplace, std::string englishValue, std::string germanValue, std::string frenchValue);

    CustomMessageManager();
    ~CustomMessageManager();

    /*
    Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
    with the provided giid (getItemID) as its key. This function also inserts the icon corresponding to
    the provided iid (itemID) at the beginning of each page of the textbox.
    */
    bool CreateGetItemMessage(std::string tableID, uint16_t giid, ItemID iid, CustomMessageEntry messages);

    /*
    Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
    with the provided textID as its key.
    */
    bool CreateMessage(std::string tableID, uint16_t textID, CustomMessageEntry messages);

    /*
    Retrieves a message from the table with id tableID with the provided textID.
    Returns a NULL_CUSTOM_MESSAGE if the message or table does not exist.
    */
    CustomMessageEntry RetrieveMessage(std::string tableID, uint16_t textID);

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

    /*
    Replaces special characters and certain symbols with control codes
    & for newline, ^ for wait-for-input, and @ for the player name,
    as well as %<letter> for colors (i.e. %r for red and %w for white).
    */
    void FormatCustomMessage(std::string& message, ItemID iid);

    /*
    Replaces special characters and certain symbols with control codes
    & for newline, ^ for wait-for-input, and @ for the player name,
    as well as %<letter> for colors (i.e. %r for red and %w for white).
    */
    void FormatCustomMessage(std::string& message);
};
