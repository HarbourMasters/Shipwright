#pragma once

#include <libultraship/libultraship.h>

namespace Rando {
class Settings;
}

class RandomizerSettingsWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void SetNeedsUpdate();

  private:
    bool mNeedsUpdate = false;
    std::shared_ptr<Rando::Settings> mSettings;
};
