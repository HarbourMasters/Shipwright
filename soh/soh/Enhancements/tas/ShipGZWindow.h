#pragma once

#ifdef __cplusplus

#include <ship/window/gui/GuiWindow.h>

class ShipGZWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    // Runs every frame even while the window is hidden: hotkeys and the on-screen display.
    void UpdateElement() override;

  private:
    void DrawMacroTab();
    void DrawStatesTab();
    void DrawInputEditorTab();
    void DrawVirtualControllerTab();
    void DrawCommandsTab();
    void DrawSettingsTab();
    void DrawFilesSection();
    void DrawOsd();
};

#endif
