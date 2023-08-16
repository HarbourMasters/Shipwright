#pragma once
// Date created: 2023 08 16
#include <libultraship/libultraship.h>

namespace AdvancedResolutionSettings {
class AdvancedResolutionSettingsWindow : public LUS::GuiWindow {
  private:
    // Stored Values
     float aspectRatio_X;
     float aspectRatio_Y;
     int verticalPixelCount;
     bool verticalResolutionToggle;
  public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
} // namespace AdvancedResolutionSettings
