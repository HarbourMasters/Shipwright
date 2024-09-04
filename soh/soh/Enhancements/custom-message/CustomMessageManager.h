#pragma once
#include <unordered_map>
#include <cstdint>
#include <exception>
#include <vector>
#include <string>

#include "../../../include/z64item.h"
#include "../../../include/message_data_textbox_types.h"
#include "../randomizer/3drando/text.hpp"

#undef MESSAGE_END

#define QM_WHITE "\x00"s
#define QM_RED "\x41"
#define QM_GREEN "\x42"
#define QM_BLUE "\x43"
#define QM_LBLUE "\x44"
#define QM_PINK "\x45"
#define QM_YELLOW "\x46"
#define QM_BLACK "\x47"

#define HS_HORSE_ARCHERY "\x00"s //HS_HBA is an enum already

typedef enum {
    MF_FORMATTED,
    MF_CLEAN,
    MF_RAW,
    MF_AUTO_FORMAT
} MessageFormat;

/**
 * @brief Encapsulates logic surrounding languages, and formatting strings for OoT's textboxes and
 * performing variable replacement across all of them at once. Also stores a message's text box type
 * (i.e. black, blue, none, typically describes the background but also changes what a few codes mean),
 * and position (i.e. top, bottom, middle).
 */
class CustomMessage {
  public:
    CustomMessage() = default;
    CustomMessage(std::string english_, std::string german_, std::string french_,
                  TextBoxType type_ = TEXTBOX_TYPE_BLACK, TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);
    CustomMessage(std::string english_, std::string german_, std::string french_, std::vector<std::string> colors_, std::vector<bool> capital_ = {},
              TextBoxType type_ = TEXTBOX_TYPE_BLACK, TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);
    CustomMessage(std::string english_, TextBoxType type_ = TEXTBOX_TYPE_BLACK, TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);
    CustomMessage(std::string english_, std::vector<std::string> colors_, std::vector<bool> capital_ = {}, TextBoxType type_ = TEXTBOX_TYPE_BLACK, TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);
    CustomMessage(Text text, TextBoxType type_ = TEXTBOX_TYPE_BLACK, TextBoxPosition position_ = TEXTBOX_POS_BOTTOM);

    static std::string MESSAGE_END() ;
    static std::string ITEM_OBTAINED(uint8_t x) ;
    static std::string NEWLINE() ;
    static std::string COLOR(std::string x) ;
    static std::string POINTS(std::string x) ;//HIGH_SCORE is also a macro
    static std::string WAIT_FOR_INPUT() ;
    static std::string PLAYER_NAME() ;

    const std::string GetEnglish(MessageFormat format = MF_FORMATTED) const;
    const std::string GetFrench(MessageFormat format = MF_FORMATTED) const;
    const std::string GetGerman(MessageFormat format = MF_FORMATTED) const;
    const std::string GetForCurrentLanguage(MessageFormat format = MF_FORMATTED) const;
    const std::string GetForLanguage(uint8_t language, MessageFormat format = MF_FORMATTED) const;
    const std::vector<std::string> GetAllMessages(MessageFormat format = MF_FORMATTED) const;
    void ProcessMessageFormat(std::string& str, MessageFormat format) const;
    const std::vector<std::string>& GetColors() const;
    void SetColors(std::vector<std::string> colors_);
    const std::vector<bool>& GetCapital() const;
    void SetCapital(std::vector<bool> capital_);
    const TextBoxType& GetTextBoxType() const;
    void SetTextBoxType(TextBoxType boxType);
    const TextBoxPosition& GetTextBoxPosition() const;

    CustomMessage operator+(const CustomMessage& right) const;
    CustomMessage operator+(const std::string& right) const;
    void operator+=(const std::string& right);
    void operator+=(const CustomMessage& right);
    bool operator==(const CustomMessage& operand) const;
    bool operator==(const std::string& operand) const;
    bool operator!=(const CustomMessage& right) const;

    /**
     * @brief Finds an instance of oldStr in each language of the CustomMessage
     * and replaces it with newStr. Typically used for dynamic variable replacement
     * (i.e. gameplay stats, skulltula count)
     *
     * @param oldStr the string to be replaced
     * @param newStr the string to replace with
     */
    void Replace(std::string&& oldStr, std::string&& newStr);

    /**
     * @brief Finds an instance of oldStr in each language of the CustomMessage,
     * and replaces it with the corresponding string in the provided CustomMessage.
     * Typically used for dynamic variable replacement (i.e. gameplay stats, skulltula count)
     *
     * @param oldStr the string to be replaced
     * @param newMessage the message containing the new strings.
     */
    void Replace(std::string&& oldStr, CustomMessage newMessage);

    /**
     * @brief Capitalizes the first letter of the string for each language.
     */
    void Capitalize();

    /**
     * @brief Replaces special characters (things like diacritics for non-english langugages)
     * with the control codes used to display them in OoT's textboxes.
     */
    void ReplaceSpecialCharacters(std::string& str) const;

    /**
     * @brief Replaces our color variable strings with the OoT control codes.
     */
    void ReplaceColors(std::string& str) const;

    /**
     * @brief Replaces `$<char>` variable strings with OoT control codes.
     */
    void ReplaceAltarIcons(std::string& str) const;

    /**
     * @brief Replaces [[1]] style variable strings with the provided vector of customMessages
     */
    void InsertNames(std::vector<CustomMessage> toInsert);

    /**
     * @brief Replaces various symbols with the control codes necessary to
     * display them in OoT's textboxes. i.e. special characters, colors, newlines,
     * wait for input, etc. Also adds the item icon to each page of the textbox.
     *
     * @param iid the ItemID whose icon should be displayed in this message's textbox.
     */
    void Format(ItemID iid);

    /**
     * @brief Replaces [[d]] in text with the supplied number, and if plural
     * options exist (2 blocks of text surrounded by |) choose the former if it 1,
     * and the latter otherwise, deleting the other and the |'s.
     *
     * @param num the number to insert.
     */
    void InsertNumber(uint8_t num);

    /**
     * @brief Replaces various symbols with the control codes necessary to
     * display them in OoT's textboxes. i.e. special characters, colors, newlines,
     * wait for input, etc.
     */
    void Format();

    /**
     * @brief formats the message specifically to fit in OoT's 
     * textboxes, and use it's formatting.
     */
    void AutoFormat();

    /**
     * @brief Removes all OoT formatting from the message, 
     * making it a good form for writing into spoiler logs.
     */
    void Clean();

    /**
     * @brief Replaces various symbols with the control codes necessary to
     * display them in OoT's textboxes for a single string
     * . i.e. special characters, colors, newlines, wait for input, etc.
     */
    void FormatString(std::string& str) const;
    
    /**
     * @brief formats the string specifically to fit in OoT's 
     * textboxes, and use it's formatting.
     * RANDOTODO whoever knows exactly what this does check my adaption
     */
    void AutoFormatString(std::string& str) const;

    /**
     * @brief Removes symbols used for control codes from the string,
     * leaving raw text
     */
    void CleanString(std::string& str) const;

  private:
    std::vector<std::string> messages = {"","",""};
    TextBoxType type = TEXTBOX_TYPE_BLACK;
    TextBoxPosition position = TEXTBOX_POS_BOTTOM;
    std::vector<std::string> colors = {};
    std::vector<bool> capital = {};
};

typedef std::unordered_map<uint16_t, CustomMessage> CustomMessageTable;

/**
 * @brief Encapsulates data and functions for creating custom message tables and storing and retrieving
 * `CustomMessage`s from them. It also converts a more user-friendly string syntax to the raw control
 * characters that OoT's message system uses (i.e. & for newline, ^ for new page (wait for input), and %
 * followed by various letters for colors).
 */
class CustomMessageManager {
  private:
    std::unordered_map<std::string, CustomMessageTable> messageTables;

    bool InsertCustomMessage(std::string tableID, uint16_t textID, CustomMessage message);

  public:
    static CustomMessageManager* Instance;

    CustomMessageManager() = default;

    /**
     * @brief Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
     * with the provided giid (getItemID) as its key. This function also inserts the icon corresponding to
     * the provided iid (itemID) at the beginning of each page of the textbox.
     *
     * @param tableID the ID of the custom message table
     * @param giid the GetItemID of the item (or the textID you want to use to retrieve it later)
     * @param iid the ItemID of the item
     * @param message the CustomMessage instance being added
     * @return true if adding the custom message succeeds, or
     * @return false if it does not.
     */
    bool CreateGetItemMessage(std::string tableID, uint16_t giid, ItemID iid, CustomMessage message);

    /**
     * @brief Formats the provided Custom Message Entry and inserts it into the table with the provided tableID,
     * with the provided textID as its key.
     *
     * @param tableID the ID of the custom message table
     * @param textID the ID to use for later retrieval
     * @param message the CustomMessage instance being added
     * @return true if adding the custom message succeeds, or
     * @return false if it does not.
     */
    bool CreateMessage(std::string tableID, uint16_t textID, CustomMessage message);

    /**
     * @brief Retrieves a message from the table with id tableID with the provided textID.
     * Throws an exception if either the table or the message do not exist. Note: this
     * returns a copy of the CustomMessage in the table on purpose, as it is sometimes normal
     * to modify it's contents between retrieval and displaying it in game, in order to
     * display some dynamic data like gameplay stats.
     *
     * @param tableID the ID of the custom message table
     * @param textID the ID of the message you want to retrieve
     * @param format the type of formatting to apply to the retrieved message
     * @return CustomMessage
     */
    CustomMessage RetrieveMessage(std::string tableID, uint16_t textID, MessageFormat format = MF_RAW);

    /**
     * @brief Empties out the message table identified by tableID.
     *
     * @param tableID the ID of the table to clear
     * @return true if it was cleared successfully, or
     * @return false if the table did not exist
     */
    bool ClearMessageTable(std::string tableID);

    /**
     * @brief Creates an empty CustomMessageTable accessible at the provided tableID
     *
     * @param tableID the ID of the table to create
     * @return true if the table was created successfully, or
     * @return false if not (i.e. because a table with that ID
     * already exists.)
     */
    bool AddCustomMessageTable(std::string tableID);
};

class MessageNotFoundException : public std::exception {
  private:
    std::string messageTableId;
    uint16_t textId;

  public:
    MessageNotFoundException(std::string messageTableId_, uint16_t textId_)
        : messageTableId(messageTableId_), textId(textId_) {
    }
    MessageNotFoundException(std::string&& messageTableId_, uint16_t textId_)
        : messageTableId(std::move(messageTableId_)), textId(textId_) {
    }
    virtual const char* what() const noexcept {
        char* message;
        sprintf(message, "Message from table %s with textId %u was not found", messageTableId.c_str(), textId);
        return message;
    }
};
