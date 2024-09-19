#ifndef CUSTOMMESSAGEDEBUGGER_H
#define CUSTOMMESSAGEDEBUGGER_H
#include "z64.h"

#ifdef __cplusplus
#include "GuiWindow.h"
#include <array>
extern "C" {
#endif
/**
 * \brief Pulls a message from the specified message table and kicks off the process of displaying that message
 * in a text box on screen.
 * \param tableId the tableId string for the table we want to pull from. Empty string for authentic/vanilla messages
 * \param textId The textId corresponding to the message to display. Putting in a textId that doesn't exist will
 * probably result in a crash.
 * \param language The Language to display on the screen.
 */
void MessageDebug_StartTextBox(const char* tableId, uint16_t textId, uint8_t language);

/**
 * \brief
 * \param customMessage A string using Custom Message Syntax.
 */
void MessageDebug_DisplayCustomMessage(const char* customMessage);
#ifdef __cplusplus
}


class MessageViewer : public Ship::GuiWindow {
public:
    static inline const char* TABLE_ID = "MessageViewer";
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;

    virtual ~MessageViewer() = default;

private:
    void DisplayExistingMessage() const;
    void DisplayCustomMessage() const;

    static constexpr uint16_t MAX_STRING_SIZE = 1024;
    static constexpr std::array<const char*, LANGUAGE_MAX> mLanguages = {"English", "German", "French"};
    static constexpr int HEXADECIMAL = 0;
    static constexpr int DECIMAL = 1;
    char* mTableIdBuf;
    std::string mTableId;
    char* mTextIdBuf;
    uint16_t mTextId;
    int mTextIdBase = HEXADECIMAL;
    size_t mLanguage = LANGUAGE_ENG;
    char* mCustomMessageBuf;
    std::string mCustomMessageString;
    bool mDisplayExistingMessageClicked = false;
    bool mDisplayCustomMessageClicked = false;
};


#endif //__cplusplus
#endif //CUSTOMMESSAGEDEBUGGER_H
