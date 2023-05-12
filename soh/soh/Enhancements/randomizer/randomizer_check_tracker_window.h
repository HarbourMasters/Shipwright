#include <libultraship/libultraship.h>

namespace CheckTracker {
class CheckTrackerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
    ~CheckTrackerSettingsWindow() {};
};

class CheckTrackerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
    ~CheckTrackerWindow() {};
};
}