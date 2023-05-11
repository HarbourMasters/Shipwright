#include <libultraship/libultraship.h>

namespace CheckTracker {
class CheckTrackerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
    void Update() override {};
    ~CheckTrackerSettingsWindow() {};
};

class CheckTrackerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
    void Update() override {};
    ~CheckTrackerWindow() {};
};
}