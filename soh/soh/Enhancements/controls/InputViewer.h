#pragma once

#include <libultraship/libultraship.h>

typedef enum {
    BUTTON_OUTLINE_ALWAYS_SHOWN,
    BUTTON_OUTLINE_NOT_PRESSED,
    BUTTON_OUTLINE_PRESSED,
    BUTTON_OUTLINE_ALWAYS_HIDDEN
} ButtonOutlineMode;

typedef enum {
    STICK_MODE_ALWAYS_SHOWN,
    STICK_MODE_HIDDEN_IN_DEADZONE,
    STICK_MODE_ALWAYS_HIDDEN,
} StickMode;

class InputViewer : public LUS::GuiWindow {
public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

    InputViewer();
    ~InputViewer();
 
    void Draw();

    private:
        void RenderButton(std::string btn, std::string btnOutline, int state, ImVec2 size, int outlineMode);
};

class InputViewerSettingsWindow : public LUS::GuiWindow {
public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

    InputViewerSettingsWindow();
    ~InputViewerSettingsWindow();

    void Draw();
};
