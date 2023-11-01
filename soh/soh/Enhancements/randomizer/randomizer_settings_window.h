#include <libultraship/libultraship.h>
#include "soh/ImGuiFileBrowser/ImGuiFileBrowser.h"

class RandomizerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    imgui_addons::ImGuiFileBrowser spoiler_dialog;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};