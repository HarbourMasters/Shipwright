#include <libultraship/libultraship.h>
#ifndef __WIIU__
#include "soh/ImGuiFileBrowser/ImGuiFileBrowser.h"
#endif

class RandomizerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
#ifndef __WIIU__
    imgui_addons::ImGuiFileBrowser spoiler_dialog;
#endif

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};