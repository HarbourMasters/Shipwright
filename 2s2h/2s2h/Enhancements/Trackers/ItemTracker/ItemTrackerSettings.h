#include <ship/window/gui/Gui.h>
#include <ship/window/gui/GuiWindow.h>

class ItemTrackerSettingsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
