#pragma once
#include <libultraship/libultraship.h>

namespace AdvancedResolutionSettings {
class AdvancedResolutionSettingsWindow : public ShipDK::GuiWindow {
  private:
    bool IsDroppingFrames();

  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
} // namespace AdvancedResolutionSettings
