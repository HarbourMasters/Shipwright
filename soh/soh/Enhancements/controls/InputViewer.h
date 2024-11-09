#pragma once

#include <libultraship/libultraship.h>

#define CVAR_INPUT_VIEWER(var) "gInputViewer." var

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

class InputViewer : public Ship::GuiWindow {
public:
    using GuiWindow::GuiWindow;

    void Draw() override;
    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

    InputViewer();
    ~InputViewer();
 
    private:
        void RenderButton(std::string btn, std::string btnOutline, int state, ImVec2 size, int outlineMode);
};

class InputViewerSettingsWindow : public Ship::GuiWindow {
public:
    using GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};

    InputViewerSettingsWindow();
    ~InputViewerSettingsWindow();
};
