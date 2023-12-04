#ifndef CUSTOMMESSAGEDEBUGGER_H
#define CUSTOMMESSAGEDEBUGGER_H
#include "z64.h"

#ifdef __cplusplus
#include "GuiWindow.h"
#include <array>
extern "C" {
#endif
    void CustomMessageDebug_StartTextBox(PlayState* play, const char* tableId, uint16_t textId, uint8_t language);
#ifdef __cplusplus
}


class MessageViewer : public LUS::GuiWindow {
public:
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
    static inline const char* TABLE_ID = "MessageViewer";
    static constexpr int HEXADECIMAL = 0;
    static constexpr int DECIMAL = 1;
    char* mTableIdBuf;
    std::string mTableId;
    char* mTextIdBuf;
    uint16_t mTextId;
    int mTextIdBase = HEXADECIMAL;
    size_t mLanguage;
    char* mCustomMessageBuf;
    std::string mCustomMessageString;
    bool mDisplayExistingMessageClicked = false;
    bool mDisplayCustomMessageClicked = false;
};


#endif //__cplusplus
#endif //CUSTOMMESSAGEDEBUGGER_H
