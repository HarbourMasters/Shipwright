#include <libultraship/libultraship.h>

class RandomizerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
};