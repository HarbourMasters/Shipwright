#pragma once
#include <libultraship/libultraship.h>

namespace AdvancedResolutionSettings {
class AdvancedResolutionSettingsWindow : public LUS::GuiWindow {
  private:
    bool IsDroppingFrames();

  protected:
    bool IsBoolArrayTrue(bool*);

  public:
    using LUS::GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
} // namespace AdvancedResolutionSettings
