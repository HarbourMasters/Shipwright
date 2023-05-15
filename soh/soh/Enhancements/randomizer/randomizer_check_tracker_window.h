#include <libultraship/libultraship.h>

namespace CheckTracker {
class CheckTrackerSettingsWindow : public LUS::GuiWindow {
  public:
    using LUS::GuiWindow::GuiWindow;
    ~CheckTrackerSettingsWindow() {};

  protected:
    void InitElement() override {};
    void DrawElement() override;
    void UpdateElement() override {};
};

class CheckTrackerWindow : public LUS::GuiWindow {
  public:
    using LUS::GuiWindow::GuiWindow;
    ~CheckTrackerWindow() {};

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};
}