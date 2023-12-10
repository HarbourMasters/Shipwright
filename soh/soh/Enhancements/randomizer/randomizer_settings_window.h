#include <libultraship/libultraship.h>

namespace Rando {
class Settings;
}

class RandomizerSettingsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;

  private:
    bool mNeedsUpdate = false;
    std::shared_ptr<Rando::Settings> mSettings;
};
